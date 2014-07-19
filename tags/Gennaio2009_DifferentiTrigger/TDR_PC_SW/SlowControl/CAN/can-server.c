// file can-server.c
//
// Stolen from Peng YANG & Jinghui Zhang (program PC.C ao) and reworked
//
// A.Lebedev, Apr-2007...
//

#include "canlib.h"
#include "tcplib.h"
#include "gsclib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x192;    // 1xx stands for Newborn ID
int P = 0;

static struct timeval tv;
static fd_set rd, wr, er;
static int s1, s2;

static int8 Request_Packet[4000000];
int Request_Packet_Length;
static int8 Reply_Packet[4000000];
int Reply_Packet_Length;
static AMSBlock Request, *pRequest = &Request;
static AMSBlock Reply,   *pReply   = &Reply;
int8 RQ_dat[1000000], RP_dat[sizeof(RQ_dat)];;

static struct timeval time_now, time_then;
int32 blocks_in_file;

//~----------------------------------------------------------------------------

void setup_socket_and_listen(void) {

  int listen_port  = CS_port;
  int queue_length = LISTEN_QUEUE_LENGTH;
  int r;
  int one = 1;
  struct sockaddr_in pc_addr;

  s1 = socket(AF_INET, SOCK_STREAM, 0);
  if (s1 < 0) QUIT("socket");

  if (fcntl(s1, F_SETFL, O_NDELAY)   == -1) QUIT("fcntl(...O_NDELAY)");
  if (fcntl(s1, F_SETFL, O_NONBLOCK) == -1) QUIT("fcntl(...O_NONBLOCK)");

  r = setsockopt(s1, SOL_SOCKET, SO_REUSEADDR, (void *)&one, sizeof(one));
  if (r == -1) QUIT("setsockop");
  
  bzero(&pc_addr, sizeof(pc_addr));
  pc_addr.sin_family      = AF_INET;
  pc_addr.sin_port        = htons(listen_port);
  pc_addr.sin_addr.s_addr = htons(INADDR_ANY);

  if (bind(s1, (struct sockaddr*)&pc_addr, sizeof(pc_addr)) == -1) QUIT("bind");
  
  if (listen(s1, queue_length) == -1) QUIT("listen");
}

//~----------------------------------------------------------------------------

bool receive_request(void) {

  int i;
  int status;
  struct pollfd ufd;
  int timeout = 2; // in msec
  struct sockaddr_in mp_addr;
  socklen_t length = sizeof(mp_addr);
  int r;
  int n_rcv;

//~--

  ufd.fd     = s1;
  ufd.events = POLLIN;
  if ((status = poll(&ufd, 1, timeout)) == -1) QUIT("poll");

  if (status == 0) return FALSE;

//~--

  if ((s2 = accept(s1, (struct sockaddr*)&mp_addr, &length)) < 0) QUIT("accept");

  FD_SET(s2, &rd);
  FD_SET(s2, &wr);
  FD_SET(s2, &er);
  tv.tv_sec  = CONNECT_TIME_OUT;
  tv.tv_usec = 0;
  if ((r = select(s2+1, &rd, NULL, &er, &tv)) == 0) PANIC("select 1: Time out");
  if (r ==  -1) QUIT("select 2");
  if (P > 0) {
    printf("after accept:\n");
    printf("FD_ISSET(s, &rd) = %d\n", FD_ISSET(s2, &rd));
    printf("FD_ISSET(s, &wr) is not checked here\n");
    printf("FD_ISSET(s, &er) = %d\n", FD_ISSET(s2, &er));
  }

  if (!FD_ISSET(s2, &rd)) PANIC("select 3: Time out");
      
//~--

  r = read(s2, Request_Packet, 1000000);

  if (r ==  0) {
    printf("read 1: Client closed connection\n");
    close(s2);
    s2 = -1;
    return FALSE;
  }

  if (r == -1) {
    printf("read 2: %s\n", strerror(errno));
    close(s2);
    s2 = -1;
    return FALSE;
  }

  n_rcv = r;

  get_Packet_Length(&Request_Packet[0], &Request_Packet_Length);
  if (P > 0) printf("read : n_rcv = %d, Request_Packet_Length = %d\n", 
                    n_rcv, Request_Packet_Length);

  while (n_rcv != Request_Packet_Length) {
    r = read(s2, Request_Packet+n_rcv, 1000000);
    if (r ==  0) {
      printf("read 3: Client closed connection\n");
      close(s2);
      s2 = -1;
      break;
    }
    if (r == -1) {
      printf("read 4: %s\n", strerror(errno));
      close(s2);
      s2 = -1;
      break;
    }
    if (P > 0) printf("read again, r = %d\n", r);
    n_rcv = n_rcv + r;
  }

  if (s2 < 0) return FALSE;

  if (P > 0) {
    int i;
    int8 ip[4];
    for (i=0; i<4; i++) ip[i] = mp_addr.sin_addr.s_addr >> (8 * i);
    printf("Received %d bytes from client %d.%d.%d.%d\n",
           n_rcv, ip[0], ip[1], ip[2], ip[3]);
  }

  if (P > 0) {
    for (i=0; i<MIN(20,Request_Packet_Length); i++) {
      printf("%02hX\n", Request_Packet[i]);
    }
  }
   //~--

  pRequest->BufferSize = sizeof(RQ_dat);
  pRequest->Data.p8    = RQ_dat;
  pRequest->Data.p16   = NULL;
    
  unpack8_AMS_Block(Request_Packet, pRequest);
  pack16(&pRequest->Source, 0xFF00, JMDC_ID);
    
  if (P > 1) print_AMS_data_block("REQUEST:", pRequest);

  pReply->Error       = 0x0000;
  pReply->BufferSize  = sizeof(RP_dat);
  pReply->Data.p8     = RP_dat;
  pReply->Data.p16    = NULL;
  pReply->BlockType   = 0x0002 | (pRequest->BlockType & 0x0001);
  pReply->NodeAddress = pRequest->NodeAddress;
  pReply->DataType    = pRequest->DataType;
  pReply->DataCount   = 0;
  pReply->Source      = pRequest->Source;

  return TRUE;
}

