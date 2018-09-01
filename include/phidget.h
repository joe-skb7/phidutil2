/* SPDX-License-Identifier: GPL-2.0 */
#ifndef PHIDGET_H
#define PHIDGET_H

#include <stdbool.h>
#include <stdint.h>

struct phidget;

struct phidget *phidget_create(int32_t serial, int channel_no);
void phidget_destroy(struct phidget *obj);
bool phidget_get_state(const struct phidget *obj, int *state);
bool phidget_set_state(const struct phidget *obj, int state);
void phidget_enable_logging(void);

#endif /* PHIDGET_H */
