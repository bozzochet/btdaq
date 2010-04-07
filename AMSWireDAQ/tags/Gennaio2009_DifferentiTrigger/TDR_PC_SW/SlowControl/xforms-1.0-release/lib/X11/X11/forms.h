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
 *  forms.h   $Revision: 1.0 $ $State: Release $
 *            $Date: 2002/05/08 05:17:18 $ $Author: zhao $
 *
 *. All XForms files as distributed in this package are
 *  Copyright(c) 1996-2002 by T.C. Zhao and Mark Overmars,
 *  with part of the code Copyright (c) 1999-2002 by T.C. Zhao
 *  and Steve Lamont.
 *  ALL RIGHTS RESERVED.
 *
 * Permission to use, copy, and distribute this software in its entirety
 * without fee, is hereby granted, provided that the above copyright 
 * notice and this permission notice appear in all copies and their 
 * documentation.
 *
 * As of April 2002, xforms is released under the GNU LGPL license.
 * You can use xforms for any purpose that's compatible with
 * LGPL with the restriction that you will need a special license
 * for distributed  binary commercial software that requires or is
 * based on xforms or its derivative.
 *
 * This software is provided "as is" without expressed or implied
 * warranty of any kind.
 *
 * Contact tc_zhao@yahoo.com or xforms@world.std.com or visit
 * http://world.std.com/~xforms 
 * if you have questions about xforms.
 *
 * ******** This file is generated automatically. DO NOT CHANGE *********
 */

#ifndef FL_FORMS_H
#define FL_FORMS_H

#define FL_VERSION             1
#define FL_REVISION            0
#define FL_FIXLEVEL            0
#define FL_INCLUDE_VERSION    (FL_VERSION * 1000 + FL_REVISION)

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>

#if defined(__cplusplus)
extern "C"
{
#endif

#if defined(_WIN32)
#define FL_WIN32
#include <windows.h>
#endif

#if !defined(FL_WIN32) || !defined(SHARED_LIB)
#define FL_EXPORT extern
#else
#ifdef MAKING_FORMS
#define FL_EXPORT __declspec( dllexport ) extern
#else
#define FL_EXPORT __declspec( dllimport ) extern
#endif				/* MAKING_FORMS */
#endif				/* FL_WIN32 */

/*
 * $Id: Basic.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *  Basic definitions and limits.
 *  Window system independent prototypes
 *
 *  Modify with care
 *
 */

#ifndef FL_BASIC_H
#define FL_BASIC_H

/* some general constants */
enum
{
    FL_ON      = 1,
    FL_OK      = 1,
    FL_VALID   = 1,
    FL_PREEMPT = 1,
    FL_AUTO    = 2,
    FL_WHEN_NEEDED = FL_AUTO,

    FL_OFF     = 0,
    FL_CANCEL  = 0,
    FL_INVALID = 0,

    /* FL_NONE = 0,  defined elsewhere */

    /* WM_DELETE_WINDOW callback return */
    FL_IGNORE = -1,
    FL_CLOSE  = -2,

   /* misc. return types */
    FL_ARGUMENT   = -3,
    FL_ALLOC      = -4,
    FL_BAD_OBJECT = -5
};

/* Max  directory length  */

#ifndef FL_PATH_MAX
#ifndef PATH_MAX
#define FL_PATH_MAX       1024
#else
#define FL_PATH_MAX       PATH_MAX
#endif
#endif /* !def FL_PATH_MAX */

/*
 * The screen coordinate unit, FL_Coord, must be of signed type.
 * If FL_Coord is float, FL_CoordIsFloat must be defined to be 1 so that
 * round-off error can be checked. **TODO Float not tested ***
 */

typedef int FL_Coord;
#define FL_COORD        FL_Coord
#define FL_CoordIsFloat 0	/* make it 1 if FL_Coord is of type float */

typedef unsigned long FL_COLOR;

/*
 * Coordinates can be in pixels, milli-meters or points (1/72inch)
 */
typedef enum
{
    FL_COORD_PIXEL,		/* default, Pixel           */
    FL_COORD_MM,		/* milli-meter              */
    FL_COORD_POINT,		/* point                    */
    FL_COORD_centiMM,		/* one hundredth of a mm    */
    FL_COORD_centiPOINT		/* one hundredth of a point */
}
FL_COORD_UNIT;

/*
 * All object classes.
 */

typedef enum
{
    FL_INVALID_CLASS,
    FL_BUTTON, FL_LIGHTBUTTON,
    FL_ROUNDBUTTON, FL_ROUND3DBUTTON,
    FL_CHECKBUTTON, FL_BITMAPBUTTON, FL_PIXMAPBUTTON,
    FL_BITMAP, FL_PIXMAP,
    FL_BOX, FL_TEXT,
    FL_MENU, FL_CHART, FL_CHOICE,
    FL_COUNTER, FL_SLIDER, FL_VALSLIDER, FL_INPUT,
    FL_BROWSER,
    FL_DIAL,
    FL_TIMER,
    FL_CLOCK,
    FL_POSITIONER,
    FL_FREE,
    FL_XYPLOT,
    FL_FRAME,
    FL_LABELFRAME,
    FL_CANVAS,
    FL_GLCANVAS,
    FL_TABFOLDER,
    FL_SCROLLBAR,
    FL_SCROLLBUTTON,
    FL_MENUBAR,
    FL_TEXTBOX,			/* for internal use only   */
    FL_LABELBUTTON,
    FL_COMBOBOX,
    FL_IMAGECANVAS,
    FL_THUMBWHEEL,
    FL_COLORWHEEL,
    FL_FORMBROWSER,
    /* sentinal */
    FL_CLASS_END
}
FL_CLASS;

#define FL_BEGIN_GROUP    10000
#define FL_END_GROUP      20000

#define FL_USER_CLASS_START   1001	/* min. user class  value */
#define FL_USER_CLASS_END     9999	/* max. user class  value */

/* how to display a form onto screen */

typedef enum
{
    FL_PLACE_FREE = 0,		/* size remain resizable      */
    FL_PLACE_MOUSE = 1,		/* mouse centered on form     */
    FL_PLACE_CENTER = 2,	/* center of the screen       */
    FL_PLACE_POSITION = 4,	/* specific size              */
    FL_PLACE_SIZE = 8,		/* specific size              */
    FL_PLACE_GEOMETRY = 16,	/* specific position          */
    FL_PLACE_ASPECT = 32,	/* keep aspect ratio          */
    FL_PLACE_FULLSCREEN = 64,	/* scale to fit screen        */
    FL_PLACE_HOTSPOT = 128,	/* so mouse fall on (x,y)     */
    FL_PLACE_ICONIC = 256,
    /* modifier */
    FL_FREE_SIZE = (1 << 14),
    FL_FIX_SIZE = (1 << 15)
}
FL_PLACE;

#define FL_PLACE_FREE_CENTER (FL_PLACE_CENTER|FL_FREE_SIZE)
#define FL_PLACE_CENTERFREE  (FL_PLACE_CENTER|FL_FREE_SIZE)

/* Window manager decoration request and forms attributes */
enum
{
    FL_FULLBORDER = 1,		/* normal */
    FL_TRANSIENT,		/* set TRANSIENT_FOR property             */
    FL_NOBORDER,		/* use override_redirect to supress decor. */
    FL_MODAL = 1 << 8		/* not implemented yet                    */
};

/* All box types */
typedef enum
{
    FL_NO_BOX,
    FL_UP_BOX,
    FL_DOWN_BOX,
    FL_BORDER_BOX,
    FL_SHADOW_BOX,
    FL_FRAME_BOX,
    FL_ROUNDED_BOX,
    FL_EMBOSSED_BOX,
    FL_FLAT_BOX,
    FL_RFLAT_BOX,
    FL_RSHADOW_BOX,
    FL_OVAL_BOX,
    FL_ROUNDED3D_UPBOX,
    FL_ROUNDED3D_DOWNBOX,
    FL_OVAL3D_UPBOX,
    FL_OVAL3D_DOWNBOX,
    FL_OVAL3D_FRAMEBOX,
    FL_OVAL3D_EMBOSSEDBOX,
    /* for internal use only */
    FL_TOPTAB_UPBOX,
    FL_SELECTED_TOPTAB_UPBOX,
    FL_BOTTOMTAB_UPBOX,
    FL_SELECTED_BOTTOMTAB_UPBOX,
    FL_OSHADOW_BOX,		/* not used */
    /* sentinel */
    FL_MAX_BOX_STYLES

}
FL_BOX_TYPE;

#define FL_IS_UPBOX(t)    ((t) == FL_UP_BOX ||         \
                           (t) == FL_OVAL3D_UPBOX ||   \
                           (t) == FL_ROUNDED3D_UPBOX)

#define FL_IS_DOWNBOX(t)  ((t) == FL_DOWN_BOX ||       \
                           (t) == FL_OVAL3D_DOWNBOX || \
                           (t) == FL_ROUNDED3D_DOWNBOX)

#define FL_TO_DOWNBOX(t)  (t==FL_UP_BOX?FL_DOWN_BOX: \
                            (t==FL_ROUNDED3D_UPBOX ? FL_ROUNDED3D_DOWNBOX:\
                              (t==FL_OVAL3D_UPBOX?FL_OVAL3D_DOWNBOX:t)))

/* How to place text relative to a box */
typedef enum
{
    FL_ALIGN_CENTER,
    FL_ALIGN_TOP = 1,
    FL_ALIGN_BOTTOM = 2,
    FL_ALIGN_LEFT = 4,
    FL_ALIGN_RIGHT = 8,
    FL_ALIGN_TOP_LEFT = (FL_ALIGN_TOP | FL_ALIGN_LEFT),
    FL_ALIGN_TOP_RIGHT = (FL_ALIGN_TOP | FL_ALIGN_RIGHT),
    FL_ALIGN_BOTTOM_LEFT = (FL_ALIGN_BOTTOM | FL_ALIGN_LEFT),
    FL_ALIGN_BOTTOM_RIGHT = (FL_ALIGN_BOTTOM | FL_ALIGN_RIGHT),
    FL_ALIGN_INSIDE = (1 << 13),
    FL_ALIGN_VERT = (1 << 14),	/* not functional yet  */

    FL_ALIGN_LEFT_TOP = FL_ALIGN_TOP_LEFT,
    FL_ALIGN_RIGHT_TOP = FL_ALIGN_TOP_RIGHT,
    FL_ALIGN_LEFT_BOTTOM = FL_ALIGN_BOTTOM_LEFT,
    FL_ALIGN_RIGHT_BOTTOM = FL_ALIGN_BOTTOM_RIGHT
}
FL_ALIGN;

/* mouse buttons. Don't have to be consecutive */
enum
{
    FL_MBUTTON1 = 1, FL_MBUTTON2, FL_MBUTTON3,
    FL_MBUTTON4, FL_MBUTTON5
};

#define FL_LEFT_MOUSE    FL_MBUTTON1
#define FL_MIDDLE_MOUSE  FL_MBUTTON2
#define FL_RIGHT_MOUSE   FL_MBUTTON3

#define FL_LEFTMOUSE     FL_LEFT_MOUSE
#define FL_MIDDLEMOUSE   FL_MIDDLE_MOUSE
#define FL_RIGHTMOUSE    FL_RIGHT_MOUSE

/* control when to reutrn input, slider and dial object. */
enum
{
    FL_RETURN_END_CHANGED = 0,
    FL_RETURN_CHANGED = 1,
    FL_RETURN_END = 2,
    FL_RETURN_ALWAYS = 3,
    FL_RETURN_DBLCLICK = 4
};

/*
 *  Some special color indeces for FL private colormap. It does not matter
 *  what the value of each enum is, but it must start from 0 and be
 *  consecutive.
 */
typedef enum
{
    FL_BLACK, FL_RED, FL_GREEN, FL_YELLOW,
    FL_BLUE, FL_MAGENTA, FL_CYAN, FL_WHITE,

    FL_TOMATO, FL_INDIANRED, FL_SLATEBLUE,

    FL_COL1, FL_RIGHT_BCOL, FL_BOTTOM_BCOL, FL_TOP_BCOL, FL_LEFT_BCOL,
    FL_MCOL,

    FL_INACTIVE, FL_PALEGREEN, FL_DARKGOLD,

    FL_ORCHID, FL_DARKCYAN, FL_DARKTOMATO, FL_WHEAT, FL_DARKORANGE,
    FL_DEEPPINK, FL_CHARTREUSE, FL_DARKVIOLET, FL_SPRINGGREEN,
    FL_DODGERBLUE,

    /* these two colors can be used to shade COL1 */
    FL_LIGHTER_COL1,		/* a little lighter than COL1 */
    FL_DARKER_COL1,		/* a little darker  than COL1 */

    FL_FREE_COL1 = 256, FL_FREE_COL2,
    FL_FREE_COL3, FL_FREE_COL4,
    FL_FREE_COL5, FL_FREE_COL6,
    FL_FREE_COL7, FL_FREE_COL8,
    FL_FREE_COL9, FL_FREE_COL10,
    FL_FREE_COL11, FL_FREE_COL12,
    FL_FREE_COL13, FL_FREE_COL14,
    FL_FREE_COL15, FL_FREE_COL16
}
FL_PD_COL;

#define FL_BUILT_IN_COLS  (FL_DARKER_COL1+1)
#define FL_INACTIVE_COL   FL_INACTIVE
#define FL_DOGERBLUE      FL_DODGERBLUE

/* Some aliases for the color. This is actually backwards ... */

#define FL_GRAY16           FL_RIGHT_BCOL
#define FL_GRAY35           FL_BOTTOM_BCOL
#define FL_GRAY80           FL_TOP_BCOL
#define FL_GRAY90           FL_LEFT_BCOL
#define FL_GRAY63           FL_COL1
#define FL_GRAY75           FL_MCOL

#define  FL_LCOL            FL_BLACK


/* Events that a form reacts to.  */
typedef enum
{
    FL_NOEVENT,
    FL_DRAW,
    FL_PUSH,
    FL_RELEASE,
    FL_ENTER,
    FL_LEAVE,
    FL_MOUSE,
    FL_DRAG = FL_MOUSE,
    FL_FOCUS,
    FL_UNFOCUS,
    FL_KEYBOARD,
    FL_KEYPRESS = FL_KEYBOARD,
    FL_MOTION,
    FL_STEP,
    FL_SHORTCUT,
    FL_FREEMEM,
    FL_OTHER,			/* property, selection etc */
    FL_DRAWLABEL,
    FL_DBLCLICK,		/* double click            */
    FL_TRPLCLICK,		/* triple click            */
    FL_ATTRIB,			/* attribute change        */
    FL_KEYRELEASE,
    FL_PS			/* dump a form into EPS    */
}
FL_EVENTS;

#define FL_MOVE   FL_MOTION	/* for compatibility */

/* Resize policies */
typedef enum
{
    FL_RESIZE_NONE, FL_RESIZE_X, FL_RESIZE_Y,
    FL_RESIZE_ALL = (FL_RESIZE_X | FL_RESIZE_Y)
}
FL_RESIZE_T;


/* Keyboard focus control */
typedef enum
{
    FL_KEY_NORMAL = 1,		/* normal keys(0-255) - tab +left/right */
    FL_KEY_TAB = 2,		/* normal keys + 4 direction cursor     */
    FL_KEY_SPECIAL = 4,		/* only needs special keys(>255)        */
    FL_KEY_ALL = 7		/* all keys                             */
}
FL_KEY;

#define FL_ALT_MASK      (1L<<25)	/* alt + Key --> FL_ALT_VAL + key */
#define FL_CONTROL_MASK  (1L<<26)
#define FL_SHIFT_MASK    (1L<<27)
#define FL_ALT_VAL       FL_ALT_MASK

/* Internal use */

typedef enum
{
    FL_FIND_INPUT,
    FL_FIND_AUTOMATIC,
    FL_FIND_MOUSE,
    FL_FIND_CANVAS,
    FL_FIND_KEYSPECIAL
}
FL_FIND;

/*
 *  Pop-up menu item attributes. NOTE if more than 8, need to change
 *  choice and menu class where mode is kept by a single byte
 */
enum
{
    FL_PUP_NONE,
    FL_PUP_GREY = 1,
    FL_PUP_BOX = 2,
    FL_PUP_CHECK = 4,
    FL_PUP_RADIO = 8
};

#define FL_PUP_GRAY      FL_PUP_GREY
#define FL_PUP_TOGGLE    FL_PUP_BOX
#define FL_PUP_INACTIVE  FL_PUP_GREY

/* read rgb.txt file */
FL_EXPORT int fl_init_RGBdatabase(
		const char *f
		);

FL_EXPORT int fl_lookup_RGBcolor(
		const char *req_name,
		int *r,
		int *g,
		int *b
		);


/* popup and menu entries */
typedef int (*FL_PUP_CB) (int);	/* call back prototype  */
typedef struct
{
    const char *text;		/* label of a popup/menu item    */
    FL_PUP_CB callback;		/* the callback function         */
    const char *shortcut;	/* hotkeys                       */
    int mode;			/* FL_PUP_GRAY, FL_PUP_CHECK etc */
    long reserved[2];
}
FL_PUP_ENTRY;

#define FL_MENU_ENTRY  FL_PUP_ENTRY


/*******************************************************************
 * FONTS
 ******************************************************************/

#define FL_MAXFONTS     48	/* max number of fonts */

typedef enum
{
    FL_INVALID_STYLE = -1,
    FL_NORMAL_STYLE,
    FL_BOLD_STYLE,
    FL_ITALIC_STYLE,
    FL_BOLDITALIC_STYLE,

    FL_FIXED_STYLE,
    FL_FIXEDBOLD_STYLE,
    FL_FIXEDITALIC_STYLE,
    FL_FIXEDBOLDITALIC_STYLE,

    FL_TIMES_STYLE,
    FL_TIMESBOLD_STYLE,
    FL_TIMESITALIC_STYLE,
    FL_TIMESBOLDITALIC_STYLE,

    FL_MISC_STYLE,
    FL_MISCBOLD_STYLE,
    FL_MISCITALIC_STYLE,
    FL_SYMBOL_STYLE,

    /* modfier masks. Need to fit a short  */
    FL_SHADOW_STYLE = (1 << 9),
    FL_ENGRAVED_STYLE = (1 << 10),
    FL_EMBOSSED_STYLE = (1 << 11)
}
FL_TEXT_STYLE;


#define FL_FONT_STYLE FL_TEXT_STYLE

#define special_style(a)  (a >=FL_SHADOW_STYLE &&\
                           a <= (FL_EMBOSSED_STYLE+FL_MAXFONTS))

/* Standard sizes in XForms */
#define FL_TINY_SIZE       8
#define FL_SMALL_SIZE      10
#define FL_NORMAL_SIZE     12
#define FL_MEDIUM_SIZE     14
#define FL_LARGE_SIZE      18
#define FL_HUGE_SIZE       24

#define FL_DEFAULT_SIZE   FL_SMALL_SIZE

/* Defines for compatibility */
#define FL_TINY_FONT      FL_TINY_SIZE
#define FL_SMALL_FONT     FL_SMALL_SIZE
#define FL_NORMAL_FONT    FL_NORMAL_SIZE
#define FL_MEDIUM_FONT    FL_MEDIUM_SIZE
#define FL_LARGE_FONT     FL_LARGE_SIZE
#define FL_HUGE_FONT      FL_HUGE_SIZE

