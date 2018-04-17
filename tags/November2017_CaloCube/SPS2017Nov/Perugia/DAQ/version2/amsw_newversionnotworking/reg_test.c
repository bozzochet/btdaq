/* -*-C-*-
 * reg_test.c --  test program for AMSWire Register access
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

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "amswire.h"
#include "amsw_func.h"

/************************************************************************/
/* Defines	                            							*/
/************************************************************************/

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

static int card;
static int nloops;
static int reg_rd;
static int reg_wr;
static int quiet;
static u_long def_data;

/************************************************************************/
/* Function:															*/
/*																*/
/* set_defaults - routine to set default parameters.					*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
void set_defaults(void) {
	card = 0;				/* The first card */
	nloops = 1;				/* No loops */
	reg_rd = -1;			/* Read all registers */
	reg_wr = -1;			/* No register to write */
	quiet = 0;				/* NOT in quiet mode */
	def_data = 0x12345678;	/* Default data */
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

	/* Check option */
	switch (arg[1]) {
		case 'c':
			if ( strlen(&arg[2]) ) sscanf(&arg[2], "%d", &card);
			break;
		case 'l':
			if ( strlen(&arg[2]) ) sscanf(&arg[2], "%d", &nloops);
			break;
		case 'r':
			if ( arg[2] == 'd' ) {
				if ( strlen(&arg[3]) ) sscanf(&arg[3], "%d", &reg_rd);
				else reg_rd = 0;
				reg_rd |= 0xD0000000;
			}
			else {
				if ( !strlen(&arg[2]) ) reg_rd = -2;
				else sscanf(&arg[2], "%d", &reg_rd);
			}
			break;
		case 'w':
			if ( arg[2] == 'd' ) {
				if ( strlen(&arg[3]) ) sscanf(&arg[3], "%d", &reg_wr);
				else reg_wr = 0;
				reg_wr |= 0xD0000000;
			}
			else {
				if ( strlen(&arg[2]) ) sscanf(&arg[2], "%d", &reg_wr);
			}
			break;
		case 'd':
			if ( strlen(&arg[2]) ) sscanf(&arg[2], "%lx", &def_data);
			break;
		case 'n':
			if ( arg[2] == 'w' ) reg_wr = -1;
			else if ( arg[2] == 'r' ) reg_rd = -1;
			break;
		case 'q':
			quiet = 1;
			break;
		case '-':
			if ( strcmp(arg, "--readonly") == 0 ) {
				reg_wr = -1;
				break;
			}
			else if ( strcmp(arg, "--writeonly") == 0 ) {
				reg_rd = -1;
				break;
			}
			else if ( strcmp(arg, "--quiet") == 0 ) {
				quiet = 1;
				break;
			}
		default:
			printf("\nUsage: reg_test [options]\n\n");
			printf("Options:    -c{n} : card number, n = 0, 1, 2, 3 [def:0]\n");
			printf("            -d{x} : data to be write [def:0x12345678]\n");
			printf("            -l{n} : loops, n = number of loops [def:0]\n");
			printf("            -r[x] : read register, x is address [def: all]\n");
			printf("            -w{x} : write register, x is address [def: no]\n\n");
			printf("            -h -? --help    : help\n");
			printf("            -q --quiet      : quiet mode\n");
			printf("            -nw --readonly  : read only\n");
			printf("            -nr --writeonly : write only\n\n");
			exit(0);
	}
}

