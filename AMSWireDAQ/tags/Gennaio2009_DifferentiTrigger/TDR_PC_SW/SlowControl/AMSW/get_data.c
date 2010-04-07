// file get_data.c
//
// Program to read events from JINJ and unpack them.
// All DAQ nodes should be connected as in flight.
//
// A.Lebedev Jul-2007... 
// A.Basili Oct-2007
// A.Lebedev Nov-2007...     (I am back!)
//

#include "jlv1def.h"
#include "jinflib.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int TX = 2;                 // global variable
int RX = 2;                 // global variable

int P = 0;                                         // global variable
 
int16 AMSW_PATH[5];         // global variable
int16 JINF_PATH = 0x0000;   // global variable

#define SDR2_0_A_LINK  6
#define SDR2_0_B_LINK  7
#define SDR2_1_A_LINK  4
#define SDR2_1_B_LINK  5
#define SDR2_2_A_LINK 18
#define SDR2_2_B_LINK 19
#define SDR2_3_A_LINK 20
#define SDR2_3_B_LINK 21
#define JLV1_A_LINK   14
#define JLV1_B_LINK   15

extern _b glob_trg_msk; 

typedef struct {
  int    num;
  int    len;
  int16  sta;
  int32  msk;   // <<<=== bad name
  int16 *dat;
  struct {
    int    len;
    int16  sta;
    int16 *dat;
  } fra[24];
} _event;

int PP = 0;
int32 masks[16];
int32 m;

//~----------------------------------------------------------------------------

void disable_global_trigger(int16 *err) {

  int16 request[1000];
  int16 reply[1000];

  begin_IO_segment(request);
  add_IO_write(request, glob_trg_msk.ind, glob_trg_msk.msk);
  end_IO_segment(request);
  
  perform_IO_operation(request, NULL, reply, err);
}

//~----------------------------------------------------------------------------

void enable_global_trigger(int16 *err) {

  int16 request[1000];
  int16 reply[1000];

  begin_IO_segment(request);
  add_IO_write(request, glob_trg_msk.ind, 0x0000);
  end_IO_segment(request);
  
  perform_IO_operation(request, NULL, reply, err);
}

//~----------------------------------------------------------------------------

bool get_JINJ_event(_event *event, int16 *err) {
  
  int16 *p;
  bool debug = FALSE;
  
  //for (i=0; i<24;i++) event->fra->len = 0;

  if (!get_JINF_event(&event->len, event->dat, err)) return FALSE;

  p = event->dat;
  event->num = *p++;
  event->sta = event->dat[event->len-1];
  
  event->msk = 0x000000;

  while (1) {
    int16 lnk;
    int32 m;
    if (p + *p >= event->dat + event->len) break;
    lnk = *(p + *p) & 0x001F;
    if (debug) printf("lnk = %2d, ", lnk);
    if (lnk > 23) {
      *err = 0x0100;
      return FALSE;
    }
    m = 1 << lnk;
    if (debug) printf("event->msk = %06X, m = %06X, ", event->msk, m);
    if (m & event->msk) {
      *err = 0x0200;
      return FALSE;
    }

    event->msk         |=  m;
    event->fra[lnk].dat =  p;
    event->fra[lnk].len = *p;
    event->fra[lnk].sta = event->fra[lnk].dat[event->fra[lnk].len];

    if (debug) printf("event->fra[%2d].len = %3d \n", lnk, event->fra[lnk].len);

    p = p + *p + 1;
  }

  return TRUE;
}

//~----------------------------------------------------------------------------

