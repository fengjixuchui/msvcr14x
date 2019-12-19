//代码来自wine的CRT代码
#define _CRT_SECURE_NO_WARNINGS
#ifdef _WDK_BUILD_
#include <Windows.h>
#include <tchar.h>
//#define _SIGN_DEFINED
#include <math.h>
#include <float.h>
#include <xmmintrin.h>
#include <fpieee.h>

#define _libm_sse2_acos my__libm_sse2_acos
#define _libm_sse2_acosf my__libm_sse2_acosf
#define _libm_sse2_asin my__libm_sse2_asin
#define _libm_sse2_asinf my__libm_sse2_asinf
#define _libm_sse2_atan my__libm_sse2_atan
#define _libm_sse2_atan2 my__libm_sse2_atan2
#define _libm_sse2_atanf my__libm_sse2_atanf
#define _libm_sse2_cos my__libm_sse2_cos
#define _libm_sse2_cosf my__libm_sse2_cosf
#define _libm_sse2_exp my__libm_sse2_exp
#define _libm_sse2_expf my__libm_sse2_expf
#define _libm_sse2_log my__libm_sse2_log
#define _libm_sse2_log10 my__libm_sse2_log10
#define _libm_sse2_log10f my__libm_sse2_log10f
#define _libm_sse2_logf my__libm_sse2_logf
#define _libm_sse2_pow my__libm_sse2_pow
#define _libm_sse2_powf my__libm_sse2_powf
#define _libm_sse2_sin my__libm_sse2_sin
#define _libm_sse2_sinf my__libm_sse2_sinf
#define _libm_sse2_tan my__libm_sse2_tan
#define _libm_sse2_tanf my__libm_sse2_tanf

#define TRACE

#define _DENORM    (-2)
#define _FINITE    (-1)
#define _INFCODE   1
#define _NANCODE   2

#define FP_INFINITE  _INFCODE
#define FP_NAN       _NANCODE
#define FP_NORMAL    _FINITE
#define FP_SUBNORMAL _DENORM
#define FP_ZERO      0

extern "C" short CDECL _dclass(double x)
{
	switch (_fpclass(x)) {
	case _FPCLASS_QNAN:
	case _FPCLASS_SNAN:
		return FP_NAN;
	case _FPCLASS_NINF:
	case _FPCLASS_PINF:
		return FP_INFINITE;
	case _FPCLASS_ND:
	case _FPCLASS_PD:
		return FP_SUBNORMAL;
	case _FPCLASS_NN:
	case _FPCLASS_PN:
	default:
		return FP_NORMAL;
	case _FPCLASS_NZ:
	case _FPCLASS_PZ:
		return FP_ZERO;
	}
}

extern "C" double CDECL trunc(double x)
{
	return (x > 0) ? floor(x) : ceil(x);
}

extern "C" double CDECL exp2(double x)
{
	return pow(2, x);
}

extern "C" double CDECL rint(double x)
{
	return x >= 0 ? floor(x + 0.5) : ceil(x - 0.5);
}

extern "C" long int CDECL lrint(double x)
{
	return (long)rint(x);
}

extern "C" long long int CDECL llrint(double x)
{
	return (long long int)rint(x);
}

#ifndef va_copy
#define va_copy(destination, source) ((destination) = (source))
#endif

extern "C" void __cdecl _vacopy(_Out_ va_list *dest, _In_ va_list src)
{
	va_copy(*dest, src);
}


typedef struct
{
	unsigned int control;
	unsigned int status;
} fenv_t;

/*********************************************************************
*		fegetenv (MSVCR120.@)
*/
int CDECL fegetenv(fenv_t *env)
{
	env->control = _controlfp(0, 0) & (_EM_INEXACT | _EM_UNDERFLOW |
		_EM_OVERFLOW | _EM_ZERODIVIDE | _EM_INVALID);
	env->status = _statusfp();
	return 0;
}

