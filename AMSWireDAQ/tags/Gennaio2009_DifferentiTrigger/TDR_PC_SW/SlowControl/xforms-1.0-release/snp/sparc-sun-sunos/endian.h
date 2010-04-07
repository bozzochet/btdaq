#ifndef _SUN4_ENDIAN_H_
#define	_SUN4_ENDIAN_H_

/*
 * Define the order of 32-bit words in 64-bit words.
 */
#define _QUAD_HIGHWORD 1
#define _QUAD_LOWWORD 0


#ifndef _POSIX_SOURCE
/*
 * Definitions for byte order, according to byte significance from low
 * address to high.
 */
#define	LITTLE_ENDIAN	1234	/* LSB first: i386, vax */
#define	BIG_ENDIAN	4321	/* MSB first: 68000, ibm, net */
#define	PDP_ENDIAN	3412	/* LSB first in word, MSW first in long */

#define	BYTE_ORDER	BIG_ENDIAN

#if 0				/* not needed by snprintf() -- spl */
#include <sys/cdefs.h>
#endif
#ifdef __STDC__
#define __P(x) x		/* prototypes */
#else
#define __P(x)			/* no prototypes */
#endif

/*
 * Macros for network/external number representation conversion.
 */
#define	NTOHL(x)	(x) = ntohl((unsigned long)(x))
#define	NTOHS(x)	(x) = ntohs((unsigned short)(x))
#define	HTONL(x)	(x) = htonl((unsigned long)(x))
#define	HTONS(x)	(x) = htons((unsigned short)(x))

#endif /* _POSIX_SOURCE */

#endif /* !_SUN4_ENDIAN_H_ */
