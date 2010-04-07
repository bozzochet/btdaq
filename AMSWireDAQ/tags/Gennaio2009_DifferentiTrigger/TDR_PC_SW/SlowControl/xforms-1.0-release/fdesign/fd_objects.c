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
 * $Id: fd_objects.c,v 1.0 2002/05/08 05:27:39 zhao Release $
 *
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 * This file is part of the Form Designer.
 *
 * It contains routines to keep track of all different object classes
 * and the types inside each class. To add a class of object to the
 * form designer, only this file has to be changed slightly:
 *      - in init_classes() the class and types must be added.
 *      - in add_an_object() the way of adding it should be added.
 *  
 * Also to support output fully, fd_spec.c needs to be modified
 *
 */

#include <string.h>
#include <stdio.h>
#include <math.h>
#include "forms.h"
#include "fd_main.h"

#define VN(a)        a,#a
#define OBJNAME(a)   #a,fl_create_##a,fl_add_##a

typedef struct
{
    int defined;
    char tname[MAX_TYPE_NAME_LEN];
}
TDEF;

typedef FL_OBJECT *(*FL_ADDPTR) (int, FL_Coord, FL_Coord, FL_Coord,
				 FL_Coord, const char *);

typedef struct
{
    int cn;
    FL_OBJECT *defobj;		/* default          */
    TDEF types[MAXTYPE];
    char cname[MAX_CLASS_NAME_LEN];	/* add_XXX    */
    char formal_cname[MAX_CLASS_NAME_LEN];
    FL_OBJECT *defobj1;		/* to specific type */
    FL_ADDPTR createit, addit;
    int var_boxtype;		/* means default boxtype may depend on type */
    int default_type;
    const char *default_label;
}
CDEF;

static CDEF classes[MAXCLASS];
static int cnumb = 0;

typedef FL_OBJECT *(*FL_ADDFREEPTR) (int, FL_Coord, FL_Coord, FL_Coord,
				     FL_Coord, const char *, FL_HANDLEPTR);

/* Adds a class definition */
static void
add_class_def(int numb, char *formal_name, char *name,
	      FL_ADDPTR createit, FL_ADDPTR addit)
{
    int i;

    if (cnumb == MAXCLASS)
    {
	fprintf(stderr, "exceeding maxclass allowed\n");
	return;
    }

    classes[cnumb].cn = numb;
    strcpy(classes[cnumb].cname, name);
    strcpy(classes[cnumb].formal_cname, formal_name);
    if (createit)
	classes[cnumb].defobj = createit(1, 0, 0, 1, 1, "");
    classes[cnumb].createit = createit;
    classes[cnumb].addit = addit;

    for (i = 0; i < MAXTYPE; i++)
	classes[cnumb].types[i].defined = FALSE;
    cnumb++;
}

static CDEF *
find_class_struct(int n)
{
    int i;
    for (i = 0; i < cnumb; i++)
    {
	if (classes[i].cn == n)
	    return classes + i;
    }
    M_err("FindClassStruct", "Can't find class %d", n);
    return 0;
}


/* Adds a type definition to a class */
static void
add_type_def(int cn, int tn, char name[])
{
    int i;

    if (tn >= MAXTYPE || tn < 0)
    {
	M_err("AddTypeDef", "Bad type: %%d", tn);
	return;
    }

    for (i = 0; i < cnumb; i++)
    {
	if (classes[i].cn == cn)
	{
	    classes[i].types[tn].defined = TRUE;
	    strcpy(classes[i].types[tn].tname, name);
	}
    }
}

/* sets a default type and label for class cn */
static void
set_class_default(int cn, int def_type, char *label)
{
    int i;
    for (i = 0; i < cnumb; i++)
    {
	if (classes[i].cn == cn)
	{
	    classes[i].default_type = def_type;
	    classes[i].default_label = label;	/* note: no copy */
	}
    }
}

static void
set_var_boxtype(int cn, int yes)
{
    int i;

    for (i = 0; i < cnumb; i++)
	if (classes[i].cn == cn)
	    classes[i].var_boxtype = 1;
}

static FL_OBJECT *
fd_create_free(int type, FL_Coord x, FL_Coord y, FL_Coord w,
	       FL_Coord h, const char *l)
{
    return fl_create_free(type, x, y, w, h, l, noop_handle);
}

static FL_OBJECT *
fd_add_free(int type, FL_Coord x, FL_Coord y, FL_Coord w,
	    FL_Coord h, const char *l)
{
    return fl_add_free(type, x, y, w, h, l, noop_handle);
}


