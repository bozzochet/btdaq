/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2004 by PADSOFT, Inc.                        */
/*                                                                           */
/*    Developed for use on AMS-2, An International Space Station payload     */
/*                      searching for missing matter                         */
/*                                                                           */
/*                        All rights reserved                                */
/*                                                                           */
/* Developed by:        Peter Armistead Dennett                              */
/*                            PADSOFT, Inc                                   */
/*                          61 Harbor Lane                                   */
/*                        Kemah, Texas 77565                                 */
/*                          (281) 334 3800                                   */
/*                      http://www.padsoft.com                               */
/*                                                                           */
/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/

//  nodes.c   Information about our nodes

//  Revision History
//    Nov 2003 by Peter Dennett
//      - Initial version
//    Jul 2004 by Peter Dennett
//      - Changed from table to callable

#include "aallib.h"
#include "nodes.h"

#if 0
static char *NodeName[512] = 
{
  "JMDC-0-CAN",  // 0x000
  "JMDC-1-CAN",  // 0x001
  "JMDC-2-CAN",  // 0x002
  "JMDC-3-CAN",  // 0x003
  "JMDC-0-EXT",  // 0x004
  "JMDC-1-EXT",  // 0x005
  "JMDC-2-EXT",  // 0x006
  "JMDC-3-EXT",  // 0x007
  "JMDC-0-AMSW", // 0x008
  "JMDC-1-AMSW", // 0x009
  "JMDC-2-AMSW", // 0x00A
  "JMDC-3-AMSW", // 0x00B
  "JMDC-0",      // 0x00C
  "JMDC-1",      // 0x00D
  "JMDC-2",      // 0x00E
  "JMDC-3",      // 0x00F

  "JMDC:HRDL",   // 0x010
  "JMDC:1553",   // 0x011
  "JMDC:DAQ",    // 0x012
  "JMDC:MCC",    // 0x013
  "JMDC-itself", // 0x014
  "DDRS-02",     // 0x015
  "ACOP",        // 0x016
  "JMDC",        // 0x017
  "CAB-A",       // 0x018
  "CAB-B",       // 0x019
  "CAB-P",       // 0x01A
  "CAB-S",       // 0x01B
  "USCM-CCEB-A", // 0x01C
  "USCM-CCEB-B", // 0x01D
  "USCM-CCEB-P", // 0x01E
  "USCM-CCEB-S", // 0x01F

  "-",           // 0x020
  "-",           // 0x021
  "-",           // 0x022
  "-",           // 0x023
  "-",           // 0x024
  "-",           // 0x025
  "-",           // 0x026
  "-",           // 0x027
  "USCM-JPD-A",  // 0x028
  "USCM-JPD-B",  // 0x029
  "USCM-JPD-P",  // 0x02A
  "USCM-JPD-S",  // 0x02B
  "USCM-M-A",    // 0x02C
  "USCM-M-B",    // 0x02D
  "USCM-M-P",    // 0x02E
  "USCM-M-S",    // 0x02F

  "PDS-A",       // 0x030
  "PDS-B",       // 0x031
  "PDS-P",       // 0x032
  "PDS-S",       // 0x033
  "-",           // 0x034
  "-",           // 0x035
  "-",           // 0x036
  "-",           // 0x037
  "-",           // 0x038
  "-",           // 0x039
  "-",           // 0x03A
  "-",           // 0x03B
  "-",           // 0x03C
  "-",           // 0x03D
  "-",           // 0x03E
  "-",           // 0x03F
  
  "-",           // 0x040
  "-",           // 0x041
  "-",           // 0x042
  "-",           // 0x043
  "-",           // 0x044
  "-",           // 0x045
  "-",           // 0x046
  "-",           // 0x047
  "-",           // 0x048
  "-",           // 0x049
  "-",           // 0x04A
  "-",           // 0x04B
  "-",           // 0x04C
  "-",           // 0x04D
  "-",           // 0x04E
  "-",           // 0x04F

  "-",           // 0x050
  "-",           // 0x051
  "-",           // 0x052
  "-",           // 0x053
  "-",           // 0x054
  "-",           // 0x055
  "-",           // 0x056
  "-",           // 0x057
  "-",           // 0x058
  "-",           // 0x059
  "-",           // 0x05A
  "-",           // 0x05B
  "-",           // 0x05C
  "-",           // 0x05D
  "-",           // 0x05E
  "-",           // 0x05F

  "-",           // 0x060
  "-",           // 0x061
  "-",           // 0x062
  "-",           // 0x063
  "-",           // 0x064
  "-",           // 0x065
  "-",           // 0x066
  "-",           // 0x067
  "-",           // 0x068
  "-",           // 0x069
  "-",           // 0x06A
  "-",           // 0x06B
  "TTCE-A",      // 0x06C
  "TTCE-B",      // 0x06D
  "TTCE-P",      // 0x06E
  "TTCE-S",      // 0x06F

  "-",           // 0x070
  "-",           // 0x071
  "-",           // 0x072
  "-",           // 0x073
  "-",           // 0x074
  "-",           // 0x075
  "-",           // 0x076
  "-",           // 0x077
  "USCM-UG-A",   // 0x078
  "USCM-UG-B",   // 0x079
  "USCM-UG-P",   // 0x07A
  "USCM-UG-S",   // 0x07B
  "-",           // 0x07C
  "-",           // 0x07D
  "-",           // 0x07E
  "-",           // 0x07F

  "JINJ-0",      // 0x080
  "JINJ-1",      // 0x081
  "JINJ-2",      // 0x082
  "JINJ-3",      // 0x083
  "JINJ-P-A",    // 0x084
  "JINJ-P-B",    // 0x085
  "JINJ-S-A",    // 0x086
  "JINJ-S-B",    // 0x087
  "JLV1-A",      // 0x088
  "JLV1-B",      // 0x089
  "JLV1-P",      // 0x08A
  "JLV1-S",      // 0x08B
  "ETRG-0-A",    // 0x08C
  "ETRG-0-B",    // 0x08D
  "ETRG-0-P",    // 0x08E
  "ETRG-0-S",    // 0x08F

  "ETRG-1-A",    // 0x090
  "ETRG-1-B",    // 0x091
  "ETRG-1-P",    // 0x092
  "ETRG-1-S",    // 0x093
  "-",           // 0x094
  "-",           // 0x095
  "JINF-E-0-A",  // 0x096
  "JINF-E-0-B",  // 0x097
  "JINF-E-0-P",  // 0x098
  "JINF-E-0-S",  // 0x099
  "JINF-E-1-A",  // 0x09A
  "JINF-E-1-B",  // 0x09B
  "JINF-E-1-P",  // 0x09C
  "JINF-E-1-S",  // 0x09D
  "JINF-R-0-A",  // 0x09E
  "JINF-R-0-B",  // 0x09F

  "JINF-R-0-P",  // 0x0A0
  "JINF-R-0-S",  // 0x0A1
  "JINF-R-1-A",  // 0x0A2
  "JINF-R-1-B",  // 0x0A3
  "JINF-R-1-P",  // 0x0A4
  "JINF-R-1-S",  // 0x0A5
  "JINF-T-0-A",  // 0x0A6
  "JINF-T-0-B",  // 0x0A7
  "JINF-T-0-P",  // 0x0A8
  "JINF-T-0-S",  // 0x0A9
  "JINF-T-1-A",  // 0x0AA
  "JINF-T-1-B",  // 0x0AB
  "JINF-T-1-P",  // 0x0AC
  "JINF-T-1-S",  // 0x0AD
  "JINF-T-2-A",  // 0x0AE
  "JINF-T-2-B",  // 0x0AF

  "JINF-T-2-P",  // 0x0B0
  "JINF-T-2-S",  // 0x0B1
  "JINF-T-3-A",  // 0x0B2
  "JINF-T-3-B",  // 0x0B3
  "JINF-T-3-P",  // 0x0B4
  "JINF-T-3-S",  // 0x0B5
  "JINF-T-4-A",  // 0x0B6
  "JINF-T-4-B",  // 0x0B7
  "JINF-T-4-P",  // 0x0B8
  "JINF-T-4-S",  // 0x0B9
  "JINF-T-5-A",  // 0x0BA
  "JINF-T-5-B",  // 0x0BB
  "JINF-T-5-P",  // 0x0BC
  "JINF-T-5-S",  // 0x0BD
  "JINF-T-6-A",  // 0x0BE
  "JINF-T-6-B",  // 0x0BF

  "JINF-T-6-P",  // 0x0C0
  "JINF-T-6-S",  // 0x0C1
  "JINF-T-7-A",  // 0x0C2
  "JINF-T-7-B",  // 0x0C3
  "JINF-T-7-P",  // 0x0C4
  "JINF-T-7-S",  // 0x0C5
  "JINF-U-0-A",  // 0x0C6
  "JINF-U-0-B",  // 0x0C7
  "JINF-U-0-P",  // 0x0C8
  "JINF-U-0-S",  // 0x0C9
  "JINF-U-1-A",  // 0x0CA
  "JINF-U-1-B",  // 0x0CB
  "JINF-U-1-P",  // 0x0CC
  "JINF-U-1-S",  // 0x0CD
  "EDR-0-0-A",   // 0x0CE
  "EDR-0-0-B",   // 0x0CF

  "EDR-0-0-P",   // 0x0D0
  "EDR-0-1-A",   // 0x0D1
  "EDR-0-1-B",   // 0x0D2
  "EDR-0-1-P",   // 0x0D3
  "EDR-0-2-A",   // 0x0D4
  "EDR-0-2-B",   // 0x0D5
  "EDR-0-2-P",   // 0x0D6
  "EDR-0-3-A",   // 0x0D7
  "EDR-0-3-B",   // 0x0D8
  "EDR-0-3-P",   // 0x0D9
  "EDR-0-4-A",   // 0x0DA
  "EDR-0-4-B",   // 0x0DB
  "EDR-0-4-P",   // 0x0DC
  "EDR-0-5-A",   // 0x0DD
  "EDR-0-5-B",   // 0x0DE
  "EDR-0-5-P",   // 0x0DF

  "EDR-1-0-A",   // 0x0E0
  "EDR-1-0-B",   // 0x0E1
  "EDR-1-0-P",   // 0x0E2
  "EDR-1-1-A",   // 0x0E3
  "EDR-1-1-B",   // 0x0E4
  "EDR-1-1-P",   // 0x0E5
  "EDR-1-2-A",   // 0x0E6
  "EDR-1-2-B",   // 0x0E7
  "EDR-1-2-P",   // 0x0E8
  "EDR-1-3-A",   // 0x0E9
  "EDR-1-3-B",   // 0x0EA
  "EDR-1-3-P",   // 0x0EB
  "EDR-1-4-A",   // 0x0EC
  "EDR-1-4-B",   // 0x0ED
  "EDR-1-4-P",   // 0x0EE
  "EDR-1-5-A",   // 0x0EF

  "EDR-1-5-B",   // 0x0F0
  "EDR-1-5-P",   // 0x0F1
  "RDR-0-00",    // 0x0F2
  "RDR-0-01",    // 0x0F3
  "RDR-0-02",    // 0x0F4
  "RDR-0-03",    // 0x0F5
  "RDR-0-04",    // 0x0F6
  "RDR-0-05",    // 0x0F7
  "RDR-0-06",    // 0x0F8
  "RDR-0-07",    // 0x0F9
  "RDR-0-08",    // 0x0FA
  "RDR-0-09",    // 0x0FB
  "RDR-0-10",    // 0x0FC
  "RDR-0-11",    // 0x0FD
  "RDR-1-00",    // 0x0FE
  "RDR-1-01",    // 0x0FF

  "RDR-1-02",    // 0x100
  "RDR-1-03",    // 0x101
  "RDR-1-04",    // 0x102
  "RDR-1-05",    // 0x103
  "RDR-1-06",    // 0x104
  "RDR-1-07",    // 0x105
  "RDR-1-08",    // 0x106
  "RDR-1-09",    // 0x107
  "RDR-1-10",    // 0x108
  "RDR-1-11",    // 0x109
  "SDR2-0-A",    // 0x10A
  "SDR2-0-B",    // 0x10B
  "SDR2-0-P",    // 0x10C
  "SDR2-0-S",    // 0x10D
  "SDR2-1-A",    // 0x10E
  "SDR2-1-B",    // 0x10F

  "SDR2-1-P",    // 0x110
  "SDR2-1-S",    // 0x111
  "SDR2-2-A",    // 0x112
  "SDR2-2-B",    // 0x113
  "SDR2-2-P",    // 0x114
  "SDR2-2-S",    // 0x115
  "SDR2-3-A",    // 0x116
  "SDR2-3-B",    // 0x117
  "SDR2-3-P",    // 0x118
  "SDR2-3-S",    // 0x119
  "TDR-0-00-A",  // 0x11A
  "TDR-0-00-B",  // 0x11B
  "TDR-0-01-A",  // 0x11C
  "TDR-0-01-B",  // 0x11D
  "TDR-0-02-A",  // 0x11E
  "TDR-0-02-B",  // 0x11F

  "TDR-0-03-A",  // 0x120
  "TDR-0-03-B",  // 0x121
  "TDR-0-04-A",  // 0x122
  "TDR-0-04-B",  // 0x123
  "TDR-0-05-A",  // 0x124
  "TDR-0-05-B",  // 0x125
  "TDR-0-06-A",  // 0x126
  "TDR-0-06-B",  // 0x127
  "TDR-0-07-A",  // 0x128
  "TDR-0-07-B",  // 0x129
  "TDR-0-08-A",  // 0x12A
  "TDR-0-08-B",  // 0x12B
  "TDR-0-09-A",  // 0x12C
  "TDR-0-09-B",  // 0x12D
  "TDR-0-10-A",  // 0x12E
  "TDR-0-10-B",  // 0x12F

  "TDR-0-11-A",  // 0x130
  "TDR-0-11-B",  // 0x131
  "TDR-1-00-A",  // 0x132
  "TDR-1-00-B",  // 0x133
  "TDR-1-01-A",  // 0x134
  "TDR-1-01-B",  // 0x135
  "TDR-1-02-A",  // 0x136
  "TDR-1-02-B",  // 0x137
  "TDR-1-03-A",  // 0x138
  "TDR-1-03-B",  // 0x139
  "TDR-1-04-A",  // 0x13A
  "TDR-1-04-B",  // 0x13B
  "TDR-1-05-A",  // 0x13C
  "TDR-1-05-B",  // 0x13D
  "TDR-1-06-A",  // 0x13E
  "TDR-1-06-B",  // 0x13F

  "TDR-1-07-A",  // 0x140
  "TDR-1-07-B",  // 0x141
  "TDR-1-08-A",  // 0x142
  "TDR-1-08-B",  // 0x143
  "TDR-1-09-A",  // 0x144
  "TDR-1-09-B",  // 0x145
  "TDR-1-10-A",  // 0x146
  "TDR-1-10-B",  // 0x147
  "TDR-1-11-A",  // 0x148
  "TDR-1-11-B",  // 0x149
  "TDR-2-00-A",  // 0x14A
  "TDR-2-00-B",  // 0x14B
  "TDR-2-01-A",  // 0x14C
  "TDR-2-01-B",  // 0x14D
  "TDR-2-02-A",  // 0x14E
  "TDR-2-02-B",  // 0x14F

  "TDR-2-03-A",  // 0x150
  "TDR-2-03-B",  // 0x151
  "TDR-2-04-A",  // 0x152
  "TDR-2-04-B",  // 0x153
  "TDR-2-05-A",  // 0x154
  "TDR-2-05-B",  // 0x155
  "TDR-2-06-A",  // 0x156
  "TDR-2-06-B",  // 0x157
  "TDR-2-07-A",  // 0x158
  "TDR-2-07-B",  // 0x159
  "TDR-2-08-A",  // 0x15A
  "TDR-2-08-B",  // 0x15B
  "TDR-2-09-A",  // 0x15C
  "TDR-2-09-B",  // 0x15D
  "TDR-2-10-A",  // 0x15E
  "TDR-2-10-B",  // 0x15F
  
  "TDR-2-11-A",  // 0x160
  "TDR-2-11-B",  // 0x161
  "TDR-3-00-A",  // 0x162
  "TDR-3-00-B",  // 0x163
  "TDR-3-01-A",  // 0x164
  "TDR-3-01-B",  // 0x165
  "TDR-3-02-A",  // 0x166
  "TDR-3-02-B",  // 0x167
  "TDR-3-03-A",  // 0x168
  "TDR-3-03-B",  // 0x169
  "TDR-3-04-A",  // 0x16A
  "TDR-3-04-B",  // 0x16B
  "TDR-3-05-A",  // 0x16C
  "TDR-3-05-B",  // 0x16D
  "TDR-3-06-A",  // 0x16E
  "TDR-3-06-B",  // 0x16F
  
  "TDR-3-07-A",  // 0x170
  "TDR-3-07-B",  // 0x171
  "TDR-3-08-A",  // 0x172
  "TDR-3-08-B",  // 0x173
  "TDR-3-09-A",  // 0x174
  "TDR-3-09-B",  // 0x175
  "TDR-3-10-A",  // 0x176
  "TDR-3-10-B",  // 0x177
  "TDR-3-11-A",  // 0x178
  "TDR-3-11-B",  // 0x179
  "TDR-4-00-A",  // 0x17A
  "TDR-4-00-B",  // 0x17B
  "TDR-4-01-A",  // 0x17C
  "TDR-4-01-B",  // 0x17D
  "TDR-4-02-A",  // 0x17E
  "TDR-4-02-B",  // 0x17F
  
  "TDR-4-03-A",  // 0x180
  "TDR-4-03-B",  // 0x181
  "TDR-4-04-A",  // 0x182
  "TDR-4-04-B",  // 0x183
  "TDR-4-05-A",  // 0x184
  "TDR-4-05-B",  // 0x185
  "TDR-4-06-A",  // 0x186
  "TDR-4-06-B",  // 0x187
  "TDR-4-07-A",  // 0x188
  "TDR-4-07-B",  // 0x189
  "TDR-4-08-A",  // 0x18A
  "TDR-4-08-B",  // 0x18B
  "TDR-4-09-A",  // 0x18C
  "TDR-4-09-B",  // 0x18D
  "TDR-4-10-A",  // 0x18E
  "TDR-4-10-B",  // 0x18F
  
  "TDR-4-11-A",  // 0x190
  "TDR-4-11-B",  // 0x191
  "TDR-5-00-A",  // 0x192
  "TDR-5-00-B",  // 0x193
  "TDR-5-01-A",  // 0x194
  "TDR-5-01-B",  // 0x195
  "TDR-5-02-A",  // 0x196
  "TDR-5-02-B",  // 0x197
  "TDR-5-03-A",  // 0x198
  "TDR-5-03-B",  // 0x199
  "TDR-5-04-A",  // 0x19A
  "TDR-5-04-B",  // 0x19B
  "TDR-5-05-A",  // 0x19C
  "TDR-5-05-B",  // 0x19D
  "TDR-5-06-A",  // 0x19E
  "TDR-5-06-B",  // 0x19F
  
  "TDR-5-07-A",  // 0x1A0
  "TDR-5-07-B",  // 0x1A1
  "TDR-5-08-A",  // 0x1A2
  "TDR-5-08-B",  // 0x1A3
  "TDR-5-09-A",  // 0x1A4
  "TDR-5-09-B",  // 0x1A5
  "TDR-5-10-A",  // 0x1A6
  "TDR-5-10-B",  // 0x1A7
  "TDR-5-11-A",  // 0x1A8
  "TDR-5-11-B",  // 0x1A9
  "TDR-6-00-A",  // 0x1AA
  "TDR-6-00-B",  // 0x1AB
  "TDR-6-01-A",  // 0x1AC
  "TDR-6-01-B",  // 0x1AD
  "TDR-6-02-A",  // 0x1AE
  "TDR-6-02-B",  // 0x1AF
  
  "TDR-6-03-A",  // 0x1B0
  "TDR-6-03-B",  // 0x1B1
  "TDR-6-04-A",  // 0x1B2
  "TDR-6-04-B",  // 0x1B3
  "TDR-6-05-A",  // 0x1B4
  "TDR-6-05-B",  // 0x1B5
  "TDR-6-06-A",  // 0x1B6
  "TDR-6-06-B",  // 0x1B7
  "TDR-6-07-A",  // 0x1B8
  "TDR-6-07-B",  // 0x1B9
  "TDR-6-08-A",  // 0x1BA
  "TDR-6-08-B",  // 0x1BB
  "TDR-6-09-A",  // 0x1BC
  "TDR-6-09-B",  // 0x1BD
  "TDR-6-10-A",  // 0x1BE
  "TDR-6-10-B",  // 0x1BF
  
  "TDR-6-11-A",  // 0x1C0
  "TDR-6-11-B",  // 0x1C1
  "TDR-7-00-A",  // 0x1C2
  "TDR-7-00-B",  // 0x1C3
  "TDR-7-01-A",  // 0x1C4
  "TDR-7-01-B",  // 0x1C5
  "TDR-7-02-A",  // 0x1C6
  "TDR-7-02-B",  // 0x1C7
  "TDR-7-03-A",  // 0x1C8
  "TDR-7-03-B",  // 0x1C9
  "TDR-7-04-A",  // 0x1CA
  "TDR-7-04-B",  // 0x1CB
  "TDR-7-05-A",  // 0x1CC
  "TDR-7-05-B",  // 0x1CD
  "TDR-7-06-A",  // 0x1CE
  "TDR-7-06-B",  // 0x1CF
  
  "TDR-7-07-A",  // 0x1D0
  "TDR-7-07-B",  // 0x1D1
  "TDR-7-08-A",  // 0x1D2
  "TDR-7-08-B",  // 0x1D3
  "TDR-7-09-A",  // 0x1D4
  "TDR-7-09-B",  // 0x1D5
  "TDR-7-10-A",  // 0x1D6
  "TDR-7-10-B",  // 0x1D7
  "TDR-7-11-A",  // 0x1D8
  "TDR-7-11-B",  // 0x1D9
  "UDR-0-0-A",   // 0x1DA
  "UDR-0-0-B",   // 0x1DB
  "UDR-0-0-P",   // 0x1DC
  "UDR-0-1-A",   // 0x1DD
  "UDR-0-1-B",   // 0x1DE
  "UDR-0-1-P",   // 0x1DF
  
  "UDR-0-2-A",   // 0x1E0
  "UDR-0-2-B",   // 0x1E1
  "UDR-0-2-P",   // 0x1E2
  "UDR-0-3-A",   // 0x1E3
  "UDR-0-3-B",   // 0x1E4
  "UDR-0-3-P",   // 0x1E5
  "UDR-0-4-A",   // 0x1E6
  "UDR-0-4-B",   // 0x1E7
  "UDR-0-4-P",   // 0x1E8
  "UDR-0-5-A",   // 0x1E9
  "UDR-0-5-B",   // 0x1EA
  "UDR-0-5-P",   // 0x1EB
  "UDR-1-0-A",   // 0x1EC
  "UDR-1-0-B",   // 0x1ED
  "UDR-1-0-P",   // 0x1EE
  "UDR-1-1-A",   // 0x1EF
  
  "UDR-1-1-B",   // 0x1F0
  "UDR-1-1-P",   // 0x1F1
  "UDR-1-2-A",   // 0x1F2
  "UDR-1-2-B",   // 0x1F3
  "UDR-1-2-P",   // 0x1F4
  "UDR-1-3-A",   // 0x1F5
  "UDR-1-3-B",   // 0x1F6
  "UDR-1-3-P",   // 0x1F7
  "UDR-1-4-A",   // 0x1F8
  "UDR-1-4-B",   // 0x1F9
  "UDR-1-4-P",   // 0x1FA
  "UDR-1-5-A",   // 0x1FB
  "UDR-1-5-B",   // 0x1FC
  "UDR-1-5-P",   // 0x1FD
  "-",           // 0x1FE
  "Server/FEP"   // 0x1FF
};
#endif

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

