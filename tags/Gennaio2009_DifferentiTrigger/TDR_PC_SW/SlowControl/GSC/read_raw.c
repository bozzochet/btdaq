// file read_raw.c
//
// read and decode block files
//
// This program was stolen from Jorge Casaus and reworked
//
// A.Lebedev - Mar 2008...
//

#include "mylib.c"

#define JMDC_BLOCK 0x07  /* JMDC block type */
#define SDDG_BLOCK 0x01  /* Subdetector Data Group block type */
#define CAL1_BLOCK 0x13  /* Calibration Block JINx level */
#define CAL2_BLOCK 0x14  /* Calibration Block xDR level */

#define JMDC0 0
char* JMDC[24]= {       "",       "",       "",       "",
                        "",       "",       "",       "",
                        "",       "",       "",       "",
        	  "JMDC-0", "JMDC-1", "JMDC-2", "JMDC-3",
                        "",       "",       "",       "",
                        "",       "",       "",       ""
                }; 


#define JINJ0 128
char* JINJ[8]=  { "JINJ-0", "JINJ-1", "JINJ-2", "JINJ-3",
		  "JINJ-P-A", "JINJ-P-B", "JINJ-S-A", "JINJ-S-B"
                };

char* JINF[24]= {   "JINFT2",   "JINFT3", "JINFU1", "JINFT0", 
                    "SDR-1A",   "SDR-1B", "SDR-0A", "SDR-0B",
                    "JINFU0",   "JINFT1", "JINFR0", "JINFR1",
                    "JINFE0",   "JINFE1", "JLV1-A", "JLV1-B",
                    "JINFT4",   "JINFT5", "SDR-2A", "SDR-2B",
                    "SDR-3A",   "SDR-3B", "JINFT6", "JINFT7"
                };

#define RDR0 0xF2
#define NRDR 24

char* DATA[4]= { "UNKNOWN", "RAW", "COMPRESSED", "MIXED" };

char* RICH_CALIB_TABLE[5] = { "PEDESTAL Gx1", "PEDESTAL Gx5", "THRESHOLD",
                              "SIGMA Gx5 (x1024)", "STATUS" };

#define RAW 1
#define COMPRESSED 2
#define MIXED 3

#define RAWBLKSIZE 993
#define CTABLESIZE 2480

int DEBUG=0;

//~- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Event Blocks
struct PrimaryHeader{
  int32 Length;
  int32 Size;
  char RR;
  char RW;
  int16 Node;
  int16 Type;
  int16 TypeExt;
};

struct SecondaryHeader{
  int16 Status;
  int16 Tag;
  int32 Time;
};

struct JMDCblock{
  int32 Length;
  int32 Size;
  char RR;
  char RW;
  int16 Node;
  int16 Type;
  int32 Tag;
  int32 Run;
  int32 Event;
  int32 Time;
  int32 Usec;
  char G;
  int16 Groups;
  int16 GPSWeek;
};

struct Event{
  int16 DataBlocks;
  int16 JMDCBlocks;
  struct PrimaryHeader PrimaryHeader;
  struct SecondaryHeader SecondaryHeader;
  struct JMDCblock JMDC;
};

struct Event EventLast, EventPrev;

int EventInit(struct Event *Event);
int EventPrint(struct Event *Event);
int EventCheck();


// Run counters
struct ShortEvent{
  int32 Run;
  int32 Event;
  int32 Tag;
  int32 Time;
  int32 Usec;
};

struct Run{
  int32 ReadEvents;
  double TDiffSum;
  int32 TDiffMin;
  int32 TDiffMax;
  struct ShortEvent FirstEvent;
  struct ShortEvent LastEvent;
};

struct Run Run;

//~- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int RunClear();
int RunReport();

