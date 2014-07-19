// file pdsdef.h
//
// PDS boards names, adresses, and many more
//
// A.Lebedev - Nov-2007...

//~----------------------------------------------------------------------------

#ifndef _PDSDEF_H
#define _PDSDEF_H

#include "uscmlib.h"

//~----------------------------------------------------------------------------

typedef struct {
  char *nam;
  char *txt[2];
} _dig;

typedef struct {
  char *nam;
  char *txt[2];
} _cmd;

typedef struct {
  char *nam;
  float min;
  float max;
} _ana;

typedef struct {
  char *nam;
  int8 adr;
  int8 sta;
  int8 ovt;
  int8 tmp;
  int8 ref;
  int8 t_w;
  int8 t_r;
  int8 adc[2];
  _dig dig[32];
  _cmd cmd[32];
  _ana ana[32];
} _brd;

//~----------------------------------------------------------------------------

typedef struct {
  char *nam;
  int8  pb2;
  int8  adr;
  int8  cmd;
  int8  sta;
  int8  cur;
} _out;

_out out[] = {
//        nam   pb2   adr   cmd   sta   cur
      {"CAB-A",   3, 0x06, 0x09, 0x11, 0x07},           //  0 0x00
      {"CAB-B",   2, 0x04, 0x09, 0x11, 0x07},           //  1 0x01
      {"ER0-A",   3, 0x06, 0x04, 0x0C, 0x02},           //  2 0x02
      {"ER0-B",   4, 0x08, 0x05, 0x0D, 0x03},           //  3 0x03
      {"ER1-A",   2, 0x04, 0x02, 0x0A, 0x00},           //  4 0x04
      {"ER1-B",   4, 0x08, 0x03, 0x0B, 0x01},           //  5 0x05
      {"JPD-A",   1, 0x02, 0x04, 0x0C, 0x02},           //  6 0x06
      {"JPD-B",   4, 0x08, 0x02, 0x0A, 0x00},           //  7 0x07
      {"MPD-A",   2, 0x04, 0x06, 0x0E, 0x04},           //  8 0x08
      {"MPD-B",   3, 0x06, 0x05, 0x0D, 0x03},           //  9 0x09
      {"SPD0-A",  4, 0x08, 0x07, 0x0F, 0x05},           // 10 0x0A
      {"SPD0-B",  1, 0x02, 0x07, 0x0F, 0x05},           // 11 0x0B
      {"SPD1-A",  1, 0x02, 0x05, 0x0D, 0x03},           // 12 0x0C
      {"SPD1-B",  4, 0x08, 0x04, 0x0C, 0x02},           // 13 0x0D
      {"SPD2-A",  2, 0x04, 0x08, 0x10, 0x06},           // 14 0x0E
      {"SPD2-B",  3, 0x06, 0x08, 0x10, 0x06},           // 15 0x0F
      {"SPD3-A",  2, 0x04, 0x05, 0x0D, 0x03},           // 16 0x10
      {"SPD3-B",  3, 0x06, 0x06, 0x0E, 0x04},           // 17 0x11
      {"TPD0",    3, 0x06, 0x02, 0x0A, 0x00},           // 18 0x12
      {"TPD1",    1, 0x02, 0x08, 0x10, 0x06},           // 19 0x13
      {"TPD2",    2, 0x04, 0x03, 0x0B, 0x01},           // 20 0x14
      {"TPD3",    1, 0x02, 0x02, 0x0A, 0x00},           // 21 0x15
      {"TPD4",    3, 0x06, 0x03, 0x0B, 0x01},           // 22 0x16
      {"TPD5",    4, 0x08, 0x08, 0x10, 0x06},           // 23 0x17
      {"TPD6",    2, 0x04, 0x04, 0x0C, 0x02},           // 24 0x18
      {"TPD7",    1, 0x02, 0x03, 0x0B, 0x01},           // 25 0x19
      {"TTPD-A",  4, 0x08, 0x09, 0x11, 0x07},           // 26 0x1A
      {"TTPD-B",  1, 0x02, 0x09, 0x11, 0x07},           // 27 0x1B
      {"UGPD-A",  1, 0x02, 0x06, 0x0E, 0x04},           // 28 0x1C
      {"UGPD-B",  3, 0x06, 0x07, 0x0F, 0x05},           // 29 0x1D
      {"UPD0",    4, 0x08, 0x06, 0x0E, 0x04},           // 30 0x1E
      {"UPD1",    2, 0x04, 0x07, 0x0F, 0x05}};          // 31 0x1F

//~----------------------------------------------------------------------------

