// file easslib.h
//
// see easslib.c
//
// A.Lebedev Mar-2008...
//
// All this was stolen from Peter Dennett (eass.h) and reworked
//

//~----------------------------------------------------------------------------

#ifndef _EASSLIB_H
#define _EASSLIB_H

#include "mylib.h"

#define closesocket close

// Error Codes:

#define BLOCK_S_END          0  // End: request completed without errors
#define BLOCK_A_ABORT        1  // Abort: abort from slave
#define BLOCK_A_UNKNOWN      2  // Abort: unknown types, operations
#define BLOCK_A_NOSPACE      3  // Abort: no space
#define BLOCK_A_NODEVICE     4  // Abort: no such device or target
#define BLOCK_A_NOTSUPPORT   5  // Abort: not supported functions

#define BLOCK_E_ERROR        8  // Error: error from slave
#define BLOCK_E_COMM         9  // Error: comm errors (TX/RX errors, CheckWord/CRC errors)
#define BLOCK_E_PROTOCOL    10  // Error: protocol errors
#define BLOCK_E_FORMAT      11  // Error: data format errors
#define BLOCK_E_TIMEOUT     12  // Error: timeout

//~----------------------------------------------------------------------------

//+++ Following section is from AALpro/eAss/eass.h

enum {
  eAss_MT_HELLO   =  1,
  eAss_MT_REPLY   =  2,
  eAss_MT_ABORT   =  3,
  eAss_MT_USERS   =  4,
  eAss_MT_COMMAND =  5,
  eAss_MT_POLL    =  6,
  eAss_MT_MONITOR =  7,
  eAss_MT_ADMIN   =  8,
  eAss_MT_LOGBOOK =  9,
  eAss_MT_PING    = 10,
  eAss_MT_PASSWD  = 11,
  eAss_MT_APPROVE = 12,
  eAss_MT_BYE     = 13,
};

typedef struct _eAssTag  *peAssTag;
typedef struct _eAssIntf  eAssIntf, *peAssIntf;
typedef void (*peAssCB)(peAssIntf eAss, int msgtype, peAssTag msg);

struct _eAssTag {
  struct _eAssTag *next;
  char *type;
  char *tag;
  int  bytecount;
  union _value {
    void  *value;
    char  *t_string;
    int8  *t_int8;
    int16 *t_int16;
    int32 *t_int32;
    float *t_float;
    struct _eAssTag *t_envelop;
  } value;
  int32 contents[2];
};

struct _eAssIntf {
  char name[32];
  int socket;
  peAssCB callback;
};

typedef struct _eAssConnection {
  bool Accepted;                  // If accepted this socket is active
  bool WriteReady;                // Socket is ready for write
  int  ClntSock;                  // Socket descriptor for client 
  struct sockaddr_in ClntAddr;    // Client address
} eAssConnection, *peAssConnection;

typedef struct _eAssServerArg {
  int Port;                     // Default 5554
  int Debug;                    // Debug level
  int Running;                  // Initialized to 1 by eAssServer, set to 0 to 
                                // request server exit
  int Node;                     // Node number for commands to us

  struct _StatseAss {
    int32 CommandCnt;
    int32 ReplyCnt;
    int32 ReplyPolls;
    int32 RepliesPosted;
  } Stats;

  pAMSBlock ReplyList[256];     // Our list of AMSBlocks for replies

  pthread_mutex_t ReplyListLock;// Lock for multi thread access
  int eAssSock;                 // The connect socket
  eAssConnection eAssConnections[20];  // Number of connections supported
} eAssServerArg, *peAssServerArg;

//~----------------------------------------------------------------------------

// eassserver.c

int eAssServer(void *arg);
int eAssPostReply(peAssServerArg this, pAMSBlock pReply);
int eAssServerStatus(peAssServerArg this);

// Callback:

int eAssQueueRequest(peAssServerArg this, pAMSBlock abi);
int eAssQueueReply(peAssServerArg this, pAMSBlock abi);

// easslib.c

int eAssInit(void);
peAssIntf eAssOpen(char *name);
int eAssClose(peAssIntf eAss);
int eAssSend(peAssIntf eAss, int msgtype, peAssTag msg);
peAssTag eAssTag(peAssTag msg, char *type, char *tag, void *value, int bytecount);
peAssTag eAssAMSBlock(peAssTag msg, pAMSBlock pRequest);
int eAssSize(peAssTag msg);
int eAssRelease(peAssTag msg);
int eAssPack(int8 **buffer, peAssTag msg);
peAssTag eAssUnpack(int8 *buffer, int cnt);
int eAssShow(peAssTag msg, int indent);
int eAssRegisterCallback(peAssIntf eAss, peAssCB callback);
peAssTag eAssRecv(peAssIntf eAss, int timeout_ms, int *rtnmsgtype);

// eassclient.c

int eAssConnect(peAssIntf eAss, char *host, char *port);
int eAssDisconnect(peAssIntf eAss);
int eAssLogin(peAssIntf eAss, char *User, char *Password, char *ApplicationName);
int eAssLogout(peAssIntf eAss);
int eAssRequest(peAssIntf eAss, pAMSBlock pRequest, bool RequestCommandTag);
int eAssRequestWithReply(pAMSBlock pRequest, pAMSBlock pReply);
int eAssPollReply(peAssIntf eAss, int CommandTag, pAMSBlock *pReply);

//~============================================================================

#endif // _EASSLIB_H
