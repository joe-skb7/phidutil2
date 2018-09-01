// SPDX-License-Identifier: GPL-2.0
#include <phidget.h>
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
	int port;		/* relay port number (0..MAX_PORT) */
	int state;		/* relay state: 0 or 1 */
};

static struct phidget *phidget;

static void print_usage(const char *app)
{
	printf("Usage: %s [-v] [-s serialno] [-p portno] state\n\n"
		"Options:\n"
		"\t-v\t\tVerbose output\n"
		"\t-s serialno\tPhidget serial number\n"
		"\t-p portno\tRelay port number (0-3)\n"
		"\tstate\t\t0 (OFF) or 1 (ON)\n",
		app);
}

static bool parse_params(int argc, char *argv[], struct params *params)
{
	int c;
	int res;

	/* Handle "--help" case */
	if (argc == 2 && !strcmp(argv[1], "--help")) {
		print_usage(argv[0]);
		exit(EXIT_SUCCESS);
	}

	/* Default values */
	memset(params, 0, sizeof(*params));
	params->serial = -1;
	params->port = -1;

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
			res = str2int(&params->port, optarg, 10);
			if (res || params->port > MAX_PORT) {
				fprintf(stderr, "Error: Wrong port\n");
				return false;
			}
			break;
		default:
			/* Invalid option optopt */
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

static void print_params(const struct params *params)
{
	printf("Parameters:\n"
		"verbose: %s\n"
		"serial: %d\n"
		"port: %d\n"
		"state: %d\n\n",
		params->verbose ? "true" : "false",
		params->serial,
		params->port,
		params->state);
}

static void sig_handler(int signum)
{
	if (signum == SIGINT) {
		phidget_destroy(phidget);
		exit(EXIT_SIGNAL + signum);
	}
}

int main(int argc, char *argv[])
{
	struct params params;

	if (!parse_params(argc, argv, &params)) {
		print_usage(argv[0]);
		return EXIT_FAILURE;
	}

	if (params.verbose) {
		print_params(&params);
		phidget_enable_logging();
	}

	if (signal(SIGINT, sig_handler) == SIG_ERR)
		fprintf(stderr, "Warning: Can't catch SIGINT\n");

	phidget = phidget_create(params.serial, params.port);
	if (phidget == NULL)
		return EXIT_FAILURE;

	//phidget_get_state(phidget, &params.state); /* XXX */
	if (!phidget_set_state(phidget, params.state))
		return EXIT_FAILURE;
	//msleep(1500);
	//phidget_destroy(phidget);
	return EXIT_SUCCESS;
}
