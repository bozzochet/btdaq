// file JPD.c
//
// JPD Controller - A.Lebedev Mar-2003...
//

#include "uscmlib.h"
#include "xformslib.h"

//~----------------------------------------------------------------------------

int JMDC_ID = 0x01;                                // global variable
int USCM_ID = 0x44;                                // global variable

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

char *config_file_name = {"JPD.configfile"};       // global variable

int P = 0;                                         // global variable

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct {
  FL_FORM *JPD;
  void *vdata;
  long ldata;
} FD_JPD;

FD_JPD *JPD;

FL_OBJECT *VOL1, *VOL2;
int hide_voltage_code = 0;

int JPD_mod;

FL_OBJECT *CUR1, *CUR2;
int hide_current_code = 0;

FL_OBJECT *MESSAGE;
int msg = 0;
int16 sta;

FL_OBJECT *BOOT_MODE;
int boot_mode;

FL_OBJECT *PERIOD;
int timer_period = 2;

FL_OBJECT *MONITOR;
int monitoring = 0;

FL_OBJECT *WRITING_LOG_FILE;
static FILE *logfile = NULL;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct {
  struct {
    int   unk;
    int   sta;
    int16 dat;
    int16 acc;
    int16 inv;
  } rea;
  struct {
    int   sta;
    int16 inp;
    int16 dat;
    int16 inv;
  } wri;
} LVDS[256];

struct {
  int   unk;
  int   sta;
  int16 dat;
  float val;
} ADC[32];

FL_OBJECT *DCDC_ctr_BOX[32];
FL_OBJECT *DCDC_ctr_WRI[32];
FL_OBJECT *DCDC_ctr_INP[32];
FL_OBJECT *DCDC_ctr_SND[32];
FL_OBJECT *DCDC_mon_BOX[32];
FL_OBJECT *DCDC_acc_BOX[32];
FL_OBJECT *DCDC_dir_BOX[32];
FL_OBJECT *DCDC_vol_cod_BOX[32];
FL_OBJECT *DCDC_vol_val_BOX1[32];
FL_OBJECT *DCDC_vol_val_BOX2[32];
FL_OBJECT *DCDC_cur_cod_BOX[32];
FL_OBJECT *DCDC_cur_val_BOX1[32];
FL_OBJECT *DCDC_cur_val_BOX2[32];

typedef struct {
  char *JPD_board;
  char *J_JT_board;
  char *feed;
  char *nominal;
  int   adr;
  struct {
    int16 msk;
    struct {
      int wri;
      int rea;
    } inv;
  } ctr;
  struct {
    int16 msk;
    int   inv;
  } mon;
  struct {
    int16 msk;
    int   inv;
  } dir;
  struct {
    int ind;
    float k;
    float b;
  } vol;
  struct {
    int ind;
    float k;
    float b;
  } cur;
} _DCDC;


_DCDC *DCDC;

_DCDC EM_DCDC[32] = {
//~--------------------------------------control----monitor-----monitor------voltage-------current--
//~------------------------------------------w-r-----static-----latched-----------------------------
//~-------------------------------adr--mask--i-i-----mask-i-----mask--i----ind--k--b----ind---k--b--
 {"JPMD-0","JMDC-0", "A", "+3.3V", 0, {0x01,{0,0}}, {0x10,0}, {0x0000,0}, { 0,435, 0}, { 4, 488, 0}},
 {"JPMD-0","JMDC-0", "A", "+5.0V", 0, {0x04,{0,0}}, {0x40,0}, {0x0000,0}, { 1,435, 0}, { 5,3320, 0}},
 {"JPMD-0","JMDC-0", "A", "-5.0V", 0, {0x04,{0,0}}, {0x40,0}, {0x0000,0}, {-1,  0, 0}, { 6,3320, 0}},

 {"JPMD-0","JMDC-1", "B", "+3.3V", 0, {0x02,{0,0}}, {0x20,0}, {0x0000,0}, { 2,435, 0}, { 7, 488, 0}},
 {"JPMD-0","JMDC-1", "B", "+5.0V", 0, {0x08,{0,0}}, {0x80,0}, {0x0000,0}, { 3,435, 0}, { 8,3320, 0}},
 {"JPMD-0","JMDC-1", "B", "-5.0V", 0, {0x08,{0,0}}, {0x80,0}, {0x0000,0}, {-1,  0, 0}, { 9,3320, 0}},

 {"JPMD-1","JMDC-2", "A", "+3.3V", 2, {0x01,{1,0}}, {0x10,1}, {0x0000,0}, {10,435, 0}, {14, 488, 0}},
 {"JPMD-1","JMDC-2", "A", "+5.0V", 2, {0x04,{1,0}}, {0x40,1}, {0x0000,0}, {11,435, 0}, {15,3320, 0}},
 {"JPMD-1","JMDC-2", "A", "-5.0V", 2, {0x04,{1,0}}, {0x40,1}, {0x0000,0}, {-1,  0, 0}, {16,3320, 0}},

 {"JPMD-1","JMDC-3", "B", "+3.3V", 2, {0x02,{1,0}}, {0x20,1}, {0x0000,0}, {12,435, 0}, {17, 488, 0}},
 {"JPMD-1","JMDC-3", "B", "+5.0V", 2, {0x08,{1,0}}, {0x80,1}, {0x0000,0}, {13,435, 0}, {18,3320, 0}},
 {"JPMD-1","JMDC-3", "B", "-5.0V", 2, {0x08,{1,0}}, {0x80,1}, {0x0000,0}, {-1,  0, 0}, {19,3320, 0}},

 {" JPIF "," JFOM ", "A", "+5.0V",-1, {0x00,{0,0}}, {0x10,1}, {0x0000,0}, {26,435, 0}, {29,3320, 0}},
 {" JPIF "," JFOM ", "B", "+5.0V",-1, {0x00,{0,0}}, {0x20,1}, {0x0000,0}, {26,435, 0}, {29,3320, 0}},
 {" JPIF "," JFOM ", "A", "-5.2V",-1, {0x00,{0,0}}, {0x40,1}, {0x0000,0}, {27,435, 0}, {30,1660, 0}},
 {" JPIF "," JFOM ", "B", "-5.2V",-1, {0x00,{0,0}}, {0x80,1}, {0x0000,0}, {27,435, 0}, {30,1660, 0}},
 {" JPIF "," JLIF ", "A", "+5.0V", 9, {0x00,{0,0}}, {0x10,1}, {0x0000,0}, {28,435, 0}, {31,3320, 0}},
 {" JPIF "," JLIF ", "B", "+5.0V", 9, {0x00,{0,0}}, {0x20,1}, {0x0000,0}, {28,435, 0}, {31,3320, 0}},
 {" JPIF "," J422 ", "A", "+5.0V", 9, {0x00,{0,0}}, {0x40,1}, {0x0000,0}, {-1,  0, 0}, {-1,   0, 0}},
 {" JPIF "," J422 ", "B", "+5.0V", 9, {0x00,{0,0}}, {0x80,1}, {0x0000,0}, {-1,  0, 0}, {-1,   0, 0}},

 {" JPJT ","JLV1-A", "A", "+3.3V", 5, {0x01,{1,0}}, {0x10,1}, {0x0000,0}, {-1,  0, 0}, {24,1660, 0}},
 {" JPJT ","JLV1-B", "B", "+3.3V", 5, {0x02,{1,0}}, {0x20,1}, {0x0000,0}, {-1,  0, 0}, {25,1660, 0}},

 {" JPJT ","JINJ-0", "A", "+3.3V", 4, {0x01,{1,0}}, {0x10,1}, {0x0000,0}, {-1,  0, 0}, {20,3320, 0}},
 {" JPJT ","JINJ-1", "B", "+3.3V", 4, {0x02,{1,0}}, {0x20,1}, {0x0000,0}, {-1,  0, 0}, {21,3320, 0}},
 {" JPJT ","JINJ-2", "A", "+3.3V", 4, {0x04,{1,0}}, {0x40,1}, {0x0000,0}, {-1,  0, 0}, {22,3320, 0}},
 {" JPJT ","JINJ-3", "B", "+3.3V", 4, {0x08,{1,0}}, {0x80,1}, {0x0000,0}, {-1,  0, 0}, {23,3320, 0}},

 {" JPSC ","      ", "A", " 28V ",-1, {0x00,{0,0}}, {0x01,1}, {0x0000,0}, {-1,  0, 0}, {-1,   0, 0}},
 {" JPSC ","      ", "B", " 28V ",-1, {0x00,{0,0}}, {0x02,1}, {0x0000,0}, {-1,  0, 0}, {-1,   0, 0}},
 {" JPSC ","      ", "A", "+5.0V", 6, {0x00,{0,0}}, {0x10,1}, {0x0000,0}, {-1,  0, 0}, {-1,   0, 0}},
 {" JPSC ","      ", "B", "+5.0V", 6, {0x00,{0,0}}, {0x20,1}, {0x0000,0}, {-1,  0, 0}, {-1,   0, 0}},
 {" JPSC ","      ", "A", "+-6.V", 6, {0x00,{0,0}}, {0x40,1}, {0x0000,0}, {-1,  0, 0}, {-1,   0, 0}},
 {" JPSC ","      ", "B", "+-6.V", 6, {0x00,{0,0}}, {0x80,1}, {0x0000,0}, {-1,  0, 0}, {-1,   0, 0}}
};