#define FL_NORMAL_FONT1   FL_SMALL_FONT
#define FL_NORMAL_FONT2   FL_NORMAL_FONT
#define FL_DEFAULT_FONT   FL_SMALL_FONT

#define FL_BOUND_WIDTH  (FL_Coord)3	/* Border width of boxes */


/*
 *  Definition of basic struct that holds an object
 */

#define  FL_CLICK_TIMEOUT  400	/* double click interval */

struct forms_;
struct fl_pixmap;

typedef struct flobjs_
{
    struct forms_ *form;	/* the form this object belongs to     */
    void *u_vdata;		/* anything the user likes             */
    char *u_cdata;		/* anything the user likes             */
    long u_ldata;		/* anything the user likes             */

    int objclass;		/* class of object, button, slider etc */
    int type;			/* type within the class               */
    int boxtype;		/* what kind of box type               */
    FL_Coord x, y, w, h;	/* obj. location and size              */
    FL_Coord bw;
    FL_COLOR col1, col2;	/* colors of obj                       */
    char *label;		/* object label                        */
    FL_COLOR lcol;		/* label color                         */
    int align;
    int lsize, lstyle;		/* label size and style                */
    long *shortcut;
    int (*handle) (struct flobjs_ *, int, FL_Coord, FL_Coord, int, void *);
    void (*object_callback) (struct flobjs_ *, long);
    long argument;
    void *spec;			/* instantiation                      */

    int (*prehandle) (struct flobjs_ *, int, FL_Coord, FL_Coord, int, void *);
    int (*posthandle) (struct flobjs_ *, int, FL_Coord, FL_Coord, int, void *);

    /* re-configure preference */
    unsigned int resize;	/* what to do if WM resizes the FORM     */
    unsigned int nwgravity;	/* how to re-position top-left corner    */
    unsigned int segravity;	/* how to re-position lower-right corner */

    struct flobjs_ *prev;	/* prev. obj in form                     */
    struct flobjs_ *next;	/* next. obj in form                     */

    struct flobjs_ *parent;
    struct flobjs_ *child;
    struct flobjs_ *nc;
    int is_child;

    void *flpixmap;		/* pixmap double buffering stateinfo     */
    int use_pixmap;		/* true to use pixmap double buffering   */

    /* some interaction flags */
    int double_buffer;		/* only used by mesa/gl canvas           */
    int pushed;
    int focus;
    int belowmouse;
    int active;			/* if accept event */
    int input;
    int wantkey;
    int radio;
    int automatic;
    int redraw;
    int visible;
    int clip;
    unsigned long click_timeout;
    void *c_vdata;		/* for class use            */
    char *c_cdata;		/* for class use            */
    long c_ldata;		/* for class use            */
    unsigned int spec_size;	/* for internal use         */
    FL_COLOR aux_col1, aux_col2;	/* aux colors               */
    FL_COLOR dbl_background;	/* double buffer background */
    int how_return;
    char *tooltip;
    int tipID;
    int group_id;
    int reserved[5];		/* for future use           */
}
FL_OBJECT;

/* callback function for an entire form */
typedef void (*FL_FORMCALLBACKPTR) (struct flobjs_ *, void *);

/* object callback function      */
typedef void (*FL_CALLBACKPTR) (FL_OBJECT *, long);

/* preemptive callback function  */
typedef int (*FL_RAW_CALLBACK) (struct forms_ *, void *);

/* at close (WM menu delete/close etc.) */
typedef int (*FL_FORM_ATCLOSE) (struct forms_ *, void *);

/* deactivate/activate callback */
typedef void (*FL_FORM_ATDEACTIVATE) (struct forms_ *, void *);
typedef void (*FL_FORM_ATACTIVATE) (struct forms_ *, void *);

typedef int (*FL_HANDLEPTR) (FL_OBJECT *, int, FL_Coord, FL_Coord, int, void *);

/* error callback */
typedef void (*FL_ERROR_FUNC)( const char *, const char *,... );

FL_EXPORT FL_OBJECT *FL_EVENT;


/*** FORM ****/

/* form visibility state: form->visible */
enum {
  FL_BEING_HIDDEN = -1, 
  FL_HIDDEN = 0, 
  FL_INVISIBLE = FL_HIDDEN,
  FL_VISIBLE = 1
}; 

typedef struct forms_
{
    void *fdui;			/* for fdesign              */
    void *u_vdata;		/* for application          */
    char *u_cdata;		/* for application          */
    long u_ldata;		/* for application          */

    char *label;		/* window title             */
    unsigned long window;	/* X resource ID for window */
    FL_Coord x, y, w, h;	/* current geometry info    */
    FL_Coord hotx, hoty;	/* hot-spot of the form     */

    struct flobjs_ *first;
    struct flobjs_ *last;
    struct flobjs_ *focusobj;

    FL_FORMCALLBACKPTR form_callback;
    FL_FORM_ATACTIVATE activate_callback;
    FL_FORM_ATDEACTIVATE deactivate_callback;
    void *form_cb_data, *activate_data, *deactivate_data;

    FL_RAW_CALLBACK key_callback;
    FL_RAW_CALLBACK push_callback;
    FL_RAW_CALLBACK crossing_callback;
    FL_RAW_CALLBACK motion_callback;
    FL_RAW_CALLBACK all_callback;

    unsigned long compress_mask;
    unsigned long evmask;

    /* WM_DELETE_WINDOW message handler */
    FL_FORM_ATCLOSE close_callback;
    void *close_data;

    void *flpixmap;		/* back buffer             */

    unsigned long icon_pixmap;
    unsigned long icon_mask;

    /* interaction and other flags */
    int vmode;			/* current X visual class  */
    int deactivated;		/* true if sensitive       */
    int use_pixmap;		/* true if dbl buffering   */
    int frozen;			/* true if sync change     */
    int visible;		/* true if mapped          */
    int wm_border;		/* window manager info     */
    unsigned int prop;		/* other attributes        */
    int has_auto;
    int top;
    int sort_of_modal;		/* internal use.           */
    struct forms_ *parent;
    struct forms_ *child;
    struct flobjs_ *parent_obj;
    int attached;               /* not independent anymore */
    void (*pre_attach)(struct forms_ *);
    void *attach_data;
    int no_tooltip;
    int reserved[10];		/* future use              */
}
FL_FORM;

/*
 * All FD_xxx structure emitted by fdesign contains at least the
 * following
 */
typedef struct
{
    FL_FORM *form;
    void *vdata;
    char *cdata;
    long ldata;
}
FD_Any;


/*
 * Async IO stuff
 */

enum
{
    FL_READ = 1, FL_WRITE = 2, FL_EXCEPT = 4
};

/* IO other than XEvent Q */
typedef void (*FL_IO_CALLBACK) (int, void *);
FL_EXPORT void fl_add_io_callback(
		int fd,
		unsigned mask,
		FL_IO_CALLBACK callback,
		void *data
		);

FL_EXPORT void fl_remove_io_callback(
		int fd,
		unsigned mask,
		FL_IO_CALLBACK cb
		);


/* signals */

typedef void (*FL_SIGNAL_HANDLER) (int, void *);
FL_EXPORT void fl_add_signal_callback(
		int s,
		void (*cb)(int,
		void *),
		void *data
		);

FL_EXPORT void fl_remove_signal_callback(
		int s
		);

FL_EXPORT void fl_signal_caught(
		int s
		);

FL_EXPORT void fl_app_signal_direct(
		int y
		);


/* timeouts */
typedef void (*FL_TIMEOUT_CALLBACK) (int, void *);
FL_EXPORT int fl_add_timeout(
		long msec,
		FL_TIMEOUT_CALLBACK callback,
		void *data
		);

FL_EXPORT void fl_remove_timeout(
		int id
		);



/*  Some utility stuff */
#ifndef FL_VN_PAIR_STRUCT
#define FL_VN_PAIR_STRUCT
typedef struct
{
    int val;
    const char *name;
}
FL_VN_PAIR;
#endif

FL_EXPORT int fl_get_vn_value(
		register FL_VN_PAIR *vn_pair,
		const char *name
		);

FL_EXPORT const char *fl_get_vn_name(
		register FL_VN_PAIR *vn_pair,
		int val
		);

FL_EXPORT unsigned long fl_msleep(
		unsigned long msec
		);


/*
 *  Basic public routine prototypes
 */

FL_EXPORT int fl_library_version(
		int *ver,
		int *rev
		);


/** Generic routines that deal with FORMS **/

FL_EXPORT FL_FORM *fl_bgn_form(
		int type,
		FL_Coord w,
		FL_Coord h
		);

FL_EXPORT void fl_end_form(
		void
		);

FL_EXPORT FL_OBJECT *fl_do_forms(
		void
		);

FL_EXPORT FL_OBJECT *fl_check_forms(
		void
		);

FL_EXPORT FL_OBJECT *fl_do_only_forms(
		void
		);

FL_EXPORT FL_OBJECT *fl_check_only_forms(
		void
		);

FL_EXPORT void fl_freeze_form(
		FL_FORM *form
		);


FL_EXPORT void fl_set_focus_object(
		FL_FORM *form,
		FL_OBJECT *obj
		);

FL_EXPORT FL_OBJECT *fl_get_focus_object(
		FL_FORM *form
		);

FL_EXPORT void fl_reset_focus_object(
		FL_OBJECT *ob
		);

#define fl_set_object_focus   fl_set_focus_object

FL_EXPORT FL_FORM_ATCLOSE fl_set_form_atclose(
		FL_FORM *form,
		FL_FORM_ATCLOSE fmclose,
		void *data
		);

FL_EXPORT FL_FORM_ATCLOSE fl_set_atclose(
		FL_FORM_ATCLOSE fmclose,
		void *data
		);


FL_EXPORT FL_FORM_ATACTIVATE fl_set_form_atactivate(
		FL_FORM *form,
		FL_FORM_ATACTIVATE cb,
		void *data
		);

FL_EXPORT FL_FORM_ATDEACTIVATE fl_set_form_atdeactivate(
		FL_FORM *form,
		FL_FORM_ATDEACTIVATE cb,
		void *data
		);


FL_EXPORT void fl_unfreeze_form(
		FL_FORM *form
		);

FL_EXPORT void fl_deactivate_form(
		FL_FORM *form
		);

FL_EXPORT void fl_activate_form(
		FL_FORM *form
		);

FL_EXPORT void fl_deactivate_all_forms(
		void
		);

FL_EXPORT void fl_activate_all_forms(
		void
		);

FL_EXPORT void fl_freeze_all_forms(
		void
		);

FL_EXPORT void fl_unfreeze_all_forms(
		void
		);

FL_EXPORT void fl_scale_form(
		FL_FORM *form,
		double xsc,
		double ysc
		);

FL_EXPORT void fl_set_form_position(
		FL_FORM *form,
		FL_Coord x,
		FL_Coord y
		);

FL_EXPORT void fl_set_form_title(
		FL_FORM *form,
		const char *name
		);


FL_EXPORT void fl_set_form_property(
		FL_FORM *form,
		unsigned prop
		);

FL_EXPORT void fl_set_app_mainform(
		FL_FORM *form
		);

FL_EXPORT FL_FORM *fl_get_app_mainform(
		void
		);

FL_EXPORT void fl_set_app_nomainform(
		int flag
		);


FL_EXPORT void fl_set_form_callback(
		FL_FORM *form,
		FL_FORMCALLBACKPTR callback,
		void *d
		);

#define  fl_set_form_call_back    fl_set_form_callback

FL_EXPORT void fl_set_form_size(
		FL_FORM *form,
		FL_Coord w,
		FL_Coord h
		);

FL_EXPORT void fl_set_form_hotspot(
		FL_FORM *form,
		FL_Coord x,
		FL_Coord y
		);

FL_EXPORT void fl_set_form_hotobject(
		FL_FORM *form,
		FL_OBJECT *ob
		);

FL_EXPORT void fl_set_form_minsize(
		FL_FORM *form,
		FL_Coord w,
		FL_Coord h
		);

FL_EXPORT void fl_set_form_maxsize(
		FL_FORM *form,
		FL_Coord w,
		FL_Coord h
		);

FL_EXPORT void fl_set_form_event_cmask(
		FL_FORM *form,
		unsigned long cmask
		);

FL_EXPORT unsigned long fl_get_form_event_cmask(
		FL_FORM *form
		);


FL_EXPORT void fl_set_form_geometry(
		FL_FORM *form,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h
		);


#define fl_set_initial_placement fl_set_form_geometry

FL_EXPORT long fl_show_form(
		FL_FORM *form,
		int place,
		int border,
		const char *name
		);

FL_EXPORT void fl_hide_form(
		FL_FORM *form
		);

FL_EXPORT void fl_free_form(
		FL_FORM *form
		);

FL_EXPORT void fl_redraw_form(
		FL_FORM *form
		);

FL_EXPORT void fl_set_form_dblbuffer(
		FL_FORM *form,
		int y
		);

FL_EXPORT long fl_prepare_form_window(
		FL_FORM *form,
		int place,
		int border,
		const char *name
		);

FL_EXPORT long fl_show_form_window(
		FL_FORM *form
		);

FL_EXPORT double fl_adjust_form_size(
		FL_FORM *form
		);

FL_EXPORT int fl_form_is_visible(
		FL_FORM *form
		);


FL_EXPORT FL_RAW_CALLBACK fl_register_raw_callback(
		FL_FORM *form,
		unsigned long mask,
		FL_RAW_CALLBACK rcb
		);


#define fl_register_call_back fl_register_raw_callback

FL_EXPORT FL_OBJECT *fl_bgn_group(
		void
		);

FL_EXPORT FL_OBJECT *fl_end_group(
		void
		);

FL_EXPORT void fl_addto_group(
		FL_OBJECT *group
		);


/****** Routines that deal with FL_OBJECTS ********/

FL_EXPORT void fl_set_object_boxtype(
		FL_OBJECT *ob,
		int boxtype
		);

FL_EXPORT void fl_set_object_bw(
		FL_OBJECT *ob,
		int bw
		);

FL_EXPORT void fl_set_object_resize(
		FL_OBJECT *ob,
		unsigned what
		);

FL_EXPORT void fl_set_object_gravity(
		FL_OBJECT *ob,
		unsigned nw,
		unsigned se
		);

FL_EXPORT void fl_set_object_lsize(
		FL_OBJECT *ob,
		int lsize
		);

FL_EXPORT void fl_set_object_lstyle(
		FL_OBJECT *ob,
		int lstyle
		);

FL_EXPORT void fl_set_object_lcol(
		FL_OBJECT *ob,
		FL_COLOR lcol
		);

FL_EXPORT void fl_set_object_return(
		FL_OBJECT *ob,
		int when
		);

FL_EXPORT void fl_set_object_lalign(
		FL_OBJECT *ob,
		int align
		);

FL_EXPORT void fl_set_object_shortcut(
		FL_OBJECT *obj,
		const char *sstr,
		int showit
		);

FL_EXPORT void fl_set_object_shortcutkey(
		FL_OBJECT *obj,
		unsigned int keysym
		);

FL_EXPORT void fl_set_object_dblbuffer(
		FL_OBJECT *ob,
		int y
		);

FL_EXPORT void fl_set_object_color(
		FL_OBJECT *ob,
		FL_COLOR col1,
		FL_COLOR col2
		);

FL_EXPORT void fl_set_object_label(
		FL_OBJECT *ob,
		const char *label
		);

FL_EXPORT void fl_set_object_helper(
		FL_OBJECT *ob,
		const char *tip
		);

FL_EXPORT void fl_set_object_position(
		FL_OBJECT *obj,
		FL_Coord x,
		FL_Coord y
		);

FL_EXPORT void fl_set_object_size(
		FL_OBJECT *obj,
		FL_Coord w,
		FL_Coord h
		);

FL_EXPORT void fl_set_object_automatic(
		FL_OBJECT *ob,
		int flag
		);

