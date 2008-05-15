#include <runtime/lib.h>
#include <kernel/uos.h>
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
*/

static unsigned char* parse_hms (unsigned char *s, unsigned *seconds)
{
	uint_t len, hour, min, sec;

	hour = min = sec = 0;
	/* LY: hours. */
	for (len = 0; *s >= '0' && *s <= '9'; s++) {
		len++;
		hour = hour * 10 + *s - '0';
		if (hour > 23)
			goto ballout;
	}
	if (len < 1)
		goto ballout;
	if (*s == ':') {
		/* LY: minutes. */
		for (++s, len = 0; *s >= '0' && *s <= '9'; s++) {
			if (min > 5)
				goto ballout;
			len++;
			min = min * 10 + *s - '0';
			if (min > 59)
				goto ballout;
		}
		if (len < 1)
			goto ballout;
		if (*s == ':') {
			/* LY: seconds. */
			for (++s, len = 0; *s >= '0' && *s <= '9'; s++) {
				if (sec > 5)
					goto ballout;
				len++;
				sec = sec * 10 + *s - '0';
				if (sec > 59)
					goto ballout;
			}
			if (len < 1)
				goto ballout;
		}
	}

	*seconds = hour * 3600 + min * 60 + sec;
	return s;

ballout:
	return 0;
}

static unsigned char* parse_offset (unsigned char *s, int *offset)
{
	sign_t sign;

	/* LY: parse sign of offset. */
	sign = -1;
	if (*s == '+' || *s == '-') {
		if (*s == '-')
			sign = 1;
		s++;
	}

	/* LY: parse offset digits "hh[:mm[:ss]]". */
	s = parse_hms (s, (unsigned*) offset);
	if (sign < 0)
		*offset = - *offset;
	return s;
}

static unsigned char* parse_rule (unsigned char *s, struct tz_rule_t *r)
{
	uint_t value, len;

	/* LY: we accept only M-type, but not J0 or J1. */
	if (*s++ != 'M')
		goto ballout;

	/* LY: scan for month 1..12 */
	for (value = 0; *s >= '0' && *s <= '9'; s++) {
		value = value * 10 + *s - '0';
		if (value > 12)
			goto ballout;
	}
	if (*s++ != '.' || value < 1)
		goto ballout;
	r->month = value;

	/* LY: scan for week 1..5 */
	for (value = 0; *s >= '0' && *s <= '9'; s++) {
		value = value * 10 + *s - '0';
		if (value > 5)
			goto ballout;
	}
	if (*s++ != '.' || value < 1)
		goto ballout;
	r->week = value;

	/* LY: scan for weekday 0..6 */
	for (value = len = 0; *s >= '0' && *s <= '9'; s++) {
		value = value * 10 + *s - '0';
		if (value > 6)
			goto ballout;
		len++;
	}
	if (len < 1)
		goto ballout;
	r->day = value;

	/* LY: parse optional time. */
	if (*s == '/')
		s = parse_hms (s + 1, &r->seconds);

	return s;

ballout:
	return 0;
}

bool_t tz_parse (unsigned char *str, struct tz_t *tz)
{
	unsigned char *s;

	memset (tz, 0, sizeof (*tz));

	if (! str || *str == 0) {
		tz->name_std[0] = tz->name_dst[0] = 'G';
		tz->name_std[1] = tz->name_dst[1] = 'M';
		tz->name_std[2] = tz->name_dst[2] = 'T';
		goto ok;
	}

	/* LY: scan for STD name. */
	for (s = str; *s >= 'A' && *s <= 'Z'; )
		s++;
	if (s != str + 3)
		goto ballout;
	tz->name_std[0] = str[0];
	tz->name_std[1] = str[1];
	tz->name_std[2] = str[2];

	/* LY: parse STD offset. */
	str = parse_offset (s, &tz->offset_std);
	if (! str)
		goto ballout;

	if (*str == 0) {
		/* LY: there is no DST. */
		tz->name_dst[0] = tz->name_std[0];
		tz->name_dst[1] = tz->name_std[1];
		tz->name_dst[2] = tz->name_std[2];
		tz->offset_dst = tz->offset_std;
		goto ok;
	}

	/* LY: scan for DST name. */
	for (s = str; *s >= 'A' && *s <= 'Z'; )
		s++;
	if (s == str + 3) {
		tz->name_dst[0] = str[0];
		tz->name_dst[1] = str[1];
		tz->name_dst[2] = str[2];

		/* LY: parse DST offset. */
		str = parse_offset (s, &tz->offset_dst);
		if (! str)
			goto ballout;
	} else if (s != str)
		goto ballout;

	/* LY: start = 2am of last week sunday of March. */
	tz->start.month = 3;
	tz->start.week = 5;
	tz->start.day = 0;
	tz->start.seconds = 3600 * 2;
	if (*str == ',') {
		str = parse_rule (str + 1, &tz->start);
		if (! str)
			goto ballout;
	}

	/* LY: end = 3am of last week sunday of October. */
	tz->end.month = 10;
	tz->end.week = 5;
	tz->end.day = 0;
	tz->end.seconds = 3600 * 3;
	if (*str == ',') {
		str = parse_rule (str + 1, &tz->end);
		if (! str)
			goto ballout;
	}

	if (*str != 0)
		goto ballout;

ok:
	return 1;

ballout:
	return 0;
}
