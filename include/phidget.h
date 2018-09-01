#ifndef PHIDGET_H
#define PHIDGET_H

#include <stdint.h>

struct phidget;

struct phidget *phidget_create(int32_t serial, int channel_no);
void phidget_destroy(struct phidget *obj);
void phidget_enable_logging(void);

#endif /* PHIDGET_H */
