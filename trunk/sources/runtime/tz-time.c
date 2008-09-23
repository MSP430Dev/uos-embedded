#include <runtime/lib.h>
/*#include <kernel/uos.h>*/
#include <runtime/time.h>
#include "time.h"

/* LY: копия из time.h для удобства:

	struct tz_rule_t {
		uint_t month, week, day;
		unsigned seconds;
	};

	struct tz_t {
		unsigned char name_std[4];
		unsigned char name_dst[4];
		int offset_std, offset_dst;
		struct tz_rule_t start, end;
	};

	struct tz_time_t {
		unsigned year, yday;		// LY: year e.g. 2000, day of year 0..365;
		int gmt_offset;			// LY: offset from GTM in seconds;
		uint_t sec, min, hour;		// LY: time of day, seconds may be 60 if leap-second;
		uint_t mday, mon, wday;		// LY: day of month 1..31, month 0..11, week day since Sunday 0...6;
		unsigned char tz_name[4];	// LY: current tz-name (STD or DST from TZ);
		sign_t isdst;			// LY: dayligth saving flag, e.g. indicate DST from TZ.
	};

*/

#define TIME_DAY	86400L		/* LY: second in 24 hours. */
#define TIME_YEAR	31536000L	/* LY: seconds in one year. */
#define TIME_2000	946684800L	/* LY: unix seconds of 2000-01-01. */

static bool_t year_isleap (unsigned year)
{
	if (year & 3)
		return 0;

#if 0 /* LY: do it simple, because only 2000 is actual. */
	if (year % 100 == 0 && year % 400 != 0)
		return 0;
#endif

	return 1;
}

/* LY: return days in the month. */
static uint_t get_mday (unsigned year, uint_t mon) {
	static const char mdays[12] = {
		31, 28, 31, 30, 31, 30,
		31, 31, 30, 31, 30, 31
	};
	uint_t days;

	days = FETCH_BYTE (mdays + mon);
	if (days == 28 && year_isleap (year))
		days++;
	return days;
};

struct parts_t {
	unsigned year, weekday;
	int_t leap_second;
};

#if UOS_LEAP_SECONDS
bool_t time_account_leap_seconds;

#	if 0 /* LY: accounting leap seconds. */
	!/bin/sh

	LEAPS="1972-06-30 1972-12-31 1973-12-31 1974-12-31 1975-12-31 1976-12-31 1977-12-31 \
		1978-12-31 1979-12-31 1981-06-30 1982-06-30 1983-06-30 1985-06-30 1987-12-31 \
		1989-12-31 1990-12-31 1992-06-30 1993-06-30 1994-06-30 1995-12-31 1997-06-30 \
		1998-12-31 2005-12-31"

	for d in $LEAPS; do
		s=`date -u -d "$d 23:59:59" +%s`
		echo "\t/* $d 23:59:59 +1 ==" `expr $s + 1` "(unix) == */" `expr $s + 1 - 946684800`L,
	done
#	endif

static const time_t leap_seconds_table[] = {
	/* 1972-06-30 23:59:59 +1 == 78796800 (unix) == */ -867888000L,
	/* 1972-12-31 23:59:59 +1 == 94694400 (unix) == */ -851990400L,
	/* 1973-12-31 23:59:59 +1 == 126230400 (unix) == */ -820454400L,
	/* 1974-12-31 23:59:59 +1 == 157766400 (unix) == */ -788918400L,
	/* 1975-12-31 23:59:59 +1 == 189302400 (unix) == */ -757382400L,
	/* 1976-12-31 23:59:59 +1 == 220924800 (unix) == */ -725760000L,
	/* 1977-12-31 23:59:59 +1 == 252460800 (unix) == */ -694224000L,
	/* 1978-12-31 23:59:59 +1 == 283996800 (unix) == */ -662688000L,
	/* 1979-12-31 23:59:59 +1 == 315532800 (unix) == */ -631152000L,
	/* 1981-06-30 23:59:59 +1 == 362793600 (unix) == */ -583891200L,
	/* 1982-06-30 23:59:59 +1 == 394329600 (unix) == */ -552355200L,
	/* 1983-06-30 23:59:59 +1 == 425865600 (unix) == */ -520819200L,
	/* 1985-06-30 23:59:59 +1 == 489024000 (unix) == */ -457660800L,
	/* 1987-12-31 23:59:59 +1 == 567993600 (unix) == */ -378691200L,
	/* 1989-12-31 23:59:59 +1 == 631152000 (unix) == */ -315532800L,
	/* 1990-12-31 23:59:59 +1 == 662688000 (unix) == */ -283996800L,
	/* 1992-06-30 23:59:59 +1 == 709948800 (unix) == */ -236736000L,
	/* 1993-06-30 23:59:59 +1 == 741484800 (unix) == */ -205200000L,
	/* 1994-06-30 23:59:59 +1 == 773020800 (unix) == */ -173664000L,
	/* 1995-12-31 23:59:59 +1 == 820454400 (unix) == */ -126230400L,
	/* 1997-06-30 23:59:59 +1 == 867715200 (unix) == */ -78969600L,
	/* 1998-12-31 23:59:59 +1 == 915148800 (unix) == */ -31536000L,
	/* 2005-12-31 23:59:59 +1 == 1136073600 (unix) == */ 189388800L
};
#endif /* UOS_LEAP_SECONDS */

