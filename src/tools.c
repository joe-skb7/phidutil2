// SPDX-License-Identifier: GPL-2.0
#include <tools.h>

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>

/**
 * Sleep for specified time in milliseconds.
 *
 * @param msec Time to sleep
 */
void msleep(int msec)
{
	struct timespec ts;

	ts.tv_sec = msec / 1000;
	ts.tv_nsec = (msec % 1000) * 1000000;

	nanosleep(&ts, NULL);
}

/**
 * Convert string s to int out.
 *
 * @param[out] out The converted int; cannot be NULL
 * @param[in] s Input string to be converted; cannot be NULL
 *              The format is the same as strtol, except that the following are
 *              inconvertible:
 *                - empty string
 *                - leading whitespace
 *                - any trailing characters that are not part of the number
 * @param[in] base Base to interpret string in. Same range as strtol (2 to 36)
 * @return Indicates if the operation succeeded, or why it failed
 */
int str2int(int *out, char *s, int base)
{
	char *end;
	long l;

	assert(out != NULL);
	assert(s != NULL);
	assert(base >= 2 && base <= 36);

	if (s[0] == '\0' || isspace(s[0]))
		return STR2INT_INCONVERTIBLE;

	errno = 0;
	l = strtol(s, &end, base);

	/* Both checks are needed because INT_MAX == LONG_MAX is possible */
	if (l > INT_MAX || (errno == ERANGE && l == LONG_MAX))
		return STR2INT_OVERFLOW;
	if (l < INT_MIN || (errno == ERANGE && l == LONG_MIN))
		return STR2INT_UNDERFLOW;
	if (*end != '\0')
		return STR2INT_INCONVERTIBLE;

	*out = l;
	return STR2INT_SUCCESS;
}
