/* -*-C-*-
 * amsw_mem.c - AMSW-PCI dual-port memory test program
 *-----------------------------------------------------------------------
 *
 * Project          : Linux
 * Moduletyp        : C-Code
 * Author           : Xudong CAI
 * Created          : 23 May 2002
 *
 *-----------------------------------------------------------------------
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
#include <sys/mman.h>
#include <getopt.h>

#include "amsw_lib.h"
#include "prod_lib.h"

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

static int format = 2;			/* Default data format 2 = sequence data */
static u_long pattern = 0;		/* Default data pattern */
static int size_rd = -1;		/* Size for read, -1 means loop test */
static int size_wr = -1;		/* Size for write, -1 means loop test */
static u_long offset = -1;		/* PCI offset, -1 means random */
static int step = 10000;		/* Display steps */
static int use_mmap = 0;		/* flag to use mmap mode, default no */
static int use_kbuf = 1;		/* flag to use kernel buffer, default yes */
static int card = 0;			/* Card number */
static int debug = 0;

/************************************************************************
   Function:

   show_usage - routine to show usage

   RETURNS:
 ************************************************************************/
void show_usage(char *title) {
    fprintf(stderr, "\nUsage: %s [options]\n\n", title);
    fprintf(stderr, "Options:    --debug     (-d) : debug flag [def: 0]\n");
    fprintf(stderr, "            --address=x (-a) : PCI address [def: 0]\n");
    fprintf(stderr, "            --card=n    (-c) : Use card n [def: 0]\n");
    fprintf(stderr, "            --format=n  (-f) : Test data format [def: sequence]\n");
    fprintf(stderr, "                                0 - Shifted 0\n");
    fprintf(stderr, "                                1 - Shifted 1\n");
    fprintf(stderr, "                                2 - Sequence data\n");
    fprintf(stderr, "                                3 - Random data\n");
    fprintf(stderr, "                                4 - Alternative 0 and f\n");
    fprintf(stderr, "                                5 - Alternative 5 and a\n");
    fprintf(stderr, "                               10 - Shifted 0\n");
    fprintf(stderr, "            --kbuf      (-k) : Use kernel buffer [def: yes]\n");
    fprintf(stderr, "            --mmap      (-m) : Use MMAP mode [def: no]\n");
    fprintf(stderr, "            --pattern=x (-p) : Pattern for fixed data format, [def: 0x12345678]\n");
    fprintf(stderr, "            --read=n    (-r) : read n bytes [def: 128]\n");
    fprintf(stderr, "            --step=n    (-s) : Display steps [def: 10000]\n");
    fprintf(stderr, "            --write=n   (-w) : write n bytes [def: 4096]\n");
    fprintf(stderr, "            --user      (-u) : Use user space buffer [def: no]\n\n");
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
		{"debug",   0, 0, 'd'},		/* case 0 */
		{"help",    0, 0, 'h'},		/* case 1 */
		{"address", 1, 0, 'a'},		/* case 2 */
		{"card",    1, 0, 'c'},		/* case 3 */
		{"format",  1, 0, 'f'},		/* case 4 */
		{"kbuf",    0, 0, 'k'},		/* case 5 */
		{"mmap",    0, 0, 'm'},		/* case 6 */
		{"pattern", 1, 0, 'p'},		/* case 7 */
		{"read",    1, 0, 'r'},		/* case 8 */
		{"step",    1, 0, 's'},		/* case 9 */
		{"write",   1, 0, 'w'},		/* case 10 */
		{"user",    0, 0, 'u'},		/* case 11 */
		{0, 0, 0, 0}
	};

	/* Turn off error check of getopt */
	opterr = 0;

	/* Loop on options */
	while (1) {

		c = getopt_long (argc, argv, "dha:c:f:kmp:r:s:w:u?", long_options, &option_index);
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
					case 2:		/* Address */
						if ( strlen(optarg) ) sscanf(optarg, "%lx", &offset);
						else offset = 0;
						break;
					case 3:		/* Card */
						if ( strlen(optarg) ) sscanf(optarg, "%d", &card);
						break;
					case 4:		/* Format */
						if ( strlen(optarg) ) sscanf(optarg, "%d", &format);
						break;
					case 5:		/* kbuf */
						use_kbuf = 1;
						break;
					case 6:		/* mmap */
						use_mmap = 1;
						break;
					case 7:	/* Data Pattern */
						if ( strlen(optarg) ) {
							if ( optarg[1] == 'x' )
								sscanf(&optarg[2], "%lx", &pattern);
							else
								sscanf(optarg, "%lx", &pattern);
						}
						break;
					case 8:		/* Read */
						if ( strlen(optarg) ) sscanf(optarg, "%d", &size_rd);
						else size_rd = 128;
						break;
					case 9:		/* Step */
						if ( strlen(optarg) ) sscanf(optarg, "%d", &step);
						break;
					case 10:		/* Write */
						if ( strlen(optarg) ) sscanf(optarg, "%d", &size_wr);
						else size_rd = 4096;
						break;
					case 11:		/* user */
						use_kbuf = 0;
						break;
				}
				break;
			case 'd':
				debug = 1;
				break;
			case 'a':		/* Address */
				if ( strlen(optarg) ) sscanf(optarg, "%lx", &offset);
				else offset = 0;
				break;
			case 'c':		/* Card */
				if ( strlen(optarg) ) sscanf(optarg, "%d", &card);
				break;
			case 'f':		/* Format */
				if ( strlen(optarg) ) sscanf(optarg, "%d", &format);
				break;
			case 'k':		/* kbuf */
				use_kbuf = 1;
				break;
			case 'm':		/* mmap */
				use_mmap = 1;
				break;
			case 'p':		/* Pattern */
				if ( strlen(optarg) ) {
					if ( optarg[1] == 'x' )
						sscanf(&optarg[2], "%lx", &pattern);
					else
						sscanf(optarg, "%lx", &pattern);
				}
				break;
			case 'r':		/* Read */
				if ( strlen(optarg) ) sscanf(optarg, "%d", &size_rd);
				else size_rd = 128;
				break;
			case 's':		/* Step */
				if ( strlen(optarg) ) sscanf(optarg, "%d", &step);
				break;
			case 'w':		/* Write */
				if ( strlen(optarg) ) sscanf(optarg, "%d", &size_wr);
				else size_wr = 128;
				break;
			case 'u':		/* user */
				use_kbuf = 0;
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

   single_read_write - Single memory read / write

   RETURNS:
     0 on success
 ************************************************************************/
