// file AMS_commanding_language.h

#include <stdlib.h>

#define MAX_NB_OF_SYNONIMS 5

#define MAX_NB_OF_KEYWORDS 15

#define EDIT   0
#define USE    1
#define AT     2
#define ON     3
#define PERIOD 4
#define ID     5
#define at     6
#define GMT    7
#define NASA   8
#define EXIT   9

struct {
  char *name[MAX_NB_OF_SYNONIMS];
  char *object;
  int present;
} keyword[MAX_NB_OF_KEYWORDS] = {

{ { "EDIT",   NULL            }, NULL                  },
{ { "USE",    "USING",    NULL}, "data file name"      },
{ { "AT",     NULL            }, "starting time"       },
{ { "ON",     NULL            }, "starting date"       },
{ { "PERIOD", "PRIORITY", NULL}, "value of period"     },
{ { "ID",     NULL            }, "command ID"          },
{ { "@",      NULL            }, "command file name"   },
{ { "GMT",    NULL            }, NULL                  },
{ { "NASA",   NULL            }, "GMT in 'NASA' format"},
{ { "EXIT",   "QUIT",     NULL}, NULL                  },
{ { NULL }}};

//**************************************************************
//                                                             *
//                         COMMAND definition:                 *
//                                                             *
//**************************************************************

// command_mode definition:

#define ALL    0xFFFF

#define GET    0x0001
#define SET    0x0002
#define EXE    0x0004
#define CANCEL 0x0008
#define START  0x0010
#define STOP   0x0020
#define RESET  0x0040
#define HKP    0x8000

#define MAX_NB_OF_COMMANDS 20

typedef struct {
          int command_mode;
          char *parent;
          char *name[MAX_NB_OF_SYNONIMS];
          unsigned short data;
          int q;
        } TABLE;

TABLE command[MAX_NB_OF_COMMANDS] = {

//  command      command                        block
//   mode         name         synonims          type

{    GET,    "", {"GET",    "READ", "SHOW", ""}, 0x1},
{    SET,    "", {"SET",    "PUT",  "INIT", ""}, 0x2},
{    EXE,    "", {"EXE",    "ACT",  "PING", ""}, 0x3},
{    CANCEL, "", {"CANCEL", "KILL",         ""}, 0x3},
{    START,  "", {"START",                  ""}, 0x3},
{    STOP,   "", {"STOP",                   ""}, 0x2},
{    RESET,  "", {"RESET",                  ""}, 0x3},
{    0,      "", {                          ""}     }};

//**************************************************************
//                                                             *
//                   SUBSYSTEM_GROUP definition:               *
//                                                             *
//**************************************************************

#define MAX_NB_OF_SUBSYSTEM_GROUPS 10

TABLE subsystem_group[MAX_NB_OF_SUBSYSTEM_GROUPS] = {

{ ALL, "", { "DAQ & Trigger",     ""} },
{ ALL, "", { "Detectors",         ""} },
{ ALL, "", { "Misc",              ""} },
{ 0,   "", {                      ""} }};

/*   alternative definition (for future):

{ ALL, "", { "DAQ & Trigger",   ""} }, // JFO,JL1, JL3A, JL3B
{ ALL, "", { "Scint./Aerogel",  ""} }, // TOF, ACC, ATC
{ ALL, "", { "Tracker",         ""} }, // TRA, TPS, TASE
{ ALL, "", { "Monitoring",      ""} }, // MCC, MSEA, MSEB
{ ALL, "", { "Power & Heaters", ""} }, // PDD, PSB, HTC
{ ALL, "", { "Processes",       ""} }, // RUN, HRDL
{ 0,   "", {                    ""} }};
*/

//**************************************************************
//                                                             *
//                         SUBSYSTEM definition:               *
//                                                             *
//**************************************************************

#define MAX_NB_OF_SUBSYSTEMS 25

