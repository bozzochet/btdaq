/* -*-C-*-
 * prod_test.h - Header file for JMDC data producer and data checker for test
 *
 *-----------------------------------------------------------------------
 * Change from P.Dennett AAL software set
 *
 *-----------------------------------------------------------------------
 */

#ifndef __prod_test_h
#define __prod_test_h

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************
	Common includes
 ****************************************************/

#include "ams_data.h"
#include "amsw_routines.h"

/****************************************************
	Definitions
 ****************************************************/

#define le2be16(x)	(((x & 0xff00U) >> 8) | ((x & 0x00ffU) << 8))
#define le2be32(x)	(((x & 0x000000ffU) << 24) | \
					 ((x & 0x0000ff00U) << 8)  | \
					 ((x & 0x00ff0000U) >> 8)  | \
					 ((x & 0xff000000U) >> 24))

#define be2le16(x)	le2be16(x)
#define be2le32(x)	le2be32(x)

/****************************************************
	typedef and struct
 ****************************************************/

typedef enum __GeneratorType {
	GenTypeSHFT0 = 0,			/* shifted 0 */
	GenTypeSHFT1 = 1,			/* shifted 1 */
	GenTypeSEQUC = 2,			/* Sequence data */
	GenTypeRANDM = 3,			/* Random data */
	GenTypeALT0F = 4,			/* Alternative 0 and f */
	GenTypeALT5A = 5,			/* Alternative 5 and a */
	GenTypeFIXED = 10,			/* Fixed double word data */
} GeneratorType;

enum AMSBlockCtrl {
	AMS_BC_FIRST = 2,			/* first packet */
	AMS_BC_LAST = 1,			/* last packet */
	AMS_BC_SINGLE = 3,			/* single packet */
	AMS_BC_INTER = 0,			/* intermediate packet */
};

enum AMSFlowCtrl {
	AMS_FLOW_NEXT = 0,			/* next packet */
	AMS_FLOW_END = 3,			/* end of packet */
	AMS_FLOW_ERROR = 1,			/* transfer error */
	AMS_FLOW_ABORT = 2,			/* abort current transfer */
};

enum AMSWBlockCtrl {
	AMSW_BC_FIRST = 1,			/* first packet */
	AMSW_BC_LAST = 2,			/* last packet */
	AMSW_BC_SINGLE = 0,			/* single packet */
	AMSW_BC_INTER = 3,			/* intermediate packet */
};

enum AMSWFlowCtrl {
	AMSW_FLOW_NEXT = 3,			/* next packet */
	AMSW_FLOW_END = 0,			/* end of packet */
	AMSW_FLOW_ERROR = 2,		/* transfer error */
	AMSW_FLOW_ABORT = 1,		/* abort current transfer */
};

enum AMSDataType {
	DataTypeBoot  = 0,			/* Boot command */
	DataTypeFile  = 1,			/* Echo command */
	DataTypeEcho  = 2,			/* Echo command */
	DataTypeStat  = 3,			/* status */
	DataTypeConf  = 4,			/* Change configuration */
	DataTypeEvent = 5,			/* Get data */
	DataTypeTime  = 6,			/* Set Time */
	DataTypeMesg  = 7,			/* Kernel log /var/log/messages file */
	DataTypeLog   = 8,			/* Error log messages */
	DataTypeCmd   = 9,			/* Command to JMDC */
	DataTypeDbug  = 29,			/* Debug */
	DataTypeTest  = 30,			/* Test */
};

enum __AMS_APID {
	APID_HK         = 10,		/* Houskeeping Data */
	APID_EVENT      = 11,		/* Physics Data */
	APID_CMD2AMS    = 12,		/* Command to AMS */
    APID_CMD2ACOP   = 13,		/* Command to ACOP */
	APID_REPLY2AMS  = 14,		/* Reply to AMS */
    APID_REPLY2ACOP = 15,		/* Reply to ACOP */
    APID_FILLFRAME  = 0x7ff,	/* Fill frame */
};

enum __AMS_HRDL_REPLY{
	ReplySuccess = 0x00000001, 	/* Success reply */
	ReplyAbort   = 0x80000001,	/* Abort reply */
	ReplyError   = 0x80000002,	/* Error reply */
};

