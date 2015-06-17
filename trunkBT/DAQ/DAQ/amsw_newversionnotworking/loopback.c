/* -*-C-*-
 * loopback.c --  test program for loopback test over AMSWire links
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
/* Defines                                                              */
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

static int nloops;
static int data_mode;
static int data_type;
static u_long chan_recv;
static u_long chan_xmit;
static u_long data_def;
static int flag_xmit;
static int flag_recv;
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
/* Function:															*/
/*																*/
/* set_defaults - routine to set default parameters.					*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
void set_defaults(void) {

	nloops = 1;				/* Not in loop mode */
	data_mode = 0;			/* 0 - single, 1 - block */
	data_type = 2;			/* 2 - sequence numbers */
	chan_recv = 1;			/* Default channel mask for receivers, channel 0 on card 0 */
	chan_xmit = 1;			/* Default channel mask for transmitters, channel 0 on card 0 */
	data_def = 0x12345678;	/* Default data */
	flag_recv = 0;			/* no receive */
	flag_xmit = 0;			/* no transmit */
	flag_verify = 0;		/* no verify */
	quiet = 0;				/* Display messages */
	dma_mode = -1;			/* No operation (use driver default) */
	irq_mode = -1;			/* No operation (use driver default) */
	size = 128;				/* default size for block mode */
	unit = 32;				/* number of bits per unit (default is 32) */
	step = 1000;			/* display steps */
	flag_rand = 0;			/* no address and size randomize */
	start = 0;				/* start address of source buffer */
}