#define __GNUC__
#define __i386__
static BOOL sse2_supported = IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE);
/*********************************************************************
*              fesetenv (MSVCR120.@)
*/
int CDECL fesetenv(const fenv_t *env)
{
#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
	struct {
		WORD control_word;
		WORD unused1;
		WORD status_word;
		WORD unused2;
		WORD tag_word;
		WORD unused3;
		DWORD instruction_pointer;
		WORD code_segment;
		WORD unused4;
		DWORD operand_addr;
		WORD data_segment;
		WORD unused5;
	} fenv;

	TRACE("(%p)\n", env);

	if (!env->control && !env->status) {
		_fpreset();
		return 0;
	}

	//__asm__ __volatile__("fnstenv %0" : "=m" (fenv));
	__asm { fnstenv byte ptr[fenv.control_word] }

	fenv.control_word &= ~0x3d;
	if (env->control & _EM_INVALID) fenv.control_word |= 0x1;
	if (env->control & _EM_ZERODIVIDE) fenv.control_word |= 0x4;
	if (env->control & _EM_OVERFLOW) fenv.control_word |= 0x8;
	if (env->control & _EM_UNDERFLOW) fenv.control_word |= 0x10;
	if (env->control & _EM_INEXACT) fenv.control_word |= 0x20;

	fenv.status_word &= ~0x3d;
	if (env->status & _SW_INVALID) fenv.status_word |= 0x1;
	if (env->status & _SW_ZERODIVIDE) fenv.status_word |= 0x4;
	if (env->status & _SW_OVERFLOW) fenv.status_word |= 0x8;
	if (env->status & _SW_UNDERFLOW) fenv.status_word |= 0x10;
	if (env->status & _SW_INEXACT) fenv.status_word |= 0x20;

	//__asm__ __volatile__("fldenv %0" : : "m" (fenv) : "st", "st(1)",
	//	"st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)");
	__asm { fldenv  byte ptr[fenv.control_word] }

	if (sse2_supported)
	{
		DWORD fpword;

		//__asm__ __volatile__("stmxcsr %0" : "=m" (fpword));
		fpword = _mm_getcsr();
		fpword &= ~0x1e80;
		if (env->control & _EM_INVALID) fpword |= 0x80;
		if (env->control & _EM_ZERODIVIDE) fpword |= 0x200;
		if (env->control & _EM_OVERFLOW) fpword |= 0x400;
		if (env->control & _EM_UNDERFLOW) fpword |= 0x800;
		if (env->control & _EM_INEXACT) fpword |= 0x1000;
		//__asm__ __volatile__("ldmxcsr %0" : : "m" (fpword));
		_mm_setcsr(fpword);
	}

	return 0;
#else
	FIXME("not implemented\n");
#endif
	return 1;
}

#define _CLASS_ARG(_Val)                                  (sizeof ((_Val) + (float)0) == sizeof (float) ? 'f' : sizeof ((_Val) + (double)0) == sizeof (double) ? 'd' : 'l')
#define _CLASSIFY(_Val, _FFunc, _DFunc, _LDFunc)          (_CLASS_ARG(_Val) == 'f' ? _FFunc((float)(_Val)) : _CLASS_ARG(_Val) == 'd' ? _DFunc((double)(_Val)) : _LDFunc((long double)(_Val)))
#define _CLASSIFY2(_Val1, _Val2, _FFunc, _DFunc, _LDFunc) (_CLASS_ARG((_Val1) + (_Val2)) == 'f' ? _FFunc((float)(_Val1), (float)(_Val2)) : _CLASS_ARG((_Val1) + (_Val2)) == 'd' ? _DFunc((double)(_Val1), (double)(_Val2)) : _LDFunc((long double)(_Val1), (long double)(_Val2)))

int __cdecl _dsign(_In_ double _X);
int __cdecl _ldsign(_In_ long double _X);
int __cdecl _fdsign(_In_ float _X);

short __cdecl _ldclass(_In_ long double _X);
short __cdecl _fdclass(_In_ float _X);

//http://zh.cppreference.com/w/c/numeric/math/signbit
//signbit:确定给定浮点数arg是否为负。该宏返回整数值。
//#define signbit(_Val)       (_CLASSIFY(_Val, _fdsign, _dsign, _ldsign))
//不能用上面这个VC的内部定义，这个VC的signbit是用_fdsign, _dsign, _ldsign来判断的和下面的设计冲突了，那样将导致递归

#undef signbit
/* FIXME: Does not work with -NAN and -0. */
#ifndef signbit
#define signbit(x) ((x) < 0)
#endif

