// file sfet2conf.h
//
// see tc.c
//
// A.Lebedev Jun-2007...
// A.Basili Aug-2007

#ifndef _SFET2CONF_H
#define _SFET2CONF_H

#include "mylib.h"

#define TDC_IDCODE     0x0011 // These are commands for the jtag
#define TDC_STATUS     0x000A // 
#define TDC_CONTROL    0x0009 //
#define TDC_SETUP      0x0018 //

#define RdWr       0x0001 // These are attributes for the register
#define Rd         0x0010 //
#define Wr         0x0100 //


#define INT_REF    0x0300 // These are for DAC settings
#define EXT_REF    0x8300


typedef struct {
  char *nam;
  int  addr;
  int  val;
  int  len;
  char *fmt;
  int attr;
} _cmd;

_cmd SFET2_setup_table [] = {
  // setup register                                ADDR      VAL LEN  FMT    ATTR

  {"TEST_SELECT",                                  0x200,    0xE,  4, "%x",  RdWr},
  {"ENABLE_ERROR_MARK",                            0x201,      1,  1, "%x",  RdWr},
  {"ENABLE_ERROR_BYPASS",                          0x202,      0,  1, "%x",  RdWr},
  {"ENABLE_ERROR",                                 0x203,  0x7FF, 11, "%x",  RdWr}, 
  {"READOUT_SINGLE_CYCLE_SPEED",                   0x204,      1,  3, "%x",  RdWr},
  {"SERIAL_DELAY",                                 0x205,      0,  4, "%x",  RdWr},
  {"STROBE_SELECT",                                0x206,      3,  2, "%x",  RdWr},
  {"READOUT_SPEED_SELECT",                         0x207,      0,  1, "%x",  RdWr},
  {"TOKEN_DELAY",                                  0x208,      0,  4, "%x",  RdWr},
  {"ENABLE_LOCAL_TRAILER",                         0x209,      1,  1, "%x",  RdWr},
  {"ENABLE_LOCAL_HEADER",                          0x20A,      1,  1, "%x",  RdWr},
  {"ENABLE_GLOBAL_TRAILER",                        0x20B,      0,  1, "%x",  RdWr}, 
  {"ENABLE_GLOBAL_HEADER",                         0x20C,      0,  1, "%x",  RdWr},
  {"KEEP_TOKEN",                                   0x20D,      1,  1, "%x",  RdWr},
  {"MASTER",                                       0x20E,      0,  1, "%x",  RdWr},
  {"ENABLE_BYTEWISE",                              0x20F,      0,  1, "%x",  RdWr},
  {"ENABLE_SERIAL",                                0x210,      1,  1, "%x",  RdWr},
  {"ENABLE_JTAG_READOUT",                          0x211,      0,  1, "%x",  RdWr},
  {"TDC_ID",                                       0x212,      0,  4, "%x",  RdWr},
  {"SELECT_BYPASS_INPUTS",                         0x213,      0,  1, "%x",  RdWr},
  {"READOUT_FIFO_SIZE",                            0x214,      7,  3, "%x",  RdWr},
  {"REJECT_COUNT_OFFSET",                          0x215,      0, 12, "%d",  RdWr},
  {"SEARCH_WINDOW",                                0x216,      0, 12, "%x",  RdWr},
  {"MATCH_WINDOW",                                 0x217,  0x100, 12, "%x",  RdWr},
  {"LEADING_RESOLUTION",                           0x218,      0,  3, "%x",  RdWr},
  {"FIXED_PATTERNH",                               0x219, 0x0ABC, 14, "%x",  RdWr},
  {"FIXED_PATTERNL",                               0x21A, 0x2DEF, 14, "%x",  RdWr},
  {"ENABLE_FIXED_PATTERN",                         0x21B,      1,  1, "%x",  RdWr},
  {"MAX_EVENT_SIZE",                               0x21C,      3,  4, "%x",  RdWr},
  {"REJECT_READOUT_FIFO_FULL",                     0x21D,      1,  1, "%x",  RdWr},
  {"ENABLE_READOUT_OCCUPANCY",                     0x21E,      0,  1, "%x",  RdWr},
  {"ENABLE_READOUT_SEPARATOR",                     0x21F,      0,  1, "%x",  RdWr},
  {"ENABLE_OVERFLOW_DETECT",                       0x220,      1,  1, "%x",  RdWr},
  {"ENABLE_RELATIVE",                              0x221,      0,  1, "%x",  RdWr},
  {"ENABLE_AUTOMATIC_REJECT",                      0x222,      1,  1, "%x",  RdWr},
  {"EVENT_COUNT_OFFSET",                           0x223,   0x80, 12, "%d",  RdWr},
  {"TRIGGER_COUNT_OFFSET",                         0x224,      8, 12, "%x",  RdWr},
  {"ENABLE_SET_COUNTERS_ON_BUNCH_RESET",           0x225,      1,  1, "%x",  RdWr},
  {"ENABLE_MASTER_RESET_CODE",                     0x226,      1,  1, "%x",  RdWr},
  {"ENABLE_MASTER_RESET_ON_EVENT_RESET",           0x227,      0,  1, "%x",  RdWr},
  {"ENABLE_RESET_CHANNEL_BUFFER_WHEN_SEPARATOR",   0x228,      0,  1, "%x",  RdWr},
  {"ENABLE_SEPARATOR_ON_EVENT_RESET",              0x229,      0,  1, "%x",  RdWr},
  {"ENABLE_SEPARATOR_ON_BUNCH_RESET",              0x22A,      0,  1, "%x",  RdWr},
  {"ENABLE_DIRECT_EVENT_RESET",                    0x22B,      1,  1, "%x",  RdWr},
  {"ENABLE_DIRECT_BUNCH_RESET",                    0x22C,      1,  1, "%x",  RdWr},
  {"ENABLE_DIRECT_TRIGGER",                        0x22D,      1,  1, "%x",  RdWr},
  {"OFFSET31",                                     0x22E,      0,  9, "%d",  RdWr},
  {"OFFSET30",                                     0x22F,      0,  9, "%d",  RdWr},
  {"OFFSET29",                                     0x230,      0,  9, "%d",  RdWr},
  {"OFFSET28",                                     0x231,      0,  9, "%d",  RdWr},
  {"OFFSET27",                                     0x232,      0,  9, "%d",  RdWr},
  {"OFFSET26",                                     0x233,      0,  9, "%d",  RdWr},
  {"OFFSET25",                                     0x234,      0,  9, "%d",  RdWr},
  {"OFFSET24",                                     0x235,      0,  9, "%d",  RdWr},
  {"OFFSET23",                                     0x236,      0,  9, "%d",  RdWr},
  {"OFFSET22",                                     0x237,      0,  9, "%d",  RdWr},
  {"OFFSET21",                                     0x238,      0,  9, "%d",  RdWr},
  {"OFFSET20",                                     0x239,      0,  9, "%d",  RdWr},
  {"OFFSET19",                                     0x23A,      0,  9, "%d",  RdWr},
  {"OFFSET18",                                     0x23B,      0,  9, "%d",  RdWr},
  {"OFFSET17",                                     0x23C,      0,  9, "%d",  RdWr},
  {"OFFSET16",                                     0x23D,      0,  9, "%d",  RdWr},
  {"OFFSET15",                                     0x23E,      0,  9, "%d",  RdWr},
  {"OFFSET14",                                     0x23F,      0,  9, "%d",  RdWr},
  {"OFFSET13",                                     0x240,      0,  9, "%d",  RdWr},
  {"OFFSET12",                                     0x241,      0,  9, "%d",  RdWr},
  {"OFFSET11",                                     0x242,      0,  9, "%d",  RdWr},
  {"OFFSET10",                                     0x243,      0,  9, "%d",  RdWr},
  {"OFFSET9",                                      0x244,      0,  9, "%d",  RdWr},
  {"OFFSET8",                                      0x245,      0,  9, "%d",  RdWr},
  {"OFFSET7",                                      0x246,      0,  9, "%d",  RdWr},
  {"OFFSET6",                                      0x247,      0,  9, "%d",  RdWr},
  {"OFFSET5",                                      0x248,      0,  9, "%d",  RdWr},
  {"OFFSET4",                                      0x249,      0,  9, "%d",  RdWr},
  {"OFFSET3",                                      0x24A,      0,  9, "%d",  RdWr},
  {"OFFSET2",                                      0x24B,      0,  9, "%d",  RdWr},
  {"OFFSET1",                                      0x24C,      0,  9, "%d",  RdWr},
  {"OFFSET0",                                      0x24D,      0,  9, "%d",  RdWr},
  {"COARSE_COUNT_OFFSET",                          0x24E,      0, 12, "%d",  RdWr},
  {"DLL_TAP31_ADJUST",                             0x24F,      0,  3, "%d",  RdWr},
  {"DLL_TAP30_ADJUST",                             0x250,      0,  3, "%d",  RdWr},
  {"DLL_TAP29_ADJUST",                             0x251,      0,  3, "%d",  RdWr},
  {"DLL_TAP28_ADJUST",                             0x252,      0,  3, "%d",  RdWr},
  {"DLL_TAP27_ADJUST",                             0x253,      0,  3, "%d",  RdWr},
  {"DLL_TAP26_ADJUST",                             0x254,      0,  3, "%d",  RdWr},
  {"DLL_TAP25_ADJUST",                             0x255,      0,  3, "%d",  RdWr},
  {"DLL_TAP24_ADJUST",                             0x256,      0,  3, "%d",  RdWr},
  {"DLL_TAP23_ADJUST",                             0x257,      0,  3, "%d",  RdWr},
  {"DLL_TAP22_ADJUST",                             0x258,      0,  3, "%d",  RdWr},
  {"DLL_TAP21_ADJUST",                             0x259,      0,  3, "%d",  RdWr},
  {"DLL_TAP20_ADJUST",                             0x25A,      0,  3, "%d",  RdWr},
  {"DLL_TAP19_ADJUST",                             0x25B,      0,  3, "%d",  RdWr},
  {"DLL_TAP18_ADJUST",                             0x25C,      0,  3, "%d",  RdWr},
  {"DLL_TAP17_ADJUST",                             0x25D,      0,  3, "%d",  RdWr},
  {"DLL_TAP16_ADJUST",                             0x25E,      0,  3, "%d",  RdWr},
  {"DLL_TAP15_ADJUST",                             0x25F,      0,  3, "%d",  RdWr},
  {"DLL_TAP14_ADJUST",                             0x260,      0,  3, "%d",  RdWr},
  {"DLL_TAP13_ADJUST",                             0x261,      0,  3, "%d",  RdWr},
  {"DLL_TAP12_ADJUST",                             0x262,      0,  3, "%d",  RdWr},
  {"DLL_TAP11_ADJUST",                             0x263,      0,  3, "%d",  RdWr},
  {"DLL_TAP1_ADJUST",                              0x264,      0,  3, "%d",  RdWr},
  {"DLL_TAP9_ADJUST",                              0x265,      0,  3, "%d",  RdWr},
  {"DLL_TAP8_ADJUST",                              0x266,      0,  3, "%d",  RdWr},
  {"DLL_TAP7_ADJUST",                              0x267,      0,  3, "%d",  RdWr},
  {"DLL_TAP6_ADJUST",                              0x268,      0,  3, "%d",  RdWr},
  {"DLL_TAP5_ADJUST",                              0x269,      0,  3, "%d",  RdWr},
  {"DLL_TAP4_ADJUST",                              0x26A,      0,  3, "%d",  RdWr},
  {"DLL_TAP3_ADJUST",                              0x26B,      0,  3, "%d",  RdWr},
  {"DLL_TAP2_ADJUST",                              0x26C,      0,  3, "%d",  RdWr},
  {"DLL_TAP1_ADJUST",                              0x26D,      0,  3, "%d",  RdWr},
  {"DLL_TAP0_ADJUST",                              0x26E,      0,  3, "%d",  RdWr},
  {"RC_ADJUST3",                                   0x26F,      0,  3, "%d",  RdWr},
  {"RC_ADJUST2",                                   0x270,      0,  3, "%d",  RdWr},
  {"RC_ADJUST1",                                   0x271,      0,  3, "%d",  RdWr},
  {"RC_ADJUST0",                                   0x272,      0,  3, "%d",  RdWr},
  {"RC_ADJUST_NU",                                 0x273,      0,  3, "%x",  RdWr},
  {"LOW_POWER_MODE",                               0x274,      0,  1, "%x",  RdWr},
  {"WIDTH_SELECT",                                 0x275,      0,  4, "%x",  RdWr},
  {"VERNIER_OFFSET",                               0x276,      0,  5, "%x",  RdWr},
  {"DLL_CONTROL",                                  0x277,      1,  4, "%x",  RdWr},
  {"DEAD_TIME",                                    0x278,      0,  2, "%x",  RdWr},
  {"TEST_INVERT",                                  0x279,      0,  1, "%x",  RdWr},
  {"TEST_MODE",                                    0x27A,      0,  1, "%x",  RdWr},
  {"ENABLE_TRAILING",                              0x27B,      0,  1, "%x",  RdWr},
  {"ENABLE_LEADING",                               0x27C,      1,  1, "%x",  RdWr},
  {"MODE_RC_COMPRESSION",                          0x27D,      0,  1, "%x",  RdWr},
  {"MODE_RC",                                      0x27E,      0,  1, "%x",  RdWr},
  {"DLL_MODE",                                     0x27F,      2,  2, "%x",  RdWr},
  {"PLL_CONTROL",                                  0x280,      4,  8, "%x",  RdWr},
  {"SERIAL_CLOCK_DELAY",                           0x281,      0,  4, "%x",  RdWr},
  {"IO_CLOCK_DELAY",                               0x282,      0,  4, "%x",  RdWr},
  {"CORE_CLOCK_DELAY",                             0x283,      0,  4, "%x",  RdWr},
  {"DLL_CLOCK_DELAY",                              0x284,      0,  4, "%x",  RdWr},
  {"SERIAL_CLOCK_SOURCE",                          0x285,      0,  2, "%x",  RdWr},
  {"IO_CLOCK_SOURCE",                              0x286,      0,  2, "%x",  RdWr},
  {"CORE_CLOCK_SOURCE",                            0x287,      0,  2, "%x",  RdWr},
  {"DLL_CLOCK_SOURCE",                             0x288,      3,  3, "%x",  RdWr},
  {"ROLL_OVER",                                    0x289,  0xFFF, 12, "%x",  RdWr},
  {"ENABLE_MATCHING",                              0x28A,      1,  1, "%x",  RdWr},
  {"ENABLE_PAIR",                                  0x28B,      0,  1, "%x",  RdWr},
  {"ENABLE_TTL_SERIAL",                            0x28C,      1,  1, "%x",  RdWr},
  {"ENABLE_TTL_CONTROL",                           0x28D,      1,  1, "%x",  RdWr},
  {"ENABLE_TTL_RESET",                             0x28E,      1,  1, "%x",  RdWr},
  {"ENABLE_TTL_CLOCK",                             0x28F,      1,  1, "%x",  RdWr},
  {"ENABLE_TTL_HIT",                               0x290,      1,  1, "%x",  RdWr},
  {"SETUP_PARITY",                                 0x291,      1,  1, "%x",  RdWr},

};	
					        
