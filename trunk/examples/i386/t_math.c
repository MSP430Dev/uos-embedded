/*
 * Testing task switching.
 */
#include "runtime/lib.h"
#include "runtime/math.h"
#include "kernel/uos.h"

char task [0x200];

void hello (void *arg)
{
	double d;
	int i;
	long long ll;

	d = acos (d);
	d = acosh (d);
	d = asin (d);
	d = asinh (d);
	d = atan (d);
	d = atan2 (d, d);
	d = atanh (d);
	d = cbrt (d);
	d = ceil (d);
	d = copysign (d, d);
	d = cos (d);
	d = cosh (d);
	d = erf (d);
	d = erfc (d);
	d = exp (d);
	d = exp2 (d);
	d = expm1 (d);
	d = fabs (d);
	d = fdim (d, d);
	d = floor (d);
	d = fma (d, d, d);
	d = fmax (d, d);
	d = fmin (d, d);
	d = fmod (d, d);
	d = frexp (d, &i);
	d = hypot (d, d);
	i = ilogb (d);

	d = j0 (d);
	d = j1 (d);
	d = jn (i, d);

	d = ldexp (d, i);
	d = lgamma (d);
	ll = llrint (d);
	ll = llround (d);
	d = log (d);
	d = log10 (d);
	d = log1p (d);
	d = log2 (d);
	d = logb (d);
	i = lrint (d);
	i = lround (d);
	d = modf (d, &d);
	d = nan ("a");
	d = nearbyint (d);
	d = nextafter (d, d);
	d = pow (d, d);
	d = remainder (d, d);
	d = remquo (d, d, &i);
	d = rint (d);
	d = round (d);

	d = scalb (d, d);

	d = scalbln (d, i);
	d = scalbn (d, i);
	d = sin (d);
	d = sinh (d);
	d = sqrt (d);
	d = tan (d);
	d = tanh (d);
	d = tgamma (d);
	d = trunc (d);

	d = y0 (d);
	d = y1 (d);
	d = yn (i, d);
#if 0
	/* Float. */
	{
	float f;

	f = acosf (f);
	f = acoshf (f);
	f = asinf (f);
	f = asinhf (f);
	f = atan2f (f, f);
	f = atanf (f);
	f = atanhf (f);
	f = cbrtf (f);
	f = ceilf (f);
	f = copysignf (f, f);
	f = cosf (f);
	f = coshf (f);
	f = erfcf (f);
	f = erff (f);
	f = exp2f (f);
	f = expf (f);
	f = expm1f (f);
	f = fabsf (f);
	f = fdimf (f, f);
	f = floorf (f);
	f = fmaf (f, f, f);
	f = fmaxf (f, f);
	f = fminf (f, f);
	f = fmodf (f, f);
	f = frexpf (f, &i);
	f = hypotf (f, f);
	f = ldexpf (f, i);
	f = lgammaf (f);
	ll = llrintf (f);
	ll = llroundf (f);
	f = log10f (f);
	f = log1pf (f);
	f = log2f (f);
	f = logbf (f);
	i = ilogbf (f);
	i = lrintf (f);
	i = lroundf (f);
	f = logf (f);
	f = modff (f, &f);
	f = nanf ("a");
	f = nearbyintf (f);
	f = nextafterf (f, f);
	f = powf (f, f);
	f = remainderf (f, f);
	f = remquof (f, f, &i);
	f = rintf (f);
	f = roundf (f);
	f = scalblnf (f, i);
	f = scalbnf (f, i);
	f = sinf (f);
	f = sinhf (f);
	f = sqrtf (f);
	f = tanf (f);
	f = tanhf (f);
	f = tgammaf (f);
	f = truncf (f);
	}
#endif
#if 0
	/* Long double. */
	{
	long double ld;

	ld = acoshl (ld);
	ld = acosl (ld);
	ld = asinhl (ld);
	ld = asinl (ld);
	ld = atan2l (ld, ld);
	ld = atanhl (ld);
	ld = atanl (ld);
	ld = cbrtl (ld);
	ld = ceill (ld);
	ld = copysignl (ld, ld);
	ld = coshl (ld);
	ld = cosl (ld);
	ld = erfcl (ld);
	ld = erfl (ld);
	ld = exp2l (ld);
	ld = expl (ld);
	ld = expm1l (ld);
	ld = fabsl (ld);
	ld = fdiml (ld, ld);
	ld = floorl (ld);
	ld = fmal (ld, ld, ld);
	ld = fmaxl (ld, ld);
	ld = fminl (ld, ld);
	ld = fmodl (ld, ld);
	ld = frexpl (ld, &i);
	ld = hypotl (ld, ld);
	ld = ldexpl (ld, i);
	ld = lgammal (ld);
	ld = log10l (ld);
	ld = log1pl (ld);
	ld = log2l (ld);
	ld = logbl (ld);
	ld = logl (ld);
	i = ilogbl (ld);
	ld = modfl (ld, &ld);
	ld = nanl ("a");
	ld = nearbyintl (ld);
	ld = nextafterl (ld, ld);
	ld = nexttowardl (ld, ld);
	d = nexttoward (d, ld);
	f = nexttowardf (f, ld);
	ld = powl (ld, ld);
	ld = remainderl (ld, ld);
	ld = remquol (ld, ld, &i);
	ld = rintl (ld);
	i = lrintl (ld);
	ll = llrintl (ld);
	ld = roundl (ld);
	i = lroundl (ld);
	ll = llroundl (ld);
	ld = scalblnl (ld, i);
	ld = scalbnl (ld, i);
	ld = sinhl (ld);
	ld = sinl (ld);
	ld = sqrtl (ld);
	ld = tanhl (ld);
	ld = tanl (ld);
	ld = tgammal (ld);
	ld = truncl (ld);
	}
#endif
	for (;;) {
		debug_printf ("Hello from `%s'! (Press Enter)\n", arg);
		debug_getchar ();
	}
}

void uos_init (void)
{
	debug_puts ("\nTesting task.\n");
	task_create (hello, "task", "hello", 1, task, sizeof (task));
}