/****
  CALLBACK ROUTINES
****/

/* handles a class change event */
void
object_cb(FL_OBJECT * obj, long arg)
{
    if (fl_get_browser(obj))
	cur_class = classes[fl_get_browser(obj) - 1].cn;
    else
	cur_class = -1;

    select_pallette_entry(cur_class);
}

/****
  INITIALIZING THE CLASSES
****/

int cur_class = -1;		/* The current class used */


static void
add_button_types(int button_class)
{
    add_type_def(button_class, FL_NORMAL_BUTTON, "NORMAL_BUTTON");
    add_type_def(button_class, FL_PUSH_BUTTON, "PUSH_BUTTON");
    add_type_def(button_class, FL_RADIO_BUTTON, "RADIO_BUTTON");
    add_type_def(button_class, FL_TOUCH_BUTTON, "TOUCH_BUTTON");
    add_type_def(button_class, FL_HIDDEN_BUTTON, "HIDDEN_BUTTON");
    add_type_def(button_class, FL_INOUT_BUTTON, "INOUT_BUTTON");
    add_type_def(button_class, FL_MENU_BUTTON, "MENU_BUTTON");
    add_type_def(button_class, FL_RETURN_BUTTON, "RETURN_BUTTON");
    add_type_def(button_class, FL_HIDDEN_RET_BUTTON, "HIDDEN_RET_BUTTON");
}

static void
add_frame_types(int frame_class)
{
    add_type_def(frame_class, FL_NO_FRAME, "NO_FRAME");
    add_type_def(frame_class, FL_UP_FRAME, "UP_FRAME");
    add_type_def(frame_class, FL_DOWN_FRAME, "DOWN_FRAME");
    add_type_def(frame_class, FL_BORDER_FRAME, "BORDER_FRAME");
    add_type_def(frame_class, FL_ENGRAVED_FRAME, "ENGRAVED_FRAME");
    add_type_def(frame_class, FL_ROUNDED_FRAME, "ROUNDED_FRAME");
    add_type_def(frame_class, FL_EMBOSSED_FRAME, "EMBOSSED_FRAME");
    add_type_def(frame_class, FL_SHADOW_FRAME, "SHADOW_FRAME");
    add_type_def(frame_class, FL_OVAL_FRAME, "OVAL_FRAME");
}

static void
add_slider_types(int sclass)
{
    add_type_def(sclass, FL_VERT_SLIDER, "VERT_SLIDER");
    add_type_def(sclass, FL_HOR_SLIDER, "HOR_SLIDER");
    add_type_def(sclass, FL_VERT_FILL_SLIDER, "VERT_FILL_SLIDER");
    add_type_def(sclass, FL_HOR_FILL_SLIDER, "HOR_FILL_SLIDER");
    add_type_def(sclass, FL_VERT_NICE_SLIDER, "VERT_NICE_SLIDER");
    add_type_def(sclass, FL_HOR_NICE_SLIDER, "HOR_NICE_SLIDER");
    add_type_def(sclass, FL_VERT_BROWSER_SLIDER, "VERT_BROWSER_SLIDER");
    add_type_def(sclass, FL_HOR_BROWSER_SLIDER, "HOR_BROWSER_SLIDER");
}


/* Initializes all the classes and types. Must be adapted
   to add new classes and types.  */
