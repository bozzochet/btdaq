// file tcplib.c
//
// Stolen from Jinghui Zhang and reworked
// JZ corrections (14.05.2007) included.
//
// A.Lebedev Apr-2007...
//

//~----------------------------------------------------------------------------

#include "tcplib.h"

static struct timeval tv;
static fd_set rd, wr, er;
static int s;

//~============================================================================

void pack8_AMS_Block(pAMSBlock pBlock, int8 *Packet) {

  int k;
  int32 Block_Primary_Header;
  
  Block_Primary_Header = 0x00000000;
  pack32(&Block_Primary_Header, 0xC0000000, pBlock->BlockType);
  pack32(&Block_Primary_Header, 0x3FE00000, pBlock->NodeAddress);

  if ((pBlock->DataType & 0x001F0000) != 0x001F0000) {
    pack32(&Block_Primary_Header, 0x001F0000, pBlock->DataType);
    Block_Primary_Header >>= 16;
    k = 2;
  }
  else {
    pack32(&Block_Primary_Header, 0x001FFFFF, pBlock->DataType);
    k = 4;
  }

  pBlock->Length = k + 6 + pBlock->DataCount;

  unpackbytes(&Packet[0], pBlock->Length, 4);
  unpackbytes(&Packet[4], Block_Primary_Header, k);
  pBlock->CommandTag = pBlock->CommandTag & 0x0FFF;
  if (pBlock->Error) pBlock->CommandTag = pBlock->CommandTag | 0xF000;
  unpackbytes(&Packet[4+k], pBlock->CommandTag, 2);
  unpackbytes(&Packet[6+k], pBlock->DataTime, 4);
  
  if (pBlock->DataCount) {
    int i;
    if      (pBlock->Data.p8) {
      for (i=0; i<pBlock->DataCount; i++) {
        unpackbytes(&Packet[10+k+i], pBlock->Data.p8[i], 1);
      }
    }
    else if (pBlock->Data.p16) {
      for (i=0; i<(pBlock->DataCount+1)/2; i++) {
        unpackbytes(&Packet[10+k+2*i], pBlock->Data.p16[i], 2);
      }
    }
  }
}

//~----------------------------------------------------------------------------

void unpack8_AMS_Block(int8 *Packet, pAMSBlock pBlock) {

  int k;
  int16 error;
  int32 Block_Primary_Header;
  
  packbytes(&pBlock->Length, &Packet[0], 4);
  packbytes(&Block_Primary_Header, &Packet[4], 4);
  
  pBlock->BlockType   = unpack32(Block_Primary_Header, 0xC0000000);
  pBlock->NodeAddress = unpack32(Block_Primary_Header, 0x3FE00000);
  if ((Block_Primary_Header & 0x001F0000) != 0x001F0000) {
    pBlock->DataType = unpack32(Block_Primary_Header, 0x001F0000);
    k = 2;
  }
  else {
    pBlock->DataType = unpack32(Block_Primary_Header, 0x001FFFFF);
    k = 4;
  }
  pBlock->DataCount = pBlock->Length - k - 6;
  
  packbytes((int32*)&pBlock->CommandTag, &Packet[4+k], 2);
  if (P > 1) printf("pBlock->CommandTag = %04X\n", pBlock->CommandTag);
  packbytes(&pBlock->DataTime,   &Packet[6+k], 4);
  if (P > 1) printf("pBlock->DataTime   = %08X\n", pBlock->DataTime);

  error = unpack16(pBlock->CommandTag, 0xF000);
  if (error) {
    pBlock->Error = 0x1300 + error;
    return;
  }

//printf("pBlock->BufferSize=%d pBlock->DataCount=%d\n", pBlock->BufferSize, pBlock->DataCount);
  if (pBlock->BufferSize < pBlock->DataCount) {
//printf("ERROR 0x1310\n");
    pBlock->Error = 0x1310;
    return;
  }

  if (pBlock->DataCount) {
    int i;
    if      (pBlock->Data.p8) {
      for (i=0; i<pBlock->DataCount; i++) {
        pBlock->Data.p8[i] = Packet[10+k+i];
      }
      if (P > 1) print_AMS_data_block("BLOCK:  ", pBlock);
    }
    else if (pBlock->Data.p16) {
      for (i=0; i<(pBlock->DataCount+1)/2; i++) {
        pBlock->Data.p16[i] = (Packet[10+k+2*i] << 8) | Packet[10+k+2*i+1];
      }
      if (P > 1) print_AMS_data_block("BLOCK:  ", pBlock);
    }
  }
}

