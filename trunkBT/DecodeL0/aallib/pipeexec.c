/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2013 by PADSOFT, Inc.                        */
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

//  pipeexec.c   popen with PID and STDERR handling to our likeing

//  Revision History
//    Aug 2013 by Peter Dennett - Kemah
//      - Coded using several Internet examples

#include "pipeexec.h"
#include "logfile.h"
#include "al_lib.h"
#include <wait.h>
#include <errno.h>

  // Help MinGW out a bit

#ifndef SIGKILL
#define SIGKILL SIGABRT
#endif
#ifndef WNOHANG
#define WNOHANG 0
#endif

//~----------------------------------------------------------------------------
// 

FILE *pipeexec(char *cmd, char *type, pid_t *pPID)
{
int piped[2];
pid_t pid;
FILE *fp = NULL;

  if ((*type != 'r' && *type != 'w') || (type[1]))
    return NULL;

  if (pipe(piped))
  {
    LogError("pipeexec: Creating pipe failed\n");
    return NULL;
  }

  switch(pid = fork())
  {
    case -1:  // Fork errored
      LogError("pipeexec: fork failed\n");
      close(piped[0]);
      close(piped[1]);
      return NULL;

    case 0:   // in child
      switch (type[0])
      {
        case 'r':
          if (piped[1] != STDOUT_FILENO)
            dup2(piped[1], STDOUT_FILENO);
          if (piped[1] != STDERR_FILENO)
            dup2(piped[1], STDERR_FILENO); // Pipe stderr to stdout   
          close(piped[0]);
          break;

        case 'w':
          if (piped[0] != STDIN_FILENO)
            dup2(piped[0], STDIN_FILENO);
          close(piped[1]);
      }

      // close any other files that you don't want the new program to access

      execl("/bin/sh", "sh", "-c", cmd, NULL);
      exit(127); // only gets here if there is an error executing the program

    default:  // in parent, just flow through
      break;
  }

  switch (type[0])
  {
    case 'r':
      fp = fdopen(piped[0], type);
      close(piped[1]);
      break;

    case 'w':
      fp = fdopen(piped[1], type);
      close(piped[0]);
      break;
  }

  if (pPID)
    *pPID = pid;
  return fp;
}


//~----------------------------------------------------------------------------
// 

void pipeexecclose(FILE *fp, pid_t PID)
{
int status;
pid_t endID;
int i;
int killed=0;

  fclose(fp);

  for(i = 0; i < 15; i++) 
  {
    endID = waitpid(PID, &status, WNOHANG|WUNTRACED);
    switch (endID)
    {
      case -1:      // error calling waitpid
        LogError("pipeexecclose: error calling waitpid (%d) %s\n", 
                 errno, strerror(errno));
        goto badpid;
        
      case 0:       // child still running
        if (i > 12)
        {
          LogWarning("pipeexecclose: Killing pid %d\n", PID);
          kill(-PID, SIGKILL);
          killed = 1;
        }
        ShortSleep(0, 100000);
        break;

      default:
        if (endID == PID)     // Child ended
        {
          if (WIFEXITED(status))
            ;// LogNormal("pipeexecclose: Child ended normally\n");
          else if (WIFSIGNALED(status))
            LogNormal("pipeexecclose: Child ended because of an uncaught signal\n");
          else if (WIFSTOPPED(status))
            LogNormal("pipeexecclose: Child process has stopped\n");
          return;
        }
    }
  }

  if (killed)
    LogWarning("pipeexecclose: Killed pid %d\n", PID);

badpid:

  return;
}
