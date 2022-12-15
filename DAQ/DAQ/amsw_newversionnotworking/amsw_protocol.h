/* -*-C-*-
 * amsw_protocol.h - Header file to define AMSWire protocol related parameters
 *
 *-----------------------------------------------------------------------
 *
 *
 *-----------------------------------------------------------------------
 */

#ifndef __amsw_protocol_h
#define __amsw_protocol_h

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************/
/* Definitions                                      */
/****************************************************/

#define __KOUNINE_PROTOCOL

/****************************************************/
/* AMSWire Requests                                 */
/****************************************************/

#define __KOUNINE_PROTOCOL
#ifdef __KOUNINE_PROTOCOL
#define AMSW_REQ_CONTINUE	0x0000
#define AMSW_REQ_EVENT		0x0001
#define AMSW_REQ_STATUS		0x0002
#define AMSW_REQ_GETPARAM	0x0003
#define AMSW_REQ_SETPARAM	0x0004
#define AMSW_REQ_PING		0x0005
#define AMSW_REQ_STARTRUN	0x0006
#define AMSW_REQ_STOPRUN	0x0007
#define AMSW_REQ_STARTCLB	0x0008
#define AMSW_REQ_STOPCLB	0x0009
#define AMSW_REQ_TEST		0x0fff
#endif	/* __KOUNINE_PROTOCOL */

/****************************************************/
/* AMSWire Replys                                   */
/****************************************************/

#ifdef __KOUNINE_PROTOCOL
#define AMSW_RPL_CONTINUE	0x0000
#define AMSW_RPL_ABORT		0x0100
#define AMSW_RPL_FAIL		0x0200
#define AMSW_RPL_NOEVENT	0x0300
#define AMSW_RPL_EVENT		0x0400
#define AMSW_RPL_STATUS		0x0500
#define AMSW_RPL_PING		0x0600
#define AMSW_RPL_PARAM		0x0700
#define AMSW_RPL_TEST		0x0fff
#endif	/* __KOUNINE_PROTOCOL */

/****************************************************/
/* AMSWire Routes                                   */
/****************************************************/

#ifdef __KOUNINE_PROTOCOL
#define AMSW_ROUTE_TEST		0x0000
#define AMSW_BID_REQ		0x8000
#define AMSW_BID_RPL		0x1000
#endif	/* __KOUNINE_PROTOCOL */

/****************************************************/
/* AMSWire Packet                                   */
/****************************************************/

#define AMSW_PCK_MASK		3 << 30

#define AMSW_PCK_SINGLE		3 << 30
#define AMSW_PCK_FIRST		1 << 30
#define AMSW_PCK_LAST		2 << 30
#define AMSW_PCK_MIDDLE		0

/****************************************************/
/* Struct and Typedef                               */
/****************************************************/

struct amsw_block {
	u_char *header;			/* Header data point */
	int 	h_size;			/* Header size in bytes */
	u_char *data;			/* Data point */
	int	d_size;			/* Data size in bytes */
};

#ifdef __KOUNINE_PROTOCOL
struct amsw_header {
	u_short		route;
	u_short		bid;		/* Block ID */
	u_short		cmd;		/* Command */
	u_short		size;		/* Size of data followed in word (may be excluded) */
	u_long		start;		/* start point for data checking (for test purpose only) */
};
#endif /* __KOUNINE_PROTOCOL */

#ifdef __cplusplus
}
#endif

#endif /* __amsw_protocol_h */