/************************************************************************/
/* Function:															*/
/*																*/
/* main - Main routine for test							.				*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
int main(int args, char *argv[]) {
	u_long data;
	int loop;
	int err;
	int i;

	/* Set default parameters */
	set_defaults();

	/* Parse input arguments */
	while ( --args  ) parse_arguments(argv[args]);
	if ( reg_rd == -1 && reg_wr == -1 ) reg_rd = -2;

	/* Open AMSWire device */
	if ( amsw_open(card) <= 0 ) {
		printf("Fail to open AMSWire Device\n");
		return 1;
	}

	/* Loop */
	for ( loop = 0; loop < nloops; loop++ ) {
		/* Write DMA register */
		if ( (reg_wr & 0xfffffff0) == 0xd0000000 ) {
			if ( (err = amsw_write_dma_reg(card, reg_wr & 15, def_data)) < 0 ) {
				printf("Error in write register DMA[%02d], Error code = %d\n", reg_wr & 15, err);
				break; 
			}

			if ( nloops == 1 )
				printf("Write DMA[%02d] = 0x%08lx\n", reg_wr & 15, def_data);
			else if ( !quiet && ((loop + 1) % 100000 == 0) )
				printf("%10d loops, Write DMA[%02d] = 0x%08lx\n", loop + 1, reg_wr & 15, def_data);
			else if ( loop == nloops - 1 )
				printf("%10d loops, Write DMA[%02d] = 0x%08lx\n", nloops, reg_wr & 15, def_data);
		}

		/* Write CSR register */
		 else if ( reg_wr != -1 ) {
			if ( (err = amsw_write_register(card, reg_wr, def_data)) < 0 ) {
				printf("Error in write register CSR[%02d], Error code = %d\n", reg_wr, err);
				break; 
			}

			if ( nloops == 1 )
				printf("Write CSR[%02d] = 0x%08lx\n", reg_wr, def_data);
			else if ( !quiet && ((loop + 1) % 100000 == 0) )
				printf("%10d loops, Write CSR[%02d] = 0x%08lx\n", loop + 1, reg_wr, def_data);
			else if ( loop == nloops -1 )
				printf("%10d loops, Write CSR[%02d] = 0x%08lx\n", nloops, reg_wr, def_data);
		}

		/* Read all registers */
		if ( reg_rd == -2 ) {
			/* Read CSR registers */
			for ( i = 0; i < 19; i++ ) {
				if ( (err = amsw_read_register(card, i, &data)) < 0 ) {
					printf("Error in read register CSR[%02d], Error code = %d\n", i, err);
					break; 
				}

				printf("CSR[%02d] = 0x%08lx\n", i, data);
			}

			/* Read DMA registers */
			for ( i = 0; i < 3; i++ ) {
				if ( (err = amsw_read_dma_reg(card, i, &data)) < 0 ) {
					printf("Error in read register DMA[%02d], Error code = %d\n", i, err);
					break; 
				}

				printf("DMA[%02d] = 0x%08lx\n", i, data);
			}
		}

		/* Read DMA register */
		else if ( (reg_rd & 0xfffffff0) == 0xd0000000 ) {
			if ( (err = amsw_read_dma_reg(card, reg_rd & 15, &data)) < 0 ) {
				printf("Error in read register DMA[%02d], Error code = %d\n", reg_rd & 15, err);
				break; 
			}

			if ( nloops == 1 )
				printf("Read  DMA[%02d] = 0x%08lx\n", reg_rd & 15, data);
			else if ( !quiet && ((loop + 1) % 100000 == 0) )
				printf("%10d loops, Read  DMA[%02d] = 0x%08lx\n", loop + 1, reg_rd & 15, data);
			else if ( loop == nloops - 1)
				printf("%10d loops, Read  DMA[%02d] = 0x%08lx\n", nloops, reg_rd & 15, data);
		}

		/* Read CSR register */
		else if ( reg_rd != -1 ) {
			if ( (err = amsw_read_register(card, reg_rd, &data)) < 0 ) {
				printf("Error in read register CSR[%02d], Error code = %d\n", reg_rd, err);
				break; 
			}

			if ( nloops == 1 )
				printf("Read  CSR[%02d] = 0x%08lx\n", reg_rd, data);
			else if ( !quiet && ((loop + 1) % 100000 == 0) )
				printf("%10d loops, Read  CSR[%02d] = 0x%08lx\n", loop + 1, reg_rd, data);
			else if ( loop == nloops - 1 )
				printf("%10d loops, Read  CSR[%02d] = 0x%08lx\n", nloops, reg_rd, data);
		}
	}

	/* Close AMSWire device */
	amsw_close(card);

	return 0;
}