int decode_block(int16 *event, int32 size, int32 length);
int decode_jmdc(char rr, char rw, int16 node, int16 *event, int32 size, int32 length);
int decode_sddg(char rr, char rw, int16 node, int16 *event, int32 size, int32 length);
int decode_cal1(char rr, char rw, int16 node, int16 *event, int32 size, int32 length);
int decode_cal2(char rr, char rw, int16 node, int16 *event, int32 size, int32 length);
int decode_trg_block(int16 node, int16 *event, int32 length);
int decode_trk_block(int16 node, int16 *event, int32 length);
int decode_tof_block(int16 node, int16 *event, int32 length);
int decode_trd_block(int16 node, int16 *event, int32 length);
int decode_rich_block(int16 node, int16 *event, int32 length);
int decode_ecal_block(int16 node, int16 *event, int32 length);
int decode_rich_calib(int16 node, int16 *event, int32 length);
int dump_block(int16 *event, int32 length);
int decode_status(int16 status);

void PrepareFCSTable();
unsigned short DoFCS(unsigned short *data, int size);

//~============================================================================

int decode_block(int16 *event, int32 size, int32 length) {

  int pos = 0;
  int16 rr, rw, node, type, type_ext = 0;
  
  rr   = (event[pos] >> 15) & 0x1;
  rw   = (event[pos] >> 14) & 0x1;
  node = (event[pos] >>  5) & 0x1ff;
  type =  event[pos]        & 0x1f;
  if (type == 0x1f) {
    pos++;
    type_ext = event[pos];
  }

  if (DEBUG) {
    printf(">>> decode_block: rr %d , rw %d, node 0x%x, type 0x%x, type_ext 0x%x\n",
           rr, rw, node, type, type_ext);
  }
  
  pos++;
  switch (type) {
    case JMDC_BLOCK: decode_jmdc(rr,rw,node,&event[pos],size,length); break;
    case SDDG_BLOCK: decode_sddg(rr,rw,node,&event[pos],size,length); break;
    case CAL1_BLOCK: decode_cal1(rr,rw,node,&event[pos],size,length); break;
    case CAL2_BLOCK: decode_cal2(rr,rw,node,&event[pos],size,length); break;
  }

  return 1;
}

//~----------------------------------------------------------------------------

int decode_jmdc(char rr, char rw, int16 node, int16 *event, int32 size, int32 length){

  int32 run_number, run_tag, event_number, event_time, event_usec;
  int16 g, groups, gps_week = 0;
  int pos;

  if (size !=24 && size != 34) {
    printf("decode_jmdc-ERROR-UnexpectedBlockSize: %d (should be 24 or 34)\n", size);
    return -1;
  }
  
  pos = 0;

  run_tag = (event[pos] << 16) | event[pos+1];

  pos += 2;
  run_number = (event[pos] << 16) | event[pos+1];

  pos += 2;
  event_number = (event[pos] << 16) | event[pos+1];

  pos += 2;
  event_time = (event[pos] << 16) | event[pos+1];

  pos +=2 ;
  event_usec = (event[pos] << 16) | event[pos+1];

  pos += 2;
  g = event[pos] >> 15;
  groups = event[pos] & 0xf;

  if ( (size==24 && g) || (size==34 && !g) ) {
    printf("decode_jmdc-ERROR-IncompatibleBlockSizeAngGPS: %d %d\n", size, g);
    return -2;
  }

  if (g) {
    pos++;
    gps_week=event[pos];
  }

  EventLast.JMDCBlocks++;

  EventLast.JMDC.Length  = length;
  EventLast.JMDC.Size    = size;
  EventLast.JMDC.RR      = rr;
  EventLast.JMDC.RW      = rw;
  EventLast.JMDC.Node    = node;
  EventLast.JMDC.Type    = JMDC_BLOCK;
  EventLast.JMDC.Tag     = run_tag;
  EventLast.JMDC.Run     = run_number;
  EventLast.JMDC.Event   = event_number;
  EventLast.JMDC.Time    = event_time;
  EventLast.JMDC.Usec    = event_usec;
  EventLast.JMDC.G       = g;
  EventLast.JMDC.Groups  = groups;
  EventLast.JMDC.GPSWeek = gps_week;

  if (DEBUG) {
    printf("decode_jmdc: Node %s, run_number 0x%x, run_tag 0x%x, event_number %d, event_time %d, event_usec %d\n",
           JMDC[node-JMDC0], run_number, run_tag, event_number, event_time, event_usec); 
  }
  
  return 0;
}

