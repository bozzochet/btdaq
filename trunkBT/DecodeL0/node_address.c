/***********************************************************
 *  File: node_address.c
 *
 *  C code for AMS-02 to define node address name string
 *
 *  History:
 *    20190730 - created by X.Cai
 ***********************************************************/

#include <stddef.h>
#include "node_address.h"

//----------------------------------------------------------
//  Node names
//----------------------------------------------------------

const char *node_names[512] = {
    "JMDC-0-CAN",  "JMDC-1-CAN",  "JMDC-2-CAN",  "JMDC-3-CAN",      // 000 - 003
    "JMDC-0-EXT",  "JMDC-1-EXT",  "JMDC-2-EXT",  "JMDC-3-EXT",      // 004 - 007
    "JMDC-0-AMSW", "JMDC-1-AMSW", "JMDC-2-AMSW", "JMDC-3-AMSW",     // 008 - 00B
    "JMDC-0",      "JMDC-1",      "JMDC-2",      "JMDC-3",          // 00C - 00F
    "JMDC-HRDL",   "JMDC-LRDL",   "JMDC-DAQ",    "JMDC-MCT",        // 010 - 013
    "JMDC-SELF",   "ACOP",        "DDRS2",       "JMDC-ALL",        // 014 - 017

    "CAB-A",       "CAB-B",       "CAB-P",       "CAB-S",           // 018 - 01B
    "USCM-CCEB-A", "USCM-CCEB-B", "USCM-CCEB-P", "USCM-CCEB-S",     // 01C - 01F
    "UTT-TTTC-1",  "UTT-TTTC-2",  "UTT-TTTC-3",  "UTT-TTTC-4",      // 020 - 023
    "UTT-CAN-A",   "UTT-CAN-B",   "UTT-CAN-P",   "UTT-CAN-S",       // 024 - 027
    "USCM-JPD-A",  "USCM-JPD-B",  "USCM-JPD-P",  "USCM-JPD-S",      // 028 - 02B
    "USCM-M-A",    "USCM-M-B",    "USCM-M-P",    "USCM-M-S",        // 02C - 02F
    "PDS-A",       "PDS-B",       "PDS-P",       "PDS-S",           // 030 - 033

    // Begin: AMSWire Group Command Addresses
//     "JINF-P",      "JINF-S",      "JINF-E-P",    "JINF-E-S",        // 034 - 037
//     "JINF-R-P",    "JINF-R-S",    "JINF-T-P",    "JINF-T-S",        // 038 - 03B
//     "JINF-U-P",    "JINF-U-S",    "SDR-P",       "SDR-S",           // 03C - 03F
//     "EDR-0-M",     "EDR-0-D",     "EDR-1-M",     "EDR-1-D",         // 040 - 043
//     "EALL-0-M",    "EALL-0-D",    "EALL-1-M",    "EALL-1-D",        // 044 - 047
//     "EDR-M",       "EDR-D",       "EALL-M",      "EALL-D",          // 048 - 04B
//     "RDR-0-M",     "RDR-0-D",     "RDR-1-M",     "RDR-1-D",         // 04C - 04F
//     "RDR-M",       "RDR-D",       "UDR-0-M",     "UDR-0-D",         // 050 - 053
//     "UDR-1-M",     "UDR-1-D",     "UDR-M",       "UDR-D",           // 054 - 057

    "UNDEF",       "UNDEF",       "UNDEF",       "UNDEF",           // 034 - 037
    "UNDEF",       "UNDEF",       "UNDEF",       "UNDEF",           // 038 - 03B
    "LINJ-A",      "LINJ-B",      "LINJ-S",      "LINJ-P",          // 03C - 03F
    "LINF-Y-0-A",  "LINF-Y-0-B",  "LINF-Y-0-P",  "LINF-Y-1-A",      // 040 - 043
    "LINF-Y-1-B",  "LINF-Y-1-P",  "LINF-Y-2-A",  "LINF-Y-2-B",      // 044 - 047
    "LINF-Y-2-P",  "LINF-Y-3-A",  "LINF-Y-3-B",  "LINF-Y-3-P",      // 048 - 04B
    "LINF-U-0-A",  "LINF-U-0-B",  "LINF-U-0-P",  "LINF-U-1-A",      // 04C - 04F
    "LINF-U-1-B",  "LINF-U-1-P",  "LINF-U-2-A",  "LINF-U-2-B",      // 050 - 053
    "LINF-U-2-P",  "LINF-U-3-A",  "LINF-U-3-B",  "LINF-U-3-P",      // 054 - 057

    "TDR-0-M",     "TDR-0-D",     "TDR-1-M",     "TDR-1-D",         // 058 - 05B
    "TDR-2-M",     "TDR-2-D",     "TDR-3-M",     "TDR-3-D",         // 05C - 05F
    "TDR-4-M",     "TDR-4-D",     "TDR-5-M",     "TDR-5-D",         // 060 - 063
    "TDR-6-M",     "TDR-6-D",     "TDR-7-M",     "TDR-7-D",         // 064 - 067
    "TDR-M",       "TDR-D",       "UNDEF",       "UNDEF",           // 068 - 06B
    // End: AMSWire Group Command Addresses

    "TTCE-A",      "TTCE-B",      "TTCE-P",      "TTCE-S",          // 06C - 06F

    "UTT-TTGA-A",  "UTT-TTGA-B",  "UTT-TTGA-P",  "UTT-TTPC-0",      // 070 - 073
    "UTT-TTPC-1",  "UTT-TTPC-2",  "UTT-TTPC-3",  "UTT-TTPC-P",      // 074 - 077

    "USCM-UG-A",   "USCM-UG-B",   "USCM-UG-P",   "USCM-UG-S",       // 078 - 07B

    "UNDEF",       "UNDEF",       "UNDEF",       "UNDEF",           // 07C - 07F

    "JINJ-0",      "JINJ-1",      "JINJ-2",      "JINJ-3",          // 080 - 083
    "JINJ-P-A",    "JINJ-P-B",    "JINJ-S-A",    "JINJ-S-B",        // 084 - 087
    "JLV1-A",      "JLV1-B",      "JLV1-P",      "JLV1-S",          // 088 - 08B

    "ETRG-0-A",    "ETRG-0-B",    "ETRG-0-P",    "ETRG-0-S",        // 08C - 08F
    "ETRG-1-A",    "ETRG-1-B",    "ETRG-1-P",    "ETRG-1-S",        // 090 - 093
    "UNDEF",       "UNDEF",       "JINF-E0-A",   "JINF-E0-B",       // 094 - 097
    "JINF-E0-P",   "JINF-E0-S",   "JINF-E1-A",   "JINF-E1-B",       // 098 - 09B
    "JINF-E1-P",   "JINF-E1-S",   "JINF-R0-A",   "JINF-R0-B",       // 09C - 09F
    "JINF-R0-P",   "JINF-R0-S",   "JINF-R1-A",   "JINF-R1-B",       // 0A0 - 0A3
    "JINF-R1-P",   "JINF-R1-S",   "JINF-T0-A",   "JINF-T0-B",       // 0A4 - 0A7
    "JINF-T0-P",   "JINF-T0-S",   "JINF-T1-A",   "JINF-T1-B",       // 0A8 - 0AB
    "JINF-T1-P",   "JINF-T1-S",   "JINF-T2-A",   "JINF-T2-B",       // 0AC - 0AF
    "JINF-T2-P",   "JINF-T2-S",   "JINF-T3-A",   "JINF-T3-B",       // 0B0 - 0B3
    "JINF-T3-P",   "JINF-T3-S",   "JINF-T4-A",   "JINF-T4-B",       // 0B4 - 0B7
    "JINF-T4-P",   "JINF-T4-S",   "JINF-T5-A",   "JINF-T5-B",       // 0B8 - 0BB
    "JINF-T5-P",   "JINF-T5-S",   "JINF-T6-A",   "JINF-T6-B",       // 0BC - 0BF
    "JINF-T6-P",   "JINF-T6-S",   "JINF-T7-A",   "JINF-T7-B",       // 0C0 - 0C3
    "JINF-T7-P",   "JINF-T7-S",   "JINF-U0-A",   "JINF-U0-B",       // 0C4 - 0C7
    "JINF-U0-P",   "JINF-U0-S",   "JINF-U1-A",   "JINF-U1-B",       // 0C8 - 0CB
    "JINF-U1-P",   "JINF-U1-S",                                     // 0CC - 0CD

                                  "EDR-0-0-A",   "EDR-0-0-B",       // 0CE - 0CF
    "EDR-0-0-P",   "EDR-0-1-A",   "EDR-0-1-B",   "EDR-0-1-P",       // 0D0 - 0D3
    "EDR-0-2-A",   "EDR-0-2-B",   "EDR-0-2-P",   "EDR-0-3-A",       // 0D4 - 0D7
    "EDR-0-3-B",   "EDR-0-3-P",   "EDR-0-4-A",   "EDR-0-4-B",       // 0D8 - 0DB
    "EDR-0-4-P",   "EDR-0-5-A",   "EDR-0-5-B",   "EDR-0-5-P",       // 0DC - 0DF
    "EDR-1-0-A",   "EDR-1-0-B",   "EDR-1-0-P",   "EDR-1-1-A",       // 0E0 - 0E3
    "EDR-1-1-B",   "EDR-1-1-P",   "EDR-1-2-A",   "EDR-1-2-B",       // 0E4 - 0E7
    "EDR-1-2-P",   "EDR-1-3-A",   "EDR-1-3-B",   "EDR-1-3-P",       // 0E8 - 0EB
    "EDR-1-4-A",   "EDR-1-4-B",   "EDR-1-4-P",   "EDR-1-5-A",       // 0EC - 0EF
    "EDR-1-5-B",   "EDR-1-5-P",                                     // 0F0 - 0F1

                                  "RDR-0-00",    "RDR-0-01",        // 0F2 - 0F3
    "RDR-0-02",    "RDR-0-03",    "RDR-0-04",    "RDR-0-05",        // 0F4 - 0F7
    "RDR-0-06",    "RDR-0-07",    "RDR-0-08",    "RDR-0-09",        // 0F8 - 0FB
    "RDR-0-10",    "RDR-0-11",    "RDR-1-00",    "RDR-1-01",        // 0FC - 0FF
    "RDR-1-02",    "RDR-1-03",    "RDR-1-04",    "RDR-1-05",        // 100 - 103
    "RDR-1-06",    "RDR-1-07",    "RDR-1-08",    "RDR-1-09",        // 104 - 107
    "RDR-1-10",    "RDR-1-11",                                      // 108 - 109

                                  "SDR-0-A",     "SDR-0-B",         // 10A - 10B
    "SDR-0-P",     "SDR-0-S",     "SDR-1-A",     "SDR-1-B",         // 10C - 10F
    "SDR-1-P",     "SDR-1-S",     "SDR-2-A",     "SDR-2-B",         // 110 - 113
    "SDR-2-P",     "SDR-2-S",     "SDR-3-A",     "SDR-3-B",         // 114 - 117
    "SDR-3-P",     "SDR-3-S",                                       // 118 - 119

                                  "TDR-0-00-A",  "TDR-0-00-B",      // 11A - 11B
    "TDR-0-01-A",  "TDR-0-01-B",  "TDR-0-02-A",  "TDR-0-02-B",      // 11C - 11F
    "TDR-0-03-A",  "TDR-0-03-B",  "TDR-0-04-A",  "TDR-0-04-B",      // 120 - 123
    "TDR-0-05-A",  "TDR-0-05-B",  "TDR-0-06-A",  "TDR-0-06-B",      // 124 - 127
    "TDR-0-07-A",  "TDR-0-07-B",  "TDR-0-08-A",  "TDR-0-08-B",      // 128 - 12B
    "TDR-0-09-A",  "TDR-0-09-B",  "TDR-0-10-A",  "TDR-0-10-B",      // 12C - 12F
    "TDR-0-11-A",  "TDR-0-11-B",                                    // 130 - 131

                                  "TDR-1-00-A",  "TDR-1-00-B",      // 132 - 133
    "TDR-1-01-A",  "TDR-1-01-B",  "TDR-1-02-A",  "TDR-1-02-B",      // 134 - 137
    "TDR-1-03-A",  "TDR-1-03-B",  "TDR-1-04-A",  "TDR-1-04-B",      // 138 - 13B
    "TDR-1-05-A",  "TDR-1-05-B",  "TDR-1-06-A",  "TDR-1-06-B",      // 13C - 13F
    "TDR-1-07-A",  "TDR-1-07-B",  "TDR-1-08-A",  "TDR-1-08-B",      // 140 - 143
    "TDR-1-09-A",  "TDR-1-09-B",  "TDR-1-10-A",  "TDR-1-10-B",      // 144 - 147
    "TDR-1-11-A",  "TDR-1-11-B",                                    // 148 - 149

                                  "TDR-2-00-A",  "TDR-2-00-B",      // 14A - 14B
    "TDR-2-01-A",  "TDR-2-01-B",  "TDR-2-02-A",  "TDR-2-02-B",      // 14C - 14F
    "TDR-2-03-A",  "TDR-2-03-B",  "TDR-2-04-A",  "TDR-2-04-B",      // 150 - 153
    "TDR-2-05-A",  "TDR-2-05-B",  "TDR-2-06-A",  "TDR-2-06-B",      // 154 - 157
    "TDR-2-07-A",  "TDR-2-07-B",  "TDR-2-08-A",  "TDR-2-08-B",      // 158 - 15B
    "TDR-2-09-A",  "TDR-2-09-B",  "TDR-2-10-A",  "TDR-2-10-B",      // 15C - 15F
    "TDR-2-11-A",  "TDR-2-11-B",                                    // 160 - 161

                                  "TDR-3-00-A",  "TDR-3-00-B",      // 162 - 163
    "TDR-3-01-A",  "TDR-3-01-B",  "TDR-3-02-A",  "TDR-3-02-B",      // 164 - 167
    "TDR-3-03-A",  "TDR-3-03-B",  "TDR-3-04-A",  "TDR-3-04-B",      // 168 - 16B
    "TDR-3-05-A",  "TDR-3-05-B",  "TDR-3-06-A",  "TDR-3-06-B",      // 16C - 16F
    "TDR-3-07-A",  "TDR-3-07-B",  "TDR-3-08-A",  "TDR-3-08-B",      // 170 - 173
    "TDR-3-09-A",  "TDR-3-09-B",  "TDR-3-10-A",  "TDR-3-10-B",      // 174 - 177
    "TDR-3-11-A",  "TDR-3-11-B",                                    // 178 - 179

                                  "TDR-4-00-A",  "TDR-4-00-B",      // 17A - 17B
    "TDR-4-01-A",  "TDR-4-01-B",  "TDR-4-02-A",  "TDR-4-02-B",      // 17C - 17F
    "TDR-4-03-A",  "TDR-4-03-B",  "TDR-4-04-A",  "TDR-4-04-B",      // 180 - 183
    "TDR-4-05-A",  "TDR-4-05-B",  "TDR-4-06-A",  "TDR-4-06-B",      // 184 - 187
    "TDR-4-07-A",  "TDR-4-07-B",  "TDR-4-08-A",  "TDR-4-08-B",      // 188 - 18B
    "TDR-4-09-A",  "TDR-4-09-B",  "TDR-4-10-A",  "TDR-4-10-B",      // 18C - 18F
    "TDR-4-11-A",  "TDR-4-11-B",                                    // 190 - 191

                                  "TDR-5-00-A",  "TDR-5-00-B",      // 192 - 193
    "TDR-5-01-A",  "TDR-5-01-B",  "TDR-5-02-A",  "TDR-5-02-B",      // 194 - 197
    "TDR-5-03-A",  "TDR-5-03-B",  "TDR-5-04-A",  "TDR-5-04-B",      // 198 - 19B
    "TDR-5-05-A",  "TDR-5-05-B",  "TDR-5-06-A",  "TDR-5-06-B",      // 19C - 19F
    "TDR-5-07-A",  "TDR-5-07-B",  "TDR-5-08-A",  "TDR-5-08-B",      // 1A0 - 1A3
    "TDR-5-09-A",  "TDR-5-09-B",  "TDR-5-10-A",  "TDR-5-10-B",      // 1A4 - 1A7
    "TDR-5-11-A",  "TDR-5-11-B",                                    // 1A8 - 1A9

                                  "TDR-6-00-A",  "TDR-6-00-B",      // 1AA - 1AB
    "TDR-6-01-A",  "TDR-6-01-B",  "TDR-6-02-A",  "TDR-6-02-B",      // 1AC - 1AF
    "TDR-6-03-A",  "TDR-6-03-B",  "TDR-6-04-A",  "TDR-6-04-B",      // 1B0 - 1B3
    "TDR-6-05-A",  "TDR-6-05-B",  "TDR-6-06-A",  "TDR-6-06-B",      // 1B4 - 1B7
    "TDR-6-07-A",  "TDR-6-07-B",  "TDR-6-08-A",  "TDR-6-08-B",      // 1B8 - 1BB
    "TDR-6-09-A",  "TDR-6-09-B",  "TDR-6-10-A",  "TDR-6-10-B",      // 1BC - 1BF
    "TDR-6-11-A",  "TDR-6-11-B",                                    // 1C0 - 1C1

                                  "TDR-7-00-A",  "TDR-7-00-B",      // 1C2 - 1C3
    "TDR-7-01-A",  "TDR-7-01-B",  "TDR-7-02-A",  "TDR-7-02-B",      // 1C4 - 1C7
    "TDR-7-03-A",  "TDR-7-03-B",  "TDR-7-04-A",  "TDR-7-04-B",      // 1C8 - 1CB
    "TDR-7-05-A",  "TDR-7-05-B",  "TDR-7-06-A",  "TDR-7-06-B",      // 1CC - 1CF
    "TDR-7-07-A",  "TDR-7-07-B",  "TDR-7-08-A",  "TDR-7-08-B",      // 1D0 - 1D3
    "TDR-7-09-A",  "TDR-7-09-B",  "TDR-7-10-A",  "TDR-7-10-B",      // 1D4 - 1D7
    "TDR-7-11-A",  "TDR-7-11-B",                                    // 1D8 - 1D9

                                  "UDR-0-0-A",   "UDR-0-0-B",        // 1DA - 1DB
    "UDR-0-0-P",   "UDR-0-1-A",   "UDR-0-1-B",   "UDR-0-1-P",        // 1DC - 1DF
    "UDR-0-2-A",   "UDR-0-2-B",   "UDR-0-2-P",   "UDR-0-3-A",        // 1E0 - 1E3
    "UDR-0-3-B",   "UDR-0-3-P",   "UDR-0-4-A",   "UDR-0-4-B",        // 1E4 - 1E7
    "UDR-0-4-P",   "UDR-0-5-A",   "UDR-0-5-B",   "UDR-0-5-P",        // 1E8 - 1EB
    "UDR-1-0-A",   "UDR-1-0-B",   "UDR-1-0-P",   "UDR-1-1-A",        // 1EC - 1EF
    "UDR-1-1-B",   "UDR-1-1-P",   "UDR-1-2-A",   "UDR-1-2-B",        // 1F0 - 1F3
    "UDR-1-2-P",   "UDR-1-3-A",   "UDR-1-3-B",   "UDR-1-3-P",        // 1F4 - 1F7
    "UDR-1-4-A",   "UDR-1-4-B",   "UDR-1-4-P",   "UDR-1-5-A",        // 1F8 - 1FB
    "UDR-1-5-B",   "UDR-1-5-P",   "UNDEF",       "FEP-SERVER",       // 1FC - 1FF
};