void
init_classes(void)
{
    static int initialized;
    VN_pair *vp;

    if (initialized)
	return;

    initialized = 1;

    fl_set_browser_fontstyle(fd_control->objectbrowser, FL_TIMES_STYLE);

    fl_add_browser_line(fd_control->objectbrowser, "box");
    add_class_def(VN(FL_BOX), OBJNAME(box));
    for (vp = vn_btype; vp->val >= 0; vp++)
	add_type_def(FL_BOX, vp->val, vp->name + 3);
    set_class_default(FL_BOX, FL_UP_BOX, "");

    fl_add_browser_line(fd_control->objectbrowser, "frame");
    add_class_def(VN(FL_FRAME), OBJNAME(frame));
    add_frame_types(FL_FRAME);
    set_class_default(FL_FRAME, FL_ENGRAVED_FRAME, "");

    fl_add_browser_line(fd_control->objectbrowser, "labelframe");
    add_class_def(VN(FL_LABELFRAME), OBJNAME(labelframe));
    add_frame_types(FL_LABELFRAME);
    set_class_default(FL_LABELFRAME, FL_ENGRAVED_FRAME, "frame");

    fl_add_browser_line(fd_control->objectbrowser, "text");
    add_class_def(VN(FL_TEXT), OBJNAME(text));
    add_type_def(FL_TEXT, FL_NORMAL_TEXT, "NORMAL_TEXT");
    set_class_default(FL_TEXT, FL_NORMAL_TEXT, "text");

    fl_add_browser_line(fd_control->objectbrowser, "bitmap");
    add_class_def(VN(FL_BITMAP), OBJNAME(bitmap));
    add_type_def(FL_BITMAP, FL_NORMAL_BITMAP, "NORMAL_BITMAP");

    fl_add_browser_line(fd_control->objectbrowser, "pixmap");
    add_class_def(VN(FL_PIXMAP), OBJNAME(pixmap));
    add_type_def(FL_PIXMAP, FL_NORMAL_PIXMAP, "NORMAL_PIXMAP");

    fl_add_browser_line(fd_control->objectbrowser, "chart");
    add_class_def(VN(FL_CHART), OBJNAME(chart));
    add_type_def(FL_CHART, FL_BAR_CHART, "BAR_CHART");
    add_type_def(FL_CHART, FL_HORBAR_CHART, "HORBAR_CHART");
    add_type_def(FL_CHART, FL_LINE_CHART, "LINE_CHART");
    add_type_def(FL_CHART, FL_FILLED_CHART, "FILLED_CHART");
    add_type_def(FL_CHART, FL_SPIKE_CHART, "SPIKE_CHART");
    add_type_def(FL_CHART, FL_PIE_CHART, "PIE_CHART");
    add_type_def(FL_CHART, FL_SPECIALPIE_CHART, "SPECIALPIE_CHART");

    fl_add_browser_line(fd_control->objectbrowser, "@_clock");
    add_class_def(VN(FL_CLOCK), OBJNAME(clock));
    add_type_def(FL_CLOCK, FL_ANALOG_CLOCK, "ANALOG_CLOCK");
    add_type_def(FL_CLOCK, FL_DIGITAL_CLOCK, "DIGITAL_CLOCK");
    set_class_default(FL_CLOCK, FL_ANALOG_CLOCK, "");

    fl_add_browser_line(fd_control->objectbrowser, "button");
    add_class_def(VN(FL_BUTTON), OBJNAME(button));
    add_button_types(FL_BUTTON);
    set_class_default(FL_BUTTON, FL_NORMAL_BUTTON, "button");

    fl_add_browser_line(fd_control->objectbrowser, "roundbutton");
    add_class_def(VN(FL_ROUNDBUTTON), OBJNAME(roundbutton));
    add_button_types(FL_ROUNDBUTTON);
    set_class_default(FL_ROUNDBUTTON, FL_PUSH_BUTTON, "button");

    fl_add_browser_line(fd_control->objectbrowser, "round3dbutton");
    add_class_def(VN(FL_ROUND3DBUTTON), OBJNAME(round3dbutton));
    add_button_types(FL_ROUND3DBUTTON);
    set_class_default(FL_ROUND3DBUTTON, FL_PUSH_BUTTON, "button");

    fl_add_browser_line(fd_control->objectbrowser, "checkbutton");
    add_class_def(VN(FL_CHECKBUTTON), OBJNAME(checkbutton));
    add_button_types(FL_CHECKBUTTON);
    set_class_default(FL_CHECKBUTTON, FL_PUSH_BUTTON, "button");

    fl_add_browser_line(fd_control->objectbrowser, "lightbutton");
    add_class_def(VN(FL_LIGHTBUTTON), OBJNAME(lightbutton));
    add_button_types(FL_LIGHTBUTTON);
    set_class_default(FL_LIGHTBUTTON, FL_PUSH_BUTTON, "button");

    fl_add_browser_line(fd_control->objectbrowser, "scrollbutton");
    add_class_def(VN(FL_SCROLLBUTTON), OBJNAME(scrollbutton));
    add_button_types(FL_SCROLLBUTTON);
    set_class_default(FL_SCROLLBUTTON, FL_TOUCH_BUTTON, "8");

    fl_add_browser_line(fd_control->objectbrowser, "bitmapbutton");
    add_class_def(VN(FL_BITMAPBUTTON), OBJNAME(bitmapbutton));
    add_button_types(FL_BITMAPBUTTON);

    fl_add_browser_line(fd_control->objectbrowser, "pixmapbutton");
    add_class_def(VN(FL_PIXMAPBUTTON), OBJNAME(pixmapbutton));
    add_button_types(FL_PIXMAPBUTTON);

    fl_add_browser_line(fd_control->objectbrowser, "@_labelbutton");
    add_class_def(VN(FL_LABELBUTTON), OBJNAME(labelbutton));
    add_button_types(FL_LABELBUTTON);

    fl_add_browser_line(fd_control->objectbrowser, "slider");
    add_class_def(VN(FL_SLIDER), OBJNAME(slider));
    add_slider_types(FL_SLIDER);

    fl_add_browser_line(fd_control->objectbrowser, "valslider");
    add_class_def(VN(FL_VALSLIDER), OBJNAME(valslider));
    add_slider_types(FL_VALSLIDER);

    fl_add_browser_line(fd_control->objectbrowser, "scrollbar");
    add_class_def(VN(FL_SCROLLBAR), OBJNAME(scrollbar));
    add_type_def(FL_SCROLLBAR, FL_VERT_SCROLLBAR, "VERT_SCROLLBAR");
    add_type_def(FL_SCROLLBAR, FL_HOR_SCROLLBAR, "HOR_SCROLLBAR");
    add_type_def(FL_SCROLLBAR, FL_VERT_THIN_SCROLLBAR, "VERT_THIN_SCROLLBAR");
    add_type_def(FL_SCROLLBAR, FL_HOR_THIN_SCROLLBAR, "HOR_THIN_SCROLLBAR");
    add_type_def(FL_SCROLLBAR, FL_VERT_NICE_SCROLLBAR, "VERT_NICE_SCROLLBAR");
    add_type_def(FL_SCROLLBAR, FL_HOR_NICE_SCROLLBAR, "HOR_NICE_SCROLLBAR");
    add_type_def(FL_SCROLLBAR, FL_VERT_PLAIN_SCROLLBAR, "VERT_PLAIN_SCROLLBAR");
    add_type_def(FL_SCROLLBAR, FL_HOR_PLAIN_SCROLLBAR, "HOR_PLAIN_SCROLLBAR");


    fl_add_browser_line(fd_control->objectbrowser, "dial");
    add_class_def(VN(FL_DIAL), OBJNAME(dial));
    add_type_def(FL_DIAL, FL_NORMAL_DIAL, "NORMAL_DIAL");
    add_type_def(FL_DIAL, FL_LINE_DIAL, "LINE_DIAL");
    add_type_def(FL_DIAL, FL_FILL_DIAL, "FILL_DIAL");
    set_var_boxtype(FL_DIAL, 1);

    fl_add_browser_line(fd_control->objectbrowser, "positioner");
    add_class_def(VN(FL_POSITIONER), OBJNAME(positioner));
    add_type_def(FL_POSITIONER, FL_NORMAL_POSITIONER, "NORMAL_POSITIONER");
    add_type_def(FL_POSITIONER, FL_OVERLAY_POSITIONER, "OVERLAY_POSITIONER");
    set_var_boxtype(FL_POSITIONER, 1);

    fl_add_browser_line(fd_control->objectbrowser, "thumbwheel");
    add_class_def(VN(FL_THUMBWHEEL), OBJNAME(thumbwheel));
    add_type_def(FL_THUMBWHEEL, FL_HOR_THUMBWHEEL, "HOR_THUMBWHEEL");
    add_type_def(FL_THUMBWHEEL, FL_VERT_THUMBWHEEL, "VERT_THUMBWHEEL");

    fl_add_browser_line(fd_control->objectbrowser, "@_counter");
    add_class_def(VN(FL_COUNTER), OBJNAME(counter));
    add_type_def(FL_COUNTER, FL_NORMAL_COUNTER, "NORMAL_COUNTER");
    add_type_def(FL_COUNTER, FL_SIMPLE_COUNTER, "SIMPLE_COUNTER");

    fl_add_browser_line(fd_control->objectbrowser, "@_input");
    add_class_def(VN(FL_INPUT), OBJNAME(input));
    add_type_def(FL_INPUT, FL_NORMAL_INPUT, "NORMAL_INPUT");
    add_type_def(FL_INPUT, FL_FLOAT_INPUT, "FLOAT_INPUT");
    add_type_def(FL_INPUT, FL_INT_INPUT, "INT_INPUT");
    add_type_def(FL_INPUT, FL_HIDDEN_INPUT, "HIDDEN_INPUT");
    add_type_def(FL_INPUT, FL_MULTILINE_INPUT, "MULTILINE_INPUT");
    add_type_def(FL_INPUT, FL_SECRET_INPUT, "SECRET_INPUT");
    add_type_def(FL_INPUT, FL_DATE_INPUT, "DATE_INPUT");

    fl_add_browser_line(fd_control->objectbrowser, "menu");
    add_class_def(VN(FL_MENU), OBJNAME(menu));
    add_type_def(FL_MENU, FL_PUSH_MENU, "PUSH_MENU");
    add_type_def(FL_MENU, FL_PULLDOWN_MENU, "PULLDOWN_MENU");
    add_type_def(FL_MENU, FL_TOUCH_MENU, "TOUCH_MENU");
    set_var_boxtype(FL_MENU, 1);

    fl_add_browser_line(fd_control->objectbrowser, "choice");
    add_class_def(VN(FL_CHOICE), OBJNAME(choice));
    add_type_def(FL_CHOICE, FL_NORMAL_CHOICE, "NORMAL_CHOICE");
    add_type_def(FL_CHOICE, FL_NORMAL_CHOICE2, "NORMAL_CHOICE2");
    add_type_def(FL_CHOICE, FL_DROPLIST_CHOICE, "DROPLIST_CHOICE");
    set_var_boxtype(FL_CHOICE, 1);

    fl_add_browser_line(fd_control->objectbrowser, "@_browser");
    add_class_def(VN(FL_BROWSER), OBJNAME(browser));
    add_type_def(FL_BROWSER, FL_NORMAL_BROWSER, "NORMAL_BROWSER");
    add_type_def(FL_BROWSER, FL_SELECT_BROWSER, "SELECT_BROWSER");
    add_type_def(FL_BROWSER, FL_HOLD_BROWSER, "HOLD_BROWSER");
    add_type_def(FL_BROWSER, FL_MULTI_BROWSER, "MULTI_BROWSER");


    fl_add_browser_line(fd_control->objectbrowser, "@_timer");
    add_class_def(VN(FL_TIMER), OBJNAME(timer));
    add_type_def(FL_TIMER, FL_NORMAL_TIMER, "NORMAL_TIMER");
    add_type_def(FL_TIMER, FL_VALUE_TIMER, "VALUE_TIMER");
    add_type_def(FL_TIMER, FL_HIDDEN_TIMER, "HIDDEN_TIMER");
    set_class_default(FL_TIMER, FL_NORMAL_TIMER, "timer");

    fl_add_browser_line(fd_control->objectbrowser, "@_xyplot");
    add_class_def(VN(FL_XYPLOT), OBJNAME(xyplot));
    add_type_def(FL_XYPLOT, FL_NORMAL_XYPLOT, "NORMAL_XYPLOT");
    add_type_def(FL_XYPLOT, FL_ACTIVE_XYPLOT, "ACTIVE_XYPLOT");
    add_type_def(FL_XYPLOT, FL_SQUARE_XYPLOT, "SQUARE_XYPLOT");
    add_type_def(FL_XYPLOT, FL_CIRCLE_XYPLOT, "CIRCLE_XYPLOT");
    add_type_def(FL_XYPLOT, FL_POINTS_XYPLOT, "POINTS_XYPLOT");
    add_type_def(FL_XYPLOT, FL_LINEPOINTS_XYPLOT, "LINEPOINTS_XYPLOT");
    add_type_def(FL_XYPLOT, FL_DASHED_XYPLOT, "DASHED_XYPLOT");
    add_type_def(FL_XYPLOT, FL_DOTTED_XYPLOT, "DOTTED_XYPLOT");
    add_type_def(FL_XYPLOT, FL_DOTDASHED_XYPLOT, "DOTDASHED_XYPLOT");
    add_type_def(FL_XYPLOT, FL_LONGDASHED_XYPLOT, "LONGDASHED_XYPLOT");
    add_type_def(FL_XYPLOT, FL_FILL_XYPLOT, "FILL_XYPLOT");
    add_type_def(FL_XYPLOT, FL_IMPULSE_XYPLOT, "IMPULSE_XYPLOT");
    add_type_def(FL_XYPLOT, FL_EMPTY_XYPLOT, "EMPTY_XYPLOT");


    fl_add_browser_line(fd_control->objectbrowser, "canvas");
    add_class_def(VN(FL_CANVAS), "canvas",
		  fl_create_simu_canvas, fl_add_simu_canvas);
    add_type_def(FL_CANVAS, FL_NORMAL_CANVAS, "NORMAL_CANVAS");
    add_type_def(FL_CANVAS, FL_SCROLLED_CANVAS, "SCROLLED_CANVAS");
    set_class_default(FL_CANVAS, FL_NORMAL_CANVAS, "");

    fl_add_browser_line(fd_control->objectbrowser, "@_glcanvas");
    add_class_def(VN(FL_GLCANVAS), "glcanvas",
		  fl_create_simu_glcanvas, fl_add_simu_glcanvas);
    add_type_def(FL_GLCANVAS, FL_NORMAL_CANVAS, "NORMAL_CANVAS");
    add_type_def(FL_GLCANVAS, FL_SCROLLED_CANVAS, "SCROLLED_CANVAS");
    set_class_default(FL_GLCANVAS, FL_NORMAL_CANVAS, "");

    fl_add_browser_line(fd_control->objectbrowser, "tabfolder");
    add_class_def(FL_NTABFOLDER, "FL_TABFOLDER", "tabfolder",
		  fl_create_ntabfolder, fl_add_ntabfolder);
    add_type_def(FL_NTABFOLDER, FL_TOP_TABFOLDER, "TOP_TABFOLDER");
    add_type_def(FL_NTABFOLDER, FL_BOTTOM_TABFOLDER, "BOTTOM_TABFOLDER");
    set_class_default(FL_NTABFOLDER, FL_TOP_TABFOLDER, "");

    fl_add_browser_line(fd_control->objectbrowser, "@_formbrowser");
    add_class_def(FL_FORMBROWSER, "FL_FORMBROWSER", "formbrowser",
		  fl_create_nformbrowser, fl_add_nformbrowser);
    add_type_def(FL_FORMBROWSER, FL_NORMAL_FORMBROWSER, "NORMAL_FORMBROWSER");


    fl_add_browser_line(fd_control->objectbrowser, "freeobject");
    add_class_def(VN(FL_FREE), "free", fd_create_free, fd_add_free);
    add_type_def(FL_FREE, FL_NORMAL_FREE, "NORMAL_FREE");
    add_type_def(FL_FREE, FL_INACTIVE_FREE, "INACTIVE_FREE");
    add_type_def(FL_FREE, FL_INPUT_FREE, "INPUT_FREE");
    add_type_def(FL_FREE, FL_CONTINUOUS_FREE, "CONTINUOUS_FREE");
    add_type_def(FL_FREE, FL_ALL_FREE, "ALL_FREE");

    /* ADD NEW CLASSES HERE */



    /* group must be last. palette.c need this */
    add_class_def(VN(FL_BEGIN_GROUP), "group", 0, 0);
    add_type_def(FL_BEGIN_GROUP, 0, "0");

    add_class_def(VN(FL_END_GROUP), "endgroup", 0, 0);
    add_type_def(FL_END_GROUP, 0, "0");
}