//~----------------------------------------------------------------------------

int decode_sddg(char rr, char rw, int16 node, int16 *event, int32 size, int32 length){

  int32 block_size;
  int16 event_number, status, crc16, node_status, node_id, data_typ;
  int pos;
  bool top_level;
  
  event_number = event[0];
  status       = event[length-3];
  crc16        = event[length-2];

  top_level = (node >= JINJ0 && node < JINJ0 + 8) ? TRUE : FALSE;

  if (DEBUG){
    if (top_level) printf("Node %s ", JINJ[node-JINJ0]);
    printf("event_number %d, node_status %x, crc16 %x\n", event_number, status, crc16);
  }

  if (DoFCS(event,length-1)){
    printf("CRC-ERROR-FOUND\n");
    return -1;
  }

  decode_status(status);

  pos = 1;

  while(pos < length - 3){
    block_size = event[pos];
    if (block_size & 0x8000){
      pos++;
      block_size = (block_size << 16) | event[pos];
    }
    node_status = event[pos+block_size];
//  printf(" pos %d block_size %d node_status 0x%x\n", pos, block_size, node_status);
    node_id  =  node_status       & 0x1F;
    data_typ = (node_status >> 6) & 3;
    if (DEBUG) {
      printf("node_id 0x%x (%s) data_typ %d (%s) block_size %d words. status 0x%x\n",
              node_id, JINF[node_id], data_typ, DATA[data_typ], block_size, node_status);
    }
    decode_status(node_status);
    pos++;
    switch (node_id) {
      case 0: case 1: case 3: case 9: case 16: case 17: case 22: case 23: 
        decode_trk_block(node_id, &event[pos], block_size);
      break;
      case 4: case 5: case 6: case 7: case 18: case 19: case 20: case 21: 
        decode_tof_block(node_id, &event[pos], block_size);
      break;
      case 2: case 8:
        decode_trd_block(node_id, &event[pos], block_size);
      break;
      case 14: case 15:
        decode_trg_block(node_id, &event[pos], block_size);
      break;
      case 10: case 11:
      	decode_rich_block(node_id, &event[pos], block_size);
      break;
      case 12: case 13:
        decode_ecal_block(node_id, &event[pos], block_size);
      break;
    }
    pos += block_size;
    EventLast.DataBlocks++;
  }
  return 0;
}

//~----------------------------------------------------------------------------

int decode_cal1(char rr, char rw, int16 node, int16 *event, int32 size, int32 length){

  int32 block_size;
  int16 status, node_status, node_id, data_typ;
  int pos;

  status=event[length-1];

  decode_status(status);

  pos = 0;
  while (pos < length - 3) {
    block_size=event[pos];
    if (block_size & 0x8000) {
       pos++;
       block_size = (block_size << 16) | event[pos];
    }
    node_status = event[pos+block_size];
//  printf(" %d %d 0x%x\n", pos, block_size, node_status);
    node_id  =  node_status       & 0x1F;
    data_typ = (node_status >> 6) & 0x03;
    if (DEBUG) {
      printf("node_id 0x%x data_typ %d (%s) block_size %d words. status 0x%x\n",
              node_id, data_typ, DATA[data_typ], block_size, node_status);
    }
    decode_status(node_status);
    pos++;
    switch (node) {
      case 0xa0: // JINF-R-0-P
      case 0xa4: // JINF-R-1-P
      case 0xf2: // RDR-0-0-A (old buggy version)
        decode_rich_calib(node_id, &event[pos], block_size);
      break;
    }
    pos += block_size;
    EventLast.DataBlocks++;
  }
  return 0;
}