//~----------------------------------------------------------------------------

void get_Packet_Length(int8 *Packet, int *Packet_Length) {

  int32 Block_Length;

  packbytes(&Block_Length, &Packet[0], 4);
  *Packet_Length = Block_Length + 4;
}

//~============================================================================

void pack8_AMS_Block_into_Peter_TCP_frame(pAMSBlock pBlock, int8 *Packet) {

  int k;
  int32 Block_Primary_Header;
  time_t now;
  
  now = time(NULL);

  Block_Primary_Header = 0x00000000;
  pack32(&Block_Primary_Header, 0xC0000000, pBlock->BlockType);
  pack32(&Block_Primary_Header, 0x3FE00000, pBlock->NodeAddress);

  if ((pBlock->DataType & 0x001F0000) != 0x001F0000) {
    pack32(&Block_Primary_Header, 0x001F0000, pBlock->DataType);
    Block_Primary_Header >>= 16;
    k = 2;
  }
  else {
    pack32(&Block_Primary_Header, 0x001FFFFF, pBlock->DataType);
    k = 4;
  }
  if (P > 2) printf("Block_Primary_Header = %08X\n", Block_Primary_Header);

  pBlock->Length = k + 2 + pBlock->DataCount;

  unpackbytes(&Packet[ 0], 0x1800+use_APID, 2);
  unpackbytes(&Packet[ 2], 0xC000, 2);
  unpackbytes(&Packet[ 4], 0x03F9, 2);
  unpackbytes(&Packet[ 6], now,    4);
  unpackbytes(&Packet[10], 0x0046, 2);
  unpackbytes(&Packet[12], 0x1111, 2);
  unpackbytes(&Packet[14], 0x2222, 2);
  unpackbytes(&Packet[16], 0x0000, 2);
  unpackbytes(&Packet[18], pBlock->Length, 2);
  unpackbytes(&Packet[20], Block_Primary_Header, k);
  unpackbytes(&Packet[20+k], pBlock->CommandTag, 2);

//printf("pack...: pBlock->Data.p8[0]=%02X...", pBlock->Data.p8[0]);
  if (pBlock->DataCount) {
    int i;
    if      (pBlock->Data.p8) {
      for (i=0; i<pBlock->DataCount; i++) {
        unpackbytes(&Packet[22+k+i], pBlock->Data.p8[i], 1);
      }
    }
    else if (pBlock->Data.p16) {
      for (i=0; i<(pBlock->DataCount+1)/2; i++) {
        unpackbytes(&Packet[22+k+2*i], pBlock->Data.p16[i], 2);
      }
    }
  }

  memset(&Packet[22+k+(pBlock->DataCount+1)/2*2], 0x00, 2);

//printf("pBlock->Data.p8[0]=%02X\n", pBlock->Data.p8[0]);
}

//~----------------------------------------------------------------------------

