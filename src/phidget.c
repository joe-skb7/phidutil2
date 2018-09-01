// SPDX-License-Identifier: GPL-2.0
#include <phidget.h>

#include <stddef.h>
#include <phidget22.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct phidget {
	PhidgetDigitalOutputHandle channel;	/* libphidget object */
	int32_t serial;				/* board serial number */
	int channel_no;				/* relay number */
};

/**
 * Constructs the phidget object for specified relay number and serial number.
 *
 * @param serial Phidget serial number
 * @param channel_no Relay port number
 * @return Constructed object or NULL on error
 */
struct phidget *phidget_create(int32_t serial, int channel_no)
{
	struct phidget *obj;
	PhidgetReturnCode res;

	/* Check for default values */
	if (serial == -1)
		serial = PHIDGET_SERIALNUMBER_ANY;
	if (channel_no == -1)
		channel_no = PHIDGET_CHANNEL_ANY;

	obj = malloc(sizeof(*obj));
	if (obj == NULL)
		return NULL;

	res = PhidgetDigitalOutput_create(&obj->channel);
	if (res != EPHIDGET_OK) {
		fprintf(stderr,
			"Error: Can't create phidget channel: err = %#x\n",
			res);
		goto err1;
	}

	res = Phidget_setDeviceSerialNumber((PhidgetHandle)obj->channel,
					    serial);
	if (res) {
		fprintf(stderr,
			"Error: Unable to set serial number: err = %#x\n", res);
		goto err2;
	}

	res = Phidget_setChannel((PhidgetHandle)obj->channel, channel_no);
	if (res) {
		fprintf(stderr, "Error: Unable to set channel: err = %#x\n",
				res);
		goto err2;
	}

	res = Phidget_openWaitForAttachment((PhidgetHandle)obj->channel,
					    PHIDGET_TIMEOUT_DEFAULT);
	if (res == EPHIDGET_TIMEOUT) {
		fprintf(stderr,
			"Error: Device is not connected or sudo is needed\n");
		goto err3;
	} else if (res != EPHIDGET_OK) {
		fprintf(stderr, "Error: Failed to open channel: err = %#x\n",
				res);
		goto err3;
	}

	obj->serial = serial;
	obj->channel_no = channel_no;

	return obj;

err3:
	Phidget_close((PhidgetHandle)obj->channel);
err2:
	PhidgetDigitalOutput_delete(&obj->channel);
err1:
	free(obj);
	return NULL;
}

/**
 * Phidget object destructor.
 *
 * @param obj Phidget object
 */
void phidget_destroy(struct phidget *obj)
{
	assert(obj != NULL);
	assert(obj->channel != NULL);

	/* Do not call these as they will always release relay to OFF state */
#if 0
	Phidget_close((PhidgetHandle)obj->channel);
	PhidgetDigitalOutput_delete(&obj->channel);
#endif

	free(obj);
}

/**
 * Get relay state (on or off).
 *
 * @param obj Phidget object
 * @param[out] state Variable to store relay state to
 * @return true on success or false on fail
 */
bool phidget_get_state(const struct phidget *obj, int *state)
{
	PhidgetReturnCode res;

	assert(obj != NULL);
	assert(state != NULL);

	res = PhidgetDigitalOutput_getState(obj->channel, state);
	if (res != EPHIDGET_OK) {
		fprintf(stderr, "Error: Can't get state: %#x\n", res);
		return false;
	}

	return true;
}

/**
 * Set relay to state (on or off).
 *
 * @param obj Phidget object
 * @param state State to set relay to
 * @return true on success or false on fail
 */
bool phidget_set_state(const struct phidget *obj, int state)
{
	PhidgetReturnCode res;

	assert(obj != NULL);

	res = PhidgetDigitalOutput_setState(obj->channel, state);
	if (res != EPHIDGET_OK) {
		fprintf(stderr, "Error: Can't set state: %#x\n", res);
		return false;
	}

	return true;
}

/**
 * Enable libphidget22 verbose logging to stdout.
 */
void phidget_enable_logging(void)
{
	PhidgetLog_enable(PHIDGET_LOG_INFO, NULL);
}
