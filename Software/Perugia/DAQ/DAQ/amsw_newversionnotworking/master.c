/* -*-C-*-
 * master.c --  test program to act as a master over AMSWire devices
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

#define OPS_XMIT			1
#define OPS_RECV			2
#define OPS_MASK			3
#define OPS_NONE			0

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

static int nloops;
static int data_type;
static u_long chan_mask;
static u_long data_def;
static int work_mode;
static int flag_verify;
static int quiet;
static int dma_mode;
static int irq_mode;
static int size;
static int unit;
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

	nloops = 1;				/* Not in loop mode */
	data_type = 2;			/* 2 - sequence numbers */
	chan_mask = 1;			/* Default channel mask for receivers, channel 0 on card 0 */
	data_def = 0x12345678;	/* Default data */
	work_mode = 0;			/* Working mode */
	flag_verify = 0;		/* no verify */
	quiet = 0;				/* Display messages */
	dma_mode = -1;			/* No operation (use driver default) */
	irq_mode = -1;			/* No operation (use driver default) */
	size = 32;				/* default size for block mode */
	unit = 16;				/* number of bits per unit (default is 32) */
	step = 1000;			/* display steps */
	flag_rand = 0;			/* no address and size randomize */
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
		case 'c':
			if ( strlen(&arg[2]) ) sscanf(&arg[2], "%lx", &chan_mask);
			break;
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
		case 'l':
			if ( strlen(&arg[2]) ) sscanf(&arg[2], "%d", &nloops);
			else nloops = 10000;
			break;
		case 'm':
			if ( strlen(&arg[2]) ) sscanf(&arg[2], "%d", &work_mode);
			if ( work_mode > 3 || work_mode < 0 ) work_mode = 0;
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
			if ( strlen(&arg[2]) ) sscanf(&arg[2], "%d", &quiet);
			if ( quiet <= 0 || quiet >= 8 ) quiet = 3;
			break;
		case 's':
			if ( strlen(&arg[2]) ) sscanf(&arg[2], "%d", &size);
			if ( size == 0 || size == 3 ) {					/* Enable randomize mode, in dword size */
				flag_rand = 3;
				unit = 32;
			}
			else if ( size == 1 || size == 2 ) {				/* Enable randomize mode, in word size */
				flag_rand = 2;
				unit = 16;
			}
			else {
				switch ( size - (size / 4 * 4) ) {
					case 1:
					case 3:
						size = (size / 2) * 2;				/* Make sure size in word */
					case 2:
						unit = 16;
						break;
					default:
						unit = 32;
						break;
				}
			}
			break;
		case 'u':
			if ( strlen(&arg[2]) ) {
				sscanf(&arg[2], "%d", &unit);
				if ( unit != 32 ) unit = 16;
			}
			break;
		case 'v':
			flag_verify = 1;
			break;
		default:
			printf("\nUsage: master [options]\n\n");
			printf("Options:    -c[x]    : channel mask [def:0x00000001]\n");
			printf("                       bit 31------------------------------0\n");
			printf("                       Chan 76543210765432107654321076543210\n");
			printf("                       Card | 3     | 2     | 1     | 0     \n");
			//printf("            -d       : enable DMA [def: no change]\n");
			printf("            -f[s][x] : s is data format [def:2]\n");
			printf("                       s = 0 : shifted 0\n");
			printf("                       s = 1 : shifted 1\n");
			printf("                       s = 2 : sequence numbers\n");
			printf("                       s = 3 : random numbers\n");
			printf("                       s = 4 : alternative 0x0000ffff and 0xffff0000\n");
			printf("                       s = 5 : alternative 0x5555aaaa and 0xaaaa5555\n");
			printf("                       s = x : fixed data, x is data [def:0x12345678]\n\n");
			//printf("            -i[s]    : enable INT [def: no change], s = all, dma, rx [def: all]\n");
			printf("            -l[n]    : number of loops [def:10000]\n");
			printf("            -m[n]    : working mode, n = 0(echo), 1(ping), 2(event), 3(status), [def:0]\n");
			//printf("            -nd      : disable DMA [def: no change]\n");
			//printf("            -ni[s]   : disbale INT [def: no change], s = all, dma, rx [def: all]\n");
			printf("            -p[n]    : display steps, n = number of loops [def:1000]\n");
			printf("            -q[n]    : quiet mode, bits: 0 - source, 1 - result, 2 - step [def:3]\n");
			printf("            -s[d]    : size in bytes (echo and ping mode only) [def:32]\n");
			printf("                       n = 0 (random in dword), 1 (random in word)\n");
			printf("            -u[n]    : unit bits, n = 16 or 32 [def:16]\n");
			printf("            -v       : verify [def: no verify]\n");
			printf("            -h -?    : help\n");
			exit(0);
	}
}
/************************************************************************/
/* Function:                                                            */
/*                                                                      */
/* handle_int - Handle interrupts.                                      */
/*                                                                      */
/* RETURNS:                                                             */
/*      No return value                                                 */
/************************************************************************/
void handle_int(int signo) {
	int card;
	/* Close AMSWire device */
	for( card = 0; card < 4; card++ ) {
		/* Find card */
		// if ( !(chan_mask & (0xff << card)) ) continue;
		/* Close device */
		amsw_close(card);
	}
	printf("\nEXIT (Signal No. %d)\n", signo);
	exit(0);
}

