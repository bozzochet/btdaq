/* -*-C-*-
 * slave.c --  test program to act as a master over AMSWire devices
 *
 *-----------------------------------------------------------------------
 *
 * Project          : Linux
 * Moduletyp        : C-Code
 * Author           : Xudong CAI
 * Created          : 05 Dec. 2001
 *
 *-----------------------------------------------------------------------
 *
 */

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include <ctype.h>
#include <sys/mman.h>
#include <signal.h>

#include "amswire.h"
#include "amsw_func.h"
#include "util_func.h"
#include "amsw_protocol.h"

/************************************************************************/
/* Defines	                            							*/
/************************************************************************/

#define TEST_HEADER_SIZE	4

/************************************************************************/
/* Typedefs / Structs                                                   */
/************************************************************************/

/************************************************************************/
/* Globals                                                              */
/************************************************************************/

/************************************************************************/
/* Forward declarations                                                 */
/************************************************************************/

/************************************************************************/
/* Locals                                                               */
/************************************************************************/

static int data_type;
static u_long data_def;
static int flag_verify;
static int quiet;
static int dma_mode;
static int irq_mode;
static int size;
static int step;
static int flag_rand;
static int start;

/************************************************************************/
/* Function:                                                            */
/*                                                                      */
/* set_defaults - routine to set default parameters.                    */
/*                                                                      */
/* RETURNS:                                                             */
/* 	0 on success                                                    */
/************************************************************************/
void set_defaults(void) {

	data_type = 2;			/* 2 - sequence numbers */
	data_def = 0x12345678;	/* Default data */
	flag_verify = 0;		/* no verify */
	quiet = 0;				/* Display messages */
	dma_mode = -1;			/* No operation (use driver default) */
	irq_mode = -1;			/* No operation (use driver default) */
	size = 32;				/* default size for block mode */
	step = 1000;			/* display steps */
	flag_rand = 1;			/* no address and size randomize */
	start = 0;				/* start address of source buffer */
}

/************************************************************************/
/* Function:                                                            */
/*                                                                      */
/* parse_input - routine to get input arguments.                        */
/*                                                                      */
/* RETURNS:                                                             */
/* 	0 on success                                                    */
/************************************************************************/
void parse_arguments(char *arg) {
	int i;

	/* Change to lowercase */
	for ( i = 0; i < strlen(arg); i++ ) arg[i] = tolower(arg[i]);

	/* Check - or / */
	if ( arg[0] != '-' && arg[0] != '/' ) arg[1] = 'h';

	/* Check argument */
	switch (arg[1]) {
		case 'd':
			dma_mode = 1;
			break;
		case 'f':
			if ( arg[2] >= '0' && arg[2] <= '5' ) data_type = arg[2] - '0';
			else if ( arg[2] == 'x' ) {
				data_type = 6;
				if ( strlen(&arg[3]) ) sscanf(&arg[3], "%lx", &data_def);
			}
			break;
		case 'i':
			if ( !strlen(&arg[2]) || arg[2] == 'a' ) irq_mode = 3;
			else if ( arg[2] == 'd' ) irq_mode = 1;
			else if ( arg[2] == 'r' ) irq_mode = 2;
			break;
		case 'n':
			if ( arg[2] == 'd' ) dma_mode = 0;
			else if ( arg[2] == 'i' ) {
				if ( !strlen(&arg[3]) || arg[3] == 'a'  ) irq_mode = 0x30000;
				else if ( arg[3] == 'd' ) irq_mode = 0x10000;
				else if ( arg[3] == 'r' ) irq_mode = 0x20000;
			}
			break;
		case 'p':
			if ( strlen(&arg[2]) ) sscanf(&arg[2], "%d", &step);
			else step = 1000;
			break;
		case 'q':
			quiet = 1;
			break;
		case 's':
			if ( strlen(&arg[2]) ) {
				sscanf(&arg[2], "%d", &size);
				flag_rand = 0;
			}
			else size = 0;
			if ( size == 0 ) flag_rand =1;		/* Enable randomize mode, in dword size */
			break;
		default:
			printf("\nUsage: slave [options]\n\n");
			printf("Options:    -f[s][x] : s is data format [def:2]\n");
			printf("                       s = 0 : shifted 0\n");
			printf("                       s = 1 : shifted 1\n");
			printf("                       s = 2 : sequence numbers\n");
			printf("                       s = 3 : random numbers\n");
			printf("                       s = 4 : alternative 0x0000ffff and 0xffff0000\n");
			printf("                       s = 5 : alternative 0x5555aaaa and 0xaaaa5555\n");
			printf("                       s = x : fixed data, x is data [def:0x12345678]\n\n");
			//printf("            -d       : enable DMA [def: no change]\n");
			//printf("            -i[s]    : enable INT [def: no change], s = all, dma, rx [def: all]\n");
			//printf("            -nd      : disable DMA [def: no change]\n");
			//printf("            -ni[s]   : disbale INT [def: no change], s = all, dma, rx [def: all]\n");
			printf("            -p[n]    : display steps, n = number of loops [def:1000]\n");
			printf("            -q       : quiet mode\n");
			printf("            -s[d]    : size in bytes, d = 0 (random size) [def:0]\n");
			printf("            -h -?    : help\n");
			exit(0);
	}
}