_brd em_brd[] = {
  
//
//~--  ESEM3-B Board (120V Distributor) ---
//

//                            slot   sta   ovt   tmp   ref   t_w   t_r  --- adc ----
  {"ESEM3-B\n120V Distrib.",  0x00, 0x00, 0x01, 0x00, 0x02, 0x1B, 0x1B, {0x07, 0x17},

// Digital Channels
     {{"Board Status",          {"OK",  "NOK"}},     //  0 0x00
      {"OVT Alarm",             {"OK",  "OVT"}},     //  1 0x01
      {"O__1 CMD: AH_OUT1",     {"OFF", "ON"}},      //  2 0x02
      {"O__2 CMD: AH_OUT2",     {"OFF", "ON"}},      //  3 0x03
      {"O__3 CMD: AH_OUT3",     {"OFF", "ON"}},      //  4 0x04
      {"O__4 CMD: AH_OUT4",     {"OFF", "ON"}},      //  5 0x05
      {"O__5 CMD: AH_OUT5",     {"OFF", "ON"}},      //  6 0x06
      {"O__6 CMD: AH_OUT6",     {"OFF", "ON"}},      //  7 0x07
      {"O__7 CMD: AH_OUT7",     {"OFF", "ON"}},      //  8 0x08
      {"O__8 CMD: AH_OUT8",     {"OFF", "ON"}},      //  9 0x09
      {"O__9 CMD: AH_OUT9",     {"OFF", "ON"}},      // 10 0x0A
      {"O_10 CMD: AH_OUT10",    {"OFF", "ON"}},      // 11 0x0B
      {"O_11 CMD: AH_OUT11",    {"OFF", "ON"}},      // 12 0x0C
      {"O_12 CMD: AO_CCEB",     {"OFF", "ON"}},      // 13 0x0D
      {"O__1 Status: AH_OUT1",  {"OFF", "ON"}},      // 14 0x0E
      {"O__2 Status: AH_OUT2",  {"OFF", "ON"}},      // 15 0x0F
      {"O__3 Status: AH_OUT3",  {"OFF", "ON"}},      // 16 0x10
      {"O__4 Status: AH_OUT4",  {"OFF", "ON"}},      // 17 0x11
      {"O__5 Status: AH_OUT5",  {"OFF", "ON"}},      // 18 0x12
      {"O__6 Status: AH_OUT6",  {"OFF", "ON"}},      // 19 0x13
      {"O__7 Status: AH_OUT7",  {"OFF", "ON"}},      // 20 0x14
      {"O__8 Status: AH_OUT8",  {"OFF", "ON"}},      // 21 0x15
      {"O__9 Status: AH_OUT9",  {"OFF", "ON"}},      // 22 0x16
      {"O_10 Status: AH_OUT10", {"OFF", "ON"}},      // 23 0x17
      {"O_11 Status: AH_OUT11", {"OFF", "ON"}},      // 24 0x18
      {"O_12 Status: AO_CCEB",  {"OFF", "ON"}},      // 25 0x19
      {"O_12 Trip: AO_CCEB",    {"OK",  "TRIP"}},    // 26 0x1A
      {"Digital I/F Test",      {"0",   "1"}},       // 27 0x1B
      {NULL,                    {NULL,  NULL}},      // 28 0x1C
      {NULL,                    {NULL,  NULL}},      // 29 0x1D
      {NULL,                    {NULL,  NULL}},      // 30 0x1E
      {NULL,                    {NULL,  NULL}}},     // 31 0x1F
// Commands
     {{NULL,                 {NULL, NULL}},          //  0 0x00
      {NULL,                 {NULL, NULL}},          //  1 0x01
      {"O__1: AH_OUT1",      {"OFF", "ON"}},         //  2 0x02
      {"O__2: AH_OUT2",      {"OFF", "ON"}},         //  3 0x03
      {"O__3: AH_OUT3",      {"OFF", "ON"}},         //  4 0x04
      {"O__4: AH_OUT4",      {"OFF", "ON"}},         //  5 0x05
      {"O__5: AH_OUT5",      {"OFF", "ON"}},         //  6 0x06
      {"O__6: AH_OUT6",      {"OFF", "ON"}},         //  7 0x07
      {"O__7: AH_OUT7",      {"OFF", "ON"}},         //  8 0x08
      {"O__8: AH_OUT8",      {"OFF", "ON"}},         //  9 0x09
      {"O__9: AH_OUT9",      {"OFF", "ON"}},         // 10 0x0A
      {"O_10: AH_OUT10",     {"OFF", "ON"}},         // 11 0x0B
      {"O_11: AH_OUT11",     {"OFF", "ON"}},         // 12 0x0C
      {"O_12: AO_CCEB",      {"OFF", "ON"}},         // 13 0x0D
      {NULL,                 {NULL, NULL}},          // 14 0x0E
      {NULL,                 {NULL, NULL}},          // 15 0x0F
      {NULL,                 {NULL, NULL}},          // 16 0x10
      {NULL,                 {NULL, NULL}},          // 17 0x11
      {NULL,                 {NULL, NULL}},          // 18 0x12
      {NULL,                 {NULL, NULL}},          // 19 0x13
      {NULL,                 {NULL, NULL}},          // 20 0x14
      {NULL,                 {NULL, NULL}},          // 21 0x15
      {NULL,                 {NULL, NULL}},          // 22 0x16
      {NULL,                 {NULL, NULL}},          // 23 0x17
      {NULL,                 {NULL, NULL}},          // 24 0x18
      {NULL,                 {NULL, NULL}},          // 25 0x19
      {NULL,                 {NULL, NULL}},          // 26 0x1A
      {"Digital I/F Test",   {"0",   "1"}},          // 27 0x1B
      {"All Outlets ON/OFF", {"OFF", "ON"}},         // 28 0x1C
      {NULL,                 {NULL, NULL}},          // 29 0x1D
      {NULL,                 {NULL, NULL}},          // 30 0x1E
      {NULL,                 {NULL, NULL}}},         // 31 0x1F
// Analog Channels
     {{"Temperature",           -40.0,  80.0},       //  0 0x00
      {"O_12 Current: AO_CCEB",   0.0,  15.0},       //  1 0x01
      {"Analog Ref. 2.5V",        0.0,   4.81765},   //  2 0x02
      {NULL,                      0.0,   0.0},       //  3 0x03
      {NULL,                      0.0,   0.0},       //  4 0x04
      {NULL,                      0.0,   0.0},       //  5 0x05
      {NULL,                      0.0,   0.0},       //  6 0x06
      {NULL,                      0.0,   0.0},       //  7 0x07
      {NULL,                      0.0,   0.0},       //  8 0x08
      {NULL,                      0.0,   0.0},       //  9 0x09
      {NULL,                      0.0,   0.0},       // 10 0x0A
      {NULL,                      0.0,   0.0},       // 11 0x0B
      {NULL,                      0.0,   0.0},       // 12 0x0C
      {NULL,                      0.0,   0.0},       // 13 0x0D
      {NULL,                      0.0,   0.0},       // 14 0x0E
      {NULL,                      0.0,   0.0},       // 15 0x0F
      {NULL,                      0.0,   0.0},       // 16 0x10
      {NULL,                      0.0,   0.0},       // 17 0x11
      {NULL,                      0.0,   0.0},       // 18 0x12
      {NULL,                      0.0,   0.0},       // 19 0x13
      {NULL,                      0.0,   0.0},       // 20 0x14
      {NULL,                      0.0,   0.0},       // 21 0x15
      {NULL,                      0.0,   0.0},       // 22 0x16
      {NULL,                      0.0,   0.0},       // 23 0x17
      {NULL,                      0.0,   0.0},       // 24 0x18
      {NULL,                      0.0,   0.0},       // 25 0x19
      {NULL,                      0.0,   0.0},       // 26 0x1A
      {NULL,                      0.0,   0.0},       // 27 0x1B
      {NULL,                      0.0,   0.0},       // 28 0x1C
      {NULL,                      0.0,   0.0},       // 29 0x1D
      {NULL,                      0.0,   0.0},       // 30 0x1E
      {NULL,                      0.0,   0.0}},      // 31 0x1F
  },

//
//~--  PB2 #1 Board (120V -> 28V Converter) ---
//

//                            slot   sta   ovt   tmp   ref   t_w   t_r  --- adc ----
  {"PB2-1\n120V->28V",        0x01, 0x00, 0x01, 0x01, 0x06, 0x06, 0x06, {0x06, 0x16},

// Digital Channels
     {{"Board Status",     {"OK",  "NOK"}},          //  0 0x00
      {"OVT Alarm",        {"OVT", "OK"}},           //  1 0x01
      {NULL,               {NULL,   NULL}},          //  2 0x02
      {NULL,               {NULL,   NULL}},          //  3 0x03
      {"Input OVC Alarm",  {"OVC", "OK"}},           //  4 0x04
      {"DCDC ON/OFF",      {"OFF", "ON"}},           //  5 0x05
      {"Digital I/F Test", {"0",   "1"}},            //  6 0x06
      {NULL,               {NULL,   NULL}},          //  7 0x07
      {NULL,               {NULL,   NULL}},          //  8 0x08
      {NULL,               {NULL,   NULL}},          //  9 0x09
      {NULL,               {NULL,   NULL}},          // 10 0x0A
      {NULL,               {NULL,   NULL}},          // 11 0x0B
      {NULL,               {NULL,   NULL}},          // 12 0x0C
      {NULL,               {NULL,   NULL}},          // 13 0x0D
      {NULL,               {NULL,   NULL}},          // 14 0x0E
      {NULL,               {NULL,   NULL}},          // 15 0x0F
      {NULL,               {NULL,   NULL}},          // 16 0x10
      {NULL,               {NULL,   NULL}},          // 17 0x11
      {NULL,               {NULL,   NULL}},          // 18 0x12
      {NULL,               {NULL,   NULL}},          // 19 0x13
      {NULL,               {NULL,   NULL}},          // 20 0x14
      {"27V UNV",          {"UNV", "OK"}},           // 21 0x15
      {"29V Trip",         {"OK",  "TRIP"}},         // 22 0x16
      {"DCDC OFF/ON CMD",  {"ON",  "OFF"}},          // 23 0x17
      {NULL,               {NULL,   NULL}},          // 24 0x18
      {NULL,               {NULL,   NULL}},          // 25 0x19
      {NULL,               {NULL,   NULL}},          // 26 0x1A
      {NULL,               {NULL,   NULL}},          // 27 0x1B
      {NULL,               {NULL,   NULL}},          // 28 0x1C
      {NULL,               {NULL,   NULL}},          // 29 0x1D
      {NULL,               {NULL,   NULL}},          // 30 0x1E
      {NULL,               {NULL,   NULL}}},         // 31 0x1F
// Commands
     {{NULL,               {NULL, NULL}},            //  0 0x00
      {NULL,               {NULL, NULL}},            //  1 0x01
      {NULL,               {NULL, NULL}},            //  2 0x02
      {NULL,               {NULL, NULL}},            //  3 0x03
      {NULL,               {NULL, NULL}},            //  4 0x04
      {NULL,               {NULL, NULL}},            //  5 0x05
      {"Digital I/F Test", {"0",   "1"}},            //  6 0x06
      {NULL,               {NULL, NULL}},            //  7 0x07
      {NULL,               {NULL, NULL}},            //  8 0x08
      {NULL,               {NULL, NULL}},            //  9 0x09
      {NULL,               {NULL, NULL}},            // 10 0x0A
      {NULL,               {NULL, NULL}},            // 11 0x0B
      {NULL,               {NULL, NULL}},            // 12 0x0C
      {NULL,               {NULL, NULL}},            // 13 0x0D
      {NULL,               {NULL, NULL}},            // 14 0x0E
      {NULL,               {NULL, NULL}},            // 15 0x0F
      {NULL,               {NULL, NULL}},            // 16 0x10
      {NULL,               {NULL, NULL}},            // 17 0x11
      {NULL,               {NULL, NULL}},            // 18 0x12
      {NULL,               {NULL, NULL}},            // 19 0x13
      {NULL,               {NULL, NULL}},            // 20 0x14
      {NULL,               {NULL, NULL}},            // 21 0x15
      {NULL,               {NULL, NULL}},            // 22 0x16
      {"DCDC OFF/ON",      {"ON",  "OFF"}},          // 23 0x17
      {NULL,               {NULL, NULL}},            // 24 0x18
      {NULL,               {NULL, NULL}},            // 25 0x19
      {NULL,               {NULL, NULL}},            // 26 0x1A
      {NULL,               {NULL, NULL}},            // 27 0x1B
      {NULL,               {NULL, NULL}},            // 28 0x1C
      {NULL,               {NULL, NULL}},            // 29 0x1D
      {NULL,               {NULL, NULL}},            // 30 0x1E
      {NULL,               {NULL, NULL}}},           // 31 0x1F
// Analog Channels
     {{NULL,                  0.0,    0.0},          //  0 0x00
      {"Temperature",       -40.0,   90.0},          //  1 0x01
      {NULL,                  0.0,    0.0},          //  2 0x02
      {NULL,                  0.0,    0.0},          //  3 0x03
      {"Output Voltage",      0.0,   36.76},         //  4 0x04
      {NULL,                  0.0,    0.0},          //  5 0x05
      {"Analog Ref. 2.0V",    0.0,    4.81765},      //  6 0x06
      {NULL,                  0.0,    0.0},          //  7 0x07
      {NULL,                  0.0,    0.0},          //  8 0x08
      {NULL,                  0.0,    0.0},          //  9 0x09
      {NULL,                  0.0,    0.0},          // 10 0x0A
      {NULL,                  0.0,    0.0},          // 11 0x0B
      {NULL,                  0.0,    0.0},          // 12 0x0C
      {NULL,                  0.0,    0.0},          // 13 0x0D
      {NULL,                  0.0,    0.0},          // 14 0x0E
      {NULL,                  0.0,    0.0},          // 15 0x0F
      {NULL,                  0.0,    0.0},          // 16 0x10
      {NULL,                  0.0,    0.0},          // 17 0x11
      {NULL,                  0.0,    0.0},          // 18 0x12
      {NULL,                  0.0,    0.0},          // 19 0x13
      {NULL,                  0.0,    0.0},          // 20 0x14
      {NULL,                  0.0,    0.0},          // 21 0x15
      {NULL,                  0.0,    0.0},          // 22 0x16
      {NULL,                  0.0,    0.0},          // 23 0x17
      {NULL,                  0.0,    0.0},          // 24 0x18
      {NULL,                  0.0,    0.0},          // 25 0x19
      {NULL,                  0.0,    0.0},          // 26 0x1A
      {NULL,                  0.0,    0.0},          // 27 0x1B
      {NULL,                  0.0,    0.0},          // 28 0x1C
      {NULL,                  0.0,    0.0},          // 29 0x1D
      {NULL,                  0.0,    0.0},          // 30 0x1E
      {NULL,                  0.0,    0.0}},         // 31 0x1F
  },

//
//~--  ESEM3-A #1 Board (28V Distributor) ---
//

//                            slot   sta   ovt   tmp   ref   t_w   t_r  --- adc ----
  {"ESEM3-A-1\n28V Distrib.", 0x02, 0x00, 0x01, 0x08, 0x09, 0x0A, 0x12, {0x05, 0x15},

// Digital Channels
     {{"Board Status",       {"OK",  "NOK"}},        //  0 0x00
      {"OVT Alarm",          {"OK",  "OVT"}},        //  1 0x01
      {"O_1 CMD: TPD3",      {"OFF", "ON"}},         //  2 0x02
      {"O_2 CMD: TPD7",      {"OFF", "ON"}},         //  3 0x03
      {"O_3 CMD: JPD_N",     {"OFF", "ON"}},         //  4 0x04
      {"O_4 CMD: SPD1_N",    {"OFF", "ON"}},         //  5 0x05
      {"O_5 CMD: UGPD_N",    {"OFF", "ON"}},         //  6 0x06
      {"O_6 CMD: SPD0_R",    {"OFF", "ON"}},         //  7 0x07
      {"O_7 CMD: TPD1",      {"OFF", "ON"}},         //  8 0x08
      {"O_8 CMD: TTPD_R",    {"OFF", "ON"}},         //  9 0x09
      {"O_1 Status: TPD3",   {"OFF", "ON"}},         // 10 0x0A
      {"O_2 Status: TPD7",   {"OFF", "ON"}},         // 11 0x0B
      {"O_3 Status: JPD_N",  {"OFF", "ON"}},         // 12 0x0C
      {"O_4 Status: SPD1_N", {"OFF", "ON"}},         // 13 0x0D
      {"O_5 Status: UGPD_N", {"OFF", "ON"}},         // 14 0x0E
      {"O_6 Status: SPD0_R", {"OFF", "ON"}},         // 15 0x0F
      {"O_7 Status: TPD1",   {"OFF", "ON"}},         // 16 0x10
      {"O_8 Status: TTPD_R", {"OFF", "ON"}},         // 17 0x11
      {"Digital I/F Test",   {"0",   "1"}},          // 18 0x12
      {NULL              ,   {NULL, NULL}},          // 19 0x13
      {NULL              ,   {NULL, NULL}},          // 20 0x14
      {NULL              ,   {NULL, NULL}},          // 21 0x15
      {NULL              ,   {NULL, NULL}},          // 22 0x16
      {NULL              ,   {NULL, NULL}},          // 23 0x17
      {NULL              ,   {NULL, NULL}},          // 24 0x18
      {NULL              ,   {NULL, NULL}},          // 25 0x19
      {NULL              ,   {NULL, NULL}},          // 26 0x1A
      {NULL              ,   {NULL, NULL}},          // 27 0x1B
      {NULL              ,   {NULL, NULL}},          // 28 0x1C
      {NULL              ,   {NULL, NULL}},          // 29 0x1D
      {NULL              ,   {NULL, NULL}},          // 30 0x1E
      {NULL              ,   {NULL, NULL}}},         // 31 0x1F
// Commands
     {{NULL,                 {NULL, NULL}},          //  0 0x00
      {NULL,                 {NULL, NULL}},          //  1 0x01
      {"O_1: TPD3",          {"OFF", "ON"}},         //  2 0x02
      {"O_2: TPD7",          {"OFF", "ON"}},         //  3 0x03
      {"O_3: JPD_N",         {"OFF", "ON"}},         //  4 0x04
      {"O_4: SPD1_N",        {"OFF", "ON"}},         //  5 0x05
      {"O_5: UGPD_N",        {"OFF", "ON"}},         //  6 0x06
      {"O_6: SPD0_R",        {"OFF", "ON"}},         //  7 0x07
      {"O_7: TPD1",          {"OFF", "ON"}},         //  8 0x08
      {"O_8: TTPD_R",        {"OFF", "ON"}},         //  9 0x09
      {"Digital I/F Test",   {"0",   "1"}},          // 10 0x0A
      {NULL,                 {NULL, NULL}},          // 11 0x0B
      {NULL,                 {NULL, NULL}},          // 12 0x0C
      {NULL,                 {NULL, NULL}},          // 13 0x0D
      {NULL,                 {NULL, NULL}},          // 14 0x0E
      {NULL,                 {NULL, NULL}},          // 15 0x0F
      {NULL,                 {NULL, NULL}},          // 16 0x10
      {NULL,                 {NULL, NULL}},          // 17 0x11
      {NULL,                 {NULL, NULL}},          // 18 0x12
      {"All Outlets ON/OFF", {"OFF", "ON"}},         // 19 0x13
      {NULL,                 {NULL, NULL}},          // 20 0x14
      {NULL,                 {NULL, NULL}},          // 21 0x15
      {NULL,                 {NULL, NULL}},          // 22 0x16
      {NULL,                 {NULL, NULL}},          // 23 0x17
      {NULL,                 {NULL, NULL}},          // 24 0x18
      {NULL,                 {NULL, NULL}},          // 25 0x19
      {NULL,                 {NULL, NULL}},          // 26 0x1A
      {NULL,                 {NULL, NULL}},          // 27 0x1B
      {NULL,                 {NULL, NULL}},          // 28 0x1C
      {NULL,                 {NULL, NULL}},          // 29 0x1D
      {NULL,                 {NULL, NULL}},          // 30 0x1E
      {NULL,                 {NULL, NULL}}},         // 31 0x1F
// Analog Channels
     {{"O_1 Current: TPD3",     0.0,   7.5},         //  0 0x00
      {"O_2 Current: TPD7",     0.0,   7.5},         //  1 0x01
      {"O_3 Current: JPD_N",    0.0,   7.5},         //  2 0x02
      {"O_4 Current: SPD1_N",   0.0,   7.5},         //  3 0x03
      {"O_5 Current: UGPD_N",   0.0,   7.5},         //  4 0x04
      {"O_6 Current: SPD0_R",   0.0,   7.5},         //  5 0x05
      {"O_7 Current: TPD1",     0.0,   7.5},         //  6 0x06
      {"O_8 Current: TTPD_R",   0.0,  15.0},         //  7 0x07
      {"Temperature",         -40.0,  80.0},         //  8 0x08
      {"Analog Ref. 3.0V",      0.0,   4.81765},     //  9 0x09
      {NULL              ,      0.0,   0.0},         // 10 0x0A
      {NULL              ,      0.0,   0.0},         // 11 0x0B
      {NULL              ,      0.0,   0.0},         // 12 0x0C
      {NULL              ,      0.0,   0.0},         // 13 0x0D
      {NULL              ,      0.0,   0.0},         // 14 0x0E
      {NULL              ,      0.0,   0.0},         // 15 0x0F
      {NULL              ,      0.0,   0.0},         // 16 0x10
      {NULL              ,      0.0,   0.0},         // 17 0x11
      {NULL              ,      0.0,   0.0},         // 18 0x12
      {NULL              ,      0.0,   0.0},         // 19 0x13
      {NULL              ,      0.0,   0.0},         // 20 0x14
      {NULL              ,      0.0,   0.0},         // 21 0x15
      {NULL              ,      0.0,   0.0},         // 22 0x16
      {NULL              ,      0.0,   0.0},         // 23 0x17
      {NULL              ,      0.0,   0.0},         // 24 0x18
      {NULL              ,      0.0,   0.0},         // 25 0x19
      {NULL              ,      0.0,   0.0},         // 26 0x1A
      {NULL              ,      0.0,   0.0},         // 27 0x1B
      {NULL              ,      0.0,   0.0},         // 28 0x1C
      {NULL              ,      0.0,   0.0},         // 29 0x1D
      {NULL              ,      0.0,   0.0},         // 30 0x1E
      {NULL              ,      0.0,   0.0}},        // 31 0x1F
  },

//
//~--  PB2 #2 Board (120V -> 28V Converter) ---
//

//                            slot   sta   ovt   tmp   ref   t_w   t_r  --- adc ----
  {"PB2-2\n120V->28V",        0x03, 0x00, 0x01, 0x01, 0x06, 0x06, 0x06, {0x04, 0x14},

// Digital Channels
     {{"Board Status",     {"OK",  "NOK"}},          //  0 0x00
      {"OVT Alarm",        {"OVT", "OK"}},           //  1 0x01
      {NULL,               {NULL,   NULL}},          //  2 0x02
      {NULL,               {NULL,   NULL}},          //  3 0x03
      {"Input OVC Alarm",  {"OVC", "OK"}},           //  4 0x04
      {"DCDC ON/OFF",      {"OFF", "ON"}},           //  5 0x05
      {"Digital I/F Test", {"0",   "1"}},            //  6 0x06
      {NULL,               {NULL,   NULL}},          //  7 0x07
      {NULL,               {NULL,   NULL}},          //  8 0x08
      {NULL,               {NULL,   NULL}},          //  9 0x09
      {NULL,               {NULL,   NULL}},          // 10 0x0A
      {NULL,               {NULL,   NULL}},          // 11 0x0B
      {NULL,               {NULL,   NULL}},          // 12 0x0C
      {NULL,               {NULL,   NULL}},          // 13 0x0D
      {NULL,               {NULL,   NULL}},          // 14 0x0E
      {NULL,               {NULL,   NULL}},          // 15 0x0F
      {NULL,               {NULL,   NULL}},          // 16 0x10
      {NULL,               {NULL,   NULL}},          // 17 0x11
      {NULL,               {NULL,   NULL}},          // 18 0x12
      {NULL,               {NULL,   NULL}},          // 19 0x13
      {NULL,               {NULL,   NULL}},          // 20 0x14
      {"27V UNV",          {"UNV", "OK"}},           // 21 0x15
      {"29V Trip",         {"OK",  "TRIP"}},         // 22 0x16
      {"DCDC OFF/ON CMD",  {"ON",  "OFF"}},          // 23 0x17
      {NULL,               {NULL,   NULL}},          // 24 0x18
      {NULL,               {NULL,   NULL}},          // 25 0x19
      {NULL,               {NULL,   NULL}},          // 26 0x1A
      {NULL,               {NULL,   NULL}},          // 27 0x1B
      {NULL,               {NULL,   NULL}},          // 28 0x1C
      {NULL,               {NULL,   NULL}},          // 29 0x1D
      {NULL,               {NULL,   NULL}},          // 30 0x1E
      {NULL,               {NULL,   NULL}}},         // 31 0x1F
// Commands
     {{NULL,               {NULL, NULL}},            //  0 0x00
      {NULL,               {NULL, NULL}},            //  1 0x01
      {NULL,               {NULL, NULL}},            //  2 0x02
      {NULL,               {NULL, NULL}},            //  3 0x03
      {NULL,               {NULL, NULL}},            //  4 0x04
      {NULL,               {NULL, NULL}},            //  5 0x05
      {"Digital I/F Test", {"0",   "1"}},            //  6 0x06
      {NULL,               {NULL, NULL}},            //  7 0x07
      {NULL,               {NULL, NULL}},            //  8 0x08
      {NULL,               {NULL, NULL}},            //  9 0x09
      {NULL,               {NULL, NULL}},            // 10 0x0A
      {NULL,               {NULL, NULL}},            // 11 0x0B
      {NULL,               {NULL, NULL}},            // 12 0x0C
      {NULL,               {NULL, NULL}},            // 13 0x0D
      {NULL,               {NULL, NULL}},            // 14 0x0E
      {NULL,               {NULL, NULL}},            // 15 0x0F
      {NULL,               {NULL, NULL}},            // 16 0x10
      {NULL,               {NULL, NULL}},            // 17 0x11
      {NULL,               {NULL, NULL}},            // 18 0x12
      {NULL,               {NULL, NULL}},            // 19 0x13
      {NULL,               {NULL, NULL}},            // 20 0x14
      {NULL,               {NULL, NULL}},            // 21 0x15
      {NULL,               {NULL, NULL}},            // 22 0x16
      {"DCDC OFF/ON",      {"ON",  "OFF"}},          // 23 0x17
      {NULL,               {NULL, NULL}},            // 24 0x18
      {NULL,               {NULL, NULL}},            // 25 0x19
      {NULL,               {NULL, NULL}},            // 26 0x1A
      {NULL,               {NULL, NULL}},            // 27 0x1B
      {NULL,               {NULL, NULL}},            // 28 0x1C
      {NULL,               {NULL, NULL}},            // 29 0x1D
      {NULL,               {NULL, NULL}},            // 30 0x1E
      {NULL,               {NULL, NULL}}},           // 31 0x1F
// Analog Channels
     {{NULL,                  0.0,    0.0},          //  0 0x00
      {"Temperature",       -40.0,   90.0},          //  1 0x01
      {NULL,                  0.0,    0.0},          //  2 0x02
      {NULL,                  0.0,    0.0},          //  3 0x03
      {"Output Voltage",      0.0,   36.76},         //  4 0x04
      {NULL,                  0.0,    0.0},          //  5 0x05
      {"Analog Ref. 2.0V",    0.0,    4.81765},      //  6 0x06
      {NULL,                  0.0,    0.0},          //  7 0x07
      {NULL,                  0.0,    0.0},          //  8 0x08
      {NULL,                  0.0,    0.0},          //  9 0x09
      {NULL,                  0.0,    0.0},          // 10 0x0A
      {NULL,                  0.0,    0.0},          // 11 0x0B
      {NULL,                  0.0,    0.0},          // 12 0x0C
      {NULL,                  0.0,    0.0},          // 13 0x0D
      {NULL,                  0.0,    0.0},          // 14 0x0E
      {NULL,                  0.0,    0.0},          // 15 0x0F
      {NULL,                  0.0,    0.0},          // 16 0x10
      {NULL,                  0.0,    0.0},          // 17 0x11
      {NULL,                  0.0,    0.0},          // 18 0x12
      {NULL,                  0.0,    0.0},          // 19 0x13
      {NULL,                  0.0,    0.0},          // 20 0x14
      {NULL,                  0.0,    0.0},          // 21 0x15
      {NULL,                  0.0,    0.0},          // 22 0x16
      {NULL,                  0.0,    0.0},          // 23 0x17
      {NULL,                  0.0,    0.0},          // 24 0x18
      {NULL,                  0.0,    0.0},          // 25 0x19
      {NULL,                  0.0,    0.0},          // 26 0x1A
      {NULL,                  0.0,    0.0},          // 27 0x1B
      {NULL,                  0.0,    0.0},          // 28 0x1C
      {NULL,                  0.0,    0.0},          // 29 0x1D
      {NULL,                  0.0,    0.0},          // 30 0x1E
      {NULL,                  0.0,    0.0}},         // 31 0x1F
  },
  
//
//~--  ESEM3-A #2 Board (28V Distributor) ---
//

//                            slot   sta   ovt   tmp   ref   t_w   t_r  --- adc ----
  {"ESEM3-A-2\n28V Distrib.", 0x04, 0x00, 0x01, 0x08, 0x09, 0x0A, 0x12, {0x03, 0x13},

// Digital Channels
     {{"Board Status",       {"OK",  "NOK"}},        //  0 0x00
      {"OVT Alarm",          {"OK",  "OVT"}},        //  1 0x01
      {"O_1 CMD: ER1_N",     {"OFF", "ON"}},         //  2 0x02
      {"O_2 CMD: TPD2",      {"OFF", "ON"}},         //  3 0x03
      {"O_3 CMD: TPD6",      {"OFF", "ON"}},         //  4 0x04
      {"O_4 CMD: SPD3_N",    {"OFF", "ON"}},         //  5 0x05
      {"O_5 CMD: MPD_N",     {"OFF", "ON"}},         //  6 0x06
      {"O_6 CMD: UPD1",      {"OFF", "ON"}},         //  7 0x07
      {"O_7 CMD: SPD2_N",    {"OFF", "ON"}},         //  8 0x08
      {"O_8 CMD: CAB_R",     {"OFF", "ON"}},         //  9 0x09
      {"O_1 Status: ER1_N",  {"OFF", "ON"}},         // 10 0x0A
      {"O_2 Status: TPD2",   {"OFF", "ON"}},         // 11 0x0B
      {"O_3 Status: TPD6",   {"OFF", "ON"}},         // 12 0x0C
      {"O_4 Status: SPD3_N", {"OFF", "ON"}},         // 13 0x0D
      {"O_5 Status: MPD_N",  {"OFF", "ON"}},         // 14 0x0E
      {"O_6 Status: UPD1",   {"OFF", "ON"}},         // 15 0x0F
      {"O_7 Status: SPD2_N", {"OFF", "ON"}},         // 16 0x10
      {"O_8 Status: CAB_R",  {"OFF", "ON"}},         // 17 0x11
      {"Digital I/F Test",   {"0",   "1"}},          // 18 0x12
      {NULL              ,   {NULL, NULL}},          // 19 0x13
      {NULL              ,   {NULL, NULL}},          // 20 0x14
      {NULL              ,   {NULL, NULL}},          // 21 0x15
      {NULL              ,   {NULL, NULL}},          // 22 0x16
      {NULL              ,   {NULL, NULL}},          // 23 0x17
      {NULL              ,   {NULL, NULL}},          // 24 0x18
      {NULL              ,   {NULL, NULL}},          // 25 0x19
      {NULL              ,   {NULL, NULL}},          // 26 0x1A
      {NULL              ,   {NULL, NULL}},          // 27 0x1B
      {NULL              ,   {NULL, NULL}},          // 28 0x1C
      {NULL              ,   {NULL, NULL}},          // 29 0x1D
      {NULL              ,   {NULL, NULL}},          // 30 0x1E
      {NULL              ,   {NULL, NULL}}},         // 31 0x1F
// Commands
     {{NULL,                 {NULL, NULL}},          //  0 0x00
      {NULL,                 {NULL, NULL}},          //  1 0x01
      {"O_1: ER1_N",         {"OFF", "ON"}},         //  2 0x02
      {"O_2: TPD2",          {"OFF", "ON"}},         //  3 0x03
      {"O_3: TPD6",          {"OFF", "ON"}},         //  4 0x04
      {"O_4: SPD3_N",        {"OFF", "ON"}},         //  5 0x05
      {"O_5: MPD_N",         {"OFF", "ON"}},         //  6 0x06
      {"O_6: UPD1",          {"OFF", "ON"}},         //  7 0x07
      {"O_7: SPD2_N",        {"OFF", "ON"}},         //  8 0x08
      {"O_8: CAB_R",         {"OFF", "ON"}},         //  9 0x09
      {"Digital I/F Test",   {"0",   "1"}},          // 10 0x0A
      {NULL,                 {NULL, NULL}},          // 11 0x0B
      {NULL,                 {NULL, NULL}},          // 12 0x0C
      {NULL,                 {NULL, NULL}},          // 13 0x0D
      {NULL,                 {NULL, NULL}},          // 14 0x0E
      {NULL,                 {NULL, NULL}},          // 15 0x0F
      {NULL,                 {NULL, NULL}},          // 16 0x10
      {NULL,                 {NULL, NULL}},          // 17 0x11
      {NULL,                 {NULL, NULL}},          // 18 0x12
      {"All Outlets ON/OFF", {"OFF", "ON"}},         // 19 0x13
      {NULL,                 {NULL, NULL}},          // 20 0x14
      {NULL,                 {NULL, NULL}},          // 21 0x15
      {NULL,                 {NULL, NULL}},          // 22 0x16
      {NULL,                 {NULL, NULL}},          // 23 0x17
      {NULL,                 {NULL, NULL}},          // 24 0x18
      {NULL,                 {NULL, NULL}},          // 25 0x19
      {NULL,                 {NULL, NULL}},          // 26 0x1A
      {NULL,                 {NULL, NULL}},          // 27 0x1B
      {NULL,                 {NULL, NULL}},          // 28 0x1C
      {NULL,                 {NULL, NULL}},          // 29 0x1D
      {NULL,                 {NULL, NULL}},          // 30 0x1E
      {NULL,                 {NULL, NULL}}},         // 31 0x1F
// Analog Channels
     {{"O_1 Current: ER1_N",    0.0,   7.5},         //  0 0x00
      {"O_2 Current: TPD2",     0.0,   7.5},         //  1 0x01
      {"O_3 Current: TPD6",     0.0,   7.5},         //  2 0x02
      {"O_4 Current: SPD3_N",   0.0,   7.5},         //  3 0x03
      {"O_5 Current: MPD_N",    0.0,   7.5},         //  4 0x04
      {"O_6 Current: UPD1",     0.0,   7.5},         //  5 0x05
      {"O_7 Current: SPD2_N",   0.0,   7.5},         //  6 0x06
      {"O_8 Current: CAB_R",    0.0,  15.0},         //  7 0x07
      {"Temperature",         -40.0,  80.0},         //  8 0x08
      {"Analog Ref. 3.0V",      0.0,   4.81765},     //  9 0x09
      {NULL              ,      0.0,   0.0},         // 10 0x0A
      {NULL              ,      0.0,   0.0},         // 11 0x0B
      {NULL              ,      0.0,   0.0},         // 12 0x0C
      {NULL              ,      0.0,   0.0},         // 13 0x0D
      {NULL              ,      0.0,   0.0},         // 14 0x0E
      {NULL              ,      0.0,   0.0},         // 15 0x0F
      {NULL              ,      0.0,   0.0},         // 16 0x10
      {NULL              ,      0.0,   0.0},         // 17 0x11
      {NULL              ,      0.0,   0.0},         // 18 0x12
      {NULL              ,      0.0,   0.0},         // 19 0x13
      {NULL              ,      0.0,   0.0},         // 20 0x14
      {NULL              ,      0.0,   0.0},         // 21 0x15
      {NULL              ,      0.0,   0.0},         // 22 0x16
      {NULL              ,      0.0,   0.0},         // 23 0x17
      {NULL              ,      0.0,   0.0},         // 24 0x18
      {NULL              ,      0.0,   0.0},         // 25 0x19
      {NULL              ,      0.0,   0.0},         // 26 0x1A
      {NULL              ,      0.0,   0.0},         // 27 0x1B
      {NULL              ,      0.0,   0.0},         // 28 0x1C
      {NULL              ,      0.0,   0.0},         // 29 0x1D
      {NULL              ,      0.0,   0.0},         // 30 0x1E
      {NULL              ,      0.0,   0.0}},        // 31 0x1F
  },

//
//~--  PB2 #3 Board (120V -> 28V Converter) ---
//

//                            slot   sta   ovt   tmp   ref   t_w   t_r  --- adc ----
  {"PB2-3\n120V->28V",        0x05, 0x00, 0x01, 0x01, 0x06, 0x06, 0x06, {0x02, 0x12},

// Digital Channels
     {{"Board Status",     {"OK",  "NOK"}},          //  0 0x00
      {"OVT Alarm",        {"OVT", "OK"}},           //  1 0x01
      {NULL,               {NULL,   NULL}},          //  2 0x02
      {NULL,               {NULL,   NULL}},          //  3 0x03
      {"Input OVC Alarm",  {"OVC", "OK"}},           //  4 0x04
      {"DCDC ON/OFF",      {"OFF", "ON"}},           //  5 0x05
      {"Digital I/F Test", {"0",   "1"}},            //  6 0x06
      {NULL,               {NULL,   NULL}},          //  7 0x07
      {NULL,               {NULL,   NULL}},          //  8 0x08
      {NULL,               {NULL,   NULL}},          //  9 0x09
      {NULL,               {NULL,   NULL}},          // 10 0x0A
      {NULL,               {NULL,   NULL}},          // 11 0x0B
      {NULL,               {NULL,   NULL}},          // 12 0x0C
      {NULL,               {NULL,   NULL}},          // 13 0x0D
      {NULL,               {NULL,   NULL}},          // 14 0x0E
      {NULL,               {NULL,   NULL}},          // 15 0x0F
      {NULL,               {NULL,   NULL}},          // 16 0x10
      {NULL,               {NULL,   NULL}},          // 17 0x11
      {NULL,               {NULL,   NULL}},          // 18 0x12
      {NULL,               {NULL,   NULL}},          // 19 0x13
      {NULL,               {NULL,   NULL}},          // 20 0x14
      {"27V UNV",          {"UNV", "OK"}},           // 21 0x15
      {"29V Trip",         {"OK",  "TRIP"}},         // 22 0x16
      {"DCDC OFF/ON CMD",  {"ON",  "OFF"}},          // 23 0x17
      {NULL,               {NULL,   NULL}},          // 24 0x18
      {NULL,               {NULL,   NULL}},          // 25 0x19
      {NULL,               {NULL,   NULL}},          // 26 0x1A
      {NULL,               {NULL,   NULL}},          // 27 0x1B
      {NULL,               {NULL,   NULL}},          // 28 0x1C
      {NULL,               {NULL,   NULL}},          // 29 0x1D
      {NULL,               {NULL,   NULL}},          // 30 0x1E
      {NULL,               {NULL,   NULL}}},         // 31 0x1F
// Commands
     {{NULL,               {NULL, NULL}},            //  0 0x00
      {NULL,               {NULL, NULL}},            //  1 0x01
      {NULL,               {NULL, NULL}},            //  2 0x02
      {NULL,               {NULL, NULL}},            //  3 0x03
      {NULL,               {NULL, NULL}},            //  4 0x04
      {NULL,               {NULL, NULL}},            //  5 0x05
      {"Digital I/F Test", {"0",   "1"}},            //  6 0x06
      {NULL,               {NULL, NULL}},            //  7 0x07
      {NULL,               {NULL, NULL}},            //  8 0x08
      {NULL,               {NULL, NULL}},            //  9 0x09
      {NULL,               {NULL, NULL}},            // 10 0x0A
      {NULL,               {NULL, NULL}},            // 11 0x0B
      {NULL,               {NULL, NULL}},            // 12 0x0C
      {NULL,               {NULL, NULL}},            // 13 0x0D
      {NULL,               {NULL, NULL}},            // 14 0x0E
      {NULL,               {NULL, NULL}},            // 15 0x0F
      {NULL,               {NULL, NULL}},            // 16 0x10
      {NULL,               {NULL, NULL}},            // 17 0x11
      {NULL,               {NULL, NULL}},            // 18 0x12
      {NULL,               {NULL, NULL}},            // 19 0x13
      {NULL,               {NULL, NULL}},            // 20 0x14
      {NULL,               {NULL, NULL}},            // 21 0x15
      {NULL,               {NULL, NULL}},            // 22 0x16
      {"DCDC OFF/ON",      {"ON",  "OFF"}},          // 23 0x17
      {NULL,               {NULL, NULL}},            // 24 0x18
      {NULL,               {NULL, NULL}},            // 25 0x19
      {NULL,               {NULL, NULL}},            // 26 0x1A
      {NULL,               {NULL, NULL}},            // 27 0x1B
      {NULL,               {NULL, NULL}},            // 28 0x1C
      {NULL,               {NULL, NULL}},            // 29 0x1D
      {NULL,               {NULL, NULL}},            // 30 0x1E
      {NULL,               {NULL, NULL}}},           // 31 0x1F
// Analog Channels
     {{NULL,                  0.0,    0.0},          //  0 0x00
      {"Temperature",       -40.0,   90.0},          //  1 0x01
      {NULL,                  0.0,    0.0},          //  2 0x02
      {NULL,                  0.0,    0.0},          //  3 0x03
      {"Output Voltage",      0.0,   36.76},         //  4 0x04
      {NULL,                  0.0,    0.0},          //  5 0x05
      {"Analog Ref. 2.0V",    0.0,    4.81765},      //  6 0x06
      {NULL,                  0.0,    0.0},          //  7 0x07
      {NULL,                  0.0,    0.0},          //  8 0x08
      {NULL,                  0.0,    0.0},          //  9 0x09
      {NULL,                  0.0,    0.0},          // 10 0x0A
      {NULL,                  0.0,    0.0},          // 11 0x0B
      {NULL,                  0.0,    0.0},          // 12 0x0C
      {NULL,                  0.0,    0.0},          // 13 0x0D
      {NULL,                  0.0,    0.0},          // 14 0x0E
      {NULL,                  0.0,    0.0},          // 15 0x0F
      {NULL,                  0.0,    0.0},          // 16 0x10
      {NULL,                  0.0,    0.0},          // 17 0x11
      {NULL,                  0.0,    0.0},          // 18 0x12
      {NULL,                  0.0,    0.0},          // 19 0x13
      {NULL,                  0.0,    0.0},          // 20 0x14
      {NULL,                  0.0,    0.0},          // 21 0x15
      {NULL,                  0.0,    0.0},          // 22 0x16
      {NULL,                  0.0,    0.0},          // 23 0x17
      {NULL,                  0.0,    0.0},          // 24 0x18
      {NULL,                  0.0,    0.0},          // 25 0x19
      {NULL,                  0.0,    0.0},          // 26 0x1A
      {NULL,                  0.0,    0.0},          // 27 0x1B
      {NULL,                  0.0,    0.0},          // 28 0x1C
      {NULL,                  0.0,    0.0},          // 29 0x1D
      {NULL,                  0.0,    0.0},          // 30 0x1E
      {NULL,                  0.0,    0.0}},         // 31 0x1F
  },

//
//~--  ESEM3-A #3 Board (28V Distributor) ---
//

//                            slot   sta   ovt   tmp   ref   t_w   t_r  --- adc ----
  {"ESEM3-A-3\n28V Distrib.", 0x06, 0x00, 0x01, 0x08, 0x09, 0x0A, 0x12, {0x01, 0x11},

// Digital Channels
     {{"Board Status",       {"OK",  "NOK"}},        //  0 0x00
      {"OVT Alarm",          {"OK",  "OVT"}},        //  1 0x01
      {"O_1 CMD: TPD0",      {"OFF", "ON"}},         //  2 0x02
      {"O_2 CMD: TPD4",      {"OFF", "ON"}},         //  3 0x03
      {"O_3 CMD: ER0_N",     {"OFF", "ON"}},         //  4 0x04
      {"O_4 CMD: MPD_R",     {"OFF", "ON"}},         //  5 0x05
      {"O_5 CMD: SPD3_R",    {"OFF", "ON"}},         //  6 0x06
      {"O_6 CMD: UGPD_R",    {"OFF", "ON"}},         //  7 0x07
      {"O_7 CMD: SPD2_R",    {"OFF", "ON"}},         //  8 0x08
      {"O_8 CMD: CAB_N",     {"OFF", "ON"}},         //  9 0x09
      {"O_1 Status: TPD0",   {"OFF", "ON"}},         // 10 0x0A
      {"O_2 Status: TPD4",   {"OFF", "ON"}},         // 11 0x0B
      {"O_3 Status: ER0_N",  {"OFF", "ON"}},         // 12 0x0C
      {"O_4 Status: MPD_R",  {"OFF", "ON"}},         // 13 0x0D
      {"O_5 Status: SPD3_R", {"OFF", "ON"}},         // 14 0x0E
      {"O_6 Status: UGPD_R", {"OFF", "ON"}},         // 15 0x0F
      {"O_7 Status: SPD2_R", {"OFF", "ON"}},         // 16 0x10
      {"O_8 Status: CAB_N",  {"OFF", "ON"}},         // 17 0x11
      {"Digital I/F Test",   {"0",   "1"}},          // 18 0x12
      {NULL              ,   {NULL, NULL}},          // 19 0x13
      {NULL              ,   {NULL, NULL}},          // 20 0x14
      {NULL              ,   {NULL, NULL}},          // 21 0x15
      {NULL              ,   {NULL, NULL}},          // 22 0x16
      {NULL              ,   {NULL, NULL}},          // 23 0x17
      {NULL              ,   {NULL, NULL}},          // 24 0x18
      {NULL              ,   {NULL, NULL}},          // 25 0x19
      {NULL              ,   {NULL, NULL}},          // 26 0x1A
      {NULL              ,   {NULL, NULL}},          // 27 0x1B
      {NULL              ,   {NULL, NULL}},          // 28 0x1C
      {NULL              ,   {NULL, NULL}},          // 29 0x1D
      {NULL              ,   {NULL, NULL}},          // 30 0x1E
      {NULL              ,   {NULL, NULL}}},         // 31 0x1F
// Commands
     {{NULL,                 {NULL, NULL}},          //  0 0x00
      {NULL,                 {NULL, NULL}},          //  1 0x01
      {"O_1: TPD0",          {"OFF", "ON"}},         //  2 0x02
      {"O_2: TPD4",          {"OFF", "ON"}},         //  3 0x03
      {"O_3: ER0_N",         {"OFF", "ON"}},         //  4 0x04
      {"O_4: MPD_R",         {"OFF", "ON"}},         //  5 0x05
      {"O_5: SPD3_R",        {"OFF", "ON"}},         //  6 0x06
      {"O_6: UGPD_R",        {"OFF", "ON"}},         //  7 0x07
      {"O_7: SPD2_R",        {"OFF", "ON"}},         //  8 0x08
      {"O_8: CAB_N",         {"OFF", "ON"}},         //  9 0x09
      {"Digital I/F Test",   {"0",   "1"}},          // 10 0x0A
      {NULL,                 {NULL, NULL}},          // 11 0x0B
      {NULL,                 {NULL, NULL}},          // 12 0x0C
      {NULL,                 {NULL, NULL}},          // 13 0x0D
      {NULL,                 {NULL, NULL}},          // 14 0x0E
      {NULL,                 {NULL, NULL}},          // 15 0x0F
      {NULL,                 {NULL, NULL}},          // 16 0x10
      {NULL,                 {NULL, NULL}},          // 17 0x11
      {NULL,                 {NULL, NULL}},          // 18 0x12
      {"All Outlets ON/OFF", {"OFF", "ON"}},         // 19 0x13
      {NULL,                 {NULL, NULL}},          // 20 0x14
      {NULL,                 {NULL, NULL}},          // 21 0x15
      {NULL,                 {NULL, NULL}},          // 22 0x16
      {NULL,                 {NULL, NULL}},          // 23 0x17
      {NULL,                 {NULL, NULL}},          // 24 0x18
      {NULL,                 {NULL, NULL}},          // 25 0x19
      {NULL,                 {NULL, NULL}},          // 26 0x1A
      {NULL,                 {NULL, NULL}},          // 27 0x1B
      {NULL,                 {NULL, NULL}},          // 28 0x1C
      {NULL,                 {NULL, NULL}},          // 29 0x1D
      {NULL,                 {NULL, NULL}},          // 30 0x1E
      {NULL,                 {NULL, NULL}}},         // 31 0x1F
// Analog Channels
     {{"O_1 Current: TPD0",     0.0,   7.5},         //  0 0x00
      {"O_2 Current: TPD4",     0.0,   7.5},         //  1 0x01
      {"O_3 Current: ER0_N",    0.0,   7.5},         //  2 0x02
      {"O_4 Current: MPD_R",    0.0,   7.5},         //  3 0x03
      {"O_5 Current: SPD3_R",   0.0,   7.5},         //  4 0x04
      {"O_6 Current: UGPD_R",   0.0,   7.5},         //  5 0x05
      {"O_7 Current: SPD2_R",   0.0,   7.5},         //  6 0x06
      {"O_8 Current: CAB_N",    0.0,  15.0},         //  7 0x07
      {"Temperature",         -40.0,  80.0},         //  8 0x08
      {"Analog Ref. 3.0V",      0.0,   4.81765},     //  9 0x09
      {NULL              ,      0.0,   0.0},         // 10 0x0A
      {NULL              ,      0.0,   0.0},         // 11 0x0B
      {NULL              ,      0.0,   0.0},         // 12 0x0C
      {NULL              ,      0.0,   0.0},         // 13 0x0D
      {NULL              ,      0.0,   0.0},         // 14 0x0E
      {NULL              ,      0.0,   0.0},         // 15 0x0F
      {NULL              ,      0.0,   0.0},         // 16 0x10
      {NULL              ,      0.0,   0.0},         // 17 0x11
      {NULL              ,      0.0,   0.0},         // 18 0x12
      {NULL              ,      0.0,   0.0},         // 19 0x13
      {NULL              ,      0.0,   0.0},         // 20 0x14
      {NULL              ,      0.0,   0.0},         // 21 0x15
      {NULL              ,      0.0,   0.0},         // 22 0x16
      {NULL              ,      0.0,   0.0},         // 23 0x17
      {NULL              ,      0.0,   0.0},         // 24 0x18
      {NULL              ,      0.0,   0.0},         // 25 0x19
      {NULL              ,      0.0,   0.0},         // 26 0x1A
      {NULL              ,      0.0,   0.0},         // 27 0x1B
      {NULL              ,      0.0,   0.0},         // 28 0x1C
      {NULL              ,      0.0,   0.0},         // 29 0x1D
      {NULL              ,      0.0,   0.0},         // 30 0x1E
      {NULL              ,      0.0,   0.0}},        // 31 0x1F
  },

//
//~--  PB2 #4 Board (120V -> 28V Converter) ---
//

//                            slot   sta   ovt   tmp   ref   t_w   t_r  --- adc ----
  {"PB2-4\n120V->28V",        0x07, 0x00, 0x01, 0x01, 0x06, 0x06, 0x06, {0x00, 0x10},

// Digital Channels
     {{"Board Status",     {"OK",  "NOK"}},          //  0 0x00
      {"OVT Alarm",        {"OVT", "OK"}},           //  1 0x01
      {NULL,               {NULL,   NULL}},          //  2 0x02
      {NULL,               {NULL,   NULL}},          //  3 0x03
      {"Input OVC Alarm",  {"OVC", "OK"}},           //  4 0x04
      {"DCDC ON/OFF",      {"OFF", "ON"}},           //  5 0x05
      {"Digital I/F Test", {"0",   "1"}},            //  6 0x06
      {NULL,               {NULL,   NULL}},          //  7 0x07
      {NULL,               {NULL,   NULL}},          //  8 0x08
      {NULL,               {NULL,   NULL}},          //  9 0x09
      {NULL,               {NULL,   NULL}},          // 10 0x0A
      {NULL,               {NULL,   NULL}},          // 11 0x0B
      {NULL,               {NULL,   NULL}},          // 12 0x0C
      {NULL,               {NULL,   NULL}},          // 13 0x0D
      {NULL,               {NULL,   NULL}},          // 14 0x0E
      {NULL,               {NULL,   NULL}},          // 15 0x0F
      {NULL,               {NULL,   NULL}},          // 16 0x10
      {NULL,               {NULL,   NULL}},          // 17 0x11
      {NULL,               {NULL,   NULL}},          // 18 0x12
      {NULL,               {NULL,   NULL}},          // 19 0x13
      {NULL,               {NULL,   NULL}},          // 20 0x14
      {"27V UNV",          {"UNV", "OK"}},           // 21 0x15
      {"29V Trip",         {"OK",  "TRIP"}},         // 22 0x16
      {"DCDC OFF/ON CMD",  {"ON",  "OFF"}},          // 23 0x17
      {NULL,               {NULL,   NULL}},          // 24 0x18
      {NULL,               {NULL,   NULL}},          // 25 0x19
      {NULL,               {NULL,   NULL}},          // 26 0x1A
      {NULL,               {NULL,   NULL}},          // 27 0x1B
      {NULL,               {NULL,   NULL}},          // 28 0x1C
      {NULL,               {NULL,   NULL}},          // 29 0x1D
      {NULL,               {NULL,   NULL}},          // 30 0x1E
      {NULL,               {NULL,   NULL}}},         // 31 0x1F
// Commands
     {{NULL,               {NULL, NULL}},            //  0 0x00
      {NULL,               {NULL, NULL}},            //  1 0x01
      {NULL,               {NULL, NULL}},            //  2 0x02
      {NULL,               {NULL, NULL}},            //  3 0x03
      {NULL,               {NULL, NULL}},            //  4 0x04
      {NULL,               {NULL, NULL}},            //  5 0x05
      {"Digital I/F Test", {"0",   "1"}},            //  6 0x06
      {NULL,               {NULL, NULL}},            //  7 0x07
      {NULL,               {NULL, NULL}},            //  8 0x08
      {NULL,               {NULL, NULL}},            //  9 0x09
      {NULL,               {NULL, NULL}},            // 10 0x0A
      {NULL,               {NULL, NULL}},            // 11 0x0B
      {NULL,               {NULL, NULL}},            // 12 0x0C
      {NULL,               {NULL, NULL}},            // 13 0x0D
      {NULL,               {NULL, NULL}},            // 14 0x0E
      {NULL,               {NULL, NULL}},            // 15 0x0F
      {NULL,               {NULL, NULL}},            // 16 0x10
      {NULL,               {NULL, NULL}},            // 17 0x11
      {NULL,               {NULL, NULL}},            // 18 0x12
      {NULL,               {NULL, NULL}},            // 19 0x13
      {NULL,               {NULL, NULL}},            // 20 0x14
      {NULL,               {NULL, NULL}},            // 21 0x15
      {NULL,               {NULL, NULL}},            // 22 0x16
      {"DCDC OFF/ON",      {"ON",  "OFF"}},          // 23 0x17
      {NULL,               {NULL, NULL}},            // 24 0x18
      {NULL,               {NULL, NULL}},            // 25 0x19
      {NULL,               {NULL, NULL}},            // 26 0x1A
      {NULL,               {NULL, NULL}},            // 27 0x1B
      {NULL,               {NULL, NULL}},            // 28 0x1C
      {NULL,               {NULL, NULL}},            // 29 0x1D
      {NULL,               {NULL, NULL}},            // 30 0x1E
      {NULL,               {NULL, NULL}}},           // 31 0x1F
// Analog Channels
     {{NULL,                  0.0,    0.0},          //  0 0x00
      {"Temperature",       -40.0,   90.0},          //  1 0x01
      {NULL,                  0.0,    0.0},          //  2 0x02
      {NULL,                  0.0,    0.0},          //  3 0x03
      {"Output Voltage",      0.0,   36.76},         //  4 0x04
      {NULL,                  0.0,    0.0},          //  5 0x05
      {"Analog Ref. 2.0V",    0.0,    4.81765},      //  6 0x06
      {NULL,                  0.0,    0.0},          //  7 0x07
      {NULL,                  0.0,    0.0},          //  8 0x08
      {NULL,                  0.0,    0.0},          //  9 0x09
      {NULL,                  0.0,    0.0},          // 10 0x0A
      {NULL,                  0.0,    0.0},          // 11 0x0B
      {NULL,                  0.0,    0.0},          // 12 0x0C
      {NULL,                  0.0,    0.0},          // 13 0x0D
      {NULL,                  0.0,    0.0},          // 14 0x0E
      {NULL,                  0.0,    0.0},          // 15 0x0F
      {NULL,                  0.0,    0.0},          // 16 0x10
      {NULL,                  0.0,    0.0},          // 17 0x11
      {NULL,                  0.0,    0.0},          // 18 0x12
      {NULL,                  0.0,    0.0},          // 19 0x13
      {NULL,                  0.0,    0.0},          // 20 0x14
      {NULL,                  0.0,    0.0},          // 21 0x15
      {NULL,                  0.0,    0.0},          // 22 0x16
      {NULL,                  0.0,    0.0},          // 23 0x17
      {NULL,                  0.0,    0.0},          // 24 0x18
      {NULL,                  0.0,    0.0},          // 25 0x19
      {NULL,                  0.0,    0.0},          // 26 0x1A
      {NULL,                  0.0,    0.0},          // 27 0x1B
      {NULL,                  0.0,    0.0},          // 28 0x1C
      {NULL,                  0.0,    0.0},          // 29 0x1D
      {NULL,                  0.0,    0.0},          // 30 0x1E
      {NULL,                  0.0,    0.0}},         // 31 0x1F
  },

//
//~--  ESEM3-A #4 Board (28V Distributor) ---
//

//                            slot   sta   ovt   tmp   ref   t_w   t_r  --- adc ----
  {"ESEM3-A-4\n28V Distrib.", 0x08, 0x00, 0x01, 0x08, 0x09, 0x0A, 0x12, {0x0F, 0x1F},

// Digital Channels
     {{"Board Status",       {"OK",  "NOK"}},        //  0 0x00
      {"OVT Alarm",          {"OK",  "OVT"}},        //  1 0x01
      {"O_1 CMD: JPD_R",     {"OFF", "ON"}},         //  2 0x02
      {"O_2 CMD: ER1_R",     {"OFF", "ON"}},         //  3 0x03
      {"O_3 CMD: SPD1_R",    {"OFF", "ON"}},         //  4 0x04
      {"O_4 CMD: ER0_R",     {"OFF", "ON"}},         //  5 0x05
      {"O_5 CMD: UPD0",      {"OFF", "ON"}},         //  6 0x06
      {"O_6 CMD: SPD0_N",    {"OFF", "ON"}},         //  7 0x07
      {"O_7 CMD: TPD5",      {"OFF", "ON"}},         //  8 0x08
      {"O_8 CMD: TTPD_N",    {"OFF", "ON"}},         //  9 0x09
      {"O_1 Status: JPD_R",  {"OFF", "ON"}},         // 10 0x0A
      {"O_2 Status: ER1_R",  {"OFF", "ON"}},         // 11 0x0B
      {"O_3 Status: SPD1_R", {"OFF", "ON"}},         // 12 0x0C
      {"O_4 Status: ER0_R",  {"OFF", "ON"}},         // 13 0x0D
      {"O_5 Status: UPD0",   {"OFF", "ON"}},         // 14 0x0E
      {"O_6 Status: SPD0_N", {"OFF", "ON"}},         // 15 0x0F
      {"O_7 Status: TPD5",   {"OFF", "ON"}},         // 16 0x10
      {"O_8 Status: TTPD_N", {"OFF", "ON"}},         // 17 0x11
      {"Digital I/F Test",   {"0",   "1"}},          // 18 0x12
      {NULL              ,   {NULL, NULL}},          // 19 0x13
      {NULL              ,   {NULL, NULL}},          // 20 0x14
      {NULL              ,   {NULL, NULL}},          // 21 0x15
      {NULL              ,   {NULL, NULL}},          // 22 0x16
      {NULL              ,   {NULL, NULL}},          // 23 0x17
      {NULL              ,   {NULL, NULL}},          // 24 0x18
      {NULL              ,   {NULL, NULL}},          // 25 0x19
      {NULL              ,   {NULL, NULL}},          // 26 0x1A
      {NULL              ,   {NULL, NULL}},          // 27 0x1B
      {NULL              ,   {NULL, NULL}},          // 28 0x1C
      {NULL              ,   {NULL, NULL}},          // 29 0x1D
      {NULL              ,   {NULL, NULL}},          // 30 0x1E
      {NULL              ,   {NULL, NULL}}},         // 31 0x1F
// Commands
     {{NULL,                 {NULL, NULL}},          //  0 0x00
      {NULL,                 {NULL, NULL}},          //  1 0x01
      {"O_1: JPD_R",         {"OFF", "ON"}},         //  2 0x02
      {"O_2: ER1_R",         {"OFF", "ON"}},         //  3 0x03
      {"O_3: SPD1_R",        {"OFF", "ON"}},         //  4 0x04
      {"O_4: ER0_R",         {"OFF", "ON"}},         //  5 0x05
      {"O_5: UPD0",          {"OFF", "ON"}},         //  6 0x06
      {"O_6: SPD0_N",        {"OFF", "ON"}},         //  7 0x07
      {"O_7: TPD5",          {"OFF", "ON"}},         //  8 0x08
      {"O_8: TTPD_N",        {"OFF", "ON"}},         //  9 0x09
      {"Digital I/F Test",   {"0",   "1"}},          // 10 0x0A
      {NULL,                 {NULL, NULL}},          // 11 0x0B
      {NULL,                 {NULL, NULL}},          // 12 0x0C
      {NULL,                 {NULL, NULL}},          // 13 0x0D
      {NULL,                 {NULL, NULL}},          // 14 0x0E
      {NULL,                 {NULL, NULL}},          // 15 0x0F
      {NULL,                 {NULL, NULL}},          // 16 0x10
      {NULL,                 {NULL, NULL}},          // 17 0x11
      {NULL,                 {NULL, NULL}},          // 18 0x12
      {"All Outlets ON/OFF", {"OFF", "ON"}},         // 19 0x13
      {NULL,                 {NULL, NULL}},          // 20 0x14
      {NULL,                 {NULL, NULL}},          // 21 0x15
      {NULL,                 {NULL, NULL}},          // 22 0x16
      {NULL,                 {NULL, NULL}},          // 23 0x17
      {NULL,                 {NULL, NULL}},          // 24 0x18
      {NULL,                 {NULL, NULL}},          // 25 0x19
      {NULL,                 {NULL, NULL}},          // 26 0x1A
      {NULL,                 {NULL, NULL}},          // 27 0x1B
      {NULL,                 {NULL, NULL}},          // 28 0x1C
      {NULL,                 {NULL, NULL}},          // 29 0x1D
      {NULL,                 {NULL, NULL}},          // 30 0x1E
      {NULL,                 {NULL, NULL}}},         // 31 0x1F
// Analog Channels
     {{"O_1 Current: JPD_R",    0.0,   7.5},         //  0 0x00
      {"O_2 Current: ER1_R",    0.0,   7.5},         //  1 0x01
      {"O_3 Current: SPD1_R",   0.0,   7.5},         //  2 0x02
      {"O_4 Current: ER0_R",    0.0,   7.5},         //  3 0x03
      {"O_5 Current: UPD0",     0.0,   7.5},         //  4 0x04
      {"O_6 Current: SPD0_N",   0.0,   7.5},         //  5 0x05
      {"O_7 Current: TPD5",     0.0,   7.5},         //  6 0x06
      {"O_8 Current: TTPD_N",   0.0,  15.0},         //  7 0x07
      {"Temperature",         -40.0,  80.0},         //  8 0x08
      {"Analog Ref. 3.0V",      0.0,   4.81765},     //  9 0x09
      {NULL              ,      0.0,   0.0},         // 10 0x0A
      {NULL              ,      0.0,   0.0},         // 11 0x0B
      {NULL              ,      0.0,   0.0},         // 12 0x0C
      {NULL              ,      0.0,   0.0},         // 13 0x0D
      {NULL              ,      0.0,   0.0},         // 14 0x0E
      {NULL              ,      0.0,   0.0},         // 15 0x0F
      {NULL              ,      0.0,   0.0},         // 16 0x10
      {NULL              ,      0.0,   0.0},         // 17 0x11
      {NULL              ,      0.0,   0.0},         // 18 0x12
      {NULL              ,      0.0,   0.0},         // 19 0x13
      {NULL              ,      0.0,   0.0},         // 20 0x14
      {NULL              ,      0.0,   0.0},         // 21 0x15
      {NULL              ,      0.0,   0.0},         // 22 0x16
      {NULL              ,      0.0,   0.0},         // 23 0x17
      {NULL              ,      0.0,   0.0},         // 24 0x18
      {NULL              ,      0.0,   0.0},         // 25 0x19
      {NULL              ,      0.0,   0.0},         // 26 0x1A
      {NULL              ,      0.0,   0.0},         // 27 0x1B
      {NULL              ,      0.0,   0.0},         // 28 0x1C
      {NULL              ,      0.0,   0.0},         // 29 0x1D
      {NULL              ,      0.0,   0.0},         // 30 0x1E
      {NULL              ,      0.0,   0.0}},        // 31 0x1F
  },

//
//~--  ESEM1-A Board (15V Internal Power Supply) ---
//

//                             slot   sta   ovt   tmp   ref   t_w   t_r  --- adc ----
  {"ESEM1-A\n15V Int. Power",  0x0A, 0x00, 0xFF, 0x01, 0x06, 0x06, 0x06, {0x0E, 0x1E},

// Digital Channels
     {{"Board Status",      {"OK",  "NOK"}},         //  0 0x00
      {NULL,                {NULL,   NULL}},         //  1 0x01
      {NULL,                {NULL,   NULL}},         //  2 0x02
      {"DCDC 1 OVC",        {"OK",  "OVC"}},         //  3 0x03
      {"DCDC 2 OVC",        {"OK",  "OVC"}},         //  4 0x04
      {"DCDC 1 Status",     {"OFF", "ON"}},          //  5 0x05
      {"Digital I/F Test",  {"0",   "1"}},           //  6 0x06
      {"DCDC 2 Status",     {"OFF", "ON"}},          //  7 0x07
      {NULL,                {NULL,   NULL}},         //  8 0x08
      {"Main Power Status", {"OFF", "ON"}},          //  9 0x09
      {NULL,                {NULL,   NULL}},         // 10 0x0A
      {NULL,                {NULL,   NULL}},         // 11 0x0B
      {NULL,                {NULL,   NULL}},         // 12 0x0C
      {NULL,                {NULL,   NULL}},         // 13 0x0D
      {NULL,                {NULL,   NULL}},         // 14 0x0E
      {NULL,                {NULL,   NULL}},         // 15 0x0F
      {NULL,                {NULL,   NULL}},         // 16 0x10
      {NULL,                {NULL,   NULL}},         // 17 0x11
      {NULL,                {NULL,   NULL}},         // 18 0x12
      {NULL,                {NULL,   NULL}},         // 19 0x13
      {NULL,                {NULL,   NULL}},         // 20 0x14
      {NULL,                {NULL,   NULL}},         // 21 0x15
      {NULL,                {NULL,   NULL}},         // 22 0x16
      {NULL,                {NULL,   NULL}},         // 23 0x17
      {NULL,                {NULL,   NULL}},         // 24 0x18
      {NULL,                {NULL,   NULL}},         // 25 0x19
      {NULL,                {NULL,   NULL}},         // 26 0x1A
      {NULL,                {NULL,   NULL}},         // 27 0x1B
      {NULL,                {NULL,   NULL}},         // 28 0x1C
      {NULL,                {NULL,   NULL}},         // 29 0x1D
      {NULL,                {NULL,   NULL}},         // 30 0x1E
      {NULL,                {NULL,   NULL}}},        // 31 0x1F
// Commands
     {{NULL,                {NULL,   NULL}},         //  0 0x00
      {NULL,                {NULL,   NULL}},         //  1 0x01
      {NULL,                {NULL,   NULL}},         //  2 0x02
      {NULL,                {NULL,   NULL}},         //  3 0x03
      {NULL,                {NULL,   NULL}},         //  4 0x04
      {NULL,                {NULL,   NULL}},         //  5 0x05
      {"Digital I/F Test",  {"0",    "1"}},          //  6 0x06
      {"Global ON/OFF",     {"ON",   "OFF"}},        //  7 0x07
      {NULL,                {NULL,   NULL}},         //  8 0x08
      {NULL,                {NULL,   NULL}},         //  9 0x09
      {NULL,                {NULL,   NULL}},         // 10 0x0A
      {NULL,                {NULL,   NULL}},         // 11 0x0B
      {NULL,                {NULL,   NULL}},         // 12 0x0C
      {NULL,                {NULL,   NULL}},         // 13 0x0D
      {NULL,                {NULL,   NULL}},         // 14 0x0E
      {NULL,                {NULL,   NULL}},         // 15 0x0F
      {NULL,                {NULL,   NULL}},         // 16 0x10
      {NULL,                {NULL,   NULL}},         // 17 0x11
      {NULL,                {NULL,   NULL}},         // 18 0x12
      {NULL,                {NULL,   NULL}},         // 19 0x13
      {NULL,                {NULL,   NULL}},         // 20 0x14
      {NULL,                {NULL,   NULL}},         // 21 0x15
      {NULL,                {NULL,   NULL}},         // 22 0x16
      {NULL,                {NULL,   NULL}},         // 23 0x17
      {NULL,                {NULL,   NULL}},         // 24 0x18
      {NULL,                {NULL,   NULL}},         // 25 0x19
      {NULL,                {NULL,   NULL}},         // 26 0x1A
      {NULL,                {NULL,   NULL}},         // 27 0x1B
      {NULL,                {NULL,   NULL}},         // 28 0x1C
      {NULL,                {NULL,   NULL}},         // 29 0x1D
      {NULL,                {NULL,   NULL}},         // 30 0x1E
      {NULL,                {NULL,   NULL}}},        // 31 0x1F
// Analog Channels
     {{NULL,                 0.0,    0.0},           //  0 0x00
      {"Temperature",      -40.0,   80.0},           //  1 0x01
      {"Main Power Volt.",   0.0,  150.0},		       //  2 0x02
      {NULL,                 0.0,    0.0},           //  3 0x03
      {NULL,                 0.0,    0.0},           //  4 0x04
      {NULL,                 0.0,    0.0},           //  5 0x05
      {"Analog Ref. 4.0V",   0.0,    4.81765},       //  6 0x06
      {NULL,                 0.0,    0.0},           //  7 0x07
      {NULL,                 0.0,    0.0},           //  8 0x08
      {NULL,                 0.0,    0.0},           //  9 0x09
      {NULL,                 0.0,    0.0},           // 10 0x0A
      {NULL,                 0.0,    0.0},           // 11 0x0B
      {NULL,                 0.0,    0.0},           // 12 0x0C
      {NULL,                 0.0,    0.0},           // 13 0x0D
      {NULL,                 0.0,    0.0},           // 14 0x0E
      {NULL,                 0.0,    0.0},           // 15 0x0F
      {NULL,                 0.0,    0.0},           // 16 0x10
      {NULL,                 0.0,    0.0},           // 17 0x11
      {NULL,                 0.0,    0.0},           // 18 0x12
      {NULL,                 0.0,    0.0},           // 19 0x13
      {NULL,                 0.0,    0.0},           // 20 0x14
      {NULL,                 0.0,    0.0},           // 21 0x15
      {NULL,                 0.0,    0.0},           // 22 0x16
      {NULL,                 0.0,    0.0},           // 23 0x17
      {NULL,                 0.0,    0.0},           // 24 0x18
      {NULL,                 0.0,    0.0},           // 25 0x19
      {NULL,                 0.0,    0.0},           // 26 0x1A
      {NULL,                 0.0,    0.0},           // 27 0x1B
      {NULL,                 0.0,    0.0},           // 28 0x1C
      {NULL,                 0.0,    0.0},           // 29 0x1D
      {NULL,                 0.0,    0.0},           // 30 0x1E
      {NULL,                 0.0,    0.0}}           // 31 0x1F
  }};

