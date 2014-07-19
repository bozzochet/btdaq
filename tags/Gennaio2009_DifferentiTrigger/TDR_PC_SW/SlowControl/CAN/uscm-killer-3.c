// file uscm-killer-3.c
//
//  test bench to develop multimaster CAN communication 
//  using a single EPP-CAN box
//
//  to test multi-master communications with USCM:
//  - send "syncronized" ECHO command from 4 JMDCs to the same USCM.
//
//  A.Lebedev Oct 2005...
//

#include "canlib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x152;    // 1xx stands for Newborn ID
int USCM2_ID = 0x137;    // 1xx stands for Newborn ID
int P = 0;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

static _CAN_cha CAN0, *pCAN0 = &CAN0;
static _CAN_cha CAN1, *pCAN1 = &CAN1;

static AMSBlock Request[4], *pRequest[4];
static AMSBlock Reply[4],   *pReply[4];

static _CAN_msg SND[4];
static _CAN_msg RCV[4];

static _ID SND_msg_ID[4];

static _channel channel[4], *pchannel[4];

int8 dat[4][8192], datdat[4][8192];

int32 ccc, eee[4];

int n;

bool Debug = FALSE;
float Delay = 0.000050;
float Delay2 = 0.000050;

//~============================================================================

void initialize_CAN_controllers(int16 *err) {

  if (!initialize_EPP()) {
    *err = 0x0004;
  }
  else {
    pCAN0->cha = 0;
    pCAN0->msk = 0x17FFFF00;
    initialize_CAN(pCAN0);
    pCAN1->cha = 1;
    pCAN1->msk = 0x17FFFF00;
    initialize_CAN(pCAN1);
    if (P > 3) printf("JMDC: CAN-controllers initialized\n");
    *err = 0;
  }
}

//~============================================================================

int16 new_CAN_SND_RCV(int action, _channel *pchannel) {

  _ID SND_msg_ID, *pSND_msg_ID = &SND_msg_ID;
  _ID RCV_msg_ID, *pRCV_msg_ID = &RCV_msg_ID;

  float timeout = 1.0;      // sec
  int8 tmp;

//~--

  if (action != CONTINUE) pchannel->state_1 = action;

  switch (pchannel->state_1) {

//~--

    case DONE:
    break;

//~--

    case START:
      unpack_CAN_msg_ID(pchannel->SND->ID, pSND_msg_ID);
      if (pSND_msg_ID->Bus_for_Request) pchannel->SND->CAN = pCAN1;
      else                              pchannel->SND->CAN = pCAN0;
      if (pSND_msg_ID->Bus_for_Reply) pchannel->RCV->CAN = pCAN1;
      else                            pchannel->RCV->CAN = pCAN0;
      pchannel->RCV->ID = pchannel->SND->ID;
      unpack_CAN_msg_ID(pchannel->RCV->ID, pRCV_msg_ID);
      pRCV_msg_ID->Request_Reply = REPLY;
      tmp                        = pRCV_msg_ID->Source;
      pRCV_msg_ID->Source        = pRCV_msg_ID->Destination;
      pRCV_msg_ID->Destination   = tmp;
      pack_CAN_msg_ID(&pchannel->RCV->ID, pRCV_msg_ID);

      start_CAN_RX(pchannel->RCV);

      if (P > 3) printf("JMDC%d STARTed\n", pchannel->timer);
    break;

//~--

    case GO:
      pchannel->c_1 = 0;
      if (P > 2) print_AMS_CAN_msg(pchannel->c_1, "JMDC TX:", NULL, pchannel->SND);
      start_CAN_TX(pchannel->SND);
      timer(pchannel->timer, START);
      pchannel->state_1 = TXING;
    break;

//~--

    case TXING:
      if (CAN_TX_done(pchannel->SND)) {
        timer(pchannel->timer, START);
        pchannel->state_1 = RXING;
      }
      else {
        if (timer(pchannel->timer, EXPIRED, timeout)) {
          stop_CAN_TX(pchannel->SND);
          stop_CAN_RX(pchannel->RCV);
          if (P > 2) printf("%6d JMDC TX: Timeout\n", pchannel->c_1);
          pchannel->RCV->err = 0x0005;
          pchannel->state_1 = DONE;
        }
      }
    break;

//~--

    case RXING:
      if (CAN_RX_done(pchannel->RCV)) {
        if (pchannel->RCV->err) {
          printf("%6d JMDC RX: Error 0x%02X\n", pchannel->c_1, pchannel->RCV->err);
          pchannel->state_1 = DONE;
        }
        else {
          if (P > 2) print_AMS_CAN_msg(pchannel->c_1, "JMDC RX:", NULL, pchannel->RCV);
          pchannel->RCV->err = 0;
          pchannel->state_1 = DONE;
        }
      }
      else {
        if (timer(pchannel->timer, EXPIRED, timeout)) {
          stop_CAN_RX(pchannel->RCV);
          if (P > 2) printf("%6d JMDC RX: Timeout\n", pchannel->c_1);
          pchannel->RCV->err = 0x0006;
          pchannel->state_1 = DONE;
        }
      }
    break;

//~--

    case STOP:
      stop_CAN_TX(pchannel->SND);
      stop_CAN_RX(pchannel->RCV);
      if (P > 3) printf("JMDC: abort!\n");
      pchannel->RCV->err = 0;
      pchannel->state_1 = DONE;
    break;
    
    default:
      printf("***ERROR: state = %d\n", pchannel->state_1);
      exit(1);
    break;

//~--

  } // end-of-switch

//~--

  if (pchannel->state_1 != pchannel->old_state_1) {
    if (P > 3) printf("%6d JMDC%d: state: %d->%d\n",
               pchannel->c_1, 
               pchannel->timer, 
               pchannel->old_state_1, 
               pchannel->state_1);
    pchannel->old_state_1 = pchannel->state_1;
  }

  pchannel->c_1 = pchannel->c_1 + 1;
  
  return pchannel->state_1;
}

