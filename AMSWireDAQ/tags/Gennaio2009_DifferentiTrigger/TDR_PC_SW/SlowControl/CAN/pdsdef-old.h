// file pdsdef.h
//
// PDS boards names, adresses, and many more
//
// A.Lebedev - Jun-2005...

//~----------------------------------------------------------------------------

#ifndef _PDSDEF_H
#define _PDSDEF_H

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include "uscmlib.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
typedef struct {
  int8  cha;
  char *nam;
  char *txt[2];
} _dig;

typedef struct {
  int8  cha;
  char *nam;
  char *txt[2];
} _cmd;

typedef struct {
  int8  cha;
  char *nam;
  float min;
  float max;
} _ana;

typedef struct {
  char *nam;
  int8 adr;
  int8 adc[2];
  _dig dig[32];
  _cmd cmd[32];
  _ana ana[32];
} _brd;

_brd brd[10] = {
  
//
//~--  ESEM3-B Board (120V Distributor) ---
//

//{"ESEM3-B\n120V Distrib.", 0x00, {0x08, 0x18},
  {"ESEM3-B\n120V Distrib.", 0x00, {0x07, 0x17},     // Modifed by XU@20071016

// Digital Channels
     {{0x00, "Board Status",          {"OK",  "NOK"}},
      {0x01, "OVT Alarm",             {"OK",  "OVT"}},
      {0x02, "O__1 CMD: AH_OUT1",     {"OFF", "ON"}},
      {0x03, "O__2 CMD: AH_OUT2",     {"OFF", "ON"}},
      {0x04, "O__3 CMD: AH_OUT3",     {"OFF", "ON"}},
      {0x05, "O__4 CMD: AH_OUT4",     {"OFF", "ON"}},
      {0x06, "O__5 CMD: AH_OUT5",     {"OFF", "ON"}},
      {0x07, "O__6 CMD: AH_OUT6",     {"OFF", "ON"}},
      {0x08, "O__7 CMD: AH_OUT7",     {"OFF", "ON"}},
      {0x09, "O__8 CMD: AH_OUT8",     {"OFF", "ON"}},
      {0x0A, "O__9 CMD: AH_OUT9",     {"OFF", "ON"}},
      {0x0B, "O_10 CMD: AH_OUT10",    {"OFF", "ON"}},
      {0x0C, "O_11 CMD: AH_OUT11",    {"OFF", "ON"}},
      {0x0D, "O_12 CMD: AO_CCEB",     {"OFF", "ON"}},
      {0x0E, "O__1 Status: AH_OUT1",  {"OFF", "ON"}},
      {0x0F, "O__2 Status: AH_OUT2",  {"OFF", "ON"}},
      {0x10, "O__3 Status: AH_OUT3",  {"OFF", "ON"}},
      {0x11, "O__4 Status: AH_OUT4",  {"OFF", "ON"}},
      {0x12, "O__5 Status: AH_OUT5",  {"OFF", "ON"}},
      {0x13, "O__6 Status: AH_OUT6",  {"OFF", "ON"}},
      {0x14, "O__7 Status: AH_OUT7",  {"OFF", "ON"}},
      {0x15, "O__8 Status: AH_OUT8",  {"OFF", "ON"}},
      {0x16, "O__9 Status: AH_OUT9",  {"OFF", "ON"}},
      {0x17, "O_10 Status: AH_OUT10", {"OFF", "ON"}},
      {0x18, "O_11 Status: AH_OUT11", {"OFF", "ON"}},
      {0x19, "O_12 Status: AO_CCEB",  {"OFF", "ON"}},
      {0x1A, "O_12 Trip: AO_CCEB",    {"OK",  "TRIP"}},
      {0x1B, "Digital I/F Test",      {"0",   "1"}},
      {0xFF             }},
// Commands
     {{0x02, "O__1: AH_OUT1",      {"OFF", "ON"}},
      {0x03, "O__2: AH_OUT2",      {"OFF", "ON"}},
      {0x04, "O__3: AH_OUT3",      {"OFF", "ON"}},
      {0x05, "O__4: AH_OUT4",      {"OFF", "ON"}},
      {0x06, "O__5: AH_OUT5",      {"OFF", "ON"}},
      {0x07, "O__6: AH_OUT6",      {"OFF", "ON"}},
      {0x08, "O__7: AH_OUT7",      {"OFF", "ON"}},
      {0x09, "O__8: AH_OUT8",      {"OFF", "ON"}},
      {0x0A, "O__9: AH_OUT9",      {"OFF", "ON"}},
      {0x0B, "O_10: AH_OUT10",     {"OFF", "ON"}},
      {0x0C, "O_11: AH_OUT11",     {"OFF", "ON"}},
      {0x0D, "O_12: AO_CCEB",      {"OFF", "ON"}},
      {0x1B, "Digital I/F Test",   {"0",   "1"}},
      {0x1C, "All Outlets ON/OFF", {"OFF", "ON"}},
      {0xFF             }},
// Analog Channels
     {{0x00, "Temperature",           -40.0,  80.0},
      {0x01, "O_12 Current: AO_CCEB",   0.0,  15.0},
      {0x02, "Analog Ref. 2.5V",        0.0,   4.81765},
      {0xFF             }},
  },

//
//~--  PB2 #1 Board (120V -> 28V Converter) ---
//

//{"PB2 #1\n120V->28V", 0x01, {0x07, 0x17},
  {"PB2 #1\n120V->28V", 0x01, {0x06, 0x16},      // Modifed by XU@20071016

// Digital Channels
     {{0x00, "Board Status",     {"OK",  "NOK"}},
      {0x01, "OVT Alarm",        {"OVT", "OK"}},
      {0x04, "Input OVC Alarm",  {"OVC", "OK"}},
      {0x05, "DCDC ON/OFF",      {"OFF", "ON"}},
      {0x06, "Digital I/F Test", {"0",   "1"}},
      {0x15, "27V UNV",          {"UNV", "OK"}},
      {0x16, "29V Trip",         {"OK",  "TRIP"}},
      {0x17, "DCDC OFF/ON CMD",  {"ON",  "OFF"}},
      {0xFF             }},
// Commands
     {{0x06, "Digital I/F Test", {"0",   "1"}},
      {0x17, "DCDC OFF/ON",      {"ON",  "OFF"}},
      {0xFF             }},
// Analog Channels
     {{0x01, "Temperature",       -40.0,   90.0},
      {0x04, "Output Voltage",      0.0,   36.76},
      {0x06, "Analog Ref. 2.0V",    0.0,    4.81765},
      {0xFF             }},
  },

//
//~--  ESEM3-A #1 Board (28V Distributor) ---
//

//{"ESEM3-A #1\n28V Distrib.", 0x02, {0x06, 0x16},
  {"ESEM3-A #1\n28V Distrib.", 0x02, {0x05, 0x15},   // Modifed by XU@20071016

// Digital Channels
     {{0x00, "Board Status",       {"OK",  "NOK"}},
      {0x01, "OVT Alarm",          {"OK",  "OVT"}},
      {0x02, "O_1 CMD: TPD3",      {"OFF", "ON"}},
      {0x03, "O_2 CMD: TPD7",      {"OFF", "ON"}},
      {0x04, "O_3 CMD: JPD_N",     {"OFF", "ON"}},
      {0x05, "O_4 CMD: SPD1_N",    {"OFF", "ON"}},
      {0x06, "O_5 CMD: UGPD_N",    {"OFF", "ON"}},
      {0x07, "O_6 CMD: SPD0_R",    {"OFF", "ON"}},
      {0x08, "O_7 CMD: TPD1",      {"OFF", "ON"}},
      {0x09, "O_8 CMD: TTPD_R",    {"OFF", "ON"}},
      {0x0A, "O_1 Status: TPD3",   {"OFF", "ON"}},
      {0x0B, "O_2 Status: TPD7",   {"OFF", "ON"}},
      {0x0C, "O_3 Status: JPD_N",  {"OFF", "ON"}},
      {0x0D, "O_4 Status: SPD1_N", {"OFF", "ON"}},
      {0x0E, "O_5 Status: UGPD_N", {"OFF", "ON"}},
      {0x0F, "O_6 Status: SPD0_R", {"OFF", "ON"}},
      {0x10, "O_7 Status: TPD1",   {"OFF", "ON"}},
      {0x11, "O_8 Status: TTPD_R", {"OFF", "ON"}},
      {0x12, "Digital I/F Test",   {"0",   "1"}},
      {0xFF             }},
// Commands
     {{0x02, "O_1: TPD3",          {"OFF", "ON"}},
      {0x03, "O_2: TPD7",          {"OFF", "ON"}},
      {0x04, "O_3: JPD_N",         {"OFF", "ON"}},
      {0x05, "O_4: SPD1_N",        {"OFF", "ON"}},
      {0x06, "O_5: UGPD_N",        {"OFF", "ON"}},
      {0x07, "O_6: SPD0_R",        {"OFF", "ON"}},
      {0x08, "O_7: TPD1",          {"OFF", "ON"}},
      {0x09, "O_8: TTPD_R",        {"OFF", "ON"}},
      {0x0A, "Digital I/F Test",   {"0",   "1"}},
      {0x13, "All Outlets ON/OFF", {"OFF", "ON"}},
      {0xFF             }},
// Analog Channels
     {{0x00, "O_1 Current: TPD3",     0.0,   7.5},
      {0x01, "O_2 Current: TPD7",     0.0,   7.5},
      {0x02, "O_3 Current: JPD_N",    0.0,   7.5},
      {0x03, "O_4 Current: SPD1_N",   0.0,   7.5},
      {0x04, "O_5 Current: UGPD_N",   0.0,   7.5},
      {0x05, "O_6 Current: SPD0_R",   0.0,   7.5},
      {0x06, "O_7 Current: TPD1",     0.0,   7.5},
      {0x07, "O_8 Current: TTPD_R",   0.0,  15.0},
      {0x08, "Temperature",         -40.0,  80.0},
      {0x09, "Analog Ref. 3.0V",      0.0,   4.81765},
      {0xFF             }},
  },

//
//~--  PB2 #2 Board (120V -> 28V Converter) ---
//

//{"PB2 #2\n120V->28V", 0x03, {0x05, 0x15},
  {"PB2 #2\n120V->28V", 0x03, {0x04, 0x14},    // Modifed by XU@20071016

// Digital Channels
     {{0x00, "Board Status",     {"OK",  "NOK"}},
      {0x01, "OVT Alarm",        {"OVT", "OK"}},
      {0x04, "Input OVC Alarm",  {"OVC", "OK"}},
      {0x05, "DCDC ON/OFF",      {"OFF", "ON"}},
      {0x06, "Digital I/F Test", {"0",   "1"}},
      {0x15, "27V UNV",          {"UNV", "OK"}},
      {0x16, "29V Trip",         {"OK",  "TRIP"}},
      {0x17, "DCDC OFF/ON CMD",  {"ON",  "OFF"}},
      {0xFF             }},
// Commands
     {{0x06, "Digital I/F Test", {"0",   "1"}},
      {0x17, "DCDC OFF/ON",      {"ON",  "OFF"}},
      {0xFF             }},
// Analog Channels
     {{0x01, "Temperature",       -40.0,   90.0},
      {0x04, "Output Voltage",      0.0,   36.76},
      {0x06, "Analog Ref. 2.0V",    0.0,    4.81765},
      {0xFF             }},
  },
  
//
//~--  ESEM3-A #2 Board (28V Distributor) ---
//

//{"ESEM3-A #2\n28V Distrib.", 0x04, {0x04, 0x14},
  {"ESEM3-A #2\n28V Distrib.", 0x04, {0x03, 0x13},       // Modifed by XU@20071016

// Digital Channels
     {{0x00, "Board Status",       {"OK",  "NOK"}},
      {0x01, "OVT Alarm",          {"OK",  "OVT"}},
      {0x02, "O_1 CMD: ER1_N",     {"OFF", "ON"}},
      {0x03, "O_2 CMD: TPD2",      {"OFF", "ON"}},
      {0x04, "O_3 CMD: TPD6",      {"OFF", "ON"}},
      {0x05, "O_4 CMD: SPD3_N",    {"OFF", "ON"}},
      {0x06, "O_5 CMD: MPD_N",     {"OFF", "ON"}},
      {0x07, "O_6 CMD: UPD1",      {"OFF", "ON"}},
      {0x08, "O_7 CMD: SPD2_N",    {"OFF", "ON"}},
      {0x09, "O_8 CMD: CAB_R",     {"OFF", "ON"}},
      {0x0A, "O_1 Status: ER1_N",  {"OFF", "ON"}},
      {0x0B, "O_2 Status: TPD2",   {"OFF", "ON"}},
      {0x0C, "O_3 Status: TPD6",   {"OFF", "ON"}},
      {0x0D, "O_4 Status: SPD3_N", {"OFF", "ON"}},
      {0x0E, "O_5 Status: MPD_N",  {"OFF", "ON"}},
      {0x0F, "O_6 Status: UPD1",   {"OFF", "ON"}},
      {0x10, "O_7 Status: SPD2_N", {"OFF", "ON"}},
      {0x11, "O_8 Status: CAB_R",  {"OFF", "ON"}},
      {0x12, "Digital I/F Test",   {"0",   "1"}},
      {0xFF             }},
// Commands
     {{0x02, "O_1: ER1_N",         {"OFF", "ON"}},
      {0x03, "O_2: TPD2",          {"OFF", "ON"}},
      {0x04, "O_3: TPD6",          {"OFF", "ON"}},
      {0x05, "O_4: SPD3_N",        {"OFF", "ON"}},
      {0x06, "O_5: MPD_N",         {"OFF", "ON"}},
      {0x07, "O_6: UPD1",          {"OFF", "ON"}},
      {0x08, "O_7: SPD2_N",        {"OFF", "ON"}},
      {0x09, "O_8: CAB_R",         {"OFF", "ON"}},
      {0x0A, "Digital I/F Test",   {"0",   "1"}},
      {0x13, "All Outlets ON/OFF", {"OFF", "ON"}},
      {0xFF             }},
// Analog Channels
     {{0x00, "O_1 Current: ER1_N",   0.0,   7.5},
      {0x01, "O_2 Current: TPD2",    0.0,   7.5},
      {0x02, "O_3 Current: TPD6",    0.0,   7.5},
      {0x03, "O_4 Current: SPD3_N",  0.0,   7.5},
      {0x04, "O_5 Current: MPD_N",   0.0,   7.5},
      {0x05, "O_6 Current: UPD1",    0.0,   7.5},
      {0x06, "O_7 Current: SPD2_N",  0.0,   7.5},
      {0x07, "O_8 Current: CAB_R",   0.0,  15.0},
      {0x08, "Temperature",        -40.0,  80.0},
      {0x09, "Analog Ref. 3.0V",     0.0,   4.81765},
      {0xFF             }},
  },

//
//~--  PB2 #3 Board (120V -> 28V Converter) ---
//

//{"PB2 #3\n120V->28V", 0x05, {0x03, 0x13},
  {"PB2 #3\n120V->28V", 0x05, {0x02, 0x12},     // Modifed by XU@20071016

// Digital Channels
     {{0x00, "Board Status",     {"OK",  "NOK"}},
      {0x01, "OVT Alarm",        {"OVT", "OK"}},
      {0x04, "Input OVC Alarm",  {"OVC", "OK"}},
      {0x05, "DCDC ON/OFF",      {"OFF", "ON"}},
      {0x06, "Digital I/F Test", {"0",   "1"}},
      {0x15, "27V UNV",          {"UNV", "OK"}},
      {0x16, "29V Trip",         {"OK",  "TRIP"}},
      {0x17, "DCDC OFF/ON CMD",  {"ON",  "OFF"}},
      {0xFF             }},
// Commands
     {{0x06, "Digital I/F Test", {"0",   "1"}},
      {0x17, "DCDC OFF/ON",      {"ON",  "OFF"}},
      {0xFF             }},
// Analog Channels
     {{0x01, "Temperature",       -40.0,   90.0},
      {0x04, "Output Voltage",      0.0,   36.76},
      {0x06, "Analog Ref. 2.0V",    0.0,    4.81765},
      {0xFF             }},
  },

//
//~--  ESEM3-A #3 Board (28V Distributor) ---
//

//{"ESEM3-A #3\n28V Distrib.", 0x06, {0x02, 0x12},
  {"ESEM3-A #3\n28V Distrib.", 0x06, {0x01, 0x11},      // Modifed by XU@20071016

// Digital Channels
     {{0x00, "Board Status",       {"OK",  "NOK"}},
      {0x01, "OVT Alarm",          {"OK",  "OVT"}},
      {0x02, "O_1 CMD: TPD0",      {"OFF", "ON"}},
      {0x03, "O_2 CMD: TPD4",      {"OFF", "ON"}},
      {0x04, "O_3 CMD: ER0_N",     {"OFF", "ON"}},
      {0x05, "O_4 CMD: MPD_R",     {"OFF", "ON"}},
      {0x06, "O_5 CMD: SPD3_R",    {"OFF", "ON"}},
      {0x07, "O_6 CMD: UGPD_R",    {"OFF", "ON"}},
      {0x08, "O_7 CMD: SPD2_R",    {"OFF", "ON"}},
      {0x09, "O_8 CMD: CAB_N",     {"OFF", "ON"}},
      {0x0A, "O_1 Status: TPD0",   {"OFF", "ON"}},
      {0x0B, "O_2 Status: TPD4",   {"OFF", "ON"}},
      {0x0C, "O_3 Status: ER0_N",  {"OFF", "ON"}},
      {0x0D, "O_4 Status: MPD_R",  {"OFF", "ON"}},
      {0x0E, "O_5 Status: SPD3_R", {"OFF", "ON"}},
      {0x0F, "O_6 Status: UGPD_R", {"OFF", "ON"}},
      {0x10, "O_7 Status: SPD2_R", {"OFF", "ON"}},
      {0x11, "O_8 Status: CAB_N",  {"OFF", "ON"}},
      {0x12, "Digital I/F Test",   {"0",   "1"}},
      {0xFF             }},
// Commands
     {{0x02, "O_1: TPD0",          {"OFF", "ON"}},
      {0x03, "O_2: TPD4",          {"OFF", "ON"}},
      {0x04, "O_3: ER0_N",         {"OFF", "ON"}},
      {0x05, "O_4: MPD_R",         {"OFF", "ON"}},
      {0x06, "O_5: SPD3_R",        {"OFF", "ON"}},
      {0x07, "O_6: UGPD_R",        {"OFF", "ON"}},
      {0x08, "O_7: SPD2_R",        {"OFF", "ON"}},
      {0x09, "O_8: CAB_N",         {"OFF", "ON"}},
      {0x0A, "Digital I/F Test",   {"0",   "1"}},
      {0x13, "All Outlets ON/OFF", {"OFF", "ON"}},
      {0xFF             }},
// Analog Channels
     {{0x00, "O_1 Current: TPD0",    0.0,   7.5},
      {0x01, "O_2 Current: TPD4",    0.0,   7.5},
      {0x02, "O_3 Current: ER0_N",   0.0,   7.5},
      {0x03, "O_4 Current: MPD_R",   0.0,   7.5},
      {0x04, "O_5 Current: SPD3_R",  0.0,   7.5},
      {0x05, "O_6 Current: UGPD_R",  0.0,   7.5},
      {0x06, "O_7 Current: SPD2_R",  0.0,   7.5},
      {0x07, "O_8 Current: CAB_N",   0.0,  15.0},
      {0x08, "Temperature",        -40.0,  80.0},
      {0x09, "Analog Ref. 3.0V",     0.0,   4.81765},
      {0xFF             }},
  },

//
//~--  PB2 #4 Board (120V -> 28V Converter) ---
//

//{"PB2 #4\n120V->28V", 0x07, {0x01, 0x11},
  {"PB2 #4\n120V->28V", 0x07, {0x00, 0x10},     // Modifed by XU@20071016

// Digital Channels
     {{0x00, "Board Status",     {"OK",  "NOK"}},
      {0x01, "OVT Alarm",        {"OVT", "OK"}},
      {0x04, "Input OVC Alarm",  {"OVC", "OK"}},
      {0x05, "DCDC ON/OFF",      {"OFF", "ON"}},
      {0x06, "Digital I/F Test", {"0",   "1"}},
      {0x15, "27V UNV",          {"UNV", "OK"}},
      {0x16, "29V Trip",         {"OK",  "TRIP"}},
      {0x17, "DCDC OFF/ON CMD",  {"ON",  "OFF"}},
      {0xFF             }},
// Commands
     {{0x06, "Digital I/F Test", {"0",   "1"}},
      {0x17, "DCDC OFF/ON",      {"ON",  "OFF"}},
      {0xFF             }},
// Analog Channels
     {{0x01, "Temperature",       -40.0,   90.0},
      {0x04, "Output Voltage",      0.0,   36.76},
      {0x06, "Analog Ref. 2.0V",    0.0,    4.81765},
      {0xFF             }},
  },

//
//~--  ESEM3-A #4 Board (28V Distributor) ---
//

//{"ESEM3-A #4\n28V Distrib.", 0x08, {0x10, 0x20},
  {"ESEM3-A #4\n28V Distrib.", 0x08, {0x0F, 0x1F},     // Modifed by XU@20071016

// Digital Channels
     {{0x00, "Board Status",       {"OK",  "NOK"}},
      {0x01, "OVT Alarm",          {"OK",  "OVT"}},
      {0x02, "O_1 CMD: JPD_R",     {"OFF", "ON"}},
      {0x03, "O_2 CMD: ER1_R",     {"OFF", "ON"}},
      {0x04, "O_3 CMD: SPD1_R",    {"OFF", "ON"}},
      {0x05, "O_4 CMD: ER0_R",     {"OFF", "ON"}},
      {0x06, "O_5 CMD: UPD0",      {"OFF", "ON"}},
      {0x07, "O_6 CMD: SPD0_N",    {"OFF", "ON"}},
      {0x08, "O_7 CMD: TPD5",      {"OFF", "ON"}},
      {0x09, "O_8 CMD: TTPD_N",    {"OFF", "ON"}},
      {0x0A, "O_1 Status: JPD_R",  {"OFF", "ON"}},
      {0x0B, "O_2 Status: ER1_R",  {"OFF", "ON"}},
      {0x0C, "O_3 Status: SPD1_R", {"OFF", "ON"}},
      {0x0D, "O_4 Status: ER0_R",  {"OFF", "ON"}},
      {0x0E, "O_5 Status: UPD0",   {"OFF", "ON"}},
      {0x0F, "O_6 Status: SPD0_N", {"OFF", "ON"}},
      {0x10, "O_7 Status: TPD5",   {"OFF", "ON"}},
      {0x11, "O_8 Status: TTPD_N", {"OFF", "ON"}},
      {0x12, "Digital I/F Test",   {"0",   "1"}},
      {0xFF             }},
// Commands
     {{0x02, "O_1: JPD_R",         {"OFF", "ON"}},
      {0x03, "O_2: ER1_R",         {"OFF", "ON"}},
      {0x04, "O_3: SPD1_R",        {"OFF", "ON"}},
      {0x05, "O_4: ER0_R",         {"OFF", "ON"}},
      {0x06, "O_5: UPD0",          {"OFF", "ON"}},
      {0x07, "O_6: SPD0_N",        {"OFF", "ON"}},
      {0x08, "O_7: TPD5",          {"OFF", "ON"}},
      {0x09, "O_8: TTPD_N",        {"OFF", "ON"}},
      {0x0A, "Digital I/F Test",   {"0",   "1"}},
      {0x13, "All Outlets ON/OFF", {"OFF", "ON"}},
      {0xFF             }},
// Analog Channels
//   {{0x00, "O_1 Current: JPD_R",   0.0,   7.339},    // my number
     {{0x00, "O_1 Current: JPD_R",   0.0,   7.293},    // Manwoo number
      {0x01, "O_2 Current: ER1_R",   0.0,   7.5},
      {0x02, "O_3 Current: SPD1_R",  0.0,   7.5},
      {0x03, "O_4 Current: ER0_R",   0.0,   7.5},
      {0x04, "O_5 Current: UPD0",    0.0,   7.5},
      {0x05, "O_6 Current: SPD0_N",  0.0,   7.5},
      {0x06, "O_7 Current: TPD5",    0.0,   7.5},
      {0x07, "O_8 Current: TTPD_N",  0.0,  15.0},
      {0x08, "Temperature",        -40.0,  80.0},
      {0x09, "Analog Ref. 3.0V",     0.0,   5.225},
      {0xFF             }},
  },

//
//~--  ESEM1-A Board (15V Internal Power Supply) ---
//

//{"ESEM1-A\n15V Int. Power", 0x10, {0x0F, 0x1f},        // wrong address!!
  {"ESEM1-A\n15V Int. Power", 0x0A, {0x0E, 0x1E},        // Modifed by XU@20071016

// Digital Channels
     {{0x00, "Board Status",      {"OK",  "NOK"}},
      {0x03, "DCDC 1 OVC",        {"OK",  "OVC"}},
      {0x04, "DCDC 2 OVC",        {"OK",  "OVC"}},
      {0x05, "DCDC 1 Status",     {"OFF", "ON"}},
      {0x06, "Digital I/F Test",  {"0",   "1"}},
      {0x07, "DCDC 2 Status",     {"OFF", "ON"}},
      {0x09, "Main Power Status", {"OFF", "ON"}},
      {0xFF             }},
// Commands
     {{0x06, "Digital I/F Test", {"0",   "1"}},
      {0x07, "Global ON/OFF",    {"OFF", "ON"}},
      {0xFF             }},
// Analog Channels
//   {{0x01, "Temperature",      -40.0,   76.5},	
     {{0x01, "Temperature",      -40.0,   80.0},        // Modifed by XU@20071016
      {0x02, "Main Power Volt.",   0.0,  150.0},		    // Modifed by XU@20071016
//    {0x03, "Analog Ref. 4.0V",   0.0,    4.81765},    // deleted 
      {0x06, "Analog Ref. 4.0V",   0.0,    4.81765},
      {0xFF             }},
  }};

//~============================================================================

#endif // _PDSDEF_H
