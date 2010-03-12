#ifdef __cplusplus
# define __MATH_INLINE	__inline
#else
# define __MATH_INLINE	extern __inline
#endif

__MATH_INLINE float sqrtf (float x)
{
	register float result;
	asm volatile ("sqrt.s	%0, %1" : "=f" (result) : "f" (x));
	return result;
}