_DCDC QM_DCDC[32] = {
//~--------------------------------------control----monitor-----monitor------voltage-------current--
//~------------------------------------------w-r-----static-----latched-----------------------------
//~-------------------------------adr--mask--i-i-----mask-i-----mask--i----ind--k--b----ind---k--b--
 {"JPMD-0","JMDC-0", "A", "+3.3V", 0, {0x01,{0,0}}, {0x10,0}, {0x0000,0}, { 0,435, 0}, { 4, 488, 0}},
 {"JPMD-0","JMDC-0", "A", "+5.0V", 0, {0x04,{0,0}}, {0x40,0}, {0x0000,0}, { 1,435, 0}, { 5,3320, 0}},
 {"JPMD-0","JMDC-0", "A", "-5.0V", 0, {0x04,{0,0}}, {0x40,0}, {0x0000,0}, {-1,  0, 0}, { 6,3320, 0}},

 {"JPMD-0","JMDC-1", "B", "+3.3V", 0, {0x02,{0,0}}, {0x20,0}, {0x0000,0}, { 2,435, 0}, { 7, 488, 0}},
 {"JPMD-0","JMDC-1", "B", "+5.0V", 0, {0x08,{0,0}}, {0x80,0}, {0x0000,0}, { 3,435, 0}, { 8,3320, 0}},
 {"JPMD-0","JMDC-1", "B", "-5.0V", 0, {0x08,{0,0}}, {0x80,0}, {0x0000,0}, {-1,  0, 0}, { 9,3320, 0}},

 {"JPMD-1","JMDC-2", "A", "+3.3V", 1, {0x01,{0,0}}, {0x10,0}, {0x0000,0}, {10,435, 0}, {14, 488, 0}},
 {"JPMD-1","JMDC-2", "A", "+5.0V", 1, {0x04,{0,0}}, {0x40,0}, {0x0000,0}, {11,435, 0}, {15,3320, 0}},
 {"JPMD-1","JMDC-2", "A", "-5.0V", 1, {0x04,{0,0}}, {0x40,0}, {0x0000,0}, {-1,  0, 0}, {16,3320, 0}},

 {"JPMD-1","JMDC-3", "B", "+3.3V", 1, {0x02,{0,0}}, {0x20,0}, {0x0000,0}, {12,435, 0}, {17, 488, 0}},
 {"JPMD-1","JMDC-3", "B", "+5.0V", 1, {0x08,{0,0}}, {0x80,0}, {0x0000,0}, {13,435, 0}, {18,3320, 0}},
 {"JPMD-1","JMDC-3", "B", "-5.0V", 1, {0x08,{0,0}}, {0x80,0}, {0x0000,0}, {-1,  0, 0}, {19,3320, 0}},

 {" JPIF "," JFOM ", "A", "+5.0V", 5, {0x00,{0,0}}, {0x01,0}, {0x0000,0}, {26,435, 0}, {29,3320, 0}},
 {" JPIF "," JFOM ", "B", "+5.0V", 5, {0x00,{0,0}}, {0x02,0}, {0x0000,0}, {26,435, 0}, {29,3320, 0}},
 {" JPIF "," JFOM ", "A", "-5.2V", 5, {0x00,{0,0}}, {0x04,0}, {0x0000,0}, {27,435, 0}, {30,1660, 0}},
 {" JPIF "," JFOM ", "B", "-5.2V", 5, {0x00,{0,0}}, {0x08,0}, {0x0000,0}, {27,435, 0}, {30,1660, 0}},
 {" JPIF "," JLIF ", "A", "+5.0V", 5, {0x00,{0,0}}, {0x10,0}, {0x0000,0}, {28,435, 0}, {31,3320, 0}},
 {" JPIF "," JLIF ", "B", "+5.0V", 5, {0x00,{0,0}}, {0x20,0}, {0x0000,0}, {28,435, 0}, {31,3320, 0}},
 {" JPIF "," J422 ", "A", "+5.0V", 5, {0x00,{0,0}}, {0x40,0}, {0x0000,0}, {-1,  0, 0}, {-1,   0, 0}},
 {" JPIF "," J422 ", "B", "+5.0V", 5, {0x00,{0,0}}, {0x80,0}, {0x0000,0}, {-1,  0, 0}, {-1,   0, 0}},

 {" JPJT ","JLV1-A", "A", "+3.3V", 3, {0x01,{0,0}}, {0x10,0}, {0x0000,0}, {-1,  0, 0}, {24,1660, 0}},
 {" JPJT ","JLV1-B", "B", "+3.3V", 3, {0x02,{0,0}}, {0x20,0}, {0x0000,0}, {-1,  0, 0}, {25,1660, 0}},

 {" JPJT ","JINJ-0", "A", "+3.3V", 2, {0x01,{0,0}}, {0x10,0}, {0x0000,0}, {-1,  0, 0}, {20,3320, 0}},
 {" JPJT ","JINJ-1", "B", "+3.3V", 2, {0x02,{0,0}}, {0x20,0}, {0x0000,0}, {-1,  0, 0}, {21,3320, 0}},
 {" JPJT ","JINJ-2", "A", "+3.3V", 2, {0x04,{0,0}}, {0x40,0}, {0x0000,0}, {-1,  0, 0}, {22,3320, 0}},
 {" JPJT ","JINJ-3", "B", "+3.3V", 2, {0x08,{0,0}}, {0x80,0}, {0x0000,0}, {-1,  0, 0}, {23,3320, 0}},

 {" JPSC ","      ", "A", " 28V ", 4, {0x00,{0,0}}, {0x01,1}, {0x0000,0}, {-1,  0, 0}, {-1,   0, 0}},
 {" JPSC ","      ", "B", " 28V ", 4, {0x00,{0,0}}, {0x02,1}, {0x0000,0}, {-1,  0, 0}, {-1,   0, 0}},
 {" JPSC ","      ", "A", "+5.0V", 4, {0x00,{0,0}}, {0x10,0}, {0x0000,0}, {-1,  0, 0}, {-1,   0, 0}},
 {" JPSC ","      ", "B", "+5.0V", 4, {0x00,{0,0}}, {0x20,0}, {0x0000,0}, {-1,  0, 0}, {-1,   0, 0}},
 {" JPSC ","      ", "A", "+-6.V", 4, {0x00,{0,0}}, {0x40,0}, {0x0000,0}, {-1,  0, 0}, {-1,   0, 0}},
 {" JPSC ","      ", "B", "+-6.V", 4, {0x00,{0,0}}, {0x80,0}, {0x0000,0}, {-1,  0, 0}, {-1,   0, 0}}
};