FL_EXPORT void fl_draw_object_label(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_draw_object_label_outside(
		FL_OBJECT *ob
		);

FL_EXPORT FL_OBJECT *fl_get_object_component(
		FL_OBJECT *composite,
		int objclass,
		int type,
		int numb
		);

FL_EXPORT void fl_for_all_objects(
		FL_FORM *form,
		int (*cb)(FL_OBJECT *,
		void *),
		void *v
		);

#define fl_draw_object_outside_label fl_draw_object_label_outside
#define  fl_set_object_dblclick(ob, timeout)  (ob)->click_timeout = (timeout);
FL_EXPORT void fl_set_object_geometry(
		FL_OBJECT *obj,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h
		);

FL_EXPORT void fl_move_object(
		FL_OBJECT *obj,
		FL_Coord dx,
		FL_Coord dy
		);


#define fl_set_object_lcolor  fl_set_object_lcol

FL_EXPORT void fl_fit_object_label(
		FL_OBJECT *obj,
		FL_Coord xmargin,
		FL_Coord ymargin
		);


FL_EXPORT void fl_get_object_geometry(
		FL_OBJECT *ob,
		FL_Coord *x,
		FL_Coord *y,
		FL_Coord *w,
		FL_Coord *h
		);


FL_EXPORT void fl_get_object_position(
		FL_OBJECT *ob,
		FL_Coord *x,
		FL_Coord *y
		);


FL_EXPORT const char *fl_get_object_label(
		FL_OBJECT *ob
		);


/* this one takes into account the label */
FL_EXPORT void fl_get_object_bbox(
		FL_OBJECT *obj,
		FL_Coord *x,
		FL_Coord *y,
		FL_Coord *w,
		FL_Coord *h
		);


#define fl_compute_object_geometry   fl_get_object_bbox

FL_EXPORT void fl_call_object_callback(
		FL_OBJECT *ob
		);

FL_EXPORT FL_HANDLEPTR fl_set_object_prehandler(
		FL_OBJECT *ob,
		FL_HANDLEPTR phandler
		);

FL_EXPORT FL_HANDLEPTR fl_set_object_posthandler(
		FL_OBJECT *ob,
		FL_HANDLEPTR post
		);

FL_EXPORT FL_CALLBACKPTR fl_set_object_callback(
		FL_OBJECT *obj,
		FL_CALLBACKPTR callback,
		long argument
		);


#define fl_set_object_align   fl_set_object_lalign
#define fl_set_call_back      fl_set_object_callback

FL_EXPORT void fl_redraw_object(
		FL_OBJECT *obj
		);

FL_EXPORT void fl_scale_object(
		FL_OBJECT *ob,
		double xs,
		double ys
		);

FL_EXPORT void fl_show_object(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_hide_object(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_free_object(
		FL_OBJECT *obj
		);

FL_EXPORT void fl_delete_object(
		FL_OBJECT *obj
		);

FL_EXPORT void fl_trigger_object(
		FL_OBJECT *obj
		);

FL_EXPORT void fl_activate_object(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_deactivate_object(
		FL_OBJECT *ob
		);


FL_EXPORT int fl_enumerate_fonts(
		void (*output)(const char *s),
		int shortform
		);

FL_EXPORT int fl_set_font_name(
		int n,
		const char *name
		);

FL_EXPORT void fl_set_font(
		int numb,
		int size
		);


/* routines that facilitate free object */

FL_EXPORT int fl_get_char_height(
		int style,
		int size,
		int *asc,
		int *desc
		);

FL_EXPORT int fl_get_char_width(
		int style,
		int size
		);

FL_EXPORT int fl_get_string_height(
		int style,
		int size,
		const char *s,
		int len,
		int *asc,
		int *desc
		);

FL_EXPORT int fl_get_string_width(
		int style,
		int size,
		const char *s,
		int len
		);

FL_EXPORT int fl_get_string_widthTAB(
		int style,
		int size,
		const char *s,
		int len
		);

FL_EXPORT void fl_get_string_dimension(
		int fntstyle,
		int fntsize,
		const char *s,
		int len,
		int *width,
		int *height
		);


#define fl_get_string_size  fl_get_string_dimension

FL_EXPORT void fl_get_align_xy(
		int align,
		int x,
		int y,
		int w,
		int h,
		int xsize,
		int ysize,
		int xoff,
		int yoff,
		int *xx,
		int *yy
		);


FL_EXPORT void fl_drw_text(
		int align,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		FL_COLOR c,
		int style,
		int size,
		char *istr
		);


FL_EXPORT void fl_drw_text_beside(
		int align,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		unsigned long c,
		int style,
		int size,
		char *str
		);


#if 0
#define fl_draw_text(a,x,y,w,h,c,st,sz,s)    \
      (((a) & FL_ALIGN_INSIDE) ? fl_drw_text:fl_drw_text_beside)\
      (a,x,y,w,h,c,st,sz,s)
#endif

FL_EXPORT void fl_drw_text_cursor(
		int align,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		FL_COLOR c,
		int style,
		int size,
		char *str,
		int cc,
		int pos
		);


FL_EXPORT void fl_drw_box(
		int style,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		FL_COLOR c,
		int bw_in
		);


typedef void (*FL_DRAWPTR) (FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
			    int, FL_COLOR);
FL_EXPORT int fl_add_symbol(
		const char *name,
		FL_DRAWPTR drawit,
		int scalable
		);

FL_EXPORT int fl_draw_symbol(
		const char *label,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		FL_COLOR col
		);

enum
{
    FL_SLIDER_NONE = 0,
    FL_SLIDER_BOX = 1,
    FL_SLIDER_KNOB = 2,
    FL_SLIDER_UP = 4,
    FL_SLIDER_DOWN = 8,
    FL_SLIDER_ALL = 15
};
FL_EXPORT void fl_drw_slider(
		int boxtype,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		FL_COLOR col1,
		FL_COLOR col2,
		int sltype,
		double size,
		double val,
		char *str,
		int d,
		int inv,
		FL_Coord bw
		);


FL_EXPORT unsigned long fl_mapcolor(
		FL_COLOR col,
		int r,
		int g,
		int b
		);

FL_EXPORT long fl_mapcolorname(
		FL_COLOR col,
		const char *name
		);

#define fl_mapcolor_name  fl_mapcolorname

FL_EXPORT void fl_free_colors(
		FL_COLOR *c,
		int n
		);

FL_EXPORT void fl_free_pixels(
		unsigned long *pix,
		int n
		);

FL_EXPORT void fl_set_color_leak(
		int y
		);

FL_EXPORT unsigned long fl_getmcolor(
		FL_COLOR i,
		int *r,
		int *g,
		int *b
		);

FL_EXPORT unsigned long fl_get_pixel(
		FL_COLOR col
		);

#define fl_get_flcolor   fl_get_pixel

FL_EXPORT void fl_get_icm_color(
		FL_COLOR col,
		int *r,
		int *g,
		int *b
		);

FL_EXPORT void fl_set_icm_color(
		FL_COLOR col,
		int r,
		int g,
		int b
		);


FL_EXPORT void fl_color(
		FL_COLOR col
		);

FL_EXPORT void fl_bk_color(
		unsigned long col
		);

FL_EXPORT void fl_textcolor(
		FL_COLOR col
		);

FL_EXPORT void fl_bk_textcolor(
		unsigned long col
		);

FL_EXPORT void fl_set_gamma(
		double r,
		double g,
		double b
		);


FL_EXPORT void fl_show_errors(
		int y
		);

/* Some macros */
#define FL_max(a,b)            ((a) > (b) ? (a):(b) )
#define FL_min(a,b)            ((a) < (b) ? (a):(b) )
#define FL_abs(a)              ((a) > 0 ? (a):(-(a)))
#define FL_nint(a)             ((a) > 0 ? ((a) + 0.5f):((a) - 0.5f))
#define FL_clamp(a,amin,amax)  ((a)<(amin) ? (amin):((a)>(amax) ? (amax):(a)))

typedef int (*FL_FSCB) (const char *, void *);

/* utilities for new objects */
FL_EXPORT FL_FORM *fl_current_form;

FL_EXPORT void fl_add_object(
		FL_FORM *form,
		FL_OBJECT *obj
		);

FL_EXPORT void fl_addto_form(
		FL_FORM *form
		);

FL_EXPORT FL_OBJECT *fl_make_object(
		int objclass,
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label,
		FL_HANDLEPTR handle
		);


FL_EXPORT void fl_set_coordunit(
		int u
		);

FL_EXPORT void fl_set_border_width(
		int bw
		);

FL_EXPORT void fl_set_scrollbar_type(
		int t
		);

#define fl_set_thinscrollbar(t)   fl_set_scrollbar_type(\
                                  t ? FL_THIN_SCROLLBAR:FL_NORMAL_SCROLLBAR)

FL_EXPORT void fl_flip_yorigin(
		void
		);


FL_EXPORT int fl_get_coordunit(
		void
		);

FL_EXPORT int fl_get_border_width(
		void
		);


/* misc. routines */
FL_EXPORT void fl_ringbell(
		int percent
		);

FL_EXPORT void fl_gettime(
		long *sec,
		long *usec
		);

FL_EXPORT const char *fl_now(
		void
		);

FL_EXPORT const char *fl_whoami(
		void
		);

FL_EXPORT long fl_mouse_button(
		void
		);

FL_EXPORT char *fl_strdup(
		const char *s
		);

FL_EXPORT void fl_set_err_logfp(
		FILE *fp
		);

FL_EXPORT void fl_set_error_handler(
		FL_ERROR_FUNC user_func
		);

#define fl_mousebutton fl_mouse_button

/* these give more flexibility for future changes. Also application
 * can re-assign these pointers to whatever function it wants, e.g.,
 * to a shared memory pool allocator.
 */
FL_EXPORT void (*fl_free) (void *);

FL_EXPORT void *(*fl_malloc) (size_t);

FL_EXPORT void *(*fl_calloc) (size_t, size_t);

FL_EXPORT void *(*fl_realloc) (void *, size_t);


#define FL_MAX_MENU_CHOICE_ITEMS   128

enum
{
    FL_TRIANGLE_UPBOX1,
    FL_TRIANGLE_UPBOX2,
    FL_TRIANGLE_UPBOX3,
    FL_TRIANGLE_UPBOX4,
    FL_TRIANGLE_UPBOX6,
    FL_TRIANGLE_UPBOX7,
    FL_TRIANGLE_UPBOX8,
    FL_TRIANGLE_UPBOX9,
    FL_TRIANGLE_DOWNBOX1,
    FL_TRIANGLE_DOWNBOX2,
    FL_TRIANGLE_DOWNBOX3,
    FL_TRIANGLE_DOWNBOX4,
    FL_TRIANGLE_DOWNBOX6,
    FL_TRIANGLE_DOWNBOX7,
    FL_TRIANGLE_DOWNBOX8,
    FL_TRIANGLE_DOWNBOX9
};

typedef const char *(*FL_VAL_FILTER) (FL_OBJECT *, double, int);

#endif

/*
 * $Id: XBasic.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *  X Window dependent stuff
 *
 */

#ifndef FL_XBASIC_H
#define FL_XBASIC_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/Xresource.h>

/* draw mode */
enum
{
    FL_XOR = GXxor,
    FL_COPY = GXcopy,
    FL_AND = GXand
};

#define FL_MINDEPTH  1

/* FL_xxx does not do anything anymore, but kept for compatibility */
enum
{
    FL_illegalVisual = -1,
    FL_StaticGray = StaticGray,
    FL_GrayScale = GrayScale,
    FL_StaticColor = StaticColor,
    FL_PseudoColor = PseudoColor,
    FL_TrueColor = TrueColor,
    FL_DirectColor = DirectColor,
    FL_DefaultVisual = 10	/* special request */
};

enum
{
    FL_North = NorthGravity,
    FL_NorthEast = NorthEastGravity,
    FL_NorthWest = NorthWestGravity,
    FL_South = SouthGravity,
    FL_SouthEast = SouthEastGravity,
    FL_SouthWest = SouthWestGravity,
    FL_East = EastGravity,
    FL_West = WestGravity,
    FL_NoGravity = ForgetGravity,
    FL_ForgetGravity = ForgetGravity
};

#ifndef GreyScale
#define GreyScale   GrayScale
#define StaticGrey  StaticGray
#endif

#define FL_is_gray(v)  (v==GrayScale || v==StaticGray)
#define FL_is_rgb(v)   (v==TrueColor || v==DirectColor)


/*
 * Internal colormap size. Not really very meaningful as fl_mapcolor
 * and company allow color "leakage", that is, although only FL_MAX_COLS
 * are kept in the internal colormap, the server might have substantially
 * more colors allocated
 */

#define FL_MAX_COLORS   1024
#define FL_MAX_COLS     FL_MAX_COLORS

/*
 * FL graphics state information. Some are redundant.
 */

typedef struct
{
    XVisualInfo *xvinfo;
    XFontStruct *cur_fnt;	/* current font in default GC       */
    Colormap colormap;		/* colormap valid for xvinfo        */
    Window trailblazer;		/* a valid window for xvinfo        */
    int vclass, depth;		/* visual class and color depth     */
    int rgb_bits;		/* primary color resolution         */
    int dithered;		/* true if dithered color           */
    int pcm;			/* true if colormap is not shared   */
    GC gc[16];			/* working GC                       */
    GC textgc[16];		/* GC used exclusively for text     */
    GC dimmedGC;		/* A GC having a checkboard stipple */
    unsigned long lut[FL_MAX_COLS];	/* secondary lookup table         */
    unsigned int rshift, rmask, rbits;
    unsigned int gshift, gmask, gbits;
    unsigned int bshift, bmask, bbits;
}
FL_STATE;

#define FL_State FL_STATE	/* for compatibility */

/***** Global variables ******/

FL_EXPORT Display *fl_display;

FL_EXPORT int fl_screen;

FL_EXPORT Window fl_root;	/* root window                */
FL_EXPORT Window fl_vroot;	/* virtual root window        */
FL_EXPORT int fl_scrh, fl_scrw;	/* screen dimension in pixels */
FL_EXPORT int fl_vmode;


/* current version only runs in single visual mode */
#define  fl_get_vclass()        fl_vmode
#define  fl_get_form_vclass(a)  fl_vmode
#define  fl_get_gc()            fl_state[fl_vmode].gc[0]

FL_EXPORT FL_State fl_state[];

FL_EXPORT char *fl_ul_magic_char;

FL_EXPORT int fl_mode_capable(
		int mode,
		int warn
		);


#define fl_default_win()       (fl_state[fl_vmode].trailblazer)
#define fl_default_window()    (fl_state[fl_vmode].trailblazer)
/*
 * All pixmaps used by FL_OBJECT to simulate double buffering have the
 * following entries in the structure. FL_Coord x,y are used to shift
 * the origin of the drawing routines
 */
typedef struct
{
    Pixmap pixmap;
    Window win;
    Visual *visual;
    FL_Coord x, y;
    unsigned int w, h;
    int depth;
    FL_COLOR dbl_background;
    FL_COLOR pixel;
}
FL_pixmap;


/* fonts related */

#define FL_MAX_FONTSIZES   10

typedef struct
{
    XFontStruct *fs[FL_MAX_FONTSIZES];	/* cached fontstruct */
    short size[FL_MAX_FONTSIZES];	/* cached sizes      */
    short nsize;		/* cached so far     */
    char fname[80];		/* without size info     */
}
FL_FONT;

/*
 * Some basic drawing routines
 */

typedef XPoint FL_POINT;
typedef XRectangle FL_RECT;

/* rectangles */
FL_EXPORT void fl_rectangle(
		int fill,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		FL_COLOR col
		);

FL_EXPORT void fl_rectbound(
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		FL_COLOR col
		);

#define fl_rectf(x,y,w,h,c)   fl_rectangle(1, x,y,w,h,c)
#define fl_rect(x,y,w,h,c)    fl_rectangle(0, x,y,w,h,c)

/* rectangle with rounded-corners */
FL_EXPORT void fl_roundrectangle(
		int fill,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		FL_COLOR col
		);

#define fl_roundrectf(x,y,w,h,c) fl_roundrectangle(1,x,y,w,h,c)
#define fl_roundrect(x,y,w,h,c) fl_roundrectangle(0,x,y,w,h,c)

/* general polygon and polylines */
FL_EXPORT void fl_polygon(
		int fill,
		FL_POINT *xp,
		int n,
		unsigned long col
		);

#define fl_polyf(p,n,c)  fl_polygon(1, p, n, c)
#define fl_polyl(p,n,c)  fl_polygon(0, p, n, c)
#define fl_polybound(p,n,c) do {fl_polyf(p,n,c);fl_polyl(p,n,FL_BLACK);}while(0)

FL_EXPORT void fl_lines(
		FL_POINT *xp,
		int n,
		FL_COLOR col
		);

FL_EXPORT void fl_line(
		FL_Coord xi,
		FL_Coord yi,
		FL_Coord xf,
		FL_Coord yf,
		FL_COLOR c
		);

FL_EXPORT void fl_point(
		FL_Coord x,
		FL_Coord y,
		FL_COLOR c
		);

FL_EXPORT void fl_points(
		FL_POINT *p,
		int np,
		FL_COLOR c
		);

#define fl_simple_line fl_line

FL_EXPORT void fl_dashedlinestyle(
		const char *dash,
		int ndash
		);

FL_EXPORT void fl_update_display(
		int block
		);



#define fl_diagline(x,y,w,h,c) fl_line(x,y,(x)+(w)-1,(y)+(h)-1,c)

/* line attributes */
enum
{
    FL_SOLID = LineSolid,
    FL_USERDASH = LineOnOffDash,
    FL_USERDOUBLEDASH = LineDoubleDash,
    FL_DOT,
    FL_DOTDASH,
    FL_DASH,
    FL_LONGDASH
};
FL_EXPORT void fl_linewidth(
		int n
		);

FL_EXPORT void fl_linestyle(
		int n
		);

FL_EXPORT void fl_drawmode(
		int request
		);


FL_EXPORT int fl_get_linewidth(
		void
		);

FL_EXPORT int fl_get_linestyle(
		void
		);

FL_EXPORT int fl_get_drawmode(
		void
		);


#define fl_set_linewidth    fl_linewidth
#define fl_set_linestyle    fl_linestyle
#define fl_set_drawmode     fl_drawmode


/** ellipses **/
FL_EXPORT void fl_oval(
		int fill,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		FL_COLOR col
		);

FL_EXPORT void fl_ovalbound(
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		FL_COLOR col
		);

FL_EXPORT void fl_ovalarc(
		int fill,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		int t0,
		int dt,
		FL_COLOR col
		);


#define fl_ovalf(x,y,w,h,c)     fl_oval(1,x,y,w,h,c)
#define fl_ovall(x,y,w,h,c)     fl_oval(0,x,y,w,h,c)
#define fl_oval_bound           fl_ovalbound

#define fl_circf(x,y,r,col)  fl_oval(1,(x)-(r),(y)-(r),2*(r),2*(r),col)
#define fl_circ(x,y,r,col)   fl_oval(0,(x)-(r),(y)-(r),2*(r),2*(r),col)

/* arcs */
FL_EXPORT void fl_pieslice(
		int fill,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		int a1,
		int a2,
		FL_COLOR col
		);


#define fl_arcf(x,y,r,a1,a2,c)  fl_pieslice(1,(x)-(r),(y)-(r),\
                                (2*(r)),(2*(r)), a1,a2,c)

#define fl_arc(x,y,r,a1,a2,c)  fl_pieslice(0,(x)-(r),(y)-(r), \
                               (2*(r)),(2*(r)), a1,a2,c)

/* misc. stuff */
FL_EXPORT void fl_add_vertex(
		FL_Coord x,
		FL_Coord y
		);

FL_EXPORT void fl_add_float_vertex(
		float x,
		float y
		);

FL_EXPORT void fl_reset_vertex(
		void
		);

FL_EXPORT void fl_endpolygon(
		void
		);

FL_EXPORT void fl_endclosedline(
		void
		);

FL_EXPORT void fl_endline(
		void
		);

#define fl_bgnline       fl_reset_vertex
#define fl_bgnclosedline fl_reset_vertex
#define fl_bgnpolygon    fl_reset_vertex
#define fl_v2s(v)        fl_add_vertex(v[0], v[1])
#define fl_v2i(v)        fl_add_vertex(v[0], v[1])
#define fl_v2f(v)        fl_add_float_vertex(v[0], v[1])
#define fl_v2d(v)        fl_add_float_vertex(v[0], v[1])

/* high level drawing routines */
FL_EXPORT void fl_drw_frame(
		int style,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		FL_COLOR c,
		int bw
		);

FL_EXPORT void fl_drw_checkbox(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		FL_COLOR col,
		int bw
		);


/*
 * Interfaces
 */
FL_EXPORT XFontStruct *fl_get_fontstruct(
		int style,
		int size
		);

#define fl_get_font_struct fl_get_fontstruct
#define fl_get_fntstruct fl_get_font_struct

FL_EXPORT Window fl_get_mouse(
		FL_Coord *x,
		FL_Coord *y,
		unsigned *keymask
		);

FL_EXPORT void fl_set_mouse(
		FL_Coord mx,
		FL_Coord my
		);

FL_EXPORT Window fl_get_win_mouse(
		Window win,
		FL_Coord *x,
		FL_Coord *y,
		unsigned *keymask
		);

FL_EXPORT Window fl_get_form_mouse(
		FL_FORM *fm,
		FL_Coord *x,
		FL_Coord *y,
		unsigned *keymask
		);

FL_EXPORT FL_FORM *fl_win_to_form(
		register Window win
		);

FL_EXPORT void fl_set_form_icon(
		FL_FORM *form,
		Pixmap p,
		Pixmap m
		);


#define fl_raise_form(f) do {if(f->window) XRaiseWindow(fl_display,f->window);}while(0)
#define fl_lower_form(f) do {if(f->window) XLowerWindow(fl_display,f->window);}while(0)

#define fl_set_foreground(gc,c) XSetForeground(fl_display,gc,fl_get_pixel(c))
#define fl_set_background(gc,c) XSetBackground(fl_display,gc,fl_get_pixel(c))

/* General windowing support */

FL_EXPORT Window fl_wincreate(
		const char *label
		);

FL_EXPORT Window fl_winshow(
		Window win
		);

FL_EXPORT Window fl_winopen(
		const char *label
		);

FL_EXPORT void fl_winhide(
		Window win
		);

FL_EXPORT void fl_winclose(
		Window win
		);

FL_EXPORT void fl_winset(
		Window win
		);

FL_EXPORT int fl_winreparent(
		Window win,
		Window new_parent
		);

FL_EXPORT void fl_winfocus(
		Window win
		);

FL_EXPORT Window fl_winget(
		void
		);

FL_EXPORT int fl_iconify(
		Window win
		);


FL_EXPORT void fl_winresize(
		Window win,
		FL_Coord neww,
		FL_Coord newh
		);

FL_EXPORT void fl_winmove(
		Window win,
		int dx,
		int dy
		);

FL_EXPORT void fl_winreshape(
		Window win,
		int dx,
		int dy,
		int w,
		int h
		);

FL_EXPORT void fl_winicon(
		Window win,
		Pixmap p,
		Pixmap m
		);

FL_EXPORT void fl_winbackground(
		Window win,
		FL_COLOR bk
		);

FL_EXPORT void fl_winstepunit(
		Window win,
		FL_Coord dx,
		FL_Coord dy
		);

FL_EXPORT int fl_winisvalid(
		Window win
		);

FL_EXPORT void fl_wintitle(
		Window win,
		const char *title
		);

FL_EXPORT void fl_winicontitle(
		Window win,
		const char *title
		);

FL_EXPORT void fl_winposition(
		FL_Coord x,
		FL_Coord y
		);


#define fl_pref_winposition fl_winposition
#define fl_win_background     fl_winbackground
#define fl_set_winstepunit    fl_winstepunit


FL_EXPORT void fl_winminsize(
		Window win,
		FL_Coord w,
		FL_Coord h
		);

FL_EXPORT void fl_winmaxsize(
		Window win,
		FL_Coord w,
		FL_Coord h
		);

FL_EXPORT void fl_winaspect(
		Window win,
		FL_Coord x,
		FL_Coord y
		);

FL_EXPORT void fl_reset_winconstraints(
		Window win
		);


FL_EXPORT void fl_winsize(
		FL_Coord w,
		FL_Coord h
		);

FL_EXPORT void fl_initial_winsize(
		FL_Coord w,
		FL_Coord h
		);

#define fl_pref_winsize  fl_winsize

FL_EXPORT void fl_initial_winstate(
		int state
		);


FL_EXPORT Colormap fl_create_colormap(
		XVisualInfo *xv,
		int nfill
		);



FL_EXPORT void fl_wingeometry(
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h
		);

#define fl_pref_wingeometry  fl_wingeometry
FL_EXPORT void fl_initial_wingeometry(
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h
		);


FL_EXPORT void fl_noborder(
		void
		);

FL_EXPORT void fl_transient(
		void
		);


FL_EXPORT void fl_get_winsize(
		Window win,
		FL_Coord *w,
		FL_Coord *h
		);

FL_EXPORT void fl_get_winorigin(
		Window win,
		FL_Coord *x,
		FL_Coord *y
		);

FL_EXPORT void fl_get_wingeometry(
		Window win,
		FL_Coord *x,
		FL_Coord *y,
		FL_Coord *w,
		FL_Coord *h
		);


/* for compatibility */
#define fl_get_win_size          fl_get_winsize
#define fl_get_win_origin        fl_get_winorigin
#define fl_get_win_geometry      fl_get_wingeometry
#define fl_initial_winposition   fl_pref_winposition

#define fl_get_display()           fl_display
#define FL_FormDisplay(form)       fl_display
#define FL_ObjectDisplay(object)   fl_display
#define FL_IS_CANVAS(o) (o->objclass == FL_CANVAS||o->objclass == FL_GLCANVAS)

/* the window an object belongs. For drawing */
#define FL_ObjWin(o) (FL_IS_CANVAS(o) ? fl_get_canvas_id(o): o->form->window)

FL_EXPORT Window fl_get_real_object_window(
		FL_OBJECT *ob
		);


#define FL_OBJECT_WID  FL_ObjWin

/*  all registerable events, including Client Message */
#define FL_ALL_EVENT  (KeyPressMask|KeyReleaseMask|      \
                      ButtonPressMask|ButtonReleaseMask|\
                      EnterWindowMask|LeaveWindowMask|    \
                      ButtonMotionMask|PointerMotionMask)

