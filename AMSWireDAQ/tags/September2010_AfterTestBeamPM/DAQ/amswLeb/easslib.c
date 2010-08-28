// file easslib.c
//
// A.Lebedev Mar-2008...
//
// All this was stolen from Peter Dennett: (files eass.c, eassclient.c
// and easstcp.c) and reworked.
//
// Re-stolen by Peter Dennett but not completely reworked

//~----------------------------------------------------------------------------

#include "easslib.h"

#ifndef ENV_PAD_AAL 
#include "mylib.h"
#endif

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

//+++ Following section is from AALpro/eAss/easslib.c

static char *eAssTypes[] =
{
  "STRING",
  "INT8",
  "INT16",
  "INT32",
  "FLOAT",
  "ENVELOP",
};

static char *eAssTags[] =
{
  "STATUS",
  "USER",
  "PASSWORD",
  "NETSIZE",
  "AMSBLOCK",
  "NODE",
  "BLOCKTYPE",
  "DATATYPE",
  "BLOCK",
  "AMSFRAME",
  "COMMENT",
  "REPLY",
  "CRC",
  "COMMANDTAG",
  "TIMEOUT",
  "IDENTIFIER",
  "SESSION",
  "TIME",
  "GROUP"
};

////////////////////////////////////////////////////////////////////////////////

peAssIntf eAssOpen(char *name)
{
peAssIntf eAss = NULL;

  eAss = (peAssIntf) malloc(sizeof *eAss);
  memset(eAss, 0, sizeof *eAss);
  strncpy(eAss->name, name, sizeof eAss->name);
  
  return eAss; 
}

////////////////////////////////////////////////////////////////////////////////