void single_read_write(u_long *source, u_long *result) {
    int i;
	u_long *amsw_mem = NULL;

	/* map memory */
	if ( use_mmap ) {
		amsw_mem = (u_long *) amsw_mmap_bar0(0);
		if ( (int) amsw_mem <= 0 ) {
			perror("AMSW_MEM> Fail to map BAR0 address ");
			exit(1);
		}
	}

	/* Check offset */
	if ( (int) offset < 0 ) offset = 0;
	printf("AMSW_MEM> single ");
	if ( size_wr > 0 ) printf("write ");
	if ( size_rd > 0 ) printf("read");
	printf("\n");

	/* Write */
    if ( size_wr > 0 ) {
        /* Write data */
		if ( use_mmap )
			memcpy((void *) &amsw_mem[offset/4], (void *) source, size_wr);
		else {
			if ( amsw_mem_write(card, offset, (void *) source, size_wr) < 0 ) {
				perror("AMSW_MEM> Fail to write AMSW-PCI memory ");
				exit(1);
			}
		}

        printf("AMSW_MEM> write %d bytes of data to memory @ 0x%08lx\n", size_wr, offset);
    }

    /* Read */
    if ( size_rd > 0 ) {
        /* Read data */
		if ( use_mmap )
			memcpy((void *) result, (void *) &amsw_mem[offset/4], size_rd);
		else {
			if ( amsw_mem_read(card, offset, (void *) result, size_rd) < 0 ) {
				perror("AMSW_MEM> Fail to read AMSW-PCI memory ");
				exit(1);
			}
		}

        printf("AMSW_MEM> read %d bytes of data from memory @ 0x%08lx\n", size_rd, offset);

		/* Display */
        printf("Result Data: \n");
        for ( i = 0; i < size_rd / 4; i++ ) {
            if ( i % 8 == 0 ) printf("\t");
            printf("%08lx ", result[i]);
            if ( i % 8 == 7 || i == (size_rd - 1)) printf("\n");
            if ( i % 256 == 255 && i < (size_rd - 1) ) {
                printf("Hit <Enter> to continue ... ");
                getc(stdin);
            }
        }
        printf("\n");
    }

	if ( use_mmap ) amsw_munmap_bar0(card, amsw_mem);

	return;
}

/************************************************************************
   Function:

   loop_test - Loop test memory

   RETURNS:
     0 on success
 ************************************************************************/
