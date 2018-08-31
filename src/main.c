#include <stddef.h>
#include <phidget22.h>

#include <tools.h>

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define EXIT_SIGNAL	128	/* terminated by signal */
#define MAX_PORT	3	/* PhidgetInterfaceKit 0/0/4 has 4 relays */

struct params {
	bool verbose;		/* print libphidget log */
	int32_t serial;		/* phidget serial number */
	int channel;		/* channel number (0..MAX_PORT) */
	int state;		/* relay state: 0 or 1 */
};

static PhidgetDigitalOutputHandle channel;

static void print_usage(const char *app)
{
	printf("Usage: %s [-v] [-s serialno] [-p portno] state\n\n"
		"Options:\n"
		"\t-v\n"
		"\t\tVerbose output (enable libphidget logging)\n"
		"\t-s serialno\n"
		"\t\tPhidget serial number\n"
		"\t-p portno\n"
		"\t\tPort (relay) number (0-3)\n"
		"\tstate: 0 (OFF) or 1 (ON)\n",
		app);
}

static bool parse_params(int argc, char *argv[], struct params *params)
{
	int c;
	int res;

	/* Default values */
	memset(params, 0, sizeof(*params));
	params->serial = PHIDGET_SERIALNUMBER_ANY;
	params->channel = PHIDGET_CHANNEL_ANY;

	/* Parse and validate optional parameters */
	while ((c = getopt(argc, argv, "s:p:v")) != -1) {
		switch (c) {
		case 'v':
			params->verbose = true;
			break;
		case 's':
			res = str2int(&params->serial, optarg, 10);
			if (res) {
				fprintf(stderr, "Error: Wrong serial\n");
				return false;
			}
			break;
		case 'p':
			res = str2int(&params->channel, optarg, 10);
			if (res || params->channel > MAX_PORT) {
				fprintf(stderr, "Error: Wrong port\n");
				return false;
			}
			break;
		default:
			fprintf(stderr, "Error: Invalid option: -%c\n", optopt);
			return false;
		}
	}

	/* Parse and validate "state" parameter (mandatory) */
	if (argv[optind] == NULL) {
		fprintf(stderr, "Error: Missing state\n");
		return false;
	}
	res = str2int(&params->state, argv[optind], 10);
	if (res || (params->state != 0 && params->state != 1)) {
		fprintf(stderr, "Error: Wrong state\n");
		return false;
	}

	return true;
}

static void cleanup(void)
{
	PhidgetReturnCode res;

	if (channel == NULL)
		return;

	res = Phidget_close((PhidgetHandle)channel);
	if (res != EPHIDGET_OK)
		fprintf(stderr, "Warning: Can't close phidget: %#x\n", res);

	res = PhidgetDigitalOutput_delete(&channel);
	if (res != EPHIDGET_OK) {
		fprintf(stderr, "Warning: Can't delete phidget channel: %#x\n",
			res);
	}
}

static void sig_handler(int signum)
{
	if (signum == SIGINT) {
		cleanup();
		exit(EXIT_SIGNAL + signum);
	}
}

static void set_params(void)
{
	/* TODO */

	Phidget_setDeviceSerialNumber((PhidgetHandle)channel,
				      PHIDGET_SERIALNUMBER_ANY);
	Phidget_setHubPort((PhidgetHandle)channel, PHIDGET_HUBPORT_ANY);
	Phidget_setChannel((PhidgetHandle)channel, 0);
}

int main(int argc, char *argv[])
{
	PhidgetReturnCode res;
	struct params params;
	int ret = EXIT_SUCCESS;

	if (!parse_params(argc, argv, &params)) {
		print_usage(argv[0]);
		return EXIT_FAILURE;
	}

	/* Enable logging to stdout */
//	if (verbose)
//		PhidgetLog_enable(PHIDGET_LOG_INFO, NULL);

	res = PhidgetDigitalOutput_create(&channel);
	if (res != EPHIDGET_OK) {
		fprintf(stderr, "Error: Can't create phidget channel: %#x\n",
				res);
		return EXIT_FAILURE;
	}

	if (signal(SIGINT, sig_handler) == SIG_ERR)
		fprintf(stderr, "Warning: Can't catch SIGINT\n");

	set_params();

	res = Phidget_openWaitForAttachment((PhidgetHandle)channel,
					    PHIDGET_TIMEOUT_DEFAULT);
	if (res != EPHIDGET_OK) {
		fprintf(stderr, "Error: Failed to open channel: %#x\n", res);
		goto err;
	}

	/* TODO: do something meaningful here */
	while (1) {
		int state;

		res = PhidgetDigitalOutput_getState(channel, &state);
		if (res != EPHIDGET_OK) {
			fprintf(stderr, "Error: Can't get state: %#x\n", res);
			goto err;
		}

		res = PhidgetDigitalOutput_setState(channel, !state);
		if (res != EPHIDGET_OK) {
			fprintf(stderr, "Error: Can't set state: %#x\n", res);
			goto err;
		}

		msleep(500);
	}

err:
	cleanup();
	return ret;
}