_DCDC FM_DCDC[32] = {
//~--------------------------------------control----monitor-----monitor------voltage-------current--
//~------------------------------------------w-r-----static-----latched-----------------------------
//~-------------------------------adr--mask--i-i-----mask-i-----mask--i----ind--k--b----ind---k--b--
 {"JPMD-0","JMDC-0", "A", "+3.3V", 0, {0x01,{0,0}}, {0x10,0}, {0x0100,0}, { 0,435, 0}, { 4, 488, 0}},
 {"JPMD-0","JMDC-0", "A", "+5.0V", 0, {0x04,{0,0}}, {0x40,0}, {0x0400,0}, { 1,435, 0}, { 5,3320, 0}},
 {"JPMD-0","JMDC-0", "A", "-5.0V", 0, {0x04,{0,0}}, {0x40,0}, {0x0400,0}, {-1,  0, 0}, { 6,3320, 0}},

 {"JPMD-0","JMDC-1", "B", "+3.3V", 0, {0x02,{0,0}}, {0x20,0}, {0x0200,0}, { 2,435, 0}, { 7, 488, 0}},
 {"JPMD-0","JMDC-1", "B", "+5.0V", 0, {0x08,{0,0}}, {0x80,0}, {0x0800,0}, { 3,435, 0}, { 8,3320, 0}},
 {"JPMD-0","JMDC-1", "B", "-5.0V", 0, {0x08,{0,0}}, {0x80,0}, {0x0800,0}, {-1,  0, 0}, { 9,3320, 0}},

 {"JPMD-1","JMDC-2", "A", "+3.3V", 1, {0x01,{0,0}}, {0x10,0}, {0x0100,0}, {10,435, 0}, {14, 488, 0}},
 {"JPMD-1","JMDC-2", "A", "+5.0V", 1, {0x04,{0,0}}, {0x40,0}, {0x0400,0}, {11,435, 0}, {15,3320, 0}},
 {"JPMD-1","JMDC-2", "A", "-5.0V", 1, {0x04,{0,0}}, {0x40,0}, {0x0400,0}, {-1,  0, 0}, {16,3320, 0}},

 {"JPMD-1","JMDC-3", "B", "+3.3V", 1, {0x02,{0,0}}, {0x20,0}, {0x0200,0}, {12,435, 0}, {17, 488, 0}},
 {"JPMD-1","JMDC-3", "B", "+5.0V", 1, {0x08,{0,0}}, {0x80,0}, {0x0800,0}, {13,435, 0}, {18,3320, 0}},
 {"JPMD-1","JMDC-3", "B", "-5.0V", 1, {0x08,{0,0}}, {0x80,0}, {0x0800,0}, {-1,  0, 0}, {19,3320, 0}},

 {" JPIF "," JFOM ", "A", "+5.0V", 5, {0x00,{0,0}}, {0x01,0}, {0x0100,0}, {26,435, 0}, {29,3320, 0}},
 {" JPIF "," JFOM ", "B", "+5.0V", 5, {0x00,{0,0}}, {0x02,0}, {0x0200,0}, {26,435, 0}, {29,3320, 0}},
 {" JPIF "," JFOM ", "A", "-5.2V", 5, {0x00,{0,0}}, {0x04,0}, {0x0400,0}, {27,435, 0}, {30,1660, 0}},
 {" JPIF "," JFOM ", "B", "-5.2V", 5, {0x00,{0,0}}, {0x08,0}, {0x0800,0}, {27,435, 0}, {30,1660, 0}},
 {" JPIF "," JLIF ", "A", "+5.0V", 5, {0x00,{0,0}}, {0x10,0}, {0x1000,0}, {28,435, 0}, {31,3320, 0}},
 {" JPIF "," JLIF ", "B", "+5.0V", 5, {0x00,{0,0}}, {0x20,0}, {0x2000,0}, {28,435, 0}, {31,3320, 0}},
 {" JPIF "," J422 ", "A", "+5.0V", 5, {0x00,{0,0}}, {0x40,0}, {0x4000,0}, {-1,  0, 0}, {-1,   0, 0}},
 {" JPIF "," J422 ", "B", "+5.0V", 5, {0x00,{0,0}}, {0x80,0}, {0x8000,0}, {-1,  0, 0}, {-1,   0, 0}},

 {" JPJT ","JLV1-A", "A", "+3.3V", 3, {0x01,{0,0}}, {0x10,0}, {0x0040,0}, {-1,  0, 0}, {24,1660, 0}},
 {" JPJT ","JLV1-B", "B", "+3.3V", 3, {0x02,{0,0}}, {0x20,0}, {0x0080,0}, {-1,  0, 0}, {25,1660, 0}},

 {" JPJT ","JINJ-0", "A", "+3.3V", 2, {0x01,{0,0}}, {0x10,0}, {0x0100,0}, {-1,  0, 0}, {20,3320, 0}},
 {" JPJT ","JINJ-1", "B", "+3.3V", 2, {0x02,{0,0}}, {0x20,0}, {0x0200,0}, {-1,  0, 0}, {21,3320, 0}},
 {" JPJT ","JINJ-2", "A", "+3.3V", 2, {0x04,{0,0}}, {0x40,0}, {0x0400,0}, {-1,  0, 0}, {22,3320, 0}},
 {" JPJT ","JINJ-3", "B", "+3.3V", 2, {0x08,{0,0}}, {0x80,0}, {0x0800,0}, {-1,  0, 0}, {23,3320, 0}},

 {" JPSC ","      ", "A", " 28V ", 4, {0x00,{0,0}}, {0x01,1}, {0x0000,0}, {-1,  0, 0}, {-1,   0, 0}},
 {" JPSC ","      ", "B", " 28V ", 4, {0x00,{0,0}}, {0x02,1}, {0x0000,0}, {-1,  0, 0}, {-1,   0, 0}},
 {" JPSC ","      ", "A", "+5.0V", 4, {0x00,{0,0}}, {0x10,0}, {0x0100,0}, {-1,  0, 0}, {-1,   0, 0}},
 {" JPSC ","      ", "B", "+5.0V", 4, {0x00,{0,0}}, {0x20,0}, {0x0200,0}, {-1,  0, 0}, {-1,   0, 0}},
 {" JPSC ","      ", "A", "+-6.V", 4, {0x00,{0,0}}, {0x40,0}, {0x0400,0}, {-1,  0, 0}, {-1,   0, 0}},
 {" JPSC ","      ", "B", "+-6.V", 4, {0x00,{0,0}}, {0x80,0}, {0x0800,0}, {-1,  0, 0}, {-1,   0, 0}}
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct {
  struct {
    int   unk;
    int   sta;
    int16 dat;
    float val;
  } DS1820[256];
} bus[2];

FL_OBJECT *Dallas_tmp_BOX[12][2];

FL_OBJECT *USCM_TMP1, *USCM_TMP2;
FL_OBJECT *USCM_tmp_BOX;

int Dallas_bus;

int64 USCM_Dallas_ID;

int   USCM_tmp_unk;
int   USCM_tmp_sta;
float USCM_tmp_val;

typedef struct {
  char *JPD_board;
  char *area;
  int64 ID[2];
} _Dallas;

_Dallas *Dallas;

_Dallas EM_Dallas[12] = {
//~-----------------+--- --1 Bus A-----+--+--- --1 Bus B-----+
 {"JPMD-0","\"A\"",{0x64000800441C9510LL, 0x6000080044147B10LL}},
 {"JPMD-0","\"B\"",{0x0000000000000000LL, 0xB10008004430FF10LL}},

 {" JPSC ","\"A\"",{0x0000000000000000LL, 0x7D00080016024110LL}},
 {" JPSC ","\"B\"",{0x630008004433A910LL, 0x0300080016048D10LL}},

 {" JPJT ","\"A\"",{0x6F00080044159E10LL, 0x13000800440A1910LL}},
 {" JPJT ","\"B\"",{0x3C00080015F49010LL, 0xF8000800441B8C10LL}},

 {"USCM-A","\"A\"",{0x0000000000000000LL, 0x0000000000000000LL}},
 {"USCM-B","\"B\"",{0x0000000000000000LL, 0x0000000000000000LL}},

 {" JPIF ","\"A\"",{0x1C00080044309D10LL, 0x75000800441BC710LL}},
 {" JPIF ","\"B\"",{0x5900080044306C10LL, 0xB3000800442FE210LL}},

 {"JPMD-1","\"A\"",{0x1100080015F54F10LL, 0x0000000000000000LL}},
 {"JPMD-1","\"B\"",{0x85000800441DD610LL, 0x0000000000000000LL}}
};

_Dallas EM2_Dallas[12] = {
//~-----------------+--- --1 Bus A-----+--+--- --1 Bus B-----+
  {"JPMD-0","\"A\"",{0x5F000800501F5110LL, 0xA500080050414810LL}},
  {"JPMD-0","\"B\"",{0x810008005053FC10LL, 0x6D00080050563410LL}},

  {" JPSC ","\"A\"",{0x4F0008005041C810LL, 0x02000800501D4610LL}},
  {" JPSC ","\"B\"",{0xF3000800504B4D10LL, 0x180008005040AB10LL}},

  {" JPJT ","\"A\"",{0xAE00080050574D10LL, 0x8800080050533510LL}},
  {" JPJT ","\"B\"",{0x230008005020DD10LL, 0xEE00080050526210LL}},

  {"USCM-A","\"A\"",{0x0000000000000000LL, 0x0000000000000000LL}},
  {"USCM-B","\"B\"",{0x0000000000000000LL, 0x0000000000000000LL}},

  {" JPIF ","\"A\"",{0x4100080050486010LL, 0x19000800502C9F10LL}},
  {" JPIF ","\"B\"",{0x9D00080050486410LL, 0x9700080050363110LL}},

  {"JPMD-1","\"A\"",{0x4F00080035DB8A10LL, 0x79000800502BAB10LL}},
  {"JPMD-1","\"B\"",{0x04000800502FC310LL, 0xE400080035E5C110LL}}
};

_Dallas QM_Dallas[12] = {
//~-----------------+--- --1 Bus A-----+--+--- --1 Bus B-----+
 {"JPMD-0","\"A\"",{0xC200080015E70C10LL, 0xCF00080015EF2B10LL}},
 {"JPMD-0","\"B\"",{0x0700080015FC3310LL, 0xDC00080015E6F410LL}},

 {" JPSC ","\"A\"",{0xEA00080044182C10LL, 0x7C000800441A1710LL}},
 {" JPSC ","\"B\"",{0xE900080044341D10LL, 0xDC00080044082110LL}},

 {" JPJT ","\"A\"",{0xFF000800440BB510LL, 0x72000800442BF510LL}},
 {" JPJT ","\"B\"",{0xF000080044302810LL, 0xAC000800442DB410LL}},

 {"USCM-A","\"A\"",{0x0000000000000000LL, 0x0000000000000000LL}},
 {"USCM-B","\"B\"",{0x0000000000000000LL, 0x0000000000000000LL}},

 {" JPIF ","\"A\"",{0x5B000800442B1510LL, 0xBA0008004425D110LL}},
 {" JPIF ","\"B\"",{0xE00008004431A910LL, 0x7300080043FF3610LL}},

 {"JPMD-1","\"A\"",{0x53000800160D9610LL, 0x8000080016081310LL}},
 {"JPMD-1","\"B\"",{0xA800080015FC4210LL, 0x3700080015E7E810LL}}
};

_Dallas FMA_Dallas[12] = {
//~-----------------+--- --1 Bus A-----+--+--- --1 Bus B-----+
 {"JPMD-0","\"A\"",{0xD800080035C25E10LL, 0x8B0008003597E010LL}},   // #06
 {"JPMD-0","\"B\"",{0x1A00080036043F10LL, 0x7E00080035EA2510LL}},   // #06
 
 {" JPSC ","\"A\"",{0x3C000800367D6610LL, 0x9C00080035E47710LL}},   // #03
 {" JPSC ","\"B\"",{0x9F00080035B33110LL, 0xAA000800364DED10LL}},   // #03

 {" JPJT ","\"A\"",{0x3800080035967A10LL, 0x8000080035BDCB10LL}},   // #02
 {" JPJT ","\"B\"",{0x8500080035AFD310LL, 0x2100080036918C10LL}},   // #02

 {"USCM-A","\"A\"",{0x0000000000000000LL, 0x0000000000000000LL}},
 {"USCM-B","\"B\"",{0x0000000000000000LL, 0x0000000000000000LL}},

 {" JPIF ","\"A\"",{0x8D0008003596CC10LL, 0x4000080035F4FA10LL}},   // #03
 {" JPIF ","\"B\"",{0xEF00080036513910LL, 0xF600080036483010LL}},   // #03

 {"JPMD-1","\"A\"",{0xB1000800361E1B10LL, 0x7B00080036766710LL}},   // #07
 {"JPMD-1","\"B\"",{0xC100080035F28D10LL, 0xA500080036354D10LL}}    // #07
};

_Dallas FMB_Dallas[12] = {
//~-----------------+--- --1 Bus A-----+--+--- --1 Bus B-----+
 {"JPMD-0","\"A\"",{0x09000800368FFE10LL, 0xB0000800368E3B10LL}},   // #04
 {"JPMD-0","\"B\"",{0xE90008003687E910LL, 0xE800080036065910LL}},   // #04

 {" JPSC ","\"A\"",{0x3000080035B51610LL, 0xFE00080035B3CF10LL}},   // #02
 {" JPSC ","\"B\"",{0x5600080036641A10LL, 0x1500080035C9A410LL}},   // #02

 {" JPJT ","\"A\"",{0x7000080035C4C910LL, 0x9F0008003690F910LL}},   // #03
 {" JPJT ","\"B\"",{0xD2000800361EF610LL, 0xA900080035A33C10LL}},   // #03

 {"USCM-A","\"A\"",{0x0000000000000000LL, 0x0000000000000000LL}},
 {"USCM-B","\"B\"",{0x0000000000000000LL, 0x0000000000000000LL}},

 {" JPIF ","\"A\"",{0x95000800364F9210LL, 0x500008003671DB10LL}},   // #02
 {" JPIF ","\"B\"",{0x9600080036110510LL, 0xC800080035C0F410LL}},   // #02

 {"JPMD-1","\"A\"",{0xB8000800362E5010LL, 0xF5000800360DCD10LL}},   // #05
 {"JPMD-1","\"B\"",{0xE900080036111A10LL, 0x14000800363B4310LL}}    // #05
};

#define N_MOD 5
struct {
  char *nam;
  _DCDC *DCDC;
  _Dallas *Dallas;
} JPD_mods[N_MOD] = {

  {"EM",  EM_DCDC, EM_Dallas },
  {"EM2", FM_DCDC, EM2_Dallas},
  {"QM",  QM_DCDC, QM_Dallas },
  {"FMA", FM_DCDC, FMA_Dallas},
  {"FMB", FM_DCDC, FMB_Dallas}
};  

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FD_JPD *create_form_JPD( void);

void create_JPD_mod_panel(int x0, int y0);
void create_DCDC_panel(int x0, int y0);
void create_USCM_panel(int x0, int y0);
void create_monitor_panel(int x0, int y0);
void create_temperature_panel(int x0, int y0);

void setup_JPD_data_structures(void);
void read_all_data();
void update_objects(void);
void start_JPD(void);
void TIMER_callback(FL_OBJECT *obj, long data);
void command_write_LVDS_bus(int8 adr, int16 dat, int16 *sta);

char *mytime(void);

//~============================================================================

FD_JPD *create_form_JPD(void) {

  FL_OBJECT *obj;
  FD_JPD *fdui = (FD_JPD *) fl_calloc(1, sizeof(*fdui));

  fdui->JPD = fl_bgn_form(FL_NO_BOX,700,680);

  obj = fl_add_box(FL_FLAT_BOX,0,0,700,680,"A.Lebedev 26-Oct-08");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_TOP_RIGHT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESITALIC_STYLE);

  obj = fl_add_box(FL_NO_BOX,165,5,530,45,"JPD  Pro  Controller");
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_EMBOSSED_STYLE);
  
  create_USCM_JMDC_panels();
  create_JPD_mod_panel(    125,   5);
  create_DCDC_panel(         5,  50);
  create_USCM_panel(         5, 570);
  create_monitor_panel(    350, 570);
  create_temperature_panel(510,  50);
  create_P_panel(          670, 625);

  MESSAGE = 
  obj = fl_add_text(FL_NORMAL_TEXT,5,635,505,40,"");
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLDITALIC_STYLE+FL_ENGRAVED_STYLE);
    fl_hide_object(obj);

#ifdef LOGFILE
  WRITING_LOG_FILE =
  obj = fl_add_button(FL_PUSH_BUTTON,450,55+18*32+5,525,30,"");
    fl_set_object_color(obj,FL_CYAN,FL_RED);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE+FL_ENGRAVED_STYLE);
    fl_set_button(obj,0);
    fl_set_object_callback(obj, swtch_callback, 0);
#endif

  obj = fl_add_timer(FL_HIDDEN_TIMER, 0, 0, 0, 0,"");
    fl_set_timer(obj, (float)timer_period);
    fl_set_object_callback(obj, TIMER_callback, 0);

  fl_end_form();
  fdui->JPD->fdui = fdui;

  return fdui;
}