TABLE subsystem[MAX_NB_OF_SUBSYSTEMS] = {

//  command     subsystm_group  subsystem                          node
//   mode           name          name        synonims             type

{ GET+SET+EXE, "DAQ & Trigger", { "JFO",  "HRDL",             ""}, 0x2},
{ GET+SET+EXE, "DAQ & Trigger", { "JL1",  "Level-1",          ""}, 0x2},
{ GET+SET+EXE+
  CANCEL,      "DAQ & Trigger", { "MCC",                      ""}, 0x8},
{ GET+SET+EXE, "DAQ & Trigger", { "CAN",                      ""}, 0x8},
{ GET+SET+EXE, "DAQ & Trigger", { "JL3",                      ""}, 0x9},
{ GET+SET+EXE, "DAQ & Trigger", { "JDQS",                     ""}, 0xA},
{ GET+SET+EXE, "Detectors",     { "CNT",  "ShortCrate",       ""}, 0xA},
{ GET+SET,     "Detectors",     { "TOF",  "Scintillators",    ""}, 0xA},
{ GET+SET,     "Detectors",     { "ACC",  "Anti",             ""}, 0xA},
{ GET+SET,     "Detectors",     { "ATC",  "Aerogel",          ""}, 0xA},
{ GET+SET+EXE, "Detectors",     { "TRA",  "Tracker",          ""}, 0xB},
{ GET+SET,     "Detectors",     { "TPS",  "Tracker_PS",       ""}, 0xB}, //Sandor
{ GET+SET+EXE, "Misc",          { "HTC",  "Heaters", "Temp",  ""}, 0xC},
{ GET+SET,     "Misc",          { "PDD",  "Power",            ""}, 0xC},
{ GET+SET,     "Misc",          { "PSB",                      ""}, 0xD},
{ GET,         "Misc",          { "MSEA",                     ""}, 0xE},
{ GET,         "Misc",          { "MSEB",                     ""}, 0xE},
{ GET+SET+EXE, "Misc",          { "TASE", "Alignment",        ""}, 0xF},
{ 0,           "",              {                             ""}     }};

//**************************************************************
//                                                             *
//                         OBJECT definition:                  *
//                                                             *
//**************************************************************

#define MAX_NB_OF_OBJECTS 100

