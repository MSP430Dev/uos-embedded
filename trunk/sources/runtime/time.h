#ifndef _TIME_H_
#define _TIME_H_

#ifndef UOS_LEAP_SECONDS
#	define UOS_LEAP_SECONDS 1
#endif

typedef long time_t;

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
	unsigned year, yday;		/* LY: year e.g. 2000, day of year 0..365; */
	int gmt_offset;			/* LY: offset from GMT in seconds; */
	uint_t sec, min, hour;		/* LY: time of day, seconds may be 60 if leap-second; */
	uint_t mday, mon, wday;		/* LY: day of month 1..31, month 0..11, week day since Sunday 0...6; */
	unsigned char tz_name[4];	/* LY: current tz-name (STD or DST from TZ); */
	sign_t isdst;			/* LY: dayligth saving flag, e.g. indicate DST from TZ. */
};

#if UOS_LEAP_SECONDS
	extern bool_t time_account_leap_seconds;
#endif /* UOS_LEAP_SECONDS */

bool_t tz_parse (unsigned char *tz_string, struct tz_t *tz);
void tz_time (time_t utz, struct tz_t *tz, struct tz_time_t *local);
int tz_offset (time_t utz, struct tz_t *tz);

#endif /* _TIME_H_ */