//~-------------------------------------------------------------------------------

void setup_JPD_data_structures(void) {

  int i, j;

  boot_mode = 1;
  
  DCDC   = JPD_mods[JPD_mod].DCDC;
  Dallas = JPD_mods[JPD_mod].Dallas;

  for (i=0; i<256; i++) {
    LVDS[i].rea.unk = 1;
    LVDS[i].rea.sta = status_NA;
    LVDS[i].rea.acc = 0x0000;
    LVDS[i].rea.inv = 0x0000;
    LVDS[i].wri.sta = status_NA;
    LVDS[i].wri.inp = 0x0000;
    LVDS[i].wri.inv = 0x0000;
  }

//printf("setup_JPD_data_structures:\n");
  for (i=0; i<32; i++) {
    int adr = DCDC[i].adr;
    if (adr >= 0) {
      if (DCDC[i].ctr.inv.wri) LVDS[adr].wri.inv |= DCDC[i].ctr.msk;
      if (DCDC[i].ctr.inv.rea) LVDS[adr].rea.inv |= DCDC[i].ctr.msk;
      if (DCDC[i].mon.inv)     LVDS[adr].rea.inv |= DCDC[i].mon.msk;
      if (DCDC[i].dir.inv)     LVDS[adr].rea.inv |= DCDC[i].dir.msk;
    }
//  printf("LVDS[%2d].wri.inv = %04X, ", adr, LVDS[adr].wri.inv);
//  printf("LVDS[%2d].rea.inv = %04X\n", adr, LVDS[adr].rea.inv);
  }

  for (i=0; i<32; i++) {
    ADC[i].unk = 1;
    ADC[i].sta = status_NA;
  }

  Dallas_bus = 0;

  USCM_tmp_unk = 1;
  USCM_tmp_sta = status_NA;

  for (j=0; j<2; j++) {
    for (i=0; i<12; i++) {
      bus[j].DS1820[i].unk = 1;
      bus[j].DS1820[i].sta = status_NA;
    }
  }
}

//~============================================================================

void JPD_txt_callback(FL_OBJECT *obj, long data);
void JPD_mod_callback(FL_OBJECT *obj, long data);
void update_JPD_mod_panel(void);
void update_JPD_mod_config_file(void);

FL_OBJECT *JPD_MOD;

char *JPD_mod_config_file_name = {"JPD-mod.configfile"};

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_JPD_mod_panel(int x0, int y0) {

  FL_OBJECT *obj;
  FILE *file;
  int x, y;
  
  JPD_mod = 1;
  file = fopen(JPD_mod_config_file_name, "r");
  if (file != NULL) {
    fread(&JPD_mod, sizeof(JPD_mod), 1, file);
    fclose(file);
  }
  setup_JPD_data_structures();

  x = x0;
  y = y0;
  
  obj = fl_add_box(FL_DOWN_BOX,x,y,45,45,"");
  
  x = x0 + 5;
  y = y0;

  obj = fl_add_button(FL_NORMAL_BUTTON,x,y,35,20,"JPD");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_callback(obj, JPD_txt_callback, 0);

  x = x0 + 5;
  y = y0 + 20;
  
  JPD_MOD =
  obj = fl_add_button(FL_NORMAL_BUTTON,x,y,35,20,"");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_WHEAT,FL_RED);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lstyle(obj,FL_FIXED_STYLE+FL_ENGRAVED_STYLE);
    fl_set_object_callback(obj, JPD_mod_callback, 0);
}

//~-------------------------------------------------------------------------------

void JPD_txt_callback(FL_OBJECT *obj, long data) {

  JPD_mod = 0;
  
  setup_JPD_data_structures();
  update_JPD_mod_config_file();
  update_objects();
}

//~-------------------------------------------------------------------------------

void JPD_mod_callback(FL_OBJECT *obj, long data) {

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  JPD_mod = MOD(JPD_mod+1, N_MOD);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) JPD_mod = MOD(JPD_mod-1, N_MOD);

  setup_JPD_data_structures();
  update_JPD_mod_config_file();
  update_objects();
}

//~-------------------------------------------------------------------------------

void update_JPD_mod_panel(void) {

  FL_OBJECT *obj = JPD_MOD;

  if (!obj) return;

  if (JPD_mod < N_MOD) fl_set_object_label(obj, JPD_mods[JPD_mod].nam);
  else                 fl_set_object_label(obj, "?");

  if (Dallas_bus < 0) {
    fl_set_object_color (obj, FL_RED, 0);
    fl_set_object_lcolor(obj, FL_YELLOW);
  }
  else {
    fl_set_object_color (obj, FL_WHEAT, 0);
    fl_set_object_lcolor(obj, FL_BLACK);
  }
}

//~-------------------------------------------------------------------------------

void update_JPD_mod_config_file(void) {

  FILE *file;

  file = fopen(JPD_mod_config_file_name, "w");
  if (file != NULL) {
    fwrite(&JPD_mod, sizeof(JPD_mod), 1, file);
    fclose(file);
  }
}

//~============================================================================

void PERIOD_callback(FL_OBJECT *obj, long data);
void MONITOR_callback(FL_OBJECT *obj, long data);
void update_monitor_panel();

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_monitor_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w;
  int wa = 45, w1 = 25, w2 = 80, h = 50;

  w = 10 + wa + w1 + w2;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, 10+h, "");
  
  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, wa, h, "Period\nsec", FL_NORMAL_SIZE);

  x = x + wa;
  PERIOD = 
  obj = add_switch_button(x, y, w1, h, "?", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, PERIOD_callback, 0);

  x = x + w1;
  MONITOR =
  obj = add_normal_button(x, y, w2, h, "MONITOR", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, MONITOR_callback, 0);
}

//~----------------------------------------------------------------------------

void PERIOD_callback(FL_OBJECT *obj, long data) {

#if 0
  int t = timer_period - 1;

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  t = MOD(t+1, 9);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) t = MOD(t-1, 9);
  timer_period = t + 1;
#else
  int t = timer_period;

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  t = MOD(t+1, 10);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) t = MOD(t-1, 10);
  timer_period = t;
#endif


  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void MONITOR_callback(FL_OBJECT *obj, long data) {

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    if (monitoring) monitoring = 0;
    else            read_all_data();
  }
    
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) monitoring = 1 - monitoring;

  if (monitoring) {
    fl_set_object_boxtype(obj, FL_DOWN_BOX);
    fl_set_object_color(obj, FL_RED, FL_RED);
    fl_set_object_lcolor(obj, FL_YELLOW);
  }
  else {
    fl_set_object_boxtype(obj, FL_UP_BOX);
    fl_set_object_color(obj, FL_CYAN, FL_RED);
    fl_set_object_lcolor(obj, FL_BLACK);
  }
  
  update_objects();
}

//~----------------------------------------------------------------------------

void update_monitor_panel(void) {

  char b[80];
  
  sprintf(b, "%d", timer_period);
  fl_set_object_label(PERIOD, b);
}

//~============================================================================

void update_temperature_panel(void);

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_temperature_panel(int x0, int y0) {

  int i;
  int x, y, w;
  int wa = 50, wb = 25, w1 = 50, h = 15;
  int size;
  
  size = FL_SMALL_SIZE;
  
  w = 10 + wa + wb + 2 * w1;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, 10+2*h+12*h, "");
  
  x = x0 + 5;
  y = y0 + 5;
  w = wa + wb;
  add_frame_box(x, y, w,  2*h, "Board Area", size);
  x = x + w;
  add_frame_box(x, y, w1, 2*h, "Temp",       size);
  x = x + w1;
  add_frame_box(x, y, w1, 2*h, "Temp",       size);
  
  y = y0 + 5 + 2 * h;
  for (i=0; i<12; i++) {
    size = FL_SMALL_SIZE;
    x = x0 + 5;
    if (i != 6 && i != 7 && i % 2 == 0)
      add_frame_box(x, y, wa,    2*h, Dallas[i].JPD_board,  size);
    if (i == 6) {
      USCM_TMP1 = fl_bgn_group();
      add_frame_box(x, y, wa+wb, 2*h, "USCM",               size);
      fl_end_group();
    }
    if (i == 6) USCM_TMP2 = fl_bgn_group();
    if (i == 7)             fl_addto_group(USCM_TMP2);
    if (i == 6 || i == 7)
      add_frame_box(x, y, wa+wb,   h, Dallas[i].JPD_board,  size);
    if (i == 6 || i == 7)   fl_end_group();
    x = x + wa;
    if (i != 6 && i != 7)
      add_frame_box(x, y, wb,      h, Dallas[i].area,       size);
    x = x + wb;
    size = FL_NORMAL_SIZE;
    if (i == 6) {
      fl_addto_group(USCM_TMP1);
      USCM_tmp_BOX       = add_data_box(x, y, 2*w1, 2*h, "t", FL_HUGE_SIZE);
      fl_end_group();
    }
    if (i == 6 || i == 7) fl_addto_group(USCM_TMP2);
    Dallas_tmp_BOX[i][0] = add_data_box(x, y,   w1,   h, "t", size);
    x = x + w1;
    Dallas_tmp_BOX[i][1] = add_data_box(x, y,   w1,   h, "t", size);
    x = x + w1;
    if (i == 6 || i == 7) fl_end_group();
    y = y + h;
  }
}

//~----------------------------------------------------------------------------

void update_temperature_panel(void) {

  FL_OBJECT *obj;
  int i, j;
  char b[80];
  int               good = 0, warning = 1, alarm = 2, error = 3;
  int colors[4]  = {FL_GREEN, FL_YELLOW,   FL_RED,    FL_BLACK};
  int lcolors[4] = {FL_BLACK, FL_BLACK,    FL_YELLOW, FL_WHITE};
  int color;

  if (Dallas_bus < 0) {
    fl_hide_object(USCM_TMP2);
    fl_show_object(USCM_TMP1);
  }
  else {
    fl_hide_object(USCM_TMP1);
    fl_show_object(USCM_TMP2);
  }

  if (USCM_tmp_unk) sprintf(b, " ");
  else              sprintf(b, "%+5.1f", USCM_tmp_val);
  
  switch (USCM_tmp_sta) {
    case status_OK: color = good;    break;
    case status_KO: color = error;   break;
    case status_NA: color = warning; break;
    default:        color = warning; break;
  }
  if ((obj = USCM_tmp_BOX)) {
    fl_set_object_label(obj, b);
    fl_set_object_color (obj,  colors[color], 0);
    fl_set_object_lcolor(obj, lcolors[color]);
  }

  for (j=0; j<2; j++) {
    for (i=0; i<12; i++) {
      if ((i == 6 && j == 1) || (i == 7 && j == 0)) {
        sprintf(b, "-");
        color = good;
      }
      else {
        if (bus[j].DS1820[i].unk) sprintf(b, " ");
        else                      sprintf(b, "%+6.2f", bus[j].DS1820[i].val);
        switch (bus[j].DS1820[i].sta) {
          case status_OK: color = good;    break;
          case status_KO: color = error;   break;
          case status_NA: color = warning; break;
          default:        color = alarm;   break;
        }
      }
      if ((obj = Dallas_tmp_BOX[i][j])) {
        fl_set_object_label(obj, b);
        fl_set_object_color (obj,  colors[color], 0);
        fl_set_object_lcolor(obj, lcolors[color]);
      }
    }
  }
}