//~============================================================================

void new_CANStateMachine(int action, _channel **pchannel) {

#define is_ERR(m)   ((m)->len == 0 && ((m)->ID & 0x60000000) == 0x20000000)
#define is_ABO(m)   ((m)->len == 0 && ((m)->ID & 0x60000000) == 0x40000000)
#define is_NXT(m)   ((m)->len == 0 && ((m)->ID & 0x60000000) == 0x00000000)
#define is_END(m)   ((m)->len == 0 && ((m)->ID & 0x60000000) == 0x60000000)
#define is_last(m)  ((m)->len != 0 && ((m)->ID & 0x20000000))
#define is_first(m) ((m)->len != 0 && ((m)->ID & 0x40000000))

  int i;
  int status;
  int16 cnt;


//~========

for (i=0; i<4; i++) {

//~--------
  
  if (action != CONTINUE) pchannel[i]->state_2 = action;

  switch (pchannel[i]->state_2) {

//~--

    case DONE:
    break;

//~--

    case START:
      pchannel[i]->c_2 = 0;

      pchannel[i]->RP->BlockType    = 0x0002 | (pchannel[i]->RQ->BlockType & 0x0001);
      pchannel[i]->RP->NodeAddress  = pchannel[i]->RQ->NodeAddress;
      pchannel[i]->RP->DataType     = pchannel[i]->RQ->DataType;
      pchannel[i]->RP->DataCount    = 0;
      pchannel[i]->RP->Source       = pchannel[i]->RQ->Source;

      pchannel[i]->SND_msg_ID->First           = 1;
      pchannel[i]->SND_msg_ID->Request_Reply   = pchannel[i]->RQ->BlockType & 0x0002 ? 1 : 0;
      pchannel[i]->SND_msg_ID->Read_Write      = pchannel[i]->RQ->BlockType & 0x0001 ? 1 : 0;
      pchannel[i]->SND_msg_ID->Bus_for_Request = BUS_FOR_REQUEST;
      pchannel[i]->SND_msg_ID->Bus_for_Reply   = BUS_FOR_REPLY;
      pchannel[i]->SND_msg_ID->Newborn         = unpack16(pchannel[i]->RQ->NodeAddress, 0x0100);
      pchannel[i]->SND_msg_ID->Source          = unpack16((int16)pchannel[i]->RQ->Source, 0xFF00);
      pchannel[i]->SND_msg_ID->Destination     = unpack16(pchannel[i]->RQ->NodeAddress, 0x00FF);

      if (pchannel[i]->RQ->DataType & 0x001F0000) {
        if ((pchannel[i]->RQ->DataType & 0x001F0000) == 0x001F0000) {
          pchannel[i]->SND->dat[0] = unpack32(pchannel[i]->RQ->DataType, 0x0000FF00);
          pchannel[i]->SND->dat[1] = unpack32(pchannel[i]->RQ->DataType, 0x000000FF);
          pchannel[i]->SND->len    = 2;
        }
        else {
          pchannel[i]->RP->Error = 0x0010;
          pchannel[i]->state_2 = DONE;
          break;
        }
      }
      else {
        pchannel[i]->SND->dat[0] = unpack32(pchannel[i]->RQ->DataType, 0x0000001F) |
                      (pchannel[i]->RQ->BlockType & 0x0001 ? 0x40 : 0x00);
        pchannel[i]->SND->len    = 1;
      }

      pchannel[i]->bytes_still_to_write = pchannel[i]->RQ->DataCount;

      if (pchannel[i]->bytes_still_to_write && pchannel[i]->RQ->Data.p8 == 0) {
        pchannel[i]->RP->Error = 0x0011;
        pchannel[i]->state_2 = DONE;
        break;
      }

      if (pchannel[i]->bytes_still_to_write) {
        cnt = MIN(8-pchannel[i]->SND->len, pchannel[i]->bytes_still_to_write);
        memcpy((int8*)&pchannel[i]->SND->dat[pchannel[i]->SND->len],
               (int8*)&pchannel[i]->RQ->Data.p8[0], 
                cnt);
        pchannel[i]->SND->len = pchannel[i]->SND->len + cnt;
        pchannel[i]->bytes_still_to_write = pchannel[i]->bytes_still_to_write - cnt;
        if (pchannel[i]->bytes_still_to_write) pchannel[i]->SND_msg_ID->Last = 0;
        else                                pchannel[i]->SND_msg_ID->Last = 1;
      }
      else {
        pchannel[i]->SND_msg_ID->Last = 1;
      }
      pack_CAN_msg_ID(&pchannel[i]->SND->ID, pchannel[i]->SND_msg_ID);
      status = new_CAN_SND_RCV(START, pchannel[i]);
      pchannel[i]->state_2 = WRITING;
      break;

//~--

    case WRITING:
      
      if (pchannel[i]->RCV->err) {
        pchannel[i]->RP->Error = pchannel[i]->RCV->err;
        pchannel[i]->state_2 = DONE;
        break;
      }

      if (pchannel[i]->bytes_still_to_write) {
        if (!is_NXT(pchannel[i]->RCV)) {
          pchannel[i]->RP->Error = 0x0012;
          pchannel[i]->state_2 = DONE;
          break;
        }
        pchannel[i]->SND_msg_ID->First = 0;
        pchannel[i]->SND->len = MIN(8, pchannel[i]->bytes_still_to_write);
        memcpy((int8*)&pchannel[i]->SND->dat[0],
               (int8*)&pchannel[i]->RQ->Data.p8[pchannel[i]->RQ->DataCount -
                pchannel[i]->bytes_still_to_write], 
                pchannel[i]->SND->len);
        pchannel[i]->bytes_still_to_write -= pchannel[i]->SND->len;
        if (pchannel[i]->bytes_still_to_write ) pchannel[i]->SND_msg_ID->Last = 0;
        else                                 pchannel[i]->SND_msg_ID->Last = 1;
        pack_CAN_msg_ID(&pchannel[i]->SND->ID, pchannel[i]->SND_msg_ID);
        status = new_CAN_SND_RCV(START, pchannel[i]);
        break;
      }

      if (is_END(pchannel[i]->RCV)) {
        pchannel[i]->RP->Error = 0;
        pchannel[i]->state_2 = DONE;
        break;
      }

      if (pchannel[i]->RQ->BlockType & 0x0001) {
        pchannel[i]->RP->Error = 0x0013;
        pchannel[i]->state_2 = DONE;
        break;
      }

      if (!is_first(pchannel[i]->RCV)) {
        pchannel[i]->RP->Error = 0x0014;
        pchannel[i]->state_2 = DONE;
        break;
      }

      if (!pchannel[i]->RP->Data.p8) {
        pchannel[i]->RP->Error = 0x0015;
        pchannel[i]->state_2 = DONE;
        break;
      }

      if (pchannel[i]->RP->DataCount + pchannel[i]->RCV->len > pchannel[i]->RP->BufferSize) {
        pchannel[i]->RP->Error = 0x0016;
        pchannel[i]->state_2 = DONE;
        break;
      }

      memcpy((int8*)&pchannel[i]->RP->Data.p8[pchannel[i]->RP->DataCount],
             (int8*)&pchannel[i]->RCV->dat[0],
              pchannel[i]->RCV->len);
      pchannel[i]->RP->DataCount = pchannel[i]->RP->DataCount + pchannel[i]->RCV->len;

      if (is_last(pchannel[i]->RCV)) {
        pchannel[i]->RP->Error = 0;
        pchannel[i]->state_2 = DONE;
      }
      else {
        pchannel[i]->SND_msg_ID->First = 0;
        pchannel[i]->SND_msg_ID->Last  = 0;
        pack_CAN_msg_ID(&pchannel[i]->SND->ID, pchannel[i]->SND_msg_ID);
        pchannel[i]->SND->len = 0;
        status = new_CAN_SND_RCV(START, pchannel[i]);
        pchannel[i]->state_2 = READING;
      }
    break;

//~--

    case READING:

      if (pchannel[i]->RCV->err) {
        pchannel[i]->RP->Error = pchannel[i]->RCV->err;
        pchannel[i]->state_2 = DONE;
        break;
      }

      if (pchannel[i]->RP->DataCount + pchannel[i]->RCV->len > pchannel[i]->RP->BufferSize) {
        pchannel[i]->RP->Error = 0x0017;
        pchannel[i]->state_2 = DONE;
        break;
      }

      memcpy((int8*)&pchannel[i]->RP->Data.p8[pchannel[i]->RP->DataCount],
             (int8*)&pchannel[i]->RCV->dat[0],
              pchannel[i]->RCV->len);
      pchannel[i]->RP->DataCount = pchannel[i]->RP->DataCount + pchannel[i]->RCV->len;

      if (!is_first(pchannel[i]->RCV) && !is_last(pchannel[i]->RCV)) {
        status = new_CAN_SND_RCV(START, pchannel[i]);
        break;
      }

      if (is_last(pchannel[i]->RCV)) {
        pchannel[i]->RP->Error = 0;
        pchannel[i]->state_2 = DONE;
        break;
      }

      pchannel[i]->RP->Error = 0x0018;
      pchannel[i]->state_2 = DONE;
    break;

    default:
      printf("***ERROR: state = %d\n", pchannel[i]->state_1);
      exit(1);
    break;
    
//~--

  } // end-of-switch

//~--

  if (pchannel[i]->state_2 != pchannel[i]->old_state_2) {
    if (P > 3) printf("%6d STAT: state: %d->%d\n", 
                      pchannel[i]->c_2, pchannel[i]->old_state_2, pchannel[i]->state_2);
    pchannel[i]->old_state_2 = pchannel[i]->state_2;
  }

  pchannel[i]->c_2 = pchannel[i]->c_2 + 1;

//~--------

} // for (i=0; i<4; i++) {

//~========

  for (i=0; i<4; i++) {
    const static int offset = 4;
    
    if ((pchannel[(i + offset) % 4]->state_1 == START)) {
      if (P > 3) printf("JMDC%d GO\n", pchannel[(i + offset) % 4]->timer);
      status = new_CAN_SND_RCV(GO, pchannel[(i + offset) % 4]);
#if 1
      if ((i == 3)) delay(Delay2);
      else          delay(Delay);
#else
      delay(Delay);
#endif
    }
  }
  
  while (1) {
    int done = 0;
    for (i=0; i<4; i++) if ((new_CAN_SND_RCV(CONTINUE, pchannel[i]) == DONE)) done++;
    if ((done == 4)) break;
  }

  return;
}

