// file philip-2.c
//
// Program to read events from JINJ and unpack them.
// All DAQ nodes should be connected as in flight.
//
// A.Lebedev Jul-2007...

#include "jlv1def.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include "daqlib.h"

int TX = 2;                 // global variable
int RX = 2;                 // global variable

int P = 0;                                         // global variable
 
int16 AMSW_PATH[5];         // global variable
int16 JINF_PATH = 0x0000;   // global variable

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
printf("event->num = %d, ", event->num);
printf("lnk = %2d = 0x%02X, ", lnk, lnk);
    m   = 1 << lnk;
printf("event->msk = %06X, m = %06X, ", event->msk, m);
    if (m & event->msk) {
      *err = 0x0200;
      return FALSE;
    }
    event->msk         |=  m;
    event->fra[lnk].dat =  p;
    event->fra[lnk].len = *p;
printf("event->fra[lnk].len = %3d = 0x%04X\n", 
        event->fra[lnk].len, event->fra[lnk].len);
    p = p + *p + 1;
  }

  return TRUE;
}

//~============================================================================

int main(int argc, char *argv[]) {

  int i;
  int nev = 0, nr = 0;
  bool got_event;
  struct timeval now, then;
  int16 err = 0;

  _event event;

//~---

//  setvbuf(stdout, NULL, _IOFBF, 0);
  setbuf(stdout, NULL); 
  setup_command_path();
  
  for (i=0; i<5; i++) AMSW_PATH[i] = 0;
  
  fprintf(stderr, "Usage: %s [port [nev [PP [P]]]]\n",
          argv[0]);
  i = 1;
  if (argc > i) TX = RX = atoi(argv[i++]);
  if (argc > i) nev  = atof(argv[i++]);
  if (argc > i) PP   = atoi(argv[i++]);
  if (argc > i) P    = atoi(argv[i++]);
  
  fprintf(stderr, "port = %d\n", TX);
  fprintf(stderr, "nev  = %d", nev);
  fprintf(stderr, "%s", nev ? "\n" : " (means forever)\n");
  fprintf(stderr, "PP   = %d\n", PP);
  fprintf(stderr, "P    = %d\n", P);

//~---

  test_JINF_slaves(&err);
  if (err) {
    fprintf(stderr, "test_JINF_slaves failed\n");
    exit(1);
  }

  AMSW_PATH[0] = 0x0E3F;
  disable_global_trigger(&err);
  if (err) {
    fprintf(stderr, "disable_global_trigger failed\n");
    exit(1);
  }
  for (i=0; i<5; i++) AMSW_PATH[i] = 0;

  AMSW_PATH[0] = 0x4000;
  reset_JINF_event_FIFO(&err);
  if (err) {
    fprintf(stderr, "reset_JINF_event_FIFO failed (1)\n");
    exit(1);
  }
  for (i=0; i<5; i++) AMSW_PATH[i] = 0;

  reset_JINF_event_FIFO(&err);
  if (err) {
    fprintf(stderr, "reset_JINF_event_FIFO failed(2)\n");
    exit(1);
  }

  AMSW_PATH[0] = 0x0E3F;
  enable_global_trigger(&err);
  if (err) {
    fprintf(stderr, "enable_global_trigger failed\n");
    exit(1);
  }
  for (i=0; i<5; i++) AMSW_PATH[i] = 0;

//~---

  event.dat = malloc(0xFFFF);
  
  gettimeofday(&then, NULL);

  while (1) {
    static float sum_1 = 0.0, sum_2 = 0.0;
    static float nr_1 = 0.0, nr_2 = 0.0;
    static int64 event_time, old_event_time;
    got_event = get_JINJ_event(&event, &err);
    if (err) {
      fprintf(stderr, "get_JINJ_event failed, err = %04X\n", err);
      exit(1);
    }
    if (got_event) {
      nr = nr + 1;
      if (PP > 1) {
        printf("\nGot event: len = %d, sta = %04X, msk = %06X\n", 
               event.len, event.sta, event.msk);
        for (i=0; i<event.len; i++) {
          printf("%04X%s", event.dat[i], (i+1) % 10 ? " " : "\n");
        }
        printf("\n");
      }
      if (event.msk == 0x004080) {
        if (event.fra[14].dat[4]) {
          sum_2 = sum_2 + (event.fra[7].dat[2+4+9*1] & 0x0FFF);
          nr_2  = nr_2 + 1.0;
        }
        else {
          sum_1 = sum_1 + (event.fra[7].dat[2+4+9*1] & 0x0FFF);
          nr_1  = nr_1 + 1.0;
        }
        if (PP > 1) {
          printf("Charge Link #4: ");
          for (i=0; i<10; i++) {
            printf("%04X ", event.fra[7].dat[2+4+9*i]);
          }
          printf("   ACC Input pattern: %04X", event.fra[14].dat[4]);
          printf("\n");
        }
        
        event_time = (int64)(event.fra[14].dat[8] & 0x00FF) << 32 |
                     (int64)(event.fra[14].dat[7] & 0xFFFF) << 16 |
                     (int64)(event.fra[14].dat[6] & 0xFFFF) <<  0;
        
        printf("%d %d %d %10lld", 
//      printf("%d %d %d   %5d %5d %5d   %20lld %10lld", 
               event.fra[7].dat[2+4+9*0]&0x0FFF,
               event.fra[7].dat[2+4+9*1]&0x0FFF,
               event.fra[14].dat[4],
//             event.fra[14].dat[8],
//             event.fra[14].dat[7],
//             event.fra[14].dat[6],
//             event_time,
               event_time-old_event_time);

        old_event_time = event_time;

        if (PP) {
          if (nr_1 && nr_2 ) {
            printf("     %.0f (%.0f) %.0f (%.0f)", sum_1/nr_1, nr_1, sum_2/nr_2, nr_2);
          }
        }
        printf("\n");
      }
      if (nev && nr >= nev) break;
    }
  }

  gettimeofday(&now, NULL);
//fprintf(stderr, "%.2f events per second\n", (float)(nr)/delta_t(&now, &then));

  return 0;
}

//~============================================================================
