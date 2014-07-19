/*
 *
 *  This file is part of the XForms library package.
 *
 * XForms is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1, or
 * (at your option) any later version.
 *
 * XForms is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with XForms; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 *
 */


/*
 * $Id: cmd_br.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1997-2002  T.C. Zhao
 *  All rights reserved.
 *.
 *
 * Run a command and have its output re-directed to a browser.
 * Not very robust as signals are not handled.
 *
 */
#include "forms.h"
#include "cmdbr.h"		/* from directory fd */

#ifndef FL_WIN32
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#else
#include <process.h>
#include <io.h>
#include <errno.h>
#include <fcntl.h>
#define waitpid(pid,pstat,code)  _cwait(pstat, pid, code)
#define pid_t                     HANDLE
#endif /* FL_WIN32 */

/* structures that hold the current running processes */
typedef struct pidlist
{
    struct pidlist *next;
    pid_t pid;
    int fd_out, fd_err, fd_user;
}
PIDList;

static PIDList *pidlist;
static FD_cmd *logger;

static void
io_cb(int fd, void *data)
{
    char buf[1024];
    int n;

#ifndef FL_WIN32
    if ((n = read(fd, buf, sizeof(buf) - 1)) > 0)
    {
	buf[n] = '\0';
	fl_addto_browser_chars(logger->browser, buf);
    }
    else if (n <= 0)
    {				/* 0: EOF   <0: error. */
	if (n < 0)
	    M_err("ExeCmd", "read returns %d", n);
	fl_remove_io_callback(fd, FL_READ, io_cb);
	close(fd);
    }
#else
    ReadFile((HANDLE) fd, buf, sizeof(buf) - 1, &n, NULL);
    if (n > 0)
    {
	buf[n] = '\0';
	fl_addto_browser_chars(logger->browser, buf);
    }
    else if (n <= 0)
    {				/* 0: EOF   <0: error. */
	if (n < 0)
	    M_err("ExeCmd", "read returns %d", n);
	fl_remove_io_callback(fd, FL_READ, io_cb);
	CloseHandle((HANDLE) fd);
    }
#endif
}


static void
check_for_activity(PIDList * cur)
{
    XEvent xev;

    /* we must read until the pipe is closed otherwise waitpid will hang */
    do
    {
	if (fl_check_forms() == FL_EVENT)
	    fl_XNextEvent(&xev);
    }
    while (fl_is_watched_io(cur->fd_out) || fl_is_watched_io(cur->fd_err));

    fl_update_display(1);
}

void
fl_cmdbr_close_cb(FL_OBJECT * ob, long data)
{
    fl_hide_form(((FD_cmd *) ob->form->fdui)->cmd);
}

void
fl_cmdbr_clear_cb(FL_OBJECT * ob, long data)
{
    fl_clear_browser(((FD_cmd *) ob->form->fdui)->browser);
}

static int
atclose(FL_FORM * form, void *data)
{
    return FL_OK;
}

static void
create_logger(void)
{
    if (!logger)
    {
	int oldy = fl_inverted_y;
	int oldu = fl_get_coordunit();
	fl_inverted_y = 0;
	fl_set_coordunit(FL_COORD_PIXEL);
	logger = create_form_cmd();
	fl_set_object_bw(logger->backface, 2);
	fl_set_form_minsize(logger->cmd, 250, 100);
	fl_set_form_atclose(logger->cmd, atclose, 0);
	fl_set_form_position(logger->cmd, -logger->cmd->w - 5, 3);
	fl_inverted_y = oldy;
	fl_set_coordunit(oldu);
    }
}

void
fl_addto_command_log(const char *s)
{
    create_logger();
    fl_addto_browser_chars(logger->browser, s);
}

void
fl_clear_command_log(void)
{
    create_logger();
    fl_clear_browser(logger->browser);
}