#define fpclassify(_Val)      (_CLASSIFY(_Val, _fdclass, _dclass, _ldclass))

#define MSVCR120_isinf(_Val)         (fpclassify(_Val) == FP_INFINITE)
#define MSVCR120_isnan(_Val)         (fpclassify(_Val) == FP_NAN)

/*********************************************************************
*      _fdsign (MSVCR120.@)
*/
int CDECL _fdsign(float x)
{
	return signbit(x) ? 0x8000 : 0;
}

/*********************************************************************
*      _dsign (MSVCR120.@)
*/
int CDECL _dsign(double x)
{
	return signbit(x) ? 0x8000 : 0;
}

int __cdecl _ldsign(_In_ long double _X)
{
	return _dsign(_X);
}

#if _MSC_VER<=1500

typedef long double LDOUBLE;  /* long double is just a double */


/*********************************************************************
 *      _fdclass (MSVCR120.@)
 */
short CDECL _fdclass(float x)
{
    return _dclass(x);
}

/*********************************************************************
 *      _ldclass (MSVCR120.@)
 */
short CDECL _ldclass(LDOUBLE x)
{
    return _dclass(x);
}

/*********************************************************************
 *		_isnan (MSVCRT.@)
 */
extern "C" INT CDECL isnan(double num)
{
  /* Some implementations return -1 for true(glibc), msvcrt/crtdll return 1.
   * Do the same, as the result may be used in calculations
   */
  //return MSVCR120_isnan(num) != 0;
	return _isnan(num);
}

extern "C" int isinf(double x)
{
	return (!(_finite(x) || _isnan(x)));
}

/*********************************************************************
 *      lrintf (MSVCR120.@)
 */
extern "C" long CDECL lrintf(float x)
{
    return lrint(x);
}
#endif

#ifndef _HUGE_ENUF
#define _HUGE_ENUF  1e+300	/* _HUGE_ENUF*_HUGE_ENUF must overflow */
#endif /* _HUGE_ENUF */

#define INFINITY   ((float)(_HUGE_ENUF * _HUGE_ENUF))  /* causes warning C4756: overflow in constant arithmetic (by design) */

/*********************************************************************
*      _except1 (MSVCR120.@)
*  TODO:
*   - find meaning of ignored cw and operation bits
*   - unk parameter
*/
extern "C" double CDECL _except1(DWORD fpe, _FP_OPERATION_CODE op, double arg, double res, DWORD cw, void *unk)
{
	ULONG_PTR exception_arg;
	DWORD exception = 0;
	fenv_t env;
	DWORD fpword = 0;
	WORD operation;

	TRACE("(%x %x %lf %lf %x %p)\n", fpe, op, arg, res, cw, unk);

#ifdef _WIN64
	cw = ((cw >> 7) & 0x3f) | ((cw >> 3) & 0xc00);
#endif
	operation = op << 5;
	exception_arg = (ULONG_PTR)&operation;

	fegetenv(&env);

	if (fpe & 0x1) { /* overflow */
		if ((fpe == 0x1 && (cw & 0x8)) || (fpe == 0x11 && (cw & 0x28))) {
			/* 32-bit version also sets SW_INEXACT here */
			env.status |= _SW_OVERFLOW;
			if (fpe & 0x10) env.status |= _SW_INEXACT;
			res = signbit(res) ? -INFINITY : INFINITY;
		}
		else {
			exception = EXCEPTION_FLT_OVERFLOW;
		}
	}
	else if (fpe & 0x2) { /* underflow */
		if ((fpe == 0x2 && (cw & 0x10)) || (fpe == 0x12 && (cw & 0x30))) {
			env.status |= _SW_UNDERFLOW;
			if (fpe & 0x10) env.status |= _SW_INEXACT;
			res = signbit(res) ? -0.0 : 0.0;
		}
		else {
			exception = EXCEPTION_FLT_UNDERFLOW;
		}
	}
	else if (fpe & 0x4) { /* zerodivide */
		if ((fpe == 0x4 && (cw & 0x4)) || (fpe == 0x14 && (cw & 0x24))) {
			env.status |= _SW_ZERODIVIDE;
			if (fpe & 0x10) env.status |= _SW_INEXACT;
		}
		else {
			exception = EXCEPTION_FLT_DIVIDE_BY_ZERO;
		}
	}
	else if (fpe & 0x8) { /* invalid */
		if (fpe == 0x8 && (cw & 0x1)) {
			env.status |= _SW_INVALID;
		}
		else {
			exception = EXCEPTION_FLT_INVALID_OPERATION;
		}
	}
	else if (fpe & 0x10) { /* inexact */
		if (fpe == 0x10 && (cw & 0x20)) {
			env.status |= _SW_INEXACT;
		}
		else {
			exception = EXCEPTION_FLT_INEXACT_RESULT;
		}
	}

	if (exception)
		env.status = 0;
	fesetenv(&env);
	if (exception)
		RaiseException(exception, 0, 1, &exception_arg);

	if (cw & 0x1) fpword |= _EM_INVALID;
	if (cw & 0x2) fpword |= _EM_DENORMAL;
	if (cw & 0x4) fpword |= _EM_ZERODIVIDE;
	if (cw & 0x8) fpword |= _EM_OVERFLOW;
	if (cw & 0x10) fpword |= _EM_UNDERFLOW;
	if (cw & 0x20) fpword |= _EM_INEXACT;
	switch (cw & 0xc00)
	{
	case 0xc00: fpword |= _RC_UP | _RC_DOWN; break;
	case 0x800: fpword |= _RC_UP; break;
	case 0x400: fpword |= _RC_DOWN; break;
	}
	switch (cw & 0x300)
	{
	case 0x0:   fpword |= _PC_24; break;
	case 0x200: fpword |= _PC_53; break;
	case 0x300: fpword |= _PC_64; break;
	}
	if (cw & 0x1000) fpword |= _IC_AFFINE;
	_control87(fpword, 0xffffffff);

	return res;
}

