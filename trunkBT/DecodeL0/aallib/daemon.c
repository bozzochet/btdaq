/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2011 by PADSOFT, Inc.                        */
/*                                                                           */
/*    Developed for use on AMS-02, An International Space Station payload    */
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

//  Daemon.c   Make the process a daemon

//  Revision History
//    Apr 2011 by Peter Dennett - MCC-H
//      - Initial coding from various Internet examples
//    Jun 2011 by Peter Dennett - MCC-H
//      - Much more robust

#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <errno.h>

#include "aallib.h"
#include "logfile.h"
#include "al_lib.h"
#include "gsclib.h"

#define RUNNING_DIR_DEF	"/var/ams"
#define LOCK_FILE_DEF 	"exampled.lock"

extern int logfile_printf;

void Daemonize(char *LOCK_FILE, char *RUNNING_DIR, 
               char *LogDirectory, char *LogName, char *Instance)
{
#ifdef OS_LINUX
int i, lfp, pid, k;
char str[10];

	if (getppid() == 1) 
  {
    printf("Daemonize: Already a daemon - ignored\n");
    return;                                     // already a daemon
  }

  if ( ! RUNNING_DIR)
    RUNNING_DIR = RUNNING_DIR_DEF;

  if ( dir_check_and_create_2(RUNNING_DIR, "root", "root"))
  {
    printf("Daemonize: Unable to create RUNNING_DIR %s - not started\n", 
           RUNNING_DIR);
    exit(1);
  }

	pid = fork();

	if (pid < 0)
  {
    printf("Daemonize: Error on fork - not started\n");
    exit(1);                                    // fork error
  }

	if (pid > 0)
  {
    //??printf("PID %d forked, waiting for it to come active\n", pid);
    if (chdir(RUNNING_DIR))  // change running directory
      printf("Daemonize: Error - failed chdir '%s' errno %d %s\n", RUNNING_DIR, errno, strerror(errno));

    ShortSleep(1, 0);
    lfp = -1;
    for (i=0; i<10; i++)
    {
      if (lfp < 0)
      {
        lfp = open(LOCK_FILE?LOCK_FILE:LOCK_FILE_DEF, O_RDWR);
      }
      if (lfp < 0)
      {
        ShortSleep(1, 0);
      }
      else if (lockf(lfp, F_TLOCK, 0)<0)        // Cannot lock
      {
        k = read(lfp, str, sizeof str);
        if ((k <=0) || (atoi(str) != pid))
          printf("Daemonize: Error - PID %d not written to %s\n", pid,
                 LOCK_FILE?LOCK_FILE:LOCK_FILE_DEF); 
        else
          printf("PID %d started\n", pid);
        exit(0);                                // parent exits (once child is running)
      }
      ShortSleep(1, 0);  
    }
    printf("Daemonize: Error - PID %d child does not appear to have started\n", pid); 
    exit(-1);
  }                                           	// Child (daemon) continues

	setsid();                                     // obtain a new process group

	for (i=getdtablesize(); i>=0; --i) 
    close(i);                                   // close all descriptors

	i = open("/dev/null",O_RDWR);                 // Handles for standard I/O
  IGNORE_RESULT (dup(i)); 
  IGNORE_RESULT (dup(i));
	//umask(027);                                 // set newly created file permissions
	IGNORE_RESULT (chdir(RUNNING_DIR?RUNNING_DIR:RUNNING_DIR_DEF)); // change running directory

  logfile_printf = 0;                           // Dont write log entries to stdout
  LogOpen3(LogDirectory, LogName, Instance);

  LogNormal("Starting up pid %d\n", getpid());
	lfp = open(LOCK_FILE?LOCK_FILE:LOCK_FILE_DEF, O_RDWR|O_CREAT, 0640);
	if (lfp < 0)                                  // Lock file
  {
    LogError("daemon: Error opening lock file %s\n",
             LOCK_FILE?LOCK_FILE:LOCK_FILE_DEF);
    exit(1); /* can not open */
  }

	if (lockf(lfp, F_TLOCK, 0)<0)                 // Cannot lock
  {
    LogError("daemon: Error locking lock file %s\n",
             LOCK_FILE?LOCK_FILE:LOCK_FILE_DEF);
    exit(0);
  }

	/* first instance continues */

	sprintf(str, "%d\n", getpid());
	IGNORE_RESULT (write(lfp, str, strlen(str)));// record pid to lockfile

	signal(SIGCHLD,SIG_IGN);                     // ignore child
	signal(SIGTSTP,SIG_IGN);                     // ignore tty signals
	signal(SIGTTOU,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
#endif
}

//~----------------------------------------------------------------------------

void DaemonKill(char *LOCK_FILE, char *RUNNING_DIR)
{
#ifdef OS_LINUX
int i, lfp, pid, k, sig=3;
char str[10];

  if (chdir(RUNNING_DIR?RUNNING_DIR:RUNNING_DIR_DEF) != 0); // change running directory
  {
    printf("DaemonKill: Could not chdir %s\n",
           RUNNING_DIR?RUNNING_DIR:RUNNING_DIR_DEF);
  }
 
  lfp = open(LOCK_FILE?LOCK_FILE:LOCK_FILE_DEF, O_RDWR);
  if (lfp < 0)
  {
    printf("DaemonKill: Could not open %s Daemon appears to be not running\n",
           LOCK_FILE?LOCK_FILE:LOCK_FILE_DEF);
    exit(0);
  }

  if (lockf(lfp, F_TLOCK, 0) >= 0)
  {
    printf("DaemonKill: Able to lock %s appears to be not running.  %s\n",
           LOCK_FILE?LOCK_FILE:LOCK_FILE_DEF,
           (unlink(LOCK_FILE?LOCK_FILE:LOCK_FILE_DEF) >= 0) ? 
           "Unlinked" : "Unlink failed");
    exit(0);
  }

  k = read(lfp, str, sizeof str);
  if (k <= 0)
  {
    printf("DaemonKill: Error - read of PID from %s failed. %s\n",
           LOCK_FILE?LOCK_FILE:LOCK_FILE_DEF, 
           (unlink(LOCK_FILE?LOCK_FILE:LOCK_FILE_DEF) >= 0) ? 
           "Unlinked" : "Unlink failed"); 
    exit(-1);
  }

  pid = atoi(str);

  for (i=0; i<10; i++)
  {
    kill(pid, sig);
    ShortSleep(5, 0);
    if (lockf(lfp, F_TLOCK, 0) >= 0)
    {
      printf("DaemonKill: kill %d to PID %d did the trick. %s\n", sig, pid,
             (unlink(LOCK_FILE?LOCK_FILE:LOCK_FILE_DEF) >= 0) ? 
             "Unlinked" : "Unlink failed"); 
      exit(0);
    }
    if (i > 3)
      sig = 9;
  }

  printf("DaemonKill: Error - kills of PID %d failed. File %s\n", pid,
         LOCK_FILE?LOCK_FILE:LOCK_FILE_DEF, 
         (unlink(LOCK_FILE?LOCK_FILE:LOCK_FILE_DEF) >= 0) ? 
         "Unlinked" : "Unlink failed"); 
  exit(-1);
#else
  exit(0);
#endif
}