//----------------------------------------------------------
//  AMSW Group Commands Node Maps
//----------------------------------------------------------

const int nmap_jinj[2][24] = {
    // JINF A side
    {JINF_T_2_A, JINF_T_3_A, JINF_U_1_A, JINF_T_0_A, SDR_1_A,    SDR_1_B,       // Link  0 -  5
     SDR_0_A,    SDR_0_B,    JINF_U_0_A, JINF_T_1_A, JINF_R_0_A, JINF_R_1_A,    // Link  6 - 11
     JINF_E_0_A, JINF_E_1_A, JLV1_A,     JLV1_B,     JINF_T_4_A, JINF_T_5_A,    // Link 12 - 17
     SDR_2_A,    SDR_2_B,    SDR_3_A,    SDR_3_B,    JINF_T_6_A, JINF_T_7_A},   // Link 18 - 23
    // JINF B side
    {JINF_T_2_B, JINF_T_3_B, JINF_U_1_B, JINF_T_0_B, SDR_1_A,    SDR_1_B,       // Link  0 -  5
     SDR_0_A,    SDR_0_B,    JINF_U_0_B, JINF_T_1_B, JINF_R_0_B, JINF_R_1_B,    // Link  6 - 11
     JINF_E_0_B, JINF_E_1_B, JLV1_A,     JLV1_B,     JINF_T_4_B, JINF_T_5_B,    // Link 12 - 17
     SDR_2_A,    SDR_2_B,    SDR_3_A,    SDR_3_B,    JINF_T_6_B, JINF_T_7_B},   // Link 18 - 23
};

