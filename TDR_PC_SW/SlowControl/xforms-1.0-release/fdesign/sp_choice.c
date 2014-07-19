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
 * $Id: sp_choice.c,v 1.0 2002/05/08 05:27:39 zhao Release $
 *
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 * Settting choice class specific attributes.
 *
 */

#include "forms.h"
#include "fd_main.h"
#include "fd_spec.h"
#include "pchoice.h"
#include "choice_spec.h"

extern FD_choiceattrib *create_form_choiceattrib(void);
static FD_choiceattrib *choice_attrib;

static SuperSPEC *choice_spec;
static void show_spec(SuperSPEC *);

void *
get_choice_spec_fdform(void)
{
    if (!choice_attrib)
    {
	choice_attrib = create_form_choiceattrib();
	fl_addto_choice(choice_attrib->mode, get_pupmode_string());
	fl_addto_choice(choice_attrib->scope, "local|global");
	fl_set_choice_item_mode(choice_attrib->mode, 3, FL_PUP_GRAY);
	fl_set_choice_item_mode(choice_attrib->mode, 4, FL_PUP_GRAY);
	fl_set_choice_item_mode(choice_attrib->mode, 5, FL_PUP_GRAY);

	fl_addto_choice(choice_attrib->align, align_name(FL_ALIGN_CENTER));
	fl_addto_choice(choice_attrib->align, align_name(FL_ALIGN_TOP));
	fl_addto_choice(choice_attrib->align, align_name(FL_ALIGN_BOTTOM));
	fl_addto_choice(choice_attrib->align, align_name(FL_ALIGN_LEFT));
	fl_addto_choice(choice_attrib->align, align_name(FL_ALIGN_RIGHT));

	fl_set_browser_dblclick_callback(choice_attrib->content_br,
					 change_choice_item_cb, 0);
    }

    return choice_attrib;
}

void
choice_spec_restore(FL_OBJECT * ob, long data)
{
    FL_OBJECT *edited = choice_attrib->vdata;

    superspec_to_spec(edited);
    show_spec(get_superspec(edited));
    redraw_the_form(0);
}

static void
show_spec(SuperSPEC * sp)
{
    int i;

    fl_freeze_form(choice_attrib->content_br->form);

    fl_set_button(choice_attrib->new_menuapi, sp->new_menuapi);
    fl_set_counter_value(choice_attrib->val, sp->int_val);
    fl_set_choice_text(choice_attrib->align, align_name(sp->align));
    fl_set_choice(choice_attrib->scope, sp->global_scope + 1);
    fl_clear_browser(choice_attrib->content_br);
    for (i = 1; i <= sp->nlines; i++)
	fl_add_browser_line(choice_attrib->content_br, sp->content[i]);

    fl_unfreeze_form(choice_attrib->content_br->form);
}

int
set_choice_attrib(FL_OBJECT * ob)
{
    choice_attrib->vdata = ob;
    choice_spec = get_superspec(ob);
    superspec_to_spec(ob);

    fl_set_counter_bounds(choice_attrib->val, 1,
		      (choice_spec->nlines >= 1 ? choice_spec->nlines : 1));

    show_spec(choice_spec);

    return 0;
}

void
emit_choice_code(FILE * fp, FL_OBJECT * ob)
{
    FL_OBJECT *defobj;
    SuperSPEC *sp, *defsp;
    int i;

    /* create a default object */
    defobj = fl_create_choice(ob->type, 0, 0, 0, 0, "");

    defsp = get_superspec(defobj);
    sp = get_superspec(ob);

    if (sp->align != defsp->align)
	fprintf(fp, "    fl_set_choice_align(obj, %s);\n", align_name(sp->align));

    if (sp->nlines < 1)
	return;

    if (sp->new_menuapi)
    {
	fprintf(fp, "    fl_set_choice_entries(obj, %s);\n", sp->misc_char);
    }
    else
    {
	for (i = 1; i <= sp->nlines; i++)
	{
	    fprintf(fp, "    fl_addto_choice(obj, \"%s\");\n", sp->content[i]);
	    if (sp->mode[i] != defsp->mode[i])
		fprintf(fp, "    fl_set_choice_item_mode(obj, %d, %s);\n",
			i, get_pupmode_name(sp->mode[i]));
	    if (sp->shortcut[i] && *sp->shortcut[i])
		fprintf(fp, "    fl_set_choice_item_shortcut(obj, %d, \"%s\");\n",
			i, sp->shortcut[i]);
	}
    }

    if (sp->int_val != defsp->int_val)
	fprintf(fp, "    fl_set_choice(obj,%d);\n", sp->int_val);
}