void unpack8_AMS_Block_from_Peter_TCP_frame(int8 *Packet, pAMSBlock pBlock) {

  int k;
  int16 error;
  int32 Block_Primary_Header;

  packbytes(&pBlock->Length, &Packet[18], 2);
  packbytes(&Block_Primary_Header, &Packet[20], 4);

  pBlock->BlockType   = unpack32(Block_Primary_Header, 0xC0000000);
  pBlock->NodeAddress = unpack32(Block_Primary_Header, 0x3FE00000);
  if ((Block_Primary_Header & 0x001F0000) != 0x001F0000) {
    pBlock->DataType = unpack32(Block_Primary_Header, 0x001F0000);
    k = 2;
  }
  else {
    pBlock->DataType = unpack32(Block_Primary_Header, 0x001FFFFF);
    k = 4;
  }

  if (pBlock->Length < (k + 6)) {
    pBlock->DataCount = 0;
    if (P > 1) printf("Secondary Header is missing in Reply.\n");
    pBlock->Error = 0x1311;
    return;
  }

  pBlock->DataCount = pBlock->Length - k - 6;
  packbytes((int32*)&pBlock->CommandTag, &Packet[20+k], 2);
  if (P > 1) printf("pBlock->CommandTag = %04X\n", pBlock->CommandTag);
  packbytes(&pBlock->DataTime,   &Packet[22+k], 4);
  if (P > 1) printf("pBlock->DataTime   = %08X\n", pBlock->DataTime);

  error = unpack16(pBlock->CommandTag, 0xF000);
  if (error) {
    pBlock->Error = 0x1300 + error;
    return;
  }

//printf("unpack8_AMS_Block_from_Peter_TCP_frame:\n");
//printf("pBlock->BufferSize = %d\n", pBlock->BufferSize);
//printf("pBlock->DataCount  = %d\n", pBlock->DataCount );
  if (pBlock->BufferSize < pBlock->DataCount) {
    pBlock->Error = 0x1310;
    return;
  }

  if (pBlock->DataCount) {
    int i;
    if      (pBlock->Data.p8) {
      for (i=0; i<pBlock->DataCount; i++) {
        pBlock->Data.p8[i] = Packet[26+k+i];
      }
      if (P > 1) print_AMS_data_block("BLOCK:  ", pBlock);
    }
    else if (pBlock->Data.p16) {
      for (i=0; i<(pBlock->DataCount+1)/2; i++) {
        pBlock->Data.p16[i] = (Packet[26+k+2*i] << 8) | Packet[26+k+2*i+1];
      }
      if (P > 1) print_AMS_data_block("BLOCK:  ", pBlock);
    }
  }
}

//~============================================================================

