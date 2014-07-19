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
 * $Id: fd_control.c,v 1.0 2002/05/08 05:27:39 zhao Release $
 *
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *  Form designer Control panel handling
 *
 */

#include "forms.h"
#include "fd_main.h"
#include "ui_theforms.h"

/* All control panel menus, File Object Options etc. have this
 * common structures
 */
typedef struct
{
    const char *entry;		/* label     */
    const char *sc;		/* shortcut  */
    void (*callback) (FL_OBJECT *, long);	/* callback functions */
    int n;			/* func_cb parameter */
    int *p;			/* binary value      */
}
MenuEntry;

extern void exit_cb(FL_OBJECT *, long), saveforms_cb(FL_OBJECT *, long);

/**** FileMenu ******{*/

static MenuEntry fmenu[] =
{
    {" Open", "Oo#o", loadforms_cb},
    {" Merge", "Mm#m", mergeforms_cb},
    {" Save", "Ss#s", saveforms_cb},
    {" Save As %l", "Aa#a", saveforms_as_cb},
    {" Exit", "Ee#e", exit_cb}
};

#define NFM (sizeof(fmenu)/sizeof(fmenu[0]))

void
filemenu_callback(FL_OBJECT * ob, long data)
{
    int n = fl_get_menu(ob) - 1;

    if (n >= 0 && fmenu[n].callback)
	fmenu[n].callback(0, 0);
}

/* Exit the program */
void
exit_cb(FL_OBJECT * obj, long arg)
{
    int rep;

    if (changed)
    {
	fl_set_choices_shortcut("Ss#s", "Ee#e", "Rr#r");
	rep = fl_show_choice("WARNING", "", "Changes have not been saved.",
			     3, "Save", "Exit", "Return", 1);
	if (rep == 1)
	{
	    if (!save_forms(NULL))
		return;
	}
	else if (rep == 3)
	    return;
    }

    fl_finish();
    exit(0);
}


/* Escape or clicked on the control panel. If escape, quit */
void
esc_exit_cb(FL_OBJECT * obj, long arg)
{
    if (fl_get_button_numb(obj) > 3)
	exit_cb(0, 0);
}

/* Merge a set of forms with the current ones. */
void
mergeforms_cb(FL_OBJECT * obj, long arg)
{
    load_forms(TRUE, NULL, 0);
    changed = 1;
    loadedfile = 0;
}

/* Save the current set of forms. */
void
saveforms_cb(FL_OBJECT * obj, long arg)
{
    changed = !save_forms(loadedfile_fullpath);
}

void
saveforms_as_cb(FL_OBJECT * obj, long arg)
{
    changed = !save_forms(0);
}

/* Load a new set of forms */
void
loadforms_cb(FL_OBJECT * obj, long arg)
{
    if (changed)
    {
	if (fl_show_question("Forms have been changed!\nShould I save them?", 1))
	{
	    if (!save_forms(NULL))
		return;
	}
    }
    load_forms(FALSE, NULL, 1);
    changed = 0;
}

/***** End of FileMenu *****/


/* Group Menu entry */
static MenuEntry gmenu[] =
{
    {"New group", "Nn#n", 0, 7},
    {"Delete Group", "Dd#d", 0, 8},
    {"Rename Group", "Rr#r", changegroupname_cb}
};
#define NGM (sizeof(gmenu)/sizeof(gmenu[0]))

void
groupmenu_callback(FL_OBJECT * ob, long data)
{
    int n = fl_get_menu(ob) - 1;

    if (n >= 0)
    {
	if (gmenu[n].callback)
	    gmenu[n].callback(0, 0);
	else
	    func_cb(0, gmenu[n].n);
    }
}

/* object menu entry */
static MenuEntry obmenu[] =
{
    {"Object Attributes", "Oo#o", 0, 1},
    {"Lower Object", "Ll#l", 0, 2},
    {"Raise Object", "Rr#r", 0, 3},
    {"Show Object", "Ss#s", 0, 5},
    {"Hide Object", "Hh#h", 0, 6},
    {"Cut Object", "Cc#c", 0, 12},
    {"Paste Object", "Pp#p", 0, 10}
};
#define NOBM  (sizeof(obmenu)/sizeof(obmenu[0]))