//~----------------------------------------------------------------------------

void execute_request(void) {

  if (pRequest->NodeAddress != 0x1FF) {
    CANStateMachine(START, pRequest, pReply);
    if (!pReply->Error) while (CANStateMachine(CONTINUE, pRequest, pReply) != DONE);
  }
  else {
    pReply->BlockType   = 0x0002 | (pRequest->BlockType & 0x0001);
    pReply->NodeAddress = pRequest->NodeAddress;
    pReply->DataType    = pRequest->DataType;
    pReply->Source      = pRequest->Source;
    pReply->DataCount   = pRequest->DataCount;
    memcpy(pReply->Data.p8, pRequest->Data.p8, pReply->DataCount);
//  introduce the error...
//  pReply->Data.p8[pReply->DataCount-1] ^= 0xFF;
  }
  
  pReply->DataTime = time(NULL);

  if (P > 1) print_AMS_data_block("REPLY:  ", pReply);
}

//~----------------------------------------------------------------------------

bool send_reply(void) {

  int i;
  int r, q;
  int n_snd;

//~--- pack the reply packet to be sent to client

  pack8_AMS_Block(pReply, Reply_Packet);
  get_Packet_Length(Reply_Packet, &Reply_Packet_Length);
    
  if (P > 3) {
    printf("Reply_Packet: ");
    for (i=0; i<MIN(20,Reply_Packet_Length); i++) {
      printf("%02hhX ", Reply_Packet[i]);
    }
    if (Reply_Packet_Length >= 20) printf("...");
    printf("\n");
  }

//~--- send the reply packet to the client

  r = write(s2, Reply_Packet, Reply_Packet_Length);

  if (r == -1) {
    printf("write: %s\n", strerror(errno));
    close(s2);
    s2 = -1;
    return FALSE;
  }

  n_snd = r;
  if (n_snd != Reply_Packet_Length) {
    printf("write: n_snd = %d, Reply_Packet_Length = %d\n", 
            n_snd, Reply_Packet_Length);
  }
  if (P > 3) printf("Sent %d bytes to client\n", n_snd);

  FD_SET(s2, &rd);
  FD_SET(s2, &wr);
  FD_SET(s2, &er);
  tv.tv_sec  = CONNECT_TIME_OUT;
  tv.tv_usec = 0;

  r = select(s2+1, &rd, NULL, &er, &tv);
  if (r ==  0) PANIC("select 4: Time out");
  if (r == -1) QUIT("select 5");

  if (P > 4) {
    printf("after write:\n");
    printf("FD_ISSET(s, &rd) = %d\n", FD_ISSET(s2, &rd));
    printf("FD_ISSET(s, &wr) is not checked here\n");
    printf("FD_ISSET(s, &er) = %d\n", FD_ISSET(s2, &er));
  }

  if (!FD_ISSET(s2, &rd)) PANIC("select 6: Time out");

  r = read(s2, (int8*)&q, 1);
  if (r ==  0) {
    if (P > 3) printf("read 5: Client closed connection\n");
    close(s2);
    s2 = -1;
    return FALSE;
  }

  if (r == -1) {
    if (P > 0) printf("read 6: %s\n", strerror(errno));
    close(s2);
    s2 = -1;
    return FALSE;
  }
  if (P > 3) printf("r = %d\n", r);

  return TRUE;
}

