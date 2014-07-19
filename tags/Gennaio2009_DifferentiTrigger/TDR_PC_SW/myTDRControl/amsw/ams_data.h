/* -*-C-*-
 * ams_data.h - Header file for AMS-02 Data Types
 *
 *-----------------------------------------------------------------------
 *
 *
 *-----------------------------------------------------------------------
 */

#ifndef __ams_data_h
#define __ams_data_h

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************
	Definitions
 ****************************************************/

#define AMS_FRAME_SIZE			4080		/* in bytes */
#define AMS_MAX_FRAME_SIZE		4096		/* in bytes */
#define CCSDS_HEADER_SIZE		18			/* in bytes */
#define CRC16_TAILER_SIZE		2			/* in bytes */

#define MAX_AMSW_PATH			5
#define MAX_AMSW_SIZE			8192 - 8	/* in bytes */
#define AMSW_HEADER_SIZE		14

#define MAX_CAN_SIZE			2048 - 8	/* in bytes */
#define CAN_HEADER_SIZE			sizeof(AMSCANBlockHeader)

/****************************************************
	typedef and struct
 ****************************************************/

typedef struct {
	int header_type;			/* AMS Block header type */
	int header_size;			/* Header size */
	int size;					/* Blcok length */
	int reply;					/* reply = 1, req = 0 */
	int write;					/* Write = 1, read = 0 */
	int node;					/* node address */
	int type;					/* data type */
	u_char *data;				/* data pointer (after header) */
} AMSBlockInfo;

typedef struct {		/* AMS Block header with 15-bit length and 5 bit data type */
#ifdef __PPC__
	u_short flg:		1;		/* length flag, must = 0 in this case */
	u_short size:		15;		/* number of bytes exclude itself */
	u_short reply:		1;		/* request = 0, reply = 1 */
	u_short write:		1;		/* read = 0, write = 1 */
	u_short node:		9;		/* address */
	u_short type:		5;		/* data type, must not = 0x1f */
#else
	u_short size:		15;		/* number of bytes exclude itself */
	u_short flg:		1;		/* length flag, must = 0 in this case */
	u_short type:		5;		/* data type, must not = 0x1f */
	u_short node:		9;		/* address */
	u_short write:		1;		/* read = 0, write = 1 */
	u_short reply:		1;		/* request = 0, reply = 1 */
#endif
} AMSBlockHeader1;

typedef struct {		/* AMS Block header with 31-bit length and 5 bit data type */
#ifdef __PPC__
	u_short flg:		1;		/* length flag, must = 1 in this case */
	u_short size_hi:	15;		/* MS 15-bit of size */
	u_short size:		16;		/* LS 16-bit of size */
	u_short reply:		1;		/* request = 0, reply = 1 */
	u_short write:		1;		/* read = 0, write = 1 */
	u_short node:		9;		/* address */
	u_short type:		5;		/* data type, must not = 0x1f */
#else
	u_short size_hi:	15;		/* MS 15-bit of size */
	u_short flg:		1;		/* length flag, must = 0 in this case */
	u_short size:		16;		/* LS 16-bit of size */
	u_short type:		5;		/* data type, must not = 0x1f */
	u_short node:		9;		/* address */
	u_short write:		1;		/* read = 0, write = 1 */
	u_short reply:		1;		/* request = 0, reply = 1 */
#endif
} AMSBlockHeader2;

typedef struct {		/* AMS Block header with 15-bit length and 16 bit data type */
#ifdef __PPC__
	u_short flg:		1;		/* length flag, must = 0 in this case */
	u_short size:		15;		/* number of bytes exclude itself */
	u_short reply:		1;		/* request = 0, reply = 1 */
	u_short write:		1;		/* read = 0, write = 1 */
	u_short node:		9;		/* address */
	u_short ext:		5;		/* data type extend, must = 0x1f */
	u_short type:		16;		/* extended data type */
#else
	u_short size:		15;		/* number of bytes exclude itself */
	u_short flg:		1;		/* length flag, must = 0 in this case */
	u_short ext:		5;		/* data type extend, must = 0x1f */
	u_short node:		9;		/* address */
	u_short write:		1;		/* read = 0, write = 1 */
	u_short reply:		1;		/* request = 0, reply = 1 */
	u_short type:		16;		/* extended data type */
#endif
} AMSBlockHeader3;

typedef struct {		/* AMS Block header with 31-bit length and 16 bit data type */
#ifdef __PPC__
	u_short flg:		1;		/* length flag, must = 1 in this case */
	u_short size_hi:	15;		/* MS 15-bit of size */
	u_short size:		16;		/* LS 16-bit of size */
	u_short reply:		1;		/* request = 0, reply = 1 */
	u_short write:		1;		/* read = 0, write = 1 */
	u_short node:		9;		/* address */
	u_short ext:		5;		/* data type extend, must = 0x1f */
	u_short type:		16;		/* extended data type */
#else
	u_short size_hi:	15;		/* MS 15-bit of size */
	u_short flg:		1;		/* length flag, must = 0 in this case */
	u_short size:		16;		/* LS 16-bit of size */
	u_short ext:		5;		/* data type extend, must = 0x1f */
	u_short node:		9;		/* address */
	u_short write:		1;		/* read = 0, write = 1 */
	u_short reply:		1;		/* request = 0, reply = 1 */
	u_short type:		16;		/* extended data type */
#endif
} AMSBlockHeader4;