//~----------------------------------------------------------------------------

_brd fm_brd[] = {
  
//
//~--  ESEM3-B Board (120V Distributor) ---
//

//                            slot   sta   ovt   tmp   ref   t_w   t_r  --- adc ----
  {"ESEM3-B\n120V Distrib.",  0x00, 0x00, 0x01, 0x00, 0x02, 0x1B, 0x1B, {0x07, 0x17},

// Digital Channels
     {{"Board Status",          {"OK",  "NOK"}},     //  0 0x00
      {"OVT Alarm",             {"OK",  "OVT"}},     //  1 0x01
      {"O__1 CMD: AH_OUT1",     {"OFF", "ON"}},      //  2 0x02
      {"O__2 CMD: AH_OUT2",     {"OFF", "ON"}},      //  3 0x03
      {"O__3 CMD: AH_OUT3",     {"OFF", "ON"}},      //  4 0x04
      {"O__4 CMD: AH_OUT4",     {"OFF", "ON"}},      //  5 0x05
      {"O__5 CMD: AH_OUT5",     {"OFF", "ON"}},      //  6 0x06
      {"O__6 CMD: AH_OUT6",     {"OFF", "ON"}},      //  7 0x07
      {"O__7 CMD: AH_OUT7",     {"OFF", "ON"}},      //  8 0x08
      {"O__8 CMD: AH_OUT8",     {"OFF", "ON"}},      //  9 0x09
      {"O__9 CMD: AH_OUT9",     {"OFF", "ON"}},      // 10 0x0A
      {"O_10 CMD: AH_OUT10",    {"OFF", "ON"}},      // 11 0x0B
      {"O_11 CMD: AH_OUT11",    {"OFF", "ON"}},      // 12 0x0C
      {"O_12 CMD: AO_CCEB",     {"OFF", "ON"}},      // 13 0x0D
      {"O__1 Status: AH_OUT1",  {"OFF", "ON"}},      // 14 0x0E
      {"O__2 Status: AH_OUT2",  {"OFF", "ON"}},      // 15 0x0F
      {"O__3 Status: AH_OUT3",  {"OFF", "ON"}},      // 16 0x10
      {"O__4 Status: AH_OUT4",  {"OFF", "ON"}},      // 17 0x11
      {"O__5 Status: AH_OUT5",  {"OFF", "ON"}},      // 18 0x12
      {"O__6 Status: AH_OUT6",  {"OFF", "ON"}},      // 19 0x13
      {"O__7 Status: AH_OUT7",  {"OFF", "ON"}},      // 20 0x14
      {"O__8 Status: AH_OUT8",  {"OFF", "ON"}},      // 21 0x15
      {"O__9 Status: AH_OUT9",  {"OFF", "ON"}},      // 22 0x16
      {"O_10 Status: AH_OUT10", {"OFF", "ON"}},      // 23 0x17
      {"O_11 Status: AH_OUT11", {"OFF", "ON"}},      // 24 0x18
      {"O_12 Status: AO_CCEB",  {"OFF", "ON"}},      // 25 0x19
      {"O_12 Trip: AO_CCEB",    {"OK",  "TRIP"}},    // 26 0x1A
      {"Digital I/F Test",      {"0",   "1"}},       // 27 0x1B
      {NULL,                    {NULL,  NULL}},      // 28 0x1C
      {NULL,                    {NULL,  NULL}},      // 29 0x1D
      {NULL,                    {NULL,  NULL}},      // 30 0x1E
      {NULL,                    {NULL,  NULL}}},     // 31 0x1F
// Commands
     {{NULL,                 {NULL, NULL}},          //  0 0x00
      {NULL,                 {NULL, NULL}},          //  1 0x01
      {"O__1: AH_OUT1",      {"OFF", "ON"}},         //  2 0x02
      {"O__2: AH_OUT2",      {"OFF", "ON"}},         //  3 0x03
      {"O__3: AH_OUT3",      {"OFF", "ON"}},         //  4 0x04
      {"O__4: AH_OUT4",      {"OFF", "ON"}},         //  5 0x05
      {"O__5: AH_OUT5",      {"OFF", "ON"}},         //  6 0x06
      {"O__6: AH_OUT6",      {"OFF", "ON"}},         //  7 0x07
      {"O__7: AH_OUT7",      {"OFF", "ON"}},         //  8 0x08
      {"O__8: AH_OUT8",      {"OFF", "ON"}},         //  9 0x09
      {"O__9: AH_OUT9",      {"OFF", "ON"}},         // 10 0x0A
      {"O_10: AH_OUT10",     {"OFF", "ON"}},         // 11 0x0B
      {"O_11: AH_OUT11",     {"OFF", "ON"}},         // 12 0x0C
      {"O_12: AO_CCEB",      {"OFF", "ON"}},         // 13 0x0D
      {NULL,                 {NULL, NULL}},          // 14 0x0E
      {NULL,                 {NULL, NULL}},          // 15 0x0F
      {NULL,                 {NULL, NULL}},          // 16 0x10
      {NULL,                 {NULL, NULL}},          // 17 0x11
      {NULL,                 {NULL, NULL}},          // 18 0x12
      {NULL,                 {NULL, NULL}},          // 19 0x13
      {NULL,                 {NULL, NULL}},          // 20 0x14
      {NULL,                 {NULL, NULL}},          // 21 0x15
      {NULL,                 {NULL, NULL}},          // 22 0x16
      {NULL,                 {NULL, NULL}},          // 23 0x17
      {NULL,                 {NULL, NULL}},          // 24 0x18
      {NULL,                 {NULL, NULL}},          // 25 0x19
      {NULL,                 {NULL, NULL}},          // 26 0x1A
      {"Digital I/F Test",   {"0",   "1"}},          // 27 0x1B
      {"All Outlets ON/OFF", {"OFF", "ON"}},         // 28 0x1C
      {NULL,                 {NULL, NULL}},          // 29 0x1D
      {NULL,                 {NULL, NULL}},          // 30 0x1E
      {NULL,                 {NULL, NULL}}},         // 31 0x1F
// Analog Channels
     {{"Temperature",           -40.0,  80.0},       //  0 0x00
      {"O_12 Current: AO_CCEB",   0.0,  15.0},       //  1 0x01
      {"Analog Ref. 2.5V",        0.0,   4.81765},   //  2 0x02
      {NULL,                      0.0,   0.0},       //  3 0x03
      {NULL,                      0.0,   0.0},       //  4 0x04
      {NULL,                      0.0,   0.0},       //  5 0x05
      {NULL,                      0.0,   0.0},       //  6 0x06
      {NULL,                      0.0,   0.0},       //  7 0x07
      {NULL,                      0.0,   0.0},       //  8 0x08
      {NULL,                      0.0,   0.0},       //  9 0x09
      {NULL,                      0.0,   0.0},       // 10 0x0A
      {NULL,                      0.0,   0.0},       // 11 0x0B
      {NULL,                      0.0,   0.0},       // 12 0x0C
      {NULL,                      0.0,   0.0},       // 13 0x0D
      {NULL,                      0.0,   0.0},       // 14 0x0E
      {NULL,                      0.0,   0.0},       // 15 0x0F
      {NULL,                      0.0,   0.0},       // 16 0x10
      {NULL,                      0.0,   0.0},       // 17 0x11
      {NULL,                      0.0,   0.0},       // 18 0x12
      {NULL,                      0.0,   0.0},       // 19 0x13
      {NULL,                      0.0,   0.0},       // 20 0x14
      {NULL,                      0.0,   0.0},       // 21 0x15
      {NULL,                      0.0,   0.0},       // 22 0x16
      {NULL,                      0.0,   0.0},       // 23 0x17
      {NULL,                      0.0,   0.0},       // 24 0x18
      {NULL,                      0.0,   0.0},       // 25 0x19
      {NULL,                      0.0,   0.0},       // 26 0x1A
      {NULL,                      0.0,   0.0},       // 27 0x1B
      {NULL,                      0.0,   0.0},       // 28 0x1C
      {NULL,                      0.0,   0.0},       // 29 0x1D
      {NULL,                      0.0,   0.0},       // 30 0x1E
      {NULL,                      0.0,   0.0}},      // 31 0x1F
  },

//
//~--  PB2 #1 Board (120V -> 28V Converter) ---
//

//                            slot   sta   ovt   tmp   ref   t_w   t_r  --- adc ----
  {"PB2-1\n120V->28V",        0x01, 0x00, 0x01, 0x01, 0x06, 0x06, 0x06, {0x06, 0x16},

// Digital Channels
     {{"Board Status",     {"OK",  "NOK"}},          //  0 0x00
      {"OVT Alarm",        {"OVT", "OK"}},           //  1 0x01
      {NULL,               {NULL,   NULL}},          //  2 0x02
      {NULL,               {NULL,   NULL}},          //  3 0x03
      {"Input OVC Alarm",  {"OVC", "OK"}},           //  4 0x04
      {"DCDC ON/OFF",      {"OFF", "ON"}},           //  5 0x05
      {"Digital I/F Test", {"0",   "1"}},            //  6 0x06
      {NULL,               {NULL,   NULL}},          //  7 0x07
      {NULL,               {NULL,   NULL}},          //  8 0x08
      {NULL,               {NULL,   NULL}},          //  9 0x09
      {NULL,               {NULL,   NULL}},          // 10 0x0A
      {NULL,               {NULL,   NULL}},          // 11 0x0B
      {NULL,               {NULL,   NULL}},          // 12 0x0C
      {NULL,               {NULL,   NULL}},          // 13 0x0D
      {NULL,               {NULL,   NULL}},          // 14 0x0E
      {NULL,               {NULL,   NULL}},          // 15 0x0F
      {NULL,               {NULL,   NULL}},          // 16 0x10
      {NULL,               {NULL,   NULL}},          // 17 0x11
      {NULL,               {NULL,   NULL}},          // 18 0x12
      {NULL,               {NULL,   NULL}},          // 19 0x13
      {NULL,               {NULL,   NULL}},          // 20 0x14
      {"27V UNV",          {"UNV", "OK"}},           // 21 0x15
      {"29V Trip",         {"OK",  "TRIP"}},         // 22 0x16
      {"DCDC OFF/ON CMD",  {"ON",  "OFF"}},          // 23 0x17
      {NULL,               {NULL,   NULL}},          // 24 0x18
      {NULL,               {NULL,   NULL}},          // 25 0x19
      {NULL,               {NULL,   NULL}},          // 26 0x1A
      {NULL,               {NULL,   NULL}},          // 27 0x1B
      {NULL,               {NULL,   NULL}},          // 28 0x1C
      {NULL,               {NULL,   NULL}},          // 29 0x1D
      {NULL,               {NULL,   NULL}},          // 30 0x1E
      {NULL,               {NULL,   NULL}}},         // 31 0x1F
// Commands
     {{NULL,               {NULL, NULL}},            //  0 0x00
      {NULL,               {NULL, NULL}},            //  1 0x01
      {NULL,               {NULL, NULL}},            //  2 0x02
      {NULL,               {NULL, NULL}},            //  3 0x03
      {NULL,               {NULL, NULL}},            //  4 0x04
      {NULL,               {NULL, NULL}},            //  5 0x05
      {"Digital I/F Test", {"0",   "1"}},            //  6 0x06
      {NULL,               {NULL, NULL}},            //  7 0x07
      {NULL,               {NULL, NULL}},            //  8 0x08
      {NULL,               {NULL, NULL}},            //  9 0x09
      {NULL,               {NULL, NULL}},            // 10 0x0A
      {NULL,               {NULL, NULL}},            // 11 0x0B
      {NULL,               {NULL, NULL}},            // 12 0x0C
      {NULL,               {NULL, NULL}},            // 13 0x0D
      {NULL,               {NULL, NULL}},            // 14 0x0E
      {NULL,               {NULL, NULL}},            // 15 0x0F
      {NULL,               {NULL, NULL}},            // 16 0x10
      {NULL,               {NULL, NULL}},            // 17 0x11
      {NULL,               {NULL, NULL}},            // 18 0x12
      {NULL,               {NULL, NULL}},            // 19 0x13
      {NULL,               {NULL, NULL}},            // 20 0x14
      {NULL,               {NULL, NULL}},            // 21 0x15
      {NULL,               {NULL, NULL}},            // 22 0x16
      {"DCDC OFF/ON",      {"ON",  "OFF"}},          // 23 0x17
      {NULL,               {NULL, NULL}},            // 24 0x18
      {NULL,               {NULL, NULL}},            // 25 0x19
      {NULL,               {NULL, NULL}},            // 26 0x1A
      {NULL,               {NULL, NULL}},            // 27 0x1B
      {NULL,               {NULL, NULL}},            // 28 0x1C
      {NULL,               {NULL, NULL}},            // 29 0x1D
      {NULL,               {NULL, NULL}},            // 30 0x1E
      {NULL,               {NULL, NULL}}},           // 31 0x1F
// Analog Channels
     {{NULL,                  0.0,    0.0},          //  0 0x00
      {"Temperature",       -40.0,   90.0},          //  1 0x01
      {NULL,                  0.0,    0.0},          //  2 0x02
      {NULL,                  0.0,    0.0},          //  3 0x03
      {"Output Voltage",      0.0,   36.76},         //  4 0x04
      {NULL,                  0.0,    0.0},          //  5 0x05
      {"Analog Ref. 2.0V",    0.0,    4.81765},      //  6 0x06
      {NULL,                  0.0,    0.0},          //  7 0x07
      {NULL,                  0.0,    0.0},          //  8 0x08
      {NULL,                  0.0,    0.0},          //  9 0x09
      {NULL,                  0.0,    0.0},          // 10 0x0A
      {NULL,                  0.0,    0.0},          // 11 0x0B
      {NULL,                  0.0,    0.0},          // 12 0x0C
      {NULL,                  0.0,    0.0},          // 13 0x0D
      {NULL,                  0.0,    0.0},          // 14 0x0E
      {NULL,                  0.0,    0.0},          // 15 0x0F
      {NULL,                  0.0,    0.0},          // 16 0x10
      {NULL,                  0.0,    0.0},          // 17 0x11
      {NULL,                  0.0,    0.0},          // 18 0x12
      {NULL,                  0.0,    0.0},          // 19 0x13
      {NULL,                  0.0,    0.0},          // 20 0x14
      {NULL,                  0.0,    0.0},          // 21 0x15
      {NULL,                  0.0,    0.0},          // 22 0x16
      {NULL,                  0.0,    0.0},          // 23 0x17
      {NULL,                  0.0,    0.0},          // 24 0x18
      {NULL,                  0.0,    0.0},          // 25 0x19
      {NULL,                  0.0,    0.0},          // 26 0x1A
      {NULL,                  0.0,    0.0},          // 27 0x1B
      {NULL,                  0.0,    0.0},          // 28 0x1C
      {NULL,                  0.0,    0.0},          // 29 0x1D
      {NULL,                  0.0,    0.0},          // 30 0x1E
      {NULL,                  0.0,    0.0}},         // 31 0x1F
  },

//
//~--  ESEM3-A #1 Board (28V Distributor) ---
//

//                            slot   sta   ovt   tmp   ref   t_w   t_r  --- adc ----
  {"ESEM3-A-1\n28V Distrib.", 0x02, 0x00, 0x01, 0x08, 0x09, 0x0A, 0x12, {0x05, 0x15},

// Digital Channels
     {{"Board Status",       {"OK",  "NOK"}},        //  0 0x00
      {"OVT Alarm",          {"OK",  "OVT"}},        //  1 0x01
      {"O_1 CMD: TPD3",      {"OFF", "ON"}},         //  2 0x02
      {"O_2 CMD: TPD7",      {"OFF", "ON"}},         //  3 0x03
      {"O_3 CMD: JPD_N",     {"OFF", "ON"}},         //  4 0x04
      {"O_4 CMD: SPD1_N",    {"OFF", "ON"}},         //  5 0x05
      {"O_5 CMD: UGPD_N",    {"OFF", "ON"}},         //  6 0x06
      {"O_6 CMD: SPD0_R",    {"OFF", "ON"}},         //  7 0x07
      {"O_7 CMD: TPD1",      {"OFF", "ON"}},         //  8 0x08
      {"O_8 CMD: TTPD_R",    {"OFF", "ON"}},         //  9 0x09
      {"O_1 Status: TPD3",   {"OFF", "ON"}},         // 10 0x0A
      {"O_2 Status: TPD7",   {"OFF", "ON"}},         // 11 0x0B
      {"O_3 Status: JPD_N",  {"OFF", "ON"}},         // 12 0x0C
      {"O_4 Status: SPD1_N", {"OFF", "ON"}},         // 13 0x0D
      {"O_5 Status: UGPD_N", {"OFF", "ON"}},         // 14 0x0E
      {"O_6 Status: SPD0_R", {"OFF", "ON"}},         // 15 0x0F
      {"O_7 Status: TPD1",   {"OFF", "ON"}},         // 16 0x10
      {"O_8 Status: TTPD_R", {"OFF", "ON"}},         // 17 0x11
      {"Digital I/F Test",   {"0",   "1"}},          // 18 0x12
      {NULL              ,   {NULL, NULL}},          // 19 0x13
      {NULL              ,   {NULL, NULL}},          // 20 0x14
      {NULL              ,   {NULL, NULL}},          // 21 0x15
      {NULL              ,   {NULL, NULL}},          // 22 0x16
      {NULL              ,   {NULL, NULL}},          // 23 0x17
      {NULL              ,   {NULL, NULL}},          // 24 0x18
      {NULL              ,   {NULL, NULL}},          // 25 0x19
      {NULL              ,   {NULL, NULL}},          // 26 0x1A
      {NULL              ,   {NULL, NULL}},          // 27 0x1B
      {NULL              ,   {NULL, NULL}},          // 28 0x1C
      {NULL              ,   {NULL, NULL}},          // 29 0x1D
      {NULL              ,   {NULL, NULL}},          // 30 0x1E
      {NULL              ,   {NULL, NULL}}},         // 31 0x1F
// Commands
     {{NULL,                 {NULL, NULL}},          //  0 0x00
      {NULL,                 {NULL, NULL}},          //  1 0x01
      {"O_1: TPD3",          {"OFF", "ON"}},         //  2 0x02
      {"O_2: TPD7",          {"OFF", "ON"}},         //  3 0x03
      {"O_3: JPD_N",         {"OFF", "ON"}},         //  4 0x04
      {"O_4: SPD1_N",        {"OFF", "ON"}},         //  5 0x05
      {"O_5: UGPD_N",        {"OFF", "ON"}},         //  6 0x06
      {"O_6: SPD0_R",        {"OFF", "ON"}},         //  7 0x07
      {"O_7: TPD1",          {"OFF", "ON"}},         //  8 0x08
      {"O_8: TTPD_R",        {"OFF", "ON"}},         //  9 0x09
      {"Digital I/F Test",   {"0",   "1"}},          // 10 0x0A
      {NULL,                 {NULL, NULL}},          // 11 0x0B
      {NULL,                 {NULL, NULL}},          // 12 0x0C
      {NULL,                 {NULL, NULL}},          // 13 0x0D
      {NULL,                 {NULL, NULL}},          // 14 0x0E
      {NULL,                 {NULL, NULL}},          // 15 0x0F
      {NULL,                 {NULL, NULL}},          // 16 0x10
      {NULL,                 {NULL, NULL}},          // 17 0x11
      {NULL,                 {NULL, NULL}},          // 18 0x12
      {"All Outlets ON/OFF", {"OFF", "ON"}},         // 19 0x13
      {NULL,                 {NULL, NULL}},          // 20 0x14
      {NULL,                 {NULL, NULL}},          // 21 0x15
      {NULL,                 {NULL, NULL}},          // 22 0x16
      {NULL,                 {NULL, NULL}},          // 23 0x17
      {NULL,                 {NULL, NULL}},          // 24 0x18
      {NULL,                 {NULL, NULL}},          // 25 0x19
      {NULL,                 {NULL, NULL}},          // 26 0x1A
      {NULL,                 {NULL, NULL}},          // 27 0x1B
      {NULL,                 {NULL, NULL}},          // 28 0x1C
      {NULL,                 {NULL, NULL}},          // 29 0x1D
      {NULL,                 {NULL, NULL}},          // 30 0x1E
      {NULL,                 {NULL, NULL}}},         // 31 0x1F
// Analog Channels
     {{"O_1 Current: TPD3",     0.0,   7.5},         //  0 0x00
      {"O_2 Current: TPD7",     0.0,   7.5},         //  1 0x01
//    {"O_3 Current: JPD_N",    0.0,   7.5},         //  2 0x02
      {"O_3 Current: JPD_N",    0.0,   0.0},         //  2 0x02     ****** for TESTS ONLY ****
      {"O_4 Current: SPD1_N",   0.0,   7.5},         //  3 0x03
      {"O_5 Current: UGPD_N",   0.0,   7.5},         //  4 0x04
      {"O_6 Current: SPD0_R",   0.0,   7.5},         //  5 0x05
      {"O_7 Current: TPD1",     0.0,   7.5},         //  6 0x06
      {"O_8 Current: TTPD_R",   0.0,  15.0},         //  7 0x07
      {"Temperature",         -40.0,  80.0},         //  8 0x08
      {"Analog Ref. 3.0V",      0.0,   4.81765},     //  9 0x09
      {NULL              ,      0.0,   0.0},         // 10 0x0A
      {NULL              ,      0.0,   0.0},         // 11 0x0B
      {NULL              ,      0.0,   0.0},         // 12 0x0C
      {NULL              ,      0.0,   0.0},         // 13 0x0D
      {NULL              ,      0.0,   0.0},         // 14 0x0E
      {NULL              ,      0.0,   0.0},         // 15 0x0F
      {NULL              ,      0.0,   0.0},         // 16 0x10
      {NULL              ,      0.0,   0.0},         // 17 0x11
      {NULL              ,      0.0,   0.0},         // 18 0x12
      {NULL              ,      0.0,   0.0},         // 19 0x13
      {NULL              ,      0.0,   0.0},         // 20 0x14
      {NULL              ,      0.0,   0.0},         // 21 0x15
      {NULL              ,      0.0,   0.0},         // 22 0x16
      {NULL              ,      0.0,   0.0},         // 23 0x17
      {NULL              ,      0.0,   0.0},         // 24 0x18
      {NULL              ,      0.0,   0.0},         // 25 0x19
      {NULL              ,      0.0,   0.0},         // 26 0x1A
      {NULL              ,      0.0,   0.0},         // 27 0x1B
      {NULL              ,      0.0,   0.0},         // 28 0x1C
      {NULL              ,      0.0,   0.0},         // 29 0x1D
      {NULL              ,      0.0,   0.0},         // 30 0x1E
      {NULL              ,      0.0,   0.0}},        // 31 0x1F
  },

//
//~--  PB2 #2 Board (120V -> 28V Converter) ---
//

//                            slot   sta   ovt   tmp   ref   t_w   t_r  --- adc ----
  {"PB2-2\n120V->28V",        0x03, 0x00, 0x01, 0x01, 0x06, 0x06, 0x06, {0x04, 0x14},

// Digital Channels
     {{"Board Status",     {"OK",  "NOK"}},          //  0 0x00
      {"OVT Alarm",        {"OVT", "OK"}},           //  1 0x01
      {NULL,               {NULL,   NULL}},          //  2 0x02
      {NULL,               {NULL,   NULL}},          //  3 0x03
      {"Input OVC Alarm",  {"OVC", "OK"}},           //  4 0x04
      {"DCDC ON/OFF",      {"OFF", "ON"}},           //  5 0x05
      {"Digital I/F Test", {"0",   "1"}},            //  6 0x06
      {NULL,               {NULL,   NULL}},          //  7 0x07
      {NULL,               {NULL,   NULL}},          //  8 0x08
      {NULL,               {NULL,   NULL}},          //  9 0x09
      {NULL,               {NULL,   NULL}},          // 10 0x0A
      {NULL,               {NULL,   NULL}},          // 11 0x0B
      {NULL,               {NULL,   NULL}},          // 12 0x0C
      {NULL,               {NULL,   NULL}},          // 13 0x0D
      {NULL,               {NULL,   NULL}},          // 14 0x0E
      {NULL,               {NULL,   NULL}},          // 15 0x0F
      {NULL,               {NULL,   NULL}},          // 16 0x10
      {NULL,               {NULL,   NULL}},          // 17 0x11
      {NULL,               {NULL,   NULL}},          // 18 0x12
      {NULL,               {NULL,   NULL}},          // 19 0x13
      {NULL,               {NULL,   NULL}},          // 20 0x14
      {"27V UNV",          {"UNV", "OK"}},           // 21 0x15
      {"29V Trip",         {"OK",  "TRIP"}},         // 22 0x16
      {"DCDC OFF/ON CMD",  {"ON",  "OFF"}},          // 23 0x17
      {NULL,               {NULL,   NULL}},          // 24 0x18
      {NULL,               {NULL,   NULL}},          // 25 0x19
      {NULL,               {NULL,   NULL}},          // 26 0x1A
      {NULL,               {NULL,   NULL}},          // 27 0x1B
      {NULL,               {NULL,   NULL}},          // 28 0x1C
      {NULL,               {NULL,   NULL}},          // 29 0x1D
      {NULL,               {NULL,   NULL}},          // 30 0x1E
      {NULL,               {NULL,   NULL}}},         // 31 0x1F
// Commands
     {{NULL,               {NULL, NULL}},            //  0 0x00
      {NULL,               {NULL, NULL}},            //  1 0x01
      {NULL,               {NULL, NULL}},            //  2 0x02
      {NULL,               {NULL, NULL}},            //  3 0x03
      {NULL,               {NULL, NULL}},            //  4 0x04
      {NULL,               {NULL, NULL}},            //  5 0x05
      {"Digital I/F Test", {"0",   "1"}},            //  6 0x06
      {NULL,               {NULL, NULL}},            //  7 0x07
      {NULL,               {NULL, NULL}},            //  8 0x08
      {NULL,               {NULL, NULL}},            //  9 0x09
      {NULL,               {NULL, NULL}},            // 10 0x0A
      {NULL,               {NULL, NULL}},            // 11 0x0B
      {NULL,               {NULL, NULL}},            // 12 0x0C
      {NULL,               {NULL, NULL}},            // 13 0x0D
      {NULL,               {NULL, NULL}},            // 14 0x0E
      {NULL,               {NULL, NULL}},            // 15 0x0F
      {NULL,               {NULL, NULL}},            // 16 0x10
      {NULL,               {NULL, NULL}},            // 17 0x11
      {NULL,               {NULL, NULL}},            // 18 0x12
      {NULL,               {NULL, NULL}},            // 19 0x13
      {NULL,               {NULL, NULL}},            // 20 0x14
      {NULL,               {NULL, NULL}},            // 21 0x15
      {NULL,               {NULL, NULL}},            // 22 0x16
      {"DCDC OFF/ON",      {"ON",  "OFF"}},          // 23 0x17
      {NULL,               {NULL, NULL}},            // 24 0x18
      {NULL,               {NULL, NULL}},            // 25 0x19
      {NULL,               {NULL, NULL}},            // 26 0x1A
      {NULL,               {NULL, NULL}},            // 27 0x1B
      {NULL,               {NULL, NULL}},            // 28 0x1C
      {NULL,               {NULL, NULL}},            // 29 0x1D
      {NULL,               {NULL, NULL}},            // 30 0x1E
      {NULL,               {NULL, NULL}}},           // 31 0x1F
// Analog Channels
     {{NULL,                  0.0,    0.0},          //  0 0x00
      {"Temperature",       -40.0,   90.0},          //  1 0x01
      {NULL,                  0.0,    0.0},          //  2 0x02
      {NULL,                  0.0,    0.0},          //  3 0x03
      {"Output Voltage",      0.0,   36.76},         //  4 0x04
      {NULL,                  0.0,    0.0},          //  5 0x05
      {"Analog Ref. 2.0V",    0.0,    4.81765},      //  6 0x06
      {NULL,                  0.0,    0.0},          //  7 0x07
      {NULL,                  0.0,    0.0},          //  8 0x08
      {NULL,                  0.0,    0.0},          //  9 0x09
      {NULL,                  0.0,    0.0},          // 10 0x0A
      {NULL,                  0.0,    0.0},          // 11 0x0B
      {NULL,                  0.0,    0.0},          // 12 0x0C
      {NULL,                  0.0,    0.0},          // 13 0x0D
      {NULL,                  0.0,    0.0},          // 14 0x0E
      {NULL,                  0.0,    0.0},          // 15 0x0F
      {NULL,                  0.0,    0.0},          // 16 0x10
      {NULL,                  0.0,    0.0},          // 17 0x11
      {NULL,                  0.0,    0.0},          // 18 0x12
      {NULL,                  0.0,    0.0},          // 19 0x13
      {NULL,                  0.0,    0.0},          // 20 0x14
      {NULL,                  0.0,    0.0},          // 21 0x15
      {NULL,                  0.0,    0.0},          // 22 0x16
      {NULL,                  0.0,    0.0},          // 23 0x17
      {NULL,                  0.0,    0.0},          // 24 0x18
      {NULL,                  0.0,    0.0},          // 25 0x19
      {NULL,                  0.0,    0.0},          // 26 0x1A
      {NULL,                  0.0,    0.0},          // 27 0x1B
      {NULL,                  0.0,    0.0},          // 28 0x1C
      {NULL,                  0.0,    0.0},          // 29 0x1D
      {NULL,                  0.0,    0.0},          // 30 0x1E
      {NULL,                  0.0,    0.0}},         // 31 0x1F
  },
  
//
//~--  ESEM3-A #2 Board (28V Distributor) ---
//

//                            slot   sta   ovt   tmp   ref   t_w   t_r  --- adc ----
  {"ESEM3-A-2\n28V Distrib.", 0x04, 0x00, 0x01, 0x08, 0x09, 0x0A, 0x12, {0x03, 0x13},

// Digital Channels
     {{"Board Status",       {"OK",  "NOK"}},        //  0 0x00
      {"OVT Alarm",          {"OK",  "OVT"}},        //  1 0x01
      {"O_1 CMD: ER1_N",     {"OFF", "ON"}},         //  2 0x02
      {"O_2 CMD: TPD2",      {"OFF", "ON"}},         //  3 0x03
      {"O_3 CMD: TPD6",      {"OFF", "ON"}},         //  4 0x04
      {"O_4 CMD: SPD3_N",    {"OFF", "ON"}},         //  5 0x05
      {"O_5 CMD: MPD_N",     {"OFF", "ON"}},         //  6 0x06
      {"O_6 CMD: UPD1",      {"OFF", "ON"}},         //  7 0x07
      {"O_7 CMD: SPD2_N",    {"OFF", "ON"}},         //  8 0x08
      {"O_8 CMD: CAB_R",     {"OFF", "ON"}},         //  9 0x09
      {"O_1 Status: ER1_N",  {"OFF", "ON"}},         // 10 0x0A
      {"O_2 Status: TPD2",   {"OFF", "ON"}},         // 11 0x0B
      {"O_3 Status: TPD6",   {"OFF", "ON"}},         // 12 0x0C
      {"O_4 Status: SPD3_N", {"OFF", "ON"}},         // 13 0x0D
      {"O_5 Status: MPD_N",  {"OFF", "ON"}},         // 14 0x0E
      {"O_6 Status: UPD1",   {"OFF", "ON"}},         // 15 0x0F
      {"O_7 Status: SPD2_N", {"OFF", "ON"}},         // 16 0x10
      {"O_8 Status: CAB_R",  {"OFF", "ON"}},         // 17 0x11
      {"Digital I/F Test",   {"0",   "1"}},          // 18 0x12
      {NULL              ,   {NULL, NULL}},          // 19 0x13
      {NULL              ,   {NULL, NULL}},          // 20 0x14
      {NULL              ,   {NULL, NULL}},          // 21 0x15
      {NULL              ,   {NULL, NULL}},          // 22 0x16
      {NULL              ,   {NULL, NULL}},          // 23 0x17
      {NULL              ,   {NULL, NULL}},          // 24 0x18
      {NULL              ,   {NULL, NULL}},          // 25 0x19
      {NULL              ,   {NULL, NULL}},          // 26 0x1A
      {NULL              ,   {NULL, NULL}},          // 27 0x1B
      {NULL              ,   {NULL, NULL}},          // 28 0x1C
      {NULL              ,   {NULL, NULL}},          // 29 0x1D
      {NULL              ,   {NULL, NULL}},          // 30 0x1E
      {NULL              ,   {NULL, NULL}}},         // 31 0x1F
// Commands
     {{NULL,                 {NULL, NULL}},          //  0 0x00
      {NULL,                 {NULL, NULL}},          //  1 0x01
      {"O_1: ER1_N",         {"OFF", "ON"}},         //  2 0x02
      {"O_2: TPD2",          {"OFF", "ON"}},         //  3 0x03
      {"O_3: TPD6",          {"OFF", "ON"}},         //  4 0x04
      {"O_4: SPD3_N",        {"OFF", "ON"}},         //  5 0x05
      {"O_5: MPD_N",         {"OFF", "ON"}},         //  6 0x06
      {"O_6: UPD1",          {"OFF", "ON"}},         //  7 0x07
      {"O_7: SPD2_N",        {"OFF", "ON"}},         //  8 0x08
      {"O_8: CAB_R",         {"OFF", "ON"}},         //  9 0x09
      {"Digital I/F Test",   {"0",   "1"}},          // 10 0x0A
      {NULL,                 {NULL, NULL}},          // 11 0x0B
      {NULL,                 {NULL, NULL}},          // 12 0x0C
      {NULL,                 {NULL, NULL}},          // 13 0x0D
      {NULL,                 {NULL, NULL}},          // 14 0x0E
      {NULL,                 {NULL, NULL}},          // 15 0x0F
      {NULL,                 {NULL, NULL}},          // 16 0x10
      {NULL,                 {NULL, NULL}},          // 17 0x11
      {NULL,                 {NULL, NULL}},          // 18 0x12
      {"All Outlets ON/OFF", {"OFF", "ON"}},         // 19 0x13
      {NULL,                 {NULL, NULL}},          // 20 0x14
      {NULL,                 {NULL, NULL}},          // 21 0x15
      {NULL,                 {NULL, NULL}},          // 22 0x16
      {NULL,                 {NULL, NULL}},          // 23 0x17
      {NULL,                 {NULL, NULL}},          // 24 0x18
      {NULL,                 {NULL, NULL}},          // 25 0x19
      {NULL,                 {NULL, NULL}},          // 26 0x1A
      {NULL,                 {NULL, NULL}},          // 27 0x1B
      {NULL,                 {NULL, NULL}},          // 28 0x1C
      {NULL,                 {NULL, NULL}},          // 29 0x1D
      {NULL,                 {NULL, NULL}},          // 30 0x1E
      {NULL,                 {NULL, NULL}}},         // 31 0x1F
// Analog Channels
     {{"O_1 Current: ER1_N",    0.0,   7.5},         //  0 0x00
      {"O_2 Current: TPD2",     0.0,   7.5},         //  1 0x01
      {"O_3 Current: TPD6",     0.0,   7.5},         //  2 0x02
      {"O_4 Current: SPD3_N",   0.0,   7.5},         //  3 0x03
      {"O_5 Current: MPD_N",    0.0,   7.5},         //  4 0x04
      {"O_6 Current: UPD1",     0.0,   7.5},         //  5 0x05
      {"O_7 Current: SPD2_N",   0.0,   7.5},         //  6 0x06
      {"O_8 Current: CAB_R",    0.0,  15.0},         //  7 0x07
      {"Temperature",         -40.0,  80.0},         //  8 0x08
      {"Analog Ref. 3.0V",      0.0,   4.81765},     //  9 0x09
      {NULL              ,      0.0,   0.0},         // 10 0x0A
      {NULL              ,      0.0,   0.0},         // 11 0x0B
      {NULL              ,      0.0,   0.0},         // 12 0x0C
      {NULL              ,      0.0,   0.0},         // 13 0x0D
      {NULL              ,      0.0,   0.0},         // 14 0x0E
      {NULL              ,      0.0,   0.0},         // 15 0x0F
      {NULL              ,      0.0,   0.0},         // 16 0x10
      {NULL              ,      0.0,   0.0},         // 17 0x11
      {NULL              ,      0.0,   0.0},         // 18 0x12
      {NULL              ,      0.0,   0.0},         // 19 0x13
      {NULL              ,      0.0,   0.0},         // 20 0x14
      {NULL              ,      0.0,   0.0},         // 21 0x15
      {NULL              ,      0.0,   0.0},         // 22 0x16
      {NULL              ,      0.0,   0.0},         // 23 0x17
      {NULL              ,      0.0,   0.0},         // 24 0x18
      {NULL              ,      0.0,   0.0},         // 25 0x19
      {NULL              ,      0.0,   0.0},         // 26 0x1A
      {NULL              ,      0.0,   0.0},         // 27 0x1B
      {NULL              ,      0.0,   0.0},         // 28 0x1C
      {NULL              ,      0.0,   0.0},         // 29 0x1D
      {NULL              ,      0.0,   0.0},         // 30 0x1E
      {NULL              ,      0.0,   0.0}},        // 31 0x1F
  },

//
//~--  PB2 #3 Board (120V -> 28V Converter) ---
//

//                            slot   sta   ovt   tmp   ref   t_w   t_r  --- adc ----
  {"PB2-3\n120V->28V",        0x05, 0x00, 0x01, 0x01, 0x06, 0x06, 0x06, {0x02, 0x12},

// Digital Channels
     {{"Board Status",     {"OK",  "NOK"}},          //  0 0x00
      {"OVT Alarm",        {"OVT", "OK"}},           //  1 0x01
      {NULL,               {NULL,   NULL}},          //  2 0x02
      {NULL,               {NULL,   NULL}},          //  3 0x03
      {"Input OVC Alarm",  {"OVC", "OK"}},           //  4 0x04
      {"DCDC ON/OFF",      {"OFF", "ON"}},           //  5 0x05
      {"Digital I/F Test", {"0",   "1"}},            //  6 0x06
      {NULL,               {NULL,   NULL}},          //  7 0x07
      {NULL,               {NULL,   NULL}},          //  8 0x08
      {NULL,               {NULL,   NULL}},          //  9 0x09
      {NULL,               {NULL,   NULL}},          // 10 0x0A
      {NULL,               {NULL,   NULL}},          // 11 0x0B
      {NULL,               {NULL,   NULL}},          // 12 0x0C
      {NULL,               {NULL,   NULL}},          // 13 0x0D
      {NULL,               {NULL,   NULL}},          // 14 0x0E
      {NULL,               {NULL,   NULL}},          // 15 0x0F
      {NULL,               {NULL,   NULL}},          // 16 0x10
      {NULL,               {NULL,   NULL}},          // 17 0x11
      {NULL,               {NULL,   NULL}},          // 18 0x12
      {NULL,               {NULL,   NULL}},          // 19 0x13
      {NULL,               {NULL,   NULL}},          // 20 0x14
      {"27V UNV",          {"UNV", "OK"}},           // 21 0x15
      {"29V Trip",         {"OK",  "TRIP"}},         // 22 0x16
      {"DCDC OFF/ON CMD",  {"ON",  "OFF"}},          // 23 0x17
      {NULL,               {NULL,   NULL}},          // 24 0x18
      {NULL,               {NULL,   NULL}},          // 25 0x19
      {NULL,               {NULL,   NULL}},          // 26 0x1A
      {NULL,               {NULL,   NULL}},          // 27 0x1B
      {NULL,               {NULL,   NULL}},          // 28 0x1C
      {NULL,               {NULL,   NULL}},          // 29 0x1D
      {NULL,               {NULL,   NULL}},          // 30 0x1E
      {NULL,               {NULL,   NULL}}},         // 31 0x1F
// Commands
     {{NULL,               {NULL, NULL}},            //  0 0x00
      {NULL,               {NULL, NULL}},            //  1 0x01
      {NULL,               {NULL, NULL}},            //  2 0x02
      {NULL,               {NULL, NULL}},            //  3 0x03
      {NULL,               {NULL, NULL}},            //  4 0x04
      {NULL,               {NULL, NULL}},            //  5 0x05
      {"Digital I/F Test", {"0",   "1"}},            //  6 0x06
      {NULL,               {NULL, NULL}},            //  7 0x07
      {NULL,               {NULL, NULL}},            //  8 0x08
      {NULL,               {NULL, NULL}},            //  9 0x09
      {NULL,               {NULL, NULL}},            // 10 0x0A
      {NULL,               {NULL, NULL}},            // 11 0x0B
      {NULL,               {NULL, NULL}},            // 12 0x0C
      {NULL,               {NULL, NULL}},            // 13 0x0D
      {NULL,               {NULL, NULL}},            // 14 0x0E
      {NULL,               {NULL, NULL}},            // 15 0x0F
      {NULL,               {NULL, NULL}},            // 16 0x10
      {NULL,               {NULL, NULL}},            // 17 0x11
      {NULL,               {NULL, NULL}},            // 18 0x12
      {NULL,               {NULL, NULL}},            // 19 0x13
      {NULL,               {NULL, NULL}},            // 20 0x14
      {NULL,               {NULL, NULL}},            // 21 0x15
      {NULL,               {NULL, NULL}},            // 22 0x16
      {"DCDC OFF/ON",      {"ON",  "OFF"}},          // 23 0x17
      {NULL,               {NULL, NULL}},            // 24 0x18
      {NULL,               {NULL, NULL}},            // 25 0x19
      {NULL,               {NULL, NULL}},            // 26 0x1A
      {NULL,               {NULL, NULL}},            // 27 0x1B
      {NULL,               {NULL, NULL}},            // 28 0x1C
      {NULL,               {NULL, NULL}},            // 29 0x1D
      {NULL,               {NULL, NULL}},            // 30 0x1E
      {NULL,               {NULL, NULL}}},           // 31 0x1F
// Analog Channels
     {{NULL,                  0.0,    0.0},          //  0 0x00
      {"Temperature",       -40.0,   90.0},          //  1 0x01
      {NULL,                  0.0,    0.0},          //  2 0x02
      {NULL,                  0.0,    0.0},          //  3 0x03
      {"Output Voltage",      0.0,   36.76},         //  4 0x04
      {NULL,                  0.0,    0.0},          //  5 0x05
      {"Analog Ref. 2.0V",    0.0,    4.81765},      //  6 0x06
      {NULL,                  0.0,    0.0},          //  7 0x07
      {NULL,                  0.0,    0.0},          //  8 0x08
      {NULL,                  0.0,    0.0},          //  9 0x09
      {NULL,                  0.0,    0.0},          // 10 0x0A
      {NULL,                  0.0,    0.0},          // 11 0x0B
      {NULL,                  0.0,    0.0},          // 12 0x0C
      {NULL,                  0.0,    0.0},          // 13 0x0D
      {NULL,                  0.0,    0.0},          // 14 0x0E
      {NULL,                  0.0,    0.0},          // 15 0x0F
      {NULL,                  0.0,    0.0},          // 16 0x10
      {NULL,                  0.0,    0.0},          // 17 0x11
      {NULL,                  0.0,    0.0},          // 18 0x12
      {NULL,                  0.0,    0.0},          // 19 0x13
      {NULL,                  0.0,    0.0},          // 20 0x14
      {NULL,                  0.0,    0.0},          // 21 0x15
      {NULL,                  0.0,    0.0},          // 22 0x16
      {NULL,                  0.0,    0.0},          // 23 0x17
      {NULL,                  0.0,    0.0},          // 24 0x18
      {NULL,                  0.0,    0.0},          // 25 0x19
      {NULL,                  0.0,    0.0},          // 26 0x1A
      {NULL,                  0.0,    0.0},          // 27 0x1B
      {NULL,                  0.0,    0.0},          // 28 0x1C
      {NULL,                  0.0,    0.0},          // 29 0x1D
      {NULL,                  0.0,    0.0},          // 30 0x1E
      {NULL,                  0.0,    0.0}},         // 31 0x1F
  },

//
//~--  ESEM3-A #3 Board (28V Distributor) ---
//

//                            slot   sta   ovt   tmp   ref   t_w   t_r  --- adc ----
  {"ESEM3-A-3\n28V Distrib.", 0x06, 0x00, 0x01, 0x08, 0x09, 0x0A, 0x12, {0x01, 0x11},

// Digital Channels
     {{"Board Status",       {"OK",  "NOK"}},        //  0 0x00
      {"OVT Alarm",          {"OK",  "OVT"}},        //  1 0x01
      {"O_1 CMD: TPD0",      {"OFF", "ON"}},         //  2 0x02
      {"O_2 CMD: TPD4",      {"OFF", "ON"}},         //  3 0x03
      {"O_3 CMD: ER0_N",     {"OFF", "ON"}},         //  4 0x04
      {"O_4 CMD: MPD_R",     {"OFF", "ON"}},         //  5 0x05
      {"O_5 CMD: SPD3_R",    {"OFF", "ON"}},         //  6 0x06
      {"O_6 CMD: UGPD_R",    {"OFF", "ON"}},         //  7 0x07
      {"O_7 CMD: SPD2_R",    {"OFF", "ON"}},         //  8 0x08
      {"O_8 CMD: CAB_N",     {"OFF", "ON"}},         //  9 0x09
      {"O_1 Status: TPD0",   {"OFF", "ON"}},         // 10 0x0A
      {"O_2 Status: TPD4",   {"OFF", "ON"}},         // 11 0x0B
      {"O_3 Status: ER0_N",  {"OFF", "ON"}},         // 12 0x0C
      {"O_4 Status: MPD_R",  {"OFF", "ON"}},         // 13 0x0D
      {"O_5 Status: SPD3_R", {"OFF", "ON"}},         // 14 0x0E
      {"O_6 Status: UGPD_R", {"OFF", "ON"}},         // 15 0x0F
      {"O_7 Status: SPD2_R", {"OFF", "ON"}},         // 16 0x10
      {"O_8 Status: CAB_N",  {"OFF", "ON"}},         // 17 0x11
      {"Digital I/F Test",   {"0",   "1"}},          // 18 0x12
      {NULL              ,   {NULL, NULL}},          // 19 0x13
      {NULL              ,   {NULL, NULL}},          // 20 0x14
      {NULL              ,   {NULL, NULL}},          // 21 0x15
      {NULL              ,   {NULL, NULL}},          // 22 0x16
      {NULL              ,   {NULL, NULL}},          // 23 0x17
      {NULL              ,   {NULL, NULL}},          // 24 0x18
      {NULL              ,   {NULL, NULL}},          // 25 0x19
      {NULL              ,   {NULL, NULL}},          // 26 0x1A
      {NULL              ,   {NULL, NULL}},          // 27 0x1B
      {NULL              ,   {NULL, NULL}},          // 28 0x1C
      {NULL              ,   {NULL, NULL}},          // 29 0x1D
      {NULL              ,   {NULL, NULL}},          // 30 0x1E
      {NULL              ,   {NULL, NULL}}},         // 31 0x1F
// Commands
     {{NULL,                 {NULL, NULL}},          //  0 0x00
      {NULL,                 {NULL, NULL}},          //  1 0x01
      {"O_1: TPD0",          {"OFF", "ON"}},         //  2 0x02
      {"O_2: TPD4",          {"OFF", "ON"}},         //  3 0x03
      {"O_3: ER0_N",         {"OFF", "ON"}},         //  4 0x04
      {"O_4: MPD_R",         {"OFF", "ON"}},         //  5 0x05
      {"O_5: SPD3_R",        {"OFF", "ON"}},         //  6 0x06
      {"O_6: UGPD_R",        {"OFF", "ON"}},         //  7 0x07
      {"O_7: SPD2_R",        {"OFF", "ON"}},         //  8 0x08
      {"O_8: CAB_N",         {"OFF", "ON"}},         //  9 0x09
      {"Digital I/F Test",   {"0",   "1"}},          // 10 0x0A
      {NULL,                 {NULL, NULL}},          // 11 0x0B
      {NULL,                 {NULL, NULL}},          // 12 0x0C
      {NULL,                 {NULL, NULL}},          // 13 0x0D
      {NULL,                 {NULL, NULL}},          // 14 0x0E
      {NULL,                 {NULL, NULL}},          // 15 0x0F
      {NULL,                 {NULL, NULL}},          // 16 0x10
      {NULL,                 {NULL, NULL}},          // 17 0x11
      {NULL,                 {NULL, NULL}},          // 18 0x12
      {"All Outlets ON/OFF", {"OFF", "ON"}},         // 19 0x13
      {NULL,                 {NULL, NULL}},          // 20 0x14
      {NULL,                 {NULL, NULL}},          // 21 0x15
      {NULL,                 {NULL, NULL}},          // 22 0x16
      {NULL,                 {NULL, NULL}},          // 23 0x17
      {NULL,                 {NULL, NULL}},          // 24 0x18
      {NULL,                 {NULL, NULL}},          // 25 0x19
      {NULL,                 {NULL, NULL}},          // 26 0x1A
      {NULL,                 {NULL, NULL}},          // 27 0x1B
      {NULL,                 {NULL, NULL}},          // 28 0x1C
      {NULL,                 {NULL, NULL}},          // 29 0x1D
      {NULL,                 {NULL, NULL}},          // 30 0x1E
      {NULL,                 {NULL, NULL}}},         // 31 0x1F
// Analog Channels
     {{"O_1 Current: TPD0",     0.0,   7.5},         //  0 0x00
      {"O_2 Current: TPD4",     0.0,   7.5},         //  1 0x01
      {"O_3 Current: ER0_N",    0.0,   7.5},         //  2 0x02
      {"O_4 Current: MPD_R",    0.0,   7.5},         //  3 0x03
      {"O_5 Current: SPD3_R",   0.0,   7.5},         //  4 0x04
      {"O_6 Current: UGPD_R",   0.0,   7.5},         //  5 0x05
      {"O_7 Current: SPD2_R",   0.0,   7.5},         //  6 0x06
      {"O_8 Current: CAB_N",    0.0,  15.0},         //  7 0x07
      {"Temperature",         -40.0,  80.0},         //  8 0x08
      {"Analog Ref. 3.0V",      0.0,   4.81765},     //  9 0x09
      {NULL              ,      0.0,   0.0},         // 10 0x0A
      {NULL              ,      0.0,   0.0},         // 11 0x0B
      {NULL              ,      0.0,   0.0},         // 12 0x0C
      {NULL              ,      0.0,   0.0},         // 13 0x0D
      {NULL              ,      0.0,   0.0},         // 14 0x0E
      {NULL              ,      0.0,   0.0},         // 15 0x0F
      {NULL              ,      0.0,   0.0},         // 16 0x10
      {NULL              ,      0.0,   0.0},         // 17 0x11
      {NULL              ,      0.0,   0.0},         // 18 0x12
      {NULL              ,      0.0,   0.0},         // 19 0x13
      {NULL              ,      0.0,   0.0},         // 20 0x14
      {NULL              ,      0.0,   0.0},         // 21 0x15
      {NULL              ,      0.0,   0.0},         // 22 0x16
      {NULL              ,      0.0,   0.0},         // 23 0x17
      {NULL              ,      0.0,   0.0},         // 24 0x18
      {NULL              ,      0.0,   0.0},         // 25 0x19
      {NULL              ,      0.0,   0.0},         // 26 0x1A
      {NULL              ,      0.0,   0.0},         // 27 0x1B
      {NULL              ,      0.0,   0.0},         // 28 0x1C
      {NULL              ,      0.0,   0.0},         // 29 0x1D
      {NULL              ,      0.0,   0.0},         // 30 0x1E
      {NULL              ,      0.0,   0.0}},        // 31 0x1F
  },

//
//~--  PB2 #4 Board (120V -> 28V Converter) ---
//

//                            slot   sta   ovt   tmp   ref   t_w   t_r  --- adc ----
  {"PB2-4\n120V->28V",        0x07, 0x00, 0x01, 0x01, 0x06, 0x06, 0x06, {0x00, 0x10},

// Digital Channels
     {{"Board Status",     {"OK",  "NOK"}},          //  0 0x00
      {"OVT Alarm",        {"OVT", "OK"}},           //  1 0x01
      {NULL,               {NULL,   NULL}},          //  2 0x02
      {NULL,               {NULL,   NULL}},          //  3 0x03
      {"Input OVC Alarm",  {"OVC", "OK"}},           //  4 0x04
      {"DCDC ON/OFF",      {"OFF", "ON"}},           //  5 0x05
      {"Digital I/F Test", {"0",   "1"}},            //  6 0x06
      {NULL,               {NULL,   NULL}},          //  7 0x07
      {NULL,               {NULL,   NULL}},          //  8 0x08
      {NULL,               {NULL,   NULL}},          //  9 0x09
      {NULL,               {NULL,   NULL}},          // 10 0x0A
      {NULL,               {NULL,   NULL}},          // 11 0x0B
      {NULL,               {NULL,   NULL}},          // 12 0x0C
      {NULL,               {NULL,   NULL}},          // 13 0x0D
      {NULL,               {NULL,   NULL}},          // 14 0x0E
      {NULL,               {NULL,   NULL}},          // 15 0x0F
      {NULL,               {NULL,   NULL}},          // 16 0x10
      {NULL,               {NULL,   NULL}},          // 17 0x11
      {NULL,               {NULL,   NULL}},          // 18 0x12
      {NULL,               {NULL,   NULL}},          // 19 0x13
      {NULL,               {NULL,   NULL}},          // 20 0x14
      {"27V UNV",          {"UNV", "OK"}},           // 21 0x15
      {"29V Trip",         {"OK",  "TRIP"}},         // 22 0x16
      {"DCDC OFF/ON CMD",  {"ON",  "OFF"}},          // 23 0x17
      {NULL,               {NULL,   NULL}},          // 24 0x18
      {NULL,               {NULL,   NULL}},          // 25 0x19
      {NULL,               {NULL,   NULL}},          // 26 0x1A
      {NULL,               {NULL,   NULL}},          // 27 0x1B
      {NULL,               {NULL,   NULL}},          // 28 0x1C
      {NULL,               {NULL,   NULL}},          // 29 0x1D
      {NULL,               {NULL,   NULL}},          // 30 0x1E
      {NULL,               {NULL,   NULL}}},         // 31 0x1F
// Commands
     {{NULL,               {NULL, NULL}},            //  0 0x00
      {NULL,               {NULL, NULL}},            //  1 0x01
      {NULL,               {NULL, NULL}},            //  2 0x02
      {NULL,               {NULL, NULL}},            //  3 0x03
      {NULL,               {NULL, NULL}},            //  4 0x04
      {NULL,               {NULL, NULL}},            //  5 0x05
      {"Digital I/F Test", {"0",   "1"}},            //  6 0x06
      {NULL,               {NULL, NULL}},            //  7 0x07
      {NULL,               {NULL, NULL}},            //  8 0x08
      {NULL,               {NULL, NULL}},            //  9 0x09
      {NULL,               {NULL, NULL}},            // 10 0x0A
      {NULL,               {NULL, NULL}},            // 11 0x0B
      {NULL,               {NULL, NULL}},            // 12 0x0C
      {NULL,               {NULL, NULL}},            // 13 0x0D
      {NULL,               {NULL, NULL}},            // 14 0x0E
      {NULL,               {NULL, NULL}},            // 15 0x0F
      {NULL,               {NULL, NULL}},            // 16 0x10
      {NULL,               {NULL, NULL}},            // 17 0x11
      {NULL,               {NULL, NULL}},            // 18 0x12
      {NULL,               {NULL, NULL}},            // 19 0x13
      {NULL,               {NULL, NULL}},            // 20 0x14
      {NULL,               {NULL, NULL}},            // 21 0x15
      {NULL,               {NULL, NULL}},            // 22 0x16
      {"DCDC OFF/ON",      {"ON",  "OFF"}},          // 23 0x17
      {NULL,               {NULL, NULL}},            // 24 0x18
      {NULL,               {NULL, NULL}},            // 25 0x19
      {NULL,               {NULL, NULL}},            // 26 0x1A
      {NULL,               {NULL, NULL}},            // 27 0x1B
      {NULL,               {NULL, NULL}},            // 28 0x1C
      {NULL,               {NULL, NULL}},            // 29 0x1D
      {NULL,               {NULL, NULL}},            // 30 0x1E
      {NULL,               {NULL, NULL}}},           // 31 0x1F
// Analog Channels
     {{NULL,                  0.0,    0.0},          //  0 0x00
      {"Temperature",       -40.0,   90.0},          //  1 0x01
      {NULL,                  0.0,    0.0},          //  2 0x02
      {NULL,                  0.0,    0.0},          //  3 0x03
      {"Output Voltage",      0.0,   36.76},         //  4 0x04
      {NULL,                  0.0,    0.0},          //  5 0x05
      {"Analog Ref. 2.0V",    0.0,    4.81765},      //  6 0x06
      {NULL,                  0.0,    0.0},          //  7 0x07
      {NULL,                  0.0,    0.0},          //  8 0x08
      {NULL,                  0.0,    0.0},          //  9 0x09
      {NULL,                  0.0,    0.0},          // 10 0x0A
      {NULL,                  0.0,    0.0},          // 11 0x0B
      {NULL,                  0.0,    0.0},          // 12 0x0C
      {NULL,                  0.0,    0.0},          // 13 0x0D
      {NULL,                  0.0,    0.0},          // 14 0x0E
      {NULL,                  0.0,    0.0},          // 15 0x0F
      {NULL,                  0.0,    0.0},          // 16 0x10
      {NULL,                  0.0,    0.0},          // 17 0x11
      {NULL,                  0.0,    0.0},          // 18 0x12
      {NULL,                  0.0,    0.0},          // 19 0x13
      {NULL,                  0.0,    0.0},          // 20 0x14
      {NULL,                  0.0,    0.0},          // 21 0x15
      {NULL,                  0.0,    0.0},          // 22 0x16
      {NULL,                  0.0,    0.0},          // 23 0x17
      {NULL,                  0.0,    0.0},          // 24 0x18
      {NULL,                  0.0,    0.0},          // 25 0x19
      {NULL,                  0.0,    0.0},          // 26 0x1A
      {NULL,                  0.0,    0.0},          // 27 0x1B
      {NULL,                  0.0,    0.0},          // 28 0x1C
      {NULL,                  0.0,    0.0},          // 29 0x1D
      {NULL,                  0.0,    0.0},          // 30 0x1E
      {NULL,                  0.0,    0.0}},         // 31 0x1F
  },

//
//~--  ESEM3-A #4 Board (28V Distributor) ---
//

//                            slot   sta   ovt   tmp   ref   t_w   t_r  --- adc ----
  {"ESEM3-A-4\n28V Distrib.", 0x08, 0x00, 0x01, 0x08, 0x09, 0x0A, 0x12, {0x0F, 0x1F},

// Digital Channels
     {{"Board Status",       {"OK",  "NOK"}},        //  0 0x00
      {"OVT Alarm",          {"OK",  "OVT"}},        //  1 0x01
      {"O_1 CMD: JPD_R",     {"OFF", "ON"}},         //  2 0x02
      {"O_2 CMD: ER1_R",     {"OFF", "ON"}},         //  3 0x03
      {"O_3 CMD: SPD1_R",    {"OFF", "ON"}},         //  4 0x04
      {"O_4 CMD: ER0_R",     {"OFF", "ON"}},         //  5 0x05
      {"O_5 CMD: UPD0",      {"OFF", "ON"}},         //  6 0x06
      {"O_6 CMD: SPD0_N",    {"OFF", "ON"}},         //  7 0x07
      {"O_7 CMD: TPD5",      {"OFF", "ON"}},         //  8 0x08
      {"O_8 CMD: TTPD_N",    {"OFF", "ON"}},         //  9 0x09
      {"O_1 Status: JPD_R",  {"OFF", "ON"}},         // 10 0x0A
      {"O_2 Status: ER1_R",  {"OFF", "ON"}},         // 11 0x0B
      {"O_3 Status: SPD1_R", {"OFF", "ON"}},         // 12 0x0C
      {"O_4 Status: ER0_R",  {"OFF", "ON"}},         // 13 0x0D
      {"O_5 Status: UPD0",   {"OFF", "ON"}},         // 14 0x0E
      {"O_6 Status: SPD0_N", {"OFF", "ON"}},         // 15 0x0F
      {"O_7 Status: TPD5",   {"OFF", "ON"}},         // 16 0x10
      {"O_8 Status: TTPD_N", {"OFF", "ON"}},         // 17 0x11
      {"Digital I/F Test",   {"0",   "1"}},          // 18 0x12
      {NULL              ,   {NULL, NULL}},          // 19 0x13
      {NULL              ,   {NULL, NULL}},          // 20 0x14
      {NULL              ,   {NULL, NULL}},          // 21 0x15
      {NULL              ,   {NULL, NULL}},          // 22 0x16
      {NULL              ,   {NULL, NULL}},          // 23 0x17
      {NULL              ,   {NULL, NULL}},          // 24 0x18
      {NULL              ,   {NULL, NULL}},          // 25 0x19
      {NULL              ,   {NULL, NULL}},          // 26 0x1A
      {NULL              ,   {NULL, NULL}},          // 27 0x1B
      {NULL              ,   {NULL, NULL}},          // 28 0x1C
      {NULL              ,   {NULL, NULL}},          // 29 0x1D
      {NULL              ,   {NULL, NULL}},          // 30 0x1E
      {NULL              ,   {NULL, NULL}}},         // 31 0x1F
// Commands
     {{NULL,                 {NULL, NULL}},          //  0 0x00
      {NULL,                 {NULL, NULL}},          //  1 0x01
      {"O_1: JPD_R",         {"OFF", "ON"}},         //  2 0x02
      {"O_2: ER1_R",         {"OFF", "ON"}},         //  3 0x03
      {"O_3: SPD1_R",        {"OFF", "ON"}},         //  4 0x04
      {"O_4: ER0_R",         {"OFF", "ON"}},         //  5 0x05
      {"O_5: UPD0",          {"OFF", "ON"}},         //  6 0x06
      {"O_6: SPD0_N",        {"OFF", "ON"}},         //  7 0x07
      {"O_7: TPD5",          {"OFF", "ON"}},         //  8 0x08
      {"O_8: TTPD_N",        {"OFF", "ON"}},         //  9 0x09
      {"Digital I/F Test",   {"0",   "1"}},          // 10 0x0A
      {NULL,                 {NULL, NULL}},          // 11 0x0B
      {NULL,                 {NULL, NULL}},          // 12 0x0C
      {NULL,                 {NULL, NULL}},          // 13 0x0D
      {NULL,                 {NULL, NULL}},          // 14 0x0E
      {NULL,                 {NULL, NULL}},          // 15 0x0F
      {NULL,                 {NULL, NULL}},          // 16 0x10
      {NULL,                 {NULL, NULL}},          // 17 0x11
      {NULL,                 {NULL, NULL}},          // 18 0x12
      {"All Outlets ON/OFF", {"OFF", "ON"}},         // 19 0x13
      {NULL,                 {NULL, NULL}},          // 20 0x14
      {NULL,                 {NULL, NULL}},          // 21 0x15
      {NULL,                 {NULL, NULL}},          // 22 0x16
      {NULL,                 {NULL, NULL}},          // 23 0x17
      {NULL,                 {NULL, NULL}},          // 24 0x18
      {NULL,                 {NULL, NULL}},          // 25 0x19
      {NULL,                 {NULL, NULL}},          // 26 0x1A
      {NULL,                 {NULL, NULL}},          // 27 0x1B
      {NULL,                 {NULL, NULL}},          // 28 0x1C
      {NULL,                 {NULL, NULL}},          // 29 0x1D
      {NULL,                 {NULL, NULL}},          // 30 0x1E
      {NULL,                 {NULL, NULL}}},         // 31 0x1F
// Analog Channels
     {{"O_1 Current: JPD_R",    0.0,   7.5},         //  0 0x00
      {"O_2 Current: ER1_R",    0.0,   7.5},         //  1 0x01
      {"O_3 Current: SPD1_R",   0.0,   7.5},         //  2 0x02
      {"O_4 Current: ER0_R",    0.0,   7.5},         //  3 0x03
      {"O_5 Current: UPD0",     0.0,   7.5},         //  4 0x04
      {"O_6 Current: SPD0_N",   0.0,   7.5},         //  5 0x05
      {"O_7 Current: TPD5",     0.0,   7.5},         //  6 0x06
      {"O_8 Current: TTPD_N",   0.0,  15.0},         //  7 0x07
      {"Temperature",         -40.0,  80.0},         //  8 0x08
      {"Analog Ref. 3.0V",      0.0,   4.81765},     //  9 0x09
      {NULL              ,      0.0,   0.0},         // 10 0x0A
      {NULL              ,      0.0,   0.0},         // 11 0x0B
      {NULL              ,      0.0,   0.0},         // 12 0x0C
      {NULL              ,      0.0,   0.0},         // 13 0x0D
      {NULL              ,      0.0,   0.0},         // 14 0x0E
      {NULL              ,      0.0,   0.0},         // 15 0x0F
      {NULL              ,      0.0,   0.0},         // 16 0x10
      {NULL              ,      0.0,   0.0},         // 17 0x11
      {NULL              ,      0.0,   0.0},         // 18 0x12
      {NULL              ,      0.0,   0.0},         // 19 0x13
      {NULL              ,      0.0,   0.0},         // 20 0x14
      {NULL              ,      0.0,   0.0},         // 21 0x15
      {NULL              ,      0.0,   0.0},         // 22 0x16
      {NULL              ,      0.0,   0.0},         // 23 0x17
      {NULL              ,      0.0,   0.0},         // 24 0x18
      {NULL              ,      0.0,   0.0},         // 25 0x19
      {NULL              ,      0.0,   0.0},         // 26 0x1A
      {NULL              ,      0.0,   0.0},         // 27 0x1B
      {NULL              ,      0.0,   0.0},         // 28 0x1C
      {NULL              ,      0.0,   0.0},         // 29 0x1D
      {NULL              ,      0.0,   0.0},         // 30 0x1E
      {NULL              ,      0.0,   0.0}},        // 31 0x1F
  },

//
//~--  ESEM1-A Board (15V Internal Power Supply) ---
//

//                             slot   sta   ovt   tmp   ref   t_w   t_r  --- adc ----
  {"ESEM1-A\n15V Int. Power",  0x0A, 0x00, 0xFF, 0x01, 0x06, 0x06, 0x06, {0x0E, 0x1E},

// Digital Channels
     {{"Board Status",      {"OK",  "NOK"}},         //  0 0x00
      {NULL,                {NULL,   NULL}},         //  1 0x01
      {NULL,                {NULL,   NULL}},         //  2 0x02
      {"DCDC 1 OVC",        {"OK",  "OVC"}},         //  3 0x03
      {"DCDC 2 OVC",        {"OK",  "OVC"}},         //  4 0x04
      {"DCDC 1 Status",     {"OFF", "ON"}},          //  5 0x05
      {"Digital I/F Test",  {"0",   "1"}},           //  6 0x06
      {"DCDC 2 Status",     {"OFF", "ON"}},          //  7 0x07
      {NULL,                {NULL,   NULL}},         //  8 0x08
      {"Main Power Status", {"OFF", "ON"}},          //  9 0x09
      {NULL,                {NULL,   NULL}},         // 10 0x0A
      {NULL,                {NULL,   NULL}},         // 11 0x0B
      {NULL,                {NULL,   NULL}},         // 12 0x0C
      {NULL,                {NULL,   NULL}},         // 13 0x0D
      {NULL,                {NULL,   NULL}},         // 14 0x0E
      {NULL,                {NULL,   NULL}},         // 15 0x0F
      {NULL,                {NULL,   NULL}},         // 16 0x10
      {NULL,                {NULL,   NULL}},         // 17 0x11
      {NULL,                {NULL,   NULL}},         // 18 0x12
      {NULL,                {NULL,   NULL}},         // 19 0x13
      {NULL,                {NULL,   NULL}},         // 20 0x14
      {NULL,                {NULL,   NULL}},         // 21 0x15
      {NULL,                {NULL,   NULL}},         // 22 0x16
      {NULL,                {NULL,   NULL}},         // 23 0x17
      {NULL,                {NULL,   NULL}},         // 24 0x18
      {NULL,                {NULL,   NULL}},         // 25 0x19
      {NULL,                {NULL,   NULL}},         // 26 0x1A
      {NULL,                {NULL,   NULL}},         // 27 0x1B
      {NULL,                {NULL,   NULL}},         // 28 0x1C
      {NULL,                {NULL,   NULL}},         // 29 0x1D
      {NULL,                {NULL,   NULL}},         // 30 0x1E
      {NULL,                {NULL,   NULL}}},        // 31 0x1F
// Commands
     {{NULL,                {NULL,   NULL}},         //  0 0x00
      {NULL,                {NULL,   NULL}},         //  1 0x01
      {NULL,                {NULL,   NULL}},         //  2 0x02
      {NULL,                {NULL,   NULL}},         //  3 0x03
      {NULL,                {NULL,   NULL}},         //  4 0x04
      {NULL,                {NULL,   NULL}},         //  5 0x05
      {"Digital I/F Test",  {"0",    "1"}},          //  6 0x06
      {"Global ON/OFF",     {"ON",   "OFF"}},        //  7 0x07
      {NULL,                {NULL,   NULL}},         //  8 0x08
      {NULL,                {NULL,   NULL}},         //  9 0x09
      {NULL,                {NULL,   NULL}},         // 10 0x0A
      {NULL,                {NULL,   NULL}},         // 11 0x0B
      {NULL,                {NULL,   NULL}},         // 12 0x0C
      {NULL,                {NULL,   NULL}},         // 13 0x0D
      {NULL,                {NULL,   NULL}},         // 14 0x0E
      {NULL,                {NULL,   NULL}},         // 15 0x0F
      {NULL,                {NULL,   NULL}},         // 16 0x10
      {NULL,                {NULL,   NULL}},         // 17 0x11
      {NULL,                {NULL,   NULL}},         // 18 0x12
      {NULL,                {NULL,   NULL}},         // 19 0x13
      {NULL,                {NULL,   NULL}},         // 20 0x14
      {NULL,                {NULL,   NULL}},         // 21 0x15
      {NULL,                {NULL,   NULL}},         // 22 0x16
      {NULL,                {NULL,   NULL}},         // 23 0x17
      {NULL,                {NULL,   NULL}},         // 24 0x18
      {NULL,                {NULL,   NULL}},         // 25 0x19
      {NULL,                {NULL,   NULL}},         // 26 0x1A
      {NULL,                {NULL,   NULL}},         // 27 0x1B
      {NULL,                {NULL,   NULL}},         // 28 0x1C
      {NULL,                {NULL,   NULL}},         // 29 0x1D
      {NULL,                {NULL,   NULL}},         // 30 0x1E
      {NULL,                {NULL,   NULL}}},        // 31 0x1F
// Analog Channels
     {{NULL,                 0.0,    0.0},           //  0 0x00
      {"Temperature",      -40.0,   80.0},           //  1 0x01
      {"Main Power Volt.",   0.0,  150.0},		       //  2 0x02
      {NULL,                 0.0,    0.0},           //  3 0x03
      {NULL,                 0.0,    0.0},           //  4 0x04
      {NULL,                 0.0,    0.0},           //  5 0x05
      {"Analog Ref. 4.0V",   0.0,    4.81765},       //  6 0x06
      {NULL,                 0.0,    0.0},           //  7 0x07
      {NULL,                 0.0,    0.0},           //  8 0x08
      {NULL,                 0.0,    0.0},           //  9 0x09
      {NULL,                 0.0,    0.0},           // 10 0x0A
      {NULL,                 0.0,    0.0},           // 11 0x0B
      {NULL,                 0.0,    0.0},           // 12 0x0C
      {NULL,                 0.0,    0.0},           // 13 0x0D
      {NULL,                 0.0,    0.0},           // 14 0x0E
      {NULL,                 0.0,    0.0},           // 15 0x0F
      {NULL,                 0.0,    0.0},           // 16 0x10
      {NULL,                 0.0,    0.0},           // 17 0x11
      {NULL,                 0.0,    0.0},           // 18 0x12
      {NULL,                 0.0,    0.0},           // 19 0x13
      {NULL,                 0.0,    0.0},           // 20 0x14
      {NULL,                 0.0,    0.0},           // 21 0x15
      {NULL,                 0.0,    0.0},           // 22 0x16
      {NULL,                 0.0,    0.0},           // 23 0x17
      {NULL,                 0.0,    0.0},           // 24 0x18
      {NULL,                 0.0,    0.0},           // 25 0x19
      {NULL,                 0.0,    0.0},           // 26 0x1A
      {NULL,                 0.0,    0.0},           // 27 0x1B
      {NULL,                 0.0,    0.0},           // 28 0x1C
      {NULL,                 0.0,    0.0},           // 29 0x1D
      {NULL,                 0.0,    0.0},           // 30 0x1E
      {NULL,                 0.0,    0.0}}           // 31 0x1F
  }};

//~============================================================================

#endif // _PDSDEF_H