//~----------------------------------------------------------------------------

int decode_cal2(char rr, char rw, int16 node, int16 *event, int32 size, int32 length){

  int32 block_size;
  int16 status, crc16;

  status=event[length-3];
  crc16=event[length-2];
//printf("status 0x%x, crc16 0x%x\n", status, crc16);

  if (DoFCS(event, length-1)) {
    printf("CRC-ERROR-FOUND\n");
    return -1;
  }

  decode_status(status);

  block_size = length-2;

  if (node >= RDR0 && node <= RDR0+NRDR) {
    decode_rich_calib(node, event, block_size);
  }
  return 0;
}

//~----------------------------------------------------------------------------

int decode_trk_block(int16 node, int16 *event, int32 length) {

  return 0;
}

//~----------------------------------------------------------------------------

int decode_trg_block(int16 node, int16 *event, int32 length) {

  return 0;
}

//~----------------------------------------------------------------------------

int decode_tof_block(int16 node, int16 *event, int32 length) {

  return 0;
}

//~----------------------------------------------------------------------------

int decode_ecal_block(int16 node, int16 *event, int32 length) {

  return 0;
}

//~----------------------------------------------------------------------------

int decode_trd_block(int16 node, int16 *event, int32 length) {

  int16 node_status, node_id, data_typ;
  int32 block_size;
  int pos;

//printf("%x %x %x %x\n", event[0], event[1], event[2], event[3]);

  pos = 0;
  while (pos < length - 1) {
    block_size=event[pos];
    if (block_size & 0x8000) {
      pos++;
      block_size = (block_size<<16) | event[pos];
    }
    node_status = event[pos+block_size];
    node_id     =  node_status       & 0x1F;
    data_typ    = (node_status >> 6) & 3;
    if (DEBUG) {
      printf("  node_id 0x%x data_typ %d (%s) block_size %d words. status 0x%x\n",
             node_id, data_typ, DATA[data_typ], block_size, node_status);
    }
    decode_status(node_status);

    pos++;
    switch (data_typ) {
      case COMPRESSED:
        if (DEBUG > 1) {
          if (block_size > 1) {
            int i = 0;
            while(i < block_size - 2) {
              printf("    Hit Address: %3d  Amplitude: 0x%04x (%.03f)\n",
                     event[pos+i], event[pos+i+1], event[pos+i+1]/8.);
              i += 2;
            }
          }
        }
      break;
    }
    pos += block_size;
  }
  return 0;
}

//~----------------------------------------------------------------------------

int decode_rich_block(int16 node, int16 *event, int32 length) {

  int16 node_status, node_id, data_typ;
  int32 block_size;
  int pos;

  //  printf("%x %x %x %x\n",event[0],event[1],event[2],event[3]);

  pos = 0;
  while (pos < length - 1){
    block_size = event[pos];
    if (block_size & 0x8000){
       pos++;
       block_size = (block_size << 16) | event[pos];
    }
    node_status = event[pos+block_size];
    node_id  =  node_status       & 0x1F;
    data_typ = (node_status >> 6) & 0x03;
    if (DEBUG) {
      printf("  node_id 0x%x data_typ %d (%s) block_size %d words. status 0x%x\n",
             node_id, data_typ, DATA[data_typ], block_size, node_status);
    }
    decode_status(node_status);
    pos++;
    switch (data_typ) {
      case RAW:
        if (block_size == RAWBLKSIZE) {
          if (DEBUG > 1) {
            int i = 0;
            while (i < block_size) {
              printf("%4d ",event[pos+i]);
              i++;
              if (!(i % 31)) printf("\n");
            }
          }
        }
        else {
          printf("decode_rich_block-ERROR-IncompatibleRawBlockSize %d\n",block_size); 
        }
      break;
    }
    pos += block_size;
  }
  return 0;
}

//~----------------------------------------------------------------------------

