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
 * $Id: sp_menu.c,v 1.0 2002/05/08 05:27:39 zhao Release $
 *
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 * Settting menu class specific attributes.
 *
 */

#include "forms.h"
#include "fd_main.h"
#include "fd_spec.h"
#include "pmenu.h"
#include "menu_spec.h"
#include <ctype.h>

extern FD_menuattrib *create_form_menuattrib(void);
static FD_menuattrib *menu_attrib;

static SuperSPEC *menu_spec;
static void show_spec(SuperSPEC *);

void *
get_menu_spec_fdform(void)
{
    if (!menu_attrib)
    {
	menu_attrib = create_form_menuattrib();
	fl_addto_choice(menu_attrib->mode, get_pupmode_string());
	fl_addto_choice(menu_attrib->scope, "local|global");
	fl_set_choice_item_shortcut(menu_attrib->mode, 1, "Nn#N");
	fl_set_choice_item_shortcut(menu_attrib->mode, 1, "Gg#G");
	fl_set_choice_item_shortcut(menu_attrib->mode, 1, "Bb#B");
	fl_set_choice_item_shortcut(menu_attrib->mode, 1, "Cc#C");
	fl_set_choice_item_shortcut(menu_attrib->mode, 1, "Rr#R");
	fl_set_browser_dblclick_callback(menu_attrib->content_br,
					 change_menu_item_cb, 0);
    }

    return menu_attrib;
}

void
menu_spec_restore(FL_OBJECT * ob, long data)
{
    FL_OBJECT *edited = menu_attrib->vdata;

    superspec_to_spec(edited);
    show_spec(get_superspec(edited));
    redraw_the_form(0);
}

static void
show_spec(SuperSPEC * sp)
{
    int i;

    fl_freeze_form(menu_attrib->content_br->form);

    fl_set_button(menu_attrib->new_menuapi, sp->new_menuapi);
    fl_clear_browser(menu_attrib->content_br);
    for (i = 1; i <= sp->nlines; i++)
	fl_add_browser_line(menu_attrib->content_br, sp->content[i]);

    fl_set_choice(menu_attrib->scope, sp->global_scope + 1);

    fl_unfreeze_form(menu_attrib->content_br->form);
}

int
set_menu_attrib(FL_OBJECT * ob)
{
    menu_attrib->vdata = ob;
    menu_spec = get_superspec(ob);
    superspec_to_spec(ob);

    show_spec(menu_spec);

    return 0;
}

/* create a name for the [menu|choice]_entry API and hang it off
 * sp->misc_char
 */
static char *
get_pupentry_name(FL_OBJECT * ob)
{
    static int n = 0;
    char pupname[128], *p;
    char objname[128], cbname[128], argname[128];
    char *what = ob->objclass == FL_MENU ? "fdmenu" : "fdchoice";
    SuperSPEC *sp = get_superspec(ob);
    int i;

    if (!sp->new_menuapi || sp->nlines <= 0)
	return "";

    if (sp->misc_char && *(sp->misc_char))
	return sp->misc_char;

    get_object_name(ob, objname, cbname, argname);

    if (objname[0])
	sprintf(pupname, "%s_%s_%d", what, objname, n);
    else if (*ob->label)
	sprintf(pupname, "%s_%s_%d", what, ob->label, n);
    else
	sprintf(pupname, "%s_%d", what, n);

    n++;

    /* get rid of illegal chars */
    for (i = 0, p = pupname; *p; p++)
	if (isalnum( ( int ) *p) || *p == '_')
	    pupname[i++] = *p;

    pupname[i++] = '\0';
    sp->misc_char = fl_strdup(pupname);

    return sp->misc_char;
}


/* emit things that are needed before code emission (file scope) */
void
emit_menu_header(FILE * fp, FL_OBJECT * ob)
{
    SuperSPEC *sp = get_superspec(ob);
    int i;

    if (!sp->new_menuapi || sp->nlines <= 0)
	return;

    get_pupentry_name(ob);

    fprintf(fp, "%sFL_PUP_ENTRY %s[] =\n{ \n",
	    sp->global_scope ? "" : "static ", sp->misc_char);
    fprintf(fp, "    /*  itemtext   callback  shortcut   mode */\n");

    for (i = 1; i <= sp->nlines; i++)
    {
	fprintf(fp, "    { \"%s\",\t0,\t\"%s\",\t %s},\n",
		sp->content[i], sp->shortcut[i] ? sp->shortcut[i] : "",
		get_pupmode_name(sp->mode[i]));
    }

    /* sentinel */
    fprintf(fp, "    {0}\n};\n\n");
}

/* emit header info that is global in nature */
void
emit_menu_global(FILE * fp, FL_OBJECT * ob)
{
    SuperSPEC *sp = get_superspec(ob);

    if (!sp->new_menuapi || sp->nlines <= 0 || !sp->global_scope)
	return;

    get_pupentry_name(ob);

    fprintf(fp, "extern FL_PUP_ENTRY %s[];\n", sp->misc_char);
}