/* LY: return year (e.g. 2000...) and calculate 'tail' since year start, etc. */
static time_t time2year (time_t t, struct parts_t *p)
{
	time_t ys;
	bool_t yl;
#if UOS_LEAP_SECONDS
	const time_t *l = leap_seconds_table;
	const time_t *h = ARRAY_END (leap_seconds_table);
	const time_t *m = leap_seconds_table;
	time_t s;

	p->leap_second = 0;
	if (time_account_leap_seconds) {
		while (h >= l) {
			m = l + ((h - l) >> 1);
			s = FETCH_LONG (m);
			if (t < s) {
				h = m - 1;
				continue;
			}
			++m;
			if (t > s) {
				l = m;
				continue;
			}
			p->leap_second = 1;
			break;
		}
		t -= m - leap_seconds_table;
	}
#else
	p->leap_second = 0;
#endif

	for (p->year = 2000; t < 0; p->year -= 28)
		t += 7 * (TIME_YEAR * 4 + TIME_DAY);

	while (t >= 7 * (TIME_YEAR * 4 + TIME_DAY)) {
		p->year += 28;
		t -= 7 * (TIME_YEAR * 4 + TIME_DAY);
	}

	/* LY: day of week since Sunday, 1/1/2000 is saturday (#6). */
	for (p->weekday = 6; t >= 4 * TIME_YEAR + TIME_DAY; ) {
		p->year += 4;
		p->weekday += (365 * 4 + 1) % 7;
		t -= 4 * TIME_YEAR + TIME_DAY;
	}

	for (;;) {
		yl = year_isleap (p->year);
		ys = yl ? TIME_YEAR + TIME_DAY : TIME_YEAR;
		if (t < ys)
			break;
		t -= ys;
		p->weekday += yl ? 366 % 7 : 365 % 7;
		p->year++;
	}

	return t;
}

/* LY: calculate the 'edge' of STD/DST by given rule. */
static time_t tz_edge (time_t t, struct tz_rule_t *r)
{
	time_t e;
	struct parts_t p;
	int_t i, d;

	/* LY: seek 't' to the 1.Jan of year. */
	t -= time2year (t, &p);

	/* LY: calculate start of target month. */
	for (i = 0; i < r->month - 1; i++) {
		d = get_mday (p.year, i);
		p.weekday += d;
		t += d * TIME_DAY;
	}

	/* LY: calculate next day beyond target month. */
	e = t + get_mday (p.year, i) * TIME_DAY;

	/* LY: rollback one week from end of target month. */
	e -= TIME_DAY * 7;

	/* LY: seek to the first target weekday in month. */
	t += TIME_DAY * (i = (r->day + 777 - p.weekday) % 7);

	/* LY: seek to target week, e.g. the N'weekday. */
	for (i = 0; i < r->week; i++) {
		if (t >= e)
			/* LY: there is the last week of month. */
			break;
		t += TIME_DAY * 7;
	}

	/* LY: account time of day. */
	return t + r->seconds;
}

/* LY: choise between STD and DST. */
static bool_t tz_isdst (time_t t, struct tz_t *tz)
{
	time_t e, y;
	struct parts_t p;

	if (tz->offset_std == tz->offset_dst)
		return 0;

	/* LY: y - start of year, for bounds checking. */
	y = t - time2year (t, &p);

	/* LY: e - egde time for dst-start. */
	e = tz_edge (t + tz->offset_std, &tz->start) - tz->offset_std;
	if (t < e || e < y)
		return 0;

	/* LY: y - end of year, for bounds checking. */
	y += year_isleap (p.year) ? TIME_YEAR + TIME_DAY : TIME_YEAR;

	/* LY: e - egde time for dst-end. */
	e = tz_edge (t + tz->offset_dst, &tz->end) - tz->offset_dst;
	if (t < e && e < y)
		return 1;
	return 0;
}

void tz_time (time_t t, struct tz_t *tz, struct tz_time_t *l)
{
	unsigned char *s;
	struct parts_t p;
	uint_t d;
	unsigned v;

	l->gmt_offset = 0;
	l->tz_name[0] = 'G';
	l->tz_name[1] = 'M';
	l->tz_name[2] = 'T';
	l->tz_name[3] = 0;
	l->isdst = 0;

	if (tz) {
		l->gmt_offset = tz->offset_std;
		s = tz->name_std;
		if (tz_isdst (t, tz)) {
			l->isdst = 1;
			l->gmt_offset = tz->offset_dst;
			s = tz->name_dst;
		}
		l->tz_name[0] = s[0];
		l->tz_name[1] = s[1];
		l->tz_name[2] = s[2];
	}

	/* LY: get the year and 'tail'. */
	t = time2year (t + l->gmt_offset, &p);
	l->year = p.year;

	/* LY: day of year, time of day. */
	l->yday = t / TIME_DAY; v = t % TIME_DAY;
	l->hour = v / 3600u; v = v % 3600u;
	l->min = v / 60u; v = v % 60u;
	l->sec = v + p.leap_second;
	l->wday = (p.weekday + l->yday) % 7;

	/* LY: calendar, e.g. month and day of month. */
	for (v = l->yday, l->mon = 0; v > 0; l->mon++) {
		d = get_mday (l->year, l->mon);
		if (v < d)
			break;
		v -= d;
	}
	l->mday = v + 1;
}

int tz_offset (time_t t, struct tz_t *tz)
{
	return tz_isdst (t, tz) ? tz->offset_dst : tz->offset_std;
}
