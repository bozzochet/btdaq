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
 * $Id: listdir.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao 
 *  All rights reserved.
 *.
 *
 *  Read a directory. Hightly system dependent.
 *  Seems opendir/readdir/closedir is the most portable.
 *
 *  Original BSD scandir is emulated using the opendir stuff.
 *
 *  This file has no xforms dependencies
 *
 */

#if defined(F_ID) || defined(DEBUG)
char *fl_id_dlist = "$Id: listdir.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <sys/types.h>

#ifndef _WIN32
#include <unistd.h>
#else
#include <io.h>
#include <time.h>
#include <direct.h>
#define FL_WIN32
#endif

/* vms 7.0 has dirent.h */
#if defined(__VMS) && __VMS_VER < 70000000
#include "dirent_vms.h"
#endif

/* work around the bugs in various cpp */

#if defined(Lynx) && !defined(__VMS)
#include <dir.h>
#include <dirent.h>
#endif

#if !defined(__VMS) && !defined(Lynx) && !defined(FL_WIN32)
#include <dirent.h>
#endif

#include <sys/stat.h>
#include "forms.h"		/* for definitation of FL_EXPORT */
#include "local.h"
#include "ulib.h"

/******** limits and macros *********/
#ifndef S_ISREG
#define S_ISREG(m)      (((m) & S_IFREG) == S_IFREG)
#define S_ISDIR(m)      (((m) & S_IFDIR) == S_IFDIR)
#endif

#if defined (__EMX__) || defined (FL_WIN32) || defined (opennt)
#define S_ISLNK(m)   0		/* no links in OS/2      */
#define S_ISBLB(m)   0		/* no blk devices in OS2 */
#else
#ifndef S_ISLNK
#define S_ISLNK(m)      (((m) & S_IFLNK) == S_IFLNK)
#endif
#ifndef S_ISBLK
#define S_ISBLK(m)      (((m) & S_IFBLK) == S_IFBLK)
#endif
#endif /* __EMX__ */

#ifndef S_ISCHR
#define S_ISCHR(m)      (((m) & S_IFCHR) == S_IFCHR)
#endif

#if !defined(S_ISSOCK) && defined(S_IFSOCK)
#define S_ISSOCK(m)      (((m) & S_IFSOCK) == S_IFSOCK)
#endif

#define MAXCACHE 10		/* upto MAXCHCHE dir will be cached */
#define MAXFL  (FL_PATH_MAX + FL_FLEN)	/* maximum file length */

#ifndef StrReDup
#define StrReDup(a,b)   do { if(a) fl_free(a); a = fl_strdup(b);} while(0)
#endif

#if defined(NEED_DIRECT)
#define DIRENT direct
#else
#define DIRENT dirent
#endif

#if defined(NEED_GETCWD)
#if defined(Lynx)
char *
getcwd(char *a, int b)
{
    return getwd(a);
}

#endif
#if defined(__VMS)
#define getcwd(a,b)    getcwd(a,b,0)	/* use unix file/path syntax */
#endif
#endif

#ifdef FL_WIN32
/* convert the backslash to slash */
static char *
fl_b2f_slash(char *dir)
{
    char *p = dir;
    for (; p && *p; p++)
	if (*p == '\\')
	    *p = '/';
    return dir;
}
#else
#define fl_b2f_slash(a)
#endif
/************* local variables ****************/

static const char *cpat;	/* current pattern          */
static const char *cdir;	/* current working directory */
static char fname[MAXFL + 2];

#define FL_NONE 0

/******* local function forward dec **********/

extern int fl_wildmat(const char *, const char *);
static int tc_sort(const void *, const void *);
static int tc_scandir(const char *, struct DIRENT ***);

static int default_filter(const char *, int);

/* default filter and sort method */
static int sort_method = FL_ALPHASORT;
static FL_DIRLIST_FILTER ffilter = default_filter;
static int filter_directory = 0;   /* true to filter directory entries */

