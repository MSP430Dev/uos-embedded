#include <string.h>
#include "regexp9.h"
#include "regpriv.h"

/*
 *  save a new match in mp
 */
extern void
_renewmatch(regexp_match_t *mp, int ms, regexp_matchlist_t *sp)
{
	int i;

	if(mp==0 || ms<=0)
		return;
	if(mp[0].s.sp==0 || sp->m[0].s.sp<mp[0].s.sp ||
	   (sp->m[0].s.sp==mp[0].s.sp && sp->m[0].e.ep>mp[0].e.ep)){
		for(i=0; i<ms && i<NSUBEXP; i++)
			mp[i] = sp->m[i];
		for(; i<ms; i++)
			mp[i].s.sp = mp[i].e.ep = 0;
	}
}

/*
 * Note optimization in _renewthread:
 * 	*lp must be pending when _renewthread called; if *l has been looked
 *		at already, the optimization is a bug.
 */
extern regexp_list_t*
_renewthread(regexp_list_t *lp,		/* _relist to add to */
	regexp_instr_t *ip,		/* instruction to add */
	int ms,
	regexp_matchlist_t *sep)	/* pointers to subexpressions */
{
	regexp_list_t *p;

	for(p=lp; p->inst; p++){
		if(p->inst == ip){
			if(sep->m[0].s.sp < p->se.m[0].s.sp){
				if(ms > 1)
					p->se = *sep;
				else
					p->se.m[0] = sep->m[0];
			}
			return 0;
		}
	}
	p->inst = ip;
	if(ms > 1)
		p->se = *sep;
	else
		p->se.m[0] = sep->m[0];
	(++p)->inst = 0;
	return p;
}

/*
 * same as renewthread, but called with
 * initial empty start pointer.
 */
extern regexp_list_t*
_renewemptythread(regexp_list_t *lp,	/* _relist to add to */
	regexp_instr_t *ip,		/* instruction to add */
	int ms,
	const char *sp)			/* pointers to subexpressions */
{
	regexp_list_t *p;

	for(p=lp; p->inst; p++){
		if(p->inst == ip){
			if(sp < p->se.m[0].s.sp) {
				if(ms > 1)
					memset(&p->se, 0, sizeof(p->se));
				p->se.m[0].s.sp = sp;
			}
			return 0;
		}
	}
	p->inst = ip;
	if(ms > 1)
		memset(&p->se, 0, sizeof(p->se));
	p->se.m[0].s.sp = sp;
	(++p)->inst = 0;
	return p;
}

extern regexp_list_t*
_rrenewemptythread(regexp_list_t *lp,	/* _relist to add to */
	regexp_instr_t *ip,		/* instruction to add */
	int ms,
	unsigned short *rsp)		/* pointers to subexpressions */
{
	regexp_list_t *p;

	for(p=lp; p->inst; p++){
		if(p->inst == ip){
			if(rsp < p->se.m[0].s.rsp) {
				if(ms > 1)
					memset(&p->se, 0, sizeof(p->se));
				p->se.m[0].s.rsp = rsp;
			}
			return 0;
		}
	}
	p->inst = ip;
	if(ms > 1)
		memset(&p->se, 0, sizeof(p->se));
	p->se.m[0].s.rsp = rsp;
	(++p)->inst = 0;
	return p;
}