double (__cdecl* _cos)(__in double _X);
double (__cdecl* _pow)(__in double _X, __in double _Y);
double (__cdecl* _exp)(__in double _X);
double (__cdecl* _sin)(__in double _X);
double (__cdecl* _sqrt)(__in double _X);
double (__cdecl* _acos)(__in double _X);
double (__cdecl* _asin)(__in double _X);
double (__cdecl* _atan)(__in double _X);
double(__cdecl* _atan2)(__in double _Y, __in double _X);
double (__cdecl* _tan)(__in double _X);
double (__cdecl* _log)(__in double _X);
double(__cdecl* _log10)(__in double _X);

#define _sinf(x) ((float)_sin((double)(x)))
#define _cosf(x) ((float)_cos((double)(x)))
#define _tanf(x) ((float)_tan((double)(x)))
#define _sinhf(x) ((float)_sinh((double)(x)))
#define _coshf(x) ((float)_cosh((double)(x)))
#define _tanhf(x) ((float)_tanh((double)(x)))
#define _asinf(x) ((float)_asin((double)(x)))
#define _acosf(x) ((float)_acos((double)(x)))
#define _atanf(x) ((float)_atan((double)(x)))
#define _atan2f(x,y) ((float)_atan2((double)(x), (double)(y)))
#define _expf(x) ((float)_exp((double)(x)))
#define _logf(x) ((float)_log((double)(x)))
#define _log10f(x) ((float)_log10((double)(x)))
#define _powf(x,y) ((float)_pow((double)(x), (double)(y)))
#define _sqrtf(x) ((float)_sqrt((double)(x)))
#define _ceilf(x) ((float)_ceil((double)(x)))
#define _floorf(x) ((float)_floor((double)(x)))
#define _fabsf(x) ((float)_fabs((double)(x)))
#define _frexpf(x) ((float)_frexp((double)(x)))
#define _modff(x,y) ((float)_modf((double)(x), (double*)(y)))
#define _fmodf(x,y) ((float)_fmod((double)(x), (double)(y)))