//~============================================================================

void reset_accumulated_status_callback(FL_OBJECT *obj, long data);
void hide_code_callback(FL_OBJECT *obj, long data);
void create_JPMD_panel(int x0, int y0, int cha);
void create_JPIF_panel(int x0, int y0, int cha);
void create_JPJT_panel(int x0, int y0, int cha);
void create_JPSC_panel(int x0, int y0, int cha);

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_DCDC_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w;
  int w1 = 30, w2 = 40, w3 = 10, w4 = 30, w5 = 40;
  int wa = 55, wb = 50, wc = 15, wd = 35;
  int h = 15;
  int size = FL_SMALL_SIZE;

  w = 10 + wa + wb + wc + wd + 2 * w1 + w3 + w1 + 4 * w2 + w3 + w4 + w5;
  fl_add_box(FL_DOWN_BOX, x0, y0, w, 10+2*h+32*h, "");

  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, wa, 2*h, "", size);
  x = x + wa;
  w = wb + wc + wd;
  add_frame_box(x, y,    w, 2*h, "", size);
  x = x + w;
  w = 2 * w1 + w3 + w1;
  add_frame_box(x, y, w, h, "Status", size);
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, w, 2*h, "");
    fl_set_object_callback(obj, reset_accumulated_status_callback, 0);
  add_frame_box(x,         y+h, w1,    h, "Ctr", size);
  add_frame_box(x+w1,      y+h, w1+w3, h, "Mon", size);
  add_frame_box(x+2*w1+w3, y+h, w1,    h, "Dir", size);
  x = x + w;
  VOL1 = fl_bgn_group();
    add_frame_box(x,      y, 2*w2, h, "Voltage", size);
    add_frame_box(x,    y+h,   w2, h, "Code", size);
    add_frame_box(x+w2, y+h,   w2, h, "Value", size);
    obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, 2*w2, h, "");
      fl_set_object_callback(obj, hide_code_callback, 0);
  fl_end_group();
  VOL2 = fl_bgn_group();
    add_frame_box(x,      y, 2*w2, 2*h, "Voltage", size);
    obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, 2*w2, 2*h, "");
      fl_set_object_callback(obj, hide_code_callback, 0);
  fl_end_group();
  x = x + 2 * w2;
  CUR1 = fl_bgn_group();
    add_frame_box(x,      y, 2*w2, h, "Current", size);
    add_frame_box(x,    y+h,   w2, h, "Code", size);
    add_frame_box(x+w2, y+h,   w2, h, "Value", size);
    obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, 2*w2, h, "");
      fl_set_object_callback(obj, hide_code_callback, 1);
  fl_end_group();
  CUR2 = fl_bgn_group();
    add_frame_box(x,      y, 2*w2, 2*h, "Current", size);
    obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, 2*w2, 2*h, "");
      fl_set_object_callback(obj, hide_code_callback, 1);
  fl_end_group();
  x = x + 2 * w2;
  w = w3 + w4 + w5;
  add_frame_box(x, y, w, 2*h, "Command", size);

  x = x0 + 5;
  y = y0 + 5 + 2 * h;
  create_JPMD_panel(x,      y,  0);
  create_JPMD_panel(x,  y+6*h,  6);
  create_JPIF_panel(x, y+12*h, 12);
  create_JPJT_panel(x, y+20*h, 20);
  create_JPSC_panel(x, y+26*h, 26);
}

//~----------------------------------------------------------------------------

void reset_accumulated_status_callback(FL_OBJECT *obj, long data) {

  int i;
  
  for (i=0; i<256; i++) {
    LVDS[i].rea.acc = 0x0000;
  }

  update_objects();
}

//~----------------------------------------------------------------------------

void hide_code_callback(FL_OBJECT *obj, long data) {

  if (data) hide_current_code = 1 - hide_current_code;
  else      hide_voltage_code = 1 - hide_voltage_code;

  update_objects();
}

//~============================================================================

void SND_callback(FL_OBJECT *obj, long data);
void JPMD_ON_OFF_callback(FL_OBJECT *obj, long data);
void update_JPMD_panel(int cha);

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_JPMD_panel(int x0, int y0, int cha) {

  FL_OBJECT *obj;
  int i, j;
  int x, y, w;
  int w1 = 30, w2 = 40, w3 = 10, w4 = 30, w5 = 40;
  int wa = 55, wb = 50, wc = 15, wd = 35;
  int h = 15;
  int size;

  size = FL_NORMAL_SIZE;

  x = x0;
  y = y0;
  add_frame_box(x, y, wa, 6*h, DCDC[cha].JPD_board, size);

  size = FL_SMALL_SIZE;

  for (i=0, j=cha; i<6; i++, j++) {
    x = x0 + wa;
    if (i % 3 == 0) add_frame_box(x, y, wb, 3*h, DCDC[j].J_JT_board, size);
    x = x + wb;
    if (i % 3 == 0) add_frame_box(x, y, wc, 3*h, DCDC[j].feed,       size);
    x = x + wc;
                    add_frame_box(x, y, wd,   h, DCDC[j].nominal,    size);
    y = y + h;
  }

  size = FL_NORMAL_SIZE;

  y = y0;
  w = wa + wb + wc + wd;
  for (i=0, j=cha; i<6; i++, j++) {
    x = x0 + w;
    if (i % 3 == 0) DCDC_ctr_BOX[j] = add_data_box(x, y, w1,   h, "0", size);
    if (i % 3 == 1) DCDC_ctr_BOX[j] = add_data_box(x, y, w1, 2*h, "0", size);
    x = x + w1;
    if (i % 3 == 0) DCDC_mon_BOX[j] = add_data_box(x, y, w1,   h, "1", size);
    if (i % 3 == 1) DCDC_mon_BOX[j] = add_data_box(x, y, w1, 2*h, "1", size);
    x = x + w1;
    if (i % 3 == 0) DCDC_acc_BOX[j] = add_data_box(x, y, w3,   h, "", size);
    if (i % 3 == 1) DCDC_acc_BOX[j] = add_data_box(x, y, w3, 2*h, "", size);
    x = x + w3;
    if (i % 3 == 0) DCDC_dir_BOX[j] = add_data_box(x, y, w1,   h, "", size);
    if (i % 3 == 1) DCDC_dir_BOX[j] = add_data_box(x, y, w1, 2*h, "", size);
    x = x + w1;
    fl_addto_group(VOL1);
      DCDC_vol_cod_BOX[j]  = add_data_box(x,      y, w2,   h, "1", size);
      DCDC_vol_val_BOX1[j] = add_data_box(x+w2,   y, w2,   h, "1", size);
    fl_end_group();
    fl_addto_group(CUR1);
      DCDC_cur_cod_BOX[j]  = add_data_box(x+2*w2, y, w2,   h, "2", size);
      DCDC_cur_val_BOX1[j] = add_data_box(x+3*w2, y, w2,   h, "2", size);
    fl_end_group();
    fl_addto_group(VOL2);
      DCDC_vol_val_BOX2[j] = add_data_box(x,      y, 2*w2, h, "3", size);
    fl_end_group();
    fl_addto_group(CUR2);
      DCDC_cur_val_BOX2[j] = add_data_box(x+2*w2, y, 2*w2, h, "3", size);
    fl_end_group();
    x = x + 4 * w2;
    if (i % 3 == 0) DCDC_ctr_WRI[j] = add_data_box(x, y, w3,   h, "", size);
    if (i % 3 == 1) DCDC_ctr_WRI[j] = add_data_box(x, y, w3, 2*h, "", size);
    x = x + w3;
    if (i % 3 == 0) {
      DCDC_ctr_INP[j] =
      obj = add_switch_button(x, y, w4,   h, "OFF", size);
        fl_set_object_callback(obj, JPMD_ON_OFF_callback, j);
    }
    if (i % 3 == 1) {
      DCDC_ctr_INP[j] =
      obj = add_switch_button(x, y, w4, 2*h, "OFF", size);
        fl_set_object_callback(obj, JPMD_ON_OFF_callback, j);
    }
    x = x + w4;
    if (i == 0) {
      DCDC_ctr_SND[j] =
      obj = add_normal_button(x, y, w5, 6*h, "SND", size);
        fl_set_object_callback(obj, SND_callback, j);
    }
    y = y + h;
  }
}

//~----------------------------------------------------------------------------

void JPMD_ON_OFF_callback(FL_OBJECT *obj, long data) {

  int cha, adr, bit;

  cha = data;
  adr = DCDC[cha].adr;
  LVDS[adr].wri.inp = LVDS[adr].wri.inp ^ DCDC[cha].ctr.msk;
  bit = LVDS[adr].wri.inp & DCDC[cha].ctr.msk;

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE) {
    cha = cha / 3 * 3 + 1 - cha % 3;
    if (bit) LVDS[adr].wri.inp = LVDS[adr].wri.inp |  DCDC[cha].ctr.msk;
    else     LVDS[adr].wri.inp = LVDS[adr].wri.inp & ~DCDC[cha].ctr.msk;
  }

  msg = 0;
  update_objects();
}

//~----------------------------------------------------------------------------

void SND_callback(FL_OBJECT *obj, long data) {

  int cha = data;
  int adr = DCDC[cha].adr;

  LVDS[adr].wri.dat = LVDS[adr].wri.inp;
  LVDS[adr].wri.sta = status_NA;
  LVDS[adr].rea.sta = status_NA;

  msg = -1;
  update_objects();

//printf("SND_callback: %d %04X %04X %04X\n",
//adr, LVDS[adr].wri.dat, LVDS[adr].wri.inv, LVDS[adr].wri.dat ^ LVDS[adr].wri.inv);

  command_write_LVDS_bus(adr, LVDS[adr].wri.dat ^ LVDS[adr].wri.inv, &sta);
  if (sta) LVDS[adr].wri.sta = status_OK;
  else     LVDS[adr].wri.sta = status_KO;

  update_objects();
}

//~----------------------------------------------------------------------------