/* Timer related */

#define FL_TIMER_EVENT 0x40000000L


FL_EXPORT int fl_XNextEvent(
		XEvent *xev
		);

FL_EXPORT int fl_XPeekEvent(
		XEvent *xev
		);

FL_EXPORT int fl_XEventsQueued(
		int mode
		);

FL_EXPORT void fl_XPutBackEvent(
		XEvent *xev
		);

FL_EXPORT const XEvent *fl_last_event(
		void
		);


typedef int (*FL_APPEVENT_CB) (XEvent *, void *);
FL_EXPORT FL_APPEVENT_CB fl_set_event_callback(
		FL_APPEVENT_CB callback,
		void *user_data
		);

FL_EXPORT FL_APPEVENT_CB fl_set_idle_callback(
		FL_APPEVENT_CB callback,
		void *user_data
		);

FL_EXPORT long fl_addto_selected_xevent(
		Window win,
		long mask
		);

FL_EXPORT long fl_remove_selected_xevent(
		Window win,
		long mask
		);

#define fl_add_selected_xevent  fl_addto_selected_xevent
FL_EXPORT void fl_set_idle_delta(
		long delta
		);



/*
 * Group some WM stuff into a structure for easy maintainance
 */
enum
{
    FL_WM_SHIFT = 1, FL_WM_NORMAL = 2
};

typedef struct
{
    int rpx, rpy;		/* reparenting offset for full border */
    int trpx, trpy;		/* reparenting offset for transient   */
    int bw;			/* additional border                  */
    int rep_method;		/* 1 for shifting, 2 for normal       */
    unsigned pos_request;	/* USPOSITION or PPOSITION            */
}
FL_WM_STUFF;

FL_EXPORT FL_APPEVENT_CB fl_add_event_callback(
		Window win,
		int ev,
		FL_APPEVENT_CB wincb,
		void *user_data
		);


FL_EXPORT void fl_remove_event_callback(
		Window win,
		int ev
		);

FL_EXPORT void fl_activate_event_callbacks(
		Window win
		);


FL_EXPORT XEvent *fl_print_xevent_name(
		const char *where,
		const XEvent *xev
		);



#define metakey_down(mask)     ((mask) & Mod1Mask)
#define shiftkey_down(mask)    ((mask) & ShiftMask)
#define controlkey_down(mask)  ((mask) & ControlMask)
#define button_down(mask)      (((mask) & Button1Mask) || \
                               ((mask) & Button2Mask) || \
			       ((mask) & Button3Mask))
#define fl_keypressed          fl_keysym_pressed

/****************** Resources ***************/

/* bool is int. */
typedef enum
{
    FL_NONE, FL_SHORT = 10, FL_BOOL, FL_INT, FL_LONG, FL_FLOAT, FL_STRING
}
FL_RTYPE;

typedef struct
{
    const char *res_name;	/* resource name                        */
    const char *res_class;	/* resource class                       */
    FL_RTYPE type;		/* FL_INT, FL_FLOAT, FL_BOOL,FL_STRING  */
    void *var;			/* address for the variable             */
    const char *defval;		/* default setting in string form       */
    int nbytes;			/* used only for strings                */
}
FL_RESOURCE;

#define FL_resource FL_RESOURCE

#define FL_CMD_OPT   XrmOptionDescRec

FL_EXPORT Display *fl_initialize(
		int *na,
		char *arg[],
		const char *appclass,
		XrmOptionDescList appopt,
		int nappopt
		);

FL_EXPORT Display *fl_init(
		void
		);


FL_EXPORT void fl_finish(
		void
		);


FL_EXPORT const char *fl_get_resource(
		const char *rname,
		const char *cname,
		FL_RTYPE dtype,
		const char *defval,
		void *val,
		int size
		);

FL_EXPORT void fl_set_resource(
		const char *str,
		const char *val
		);


FL_EXPORT void fl_get_app_resources(
		FL_RESOURCE *appresource,
		int n
		);

FL_EXPORT void fl_set_graphics_mode(
		int mode,
		int doublebuf
		);

FL_EXPORT void fl_set_visualID(
		long id
		);

FL_EXPORT int fl_keysym_pressed(
		KeySym k
		);


#define buttonLabelSize  buttonFontSize
#define sliderLabelSize  sliderFontSize
#define inputLabelSize   inputFontSize

/* All Form control variables. Named closely as its resource name */
typedef struct
{
    float rgamma, ggamma, bgamma;
    int debug, sync;
    int depth, vclass, doubleBuffer;
    int ulPropWidth, ulThickness;	/* underline stuff       */
    int buttonFontSize;
    int sliderFontSize;
    int inputFontSize;
    int browserFontSize;
    int menuFontSize;
    int choiceFontSize;
    int labelFontSize;		/* all other labels fonts */
    int pupFontSize, pupFontStyle;	/* font for pop-up menus  */
    int privateColormap;
    int sharedColormap;
    int standardColormap;
    int scrollbarType;
    int backingStore;
    int coordUnit;
    int borderWidth;
    int safe;
    char *rgbfile;		/* where RGB file is     */
    char vname[24];
}
FL_IOPT;

#define FL_SBIT(n)   (1<<(n))

#define FL_PDButtonLabelSize FL_PDButtonFontSize
#define FL_PDSliderLabelSize FL_PDSliderFontSize
#define FL_PDInputLabelSize  FL_PDInputFontSize

/* program default masks */
enum
{
    FL_PDDepth = FL_SBIT(1),
    FL_PDClass = FL_SBIT(2),
    FL_PDDouble = FL_SBIT(3),
    FL_PDSync = FL_SBIT(4),
    FL_PDPrivateMap = FL_SBIT(5),
    FL_PDScrollbarType = FL_SBIT(6),
    FL_PDPupFontSize = FL_SBIT(7),
    FL_PDButtonFontSize = FL_SBIT(8),
    FL_PDInputFontSize = FL_SBIT(9),
    FL_PDSliderFontSize = FL_SBIT(10),
    FL_PDVisual = FL_SBIT(11),
    FL_PDULThickness = FL_SBIT(12),
    FL_PDULPropWidth = FL_SBIT(13),
    FL_PDBS = FL_SBIT(14),
    FL_PDCoordUnit = FL_SBIT(15),
    FL_PDDebug = FL_SBIT(16),
    FL_PDSharedMap = FL_SBIT(17),
    FL_PDStandardMap = FL_SBIT(18),
    FL_PDBorderWidth = FL_SBIT(19),
    FL_PDSafe = FL_SBIT(20),
    FL_PDMenuFontSize = FL_SBIT(21),
    FL_PDBrowserFontSize = FL_SBIT(22),
    FL_PDChoiceFontSize = FL_SBIT(23),
    FL_PDLabelFontSize = FL_SBIT(24)
};

#define FL_PDButtonLabel   FL_PDButtonLabelSize

FL_EXPORT void fl_set_defaults(
		unsigned long mask,
		FL_IOPT *cntl
		);

FL_EXPORT void fl_set_tabstop(
		const char *s
		);

FL_EXPORT void fl_get_defaults(
		FL_IOPT *cntl
		);

FL_EXPORT int fl_get_visual_depth(
		void
		);

FL_EXPORT const char *fl_vclass_name(
		int n
		);

FL_EXPORT int fl_vclass_val(
		const char *v
		);

FL_EXPORT void fl_set_ul_property(
		int prop,
		int thickness
		);

FL_EXPORT void fl_set_clipping(
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h
		);

FL_EXPORT void fl_set_gc_clipping(
		GC gc,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h
		);

FL_EXPORT void fl_unset_gc_clipping(
		GC gc
		);

FL_EXPORT void fl_set_clippings(
		FL_RECT *xrect,
		int n
		);

FL_EXPORT void fl_unset_clipping(
		void
		);

FL_EXPORT void fl_set_text_clipping(
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h
		);

FL_EXPORT void fl_unset_text_clipping(
		void
		);


/* how we pack and unpack colors */
#ifndef FL_PCBITS
typedef unsigned char FL_PCTYPE;	/* primary color type */
#define FL_PCBITS          8	/* primary color bits */
#define FL_PCMAX          ((1<<FL_PCBITS)-1)
#define FL_PCCLAMP(a)     ((a)>(FL_PCMAX) ? (FL_PCMAX):((a) < 0 ? 0:(a)))
typedef unsigned int       FL_PACKED4;
#define FL_PACKED          FL_PACKED4

#define FL_RMASK          0x000000ff
#define FL_RSHIFT         0
#define FL_GMASK          0x0000ff00
#define FL_GSHIFT         8
#define FL_BMASK          0x00ff0000
#define FL_BSHIFT         16
#define FL_AMASK          0xff000000
#define FL_ASHIFT         24
/* if PCBITS is not 8, we need to apply the RGBmask */
#define FL_GETR(packed)   ((packed)&FL_RMASK)
#define FL_GETG(packed)   (((packed)>>FL_GSHIFT)&FL_PCMAX)
#define FL_GETB(packed)   (((packed)>>FL_BSHIFT)&FL_PCMAX)
#define FL_GETA(packed)   (((packed)>>FL_ASHIFT)&FL_PCMAX)

#define FL_PACK3(r,g,b)   (((r)<<FL_RSHIFT)|((g)<<FL_GSHIFT)|((b)<<FL_BSHIFT))
#define FL_PACK            FL_PACK3
#define FL_PACK4(r,g,b,a) (FL_PACK3(r,g,b)|((a)<<FL_ASHIFT))

#define FL_UNPACK(p,r,g,b) do {r=FL_GETR(p);g=FL_GETG(p),b=FL_GETB(p);} while(0)
#define FL_UNPACK3         FL_UNPACK
#define FL_UNPACK4(p,r,g,b,a) do {FL_UNPACK3(p,r,g,b);a=FL_GETA(p);} while(0) 
#endif

typedef struct
{
   unsigned int rshift, rmask, rbits;
   unsigned int gshift, gmask, gbits;
   unsigned int bshift, bmask, bbits;
   int bits_per_rgb;
   int colormap_size;
   int reserved[2];
} FL_RGB2PIXEL_;

#define FL_RGB2PIXEL  FL_RGB2PIXEL_

#endif 

/*
 * $Id: bitmap.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *   Object Class: Bitmap
 */
#ifndef FL_BITMAP_H
#define FL_BITMAP_H

#define    FL_NORMAL_BITMAP      0

/***** Defaults *****/

#define FL_BITMAP_BOXTYPE	FL_NO_BOX
#define FL_BITMAP_COL1		FL_COL1		/* background of bitmap */
#define FL_BITMAP_COL2		FL_COL1		/* not used currently   */
#define FL_BITMAP_LCOL		FL_LCOL		/* foreground of bitmap */
#define FL_BITMAP_ALIGN		FL_ALIGN_BOTTOM