TABLE object[MAX_NB_OF_OBJECTS] = {

//  command   subsystem   object                      node
//   mode       name       name      synonims        number

{ GET+SET+EXE, "JFO",  { "JFO",    "JFO.02.01",  ""}, 0x0},
{ GET+SET+EXE, "JL1",  { "JL1",    "JL1.02.09",  ""}, 0x1},
{ GET+SET+EXE, "JL1",  { "JL1F",   "JL1F.02.11", ""}, 0x2},
{ GET+SET+EXE+
  CANCEL     , "MCC",  { "MCC",                  ""}, 0x0},
{ GET+SET+EXE, "MCC",  { "MCC2",                 ""}, 0x1},
{ GET+SET+EXE, "CAN",  { "CAN",                  ""}, 0x0},
{ GET+SET+EXE, "JL3",  { "JL3A02", "JL3A.02.02", ""}, 0x0},
{ GET+SET+EXE, "JL3",  { "JL3B04", "JL3B.02.04", ""}, 0x1},
{ GET+SET+EXE, "JL3",  { "JL3B06", "JL3B.02.06", ""}, 0x2},
{ GET+SET+EXE, "JL3",  { "JL3B13", "JL3B.02.13", ""}, 0x3},
{ GET+SET+EXE, "JL3",  { "JL3B15", "JL3B.02.15", ""}, 0x4},
{ GET+SET+EXE, "JL3",  { "JL3A17", "JL3A.02.17", ""}, 0x5},

{ GET+SET+EXE, "JDQS", { "JDQS01",      ""}, 0x0},
{ GET+SET+EXE, "JDQS", { "JDQS31",      ""}, 0x1},
{ GET+SET+EXE, "JDQS", { "JDQS41",      ""}, 0x2},
{ GET+SET+EXE, "JDQS", { "JDQS71",      ""}, 0x3},
{ GET+SET+EXE, "JDQS", { "JDQS03",      ""}, 0x4},
{ GET+SET+EXE, "JDQS", { "JDQS33",      ""}, 0x5},
{ GET+SET+EXE, "JDQS", { "JDQS43",      ""}, 0x6},
{ GET+SET+EXE, "JDQS", { "JDQS73",      ""}, 0x7},

{ GET+SET+EXE, "CNT",  { "CNT01",  "CNT.01",     ""}, 0x0},
{ GET+SET+EXE, "CNT",  { "CNT31",  "CNT.31",     ""}, 0x1},
{ GET+SET+EXE, "CNT",  { "CNT41",  "CNT.41",     ""}, 0x2},
{ GET+SET+EXE, "CNT",  { "CNT71",  "CNT.71",     ""}, 0x3},
{ GET+SET+EXE, "CNT",  { "CNT03",  "CNT.03",     ""}, 0x4},
{ GET+SET+EXE, "CNT",  { "CNT33",  "CNT.33",     ""}, 0x5},
{ GET+SET+EXE, "CNT",  { "CNT43",  "CNT.43",     ""}, 0x6},
{ GET+SET+EXE, "CNT",  { "CNT73",  "CNT.73",     ""}, 0x7},

{ GET+SET,     "TOF",  { "TOF01",  "TOF.01",     ""}, 0x0},
{ GET+SET,     "TOF",  { "TOF31",  "TOF.31",     ""}, 0x1},
{ GET+SET,     "TOF",  { "TOF41",  "TOF.41",     ""}, 0x2},
{ GET+SET,     "TOF",  { "TOF71",  "TOF.71",     ""}, 0x3},
{ GET+SET,     "TOF",  { "TOF03",  "TOF.03",     ""}, 0x4},
{ GET+SET,     "TOF",  { "TOF33",  "TOF.33",     ""}, 0x5},
{ GET+SET,     "TOF",  { "TOF43",  "TOF.43",     ""}, 0x6},
{ GET+SET,     "TOF",  { "TOF73",  "TOF.73",     ""}, 0x7},

{ GET+SET,     "ACC",  { "ACC01",  "ACC.01",     ""}, 0x0},
{ GET+SET,     "ACC",  { "ACC31",  "ACC.31",     ""}, 0x1},
{ GET+SET,     "ACC",  { "ACC41",  "ACC.41",     ""}, 0x2},
{ GET+SET,     "ACC",  { "ACC71",  "ACC.71",     ""}, 0x3},
{ GET+SET,     "ACC",  { "ACC03",  "ACC.03",     ""}, 0x4},
{ GET+SET,     "ACC",  { "ACC33",  "ACC.33",     ""}, 0x5},
{ GET+SET,     "ACC",  { "ACC43",  "ACC.43",     ""}, 0x6},
{ GET+SET,     "ACC",  { "ACC73",  "ACC.73",     ""}, 0x7},

{ GET+SET,     "ATC",  { "ATC01",  "ATC.01",     ""}, 0x0},
{ GET+SET,     "ATC",  { "ATC41",  "ATC.41",     ""}, 0x2},
{ GET+SET,     "ATC",  { "ATC03",  "ATC.03",     ""}, 0x4},
{ GET+SET,     "ATC",  { "ATC33",  "ATC.33",     ""}, 0x5},
{ GET+SET,     "ATC",  { "ATC43",  "ATC.43",     ""}, 0x6},
{ GET+SET,     "ATC",  { "ATC73",  "ATC.73",     ""}, 0x7},

{ GET+SET+EXE, "TRA",  { "TRA32",  "TRA.32",     ""}, 0x2},
{ GET+SET+EXE, "TRA",  { "TRA72",  "TRA.72",     ""}, 0x5},
{ GET+SET,     "TPS",  { "TPS32",                ""}, 0x2}, //Sandor
{ GET+SET,     "TPS",  { "TPS72",                ""}, 0x5}, //Sandor
{ GET+SET+EXE, "HTC",  { "HTC11",  "HTC.11",     ""}, 0x0},
{ GET+SET+EXE, "HTC",  { "HTC23",  "HTC.23",     ""}, 0x1},
{ GET+SET+EXE, "HTC",  { "HTC51",  "HTC.51",     ""}, 0x2},
{ GET+SET+EXE, "HTC",  { "HTC63",  "HTC.63",     ""}, 0x3},
{ GET+SET+EXE, "HTC",  { "HTC83",  "HTC.8",      ""}, 0x4},
{ GET+SET,     "PDD",  { "PDD",                  ""}, 0x5},

{ GET+SET,     "PSB",  { "PSB0",   "PSB.0",      ""}, 0x0},
{ GET+SET,     "PSB",  { "PSB3",   "PSB.3",      ""}, 0x4},
{ GET+SET,     "PSB",  { "PSB4",   "PSB.4",      ""}, 0x2},
{ GET+SET,     "PSB",  { "PSB7",   "PSB.7",      ""}, 0x3},

{ GET,         "MSEA", { "MSEA31", "MSEA.31.08", ""}, 0x1},
{ GET,         "MSEA", { "MSEA71", "MSEA.71.08", ""}, 0x6},
{ GET,         "MSEB", { "MSEB02", "MSEB.02.19", ""}, 0x5},

{ GET+SET+EXE, "TASE", { "TASE33", "TASE.33.07",
                                   "U101",
                                   "LAS11",      ""}, 0x1},
{ GET+SET+EXE, "TASE", { "TASE73", "TASE.73.07",
                                   "U202",
                                   "LAS7",       ""}, 0x0},
{ 0,           "",     {                         ""}     }};


