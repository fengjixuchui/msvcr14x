struct msvcrt__iobuf {
	char *_ptr;
	int   _cnt;
	char *_base;
	int   _flag;
	int   _file;
	int   _charbuf;
	int   _bufsiz;
	char *_tmpfname;
};
typedef struct msvcrt__iobuf msvcrt_FILE;

typedef struct {
	msvcrt_FILE f;
	CRITICAL_SECTION lock;
}   msvcrt__FILEX;