#include <phidget.h>

#include <stddef.h>
#include <phidget22.h>

struct phidget {
	PhidgetDigitalOutputHandle channel;	/* libphidget object */
	int32_t serial;				/* board serial number */
	int port;				/* relay number */
};

/**
 * Phidget object constructor.
 *
 * @param serial Phidget serial number
 * @param channel_no Relay number
 * @return Constructed object or NULL on error
 */
struct phidget *phidget_create(int32_t serial, int channel_no)
{
	struct phidget *obj;
	PhidgetReturnCode res;

	obj = malloc(sizeof(*obj));
	if (obj == NULL)
		return NULL;

	res = PhidgetDigitalOutput_create(&obj->channel);
	if (res != EPHIDGET_OK) {
		fprintf(stderr, "Error: Can't create phidget channel: "
				"err = %#x\n", res);
		goto err1;
	}

	res = Phidget_setDeviceSerialNumber((PhidgetHandle)obj->channel,
					    serial);
	if (res) {
		fprintf(stderr, "Error: Unable to set serial number: "
				"err = %#x\n", res);
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
	if (res != EPHIDGET_OK) {
		fprintf(stderr, "Error: Failed to open channel: err = %#x\n",
				res);
		goto err3;
	}

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

	Phidget_close((PhidgetHandle)obj->channel);
	PhidgetDigitalOutput_delete(&obj->channel);
	free(obj);
}

/**
 * Enable libphidget22 verbose logging to stdout.
 */
void phidget_enable_logging(void)
{
	PhidgetLog_enable(PHIDGET_LOG_INFO, NULL);
}