void loop_test(u_long *source, u_long *result) {
    int loop;
    int i;
    int len;
    int nerrs;
    struct timeval t1, t2, ts1, ts2;
    double wc;
    double rate, wrate, rrate, vrate;
    int flag_rand = 1;
    int start;
	u_long *amsw_mem = NULL;

	/* map memory */
	if ( use_mmap ) {
		amsw_mem = (u_long *) amsw_mmap_bar0(0);
		if ( (int) amsw_mem <= 0 ) {
			perror("AMSW_MEM> Fail to map BAR0 address ");
			exit(1);
		}
	}

	/* Init word counter and rates */
    gettimeofday(&ts1, NULL);
    wc = rate = wrate = rrate = vrate = 0.0;
    if ( (int) offset >= 0 ) flag_rand = 0;

    /* Start loop */
    for ( loop = 0, nerrs = 0; ; loop++ ) {
        /* Prepare for write */
        len = rand() % 4096;
        len &= 0xfffffffc;
        if ( !len ) len = 4;
        start = (rand() % (16384 - len)) >> 2;
        if ( flag_rand ) offset = (rand() % (256 * 1024 - len)) & 0xfffffffc;

        /* Add word count */
        wc += (double) len;

        /* Write data */
        gettimeofday(&t1, NULL);
		if ( use_mmap )
			memcpy((void *) &amsw_mem[offset/4], (void *) &source[start], len);
		else {
			if ( amsw_mem_write(card, offset, (void *) &source[start], len) < 0 ) {
				perror("AMSW_MEM> Fail to write AMSW-PCI memory ");
				exit(1);
			}
		}
		gettimeofday(&t2, NULL);
        wrate += (double) ((t2.tv_sec - t1.tv_sec) * 1000000 + t2.tv_usec - t1.tv_usec);

        /* Read data */
        gettimeofday(&t1, NULL);
		if ( use_mmap )
			memcpy((void *) result, (void *) &amsw_mem[offset/4], len);
		else {
        	if ( amsw_mem_read(card, offset, (void *) result, len) < 0 ) {
            	perror("AMSW_MEM> Fail to read AMSW-PCI memory ");
            	exit(1);
        	}
		}
        gettimeofday(&t2, NULL);
        rrate += (double) ((t2.tv_sec - t1.tv_sec) * 1000000 + t2.tv_usec - t1.tv_usec);

        /* Verify data */
        gettimeofday(&t1, NULL);
        for ( i = 0; i < len / 4; i++ ) {
            if ( source[start + i] != result[i] ) {
                if ( nerrs++ < 10 ) {
                    printf("AMSW_MEM> %10d loops, data error: write = 0x%08lx, read = 0x%08lx, offset = 0x%08x\n",
                            loop, source[start + i], result[i], i * 4);
                }
            }
        }
        gettimeofday(&t2, NULL);
        vrate += (double) ((t2.tv_sec - t1.tv_sec) * 1000000 + t2.tv_usec - t1.tv_usec);

        /* Display information for loops */
        if ( (loop + 1) % step == 0 ) {
            /* Loop info */
            printf("AMSW_MEM> %10d loops : ", loop + 1);

            /* Write info */
            wrate = wc / wrate * 1000000 / 1024 / 1024;
            printf("write %6.2f Mbyte/sec, ", wrate);

            /* Read info */
            rrate = wc / rrate * 1000000 / 1024 / 1024;
            printf("read %6.2f Mbyte/sec, ", rrate);

            /* Verify info */
            vrate = wc / vrate * 1000000 / 1024 / 1024;
            printf("verify %6.2f Mbyte/sec, ", vrate);
            if ( nerrs ) printf("%d errors, ", nerrs);
            else printf("no error, ");

            /* Total rate */
            gettimeofday(&ts2, NULL);
            rate = wc / ((double) ((ts2.tv_sec - ts1.tv_sec) * 1000000 + ts2.tv_usec - ts1.tv_usec)) * 1000000 / 1024 / 1024;
            printf("total %.2f Mbyte/sec\n", rate);

            /* Clear word count and rates */
            wc = rate = wrate = rrate = vrate = 0.0;
            gettimeofday(&ts1, NULL);
        }
    }

	if ( use_mmap ) amsw_munmap_bar0(card, amsw_mem);

	return;
}

/************************************************************************
   Function:

   main - Main routine for test

   RETURNS:
     0 on success
 ************************************************************************/
int main(int argc, char *argv[]) {
	u_long src[4096];
    u_long res[2048];
    u_long *source;
    u_long *result;

	/* Process options */
	parse_options(argc, argv);

    /* Open AMSW-PCI device */
    if ( amsw_open(card) < 0 ) {
        perror("Fail to open AMSW-PCI device ");
        return 1;
    }

    /* allocate memories */
	if ( use_kbuf ) {
		source = (u_long *) amsw_malloc(card, 16384);
    	if ( !source ) {
        	perror("Fail to allocate source memory ");
        	return 1;
    	}

    	result = (u_long *) amsw_malloc(card, 8192);
		if ( !result ) {
        	perror("Fail to allocate result memory ");
        	return 1;
    	}
	}
	else {
		source = src;
		result = res;
	}

    /* Fill source data */
    prod_data((u_char *) source, 16384, format, pattern);

    /* Check read & write flag for single time */
    if ( size_rd > 0 || size_wr > 0 ) single_read_write(source, result);
    else loop_test(source, result);

    /* Free memories */
	if ( use_kbuf ) {
    	amsw_free(card, result);
    	amsw_free(card, source);
	}

    /* Close AMSWire device */
    amsw_close(card);

    return 0;
}