int decode_rich_calib(int16 node, int16 *event, int32 length) {

  if (length != CTABLESIZE + 1) return -1;

  if (DEBUG > 1) {
    int ind, chn, pmt, pos = 0;
    for (ind=0; ind<5; ind++) {
      printf("%s\n",RICH_CALIB_TABLE[ind]);
      for (chn=0; chn<16; chn++) {
        for (pmt=0; pmt<31; pmt++) printf ("%04x ",event[pos++]);
        printf("\n");
      }
    }
  }
//dump_block(event,length);
  return 0;
}

//~----------------------------------------------------------------------------

int EventInit(struct Event *Event) {

  EventLast.PrimaryHeader.Size = 0;
  EventLast.JMDC.Size          = 0;
  EventLast.DataBlocks         = 0;
  EventLast.JMDCBlocks         = 0;

  return 0;
}

//~----------------------------------------------------------------------------

int EventPrint(struct Event *Event) {

  printf(">>>>>> EVENT PRINT <<<<<<\n");

  printf("Data Blocks: %d\n",Event->DataBlocks);

  if (Event->PrimaryHeader.Size) {
    printf("Event Header:\n Event.\tSize\t%d\n \tRR\t%d\n \tRW\t%d\n \tNode\t%d\n \tType\t%d\n \tTypeExt\t%d\n \tStatus\t0x%x\n \tTag\t0x%x\n \tTime\t%d\n",
            Event->PrimaryHeader.Size,
            Event->PrimaryHeader.RR,
            Event->PrimaryHeader.RW,
            Event->PrimaryHeader.Node,
            Event->PrimaryHeader.Type,
            Event->PrimaryHeader.TypeExt,
            Event->SecondaryHeader.Status,
            Event->SecondaryHeader.Tag,
            Event->SecondaryHeader.Time);
  }

  if (Event->JMDC.Size) {
    printf("JMDC Block:\n \tSize\t%d\n \tRR\t%d\n \tRW\t%d\n \tNode\t%d\n \tType\t%d\n \tTag\t0x%x\n \tRun\t%d\n \tEvent\t%d\n \tTime\t%d\n \tusec\t%d\n \tG\t%d\n \tGroups\t%d\n \tGPSWeek\t%d\n", 
            Event->JMDC.Size,
            Event->JMDC.RR,
            Event->JMDC.RW,
            Event->JMDC.Node,
            Event->JMDC.Type,
            Event->JMDC.Tag,
            Event->JMDC.Run,
            Event->JMDC.Event,
            Event->JMDC.Time,
            Event->JMDC.Usec,
            Event->JMDC.G,
            Event->JMDC.Groups,
            Event->JMDC.GPSWeek);
  }
  return 0;
}

//~----------------------------------------------------------------------------