int vc2013_crt_libm_init()
{
	HMODULE hMod=GetModuleHandle(_T("msvcrt.dll"));
	_cos = (double(__cdecl *)(double))GetProcAddress(hMod, "cos");
	_pow = (double(__cdecl *)(double, double))GetProcAddress(hMod, "pow");
	_exp = (double(__cdecl *)(double))GetProcAddress(hMod, "exp");
	_sin = (double(__cdecl *)(double))GetProcAddress(hMod, "sin");
	_sqrt = (double(__cdecl *)(double))GetProcAddress(hMod, "sqrt");
	_acos = (double(__cdecl *)(double))GetProcAddress(hMod, "acos");
	_asin = (double(__cdecl *)(double))GetProcAddress(hMod, "asin");
	_atan = (double(__cdecl *)(double))GetProcAddress(hMod, "atan");
	_atan2 = (double(__cdecl *)(double,double))GetProcAddress(hMod, "atan2");
	_tan = (double(__cdecl *)(double))GetProcAddress(hMod, "tan");
	_log = (double(__cdecl *)(double))GetProcAddress(hMod, "log");
	_log10 = (double(__cdecl *)(double))GetProcAddress(hMod, "log10");
	return 1;
}
int nResult_Of_vc2013_crt_init = vc2013_crt_libm_init();



//sse2数学函数
/***********************************************************************
*		__libm_sse2_cos   (MSVCRT.@)
*/
extern "C"  void __cdecl _libm_sse2_cos(void)
{
	double d;
	//__asm__ __volatile__("movq %%xmm0,%0" : "=m" (d));
	__asm movq    qword ptr[d], xmm0
	d = _cos(d);
	//__asm__ __volatile__("movq %0,%%xmm0" : : "m" (d));
	__asm movq    xmm0, qword ptr[d]
}

/***********************************************************************
*		__libm_sse2_cosf   (MSVCRT.@)
*/
extern "C" void __cdecl _libm_sse2_cosf(void)
{
	float f;
	//__asm__ __volatile__("movd %%xmm0,%0" : "=g" (f));
	__asm movd dword ptr[f], xmm0
	f = _cosf(f);
	//__asm__ __volatile__("movd %0,%%xmm0" : : "g" (f));
	__asm movd xmm0, dword ptr[f]
}

extern "C" void __cdecl _libm_sse2_cos_precise(void)
{
	_libm_sse2_cos();
}

/***********************************************************************
*		__libm_sse2_pow   (MSVCRT.@)
*/
extern "C" void __cdecl _libm_sse2_pow(void)
{
	double d1, d2;
	//__asm__ __volatile__("movq %%xmm0,%0; movq %%xmm1,%1 " : "=m" (d1), "=m" (d2));
	__asm movq[d1], xmm0
	__asm movq qword ptr[d2], xmm1
	d1 = _pow(d1, d2);
	//__asm__ __volatile__("movq %0,%%xmm0" : : "m" (d1));
	__asm movq    xmm0, [d1]
}

/***********************************************************************
*		__libm_sse2_powf   (MSVCRT.@)
*/
extern "C" void __cdecl _libm_sse2_powf(void)
{
	float f1, f2;
	//__asm__ __volatile__("movd %%xmm0,%0; movd %%xmm1,%1" : "=g" (f1), "=g" (f2));
	__asm movd dword ptr[f1], xmm0
	__asm movd dword ptr[f2], xmm1
	f1 = _powf(f1, f2);
	//__asm__ __volatile__("movd %0,%%xmm0" : : "g" (f1));
	__asm movd xmm0, dword ptr[f1]
}

extern "C" void __cdecl _libm_sse2_pow_precise(void)
{
	_libm_sse2_pow();
}

/***********************************************************************
*		__libm_sse2_exp   (MSVCRT.@)
*/
extern "C" void __cdecl _libm_sse2_exp(void)
{
	double d;
	//__asm__ __volatile__("movq %%xmm0,%0" : "=m" (d));
	__asm movq    qword ptr[d], xmm0
	d = _exp(d);
	//__asm__ __volatile__("movq %0,%%xmm0" : : "m" (d));
	__asm movq    xmm0, qword ptr[d]
}

/***********************************************************************
*		__libm_sse2_expf   (MSVCRT.@)
*/
extern "C" void __cdecl _libm_sse2_expf(void)
{
	float f;
	//__asm__ __volatile__("movd %%xmm0,%0" : "=g" (f));
	__asm movd dword ptr[f], xmm0
	f = _expf(f);
	//__asm__ __volatile__("movd %0,%%xmm0" : : "g" (f));
	__asm movd xmm0, dword ptr[f]
}

extern "C" void __cdecl _libm_sse2_exp_precise(void)
{
	_libm_sse2_exp();
}