//~============================================================================

void prepare_USCM_commands(void) {

  int i, j;

//~--

  for (i=0; i<4; i++) {
    pRequest[i]               = &Request[i];
    pRequest[i]->BlockType    = 0;
    pRequest[i]->NodeAddress  = (i == 0 ? USCM_ID : USCM2_ID);
    pRequest[i]->DataType     = 0x01;
    pRequest[i]->DataCount    = n;
    pRequest[i]->Data.p8      = &dat[i][0];
    pack16(&pRequest[i]->Source, 0xFF00, i);
    for (j=0; j<8192; j++) dat[i][j] = j & 0x00FF;
    pReply[i]             = &Reply[i];
    pReply[i]->BufferSize = 8192;
    pReply[i]->Data.p8    = &datdat[i][0];
    pReply[i]->Source     = pRequest[i]->Source;
    pchannel[i] = &channel[i];
    pchannel[i]->SND_msg_ID = &SND_msg_ID[i];
    pchannel[i]->SND = &SND[i];
    pchannel[i]->RCV = &RCV[i];
    pchannel[i]->timer = i;
    pchannel[i]->SND->buf = 2 * i + 1;
    pchannel[i]->RCV->buf = 2 * i + 2;
    pchannel[i]->state_1 = DONE;
    pchannel[i]->old_state_1 = -1;
    pchannel[i]->c_1 = 0;
    pchannel[i]->RQ = pRequest[i];
    pchannel[i]->RP = pReply[i];
    pchannel[i]->state_2 = DONE;
    pchannel[i]->old_state_2 = -1;
    pchannel[i]->c_2 = 0;
  }
}