/*
 * convert native file types to FL
 */
#ifndef FL_WIN32		/* [  */

static void
mode2type(unsigned mode, int *type)
{
    if (S_ISDIR(mode))
	*type = FT_DIR;
    else if (S_ISREG(mode))
	*type = FT_FILE;
    else if (S_ISLNK(mode))
	*type = FT_LINK;
#if !defined(NO_SOCK)		/* only old SCO */
    else if (S_ISSOCK(mode))
	*type = FT_SOCK;
#endif
    else if (S_ISFIFO(mode))
	*type = FT_FIFO;
    else if (S_ISCHR(mode))
	*type = FT_CHR;
#if !defined(__EMX__)
    else if (S_ISBLK(mode))
	*type = FT_BLK;
#endif
    else
	*type = FT_OTHER;
}

/******************************************************************
 * Filter the filename before handing over to the "file is here" list.
 * Only files (include links) are shown,
 ******************************************************************/
static int
fselect(const char *d_name, struct stat *ffstat, int *type)
{
    int ret = 0;
    unsigned mode;

    strcat(strcpy(fname, cdir), d_name);
    stat(fname, ffstat);
    mode = ffstat->st_mode;
    mode2type(mode, type);

    if (!ffilter)
	ret = 1;
    else if (ffilter == default_filter)
    {
	/* always keep directory and links */
	ret = S_ISDIR(mode) ||
	    ((S_ISREG(mode) || S_ISLNK(mode)) && fl_wildmat(d_name, cpat));
    }
    else
    {
        /* we don't filter directories unless requested */
        if(!filter_directory)
           ret = *type == FT_DIR ||
                       (fl_wildmat(d_name, cpat) && ffilter(fname, *type));
        else
           ret = (*type == FT_DIR || fl_wildmat(d_name, cpat)) && 
                  ffilter(fname, *type);
    }
    return ret;
}

#else /* WIN 32 ][ */

static int
fselect(struct _finddata_t *c_file, FL_Dirlist * dl)
{
    int type, ret = 0;

    if (c_file->attrib & _A_SUBDIR)
	type = FT_DIR;
    else
	type = FT_FILE;

    if (!ffilter)
	ret = 1;
    else if (ffilter == default_filter)
    {				/* always keep directory and links */
	ret = (type == FT_DIR) || fl_wildmat(c_file->name, cpat);
    }
    else
    {
	strcat(strcpy(fname, cdir), c_file->name);
	ret = (type == FT_DIR) ||
	    (fl_wildmat(c_file->name, cpat) && ffilter(fname, type));
    }

    if (ret)
    {
	dl->name = fl_strdup(c_file->name);
	dl->type = type;
	dl->dl_mtime = c_file->time_write;
	dl->dl_size = c_file->size;
    }
    return ret;
}

#endif /* FL_WIN32 ] */

/* Sort the directory entries according to the sort method */
static int
tc_sort(const void *a, const void *b)
{
    FL_Dirlist *da = (FL_Dirlist *) a;
    FL_Dirlist *db = (FL_Dirlist *) b;

    switch (sort_method)
    {
    case FL_RALPHASORT:
	return strcmp(db->name, da->name);
    case FL_RCASEALPHASORT:
	return strcasecmp(db->name, da->name);
    case FL_MTIMESORT:
	return (da->dl_mtime - db->dl_mtime);
    case FL_RMTIMESORT:
	return (db->dl_mtime - da->dl_mtime);
    case FL_SIZESORT:
	return (da->dl_size > db->dl_size) ? 1 :
	    ((da->dl_size == db->dl_size) ? 0 : -1);
    case FL_RSIZESORT:
	return (da->dl_size < db->dl_size) ? 1 :
	    ((da->dl_size == db->dl_size) ? 0 : -1);
    case FL_CASEALPHASORT:
	return strcasecmp(da->name, db->name);
    case FL_ALPHASORT:
    default:
	return strcmp(da->name, db->name);
    }
}