void TCP_SND_RCV(pAMSBlock pRequest, pAMSBlock pReply) { 

  int r;
  int n_snd, n_rcv;
  static int8 Request_Packet[4000000];
  int Request_Packet_Length;
  int Reply_Packet_Length;
  static int8 Reply_Packet[4000000];
  struct timeval now;

//~--- prepare the request packet (pack AMS Block into TCP packet)---

  gettimeofday(&now, NULL);
  pRequest->CommandTag = now.tv_usec & 0x0FFF;

  if (use_Peter_TCP) {
    Request_Packet_Length = PETER_TCP_PACKET_LENGTH;
    pack8_AMS_Block_into_Peter_TCP_frame(pRequest, Request_Packet);
  }
  else {
    pack8_AMS_Block(pRequest, Request_Packet);
    get_Packet_Length(Request_Packet, &Request_Packet_Length);
  }

//~--- send the request packet ---

  FD_SET(s, &rd);
  FD_SET(s, &wr);
  FD_SET(s, &er);
  tv.tv_sec = CONNECT_TIME_OUT;
  tv.tv_usec = 0;
  r = select(s+1, &rd, &wr, &er, &tv);
  if (r ==  0) {
    pReply->Error = 0x1201;
    printf("*** select: Time out, err = 0x%04X\n", pReply->Error);
    return;
  }
  if (r == -1) {
    pReply->Error = 0x1202;
    printf("*** select: %s, err = 0x%04X\n", strerror(errno), pReply->Error);
    return;
  }
  if (P > 4) {
    printf("before write:\n");
    printf("FD_ISSET(s, &rd) = %d\n", FD_ISSET(s, &rd));
    printf("FD_ISSET(s, &wr) = %d\n", FD_ISSET(s, &wr));
    printf("FD_ISSET(s, &er) = %d\n", FD_ISSET(s, &er));
  }
  if ( FD_ISSET(s, &rd)) {
    pReply->Error = 0x1203;
    printf("*** select: Ready to read(?), err = 0x%04X\n", pReply->Error);
    return;
  }
  if (!FD_ISSET(s, &wr)) {
    pReply->Error = 0x1204;
    printf("*** select: Not ready to write, err = 0x%04X\n", pReply->Error);
    return;
  }
  if ( FD_ISSET(s, &er)) {
    pReply->Error = 0x1205;
    printf("*** select: Exception happened, err = 0x%04X\n", pReply->Error);
    return;
  }

  errno = 0;
  r = write(s, Request_Packet, Request_Packet_Length);
  if (r == -1) {
    pReply->Error = 0x1206;
    printf("*** write: %s, err = 0x%04X\n", strerror(errno), pReply->Error);
    return;
  }
  n_snd = r;
  if (n_snd != Request_Packet_Length) {
    printf("write: n_snd = %d, Request_Packet_Length = %d\n",
            n_snd, Request_Packet_Length);
  }
  if (P > 2) printf("Sent %d bytes to server\n", n_snd);
  if (P > 3) {
    int i;
    for (i=0; i<n_snd; i++) {
      printf("%02X ", Request_Packet[i]);
    }
    printf("\n");
  }
	    
  FD_SET(s, &wr);
  FD_SET(s, &er);
  tv.tv_sec = CONNECT_TIME_OUT;
  tv.tv_usec = 0;
  r = select(s+1, NULL, &wr, &er, &tv);
  if (r ==  0) {
    pReply->Error = 0x1207;
    printf("*** select: Time out, err = 0x%04X\n", pReply->Error);
    return;
  }
  if (r == -1) {
    pReply->Error = 0x1208;
    printf("*** select: %s, err = 0x%04X\n", strerror(errno), pReply->Error);
    return;
  }
  if (P > 4) {
    printf("after write:\n");
    printf("FD_ISSET(s, &rd) is not checked here\n");
    printf("FD_ISSET(s, &wr) = %d\n", FD_ISSET(s, &wr));
    printf("FD_ISSET(s, &er) = %d\n", FD_ISSET(s, &er));
  }
  if (!FD_ISSET(s, &wr)) {
    pReply->Error = 0x1209;
    printf("*** select: Not ready to write, err = 0x%04X\n", pReply->Error);
    return;
  }
  if ( FD_ISSET(s, &er)) {
    pReply->Error = 0x120A;
    printf("*** select: Exception happened, err = 0x%04X\n", pReply->Error);
    return;
  }

//~--- receive the reply packet ---

  bzero(Reply_Packet, sizeof(Reply_Packet));

  timer(0, START);
  while (1) {
    if (timer(0, EXPIRED, 6.0)) {
      pReply->Error = 0x120B;
      printf("*** timer: Time out, err = 0x%04X\n", pReply->Error);
      return;
    }
    n_rcv = 0;
    Reply_Packet_Length = 4;
    pReply->Error = 0x0000;
    while (n_rcv < Reply_Packet_Length) {

      FD_SET(s, &rd);
      FD_SET(s, &er);
      tv.tv_sec = 3;
      tv.tv_usec = 0;
      if (P > 4) {
        printf("before select\n");
      }

      r = select(s+1, &rd, NULL, &er, &tv);
      if (r ==  0) {
        pReply->Error = 0x120B;
        printf("*** select: Time out, err = 0x%04X\n", pReply->Error);
        return;
      }
      if (r == -1) {
        pReply->Error = 0x120C;
        printf("*** select: %s, err = 0x%04X\n", strerror(errno), pReply->Error);
        return;
      }
      if (P > 4) {
        printf("before read:\n");
        printf("FD_ISSET(s, &rd) = %d\n", FD_ISSET(s, &rd));
        printf("FD_ISSET(s, &wr) is not checked here\n");
        printf("FD_ISSET(s, &er) = %d\n", FD_ISSET(s, &er));
      }
      if (!FD_ISSET(s, &rd)) {
        pReply->Error = 0x120D;
        printf("*** select: Not ready to read, err = 0x%04X\n", pReply->Error);
        return;
      }
      if ( FD_ISSET(s, &er)) {
        pReply->Error = 0x120E;
        printf("*** select: Exception happened, err = 0x%04X\n", pReply->Error);
        return;
      }

      if (P > 3) printf("before read : n_rcv = %d, want_to_read = %d\n",
                         n_rcv, Reply_Packet_Length-n_rcv);
      errno = 0;
      r = read(s, Reply_Packet+n_rcv, Reply_Packet_Length-n_rcv);
      if (r ==  0) {
        pReply->Error = 0x120F;
        printf("*** read: Server closed connection, err = 0x%04X\n", pReply->Error);
        return;
      }
      if (r == -1) {
        pReply->Error = 0x1210;
        printf("*** read: %s, err = 0x%04X\n", strerror(errno), pReply->Error);
        return;
      }
      if (P > 3) printf(" after read : n_rcv = %d, r = %d\n", n_rcv, r);
      if (!n_rcv && r >= 4) {
        if (!use_Peter_TCP) get_Packet_Length(&Reply_Packet[0], &Reply_Packet_Length);
        else                Reply_Packet_Length = PETER_TCP_PACKET_LENGTH;
      }
      n_rcv = n_rcv + r;
    }
    if (P > 2) printf("Received %d bytes from server\n", n_rcv);
    if (P > 3) {
      int i;
      for (i=0; i<n_rcv; i++) {
        printf("%02X ", Reply_Packet[i]);
      }
      printf("\n");
    }
    Reply_Packet_Length = n_rcv;
    if (use_Peter_TCP) {
      unpack8_AMS_Block_from_Peter_TCP_frame(Reply_Packet, pReply);
//printf("diagnostics:\n");
//printf("CommandTag:  RQ = %04X,   RP = %04X\n", pRequest->CommandTag, pReply->CommandTag);
//printf("NodeAddress: RQ = %03X,    RP = %03X\n", pRequest->NodeAddress, pReply->NodeAddress);
//printf("DataType:    RQ = %06X, RP = %06X\n", pRequest->DataType, pReply->DataType);
      if (pReply->Error) break;
      if ((pRequest->CommandTag  == (pReply->CommandTag & 0x0FFF)) &&
          (pRequest->NodeAddress ==  pReply->NodeAddress)          &&
          (pRequest->DataType    ==  pReply->DataType   )) {
        break;
      }
    }
    else {
      unpack8_AMS_Block(Reply_Packet, pReply);
      break;
    }
  }
  
}

