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

//  mcast.c   Routines to support multicasting UDP

//  Revision History
//    Oct 2003 by Peter Dennett
//      - Initial development at PADSOFT
//    Apr 2004 by Peter Dennett
//      - Changed BASE_PORT from 7000 to 62000 due to problems at CERN
//    Sep 2010 by Peter Dennett
//      - Resurrected from MDCSoftware project

#include "mcast.h"
#include "logfile.h"

#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <ctype.h>

int inet_aton (const char *cp, struct in_addr *addr);

////////////////////////////////////////////////////////////////////////////
//-Extern: MCastOpenFrom
bool MCastOpenFrom(int *Socket, struct sockaddr_in *Addr, char *Address)
//-Synopsis:    Create a multicast socket to receive from
//-Returns:     TRUE if no errors, else FALSE
//-ARG_U:       Socket         Pointer to location to store socket handle
//-ARG_U:       Addr           An area into which a sendto address is built.
//-ARG_I:       Address        A string of the form IPAddress:port:interface
//-
//-                            IPAddress is a dotted address.
//-                            port is the port we are listening (decimal)
//-                            interface is an interface name such as eth0
//-
//-                            Embedded spaces are not allowed
//-Description: Create a multcast socket to receive from
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
char *cp, *p1, *p2, *p3, nil=0;
int sd, port;
#ifndef OS_WIN32
const int on=1;
#else
const char on=1;
#endif
struct ip_mreqn mreq;
char dport[] = "62000";
char dip[]   = "224.0.0.24";
char diface[20]   = "eth0";
char *strx;

  if ( ! Address)
    Address = &nil;

  strx = strdup(Address);

  p1 = NULL;
  p2 = NULL;
  p3 = NULL;

  cp = strx;

  while ( ! p1)
  {
    if      ( ! *cp) p1 = &nil;
    else if (isspace(*cp)) cp++;
    else if (*cp == ':') p1 = &nil;
    else p1 = cp++;
  }

  while ((*cp) && (! isspace(*cp)) && (*cp != ':')) cp++;
  if (*cp)
    *cp++ = 0;

  while ( ! p2)
  {
    if      ( ! *cp) p2 = &nil;
    else if (isspace(*cp)) cp++;
    else if (*cp == ':') p2 = &nil;
    else p2 = cp++;
  }

  while ((*cp) && (! isspace(*cp)) && (*cp != ':')) cp++;
  if (*cp)
    *cp++ = 0;

  while ( ! p3)
  {
    if      ( ! *cp) p3 = &nil;
    else if (isspace(*cp)) cp++;
    else if (*cp == ':') p3 = &nil;
    else p3 = cp++;
  }

  while ((*cp) && (! isspace(*cp)) && (*cp != ':')) cp++;
  if (*cp)
    *cp++ = 0;

  if ( ! *p1)
    p1 = dip;

  if ( ! *p2)
    p2 = dport;

  if ( ! *p3) {
    // Modified by X.Cai to detect default interface
    int i;

    // Search through all inetrfaces
    for ( i = 1; ; i++ ) {
        // Find interface name one by one
        if ( if_indextoname(i, diface) == NULL ) {
            // No more interface, use "eth0" as default
            strcpy(diface, "eth0");
            break;
        }

        // Use the first interface which is not "lo"
        if ( strcasecmp(diface, "lo") != 0 ) break;
    }

    p3 = diface;
  }

#if 0
  LogPrintf("MCastOpenFrom: IP: [%s] PORT: [%s] IF: [%s]\n", p1, p2, p3);
#endif

  sd = socket(AF_INET, SOCK_DGRAM, 0);
  if ( sd < 0 )
  {
    LogError("MCastOpenFrom: socket failed %d\n", sd);
    return FALSE;
  }

  port = strtol(p2, NULL, 0);

  if (port == 0)
  {
    LogError("MCastOpenFrom: port '%s' invalid\n", p2);
    closesocket(sd);
    return FALSE;
  }

  memset(Addr, 0, sizeof(*Addr));
  Addr->sin_family = AF_INET;
  Addr->sin_port = htons(port);
  Addr->sin_addr.s_addr = INADDR_ANY;

  if ( setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) != 0 )
  {
    closesocket(sd);
    LogError("MCastOpenFrom: Can't reuse address/ports\n");
    return FALSE;
  }

  if ( bind(sd, (struct sockaddr*)Addr, sizeof(*Addr)) != 0 )
  {
    closesocket(sd);
    LogError("MCastOpen: bind failed\n");
    return FALSE;
  }

  memset(&mreq, 0, sizeof mreq);

