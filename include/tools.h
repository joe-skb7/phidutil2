/* SPDX-License-Identifier: GPL-2.0 */
#ifndef TOOLS_H
#define TOOLS_H

enum str2int_err {
	STR2INT_SUCCESS		= 0,
	STR2INT_OVERFLOW	= -1,
	STR2INT_UNDERFLOW	= -2,
	STR2INT_INCONVERTIBLE	= -3
};

void msleep(int msec);
int str2int(int *out, char *s, int base);

#endif /* TOOLS_H */
