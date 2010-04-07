/*
 * Copyright (c) 1983, 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)syslog.c	5.36 (Berkeley) 10/4/92";
#endif /* LIBC_SCCS and not lint */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include "syslog.h"
#include <sys/uio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/un.h>

#include <errno.h>
extern int	errno;
extern int sys_nerr;
extern const char *const sys_errlist[];

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sysexits.h>

#if __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#ifndef SUN_LEN
/* actual length of an initialized sockaddr_un */
#define SUN_LEN(su) \
	(sizeof(*(su)) - sizeof((su)->sun_path) + strlen((su)->sun_path))
#endif

#ifndef	_PATH_LOG
#define	_PATH_LOG "/dev/log"
#endif

#ifndef	_PATH_CONSOLE
#define	_PATH_CONSOLE "/dev/console"
#endif

#ifndef	LOG_PRI
#define LOG_PRI(p)      ((p) & LOG_PRIMASK)
#endif

static struct sockaddr_un SyslogAddr;
static int      connected=0;
static int      regress=0;
static int	LogFile = -1;		/* fd for log */
static int	LogStat = 0;		/* status bits, set by openlog() */
static const char *LogTag = "syslog";	/* string to tag the entry with */
static int	LogFacility = LOG_USER;	/* default facility code */
static int	LogMask = 0xff;		/* mask of priorities to be logged */
static void   (*logfunc)__P((char *));


void vsyslog       __P((int, const char *, va_list));
void openlog       __P((const char *,int,int));
void openlog1      __P((const char *,int,int));
void openlog_debug __P((const char *,int,int));
extern	char *index();

/*
 * syslog, vsyslog --
 *	print message on log file; output is intended for syslogd(8).
 */
void
#if __STDC__
syslog(int pri, const char *fmt, ...)
#else
syslog(pri, fmt, va_alist)
	int pri;
	char *fmt;
	va_dcl
#endif
{
	va_list ap;

#if __STDC__
	va_start(ap, fmt);
#else
	va_start(ap);
#endif
	vsyslog(pri, fmt, ap);
	va_end(ap);
}

void vsyslog(pri, fmt, ap)
	int pri;
	const char *fmt;
	va_list ap;
{
  int cnt;
  char *p;
  time_t now;
  int fd, saved_errno;

#define TBUFSIZE 2048
  char *stdp, tbuf[TBUFSIZE], fmt_cpy[1024];
  
#define	INTERNALLOG	LOG_ERR|LOG_CONS|LOG_PID
  
  /* Check for invalid bits. */
  if (pri & ~(LOG_PRIMASK|LOG_FACMASK)) {
    syslog(INTERNALLOG,
	   "syslog: unknown facility/priority: %x", pri);
    pri &= LOG_PRIMASK|LOG_FACMASK;
  }
  
  /* Check priority against setlogmask values. */
  if (!LOG_MASK(LOG_PRI(pri)) & LogMask)
    return;
  
  saved_errno = errno;
  
  /* set default facility if none specified */
  if ((pri & LOG_FACMASK) == 0)
    pri |= LogFacility;
  
  /* build the message */
  (void)time(&now);
  if(regress) {
    /* make dates always the same */
    now=60000;
  }
    
  (void)sprintf(tbuf, "<%d>%.15s ", pri, ctime(&now) + 4);
  for (p = tbuf; *p; ++p);
  if (LogTag) {
    (void)strcpy(p, LogTag);
    for (; *p; ++p);
  }
  if (!regress && (LogStat & LOG_PID)) {
    (void)sprintf(p, "[%d]", getpid());
    for (; *p; ++p);
  }
  if (LogTag) {
    *p++ = ':';
    *p++ = ' ';
  }
  
  /* substitute error message for %m */
  {
    char ch, *t1, *t2;

    for (t1 = fmt_cpy; ch = *fmt; ++fmt)
      if (ch == '%' && fmt[1] == 'm') {
	++fmt;
	for (t2 = sys_errlist[saved_errno];
	     *t1 = *t2++; ++t1);
      }
      else
	*t1++ = ch;
    *t1 = '\0';
  }
  
  (void)vsnprintf(p, TBUFSIZE - (p-tbuf), fmt_cpy, ap);

  if(logfunc) {
    (*logfunc)(tbuf);
  }
}
 

static void udp_logfunc(m)
     char *m;
{
  int cnt,fd;
  char *p;
  
  cnt = strlen(m);

  if (LogFile >=0 && send(LogFile, m, cnt, 0) >= 0) {
    return;
  }
  
  /* see if should attempt the console */
  if (!(LogStat&LOG_CONS)) {
    return;
  }
  
  /*
   * Output the message to the console; don't worry about blocking,
   * if console blocks everything will.  Make sure the error reported
   * is the one from the syslogd failure.
   */
  if ((fd = open(_PATH_CONSOLE, O_WRONLY, 0)) >= 0) {
    (void)strcat(m, "\r\n");
    cnt += 2;
    p = index(m, '>') + 1;
    (void)write(fd, p, cnt - (p - m));
    (void)close(fd);
  }
}

static void stderr_logfunc(m)
     char *m;
{
  write(2,"syslog: ",8);
  write(2,m,strlen(m));
  write(2,"\n",1);
}

void openlog(ident, logstat, logfac)
	const char *ident;
	int logstat, logfac;
{
  if (ident != NULL) {
    LogTag = ident;
  }

  regress=0;
  
  LogStat = logstat;
  if (logfac != 0 && (logfac &~ LOG_FACMASK) == 0) {
    LogFacility = logfac;
  }

  if (LogFile == -1) {
    SyslogAddr.sun_family = AF_UNIX;
    (void)strncpy(SyslogAddr.sun_path, _PATH_LOG,
		  sizeof(SyslogAddr.sun_path));
    if ((LogFile = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
      return;
    }
    (void)fcntl(LogFile, F_SETFD, 1);
  }
  
  if (LogFile != -1) {
    if (connect(LogFile, (struct sockaddr *)&SyslogAddr,
		SUN_LEN(&SyslogAddr)) == -1) {
      (void)close(LogFile);
      LogFile = -1;
    } else {
      logfunc=udp_logfunc;
      connected = 1;
    }
  }
}

void openlog_debug(ident, logstat, logfac)
     const char *ident;
     int logstat, logfac;
{
  struct stat sb;

  if (ident != NULL) {
    LogTag = ident;
  }

  LogStat = logstat;
  if (logfac != 0 && (logfac &~ LOG_FACMASK) == 0) {
    LogFacility = logfac;
  }

  logfunc=stderr_logfunc;
  connected=1;
  regress=1;
}

void
closelog()
{
  (void)close(LogFile);
  LogFile = -1;
}

/* setlogmask -- set the log mask level */
int
setlogmask(pmask)
     int pmask;
{
  int omask;

  omask = LogMask;
  if (pmask != 0) {
    LogMask = pmask;
  }
  return (omask);
}