#if 0
  mreq.imr_multiaddr = inet_addr(p1);
  if (mreq.imr_multiaddr == INADDR_NONE)
  {
    closesocket(sd);
    LogError("MCastOpenFrom: Address '%s' is invalid\n", p1);
    return FALSE;
  }
#else
  if ( inet_aton(p1, &mreq.imr_multiaddr) == 0 )
  {
    closesocket(sd);
    LogError("MCastOpen: address (%s) bad\n", p1);
    return FALSE;
  }
#endif

  mreq.imr_address.s_addr = INADDR_ANY;
  mreq.imr_ifindex = if_nametoindex(p3);
  if(mreq.imr_ifindex == 0)
  {
    closesocket(sd);
    LogError("MCastOpen: interface (%s) not found\n", p3);
    return FALSE;
  }

#if 0
  p1 = strdup(inet_ntoa(mreq.imr_multiaddr));
  LogNormal("mcast: Add membership to %s on interface %s\n", p1, p3);
  free(p1);
#endif

  if ( setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*) &mreq, sizeof(mreq)) != 0 )
  {
    closesocket(sd);
    LogError("MCastOpen: Join multicast failed\n");
    return FALSE;
  }

//  We like non blocking sockets

#ifndef OS_WIN32
  fcntl(sd, F_SETFL, O_NONBLOCK);
  signal(SIGPIPE, SIG_IGN);
#else
  {
    // If iMode = 0, blocking is enabled;
    // If iMode != 0, non-blocking mode is enabled.
    int32 iMode = 1;

    ioctlsocket(sd, FIONBIO, &iMode);
  }
#endif

  free(strx);
  *Socket = sd;

  return TRUE;
}

#ifdef OS_WIN32
int if_nametoindex(char *in)
{
  while (*in && ( ! isdigit(*in))) in++;

  return atoi(in);

  return 0;
}
#endif

bool MCastOpenFromMArgs(int *Socket, struct sockaddr_in *Addr, char *Address,
	int32 port, char *interfacex){
	/* I want "mcastaddress:port:interface", ex. "239.1.2.3:5600:eth0" */
	char tmp[80];
	sprintf(tmp,"%s:%d:%s",Address,port,interfacex);
	return MCastOpenFrom(Socket,Addr,tmp);
}