_cmd SFET2_control_table [] = {
  // control register                              ADDR      DEF LEN  FMT    ATTR
  {"ENABLE_PATTERN",                               0x300,      4,  4, "%x",  RdWr},
  {"GLOBAL_RESET",                                 0x301,      0,  1, "%x",  RdWr},
  {"ENABLE_CHANNEL_H",                             0x302, 0x0000, 16, "%x",  RdWr},
  {"ENABLE_CHANNEL_L",                             0x303, 0x0000, 16, "%x",  RdWr},
  {"DLL_RESET",                                    0x304,      0,  1, "%x",  RdWr},
  {"PLL_RESET",                                    0x305,      0,  1, "%x",  RdWr},
  {"CONTROL_PARITY",                               0x306,      1,  1, "%x",  RdWr},

};						        

_cmd SFET2_status_table [] = {
  // status register                               ADDR      DEF LEN  FMT    ATTR
  {"ERRORS",                                       0x500,      0,  4, "%x",    Rd},
  {"HAVE_TOKEN",                                   0x501,      0,  1, "%x",    Rd},
  {"READOUT_FIFO_OCCUPANCY",                       0x502,      0, 16, "%x",    Rd},
  {"READOUT_FIFO_FULL",                            0x503,      0, 16, "%x",    Rd},
  {"READOUT_FIFO_EMPTY",                           0x504,      0,  1, "%x",    Rd},
  {"GROUP3_L1_OCCUPANCY",                          0x505,      0,  8, "%x",    Rd},
  {"GROUP2_L1_OCCUPANCY",                          0x506,      0,  8, "%x",    Rd},
  {"GROUP1_L1_OCCUPANCY",                          0x507,      0,  8, "%x",    Rd},
  {"GROUP0_L1_OCCUPANCY",                          0x508,      0,  8, "%x",    Rd},
  {"TRIGGER_FIFO_OCCUPANCY",                       0x509,      0,  1, "%x",    Rd},
  {"TRIGGER_FIFO_FULL",                            0x50A,      0,  1, "%x",    Rd},
  {"TRIGGER_FIFO_EMPTY",                           0x50B,      0,  1, "%x",    Rd},
  {"DLL_LOCK",                                     0x50C,      0,  1, "%x",    Rd},
  {"NU",                                           0x50D,      0,  1, "%x",    Rd},

};