void
emit_menu_code(FILE * fp, FL_OBJECT * ob)
{
    FL_OBJECT *defobj;
    SuperSPEC *sp, *defsp;
    int i;

    /* create a default object */
    defobj = fl_create_menu(ob->type, 0, 0, 0, 0, "");

    defsp = get_superspec(defobj);
    sp = get_superspec(ob);

    if (sp->nlines == 0)
	return;

    if (sp->new_menuapi)
    {
	fprintf(fp, "    fl_set_menu_entries(obj, %s);\n", sp->misc_char);
    }
    else
    {

	for (i = 1; i <= sp->nlines; i++)
	{
	    fprintf(fp, "    fl_addto_menu(obj, \"%s\");\n", sp->content[i]);
	    if (sp->mode[i] != defsp->mode[i])
		fprintf(fp, "    fl_set_menu_item_mode(obj, %d, %s);\n",
			i, get_pupmode_name(sp->mode[i]));
	    if (sp->shortcut[i] && *sp->shortcut[i])
		fprintf(fp, "    fl_set_menu_item_shortcut(obj, %d, \"%s\");\n",
			i, sp->shortcut[i]);
	}
    }

}

void
save_menu_attrib(FILE * fp, FL_OBJECT * ob)
{
    FL_OBJECT *defobj;
    SuperSPEC *defsp, *sp;
    int i;

    /* create a default object */
    defobj = fl_create_menu(ob->type, 0, 0, 0, 0, "");

    defsp = get_superspec(defobj);
    sp = get_superspec(ob);

    if (sp->new_menuapi != defsp->new_menuapi)
	fprintf(fp, "  struct: %d\n", sp->new_menuapi);
    if (sp->global_scope != defsp->global_scope)
	fprintf(fp, "  global: %d\n", sp->global_scope);

    for (i = 1; i <= sp->nlines; i++)
    {
	fprintf(fp, "  content: %s\n", sp->content[i]);
	if (sp->mode[i] != defsp->mode[i])
	    fprintf(fp, "  mode: %s\n", get_pupmode_name(sp->mode[i]));
	if (sp->shortcut[i] && *sp->shortcut[i])
	    fprintf(fp, "  shortcut: %s\n", sp->shortcut[i]);
    }
}

/*
 * attributes callbacks
 */

/* callbacks and freeobj handles for form choiceattrib */
void
add_menu_item_cb(FL_OBJECT * ob, long data)
{
    FD_menuattrib *ui = ob->form->fdui;
    const char *s = fl_get_input(ui->input);
    const char *sc = fl_get_input(ui->shortcut);
    const char *mode = fl_get_choice_text(ui->mode);
    int i;

    if (s && *s)
    {
	fl_addto_browser(ui->content_br, s);
	i = fl_addto_menu(ui->vdata, s);
	fl_set_menu_item_shortcut(ui->vdata, i, sc);
	fl_set_menu_item_mode(ui->vdata, i, get_pupmode_value(mode));

	if (fl_get_button(ui->auto_clear))
	    clear_menu_field_cb(ui->auto_clear, 0);
	if (auto_apply)
	    redraw_the_form(0);
    }
}

void
replace_menu_item_cb(FL_OBJECT * ob, long data)
{
    FD_menuattrib *ui = ob->form->fdui;
    int i = fl_get_browser(ui->content_br);
    const char *s = fl_get_input(ui->input);
    const char *sc = fl_get_input(ui->shortcut);
    const char *mode = fl_get_choice_text(ui->mode);

    if (*s && i > 0)
    {
	fl_replace_browser_line(ui->content_br, i, s);
	fl_replace_menu_item(ui->vdata, i, s);
	fl_set_menu_item_shortcut(ui->vdata, i, sc);
	fl_set_menu_item_mode(ui->vdata, i, get_pupmode_value(mode));

	if (fl_get_button(ui->auto_clear))
	    clear_menu_field_cb(ui->auto_clear, 0);
    }

    if (auto_apply)
	redraw_the_form(0);
}


void
delete_menu_item_cb(FL_OBJECT * ob, long data)
{
    FD_menuattrib *ui = ob->form->fdui;
    int i = fl_get_browser(ui->content_br);

    if (i > 0)
    {
	fl_delete_browser_line(ui->content_br, i);
	fl_delete_menu_item(ui->vdata, i);
	if (auto_apply)
	    redraw_the_form(0);
    }
}

void
change_menu_item_cb(FL_OBJECT * ob, long data)
{
    FD_menuattrib *ui = ob->form->fdui;
    int i = fl_get_browser(ui->content_br);
    FL_OBJECT *edited = ui->vdata;
    FL_MENU_SPEC *sp = edited->spec;

    if (i > 0)
    {
	fl_set_input(ui->input, fl_get_browser_line(ui->content_br, i));
	if (sp->shortcut[i])
	    fl_set_input(ui->shortcut, sp->shortcut[i]);
	fl_set_choice_text(ui->mode, get_pupmode_name(sp->mode[i]) + 3);
    }
}

void
clear_menu_field_cb(FL_OBJECT * ob, long data)
{
    FD_menuattrib *ui = ob->form->fdui;

    fl_set_input(ui->input, "");
    fl_set_input(ui->shortcut, "");
    fl_set_choice(ui->mode, 1);
}

void
new_menuapi_cb(FL_OBJECT * ob, long data)
{
    menu_spec->new_menuapi = fl_get_button(ob);
}

void
menuentry_scope_cb(FL_OBJECT * ob, long data)
{
    menu_spec->global_scope = (fl_get_choice(ob) - 1) > 0;
}

#include "menu_spec.c"