//~============================================================================

#if 0
void open_TCP_connection_old(char *CS_name, int CS_port, int16 *err) {

  struct sockaddr_in client_adr;
  struct sockaddr_in server_adr;           
  struct hostent *h;
  int one = 1;
//struct linger lng = {1, 0};

  struct linger lng = {0, 0};
  int r, len, flags;

//~---

  *err = 0x0000;

  FD_ZERO(&rd);
  FD_ZERO(&wr);
  FD_ZERO(&er);

//~--- setup the socket ---

  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) {
    *err = 0x1101;
    printf("*** socket: %s, err = 0x%04X\n", strerror(errno), *err);
    return;
  }
		
  r = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (void *)&one, sizeof(one));
  if (r == -1) {
    *err = 0x1102;
    printf("*** setsockopt: %s, err = 0x%04X\n", strerror(errno), *err);
    return;
  }

  r = setsockopt(s, SOL_SOCKET, SO_LINGER, (void *)&lng, sizeof(lng));
  if (r == -1) {
    *err = 0x1103;
    printf("*** setsockopt: %s, err = 0x%04X\n", strerror(errno), *err);
    return;
  }

  bzero(&client_adr, sizeof(client_adr));
  client_adr.sin_family      = AF_INET;
  client_adr.sin_addr.s_addr = htons(INADDR_ANY);
  client_adr.sin_port        = 0;

  r = bind(s, (struct sockaddr*)&client_adr, sizeof(client_adr));
  if (r == -1) {
    *err = 0x1104;
    printf("*** bind: %s, err = 0x%04X\n", strerror(errno), *err);
    return;
  }

  flags = fcntl(s, F_GETFL, 0);          // ???