void
select_object_by_class(int cn)
{
    int i;

    for (i = 0; i < cnumb; i++)
	if (classes[i].cn == cn)
	{
	    fl_select_browser_line(fd_control->objectbrowser, i + 1);
	    fl_show_browser_line(fd_control->objectbrowser, i + 1);
	    cur_class = cn;
	    break;
	}
}


/****
  GETTING INFORMATION ABOUT CLASSES AND TYPES
****/

/* Returns a pointer to the (short) name of the class */
char *
find_class_name(int cln)
{
    int i;
    static char buf[MAX_CLASS_NAME_LEN];
    for (i = 0; i < cnumb; i++)
	if (classes[i].cn == cln)
	    return classes[i].cname;
    sprintf(buf, "%d", cln);
    return buf;
}

/* return a pointer to the formal name of the class */
char *
class_name(int cls)
{
    int i;
    static char buf[MAX_CLASS_NAME_LEN];
    for (i = 0; i < cnumb; i++)
	if (classes[i].cn == cls)
	    return classes[i].formal_cname;
    sprintf(buf, "%d", cls);
    return buf;
}

/* return the class value */
int
class_val(const char *name)
{
    int i;

    for (i = 0; i < cnumb; i++)
    {
	if (strcmp(classes[i].cname, name) == 0 ||
	    strcmp(classes[i].formal_cname, name) == 0)
	    return classes[i].cn;
    }

    return atoi(name);
}

