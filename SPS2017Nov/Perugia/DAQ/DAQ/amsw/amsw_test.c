/* -*-C-*-
 * amsw_test.c --  program to test AMSWire links
 *
 *-----------------------------------------------------------------------
 *
 * Project          : Linux
 * Moduletyp        : C-Code
 * Author           : Xudong CAI
 * Created          : 30 Oct. 2001
 *
 *-----------------------------------------------------------------------
 *
 */

/************************************************************************
	Includes
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include <ctype.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <getopt.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

#include "amsw_lib.h"
#include "prod_lib.h"
//#include "aal.h"
#include "proc_comm.h"

/************************************************************************
	Defines
 ************************************************************************/

#define VERS				"AMSW-PCI Test Version 2.0"
#define AMSW_REPLY_TIMEOUT	200000

/************************************************************************
	Typedefs / Structs
 ************************************************************************/

typedef struct __amsw_task {
	int mode;			/* 0 - no action, 1 - slave, 2 - master */

	int timeout;		/* none zero value means link is active, zero mean link is disconnected */
	int retry;			/* number of retries */

	int	tx_active;		/* 1 means TX active, 0 means TX free */
	int rx_active;		/* 1 means waiting for RX, 0 means free */

	u_char *data;		/* Data */
	int size;			/* Data size */

	int cmd;			/* Flag to process command */

	u_long loops;		/* Number of loops for the task */
	u_long errs;		/* number of errors for the task */
	u_long timeouts;	/* number of timeouts */
} AMSWTask;

/************************************************************************
	Globals
 ************************************************************************/

static AMSWTask tasks[32];

/************************************************************************
	Forward declarations
 ************************************************************************/

/************************************************************************
	Locals
 ************************************************************************/

static int format = GenTypeSEQUC;
static u_long pattern = 0;
static int step = 1000;
static int debug = 0;
static u_long master = 0x0000ffff;		/* 16 master */
static u_long slave  = 0x00000000;		/* no slaves */
static char filename[128];
static int msgid;

/************************************************************************
   Function:

   show_usage - routine to show usage

   RETURNS:
 ************************************************************************/
void show_usage(char *title) {
    printf("\nUsage: %s [options]\n\n", title);
    printf("Options:    --debug     (-d) : debug flag [def: 0]\n");
    printf("            --format=n  (-f) : Test data format [def: sequence]\n");
    printf("                                0 - Shifted 0\n");
    printf("                                1 - Shifted 1\n");
    printf("                                2 - Sequence data\n");
    printf("                                3 - Random data\n");
    printf("                                4 - Alternative 0 and f\n");
    printf("                                5 - Alternative 5 and a\n");
    printf("                               10 - Fixed data in pattern\n");
    printf("            --master=x  (-m) : master channel mask [def: 0x00000000]\n");
    printf("            --pattern=x (-p) : Pattern for fixed data format, [def: 0x12345678]\n\n");
    printf("            --slave=x   (-s) : Slave chennel mask [def: 0x000000ff]\n");
    printf("            --step=n    (-t) : Display step [def: 1000]\n");
    printf("            --help -h -?     : help\n");
    exit(0);
}

/************************************************************************
   Function:

   parse_options - routine to get options

   RETURNS:
 ************************************************************************/