//~----------------------------------------------------------------------------

void start_USCM_commands(_channel **pchannel) {

  int i;
  
  for (i=0; i<4; i++) {
    if (P > 1) print_AMS_data_block("REQUEST:", pchannel[i]->RQ);
  }
    
  new_CANStateMachine(START, pchannel);
}

//~----------------------------------------------------------------------------

void check_USCM_command(_channel *pchannel) {

  int i;
  bool err = FALSE;  

  if (Debug) {
    if (rand() & 1) pchannel->RP->DataCount++;
    else            pchannel->RP->Data.p8[rand()&0xFF]++;
    Debug = FALSE;
  }
  
  if (pchannel->RQ->DataCount != pchannel->RP->DataCount) {
#if 0 // HBB DISABLED 20051109
    err = TRUE;
#endif    
    if (P) printf("Error: SND %d bytes, RCV %d bytes.\n", 
                  pchannel->RQ->DataCount,
                  pchannel->RP->DataCount);
  }
  if (!err) {
    for (i=0; i<n; i++) {
      if (pchannel->RQ->Data.p8[i] != pchannel->RP->Data.p8[i]) err = TRUE;
    }
    if (err) {
      if (P) printf("Error:\n   i  SND  RCV\n");
      for (i=0; i<n; i++) {
        if (pchannel->RQ->Data.p8[i] != pchannel->RP->Data.p8[i]) {
          if (P) printf("%4d 0x%02X 0x%02X\n",
                           i, pchannel->RQ->Data.p8[i], pchannel->RP->Data.p8[i]);
        }
      }
    }
  }

  if (err) eee[pchannel->timer]++;
  
//if ((P > 1) || (err && P)) print_AMS_data_block("REPLY:  ", pchannel->RP);
  if (P > 1) print_AMS_data_block("REPLY:  ", pchannel->RP);
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int nnn;
  int16 err;
  static bool first = TRUE;
  bool Auto = FALSE;
  bool Auto2 = FALSE;
  
//~--

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [<USCM_ID> [<USCM2_ID [<ECHO length> [<Debug Printout Level>]]]]\n",argv[0]);

  USCM_ID = 0x152;
  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);

  USCM2_ID = 0x137;
  if (argc > 2) USCM2_ID = strtoul(argv[2], NULL, 16);

  n = 255;
  if (argc > 3) n = atoi(argv[3]);

  P = 0;
  if (argc > 4) P = atoi(argv[4]);
  
  printf("USCM  ID = 0x%02X\n", USCM_ID);
  printf("USCM2 ID = 0x%02X\n", USCM2_ID);
  printf("n        = %d\n", n);
  printf("P        = %d\n", P);
 
  initialize_CAN_controllers(&err);
  if (err) exit(1);