typedef struct __CCSDS {
#ifdef __PPC__
	u_short vers:		3;		/* version ID */
	u_short type:		1;		/* type, should = 1 */
	u_short secf:		1;		/* secondary header flag, should = 1 */
	u_short apid:		11;		/* Application Process ID (APID) */
	u_short seqflg:		2;		/* Sequence flag */
	u_short seq:		14;		/* Packet sequence count */
	u_short psize:		16;		/* packet size - 1 followed (frame size - 7) */
	u_short timehi:		16;		/* time MSW */
	u_short timelo:		16;		/* time LSW */
	u_short finetime:	8;		/* fine time */
	u_short timeid:		2;		/* time ID */
	u_short chk:		1;		/* NASA Check word, should = 0 */
	u_short zoe:		1;		/* Data from ZOE, should = 0 */
	u_short ptype:		4;		/* packet type, should = 6 (Payload private/science data) */
	u_short spare:		1;
	u_short eid:		4;		/* element ID */
	u_short pid1:		11;		/* Packet ID1 */
	u_short pid2:		16;		/* Packet ID2 */
	u_short fstat:		4;		/* Frame status */
	u_short start:		12;		/* start point of first AMS block, 0xfff for fill frame */
#else	/* __PPC__ */
	u_short apid:		11;		/* Application Process ID (APID) */
	u_short secf:		1;		/* secondary header flag, should = 1 */
	u_short type:		1;		/* type, should = 1 */
	u_short vers:		3;		/* version ID */
	u_short seq:		14;		/* Packet sequence count */
	u_short seqflg:		2;		/* Sequence flag */
	u_short psize:		16;		/* packet size - 1 followed (frame size - 7) */
	u_short timehi:		16;		/* time */
	u_short timelo:		16;		/* time */
	u_short ptype:		4;		/* packet type, should = 6 (Payload private/science data) */
	u_short zoe:		1;		/* Data from ZOE, should = 0 */
	u_short chk:		1;		/* NASA check word, should = 0 */
	u_short timeid:		2;		/* time ID */
	u_short finetime:	8;		/* fine time */
	u_short pid1:		11;		/* Packet ID */
	u_short eid:		4;		/* element ID */
	u_short spare:		1;
    u_short pid2:		16;		/* Packet ID */
    u_short start:		12;		/* start point of first AMS block, 0xfff for fill frame */
	u_short fstat:		4;		/* Frame status */
#endif	/* __PPC__ */
} CCSDSHeader;

typedef struct {
	int fsize;					/* Frame size */
	int apid;					/* APID */
	int seq;					/* Sequence number of the stream */
	int crc;					/* CRC flag */
	int randomize;				/* Randomize flag */
	int start;					/* AMS block start point */
	int fstat;					/* Frame status */
	int error;					/* Bit 0: Header error, Bit 1: CRC error */
	u_char *next;				/* Next AMS block pointer */
} CCSDSHeaderInfo;

typedef struct __AMS_FRAME {
	CCSDSHeader	ccsds;
	u_char		data[AMS_FRAME_SIZE - sizeof(CCSDSHeader) - sizeof(u_short)];
	u_short		crc;
} AMSFrame;

typedef struct __AMSW_BLOCK_HEADER {
	int			link;			/* Link number to be sent */
	u_long 		ctrl;			/* Link control word */
	u_char		path[MAX_AMSW_PATH];		/* AMSWire route pathes */
	u_char		bid;			/* block ID */
} AMSWireBlockHeader;

typedef struct __AMSW_BLOCK_ID {
#ifdef __PPC__
	u_char reply:	1;			/* request = 0, reply = 1 */
	u_char write:	1;			/* read = 0, write = 1 */
	u_char type:	6;			/* data type */
#else	/* __PPC__ */
	u_char type:	6;			/* data type */
	u_char write:	1;			/* read = 0, write = 1 */
	u_char reply:	1;			/* request = 0, reply = 1 */
#endif	/* __PPC__ */
} AMSWBlockID;

typedef struct __AMS_CAN_ID {
#ifdef __PPC__
	unsigned rsv1:		1;		/* Reserved bit, must 0 */
	unsigned bc:		2;		/* Block control :
								   10 = first, 01 - last, 00 -intermediate, 11 -single */
	unsigned req:		1;		/* Request = 0, reply = 1 */
	unsigned rw:		1;		/* Read = 0, write = 1 */
	unsigned bus_req:	1;		/* bus number for request */
	unsigned bus_rpl:	1;		/* bus number for reply */
	unsigned newborn:	1;		/* new born id, new born = 1 */
	unsigned src:		8;		/* source ID */
	unsigned dest:		8;		/* destination ID */
	unsigned rsv2:		5;		/* Reserved bits */
	unsigned unuse:		3;		/* unused bits */
#else	/* __PPC__ */
	unsigned unuse:		3;		/* unused bits */
	unsigned rsv2:		5;		/* Reserved bits */
	unsigned dest:		8;		/* destination ID */
	unsigned src:		8;		/* source ID */
	unsigned newborn:	1;		/* new born id, new born = 1 */
	unsigned bus_rpl:	1;		/* bus number for reply */
	unsigned bus_req:	1;		/* bus number for request */
	unsigned rw:		1;		/* Read = 0, write = 1 */
	unsigned req:		1;		/* Request = 0, reply = 1 */
	unsigned bc:		2;		/* Block control :
								   01 = first, 10 - last, 00 -intermediate, 11 -single */
	unsigned rsv1:		1;		/* Reserved bit, must 0 */
#endif	/* __PPC__ */
} AMSCANID;

typedef struct __CAN_BLOCK_HEADER {
	int 			task;		/* task number */
	u_long			nbytes;		/* number of bytes excluded itself */
	AMSCANID		id;			/* AMS CAN ID */
} AMSCANBlockHeader;

/****************************************************
	End
 ****************************************************/
#ifdef __cplusplus
}
#endif

#endif /* __ams_data_h */
