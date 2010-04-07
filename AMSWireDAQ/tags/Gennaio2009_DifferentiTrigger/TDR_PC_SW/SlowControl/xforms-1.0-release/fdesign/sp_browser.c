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
 * $Id: sp_browser.c,v 1.0 2002/05/08 05:27:39 zhao Release $
 *
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 * Settting browser class specific attributes.
 *
 */

#include "forms.h"
#include "fd_main.h"
#include "fd_spec.h"
#include "pbrowser.h"
#include "browser_spec.h"

extern FD_browserattrib *create_form_browserattrib(void);
static FD_browserattrib *br_attrib;

static SuperSPEC *browser_spec;
static void show_spec(SuperSPEC *);
void change_item_cb(FL_OBJECT * ob, long data);

void *
get_browser_spec_fdform(void)
{
    if (!br_attrib)
    {
	br_attrib = create_form_browserattrib();
	fl_addto_choice(br_attrib->hscb_pref, get_scrollbar_pref_string());
	fl_addto_choice(br_attrib->vscb_pref, get_scrollbar_pref_string());
	fl_set_browser_dblclick_callback(br_attrib->content_br,
					 change_item_cb, 0);
    }

    return br_attrib;
}

void
browser_spec_restore(FL_OBJECT * ob, long data)
{
    FL_OBJECT *edited = br_attrib->vdata;

    superspec_to_spec(edited);
    show_spec(get_superspec(edited));
    redraw_the_form(0);
}

static void
show_spec(SuperSPEC * spec)
{
    int i;

    fl_set_choice(br_attrib->hscb_pref, spec->h_pref + 1);
    fl_set_choice(br_attrib->vscb_pref, spec->v_pref + 1);

    fl_freeze_form(br_attrib->content_br->form);
    fl_clear_browser(br_attrib->content_br);
    for (i = 1; i <= spec->nlines; i++)
	fl_add_browser_line(br_attrib->content_br, spec->content[i]);
    fl_unfreeze_form(br_attrib->content_br->form);
}

int
set_browser_attrib(FL_OBJECT * ob)
{
    br_attrib->vdata = ob;
    browser_spec = get_superspec(ob);
    superspec_to_spec(ob);

    show_spec(browser_spec);

    return 0;
}

void
emit_browser_code(FILE * fp, FL_OBJECT * ob)
{
    FL_OBJECT *defobj;
    SuperSPEC *spec, *defspec;
    int i;

    if (ob->objclass != FL_BROWSER)
	return;

    /* create a default object */
    defobj = fl_create_browser(ob->type, 0, 0, 0, 0, "");

    defspec = get_superspec(defobj);
    spec = get_superspec(ob);


    if (spec->h_pref != defspec->h_pref)
	fprintf(fp, "    fl_set_browser_hscrollbar(obj, %s);\n",
		get_scrollbar_pref_name(spec->h_pref));
    if (spec->v_pref != defspec->v_pref)
	fprintf(fp, "    fl_set_browser_vscrollbar(obj, %s);\n",
		get_scrollbar_pref_name(spec->v_pref));

    for (i = 1; i <= spec->nlines; i++)
	fprintf(fp, "    fl_add_browser_line(obj, \"%s\");\n",
		spec->content[i]);
}

void
save_browser_attrib(FILE * fp, FL_OBJECT * ob)
{
    FL_OBJECT *defobj;
    SuperSPEC *defspec, *spec;
    int i;

    if (ob->objclass != FL_BROWSER)
	return;

    /* create a default object */
    defobj = fl_create_browser(ob->type, 0, 0, 0, 0, "");

    defspec = get_superspec(defobj);
    spec = get_superspec(ob);

    if (defspec->h_pref != spec->h_pref)
	fprintf(fp, "  h_pref: %s\n", get_scrollbar_pref_name(spec->h_pref));
    if (defspec->v_pref != spec->v_pref)
	fprintf(fp, "  v_pref: %s\n", get_scrollbar_pref_name(spec->v_pref));

    for (i = 1; i <= spec->nlines; i++)
	fprintf(fp, "  content: %s\n", spec->content[i]);

}

/*
 * attributes callbacks
 */


/* callbacks and freeobj handles for form browserattrib */
void
add_item_cb(FL_OBJECT * ob, long data)
{
    FD_browserattrib *ui = ob->form->fdui;
    const char *s = fl_get_input(ui->input);

    if (s && *s)
    {
	fl_addto_browser(ui->content_br, s);
	fl_addto_browser(ui->vdata, s);
	if (fl_get_button(ui->auto_clear))
	    fl_set_input(ui->input, "");
	if (auto_apply)
	    redraw_the_form(0);
    }
}

void
replace_item_cb(FL_OBJECT * ob, long data)
{
    FD_browserattrib *ui = ob->form->fdui;
    int i = fl_get_browser(ui->content_br);
    const char *s = fl_get_input(ui->input);

    if (*s && i > 0)
    {
	fl_replace_browser_line(ui->content_br, i, s);
	fl_replace_browser_line(ui->vdata, i, s);
	if (fl_get_button(ui->auto_clear))
	    fl_set_input(ui->input, "");
    }

    if (auto_apply)
	redraw_the_form(0);
}

void
insert_cb(FL_OBJECT * ob, long data)
{
    FD_browserattrib *ui = ob->form->fdui;
    int i = fl_get_browser(ui->content_br);
    const char *s = fl_get_input(ui->input);

    if (*s && i > 0)
    {
	fl_insert_browser_line(ui->content_br, i, s);
	fl_insert_browser_line(ui->vdata, i, s);
	if (fl_get_button(ui->auto_clear))
	    fl_set_input(ui->input, "");
    }

    if (auto_apply)
	redraw_the_form(0);
}

void
delete_item_cb(FL_OBJECT * ob, long data)
{
    FD_browserattrib *ui = ob->form->fdui;
    int i = fl_get_browser(ui->content_br);

    if (i > 0)
    {
	fl_delete_browser_line(ui->content_br, i);
	fl_delete_browser_line(ui->vdata, i);
	if (auto_apply)
	    redraw_the_form(0);
    }
}

void
change_item_cb(FL_OBJECT * ob, long data)
{
    FD_browserattrib *ui = ob->form->fdui;
    int i = fl_get_browser(ui->content_br);

    if (i > 0)
	fl_set_input(ui->input, fl_get_browser_line(ui->content_br, i));
}

void
hscb_pref_cb(FL_OBJECT * ob, long data)
{
    FD_browserattrib *ui = ob->form->fdui;
    int i = fl_get_choice(ob);

    fl_set_browser_hscrollbar(ui->vdata, i - 1);

    if (auto_apply)
	redraw_the_form(0);

}

void
vscb_pref_cb(FL_OBJECT * ob, long data)
{
    FD_browserattrib *ui = ob->form->fdui;
    int i = fl_get_choice(ob);

    fl_set_browser_vscrollbar(ui->vdata, i - 1);

    if (auto_apply)
	redraw_the_form(0);

}

void
clear_field_cb(FL_OBJECT * ob, long data)
{
    FD_browserattrib *ui = ob->form->fdui;

    fl_set_input(ui->input, "");
}


#include "browser_spec.c"