const int nmap_jinfe[2][24] = {
    // E0 Crate
    {EDR_0_0_A, EDR_0_0_B, -1,        -1,        EDR_0_1_A, EDR_0_1_B,          // Link  0 -  5
     -1,        -1,        EDR_0_2_A, EDR_0_2_B, -1,        -1,                 // Link  6 - 11
     EDR_0_3_A, EDR_0_3_B, -1,        -1,        EDR_0_4_A, EDR_0_4_B,          // Link 12 - 17
     -1,        -1,        EDR_0_5_A, EDR_0_5_B, ETRG_0_A,  ETRG_0_B},          // Link 18 - 23
    // E1 Crate
    {EDR_1_0_A, EDR_1_0_B, -1,        -1,        EDR_1_1_A, EDR_1_1_B,          // Link  0 -  5
     -1,        -1,        EDR_1_2_A, EDR_1_2_B, -1,        -1,                 // Link  6 - 11
     EDR_1_3_A, EDR_1_3_B, -1,        -1,        EDR_1_4_A, EDR_1_4_B,          // Link 12 - 17
     -1,        -1,        EDR_1_5_A, EDR_1_5_B, ETRG_1_A,  ETRG_1_B},          // Link 18 - 23
};

const int nmap_jinfr[2][24] = {
    // R0 Crate
    {RDR_0_07,  RDR_0_05,  -1,        -1,        RDR_0_06, RDR_0_09,            // Link  0 -  5
     -1,        -1,        RDR_0_10,  RDR_0_11,  -1,       -1,                  // Link  6 - 11
     RDR_0_08,  RDR_0_02,  -1,        -1,        RDR_0_01, RDR_0_00,            // Link 12 - 17
     -1,        -1,        RDR_0_03,  RDR_0_04,  -1,       -1},                 // Link 18 - 23
    // R1 Crate
    {RDR_1_07,  RDR_1_11,  -1,        -1,        RDR_1_06, RDR_1_09,            // Link  0 -  5
     -1,        -1,        RDR_1_10,  RDR_1_05,  -1,       -1,                  // Link  6 - 11
     RDR_1_08,  RDR_1_02,  -1,        -1,        RDR_1_01, RDR_1_00,            // Link 12 - 17
     -1,        -1,        RDR_1_03,  RDR_1_04,  -1,       -1},                 // Link 18 - 23
};

