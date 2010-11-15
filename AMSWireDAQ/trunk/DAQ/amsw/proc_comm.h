/* -*-C-*-
 * proc_comm.h - header file for communication between processes
 *
 *-----------------------------------------------------------------------
 *
 * Project          : Linux
 * Moduletyp        : C-Code
 * Author           : Xudong CAI
 * Created          : 31.07.2002
 *
 *-----------------------------------------------------------------------
 * 31.07.2002 - created
 *-----------------------------------------------------------------------
 */

#ifndef __PROC_COMM_H__
#define __PROC_COMM_H__

/************************************************************************
	Common include files
 ************************************************************************/

#ifdef __KERNEL__
#include <linux/ipc.h>
#include <linux/msg.h>
#include <linux/types.h>
#else   /* __KERNEL__ */
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#endif  /* __KERNEL__ */
#include <asm/errno.h>

/************************************************************************
	Definitions
 ************************************************************************/

#define AMS_MAX_MSG			4080

/* IPC types */
#define AMS_MSGS			1 << 24
#define AMS_SEMS			1 << 25
#define AMS_SHMS			1 << 26

/* TASK types */
#define AMS_IPC_AMSW		1 << 16
#define AMS_IPC_HRDL		1 << 17
#define AMS_IPC_JCAN		1 << 18
#define AMS_IPC_DIGI		1 << 19

/* Message types */
#define AMS_MTYPE_CMD		1

#define AMS_MTYPE_ANY		0
#define AMS_MTYPE_PRIO		-100

/* Message queue ID for command */
#define AMSW_MSGQUE_CMD		AMS_MSGS | AMS_IPC_AMSW | AMS_MTYPE_CMD
#define HRDL_MSGQUE_CMD		AMS_MSGS | AMS_IPC_HRDL | AMS_MTYPE_CMD
#define JCAN_MSGQUE_CMD		AMS_MSGS | AMS_IPC_JCAN | AMS_MTYPE_CMD
#define DIGI_MSGQUE_CMD		AMS_MSGS | AMS_IPC_DIGI | AMS_MTYPE_CMD

/************************************************************************
	Typedef & Struct
 ************************************************************************/

typedef struct __ams_cmd_msgbuf {
    long        type;       /* Message type */
    int			size;       /* size of following data */
} CMD_MSGBUF;

/************************************************************************
	Function Prototypes
 ************************************************************************/

void proc_send_cmd(int key, u_char *cmd, int size);
int proc_open_cmd(int key);
void proc_close_cmd(int msgid);
int proc_get_cmd(int msgid, u_char *cmd);

/************************************************************************
	End
 ************************************************************************/
#endif  /* __PROC_COMM__ */
