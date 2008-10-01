/*
 * http://www.opengroup.org/onlinepubs/009695399/basedefs/math.h.html
 */
#ifndef _MATH_H
#define _MATH_H 1

#ifdef __cplusplus
extern "C" {
#endif

#define M_E		2.7182818284590452354		/* e */
#define M_LOG2E		1.4426950408889634074		/* log_2 e */
#define M_LOG10E	0.43429448190325182765		/* log_10 e */
#define M_LN2		0.69314718055994530942		/* log_e 2 */
#define M_LN10		2.30258509299404568402		/* log_e 10 */
#define M_PI		3.14159265358979323846		/* pi */
#define M_PI_2		1.57079632679489661923		/* pi/2 */
#define M_PI_4		0.78539816339744830962		/* pi/4 */
#define M_1_PI		0.31830988618379067154		/* 1/pi */
#define M_2_PI		0.63661977236758134308		/* 2/pi */
#define M_2_SQRTPI	1.12837916709551257390		/* 2/sqrt(pi) */
#define M_SQRT2		1.41421356237309504880		/* sqrt(2) */
#define M_SQRT1_2	0.70710678118654752440		/* 1/sqrt(2) */

#define M_El		2.7182818284590452353602874713526625L	/* e */
#define M_LOG2El	1.4426950408889634073599246810018921L	/* log_2 e */
#define M_LOG10El	0.4342944819032518276511289189166051L	/* log_10 e */
#define M_LN2l		0.6931471805599453094172321214581766L	/* log_e 2 */
#define M_LN10l		2.3025850929940456840179914546843642L	/* log_e 10 */
#define M_PIl		3.1415926535897932384626433832795029L	/* pi */
#define M_PI_2l		1.5707963267948966192313216916397514L	/* pi/2 */
#define M_PI_4l		0.7853981633974483096156608458198757L	/* pi/4 */
#define M_1_PIl		0.3183098861837906715377675267450287L	/* 1/pi */
#define M_2_PIl		0.6366197723675813430755350534900574L	/* 2/pi */
#define M_2_SQRTPIl	1.1283791670955125738961589031215452L	/* 2/sqrt(pi) */
#define M_SQRT2l	1.4142135623730950488016887242096981L	/* sqrt(2) */
#define M_SQRT1_2l	0.7071067811865475244008443621048490L	/* 1/sqrt(2) */

#if I386

#define MAXFLOAT	3.40282347e+38F
#define HUGE_VAL	(__builtin_huge_val())
#define HUGE_VALF	(__builtin_huge_valf())
#define HUGE_VALL	(__builtin_huge_vall())
#define INFINITY	(__builtin_inff())

#define NAN 		(__extension__ (( union { \
			unsigned __l __attribute__((__mode__(__SI__))); \
			float __d; }) { __l: 0x7fc00000UL }).__d)

/* The values returned by `ilogb' for 0 and NaN respectively.  */
#define FP_ILOGB0	(-2147483647 - 1)
#define FP_ILOGBNAN	(-2147483647 - 1)

#endif

/* Addition rounds to 0: zero, 1: nearest, 2: +inf, 3: -inf, -1: unknown.  */
/* ??? This is supposed to change with calls to fesetround in <fenv.h>.  */
#undef FLT_ROUNDS
#define FLT_ROUNDS 1

/* All floating-point numbers can be put in one of these categories.  */
enum {
	FP_NAN,
#define FP_NAN FP_NAN
	FP_INFINITE,
#define FP_INFINITE FP_INFINITE
	FP_ZERO,
#define FP_ZERO FP_ZERO
	FP_SUBNORMAL,
#define FP_SUBNORMAL FP_SUBNORMAL
	FP_NORMAL,
#define FP_NORMAL FP_NORMAL
};

int isnan (double);
int isinf (double);

/* Bitmasks for the math_errhandling macro.  */
#define MATH_ERRNO	1	/* errno set by math functions.  */
#define MATH_ERREXCEPT	2	/* Exceptions raised by math functions.  */

#if defined FLT_EVAL_METHOD && FLT_EVAL_METHOD == 2
typedef long double float_t;
typedef long double double_t;
#elif defined FLT_EVAL_METHOD && FLT_EVAL_METHOD == 1
typedef double float_t;
typedef double double_t;
#else
typedef float float_t;
typedef double double_t;
#endif