//printf("flags = 0x%08X, O_NONBLOCK = 0x%08X\n", flags, O_NONBLOCK);
  if (flags == -1) {
    *err = 0x1105;
    printf("*** fcntl: %s, err = 0x%04X\n", strerror(errno), *err);
    return;
  }
  r = fcntl(s, F_SETFL, O_NONBLOCK);
  if (r == -1) {
    *err = 0x1106;
    printf("*** fcntl: %s, err = 0x%04X\n", strerror(errno), *err);
    return;
  }

//~--- connect to the server ---

  h = gethostbyname(CS_name);
  if (!h) {
    printf("gethostbyname: %s\n", hstrerror(h_errno));
    exit(1);
  }
  bzero(&server_adr, sizeof(server_adr));
  server_adr.sin_family = AF_INET;
  memcpy(&server_adr.sin_addr, h->h_addr_list[0], h->h_length);
  server_adr.sin_port   = htons(CS_port);

//printf("%s \n", inet_ntoa(*(struct in_addr*)(h->h_addr_list[0])));

  r = connect(s, (struct sockaddr*)&server_adr, sizeof(server_adr));
  if ((r == -1) && (errno != EINPROGRESS)) {
    *err = 0x1107;
    printf("*** connect: %s, err = 0x%04X\n", strerror(errno), *err);
    return;
  }

  FD_SET(s, &rd);
  FD_SET(s, &wr);
  FD_SET(s, &er);
  tv.tv_sec  = CONNECT_TIME_OUT;
  tv.tv_usec = 0;
  r = select(s+1, &rd, &wr, &er, &tv);
  if (r ==  0) {
    *err = 0x1108;
    printf("*** select: Time out, err = 0x%04X\n", *err);
    return;
  }
  if (r == -1) {
    *err = 0x1109;
    printf("*** select: %s, err = 0x%04X\n", strerror(errno), *err);
    return;
  }
  if (P > 4) {
    printf("after connect:\n");
    printf("FD_ISSET(s, &rd) = %d\n", FD_ISSET(s, &rd));
    printf("FD_ISSET(s, &wr) = %d\n", FD_ISSET(s, &wr));
    printf("FD_ISSET(s, &er) = %d\n", FD_ISSET(s, &er));
  }
  if ( FD_ISSET(s, &rd)) {
    *err = 0x110A;
    printf("*** select: Ready to read(?), err = 0x%04X\n", *err);
    return;
  }
  if (!FD_ISSET(s, &wr)) {
    *err = 0x110B;
    printf("*** select: Not ready to write, err = 0x%04X\n", *err);
    return;
  }
  if ( FD_ISSET(s, &er)) {
    *err = 0x110C;
    printf("*** select: Exception happened, err = 0x%04X\n", *err);
    return;
  }

  len = sizeof(errno);
  r = getsockopt(s, SOL_SOCKET, SO_ERROR, &errno, (socklen_t *)&len);
  if (r == -1) {
    *err = 0x110D;
    printf("*** getsockopt: %s, err = 0x%04X\n", strerror(errno), *err);
    return;
  }
  if (errno) {
    *err = 0x110E;
    printf("*** getsockopt: %s, err = 0x%04X\n", strerror(errno), *err);
    return;
  }

  if (P > 2) printf("Connected to server %s port %d\n", CS_name, CS_port);

  r = fcntl(s, F_SETFL, flags);   // ???
  if (r == -1) {
    *err = 0x110F;
    printf("*** fcntl: %s, err = 0x%04X\n", strerror(errno), *err);
    return;
  }
}
#endif

//~----------------------------------------------------------------------------

void open_TCP_connection(char *CS_name, int CS_port, int16 *err) {

  struct sockaddr_in server_adr;           
  struct hostent *h;
  int one = 1;
  int r, len, flags;

//~---

  *err = 0x0000;

  FD_ZERO(&rd);
  FD_ZERO(&wr);
  FD_ZERO(&er);

//~--- setup the socket ---

  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) {
    *err = 0x1101;
    printf("*** socket: %s, err = 0x%04X\n", strerror(errno), *err);
    return;
  }
  r = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (void *)&one, sizeof(one));
  if (r == -1) {
    *err = 0x1102;
    printf("*** setsockopt: %s, err = 0x%04X\n", strerror(errno), *err);
    return;
  }

  flags = fcntl(s, F_GETFL, 0);          // ???
