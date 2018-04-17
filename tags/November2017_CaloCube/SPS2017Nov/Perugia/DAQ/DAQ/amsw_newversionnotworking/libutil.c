/* -*-C-*-
 * libamsw.c - AMSWire PCI Board (version 1) library
 *
 *-----------------------------------------------------------------------
 *
 * Project          : Linux
 * Moduletyp        : C-Code
 * Author           : Xudong CAI
 * Created          : 24 May 2001
 *
 *-----------------------------------------------------------------------
 */


/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "util_func.h"

/************************************************************************/
/* Defines	                            							*/
/************************************************************************/

/************************************************************************/
/* Typedefs / Structs                                                   */
/************************************************************************/

/************************************************************************/
/* Globals                                                              */
/************************************************************************/

static u_short crc16_table[65536];

/************************************************************************/
/* Forward declarations                                                 */
/************************************************************************/

/************************************************************************/
/* Locals                                                               */
/************************************************************************/

/************************************************************************/
/* Function:															*/
/*																*/
/* crc16_table_init - Initialize CRC16 look-up table					*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
void crc16_table_init() {
	u_short crc;
	u_short poly = 0x1021;		/* x16 + x12 + x5 + x0 */
	int i;
	int j;
	for (i = 0; i < 65536; i++) {
		crc = (u_short) i;
		for (j = 0; j < 16; j++) crc = (crc & 0x8000) ? ( (crc<<1) ^ poly ) : (crc<<1);
		crc16_table[i] = crc;
	}
}

/************************************************************************/
/* Function:															*/
/*																*/
/* crc16 - Calculate CRC16												*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
u_short crc16(u_char *data, size_t nbytes) {
	u_short crc;
	size_t n;
	u_short *p;

	crc = 0xffff;				/* initial crc value */
	n = nbytes >> 1;			/* Change word count from byte to word */
	p = (u_short *) data;		/* Change point from long word to word */

	while ( n-- ) {
		crc = crc16_table[*p ^ crc];
		p++;
	}
	return crc;
}

/************************************************************************/
/* Function:															*/
/*																*/
/* crc16 - Calculate CRC16 and return in long word						*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
u_long crc16_long(u_long *data, size_t nwords) {
	u_short crc;
	size_t n;
	u_short *p;

	crc = 0xffff;				/* initial crc value */
	n = nwords * 2;				/* Change word count from long word to word */
	p = (u_short *) data;		/* Change point from long word to word */

	while ( n-- ) {
		crc = crc16_table[*p ^ crc];
		p++;
	}

	crc = crc16_table[crc];		/* Upper word is zero for return value */

	return (u_long) crc;
}

/************************************************************************/
/* Function:															*/
/*																*/
/* amsw_fill_data - Fill test data patterns (size in long word)			*/
/*																*/
/* RETURNS:															*/
/* 	0 on success													*/
/************************************************************************/
void fill_data(void *buff, int nbytes, int mode, u_long pattern) {
	int i;
	u_long data;
	u_long *src;
	int size;

	src = (u_long *) buff;
	size = (nbytes + 3) / 4;

	/* Fill data to buffer */
	data = pattern;
	for ( i = 0; i < size; i++ ) {
		switch (mode) {
			case 0:		/* Shifted 0 */
				if ( i % 16 == 0 ) data = 0xFFFEFFFE;
				else data = (data << 1) + 1;
				break;
			case 1:		/* Shifted 1 */
				if ( i % 16 == 0 ) data = 0x00010001;
				else data <<= 1;
				break;
			case 2:		/* Sequencial data */
				if ( i % 64 == 0 ) data = 0x00010203;
				else			   data += 0x04040404;
				break;
			case 3:		/* Random data */
				data = (u_long) (rand() & 0xffff) | ((u_long) (rand() & 0xffff) << 16);
				break;
			case 4:		/* 0x0000FFFF and 0xFFFF0000 */
				data = (i % 2 == 0) ? 0x0000FFFF : 0xFFFF0000;
				break;
			case 5:		/* 0x5555AAAA and 0xAAAA5555 */
				data = (i % 2 == 0) ? 0x5555AAAA : 0xAAAA5555;
				break;
		}
		src[i] = data;
	}

	return;
}