/***********************************************************************
*		__libm_sse2_sin   (MSVCRT.@)
*/
extern "C" void __cdecl _libm_sse2_sin(void)
{
	double d;
	//__asm__ __volatile__("movq %%xmm0,%0" : "=m" (d));
	__asm movq    qword ptr[d], xmm0
	d = _sin(d);
	//__asm__ __volatile__("movq %0,%%xmm0" : : "m" (d));
	__asm movq    xmm0, qword ptr[d]
}

/***********************************************************************
*		__libm_sse2_sinf   (MSVCRT.@)
*/
extern "C" void __cdecl _libm_sse2_sinf(void)
{
	float f;
	//__asm__ __volatile__("movd %%xmm0,%0" : "=g" (f));
	__asm movd dword ptr[f], xmm0
	f = _sinf(f);
	//__asm__ __volatile__("movd %0,%%xmm0" : : "g" (f));
	__asm movd xmm0, dword ptr[f]
}

extern "C" void __cdecl _libm_sse2_sin_precise(void)
{
	_libm_sse2_sin();
}

/***********************************************************************
*		__libm_sse2_sqrt_precise   (MSVCR110.@)
*/
extern "C" void __cdecl _libm_sse2_sqrt_precise(void)
{
	double d;
	//__asm__ __volatile__("movq %%xmm0,%0" : "=m" (d));
	__asm  movq    qword ptr[d], xmm0
	d = _sqrt(d);
	//__asm__ __volatile__("movq %0,%%xmm0" : : "m" (d));
	__asm  movq    xmm0, qword ptr[d]
}

/***********************************************************************
*		__libm_sse2_acos   (MSVCRT.@)
*/
extern "C" void __cdecl _libm_sse2_acos(void)
{
	double d;
	//__asm__ __volatile__("movq %%xmm0,%0" : "=m" (d));
	__asm  movq    qword ptr[d], xmm0
	d = _acos(d);
	//__asm__ __volatile__("movq %0,%%xmm0" : : "m" (d));
	__asm  movq    xmm0, qword ptr[d]
}

/***********************************************************************
*		__libm_sse2_acosf   (MSVCRT.@)
*/
void __cdecl _libm_sse2_acosf(void)
{
	float f;
	//__asm__ __volatile__("movd %%xmm0,%0" : "=g" (f));
	__asm movd dword ptr[f], xmm0
	f = _acosf(f);
	//__asm__ __volatile__("movd %0,%%xmm0" : : "g" (f));
	__asm movd xmm0, dword ptr[f]
}

extern "C" void __cdecl _libm_sse2_acos_precise(void)
{
	_libm_sse2_acos();
}

/***********************************************************************
*		__libm_sse2_asin   (MSVCRT.@)
*/
extern "C" void __cdecl _libm_sse2_asin(void)
{
	double d;
	//__asm__ __volatile__("movq %%xmm0,%0" : "=m" (d));
	__asm  movq    qword ptr[d], xmm0
	d = _asin(d);
	//__asm__ __volatile__("movq %0,%%xmm0" : : "m" (d));
	__asm  movq    xmm0, qword ptr[d]
}

/***********************************************************************
*		__libm_sse2_asinf   (MSVCRT.@)
*/
void __cdecl _libm_sse2_asinf(void)
{
	float f;
	//__asm__ __volatile__("movd %%xmm0,%0" : "=g" (f));
	__asm movd dword ptr[f], xmm0
	f = asinf(f);
	//__asm__ __volatile__("movd %0,%%xmm0" : : "g" (f));
	__asm movd xmm0, dword ptr[f]
}

extern "C" void __cdecl _libm_sse2_asin_precise(void)
{
	_libm_sse2_asin();
}

/***********************************************************************
*		__libm_sse2_atan   (MSVCRT.@)
*/
extern "C" void __cdecl _libm_sse2_atan(void)
{
	double d;
	//__asm__ __volatile__("movq %%xmm0,%0" : "=m" (d));
	__asm  movq    qword ptr[d], xmm0
	d = _atan(d);
	//__asm__ __volatile__("movq %0,%%xmm0" : : "m" (d));
	__asm  movq    xmm0, qword ptr[d]
}