const int nmap_jinfu[2][24] = {
    // E0 Crate
    {UDR_0_0_A, UDR_0_0_B, -1,        -1,        UDR_0_1_A, UDR_0_1_B,          // Link  0 -  5
     -1,        -1,        UDR_0_2_A, UDR_0_2_B, -1,        -1,                 // Link  6 - 11
     UDR_0_3_A, UDR_0_3_B, -1,        -1,        UDR_0_4_A, UDR_0_4_B,          // Link 12 - 17
     -1,        -1,        UDR_0_5_A, UDR_0_5_B, -1,        -1},                // Link 18 - 23
    // E1 Crate
    {UDR_1_0_A, UDR_1_0_B, -1,        -1,        UDR_1_1_A, UDR_1_1_B,          // Link  0 -  5
     -1,        -1,        UDR_1_2_A, UDR_1_2_B, -1,        -1,                 // Link  6 - 11
     UDR_1_3_A, UDR_1_3_B, -1,        -1,        UDR_1_4_A, UDR_1_4_B,          // Link 12 - 17
     -1,        -1,        UDR_1_5_A, UDR_1_5_B, -1,        -1},                // Link 18 - 23
};

const int nmap_jinft[8][24] = {
    // T0 Crate
    {TDR_0_00, TDR_0_01, TDR_0_02, TDR_0_03, TDR_0_04, TDR_0_05,
     TDR_0_06, TDR_0_07, TDR_0_08, TDR_0_09, TDR_0_10, TDR_0_11,
     TDR_0_12, TDR_0_13, TDR_0_14, TDR_0_15, TDR_0_16, TDR_0_17,
     TDR_0_18, TDR_0_19, TDR_0_20, TDR_0_21, TDR_0_22, TDR_0_23},
    // T1 Crate
    {TDR_1_00, TDR_1_01, TDR_1_02, TDR_1_03, TDR_1_04, TDR_1_05,
     TDR_1_06, TDR_1_07, TDR_1_08, TDR_1_09, TDR_1_10, TDR_1_11,
     TDR_1_12, TDR_1_13, TDR_1_14, TDR_1_15, TDR_1_16, TDR_1_17,
     TDR_1_18, TDR_1_19, TDR_1_20, TDR_1_21, TDR_1_22, TDR_1_23},
    // T2 Crate
    {TDR_2_00, TDR_2_01, TDR_2_02, TDR_2_03, TDR_2_04, TDR_2_05,
     TDR_2_06, TDR_2_07, TDR_2_08, TDR_2_09, TDR_2_10, TDR_2_11,
     TDR_2_12, TDR_2_13, TDR_2_14, TDR_2_15, TDR_2_16, TDR_2_17,
     TDR_2_18, TDR_2_19, TDR_2_20, TDR_2_21, TDR_2_22, TDR_2_23},
    // T3 Crate
    {TDR_3_00, TDR_3_01, TDR_3_02, TDR_3_03, TDR_3_04, TDR_3_05,
     TDR_3_06, TDR_3_07, TDR_3_08, TDR_3_09, TDR_3_10, TDR_3_11,
     TDR_3_12, TDR_3_13, TDR_3_14, TDR_3_15, TDR_3_16, TDR_3_17,
     TDR_3_18, TDR_3_19, TDR_3_20, TDR_3_21, TDR_3_22, TDR_3_23},
    // T4 Crate
    {TDR_4_00, TDR_4_01, TDR_4_02, TDR_4_03, TDR_4_04, TDR_4_05,
     TDR_4_06, TDR_4_07, TDR_4_08, TDR_4_09, TDR_4_10, TDR_4_11,
     TDR_4_12, TDR_4_13, TDR_4_14, TDR_4_15, TDR_4_16, TDR_4_17,
     TDR_4_18, TDR_4_19, TDR_4_20, TDR_4_21, TDR_4_22, TDR_4_23},
    // T5 Crate
    {TDR_5_00, TDR_5_01, TDR_5_02, TDR_5_03, TDR_5_04, TDR_5_05,
     TDR_5_06, TDR_5_07, TDR_5_08, TDR_5_09, TDR_5_10, TDR_5_11,
     TDR_5_12, TDR_5_13, TDR_5_14, TDR_5_15, TDR_5_16, TDR_5_17,
     TDR_5_18, TDR_5_19, TDR_5_20, TDR_5_21, TDR_5_22, TDR_5_23},
    // T6 Crate
    {TDR_6_00, TDR_6_01, TDR_6_02, TDR_6_03, TDR_6_04, TDR_6_05,
     TDR_6_06, TDR_6_07, TDR_6_08, TDR_6_09, TDR_6_10, TDR_6_11,
     TDR_6_12, TDR_6_13, TDR_6_14, TDR_6_15, TDR_6_16, TDR_6_17,
     TDR_6_18, TDR_6_19, TDR_6_20, TDR_6_21, TDR_6_22, TDR_6_23},
    // T7 Crate
    {TDR_7_00, TDR_7_01, TDR_7_02, TDR_7_03, TDR_7_04, TDR_7_05,
     TDR_7_06, TDR_7_07, TDR_7_08, TDR_7_09, TDR_7_10, TDR_7_11,
     TDR_7_12, TDR_7_13, TDR_7_14, TDR_7_15, TDR_7_16, TDR_7_17,
     TDR_7_18, TDR_7_19, TDR_7_20, TDR_7_21, TDR_7_22, TDR_7_23},
};

const int *jinf_nmap[24] = {
    nmap_jinft[2], nmap_jinft[3], nmap_jinfu[1], nmap_jinft[0],
    NULL,          NULL,          NULL,          NULL,
    nmap_jinfu[0], nmap_jinft[1], nmap_jinfr[0], nmap_jinfr[1],
    nmap_jinfe[0], nmap_jinfe[1], NULL,          NULL,
    nmap_jinft[4], nmap_jinft[5], NULL,          NULL,
    NULL,          NULL,          nmap_jinft[6], nmap_jinft[7],
};

const int subd_index[24] = {
    2,  3,  1,  0,  1,  1,  0,  0,  0,  1,  0,  1,
    0,  1, -1, -1,  4,  5,  2,  2,  3,  3,  6,  7,
};