int eAssClose(peAssIntf eAss)
{
  free((char*) eAss);

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

peAssTag eAssTag(peAssTag msg, char *type, char *tag, void *value, int bytecount)
{
  peAssTag t, m;
  int itype, itag;

  for (itype=0; itype<((sizeof eAssTypes)/(sizeof eAssTypes[0])); itype++)
    if (strcmp(eAssTypes[itype], type) == 0)
      goto GotType;

  return NULL;

 GotType:

  for (itag=0; itag<((sizeof eAssTags)/(sizeof eAssTags[0])); itag++)
    if (strcmp(eAssTags[itag], tag) == 0)
      goto GotTag;

  itag = -1;

 GotTag:

  if (strcmp(type, "ENVELOP") == 0)
    bytecount = 0;
  else if (bytecount >= 0)
    ;
  else if (strcmp(type, "STRING") == 0)
    bytecount = strlen((char *) value);
  else 
    bytecount = 1;

  t = (peAssTag) malloc((sizeof *t) + bytecount + strlen(tag));
  if ( ! t)
    return NULL;

  memset(t, 0, sizeof *t);

  if (bytecount)
    memcpy((char*)t->contents, value, bytecount);

  if (itag < 0)
  {
    strcpy(((char*)(t->contents))+bytecount, tag);
    tag = ((char*)(t->contents))+bytecount;
  }
  else
  {
    tag = eAssTags[itag];
  }

  t->tag = tag;
  t->type = eAssTypes[itype];
  if (strcmp(type, "ENVELOP") == 0)
    t->value.t_envelop = value;
  else
    t->value.t_string = (char*)&(t->contents);
  t->bytecount = bytecount;

  if (msg)
  {
    for (m=msg; m->next; m=m->next) ;
    m->next = t;
  }

  //?? printf("eAssTag %s %s %d\n", t->type, t->tag, t->bytecount);

  return t;
}

////////////////////////////////////////////////////////////////////////////////

peAssTag eAssAMSBlock(peAssTag msg, pAMSBlock pRequest)
{
  peAssTag m;

  m =    eAssTag(NULL, "INT16",   "NODE",      &pRequest->NodeAddress, sizeof (int16));
         eAssTag(m,    "INT8",    "BLOCKTYPE", &pRequest->BlockType, sizeof (int8));
         eAssTag(m,    "INT32",   "DATATYPE",  &pRequest->DataType, sizeof (int32));
         eAssTag(m,    "INT16",   "ERROR",     &pRequest->Error, sizeof (int16));
         eAssTag(m,    "INT16",   "COMMANDTAG", &pRequest->CommandTag, sizeof (int16));
         eAssTag(m,    "INT32",   "DATATIME",  &pRequest->DataTime, sizeof (int32));
#ifndef ENV_PAD_AAL 
	 if (pRequest->Data.p8)
	   pRequest->Contents = pRequest->Data.p8;
	 else if (pRequest->Data.p16)
	   pRequest->Contents = pRequest->Data.p16;
	 else
	   pRequest->Contents = m;  //?? Just avoid segfault for now
#endif
         eAssTag(m,    "INT16",   "BLOCK",     pRequest->Contents, pRequest->DataCount);
  return eAssTag(msg,  "ENVELOP", "AMSBLOCK",  m, 1);
}

////////////////////////////////////////////////////////////////////////////////

int eAssSize(peAssTag msg)
{
  int len = 0;
  int i;

  for (; msg; msg=msg->next)
  {
    for (i=0; i<((sizeof eAssTypes)/(sizeof eAssTypes[0])); i++)
      if (strcmp(eAssTypes[i], msg->type) == 0)
        goto GotType;

    return 0;

  GotType:

  len++;

    for (i=0; i<((sizeof eAssTags)/(sizeof eAssTags[0])); i++)
      if (strcmp(eAssTags[i], msg->tag) == 0)
      {
        len += 1 + 1;
        goto GotTag;
      }

    len += 1 + strlen(msg->tag);
    
  GotTag:

    len += 4;

    if (strcmp(msg->type, "ENVELOP") == 0)
      len += eAssSize(msg->value.t_envelop);
    else
      len += msg->bytecount;
  }

  return len;
}

////////////////////////////////////////////////////////////////////////////////

int eAssRelease(peAssTag msg)
{
  peAssTag m;

  while (msg)
  {
    if (strcmp(msg->type, "ENVELOP") == 0)
      eAssRelease(msg->value.t_envelop);

    m = msg->next;
    free((char*)msg);
    msg = m;
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

int eAssPack(int8 **buffer, peAssTag msg)
{
  int i, len;
  int8 *cp;

  for (; msg; msg=msg->next)
  {
    for (i=0; i<((sizeof eAssTypes)/(sizeof eAssTypes[0])); i++)
      if (strcmp(eAssTypes[i], msg->type) == 0)
        goto GotType;

    return 0;

  GotType:

    *(*buffer)++ = i;

    for (i=0; i<((sizeof eAssTags)/(sizeof eAssTags[0])); i++)
      if (strcmp(eAssTags[i], msg->tag) == 0)
      {
        *(*buffer)++ = 0;
        *(*buffer)++ = i;
        goto GotTag;
      }
    
    len = strlen(msg->tag);
    if (len > 128)
      return 0;

    *(*buffer)++ = len;
    strcpy((char*)*buffer, (char*)msg->tag);
    *buffer += len;    
    
  GotTag:

    if (strcmp(msg->type, "ENVELOP") == 0)
    {
      len = eAssSize(msg->value.t_envelop);
      cp = (int8*)&len;
      for (i=0; i<4; i++)
        *(*buffer)++ = *cp++;
      
      eAssPack(buffer, msg->value.t_envelop);
    }
    else
    { 
      cp = (int8*)&msg->bytecount;
      for (i=0; i<4; i++)
        *(*buffer)++ = *cp++;
      
      cp = msg->value.t_int8;
      for (i=0; i<msg->bytecount; i++)
        *(*buffer)++ = *cp++;
    }
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

peAssTag eAssUnpack(int8 *buffer, int cnt)
{
peAssTag rmsg = NULL;
peAssTag msg = NULL;
char *type;
char *tag, tagv[129];
void *value;
int i;
int8 *cp;
int bytecount;

//??printf("eAssUnpack:\n");

  while (cnt)
  {
    i = *buffer++;
    cnt--;
    if (i > ((sizeof eAssTypes)/(sizeof eAssTypes[0])))
      return NULL;
    type = eAssTypes[i];

    i = *buffer++;
    cnt--;
    if (i == 0)
    {
      i = *buffer++;
      cnt--;
      if (i > ((sizeof eAssTags)/(sizeof eAssTags[0])))
        return NULL;
      tag = eAssTags[i];
    }
    else
    {      if (i > ((sizeof tagv)-1))
        return NULL;
      strncpy((char*)tagv, (char*)buffer, i);
      tagv[i] = 0;
      tag = tagv;
      buffer += i;
      cnt -= i;
    }

    cp = (int8*)&bytecount;
    *cp++ = *buffer++;
    *cp++ = *buffer++;
    *cp++ = *buffer++;
    *cp++ = *buffer++;
    cnt -= 4;

    if (cnt < bytecount)
      return NULL;

    if (strcmp(type, "ENVELOP") == 0)
      value = eAssUnpack(buffer, bytecount);
    else
      value = buffer;

    if ( ! value)
        return NULL;

    buffer += bytecount;
    cnt -= bytecount;
    
    msg = eAssTag(rmsg, type, tag, value, bytecount);
    if ( ! msg)
      return NULL;
    if ( ! rmsg)
      rmsg = msg;
  }

  return rmsg;
}

////////////////////////////////////////////////////////////////////////////////

int eAssShow(peAssTag msg, int indent)
{
  int i, cnt;
  int MaxValues = 24;
  for (; msg; msg=msg->next)
  {
    for (i=0; i<indent; i++)
      printf(" ");

    printf("%-7.7s %s [%d]", msg->type, msg->tag, msg->bytecount);

    if        (strcmp(msg->type, "STRING") == 0)
    {
      //      printf(" \"%s\"", msg->value.t_string);
      printf(" \"");
      cnt = msg->bytecount / sizeof(int8);
      if (cnt > MaxValues) cnt = MaxValues;
      for (i=0; i<cnt; i++)
        printf("%c", msg->value.t_int8[i]);
      printf("\"%s\n", msg->bytecount>MaxValues?" ...":"");
    } else if (strcmp(msg->type, "INT8") == 0)
    {
      cnt = msg->bytecount / sizeof(int8);
      if (cnt > MaxValues) cnt = MaxValues;
      for (i=0; i<cnt; i++)
        printf(" %d", msg->value.t_int8[i]);
      printf("%s\n", msg->bytecount>MaxValues?" ...":"");
    }
    else if (strcmp(msg->type, "INT16") == 0)
    {
      cnt = msg->bytecount / sizeof(int16);
      if (cnt > MaxValues) cnt = MaxValues;
      for (i=0; i<cnt; i++)
        printf(" %d", (msg->value.t_int16)[i]);
      printf("%s\n", msg->bytecount>MaxValues?" ...":"");
    }
    else if (strcmp(msg->type, "INT32") == 0)
    {
      cnt = msg->bytecount / sizeof(int32);
      if (cnt > MaxValues) cnt = MaxValues;
      for (i=0; i<cnt; i++)
        printf(" %d", msg->value.t_int32[i]);
      printf("%s\n", msg->bytecount>MaxValues?" ...":"");
    }
    else if (strcmp(msg->type, "FLOAT") == 0)
    {
      cnt = msg->bytecount / sizeof(float);
      if (cnt > MaxValues) cnt = MaxValues;
      for (i=0; i<cnt; i++)
        printf(" %g", msg->value.t_float[i]);
      printf("%s\n", msg->bytecount>MaxValues?" ...":"");
    }
    else if (strcmp(msg->type, "ENVELOP") == 0)
    {
      printf("\n");
       eAssShow(msg->value.t_envelop, indent+2);
    }
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

int eAssRegisterCallback(peAssIntf eAss, peAssCB callback)
{
  eAss->callback = callback;
  return 0;
}

//+++ Following section is from AALpro/eAss/easstcp.c

////////////////////////////////////////////////////////////////////////////////

int eAssInit(void)
{
#ifdef OS_WIN32
  {
  WSADATA wsadata;
    if (WSAStartup(MAKEWORD(1,1), &wsadata) == SOCKET_ERROR)
      return 0;
  }
#endif

  return 1; 
}

////////////////////////////////////////////////////////////////////////////////

int eAssSend(peAssIntf eAss, int msgtype, peAssTag msg)
{
  int32 len;
  int8 *buff, *b2, *cp;
  int i;

  len = eAssSize(msg);

//printf("eAssSend: len = %d\n", len);

  buff = malloc(4 + 4 + 2 + len);
  if ( ! buff)
    return 0;

  b2 = buff;
  strncpy((char*)b2, "eAS0", 4);
  b2 += 4;

  cp = (int8*)&len;
  for (i=0; i<4; i++)
    *b2++ = *cp++;

  cp = (int8*)&msgtype;
  for (i=0; i<2; i++)
    *b2++ = *cp++;

  eAssPack(&b2, msg);

  i = send(eAss->socket, buff, 4+4+2+len, 0);
  free((char*) buff);

  if (i != (4+4+2+len))
    return 0;

  return 1;
}

////////////////////////////////////////////////////////////////////////////////

int eAssRecvBlock(int socket, int8 *block, int32 len, int timeout_ms)
{
int rcvd=0, r;
fd_set rd;
struct timeval tv;

  tv.tv_sec = timeout_ms / 1000;
  tv.tv_usec = (timeout_ms - tv.tv_sec*1000) * 1000;

  while (rcvd < len)
  {
    FD_ZERO(&rd);
    FD_SET(socket, &rd);

    r = select(socket+1, &rd, NULL, NULL, &tv);

    if (r ==  0)
    {
      printf("eAssRecvBlock: select time out after %d bytes\n", rcvd);
      return -2;
    }

    if (r == -1) 
    {
      printf("eAssRecvBlock: select %s\n", strerror(errno));
      return r;
    }

    r = recv(socket, &block[rcvd], len - rcvd, MSG_WAITALL);

    if (r <= 0)
    {
      //printf("eAssRecvBlock: error on recv %d\n", r);
      return -1;
    }

    rcvd += r;
  }
  
  return rcvd;
}

////////////////////////////////////////////////////////////////////////////////

peAssTag eAssRecv(peAssIntf eAss, int timeout_ms, int *rtnmsgtype)
{
  int32 len;
  //??  int err;
  int8 header[10];
  int16 msgtype;
  int8 *cp;
  int8 *buff;
  peAssTag msg;
  int r;

  if ((r=eAssRecvBlock(eAss->socket, header, sizeof header, timeout_ms)) != sizeof header)
  {
    //printf("eAssRecv: eAssRecvBlock returned %d\n", r);
    return NULL;
  }

  if (strncmp((char*)header, "eAS0", 4) != 0)
  {
    printf("eAssRecv: invalid header %4.4s\n", header);
    return NULL;
  }

  cp = (int8*)&len;
  *cp++ = header[4];
  *cp++ = header[5];
  *cp++ = header[6];
  *cp++ = header[7];

  //??  During testing limit messages by size

  if (len > 1000000)
  {
    printf("eAssRecv: message too large %d\n", len);
    return NULL;
  }

  cp = (int8*)&msgtype;
  *cp++ = header[8];
  *cp++ = header[9];

  if ((msgtype>eAss_MT_BYE) | (msgtype<eAss_MT_HELLO))
  {
    printf("eAssRecv: msgtype invalid %d\n", msgtype);
    return NULL;
  }

  buff = (int8*)malloc(len);

  if (eAssRecvBlock(eAss->socket, buff, len, timeout_ms) != len)
  {
    //printf("eAssRecvBlock: failed\n");
    free((char*) buff);
    return NULL;
  }

  msg = eAssUnpack(buff, len);
  if ( ! msg)
  {
    printf("eAssRecv: eAssUnpack failed\n");
  }
  free((char*) buff);
  *rtnmsgtype = msgtype;

  return msg;
}

//+++ Following section is from AALpro/eAssClient/eassclient.c

////////////////////////////////////////////////////////////////////////////////

int eAssConnect(peAssIntf eAss, char *Host, char *Port)
{
#ifndef OS_WIN32
struct utsname uts;
#endif
struct hostent *hp; 
int sd, bport;
#ifndef OS_WIN32
const int on=1;
#else
const char on=1;
#endif
char dport[] = "5554";
char dhost[] = "127.0.0.1";
struct sockaddr_in Addr;

  if (( ! Host) || (! *Host))
  {
    Host = dhost;
#ifndef OS_WIN32
    uname( &uts );
    Host = uts.nodename;
#endif
  }

  if (( ! Port) || ( ! *Port))
    Port = dport;

  //??  sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  sd = socket(AF_INET, SOCK_STREAM, 0);
  if ( sd < 0 )
  {
    printf("eAssConnect: socket failed %d\n", sd);
    return 0;
  }

  bport = strtol(Port, NULL, 0);

  memset(&Addr, 0, sizeof(Addr));
  Addr.sin_family = AF_INET;
  Addr.sin_port = htons(bport);

  hp = gethostbyname(Host);
  if (hp == 0)
    Addr.sin_addr.s_addr = inet_addr(Host);
  else
    memcpy((char*)&Addr.sin_addr, (char*)hp->h_addr_list[0], hp->h_length);

  if ( setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) != 0 )
  {
    printf("eAssConnect: setsockopt failed\n");
    closesocket(sd);
    return 0;
  }

  if (connect(sd, (struct sockaddr *) &Addr, sizeof Addr) < 0)
  {
    //printf("eAssConnect: connect failed\n");
    closesocket(sd);
    return 0;
  }

#ifndef OS_WIN32
  //??  fcntl(sd, F_SETFL, O_NONBLOCK);
  signal(SIGPIPE, SIG_IGN);
#else
#endif

  eAss->socket = sd;

  return 1; 
}

////////////////////////////////////////////////////////////////////////////////

int eAssDisconnect(peAssIntf eAss)
{
  shutdown(eAss->socket, 0);
  closesocket(eAss->socket);
  eAss->socket = 0;

  return 0; 
}

////////////////////////////////////////////////////////////////////////////////

int eAssLogin(peAssIntf eAss, char *User, char *Password, char *ApplicationName)
{
peAssTag msg;
char Host[32] = "host";
char Pid[12] = "*";
int msgtype;

#ifdef OS_WIN32
DWORD len;

  len = sizeof (Host) - 1;
  GetComputerName (Host, &len);
#else
struct utsname uts;

  uname( &uts );
  strncpy(Host, uts.nodename, sizeof Host);
#endif

  sprintf(Pid, "%d", getpid());

  msg = eAssTag(NULL, "STRING", "USER",     User, -1);
        eAssTag(msg,  "STRING", "PASSWORD", Password, -1);
        eAssTag(msg,  "STRING", "HOST",     Host, -1);
        eAssTag(msg,  "STRING", "PID",      Pid, -1);
        eAssTag(msg,  "STRING", "APP",      ApplicationName, -1);

  if ( ! eAssSend(eAss, eAss_MT_HELLO, msg))
  {
    eAssRelease(msg);
    return 0;
  }

  eAssRelease(msg);

  msg = eAssRecv(eAss, 2000, &msgtype);
#if 0
  if (msg)
  {
    printf("eAssLogin %d\n", msgtype);
    eAssShow(msg, 0);
  }
#endif

  eAssRelease(msg);

  return 1;
}

////////////////////////////////////////////////////////////////////////////////

int eAssLogout(peAssIntf eAss)
{
  //?? peAssTag msg;
  return 1;
}

////////////////////////////////////////////////////////////////////////////////

int eAssRequest(peAssIntf eAss, pAMSBlock pRequest, bool RequestCommandTag)
{
int tagval = 0;
peAssTag  msg, m;
int msgtype;
int status;

//printf("eAssRequest: pRequest->DataCount=%d\n", pRequest->DataCount);

  msg = eAssTag(NULL, "STRING", "COMMENT", "Command Desc", -1);
  if (RequestCommandTag)
        eAssTag(msg,  "INT16", "COMMANDTAG", &tagval, 1);
	eAssAMSBlock(msg, pRequest);

	//?? printf("eAssRequest: Sending\n");
	//?? eAssShow(msg, 0);
  
  if ( ! eAssSend(eAss, eAss_MT_COMMAND,  msg))
  {
    eAssDisconnect(eAss);
    eAssRelease(msg);
    return -1000;
  }

  //??  printf("eAssRequestWithReply: Recving\n");

  eAssRelease(msg);
  msg = eAssRecv(eAss, 10000, &msgtype);
  tagval = 0;
  status = 900;
  if (msg)
  {
    //??    printf("eAssRequest %d\n", msgtype);
    //??    eAssShow(msg, 0);

    for (m=msg; m; m=m->next)
    {
      if ((strcmp(m->type, "INT16") == 0) &&
          (strcmp(m->tag,  "COMMANDTAG") == 0))
        tagval = *m->value.t_int16;

      if ((strcmp(m->type, "INT8") == 0) &&
          (strcmp(m->tag,  "STATUS") == 0))
        status = *m->value.t_int8;
    }
    eAssRelease(msg);
  }
  else
  {
    printf("eAssRequest: eAssRecv failed\n");
    return -1001;
  }

  if (status != 0)
  {
    printf("eAssRequest: Request error %d\n", -status);
    tagval = -status;
  }

  return tagval;
}

////////////////////////////////////////////////////////////////////////////////

int eAssPollReply(peAssIntf eAss, int CommandTag, pAMSBlock *pReply)
{
static AMSBlock Reply;
int16 tagval;
static char *BLOCKDATA = NULL;  //???  Really bad style!!!!!!!
int msgtype;
peAssTag  msg, m, n;

  *pReply = NULL;

  if (CommandTag == 0)
  {
    printf("eAssPollReply: Ignoring poll for CommandTag of zero\n");
    return 0;
  }

  msg = eAssTag(NULL, "INT16", "AMSBLOCK", &CommandTag, sizeof (int16));
  if ( ! eAssSend(eAss, eAss_MT_POLL,  msg))
  {
    //?? really should somehow shut down polling on this error
    //printf("eAssPollReply: eAss_MT_POLL failed to send\n");
    eAssRelease(msg);
    return -1;
  }
  eAssRelease(msg);

  tagval = CommandTag;
  msg = eAssRecv(eAss, 2000, &msgtype);
  if ((msgtype == eAss_MT_REPLY) && msg)
  {
    for (m=msg; m; m=m->next)
    {
      if ((strcmp(m->type, "ENVELOP") == 0) && (strcmp(m->tag,  "AMSBLOCK") == 0))
      {
        //eAssShow(msg, 0);
        memset((char*) &Reply, 0, sizeof Reply);
        for (n=m->value.t_envelop; n; n=n->next)
        {
          if      ((strcmp(n->type, "INT16") == 0) && (strcmp(n->tag, "NODE") == 0))
            Reply.NodeAddress = n->value.t_int16[0];
          else if ((strcmp(n->type, "INT8")  == 0) && (strcmp(n->tag, "BLOCKTYPE") == 0))
            Reply.BlockType = n->value.t_int8[0];
          else if ((strcmp(n->type, "INT32") == 0) && (strcmp(n->tag, "DATATYPE") == 0))
            Reply.DataType = n->value.t_int32[0];
          else if ((strcmp(n->type, "INT16")  == 0) && (strcmp(n->tag, "ERROR") == 0))
            Reply.Error = n->value.t_int16[0];
          else if ((strcmp(n->type, "INT16")  == 0) && (strcmp(n->tag, "COMMANDTAG") == 0))
            Reply.CommandTag = n->value.t_int16[0];
          else if ((strcmp(n->type, "INT32") == 0) && (strcmp(n->tag, "DATATIME") == 0))
            Reply.DataTime = n->value.t_int32[0];
          else if ((strcmp(n->type, "INT16")  == 0) && (strcmp(n->tag, "BLOCK") == 0))
          {
            Reply.Contents = n->value.value;
            Reply.DataCount = n->bytecount;
//printf("eAssPollReply: Reply.DataCount=%d\n", Reply.DataCount);
          }
        }

        if (BLOCKDATA) free(BLOCKDATA);   //?? Should handle memory better then this
        BLOCKDATA = malloc(Reply.DataCount);
        memcpy(BLOCKDATA, Reply.Contents, Reply.DataCount);
        Reply.Contents = BLOCKDATA;
        tagval = 0;
      }
    }
    eAssRelease(msg);
  }

  if (tagval != 0)
    return 0;

  //?? abiPrint("REPLY", &Reply, 32);
  *pReply = &Reply;
  return 1;
}

//~----------------------------------------------------------------------------

int eAssRequestWithReply(pAMSBlock pRequest, pAMSBlock pReply) {

  peAssIntf eAss;
  int tagval;
  pAMSBlock pBlock;
  char port[20];
  int to;
  char *local_progname = progname?progname:"NULL";

  *pReply = *pRequest;
  pReply->Error = BLOCK_E_COMM;
  pReply->DataCount = 0;

//~   Initialize the eAss system

  eAssInit();

//~   Open the client services

  eAss = eAssOpen(local_progname); // AALs external
  if (!eAss) {
    printf("eAssRequestWithReply: eAssOpen failed\n");
    return 0;
  }

  sprintf(port, "%d", CS_port); // AALs external

//~   Make a connection to the server

  if (!eAssConnect(eAss, CS_address, port)) {
    printf("eAssRequestWithReply: eAssConnect(%s, %s) failed\n", CS_address, port);
    return 0; 
  }

//~   Log on the user

  if (!eAssLogin(eAss, "pdennett", "pw", local_progname)) {
    eAssDisconnect(eAss);
    printf("eAssRequestWithReply: eAssLogin failed\n");
    return 0;
  }

//~   This sends an AMSBlock to the server and requests the return of a CommandTag

  if (P > 3) printf("eAssRequestWithReply: pRequest->DataCount=%d\n", pRequest->DataCount);

  tagval = eAssRequest(eAss, pRequest, TRUE);
  if (tagval <= 0) {
    printf("eAssRequestWithReply: eAssRequest failed %d\n", tagval);
    return 0;
  }

//~   This sends a poll to the server to see if an AMSBlock with a matching CommandTag
//~   can be returned.

#ifndef ENV_PAD_AAL 

  if (P > 3) printf("ENV_PAD_ALL not defined\n");

  if (!get_command_timeout(NULL)) set_command_timeout(20.0);

  timer(0, START);
  for (pBlock=NULL, to=0; !timer(0, EXPIRED, command_timeout) && !pBlock; to++) {
    if (P > 3) printf("eAssRequestWithReply: Polling tag %d to=%d\n", tagval, to);
    if (!eAssPollReply(eAss, tagval, &pBlock)) ShortSleep(0, 20000);
  }

#else

  if (P > 3) printf("ENV_PAD_ALL defined\n");

  for (to=0, pBlock=NULL; to < 500 && !pBlock; to++) {
    if (P > 3) printf("eAssRequestWithReply: Polling tag %d to=%d\n", tagval, to);
    if (!eAssPollReply(eAss, tagval, &pBlock)) ShortSleep(0, 20000);
  }

#endif

  if (pBlock) *pReply = *pBlock;
  else         pReply->Error = BLOCK_E_TIMEOUT;

  if (P > 3) printf("eAssRequestWithReply: pReply->DataCount=%d\n", pReply->DataCount);

  eAssDisconnect(eAss);
  eAssClose(eAss);

  return 1;
}

//~----------------------------------------------------------------------------

int eAssLogPrintf(peAssIntf eAss, char *fmt, ...) {

  int msgtype;
  peAssTag  msg;
  int16 Action = 0;
  int rtn;
  va_list arg_marker;
  char txtmsg[128];

  if (!fmt) return 0;

  va_start(arg_marker, fmt);
  vsnprintf(txtmsg, sizeof txtmsg - 2, fmt, arg_marker);
  va_end(arg_marker);

  rtn = strlen(txtmsg);
  printf("eAssLogPrintf: %s", txtmsg);
  if (txtmsg[rtn-1] != '\n') printf("\n");

  msg = eAssTag(NULL,  "STRING", "Log", txtmsg, -1);
        eAssTag(msg,   "INT16",  "ACTION", &Action, sizeof (int16));
  if (!eAssSend(eAss, eAss_MT_LOGBOOK, msg)) {
    printf("eAssLogPrintf: eAssSend failed\n");
    eAssRelease(msg);
    return -1;
  }
  eAssRelease(msg);

  msg = eAssRecv(eAss, 2000, &msgtype);
  if (msg) eAssRelease(msg);

  return rtn;
}

//~----------------------------------------------------------------------------

int eAssLogPrintfAndAbortServer(peAssIntf eAss, char *fmt, ...) {

  int msgtype;
  peAssTag  msg;
  int16 Action = 86;
  int rtn;
  va_list arg_marker;
  char txtmsg[128];

  if (!fmt) return 0;

  va_start(arg_marker, fmt);
  vsnprintf(txtmsg, sizeof txtmsg - 2, fmt, arg_marker);
  va_end(arg_marker);

  rtn = strlen(txtmsg);
  printf("eAssLogPrintf: %s", txtmsg);
  if (txtmsg[rtn-1] != '\n')
    printf("\n");

  msg = eAssTag(NULL,  "STRING", "LogAndAbort", txtmsg, -1);
        eAssTag(msg,   "INT16",  "ACTION", &Action, sizeof (int16));
  if (!eAssSend(eAss, eAss_MT_LOGBOOK, msg)) {
    eAssRelease(msg);
    return -1;
  }
  eAssRelease(msg);

  msg = eAssRecv(eAss, 2000, &msgtype);
  if (msg) eAssRelease(msg);

  return strlen(txtmsg);
}

//~============================================================================