int EventCheck(void) {

  if (EventLast.JMDC.Size==0) {
    printf("\nERROR : JMDCBlockNotFound\n");
    EventPrint(&EventLast);
    return 0;
  }

  if (EventLast.JMDC.Run != Run.LastEvent.Run ||
      EventLast.JMDC.Tag != Run.LastEvent.Tag ){
    if (Run.LastEvent.Run) RunReport();
    RunClear();
    Run.FirstEvent.Run   = EventLast.JMDC.Run;
    Run.FirstEvent.Tag   = EventLast.JMDC.Tag;
    Run.FirstEvent.Event = EventLast.JMDC.Event;
    Run.FirstEvent.Time  = EventLast.JMDC.Time;
    Run.FirstEvent.Usec  = EventLast.JMDC.Usec;
    printf("NewRunFound: run 0x%x, tag 0x%x, FirstEvent Time %d, Usec %d\n",
            Run.FirstEvent.Run, Run.FirstEvent.Tag, Run.FirstEvent.Time, Run.FirstEvent.Usec);
  }
  else if (EventLast.JMDC.Event <= EventPrev.JMDC.Event) {
    printf("\nERROR : EventNumberNotIncreasing %d %d\n",
            EventLast.JMDC.Event, EventPrev.JMDC.Event);
    EventPrint(&EventPrev);
    EventPrint(&EventLast);
    return 0;
  }
  else if (EventLast.JMDC.Time < EventPrev.JMDC.Time) {
    printf("\nERROR : AbsoluteTimeNotIncreasing %d %d\n",
            EventLast.JMDC.Time, EventPrev.JMDC.Time);
    EventPrint(&EventPrev);
    EventPrint(&EventLast);
    return 0;
  }

  Run.ReadEvents++;

  Run.LastEvent.Run   = EventLast.JMDC.Run;
  Run.LastEvent.Tag   = EventLast.JMDC.Tag;
  Run.LastEvent.Event = EventLast.JMDC.Event;
  Run.LastEvent.Time  = EventLast.JMDC.Time;
  Run.LastEvent.Usec  = EventLast.JMDC.Usec;

  // Time diff btwn consecutive evts.
  if (Run.ReadEvents > 1){
    int tdiff = EventLast.JMDC.Time-EventPrev.JMDC.Time;
    int udiff = EventLast.JMDC.Usec-EventPrev.JMDC.Usec;
    if (tdiff > 1) printf("Time Difference above 1s (%d-%d)\n",EventLast.JMDC.Time,EventPrev.JMDC.Time);
    if (udiff < 0) udiff += 1000000;
    Run.TDiffSum += (double)udiff / 1.e6;

    if      (udiff < Run.TDiffMin) Run.TDiffMin = udiff;
    else if (udiff > Run.TDiffMax) Run.TDiffMax = udiff;

//printf("udiff: %d %d %d\n",EventPrev.JMDC.Usec,EventLast.JMDC.Usec,udiff);
  }
  return 1;
}

//~----------------------------------------------------------------------------

int RunClear(){
  Run.ReadEvents=0;
  Run.TDiffSum=0;
  Run.TDiffMin=1000000;
  Run.TDiffMax=0;

  Run.FirstEvent.Run=0;
  Run.FirstEvent.Event=0;
  Run.FirstEvent.Tag=0;
  Run.FirstEvent.Time=0;
  Run.FirstEvent.Usec=0;

  Run.LastEvent.Run=0;
  Run.LastEvent.Event=0;
  Run.LastEvent.Tag=0;
  Run.LastEvent.Time=0;
  Run.LastEvent.Usec=0;
}

//~----------------------------------------------------------------------------

int RunReport(){

  printf("\n   >>>>> RUN REPORT <<<<<<\n");

  int32 ExpectedEvents=Run.LastEvent.Event-Run.FirstEvent.Event+1;
  int32 TimeSpan=Run.LastEvent.Time-Run.FirstEvent.Time;
  float AverageRate=(float)ExpectedEvents/Run.TDiffSum;

  printf("Events Read     : \t%d\n",Run.ReadEvents);
  printf("Events Expected : \t%d\n",ExpectedEvents);
  printf("Time Span (s)   : \t%d\n",TimeSpan);
  printf("Time Span (s)   : \t%.03f\n",Run.TDiffSum);
  printf("AverageRate (Hz): \t%.1f\n",AverageRate);
  printf("1/<TDiff>   (Hz): \t%.1f\n",(Run.ReadEvents-1)/Run.TDiffSum);
  printf("TDiffMin    (ms): \t%.03f\n",(float)Run.TDiffMin/1.e3);
  printf("TDiffMax    (ms): \t%.03f\n",(float)Run.TDiffMax/1.e3);

  printf("First Event:\n \tRun\t0x%x\n \tTag\t0x%x\n \tEvent\t%d\n \tTime\t%d\n \tusec\t%d\n",
	 Run.FirstEvent.Run,
	 Run.FirstEvent.Tag,
	 Run.FirstEvent.Event,
	 Run.FirstEvent.Time,
	 Run.FirstEvent.Usec);

  printf("Last Event:\n \tRun\t0x%x\n \tTag\t0x%x\n \tEvent\t%d\n \tTime\t%d\n \tusec\t%d\n",
	 Run.LastEvent.Run,
	 Run.LastEvent.Tag,
	 Run.LastEvent.Event,
	 Run.LastEvent.Time,
	 Run.LastEvent.Usec);
}

