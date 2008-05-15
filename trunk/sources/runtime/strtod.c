/*
 * An alternative implemtation of "strtod()" that is both
 * simplier, and thread-safe.
 */
#include <runtime/lib.h>
#include <runtime/math.h>

static const double scaler10 [] = {
	1.0, 1e10, 1e20, 1e30, 1e40, 1e50, 1e60, 1e70, 1e80, 1e90
};

static const double scaler1 [] = {
	1.0, 10.0, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9
};

static const double pastpoint [] = {
	1e-1, 1e-2, 1e-3, 1e-4, 1e-5, 1e-6, 1e-7, 1e-8, 1e-9,
	1e-10, 1e-11, 1e-12, 1e-13, 1e-14, 1e-15, 1e-16, 1e-17, 1e-18, 1e-19,
	1e-20, 1e-21, 1e-22, 1e-23, 1e-24, 1e-25, 1e-26, 1e-27, 1e-28, 1e-29,
	1e-30, 1e-31, 1e-32, 1e-33, 1e-34, 1e-35, 1e-36, 1e-37, 1e-38, 1e-39,
	1e-40, 1e-41, 1e-42, 1e-43, 1e-44, 1e-45, 1e-46, 1e-47, 1e-48, 1e-49,
	1e-50, 1e-51, 1e-52, 1e-53, 1e-54, 1e-55, 1e-56, 1e-57, 1e-58, 1e-59,
};

double strtod (const unsigned char *str, unsigned char **endp)
{
	double result = 0.0;
	int is_negative = 0;

	while (isspace (*str)) {
		str++;
	}
	if (*str == '-') {
		str++;
		is_negative = 1;
	} else if (*str == '+') {
		str++;
	}
	while (isdigit (*str)) {
		result = result * 10.0 + (*str - '0');
		str++;
	}
	if (*str == '.') {
		int n = 0;

		str++;
		while (isdigit (*str)) {
			if (n < sizeof (pastpoint) / sizeof (pastpoint[0])) {
				result += pastpoint[n] * (*str - '0');
				n++;
			}
			str++;
		}
	}
	if (*str == 'e' || *str == 'E') {
		int exp_val = 0;
		int is_neg_exp = 0;
		const unsigned char *exp_start = str;

		str++;
		if (*str == '-') {
			is_neg_exp = 1;
			str++;
		} else if (*str == '+') {
			str++;
		}
		if (! isdigit (*str)) {
			str = exp_start;
		} else {
			double scaler = 1.0;

			while (isdigit (*str)) {
				exp_val = exp_val * 10 + *str - '0';
				str++;
			}
			if (exp_val >= 1000) {
				if (is_neg_exp) {
					result = 0.0;
				} else {
					result = DBL_MAX;
				}
				goto done;
			}
			while (exp_val >= 100) {
				scaler *= 1.0e100;
				exp_val -= 100;
			}
			scaler *= scaler10 [exp_val / 10] *
				scaler1 [exp_val % 10];
			if (is_neg_exp) {
				scaler = 1.0 / scaler;
			}
			result *= scaler;
		}

	}
done:
	if (endp) {
		*endp = (unsigned char*) str;
	}
	if (is_negative && result != 0.0) {
		result = -result;
	}
	return result;
}