void
fl_show_command_log(int border)
{
    create_logger();
    fl_fit_object_label(logger->close_br, 2, 1);
    fl_fit_object_label(logger->clear_br, 2, 1);
    fl_show_form(logger->cmd, FL_PLACE_POSITION, border, "MessageLog");
}

void
fl_hide_command_log(void)
{
    create_logger();

    if (logger->cmd->visible)
	fl_hide_form(logger->cmd);
}

/*
 * VMS support is from the patch by
 * David Mathog (mathog@seqaxp.bio.caltech.edu)
 */

/* OpenVMS < 7.0 is missing some key pieces here, notably waitpid, so
   it uses wait instead.  Also, vfork instead of fork (see local.h). */

#ifdef __VMS
#include <processes.h>		/* for wait(),vfork() */

static void
fl_execl(const char *path, const char *arg0, const char *arg1,
	 const char *arg2, const char *arg3)
{
    char cmd_tmp[1024];
    char *cmd_token[16];
    int i;

    /* VMS needs the tokens as tokens, not in one big command line. The
       following code is really simple minded, and will screw up if one of
       the command line arguments includes double quotes around spaces. A
       maximum of 16 tokens can exist in the command line. */

    for (i = 0; i < 16; i++)
	cmd_token[i] = NULL;
    strncpy(cmd_tmp, arg2, 1024);
    cmd_token[0] = strtok(cmd_tmp, " ");
    cmd_token[15] = NULL;

    for (i = 1; i < 15, (cmd_token[i - 1] != NULL); i++)
	cmd_token[i] = strtok(NULL, " ");

    execl(cmd_token[0],
	  cmd_token[0], cmd_token[1], cmd_token[2], cmd_token[3],
	  cmd_token[4], cmd_token[5], cmd_token[6], cmd_token[7],
	  cmd_token[8], cmd_token[9], cmd_token[10], cmd_token[11],
	  cmd_token[12], cmd_token[13], cmd_token[14], cmd_token[15],
	  (char *) 0);
}

#if __VMS_VER < 70000000
pid_t
waitpid(pid_t pid, int *status, int options)
{
    return wait(status);
}
#endif /* VMS_VER < 7.0 */
#else
/* on all other platforms use native execl */
#define  fl_execl    execl
#endif /* VMS */

#ifndef FL_WIN32
FL_PID_T
fl_exe_command(const char *cmd, int block)
{
    pid_t pid;
    static int p_err[2], p_inout[2];
    char buf[256];

    create_logger();

    if (pipe(p_err) < 0 || pipe(p_inout) < 0)
    {
	snprintf(buf,sizeof(buf), 
                     "Can't create pipe - %s", fl_get_syserror_msg());
	fprintf(stderr, "%s\n", buf);
	fl_addto_browser(logger->browser, buf);
	return -1;
    }

    if ((pid = fork()) < 0)
    {
	snprintf(buf,sizeof(buf),"fork failed: %s",fl_get_syserror_msg());
	fl_addto_browser(logger->browser, buf);
	perror("fork");
	close(p_inout[0]);
	close(p_inout[1]);
	close(p_err[0]);
	close(p_err[1]);
	return -1;
    }

    if (pid == 0)
    {
	dup2(p_inout[1], fileno(stdout));
	close(p_inout[1]);
	close(p_inout[0]);
	dup2(p_err[1], fileno(stderr));
	close(p_err[1]);
	close(p_err[0]);
	fl_execl("/bin/sh", "sh", "-c", cmd, (char *) 0);
	perror("execle");
	_exit(127);
    }
    else
    {
	PIDList *cur = fl_calloc(1, sizeof(*cur));

	cur->next = pidlist;
	cur->pid = pid;
	pidlist = cur;

	/* close write end of the pipe, only child process uses them */
	close(p_inout[1]);
	close(p_err[1]);
	cur->fd_out = p_inout[0];
	cur->fd_err = p_err[0];
	/* add_io prepends. handle stdout first when data is present */
	fl_add_io_callback(cur->fd_err,
			   FL_READ,
			   io_cb,
			   (void *) ( long ) cur->pid);
	fl_add_io_callback(cur->fd_out,
			   FL_READ,
			   io_cb, 
			   (void *) ( long ) cur->pid);
    }

    return block ? (FL_PID_T) fl_end_command(pid) : pid;
}