/***** Routines *****/
FL_EXPORT FL_OBJECT *fl_create_bitmap(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_add_bitmap(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT void fl_set_bitmap_data(
		FL_OBJECT *ob,
		int w,
		int h,
		unsigned char *data
		);

FL_EXPORT void fl_set_bitmap_file(
		FL_OBJECT *ob,
		const char *fname
		);

FL_EXPORT Pixmap fl_read_bitmapfile(
		Window win,
		const char *file,
		unsigned int *w,
		unsigned int *h,
		int *hotx,
		int *hoty
		);


#define fl_create_from_bitmapdata(win, data, w, h)\
                   XCreateBitmapFromData(fl_get_display(), win, \
                   (char *)data, w, h)

/* for compatibility */
#define fl_set_bitmap_datafile fl_set_bitmap_file


/* PIXMAP stuff */

#define FL_NORMAL_PIXMAP   0

FL_EXPORT FL_OBJECT *fl_create_pixmap(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_add_pixmap(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT void fl_set_pixmap_data(
		FL_OBJECT *ob,
		char **bits
		);

FL_EXPORT void fl_set_pixmap_file(
		FL_OBJECT *ob,
		const char *fname
		);

FL_EXPORT void fl_set_pixmap_align(
		FL_OBJECT *ob,
		int align,
		int xmargin,
		int ymargin
		);

FL_EXPORT void fl_set_pixmap_pixmap(
		FL_OBJECT *ob,
		Pixmap id,
		Pixmap mask
		);

FL_EXPORT void fl_set_pixmap_colorcloseness(
		int red,
		int green,
		int blue
		);

FL_EXPORT void fl_free_pixmap_pixmap(
		FL_OBJECT *ob
		);

FL_EXPORT Pixmap fl_get_pixmap_pixmap(
		FL_OBJECT *ob,
		Pixmap *p,
		Pixmap *m
		);


FL_EXPORT Pixmap fl_read_pixmapfile(
		Window win,
		const char *file,
		unsigned int *w,
		unsigned int *h,
		Pixmap *shape_mask,
		int *hotx,
		int *hoty,
		FL_COLOR tran
		);

FL_EXPORT Pixmap fl_create_from_pixmapdata(
		Window win,
		char **data,
		unsigned *w,
		unsigned *h,
		Pixmap *sm,
		int *hotx,
		int *hoty,
		FL_COLOR tran
		);

#define fl_free_pixmap(id)  do {if(id != None) XFreePixmap(fl_display, id);}while(0)

#endif

/*
 * $Id: box.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 */
#ifndef FL_BOX_H
#define FL_BOX_H


/* type already defined in Basic.h */


FL_EXPORT FL_OBJECT *fl_create_box(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT FL_OBJECT *fl_add_box(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


#endif

/*
 * $Id: browser.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *  Object class Browser
 */
#ifndef FL_BROWSER_H
#define FL_BROWSER_H


/***** Types    *****/
typedef enum
{
    FL_NORMAL_BROWSER,
    FL_SELECT_BROWSER,
    FL_HOLD_BROWSER,
    FL_MULTI_BROWSER
}
FL_BROWSER_TYPE;

/***** Defaults *****/

#define FL_BROWSER_BOXTYPE	FL_DOWN_BOX
#define FL_BROWSER_COL1		FL_COL1
#define FL_BROWSER_COL2		FL_YELLOW
#define FL_BROWSER_LCOL		FL_LCOL
#define FL_BROWSER_ALIGN	FL_ALIGN_BOTTOM

/***** Others   *****/

#define FL_BROWSER_SLCOL	FL_COL1
#define FL_BROWSER_LINELENGTH	2048
#define FL_BROWSER_FONTSIZE     FL_SMALL_FONT

/***** Routines *****/

FL_EXPORT FL_OBJECT *fl_create_browser(
		int type,
		int x,
		int y,
		int w,
		int h,
		const char *l
		);

FL_EXPORT FL_OBJECT *fl_add_browser(
		int type,
		int x,
		int y,
		int w,
		int h,
		const char *l
		);

FL_EXPORT void fl_clear_browser(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_add_browser_line(
		FL_OBJECT *ob,
		const char *newtext
		);

FL_EXPORT void fl_addto_browser(
		FL_OBJECT *ob,
		const char *newtext
		);

FL_EXPORT void fl_addto_browser_chars(
		FL_OBJECT *ob,
		const char *str
		);

#define fl_append_browser  fl_addto_browser_chars
FL_EXPORT void fl_insert_browser_line(
		FL_OBJECT *ob,
		int linenumb,
		const char *newtext
		);

FL_EXPORT void fl_delete_browser_line(
		FL_OBJECT *ob,
		int linenumb
		);

FL_EXPORT void fl_replace_browser_line(
		FL_OBJECT *ob,
		int linenumb,
		const char *newtext
		);

FL_EXPORT const char *fl_get_browser_line(
		FL_OBJECT *ob,
		int linenumb
		);

FL_EXPORT int fl_load_browser(
		FL_OBJECT *ob,
		const char *f
		);


FL_EXPORT void fl_select_browser_line(
		FL_OBJECT *ob,
		int line
		);

FL_EXPORT void fl_deselect_browser_line(
		FL_OBJECT *ob,
		int line
		);

FL_EXPORT void fl_deselect_browser(
		FL_OBJECT *ob
		);

FL_EXPORT int fl_isselected_browser_line(
		FL_OBJECT *ob,
		int line
		);


FL_EXPORT int fl_get_browser_topline(
		FL_OBJECT *ob
		);

FL_EXPORT int fl_get_browser(
		FL_OBJECT *ob
		);

FL_EXPORT int fl_get_browser_maxline(
		FL_OBJECT *ob
		);

FL_EXPORT int fl_get_browser_screenlines(
		FL_OBJECT *ob
		);


FL_EXPORT void fl_set_browser_topline(
		FL_OBJECT *ob,
		int topline
		);

FL_EXPORT void fl_set_browser_fontsize(
		FL_OBJECT *ob,
		int size
		);

FL_EXPORT void fl_set_browser_fontstyle(
		FL_OBJECT *ob,
		int style
		);

FL_EXPORT void fl_set_browser_specialkey(
		FL_OBJECT *ob,
		int specialkey
		);

FL_EXPORT void fl_set_browser_vscrollbar(
		FL_OBJECT *ob,
		int on
		);

FL_EXPORT void fl_set_browser_hscrollbar(
		FL_OBJECT *ob,
		int on
		);

FL_EXPORT void fl_set_browser_line_selectable(
		FL_OBJECT *ob,
		int line,
		int flag
		);

FL_EXPORT void fl_get_browser_dimension(
		FL_OBJECT *ob,
		FL_Coord *x,
		FL_Coord *y,
		FL_Coord *w,
		FL_Coord *h
		);

FL_EXPORT void fl_set_browser_dblclick_callback(
		FL_OBJECT *ob,
		FL_CALLBACKPTR cb,
		long a
		);


FL_EXPORT void fl_set_browser_xoffset(
		FL_OBJECT *ob,
		FL_Coord npixels
		);

FL_EXPORT FL_Coord fl_get_browser_xoffset(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_set_browser_scrollbarsize(
		FL_OBJECT *ob,
		int hh,
		int vw
		);

FL_EXPORT void fl_show_browser_line(
		FL_OBJECT *ob,
		int j
		);

FL_EXPORT int fl_set_default_browser_maxlinelength(
		int n
		);


#ifndef FL_BROWSER_SCROLL_CALLBACKt
#define FL_BROWSER_SCROLL_CALLBACKt
typedef void (*FL_BROWSER_SCROLL_CALLBACK)(FL_OBJECT *ob, int, void *);
#endif

FL_EXPORT void fl_set_browser_hscroll_callback(
		FL_OBJECT *ob,
		FL_BROWSER_SCROLL_CALLBACK cb,
		void *data
		);

FL_EXPORT void fl_set_browser_vscroll_callback(
		FL_OBJECT *ob,
		FL_BROWSER_SCROLL_CALLBACK cb,
		void *data
		);


FL_EXPORT FL_BROWSER_SCROLL_CALLBACK fl_get_browser_hscroll_callback(
		FL_OBJECT *ob
		);

FL_EXPORT FL_BROWSER_SCROLL_CALLBACK fl_get_browser_vscroll_callback(
		FL_OBJECT *ob
		);

#endif

/*
 * $Id: button.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 * All Buttons: regular button, light button and round button
 *
 */

#ifndef FL_BUTTON_H
#define FL_BUTTON_H


typedef enum
{
    FL_NORMAL_BUTTON,
    FL_PUSH_BUTTON,
    FL_RADIO_BUTTON,
    FL_HIDDEN_BUTTON,
    FL_TOUCH_BUTTON,
    FL_INOUT_BUTTON,
    FL_RETURN_BUTTON,
    FL_HIDDEN_RET_BUTTON,
    FL_MENU_BUTTON
}
FL_BUTTON_TYPE;

#define FL_TOGGLE_BUTTON    FL_PUSH_BUTTON

typedef struct
{
    Pixmap pixmap, mask;
    unsigned bits_w, bits_h;
    int val;			/* whether on */
    int mousebut;		/* mouse button that caused the push     */
    int timdel;			/* time since last touch (TOUCH buttons) */
    int event;			/* what event triggers redraw            */
    long cspecl;		/* reserved for class specfic stuff      */
    void *cspecv;		/* misc. things                          */
    char *filename;
    Pixmap focus_pixmap, focus_mask;
    char *focus_filename;
    long reserverd[2];
}
FL_BUTTON_SPEC;

#define FL_BUTTON_STRUCT FL_BUTTON_SPEC

typedef void (*FL_DrawButton) (FL_OBJECT *);
typedef void (*FL_CleanupButton) (FL_BUTTON_STRUCT *);

#define FL_DRAWBUTTON      FL_DrawButton
#define FL_CLEANUPBUTTON   FL_CleanupButton

/*
 *  normal button default
 */
#define FL_BUTTON_BOXTYPE	FL_UP_BOX
#define FL_BUTTON_COL1		FL_COL1
#define FL_BUTTON_COL2		FL_COL1
#define FL_BUTTON_LCOL		FL_LCOL
#define FL_BUTTON_ALIGN		FL_ALIGN_CENTER
#define FL_BUTTON_MCOL1		FL_MCOL
#define FL_BUTTON_MCOL2		FL_MCOL
#define FL_BUTTON_BW		FL_BOUND_WIDTH

/*
 *  light button defaults
 */
#define FL_LIGHTBUTTON_BOXTYPE	FL_UP_BOX
#define FL_LIGHTBUTTON_COL1	FL_COL1
#define FL_LIGHTBUTTON_COL2	FL_YELLOW
#define FL_LIGHTBUTTON_LCOL	FL_LCOL
#define FL_LIGHTBUTTON_ALIGN	FL_ALIGN_CENTER
#define FL_LIGHTBUTTON_TOPCOL	FL_COL1
#define FL_LIGHTBUTTON_MCOL	FL_MCOL
#define FL_LIGHTBUTTON_MINSIZE	(FL_Coord)12

/** round button defaults ***/

#define FL_ROUNDBUTTON_BOXTYPE FL_NO_BOX
#define FL_ROUNDBUTTON_COL1	 FL_MCOL
#define FL_ROUNDBUTTON_COL2	 FL_YELLOW
#define FL_ROUNDBUTTON_LCOL	 FL_LCOL
#define FL_ROUNDBUTTON_ALIGN	 FL_ALIGN_CENTER
#define FL_ROUNDBUTTON_TOPCOL	 FL_COL1
#define FL_ROUNDBUTTON_MCOL	 FL_MCOL

/* round3d button defaults   */

#define FL_ROUND3DBUTTON_BOXTYPE FL_NO_BOX
#define FL_ROUND3DBUTTON_COL1	 FL_COL1
#define FL_ROUND3DBUTTON_COL2	 FL_BLACK
#define FL_ROUND3DBUTTON_LCOL	 FL_LCOL
#define FL_ROUND3DBUTTON_ALIGN	 FL_ALIGN_CENTER
#define FL_ROUND3DBUTTON_TOPCOL	 FL_COL1
#define FL_ROUND3DBUTTON_MCOL	 FL_MCOL

/** check button defaults ***/

#define FL_CHECKBUTTON_BOXTYPE	FL_NO_BOX
#define FL_CHECKBUTTON_COL1	FL_COL1
#define FL_CHECKBUTTON_COL2	FL_YELLOW
#define FL_CHECKBUTTON_LCOL	FL_LCOL
#define FL_CHECKBUTTON_ALIGN	FL_ALIGN_CENTER

#define FL_CHECKBUTTON_TOPCOL	FL_COL1
#define FL_CHECKBUTTON_MCOL	FL_MCOL

/** bitmap button defaults **/
#define FL_BITMAPBUTTON_BOXTYPE	FL_UP_BOX
#define FL_BITMAPBUTTON_COL1	FL_COL1		/* bitmap background  */
#define FL_BITMAPBUTTON_COL2	FL_BLUE		/* "focus" color       */
#define FL_BITMAPBUTTON_LCOL	FL_LCOL		/* bitmap foreground   */
#define FL_BITMAPBUTTON_ALIGN	FL_ALIGN_BOTTOM

/** bitmap button defaults **/
#define FL_PIXMAPBUTTON_BOXTYPE	FL_UP_BOX
#define FL_PIXMAPBUTTON_COL1	FL_COL1		/* box col    */
#define FL_PIXMAPBUTTON_COL2	FL_YELLOW	/* bound rect */
#define FL_PIXMAPBUTTON_LCOL	FL_LCOL
#define FL_PIXMAPBUTTON_ALIGN	FL_ALIGN_BOTTOM

/***** Routines *****/

FL_EXPORT FL_OBJECT *fl_create_button(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_create_roundbutton(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_create_round3dbutton(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_create_lightbutton(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_create_checkbutton(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_create_bitmapbutton(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_create_pixmapbutton(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_create_scrollbutton(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_create_labelbutton(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT FL_OBJECT *fl_add_roundbutton(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_add_round3dbutton(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_add_lightbutton(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_add_checkbutton(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_add_button(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT FL_OBJECT *fl_add_bitmapbutton(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_add_scrollbutton(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_add_labelbutton(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT void fl_set_bitmapbutton_file(
		FL_OBJECT *ob,
		const char *file
		);

FL_EXPORT void fl_set_bitmapbutton_data(
		FL_OBJECT *ob,
		int w,
		int h,
		unsigned char *bits
		);


#define fl_set_bitmapbutton_datafile  fl_set_bitmapbutton_file

FL_EXPORT FL_OBJECT *fl_add_pixmapbutton(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


#define fl_set_pixmapbutton_data       fl_set_pixmap_data
#define fl_set_pixmapbutton_file       fl_set_pixmap_file
#define fl_set_pixmapbutton_pixmap     fl_set_pixmap_pixmap
#define fl_get_pixmapbutton_pixmap     fl_get_pixmap_pixmap
#define fl_set_pixmapbutton_align      fl_set_pixmap_align
#define fl_free_pixmapbutton_pixmap    fl_free_pixmap_pixmap
#define fl_set_pixmapbutton_datafile   fl_set_pixmapbutton_file
#define fl_set_pixmapbutton_show_focus fl_set_pixmapbutton_focus_outline

FL_EXPORT void fl_set_pixmapbutton_focus_outline(
		FL_OBJECT *ob,
		int yes
		);

FL_EXPORT void fl_set_pixmapbutton_focus_data(
		FL_OBJECT *ob,
		char **bits
		);

FL_EXPORT void fl_set_pixmapbutton_focus_file(
		FL_OBJECT *ob,
		const char *fname
		);

FL_EXPORT void fl_set_pixmapbutton_focus_pixmap(
		FL_OBJECT *ob,
		Pixmap id,
		Pixmap mask
		);


FL_EXPORT int fl_get_button(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_set_button(
		FL_OBJECT *ob,
		int pushed
		);

FL_EXPORT int fl_get_button_numb(
		FL_OBJECT *ob
		);


#define fl_set_button_shortcut  fl_set_object_shortcut

FL_EXPORT FL_OBJECT *fl_create_generic_button(
		int objclass,
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT void fl_add_button_class(
		int bclass,
		FL_DrawButton drawit,
		FL_CleanupButton cleanup
		);



#endif 

/*
 * $Id: canvas.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 * Header for FL_CANVAS
 *
 */

#ifndef FL_CANVAS_H_
#define FL_CANVAS_H_

typedef enum
{
    FL_NORMAL_CANVAS,
    FL_SCROLLED_CANVAS
}
FL_CANVAS_TYPE;


typedef int (*FL_HANDLE_CANVAS) (FL_OBJECT * ob,
				 Window,
				 int, int,
				 XEvent *, void *);

typedef int (*FL_MODIFY_CANVAS_PROP) (FL_OBJECT *);

/******************** Default *********************/

#define FL_CANVAS_BOXTYPE   FL_DOWN_BOX		/* really the decoration
						   frame */
#define FL_CANVAS_ALIGN     FL_ALIGN_TOP


/************ Interfaces    ************************/


FL_EXPORT FL_OBJECT *fl_create_generic_canvas(
		int canvas_class,
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT FL_OBJECT *fl_add_canvas(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT FL_OBJECT *fl_create_canvas(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT FL_OBJECT *fl_create_mesacanvas(int, FL_Coord, FL_Coord, FL_Coord,
					  FL_Coord, const char *);


FL_EXPORT FL_OBJECT *fl_add_mesacanvas(int, FL_Coord, FL_Coord, FL_Coord,
				       FL_Coord, const char *);



/* backward compatibility */
#define fl_set_canvas_decoration fl_set_object_boxtype

FL_EXPORT void fl_set_canvas_colormap(
		FL_OBJECT *ob,
		Colormap colormap
		);

FL_EXPORT void fl_set_canvas_visual(
		FL_OBJECT *obj,
		Visual *vi
		);

FL_EXPORT void fl_set_canvas_depth(
		FL_OBJECT *obj,
		int depth
		);

FL_EXPORT void fl_set_canvas_attributes(
		FL_OBJECT *ob,
		unsigned mask,
		XSetWindowAttributes *xswa
		);


FL_EXPORT FL_HANDLE_CANVAS fl_add_canvas_handler(
		FL_OBJECT *ob,
		int ev,
		FL_HANDLE_CANVAS h,
		void *udata
		);


FL_EXPORT Window fl_get_canvas_id(
		FL_OBJECT *ob
		);

FL_EXPORT Colormap fl_get_canvas_colormap(
		FL_OBJECT *ob
		);

FL_EXPORT int fl_get_canvas_depth(
		FL_OBJECT *obj
		);

FL_EXPORT void fl_remove_canvas_handler(
		FL_OBJECT *ob,
		int ev,
		FL_HANDLE_CANVAS h
		);

FL_EXPORT void fl_hide_canvas(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_share_canvas_colormap(
		FL_OBJECT *ob,
		Colormap colormap
		);

FL_EXPORT void fl_clear_canvas(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_modify_canvas_prop(
		FL_OBJECT *obj,
		FL_MODIFY_CANVAS_PROP init,
		FL_MODIFY_CANVAS_PROP activate,
		FL_MODIFY_CANVAS_PROP cleanup
		);


FL_EXPORT void fl_canvas_yield_to_shortcut(
		FL_OBJECT *ob,
		int yes
		);

#if defined(__GLX_glx_h__) || defined(GLX_H)
#include <X11/glcanvas.h>
#endif

#endif
/****FL_CANVAS_H ******/

/*
 * $Id: chart.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 * Object Class: Chart
 *
 */
#ifndef FL_CHART_H
#define FL_CHART_H

typedef enum
{
    FL_BAR_CHART,
    FL_HORBAR_CHART,
    FL_LINE_CHART,
    FL_FILL_CHART,
    FL_SPIKE_CHART,
    FL_PIE_CHART,
    FL_SPECIALPIE_CHART
}
FL_CHART_TYPE;

#define FL_FILLED_CHART  FL_FILL_CHART	/* compatibility */

/***** Defaults *****/

#define FL_CHART_BOXTYPE	FL_BORDER_BOX
#define FL_CHART_COL1		FL_COL1
#define FL_CHART_LCOL		FL_LCOL
#define FL_CHART_ALIGN		FL_ALIGN_BOTTOM

/***** Others   *****/

#define FL_CHART_MAX		2048

/***** Routines *****/

FL_EXPORT FL_OBJECT *fl_create_chart(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_add_chart(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT void fl_clear_chart(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_add_chart_value(
		FL_OBJECT *ob,
		double val,
		const char *str,
		int col
		);

FL_EXPORT void fl_insert_chart_value(
		FL_OBJECT *ob,
		int indx,
		double val,
		const char *str,
		int col
		);

FL_EXPORT void fl_replace_chart_value(
		FL_OBJECT *ob,
		int indx,
		double val,
		const char *str,
		int col
		);

FL_EXPORT void fl_set_chart_bounds(
		FL_OBJECT *ob,
		double min,
		double max
		);

FL_EXPORT void fl_get_chart_bounds(
		FL_OBJECT *ob,
		double *min,
		double *max
		);

FL_EXPORT int fl_set_chart_maxnumb(
		FL_OBJECT *ob,
		int maxnumb
		);

FL_EXPORT void fl_set_chart_autosize(
		FL_OBJECT *ob,
		int autosize
		);

FL_EXPORT void fl_set_chart_lstyle(
		FL_OBJECT *ob,
		int lstyle
		);

FL_EXPORT void fl_set_chart_lsize(
		FL_OBJECT *ob,
		int lsize
		);

FL_EXPORT void fl_set_chart_lcolor(
		FL_OBJECT *ob,
		FL_COLOR lcol
		);

FL_EXPORT void fl_set_chart_baseline(
		FL_OBJECT *ob,
		int iYesNo
		);

#define fl_set_chart_lcol   fl_set_chart_lcolor

#endif

/*
 * $Id: choice.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 */
#ifndef FL_CHOICE_H
#define FL_CHOICE_H


typedef enum
{
    FL_NORMAL_CHOICE,
    FL_NORMAL_CHOICE2,
    FL_DROPLIST_CHOICE,
    FL_BROWSER_CHOICE
}
FL_CHOICE_TYPE;

#define  FL_SIMPLE_CHOICE  FL_NORMAL_CHOICE

/***** Defaults *****/

#define FL_CHOICE_BOXTYPE	FL_ROUNDED_BOX
#define FL_CHOICE_COL1		FL_COL1
#define FL_CHOICE_COL2		FL_LCOL
#define FL_CHOICE_LCOL		FL_LCOL
#define FL_CHOICE_ALIGN		FL_ALIGN_LEFT

/***** Others   *****/

#define FL_CHOICE_MCOL		FL_MCOL
#define FL_CHOICE_MAXITEMS	128

/***** Routines *****/

FL_EXPORT FL_OBJECT *fl_create_choice(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT FL_OBJECT *fl_add_choice(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *l
		);

FL_EXPORT void fl_clear_choice(
		FL_OBJECT *ob
		);

FL_EXPORT int fl_addto_choice(
		FL_OBJECT *ob,
		const char *str
		);

FL_EXPORT void fl_replace_choice(
		FL_OBJECT *ob,
		int numb,
		const char *str
		);

FL_EXPORT void fl_delete_choice(
		FL_OBJECT *ob,
		int numb
		);

FL_EXPORT void fl_set_choice(
		FL_OBJECT *ob,
		int choice
		);

FL_EXPORT void fl_set_choice_text(
		FL_OBJECT *ob,
		const char *txt
		);

FL_EXPORT int fl_get_choice(
		FL_OBJECT *ob
		);

FL_EXPORT const char *fl_get_choice_item_text(
		FL_OBJECT *ob,
		int n
		);

FL_EXPORT int fl_get_choice_maxitems(
		FL_OBJECT *ob
		);

FL_EXPORT const char *fl_get_choice_text(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_set_choice_fontsize(
		FL_OBJECT *ob,
		int size
		);

FL_EXPORT void fl_set_choice_fontstyle(
		FL_OBJECT *ob,
		int style
		);

FL_EXPORT void fl_set_choice_align(
		FL_OBJECT *ob,
		int align
		);

FL_EXPORT void fl_set_choice_item_mode(
		FL_OBJECT *ob,
		int item,
		unsigned mode
		);

FL_EXPORT void fl_set_choice_item_shortcut(
		FL_OBJECT *ob,
		int item,
		const char *sc
		);

FL_EXPORT int fl_set_choice_entries(
		FL_OBJECT *ob,
		FL_PUP_ENTRY *ent
		);

FL_EXPORT int fl_set_choice_notitle(
		FL_OBJECT *ob,
		int n
		);


#endif 

/*
 * $Id: clipbd.h,v 0.89 1999/07/17 03:17:52 zhao Beta $
 *
 * prototypes for clipboard stuff
 *
 */
#ifndef FL_CLIPBD_H
#define FL_CLIPBD_H

typedef Atom FL_CPTYPE;

typedef int (*FL_LOSE_SELECTION_CB)(FL_OBJECT *ob, long type);
typedef int (*FL_SELECTION_CB)(FL_OBJECT *ob, long type,
			     const void *data, long size);

#define FL_SELECTION_CALLBACK        FL_SELECTION_CB
#define FL_LOSE_SELECTION_CALLBACK   FL_LOSE_SELECTION_CB

FL_EXPORT int fl_stuff_clipboard(
		FL_OBJECT *ob,
		long type,
		const void *data,
		long size,
		FL_LOSE_SELECTION_CB lose_callback
		);


FL_EXPORT int fl_request_clipboard(
		FL_OBJECT *ob,
		long type,
		FL_SELECTION_CB got_it_callback
		);


#endif /* !def CLIPBD_H */


/*
 * $Id: clock.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 */
#ifndef FL_CLOCK_H
#define FL_CLOCK_H

enum
{
    FL_ANALOG_CLOCK,
    FL_DIGITAL_CLOCK
};

#define FL_CLOCK_BOXTYPE   FL_UP_BOX
#define FL_CLOCK_COL1      FL_INACTIVE_COL
#define FL_CLOCK_COL2      FL_BOTTOM_BCOL
#define FL_CLOCK_LCOL      FL_BLACK
#define FL_CLOCK_ALIGN     FL_ALIGN_BOTTOM

#define FL_CLOCK_TOPCOL  FL_COL1

FL_EXPORT FL_OBJECT *fl_create_clock(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *s
		);


FL_EXPORT FL_OBJECT *fl_add_clock(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *s
		);

FL_EXPORT void fl_get_clock(
		FL_OBJECT *ob,
		int *h,
		int *m,
		int *s
		);

FL_EXPORT long fl_set_clock_adjustment(
		FL_OBJECT *ob,
		long offset
		);

FL_EXPORT void fl_set_clock_ampm(
		FL_OBJECT *ob,
		int y
		);


#endif

/*
 * $Id: counter.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 */

#ifndef FL_COUNTER_H
#define FL_COUNTER_H

typedef enum
{
    FL_NORMAL_COUNTER,
    FL_SIMPLE_COUNTER
}
FL_COUNTER_TYPE;

/***** Defaults *****/

#define FL_COUNTER_BOXTYPE	FL_UP_BOX
#define FL_COUNTER_COL1		FL_COL1
#define FL_COUNTER_COL2		FL_BLUE		/* ct label     */
#define FL_COUNTER_LCOL		FL_LCOL		/* ct reporting */
#define FL_COUNTER_ALIGN	FL_ALIGN_BOTTOM

/***** Others   *****/

#define FL_COUNTER_BW		(FL_BOUND_WIDTH-1)

/***** Routines *****/
FL_EXPORT FL_OBJECT *fl_create_counter(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT FL_OBJECT *fl_add_counter(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT void fl_set_counter_value(
		FL_OBJECT *ob,
		double val
		);

FL_EXPORT void fl_set_counter_bounds(
		FL_OBJECT *ob,
		double min,
		double max
		);

FL_EXPORT void fl_set_counter_step(
		FL_OBJECT *ob,
		double s,
		double l
		);

FL_EXPORT void fl_set_counter_precision(
		FL_OBJECT *ob,
		int prec
		);

FL_EXPORT void fl_set_counter_return(
		FL_OBJECT *ob,
		int how
		);

FL_EXPORT double fl_get_counter_value(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_get_counter_bounds(
		FL_OBJECT *ob,
		double *min,
		double *max
		);

FL_EXPORT void fl_get_counter_step(
		FL_OBJECT *ob,
		float *s,
		float *l
		);

FL_EXPORT void fl_set_counter_filter(
		FL_OBJECT *ob,
		FL_VAL_FILTER filter
		);


#endif

/*
 * $Id: cursor.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 * Cursor defs and prototypes
 *
 */

#ifndef FL_CURSOR_H
#define FL_CURSOR_H

#include <X11/cursorfont.h>

enum
{
    FL_DEFAULT_CURSOR = -1,
    FL_INVISIBLE_CURSOR = -2,
    FL_BUSY_CURSOR = XC_watch,
    FL_CROSSHAIR_CURSOR = XC_tcross,
    FL_KILL_CURSOR = XC_pirate,
    FL_NWARROW_CURSOR = XC_top_left_arrow,
    FL_NEARROW_CURSOR = XC_arrow
};

#ifndef XC_invisible
#define XC_invisible   FL_INVISIBLE_CURSOR
#endif

FL_EXPORT void fl_set_cursor(
		Window win,
		int name
		);

FL_EXPORT void fl_set_cursor_color(
		int name,
		FL_COLOR fg,
		FL_COLOR bg
		);

FL_EXPORT int fl_create_bitmap_cursor(
		const char *source,
		const char *mask,
		int w,
		int h,
		int hotx,
		int hoty
		);

FL_EXPORT int fl_create_animated_cursor(
		int *cur_names,
		int timeout
		);


FL_EXPORT Cursor fl_get_cursor_byname(
		int name
		);


#define fl_reset_cursor(win) fl_set_cursor(win, FL_DEFAULT_CURSOR);

#endif

/*
 * $Id: dial.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 */
#ifndef FL_DIAL_H
#define FL_DIAL_H

typedef enum
{
    FL_NORMAL_DIAL,
    FL_LINE_DIAL,
    FL_FILL_DIAL
}
FL_DIAL_TYPE;

enum
{
    FL_DIAL_CW, FL_DIAL_CCW
};

/***** Defaults *****/

#define FL_DIAL_BOXTYPE		FL_FLAT_BOX
#define FL_DIAL_COL1		FL_COL1
#define FL_DIAL_COL2		FL_RIGHT_BCOL
#define FL_DIAL_LCOL		FL_LCOL
#define FL_DIAL_ALIGN		FL_ALIGN_BOTTOM

/***** Others   *****/

#define FL_DIAL_TOPCOL		FL_COL1

/***** Routines *****/

FL_EXPORT FL_OBJECT *fl_create_dial(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_add_dial(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT void fl_set_dial_value(
		FL_OBJECT *ob,
		double val
		);

FL_EXPORT double fl_get_dial_value(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_set_dial_bounds(
		FL_OBJECT *ob,
		double min,
		double max
		);

FL_EXPORT void fl_get_dial_bounds(
		FL_OBJECT *ob,
		double *min,
		double *max
		);


FL_EXPORT void fl_set_dial_step(
		FL_OBJECT *ob,
		double value
		);

FL_EXPORT void fl_set_dial_return(
		FL_OBJECT *ob,
		int value
		);

FL_EXPORT void fl_set_dial_angles(
		FL_OBJECT *ob,
		double amin,
		double amax
		);

FL_EXPORT void fl_set_dial_cross(
		FL_OBJECT *ob,
		int flag
		);

#define fl_set_dial_crossover  fl_set_dial_cross

FL_EXPORT void fl_set_dial_direction(
		FL_OBJECT *ob,
		int dir
		);


#endif

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

/*
 * $Id: flps.h,v 0.89 1999/07/17 03:17:52 zhao Beta $
 */

#ifndef FLPS_H
#define FLPS_H

/* postscript stuff */

enum 
{
   FLPS_AUTO,                  /* switch to landscale if does not fit */
   FLPS_LANDSCAPE,             /* landscape always                    */
   FLPS_PORTRAIT,              /* portrait always                     */
   FLPS_BESTFIT                /* even margins/best fit               */
};

enum
{
  FLPS_BW = -1, FLPS_GRAYSCALE, FLPS_COLOR
};

#define FLPS_MEMBERS                                     \
             int        ps_color;                        \
             int        orientation;                     \
             int        auto_fit;                        \
             int        drawbox;                         \
             int        eps;                             \
             float      xdpi, ydpi;                      \
             float      paper_w, paper_h;                \
             float      gamma;                           \
             const char *tmpdir;                         \
             int        printer_dpi;                     \
             float      hm, vm;                          \
             float      xscale, yscale;                  \
             int        scale_text;                      \
             int        first_page_only;                 \
             int        clip;                            \
             int        reserved[10]             

/* configuration infomation */
typedef struct flps_cntl_ { FLPS_MEMBERS; } FLPS_CONTROL;

FL_EXPORT FLPS_CONTROL *flps_init(
		void
		);

FL_EXPORT int fl_object_ps_dump(
		FL_OBJECT *ob,
		const char *fname
		);


#endif

/*
 * $Id: formbrowser.h,v 0.89 1999/07/17 03:17:52 zhao Beta $
 *
 */
#ifndef FL_FORMBROWSER_H
#define FL_FORMBROWSER_H

enum
{
    FL_NORMAL_FORMBROWSER
};

enum 
{
   FL_SMOOTH_SCROLL,
   FL_JUMP_SCROLL
};

#define   FL_FORMBROWSER_BOXTYPE  FL_DOWN_BOX
#define   FL_FORMBROWSER_COL1     FL_COL1
#define   FL_FORMBROWSER_ALIGN    FL_ALIGN_TOP


FL_EXPORT int fl_addto_formbrowser(
		FL_OBJECT *ob,
		FL_FORM *form
		);

FL_EXPORT FL_FORM *fl_delete_formbrowser_bynumber(
		FL_OBJECT *ob,
		int num
		);

FL_EXPORT int fl_delete_formbrowser(
		FL_OBJECT *ob,
		FL_FORM *candidate_form
		);

FL_EXPORT FL_FORM *fl_replace_formbrowser(
		FL_OBJECT *ob,
		int num,
		FL_FORM *form
		);

FL_EXPORT int fl_insert_formbrowser(
		FL_OBJECT *ob,
		int line,
		FL_FORM *new_form
		);

FL_EXPORT int fl_get_formbrowser_area(
		FL_OBJECT *ob,
		int *x,
		int *y,
		int *w,
		int *h
		);

FL_EXPORT void fl_set_formbrowser_scroll(
		FL_OBJECT *ob,
		int how
		);

FL_EXPORT void fl_set_formbrowser_hscrollbar(
		FL_OBJECT *ob,
		int how
		);

FL_EXPORT void fl_set_formbrowser_vscrollbar(
		FL_OBJECT *ob,
		int how
		);

FL_EXPORT FL_FORM *fl_get_formbrowser_topform(
		FL_OBJECT *ob
		);

FL_EXPORT int fl_set_formbrowser_topform(
		FL_OBJECT *ob,
		FL_FORM *form
		);

FL_EXPORT FL_FORM *fl_set_formbrowser_topform_bynumber(
		FL_OBJECT *ob,
		int n
		);


FL_EXPORT int fl_set_formbrowser_xoffset(
		FL_OBJECT *ob,
		int offset
		);

FL_EXPORT int fl_set_formbrowser_yoffset(
		FL_OBJECT *ob,
		int offset
		);

FL_EXPORT int fl_get_formbrowser_xoffset(
		FL_OBJECT *ob
		);

FL_EXPORT int fl_get_formbrowser_yoffset(
		FL_OBJECT *ob
		);


FL_EXPORT int fl_find_formbrowser_form_number(
		FL_OBJECT *ob,
		FL_FORM *candidate_form
		);


FL_EXPORT FL_OBJECT *fl_add_formbrowser(
		int type,
		int x,
		int y,
		int w,
		int h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_create_formbrowser(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT int fl_get_formbrowser_numforms(
		FL_OBJECT *ob
		);

#define    fl_get_formbrowser_forms fl_get_formbrowser_numforms
FL_EXPORT FL_FORM *fl_get_formbrowser_form(
		FL_OBJECT *ob,
		int n
		);


#endif /* !def TEXT_H } */


/*
 * $Id: frame.h,v 0.11 1995/04/01 00:39:31 zhao Exp $
 *
 */

#ifndef FL_FRAME_H
#define FL_FRAME_H		/* { */

/* types of frames */
enum
{
    FL_NO_FRAME,
    FL_UP_FRAME,
    FL_DOWN_FRAME,
    FL_BORDER_FRAME,
    FL_SHADOW_FRAME,
    FL_ENGRAVED_FRAME,
    FL_ROUNDED_FRAME,
    FL_EMBOSSED_FRAME,
    FL_OVAL_FRAME
};

#define FL_FRAME_COL1   FL_BLACK   /* border color     */ 
#define FL_FRAME_COL2   FL_COL1    /* label background */
#define FL_FRAME_LCOL   FL_BLACK   /* label color      */

FL_EXPORT FL_OBJECT *fl_create_frame(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT FL_OBJECT *fl_add_frame(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


/* labeld frame */
FL_EXPORT FL_OBJECT *fl_create_labelframe(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT FL_OBJECT *fl_add_labelframe(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

#endif /* !def FRAME_H } */


/*
 * $Id: free.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *  Object Class: Free
 */
#ifndef FL_FREE_H
#define FL_FREE_H		/* { */


typedef enum
{
    FL_NORMAL_FREE,
    FL_INACTIVE_FREE,
    FL_INPUT_FREE,
    FL_CONTINUOUS_FREE,
    FL_ALL_FREE
}
FL_FREE_TYPE;

#define FL_SLEEPING_FREE  FL_INACTIVE_FREE

FL_EXPORT FL_OBJECT *fl_create_free(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label,
		FL_HANDLEPTR handle
		);

FL_EXPORT FL_OBJECT *fl_add_free(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label,
		FL_HANDLEPTR handle
		);

#endif /* !def FREE_H } */

/*
 * $Id: goodies.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 */
#ifndef FL_GOODIES_H
#define FL_GOODIES_H		/* { */

/***** Resources and misc. goodie routines ******/
#define FLAlertDismissLabel     "flAlert.dismiss.label"
#define FLAlertTitle            "flAlert.title"

#define FLQuestionYesLabel      "flQuestion.yes.label"
#define FLQuestionNoLabel       "flQuestion.no.label"
#define FLQuestionTitle         "flQuestion.title"

#define FLOKLabel               "flInput.ok.label"
#define FLInputClearLabel       "flInput.clear.label"
#define FLInputCancelLabel      "flInput.cancel.label"
#define FLInputTitle            "flInput.title"

#define FLChoiceTitle           "flChoice.title"

FL_EXPORT void fl_set_goodies_font(
		int style,
		int size
		);


/*********** messages and questions **************/

FL_EXPORT void fl_show_message(
		const char *s1,
		const char *s2,
		const char *s3
		);

FL_EXPORT void fl_show_messages(
		const char *str
		);

FL_EXPORT void fl_hide_message(
		void
		);


FL_EXPORT int fl_show_question(
		const char *str,
		int ans
		);

FL_EXPORT void fl_hide_question(
		void
		);


FL_EXPORT void fl_show_alert(
		const char *title,
		const char *str1,
		const char *str2,
		int c
		);

FL_EXPORT void fl_hide_alert(
		void
		);


FL_EXPORT const char *fl_show_input(
		const char *str1,
		const char *defstr
		);

FL_EXPORT void fl_hide_input(
		void
		);

FL_EXPORT const char *fl_show_simple_input(
		const char *str1,
		const char *defstr
		);


FL_EXPORT int fl_show_colormap(
		int oldcol
		);

FL_EXPORT int fl_goodie_atclose(FL_FORM *, FL_OBJECT *);


/********* choices *****************/
FL_EXPORT int fl_show_choices(
		const char *msg,
		int numb,
		const char *c1,
		const char *c2,
		const char *c3,
		int def
		);


FL_EXPORT int fl_show_choice(
		const char *m1,
		const char *m2,
		const char *m3,
		int numb,
		const char *c1,
		const char *c2,
		const char *c3,
		int def
		);

FL_EXPORT void fl_hide_choice(
		void
		);


FL_EXPORT void fl_set_choice_shortcut(
		const char *a,
		const char *b,
		const char *c
		);

#define fl_set_choices_shortcut fl_set_choice_shortcut

/************ one liner ***************/

FL_EXPORT void fl_show_oneliner(
		const char *s,
		int x,
		int y
		);

FL_EXPORT void fl_hide_oneliner(
		void
		);

FL_EXPORT void fl_set_oneliner_font(
		int style,
		int size
		);

FL_EXPORT void fl_set_oneliner_color(
		FL_COLOR tc,
		FL_COLOR bc
		);


FL_EXPORT void fl_set_tooltip_font(
		int style,
		int size
		);

FL_EXPORT void fl_set_tooltip_color(
		FL_COLOR tc,
		FL_COLOR bc
		);

FL_EXPORT void fl_set_tooltip_boxtype(
		int bt
		);

FL_EXPORT void fl_set_tooltip_lalign(
		int align
		);


/************* command log **************/

typedef struct
{
    FL_FORM *form;
    void *vdata;
    char *cdata;
    long  ldata;
    FL_OBJECT *browser;
    FL_OBJECT *close_browser;
    FL_OBJECT *clear_browser;
}
FD_CMDLOG;

#ifdef FL_WIN32
#define FL_PID_T HANDLE
#else
#define FL_PID_T long
#endif

FL_EXPORT long fl_exe_command(
		const char *cmd,
		int block
		);

FL_EXPORT int fl_end_command(
		long pid
		);

FL_EXPORT int fl_check_command(
		long pid
		);

FL_EXPORT FILE *fl_popen(
		const char *cmd,
		const char *type
		);

FL_EXPORT int fl_pclose(
		FILE *stream
		);

FL_EXPORT int fl_end_all_command(
		void
		);

FL_EXPORT void fl_show_command_log(
		int border
		);

FL_EXPORT void fl_hide_command_log(
		void
		);

FL_EXPORT void fl_clear_command_log(
		void
		);

FL_EXPORT void fl_addto_command_log(
		const char *s
		);

FL_EXPORT void fl_set_command_log_position(
		int x,
		int y
		);

FL_EXPORT FD_CMDLOG *fl_get_command_log_fdstruct(
		void
		);


/* aliases */
#define fl_open_command    fl_exe_command
#define fl_close_command   fl_end_command

/******* file selector *****************/

#define FL_MAX_FSELECTOR  6

typedef struct
{
    FL_FORM *fselect;
    void *vdata;
    void *cdata;
    long  ldata;
    FL_OBJECT *browser, *input, *prompt, *resbutt;
    FL_OBJECT *patbutt, *dirbutt, *cancel, *ready;
    FL_OBJECT *dirlabel, *patlabel;
    FL_OBJECT *appbutt[3];
}
FD_FSELECTOR;

FL_EXPORT int fl_use_fselector(
		int n
		);

FL_EXPORT const char *fl_show_fselector(
		const char *message,
		const char *dir,
		const char *pat,
		const char *fname
		);

FL_EXPORT void fl_hide_fselector(
		void
		);


FL_EXPORT void fl_set_fselector_fontsize(
		int fsize
		);

FL_EXPORT void fl_set_fselector_fontstyle(
		int fstyle
		);

FL_EXPORT void fl_set_fselector_placement(
		int place
		);

FL_EXPORT void fl_set_fselector_border(
		int b
		);


#define fl_set_fselector_transient(b)   \
                     fl_set_fselector_border((b)?FL_TRANSIENT:FL_FULLBORDER)

FL_EXPORT void fl_set_fselector_callback(
		FL_FSCB fscb,
		void *data
		);

FL_EXPORT const char *fl_get_filename(
		void
		);

FL_EXPORT const char *fl_get_directory(
		void
		);

FL_EXPORT const char *fl_get_pattern(
		void
		);

FL_EXPORT int fl_set_directory(
		const char *p
		);

FL_EXPORT void fl_set_pattern(
		const char *s
		);

FL_EXPORT void fl_refresh_fselector(
		void
		);

FL_EXPORT void fl_add_fselector_appbutton(
		const char *label,
		void (*cb)(void *),
		void *data
		);

FL_EXPORT void fl_remove_fselector_appbutton(
		const char *label
		);

FL_EXPORT void fl_disable_fselector_cache(
		int yes
		);

FL_EXPORT void fl_invalidate_fselector_cache(
		void
		);

FL_EXPORT FL_FORM *fl_get_fselector_form(
		void
		);

FL_EXPORT FD_FSELECTOR *fl_get_fselector_fdstruct(
		void
		);

FL_EXPORT void fl_hide_fselector(
		void
		);


FL_EXPORT void fl_set_fselector_filetype_marker(
		int dir,
		int fifo,
		int sock,
		int cdev,
		int bdev
		);


#define fl_show_file_selector     fl_show_fselector
#define fl_set_fselector_cb       fl_set_fselector_callback
#define fl_set_fselector_title(s) fl_set_form_title(fl_get_fselector_form(),s)

FL_EXPORT int fl_goodies_atclose(
		FL_FORM *form,
		void *data
		);


#endif /* } */

/*
 * $Id: input.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 */
#ifndef FL_INPUT_H
#define FL_INPUT_H		/* { */


/***** Types    *****/

typedef enum
{
    FL_NORMAL_INPUT,
    FL_FLOAT_INPUT,
    FL_INT_INPUT,
    FL_DATE_INPUT,
    FL_MULTILINE_INPUT,
    FL_HIDDEN_INPUT,
    FL_SECRET_INPUT
}
FL_INPUT_TYPE;

/* for date input */
enum
{
    FL_INPUT_MMDD, FL_INPUT_DDMM
};

/***** Defaults *****/

#define FL_INPUT_BOXTYPE	FL_DOWN_BOX
#define FL_INPUT_COL1		FL_COL1
#define FL_INPUT_COL2		FL_MCOL
#define FL_INPUT_LCOL		FL_LCOL
#define FL_INPUT_ALIGN		FL_ALIGN_LEFT

/***** Others   *****/

#define FL_INPUT_TCOL		FL_LCOL
#define FL_INPUT_CCOL		FL_BLUE

#define FL_RINGBELL             (1<<4)

/***** Routines *****/

FL_EXPORT FL_OBJECT *fl_create_input(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT FL_OBJECT *fl_add_input(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT void fl_set_input(
		FL_OBJECT *ob,
		const char *str
		);

FL_EXPORT void fl_set_input_return(
		FL_OBJECT *ob,
		int value
		);

FL_EXPORT void fl_set_input_color(
		FL_OBJECT *ob,
		int textcol,
		int curscol
		);

FL_EXPORT void fl_set_input_scroll(
		FL_OBJECT *ob,
		int yes
		);

FL_EXPORT void fl_set_input_cursorpos(
		FL_OBJECT *ob,
		int xpos,
		int ypos
		);

FL_EXPORT void fl_set_input_selected(
		FL_OBJECT *ob,
		int yes
		);

FL_EXPORT void fl_set_input_selected_range(
		FL_OBJECT *ob,
		int begin,
		int end
		);

FL_EXPORT const char *fl_get_input_selected_range(
		FL_OBJECT *ob,
		int *begin,
		int *end
		);

FL_EXPORT void fl_set_input_maxchars(
		FL_OBJECT *ob,
		int maxchars
		);

FL_EXPORT void fl_set_input_format(
		FL_OBJECT *ob,
		int fmt,
		int sep
		);

FL_EXPORT void fl_set_input_hscrollbar(
		FL_OBJECT *ob,
		int pref
		);

FL_EXPORT void fl_set_input_vscrollbar(
		FL_OBJECT *ob,
		int pref
		);

FL_EXPORT void fl_set_input_topline(
		FL_OBJECT *ob,
		int top
		);

FL_EXPORT void fl_set_input_scrollbarsize(
		FL_OBJECT *ob,
		int hh,
		int vw
		);

FL_EXPORT void fl_set_input_xoffset(
		FL_OBJECT *ob,
		int xoff
		);

FL_EXPORT int fl_get_input_xoffset(
		FL_OBJECT *ob
		);

FL_EXPORT int fl_set_input_fieldchar(
		FL_OBJECT *ob,
		int fchar
		);


FL_EXPORT int fl_get_input_topline(
		FL_OBJECT *ob
		);

FL_EXPORT int fl_get_input_screenlines(
		FL_OBJECT *ob
		);

FL_EXPORT int fl_get_input_cursorpos(
		FL_OBJECT *ob,
		int *x,
		int *y
		);

FL_EXPORT void fl_set_input_cursor_visible(
		FL_OBJECT *ob,
		int visible
		);

FL_EXPORT int fl_get_input_numberoflines(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_get_input_format(
		FL_OBJECT *ob,
		int *fmt,
		int *sep
		);

FL_EXPORT const char *fl_get_input(
		FL_OBJECT *ob
		);


typedef int (*FL_INPUTVALIDATOR) (FL_OBJECT *, const char *, const char *, int);
FL_EXPORT FL_INPUTVALIDATOR fl_set_input_filter(
		FL_OBJECT *ob,
		FL_INPUTVALIDATOR validate
		);

#define fl_set_input_shortcut fl_set_object_shortcut

/* edit keys. */
typedef struct
{
    /* basic editing */
    long del_prev_char;		/* delete previous char    */
    long del_next_char;		/* delete next char        */
    long del_prev_word;		/* delete previous word    */
    long del_next_word;		/* delete next word        */

    /* movement */
    long moveto_prev_line;	/* one line  up             */
    long moveto_next_line;	/* one line down            */
    long moveto_prev_char;	/* one char left            */
    long moveto_next_char;	/* one char right           */
    long moveto_prev_word;	/* one word left            */
    long moveto_next_word;	/* one word right           */
    long moveto_prev_page;	/* one page up              */
    long moveto_next_page;	/* one page down            */
    long moveto_bol;		/* move to begining of line */
    long moveto_eol;		/* move to end of line      */
    long moveto_bof;		/* move to begin of file    */
    long moveto_eof;		/* move to end of file      */

    /* misc. stuff */
    long transpose;		/* switch two char positions */
    long paste;			/* paste the edit buffer    */
    long backspace;		/* another  del_prev_char   */
    long del_to_bol;		/* cut to begining of line  */
    long del_to_eol;		/* cut to end of line       */
    long clear_field;		/* delete everything        */
    long del_to_eos;		/* not implemented          */
    long reserverd[4];		/* filler                  */
}
FL_EditKeymap;

FL_EXPORT void fl_set_input_editkeymap(
		const FL_EditKeymap *keymap
		);


#endif /* Input_H  } */

/*
 * $Id: menu.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 */
#ifndef FL_MENU_H
#define FL_MENU_H		/* { */


/************   Object Class: Menu         ************/

typedef enum
{
    FL_TOUCH_MENU,
    FL_PUSH_MENU,
    FL_PULLDOWN_MENU
}
FL_MENU_TYPE;

/***** Defaults *****/

#define FL_MENU_BOXTYPE		FL_BORDER_BOX
#define FL_MENU_COL1		FL_COL1
#define FL_MENU_COL2		FL_MCOL
#define FL_MENU_LCOL		FL_LCOL
#define FL_MENU_ALIGN		FL_ALIGN_CENTER

/***** Others   *****/

#define FL_MENU_MAXITEMS	128
#define FL_MENU_MAXSTR		64

/***** Routines *****/

FL_EXPORT FL_OBJECT *fl_create_menu(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT FL_OBJECT *fl_add_menu(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT void fl_clear_menu(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_set_menu(
		FL_OBJECT *ob,
		const char *menustr
		);

FL_EXPORT int fl_addto_menu(
		FL_OBJECT *ob,
		const char *menustr
		);

FL_EXPORT void fl_replace_menu_item(
		FL_OBJECT *ob,
		int numb,
		const char *str
		);

FL_EXPORT void fl_delete_menu_item(
		FL_OBJECT *ob,
		int numb
		);


FL_EXPORT void fl_set_menu_item_shortcut(
		FL_OBJECT *ob,
		int numb,
		const char *str
		);

FL_EXPORT void fl_set_menu_item_mode(
		FL_OBJECT *ob,
		int numb,
		unsigned mode
		);

FL_EXPORT void fl_show_menu_symbol(
		FL_OBJECT *ob,
		int show
		);

FL_EXPORT void fl_set_menu_popup(
		FL_OBJECT *ob,
		int pup
		);

FL_EXPORT int fl_get_menu_popup(
		FL_OBJECT *ob
		);


FL_EXPORT int fl_get_menu(
		FL_OBJECT *ob
		);

FL_EXPORT const char *fl_get_menu_item_text(
		FL_OBJECT *ob,
		int n
		);

FL_EXPORT int fl_get_menu_maxitems(
		FL_OBJECT *ob
		);

FL_EXPORT unsigned fl_get_menu_item_mode(
		FL_OBJECT *ob,
		int n
		);

FL_EXPORT const char *fl_get_menu_text(
		FL_OBJECT *ob
		);

FL_EXPORT int fl_set_menu_entries(
		FL_OBJECT *ob,
		FL_PUP_ENTRY *ent
		);


#endif /* MENU } */

/*
 * $Id: menu.h,v 0.86 1997/03/22 06:04:08 zhao Beta $
 *
 */
#ifndef FL_MENUBAR_H
#define FL_MENUBAR_H		/* { */


/************   Object Class: MenuBar         ************/

enum
{
    FL_NORMAL_MENUBAR
};

/***** Defaults *****/

#define FL_MENUBAR_BOXTYPE	FL_UP_BOX
#define FL_MENUBAR_COL1		FL_COL1
#define FL_MENUBAR_COL2		FL_MCOL
#define FL_MENUBAR_LCOL		FL_LCOL

/***** Routines *****/

FL_EXPORT FL_OBJECT *fl_create_menubar(int, FL_Coord, FL_Coord, FL_Coord, FL_Coord,
				 const char *);


FL_EXPORT FL_OBJECT *fl_add_menubar(int, FL_Coord, FL_Coord, FL_Coord, FL_Coord,
			      const char *);


FL_EXPORT void fl_clear_menubar(FL_OBJECT *);

FL_EXPORT void fl_set_menubar(FL_OBJECT *, const char *);

FL_EXPORT void fl_set_menubar_entries(FL_OBJECT *, const char *, FL_PUP_ENTRY *);


#endif /* MENUBAR } */

/*
 * $Id: popup.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *  Prototypes for pop-up menus
 */

#ifndef FL_POPUP_H
#define FL_POPUP_H		/* { */


#define FL_MAXPUPI     128	/* max item each pup        */
#define FL_PUP_PADH    4	/* space between each items */

typedef void (*FL_PUP_ENTERCB) (int, void *);	/* call back prototype  */
typedef void (*FL_PUP_LEAVECB) (int, void *);	/* call back prototype  */

FL_EXPORT int fl_setpup_entries(
		int nm,
		FL_PUP_ENTRY *entries
		);

FL_EXPORT int fl_newpup(
		Window win
		);

FL_EXPORT int fl_defpup(
		Window win,
		const char *str,
		...
		);

FL_EXPORT int fl_addtopup(
		int n,
		const char *str,
		...
		);

FL_EXPORT int fl_setpup_mode(
		int nm,
		int ni,
		unsigned int mode
		);

FL_EXPORT void fl_freepup(
		int n
		);

FL_EXPORT int fl_dopup(
		int n
		);


FL_EXPORT Cursor fl_setpup_default_cursor(
		int cursor
		);


/* setpup_{colors|fontsize} etc will be removed in next revision
 * use fl_set_default_xxx instead
 */
FL_EXPORT void fl_setpup_color(
		FL_COLOR fg,
		FL_COLOR bg
		);

FL_EXPORT void fl_setpup_checkcolor(
		FL_COLOR col
		);

FL_EXPORT int fl_setpup_fontsize(
		int size
		);

FL_EXPORT int fl_setpup_fontstyle(
		int style
		);


#define fl_setpup_default_fontsize    fl_setpup_fontsize
#define fl_setpup_default_fontstyle   fl_setpup_fontstyle
#define fl_setpup_default_color       fl_setpup_color
#define fl_setpup_default_checkcolor  fl_setpup_checkcolor

FL_EXPORT int fl_setpup_default_bw(
		int bw
		);

FL_EXPORT void fl_setpup_shortcut(
		int nm,
		int ni,
		const char *sc
		);

FL_EXPORT void fl_setpup_position(
		int x,
		int y
		);

FL_EXPORT void fl_setpup_selection(
		int nm,
		int ni
		);

FL_EXPORT void fl_setpup_shadow(
		int n,
		int y
		);

FL_EXPORT void fl_setpup_softedge(
		int n,
		int y
		);

FL_EXPORT void fl_setpup_bw(
		int n,
		int bw
		);

FL_EXPORT void fl_setpup_title(
		int nm,
		const char *title
		);

FL_EXPORT FL_PUP_ENTERCB fl_setpup_entercb(
		int nm,
		FL_PUP_ENTERCB cb,
		void *data
		);

FL_EXPORT FL_PUP_LEAVECB fl_setpup_leavecb(
		int nm,
		FL_PUP_LEAVECB cb,
		void *data
		);

FL_EXPORT void fl_setpup_pad(
		int n,
		int padw,
		int padh
		);

FL_EXPORT Cursor fl_setpup_cursor(
		int nm,
		int cursor
		);

FL_EXPORT int fl_setpup_maxpup(
		int n
		);

FL_EXPORT unsigned fl_getpup_mode(
		int nm,
		int ni
		);

FL_EXPORT const char *fl_getpup_text(
		int nm,
		int ni
		);

FL_EXPORT void fl_showpup(
		int n
		);

FL_EXPORT void fl_hidepup(
		int n
		);

FL_EXPORT int fl_getpup_items(
		int n
		);

FL_EXPORT int fl_current_pup(
		void
		);


#define fl_setpup_hotkey    fl_setpup_shortcut

FL_EXPORT FL_PUP_CB fl_setpup_itemcb(
		int nm,
		int ni,
		FL_PUP_CB cb
		);

FL_EXPORT FL_PUP_CB fl_setpup_menucb(
		int nm,
		FL_PUP_CB cb
		);

FL_EXPORT void fl_setpup_submenu(
		int m,
		int i,
		int subm
		);


#define fl_setpup    fl_setpup_mode

#endif /* Xpopup } */

/*
 * $Id: positioner.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 */
#ifndef FL_POSITIONER_H
#define FL_POSITIONER_H		/* { */


#define FL_NORMAL_POSITIONER	0
#define FL_OVERLAY_POSITIONER	1

/***** Defaults *****/

#define FL_POSITIONER_BOXTYPE	FL_DOWN_BOX
#define FL_POSITIONER_COL1	FL_COL1
#define FL_POSITIONER_COL2	FL_RED
#define FL_POSITIONER_LCOL	FL_LCOL
#define FL_POSITIONER_ALIGN	FL_ALIGN_BOTTOM

/***** Others   *****/


/***** Routines *****/

FL_EXPORT FL_OBJECT *fl_create_positioner(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT FL_OBJECT *fl_add_positioner(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT void fl_set_positioner_xvalue(
		FL_OBJECT *ob,
		double val
		);

FL_EXPORT double fl_get_positioner_xvalue(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_set_positioner_xbounds(
		FL_OBJECT *ob,
		double min,
		double max
		);

FL_EXPORT void fl_get_positioner_xbounds(
		FL_OBJECT *ob,
		double *min,
		double *max
		);

FL_EXPORT void fl_set_positioner_yvalue(
		FL_OBJECT *ob,
		double val
		);

FL_EXPORT double fl_get_positioner_yvalue(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_set_positioner_ybounds(
		FL_OBJECT *ob,
		double min,
		double max
		);

FL_EXPORT void fl_get_positioner_ybounds(
		FL_OBJECT *ob,
		double *min,
		double *max
		);

FL_EXPORT void fl_set_positioner_xstep(
		FL_OBJECT *ob,
		double value
		);

FL_EXPORT void fl_set_positioner_ystep(
		FL_OBJECT *ob,
		double value
		);

FL_EXPORT void fl_set_positioner_return(
		FL_OBJECT *ob,
		int value
		);



#endif /* Positioner.H } */

#ifndef FL_SCROLLBAR_H
#define FL_SCROLLBAR_H

enum
{
    FL_HOR_SCROLLBAR,
    FL_VERT_SCROLLBAR,
    FL_HOR_THIN_SCROLLBAR,
    FL_VERT_THIN_SCROLLBAR,
    FL_HOR_NICE_SCROLLBAR,
    FL_VERT_NICE_SCROLLBAR,
    FL_HOR_PLAIN_SCROLLBAR,
    FL_VERT_PLAIN_SCROLLBAR
};

#define FL_HOR_BASIC_SCROLLBAR      FL_HOR_PLAIN_SCROLLBAR
#define FL_VERT_BASIC_SCROLLBAR     FL_VERT_PLAIN_SCROLLBAR

enum
{
   FL_THIN_SCROLLBAR,
   FL_PLAIN_SCROLLBAR,
   FL_NORMAL_SCROLLBAR,
   FL_NICE_SCROLLBAR
};

FL_EXPORT FL_OBJECT *fl_create_scrollbar(
		int type,
		int x,
		int y,
		int w,
		int h,
		const char *l
		);

FL_EXPORT FL_OBJECT *fl_add_scrollbar(
		int type,
		int x,
		int y,
		int w,
		int h,
		const char *l
		);


FL_EXPORT double fl_get_scrollbar_value(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_set_scrollbar_value(
		FL_OBJECT *ob,
		double val
		);

FL_EXPORT void fl_set_scrollbar_size(
		FL_OBJECT *ob,
		double val
		);

FL_EXPORT void fl_set_scrollbar_increment(
		FL_OBJECT *ob,
		double l,
		double r
		);

FL_EXPORT void fl_get_scrollbar_increment(
		FL_OBJECT *ob,
		float *a,
		float *b
		);

FL_EXPORT void fl_set_scrollbar_bounds(
		FL_OBJECT *ob,
		double b1,
		double b2
		);

FL_EXPORT void fl_get_scrollbar_bounds(
		FL_OBJECT *ob,
		float *b1,
		float *b2
		);

FL_EXPORT void fl_set_scrollbar_return(
		FL_OBJECT *ob,
		int ret
		);

FL_EXPORT void fl_set_scrollbar_step(
		FL_OBJECT *ob,
		double step
		);


#endif   /* SCROLLBAR */

/*
 * $Id: slider.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 * Object Class: Slider
 *
 */
#ifndef FL_SLIDER_H
#define FL_SLIDER_H


typedef enum
{
    FL_VERT_SLIDER,
    FL_HOR_SLIDER,
    FL_VERT_FILL_SLIDER,
    FL_HOR_FILL_SLIDER,
    FL_VERT_NICE_SLIDER,
    FL_HOR_NICE_SLIDER,
    FL_HOR_BROWSER_SLIDER,
    FL_VERT_BROWSER_SLIDER,
    FL_HOR_BROWSER_SLIDER2,	/* for internal use only */
    FL_VERT_BROWSER_SLIDER2,	/* for internal use only */
    FL_HOR_THIN_SLIDER,		/* internal use          */
    FL_VERT_THIN_SLIDER,	/* internal use          */
    FL_HOR_BASIC_SLIDER,	/* internal use          */
    FL_VERT_BASIC_SLIDER	/* internal use          */
}
FL_SLIDER_TYPE;


/***** Defaults *****/

#define FL_SLIDER_BW1           FL_BOUND_WIDTH
#define FL_SLIDER_BW2           (FL_abs(FL_BOUND_WIDTH)-1)
#define FL_SLIDER_BOXTYPE	FL_DOWN_BOX
#define FL_SLIDER_COL1		FL_COL1
#define FL_SLIDER_COL2		FL_COL1
#define FL_SLIDER_LCOL		FL_LCOL
#define FL_SLIDER_ALIGN		FL_ALIGN_BOTTOM

/***** Others   *****/

#define FL_SLIDER_FINE		0.05
#define FL_SLIDER_WIDTH		0.10


/***** Routines *****/

FL_EXPORT FL_OBJECT *fl_create_slider(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_add_slider(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT FL_OBJECT *fl_create_valslider(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_add_valslider(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT void fl_set_slider_value(
		FL_OBJECT *ob,
		double val
		);

FL_EXPORT double fl_get_slider_value(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_set_slider_bounds(
		FL_OBJECT *ob,
		double min,
		double max
		);

FL_EXPORT void fl_get_slider_bounds(
		FL_OBJECT *ob,
		double *min,
		double *max
		);


FL_EXPORT void fl_set_slider_return(
		FL_OBJECT *ob,
		int value
		);


FL_EXPORT void fl_set_slider_step(
		FL_OBJECT *ob,
		double value
		);

FL_EXPORT void fl_set_slider_increment(
		FL_OBJECT *ob,
		double l,
		double r
		);

FL_EXPORT void fl_get_slider_increment(
		FL_OBJECT *ob,
		float *l,
		float *r
		);

FL_EXPORT void fl_set_slider_size(
		FL_OBJECT *ob,
		double size
		);

FL_EXPORT void fl_set_slider_precision(
		FL_OBJECT *ob,
		int prec
		);

FL_EXPORT void fl_set_slider_filter(
		FL_OBJECT *ob,
		FL_VAL_FILTER filter
		);



#endif 

/*
 * $Id: tabfolder.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 */
#ifndef FL_FOLDER_H
#define FL_FOLDER_H

enum
{
    FL_TOP_TABFOLDER,		/* tab on top */
    FL_BOTTOM_TABFOLDER,
    FL_LEFT_TABFOLDER,
    FL_RIGHT_TABFOLDER,
    FL_NORMAL_TABFOLDER = FL_TOP_TABFOLDER
};

enum
{
    FL_NO, FL_FIT, FL_ENLARGE_ONLY
};



FL_EXPORT FL_OBJECT *fl_create_tabfolder(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT FL_OBJECT *fl_add_tabfolder(
		int type,
		int x,
		int y,
		int w,
		int h,
		const char *l
		);


FL_EXPORT FL_OBJECT *fl_addto_tabfolder(
		FL_OBJECT *ob,
		const char *title,
		FL_FORM *form
		);


FL_EXPORT FL_FORM *fl_get_tabfolder_folder_bynumber(
		FL_OBJECT *ob,
		int num
		);

FL_EXPORT FL_FORM *fl_get_tabfolder_folder_byname(
		FL_OBJECT *ob,
		const char *name
		);


FL_EXPORT void fl_delete_folder(
		FL_OBJECT *ob,
		FL_FORM *form
		);

FL_EXPORT void fl_delete_folder_bynumber(
		FL_OBJECT *ob,
		int num
		);

FL_EXPORT void fl_delete_folder_byname(
		FL_OBJECT *ob,
		const char *name
		);


FL_EXPORT void fl_set_folder(
		FL_OBJECT *ob,
		FL_FORM *form
		);

FL_EXPORT void fl_set_folder_byname(
		FL_OBJECT *ob,
		const char *name
		);

FL_EXPORT void fl_set_folder_bynumber(
		FL_OBJECT *ob,
		int num
		);



FL_EXPORT FL_FORM *fl_get_folder(
		FL_OBJECT *ob
		);

FL_EXPORT int fl_get_folder_number(
		FL_OBJECT *ob
		);

FL_EXPORT const char *fl_get_folder_name(
		FL_OBJECT *ob
		);

FL_EXPORT int fl_get_tabfolder_numfolders(
		FL_OBJECT *ob
		);


FL_EXPORT FL_FORM *fl_get_active_folder(
		FL_OBJECT *ob
		);

FL_EXPORT int fl_get_active_folder_number(
		FL_OBJECT *ob
		);

FL_EXPORT const char *fl_get_active_folder_name(
		FL_OBJECT *ob
		);


FL_EXPORT void fl_get_folder_area(
		FL_OBJECT *ob,
		int *x,
		int *y,
		int *w,
		int *h
		);


FL_EXPORT void fl_replace_folder_bynumber(
		FL_OBJECT *ob,
		int num,
		FL_FORM *form
		);

FL_EXPORT int fl_set_tabfolder_autofit(
		FL_OBJECT *ob,
		int y
		);

FL_EXPORT int fl_set_default_tabfolder_corner(
		int n
		);

FL_EXPORT int fl_set_tabfolder_offset(
		FL_OBJECT *ob,
		int offset
		);


#endif 

/*
 * $Id: text.h,v 1.1 1995/04/01 00:39:31 zhao Exp $
 *
 */
#ifndef FL_TEXT_H
#define FL_TEXT_H

enum
{
    FL_NORMAL_TEXT
};

#define FL_TEXT_BOXTYPE    FL_FLAT_BOX
#define FL_TEXT_COL1       FL_COL1
#define FL_TEXT_COL2       FL_MCOL
#define FL_TEXT_LCOL       FL_LCOL
#define FL_TEXT_ALIGN      FL_ALIGN_LEFT

FL_EXPORT FL_OBJECT *fl_create_text(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT FL_OBJECT *fl_add_text(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


#endif 


/*
 * $Id: thumbwheel.h,v 0.10 1998/09/24 06:05:44 zhao Exp $
 */

#ifndef FL_THUMBWHEEL_H
#define FL_THUMBWHEEL_H

enum
{
    FL_VERT_THUMBWHEEL,
    FL_HOR_THUMBWHEEL
};



FL_EXPORT double fl_get_thumbwheel_value(
		FL_OBJECT *ob
		);

FL_EXPORT double fl_set_thumbwheel_value(
		FL_OBJECT *ob,
		double value
		);

FL_EXPORT double fl_get_thumbwheel_step(
		FL_OBJECT *ob
		);

FL_EXPORT double fl_set_thumbwheel_step(
		FL_OBJECT *ob,
		double step
		);

FL_EXPORT int fl_set_thumbwheel_return(
		FL_OBJECT *ob,
		int how
		);

FL_EXPORT int fl_set_thumbwheel_crossover(
		FL_OBJECT *ob,
		int flag
		);


FL_EXPORT void fl_set_thumbwheel_bounds(
		FL_OBJECT *ob,
		double min,
		double max
		);

FL_EXPORT void fl_get_thumbwheel_bounds(
		FL_OBJECT *ob,
		double *min,
		double *max
		);


FL_EXPORT FL_OBJECT *fl_create_thumbwheel(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_add_thumbwheel(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


/* defaults */

#define FL_THUMBWHEEL_COL1      FL_COL1
#define FL_THUMBWHEEL_COL2      FL_COL1
#define FL_THUMBWHEEL_LCOL      FL_BLACK
#define FL_THUMBWHEEL_BOXTYPE   FL_BORDER_BOX
#define FL_THUMBWHEEL_ALIGN     FL_ALIGN_BOTTOM

#endif    /* THUMBWHEEL_H */

/*
 * $Id: timer.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *  Object Class: Timer
 *
 */
#ifndef FL_TIMER_H
#define FL_TIMER_H


typedef enum
{
    FL_NORMAL_TIMER,
    FL_VALUE_TIMER,
    FL_HIDDEN_TIMER
}
FL_TIMER_TYPE;


/***** Defaults *****/

#define FL_TIMER_BOXTYPE	FL_DOWN_BOX
#define FL_TIMER_COL1		FL_COL1
#define FL_TIMER_COL2		FL_RED
#define FL_TIMER_LCOL		FL_LCOL
#define FL_TIMER_ALIGN		FL_ALIGN_CENTER

/***** Others   *****/

#define FL_TIMER_BLINKRATE	0.2

/***** Routines *****/

typedef char *(*FL_TIMER_FILTER) (FL_OBJECT *, double);

FL_EXPORT FL_OBJECT *fl_create_timer(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *l
		);


FL_EXPORT FL_OBJECT *fl_add_timer(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *l
		);


FL_EXPORT void fl_set_timer(
		FL_OBJECT *ob,
		double total
		);

FL_EXPORT double fl_get_timer(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_set_timer_countup(
		FL_OBJECT *ob,
		int yes
		);

FL_EXPORT FL_TIMER_FILTER fl_set_timer_filter(
		FL_OBJECT *ob,
		FL_TIMER_FILTER filter
		);

FL_EXPORT void fl_suspend_timer(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_resume_timer(
		FL_OBJECT *ob
		);



#endif

/*
 * $Id: xyplot.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 */
#ifndef FL_XYPLOT_H_
#define FL_XYPLOT_H_

/*  Class FL_XYPLOT */
typedef enum
{
    FL_NORMAL_XYPLOT,		/* solid line                        */
    FL_SQUARE_XYPLOT,		/* with added square                 */
    FL_CIRCLE_XYPLOT,		/* with added circle                 */
    FL_FILL_XYPLOT,		/* fill completely                   */
    FL_POINTS_XYPLOT,		/* only data points                  */
    FL_DASHED_XYPLOT,		/* dashed line                       */
    FL_IMPULSE_XYPLOT,
    FL_ACTIVE_XYPLOT,		/* accepts interactive manipulations */
    FL_EMPTY_XYPLOT,
    FL_DOTTED_XYPLOT,
    FL_DOTDASHED_XYPLOT,
    FL_LONGDASHED_XYPLOT,
    FL_LINEPOINTS_XYPLOT	/* line & points                     */
} FL_XYPLOT_TYPE;

enum
{
    FL_LINEAR,
    FL_LOG
};

enum
{
    FL_GRID_NONE = 0,
    FL_GRID_MAJOR = 1,
    FL_GRID_MINOR = 2
};

/***** Defaults *****/

#define FL_XYPLOT_BOXTYPE       FL_FLAT_BOX
#define FL_XYPLOT_COL1          FL_COL1
#define FL_XYPLOT_LCOL          FL_LCOL
#define FL_XYPLOT_ALIGN         FL_ALIGN_BOTTOM
#define FL_MAX_XYPLOTOVERLAY    32

/***** Others   *****/


FL_EXPORT FL_OBJECT *fl_create_xyplot(
		int t,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *l
		);

FL_EXPORT FL_OBJECT *fl_add_xyplot(
		int t,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *l
		);

FL_EXPORT int fl_set_xyplot_data(
		FL_OBJECT *ob,
		float *x,
		float *y,
		int n,
		const char *title,
		const char *xlabel,
		const char *ylabel
		);

FL_EXPORT int fl_set_xyplot_data_double(
		FL_OBJECT *ob,
		double *x,
		double *y,
		int n,
		const char *title,
		const char *xlabel,
		const char *ylabel
		);

FL_EXPORT int fl_set_xyplot_file(
		FL_OBJECT *ob,
		const char *f,
		const char *title,
		const char *xl,
		const char *yl
		);

FL_EXPORT void fl_insert_xyplot_data(
		FL_OBJECT *ob,
		int id,
		int n,
		double x,
		double y
		);


#define fl_set_xyplot_datafile fl_set_xyplot_file

FL_EXPORT void fl_add_xyplot_text(
		FL_OBJECT *ob,
		double x,
		double y,
		const char *text,
		int al,
		FL_COLOR col
		);


FL_EXPORT void fl_delete_xyplot_text(
		FL_OBJECT *ob,
		const char *text
		);

FL_EXPORT int fl_set_xyplot_maxoverlays(
		FL_OBJECT *ob,
		int maxover
		);

FL_EXPORT void fl_add_xyplot_overlay(
		FL_OBJECT *ob,
		int id,
		float *x,
		float *y,
		int n,
		FL_COLOR col
		);

FL_EXPORT int fl_add_xyplot_overlay_file(
		FL_OBJECT *ob,
		int id,
		const char *f,
		FL_COLOR c
		);

FL_EXPORT void fl_set_xyplot_return(
		FL_OBJECT *ob,
		int when
		);

FL_EXPORT void fl_set_xyplot_xtics(
		FL_OBJECT *ob,
		int major,
		int minor
		);

FL_EXPORT void fl_set_xyplot_ytics(
		FL_OBJECT *ob,
		int major,
		int minor
		);

FL_EXPORT void fl_set_xyplot_xbounds(
		FL_OBJECT *ob,
		double xmin,
		double xmax
		);

FL_EXPORT void fl_set_xyplot_ybounds(
		FL_OBJECT *ob,
		double ymin,
		double ymax
		);

FL_EXPORT void fl_get_xyplot_xbounds(
		FL_OBJECT *ob,
		float *xmin,
		float *xmax
		);

FL_EXPORT void fl_get_xyplot_ybounds(
		FL_OBJECT *ob,
		float *ymin,
		float *ymax
		);

FL_EXPORT void fl_get_xyplot(
		FL_OBJECT *ob,
		float *x,
		float *y,
		int *i
		);

FL_EXPORT void fl_get_xyplot_data(
		FL_OBJECT *ob,
		float *x,
		float *y,
		int *n
		);

FL_EXPORT void fl_get_xyplot_data_pointer(
		FL_OBJECT *ob,
		int id,
		float **x,
		float **y,
		int *n
		);

FL_EXPORT void fl_get_xyplot_overlay_data(
		FL_OBJECT *ob,
		int id,
		float *x,
		float *y,
		int *n
		);

FL_EXPORT void fl_set_xyplot_overlay_type(
		FL_OBJECT *ob,
		int id,
		int type
		);

FL_EXPORT void fl_delete_xyplot_overlay(
		FL_OBJECT *ob,
		int id
		);

FL_EXPORT void fl_set_xyplot_interpolate(
		FL_OBJECT *ob,
		int id,
		int deg,
		double grid
		);

FL_EXPORT void fl_set_xyplot_inspect(
		FL_OBJECT *ob,
		int yes
		);

FL_EXPORT void fl_set_xyplot_symbolsize(
		FL_OBJECT *ob,
		int n
		);

FL_EXPORT void fl_replace_xyplot_point(
		FL_OBJECT *ob,
		int i,
		double x,
		double y
		);

FL_EXPORT void fl_get_xyplot_xmapping(
		FL_OBJECT *ob,
		float *a,
		float *b
		);

FL_EXPORT void fl_get_xyplot_ymapping(
		FL_OBJECT *ob,
		float *a,
		float *b
		);

FL_EXPORT void fl_set_xyplot_keys(
		FL_OBJECT *ob,
		char **keys,
		float x,
		float y,
		int align
		);

FL_EXPORT void fl_set_xyplot_key(
		FL_OBJECT *ob,
		int id,
		const char *key
		);

FL_EXPORT void fl_set_xyplot_key_position(
		FL_OBJECT *ob,
		float x,
		float y,
		int align
		);

FL_EXPORT void fl_set_xyplot_key_font(
		FL_OBJECT *ob,
		int style,
		int size
		);

FL_EXPORT int fl_get_xyplot_numdata(
		FL_OBJECT *ob,
		int id
		);


/* The following two functions will be removed.
 * Use fl_set_object_l[size|style] for the functionalities
 */
FL_EXPORT void fl_set_xyplot_fontsize(
		FL_OBJECT *ob,
		int size
		);

FL_EXPORT void fl_set_xyplot_fontstyle(
		FL_OBJECT *ob,
		int style
		);


FL_EXPORT void fl_xyplot_s2w(
		FL_OBJECT *ob,
		double sx,
		double sy,
		float *wx,
		float *wy
		);

FL_EXPORT void fl_xyplot_w2s(
		FL_OBJECT *ob,
		double wx,
		double wy,
		float *sx,
		float *sy
		);

FL_EXPORT void fl_set_xyplot_xscale(
		FL_OBJECT *ob,
		int scale,
		double base
		);

FL_EXPORT void fl_set_xyplot_yscale(
		FL_OBJECT *ob,
		int scale,
		double base
		);


FL_EXPORT void fl_clear_xyplot(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_set_xyplot_linewidth(
		FL_OBJECT *ob,
		int id,
		int lw
		);

FL_EXPORT void fl_set_xyplot_xgrid(
		FL_OBJECT *ob,
		int xgrid
		);

FL_EXPORT void fl_set_xyplot_ygrid(
		FL_OBJECT *ob,
		int ygrid
		);

FL_EXPORT int fl_set_xyplot_grid_linestyle(
		FL_OBJECT *ob,
		int style
		);

FL_EXPORT void fl_set_xyplot_alphaxtics(
		FL_OBJECT *ob,
		const char *m,
		const char *s
		);

FL_EXPORT void fl_set_xyplot_alphaytics(
		FL_OBJECT *ob,
		const char *m,
		const char *s
		);

FL_EXPORT void fl_set_xyplot_fixed_xaxis(
		FL_OBJECT *ob,
		const char *lm,
		const char *rm
		);

FL_EXPORT void fl_set_xyplot_fixed_yaxis(
		FL_OBJECT *ob,
		const char *bm,
		const char *tm
		);


FL_EXPORT int fl_interpolate(
		const float *wx,
		const float *wy,
		int nin,
		float *x,
		float *y,
		double grid,
		int ndeg
		);

FL_EXPORT int fl_spline_interpolate(
		const float *wx,
		const float *wy,
		int nin,
		float *x,
		float *y,
		double grid
		);


typedef void (*FL_XYPLOT_SYMBOL) (FL_OBJECT *, int, FL_POINT *, int, int, int);
FL_EXPORT FL_XYPLOT_SYMBOL fl_set_xyplot_symbol(
		FL_OBJECT *ob,
		int id,
		FL_XYPLOT_SYMBOL symbol
		);


FL_EXPORT int fl_set_xyplot_mark_active(
		FL_OBJECT *ob,
		int y
		);


#endif  /* FL_XYPLOT_H */

/* the following (fl_fheight) etc. were never documented and were
   removed from V0.89, but apparently this broke some applications that
   were using them. Put them back in 10/22/00 */

#define fl_textgc      fl_textgc_()
#define fl_gc          fl_gc_()
#define fl_cur_win     fl_cur_win_()
#define fl_fheight     fl_fheight_()
#define fl_fdesc       fl_fdesc_()
#define fl_cur_fs      fl_cur_fs_()

extern GC           fl_gc_(void), fl_textgc_(void);
extern int          fl_fheight_(void), fl_fdesc_(void);
extern Window       fl_cur_win_(void);
extern XFontStruct* fl_cur_fs_(void);
extern Display*     fl_display_(void);

#if !defined(TRUE) && !defined(FALSE)
#define FALSE        0
#define TRUE        (!FALSE)
#endif

#if defined(MAKING_FORMS) || defined(MAKING_FD)
#include "flinternal.h"
#endif /* MAKING_FORMS */

#if defined(__cplusplus)
}
#endif

#endif /* FL_FORMS_H */