void parse_options(int argc, char *argv[]) {
	int c;
	int option_index = 0;
	static struct option long_options[] = {
		{"debug",   0, 0, 'd'},		/* case 0 */
		{"help",    0, 0, 'h'},		/* case 1 */
		{"format",  1, 0, 'f'},		/* case 2 */
		{"master", 	1, 0, 'm'},		/* case 3 */
		{"pattern", 1, 0, 'p'},		/* case 4 */
		{"slave",  	1, 0, 's'},		/* case 5 */
		{"step",   	1, 0, 't'},		/* case 6 */
		{0, 0, 0, 0}
	};

	/* Turn off error check of getopt */
	opterr = 0;

	/* Loop on options */
	while (1) {

		c = getopt_long (argc, argv, "dhf:m:p:s:t:?", long_options, &option_index);
		if (c == -1) break;

		switch (c) {
			case 0:		/* Long options */
				switch (option_index) {
					case 0:		/* debug */
						debug = 1;
						break;
					case 1:		/* help */
						show_usage(argv[0]);
						exit(0);
					case 2:		/* Format */
						if ( strlen(optarg) ) sscanf(optarg, "%d", &format);
						break;
					case 3:		/* Master */
						if ( strlen(optarg) ) sscanf(optarg, "%lx", &master);
						break;
					case 4:	/* Data Pattern */
						if ( strlen(optarg) ) {
							if ( optarg[1] == 'x' )
								sscanf(&optarg[2], "%lx", &pattern);
							else
								sscanf(optarg, "%lx", &pattern);
						}
						break;
					case 5:		/* Slave */
						if ( strlen(optarg) ) sscanf(optarg, "%lx", &slave);
						break;
					case 6:		/* Step */
						if ( strlen(optarg) ) sscanf(optarg, "%d", &step);
						break;
				}
				break;
			case 'd':
				debug = 1;
				break;
			case 'f':		/* Format */
				if ( strlen(optarg) ) sscanf(optarg, "%d", &format);
				break;
			case 'm':		/* Master */
				if ( strlen(optarg) ) sscanf(optarg, "%lx", &master);
				break;
			case 'p':		/* Pattern */
				if ( strlen(optarg) ) {
					if ( optarg[1] == 'x' )
						sscanf(&optarg[2], "%lx", &pattern);
					else
						sscanf(optarg, "%lx", &pattern);
				}
				break;
			case 's':		/* Slave */
				if ( strlen(optarg) ) sscanf(optarg, "%lx", &slave);
				break;
			case 't':		/* Step */
				if ( strlen(optarg) ) sscanf(optarg, "%d", &step);
				break;
			case 'h':
			case '?':
			default:
				show_usage(argv[0]);
				exit(0);
		}
	}
}

/************************************************************************
	Function:
		logmsg - print log messages to both screen and file

	RETURNS:
		NONE
 ************************************************************************/
void logmsg(char *name, char *fmt, ...) {
	char msg[256];
	va_list ap;
	int size;
	time_t tt;
	FILE *file;

	/* add name */
	size = sprintf(msg, "%9s", name);
	if ( size > 11 ) size = 11;
	msg[size] = ' ';
	msg[size+1] = '(';
	size += 2;

	/* add time */
	time(&tt);
	size += strftime(&msg[size], 80, "%Y-%m-%d %H:%M:%S", localtime(&tt));
	size += sprintf(&msg[size], ")> ");

	/* get message */
	va_start(ap, fmt);
	size += vsnprintf(&msg[size], 256 - size, fmt, ap);
	va_end(ap);

	/* Display on screen */
	printf("%s", msg);

	/* check file name */
	if ( !strlen(filename) ) {
		size = sprintf(filename, "/home/ams/aal/log/amsw_test_");
		size += strftime(&filename[size], 80, "%Y%m%d_%H", localtime(&tt));
		filename[size] = '\0';
	}

	/* open file */
	if ( !(file = fopen(filename, "a")) ) return;

	/* Save status */
	fprintf(file, "%s", msg);

	/* close file */
	fclose(file);

	return;
}

/************************************************************************
	Function:
		init_tasks - Initialize tasks

	RETURNS:
		none
 ************************************************************************/
void init_tasks(void) {
	int i;

	/* Init tasks */
	for ( i = 0; i < 32; i++ ) {
		if ( slave & (1 << i) ) tasks[i].mode = 1;
		else if ( master & (1 << i) ) tasks[i].mode = 2;
		else tasks[i].mode = 0;

		tasks[i].timeout = AMSW_REPLY_TIMEOUT;					/* initial timeout value */
		tasks[i].retry   = 0;									/* number of retries */

		tasks[i].tx_active = 0;									/* no TX yet */
		tasks[i].rx_active = (tasks[i].mode == 1 ? 1 : 0);		/* Wait for request */

		if ( tasks[i].mode ) tasks[i].data = (u_char *) amsw_malloc(0, 8192);
		else                 tasks[i].data = NULL;

		tasks[i].loops    = 0;
		tasks[i].errs     = 0;
		tasks[i].timeouts = 0;
	}
}

