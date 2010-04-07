/* -*-C-*-
 * amsw_reg.c --  test program for JIM-HRDL/422 Register access
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
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/user.h>
#include <sys/time.h>
#include <errno.h>
#include <ctype.h>
#include <getopt.h>

#include "amsw_lib.h"

/************************************************************************
   Defines
 ************************************************************************/

/************************************************************************
   Typedefs / Structs
 ************************************************************************/

/************************************************************************
   Globals
 ************************************************************************/

/************************************************************************
   Forward declarations
 ************************************************************************/

/************************************************************************
   Locals
 ************************************************************************/

static int reg_rd = -2;
static int reg_wr = -1;
static u_long def_data = 0x12345678;
static int use_mmap = 0;
static int card = 0;

/************************************************************************
   Function:

   show_usage - routine to show usage

   RETURNS:
 ************************************************************************/
void show_usage(char *title) {
    fprintf(stderr, "\nUsage: %s [options]\n\n", title);
    fprintf(stderr, "Options:    --all       (-a) : Read all registers [default mode]\n");
    fprintf(stderr, "            --card=n    (-c) : Use card n [def: 0]\n");
    fprintf(stderr, "            --data=x    (-d) : Data value for write [def: 0x12345678]\n");
    fprintf(stderr, "            --mmap      (-m) : Use mmap mode [def: no]\n");
    fprintf(stderr, "            --read=n    (-r) : read register n or dn [def: all]\n");
    fprintf(stderr, "            --write=n   (-w) : write register n [def: 0]\n");
    fprintf(stderr, "            --help -h -?     : help\n");
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
		{"help",  0, 0, 'h'},		/* case 0 */
		{"all",   0, 0, 'a'},		/* case 1 */
		{"card",  1, 0, 'c'},		/* case 2 */
		{"data",  1, 0, 'd'},		/* case 3 */
		{"mmap",  0, 0, 'm'},		/* case 4 */
		{"read",  1, 0, 'r'},		/* case 5 */
		{"write", 1, 0, 'w'},		/* case 6 */
		{0, 0, 0, 0}
	};

	/* Turn off error check of getopt */
	opterr = 0;

	/* Loop on options */
	while (1) {

		c = getopt_long (argc, argv, "hac:d:mr:w:?", long_options, &option_index);
		if (c == -1) break;

		switch (c) {
			case 0:		/* Long options */
				switch (option_index) {
					case 0:		/* help */
						show_usage(argv[0]);
						exit(0);
					case 1:		/* All */
						reg_rd = -3;
						break;
					case 2:		/* Card */
						if ( strlen(optarg) ) sscanf(optarg, "%d", &card);
						break;
					case 3:	/* Data */
						if ( strlen(optarg) ) {
							if ( optarg[1] == 'x' )
								sscanf(&optarg[2], "%lx", &def_data);
							else
								sscanf(optarg, "%lx", &def_data);
						}
						break;
					case 4:		/* mmap */
						use_mmap = 1;
						break;
					case 5:		/* Read */
						if ( strlen(optarg) ) {
							if ( optarg[0] == 'd' ) {
								sscanf(&optarg[1], "%x", &reg_rd);
								reg_rd |= 0xdd000000;
							}
							else sscanf(optarg, "%d", &reg_rd);
						}
						break;
					case 6:		/* Write */
						if ( strlen(optarg) ) {
							if ( optarg[0] == 'd' ) {
								sscanf(&optarg[1], "%x", &reg_wr);
								reg_wr |= 0xdd000000;
							}
							else sscanf(optarg, "%d", &reg_wr);
							if ( reg_rd == -2 ) reg_rd = -1;
						}
						break;
				}
				break;
			case 'a':		/* All */
				reg_rd = -3;
				break;
			case 'c':		/* Card */
				if ( strlen(optarg) ) sscanf(optarg, "%d", &card);
				break;
			case 'd':		/* Data */
				if ( strlen(optarg) ) {
					if ( optarg[1] == 'x' )
						sscanf(&optarg[2], "%lx", &def_data);
					else
						sscanf(optarg, "%lx", &def_data);
				}
				break;
			case 'm':		/* mmap */
				use_mmap = 1;
				break;
			case 'r':		/* Read */
				if ( strlen(optarg) ) {
					if ( optarg[0] == 'd' ) {
						sscanf(&optarg[1], "%x", &reg_rd);
						reg_rd |= 0xdd000000;
					}
					else sscanf(optarg, "%d", &reg_rd);
				}
				break;
			case 'w':		/* Write */
				if ( strlen(optarg) ) {
					if ( optarg[0] == 'd' ) {
						sscanf(&optarg[1], "%x", &reg_wr);
						reg_wr |= 0xdd000000;
					}
					else sscanf(optarg, "%d", &reg_wr);
					if ( reg_rd == -2 ) reg_rd = -1;
				}
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

   main - Main routine for test

   RETURNS:
 	0 on success
 ************************************************************************/
int main(int argc, char *argv[]) {
	u_long *amswreg = NULL;
	u_long data;
	int i;

	/* Process options */
	parse_options(argc, argv);

	/* Open AMSW-PCI device */
	if ( amsw_open(card) < 0 ) {
		perror("Fail to open AMSW-PCI ");
		return 1;
	}

	/* Map BAR1 memory space */
	if ( use_mmap ) {
		amswreg = (u_long *) amsw_mmap_bar1(card);
		if ( ((int) amswreg) < 0 ) {
			perror("Fail to map BAR1 to user space ");
			return 1;
		}
	}

	/* Write configuration space */
	if ( (reg_wr & 0xff000000) == 0xdd000000 ) {
		amsw_dma_write(card, (reg_wr & 0xff), def_data);
	    printf("Write DMA[%02d] = 0x%08lx\n", (reg_wr & 0xff), def_data);
	}

	/* Write CSR register */
	else if ( reg_wr != -1 ) {
		if ( use_mmap )
			amswreg[reg_wr] = def_data;
		else
			amsw_reg_write(card, reg_wr, def_data);
	    printf("Write CSR[%02d] = 0x%08lx\n", reg_wr, def_data);
	}

	/* Read all registers */
	if ( reg_rd == -2 || reg_rd == -3 ) {
		/* Read CSR registers */
		for ( i = 0; i < 20; i++ ) {
			if ( use_mmap )
				data = amswreg[i];
			else
				amsw_reg_read(card, i, &data);
			printf("CSR[%02d] = 0x%08lx\n", i, data);
		}
		printf("\n");

		/* Read configuration space */
		for ( i = 0; i < 3; i++ ) {
			amsw_dma_read(card, i, &data);
			printf("DMA[%02d] = 0x%08lx\n", i, data);
		}
	}

	/* Read configuration space */
	else if ( (reg_rd & 0xff000000) == 0xdd000000 ) {
		amsw_dma_read(card, (reg_rd & 0xff), &data);
	    printf("Read  DMA[%02d] = 0x%08lx\n", (reg_rd & 0xff), data);
	}

	/* Read CSR register */
	else if ( reg_rd != -1 ) {
		if ( use_mmap )
			data = amswreg[reg_rd];
		else
			amsw_reg_read(card, reg_rd, &data);
	    printf("Read  CSR[%02d] = 0x%08lx\n", reg_rd, data);
	}

	/* Unmap BAR1 spaces */
	if ( use_mmap ) amsw_munmap_bar1(card, amswreg);

	/* Close AMSW-PCI device */
	amsw_close(card);

	return 0;
}