/************************************************************************/
/* Function:                                                            */
/*                                                                      */
/* main - Main routine for test.                                        */
/*                                                                      */
/* RETURNS:                                                             */
/* 	0 on success                                                    */
/************************************************************************/
int main(int args, char *argv[]) {
	int i;
	int loop;
	int ich;
	int len;
	int card;
	int chan;
	u_char *ps_byte, *pr_byte;
	u_short *ps_word, *pr_word;
	u_long *pr_dword;
	u_long status;
	struct amsw_block block;
	struct amsw_header header;
	int ops;
	int chan_ops[32];
	int chan_count[32];
	int chan_errs[32];
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
	if ( work_mode ) {
		unit = 16;
		if ( flag_rand ) flag_rand = 2;
		if ( size > 8180 ) size = 8180;
	}
	else if ( size > 8188 ) size = 8188;

	/* Find channels */
	for ( chan = 0, card = -1; chan < 32; chan++ ) {
		/* Check channel mask */
		if ( !(chan_mask & (1 << chan)) ) {
			chan_ops[chan] = OPS_NONE;		/* No operations on not existing channels */
			continue;
		}

		/* Enable transmit operation for existing channel */
		chan_ops[chan] = OPS_XMIT;			/* Transmit operation */

		/* Initialize cards */
		if ( card != chan / 8 ) {
			card = chan / 8;

			/* Open device */
			printf("Opening card %d, channel mask = %08lx\n", card, chan_mask);
			if ( amsw_open(card) < 0 ) {
				printf("Can't open device driver for card %d\n", card);
				return 1;
			}

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
	for ( chan = 0; chan < 32; chan++ ) {
		chan_count[chan] = 0;
		chan_errs[chan] = 0;
		chan_wc[chan] = 0.0;
		gettimeofday(&chan_time[chan], NULL);
	}
	rate = 0.0;

	/* Start loop */
	loop = 0;
	while (1) {
		/* Transmit data for each active channel */
		for ( ich = 0; ich < 32; ich++ ) {
			/* Check TX operation */
			if ( !(chan_ops[ich] & OPS_XMIT) ) continue;
			card = ich / 8;
			chan = ich % 8;

			/* Check transmiter status */
			if ( !amsw_tx_ready(card, chan) ) {
				if ( ++loop > 1000 ) {
					amsw_command(card, AMSW_CMD_CUR_STAT, (u_long) &status);
					printf("Loop %d: Waiting for transmitter ready. current status = %08lx, ", chan_count[ich], status);
					amsw_command(card, AMSW_CMD_NEW_STAT, (u_long) &status);
					printf("new status = %08lx\n", status);
					return -1;
				}
				else continue;
			}
			loop = 0;

			/* get random size and address */
			if ( flag_rand ) {
				size = rand() % 8181;
				if ( flag_rand == 2 ) {
					size &= 0xfffffffe;
					if ( !size ) size = 2;
				}
				else if ( flag_rand == 3 ) {
					size &= 0xfffffffc;
					if ( !size ) size = 4;
				}
				else if ( !size ) size = 1;
				start = (rand() % (8192 - (size & 0xfffffffc))) >> 2;
			}

			/* Prepare header */
			switch ( work_mode ) {
				case 1:
					header.route = AMSW_ROUTE_TEST;
					header.bid   = AMSW_BID_REQ;
					header.cmd   = AMSW_REQ_PING;
					header.size  = (size >> 1) + 2;
					header.start = start;
					block.header = (u_char *) &header;
					block.h_size = sizeof(struct amsw_header);
					block.d_size = size;
					block.data   = (u_char *) &source[start];
					break;
				case 2:
					header.route = AMSW_ROUTE_TEST;
					header.bid   = AMSW_BID_REQ;
					header.cmd   = AMSW_REQ_EVENT;
					block.header = (u_char *) &header;
					block.h_size = 6;
					block.d_size = 0;
					block.data   = (u_char *) source;
					size = 6;
					break;
				case 3:
					header.route = AMSW_ROUTE_TEST;
					header.bid   = AMSW_BID_REQ;
					header.cmd   = AMSW_REQ_STATUS;
					block.header = (u_char *) &header;
					block.h_size = 6;
					block.d_size = 0;
					block.data   = (u_char *) source;
					size = 6;
					break;
				case 0:
				default:
					block.header = (u_char *) &start;
					block.h_size = TEST_HEADER_SIZE;
					block.d_size = size;
					block.data   = (u_char *) &source[start];
					break;
			}

			/* Start transfer */
			if ( amsw_transmit_block(card, chan, &block) < 0 ) {
				if ( nloops > 1 ) printf("Loop %d: ", chan_count[ich]);
				printf("Fail to transmit block data to card %d channel %d, size = %d\n", card, chan, size);
				return 1;
			}
			else if ( nloops == 1 )
				printf("  Xmit : card %d channel %d, size = %d\n", card, chan, size);

			/* Enable receive operation */
			chan_ops[ich] = OPS_RECV;
		}

		/* Receive data for each active channel */
		for ( ich = 0; ich < 32; ich++ ) {
			/* Check RX operation */
			if ( !(chan_ops[ich] & OPS_RECV) ) continue;
			card = ich / 8;
			chan = ich % 8;

			/* Check receiver status */
			if ( !amsw_rx_done(card, chan) ) {
				if ( ++loop > 10000 ) {
					amsw_command(card, AMSW_CMD_CUR_STAT, (u_long) &status);
					printf("Loop %d, Waiting for receiving done. current status = %08lx\n", chan_count[ich], status);
					amsw_command(card, AMSW_CMD_NEW_STAT, (u_long) &status);

					if ( ++chan_count[ich] == nloops ) chan_ops[ich] = OPS_NONE;
					else chan_ops[ich] = OPS_XMIT;
					break;
				}
				else continue;
			}
			loop = 0;

			/* Receive data */
			block.data = (u_char *) result;
			if ( amsw_receive_block(card, chan, &block) < 0 ) {
				if ( nloops > 1 ) printf("Loop %d: ", chan_count[ich]);
				printf("Fail to receive data, card %d channel %d, size = %d\n", card, chan, size);
				break;
			}
			else if ( nloops == 1 ) {
				printf("  Recv : card %d channel %d, size = %d\n", card, chan, block.d_size);
			}

			/* Check for no event */
			if ( work_mode == 2 && *((u_short *) (result + 4)) == AMSW_RPL_NOEVENT ) continue;

#define CHECK_HEADER_WORDS
			/* Check header */
			pr_word = (u_short *) result;
			switch ( work_mode ) {
				case 1:
#ifdef CHECK_HEADER_WORDS
					/* Check route */
					if ( pr_word[0] != AMSW_ROUTE_TEST ) {
						if ( chan_errs[ich]++ < 10 ) {
							if ( nloops > 1 ) printf("%10d loops, ", chan_count[ich]);
							printf("route error in card %d channel %d, route = %x\n", card, chan, pr_word[0]);
						}
					}

					/* Check Block ID */
					if ( pr_word[1] != AMSW_BID_RPL ) {
						if ( chan_errs[ich]++ < 10 ) {
							if ( nloops > 1 ) printf("%10d loops, ", chan_count[ich]);
							printf("block ID error in card %d channel %d, ID = %x\n", card, chan, pr_word[1]);
						}
					}

					/* Check command */
					if ( pr_word[2] != AMSW_RPL_PING ) {
						if ( chan_errs[ich]++ < 10 ) {
							if ( nloops > 1 ) printf("%10d loops, ", chan_count[ich]);
							printf("command error in card %d channel %d, command = %x\n", card, chan, pr_word[2]);
						}
					}
#endif	/* CHECK_HEADER_WORDS */

					block.header = (u_char *) &header;
					block.h_size = sizeof(struct amsw_header);
					block.d_size -= block.h_size;
					block.data   = (u_char *) &pr_word[block.h_size >> 1];
					start = *((u_long *) &pr_word[4]);
					break;
				case 2:
#ifdef CHECK_HEADER_WORDS
					/* Check route */
					if ( pr_word[0] != AMSW_ROUTE_TEST ) {
						if ( chan_errs[ich]++ < 10 ) {
							if ( nloops > 1 ) printf("%10d loops, ", chan_count[ich]);
							printf("route error in card %d channel %d, route = %x\n", card, chan, pr_word[0]);
						}
					}

					/* Check Block ID */
					if ( pr_word[1] != AMSW_BID_RPL ) {
						if ( chan_errs[ich]++ < 10 ) {
							if ( nloops > 1 ) printf("%10d loops, ", chan_count[ich]);
							printf("block ID error in card %d channel %d, ID = %x\n", card, chan, pr_word[1]);
						}
					}

					/* Check command */
					if ( pr_word[2] != AMSW_RPL_EVENT ) {
						if ( chan_errs[ich]++ < 10 ) {
							if ( nloops > 1 ) printf("%10d loops, ", chan_count[ich]);
							printf("command error in card %d channel %d, command = %x\n", card, chan, pr_word[2]);
						}
					}
#endif	/* CHECK_HEADER_WORDS */

					block.header = (u_char *) &header;
					block.h_size = 8;
					block.d_size -= block.h_size;
					block.data   = (u_char *) &pr_word[block.h_size >> 1];
					start = (u_long) pr_word[3];
					break;
				case 3:
#ifdef CHECK_HEADER_WORDS
					/* Check route */
					if ( pr_word[0] != AMSW_ROUTE_TEST ) {
						if ( chan_errs[ich]++ < 10 ) {
							if ( nloops > 1 ) printf("%10d loops, ", chan_count[ich]);
							printf("route error in card %d channel %d, route = %x\n", card, chan, pr_word[0]);
						}
					}

					/* Check Block ID */
					if ( pr_word[1] != AMSW_BID_RPL ) {
						if ( chan_errs[ich]++ < 10 ) {
							if ( nloops > 1 ) printf("%10d loops, ", chan_count[ich]);
							printf("block ID error in card %d channel %d, ID = %x\n", card, chan, pr_word[1]);
						}
					}

					/* Check command */
					if ( pr_word[2] != AMSW_RPL_STATUS ) {
						if ( chan_errs[ich]++ < 10 ) {
							if ( nloops > 1 ) printf("%10d loops, ", chan_count[ich]);
							printf("command error in card %d channel %d, command = %x\n", card, chan, pr_word[2]);
						}
					}
#endif	/* CHECK_HEADER_WORDS */

					block.header = (u_char *) &header;
					block.h_size = 8;
					block.d_size -= block.h_size;
					block.data   = (u_char *) &pr_word[block.h_size >> 1];
					start = (u_long) pr_word[3];
					break;
				case 0:
				default:
					start = result[0];
					block.h_size = 4;
					block.d_size -= 4;
					block.data   = (u_char *) &result[1];
					break;
			}

			/* Check data */
			if ( flag_verify ) {
				switch ( unit ) {
					case 8:
						ps_byte = (u_char *) &source[start];
						pr_byte = block.data;
						for ( i = 0; i < block.d_size; i++ ) {
							if ( ps_byte[i] != pr_byte[i] ) {
								if ( chan_errs[ich]++ < 10 ) {
									if ( nloops > 1 ) printf("%10d loops, ", chan_count[ich]);
									printf("data error in card %d channel %d, write = 0x%02x, read = 0x%02x, index = %d, size = %d\n", 
											card, chan, ps_byte[i], pr_byte[i], i, size);
								}
							}
						}
						break;
					case 16:
						ps_word = (u_short *) &source[start];
						pr_word = (u_short *) block.data;
						len = (block.d_size + 1) / 2;
						for ( i = 0; i < len; i++ ) {
							if ( ps_word[i] != pr_word[i] ) {
								if ( chan_errs[ich]++ < 10 ) {
									if ( nloops > 1 ) printf("%10d loops, ", chan_count[ich]);
									printf("data error in card %d channel %d, write = 0x%04x, read = 0x%04x, index = %d, size = %d\n", 
											card, chan, ps_word[i], pr_word[i], i, size);
								}
							}
						}
						break;
					case 32:
					default:
						pr_dword = (u_long *) block.data;
						len = (block.d_size + 3) / 4;
						for ( i = 0; i < len; i++ ) {
							if ( source[start + i] != pr_dword[i] ) {
								if ( chan_errs[ich]++ < 10 ) {
									if ( nloops > 1 ) printf("%10d loops, ", chan_count[ich]);
									printf("data error in card %d channel %d, write = 0x%08lx, read = 0x%08lx, index = %d, size = %d\n", 
											card, chan, source[start + i], pr_dword[i], i, size);
								}
							}
						}
				}

				/* Display result */
				if ( nloops == 1) {
					if ( chan_errs[ich] ) printf("Verify : %d error found\n", chan_errs[ich]);
					else printf("Verify : no error\n");
				}
			}

			/* Enable transmit operation */
			if ( ++chan_count[ich] == nloops ) chan_ops[ich] = OPS_NONE;
			else chan_ops[ich] = OPS_XMIT;
			chan_wc[ich] += (block.h_size + block.d_size);

			/* Display information for loops */
			if ( nloops > 1 && ((!(quiet & 4) && ((chan_count[ich]) % step == 0)) || (chan_count[ich] == nloops)) ) {
				/* Loop info */
				printf("%10d loops (%d @ %d): ", chan_count[ich], chan, card);

				/* Get current time */
				gettimeofday(&now, NULL);

				/* Total Time */
				rate = (double) ((now.tv_sec - chan_time[ich].tv_sec) * 1000000 + now.tv_usec - chan_time[ich].tv_usec);
				printf("time spent %.0f usec, ", rate);

				/* event rate */
				printf("event rate %.2f kHz ", step / rate * 1000.0);

				/* Total rate */
				rate = chan_wc[ich] / rate * 1000000 / 1024 / 1024;
				printf("transfer rate %.2f Mbyte/sec", rate);

				/* Verify info */
				if ( flag_verify ) {
					if ( chan_errs[ich] ) printf(", %d errors", chan_errs[ich]);
					else printf(", no error");
				}

				/* Clear word count and rates */
				printf("\n");
				chan_wc[ich] = rate = 0.0;
				gettimeofday(&chan_time[ich], NULL);
			}

			/* display block data only at the end of the last loop */
			if ( chan_count[ich] == nloops ) {
				if ( !(quiet & 1) ) {
					printf("Source Data: \n");
					switch ( unit ) {
						case 8:
							pr_byte = (u_char *) &source[start];
							for ( i = 0; i < block.d_size; i++ ) {
								if ( i % 32 == 0 ) printf("\t");
								printf("%02x ", pr_byte[i]);
								if ( i % 32 == 31 || i == (size - 1) ) printf("\n");
								if ( i % 1024 == 1023 && i < (size - 1) ) {
									printf("Hit <Enter> to continue ... ");
									getc(stdin);
								}
							}
							break;
						case 16:
							pr_word = (u_short *) &source[start];
							len = (block.d_size + 1) / 2;
							for ( i = 0; i < len; i++ ) {
								if ( i % 16 == 0 ) printf("\t");
								printf("%04x ", pr_word[i]);
								if ( i % 16 == 15 || i == (len - 1) ) printf("\n");
								if ( i % 512 == 511 && i < (len - 1) ) {
									printf("Hit <Enter> to continue ... ");
									getc(stdin);
								}
							}
							break;
						case 32:
						default:
							len = (block.d_size + 3) / 4;
							for ( i = 0; i < len; i++ ) {
								if ( i % 8 == 0 ) printf("\t");
								printf("%08lx ", source[start + i]);
								if ( i % 8 == 7 || i == (len - 1)) printf("\n");
								if ( i % 256 == 255 && i < (len - 1) ) {
									printf("Hit <Enter> to continue ... ");
									getc(stdin);
								}
							}
					}
				}

				if ( !(quiet & 2) ) {
					printf("Result Data: \n");
					switch ( unit ) {
						case 8:
							pr_byte = (u_char *) result;
							for ( i = 0; i < block.h_size + block.d_size; i++ ) {
								if ( i % 32 == 0 ) printf("\t");
								printf("%02x ", pr_byte[i]);
								if ( i % 32 == 31 || i == (size - 1) ) printf("\n");
								if ( i % 1024 == 1023 && i < (size - 1) ) {
									printf("Hit <Enter> to continue ... ");
									getc(stdin);
								}
							}
							break;
						case 16:
							pr_word = (u_short *) result;
							len = (block.h_size + block.d_size + 1) / 2;
							for ( i = 0; i < len; i++ ) {
								if ( i % 16 == 0 ) printf("\t");
								printf("%04x ", pr_word[i]);
								if ( i % 16 == 15 || i == (len - 1) ) printf("\n");
								if ( i % 512 == 511 && i < (len - 1) ) {
									printf("Hit <Enter> to continue ... ");
									getc(stdin);
								}
							}
							break;
						case 32:
						default:
							len = (block.h_size + block.d_size + 3) / 4;
							for ( i = 0; i < len; i++ ) {
								if ( i % 8 == 0 ) printf("\t");
								printf("%08lx ", result[i]);
								if ( i % 8 == 7 || i == (len - 1)) printf("\n");
								if ( i % 256 == 255 && i < (len - 1) ) {
									printf("Hit <Enter> to continue ... ");
									getc(stdin);
								}
							}
					}
				}
			}
		}

		/* Check end of loop */
		for ( ich = 0, ops = 0; ich < 32; ich++ ) ops += chan_ops[ich];
		if ( !ops ) break;
	}

#ifdef USE_KERNEL_BUFFER
	/* Free memories */
	amsw_free(0, source);
	amsw_free(0, result);
#endif	/* USE_KERNEL_BUFFER */

	/* Close AMSWire device */
	for ( card = 0; card < 4; card++ ) {
		/* Find card */
		if ( !(chan_mask & (0xff << card)) ) continue;

		/* close device */
		amsw_close(card);
	}

	return 0;
}
