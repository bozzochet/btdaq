/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2009 by PADSOFT, Inc.                        */
/*                                                                           */
/*    Developed for use on AMS-2, An International Space Station payload     */
/*                      searching for missing matter                         */
/*                                                                           */
/*                         No rights reserved                                */
/*                                                                           */
/* Developed by:        Peter Armistead Dennett                              */
/*                            PADSOFT, Inc                                   */
/*                          61 Harbor Lane                                   */
/*                        Kemah, Texas 77565                                 */
/*                          (281) 334 3800                                   */
/*                      http://www.padsoft.com                               */
/*                                                                           */
/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/

//  EHSpd.h   Defines for the EHS Protocol Data handling

//  Revision History
//    Dec 2004 by Peter Dennett
//      - Initial version at PADSOFT (as ehsheaders.h)
//    Aug 2009 by Peter Dennett
//      - Added GSE and CDP secondary headers
//    Nov 2009 by Peter Dennett
//      - 

#ifndef _EHSpd_H
#define _EHSpd_H

#ifdef __cplusplus
extern "C" {
#endif

//~----------------------------------------------------------------------------

// Primary EHS header as defined in SSP50305-01 Rev B Table 8-1.

struct _EHS_HEADER 
{
  //-
  unsigned projid:4;              // Project Identfier, set to 3.
  unsigned identifier:4;          // EHS Protocol version ID, set to 2.
  //-
  unsigned support_mode:4;        // Mode of the telemetry: 0001 Real-time
  unsigned data_mode:4;           // Data mode: 0001 Flight
                                  //            0010 Ground test
                                  //            0011 Simulation
                                  //            0100 Verification/validation
                                  //            0101 Development
                                  //            0110 Training
  //-
  unsigned increment:8;           // The ISS increment number
  //-
  unsigned sec_pro_type:8;        // Secondary protocol header type: 
                                  //            0x03 - Pseudo
                                  //            0x0B - PDSS payload CCSDS packet
  unsigned grt_year:8;            // Ground receipt time (years since 1900)
  unsigned grt_yearday:16;
  unsigned grt_hour:8;
  unsigned grt_minute:8;
  unsigned grt_second:8;
  //-
  unsigned grt_tenthsofsecond:4;
  unsigned sign:1;
  unsigned hold_condition:1;
  unsigned unusedbit01:1;
  unsigned grt_status:1;
  //-
  unsigned  misc_status:2;
  unsigned  unusedbit02:6;
  //-
  unsigned unusedbyte01:16;
  unsigned hosc_size:16;          // Length of the entire HOSC packet in
                                  // bytes. Includes both primary and
                                  // secondary headers.
} __attribute__((packed));

typedef struct _EHS_HEADER EHS_HEADER, *pEHS_HEADER;

//~----------------------------------------------------------------------------

// Primary EHS secondary header as defined in SSP50305-01 Rev D Table 8.2.2.1.

struct _EHS_SECONDARY_HEADER_PDSS
{
  unsigned unused1:1;
  unsigned PktSeqErr:1;           // Packet sequence error indicator
  unsigned VCDU:1;                // VCDU sequence error indicator
  unsigned unused2:3;
  unsigned VersionNumber:2;       // Set to 00
  //-
  unsigned VCDUseqnoB1:8;
  //-
  unsigned VCDUseqnoB2:8;
  //-
  unsigned VCDUseqnoB3:8;
  //-
  unsigned unused3:7;
  unsigned DSIDccsds:1;
  //-
  unsigned unused4:8;
  //-
  unsigned unused5:3;
  unsigned unused6:1;
  unsigned DSIDcore:1;
  unsigned DSIDapidMSB:3;
  //-
  unsigned DSIDapidLSB:8;
  //-
  unsigned VCIDmsb:8;
  //-
  unsigned VCIDlsb:8;
  //-
  unsigned PDSSkeyMSB:8;
  //-
  unsigned PDSSkeyLSB:8;
  //-
} __attribute__((packed));

typedef struct _EHS_SECONDARY_HEADER_PDSS EHS_SECONDARY_HEADER_PDSS,
               *pEHS_SECONDARY_HEADER_PDSS;

//~----------------------------------------------------------------------------

// EHS secondary header for CORE CCSDS packets (reverse engineered from dumps)

struct _EHS_SECONDARY_HEADER_CORE
{
  //-
  unsigned unknown1:8;
  //-
  unsigned unknown2:8;
  //-
  unsigned unknown3:8;
  //-
  unsigned unknown4:8;
  //-
  unsigned unknown5:8;
  //-
  unsigned unknown6:8;
  //-
  unsigned DSIDapidMSB:3;         // APID
  unsigned DSIDcore:1;            // 0:Core 1:payload
  unsigned DSIDpacketid:1;     
  unsigned unused6:3;
  //-
  unsigned DSIDapidLSB:8;         // APID
  //-
  unsigned unknown7:8;
  //-
  unsigned unknown8:8;
  //-
  unsigned unknown9:8;
  //-
  unsigned unknown10:8;
} __attribute__((packed));

typedef struct _EHS_SECONDARY_HEADER_CORE EHS_SECONDARY_HEADER_CORE,
               *pEHS_SECONDARY_HEADER_CORE;

//~----------------------------------------------------------------------------

struct _EHS_SECONDARY_HEADER_GSE
{
  unsigned unused1:6;
  unsigned RequestType:1;         // 0=POIC, 1=Remote User
  unsigned RoutingFlag:1;         // 0=TNS routed, 1= PDSS routed
  //-
  unsigned unused2:8;
  //-
  unsigned WorkstationID:16;
  //-
  unsigned unused3:7;   
  unsigned DSID:1;                // 0=CCSDS packet, 1=BPDU
  //-
  unsigned unused4:8;
  //-
  unsigned DSID_APID_MSB:3;
  unsigned DSID_Payload:1;        // 0=Core (systems) data, 1=Payload data
  unsigned DSID_GSE:1;            // 1=POIC created packet (set to 1)
  unsigned unused5:3;
  //-
  unsigned DSID_APID_LSB:8;
  //-
  unsigned unused6:16;
  unsigned unused7:16;
  unsigned UserID:16;
  unsigned ProcessID:16;
} __attribute__((packed));

typedef struct _EHS_SECONDARY_HEADER_GSE EHS_SECONDARY_HEADER_GSE,
               *pEHS_SECONDARY_HEADER_GSE;

//~----------------------------------------------------------------------------

struct _EHS_SECONDARY_HEADER_CDP
{
  unsigned unused1:16;
  //-
  unsigned WorkstationID:16;
  //-
  unsigned unused2:16;
  unsigned unused3:16;
  unsigned unused4:16;
  unsigned unused5:16;

  unsigned UserID:16;
  unsigned ProcessID:16;
} __attribute__((packed));

typedef struct _EHS_SECONDARY_HEADER_CDP EHS_SECONDARY_HEADER_CDP,
               *pEHS_SECONDARY_HEADER_CDP;

//~----------------------------------------------------------------------------

//  Secondary CCSDS header for UDSM packets

struct _EHS_SECONDARY_HEADER_UDSM
{
  //-                             [0]
  unsigned VersionNumber:2;       // Set to 00
  unsigned DataStatus:6;          // Unused, set to zero
  //-                             [1]
  unsigned unused1:8;             // VCDU seqno, not used for UDSM
  //-                             [2]
  unsigned unused2:8;
  //-                             [3]
  unsigned unused3:8;
  //-                             [4]
  unsigned unused4:7;
  unsigned ccsds_bpdu:1;          // 0:CCSDS, 1:BPDU
  //-                             [5]
  unsigned unused5:8;
  //-                             [6]
  unsigned DSIDapidMSB:3;         // APID
  unsigned DSIDcore:1;            // 0:Core 1:payload
  unsigned DSIDpacketid:1;        // Set to 0
  unsigned unused6:3;
  //-                             [7]
  unsigned DSIDapidLSB:8;         // APID
  //-                             [8-11]
  unsigned char unused7[4];
} __attribute__((packed));

typedef struct _EHS_SECONDARY_HEADER_UDSM EHS_SECONDARY_HEADER_UDSM,
               *pEHS_SECONDARY_HEADER_UDSM;

//~----------------------------------------------------------------------------

//  UDSM packet payload

struct _EHS_UDSM_PAYLOAD
{
  //-                             [0]
  unsigned unused1:7;
  unsigned ccsds_bpdu:1;          // 0:CCSDS, 1:BPDU
  //-                             [1]
  unsigned unused2:8;
  //-                             [2]
  unsigned DSIDapidMSB:3;         // APID
  unsigned DSIDcore:1;            // 0:Core 1:payload
  unsigned DSIDpacketid:1;        // Set to 0
  unsigned unused3:3;
  //-                             [3]
  unsigned DSIDapidLSB:8;         // APID
  //-                             [4]
  unsigned grt_s_year:8;            // Start Ground receipt time (years since 1900)
  unsigned grt_s_yearday:16;
  unsigned grt_s_hour:8;
  unsigned grt_s_minute:8;
  unsigned grt_s_second:8;
  //-                             [10]
  unsigned grt_e_year:8;            // End Ground receipt time (years since 1900)
  unsigned grt_e_yearday:16;
  unsigned grt_e_hour:8;
  unsigned grt_e_minute:8;
  unsigned grt_e_second:8;
  //-                             [16]
  unsigned unused4:16;
  //-                             [18]
  unsigned CntValidPackets:16;      // Modulo 65535 (counter resets)
  //-                             [20]
  unsigned CntVCDUSeqErrs:16;       // Number of VCDU sequence errors during period
  //-                             [22]
  unsigned CntPktSeqErrs:16;        // Number of packet sequence errors during period
  //-                             [24]
  unsigned CntPktLenErrs:16;        // Number of packet length errors during period
  //-                             [26] 1: Actual LOS, 2: Scheduled end of data, 3: Operator request
  unsigned UDSM_event:8;
  //-                             [27]
  unsigned CntValidPktRollOvr:8;    // Number of times CntValidPackets rolled over
} __attribute__((packed));

typedef struct _EHS_UDSM_PAYLOAD EHS_UDSM_PAYLOAD,
               *pEHS_UDSM_PAYLOAD;

//~----------------------------------------------------------------------------

//  RPSM packet payload - See PGUIDD SSP50305 Vol 1 Section 8.5

struct _EHS_RPSM_PAYLOAD
{
  //-                             [0-7]
  unsigned long long SetSize;        // Number of bytes
  //-                             [8-15]
  unsigned long long PacketCount;    // Number of packets
  //-                             [16]
  unsigned unused1:8;
  //-                             [17]
  unsigned unused2:8;
  //-                             [18]
  unsigned unused3:8;
  //-                             [19]
  unsigned unused4:8;
  //-                             [20]
  unsigned unused5:5;
  unsigned TimeRef:1;                // 0:Embedded 1:GroundReceipt
  unsigned RetrievalStatus:2;        // 0:Success 1:Cancelled 2:Failed
  //-                             [21]
  unsigned rt_s_year:8;              // Start time (years since 1900) of data request
  unsigned rt_s_yearday:16;
  unsigned rt_s_hour:8;
  unsigned rt_s_minute:8;
  unsigned rt_s_second:8;
  //-                             [27]
  unsigned rt_e_year:8;              // End time (years since 1900) of data request
  unsigned rt_e_yearday:16;
  unsigned rt_e_hour:8;
  unsigned rt_e_minute:8;
  unsigned rt_e_second:8;
  //-                             [33]
  unsigned dt_s_year:8;              // Start time of first packet transfered
  unsigned dt_s_yearday:16;
  unsigned dt_s_hour:8;
  unsigned dt_s_minute:8;
  unsigned dt_s_second:8;
  //-                             [39]
  unsigned dt_e_year:8;              // End time of last time transfered
  unsigned dt_e_yearday:16;
  unsigned dt_e_hour:8;
  unsigned dt_e_minute:8;
  unsigned dt_e_second:8;
  //-
} __attribute__((packed));

typedef struct _EHS_RPSM_PAYLOAD EHS_RPSM_PAYLOAD,
               *pEHS_RPSM_PAYLOAD;

//~----------------------------------------------------------------------------

//  Primary CCSDS header

struct _CCSDS_HEADER
{
  unsigned   apid_msb:3;
  unsigned   flag:1;
  unsigned   type:1;
  unsigned   version:3;
  //-
  unsigned   apid_lsb:8;
  //-
  unsigned   pkt_seq_count_msb:6;
  unsigned   sequence_flag:2;
  //-
  unsigned   pkt_seq_count_lsb:8;
  //-
  unsigned   packet_length_msb:8;
  //-
  unsigned   packet_length_lsb:8;
} __attribute__((packed));

typedef struct _CCSDS_HEADER CCSDS_HEADER, *pCCSDS_HEADER;

//~----------------------------------------------------------------------------

//  Secondary CCSDS header

struct _CCSDS_SECONDARY_HEADER
{
  unsigned   coarse_time_msb:16;
  //-
  unsigned   coarse_time_lsb:16;
  //-
  unsigned   fine_time:8;
  //-
  unsigned   packet_type:4;
  unsigned   zoe_tlm:1;
  unsigned   checkword_indicator:1;
  unsigned   time_id:2;
  //-
  unsigned   version_id:16;
  //-
  unsigned   cycle_counter:16;
} __attribute__((packed));

typedef struct _CCSDS_SECONDARY_HEADER CCSDS_SECONDARY_HEADER,
              *pCCSDS_SECONDARY_HEADER;

//~----------------------------------------------------------------------------

//  Secondary CCSDS header for GSE packets

struct _CCSDS_SECONDARY_HEADER_GSE
{
  unsigned   unused_0:3;
  unsigned   embedded_time:1;    // 0:Embedded 1:Packet generation time
  unsigned   select_merge:1;     // 0:Off 1:On
  unsigned   format_id:3;        // Set to value assigned in TLB
} __attribute__((packed));

typedef struct _CCSDS_SECONDARY_HEADER_GSE CCSDS_SECONDARY_HEADER_GSE,
               *pCCSDS_SECONDARY_HEADER_GSE;

//~----------------------------------------------------------------------------

char *EHSpdModeName(int Mode);
int EHSpdMode(char *Name);
char *EHSpdOpsModeName(int Mode);
int EHSpdOpsMode(char *Name);

void EHSpdLogPacket(int8 *GSE, int32 GSEBytes, bool RIC, int32 Debug);
void EHSpdLogCDPPacket(int8 *GSE, int32 GSEBytes, bool RIC, int32 Debug);

int EHSpdFrameToGSE(int8 *Frame, int32 FrameBytes, int8 *GSE, int32 *GSEBytes);
int EHSpdGSEtoFrame(int8 *GSE, int32 GSEBytes, int8 *Frame, int32 *FrameBytes);

int EHSpdCDPtoFrame(int8 *GSE, int32 GSEBytes, int8 *Frame, int32 *FrameBytes, bool RIC);
int EHSpdCDPConfigure(bool RIC, int8 *buff, int32 *Bytes, int *nPDLrtn);
int EHSpdCDPConfigReply(bool RIC, int8 *buff, int32 Bytes, int Debug);

#ifdef __cplusplus
}
#endif

#endif // _EHSpd_H
