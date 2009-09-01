/*
 *  substitution list
 */
#define nelem(x) (sizeof(x)/sizeof((x)[0]))

typedef struct _regexp_class_t	regexp_class_t;
typedef struct _regexp_instr_t	regexp_instr_t;

/*
 * Character class, each pair of rune's defines a range
 */
struct _regexp_class_t {
	unsigned short	*end;
	unsigned short	spans[64];
};

/*
 * Machine instructions
 */
struct _regexp_instr_t {
	int	type;
	union	{
		regexp_class_t	*cp;	/* class pointer */
		unsigned short	r;	/* character */
		int		subid;	/* sub-expression id for RBRA and LBRA */
		regexp_instr_t	*right;	/* right child of OR */
	} u1;
	union {		/* regexp relies on these two being in the same union */
		regexp_instr_t *left;	/* left child of OR */
		regexp_instr_t *next;	/* next instruction for CAT & LBRA */
	} u2;
};

/* max character classes per program */
#define	NCLASS	16

/*
 * Reprogram definition
 */
struct _regexp_t {
	regexp_instr_t	*startinst;	/* start pc */
	regexp_class_t	class [NCLASS];	/* .data */
	regexp_instr_t	firstinst [5];	/* .text */
};

#define NSUBEXP 32
typedef struct _regexp_matchlist_t regexp_matchlist_t;

struct _regexp_matchlist_t
{
	regexp_match_t	m[NSUBEXP];
};

/* max rune ranges per character class */
#define NCCRUNE	(sizeof(regexp_class_t)/sizeof(unsigned short))

/*
 * Actions and Tokens (regexp_instr_t types)
 *
 *	02xx are operators, value == precedence
 *	03xx are tokens, i.e. operands for operators
 */
#define RUNE		0177
#define	OPERATOR	0200	/* Bitmask of all operators */
#define	START		0200	/* Start, used for marker on stack */
#define	RBRA		0201	/* Right bracket, ) */
#define	LBRA		0202	/* Left bracket, ( */
#define	OR		0203	/* Alternation, | */
#define	CAT		0204	/* Concatentation, implicit operator */
#define	STAR		0205	/* Closure, * */
#define	PLUS		0206	/* a+ == aa* */
#define	QUEST		0207	/* a? == a|nothing, i.e. 0 or 1 a's */
#define	ANY		0300	/* Any character except newline, . */
#define	ANYNL		0301	/* Any character including newline, . */
#define	NOP		0302	/* No operation, internal use only */
#define	BOL		0303	/* Beginning of line, ^ */
#define	EOL		0304	/* End of line, $ */
#define	CCLASS		0305	/* Character class, [] */
#define	NCCLASS		0306	/* Negated character class, [] */
#define	END		0377	/* Terminate: match found */

/*
 *  regexec execution lists
 */
#define LISTSIZE	10
#define BIGLISTSIZE	(10*LISTSIZE)
typedef struct _regexp_list_t regexp_list_t;
struct _regexp_list_t
{
	regexp_instr_t		*inst;	/* Reinstruction of the thread */
	regexp_matchlist_t	se;	/* matched subexpressions in this thread */
};

typedef struct _regexp_ljunk_t regexp_ljunk_t;
struct _regexp_ljunk_t
{
	regexp_list_t*	relist[2];
	regexp_list_t*	reliste[2];
	int		starttype;
	unsigned short	startchar;
	const char*	starts;
	const char*	eol;
	unsigned short*	rstarts;
	unsigned short*	reol;
};

extern regexp_list_t*	_renewthread(regexp_list_t*, regexp_instr_t*, int, regexp_matchlist_t*);
extern void		_renewmatch(regexp_match_t*, int, regexp_matchlist_t*);
extern regexp_list_t*	_renewemptythread(regexp_list_t*, regexp_instr_t*, int, const char*);
extern regexp_list_t*	_rrenewemptythread(regexp_list_t*, regexp_instr_t*, int, unsigned short*);

int _chartorune(unsigned short*, const char*);
char *_utfrune(const char*, unsigned short);
unsigned short *_runestrchr(const unsigned short*, unsigned short);