double      acos (double);
float       acosf (float);
double      acosh (double);
float       acoshf (float);
long double acoshl (long double);
long double acosl (long double);
double      asin (double);
float       asinf (float);
double      asinh (double);
float       asinhf (float);
long double asinhl (long double);
long double asinl (long double);
double      atan (double);
double      atan2 (double, double);
float       atan2f (float, float);
long double atan2l (long double, long double);
float       atanf (float);
double      atanh (double);
float       atanhf (float);
long double atanhl (long double);
long double atanl (long double);
double      cbrt (double);
float       cbrtf (float);
long double cbrtl (long double);
double      ceil (double);
float       ceilf (float);
long double ceill (long double);
double      copysign (double, double);
float       copysignf (float, float);
long double copysignl (long double, long double);
double      cos (double);
float       cosf (float);
double      cosh (double);
float       coshf (float);
long double coshl (long double);
long double cosl (long double);
double      erf (double);
double      erfc (double);
float       erfcf (float);
long double erfcl (long double);
float       erff (float);
long double erfl (long double);
double      exp (double);
double      exp2 (double);
float       exp2f (float);
long double exp2l (long double);
float       expf (float);
long double expl (long double);
double      expm1 (double);
float       expm1f (float);
long double expm1l (long double);
double      fabs (double);
float       fabsf (float);
long double fabsl (long double);
double      fdim (double, double);
float       fdimf (float, float);
long double fdiml (long double, long double);
double      floor (double);
float       floorf (float);
long double floorl (long double);
double      fma (double, double, double);
float       fmaf (float, float, float);
long double fmal (long double, long double, long double);
double      fmax (double, double);
float       fmaxf (float, float);
long double fmaxl (long double, long double);
double      fmin (double, double);
float       fminf (float, float);
long double fminl (long double, long double);
double      fmod (double, double);
float       fmodf (float, float);
long double fmodl (long double, long double);
double      frexp (double, int *);
float       frexpf (float value, int *);
long double frexpl (long double value, int *);
double      hypot (double, double);
float       hypotf (float, float);
long double hypotl (long double, long double);
int         ilogb (double);
int         ilogbf (float);
int         ilogbl (long double);

double      j0 (double);
double      j1 (double);
double      jn (int, double);

double      ldexp (double, int);
float       ldexpf (float, int);
long double ldexpl (long double, int);
double      lgamma (double);
float       lgammaf (float);
long double lgammal (long double);
long long   llrint (double);
long long   llrintf (float);
long long   llrintl (long double);
long long   llround (double);
long long   llroundf (float);
long long   llroundl (long double);
double      log (double);
double      log10 (double);
float       log10f (float);
long double log10l (long double);
double      log1p (double);
float       log1pf (float);
long double log1pl (long double);
double      log2 (double);
float       log2f (float);
long double log2l (long double);
double      logb (double);
float       logbf (float);
long double logbl (long double);
float       logf (float);
long double logl (long double);
long        lrint (double);
long        lrintf (float);
long        lrintl (long double);
long        lround (double);
long        lroundf (float);
long        lroundl (long double);
double      modf (double, double *);
float       modff (float, float *);
long double modfl (long double, long double *);
double      nan (const char *);
float       nanf (const char *);
long double nanl (const char *);
double      nearbyint (double);
float       nearbyintf (float);
long double nearbyintl (long double);
double      nextafter (double, double);
float       nextafterf (float, float);
long double nextafterl (long double, long double);
double      nexttoward (double, long double);
float       nexttowardf (float, long double);
long double nexttowardl (long double, long double);
double      pow (double, double);
float       powf (float, float);
long double powl (long double, long double);
double      remainder (double, double);
float       remainderf (float, float);
long double remainderl (long double, long double);
double      remquo (double, double, int *);
float       remquof (float, float, int *);
long double remquol (long double, long double, int *);
double      rint (double);
float       rintf (float);
long double rintl (long double);
double      round (double);
float       roundf (float);
long double roundl (long double);

double      scalb (double, double);

double      scalbln (double, long);
float       scalblnf (float, long);
long double scalblnl (long double, long);
double      scalbn (double, int);
float       scalbnf (float, int);
long double scalbnl (long double, int);
double      sin (double);
float       sinf (float);
double      sinh (double);
float       sinhf (float);
long double sinhl (long double);
long double sinl (long double);
double      sqrt (double);
float       sqrtf (float);
long double sqrtl (long double);
double      tan (double);
float       tanf (float);
double      tanh (double);
float       tanhf (float);
long double tanhl (long double);
long double tanl (long double);
double      tgamma (double);
float       tgammaf (float);
long double tgammal (long double);
double      trunc (double);
float       truncf (float);
long double truncl (long double);

double      y0 (double);
double      y1 (double);
double      yn (int, double);

#if I386
#include <runtime/i386/mathinline.h>
#endif

#ifdef __cplusplus
}
#endif

#endif /* math.h */