//printf("flags = 0x%08X, O_NONBLOCK = 0x%08X\n", flags, O_NONBLOCK);
  if (flags == -1) {
    *err = 0x1105;
    printf("*** fcntl: %s, err = 0x%04X\n", strerror(errno), *err);
    return;
  }
  r = fcntl(s, F_SETFL, O_NONBLOCK);
  if (r == -1) {
    *err = 0x1106;
    printf("*** fcntl: %s, err = 0x%04X\n", strerror(errno), *err);
    return;
  }

//~--- connect to the server ---

  h = gethostbyname(CS_name);
  if (!h) {
    printf("gethostbyname: %s\n", hstrerror(h_errno));
    exit(1);
  }
  else {
    bzero(&server_adr, sizeof(server_adr));
    server_adr.sin_family = AF_INET;
    memcpy(&server_adr.sin_addr, h->h_addr_list[0], h->h_length);
    server_adr.sin_port   = htons(CS_port);
  }

//printf("%s \n", inet_ntoa(*(struct in_addr*)(h->h_addr_list[0])));

  r = connect(s, (struct sockaddr*)&server_adr, sizeof(server_adr));
  if ((r == -1) && (errno != EINPROGRESS)) {
    *err = 0x1107;
    printf("*** connect %s:%i: %s, err = 0x%04X\n", 
        CS_name, CS_port, strerror(errno), *err);
    return;
  }

  FD_SET(s, &rd);
  FD_SET(s, &wr);
  FD_SET(s, &er);
  tv.tv_sec  = CONNECT_TIME_OUT;
  tv.tv_usec = 0;
  r = select(s+1, &rd, &wr, &er, &tv);
  if (r ==  0) {
    *err = 0x1108;
    printf("*** select: Time out, err = 0x%04X\n", *err);
    return;
  }
  if (r == -1) {
    *err = 0x1109;
    printf("*** select: %s, err = 0x%04X\n", strerror(errno), *err);
    return;
  }
  if (P > 4) {
    printf("after connect:\n");
    printf("FD_ISSET(s, &rd) = %d\n", FD_ISSET(s, &rd));
    printf("FD_ISSET(s, &wr) = %d\n", FD_ISSET(s, &wr));
    printf("FD_ISSET(s, &er) = %d\n", FD_ISSET(s, &er));
  }
  if ( FD_ISSET(s, &rd)) {
    *err = 0x110A;
    printf("*** select: Ready to read(?), err = 0x%04X\n", *err);
    return;
  }
  if (!FD_ISSET(s, &wr)) {
    *err = 0x110B;
    printf("*** select: Not ready to write, err = 0x%04X\n", *err);
    return;
  }
  if ( FD_ISSET(s, &er)) {
    *err = 0x110C;
    printf("*** select: Exception happened, err = 0x%04X\n", *err);
    return;
  }

  len = sizeof(errno);
  r = getsockopt(s, SOL_SOCKET, SO_ERROR, &errno, (socklen_t *)&len);
  if (r == -1) {
    *err = 0x110D;
    printf("*** getsockopt: %s, err = 0x%04X\n", strerror(errno), *err);
    return;
  }
  if (errno) {
    *err = 0x110E;
    printf("*** getsockopt: %s, err = 0x%04X\n", strerror(errno), *err);
    return;
  }

  if (P > 2) printf("Connected to server %s port %d\n", CS_name, CS_port);

  r = fcntl(s, F_SETFL, flags);   // ???
  if (r == -1) {
    *err = 0x110F;
    printf("*** fcntl: %s, err = 0x%04X\n", strerror(errno), *err);
    return;
  }
}

//~----------------------------------------------------------------------------

void close_TCP_connection(void) {

  int r;

  r = shutdown(s, SHUT_RDWR);
  if (r == -1 && (errno != ENOTCONN)) {
    printf("*** shutdown: %s\n", strerror(errno));
  }
  close(s);
  if (P > 2) printf("Disconnected from server\n");
}

//~============================================================================

int get_TCP_socket(void) {

  return s;
}

//~============================================================================