//  Define the phone list of nodes

NodeEntry NodeNames[] =
{
  //  JMDC Addresses 0x00 - 0x17

  { Name: "JMDC-0-CAN",    Group: nodesGroupCAN},  // 0000  0x000
  { Name: "JMDC-1-CAN",    Group: nodesGroupCAN},  // 0001  0x001
  { Name: "JMDC-2-CAN",    Group: nodesGroupCAN},  // 0002  0x002
  { Name: "JMDC-3-CAN",    Group: nodesGroupCAN},  // 0003  0x003
  { Name: "JMDC-0-EXT",    Group: nodesGroupEXTERN}, // 0004  0x004
  { Name: "JMDC-1-EXT",    Group: nodesGroupEXTERN}, // 0005  0x005
  { Name: "JMDC-2-EXT",    Group: nodesGroupEXTERN}, // 0006  0x006
  { Name: "JMDC-3-EXT",    Group: nodesGroupEXTERN}, // 0007  0x007
  { Name: "JMDC-0-AMSW",   Group: nodesGroupAMSW}, // 0008  0x008
  { Name: "JMDC-1-AMSW",   Group: nodesGroupAMSW}, // 0009  0x009
  { Name: "JMDC-2-AMSW",   Group: nodesGroupAMSW}, // 0010  0x00A
  { Name: "JMDC-3-AMSW",   Group: nodesGroupAMSW}, // 0011  0x00B
  { Name: "JMDC0",         Group: nodesGroupEXTERN}, // 0012  0x00C
  { Name: "JMDC1",         Group: nodesGroupEXTERN}, // 0013  0x00D
  { Name: "JMDC2",         Group: nodesGroupEXTERN}, // 0014  0x00E
  { Name: "JMDC3",         Group: nodesGroupEXTERN}, // 0015  0x00F
  { Name: "OWNER-HRDL",    Group: nodesGroupFUNC}, // 0016  0x010
  { Name: "OWNER-1553",    Group: nodesGroupFUNC}, // 0017  0x011
  { Name: "OWNER-DAQ",     Group: nodesGroupFUNC}, // 0018  0x012
  { Name: "OWNER-MCC",     Group: nodesGroupFUNC}, // 0019  0x013
  { Name: "OWNER_UDP",     Group: nodesGroupFUNC}, // 0020  0x014
  { Name: "NA 0x15",       Group: nodesGroupUNASG}, // 0021  0x015
  { Name: "ACOP",          Group: nodesGroupHRDL}, // 0022  0x016
  { Name: "OWNER-BC",      Group: nodesGroupFUNC}, // 0023  0x017

  //  Slow control addresses 0x18 - 0x7F

  { Name: "CAB-A",         Group: nodesGroupCAN},  // 0024  0x018
  { Name: "CAB-B",         Group: nodesGroupCAN},  // 0025  0x019
  { Name: "CAB-P",         Group: nodesGroupCAN},  // 0026  0x01A
  { Name: "CAB-S",         Group: nodesGroupCAN},  // 0027  0x01B
  { Name: "USCM-CCEB-A",   Group: nodesGroupCAN},  // 0028  0x01C
  { Name: "USCM-CCEB-B",   Group: nodesGroupCAN},  // 0029  0x01D
  { Name: "USCM-CCEB-P",   Group: nodesGroupCAN},  // 0030  0x01E
  { Name: "USCM-CCEB-S",   Group: nodesGroupCAN},  // 0031  0x01F
  { Name: "USCM-E-0-A",    Group: nodesGroupCAN},  // 0032  0x020
  { Name: "USCM-E-0-B",    Group: nodesGroupCAN},  // 0033  0x021
  { Name: "USCM-E-0-P",    Group: nodesGroupCAN},  // 0034  0x022
  { Name: "USCM-E-0-S",    Group: nodesGroupCAN},  // 0035  0x023
  { Name: "USCM-E-1-A",    Group: nodesGroupCAN},  // 0036  0x024
  { Name: "USCM-E-1-B",    Group: nodesGroupCAN},  // 0037  0x025
  { Name: "USCM-E-1-P",    Group: nodesGroupCAN},  // 0038  0x026
  { Name: "USCM-E-1-S",    Group: nodesGroupCAN},  // 0039  0x027
  { Name: "USCM-JPD-A",    Group: nodesGroupCAN},  // 0040  0x028
  { Name: "USCM-JPD-B",    Group: nodesGroupCAN},  // 0041  0x029
  { Name: "USCM-JPD-P",    Group: nodesGroupCAN},  // 0042  0x02A
  { Name: "USCM-JPD-S",    Group: nodesGroupCAN},  // 0043  0x02B
  { Name: "USCM-M-A",      Group: nodesGroupCAN},  // 0044  0x02C
  { Name: "USCM-M-B",      Group: nodesGroupCAN},  // 0045  0x02D
  { Name: "USCM-M-P",      Group: nodesGroupCAN},  // 0046  0x02E
  { Name: "USCM-M-S",      Group: nodesGroupCAN},  // 0047  0x02F
  { Name: "PDB-A",         Group: nodesGroupCAN},  // 0048  0x030
  { Name: "PDB-B",         Group: nodesGroupCAN},  // 0049  0x031
  { Name: "PDB-P",         Group: nodesGroupCAN},  // 0050  0x032
  { Name: "PDB-S",         Group: nodesGroupCAN},  // 0051  0x033
  { Name: "USCM-R-0-A",    Group: nodesGroupCAN},  // 0052  0x034
  { Name: "USCM-R-0-B",    Group: nodesGroupCAN},  // 0053  0x035
  { Name: "USCM-R-0-P",    Group: nodesGroupCAN},  // 0054  0x036
  { Name: "USCM-R-0-S",    Group: nodesGroupCAN},  // 0055  0x037
  { Name: "USCM-R-1-A",    Group: nodesGroupCAN},  // 0056  0x038
  { Name: "USCM-R-1-B",    Group: nodesGroupCAN},  // 0057  0x039
  { Name: "USCM-R-1-P",    Group: nodesGroupCAN},  // 0058  0x03A
  { Name: "USCM-R-1-S",    Group: nodesGroupCAN},  // 0059  0x03B
  { Name: "USCM-S-0-A",    Group: nodesGroupCAN},  // 0060  0x03C
  { Name: "USCM-S-0-B",    Group: nodesGroupCAN},  // 0061  0x03D
  { Name: "USCM-S-0-P",    Group: nodesGroupCAN},  // 0062  0x03E
  { Name: "USCM-S-0-S",    Group: nodesGroupCAN},  // 0063  0x03F
  { Name: "USCM-S-1-A",    Group: nodesGroupCAN},  // 0064  0x040
  { Name: "USCM-S-1-B",    Group: nodesGroupCAN},  // 0065  0x041
  { Name: "USCM-S-1-P",    Group: nodesGroupCAN},  // 0066  0x042
  { Name: "USCM-S-1-S",    Group: nodesGroupCAN},  // 0067  0x043
  { Name: "USCM-S-2-A",    Group: nodesGroupCAN},  // 0068  0x044
  { Name: "USCM-S-2-B",    Group: nodesGroupCAN},  // 0069  0x045
  { Name: "USCM-S-2-P",    Group: nodesGroupCAN},  // 0070  0x046
  { Name: "USCM-S-2-S",    Group: nodesGroupCAN},  // 0071  0x047
  { Name: "USCM-S-3-A",    Group: nodesGroupCAN},  // 0072  0x048
  { Name: "USCM-S-3-B",    Group: nodesGroupCAN},  // 0073  0x049
  { Name: "USCM-S-3-P",    Group: nodesGroupCAN},  // 0074  0x04A
  { Name: "USCM-S-3-S",    Group: nodesGroupCAN},  // 0075  0x04B
  { Name: "USCM-T-0-A",    Group: nodesGroupCAN},  // 0076  0x04C
  { Name: "USCM-T-0-B",    Group: nodesGroupCAN},  // 0077  0x04D
  { Name: "USCM-T-0-P",    Group: nodesGroupCAN},  // 0078  0x04E
  { Name: "USCM-T-0-S",    Group: nodesGroupCAN},  // 0079  0x04F
  { Name: "USCM-T-1-A",    Group: nodesGroupCAN},  // 0080  0x050
  { Name: "USCM-T-1-B",    Group: nodesGroupCAN},  // 0081  0x051
  { Name: "USCM-T-1-P",    Group: nodesGroupCAN},  // 0082  0x052
  { Name: "USCM-T-1-S",    Group: nodesGroupCAN},  // 0083  0x053
  { Name: "USCM-T-2-A",    Group: nodesGroupCAN},  // 0084  0x054
  { Name: "USCM-T-2-B",    Group: nodesGroupCAN},  // 0085  0x055
  { Name: "USCM-T-2-P",    Group: nodesGroupCAN},  // 0086  0x056
  { Name: "USCM-T-2-S",    Group: nodesGroupCAN},  // 0087  0x057
  { Name: "USCM-T-3-A",    Group: nodesGroupCAN},  // 0088  0x058
  { Name: "USCM-T-3-B",    Group: nodesGroupCAN},  // 0089  0x059
  { Name: "USCM-T-3-P",    Group: nodesGroupCAN},  // 0090  0x05A
  { Name: "USCM-T-3-S",    Group: nodesGroupCAN},  // 0091  0x05B
  { Name: "USCM-T-4-A",    Group: nodesGroupCAN},  // 0092  0x05C
  { Name: "USCM-T-4-B",    Group: nodesGroupCAN},  // 0093  0x05D
  { Name: "USCM-T-4-P",    Group: nodesGroupCAN},  // 0094  0x05E
  { Name: "USCM-T-4-S",    Group: nodesGroupCAN},  // 0095  0x05F
  { Name: "USCM-T-5-A",    Group: nodesGroupCAN},  // 0096  0x060
  { Name: "USCM-T-5-B",    Group: nodesGroupCAN},  // 0097  0x061
  { Name: "USCM-T-5-P",    Group: nodesGroupCAN},  // 0098  0x062
  { Name: "USCM-T-5-S",    Group: nodesGroupCAN},  // 0099  0x063
  { Name: "USCM-T-6-A",    Group: nodesGroupCAN},  // 0100  0x064
  { Name: "USCM-T-6-B",    Group: nodesGroupCAN},  // 0101  0x065
  { Name: "USCM-T-6-P",    Group: nodesGroupCAN},  // 0102  0x066
  { Name: "USCM-T-6-S",    Group: nodesGroupCAN},  // 0103  0x067
  { Name: "USCM-T-7-A",    Group: nodesGroupCAN},  // 0104  0x068
  { Name: "USCM-T-7-B",    Group: nodesGroupCAN},  // 0105  0x069
  { Name: "USCM-T-7-P",    Group: nodesGroupCAN},  // 0106  0x06A
  { Name: "USCM-T-7-S",    Group: nodesGroupCAN},  // 0107  0x06B
  { Name: "USCM-TT-A",     Group: nodesGroupCAN},  // 0108  0x06C
  { Name: "USCM-TT-B",     Group: nodesGroupCAN},  // 0109  0x06D
  { Name: "USCM-TT-P",     Group: nodesGroupCAN},  // 0110  0x06E
  { Name: "USCM-TT-S",     Group: nodesGroupCAN},  // 0111  0x06F
  { Name: "USCM-U-0-A",    Group: nodesGroupCAN},  // 0112  0x070
  { Name: "USCM-U-0-B",    Group: nodesGroupCAN},  // 0113  0x071
  { Name: "USCM-U-0-P",    Group: nodesGroupCAN},  // 0114  0x072
  { Name: "USCM-U-0-S",    Group: nodesGroupCAN},  // 0115  0x073
  { Name: "USCM-U-1-A",    Group: nodesGroupCAN},  // 0116  0x074
  { Name: "USCM-U-1-B",    Group: nodesGroupCAN},  // 0117  0x075
  { Name: "USCM-U-1-P",    Group: nodesGroupCAN},  // 0118  0x076
  { Name: "USCM-U-1-S",    Group: nodesGroupCAN},  // 0119  0x077
  { Name: "USCM-UG-A",     Group: nodesGroupCAN},  // 0120  0x078
  { Name: "USCM-UG-B",     Group: nodesGroupCAN},  // 0121  0x079
  { Name: "USCM-UG-P",     Group: nodesGroupCAN},  // 0122  0x07A
  { Name: "USCM-UG-S",     Group: nodesGroupCAN},  // 0123  0x07B
  { Name: "CAB-A",         Group: nodesGroupCAN},  // 0124  0x07C
  { Name: "CAB-B",         Group: nodesGroupCAN},  // 0125  0x07D
  { Name: "CAB-P",         Group: nodesGroupCAN},  // 0126  0x07E
  { Name: "CAB-S",         Group: nodesGroupCAN},  // 0127  0x07F

  //  JINJ and Trigger addresses

  { Name: "JINJ-0",        Group: nodesGroupAMSW},
  { Name: "JINJ-1",        Group: nodesGroupAMSW},
  { Name: "JINJ-2",        Group: nodesGroupAMSW},
  { Name: "JINJ-3",        Group: nodesGroupAMSW},
  { Name: "JINJ-P",        Group: nodesGroupAMSW},
  { Name: "JINJ-S",        Group: nodesGroupAMSW},
  { Name: "JINJ-T",        Group: nodesGroupAMSW},
  { Name: "JINJ-Q",        Group: nodesGroupAMSW},
  { Name: "JLV1-A",        Group: nodesGroupAMSW},
  { Name: "JLV1-B",        Group: nodesGroupAMSW},
  { Name: "JLV1-P",        Group: nodesGroupAMSW},
  { Name: "JLV1-S",        Group: nodesGroupAMSW},
  { Name: "ETRG-0-A",      Group: nodesGroupAMSW},
  { Name: "ETRG-0-B",      Group: nodesGroupAMSW},
  { Name: "ETRG-0-P",      Group: nodesGroupAMSW},
  { Name: "ETRG-0-S",      Group: nodesGroupAMSW},
  { Name: "ETRG-1-A",      Group: nodesGroupAMSW},
  { Name: "ETRG-1-B",      Group: nodesGroupAMSW},
  { Name: "ETRG-1-P",      Group: nodesGroupAMSW},
  { Name: "ETRG-1-S",      Group: nodesGroupAMSW},
  { Name: "NA 0x94",       Group: nodesGroupUNASG},
  { Name: "NA 0x95",       Group: nodesGroupUNASG},

  //  DAQ Addresses

  { Name: "JINF-E-0-B",    Group: nodesGroupAMSW},
  { Name: "JINF-E-0-P",    Group: nodesGroupAMSW},
  { Name: "JINF-E-0-S",    Group: nodesGroupAMSW},
  { Name: "JINF-E-1-A",    Group: nodesGroupAMSW},
  { Name: "JINF-E-1-B",    Group: nodesGroupAMSW},
  { Name: "JINF-E-1-P",    Group: nodesGroupAMSW},
  { Name: "JINF-E-1-S",    Group: nodesGroupAMSW},
  { Name: "JINF-R-0-A",    Group: nodesGroupAMSW},
  { Name: "JINF-R-0-B",    Group: nodesGroupAMSW},
  { Name: "JINF-R-0-P",    Group: nodesGroupAMSW},
  { Name: "JINF-R-0-S",    Group: nodesGroupAMSW},
  { Name: "JINF-R-1-A",    Group: nodesGroupAMSW},
  { Name: "JINF-R-1-B",    Group: nodesGroupAMSW},
  { Name: "JINF-R-1-P",    Group: nodesGroupAMSW},
  { Name: "JINF-R-1-S",    Group: nodesGroupAMSW},
  { Name: "JINF-T-0-A",    Group: nodesGroupAMSW},
  { Name: "JINF-T-0-B",    Group: nodesGroupAMSW},
  { Name: "JINF-T-0-P",    Group: nodesGroupAMSW},
  { Name: "JINF-T-0-S",    Group: nodesGroupAMSW},
  { Name: "JINF-T-1-A",    Group: nodesGroupAMSW},
  { Name: "JINF-T-1-B",    Group: nodesGroupAMSW},
  { Name: "JINF-T-1-P",    Group: nodesGroupAMSW},
  { Name: "JINF-T-1-S",    Group: nodesGroupAMSW},
  { Name: "JINF-T-2-A",    Group: nodesGroupAMSW},
  { Name: "JINF-T-2-B",    Group: nodesGroupAMSW},
  { Name: "JINF-T-2-P",    Group: nodesGroupAMSW},
  { Name: "JINF-T-2-S",    Group: nodesGroupAMSW},
  { Name: "JINF-T-3-A",    Group: nodesGroupAMSW},
  { Name: "JINF-T-3-B",    Group: nodesGroupAMSW},
  { Name: "JINF-T-3-P",    Group: nodesGroupAMSW},
  { Name: "JINF-T-3-S",    Group: nodesGroupAMSW},
  { Name: "JINF-T-4-A",    Group: nodesGroupAMSW},
  { Name: "JINF-T-4-B",    Group: nodesGroupAMSW},
  { Name: "JINF-T-4-P",    Group: nodesGroupAMSW},
  { Name: "JINF-T-4-S",    Group: nodesGroupAMSW},
  { Name: "JINF-T-5-A",    Group: nodesGroupAMSW},
  { Name: "JINF-T-5-B",    Group: nodesGroupAMSW},
  { Name: "JINF-T-5-P",    Group: nodesGroupAMSW},
  { Name: "JINF-T-5-S",    Group: nodesGroupAMSW},
  { Name: "JINF-T-6-A",    Group: nodesGroupAMSW},
  { Name: "JINF-T-6-B",    Group: nodesGroupAMSW},
  { Name: "JINF-T-6-P",    Group: nodesGroupAMSW},
  { Name: "JINF-T-6-S",    Group: nodesGroupAMSW},
  { Name: "JINF-T-7-A",    Group: nodesGroupAMSW},
  { Name: "JINF-T-7-B",    Group: nodesGroupAMSW},
  { Name: "JINF-T-7-P",    Group: nodesGroupAMSW},
  { Name: "JINF-T-7-S",    Group: nodesGroupAMSW},
  { Name: "JINF-U-0-A",    Group: nodesGroupAMSW},
  { Name: "JINF-U-0-B",    Group: nodesGroupAMSW},
  { Name: "JINF-U-0-P",    Group: nodesGroupAMSW},
  { Name: "JINF-U-0-S",    Group: nodesGroupAMSW},
  { Name: "JINF-U-1-A",    Group: nodesGroupAMSW},
  { Name: "JINF-U-1-B",    Group: nodesGroupAMSW},
  { Name: "JINF-U-1-P",    Group: nodesGroupAMSW},
  { Name: "JINF-U-1-S",    Group: nodesGroupAMSW},
  { Name: "EDR-0-0-A",     Group: nodesGroupAMSW},
  { Name: "EDR-0-0-B",     Group: nodesGroupAMSW},
  { Name: "EDR-0-0-P",     Group: nodesGroupAMSW},
  { Name: "EDR-0-1-A",     Group: nodesGroupAMSW},
  { Name: "EDR-0-1-B",     Group: nodesGroupAMSW},
  { Name: "EDR-0-1-P",     Group: nodesGroupAMSW},
  { Name: "EDR-0-2-A",     Group: nodesGroupAMSW},
  { Name: "EDR-0-2-B",     Group: nodesGroupAMSW},
  { Name: "EDR-0-2-P",     Group: nodesGroupAMSW},
  { Name: "EDR-0-3-A",     Group: nodesGroupAMSW},
  { Name: "EDR-0-3-B",     Group: nodesGroupAMSW},
  { Name: "EDR-0-3-P",     Group: nodesGroupAMSW},
  { Name: "EDR-0-4-A",     Group: nodesGroupAMSW},
  { Name: "EDR-0-4-B",     Group: nodesGroupAMSW},
  { Name: "EDR-0-4-P",     Group: nodesGroupAMSW},
  { Name: "EDR-0-5-A",     Group: nodesGroupAMSW},
  { Name: "EDR-0-5-B",     Group: nodesGroupAMSW},
  { Name: "EDR-0-5-P",     Group: nodesGroupAMSW},
  { Name: "EDR-1-0-A",     Group: nodesGroupAMSW},
  { Name: "EDR-1-0-B",     Group: nodesGroupAMSW},
  { Name: "EDR-1-0-P",     Group: nodesGroupAMSW},
  { Name: "EDR-1-1-A",     Group: nodesGroupAMSW},
  { Name: "EDR-1-1-B",     Group: nodesGroupAMSW},
  { Name: "EDR-1-1-P",     Group: nodesGroupAMSW},
  { Name: "EDR-1-2-A",     Group: nodesGroupAMSW},
  { Name: "EDR-1-2-B",     Group: nodesGroupAMSW},
  { Name: "EDR-1-2-P",     Group: nodesGroupAMSW},
  { Name: "EDR-1-3-A",     Group: nodesGroupAMSW},
  { Name: "EDR-1-3-B",     Group: nodesGroupAMSW},
  { Name: "EDR-1-3-P",     Group: nodesGroupAMSW},
  { Name: "EDR-1-4-A",     Group: nodesGroupAMSW},
  { Name: "EDR-1-4-B",     Group: nodesGroupAMSW},
  { Name: "EDR-1-4-P",     Group: nodesGroupAMSW},
  { Name: "EDR-1-5-A",     Group: nodesGroupAMSW},
  { Name: "EDR-1-5-B",     Group: nodesGroupAMSW},
  { Name: "EDR-1-5-P",     Group: nodesGroupAMSW},
  { Name: "RDR-0-0-A",     Group: nodesGroupAMSW},
  { Name: "RDR-0-0-B",     Group: nodesGroupAMSW},
  { Name: "RDR-0-1-A",     Group: nodesGroupAMSW},
  { Name: "RDR-0-1-B",     Group: nodesGroupAMSW},
  { Name: "RDR-0-2-A",     Group: nodesGroupAMSW},
  { Name: "RDR-0-2-B",     Group: nodesGroupAMSW},
  { Name: "RDR-0-3-A",     Group: nodesGroupAMSW},
  { Name: "RDR-0-3-B",     Group: nodesGroupAMSW},
  { Name: "RDR-0-4-A",     Group: nodesGroupAMSW},
  { Name: "RDR-0-4-B",     Group: nodesGroupAMSW},
  { Name: "RDR-0-5-A",     Group: nodesGroupAMSW},
  { Name: "RDR-0-5-B",     Group: nodesGroupAMSW},
  { Name: "RDR-1-0-A",     Group: nodesGroupAMSW},
  { Name: "RDR-1-0-B",     Group: nodesGroupAMSW},
  { Name: "RDR-1-1-A",     Group: nodesGroupAMSW},
  { Name: "RDR-1-1-B",     Group: nodesGroupAMSW},
  { Name: "RDR-1-2-A",     Group: nodesGroupAMSW},
  { Name: "RDR-1-2-B",     Group: nodesGroupAMSW},
  { Name: "RDR-1-3-A",     Group: nodesGroupAMSW},
  { Name: "RDR-1-3-B",     Group: nodesGroupAMSW},
  { Name: "RDR-1-4-A",     Group: nodesGroupAMSW},
  { Name: "RDR-1-4-B",     Group: nodesGroupAMSW},
  { Name: "RDR-1-5-A",     Group: nodesGroupAMSW},
  { Name: "RDR-1-5-B",     Group: nodesGroupAMSW},
  { Name: "SDR-0-A",       Group: nodesGroupAMSW},
  { Name: "SDR-0-B",       Group: nodesGroupAMSW},
  { Name: "SDR-0-P",       Group: nodesGroupAMSW},
  { Name: "SDR-0-S",       Group: nodesGroupAMSW},
  { Name: "SDR-1-A",       Group: nodesGroupAMSW},
  { Name: "SDR-1-B",       Group: nodesGroupAMSW},
  { Name: "SDR-1-P",       Group: nodesGroupAMSW},
  { Name: "SDR-1-S",       Group: nodesGroupAMSW},
  { Name: "SDR-2-A",       Group: nodesGroupAMSW},
  { Name: "SDR-2-B",       Group: nodesGroupAMSW},
  { Name: "SDR-2-P",       Group: nodesGroupAMSW},
  { Name: "SDR-2-S",       Group: nodesGroupAMSW},
  { Name: "SDR-3-A",       Group: nodesGroupAMSW},
  { Name: "SDR-3-B",       Group: nodesGroupAMSW},
  { Name: "SDR-3-P",       Group: nodesGroupAMSW},
  { Name: "SDR-3-S",       Group: nodesGroupAMSW},
  { Name: "TDR-0-00-A",    Group: nodesGroupAMSW},
  { Name: "TDR-0-00-B",    Group: nodesGroupAMSW},
  { Name: "TDR-0-01-A",    Group: nodesGroupAMSW},
  { Name: "TDR-0-01-B",    Group: nodesGroupAMSW},
  { Name: "TDR-0-02-A",    Group: nodesGroupAMSW},
  { Name: "TDR-0-02-B",    Group: nodesGroupAMSW},
  { Name: "TDR-0-03-A",    Group: nodesGroupAMSW},
  { Name: "TDR-0-03-B",    Group: nodesGroupAMSW},
  { Name: "TDR-0-04-A",    Group: nodesGroupAMSW},
  { Name: "TDR-0-04-B",    Group: nodesGroupAMSW},
  { Name: "TDR-0-05-A",    Group: nodesGroupAMSW},
  { Name: "TDR-0-05-B",    Group: nodesGroupAMSW},
  { Name: "TDR-0-06-A",    Group: nodesGroupAMSW},
  { Name: "TDR-0-06-B",    Group: nodesGroupAMSW},
  { Name: "TDR-0-07-A",    Group: nodesGroupAMSW},
  { Name: "TDR-0-07-B",    Group: nodesGroupAMSW},
  { Name: "TDR-0-08-A",    Group: nodesGroupAMSW},
  { Name: "TDR-0-08-B",    Group: nodesGroupAMSW},
  { Name: "TDR-0-09-A",    Group: nodesGroupAMSW},
  { Name: "TDR-0-09-B",    Group: nodesGroupAMSW},
  { Name: "TDR-0-10-A",    Group: nodesGroupAMSW},
  { Name: "TDR-0-10-B",    Group: nodesGroupAMSW},
  { Name: "TDR-0-11-A",    Group: nodesGroupAMSW},
  { Name: "TDR-0-11-B",    Group: nodesGroupAMSW},
  { Name: "TDR-1-00-A",    Group: nodesGroupAMSW},
  { Name: "TDR-1-00-B",    Group: nodesGroupAMSW},
  { Name: "TDR-1-01-A",    Group: nodesGroupAMSW},
  { Name: "TDR-1-01-B",    Group: nodesGroupAMSW},
  { Name: "TDR-1-02-A",    Group: nodesGroupAMSW},
  { Name: "TDR-1-02-B",    Group: nodesGroupAMSW},
  { Name: "TDR-1-03-A",    Group: nodesGroupAMSW},
  { Name: "TDR-1-03-B",    Group: nodesGroupAMSW},
  { Name: "TDR-1-04-A",    Group: nodesGroupAMSW},
  { Name: "TDR-1-04-B",    Group: nodesGroupAMSW},
  { Name: "TDR-1-05-A",    Group: nodesGroupAMSW},
  { Name: "TDR-1-05-B",    Group: nodesGroupAMSW},
  { Name: "TDR-1-06-A",    Group: nodesGroupAMSW},
  { Name: "TDR-1-06-B",    Group: nodesGroupAMSW},
  { Name: "TDR-1-07-A",    Group: nodesGroupAMSW},
  { Name: "TDR-1-07-B",    Group: nodesGroupAMSW},
  { Name: "TDR-1-08-A",    Group: nodesGroupAMSW},
  { Name: "TDR-1-08-B",    Group: nodesGroupAMSW},
  { Name: "TDR-1-09-A",    Group: nodesGroupAMSW},
  { Name: "TDR-1-09-B",    Group: nodesGroupAMSW},
  { Name: "TDR-1-10-A",    Group: nodesGroupAMSW},
  { Name: "TDR-1-10-B",    Group: nodesGroupAMSW},
  { Name: "TDR-1-11-A",    Group: nodesGroupAMSW},
  { Name: "TDR-1-11-B",    Group: nodesGroupAMSW},
  { Name: "TDR-2-00-A",    Group: nodesGroupAMSW},
  { Name: "TDR-2-00-B",    Group: nodesGroupAMSW},
  { Name: "TDR-2-01-A",    Group: nodesGroupAMSW},
  { Name: "TDR-2-01-B",    Group: nodesGroupAMSW},
  { Name: "TDR-2-02-A",    Group: nodesGroupAMSW},
  { Name: "TDR-2-02-B",    Group: nodesGroupAMSW},
  { Name: "TDR-2-03-A",    Group: nodesGroupAMSW},
  { Name: "TDR-2-03-B",    Group: nodesGroupAMSW},
  { Name: "TDR-2-04-A",    Group: nodesGroupAMSW},
  { Name: "TDR-2-04-B",    Group: nodesGroupAMSW},
  { Name: "TDR-2-05-A",    Group: nodesGroupAMSW},
  { Name: "TDR-2-05-B",    Group: nodesGroupAMSW},
  { Name: "TDR-2-06-A",    Group: nodesGroupAMSW},
  { Name: "TDR-2-06-B",    Group: nodesGroupAMSW},
  { Name: "TDR-2-07-A",    Group: nodesGroupAMSW},
  { Name: "TDR-2-07-B",    Group: nodesGroupAMSW},
  { Name: "TDR-2-08-A",    Group: nodesGroupAMSW},
  { Name: "TDR-2-08-B",    Group: nodesGroupAMSW},
  { Name: "TDR-2-09-A",    Group: nodesGroupAMSW},
  { Name: "TDR-2-09-B",    Group: nodesGroupAMSW},
  { Name: "TDR-2-10-A",    Group: nodesGroupAMSW},
  { Name: "TDR-2-10-B",    Group: nodesGroupAMSW},
  { Name: "TDR-2-11-A",    Group: nodesGroupAMSW},
  { Name: "TDR-2-11-B",    Group: nodesGroupAMSW},
  { Name: "TDR-3-00-A",    Group: nodesGroupAMSW},
  { Name: "TDR-3-00-B",    Group: nodesGroupAMSW},
  { Name: "TDR-3-01-A",    Group: nodesGroupAMSW},
  { Name: "TDR-3-01-B",    Group: nodesGroupAMSW},
  { Name: "TDR-3-02-A",    Group: nodesGroupAMSW},
  { Name: "TDR-3-02-B",    Group: nodesGroupAMSW},
  { Name: "TDR-3-03-A",    Group: nodesGroupAMSW},
  { Name: "TDR-3-03-B",    Group: nodesGroupAMSW},
  { Name: "TDR-3-04-A",    Group: nodesGroupAMSW},
  { Name: "TDR-3-04-B",    Group: nodesGroupAMSW},
  { Name: "TDR-3-05-A",    Group: nodesGroupAMSW},
  { Name: "TDR-3-05-B",    Group: nodesGroupAMSW},
  { Name: "TDR-3-06-A",    Group: nodesGroupAMSW},
  { Name: "TDR-3-06-B",    Group: nodesGroupAMSW},
  { Name: "TDR-3-07-A",    Group: nodesGroupAMSW},
  { Name: "TDR-3-07-B",    Group: nodesGroupAMSW},
  { Name: "TDR-3-08-A",    Group: nodesGroupAMSW},
  { Name: "TDR-3-08-B",    Group: nodesGroupAMSW},
  { Name: "TDR-3-09-A",    Group: nodesGroupAMSW},
  { Name: "TDR-3-09-B",    Group: nodesGroupAMSW},
  { Name: "TDR-3-10-A",    Group: nodesGroupAMSW},
  { Name: "TDR-3-10-B",    Group: nodesGroupAMSW},
  { Name: "TDR-3-11-A",    Group: nodesGroupAMSW},
  { Name: "TDR-3-11-B",    Group: nodesGroupAMSW},
  { Name: "TDR-4-00-A",    Group: nodesGroupAMSW},
  { Name: "TDR-4-00-B",    Group: nodesGroupAMSW},
  { Name: "TDR-4-01-A",    Group: nodesGroupAMSW},
  { Name: "TDR-4-01-B",    Group: nodesGroupAMSW},
  { Name: "TDR-4-02-A",    Group: nodesGroupAMSW},
  { Name: "TDR-4-02-B",    Group: nodesGroupAMSW},
  { Name: "TDR-4-03-A",    Group: nodesGroupAMSW},
  { Name: "TDR-4-03-B",    Group: nodesGroupAMSW},
  { Name: "TDR-4-04-A",    Group: nodesGroupAMSW},
  { Name: "TDR-4-04-B",    Group: nodesGroupAMSW},
  { Name: "TDR-4-05-A",    Group: nodesGroupAMSW},
  { Name: "TDR-4-05-B",    Group: nodesGroupAMSW},
  { Name: "TDR-4-06-A",    Group: nodesGroupAMSW},
  { Name: "TDR-4-06-B",    Group: nodesGroupAMSW},
  { Name: "TDR-4-07-A",    Group: nodesGroupAMSW},
  { Name: "TDR-4-07-B",    Group: nodesGroupAMSW},
  { Name: "TDR-4-08-A",    Group: nodesGroupAMSW},
  { Name: "TDR-4-08-B",    Group: nodesGroupAMSW},
  { Name: "TDR-4-09-A",    Group: nodesGroupAMSW},
  { Name: "TDR-4-09-B",    Group: nodesGroupAMSW},
  { Name: "TDR-4-10-A",    Group: nodesGroupAMSW},
  { Name: "TDR-4-10-B",    Group: nodesGroupAMSW},
  { Name: "TDR-4-11-A",    Group: nodesGroupAMSW},
  { Name: "TDR-4-11-B",    Group: nodesGroupAMSW},
  { Name: "TDR-5-00-A",    Group: nodesGroupAMSW},
  { Name: "TDR-5-00-B",    Group: nodesGroupAMSW},
  { Name: "TDR-5-01-A",    Group: nodesGroupAMSW},
  { Name: "TDR-5-01-B",    Group: nodesGroupAMSW},
  { Name: "TDR-5-02-A",    Group: nodesGroupAMSW},
  { Name: "TDR-5-02-B",    Group: nodesGroupAMSW},
  { Name: "TDR-5-03-A",    Group: nodesGroupAMSW},
  { Name: "TDR-5-03-B",    Group: nodesGroupAMSW},
  { Name: "TDR-5-04-A",    Group: nodesGroupAMSW},
  { Name: "TDR-5-04-B",    Group: nodesGroupAMSW},
  { Name: "TDR-5-05-A",    Group: nodesGroupAMSW},
  { Name: "TDR-5-05-B",    Group: nodesGroupAMSW},
  { Name: "TDR-5-06-A",    Group: nodesGroupAMSW},
  { Name: "TDR-5-06-B",    Group: nodesGroupAMSW},
  { Name: "TDR-5-07-A",    Group: nodesGroupAMSW},
  { Name: "TDR-5-07-B",    Group: nodesGroupAMSW},
  { Name: "TDR-5-08-A",    Group: nodesGroupAMSW},
  { Name: "TDR-5-08-B",    Group: nodesGroupAMSW},
  { Name: "TDR-5-09-A",    Group: nodesGroupAMSW},
  { Name: "TDR-5-09-B",    Group: nodesGroupAMSW},
  { Name: "TDR-5-10-A",    Group: nodesGroupAMSW},
  { Name: "TDR-5-10-B",    Group: nodesGroupAMSW},
  { Name: "TDR-5-11-A",    Group: nodesGroupAMSW},
  { Name: "TDR-5-11-B",    Group: nodesGroupAMSW},
  { Name: "TDR-6-00-A",    Group: nodesGroupAMSW},
  { Name: "TDR-6-00-B",    Group: nodesGroupAMSW},
  { Name: "TDR-6-01-A",    Group: nodesGroupAMSW},
  { Name: "TDR-6-01-B",    Group: nodesGroupAMSW},
  { Name: "TDR-6-02-A",    Group: nodesGroupAMSW},
  { Name: "TDR-6-02-B",    Group: nodesGroupAMSW},
  { Name: "TDR-6-03-A",    Group: nodesGroupAMSW},
  { Name: "TDR-6-03-B",    Group: nodesGroupAMSW},
  { Name: "TDR-6-04-A",    Group: nodesGroupAMSW},
  { Name: "TDR-6-04-B",    Group: nodesGroupAMSW},
  { Name: "TDR-6-05-A",    Group: nodesGroupAMSW},
  { Name: "TDR-6-05-B",    Group: nodesGroupAMSW},
  { Name: "TDR-6-06-A",    Group: nodesGroupAMSW},
  { Name: "TDR-6-06-B",    Group: nodesGroupAMSW},
  { Name: "TDR-6-07-A",    Group: nodesGroupAMSW},
  { Name: "TDR-6-07-B",    Group: nodesGroupAMSW},
  { Name: "TDR-6-08-A",    Group: nodesGroupAMSW},
  { Name: "TDR-6-08-B",    Group: nodesGroupAMSW},
  { Name: "TDR-6-09-A",    Group: nodesGroupAMSW},
  { Name: "TDR-6-09-B",    Group: nodesGroupAMSW},
  { Name: "TDR-6-10-A",    Group: nodesGroupAMSW},
  { Name: "TDR-6-10-B",    Group: nodesGroupAMSW},
  { Name: "TDR-6-11-A",    Group: nodesGroupAMSW},
  { Name: "TDR-6-11-B",    Group: nodesGroupAMSW},
  { Name: "TDR-7-00-A",    Group: nodesGroupAMSW},
  { Name: "TDR-7-00-B",    Group: nodesGroupAMSW},
  { Name: "TDR-7-01-A",    Group: nodesGroupAMSW},
  { Name: "TDR-7-01-B",    Group: nodesGroupAMSW},
  { Name: "TDR-7-02-A",    Group: nodesGroupAMSW},
  { Name: "TDR-7-02-B",    Group: nodesGroupAMSW},
  { Name: "TDR-7-03-A",    Group: nodesGroupAMSW},
  { Name: "TDR-7-03-B",    Group: nodesGroupAMSW},
  { Name: "TDR-7-04-A",    Group: nodesGroupAMSW},
  { Name: "TDR-7-04-B",    Group: nodesGroupAMSW},
  { Name: "TDR-7-05-A",    Group: nodesGroupAMSW},
  { Name: "TDR-7-05-B",    Group: nodesGroupAMSW},
  { Name: "TDR-7-06-A",    Group: nodesGroupAMSW},
  { Name: "TDR-7-06-B",    Group: nodesGroupAMSW},
  { Name: "TDR-7-07-A",    Group: nodesGroupAMSW},
  { Name: "TDR-7-07-B",    Group: nodesGroupAMSW},
  { Name: "TDR-7-08-A",    Group: nodesGroupAMSW},
  { Name: "TDR-7-08-B",    Group: nodesGroupAMSW},
  { Name: "TDR-7-09-A",    Group: nodesGroupAMSW},
  { Name: "TDR-7-09-B",    Group: nodesGroupAMSW},
  { Name: "TDR-7-10-A",    Group: nodesGroupAMSW},
  { Name: "TDR-7-10-B",    Group: nodesGroupAMSW},
  { Name: "TDR-7-11-A",    Group: nodesGroupAMSW},
  { Name: "TDR-7-11-B",    Group: nodesGroupAMSW},
  { Name: "UDR-0-0-A",     Group: nodesGroupAMSW},
  { Name: "UDR-0-0-B",     Group: nodesGroupAMSW},
  { Name: "UDR-0-0-P",     Group: nodesGroupAMSW},
  { Name: "UDR-0-1-A",     Group: nodesGroupAMSW},
  { Name: "UDR-0-1-B",     Group: nodesGroupAMSW},
  { Name: "UDR-0-1-P",     Group: nodesGroupAMSW},
  { Name: "UDR-0-2-A",     Group: nodesGroupAMSW},
  { Name: "UDR-0-2-B",     Group: nodesGroupAMSW},
  { Name: "UDR-0-2-P",     Group: nodesGroupAMSW},
  { Name: "UDR-0-3-A",     Group: nodesGroupAMSW},
  { Name: "UDR-0-3-B",     Group: nodesGroupAMSW},
  { Name: "UDR-0-3-P",     Group: nodesGroupAMSW},
  { Name: "UDR-0-4-A",     Group: nodesGroupAMSW},
  { Name: "UDR-0-4-B",     Group: nodesGroupAMSW},
  { Name: "UDR-0-4-P",     Group: nodesGroupAMSW},
  { Name: "UDR-0-5-A",     Group: nodesGroupAMSW},
  { Name: "UDR-0-5-B",     Group: nodesGroupAMSW},
  { Name: "UDR-0-5-P",     Group: nodesGroupAMSW},
  { Name: "UDR-1-0-A",     Group: nodesGroupAMSW},
  { Name: "UDR-1-0-B",     Group: nodesGroupAMSW},
  { Name: "UDR-1-0-P",     Group: nodesGroupAMSW},
  { Name: "UDR-1-1-A",     Group: nodesGroupAMSW},
  { Name: "UDR-1-1-B",     Group: nodesGroupAMSW},
  { Name: "UDR-1-1-P",     Group: nodesGroupAMSW},
  { Name: "UDR-1-2-A",     Group: nodesGroupAMSW},
  { Name: "UDR-1-2-B",     Group: nodesGroupAMSW},
  { Name: "UDR-1-2-P",     Group: nodesGroupAMSW},
  { Name: "UDR-1-3-A",     Group: nodesGroupAMSW},
  { Name: "UDR-1-3-B",     Group: nodesGroupAMSW},
  { Name: "UDR-1-3-P",     Group: nodesGroupAMSW},
  { Name: "UDR-1-4-A",     Group: nodesGroupAMSW},
  { Name: "UDR-1-4-B",     Group: nodesGroupAMSW},
  { Name: "UDR-1-4-P",     Group: nodesGroupAMSW},
  { Name: "UDR-1-5-A",     Group: nodesGroupAMSW},
  { Name: "UDR-1-5-B",     Group: nodesGroupAMSW},
  { Name: "UDR-1-5-P",     Group: nodesGroupAMSW},
  { Name: "NA 0x1FE",      Group: nodesGroupUNASG},
  { Name: "AFU",           Group: nodesGroupUNASG},
  { Name: "BROADCAST",     Group: nodesGroupEXTERN},
};

#define NodesDim ((sizeof NodeNames) / (sizeof NodeNames[0]))

int32 NodeNamesDim = NodesDim;

////////////////////////////////////////////////////////////////////////////
//-Extern: nodesLookupValue
int32 nodesLookupValue(char *Name)
//-Synopsis:    Look up a node's value from a symbolic name
//-Returns:     Returns the node's value or max node number defined plus one.
//-Description: Look up a node's value from a symbolic name
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
int i;
char *endptr;
int32 rtn;

  for (i=0; i<NodesDim; i++)
    if (strcasecmp(NodeNames[i].Name, Name) == 0)
      return i;

  if (*Name == '\0')
    return NodesDim;

  rtn = strtol(Name, &endptr, 0);
  if (*endptr == '\0')
    return rtn;

  return NodesDim;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: nodesLookupName
char *nodesLookupName(int32 Node)
//-Synopsis:    Look up a APIDs value from a symbolic name
//-Returns:     Returns the APID's value or zero on error
//-Description: Look up a APIDs value from a symbolic name
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
static char NotFound[10];

  if (Node > NodesDim)
  {
    sprintf(NotFound, "0x%lX", Node);
    return NotFound;
  }

  return NodeNames[Node].Name;
}