_cmd SFET2_bsr_table [] = {
  // bsr  register                                 ADDR      DEF LEN  FMT  ATTR
  {"TOKEN_OUT",                                    0x400,      0,  1, "%x",  RdWr},
  {"STROBE_OUT",                                   0x401,      0,  1, "%x",  RdWr},
  {"SERIAL_OUT",                                   0x402,      0,  1, "%x",  RdWr},
  {"TEST",                                         0x403,      0,  1, "%x",  RdWr},
  {"ERROR",                                        0x404,      0,  1, "%x",  RdWr},
  {"DATA_READY",                                   0x405,      0,  1, "%x",  RdWr},
  {"PARALLEL_ENABLE",                              0x406,      0,  1, "%x",  RdWr},
  {"PARALLEL_DATAOUTH",                            0x407,      0, 16, "%x",  RdWr},
  {"PARALLEL_DATAOUTL",                            0x408,      0, 16, "%x",  RdWr},
  {"ENCODED_CONTROL",                              0x409,      0,  1, "%x",  RdWr},
  {"TRIGGER",                                      0x40A,      0,  1, "%x",  RdWr},
  {"EVENT_RESET",                                  0x40B,      0,  1, "%x",  RdWr},
  {"BUNCH_RESET",                                  0x40C,      0,  1, "%x",  RdWr},
  {"GET_DATA",                                     0x40D,      0,  1, "%x",  RdWr},
  {"SERIAL_BYPASS_IN",                             0x40E,      0,  1, "%x",  RdWr},
  {"SERIAL_IN",                                    0x40F,      0,  1, "%x",  RdWr},
  {"TOKEN_BYPASS_IN",                              0x410,      0,  1, "%x",  RdWr},
  {"TOKEN_IN",                                     0x411,      0,  1, "%x",  RdWr},
  {"RESET",                                        0x412,      0,  1, "%x",  RdWr},
  {"AUX_CLOCK",                                    0x413,      0,  1, "%x",  RdWr},
  {"HPTDC_CLK",                                    0x414,      0,  1, "%x",  RdWr},
  {"HITSH",                                        0x415,      0, 16, "%x",  RdWr},
  {"HITSL",                                        0x416,      0, 16, "%x",  RdWr},
};						        