/***********************************************************************
*		__libm_sse2_atan2   (MSVCRT.@)
*/
extern "C" void __cdecl _libm_sse2_atan2(void)
{
	double d1, d2;
	//__asm__ __volatile__("movq %%xmm0,%0; movq %%xmm1,%1 " : "=m" (d1), "=m" (d2));
	__asm  movq    qword ptr[d1], xmm0
	__asm  movq    qword ptr[d2], xmm1
	d1 = _atan2(d1, d2);
	//__asm__ __volatile__("movq %0,%%xmm0" : : "m" (d1));
	__asm  movq    xmm0, qword ptr[d1]
}

/***********************************************************************
*		__libm_sse2_atanf   (MSVCRT.@)
*/
extern "C" void __cdecl _libm_sse2_atanf(void)
{
	float f;
	//__asm__ __volatile__("movd %%xmm0,%0" : "=g" (f));
	__asm movd dword ptr[f], xmm0
	f = _atanf(f);
	//__asm__ __volatile__("movd %0,%%xmm0" : : "g" (f));
	__asm movd xmm0, dword ptr[f]
}

extern "C" void __cdecl _libm_sse2_atan_precise(void)
{
	_libm_sse2_atan();
}

/***********************************************************************
*		__libm_sse2_tan   (MSVCRT.@)
*/
extern "C" void __cdecl _libm_sse2_tan(void)
{
	double d;
	//__asm__ __volatile__("movq %%xmm0,%0" : "=m" (d));
	__asm  movq    qword ptr[d], xmm0
	d = _tan(d);
	//__asm__ __volatile__("movq %0,%%xmm0" : : "m" (d));
	__asm  movq    xmm0, qword ptr[d]
}

/***********************************************************************
*		__libm_sse2_tanf   (MSVCRT.@)
*/
extern "C" void __cdecl _libm_sse2_tanf(void)
{
	float f;
	//__asm__ __volatile__("movd %%xmm0,%0" : "=g" (f));
	__asm movd dword ptr[f], xmm0
	f = _tanf(f);
	//__asm__ __volatile__("movd %0,%%xmm0" : : "g" (f));
	__asm movd xmm0, dword ptr[f]
}

extern "C" void __cdecl _libm_sse2_tan_precise(void)
{
	_libm_sse2_tan();
}

/***********************************************************************
*		__libm_sse2_log   (MSVCRT.@)
*/
extern "C" void __cdecl _libm_sse2_log(void)
{
	double d;
	//__asm__ __volatile__("movq %%xmm0,%0" : "=m" (d));
	__asm  movq    qword ptr[d], xmm0
	d = _log(d);
	//__asm__ __volatile__("movq %0,%%xmm0" : : "m" (d));
	__asm  movq    xmm0, qword ptr[d]
}

/***********************************************************************
*		__libm_sse2_log10   (MSVCRT.@)
*/
extern "C" void __cdecl _libm_sse2_log10(void)
{
	double d;
	//__asm__ __volatile__("movq %%xmm0,%0" : "=m" (d));
	__asm  movq    qword ptr[d], xmm0
	d = _log10(d);
	//__asm__ __volatile__("movq %0,%%xmm0" : : "m" (d));
	__asm  movq    xmm0, qword ptr[d]
}

/***********************************************************************
*		__libm_sse2_log10f   (MSVCRT.@)
*/
extern "C" void __cdecl _libm_sse2_log10f(void)
{
	float f;
	//__asm__ __volatile__("movd %%xmm0,%0" : "=g" (f));
	__asm movd dword ptr[f], xmm0
	f = _log10f(f);
	//__asm__ __volatile__("movd %0,%%xmm0" : : "g" (f));
	__asm movd xmm0, dword ptr[f]
}

/***********************************************************************
*		__libm_sse2_logf   (MSVCRT.@)
*/
extern "C" void __cdecl _libm_sse2_logf(void)
{
	float f;
	//__asm__ __volatile__("movd %%xmm0,%0" : "=g" (f));
	__asm movd dword ptr[f], xmm0
	f = _logf(f);
	//__asm__ __volatile__("movd %0,%%xmm0" : : "g" (f));
	__asm movd xmm0, dword ptr[f]
}

extern "C" void __cdecl _libm_sse2_log_precise(void)
{
	_libm_sse2_log();
}

#endif//_WDK_BUILD_