void
objectmenu_callback(FL_OBJECT * ob, long data)
{
    int n = fl_get_menu(ob) - 1;

    if (n >= 0)
	func_cb(0, obmenu[n].n);
}

/* form menu entry */
static MenuEntry fmmenu[] =
{
    {"New Form", "Nn#n", addform_cb},
    {"Delete Form", "Dd#d", deleteform_cb},
    {"Rename Form", "Rr#r", changename_cb}
};
#define NFMM (sizeof(fmmenu)/sizeof(fmmenu[0]))

void
formmenu_callback(FL_OBJECT * ob, long data)
{
    int n = fl_get_menu(ob) - 1;
    if (n >= 0 && fmmenu[n].callback)
	fmmenu[n].callback(0, 0);
}

/* Option menu */
static MenuEntry opmenu[] =
{
    {"Track Geometry", "Gg#g", 0, 0, &fd_trackgeometry},
    {"Show Pallette", "Pp#p", 0, 0, &fd_show_palette},
    {"Emit %s UI code", "Ee#e", 0, 0, &fdopt.emit_code},
    {"Emit Callback", "Cc#c", 0, 0, &fdopt.emit_cb},
    {"Emit Main", "Mm#m", 0, 0, &fdopt.emit_main},
    {"Alt Format", "Aa#a", 0, 0, &fdopt.altformat},
    {"FS Compensate ", "Ff#f", 0, 0, &fdopt.compensate}
};
#define NOPM (sizeof(opmenu)/sizeof(opmenu[0]))

void
optionmenu_callback(FL_OBJECT * ob, long data)
{
    int n = fl_get_menu(ob) - 1;
    char buf[32];

    if (n >= 0)
    {
	*(opmenu[n].p) = !*(opmenu[n].p);
	sprintf(buf, "%s%%%c", opmenu[n].entry, *(opmenu[n].p) ? 'B' : 'b');
	fl_replace_menu_item(fd_control->optionmenu, n + 1, buf);
	(fd_show_palette ? show_pallette : hide_pallette) ();
    }
}

void
reset_pallette_menu_status(void)
{
    fl_set_menu_item_mode(fd_control->optionmenu, 2, FL_PUP_BOX);
    fd_show_palette = 0;
}

static void
deactivate_control(FL_FORM * form, void *data)
{
    fl_set_object_lcol(fd_control->title, FL_SLATEBLUE);
    fl_set_object_lcol(fd_control->menubar_group, FL_INACTIVE);
    fl_set_object_lcol(fd_control->shortcut_group, FL_INACTIVE);
    fl_set_object_lcol(fd_control->fkey_group, FL_INACTIVE);
}

static void
activate_control(FL_FORM * form, void *data)
{
    fl_set_object_lcol(fd_control->title, FL_BLUE);
    fl_set_object_lcol(fd_control->menubar_group, FL_BLACK);
    fl_set_object_lcol(fd_control->shortcut_group, FL_BLACK);
    fl_set_object_lcol(fd_control->fkey_group, FL_BLACK);
}

/*
 * Initialize the control panel by filling in the menu entries
 */
