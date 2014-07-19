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
 * $Id: sp_freeobj.c,v 1.0 2002/05/08 05:27:39 zhao Release $
 *
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 * Settting free object class specific attributes, in this
 * case, the handler name.  We store this piece of into
 * in ob->c_vdata;
 *
 */

#include "forms.h"
#include "fd_main.h"
#include "fd_spec.h"
#include <ctype.h>
#include "spec/freeobj_spec.h"

static char *get_free_handle(FL_OBJECT * ob, const char *name);
static char ori_handle_name[128];

extern FD_freeobjattrib *create_form_freeobjattrib(void);

static FD_freeobjattrib *fo_attrib;
static SuperSPEC *freeobj_spec;
static void show_spec(SuperSPEC *);
static FL_OBJECT *edited;

void *
get_freeobj_spec_fdform(void)
{
    if (!fo_attrib)
	fo_attrib = create_form_freeobjattrib();
    return fo_attrib;
}

void
freeobj_spec_restore(FL_OBJECT * ob, long data)
{
    if (ob->c_vdata)
	fl_free(ob->c_vdata);
    ob->c_vdata = fl_strdup(ori_handle_name);

}

static void
show_spec(SuperSPEC * spec)
{
    fl_set_input(fo_attrib->hname, get_free_handle(edited, 0));
}

int
set_freeobj_attrib(FL_OBJECT * ob)
{
    fo_attrib->vdata = edited = ob;

    ori_handle_name[0] = '\0';
    if (ob->c_vdata)
	strcpy(ori_handle_name, ob->c_vdata);

    show_spec(freeobj_spec);
    return 0;
}

int
noop_handle(FL_OBJECT * ob, int e, FL_Coord mx, FL_Coord my, int k, void *xev)
{
    if (e == FL_DRAW)
    {
	fl_drw_box(ob->boxtype, ob->x, ob->y, ob->w, ob->h, ob->col1, ob->bw);
	return 0;
    }

    if (ob->type == FL_INACTIVE_FREE)
	return 0;
    if (ob->type == FL_INPUT_FREE)
	return (e == FL_KEYBOARD);
    return 1;
}

#if 0
static FL_OBJECT *
create_a_freeobj(FL_OBJECT * ob)
{
    FL_OBJECT *defobj = 0;

    defobj = fl_create_free(ob->type, ob->x, ob->y, ob->w, ob->h,
			    ob->label, noop_handle);

    return defobj;
}
#endif


void
emit_freeobj_code(FILE * fp, FL_OBJECT * ob)
{
}


void
save_freeobj_attrib(FILE * fp, FL_OBJECT * ob)
{
    if (ob->c_vdata)
	fprintf(fp, "   handler: %s\n", (char *) ob->c_vdata);
}

void
handler_name_change_cb(FL_OBJECT * ob, long data)
{
    edited->c_vdata = fl_strdup(fl_get_input(ob));
}


/* default free object handler name */
#define MAXFREEOBJ 16
static char *
get_free_handle(FL_OBJECT * ob, const char *name)
{
    static int n;
    static char buf[1024];
    static FL_OBJECT *freeobj[MAXFREEOBJ];
    int i, k;

    if (ob->c_vdata)
	strcpy(buf, ob->c_vdata);
    else if (name && *name)
	sprintf(buf, "freeobj_%s_handle", name);
    else if (*ob->label)
	sprintf(buf, "freeobj_%s_handle", ob->label);
    else
    {
	for (k = -1, i = 0; i < MAXFREEOBJ && k < 0; i++)
	{
	    if (freeobj[i] == ob)
		k = i;
	}

	if (k < 0)
	{
	    k = ++n;
	    freeobj[k] = ob;
	}
	sprintf(buf, "freeobj%d_handle", k);
    }

    return buf;
}


#include "spec/freeobj_spec.c"
