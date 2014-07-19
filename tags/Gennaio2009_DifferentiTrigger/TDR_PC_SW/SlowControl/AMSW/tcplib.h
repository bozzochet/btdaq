// file tcplib.h
//
// Stolen from Jinghui Zhang and reworked
//
// see file tcplib.c
//
// A.Lebedev Mar-2005...
//

#ifndef _TCPLIB_H
#define _TCPLIB_H

//~----------------------------------------------------------------------------

#include "mylib.h"

#define CAN_SERVER_PORT         10224    // advised by AE
#define LISTEN_QUEUE_LENGTH        10
#define CONNECT_TIME_OUT           30
#define PETER_TCP_PACKET_LENGTH  4096

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void pack8_AMS_Block(pAMSBlock pBlock, int8 *Packet);
void unpack8_AMS_Block(int8 *Packet, pAMSBlock pBlock);
void get_Packet_Length(int8 *Packet, int *Packet_Length);

void pack8_AMS_Block_into_Peter_TCP_frame(pAMSBlock pBlock, int8 *Packet);
void unpack8_AMS_Block_from_Peter_TCP_frame(int8 *Packet, pAMSBlock pBlock);

void TCP_SND_RCV(pAMSBlock pRequest, pAMSBlock pReply);

void open_TCP_connection(char *CS_name, int CS_port, int16 *err);
void send_command_via_TCP(pAMSBlock pRequest, pAMSBlock pReply);
void close_TCP_connection(void);

//~============================================================================

#endif // _TCPLIB_H