void
control_init(FD_control * ui)
{
    static int control_initialized;
    MenuEntry *m, *me;
    char buf[32];
    int i;

    if (control_initialized)
	return;

    control_initialized = 1;

#ifdef __sgi
    fl_set_object_lsize(ui->filemenu, FL_SMALL_SIZE);
    fl_set_object_lsize(ui->formmenu, FL_SMALL_SIZE);
    fl_set_object_lsize(ui->groupmenu, FL_SMALL_SIZE);
    fl_set_object_lsize(ui->objectmenu, FL_SMALL_SIZE);
    fl_set_object_lsize(ui->optionmenu, FL_SMALL_SIZE);
#endif

    fl_set_form_atdeactivate(ui->control, deactivate_control, ui);
    fl_set_form_atactivate(ui->control, activate_control, ui);

    fl_set_object_dblbuffer(ui->selmsg, 1);
    fl_set_object_dblbuffer(ui->sizestatus, 1);
    fl_set_object_dblbuffer(ui->oristatus, 1);

    fl_set_object_color(ui->formbrowser, FL_COL1, FL_WHITE);

    /* file menu */
    for (m = fmenu, me = m + NFM, i = 1; m < me; m++, i++)
    {
	fl_addto_menu(ui->filemenu, m->entry);
	fl_set_menu_item_shortcut(ui->filemenu, i, m->sc);
    }

    /* form menu */
    for (m = fmmenu, me = m + NFMM, i = 1; m < me; m++, i++)
    {
	fl_addto_menu(ui->formmenu, m->entry);
	fl_set_menu_item_shortcut(ui->formmenu, i, m->sc);
    }

    /* group menu */
    for (m = gmenu, me = m + NGM, i = 1; m < me; m++, i++)
    {
	fl_addto_menu(ui->groupmenu, m->entry);
	fl_set_menu_item_shortcut(ui->groupmenu, i, m->sc);
    }

    /* object menu */
    for (m = obmenu, me = m + NOBM, i = 1; m < me; m++, i++)
    {
	fl_addto_menu(ui->objectmenu, m->entry);
	fl_set_menu_item_shortcut(ui->objectmenu, i, m->sc);
    }

    /* option menu.  all are binary items */
    for (m = opmenu, me = m + NOPM, i = 1; m < me; m++, i++)
    {
	if (strncmp(m->entry, "Emit %s", 7) == 0)
	{
	    static char tmpbuf[128];
	    sprintf(tmpbuf, m->entry, convertor[fdopt.language].lang_name);
	    m->entry = tmpbuf;
	}
	sprintf(buf, "%s%%%c", m->entry, *(m->p) ? 'B' : 'b');
	fl_addto_menu(ui->optionmenu, buf);
	fl_set_menu_item_shortcut(ui->optionmenu, i, m->sc);
    }

/*    fl_adjust_form_size(ui->objectmenu->form); */
}

/* Misc. service routines */
void
show_geometry(const char *s, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h)
{
    char buf[128];
    static FL_Coord lx = -1, ly, lw = -1, lh;

    /* canonicalize rectangle */
    if (w < 0)
    {
	x += w;
	w = -w;
    }

    if (h < 0)
    {
	y += h;
	h = -h;
    }

    if (x != lx || y != ly)
    {
	sprintf(buf, "(%d %d)", x, y);
	fl_set_object_label(fd_control->oristatus, buf);
	lx = x;
	ly = y;
    }

    if (w != lw || h != lh)
    {
	sprintf(buf, "(%d %d)", w, h);
	fl_set_object_label(fd_control->sizestatus, buf);
	lw = w;
	lh = h;
    }
}

void
show_selmessage(FL_OBJECT * sel[], int n)
{
    char objname[MAX_VAR_LEN], cbname[MAX_VAR_LEN], argname[MAX_VAR_LEN];
    char buf[MAX_VAR_LEN];

    buf[0] = '\0';
    if (n == 1)
    {
	get_object_name(sel[0], objname, cbname, argname);
	sprintf(buf, "%s Name:%s  %s%s",
		find_type_name(sel[0]->objclass, sel[0]->type),
		objname[0] ? objname : "None",
		cbname[0] ? "Callback: " : "",
		cbname[0] ? cbname : "");
    }
    else
    {
	if (sel[0]->objclass == FL_BEGIN_GROUP)
	    n -= 2;
	sprintf(buf, "%d objects", n);
    }
    fl_set_object_label(fd_control->selmsg, buf);
}


/********* callback routines for the control panel  ***************/

/*
 * form designer itself does not use resource settings, but when testing
 * the newly created forms, resources are turned on. At the moment, we
 * only allow button label size to be changed
 */

FL_FORM *thetestform;

typedef struct
{
    int x, y, w, h;
}
GEOM;

static short osize[MAXOBJS];
static GEOM oldgeom[MAXOBJS];
static FL_Coord formw, formh;	/* original form size */

