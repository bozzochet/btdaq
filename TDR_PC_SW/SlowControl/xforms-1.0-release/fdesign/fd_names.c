/*
 *
 * This file is part of XForms.
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
 * $Id: fd_names.c,v 1.0 2002/05/08 05:27:39 zhao Release $
 *
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 * This file is part of the Form Designer.
 *
 * It contains the routines that keep track of the names and
 * callback routines associated to the different objects.
 *
 */

#include <string.h>
#include <stdio.h>
#include "fd_main.h"

/****
  THE DATA STRUCTURE
****/

typedef struct
{
    FL_OBJECT *obj;		/* The object */
    char name[MAX_VAR_LEN];	/* Its name (if any) */
    char cbname[MAX_VAR_LEN];	/* Callback Routine */
    char argname[MAX_VAR_LEN];	/* The argument */
}
OBJ;

#define MAXOBJ  1024

static OBJ objects[MAXOBJ];	/* The stored objects */
static int objnumb = 0;		/* Their number */

/* Returns the number of the object in the list */
static int
get_object_numb(const FL_OBJECT * obj)
{
    int i;
    for (i = 0; i < objnumb; i++)
	if (objects[i].obj == obj)
	    return i;
    return -1;
}

/* Checks whether the names are correct C-names and don't occur already. */
static void
check_names(int on)
{
    /* Fill in argument if missing */
    if (objects[on].cbname[0] != '\0' && objects[on].argname[0] == '\0')
	strcpy(objects[on].argname, "0");
    /* HAS TO BE EXTENDED */
}

/****
  ACTUAL ROUTINES
****/

/* returns the names of an object */
void
get_object_name(const FL_OBJECT * obj, char *name, char *cbname, char *argname)
{
    int on = get_object_numb(obj);

    name[0] = '\0';
    cbname[0] = '\0';
    argname[0] = '\0';
    if (on == -1)
	return;
    strcpy(name, objects[on].name);
    strcpy(cbname, objects[on].cbname);
    strcpy(argname, objects[on].argname);
}

/* returns the names of an object */
void
set_object_name(FL_OBJECT * obj, const char *name,
		const char *cbname, const char *argname)
{
    int on = get_object_numb(obj);

    if (obj == NULL)
	return;

    if (on == -1)
    {
	if (name[0] == '\0' && cbname[0] == '\0' && argname[0] == '\0')
	    return;
	if (objnumb >= MAXOBJ)
	    return;
	objects[objnumb].obj = obj;
	on = objnumb++;
    }
    strcpy(objects[on].name, name);
    strcpy(objects[on].cbname, cbname);
    strcpy(objects[on].argname, argname);
    check_names(on);
}