void update_DCDC_status(int cha, int mode) {

  FL_OBJECT *obj = NULL;
  int               good = 0, warning = 1, alarm = 2, error = 3;
  int colors[4]  = {FL_GREEN, FL_YELLOW,   FL_RED,    FL_BLACK};
  int lcolors[4] = {FL_BLACK, FL_BLACK,    FL_YELLOW, FL_WHITE};
  int color = alarm;
  int adr;
  int ctr_rea = -1, ctr_wri = -1, mon_rea = -1;

  if      (mode == 0) obj = DCDC_ctr_BOX[cha];
  else if (mode == 1) obj = DCDC_mon_BOX[cha];
  else if (mode == 2) obj = DCDC_acc_BOX[cha];
  else if (mode == 3) obj = DCDC_dir_BOX[cha];

  if (!obj) return;

  adr = DCDC[cha].adr;

  if (mode == 0 || mode == 1 || mode == 3) {
    if (adr < 0                              || 
       (mode == 0 && DCDC[cha].ctr.msk == 0) ||
       (mode == 1 && DCDC[cha].mon.msk == 0) ||
       (mode == 3 && DCDC[cha].dir.msk == 0)) {
      fl_set_object_label(obj, "-");
      color = good;
    }
    else {
      if (LVDS[adr].rea.unk) {
        fl_set_object_label(obj, " ");
      }
      else {
        if (DCDC[cha].ctr.msk) {
          ctr_rea = unpack16(LVDS[adr].rea.dat, DCDC[cha].ctr.msk);
          ctr_wri = unpack16(LVDS[adr].wri.dat, DCDC[cha].ctr.msk);
        }
        else {
          ctr_rea = 1;
          ctr_wri = 1;
        }
        if (mode == 3) mon_rea = unpack16(LVDS[adr].rea.dat, DCDC[cha].dir.msk);
        else           mon_rea = unpack16(LVDS[adr].rea.dat, DCDC[cha].mon.msk);
        if (mode == 0) {
          if (ctr_rea) fl_set_object_label(obj, "ON");
          else         fl_set_object_label(obj, "OFF");
        }
        else {
          if (mon_rea) fl_set_object_label(obj, "PG");
          else         fl_set_object_label(obj, "FAI");
        }
      }
      if (LVDS[adr].rea.sta == status_NA) color = warning;
      if (LVDS[adr].rea.sta == status_KO) color = error;
      if (LVDS[adr].rea.sta == status_OK) {
        color = good;
        if (DCDC[cha].ctr.msk && 
            LVDS[adr].wri.sta == status_OK && 
            ctr_rea != ctr_wri) color = warning;
        if (ctr_rea != mon_rea) color = warning;
      }
    }
  }
  
  if (mode == 2) {
    fl_set_object_label(obj, "");
    mon_rea = unpack16(LVDS[adr].rea.acc, DCDC[cha].mon.msk);
    if (mon_rea) color = warning;
    else         color = good;
  }
  
  fl_set_object_color (obj,  colors[color], 0);
  fl_set_object_lcolor(obj, lcolors[color]);
}

//~----------------------------------------------------------------------------

void update_DCDC_voltage(int cha) {

  FL_OBJECT *obj;
  char cod[80], val[80];
  int               good = 0, warning = 1, alarm = 2, error = 3;
  int colors[4]  = {FL_GREEN, FL_YELLOW,   FL_RED,    FL_BLACK};
  int lcolors[4] = {FL_BLACK, FL_BLACK,    FL_YELLOW, FL_WHITE};
  int color;
  int ind;
    
  ind = DCDC[cha].vol.ind;
  
  if (ind < 0) {
    sprintf(cod, "-");
    sprintf(val, "-");
    color = good;
  }
  else {
    if (ADC[ind].unk) sprintf(cod, " ");
    else              sprintf(cod, "%4d", ADC[ind].dat);
    if (ADC[ind].unk) sprintf(val, " ");
    else              sprintf(val, "%5.3f", ADC[ind].val);
    switch (ADC[ind].sta) {
      case status_OK: color = good;    break;
      case status_KO: color = error;   break;
      case status_NA: color = warning; break;
      default:        color = alarm;   break;
    }
  }

  if ((obj = DCDC_vol_cod_BOX[cha])) {
    fl_set_object_label(obj, cod);
    fl_set_object_color (obj,  colors[color], 0);
    fl_set_object_lcolor(obj, lcolors[color]);
  }

  if ((obj = DCDC_vol_val_BOX1[cha])) {
    fl_set_object_label(obj, val);
    fl_set_object_color (obj,  colors[color], 0);
    fl_set_object_lcolor(obj, lcolors[color]);
  }

  if ((obj = DCDC_vol_val_BOX2[cha])) {
    fl_set_object_label(obj, val);
    fl_set_object_color (obj,  colors[color], 0);
    fl_set_object_lcolor(obj, lcolors[color]);
  }
}

//~-----------------------------------------------------------------------------

void update_DCDC_current(int cha) {

  FL_OBJECT *obj;
  char cod[80], val[80];
  int               good = 0, warning = 1, alarm = 2, error = 3;
  int colors[4]  = {FL_GREEN, FL_YELLOW,   FL_RED,    FL_BLACK};
  int lcolors[4] = {FL_BLACK, FL_BLACK,    FL_YELLOW, FL_WHITE};
  int color;
  int ind;
    
  ind = DCDC[cha].cur.ind;
  
  if (ind < 0) {
    sprintf(cod, "-");
    sprintf(val, "-");
    color = good;
  }
  else {
    if (ADC[ind].unk) sprintf(cod, " ");
    else              sprintf(cod, "%4d", ADC[ind].dat);
    if (ADC[ind].unk) sprintf(val, " ");
    else              sprintf(val, "%5.3f", ADC[ind].val);
    switch (ADC[ind].sta) {
      case status_OK: color = good;    break;
      case status_KO: color = error;   break;
      case status_NA: color = warning; break;
      default:        color = alarm; break;
    }
  }

  if ((obj = DCDC_cur_cod_BOX[cha])) {
    fl_set_object_label(obj, cod);
    fl_set_object_color (obj,  colors[color], 0);
    fl_set_object_lcolor(obj, lcolors[color]);
  }

  if ((obj = DCDC_cur_val_BOX1[cha])) {
    fl_set_object_label(obj, val);
    fl_set_object_color (obj,  colors[color], 0);
    fl_set_object_lcolor(obj, lcolors[color]);
  }

  if ((obj = DCDC_cur_val_BOX2[cha])) {
    fl_set_object_label(obj, val);
    fl_set_object_color (obj,  colors[color], 0);
    fl_set_object_lcolor(obj, lcolors[color]);
  }
}

//~-----------------------------------------------------------------------------

void update_DCDC_button(int cha) {

  int adr = DCDC[cha].adr;
  FL_OBJECT *obj;
  
  obj = DCDC_ctr_INP[cha];

  if (!obj) return;

  if (LVDS[adr].wri.inp & DCDC[cha].ctr.msk) {
    fl_set_object_color(obj, FL_RED, FL_RED);
    fl_set_object_lcolor(obj, FL_YELLOW);
    fl_set_object_label(obj, "ON");
  }
  else {
    fl_set_object_color(obj, FL_GREEN, FL_GREEN);
    fl_set_object_lcolor(obj, FL_BLACK);
    fl_set_object_label(obj, "OFF");
  }

  obj = DCDC_ctr_WRI[cha];

  if (!obj) return;

  if (LVDS[adr].wri.sta == status_NA) {
    fl_set_object_color(obj, FL_YELLOW, FL_YELLOW);
    fl_set_object_label(obj, "");
  }
  else if (LVDS[adr].wri.dat & DCDC[cha].ctr.msk) {
    fl_set_object_color(obj, FL_RED, FL_RED);
    fl_set_object_label(obj, "");
  }
  else {
    fl_set_object_color(obj, FL_GREEN, FL_GREEN);
    fl_set_object_label(obj, "");
  }
}

//~-----------------------------------------------------------------------------

void update_JPMD_panel(int cha) {

  int i, j;

  for (i=0, j=cha; i<6; i++, j++) {
    update_DCDC_status(j, 0);
    update_DCDC_status(j, 1);
    update_DCDC_status(j, 2);
    update_DCDC_status(j, 3);
    update_DCDC_voltage(j);
    update_DCDC_current(j);
    update_DCDC_button(j);
  }
}

//~============================================================================

void update_JPIF_panel(int cha);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_JPIF_panel(int x0, int y0, int cha) {

  int i, j;
  int x, y, w;
  int w1 = 30, w2 = 40, w3 = 10;
  int wa = 55, wb = 50, wc = 15, wd = 35;
  int h = 15;
  int size;
  
  size = FL_NORMAL_SIZE;

  x = x0;
  y = y0;
  add_frame_box(x, y, wa, 8*h, DCDC[cha].JPD_board, size);

  size = FL_SMALL_SIZE;

  for (i=0, j=cha; i<8; i++, j++) {
    x = x0 + wa;
    if (i == 0) add_frame_box(x, y, wb, 4*h, DCDC[j].J_JT_board, size);
    if (i == 4) add_frame_box(x, y, wb, 2*h, DCDC[j].J_JT_board, size);
    if (i == 6) add_frame_box(x, y, wb, 2*h, DCDC[j].J_JT_board, size);
    x = x + wb;
                add_frame_box(x, y, wc,   h, DCDC[j].feed,       size);
    x = x + wc;
                add_frame_box(x, y, wd,   h, DCDC[j].nominal,    size);
    y = y + h;
  }

  size = FL_NORMAL_SIZE;

  y = y0;
  w = wa + wb + wc + wd;
  for (i=0, j=cha; i<8; i++, j++) {
    x = x0 + w;
    DCDC_ctr_BOX[j] =       add_data_box(x, y, w1,   h, "0", size);
    x = x + w1;
    DCDC_mon_BOX[j] =       add_data_box(x, y, w1,   h, "1", size);
    x = x + w1;
    DCDC_acc_BOX[j] =       add_data_box(x, y, w3,   h, "", size);
    x = x + w3;
    DCDC_dir_BOX[j] =       add_data_box(x, y, w1,   h, "", size);
    x = x + w1;
    if (i % 2 == 0) {
      fl_addto_group(VOL1);
        DCDC_vol_cod_BOX[j]  = add_data_box(x,      y,   w2, 2*h, "2", size);
        DCDC_vol_val_BOX1[j] = add_data_box(x+w2,   y,   w2, 2*h, "3", size);
      fl_end_group();
      fl_addto_group(CUR1);
        DCDC_cur_cod_BOX[j]  = add_data_box(x+2*w2, y,   w2, 2*h, "4", size);
        DCDC_cur_val_BOX1[j] = add_data_box(x+3*w2, y,   w2, 2*h, "5", size);
      fl_end_group();
      fl_addto_group(VOL2);
        DCDC_vol_val_BOX2[j] = add_data_box(x,      y, 2*w2, 2*h, "3", size);
      fl_end_group();
      fl_addto_group(CUR2);
        DCDC_cur_val_BOX2[j] = add_data_box(x+2*w2, y, 2*w2, 2*h, "5", size);
      fl_end_group();
    }
    x = x + 4*w2;
    y = y + h;
  }
}

//~-----------------------------------------------------------------------------

void update_JPIF_panel(int cha) {

  int i, j;

  for (i=0, j=cha; i<8; i++, j++) {
    update_DCDC_status(j, 0);
    update_DCDC_status(j, 1);
    update_DCDC_status(j, 2);
    update_DCDC_status(j, 3);
    update_DCDC_voltage(j);
    update_DCDC_current(j);
  }
}

//~============================================================================