_cmd SFET2_idcode_table [] = {
  // idcode  register                              ADDR      DEF LEN  FMT    ATTR
  {"IDCODE_H",                                     0x600, 0x8470, 16, "%x",    Rd},
  {"IDCODE_L",                                     0x601, 0xDACE, 16, "%x",    Rd},

};						        

_cmd SFET2_dac_table [] = {
  // dac  register                                 ADDR      DEF LEN  FMT    ATTR
  {"LT_DAC_A",                                     0x000,      0, 16, "%x",  RdWr},
  {"LT_DAC_B",                                     0x001,      0, 16, "%x",  RdWr},
  {"HT_DAC_A",                                     0x002,      0, 16, "%x",  RdWr},
  {"HT_DAC_B",                                     0x003,      0, 16, "%x",  RdWr},
  {"SHT_DAC_A",                                    0x004,      0, 16, "%x",  RdWr},
  {"SHT_DAC_B",                                    0x005,      0, 16, "%x",  RdWr},
};		

_cmd SFET2_temp_table [] = {
  // temperature registers                         ADDR      DEF LEN  FMT    ATTR
  {"TEMPERATUREH",                                 0x100,      0, 16, "%x",    Rd},
  {"TEMPERATUREL",                                 0x101,      0, 16, "%x",    Rd},
  {"SC_TEMPERATUREH",                              0x102,      0, 12, "%x",    Rd},
  {"SC_TEMPERATUREL",                              0x103,      0, 12, "%x",    Rd},
};				