void setup_DAQ_system(void) {

  int i;
  int16 err;

  m = 1<<SDR2_0_A_LINK | 1<<SDR2_0_B_LINK | 1<<SDR2_1_A_LINK | 1<<SDR2_1_B_LINK |
      1<<SDR2_2_A_LINK | 1<<SDR2_2_B_LINK | 1<<SDR2_3_A_LINK | 1<<SDR2_3_B_LINK |
      1<<JLV1_A_LINK   | 1<<JLV1_B_LINK;

//~---

  for (i=0; i<5; i++) AMSW_PATH[i] = 0;

  test_JINF_slaves(&err);
  if (err) EXIT("test slaves in JINJ failed");

  get_JINF_slave_masks(masks, &err);
  if (err) EXIT("get slave masks from JINJ failed");

//~---

  for (i=0; i<5; i++) AMSW_PATH[i] = 0;

  if (masks[0] & 1 << JLV1_A_LINK) {
    AMSW_PATH[0] = JLV1_A_LINK << 8 | 0x003F;
    disable_global_trigger(&err);
    if (err) EXIT("disable trigger in JLV1-A failed");
  }
  
  if (masks[0] & 1 << JLV1_B_LINK) {
    AMSW_PATH[0] = JLV1_B_LINK << 8 | 0x003F;
    disable_global_trigger(&err);
    if (err) EXIT("disable trigger in JLV1-B failed");
  }
  
//~---

  for (i=0; i<5; i++) AMSW_PATH[i] = 0;

  if (m & masks[0]) {                              // error in JINJ DSP program
    set_JINF_slave_mask(1, m&masks[0], &err);
    if (err) EXIT("set slave mask 1 in JINJ failed");

    AMSW_PATH[0] = 0x4001;
    reset_JINF_event_FIFO(&err);
    if (err) EXIT("reset event FIFO in SDR2s and JLV1s failed");
  }

//~---

#if 0
  for (i=0; i<5; i++) AMSW_PATH[i] = 0;

  AMSW_PATH[0] = 0x083F;
  AMSW_PATH[1] = 0x003F;
  reset_JINF_event_FIFO(&err);
  if (err) {
    fprintf(stderr, "reset_JINF_event_FIFO failed (1) ERR = %04X\n", err);
    exit(1);
  }
#endif

//~---

  for (i=0; i<5; i++) AMSW_PATH[i] = 0;

  if (~m & masks[0]) {                              // error in JINJ DSP program
    set_JINF_slave_mask(1, ~m&masks[0], &err);
    if (err) EXIT("set slave mask ~1 in JINJ failed");

    AMSW_PATH[0] = 0x4001;
    reset_JINF_event_FIFO(&err);
    if (err) EXIT("reset event FIFO in not SDR2s and not JLV1s failed");
  }

//~---

  for (i=0; i<5; i++) AMSW_PATH[i] = 0;

  reset_JINF_event_FIFO(&err);
  if (err) EXIT("reset event FIFO in JINJ failed");

//~---

  for (i=0; i<5; i++) AMSW_PATH[i] = 0;

  if (m & masks[0]) {                              // error in JINJ DSP program
    set_JINF_slave_mask(1, m&masks[0], &err);
    if (err) EXIT("set slave mask 1 in JINJ failed");

    AMSW_PATH[0] = 0x4001;
    set_JINF_proc_mode(1, &err);
    if (err) EXIT("set proc mode in SDR2s and JLV1s failed");
  }

//~---

  for (i=0; i<5; i++) AMSW_PATH[i] = 0;

  set_JINF_proc_mode(1, &err);
  if (err) EXIT("set proc mode in JINJ failed");

//~---

  if (masks[0] & 1 << JLV1_A_LINK) {
    AMSW_PATH[0] = JLV1_A_LINK << 8 | 0x003F;
    enable_global_trigger(&err);
    if (err) EXIT("enable trigger in JLV1-A failed");
  }
}

//~============================================================================

int main(int argc, char *argv[]) {

  int i, j, k;
  int nev = 0, nr;
  bool got_event;
  struct timeval now, then;
  int16 mask, err = 0;

  _event event;

//~---

//  setvbuf(stdout, NULL, _IOFBF, 0);
  setbuf(stdout, NULL); 
  setup_command_path();
  
  fprintf(stderr, "Usage: %s [port [nev [PP [P]]]]\n",
          argv[0]);
  if (argc < 2) {
    printf("Too few arguments.\n");
    exit(1);
  }
  
  k = 1;
  if (argc > k) TX = RX = atoi(argv[k++]);
  if (argc > k) nev     = atof(argv[k++]);
  if (argc > k) PP      = atoi(argv[k++]);
  if (argc > k) P       = atoi(argv[k++]);
  
  fprintf(stderr, "port = %d\n", TX);
  fprintf(stderr, "nev  = %d", nev);
  fprintf(stderr, "%s", nev ? "\n" : " (means forever)\n");
  fprintf(stderr, "PP   = %d\n", PP);
  fprintf(stderr, "P    = %d\n", P);

  set_AMSW_timeout(1.0);
  
//~---

  for (i=0; i<5; i++) AMSW_PATH[i] = 0;

  event.dat = malloc(0xFFFF);
  
  gettimeofday(&then, NULL);

  nr = 1;
  while (1) {
    got_event = get_JINJ_event(&event, &err);
    if (err) EXIT("get JINJ event failed");
    if (got_event) {
      printf("%04X ", event.len);    
      for (i=0; i<event.len; i++) {
	printf("%04X ", event.dat[i]);
      }
      printf("\n");
// print event fragments if present
      if (PP > 1) {
        fprintf(stderr, "\nJINJ event: num = %3d, len = %4d, sta = %04X\n", 
                event.num, event.len, event.sta);
	for (i=0; i<24;i++){
	  mask = event.msk;
	  if ((mask >> i) & 0x1){
	    fprintf(stderr, "Event frag: lnk = %03X, len = %4d, sta = %04X\n", 
                    i, event.fra[i].len, event.fra[i].sta);
	    for (j=0; j<event.fra[i].len; j++) {
	      fprintf(stderr, "%04X%s", event.fra[i].dat[j], (j+1) % 10 ? " " : "\n");
	    }
	    fprintf(stderr, "\n");
	    
	    
	  }
	}   
      }
      fprintf(stderr, "\rNUMBER OF EVENTS = %d\r", nr);
      if (nr == nev) break;
      nr++;
    }

  }
  gettimeofday(&now, NULL);
  fprintf(stderr, "\n%.2f events per second\n", (float)(nr)/delta_t(&now, &then));

//~--

  for (i=0; i<5; i++) AMSW_PATH[i] = 0;

  if (masks[0] & 1 << JLV1_A_LINK) {
    AMSW_PATH[0] = JLV1_A_LINK << 8 | 0x003F;
    disable_global_trigger(&err);
    if (err) EXIT("disable trigger in JLV1-A failed");
  }
  
  return 0;
}

//~============================================================================