void JPJT_ON_OFF_callback(FL_OBJECT *obj, long data);
void update_JPJT_panel(int cha);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_JPJT_panel(int x0, int y0, int cha) {

  FL_OBJECT *obj;
  int i, j;
  int x, y, w;
  int w1 = 30, w2 = 40, w3 = 10, w4 = 30, w5 = 40;
  int wa = 55, wb = 50, wc = 15, wd = 35;
  int h = 15;
  int size;
  
  size = FL_NORMAL_SIZE;

  x = x0;
  y = y0;
  add_frame_box(x, y, wa, 6*h, DCDC[cha].JPD_board, size);

  size = FL_SMALL_SIZE;

  for (i=0, j=cha; i<6; i++, j++) {
    x = x0 + wa;
    add_frame_box(x, y, wb, h, DCDC[j].J_JT_board, size);
    x = x + wb;
    add_frame_box(x, y, wc, h, DCDC[j].feed,       size);
    x = x + wc;
    add_frame_box(x, y, wd, h, DCDC[j].nominal,    size);
    y = y + h;
  }

  size = FL_NORMAL_SIZE;

  y = y0;
  w = wa + wb + wc + wd;
  for (i=0, j=cha; i<6; i++, j++) {
    x = x0 + w;
    DCDC_ctr_BOX[j] =        add_data_box(x, y, w1, h, "0", size);
    x = x + w1;
    DCDC_mon_BOX[j] =        add_data_box(x, y, w1, h, "1", size);
    x = x + w1;
    DCDC_acc_BOX[j] =        add_data_box(x, y, w3,   h, "", size);
    x = x + w3;
    DCDC_dir_BOX[j] =        add_data_box(x, y, w1,   h, "", size);
    x = x + w1;
    fl_addto_group(VOL1);
      DCDC_vol_cod_BOX[j]  = add_data_box(x,      y,   w2, h, "2", size);
      DCDC_vol_val_BOX1[j] = add_data_box(x+w2,   y,   w2, h, "3", size);
    fl_end_group();
    fl_addto_group(CUR1);
      DCDC_cur_cod_BOX[j]  = add_data_box(x+2*w2, y,   w2, h, "4", size);
      DCDC_cur_val_BOX1[j] = add_data_box(x+3*w2, y,   w2, h, "5", size);
    fl_end_group();
    fl_addto_group(VOL2);
      DCDC_vol_val_BOX2[j] = add_data_box(x,      y, 2*w2, h, "3", size);
    fl_end_group();
    fl_addto_group(CUR2);
      DCDC_cur_val_BOX2[j] = add_data_box(x+2*w2, y, 2*w2, h, "5", size);
    fl_end_group();
    x = x + 4*w2;
    DCDC_ctr_WRI[j] =        add_data_box(x, y, w3, h, "", size);
    x = x + w3;
    DCDC_ctr_INP[j] =
    obj = add_switch_button(x, y, w4,   h, "OFF", size);
      fl_set_object_callback(obj, JPJT_ON_OFF_callback, j);
    x = x + w4;
    if (i == 0) {
      DCDC_ctr_SND[j] =
      obj = add_normal_button(x, y, w5, 2*h, "SND", size);
        fl_set_object_callback(obj, SND_callback, j);
    }
    if (i == 2) {
      DCDC_ctr_SND[j] =
      obj = add_normal_button(x, y, w5, 4*h, "SND", size);
        fl_set_object_callback(obj, SND_callback, j);
    }
    y = y + h;
  }
}

//~----------------------------------------------------------------------------

void JPJT_ON_OFF_callback(FL_OBJECT *obj, long data) {

  int cha, adr;

  cha = data;
  adr = DCDC[cha].adr;
  LVDS[adr].wri.inp = LVDS[adr].wri.inp ^ DCDC[cha].ctr.msk;

  msg = 0;
  update_objects();
}

//~-----------------------------------------------------------------------------

void update_JPJT_panel(int cha) {

  int i, j;

  for (i=0, j=cha; i<6; i++, j++) {
    update_DCDC_status(j, 0);
    update_DCDC_status(j, 1);
    update_DCDC_status(j, 2);
    update_DCDC_status(j, 3);
    update_DCDC_voltage(j);
    update_DCDC_current(j);
    update_DCDC_button(j);
  }
}

//~============================================================================

void update_JPSC_panel(int cha);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_JPSC_panel(int x0, int y0, int cha) {

  int i, j;
  int x, y, w;
  int w1 = 30, w2 = 40, w3 = 10;
  int wa = 55, wb = 50, wc = 15, wd = 35;
  int h = 15;
  int size;
  
  size = FL_NORMAL_SIZE;

  x = x0;
  y = y0;
  add_frame_box(x, y, wa, 6*h, DCDC[cha].JPD_board, size);

  size = FL_SMALL_SIZE;

  for (i=0, j=cha; i<6; i++, j++) {
    x = x0 + wa;
    if (i == 0) add_frame_box(x, y, wb, 6*h, "JPD\n&\nUSCMs", size);
    x = x + wb;
                add_frame_box(x, y, wc,   h, DCDC[j].feed,    size);
    x = x + wc;
                add_frame_box(x, y, wd,   h, DCDC[j].nominal, size);
    y = y + h;
  }

  size = FL_NORMAL_SIZE;

  y = y0;
  w = wa + wb + wc + wd;
  for (i=0, j=cha; i<6; i++, j++) {
    x = x0 + w;
    DCDC_ctr_BOX[j] =        add_data_box(x, y, w1, h, "0", size);
    x = x + w1;
    DCDC_mon_BOX[j] =        add_data_box(x, y, w1, h, "1", size);
    x = x + w1;
    DCDC_acc_BOX[j] =        add_data_box(x, y, w3,   h, "", size);
    x = x + w3;
    DCDC_dir_BOX[j] =        add_data_box(x, y, w1,   h, "", size);
    x = x + w1;
    fl_addto_group(VOL1);
      DCDC_vol_cod_BOX[j]  = add_data_box(x,      y, w2, h, "2", size);
      DCDC_vol_val_BOX1[j] = add_data_box(x+w2,   y, w2, h, "3", size);
    fl_end_group();
    fl_addto_group(CUR1);
      DCDC_cur_cod_BOX[j]  = add_data_box(x+2*w2, y, w2, h, "4", size);
      DCDC_cur_val_BOX1[j] = add_data_box(x+3*w2, y, w2, h, "5", size);
    fl_end_group();
    fl_addto_group(VOL2);
      DCDC_vol_val_BOX2[j] = add_data_box(x,      y, 2*w2, h, "3", size);
    fl_end_group();
    fl_addto_group(CUR2);
      DCDC_cur_val_BOX2[j] = add_data_box(x+2*w2, y, 2*w2, h, "5", size);
    fl_end_group();
    x = x + 4 * w2;
    y = y + h;
  }
}

//~-----------------------------------------------------------------------------

void update_JPSC_panel(int cha) {

  int i, j;

  for (i=0, j=cha; i<6; i++, j++) {
    update_DCDC_status(j, 0);
    update_DCDC_status(j, 1);
    update_DCDC_status(j, 2);
    update_DCDC_status(j, 3);
    update_DCDC_voltage(j);
    update_DCDC_current(j);
  }
}

//~============================================================================

void read_all_data(void) {

  bool ok[513];

  int16 err;
  int i, j, k;
  int8 n;
  int16 dat[513];
  int known[513];
  int64 Dallas_ID[513];
  float val[513];
  int n1 = 0, n2 = 0, nmax, kmax;
  int done[256];

//~-- "Go" Command ---

  setup_DS1820_readout(0xFF, 0x00, 0x00, 0x00, 1, &err);

//~-- Read JPD Status ---

  for (i=0; i<256; i++) {
    done[i] = 0;
  }
  for (i=0; i<32; i++) {
    int adr = DCDC[i].adr;
    int16 dat;
    if (adr >= 0 && adr <= 255 && done[adr] == 0) {
      read_LVDS_bus(adr, &dat, &err);
      done[adr] = 1;
      if (err) {
        LVDS[adr].rea.sta = status_KO;
        update_objects();
      }
      else {
        LVDS[adr].rea.unk = 0;
        LVDS[adr].rea.sta = status_OK;
        LVDS[adr].rea.dat = dat ^ LVDS[adr].rea.inv;
        LVDS[adr].rea.acc = LVDS[adr].rea.acc | ~LVDS[adr].rea.dat;
      }
    }
  }

//~-- Read JPD Voltages and Currents ---

  n = 32;
  read_ADC(dat, &err);
  if (err) {
    for (i=0; i<32; i++) {
      ADC[i].sta = status_KO;
    }
  }
  else {
    for (i=0; i<32; i++) {
      ADC[i].unk = 0;
      ADC[i].sta = status_OK;
      ADC[i].dat = dat[i];
    }
    for (i=0; i<32; i++) {
      int ind;
      float K, B;
      ind = DCDC[i].cur.ind;
      if (ind >= 0 && ind <= 31) {
        K   = DCDC[i].cur.k;
        B   = DCDC[i].cur.b;
        ADC[ind].val = (ADC[ind].dat - B) / K;
      }
      ind = DCDC[i].vol.ind;
      if (ind >= 0 && ind <= 31) {
        K   = DCDC[i].vol.k;
        B   = DCDC[i].vol.b;
        ADC[ind].val = (ADC[ind].dat - B) / K;
      }
    }
  }

//~-- Read Dallas Sensors IDs and Temperatures ---

  if (!USCM_tmp_unk) USCM_tmp_sta = status_KO;
  for (j=0; j<12; j++) {
    if (!bus[0].DS1820[j].unk) bus[0].DS1820[j].sta = status_KO;
    if (!bus[1].DS1820[j].unk) bus[1].DS1820[j].sta = status_KO;
  }

  for (i=0; i<513; i++) {
    known[i] = 0;
    Dallas_ID[i] = 0;
  }
  read_DS1820_table(9, &n, &Dallas_ID[0], NULL, &err);
  if (P > 0) printf("read_DS1820_table(9,..): err=%04X, n=%d\n", err, n);
  if (!err) {
    n1 = n;
    for (i=1; i<9; i++) {
      read_DS1820_table(i, &n, &Dallas_ID[n1], NULL, &err);
      if (P > 0) printf("read_DS1820_table(%d,..): err=%04X, n=%d\n", i, err, n);
      if (!err) n1 = n1 + n;
      else      break;
    }
  }
  if (err) {
    USCM_tmp_sta = status_KO;
    for (j=0; j<12; j++) {
      bus[0].DS1820[j].sta = status_KO;
      bus[1].DS1820[j].sta = status_KO;
    }
  }
  else {
    Dallas_bus = -1;
    if (n1) USCM_Dallas_ID = Dallas_ID[0];
    else    USCM_Dallas_ID = 0x0000000000000000;
    read_DS1820_temp_long(9, &n, &ok[0], &val[0], NULL, &err);
    if (P > 0) printf("read_DS1820_temp_long(9,..): err=%04X\n", err);
    if (!err) {
      n2 = n;
      for (i=1; i<9; i++) {
        read_DS1820_temp_long(i, &n, &ok[n2], &val[n2], NULL, &err);
        if (P > 0) printf("read_DS1820_temp_long(%d,..): err=%04X, n=%d\n", i, err, n);
        if (!err) n2 = n2 + n;
        else      break;
      }
    }
    if (err) {
      USCM_tmp_sta = status_KO;
      for (j=0; j<12; j++) {
        bus[0].DS1820[j].sta = status_KO;
        bus[1].DS1820[j].sta = status_KO;
      }
    }
    else {
      if (n2) {
        USCM_tmp_unk = 0;
        USCM_tmp_sta = status_OK;
        USCM_tmp_val = val[0];
      }
      nmax = -1;
      kmax = -1;
//    for (k=2*JPD_mod; k<2*JPD_mod+2; k++) {
      for (k=0; k<2; k++) {
        n = 0;        
        for (j=0; j<12; j++) {
          for (i=0; i<n1; i++) {
            if (Dallas[j].ID[k] == Dallas_ID[i]) n = n + 1;
          }
        }
        if (P > 0) printf("k=%d n=%d\n", k, n);
        if (n > 0 && n > nmax) {
          nmax = n;
          kmax = k;
        }
      }
      if (kmax >= 0) {
        Dallas_bus = kmax % 2;
        for (j=0; j<12; j++) {
          if (strncmp(Dallas[j].JPD_board,"USCM",4) == 0) {
            Dallas[j].ID[kmax] = USCM_Dallas_ID;
          }
        }
      }
      if (P > 0) {
        printf("nmax=%d at kmax=%d Dallas_bus=%d(%c) JPD_model=%d(%s)\n", 
                nmax, kmax, 
                Dallas_bus, 
                Dallas_bus<0 ? '?' : (Dallas_bus ? 'B' : 'A'),
                kmax/2, 
                kmax < 0 ? "??" : (kmax/2<N_MOD ? JPD_mods[kmax/2].nam : "??"));
      }
      if (P > 0) printf("n1 = %d\n", n1);
      for (i=0; i<n1; i++) {
        if (P > 0) {
          printf("%3d: %08X%08X ", 
                i, (int32)(Dallas_ID[i]>>32), (int32)((Dallas_ID[i]<<32)>>32));
        }
        for (j=0; j<12; j++) {
          if (kmax >=0 && (Dallas[j].ID[kmax] == Dallas_ID[i]) && ok[i]) {
            if (P > 0) printf("*");
            bus[Dallas_bus].DS1820[j].unk = 0;
            bus[Dallas_bus].DS1820[j].sta = status_OK;
            bus[Dallas_bus].DS1820[j].dat = dat[i];
            bus[Dallas_bus].DS1820[j].val = val[i];
          }
        }
        if (P > 0) printf("\n");
      }
    }
  }
}

