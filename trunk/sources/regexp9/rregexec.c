#include "regexp9.h"
#include "regpriv.h"

/*
 *  return	0 if no match
 *		>0 if a match
 *		<0 if we ran out of _relist space
 */
static int
rregexec1(regexp_t *progp,	/* program to run */
	unsigned short *bol,	/* string to run machine on */
	regexp_match_t *mp,	/* subexpression elements */
	int ms,			/* number of elements at mp */
	regexp_ljunk_t *j)
{
	int flag=0;
	regexp_instr_t *inst;
	regexp_list_t *tlp;
	unsigned short *s;
	int i, checkstart;
	unsigned short r, *rp, *ep;
	regexp_list_t* tl;	/* This list, next list */
	regexp_list_t* nl;
	regexp_list_t* tle;	/* ends of this and next list */
	regexp_list_t* nle;
	int match;
	unsigned short *p;

	match = 0;
	checkstart = j->startchar;
	if(mp)
		for(i=0; i<ms; i++) {
			mp[i].s.rsp = 0;
			mp[i].e.rep = 0;
		}
	j->relist[0][0].inst = 0;
	j->relist[1][0].inst = 0;

	/* Execute machine once for each character, including terminal NUL */
	s = j->rstarts;
	do{

		/* fast check for first char */
		if(checkstart) {
			switch(j->starttype) {
			case RUNE:
				p = _runestrchr(s, j->startchar);
				if(p == 0 || p == j->reol)
					return match;
				s = p;
				break;
			case BOL:
				if(s == bol)
					break;
				p = _runestrchr(s, '\n');
				if(p == 0 || s == j->reol)
					return match;
				s = p+1;
				break;
			}
		}

		r = *s;

		/* switch run lists */
		tl = j->relist[flag];
		tle = j->reliste[flag];
		nl = j->relist[flag^=1];
		nle = j->reliste[flag];
		nl->inst = 0;

		/* Add first instruction to current list */
		_rrenewemptythread(tl, progp->startinst, ms, s);

		/* Execute machine until current list is empty */
		for(tlp=tl; tlp->inst; tlp++){
			for(inst=tlp->inst; ; inst = inst->u2.next){
				switch(inst->type){
				case RUNE:	/* regular character */
					if(inst->u1.r == r)
						if(_renewthread(nl, inst->u2.next, ms, &tlp->se)==nle)
							return -1;
					break;
				case LBRA:
					tlp->se.m[inst->u1.subid].s.rsp = s;
					continue;
				case RBRA:
					tlp->se.m[inst->u1.subid].e.rep = s;
					continue;
				case ANY:
					if(r != '\n')
						if(_renewthread(nl, inst->u2.next, ms, &tlp->se)==nle)
							return -1;
					break;
				case ANYNL:
					if(_renewthread(nl, inst->u2.next, ms, &tlp->se)==nle)
							return -1;
					break;
				case BOL:
					if(s == bol || *(s-1) == '\n')
						continue;
					break;
				case EOL:
					if(s == j->reol || r == 0 || r == '\n')
						continue;
					break;
				case CCLASS:
					ep = inst->u1.cp->end;
					for(rp = inst->u1.cp->spans; rp < ep; rp += 2)
						if(r >= rp[0] && r <= rp[1]){
							if(_renewthread(nl, inst->u2.next, ms, &tlp->se)==nle)
								return -1;
							break;
						}
					break;
				case NCCLASS:
					ep = inst->u1.cp->end;
					for(rp = inst->u1.cp->spans; rp < ep; rp += 2)
						if(r >= rp[0] && r <= rp[1])
							break;
					if(rp == ep)
						if(_renewthread(nl, inst->u2.next, ms, &tlp->se)==nle)
							return -1;
					break;
				case OR:
					/* evaluate right choice later */
					if(_renewthread(tlp, inst->u1.right, ms, &tlp->se) == tle)
						return -1;
					/* efficiency: advance and re-evaluate */
					continue;
				case END:	/* Match! */
					match = 1;
					tlp->se.m[0].e.rep = s;
					if(mp != 0)
						_renewmatch(mp, ms, &tlp->se);
					break;
				}
				break;
			}
		}
		if(s == j->reol)
			break;
		checkstart = j->startchar && nl->inst==0;
		s++;
	}while(r);
	return match;
}

static int
rregexec2(regexp_t *progp,	/* program to run */
	unsigned short *bol,	/* string to run machine on */
	regexp_match_t *mp,	/* subexpression elements */
	int ms,			/* number of elements at mp */
	regexp_ljunk_t *j
)
{
	regexp_list_t relist0[5*LISTSIZE], relist1[5*LISTSIZE];

	/* mark space */
	j->relist[0] = relist0;
	j->relist[1] = relist1;
	j->reliste[0] = relist0 + nelem(relist0) - 2;
	j->reliste[1] = relist1 + nelem(relist1) - 2;

	return rregexec1(progp, bol, mp, ms, j);
}

extern int
regexp_execute_unicode(regexp_t *progp,	/* program to run */
	unsigned short *bol,	/* string to run machine on */
	regexp_match_t *mp,	/* subexpression elements */
	int ms)			/* number of elements at mp */
{
	regexp_ljunk_t j;
	regexp_list_t relist0[LISTSIZE], relist1[LISTSIZE];
	int rv;

	/*
 	 *  use user-specified starting/ending location if specified
	 */
	j.rstarts = bol;
	j.reol = 0;
	if(mp && ms>0){
		if(mp->s.sp)
			j.rstarts = mp->s.rsp;
		if(mp->e.ep)
			j.reol = mp->e.rep;
	}
	j.starttype = 0;
	j.startchar = 0;
	if(progp->startinst->type == RUNE && progp->startinst->u1.r < 0x80) {
		j.starttype = RUNE;
		j.startchar = progp->startinst->u1.r;
	}
	if(progp->startinst->type == BOL)
		j.starttype = BOL;

	/* mark space */
	j.relist[0] = relist0;
	j.relist[1] = relist1;
	j.reliste[0] = relist0 + nelem(relist0) - 2;
	j.reliste[1] = relist1 + nelem(relist1) - 2;

	rv = rregexec1(progp, bol, mp, ms, &j);
	if(rv >= 0)
		return rv;
	rv = rregexec2(progp, bol, mp, ms, &j);
	if(rv >= 0)
		return rv;
	return -1;
}