//~--

  ccc = 0;
  eee[0] = eee[1] = eee[2] = eee[3] = 0;
  
  prepare_USCM_commands();
  
  while (1) {
    if (kbhit()) {
      int ch = toupper(fgetc(stdin));
      switch (ch) {
        case '=': Delay = Delay + 0.000010; printf("Delay = %f\n", Delay); break;
        case '+': Delay = Delay + 0.000100; printf("Delay = %f\n", Delay); break;
        case '_': Delay = MAX(0.0, Delay - 0.000100); printf("Delay = %f\n", Delay); break;
        case '-': Delay = MAX(0.0, Delay - 0.000010); printf("Delay = %f\n", Delay); break;
        case '.': Delay2 = Delay2 + 0.000010; printf("Delay2 = %f\n", Delay2); break;
        case '>': Delay2 = Delay2 + 0.000100; printf("Delay2 = %f\n", Delay2); break;
        case ',': Delay2 = MAX(0.0, Delay2 - 0.000010); printf("Delay2 = %f\n", Delay2); break;
        case '<': Delay2 = MAX(0.0, Delay2 - 0.000100); printf("Delay2 = %f\n", Delay2); break;
        case '0': eee[0] = eee[1] = eee[2] = eee[3] = ccc = 0; printf("Counters to zero\n"); break;
        case 'A': Auto = !Auto; printf("%sauto mode\n", Auto ? "" : "Non-"); break;
        case '2': Auto2 = !Auto2; printf("%sauto-2 mode\n", Auto2 ? "" : "Non-"); break;
        case 'D': Debug = TRUE; printf("Fake error:\n"); break;
        case 'P': P = 3 - P; break;
        case 'Q': exit(0);
      }
    }

    while (1) {
      new_CANStateMachine(CONTINUE, pchannel);
//    printf("%d %d %d %d\n", pchannel[0]->state_2, pchannel[1]->state_2, pchannel[2]->state_2, pchannel[3]->state_2);
      if ((pchannel[0]->state_2 == DONE) &&
          (pchannel[1]->state_2 == DONE) &&
          (pchannel[2]->state_2 == DONE) &&
          (pchannel[3]->state_2 == DONE)) break;
    }
    
    if (first) {
      first = FALSE;
    }
    else {
      check_USCM_command(pchannel[0]);
      check_USCM_command(pchannel[1]);
      check_USCM_command(pchannel[2]);
      check_USCM_command(pchannel[3]);
    }
//if (ccc == 1) break;
//sleep(1);
    start_USCM_commands(pchannel);
    ccc++;
    if (n < 256) nnn = 100;
    else         nnn = 100;
    if (!(ccc % nnn)) {
      printf("Delay = %f/%f: ccc = %d, eee[0] = %d, eee[1] = %d, eee[2] = %d, eee[3] = %d\n",
              Delay, Delay2, ccc, eee[0], eee[1], eee[2], eee[3]);
      if (Auto) {
        ccc = 0;
        eee[0] = eee[1] = eee[2] = eee[3] = 0;
        Delay = Delay + 0.000010;
      } else if (Auto2) {
        ccc = 0;
        eee[0] = eee[1] = eee[2] = eee[3] = 0;
        Delay2 = Delay2 + 0.000010;
      }
    }
  }
  return 0;
}

//~----------------------------------------------------------------------------