/*******************************************************************
 * On entry, dir must be no zero and be terminated properly, i.e.,
 * ends with /
 *******************************************************************/
#ifndef FL_WIN32		/* [ */
static int
scandir_get_entries(const char *dir, const char *pat, FL_Dirlist ** dirlist)
{
    static struct DIRENT **dlist;
    FL_Dirlist *dl;
    static int lastn;
    static struct stat ffstat;
    int i, n;

    cpat = pat;
    cdir = dir;

    /* free all memory used last time we were here */
    if (dlist)
    {
	while (--lastn >= 0)
	    if (dlist[lastn])
		free(dlist[lastn]);
	free(dlist);
	dlist = 0;
    }

    n = 0;
    if ((lastn = tc_scandir((char *) dir, &dlist)) > 0)
    {
	dl = *dirlist = (FL_Dirlist *) fl_malloc((lastn + 1) * sizeof(FL_Dirlist));
	for (i = n = 0; i < lastn; i++)
	{
	    if (fselect(dlist[i]->d_name, &ffstat, &dl->type))
	    {
		dl->name = fl_strdup(dlist[i]->d_name);
		dl->dl_mtime = ffstat.st_mtime;
		dl->dl_size = ffstat.st_size;
		dl++;
		n++;
	    }
	}

	dl->name = 0;		/* sentinel */

	if (sort_method != FL_NONE)
	    qsort(*dirlist, n, sizeof(**dirlist), tc_sort);
    }

    return n;
}

#else /* FL_WIN32 ][ */
static int
scandir_get_entries(const char *dir, const char *pat, FL_Dirlist ** dirlist)
{
    FL_Dirlist *dl;
    char cwd[FL_PATH_MAX];
    struct _finddata_t c_file;
    long hFile;
    int n, lastn;

    cpat = pat;
    cdir = dir;
    n = 0;

    /* save the working directory */
    getcwd(cwd, FL_PATH_MAX);
    if (chdir(dir) != 0)	/* invalid directory */
	return 0;

    /* Find all files matched the pattern in current directory */
    if ((hFile = _findfirst("*", &c_file)) == -1L)
    {
	/* directory is empty, nothing to do */
	chdir(cwd);
	return 0;
    }

    lastn = 10;
    dl = *dirlist = (FL_Dirlist *) fl_malloc((lastn + 1) * sizeof(FL_Dirlist));
    if (fselect(&c_file, dl))
    {
	dl++;
	n++;
    }

    /* Find the rest of the files */
    while (_findnext(hFile, &c_file) == 0)
    {
	if (fselect(&c_file, dl))
	{
	    dl++;
	    n++;
	    if (n > lastn)
	    {
		lastn += 10;
		*dirlist = (FL_Dirlist *) fl_realloc(*dirlist,
					  (lastn + 1) * sizeof(FL_Dirlist));
		dl = *dirlist + n;
	    }
	}

    }
    _findclose(hFile);
    chdir(cwd);

    dl->name = 0;		/* sentinel */

    if (sort_method != FL_NONE)
	qsort(*dirlist, n, sizeof(**dirlist), tc_sort);
    return n;
}

#endif /* FL_WIN32 ] */


/********************************************************************
 * The user routine.
 *
 * Get a list of files in directory, subject to pattern matching,
 * and return the file list. Rescan will force a read even the requested
 * directory is cached.
 *
 ********************************************************************/

static char *lastdir[MAXCACHE], *lastpat[MAXCACHE];
static int lastn[MAXCACHE], last_sort[MAXCACHE];
static FL_Dirlist *dirlist[MAXCACHE];

/********************************************************************
 * Check if a particular directory is cached
 ********************************************************************/