////////////////////////////////////////////////////////////////////////////
//-Extern: MCastOpenTo
bool MCastOpenTo(int *Socket, struct sockaddr_in *Addr, char *Address)
//-Synopsis:    Create a multicast socket to send to
//-Returns:     TRUE if no errors, else FALSE
//-ARG_U:       Socket         Pointer to location to store socket handle
//-ARG_U:       Addr           An area into which a sendto address is built.
//-ARG_I:       Address        A string of the form IPAddress:Port:ttl:interface
//-
//-                            IPAddress can be a host name or dotted address.
//-                            port is the port we are listening (decimal)
//-                            The ttl value (time to live) must be
//-                            decimal.  This is the number of router hops
//-                            that any multicast packet will live.  TTL has
//-                            no meaning for receives.
//-                            interface is an interface name such as eth0
//-
//-                            Embedded spaces are not allowed
//-Description: Create a multcast socket to send to
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
char *cp, *p1, *p2, *p3, *p4, nil=0;
int sd, port;
#ifndef OS_WIN32
const int on=1;
#else
const char on=1;
#endif
struct ip_mreqn mreq;
char dport[] = "62000";
char dip[]   = "224.0.0.24";
char dttl[]   = "1";
char diface[] = "eth0";
char *strx;
int8 ttl;
int8 one = 1;
int err;

  if ( ! Address)
    Address = &nil;

  strx = strdup(Address);

  p1 = NULL;
  p2 = NULL;
  p3 = NULL;
  p4 = NULL;

  cp = strx;

  while ( ! p1)
  {
    if      ( ! *cp) p1 = &nil;
    else if (isspace(*cp)) cp++;
    else if (*cp == ':') p1 = &nil;
    else p1 = cp++;
  }

  while ((*cp) && (! isspace(*cp)) && (*cp != ':')) cp++;
  if (*cp)
    *cp++ = 0;

  while ( ! p2)
  {
    if      ( ! *cp) p2 = &nil;
    else if (isspace(*cp)) cp++;
    else if (*cp == ':') p2 = &nil;
    else p2 = cp++;
  }

  while ((*cp) && (! isspace(*cp)) && (*cp != ':')) cp++;
  if (*cp)
    *cp++ = 0;

  while ( ! p3)
  {
    if      ( ! *cp) p3 = &nil;
    else if (isspace(*cp)) cp++;
    else if (*cp == ':') p3 = &nil;
    else p3 = cp++;
  }

  while ((*cp) && (! isspace(*cp)) && (*cp != ':')) cp++;
  if (*cp)
    *cp++ = 0;

  while ( ! p4)
  {
    if      ( ! *cp) p4 = &nil;
    else if (isspace(*cp)) cp++;
    else if (*cp == ':') p4 = &nil;
    else p4 = cp++;
  }

  while ((*cp) && (! isspace(*cp)) && (*cp != ':')) cp++;
  if (*cp)
    *cp++ = 0;

  if ( ! *p1)
    p1 = dip;

  if ( ! *p2)
    p2 = dport;

  if ( ! *p3)
    p3 = dttl;

  if ( ! *p4)
    p4 = diface;

#if 0
  LogPrintf("MCastOpenTo: IP: [%s] PORT: [%s] TTL: [%s] IF: [%s]\n",
    p1, p2, p3, p4);
#endif

  sd = socket(AF_INET, SOCK_DGRAM, 0);
  if ( sd < 0 )
  {
    LogError("MCastOpenTo: socket failed %d\n", sd);
    return FALSE;
  }

  port = strtol(p2, NULL, 0);

  if (port == 0)
  {
    LogError("MCastOpenTo: failed to find port '%s'\n", p2);
    return FALSE;
  }

  ttl =  strtol(p3, NULL, 0);

  memset(Addr, 0, sizeof(*Addr));
  Addr->sin_family = AF_INET;
  Addr->sin_port = htons(port);

  if ( inet_aton(p1, &Addr->sin_addr) == 0 )
  {
    closesocket(sd);
    LogError("MCastOpenTo: inet_aton failed (%s)\n", p1);
    return FALSE;
  }

  memset(&mreq, 0, sizeof mreq);
  mreq.imr_address.s_addr = INADDR_ANY;
  mreq.imr_ifindex = if_nametoindex(p4);

  err = setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (void*)&mreq,
                   sizeof(mreq));
  if (err != 0)
    LogWarning("McastOpenTo: setsockopt IP_MULTICAST_IF failed\n");

  // Set multicast packet TTL; default TTL is 1

  err = setsockopt(sd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl,
                   sizeof(unsigned char));
  if (err != 0)
    LogWarning("McastOpenTo: setsockopt IP_MULTICAST_TTL %d failed\n", ttl);

  // send multicast traffic to myself too

  err = setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP,
                   &one, sizeof(unsigned char));
  if (err != 0)
    LogWarning("McastOpenTo: setsockopt IP_MULTICAST_LOOP failed\n");

  if ( setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) != 0 )
  {
    closesocket(sd);
    LogError("MCastOpenTo: Can't reuse address/ports\n");
    return FALSE;
  }

