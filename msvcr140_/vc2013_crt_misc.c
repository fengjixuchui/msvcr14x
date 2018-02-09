//防止我们实现的CRT函数被导出
#define _CRTIMP
#include <malloc.h>
#undef _CRTIMP
#include <crtdefs.h>

#include <stdlib.h>
#include <errno.h>
#include <Windows.h>

#define _aligned_free my_aligned_free
#define _aligned_malloc my_aligned_malloc
#define _aligned_offset_malloc my_aligned_offset_malloc
#define _aligned_offset_realloc my_aligned_offset_realloc
#define _aligned_realloc my_aligned_realloc


#define _CRT_ALLOCATION_DEFINED

//#ifdef _DEBUG
//#define TRACE Trace
//#else
#define TRACE
//#endif

//代码来自wine的CRT代码
#ifdef _WDK_BUILD_

/* _aligned */
#define SAVED_PTR(x) ((void *)((DWORD_PTR)((char *)x - sizeof(void *)) & \
                               ~(sizeof(void *) - 1)))
#define ALIGN_PTR(ptr, alignment, offset) ((void *) \
    ((((DWORD_PTR)((char *)ptr + alignment + sizeof(void *) + offset)) & \
      ~(alignment - 1)) - offset))

#define SB_HEAP_ALIGN 16

/*********************************************************************
*		_aligned_free (MSVCRT.@)
*/
void CDECL _aligned_free(void *memblock)
{
	TRACE("(%p)\n", memblock);

	if (memblock)
	{
		void **saved = SAVED_PTR(memblock);
		free(*saved);
	}
}

/*********************************************************************
*		_aligned_offset_malloc (MSVCRT.@)
*/
void * CDECL _aligned_offset_malloc(size_t size, size_t alignment, size_t offset)
{
	void *memblock, *temp, **saved;
	TRACE("(%lu, %lu, %lu)\n", size, alignment, offset);

	/* alignment must be a power of 2 */
	if ((alignment & (alignment - 1)) != 0)
	{
		*_errno() = EINVAL;
		return NULL;
	}

	/* offset must be less than size */
	if (offset && offset >= size)
	{
		*_errno() = EINVAL;
		return NULL;
	}

	/* don't align to less than void pointer size */
	if (alignment < sizeof(void *))
		alignment = sizeof(void *);

	/* allocate enough space for void pointer and alignment */
	temp = malloc(size + alignment + sizeof(void *));

	if (!temp)
		return NULL;

	/* adjust pointer for proper alignment and offset */
	memblock = ALIGN_PTR(temp, alignment, offset);

	/* Save the real allocation address below returned address */
	/* so it can be found later to free. */
	saved = SAVED_PTR(memblock);
	*saved = temp;

	return memblock;
}

/*********************************************************************
*		_aligned_malloc (MSVCRT.@)
*/
void * CDECL _aligned_malloc(size_t size, size_t alignment)
{
	TRACE("(%lu, %lu)\n", size, alignment);
	return _aligned_offset_malloc(size, alignment, 0);
}

/*********************************************************************
*		_aligned_offset_realloc (MSVCRT.@)
*/
void * CDECL _aligned_offset_realloc(void *memblock, size_t size,
	size_t alignment, size_t offset)
{
	void * temp, **saved;
	size_t old_padding, new_padding, old_size;
	TRACE("(%p, %lu, %lu, %lu)\n", memblock, size, alignment, offset);

	if (!memblock)
		return _aligned_offset_malloc(size, alignment, offset);

	/* alignment must be a power of 2 */
	if ((alignment & (alignment - 1)) != 0)
	{
		*_errno() = EINVAL;
		return NULL;
	}

	/* offset must be less than size */
	if (offset >= size)
	{
		*_errno() = EINVAL;
		return NULL;
	}

	if (size == 0)
	{
		_aligned_free(memblock);
		return NULL;
	}

	/* don't align to less than void pointer size */
	if (alignment < sizeof(void *))
		alignment = sizeof(void *);

	/* make sure alignment and offset didn't change */
	saved = SAVED_PTR(memblock);
	if (memblock != ALIGN_PTR(*saved, alignment, offset))
	{
		*_errno() = EINVAL;
		return NULL;
	}

	old_padding = (char *)memblock - (char *)*saved;

	/* Get previous size of block */
	old_size = _msize(*saved);
	if (old_size == -1)
	{
		/* It seems this function was called with an invalid pointer. Bail out. */
		return NULL;
	}

	/* Adjust old_size to get amount of actual data in old block. */
	if (old_size < old_padding)
	{
		/* Shouldn't happen. Something's weird, so bail out. */
		return NULL;
	}
	old_size -= old_padding;

	temp = realloc(*saved, size + alignment + sizeof(void *));

	if (!temp)
		return NULL;

	/* adjust pointer for proper alignment and offset */
	memblock = ALIGN_PTR(temp, alignment, offset);

	/* Save the real allocation address below returned address */
	/* so it can be found later to free. */
	saved = SAVED_PTR(memblock);

	new_padding = (char *)memblock - (char *)temp;

	/*
	Memory layout of old block is as follows:
	+-------+---------------------+-+--------------------------+-----------+
	|  ...  | "old_padding" bytes | | ... "old_size" bytes ... |    ...    |
	+-------+---------------------+-+--------------------------+-----------+
	^                     ^ ^
	|                     | |
	*saved               saved memblock

	Memory layout of new block is as follows:
	+-------+-----------------------------+-+----------------------+-------+
	|  ...  |    "new_padding" bytes      | | ... "size" bytes ... |  ...  |
	+-------+-----------------------------+-+----------------------+-------+
	^                             ^ ^
	|                             | |
	temp                       saved memblock

	However, in the new block, actual data is still written as follows
	(because it was copied by realloc):
	+-------+---------------------+--------------------------------+-------+
	|  ...  | "old_padding" bytes |   ... "old_size" bytes ...     |  ...  |
	+-------+---------------------+--------------------------------+-------+
	^                             ^ ^
	|                             | |
	temp                       saved memblock

	Therefore, min(old_size,size) bytes of actual data have to be moved
	from the offset they were at in the old block (temp + old_padding),
	to the offset they have to be in the new block (temp + new_padding == memblock).
	*/
	if (new_padding != old_padding)
		memmove((char *)memblock, (char *)temp + old_padding, (old_size < size) ? old_size : size);

	*saved = temp;

	return memblock;
}

/*********************************************************************
*		_aligned_realloc (MSVCRT.@)
*/
void * CDECL _aligned_realloc(void *memblock, size_t size, size_t alignment)
{
	TRACE("(%p, %lu, %lu)\n", memblock, size, alignment);
	return _aligned_offset_realloc(memblock, size, alignment, 0);
}

#endif//_WDK_BUILD_