/* Returns a pointer to the default object of the class. var_boxtype
 * means different type may have different boxtypes
 */

FL_OBJECT *
find_class_default(int cln, int t)
{
    int i;
    CDEF *c = classes;

    for (i = 0; i < cnumb; i++, c++)
	if (c->cn == cln)
	{
	    if (c->var_boxtype)
	    {
		if (c->defobj1)
		    fl_free_object(c->defobj1);
		return c->defobj1 = c->createit(t, 0, 0, 1, 1, "");
	    }
	    else
		return c->defobj;
	}
    return NULL;
}

/* Returns the number of types in the class */
int
find_class_maxtype(int cln)
{
    int i, j, n = 0;
    for (i = 0; i < cnumb; i++)
	if (classes[i].cn == cln)
	    for (j = 0; j < MAXTYPE; j++)
		if (classes[i].types[j].defined)
		    n++;
    return n;
}

/* Returns a pointer to the name of the type in the class */
const char *
find_type_name(int cln, int tyn)
{
    int i;
    static char buf[MAX_TYPE_NAME_LEN];

    for (i = 0; i < cnumb; i++)
	if (classes[i].cn == cln)
	    return classes[i].types[tyn].tname;
    sprintf(buf, "%d", tyn);
    return buf;
}

/* returns a integer indicating the value of a type */
int
find_type_value(int cln, const char *tyname)
{
    int i, jmax = find_class_maxtype(cln), j;
    for (i = 0; i < cnumb; i++)
    {
	if (classes[i].cn == cln)
	{
	    for (j = 0; j < jmax; j++)
	    {
		if (strcmp(classes[i].types[j].tname, tyname) == 0)
		    return j;
	    }
	}
    }
    M_err("TypeValue", "type %s is unknown", tyname);
    return atoi(tyname);
}

