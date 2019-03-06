/* -*-C-*-
 * cmd_test.c --  test program for AMSWire Device Driver Commands
 *
 *-----------------------------------------------------------------------
 *
 * Project          : Linux
 * Moduletyp        : C-Code
 * Author           : Xudong CAI
 * Created          : 31 Oct. 2001
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
static int cmd[8];
static u_long data[8];
static int ncmds;

/************************************************************************/
/* Function:															*/
/*																*/
/* set_defaults - routine to set default parameters.					*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
void set_defaults(void) {
	int i;

	card = 0;				/* The first card */
	ncmds = 0;				/* Set number of commands to zero */
	for ( i = 0; i < 8; i++ ) {
		cmd[i]  = -1;	/* Set no command */
		data[i] = 0;	/* Set no data */
	}
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
		case 'c':
			if ( strlen(&arg[2]) ) sscanf(&arg[2], "%d", &card);
			break;
		case 'b':
			if ( arg[2] == '0' ) cmd[ncmds++] = AMSW_CMD_BAR0;
			else if ( arg[2] == '1' ) cmd[ncmds++] = AMSW_CMD_BAR1;
			break;
		case 'd':
			cmd[ncmds++] = AMSW_CMD_ENA_DMA;
			break;
		case 'i':
			if ( !strlen(&arg[2]) ) cmd[ncmds++] = AMSW_CMD_EIRQ_ALL;
			else if ( arg[2] == 'a' ) cmd[ncmds++] = AMSW_CMD_EIRQ_ALL;
			else if ( arg[2] == 'd' ) cmd[ncmds++] = AMSW_CMD_EIRQ_DMA;
			else if ( arg[2] == 'r' ) cmd[ncmds++] = AMSW_CMD_EIRQ_RX;
			break;
		case 'a':
			if ( strlen(&arg[2]) ) sscanf(&arg[2], "%ld", &data[ncmds]);
			else data[ncmds] = 4096;
			cmd[ncmds++] = AMSW_CMD_MALLOC;
			break;
		case 'f':
			if ( strlen(&arg[2]) ) {
				sscanf(&arg[2], "%lx", &data[ncmds]);
				cmd[ncmds++] = AMSW_CMD_FREE;
			}
			break;
		case 'm':
			if ( strlen(&arg[2]) ) sscanf(&arg[2], "%lx", &data[ncmds]);
			else data[ncmds] = 0;
			cmd[ncmds++] = AMSW_CMD_SET_MASK;
		case 'n':
			if ( arg[2] == 'd' ) cmd[ncmds++] = AMSW_CMD_DIS_DMA;
			else if ( arg[2] == 'i' ) {
				if ( !strlen(&arg[3]) ) cmd[ncmds++] = AMSW_CMD_DIRQ_ALL;
				else if ( arg[3] == 'a' ) cmd[ncmds++] = AMSW_CMD_DIRQ_ALL;
				else if ( arg[3] == 'd' ) cmd[ncmds++] = AMSW_CMD_DIRQ_DMA;
				else if ( arg[3] == 'r' ) cmd[ncmds++] = AMSW_CMD_DIRQ_RX;
			}
			break;
		case 's':
			if ( arg[2] == '1' ) cmd[ncmds++] = AMSW_CMD_NEW_STAT;
			else cmd[ncmds++] = AMSW_CMD_CUR_STAT;
			break;
		case 'u':
			cmd[ncmds++] = AMSW_CMD_INUSE;
			break;
		default:
			printf("\nUsage: cmd_test {options}\n\n");
			printf("Options:    -b[n]  : BAR Address, n = 0, 1\n");
			printf("            -c{n}  : card number, n = 0, 1, 2, 3 [def:0]\n");
			printf("            -d     : enable DMA\n");
			printf("            -i[s]  : enable INT, s = all, dma, rx [def: all]\n");
			printf("            -nd    : disable DMA\n");
			printf("            -ni[s] : disbale INT, s = all, dma, rx [def: all]\n");
			printf("            -m{x}  : set INT mask, x = mask\n");
			printf("            -s[n]  : interrupt status, s = 0 (current), 1 (new) [def:0]\n");
			printf("            -a{d}  : allocate memory, d = size\n");
			printf("            -f{x}  : free memory, x = address\n");
			printf("            -u     : in use\n\n");
			printf("            -h -? --help    : help\n\n");
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
	int i;
	char def_argv[10] = "--help";

	/* Set default parameters value */
	set_defaults();

	/* Parse input arguments */
	while ( --args  ) parse_arguments(argv[args]);
	if ( cmd[0] == -1 ) parse_arguments(def_argv);

	/* Open AMSWire device */
	if ( amsw_open(card) <= 0 ) {
		printf("Fail to open AMSWire Device\n");
		return 1;
	}

	/* Loop */
	for ( i = 0; ; i++ ) {
		if ( cmd[i] == -1 ) break;
		else if ( cmd[i] < 20 && cmd[i] != AMSW_CMD_NEW_STAT ) {
			if ( amsw_command(card, cmd[i], data[i]) < 0 ) {
				printf("Fail to send command, cmd = %x, data = %lx\n", cmd[i], data[i]);
				break;
			}
		}
		else {
			if ( amsw_command(card, cmd[i], (u_long) &data[i]) < 0 ) {
				printf("Fail to send command, cmd = %x\n", cmd[i]);
				break;
			}
		}

		switch ( cmd[i] ) {
			case AMSW_CMD_ENA_DMA:
				printf("DMA is enabled\n");
				break;
			case AMSW_CMD_DIS_DMA:
				printf("DMA is disabled\n");
				break;
			case AMSW_CMD_EIRQ_ALL:
				printf("All interrupts are enabled\n");
				break;
			case AMSW_CMD_DIRQ_ALL:
				printf("All interrupts are disabled\n");
				break;
			case AMSW_CMD_EIRQ_DMA:
				printf("DMA done interrupt is enabled\n");
				break;
			case AMSW_CMD_DIRQ_DMA:
				printf("DMA done interrupt is disabled\n");
				break;
			case AMSW_CMD_EIRQ_RX:
				printf("RX done interrupts are enabled\n");
				break;
			case AMSW_CMD_DIRQ_RX:
				printf("RX done interrupts are disabled\n");
				break;
			case AMSW_CMD_SET_MASK:
				printf("Interrupt mask is set to %lx\n", data[i]);
				break;
			case AMSW_CMD_INUSE:
				if ( data[i] ) printf("Card No.%d is in use\n", card);
				else printf("Card No.%d is free\n", card);
				break;
			case AMSW_CMD_BAR0:
				printf("BAR0 address = %lx\n", data[i]);
				break;
			case AMSW_CMD_BAR1:
				printf("BAR1 address = %lx\n", data[i]);
				break;
			case AMSW_CMD_CUR_STAT:
				printf("Current interrupt status = %08lx\n", data[i]);
				break;
			case AMSW_CMD_NEW_STAT:
				printf("New interrupt status = %08lx\n", data[i]);
				break;
			case AMSW_CMD_MALLOC:
				printf("Returned address = %lx\n", data[i]);
				break;
			case AMSW_CMD_FREE:
				printf("Address %lx is freed\n", data[i]);
				break;
		}
	}

	/* Close AMSWire device */
	amsw_close(card);

	return 0;
}