/************************************************************************/
/* Function:															*/
/*																*/
/* parse_input - routine to get input arguments.						*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
void parse_arguments(char *arg) {
	int i;

	/* Change to lowercase */
	for ( i = 0; i < strlen(arg); i++ ) arg[i] = tolower(arg[i]);

	/* Check - or / */
	if ( arg[0] != '-' && arg[0] != '/' ) arg[1] = 'h';

	/* Check argument */
	switch (arg[1]) {
		case 'b':
			if ( strlen(&arg[2]) ) sscanf(&arg[2], "%d", &size);
			if ( size == 0 || size == 3 ) flag_rand = 3;	/* Enable randomize mode, in dword size */
			else if ( size == 2 ) {							/* Enable randomize mode, in word size */
				flag_rand = 2;
				unit = 16;
			}
			else if ( size == 1 ) {							/* Enable randomize mode, in byte size */
				flag_rand = 1;
				unit = 8;
			}
			else {
				flag_rand = 0;
				switch ( size - (size / 4 * 4) ) {
					case 1:
					case 3:
						unit = 8;
						break;
					case 2:
						unit = 16;
						break;
					default:
						unit = 32;
						break;
				}
			}
			data_mode = 1;									/* Block mode */
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
		case 'n':
			if ( arg[2] == 'd' ) dma_mode = 0;
			else if ( arg[2] == 'i' ) {
				if ( !strlen(&arg[3]) || arg[3] == 'a'  ) irq_mode = 0x30000;
				else if ( arg[3] == 'd' ) irq_mode = 0x10000;
				else if ( arg[3] == 'r' ) irq_mode = 0x20000;
			}
			else if ( arg[2] == 'r' ) {
				flag_recv = 0;
				flag_xmit = 1;
			}
			else if ( arg[2] == 't' ) {
				flag_xmit = 0;
				flag_recv = 1;
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
		case 'r':
			flag_recv = 1;
			if ( strlen(&arg[2]) ) sscanf(&arg[2], "%lx", &chan_recv);
			break;
		case 's':
			if ( strlen(&arg[2]) ) sscanf(&arg[2], "%lx", &data_def);
			data_mode = 0;
			break;
		case 't':
			flag_xmit = 1;
			if ( strlen(&arg[2]) ) sscanf(&arg[2], "%lx", &chan_xmit);
			break;
		case 'u':
			if ( strlen(&arg[2]) ) {
				sscanf(&arg[2], "%d", &unit);
				if ( unit != 8 && unit != 16 ) unit = 32;
			}
			break;
		case 'v':
			flag_verify = 1;
			break;
		default:
			printf("\nUsage: loopback [options]\n\n");
			printf("Options:    -b[n]    : block mode, n = block size (in bytes) [def:128]\n");
			printf("                       n = 0 (random in dword), 1 (random in byte) and 2 (random in word)\n");
			printf("            -d       : enable DMA [def: no change]\n");
			printf("            -f[s][x] : s is data format [def:2], x is data [def:0x12345678]\n");
			printf("            -i[s]    : enable INT [def: no change], s = all, dma, rx [def: all]\n");
			printf("            -l[n]    : number of loops [def:10000]\n");
			printf("            -nd      : disable DMA [def: no change]\n");
			printf("            -ni[s]   : disbale INT [def: no change], s = all, dma, rx [def: all]\n");
			printf("            -p[n]    : display steps, n = number of loops [def:1000]\n");
			printf("            -q[n]    : quiet mode, bits: 0 - source, 1 - result, 2 - step [def:3]\n");
			printf("            -r[x]    : receive, x = channel mask [def:0x00000001]\n");
			printf("            -s[x]    : single mode [default], x = data [def:0x12345678]\n");
			printf("            -t[x]    : transmit [default], x = channel mask [def:0x00000001]\n");
			printf("            -u[n]    : unit bits, n = 8, 16 or 32 [def:32]\n");
			printf("            -v       : verify [def: no verify]\n");
			printf("            -h -?    : help\n\n");
			printf("Channel Mask  : bit 31------------------------------0\n");
			printf("                Chan 76543210765432107654321076543210\n");
			printf("                Card | 3     | 2     | 1     | 0     \n\n");
			printf("Format:     0 : shifted 0\n");
			printf("            1 : shifted 1\n");
			printf("            2 : sequence numbers\n");
			printf("            3 : random numbers\n");
			printf("            4 : alternative 0x0000ffff and 0xffff0000\n");
			printf("            5 : alternative 0x5555aaaa and 0xaaaa5555\n");
			printf("            x : fixed data\n\n");
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
	for ( card = 0; card < 4; card ++ ) {
		/* Find card */
		// if ( !(chan_xmit & (0xff << card)) && !(chan_recv & (0xff << card)) ) continue;
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
	int loop;
	int i, j;
	int len;
	int nerrs;
	int card;
	int chan;
	u_char *ps_byte, *pr_byte;
	u_short *ps_word, *pr_word;
	struct timeval ts1, ts2;
	double wc;
	double rate;
	struct amsw_block block;
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
	if ( !flag_recv && !flag_xmit ) flag_xmit = 1;
	if ( !flag_recv ) flag_verify = 0;

	for ( card = 0; card < 4; card++ ) {
		/* Find card */
		if ( !(chan_xmit & (0xff << (card * 8))) && !(chan_recv & (0xff << (card * 8))) ) continue;

		/* Open device */
		printf("Opening card %d, channel mask = %08lx, %08lx\n", card, chan_xmit, chan_recv);
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

	/* Init word counter and rates */
	gettimeofday(&ts1, NULL);
	wc = rate = 0.0;

	/* Start loop */
	for ( loop = 0, nerrs = 0; loop < nloops; loop++ ) {
		/* get random size and address */
		if ( flag_rand ) {
			size = rand() & 8185;
			if ( flag_rand == 2 ) {
				size &= 0xfffffffe;
				if ( !size ) size = 2;
			}
			else if ( flag_rand == 3 ) {
				size &= 0xfffffffc;
				if ( !size ) size = 4;
			}
			else if ( !size ) size = 1;
			start = (rand() % (16384 - (size & 0xfffffffc))) >> 2;
		}

		/* Add word count */
		wc += (double) (data_mode ? size : 4);

		/* Transmit data */
		if ( flag_xmit ) {
			for ( i = 0; i < 32; i++ ) {
				if ( !(chan_xmit & (1 << i)) ) continue;
				card = i / 8;
				chan = i % 8;

				if ( data_mode ) {
					block.header = (u_char *) &start;
					block.h_size = TEST_HEADER_SIZE;
					block.data   = (u_char *) &source[start];
					block.d_size = size;
					if ( amsw_transmit_block(card, chan, &block) < 0 ) {
						if ( nloops > 1 ) printf("Loop %d: ", loop);
						printf("Fail to transmit block data to card %d channel %d, size = %d\n", card, chan, size);
						break;
					}
					else if ( nloops == 1 )
						printf("  Xmit : card %d channel %d, size = %d\n", card, chan, size);
				}
				else {
					block.h_size = 0;
					block.data   = (u_char *) &data_def;
					block.d_size = 4;
					if ( amsw_transmit_block(card, chan, &block) < 0 ) {
						if ( nloops > 1 ) printf("Loop %d: ", loop);
						printf("Fail to transmit data, card %d channel %d, data = 0x%08lx\n", card, chan, data_def);
						break;
					}
					else if ( nloops == 1 )
						printf("  Xmit : card %d channel %d, data = 0x%08lx\n", card, chan, data_def);
				}
			}
		}

		/* Receive data */
		if ( flag_recv ) {
			for ( j = 0; j < 32; j++ ) {
				if ( !(chan_recv & (1 << j)) ) continue;
				card = j / 8;
				chan = j % 8;

				if ( data_mode ) {
					block.d_size = size;
					block.data   = (u_char *) result;
					if ( amsw_receive_block(card, chan, &block) < 0 ) {
						if ( nloops > 1 ) printf("Loop %d: ", loop);
						printf("Fail to receive data, card %d channel %d, size = %d\n", card, chan, size);
						break;
					}
					else if ( nloops == 1 ) {
						printf("  Recv : card %d channel %d, size = %d\n", card, chan, size);
					}
					size = block.d_size - 4;
				}
				else {
					block.h_size = 0;
					block.d_size = 4;
					block.data   = (u_char *) result;
					if ( amsw_receive_block(card, chan, &block) < 0 ) {
						if ( nloops > 1 ) printf("Loop %d: ", loop);
						printf("Fail to receive data, card %d channel %d, data = 0x%08lx\n", card, chan, result[0]);
						break;
					}
					else if ( nloops == 1 )
						printf("  Recv : card %d channel %d, data = 0x%08lx\n", card, chan, result[0]);
				}

				/* Verify data */
				if ( flag_verify ) {
					if ( data_mode ) {
						switch ( unit ) {
							case 8:
								ps_byte = (u_char *) &source[start];
								pr_byte = (u_char *) &result[1];
								for ( i = 0; i < size; i++ ) {
									if ( ps_byte[i] != pr_byte[i] ) {
										if ( nerrs++ < 10 ) {
											if ( nloops > 1 ) printf("%10d loops, ", loop);
											printf("data error, write = 0x%02x, read = 0x%02x, start = 0x%x, size = %d\n", 
													ps_byte[i], pr_byte[i], (int) ps_byte, size);
										}
									}
								}
								break;
							case 16:
								ps_word = (u_short *) &source[start];
								pr_word = (u_short *) &result[1];
								len = (size + 1) / 2;
								for ( i = 0; i < len; i++ ) {
									if ( ps_word[i] != pr_word[i] ) {
										if ( nerrs++ < 10 ) {
											if ( nloops > 1 ) printf("%10d loops, ", loop);
											printf("data error, write = 0x%04x, read = 0x%04x, start = 0x%x, size = %d\n", 
													ps_word[i], pr_word[i], (int) ps_word, size);
										}
									}
								}
								break;
							case 32:
							default:
								len = (size + 3) / 4;
								for ( i = 0; i < len; i++ ) {
									if ( source[start + i] != result[i + 1] ) {
										if ( nerrs++ < 10 ) {
											if ( nloops > 1 ) printf("%10d loops, ", loop);
											printf("data error, write = 0x%08lx, read = 0x%08lx, start = 0x%x, size = %d\n", 
													source[start + i], result[i + 1], (int) &source[start], size);
										}
									}
								}
						}

						if ( nloops == 1) {
							if ( nerrs ) printf("Verify : %d error found\n", nerrs);
							else printf("Verify : no error\n");
						}
					}
					else {
						if ( data_def != result[0] ) {
							if ( nerrs++ < 10 ) {
								if ( nloops > 1 ) printf("%10d loops, ", loop);
								printf("Verify : data error, write = 0x%08lx, read = 0x%08lx\n", data_def, result[0]);
							}
						}
						else if ( nloops == 1 )
							printf("Verify : no error\n");
					}
				}
			}
		}

		/* Display information for loops */
		if ( nloops > 1 && ((!(quiet & 4) && ((loop + 1) % step == 0)) || (loop == nloops - 1)) ) {
			/* Loop info */
			printf("%10d loops : ", loop + 1);

			/* Get current time */
			gettimeofday(&ts2, NULL);

			/* Total Time */
			rate = (double) ((ts2.tv_sec - ts1.tv_sec) * 1000000 + ts2.tv_usec - ts1.tv_usec);
			printf("time spent %.0f usec, ", rate);

			/* event rate */
			printf("event rate %.2f kHz ", step / rate * 1000.0);

			/* Total rate */
			rate = wc / rate * 1000000 / 1024 / 1024;
			printf("transfer rate %.2f Mbyte/sec", rate);

			/* Verify info */
			if ( flag_verify ) {
				if ( nerrs ) printf(", %d errors", nerrs);
				else printf(", no error");
			}

			/* Clear word count and rates */
			printf("\n");
			wc = rate = 0.0;
			gettimeofday(&ts1, NULL);
		}

		/* display block data only at the end of the last loop */
		if ( loop == nloops - 1 && flag_recv && data_mode ) {
			if ( !(quiet & 1) ) {
				printf("Source Data: \n");
				switch ( unit ) {
					case 8:
						pr_byte = (u_char *) source;
						for ( i = 0; i < size; i++ ) {
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
						pr_word = (u_short *) source;
						len = (size + 1) / 2;
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
						len = (size + 3) / 4;
						for ( i = 0; i < len; i++ ) {
							if ( i % 8 == 0 ) printf("\t");
							printf("%08lx ", source[i]);
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
						for ( i = 0; i < size + 4; i++ ) {
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
						len = (size + 5) / 2;
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
						len = (size + 7) / 4;
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

#ifdef USE_KERNEL_BUFFER
	/* Free memories */
	amsw_free(0, source);
	amsw_free(0, result);
#endif	/* USE_KERNEL_BUFFER */

	/* Close AMSWire device */
	for ( card = 0; card < 4; card++ ) {
		/* Find card */
		if ( !(chan_xmit & (0xff << card)) && !(chan_recv & (0xff << card)) ) continue;

		/* close device */
		amsw_close(card);
	}

	return 0;
}