/****
  ADDING OBJECTS
****/

#define bm1_width 16
#define bm1_height 16
static unsigned char bm1_bits[] =
{
    0x00, 0x00, 0x00, 0x57, 0x7c, 0x72, 0xc4, 0x52, 0xc4, 0x00, 0x44, 0x01,
    0x44, 0x1f, 0xfc, 0x22, 0x40, 0x42, 0x40, 0x44, 0x40, 0x43, 0xc0, 0x40,
    0x70, 0x40, 0x8c, 0x20, 0x00, 0x1f, 0x00, 0x00
};


#define pm_width 28
#define pm_height 28
#include "crab.xpm"


void
set_testing_pixmap(FL_OBJECT * ob)
{
    fl_set_pixmap_data(ob, crab);
}


/* adds an object to the current form  type = -1 means default type */
FL_OBJECT *
add_an_object(int objclass, int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h)
{
    FL_OBJECT *obj = 0;
    int nosuper = 0, i;
    char buf[128];
    CDEF *cls;

    if (cur_form == NULL)
	return NULL;

    /* this routine is called with X coordinate system, need to translate to
       up-right coordinate system */

    if (!(cls = find_class_struct(objclass)))
    {
	fprintf(stderr, "unknown class %d\n", objclass);
	return 0;
    }

    fl_addto_form(cur_form);

    switch (objclass)
    {
    case FL_BEGIN_GROUP:
	obj = fl_bgn_group();
	break;
    case FL_END_GROUP:
	obj = fl_end_group();
	break;
    case FL_BITMAP:
	if (type == -1)
	    type = FL_NORMAL_BITMAP;
	obj = cls->addit(type, x, y, w, h, "");
	if (!fdopt.conv_only)
	    fl_set_bitmap_data(obj, bm1_width, bm1_height, bm1_bits);
	break;
    case FL_PIXMAP:
	if (type == -1)
	    type = FL_NORMAL_PIXMAP;
	obj = cls->addit(type, x, y, w, h, "");
	if (!fdopt.conv_only)
	    fl_set_pixmap_data(obj, crab);
	break;
    case FL_CHART:
	if (type == -1)
	    type = FL_BAR_CHART;
	obj = cls->addit(type, x, y, w, h, "");
	fl_add_chart_value(obj, 4.0, "item 1", 2);
	fl_add_chart_value(obj, 8.0, "item 2", 3);
	fl_add_chart_value(obj, 2.0, "item 3", 4);
	fl_add_chart_value(obj, 5.0, "item 4", 5);
	break;
    case FL_BITMAPBUTTON:
	if (type == -1)
	    type = FL_NORMAL_BUTTON;
	obj = cls->addit(type, x, y, w, h, "");
	if (!fdopt.conv_only)
	    fl_set_bitmapbutton_data(obj, bm1_width, bm1_height, bm1_bits);
	break;
    case FL_PIXMAPBUTTON:
	if (type == -1)
	    type = FL_NORMAL_BUTTON;
	obj = cls->addit(type, x, y, w, h, "");
	if (!fdopt.conv_only)
	    fl_set_pixmapbutton_data(obj, crab);
	break;
    case FL_SLIDER:
	if (type == -1)
	    type = (h > w) ? FL_VERT_BROWSER_SLIDER : FL_HOR_BROWSER_SLIDER;
	obj = cls->addit(type, x, y, w, h, "");
	break;
    case FL_VALSLIDER:
	if (type == -1)
	    type = (h > w) ? FL_VERT_BROWSER_SLIDER : FL_HOR_BROWSER_SLIDER;
	obj = cls->addit(type, x, y, w, h, "");
	break;
    case FL_SCROLLBAR:
	if (type == -1)
	    type = (h > w) ? FL_VERT_SCROLLBAR : FL_HOR_SCROLLBAR;
	obj = cls->addit(type, x, y, w, h, "");
	break;
    case FL_THUMBWHEEL:
	if (type == -1)
	    type = (h > w) ? FL_VERT_THUMBWHEEL : FL_HOR_THUMBWHEEL;
	obj = cls->addit(type, x, y, w, h, "");
	break;
    case FL_INPUT:
	if (type == -1)
	    type = h < 60 ? FL_NORMAL_INPUT : FL_MULTILINE_INPUT;
	obj = cls->addit(type, x, y, w, h, "Input");
	fl_set_input_hscrollbar(obj, FL_OFF);
	fl_set_input_vscrollbar(obj, FL_OFF);
	break;
    case FL_MENU:
	if (type == -1)
	    type = FL_PULLDOWN_MENU;
	obj = cls->addit(type, x, y, w, h, "Menu");
	spec_to_superspec(obj);
	nosuper = 1;
	fl_addto_menu(obj, "menuitem 1");
	fl_addto_menu(obj, "menuitem 2");
	fl_addto_menu(obj, "menuitem 3");
	fl_addto_menu(obj, "menuitem 4");
	break;
    case FL_CHOICE:
	if (type == -1)
	    type = FL_NORMAL_CHOICE2;
	obj = cls->addit(type, x, y, w, h, "");
	spec_to_superspec(obj);
	nosuper = 1;

	fl_addto_choice(obj, "choice 1");
	fl_addto_choice(obj, "choice 2");
	fl_addto_choice(obj, "choice 3");
	fl_addto_choice(obj, "choice 4");
	break;
    case FL_BROWSER:
	if (type == -1)
	    type = FL_NORMAL_BROWSER;
	obj = cls->addit(type, x, y, w, h, "");
	spec_to_superspec(obj);
	nosuper = 1;

	for (i = 0; i < 30; i++)
	{
	    sprintf(buf, "Browser line %d", i + 1);
	    fl_add_browser_line(obj, buf);
	}
	break;
    case FL_XYPLOT:
	if (type == -1)
	    type = FL_NORMAL_XYPLOT;
	{
	    float xx[30], yy[30];
	    int ii;

	    for (ii = 0; ii < 30; ii++)
	    {
		xx[ii] = 3.1415 * (ii + 0.5) / 8.0;
		yy[ii] = sin(2 * xx[ii]) + cos(xx[ii]);
		yy[ii] = FL_abs(yy[ii]);
	    }

	    obj = cls->addit(type, x, y, w, h, "");
/*          fl_set_xyplot_data(obj, xx, yy, 30, "XyPlot", "X-axis", "Y"); */
	    fl_set_xyplot_data(obj, xx, yy, 30, "", "", "");
	}
	break;

    case FL_FREE:
	if (type == -1)
	    type = FL_NORMAL_FREE;
	obj = fl_add_free(type, x, y, w, h, "", noop_handle);
	break;

    default:
	if (type == -1)
	    type = cls->default_type;
	obj = cls->addit(type, x, y, w, h, cls->default_label);
	break;
    }

    if (!obj)
	fprintf(stderr, "Unknown object(class=%d type=%d)\n", objclass, type);

    fl_end_form();

    if (!nosuper)
	get_superspec(obj);

    obj->active = TRUE;

    return obj;
}