_cmd SFET2_event_table [] = {
  // event registers                               ADDR      DEF LEN  FMT    ATTR
  //  {"BLT",                                          0x800,      0,  0, "%x",    Rd},
  {"TDC_RST",                                      0x801,      0,  0, "%x",    Wr},
  {"BUNCH_RST",                                    0x802,      0,  0, "%x",    Wr},
  {"EVENT_RST",                                    0x803,      0,  0, "%x",    Wr},
  {"TDC_TIMEOUT",                                  0x804,      0, 16, "%x",  RdWr},
  //{"TDC_DATAH",                                    0x805,      0, 16, "%x",  RdWr},
  //{"TDC_DATAL",                                    0x806,      0, 16, "%x",  RdWr},
  {"MASK_FT",                                      0x807,      0,  1, "%x",  RdWr},
  {"ENABLE_TDC_CLK",                               0x808,      0,  1, "%x",  RdWr},
  {"TEMP_DUMMYH",                                  0x809,      0, 16, "%x",  RdWr},
  {"TEMP_DUMMYL",                                  0x80A,      0, 16, "%x",  RdWr},
  {"HEAD_DUMMYH",                                  0x80B,      0, 16, "%x",  RdWr},
  {"HEAD_DUMMYL",                                  0x80C,      0, 16, "%x",  RdWr},
  {"MEAS_DUMMYH",                                  0x80D,      0, 16, "%x",  RdWr},
  {"MEAS_DUMMYL",                                  0x80E,      0, 16, "%x",  RdWr},
  {"FOOT_DUMMYH",                                  0x80F,      0, 16, "%x",  RdWr},
  {"FOOT_DUMMYL",                                  0x810,      0, 16, "%x",  RdWr},
  {"ERROR_DUMMYH",                                 0x811,      0, 16, "%x",  RdWr},
  {"ERROR_DUMMYL",                                 0x812,      0, 16, "%x",  RdWr},
  {"NWORDS",                                       0x813,      0,  4, "%x",  RdWr},
  {"EN_ERR",                                       0x814,      0,  1, "%x",  RdWr},
  //  {"BLT_DUMMY",                                    0x820,      0,  0, "%x",    Rd},
  {"TRIGGER_DELAY",                                0x900,      0, 10, "%x",  RdWr},
  {"VERSION_ID",                                   0xFFF, 0x7807, 16, "%x",    Rd},
};						        
		        
_cmd SFET2_op_table [] = {
  // op  register                                  ADDR      DEF LEN  FMT    ATTR
  {"CMD",                                          0x700,      0,  2, "%x",  RdWr},
  {"IR_REG",                                       0x701,      0,  5, "%x",  RdWr},
  {"EN_EXT_JTAG",                                  0x702,      0,  1, "%x",  RdWr},
  {"EXT_JTAG",                                     0x703,      0,  4, "%x",  RdWr},
};		
				        
#endif // _SFET2CONF_H