static void
fix_button_label_size(FL_FORM * form, int save)
{
    FL_OBJECT *ob = form->first;
    int i;

    if (fd_buttonLabelSize == 0)
	return;

    for (i = 0; ob && i < MAXOBJS; ob = ob->next)
    {
	if (ob->objclass == FL_BUTTON)
	{
	    if (save)
	    {
		osize[i++] = ob->lsize;
		if (ob->lsize == FL_NORMAL_FONT)
		    ob->lsize = fd_buttonLabelSize;
	    }
	    else
		ob->lsize = osize[i++];
	}
    }
}


/* testing. */
void
test_cb(FL_OBJECT * obj, long arg)
{
    int i;
    FL_OBJECT *ob;
    GEOM *p = oldgeom;
    int resizeable;

    if (cur_form == NULL)
	return;

    fl_deactivate_form(fd_control->control);
    thetestform = cur_form;

    /* during test, accumulation error might result. Save the old size */
    formw = cur_form->w;
    formh = cur_form->h;
    for (i = 0, ob = cur_form->first; ob && i < MAXOBJS; ob = ob->next, i++, p++)
    {
	p->x = ob->x;
	p->y = ob->y;
	p->w = ob->w;
	p->h = ob->h;
    }

    if (i >= MAXOBJS)
	fprintf(stderr, "resizing may cause accumulation errors\n");

    /* change button label size to the one requested */
    fix_button_label_size(thetestform, 1);

    cur_form = NULL;
    redraw_the_form(1);

    fl_clear_browser(fd_test->browser);

    resizeable = (strstr(get_placement(thetestform), "FREE") != 0);

    if (resizeable)
    {

	fl_set_form_minsize(thetestform, formw >= 10 ? formw / 2 : formw,
			    formh >= 10 ? formh / 2 : formh);
	fl_set_form_maxsize(thetestform, fl_scrw, fl_scrh);
    }
    else
    {
	fl_set_form_minsize(thetestform, formw, formh);
	fl_set_form_maxsize(thetestform, formw, formh);
    }


    fl_show_form(thetestform,
		 resizeable ? FL_PLACE_CENTERFREE : FL_PLACE_CENTER,
		 FL_FULLBORDER, "Test Form");

    fl_show_form(fd_test->test, FL_PLACE_POSITION | FL_PLACE_FREE,
		 FL_FULLBORDER, "Test");
}

/* Ends a testing session */
void
stoptest_cb(FL_OBJECT * obj, long arg)
{
    int i;
    FL_OBJECT *ob;
    GEOM *p = oldgeom;

    if (thetestform == NULL)
	return;

    fl_hide_form(fd_test->test);
    fl_hide_form(thetestform);

    fix_button_label_size(thetestform, 0);

    cur_form = thetestform;
    thetestform = NULL;

    fl_set_form_size(cur_form, formw, formh);

    for (i = 0, ob = cur_form->first; ob && i < MAXOBJS; ob = ob->next, i++, p++)
    {
	ob->x = p->x;
	ob->y = p->y;
	ob->w = p->w;
	ob->h = p->h;
    }

    redraw_the_form(0);
    fl_activate_form(fd_control->control);
}

/** do alignment */
/* Shows the align form. */
void
align_cb(FL_OBJECT * obj, long arg)
{
    if (fd_align->align->visible)
	fl_hide_form(fd_align->align);
    else
	fl_show_form(fd_align->align, FL_PLACE_GEOMETRY, FL_TRANSIENT, "Snap");

}


/* Stop showing the align window */
void
exitalign_cb(FL_OBJECT * obj, long arg)
{
    fl_hide_form(fd_align->align);
}

/* Does some alignment action */
void
doalign_cb(FL_OBJECT * obj, long arg)
{
    if (fd_align->vdata)
	free_dupped_selection(fd_align->vdata);
    fd_align->vdata = dup_selection();
    align_selection(arg);
}

void
undoalign_cb(FL_OBJECT * obj, long arg)
{

    if (fd_align->vdata)
    {
	set_selection(fd_align->vdata);
	/* only allow undo once */
	free_dupped_selection(fd_align->vdata);
	fd_align->vdata = 0;
    }
}