//**************************************************************
//                                                             *
//                         DATASET definition:                 *
//                                                             *
//**************************************************************

#define MAX_NB_OF_DATASETS 250

TABLE dataset[MAX_NB_OF_SUBSYSTEMS*MAX_NB_OF_DATASETS] = {

//  command   subsystem  dataset                      data
//   mode       name      name      synonims          type

{ GET+SET+EXE, "JFO",  { "FLAGS",               ""},  0x01 },
//===================================================================  MCC  ======
{ GET+SET+EXE, "MCC",  { "",                    ""},  0x00 },  //  ping command
{ GET+SET+EXE, "CAN",  { "MAP",                 ""},  0x01 },
{ GET+SET+EXE, "CAN",  { "MASK",                ""},  0x02 },
{ GET,         "MCC",  { "LIST",                ""},  0x03 },
{ EXE+CANCEL,  "MCC",  { "FEW_CMDS",            ""},  0x04 },
{ EXE+CANCEL,  "MCC",  { "ALL_CMDS",            ""},  0x05 },
{ GET+SET+EXE, "MCC",  { "DELAYS",              ""},  0x06 },
{ GET+SET+EXE, "MCC",  { "TIME_X",              ""},  0x07 },
{ EXE,         "MCC",  { "WAIT",                ""},  0x08 },
{ EXE,         "MCC",  { "SAVE",                ""},  0x09 },	// save context
{ EXE,         "MCC",  { "RESTORE",             ""},  0x0A },	// restore context
{ EXE,         "MCC",  { "JL3_TIME",            ""},  0x0B },	// set time in JL3
{ GET,         "MCC",  { "STATUS",              ""},  0x0C },
{ EXE,         "MCC",  { "D-CMD",               ""},  0x0D },
{ GET,         "MCC",  { "E-CMD",               ""},  0x0E },
{ EXE,         "MCC",  { "F-CMD",               ""},  0x0F },
{ EXE,         "MCC",  { "10-CMD",              ""},  0x10 },
{ EXE,         "MCC",  { "11-CMD",              ""},  0x11 },
{ EXE,         "MCC",  { "12-CMD",              ""},  0x12 },
{ EXE,         "MCC",  { "13-CMD",              ""},  0x13 },
{ EXE,         "MCC",  { "14-CMD",              ""},  0x14 },
{ EXE,         "MCC",  { "15-CMD",              ""},  0x15 },
{ EXE,         "MCC",  { "CRC_ERROR_1",         ""},  0x1D },
{ EXE,         "MCC",  { "CRC_ERROR_2",         ""},  0x1E },
{ EXE,         "MCC",  { "TEST",                ""},  0x1F },
{ EXE,         "MCC",  { "20HEX",               ""},  0x20 },
{ EXE,         "MCC",  { "21HEX",               ""},  0x21 },
{ EXE,         "MCC",  { "22HEX",               ""},  0x22 },
{ EXE,         "MCC",  { "23HEX",               ""},  0x23 },

//===================================================================  CNT  ======
{ GET+SET,     "TOF",  { "HV",                  ""},  0x01 },
{ GET+SET,     "TOF",  { "THR",  "Thresholds",  ""},  0x02 },
{ GET+SET,     "ACC",  { "HV",                  ""},  0x03 },
{ GET+SET,     "ACC",  { "THR",  "Thresholds",  ""},  0x04 },
{ GET+SET,     "ATC",  { "HT",                  ""},  0x05 },
{ GET+SET,     "JDQS", { "STATUS",              ""},  0x06 },
{ GET+SET,     "TOF",  { "TEMP", "Temperature", ""},  0x07 },
{ GET+SET+EXE, "JDQS", { "DEBUG",               ""},  0x08 },
{ GET+SET+EXE, "CNT",  { "POWER",               ""},  0x09 },

{ GET+SET,     "TOF",  { "HVW",                 ""},  0x21 },
{ GET+SET,     "TOF",  { "THRW",  "Thresholds", ""},  0x22 },
{ GET+SET,     "ACC",  { "HVW",                 ""},  0x23 },
{ GET+SET,     "ACC",  { "THRW",  "Thresholds", ""},  0x24 },
{ GET+SET,     "ATC",  { "HTW",                 ""},  0x25 },
{ GET+SET,     "JDQS", { "STATUSW",             ""},  0x26 },
{ GET+SET,     "TOF",  { "TEMPW", "Temperature",""},  0x27 },
{ GET+SET+EXE, "JDQS", { "DEBUGW",              ""},  0x28 },
{ GET+SET+EXE, "CNT",  { "POWERW",              ""},  0x29 },

//===================================================================  JL3  ======
{ SET,         "JL3",  { "TIME",                ""},  0x01 },
{ SET,         "JL3",  { "SCHEDULE",            ""},  0x02 },
{ SET,         "JL3",  { "CTRL_BLK",            ""},  0x03 },
{ SET,         "JL3",  { "PROGRAM",             ""},  0x04 },
{ SET,         "JL3",  { "REBOOT",              ""},  0x05 },
{ SET,         "JL3",  { "BUFFER",              ""},  0x06 },
{ SET,         "JL3",  { "JL1",                 ""},  0x07 },
{ SET,         "JL3",  { "TRIGGER",             ""},  0x08 },
{ SET,         "JL3",  { "INIT",                ""},  0x09 },
{ SET,         "JL3",  { "TEMPOR",              ""},  0x0A },
{ SET,         "JL3",  { "ADDRESS",             ""},  0x0B },
{ SET,         "JL3",  { "JDQS_PAR",            ""},  0x0C },
{ SET,         "JL3",  { "JDQT_PAR",            ""},  0x0D },

{ GET,         "JL3",  { "STATUS",              ""},  0x01 },
{ GET,         "JL3",  { "UPLOAD",              ""},  0x02 },
{ GET,         "JL3",  { "CHECK_FM",            ""},  0x03 },
{ GET,         "JL3",  { "READ_FM",             ""},  0x04 },
{ GET,         "JL3",  { "EVENT",               ""},  0x05 },
{ GET,         "JL3",  { "JDQT_INFO",           ""},  0x06 },
{ GET,         "JL3",  { "JDQS_INFO",           ""},  0x07 },
{ GET,         "JL3",  { "L1_SCAL",             ""},  0x08 },
{ GET,         "JL3",  { "L3_HIST",             ""},  0x09 },

{ GET+SET,     "JL3",  { "TEST_0",              ""},  0x10 },
{ GET+SET,     "JL3",  { "TEST_1",              ""},  0x11 },
{ GET+SET,     "JL3",  { "TEST_2",              ""},  0x12 },
{ GET+SET,     "JL3",  { "TEST_3",              ""},  0x13 },
{ GET+SET,     "JL3",  { "TEST_4",              ""},  0x14 },
{ GET+SET,     "JL3",  { "TEST_5",              ""},  0x15 },
{ GET+SET,     "JL3",  { "TEST_6",              ""},  0x16 },
{ GET+SET,     "JL3",  { "TEST_7",              ""},  0x17 },

//===================================================================  PSB  ======
{ GET+SET,     "PSB",  { "STAT",                ""},  0x14 },
{ GET,         "PSB",  { "VOLT",                ""},  0x1C },
{ GET,         "PSB",  { "TEMP",                ""},  0x12 },
{ GET+SET,     "PSB",  { "STATW",               ""},  0x34 },
{ GET,         "PSB",  { "VOLTW",               ""},  0x3C },
{ GET,         "PSB",  { "TEMPW",               ""},  0x32 },
{ GET,         "PSB",  { "AWR",                 ""},  0x13 },
{ SET,         "PSB",  { "AWR",                 ""},  0x05 },
{ GET+SET,     "PSB",  { "ALARMH",              ""},  0x15 },
{ GET+SET,     "PSB",  { "ALARML",              ""},  0x16 },
{ GET+SET,     "PSB",  { "WARNH",               ""},  0x17 },
{ GET+SET,     "PSB",  { "WARNL",               ""},  0x18 },
{ GET+SET,     "PSB",  { "REGH",                ""},  0x19 },
{ GET+SET,     "PSB",  { "REGL",                ""},  0x1A },
{ GET+SET,     "PSB",  { "NOM",                 ""},  0x1B },
{ GET,         "PSB",  { "ERR_LST",             ""},  0x08 },
{ SET,         "PSB",  { "ERR_CLR",             ""},  0x08 },
{ GET,         "PSB",  { "MCU_STAT",            ""},  0x01 },
{ GET,         "PSB",  { "MCU_INFO",            ""},  0x02 },

//===================================================================  HTC  ===============
{ GET+SET,     "HTC",  { "STAT",                ""},  0x14 },
{ GET,         "HTC",  { "TEMP",                ""},  0x12 },
{ GET+SET,     "HTC",  { "STATW",               ""},  0x34 },
{ GET,         "HTC",  { "TEMPW",               ""},  0x32 },
{ GET,         "HTC",  { "AWR",                 ""},  0x13 },
{ SET,         "HTC",  { "AWR",                 ""},  0x05 },
{ GET+SET,     "HTC",  { "ALARMH",              ""},  0x15 },
{ GET+SET,     "HTC",  { "ALARML",              ""},  0x16 },
{ GET+SET,     "HTC",  { "WARNH",               ""},  0x17 },
{ GET+SET,     "HTC",  { "WARNL",               ""},  0x18 },
{ GET+SET,     "HTC",  { "REGH",                ""},  0x19 },
{ GET+SET,     "HTC",  { "REGL",                ""},  0x1A },
{ GET+SET,     "HTC",  { "NOM",                 ""},  0x1B },
{ GET,         "HTC",  { "ERR_LST",             ""},  0x08 },
{ SET,         "HTC",  { "ERR_CLR",             ""},  0x08 },
{ GET,         "HTC",  { "MCU_STAT",            ""},  0x01 },
{ GET,         "HTC",  { "MCU_INFO",            ""},  0x02 },

//===================================================================  PDD  ===============
{ GET+SET,     "PDD",  { "STAT",                ""},  0x14 },
{ GET,         "PDD",  { "VOLT",                ""},  0x1C },
{ GET,         "PDD",  { "TEMP",                ""},  0x12 },
{ GET+SET,     "PDD",  { "STATW",               ""},  0x34 },
{ GET,         "PDD",  { "VOLTW",               ""},  0x3C },
{ GET,         "PDD",  { "TEMPW",               ""},  0x32 },
{ GET,         "PDD",  { "AWR",                 ""},  0x13 },
{ GET+SET,     "PDD",  { "ALARMH",              ""},  0x15 },
{ GET+SET,     "PDD",  { "ALARML",              ""},  0x16 },
{ GET+SET,     "PDD",  { "WARNH",               ""},  0x17 },
{ GET+SET,     "PDD",  { "WARNL",               ""},  0x18 },
{ GET+SET,     "PDD",  { "REGH",                ""},  0x19 },
{ GET+SET,     "PDD",  { "REGL",                ""},  0x1A },
{ GET+SET,     "PDD",  { "NOM",                 ""},  0x1B },
{ GET,         "PDD",  { "ERR_LST",             ""},  0x08 },
{ SET,         "PDD",  { "ERR_CLR",             ""},  0x08 },
{ GET,         "PDD",  { "MCU_STAT",            ""},  0x01 },
{ GET,         "PDD",  { "MCU_INFO",            ""},  0x02 },

//===================================================================  MSEA ===============
{ GET,         "MSEA", { "TEMP",                ""},  0x12 },
{ GET,         "MSEA", { "B_FIELD",             ""},  0x1E },
{ GET,         "MSEA", { "ERR_LST",             ""},  0x08 },
{ SET,         "MSEA", { "ERR_CLR",             ""},  0x08 },
{ GET,         "MSEA", { "MCU_STAT",            ""},  0x01 },
{ GET,         "MSEA", { "MCU_INFO",            ""},  0x02 },
{ GET,         "MSEA", { "TASE",                ""},  0x1F },   // for Werner

//===================================================================  MSEB ===============
{ GET,         "MSEB", { "TEMP",                ""},  0x1C },
{ GET,         "MSEB", { "ERR_LST",             ""},  0x08 },
{ SET,         "MSEB", { "ERR_CLR",             ""},  0x08 },
{ GET,         "MSEB", { "MCU_STAT",            ""},  0x01 },
{ GET,         "MSEB", { "MCU_INFO",            ""},  0x02 },

//===================================================================  TRA  ===============
{ GET+SET+EXE, "TRA",  { "SOMETHING",           ""},  0x01 },
{ SET,         "TPS",  { "",                    ""},  0x10 },  // Sandor's Unified set
{ SET,         "TPS",  { "UPLOAD",   "U",       ""},  0x11 },  // Sandor's Memory upload
{ GET,         "TPS",  { "",                    ""},  0x18 },  // Sandor's Unified read
{ GET,         "TPS",  { "DOWNLOAD", "D",       ""},  0x19 },  // Sandor's Memory Read
{ GET,         "TPS",  { "MEMCRC",   "M",       ""},  0x1A },  // Sandor's Memory CRC

//===================================================================  TASE ===============
{ GET+SET+EXE, "TASE", { "",                    ""},  0x1F },
{ GET,         "TASE", { "ERR_LST",             ""},  0x08 },
{ SET,         "TASE", { "ERR_CLR",             ""},  0x08 },
{ GET,         "TASE", { "MCU_STAT",            ""},  0x01 },
{ GET,         "TASE", { "MCU_INFO",            ""},  0x02 },

{   0 ,         "",    {                        ""}        }};