enum __AMS_DEST_PROC {
	DestProcTHIS = 0,			/* The current process */
	DestProcAMSW = 1,			/* AMSWire Process */
	DestProcHRDL = 2,			/* HRDL & RS422 process */
	DestProc1553 = 3,			/* 1553B process */
	DestProcJCAN = 4,			/* JIM-CAM process */
	DestProcJSBC = 5,			/* JSBC command */
	DestProcJBUM = 6,			/* JBU process */
};

enum __JMDC_COMMANDS {
	JMDCCmdStop    = 1,			/* Stop process */
	JMDCCmdInit    = 2,			/* Re-initialize process */
	JMDCCmdRescan  = 3,			/* Rescan AMSWire master links */
	JMDCCmdStopTX  = 4,			/* Stop TX for HRDL or 422 */
	JMDCCmdHRDLTX  = 5,			/* Enable HRDL TX and RX */
	JMDCCmdHRDLRX  = 6,			/* Enable HRDL RX only */
	JMDCCmd422TX   = 7,			/* Enable RS422 TX and RX */
	JMDCCmd422RX   = 8,			/* Enable RS422 RX only */
	JMDCCmdRst8051 = 9,			/* Reset JIM-CAN 8051 processor */
	JMDCCmdShell   = 10,		/* Execute shell command */
};

/****************************************************
	function declarations
 ****************************************************/

u_short prod_crc16(u_char *data, int len);
int prod_crc_check(u_char *data, int len);
u_short prod_crc_attach(u_char *data, int len);
u_short prod_crc_continue(u_char *msg, int len, u_short crc);

void prod_le2be16(u_short *data, int nbytes);
void prod_le2be32(u_long *data, int nbytes);
void prod_be2le16(u_short *data, int nbytes);
void prod_be2le32(u_long *data, int nbytes);

void prod_data(u_char *buff, int nbytes, int type, u_long pattern);

int prod_amsw_tx_header(u_char *buff, u_char bid);
int prod_amsw_echo_data(u_char *buff, int type, u_long data);
void prod_amsw_set_reply(u_char *buff, int size);

void prod_ccsds_header(u_char *buff, CCSDSHeaderInfo *info);
int prod_ccsds_check(u_char *frame, CCSDSHeaderInfo *info);
void prod_randomize(u_char *frame, int fsize);
void prod_fill_frame(u_char *frame, int fsize, int type, u_long data);
void prod_echo_frame(u_char *frame, int fsize, int jmdc, int crc, int randomize, int type, u_long data);
void prod_req_stat_frame(u_char *frame, int fsize, int jmdc);
void prod_req_elog_frame(u_char *frame, int fsize, int jmdc);
void prod_req_stat_frame(u_char *frame, int fsize, int jmdc);
int prod_command_frame(u_char *frame, int fsize, int jmdc, u_char *cmd, int csize);

int prod_ams_header1(u_char *buff, int size, int reply, int write, int node, int type);
int prod_ams_header2(u_char *buff, int size, int reply, int write, int node, int type);
int prod_ams_header3(u_char *buff, int size, int reply, int write, int node, int type);
int prod_ams_header4(u_char *buff, int size, int reply, int write, int node, int type);
void prod_ams_check_header(u_char *block, AMSBlockInfo *info);
void prod_ams_reply_header(int header_type, int header_size, u_char *buff, int size, int type);
int prod_amsw_req_stat(u_char *buff);
int prod_amsw_req_elog(u_char *buff);
int prod_amsw_req_klog(u_char *buff);
int prod_amsw_command(u_char *buff, u_char *cmd, int csize);

void prod_save_status(char id, u_char *stat, int size);
void prod_amsw_save_stat(u_char *packet, int size);
void prod_hrdl_save_stat(u_char *frame);
void prod_422_save_stat(u_char *frame);
void prod_can_save_stat(u_char *block, int size);
void prod_save_elog(u_char *msg, int size);
void prod_save_klog(u_char *msg, int size);

#ifdef __cplusplus
}
#endif

#endif /* __prod_test_h */