void
save_choice_attrib(FILE * fp, FL_OBJECT * ob)
{
    FL_OBJECT *defobj;
    SuperSPEC *defsp, *sp;
    int i;

    /* create a default object */
    defobj = fl_create_choice(ob->type, 0, 0, 0, 0, "");

    defsp = get_superspec(defobj);
    sp = get_superspec(ob);

    if (sp->align != defsp->align)
	fprintf(fp, "  align: %s\n", align_name(sp->align));
    if (sp->new_menuapi != defsp->new_menuapi)
	fprintf(fp, "  struct: %d\n", sp->new_menuapi);

    for (i = 1; i <= sp->nlines; i++)
    {
	fprintf(fp, "  content: %s\n", sp->content[i]);
	if (sp->mode[i] != defsp->mode[i])
	    fprintf(fp, "  mode: %s\n", get_pupmode_name(sp->mode[i]));
	if (sp->shortcut[i] && *sp->shortcut[i])
	    fprintf(fp, "  shortcut: %s\n", sp->shortcut[i]);
    }

    if (sp->int_val != defsp->int_val)
	fprintf(fp, "  value: %d\n", sp->int_val);
}

/*
 * attributes callbacks
 */


/* callbacks and freeobj handles for form choiceattrib */
void
add_choice_item_cb(FL_OBJECT * ob, long data)
{
    FD_choiceattrib *ui = ob->form->fdui;
    const char *s = fl_get_input(ui->input);
    const char *sc = fl_get_input(ui->shortcut);
    const char *mode = fl_get_choice_text(ui->mode);
    int i;

    if (s && *s)
    {
	fl_addto_browser(ui->content_br, s);
	i = fl_addto_choice(ui->vdata, s);
	fl_set_choice_item_shortcut(ui->vdata, i, sc);
	fl_set_choice_item_mode(ui->vdata, i, get_pupmode_value(mode));

	if (fl_get_button(ui->auto_clear))
	    clear_choice_field_cb(ui->auto_clear, 0);

	if (auto_apply)
	    redraw_the_form(0);

	fl_set_counter_bounds(ui->val, 1,
			      fl_get_choice_maxitems(ui->vdata));
    }
}

void
replace_choice_item_cb(FL_OBJECT * ob, long data)
{
    FD_choiceattrib *ui = ob->form->fdui;
    int i = fl_get_browser(ui->content_br);
    const char *s = fl_get_input(ui->input);
    const char *sc = fl_get_input(ui->shortcut);
    const char *mode = fl_get_choice_text(ui->mode);

    if (*s && i > 0)
    {
	fl_replace_browser_line(ui->content_br, i, s);
	fl_replace_choice(ui->vdata, i, s);
	fl_set_choice_item_shortcut(ui->vdata, i, sc);
	fl_set_choice_item_mode(ui->vdata, i, get_pupmode_value(mode));

	if (fl_get_button(ui->auto_clear))
	    clear_choice_field_cb(ui->auto_clear, 0);
    }

    if (auto_apply)
	redraw_the_form(0);
}

void
delete_choice_item_cb(FL_OBJECT * ob, long data)
{
    FD_choiceattrib *ui = ob->form->fdui;
    int i = fl_get_browser(ui->content_br);

    if (i > 0)
    {
	fl_delete_browser_line(ui->content_br, i);
	fl_delete_choice(ui->vdata, i);
	if (auto_apply)
	    redraw_the_form(0);
    }
}

void
change_choice_item_cb(FL_OBJECT * ob, long data)
{
    FD_choiceattrib *ui = ob->form->fdui;
    int i = fl_get_browser(ui->content_br);
    FL_OBJECT *edited = ui->vdata;
    FL_CHOICE_SPEC *sp = edited->spec;

    if (i > 0)
    {
	fl_set_input(ui->input, fl_get_browser_line(ui->content_br, i));
	if (sp->shortcut[i])
	    fl_set_input(ui->shortcut, sp->shortcut[i]);
	fl_set_choice_text(ui->mode, get_pupmode_name(sp->mode[i]) + 3);
    }
}

void
clear_choice_field_cb(FL_OBJECT * ob, long data)
{
    FD_choiceattrib *ui = ob->form->fdui;

    fl_set_input(ui->input, "");
    fl_set_input(ui->shortcut, "");
    fl_set_choice(ui->mode, 1);
}

void
choice_align_cb(FL_OBJECT * ob, long data)
{
    FD_choiceattrib *ui = ob->form->fdui;
    const char *s = fl_get_choice_text(ob);

    fl_set_choice_align(ui->vdata, align_val(s));
    if (auto_apply)
	redraw_the_form(0);
}

void
choice_val_cb(FL_OBJECT * ob, long data)
{
    FD_choiceattrib *ui = ob->form->fdui;
    int v = fl_get_counter_value(ob) + 0.1;

    fl_set_choice(ui->vdata, v);
    if (auto_apply)
	redraw_the_form(0);

}

void
use_struct_cb(FL_OBJECT * ob, long data)
{
    choice_spec->new_menuapi = fl_get_button(ob);
}

void
choiceentry_scope_cb(FL_OBJECT * ob, long data)
{
    choice_spec->global_scope = (fl_get_choice(ob) - 1) > 0;
}


#include "choice_spec.c"