/************************************************************************
	Function:
		tasks_status - Display task's status

	RETURNS:
		none
 ************************************************************************/
void tasks_status(void) {
	int i;

	for ( i = 0; i < 32; i++ ) {
		if ( tasks[i].mode == 0 ) continue;
		printf("AMSW_TEST> %s %02d: TX = %d, RX = %d, Timeout = %d, loops = %ld, errs = %ld, last size = %d (%x)\n",
				(tasks[i].mode == 1 ? "Slave Link " : "Master Link"), i, tasks[i].tx_active, tasks[i].rx_active,
				tasks[i].timeout, tasks[i].loops, tasks[i].errs, tasks[i].size, tasks[i].size);
	}
}

/************************************************************************
	Function:
		signal_handler - signal handler routine

	RETURNS:
		none
 ************************************************************************/
void signal_handler(int signum) {
	exit(0);
}

/************************************************************************
	Function:
		pre_exit - stuff to be executed before exit

	RETURNS:
		none
 ************************************************************************/
void pre_exit(void) {
	/* Display the current status */
	tasks_status();

	logmsg("AMSW_TEST", "program ended\n");

    /* Close all AMSW-PCI device */
	amsw_close(-1);

	/* close message queue */
	proc_close_cmd(msgid);

	return;
}

/************************************************************************
	Function:
		main - Main routine for test

	RETURNS:
		0 on success
 ************************************************************************/