//
  {
  int bufsize;
  unsigned int size = sizeof(bufsize);
  int rtn;

    rtn = getsockopt(sd, SOL_SOCKET, SO_SNDBUF, &bufsize, &size);
    if (rtn < 0)
      LogError("MCastOpenTo: Failed to get buffer size for multicast send\n");
    else
      LogNormal("MCastOpenTo: Socket buffer size for multicast send is %d\n", bufsize);

#if 1
    bufsize = 512*1000;
    rtn = setsockopt(sd, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize));
    if (rtn < 0)
      LogError("MCastOpenTo: Failed to set buffer size for multicast send\n");
    else
    {
      rtn = getsockopt(sd, SOL_SOCKET, SO_SNDBUF, &bufsize, &size);
      if (rtn < 0)
        LogError("MCastOpenTo: Failed to get new buffer size for multicast send\n");
      else
        LogNormal("MCastOpenTo: Socket buffer new size for multicast send is %d\n", bufsize);
    }
#endif
  }

//  We like non blocking sockets (well not on sendto for LINUX we dont)

#ifndef OS_WIN32

#if 0
  fcntl(sd, F_SETFL, O_NONBLOCK);
#endif

  signal(SIGPIPE, SIG_IGN);
#else
  {
    // If iMode = 0, blocking is enabled;
    // If iMode != 0, non-blocking mode is enabled.
    int32 iMode = 1;

    ioctlsocket(sd, FIONBIO, &iMode);
  }
#endif

  free(strx);
  *Socket = sd;
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: MCastClose
bool MCastClose(int Socket)
//-Synopsis:    Close a multicast socket
//-Returns:     TRUE if no errors, else FALSE
//-ARG_U:       Socket         Handle to close
//-Description: Close a multcast socket
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  shutdown(Socket, 0);
  close(Socket);   // Not clear to me if needed by should not panic...
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////
int inet_aton (const char *cp, struct in_addr *addr);

/*
 * Check whether "cp" is a valid ascii representation
 * of an Internet address and convert to a binary address.
 * Returns 1 if the address is valid, 0 if not.
 * This replaces inet_addr, the return value from which
 * cannot distinguish between failure and a local broadcast address.  */
int
inet_aton(const char *cp_arg, struct in_addr *addr)
{
	register unsigned long val;
	register int base, n;
	register unsigned char c;
	register unsigned const char *cp = (unsigned const char *) cp_arg;
	unsigned int parts[4];
	register unsigned int *pp = parts;

	for (;;) {
		/*
		 * Collect number up to ``.''.
		 * Values are specified as for C:
		 * 0x=hex, 0=octal, other=decimal.
		 */
		val = 0; base = 10;
		if (*cp == '0') {
			if (*++cp == 'x' || *cp == 'X')
				base = 16, cp++;
			else
				base = 8;
		}
		while ((c = *cp) != '\0') {
			if (isascii(c) && isdigit(c)) {
				val = (val * base) + (c - '0');
				cp++;
				continue;
			}
			if (base == 16 && isascii(c) && isxdigit(c)) {
				val = (val << 4) +
					(c + 10 - (islower(c) ? 'a' : 'A'));
				cp++;
				continue;
			}
			break;
		}
		if (*cp == '.') {
			/*
			 * Internet format:
			 *	a.b.c.d
			 *	a.b.c	(with c treated as 16-bits)
			 *	a.b	(with b treated as 24 bits)
			 */
			if (pp >= parts + 3 || val > 0xff)
				return (0);
			*pp++ = val, cp++;
		} else
			break;
	}
	/*
	 * Check for trailing characters.
	 */
	if (*cp && (!isascii(*cp) || !isspace(*cp)))
		return (0);
	/*
	 * Concoct the address according to
	 * the number of parts specified.
	 */
	n = pp - parts + 1;
	switch (n) {

	case 1:				/* a -- 32 bits */
		break;

	case 2:				/* a.b -- 8.24 bits */
		if (val > 0xffffff)
			return (0);
		val |= parts[0] << 24;
		break;

	case 3:				/* a.b.c -- 8.8.16 bits */
		if (val > 0xffff)
			return (0);
		val |= (parts[0] << 24) | (parts[1] << 16);
		break;

	case 4:				/* a.b.c.d -- 8.8.8.8 bits */
		if (val > 0xff)
			return (0);
		val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
		break;
	}
	if (addr)
		addr->s_addr = htonl(val);
	return (1);
}