//~----------------------------------------------------------------------------

void write_reply_in(FILE *block_file) {

  int w_cnt = (Reply_Packet_Length - 2 + 1) / 2 * 2;
  
  swap16((int16*)Reply_Packet, (Reply_Packet_Length+1)/2);
  if (P > 5) {
    int i;
    printf("Writing File: Reply_Packet_Length = %d \n", Reply_Packet_Length);
    printf("Reply_Packet: ");
    for (i=0; i<Reply_Packet_Length; i++) printf("%02hhX ", Reply_Packet[i]);
    printf("\n");
  }
  if (fwrite(&Reply_Packet[2], 1, w_cnt, block_file) != w_cnt) PANIC("Disk full.");
}

//~----------------------------------------------------------------------------

void user_interface(void) {

  if (kbhit()) {
    int ch = fgetc(stdin);
    switch (ch) {
      case 'p': P = MAX( 0, P-1); printf("Now P = %d\n", P); break;
      case 'P': P = MIN(10, P+1); printf("Now P = %d\n", P); break;
      case 'q':
      case 'Q':
        exit(0);
      break;
    }
  }
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int k;
  int file_number;
  char *block_directory;
  FILE *block_file = NULL;

//~--

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s <Block Directory> [JMDC_ID [P]]\n", argv[0]);

  if (argc < 2) PANIC("Too few parameters");

  k = 1;
  block_directory = argv[k++];
  remove_trailing_slash(block_directory);
  if (argc > k) JMDC_ID = atoi(argv[k++]);
  if (argc > k) P       = atoi(argv[k++]);

  printf("Block Directory = %s\n", block_directory);
  printf("JMDC ID         = %d\n", JMDC_ID);
  printf("P               = %d\n", P);

//~--

  file_number = the_last_file_in(block_directory);
  if (file_number) file_number++;
  printf("First block file to write = %s/%04d/%03d\n",
         block_directory, file_number/1000, file_number%1000);
  block_file = open_temp_file_for_writing(block_directory, 0);
  blocks_in_file = 0;
  if (!block_file) {
    printf("open_temp_file_for_writing(1) failed.\n");
    exit(1);
  }
  printf("%04d/%03d... \n", file_number/1000, file_number%1000);
  if (P) {
    printf("File %s/%04d/%03d opened. \n", 
           block_directory, file_number/1000, file_number%1000);
  }
  gettimeofday(&time_then, NULL);

//~--

  set_CAN_timeout(1.0);
  CS_port = CAN_SERVER_PORT;
  
  setup_socket_and_listen();
  while (1) {
    if (receive_request()) {
      execute_request();
      send_reply();
      write_reply_in(block_file);
      blocks_in_file++;
    }
    gettimeofday(&time_now, NULL);
    if ((delta_t(&time_now, &time_then) > 10.0)) {
      fclose(block_file);
      if (P) {
        printf("File %s/%04d/%03d closed, ", 
               block_directory, file_number/1000, file_number%1000);
        printf("%d blocks written\n", blocks_in_file);
      }
      rename_temp_file(block_directory, 0, file_number);
      if (blocks_in_file) file_number++;
      block_file = open_temp_file_for_writing(block_directory, 0);
      if (!block_file) {
        printf("open_temp_file_for_writing(2) failed.\n");
        exit(1);
      }
      else {
        printf("%04d/%03d... \n", file_number/1000, file_number%1000);
        blocks_in_file = 0;
      }
      gettimeofday(&time_then, NULL);
    }
    user_interface();
  }
  return 0;
}

//~============================================================================