//~============================================================================

void SUIC_callback(FL_OBJECT *obj, long data);
void SOFF_callback(FL_OBJECT *obj, long data);
void SON_callback(FL_OBJECT *obj, long data);
void BOOT_MODE_callback(FL_OBJECT *obj, long data);
void REBOOT_callback(FL_OBJECT *obj, long data);
void GO_callback(FL_OBJECT *obj, long data);
void POWER_FF_callback(FL_OBJECT *obj, long data);
void update_USCM_panel(void);

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void create_USCM_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y, w, size;
  int wa = 55, w1 = 50, w2 = 50, w3 = 50, w4 = 60, h1 = 50, h2 = 25;
  
  w = 5 + wa + w1 + w2 + w3 + w4 + w2 + 5;
  obj = fl_add_box(FL_DOWN_BOX, x0, y0, w, 10+h1, "");

  size = FL_MEDIUM_SIZE;

  x = x0 + 5;
  y = y0 + 5;
  obj = add_frame_box(x, y, wa, h1, "USCM", FL_NORMAL_SIZE);

  x = x + wa;
  obj = add_normal_button(x, y, w1, h1, "SUIC", size);
    fl_set_object_callback(obj, SUIC_callback, 0);

  x = x + w1;
  obj = add_normal_button(x, y, w2, h1, "SOFF", size);
    fl_set_object_callback(obj, SOFF_callback, 0);

  x = x + w2;
  obj = add_normal_button(x, y, w3, h1, "SON", size);
    fl_set_object_callback(obj, SON_callback, 0);

  x = x + w3;
  BOOT_MODE = 
  obj = add_switch_button(x, y, w4, h2, "?", size);
    fl_set_object_callback(obj, BOOT_MODE_callback, 0);
  obj = add_normal_button(x, y+h2, w4, h1-h2, "BOOT", size);
    fl_set_object_callback(obj, REBOOT_callback, 0);

  x = x + w4;
  obj = add_normal_button(x, y, w2, h2, "GO", size);
    fl_set_object_callback(obj, GO_callback, 0);
  obj = add_normal_button(x, y+h2, w2, h2, "PWFF", size);
    fl_set_object_callback(obj, POWER_FF_callback, 0);
}

//~----------------------------------------------------------------------------

void SUIC_callback(FL_OBJECT *obj, long data) {

  boot_USCM(5, &sta);
}

//~----------------------------------------------------------------------------

void SOFF_callback(FL_OBJECT *obj, long data) {

  boot_USCM(3, &sta);
}

//~----------------------------------------------------------------------------

void SON_callback(FL_OBJECT *obj, long data) {

  boot_USCM(4, &sta);
}

//~----------------------------------------------------------------------------

void BOOT_MODE_callback(FL_OBJECT *obj, long data) {

  boot_mode = MOD(boot_mode+1, 2);
  
  msg = 0;
  update_USCM_panel();
}

//~----------------------------------------------------------------------------

void REBOOT_callback(FL_OBJECT *obj, long data) {

  int16 err = 1;

  switch (boot_mode) {
    case 0: boot_from_OTP(&err);   break;
    case 1: boot_from_FLASH(&err); break;
  }

  if (boot_mode == 1) start_DS1820_scan(0xFF, &err);

  if (err) sta = 0;
  else     sta = 1;
}

//~----------------------------------------------------------------------------

void GO_callback(FL_OBJECT *obj, long data) {

  int16 err;
  
  msg = -1;
  update_objects();
  
  start_DS1820_scan(0xFF, &err);

  if (err) sta = 0;
  else     sta = 1;

  msg = 1;
  update_objects();
}

//~----------------------------------------------------------------------------

void POWER_FF_callback(FL_OBJECT *obj, long data) {
}

//~----------------------------------------------------------------------------

void update_USCM_panel(void) {

  char *t[3] = {"OTP", "FLASH"};
  
  fl_set_object_label(BOOT_MODE, t[boot_mode]);
}

//~===============================================================================

void update_objects(void) {

  FL_OBJECT *obj;

  update_USCM_JMDC_panels();
  update_JPD_mod_panel();

  if (hide_voltage_code) {
    fl_hide_object(VOL1);
    fl_show_object(VOL2);
  }
  else {
    fl_hide_object(VOL2);
    fl_show_object(VOL1);
  }

  if (hide_current_code) {
    fl_hide_object(CUR1);
    fl_show_object(CUR2);
  }
  else {
    fl_hide_object(CUR2);
    fl_show_object(CUR1);
  }

  update_JPMD_panel( 0);
  update_JPMD_panel( 6);
  update_JPIF_panel(12);
  update_JPJT_panel(20);
  update_JPSC_panel(26);
  update_USCM_panel();
  update_monitor_panel();
  update_temperature_panel();
  update_P_panel();

//~-- Message ---

  obj = MESSAGE;
  if (msg) {
    if (msg < 0) {
      fl_set_object_color(obj,FL_YELLOW,FL_MCOL);
      fl_set_object_lcolor(obj,FL_BLACK);
      fl_set_object_label(obj,"Command is being sent...");
    }
    else {
      if (sta) {
        fl_set_object_color(obj,FL_CYAN,FL_MCOL);
        fl_set_object_lcolor(obj,FL_BLACK);
        fl_set_object_label(obj,"Command executed O.K.");
      }
      else {
        fl_set_object_color(obj,FL_BLACK,FL_MCOL);
        fl_set_object_lcolor(obj,FL_YELLOW);
        fl_set_object_label(obj,"Command execution failed!");
      }
    }
    fl_show_object(obj);
  }
  else {
    fl_hide_object(obj);
  }


#ifdef LOGFILE
//~-- Log File writing ---
  
  obj = WRITING_LOG_FILE;
  if (fl_get_button(obj)) {
    fl_set_object_lcolor(obj, FL_YELLOW);
    fl_set_object_label(obj, "JPD.logfile is being written now");
    if (logfile == NULL) logfile = fopen("JPD.logfile", "a");
  }
  else {
    fl_set_object_lcolor(obj, FL_BLACK);
    fl_set_object_label(obj, "JPD.logfile is NOT being written now");
    if (logfile != NULL) {
      fclose(logfile);
      logfile = NULL;
    }
  }

//~-- monitoring ---
  
  for (i=0; i<32; i++) {
    FL_OBJECT *obj = MONITOR[i];
    fl_set_object_label(obj, (char*)mon_text(i));
    if (logfile != NULL) {
      if (i == 0) fprintf(logfile, "===================== %s ======\n", (char*)mytime());
      fprintf(logfile, (char*)mon_text(i));
      fprintf(logfile, "\n");
    }
  }
#endif

//~-- update the image ---

  fl_check_forms();
}

//~----------------------------------------------------------------------------

void command_write_LVDS_bus(int8 adr, int16 dat, int16 *sta) {

  int16 err;

  msg = -1;
  update_objects();

  write_LVDS_bus(adr, dat, &err);
  if (err) *sta = 0;
  else     *sta = 1;

  msg = 1;
  update_objects();

  if (logfile != NULL) {
    fprintf(logfile, "===================== %s ======\n", (char*)mytime());
    fprintf(logfile, "***** Command: adr=0x%02X  dat=0x%04X  sta=%d %s\n",
            adr, dat, *sta, *sta ? "  [OK]" : "[FAIL]");
  }
}

//~----------------------------------------------------------------------------

void TIMER_callback(FL_OBJECT *obj, long data) {

  if (monitoring) {
    read_all_data();
    update_objects();
  }
  
  fl_set_timer(obj, (float)timer_period+0.1);
}

//~----------------------------------------------------------------------------

void start_JPD(void) {

  int one = 1;
  static char *title = {"JPD Pro Controller"};
  char *adr[] = {(char*)&title};

//if (!initialize_EPP()) exit(1);

  fl_initialize( &one, adr, " ", 0, 0);
  JPD = create_form_JPD();
  fl_set_form_position( JPD->JPD, 0, 0);
  fl_show_form( JPD->JPD, FL_PLACE_GEOMETRY, FL_FULLBORDER, "JPD");

  update_objects();
}

//~============================================================================

int main(int argc, char *argv[]) {

  setbuf(stdout, NULL);	// set "no buffering" for stdout stream
  setup_command_path();

  printf("Usage: JPD [<Debug Printout Level>]\n");

  if (argc > 1) P = atoi(argv[1]);
  printf("P = %d\n", P);

  start_JPD();

  while(1) {
    fl_do_forms();
  }
  return 0;
}

//~============================================================================
