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

/********************** crop here for forms.h **********************/

/*
 * $Id: filesys.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *  Convenience functions to read a directory
 */

#ifndef FL_FILESYS_H
#define FL_FILESYS_H

/*  File types */
enum
{
    FT_FILE, FT_DIR, FT_LINK, FT_SOCK,
    FT_FIFO, FT_BLK, FT_CHR, FT_OTHER
};

typedef struct
{
    char *name;			/* entry name             */
    int type;			/* FILE_TYPE              */
    long dl_mtime;		/* file modification time */
    unsigned long dl_size;	/* file size in bytes     */
    long filler[3];		/* reserved               */
}
FL_Dirlist;

enum
{
    FL_ALPHASORT = 1,		/* sort in alphabetic order           */
    FL_RALPHASORT,		/* sort in reverse alphabetic order   */
    FL_MTIMESORT,		/* sort according to modifcation time */
    FL_RMTIMESORT,		/* sort in reverse modificaiton time  */
    FL_SIZESORT,		/* sort in increasing size order      */
    FL_RSIZESORT,		/* sort in decreasing size order      */
    FL_CASEALPHASORT,		/* sort case insensitive              */
    FL_RCASEALPHASORT		/* sort case insensitive              */
};

typedef int (*FL_DIRLIST_FILTER) (const char *, int);

/* read dir with pattern filtering. All dirs read might be cached.
 * must not change dirlist in anyway.
 */
FL_EXPORT const FL_Dirlist *fl_get_dirlist(
		const char *dir,
		const char *pattern,
		int *n,
		int rescan
		);


FL_EXPORT FL_DIRLIST_FILTER fl_set_dirlist_filter(
		FL_DIRLIST_FILTER filter
		);

FL_EXPORT int fl_set_dirlist_sort(
		int method
		);

FL_EXPORT int fl_set_dirlist_filterdir(
		int yes
		);


FL_EXPORT void fl_free_dirlist(
		FL_Dirlist *dl
		);


/* Free all directory caches */
FL_EXPORT void fl_free_all_dirlist(
		void
		);



FL_EXPORT int fl_is_valid_dir(
		const char *name
		);

FL_EXPORT unsigned long fl_fmtime(
		const char *s
		);

FL_EXPORT char *fl_fix_dirname(
		char dir[]
		);


#endif