//~----------------------------------------------------------------------------

int dump_block(int16 *event, int32 length){
  int i;
  for (i=0; i<length; i++)
    printf("%d (%d) 0x%0x (%d)\n",i,i%496,event[i],event[i]);
}

//~----------------------------------------------------------------------------

int decode_status(int16 status){

  int error=status&0x7e00;

  if (error) printf("ERROR-FOUND-IN-STATUS-WORD\n");

  if (error || DEBUG>2) {

    printf("Reply Status 0x%x\n",status);
    printf("  slave ID 0x%x\n",status&0x1f);
    if (status>>5&1) printf("  replying node is CDP\n");
    if (status>>6&1) printf("  building RAW Events\n");
    if (status>>7&1) printf("  building COMPRESSED Events\n");
    printf("  cummulative node status %x\n", status>>8&1);
    if (status>>9&1)  printf("  build errors (sequencer errors)\n");
    if (status>>10&1) printf("  build conditions (FE power fault)\n");
    if (status>>15&1) {
      if (status>>11&1) printf("  TIMEOUT\n");
      if (status>>12&1) printf("  AMSW ERROR\n");
      if (status>>13&1) printf("  ASSEMBLY ERROR\n");
      if (status>>14&1) printf("  CRC ERROR\n");
      printf("  DATA bit set\n");
    }
    else {
      if (status>>11&1) printf("  NEXT bit set\n");
      if (status>>12&1) printf("  ERROR bit set\n");
      if (status>>13&1) printf("  ABORT bit set\n");
      if (status>>14&1) printf("  END bit set\n");
    }

  }

  return error;

}

//~----------------------------------------------------------------------------

unsigned short FCS_Table[256];

void PrepareFCSTable() {

  // from Kounine + Koutsenko documentation, p30

  unsigned short const g=0x1021;
  unsigned short d,i,k,fcs;

  // prepare lookup table

  for(i=0; i<256; i++) {
    fcs=0;
    d=i<<8;
    for(k=0; k<8; k++) {
      if ((fcs^d) & 0x8000) fcs=(fcs<<1)^g;
      else fcs<<=1;
      d<<=1;
    }
    FCS_Table[i]=fcs;
  }
}

//~----------------------------------------------------------------------------

unsigned short DoFCS(unsigned short *data, int size) {

  // from Kounine + Koutsenko documentation, p30

  unsigned short fcs=0xffff;
  unsigned char val;

  int i;

  for(i=0; i<size; i++) { 
    val=data[i]>>8;
    fcs=FCS_Table[(fcs>>8)^val]^(fcs<<8);
    val=data[i] & 0xff;
    fcs=FCS_Table[(fcs>>8)^val]^(fcs<<8);
  }

  //  printf("DoFCS = %04x\n",fcs);

  return fcs;
}

//~============================================================================