/************************************************************************/
/* Function:                                                            */
/*                                                                      */
/* handle_int - Handle interrupts                                       */
/*                                                                      */
/* RETURNS:                                                             */
/*      No return value                                                 */
/************************************************************************/
void handle_int(int signo) {
	int card;
	/* Close AMSWire device */
	for ( card = 0; card < 4; card++ ) {
		/* Find card */
		// if ( !(chan_mask & (0xff << (card * 8))) ) continue;
		/* close device */
		amsw_close(card);
	}
	printf("\nEXIT (Signal No. %d)\n", signo);
	exit(0);
}

/************************************************************************/
/* Function:                                                            */
/*                                                                      */
/* main - Main routine for test                                         */
/*                                                                      */
/* RETURNS:                                                             */
/* 	0 on success                                                    */
/************************************************************************/
int main(int args, char *argv[]) {
	int ich;
	int card;
	int chan;
	u_long chan_mask;
	struct amsw_block block;
	struct amsw_header *header;
	int chan_count[32];
	int chan_errs[32];
	int chan_mode[32];
	double chan_wc[32];
	double rate;
	struct timeval now;
	struct timeval chan_time[32];
//#define USE_KERNEL_BUFFER
#ifndef USE_KERNEL_BUFFER
	u_long source[4096];
	u_long result[2048];
#endif	/* USE_KERNEL_BUFFER */

#ifdef USE_KERNEL_BUFFER
	u_long *source;
	u_long *result;
#endif	/* USE_KERNEL_BUFFER */

	/* Set default parameters value */
	set_defaults();

	/* Set interrupt handling function */
	signal(SIGINT, handle_int);
	signal(SIGQUIT, handle_int);

	/* Parse input arguments */
	while ( --args  ) parse_arguments(argv[args]);
	if ( size > 8188 ) size = 8188;

	/* Initialize card */
	for ( card = 0, chan_mask = 0; card < 4; card++ ) {
		/* Open device */
		if ( amsw_open(card) < 0 ) continue;
		printf("card %d is opened\n", card);
		chan_mask |= (0xff << (card * 8));

		/* check DMA mode */
		if ( dma_mode == 0 ) amsw_command(card, AMSW_CMD_DIS_DMA, 0);
		else if ( dma_mode == 1 ) amsw_command(card, AMSW_CMD_ENA_DMA, 0);

		/* Check IRQ mode */
		switch ( irq_mode ) {
			case 1:
				amsw_command(card, AMSW_CMD_EIRQ_DMA, 0);
				break;
			case 2:
				amsw_command(card, AMSW_CMD_EIRQ_RX, 0);
				break;
			case 3:
				amsw_command(card, AMSW_CMD_EIRQ_ALL, 0);
				break;
			case 0x10000:
				amsw_command(card, AMSW_CMD_DIRQ_DMA, 0);
				break;
			case 0x20000:
				amsw_command(card, AMSW_CMD_DIRQ_RX, 0);
				break;
			case 0x30000:
				amsw_command(card, AMSW_CMD_DIRQ_ALL, 0);
				break;
		}
	}
	if ( !chan_mask ) {
		printf("No AMSWire cards are opened\n");
		return -1;
	}

#ifdef USE_KERNEL_BUFFER
	/* allocate memories */
	if ( !(source = (u_long *) amsw_malloc(0, 16384)) ) {
		printf("Fail to allocate source memory\n");
		return 1;
	}

	if ( !(result = (u_long *) amsw_malloc(0, 8192)) ) {
		printf("Fail to allocate result memory\n");
		return 1;
	}
#endif	/* USE_KERNEL_BUFFER */

	/* Fill source data */
	fill_data(source, 16384, data_type, data_def);


	/* initialize parameters */
	for ( ich = 0; ich < 32; ich++ ) {
		chan_count[ich] = 0;
		chan_errs[ich] = 0;
		chan_mode[ich] = 1;
		chan_wc[ich] = 0.0;

		gettimeofday(&chan_time[ich], NULL);
	}
	rate = 0.0;
	header = (struct amsw_header *) result;

	/* Start loop */
	while (1) {
		/* Loop for each active channel */
		for ( ich = 0; ich < 32; ich++ ) {
			/* Check RX operation */
			if ( !(chan_mask & (1 << ich)) ) continue;
			card = ich / 8;
			chan = ich % 8;

			/* Check receiver status */
			if ( !amsw_rx_done(card, chan) ) continue;

			/* Receive data */
			block.data = (u_char *) result;
			if ( amsw_receive_block(card, chan, &block) < 0 ) {
				printf("Loop %d: Fail to receive data, card %d channel %d, size = %d\n", 
					chan_count[ich], card, chan, size);
				break;
			}

			/* Check working mode */
			if ( header->route == AMSW_ROUTE_TEST && header->bid == AMSW_BID_REQ ) {
				if ( !chan_mode[ich] ) {
					chan_mode[ich] = 1;
					printf("Card %d channel %d change to normal mode\n", card, chan);
				}
			}
			else {
				if ( chan_mode[ich] ) {
					chan_mode[ich] = 0;
					printf("%x\t%x\t%x\t%x\n",header->route,AMSW_ROUTE_TEST,header->bid,AMSW_BID_REQ);
					printf("Card %d channel %d change to loopback mode\n", card, chan);
				}
			}

			/* Prepare data for reply */
			if ( chan_mode[ich] == 0 )			/* Loopback working mode */
				block.h_size = 0;			/* Send back what received */
			else {						/* Normal working mode */
				/* Set Block ID to reply */
				header->bid = AMSW_BID_RPL;

				/* Check command */
				switch ( header->cmd ) {
					case AMSW_REQ_PING:
						/* Replace Block ID and command words only */
						header->bid = AMSW_BID_RPL;
						header->cmd = AMSW_RPL_PING;
						block.h_size = 0;
						break;
					case AMSW_REQ_EVENT:
					case AMSW_REQ_STATUS:
						/* Replace block ID amd command */
						header->bid = AMSW_BID_RPL;
						header->cmd = (header->cmd == AMSW_REQ_STATUS) ? AMSW_RPL_STATUS : AMSW_RPL_EVENT;
						/* get random size and address */
						if ( flag_rand ) {
							size = rand() % (header->cmd == AMSW_RPL_STATUS ? 128 : 8181);
							size &= 0xfffffffe;
							if ( !size ) size = 2;
							start = (rand() % (8192 - (size & 0xfffffffc))) >> 2;
						}

						header->size = (u_short) start;
						block.h_size = 8;
						block.header = (u_char *) header;
						block.d_size = size;
						block.data   = (u_char *) &source[start];
						break;
					default:
						/* Increase error count */
						if ( chan_errs[ich]++ < 10 ) {
							printf("%10d loop : Unknown command in card %d channel %d, command = %x\n", 
									chan_count[ich], card, chan, header->cmd);
						}

						/* Prepare reply "FAIL" */
						header->cmd = AMSW_RPL_FAIL;
						block.h_size = 6;
						block.d_size = 0;
						block.header = (u_char *) header;
				}
			}


			/* Start transfer */
			if ( amsw_transmit_block(card, chan, &block) < 0 ) {
				printf("%10d loop : Fail to transmit block data to card %d channel %d, size = %d\n", 
						chan_count[ich], card, chan, size);
				return 1;
			}

			/* Increase count */
			chan_count[ich]++;
			chan_wc[ich] += block.h_size + block.d_size;

			/* Display information for loops */
			if ( !quiet && ((chan_count[ich]) % step == 0) ) {
				/* Loop info */
				printf("%10d loops (%d @ %d) : ", chan_count[ich], chan, card);

				/* Total Time */
				gettimeofday(&now, NULL);
				rate = ((now.tv_sec - chan_time[ich].tv_sec) * 1000000 + now.tv_usec - chan_time[ich].tv_usec);
				printf("time spent %.0f usec, ", rate);

				/* event rate */
				printf("event rate %.2f kHz ", ((double) step) / rate * 1000.0);

				/* Total rate */
				rate = ((double) chan_wc[ich]) / rate * 1000000 / 1024 / 1024;
				printf("transfer rate %.2f Mbyte/sec", rate);

				/* Verify info */
				if ( chan_errs[ich] ) printf(", %d errors\n", chan_errs[ich]);
				else printf(", no error\n");

				/* Clear word count and rates */
				chan_wc[ich] = rate = 0.0;
				gettimeofday(&chan_time[ich], NULL);
			}
		}
	}

#ifdef USE_KERNEL_BUFFER
	/* Free memories */
	amsw_free(0, source);
	amsw_free(0, result);
#endif	/* USE_KERNEL_BUFFER */

	/* Close AMSWire device */
	for ( card = 0; card < 4; card++ ) {
		/* Find card */
		if ( !(chan_mask & (0xff << (card * 8))) ) continue;

		/* close device */
		amsw_close(card);
	}

	return 0;
}