void
snap_cb(FL_OBJECT * obj, long arg)
{
    float u = fl_get_counter_value(obj);
    set_step_size(u);
    fl_winstepunit(main_window, (int) (u + 0.1), (int) (u + 0.1));
}

/***** End of alignment stuff */

/* The user pressed one of the function keys on the main form */
void
func_cb(FL_OBJECT * obj, long arg)
{
    switch (arg)
    {
    case 1:
	change_selection();
	redraw_the_form(0);
	break;
    case 2:
	lower_selection();
	redraw_the_form(0);
	break;
    case 3:
	raise_selection();
	redraw_the_form(0);
	break;
    case 4:
	select_all();
	redraw_the_form(0);
	break;

    case 5:
	show_selection();
	redraw_the_form(0);
	break;
    case 6:
	hide_selection();
	redraw_the_form(0);
	break;
    case 7:
	group_selection();
	break;
    case 8:
	flatten_selection();
	break;
    case 9:
	copy_selection();
	break;
    case 10:
	paste_selection();
	redraw_the_form(0);
	break;
    case 11:
	next_selection();
	break;
    case 12:
	cut_selection();
	redraw_the_form(0);
	break;
    }
}


#define USE_SYMBOL

#if defined(USE_SYMBOL)

static void
draw_centering_symbol(FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		      int angle, FL_COLOR col)
{
    int delta = 4;

    if (angle == 0 || angle == 180)
    {
	fl_draw_symbol("@->", x, y, w / 2 + delta, h, col);
	fl_draw_symbol("@<-", x + w / 2 - delta, y, w / 2 + delta, h, col);
    }
    else
    {
	fl_draw_symbol("@2->", x, y + 1, w, h / 2 + delta, col);
	fl_draw_symbol("@8->", x, y + h / 2 - delta - 1, w, h / 2 + delta - 1, col);
    }
}

#if 0
static void
draw_equal_symbol(FL_Coord x, FL_Coord y, FL_Coord w, FL_Coor h,
		  int angle, FL_COLOR col)
{
    int bw = 3;

    x += bw;
    y += bw;
    w -= 2 * bw;
    h -= 2 * bw;

    if (angle == 0 || angle == 180)
    {

    }
}

#endif

#endif

#if !defined(USE_SYMBOL)
#include "left.xbm"
#include "right.xbm"
#include "hcenter.xbm"
#endif

void
init_align(void)
{
#if defined(USE_SYMBOL)
    fl_add_symbol("-><-", draw_centering_symbol, 0);
    fl_set_object_label(fd_align->hcenter, "@-><-");
    fl_set_object_label(fd_align->vcenter, "@8-><-");
#else
    FL_OBJECT *ob;
    ob = fd_align->left;
    ob->objclass = FL_BITMAPBUTTON;
    fl_set_object_color(ob, FL_COL1, FL_BLUE);
    fl_set_bitmapbutton_data(ob, left_height, left_width, left_bits);
    fl_set_object_label(ob, "");

    ob = fd_align->hcenter;
    ob->objclass = FL_BITMAPBUTTON;
    fl_set_object_color(ob, FL_COL1, FL_BLUE);
    fl_set_bitmapbutton_data(ob, hcenter_height, hcenter_width, hcenter_bits);
    fl_set_object_label(ob, "");

    ob = fd_align->right;
    ob->objclass = FL_BITMAPBUTTON;
    fl_set_object_color(ob, FL_COL1, FL_BLUE);
    fl_set_bitmapbutton_data(ob, right_height, right_width, right_bits);
    fl_set_object_label(ob, "");
#endif
    fl_set_object_helper(fd_align->left, "flush left");
    fl_set_object_helper(fd_align->hcenter, "center horizontally");
    fl_set_object_helper(fd_align->hequal, "Equal distance");
    fl_set_object_helper(fd_align->right, "flush right");
    fl_set_object_helper(fd_align->top, "flush top");
    fl_set_object_helper(fd_align->bottom, "flush bottom");
    fl_set_object_helper(fd_align->vcenter, "center vertically");
    fl_set_object_helper(fd_align->vequal, "Equal distance");
}