int main(int argc, char *argv[]){

  char fname[200];

//  char *mfname="/home/jorge/ams/Data/BLOCKS/ACOP/0013/108"; int offset=0;
//  char *mfname="/home/jorge/ams/Data/BLOCKS/ACOP/0026/091"; int offset=0;
//  char *mfname="/home/jorge/ams/Digest/1202483770.rich.sci"; int offset=0;
  char *mfname="/home/jorge/ams/ACOP/0032/075"; // BUILD ERRORS @ 250 Hz
//  char *mfname="/home/jorge/ams/ACOP/0031/865"; // R1 RAW DATA @ 1kHz
//  char *mfname="/home/jorge/ams/ACOP/0031/885"; // R0 RAW DATA @ 1kHz
//  char *mfname="/home/jorge/ams/ACOP/0031/720"; // CALIBRATION TABLES

  if (argc>1) sprintf(fname,argv[1]);
  else        sprintf(fname,mfname);

  printf("Opening file %s\n",fname);
  FILE *pfile=fopen(fname,"r");
  if (pfile==NULL){
    printf("Error Opening File %s\n",fname);
    return -1;
  }

  int32 events_read=0,events_good=0,event_ok=0;

  int32 event_size,event_length,time;
  int16 size,size_ext,node,type,type_ext=0,status,tag,time_msb,time_lsb;
  char rr,rw;

  // Initialize counters
  RunClear();

  // Init FCS Table
  PrepareFCSTable();

  // trash offset words
  int offset=0;
  int nw;
  for (nw=0; nw<offset; nw++) fread(&size,2,1,pfile);

  // main loop
  while (fread(&size,2,1,pfile)==1){

    if (event_ok)
      memcpy(&EventPrev,&EventLast,sizeof(struct Event));

    EventInit(&EventLast);

    event_size=size&0x7fff;
    if (size&0x8000){
      fread(&size_ext,2,1,pfile);
      event_size&~0x8000;
      event_size=(event_size<<16)|size_ext;
    }
    if (DEBUG) printf("\n>>> NEW EVENT <<<\nEvent Size %d (%x) (Bytes)\n",event_size,event_size);
    /*
    if (event_size%2){
      printf("Odd event_size %d\n",event_size);
      return -2;
    }
    */
    event_length=event_size/2;
    int16 *event=calloc(event_length,2);
    if (fread(event,2,event_length,pfile)==event_length){

      /* Event Primary Header */
      int pos=0;
      rr=(event[pos]>>15)&0x1;
      rw=(event[pos]>>14)&0x1;
      node=(event[pos]>>5)&0x1ff;
      type=event[pos]&0x1f;
      if (type==0x1f){
	pos++;
	type_ext=event[pos];
      }

      EventLast.PrimaryHeader.Length=event_length;
      EventLast.PrimaryHeader.Size=event_size;
      EventLast.PrimaryHeader.RR=rr;
      EventLast.PrimaryHeader.RW=rw;
      EventLast.PrimaryHeader.Node=node;
      EventLast.PrimaryHeader.Type=type;
      EventLast.PrimaryHeader.TypeExt=type_ext;

      /* Event Secondary Header */
      pos++;
      status=(event[pos]>>12)&0xf;
      tag=event[pos]&0xfff;
      pos++;
      time_msb=event[pos];
      pos++;
      time_lsb=event[pos];
      time=time_msb<<16|time_lsb;
      if (DEBUG) printf(" Unix time %d (0x%x)\n",time,time);

      EventLast.SecondaryHeader.Status=status;
      EventLast.SecondaryHeader.Tag=tag;
      EventLast.SecondaryHeader.Time=time;

      if (!status) {

	/* Data Blocks */
	pos++;
	int32 block_size,block_length;
	while(pos<event_length){
	  block_size=event[pos];
	  if (block_size&0x8000){
	    pos++;
	    block_size=block_size&~0x8000;
	    block_size=(block_size<<16)|event[pos];
	  }
	  block_length=block_size/2;
	  pos++;
	  decode_block(&event[pos],block_size,block_length);
	  pos+=block_length;
	}

      }
      else {
	printf("Wrong status found : 0x%0x\n",status);
	EventPrint(&EventLast);
      }

    }
    else{
      printf("Error Reading File %s\n",fname);
      return -3;
    }

    events_read++;
    event_ok = EventCheck(events_read);
    if (event_ok) events_good++;

    if(DEBUG || !(events_read%10000)){
      printf("\nEvents Read %d, Last Run 0x%x, Last Event %d\n",
	     events_read, EventLast.JMDC.Run, EventLast.JMDC.Event);
      EventPrint(&EventLast);
    }

    free(event);
  }

  RunReport();

  printf("Total Events Read %d\n",events_read);
  printf("Good  Events Read %d\n",events_good);

  fclose(pfile);
}

//~============================================================================