#else /* Win/NT platform */

FL_PID_T
fl_exe_command(const char *cmd, int block)
{
    SECURITY_ATTRIBUTES sa =
    {0};
    STARTUPINFO si =
    {0};
    PROCESS_INFORMATION pi =
    {0};
    HANDLE hPipeWrite = NULL;
    HANDLE hPipeRead = NULL;
    char buf[512];
    PIDList *cur;

    create_logger();

    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    if (!CreatePipe(&hPipeRead, &hPipeWrite, &sa, 255))
    {
	snprintf(buf, sizeof(buf), "Can't create pipe - %s", 
                    fl_get_syserror_msg());
	fprintf(stderr, "%s\n", buf);
	fl_addto_browser(logger->browser, buf);
	return (FL_PID_T) - 1;
    }

    /* Make child process use p_inout as standard out, and make sure it does
       not show on screen. */
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput = hPipeWrite;
    si.hStdError = hPipeWrite;

    if (CreateProcess(NULL, (LPTSTR) cmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
    {
	/* close write end of the pipe, only child process uses them */
	CloseHandle(hPipeWrite);
	CloseHandle(pi.hThread);	/* close the primay thread handle */
	cur = fl_malloc(sizeof(*cur));
	cur->next = pidlist;
	cur->pid = pi.hProcess;
	pidlist = cur;
	cur->fd_out = hPipeRead;

	/* add_io prepends. handle stdout first when data is present */
	fl_add_io_callback((int) cur->fd_out, FL_READ, io_cb, (void *) cur->pid);
    }
    else
    {
	snprintf(buf,sizeof(buf),"spawn failed: %s",fl_get_syserror_msg());
	fl_addto_browser(logger->browser, buf);
	perror("spawn");
	/* close the pipes */
	CloseHandle(hPipeRead);
	CloseHandle(hPipeWrite);
	return (FL_PID_T) - 1;
    }

    return block ? (FL_PID_T) fl_end_command(cur->pid) : cur->pid;
}
#endif

/*
 * essentially a bi-directional pipe
 */
FILE *
fl_popen(const char *cmd, const char *type)
{
    pid_t pid;
    static int p_err[2], p_p2c[2], p_c2p[2];
    char buf[512];
    int iswrite, i;
    FILE *fp = 0;

    if (!cmd || !*cmd || !type || !*type || (*type != 'r' && *type != 'w'))
	return 0;

    iswrite = (*type == 'w');

    create_logger();

    p_err[0] = p_err[1] = p_p2c[0] = p_p2c[1] = -1;

    if (pipe(p_err) < 0 || pipe(p_p2c) < 0 || pipe(p_c2p) < 0)
    {
	snprintf(buf, sizeof(buf),
                    "Can't create pipe - %s", fl_get_syserror_msg());
	fprintf(stderr, "%s\n", buf);
	fl_addto_browser(logger->browser, buf);
	for (i = 0; i < 2; i++)
	{
	    if (p_err[i] >= 0)
		close(p_err[i]);
	    if (p_p2c[i] >= 0)
		close(p_p2c[i]);
	}
	return NULL;
    }

    if ((pid = fork()) < 0)
    {
	snprintf(buf,sizeof(buf),"fork failed: %s", fl_get_syserror_msg());
	fl_addto_browser(logger->browser, buf);
	perror("fork");
	for (i = 0; i < 2; i++)
	{
	    close(p_err[i]);
	    close(p_p2c[i]);
	    close(p_c2p[i]);
	}
	return NULL;
    }

    if (pid == 0)
    {
	dup2(p_p2c[0], fileno(stdin));
	dup2(p_c2p[1], fileno(stdout));
	dup2(p_err[1], fileno(stderr));

	close(p_p2c[0]);
	close(p_p2c[1]);
	close(p_c2p[0]);
	close(p_c2p[1]);
	close(p_err[0]);
	close(p_err[1]);

	fl_execl("/bin/sh", "sh", "-c", cmd, (char *) 0);

	perror("execle");
	_exit(127);
    }
    else
    {
	/* the parent process */
	PIDList *cur = (PIDList *) fl_malloc(sizeof(*cur));

	cur->next = pidlist;
	cur->pid = pid;
	pidlist = cur;

	close(p_p2c[0]);
	close(p_c2p[1]);
	close(p_err[1]);

	cur->fd_err = p_err[0];
	cur->fd_out = p_c2p[0];

	fl_add_io_callback(cur->fd_err,
			   FL_READ, io_cb,
			   (void *) ( long ) pid);

	if (iswrite)
	{
	    cur->fd_user = p_p2c[1];
	    fl_add_io_callback(cur->fd_out,
			       FL_READ,
			       io_cb,
			       (void *) ( long ) pid );
	}
	else
	{
	    cur->fd_user = p_c2p[0];
            /* should we dup the stream so the browser gets a copy ? */
	}
	fp = fdopen(cur->fd_user, type);
    }
    return fp;
}

/* close pipe */
int
fl_pclose(FILE * stream)
{
    register PIDList *cur, *last;
    int fd;

    if (!stream || (fd = fileno(stream)) < 0)
	return -1;

    fclose(stream);

    for (last = 0, cur = pidlist; cur && cur->fd_user != fd;
	 last = cur, cur = last->next) 
        ;

    if (!cur)
	return -1;

    check_for_activity(cur);

    return fl_end_command(cur->pid);
}

int
fl_end_command(FL_PID_T pid)
{
    register PIDList *cur, *last;
    int pstat;

    for (last = 0, cur = pidlist; cur && cur->pid != pid;
	 last = cur, cur = last->next)
	;

    if (!cur)
	return -1;

    do
    {
	check_for_activity(cur);
	pid = waitpid(cur->pid, &pstat, 0);
    }
    while (pid == -1 && errno == EINTR);

    if (last)
	last->next = cur->next;
    else
	pidlist = cur->next;

    fl_addto_freelist(cur);

    return (pid == -1 ? -1 : pstat);
}


/* check if PID exists or not. Return 0 if it does not, 1 if still
 * running, any other value indicates an error
 */
int
fl_check_command(FL_PID_T pid)
{
#ifndef FL_WIN32
    if (kill(pid, 0) == 0)
    {
	waitpid(pid, 0, WNOHANG);
	return 1;
    }
    return errno == ESRCH ? 0 : -1;
#else
    DWORD ExitCode = 0;
    GetExitCodeProcess(pid, &ExitCode);

    if (ExitCode == STILL_ACTIVE)
	return 1;
    return (GetLastError() == ERROR_INVALID_HANDLE) ? 0 : -1;
#endif
}

/* clean up all child processes */
int
fl_end_all_command(void)
{
    register PIDList *cur, *next;
    int pstat = 0;
    pid_t pid = 0;

    for (cur = pidlist; cur; cur = next)
    {
	next = cur->next;
	check_for_activity(cur);
	pid = waitpid(cur->pid, &pstat, 0);
	fl_addto_freelist(cur);
    }

    pidlist = 0;

    return pid == -1 ? pid : pstat;
}

FD_CMDLOG *
fl_get_command_log_fdstruct(void)
{
    static FD_CMDLOG ret;

    create_logger();
    ret.form = logger->cmd;
    ret.browser = logger->browser;
    ret.close_browser = logger->close_br;
    ret.clear_browser = logger->clear_br;
    return &ret;
}

void
fl_set_command_log_position(int x, int y)
{
    create_logger();
    fl_set_form_position(logger->cmd, x, y);
}

#include "cmdbr.c"		/* from directory fd */