static int
is_cached(const char *dir, const char *pat, int *c)
{
    int cached = 0, i = 0;
    static int lastcache;

    do
    {
	cached = lastpat[i] && lastdir[i] &&
	    strcmp(lastdir[i], dir) == 0 && strcmp(lastpat[i], pat) == 0 &&
	    (dirlist[i] && dirlist[i]->name);
	*c = i++;
    }
    while (!cached && i < MAXCACHE);

    /* search for the least used slot if not cached */
    if (!cached)
	*c = (++lastcache) % MAXCACHE;

    lastcache = *c;
    M_info("CheckDirCache", "%s is %s cached", dir, cached ? "" : "not");
    return cached;
}

void
fl_free_dirlist(FL_Dirlist * dl)
{
    int i, j;

    for (j = -1, i = 0; j < 0 && i < MAXCACHE; i++)
	if (dl == dirlist[i])
	    j = i;

    if (j < 0)
    {
	M_err("FreeDirList", "Bad list");
	return;
    }

    while (dl && dl->name)
    {
	fl_free(dl->name);
	dl->name = 0;		/* important: signifies empty list */
	dl++;
    }

    if ( dirlist[j])
       fl_free(dirlist[j]);
    dirlist[j] = 0;
}

void
fl_free_all_dirlist(void)
{
    int i;

    for (i = 0; i < MAXCACHE; i++)
	fl_free_dirlist(dirlist[i]);
}


/**********************************************************************
 * The user callable routine to read a directory
 *********************************************************************/

const FL_Dirlist *
fl_get_dirlist(const char *dir, const char *pattern, int *n, int rescan)
{
    int i, c;
    const char *pat = pattern;
    char okdir[FL_PATH_MAX + 1];

    if (!dir || !*dir)
	return 0;
    if (!pat || !*pat)
	pat = "*";

    /* fix the directory on the fly */
    i = strlen(strcpy(okdir, dir));
    if (okdir[i - 1] != '/')
    {
	okdir[i] = '/';
	okdir[++i] = '\0';
    }

    /* is_cached must go first to get correct cache location */
    if (!is_cached(okdir, pat, &c) || rescan)
    {
	fl_free_dirlist(dirlist[c]);
	lastn[c] = scandir_get_entries(okdir, pat, dirlist + c);
	last_sort[c] = sort_method;
	StrReDup(lastpat[c], pat);
	StrReDup(lastdir[c], okdir);
    }

    *n = lastn[c];
    if (last_sort[c] != sort_method)
    {
	qsort(dirlist[c], *n, sizeof(**dirlist), tc_sort);
	last_sort[c] = sort_method;
    }

    return dirlist[c];
}

/***********************************************************************
 * Misc. routines related directory
 **********************************************************************/
int
fl_is_valid_dir(const char *name)
{
    struct stat stbuf;

#ifdef __EMX__
    if (isalpha(name[0]) && name[1] == ':' && name[2] == '0')
	return 1;
#endif

    /* on some machines, name should be plain char * */
    return name && (stat((char *) name, &stbuf) == 0 && S_ISDIR(stbuf.st_mode));
}




/*
 * Fix directory name, such as ../../a/b etc
 */
static void add_one(char[], char[]);

