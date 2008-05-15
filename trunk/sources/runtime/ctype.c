#include <runtime/lib.h>

#define	_U	0x01
#define	_L	0x02
#define	_N	0x04
#define	_S	0x08
#define _P	0x10
#define _C	0x20
#define	_X	0x40
#define	_E	0x80

const unsigned char _ctype_ [257] = {
	0,
	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C,
	_C,	_S,	_S,	_S,	_S,	_S,	_C,	_C,
	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C,
	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C,

	_S,	_P,	_P,	_P,	_P,	_P,	_P,	_P,
	_P,	_P,	_P,	_P,	_P,	_P,	_P,	_P,
	_N,	_N,	_N,	_N,	_N,	_N,	_N,	_N,
	_N,	_N,	_P,	_P,	_P,	_P,	_P,	_P,

	_P,	_U|_X,	_U|_X,	_U|_X,	_U|_X,	_U|_X,	_U|_X,	_U,
	_U,	_U,	_U,	_U,	_U,	_U,	_U,	_U,
	_U,	_U,	_U,	_U,	_U,	_U,	_U,	_U,
	_U,	_U,	_U,	_P,	_P,	_P,	_P,	_P,

	_P,	_L|_X,	_L|_X,	_L|_X,	_L|_X,	_L|_X,	_L|_X,	_L,
	_L,	_L,	_L,	_L,	_L,	_L,	_L,	_L,
	_L,	_L,	_L,	_L,	_L,	_L,	_L,	_L,
	_L,	_L,	_L,	_P,	_P,	_P,	_P,	_C,

	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C,
	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C,
	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C,
	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C,

	_E,	_E,	_E,	_E,	_E,	_E,	_E,	_E,
	_E,	_E,	_E,	_E,	_E,	_E,	_E,	_E,
	_E,	_E,	_E,	_E,	_E,	_E,	_E,	_E,
	_E,	_E,	_E,	_E,	_E,	_E,	_E,	_E,

	_E,	_E,	_E,	_E,	_E,	_E,	_E,	_E,
	_E,	_E,	_E,	_E,	_E,	_E,	_E,	_E,
	_E,	_E,	_E,	_E,	_E,	_E,	_E,	_E,
	_E,	_E,	_E,	_E,	_E,	_E,	_E,	_E,

	_E,	_E,	_E,	_E,	_E,	_E,	_E,	_E,
	_E,	_E,	_E,	_E,	_E,	_E,	_E,	_E,
	_E,	_E,	_E,	_E,	_E,	_E,	_E,	_E,
	_E,	_E,	_E,	_E,	_E,	_E,	_E,	_E,
};