int main(int argc, char *argv[]) {
	int link;
	register AMSWTask *task;
	int ret;
	int rx_bc;
	int dtype;
	int reply;
	u_char *d;
	int csize;
	u_char cmd[4096];

	filename[0] = '\0';			/* create new file */

//	printf("%s in AAL Package %s (Compiled @ " __DATE__ " " __TIME__ ")\n",
//    		VERS, AAL_PACKAGE_VERSION);

	logmsg("AMSW_TEST", "program started\n");

	/* Process options */
	parse_options(argc, argv);

	/* initialize tasks */
	init_tasks();

	/* open message queue */
	msgid = proc_open_cmd(AMSW_MSGQUE_CMD);

	/* Register pre-exit routine */
	atexit(pre_exit);

	/* Register CTRL-C handler */
	if ( signal(SIGINT, signal_handler) < 0 ) {
		logmsg("AMSW_TEST", "Fail to register CTRL-C handler, Error: %s\n", strerror(errno));
	}

	/* start test */
	for ( link = 0; ; link = (++link % 32) ) {
		task = &tasks[link];

		/* Check for no action */
		if ( task->mode == 0 ) continue;

		/* Check for timeout */
		if ( task->mode == 2 && task->rx_active && task->timeout <= 0 ) {
			if ( task->retry < 4 ) {
				task->retry++;
				task->timeout = AMSW_REPLY_TIMEOUT;
				task->rx_active = 0;
			}
			continue;
		}

		/* Start a new action */
		if ( task->mode == 2 && !task->tx_active && !task->rx_active ) {
			/* Command data */
		  //task->cmd =1;
		  //task->data="0x2e01";
		  //logmsg("AMSW-TEST","FHTEST");
		  if ( task->cmd ) {
				task->size = prod_amsw_command(task->data, cmd + 9, task->cmd);
				task->cmd  = 0;
			}

			/* Request status */
			else if ( task->loops && (task->loops % 4000) == (link % 4) * 1000 ) {
				task->size = prod_amsw_req_stat(task->data);
			}

			/* Request error logs */
			else if ( task->loops && (task->loops % 4000) == (link % 4) * 1000 + 300 ) {
				task->size = prod_amsw_req_elog(task->data);
			}

			/* Request kernel logs */
			else if ( task->loops && (task->loops % 4000) == (link % 4) * 1000 + 600 ) {
				task->size = prod_amsw_req_klog(task->data);
			}

			/* Prepare echo data */
			else {
				if ( format != GenTypeFIXED ) pattern = task->loops & 0xff;
				task->size = prod_amsw_echo_data(task->data, format, pattern);
			}

			/* Start TX */
			task->tx_active = 1;
			task->loops++;
		}

		/* Check if TX is active */
		if ( task->tx_active ) {
			/* check TX status */
			if ( amsw_tx_ready(link) ) {
				/* Send data out */
				ret = amsw_tx_packet(link, task->data, task->size, 0);
				if ( ret < 0 ) {
					if ( debug ) {
						logmsg("AMSW_TEST", "Fail to send TX data to link %d, Error: %s\n", link, strerror(errno));
					}
					task->errs++;

					if ( task->mode == 2 ) continue;
				}
				else if ( debug ) {		/* clear data for receive */
					int ii;
					for ( ii = 0; ii < 8192; ii++ ) task->data[ii] = 0;
				}

				/* End of task */
				task->tx_active = 0;
				task->rx_active = 1;
				task->timeout   = AMSW_REPLY_TIMEOUT;

				/* Display info */
				if ( task->mode == 1 && (task->loops % step) == 0 )
					logmsg("AMSW_TEST", "Link %02d: Slave      loops: %-12ld errs: %ld\n", link, task->loops, task->errs);
			}
		}

		/* Check if RX is active */
		if ( task->rx_active ) {
			/* Check receive status */
			if ( (task->size = amsw_rx_ready(link)) >= 0 ) {
				if ( task->mode == 1 ) task->loops++;
				else if ( task->mode == 2 ) task->retry = 0;

				/* Get RX data */
				ret = amsw_rx_packet(link, task->data, task->size, &rx_bc);

				if ( ret < 0 ) {
					if ( debug ) {
						logmsg("AMSW_TEST", "Fail to get RX data from link %d, Error: %s\n", link, strerror(errno));
					}
					task->errs++;

					if ( task->mode == 1 ) continue;
				}
				else {
					/* Check data */
					if ( ret > 0 ) {
						if ( debug > 0 )
							logmsg("AMSW_TEST", "RX %s from driver\n", (ret > 1 ? "error" : "warning"));
						task->errs++;
						if ( debug && ret > 1 ) break;
					}
					/* nothing to reply when error happens */
					else if ( task->mode == 1 && ret > 1 ) continue;

					/* nothing to do for zero length packet */
					if ( task->size ) {
						/* Decode header */
						d = task->data;
						reply = d[1] >> 7;
						dtype = d[1] & 0x3f;

						if ( task->size > 4 ) {
							if ( !prod_crc_check(task->data, task->size) ) {
								if ( debug > 0 )
									logmsg("AMSW_TEST", "Received data from link %d has CRC error\n", link);
								task->errs++;
							}
						}

						/* Try to send back */
						if ( task->mode == 1 ) {
							task->tx_active = 1;

							/* set reply */
							prod_amsw_set_reply(task->data, task->size);
						}
						/* status data */
						else if ( reply && dtype == DataTypeStat )
							prod_amsw_save_stat(task->data, task->size);
						/* error logs data */
						else if ( reply && dtype == DataTypeLog )
							prod_save_elog(task->data + 2, task->size - 2);
						/* kernel logs data */
						else if ( reply && dtype == DataTypeMesg )
							prod_save_klog(task->data + 2, task->size - 2);
					}
				}

				/* finish RX */
				task->rx_active = 0;

				/* Display info */
				if ( task->mode == 2 && (task->loops % step == 0) )
					logmsg("AMSW_TEST", "Link %02d: Master     loops: %-12ld errs: %ld\n", link, task->loops, task->errs);
			}
			else if ( task->mode == 2 && task->timeout > 0 ) {
				task->timeout--;
				if ( task->timeout == 0 && task->retry == 4 )
					logmsg("AMSW_TEST", "Link %02d: timeout\n", link);
			}
		}

		/* Check command */
		csize = proc_get_cmd(msgid, cmd);
		if ( csize ) {
			CMD_MSGBUF *msg = (CMD_MSGBUF *) cmd;
			int ll;
			int ss;

			ss = msg->size - 1;
			ll = cmd[8];
			tasks[ll].cmd = ss;
		}
	}

    return 0;
}
