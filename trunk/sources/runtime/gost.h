/*
 * The GOST 28147-89 cipher.
 *
 * This code is placed in the public domain.
 */
typedef struct _gost_t {
	/* Byte-at-a-time substitution boxes */
	unsigned char k87 [256];
	unsigned char k65 [256];
	unsigned char k43 [256];
	unsigned char k21 [256];
} gost_t;

/*
 * A 32-bit data type
 */
#ifdef __alpha  /* Any other 64-bit machines? */
typedef unsigned int word32_t;
#else
typedef unsigned long word32_t;
#endif

void gost_init (gost_t *g, const unsigned char k1[16],
	const unsigned char k2[16], const unsigned char k3[16],
	const unsigned char k4[16], const unsigned char k5[16],
	const unsigned char k6[16], const unsigned char k7[16],
	const unsigned char k8[16]);
void gost_crypt (gost_t *g, word32_t const in[2], word32_t out[2],
	word32_t const key[8]);
void gost_decrypt (gost_t *g, word32_t const in[2], word32_t out[2],
	word32_t const key[8]);
void gost_create_gamma (gost_t *g, word32_t *gamma, word32_t const synchro[2],
	word32_t const key[8]);
void gost_cfb_encrypt (gost_t *g, word32_t const *in, word32_t *out, int len,
	word32_t iv[2], word32_t const key[8]);
void gost_cfb_decrypt (gost_t *g, word32_t const *in, word32_t *out, int len,
	word32_t iv[2], word32_t const key[8]);
void gost_mac (gost_t *g, word32_t const *in, int len, word32_t out[2],
	word32_t const key[8]);
