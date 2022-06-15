/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2003 by PADSOFT, Inc.                        */
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

//  mcast.h   Routines to support multicasting UDP

//  Revision History
//    Oct 2003 by Peter Dennett
//      - Initial development at PADSOFT
//    Sep 2010 by Peter Dennett
//      - Resurrected from MDCSoftware project

#ifndef _MCAST_H
#define _MCAST_H

#include "aallib.h"
#include "abi.h"

#ifdef OS_WIN32
# include <winsock2.h>
# include <ws2tcpip.h>

struct ip_mreqn
{
  struct in_addr imr_multiaddr;
  struct in_addr imr_address;
  int            imr_ifindex;
};

int if_nametoindex(char* InterfaceName);

#else
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <resolv.h>
# include <netdb.h>
# include <sys/utsname.h>
# include <arpa/inet.h>
#include <net/if.h>
# include <signal.h>
# include <unistd.h>
# include <errno.h>
# include <fcntl.h>
# define closesocket close
#endif // OS_WIN32

bool MCastOpen(int *Socket, char *ListenPort, char *DestAddr, char *DestPort,
               struct sockaddr_in *Addr);
bool MCastOpenTo(int *Socket, struct sockaddr_in *Addr, char *Address);
bool MCastOpenFrom(int *Socket, struct sockaddr_in *Addr, char *Address);
bool MCastClose(int Socket);
bool MCastOpenFromMArgs(int *Socket, struct sockaddr_in *Addr, char *Address, 
                       int32 port, char *interfacex);
#endif // _MCAST_H