char *
fl_fix_dirname(char dir[])
{
    static char ldir[FL_PATH_MAX], one[FL_PATH_MAX];
    char *p = ldir, *q = one;

    fl_b2f_slash(dir);

    if (!dir[0])
    {
	return fl_getcwd((dir ? dir : ldir), FL_PATH_MAX - 2);
    }
    else if (dir[0] == '.' && dir[1] == '.' && dir[2] == '\0')
    {
	fl_getcwd((dir ? dir : ldir), FL_PATH_MAX - 2);
	if ((p = strrchr(dir ? dir : ldir, '/')))
	    *p = '\0';
	return dir ? dir : ldir;
    }
    else if (dir[0] == '/')
    {
	if (dir[1] == '\0' ||
	    (dir[1] == '.' && dir[2] == '.' && (dir[3] == '/' || dir[3] == '\0')))
	    return strcpy(dir, "/");
    }
    strcpy(ldir, dir);
    p = ldir;

#if defined __EMX__ || defined FL_WIN32
    if (isalpha(ldir[0]) && ldir[1] == ':')
    {				/* drive letter */
	dir[0] = ldir[0];
	dir[1] = ldir[1];
	dir[2] = '\0';
	p = ldir + 2;
    }
    else
#elif defined opennt
    if (ldir[0] == '/' && ldir[1] == '/' && isalpha(ldir[2]))
    {				/* //E is E dirver */
	dir[0] = ldir[0];
	dir[1] = ldir[1];
	dir[2] = ldir[2];
	dir[3] = '\0';
	p = ldir + 3;
    }
    else
#endif
    if (ldir[0] != '/' && ldir[0] != '~')
	fl_getcwd(dir, FL_PATH_MAX - 2);
    else
	dir[0] = '\0';

    while (*p)
    {
#ifdef __EMX__
	if (*p == '/' || *p == '\\')
	{
#else
	if (*p == '/')
	{
#endif
	    *q = '\0';
	    if (q > one)
		add_one(dir, (q = one));
	}
	else
	    *q++ = *p;
	p++;
    }

    *q = '\0';
    if (q > one)
	add_one(dir, one);

#if defined __EMX__ || defined FL_WIN32
    if (strlen(dir) == 2 && dir[1] == ':')
    {				/* fix a single "C:" */
	dir[2] = '/';
	dir[3] = '\0';
    }
#endif

#if defined opennt
    if (strlen(dir) == 3 && dir[0] == '/' &&
	dir[1] == '/' && isalpha(ldir[2]))
    {				/* fix  "//C" */
	dir[3] = '/';
	dir[4] = '\0';
    }
#endif

    fl_b2f_slash(dir);

    return dir;
}

#ifndef FL_WIN32
#include <pwd.h>
#endif

static void
add_one(char dir[], char one[])
{
    char *q;

    if (one[0] == '.' && one[1] == '.' && one[2] == '\0')
    {
	if ((q = strrchr(dir, '/')))
	    *(q + (q == dir)) = '\0';
#ifndef FL_WIN32
    }
    else if (one[0] == '~')
    {
	if (one[1] == '\0')
	{			/* must be ~/ ... */
	    strcat(dir, (q = getenv("HOME")) ? q : "/");
	}
	else
	{			/* must be ~name */
	    /* Mathod: vms <7.0 has no getpwnam(). Ignore ~name */
#if !defined(__VMS) || __VMS_VER >=  70000000
	    struct passwd *p = getpwnam(one + 1);
	    strcat(dir, p ? p->pw_dir : "/");
#ifndef opennt
	    endpwent();
#endif
#endif
	}
#endif /* FL_WIN32 */
    }
    else if (!(one[0] == '.' && one[1] == '\0'))
    {
	strcat(strcat(dir, "/"), one);
    }

#ifdef __VMS
    /* VMS has directory extensions, strip it */
    {
	int n = strlen(dir);
	char *p;

	if (n > 4)
	{
	    for (p = dir + n - 4; *p; p++)
		*p = toupper(*p);
	    if (strcmp((p = dir + n - 4), ".DIR") == 0)
		*p = '\0';
	}
    }
#endif
}

/************************************************************
 * File modification time
 ***********************************************************/
unsigned long
fl_fmtime(const char *s)
{
    struct stat fffstat;
    stat((char *) s, &fffstat);
    return fffstat.st_mtime;
}

/* String matching routine is adapted from Rick Salz */

static int match_star(const char *s, const char *p);

/* match string "s" to pattern "p" */
static int
do_matching(register const char *s, register const char *p)
{
    register int last, matched, reverse;

    for (; *p; s++, p++)
    {
	if (*s == '\0')
	    return (*p == '*' && *++p == '\0' ? 1 : -1);
	switch (*p)
	{			/* parse pattern */
	case '\\':		/* Literal match with following character. */
	    if (*s != *++p)
		return 0;
	    continue;
	default:		/* literal match */
#ifdef __VMS			/* vms filenames are not case sensitive */
	    if (toupper(*s) != toupper(*p))
#else
	    if (*s != *p)
#endif
		return 0;
	    continue;
	case '?':		/* Match anything. */
	    continue;
	case '*':		/* Trailing star matches everything. */
	    return (*++p ? match_star(s, p) : 1);
	case '[':		/* [!....] means inverse character class. */
	    if ((reverse = p[1] == '!'))
		p++;
	    for (last = 0400, matched = 0; *++p && *p != ']'; last = *p)
		/* This next line requires a good C compiler. */
		/* range?        (in bounds) (equal) */
		if ((*p == '-') ? (*s <= *++p && *s >= last) : (*s == *p))
		    matched = 1;
	    if (matched == reverse)
		return 0;
	    continue;
	}
    }
    return *s == '\0';
}

static int
match_star(const char *s, const char *p)
{
    int result;

    while ((result = do_matching(s, p)) == 0)	/* gobble up * match */
	if (*++s == '\0')
	    return -1;
    return result;
}

/* check if s matches pattern p */
int
fl_wildmat(const char *s, const char *p)
{
    if (*p == '\0' && *s != '.')
	return 1;
    else if (*p == '\0')
	return 0;
    else if ((*p == '?' || *p == '*') && *s == '.')
	return 0;
    else
	return do_matching(s, p) == 1;
}


/*
 * scandir emulation
 */

#ifndef FL_WIN32		/* [  */

static int
tc_scandir(const char *dirname, struct DIRENT ***namelist)
{
    DIR *dir;
    struct DIRENT *dentry, **head;
    int n = 0, total;
    static int dname_is_1;

    if (!(dir = opendir(dirname)))
	return -1;

    if (sizeof(struct DIRENT) < 100 && !dname_is_1)
    {
	M_warn("scandir", "Bad dirent--will fix on the fly");
	dname_is_1 = 1;
    }

    /* start reading the darn thing */
    for (n = 0, head = 0; (dentry = readdir(dir));)
    {
	/* not all system has conforming realloc when head == 0 */

	head = head ? realloc(head, (n + 1) * sizeof(*head)) :
	    malloc(sizeof(*head));

	/* here it is even weirder: some systems have d_reclen =
	   sizeof(struct dirent) + strlen(d_name) and some have it as
	   d_reclen = strlen(d_name) */

	/* Mathog, VMS<7.0, at least has no d_reclen *at all */
#if defined(__VMS) && __VMS_VER < 70000000 || defined opennt || defined __CYGWIN__
	total = dname_is_1 ? strlen(dentry->d_name) : sizeof(*dentry);
#else
	total = dname_is_1 ? dentry->d_reclen : sizeof(*dentry);
#endif
	memcpy(head[n] = malloc(total), dentry, total);
	n++;
    }

    closedir(dir);
    *namelist = head;

    return n;
}

#endif /* FL_WIN32 ] */

FL_DIRLIST_FILTER
fl_set_dirlist_filter(FL_DIRLIST_FILTER filter)
{
    FL_DIRLIST_FILTER old = ffilter;
    ffilter = filter;
    return old;
}

int
fl_set_dirlist_filterdir(int yes)
{
    int old = filter_directory;
    filter_directory = yes;
    return old;
}

/* for application's benifit */
static int
default_filter(const char *name, int type)
{
    return type == FT_FILE || type == FT_DIR || type == FT_LINK;
}

int
fl_set_dirlist_sort(int method)
{
    int old = sort_method;
    sort_method = method;
    return old;
}

#ifdef __VMS
#include "vms_readdir.c"
#endif

char *
fl_getcwd(char *buf, int len)
{
#ifdef FL_WIN32
    return fl_b2f_slash(getcwd(buf, len));
#else
    return getcwd(buf, len);
#endif


}

char *
fl_basename(char name[])
{
    char *p = strrchr(name, '/');
    return p ? p : name;
}
