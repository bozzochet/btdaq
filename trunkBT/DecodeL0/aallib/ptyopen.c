/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2014 by PADSOFT, Inc.                        */
/*                                                                           */
/*    Developed for use on AMS-02, An International Space Station payload    */
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

//  ptyopen.c   Wrapper to open a psuedo TTY

//  Revision History
//    Feb 2014 by Peter Dennett at GVA->IAD, whole row
//      -  Baseline development

#define _XOPEN_SOURCE 600
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#define __USE_BSD
#include <termios.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h> 
#ifndef OS_WIN32
#include <grp.h>
#include <pwd.h>
#endif
#include <signal.h>

#include "ptyopen.h"
#include "aallib.h"
#include "logfile.h"
#include "argv.h"

////////////////////////////////////////////////////////////////////////////////

// Information needed to reap our children

typedef struct _Slave
{
  int    fdm;                   // Master descripter
  int    running;
  int    kills;
  pid_t	 pid;		                // Process Id
  int		 status;	              // Exit status
  struct _Slave	*next;	  	    // list 
} Slave, *pSlave;

typedef void (*sighandler_t)(int);
static pSlave Slaves = NULL;
static sighandler_t prevchld = 0;

////////////////////////////////////////////////////////////////////////////////
// Signal from exiting child
static void sigchld(int signum)
{
  int	   status = 0;
  pSlave zombie = NULL;
  pid_t	 pid =    wait(&status);

  for (zombie=Slaves;  zombie;  zombie=zombie->next)
    if (zombie->pid == pid)
      break;

  if ( ! zombie)
  {
    LogError("ptyopen: Unexpected SIGCHLD for pid %d\n", pid);
  }
  else
  {
    zombie->status = status;
    zombie->running = 0;
  }
}

////////////////////////////////////////////////////////////////////////////////
//  Close a pseudo TTY
int ptyclose(int fdm, int debug)
{
pSlave Slave, Prior;

// Reap our childeren.  First do two passes to try kills (unless dead aready)

  for (Slave=Slaves; Slave; Slave=Slave->next)
  {
    if ((fdm == Slave->fdm) && (Slave->running))
    {
      kill(Slave->pid, SIGKILL);
      Slave->kills++;
      usleep(200*1000);
    }
  }

  for (Slave=Slaves; Slave; Slave=Slave->next)
  {
    if ((fdm == Slave->fdm) && (Slave->running))
    {
      kill(Slave->pid, SIGKILL);
      Slave->kills++;
      usleep(200*1000);
    }
  }

  Prior = NULL;
  for (Slave=Slaves; Slave; Slave=Slave->next)
  {
    if (fdm == Slave->fdm)
    {
      if ( ! Slave->running)
        LogNormal("ptyclose: PID %d exited 0x%X.  %d kills\n", Slave->pid,
                  Slave->status, Slave->kills);
      else
        LogWarning("ptyclose: PID %d did not report exit.  %d kills\n", 
                   Slave->pid, Slave->kills);

      if (Prior)
        Prior->next = Slave->next;
      else
        Slaves = Slave->next;

      free(Slave);
    }
    else
    {
      Prior = Slave;
    }
  }

  close(fdm);

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
//  Return a file descripter to an open Psuedo TTY running command.  
//  Negative returned on error
int ptyopen(char *command, char *user, int debug)
{
  int fdm, fds;
  int rc;
  int pid;
  int argc;
  char **argv;
  struct termios slave_orig_term_settings; // Saved terminal settings
  struct termios new_term_settings; // Current terminal settings
#ifndef OS_WIN32
  struct passwd *pwd=NULL;
#endif
pSlave Slave;

  prevchld = signal(SIGCHLD, sigchld);

  // Set up the program to execute

  if ( ! command)
  {
    LogError("ptyopen: must have command to execute\n");
    return -1;
  }

  argv = argvbuild(command, &argc);
  if ( ! argv)
  {
    LogError("ptyopen: argvbuild failed\n");
    return -1;
  }

#if 0
  {
    int i;
    for (i=0; i<=argc; i++)  //?? Pretty sure argvbuild gives us an extra NULL (execvp requirement)
      printf("argv[%d] %s\n", i, argv[i]?argv[i]:"NULL");
  }
#endif

#ifndef OS_WIN32
  if (user)
  {
    pwd = getpwnam(user);
    if ( ! pwd) 
    {
      LogError("ptyopen: getpwnam(%s) failed: %s\n", user, strerror(errno));
      return -1;
    }
  }
#endif

  fdm = posix_openpt(O_RDWR | O_NOCTTY);
  if (fdm < 0)
  {
    LogError("ptyopen: posix_openpt error: %s (%d)\n", strerror(errno), errno);
    return -1;
  }

  rc = grantpt(fdm);
  if (rc != 0)
  {
    LogError("ptyopen: grantpt error: %s (%d)\n", strerror(errno), errno);
    close(fdm);
    return -1;
  }

  rc = unlockpt(fdm);
  if (rc != 0)
  {
    LogError("ptyopen: unlockpt error: %s (%d)\n", strerror(errno), errno);
    close(fdm);
    return -1;
  }

  // Open the slave side ot the PTY

  fds = open(ptsname(fdm), O_RDWR);
  if (fds < 0)
  {
    LogError("ptyopen: open of slave error: %s (%d)\n", strerror(errno), errno);
    close(fdm);
    return -1;
  }

  // Create the child process

  pid = fork();
  if (pid)  // Parent -------------------------------------------------------
  {
   // Close the slave side of the PTY, free memory, and return

    Slave= (pSlave)malloc(sizeof(*Slave));

    Slave->fdm = fdm;
    Slave->running = 1;
    Slave->kills = 0;
    Slave->pid = pid;
    Slave->next = Slaves;
    Slaves = Slave;

    //if (debug > 0)
      LogNormal("ptyopen: PID %d forked for: %s\n", pid, command);

    close(fds);
    argvfree(argv);

    return fdm;
  }
  else // Child ----------------------------------------------------------------
  {
   // Close the master side of the PTY

    close(fdm);

   // Save the defaults parameters of the slave side of the PTY

    rc = tcgetattr(fds, &slave_orig_term_settings);

   // Set RAW mode on slave side of PTY

    new_term_settings = slave_orig_term_settings;
    cfmakeraw (&new_term_settings);
    tcsetattr (fds, TCSANOW, &new_term_settings);

    // The slave side of the PTY becomes the standard input and
    // outputs of the child process

    close(0); // Close standard input (current terminal)
    close(1); // Close standard output (current terminal)
    close(2); // Close standard error (current terminal)

    dup(fds); // PTY becomes standard input (0)
    dup(fds); // PTY becomes standard output (1)
    dup(fds); // PTY becomes standard error (2)

    // Now the original file descriptor is useless

    close(fds);

    // Change effective and real UID to requested user

    if (user)
    {
      if (setreuid(pwd->pw_uid, pwd->pw_uid) != 0)
      {
        LogError("ptyopen: setreuid(%s) failed: %s\n", user, strerror(errno));
      }
    }

    // Make the current process a new session leader

    setsid();

    // As the child is a session leader, set the controlling terminal 
    // to be the slave side of the PTY (Mandatory for programs like the
    // shell to make them manage correctly their outputs)

    ioctl(0, TIOCSCTTY, 1);

    // Execution of the program

    execvp(argv[0], argv);

    return 1; // if Error...
  }
}
