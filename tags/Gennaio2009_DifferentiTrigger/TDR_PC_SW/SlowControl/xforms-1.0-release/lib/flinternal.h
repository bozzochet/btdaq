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
 *  flinternal.h   $Revision: 1.0 $ $State: Release $
 *                 $Date: 2002/05/08 05:16:30 $ $Author: zhao $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-1998  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *-------
 *
 * Internal routines used by the Forms Library. An application should
 * not use any of this since there is no guarantee that exactly some
 * thing will exist in future versions of XForms.
 *
 * This header file is needed by forms.h only if MAKING_FORMS is defined.
 *
 */

#ifndef FLINTERNAL_H
#define FLINTERNAL_H

#include <stdlib.h>


#include "local.h"
#include "ulib.h"

/*  macros  and constants */

/* FL_DEBUG controls some conditional compilations. Even if the code
 * is compiled in, it still needs -debug level on the command line
 * to activate the message output. FL_DEBUG less than ML_ERR probably
 * is not a good idea. Only when making real production executables
 * should FL_DEBUG be less than ML_ERR (say ML_ERR-1)
 *
 *   ML_ERR    -- print some error messages
 *   ML_WARN    -- print some error messages
 *   ML_INFO1  -- some messages
 *   ML_INFO2  -- more messages
 *   ML_DEBUG  -- debugging code compiled in
 *   ML_TRACE  -- can be unbearable
 */

#ifndef FL_DEBUG
#define FL_DEBUG   ML_WARN
#endif

#define DEACTIVATED   -1	/* must be none zero and negative */

/*
 * There are two ways to handle interaction. One is to process object
 * Q only if there are no more event pending. This can result in
 * an event entered into object Q more than once if the connection
 * is slow and clicking is rapid. If subsequently the object is
 * hidden or deleted/freed, problem occurs as the callback will
 * still be processed. To fix this, need to flush the object Q
 * when hiding/deleting objects. Also if an object is entered
 * multiple times, and its status changed each time, when the
 * Q is handled, we get wrong status.
 *
 * The other way to handle the interaction is to process object
 * Q as soon as the status of the object is changed. This is
 * the correct behavior. However, a change in status is typically
 * preceeded by a redraw (say mouse release on button), and due to
 * the X buffering mechnism, the redraw probably not get shown yet
 * when the callback is evoked. To fix this, an explicit flush
 * is needed. This can be time consuming. Also objects that do
 * not have callbacks may be handled out of order (after the one's
 * having callbacks).
 *
 */

#if 1
#define DELAYED_ACTION
#endif

typedef void (*FL_DRAW_BOX) (int, FL_Coord, FL_Coord, FL_Coord, FL_Coord,
			     FL_COLOR, int);

/*
 * If we want to have gamma correction as a built-in feature.
 * Probably useless
 */

#define DO_GAMMA_CORRECTION

/*
 * FL_MAX_MASK_BITS is the same as the event masks defined by X
 */

#define FL_MAX_MASK_BITS    24

#define FL_ALL_MASKS        ((1L << (FL_MAX_MASK_BITS+1))-1)

/* XForms internal colormap */
typedef struct
{
    const char *name;
    FL_COLOR index;
    unsigned short r, g, b, a;
    int grayval;
}
FL_IMAP;

#define FL_NoColor     0x7fffffffL
#define BadPixel       FL_NoColor


#define Clamp(v, vmin, vmax) ((v)<(vmin)? (vmin):((v)>(vmax)?(vmax):(v)))
#define TIMER3 (-1)

#define IsValidClass(o,c) (o && o->objclass == c)

#define fl_safe_free(p) do {if(p) {fl_free(p);p=0;}} while(0)
#define fl_class(i)     fl_state[i].vclass
#define fl_depth(i)     fl_state[i].depth
#define fl_visual(i)    fl_state[i].xvinfo->visual
#define fl_colormap(i)  fl_state[i].colormap
#define fl_map(i)       fl_state[i].colormap
#define fl_dithered(i)  fl_state[i].dithered

#define fl_rgbmode      (fl_class(fl_vmode) == TrueColor || \
                         fl_class(fl_vmode) == DirectColor)

#define ringbell()  XBell(fl_display, 0)

/* events.c or event related */

extern void fl_object_qenter(FL_OBJECT *);
extern FL_OBJECT *fl_object_qread(void);
extern FL_OBJECT *fl_object_qread_direct(void);
extern void fl_object_qflush(FL_FORM *);
extern void fl_object_qflush_object(FL_OBJECT *);
extern void fl_treat_user_events(void);
extern void fl_treat_interaction_events(int);
extern void fl_compress_event(XEvent *, unsigned long);
extern const char *fl_event_name(int);
extern XEvent *fl_xevent_name(const char *, const XEvent *);
extern void fl_handle_automatic(XEvent *, int);
extern FL_FORM *fl_get_fselector_form(void);

/* misc. utilitnes */
extern void fl_set_no_connection(int);
extern void fl_print_version(int);
extern const char *fl_rm_rcs_kw(const char *);
extern void fl_set_debug_level(int);

/* from forms.c and object.c */
extern FL_FORM *fl_make_form(FL_Coord, FL_Coord);
extern void fl_detach_form(FL_FORM *);
extern void fl_handle_object(FL_OBJECT *, int, FL_Coord, FL_Coord, int,
			     XEvent *);
extern int fl_handle_object_direct(FL_OBJECT *, int, FL_Coord, FL_Coord, int,
				   XEvent *);

extern FL_OBJECT *fl_find_first(FL_FORM *, int, FL_Coord, FL_Coord);
extern FL_OBJECT *fl_find_last(FL_FORM *, int, FL_Coord, FL_Coord);
extern FL_OBJECT *fl_find_object(FL_OBJECT *, int, FL_Coord, FL_Coord);
extern void fl_insert_object(FL_OBJECT *, FL_OBJECT *);
/* double buffering etc. */
extern void fl_free_flpixmap(FL_pixmap *);
extern void fl_create_object_pixmap(FL_OBJECT *);
extern void fl_show_object_pixmap(FL_OBJECT *);
extern void fl_create_form_pixmap(FL_FORM *);
extern void fl_show_form_pixmap(FL_FORM *);

/* windowing support */
extern void fl_default_xswa(void);
extern Window fl_cmap_winopen(Window, Colormap, const char *);
extern Window fl_create_window(Window, Colormap, const char *);
extern void fl_create_gc(Window);
extern void fl_demand_event(int);

extern Window fl_get_win_mouse(Window win, FL_Coord *, FL_Coord *, unsigned *);

enum
{
    FL_COMMAND_PROP = 1,
    FL_MACHINE_PROP,
    FL_PROP_SET = (1 << 10)	/* really set */
};

extern FL_FORM *fl_property_set(unsigned);
extern void fl_set_winproperty(Window, unsigned);
#define fl_set_property  fl_set_winproperty

/* timer stuff */

#define FL_NTIMER  5
enum
{
    FL_MAIN_T, FL_FS_T, FL_PUP_T
};

extern float fl_time_passed(int);
extern void fl_reset_time(int);

/* graphics related */

extern int fl_mode_capable(int, int);
extern int fl_doublebuffer_capable(int);
extern void fl_init_colormap(int);
extern void fl_dump_state_info(int, const char *);
extern void fl_init_stipples(void);

/* for fdesign */
const char *fl_query_colorname(FL_COLOR);
extern long fl_query_namedcolor(const char *s);
extern void fl_error(const char *, const char *);

extern int fl_get_pos_in_string(int, int, FL_Coord, FL_Coord, FL_Coord,
				FL_Coord, int, int, FL_Coord, FL_Coord,
				const char *, int *, int *);

extern int fl_drw_stringTAB(Window, GC, int, int,
			    int, int, const char *s, int, int);

extern int fl_drw_string(int, int,
			 FL_Coord, FL_Coord, FL_Coord, FL_Coord, int,
			 FL_COLOR, FL_COLOR, FL_COLOR, int, int, int,
			 int, int, char *, int, int, int, FL_COLOR);

extern int fl_get_maxpixel_line(void);

extern int fl_get_string_widthTABfs(XFontStruct *, const char *, int);



extern void fl_init_font(void);
extern void fl_canonicalize_rect(FL_Coord *, FL_Coord *, FL_Coord *, FL_Coord *);
extern void fl_init_goodies(void);
extern void fl_get_goodie_title(FL_FORM *, const char *);
extern void fl_init_pup(void);
extern void fl_add_q_icon(FL_Coord, FL_Coord, FL_Coord, FL_Coord);
extern void fl_add_warn_icon(FL_Coord, FL_Coord, FL_Coord, FL_Coord);
extern void fl_check_key_focus(const char *, Window);

extern Pixmap fl_read_bitmapfile(Window, const char *, unsigned *, unsigned *,
				 int *, int *);

extern char **fl_get_cmdline_args(int *);


/* just for fun */

extern int fl_check_it(const char *);
extern XRectangle *fl_get_underline_rect(XFontStruct *,
				     FL_Coord, FL_Coord, const char *, int);

/***** Routines in sldraw.c. */

typedef struct
{
    FL_Coord x, y, w, h;	/* slider           */
    FL_Coord buttw, butth;	/* line button size */
}
FL_SCROLL_BAR;

extern void fl_calc_slider_size(int, FL_Coord, FL_Coord, FL_Coord, FL_Coord,
			      int, float, float, FL_SCROLL_BAR *, int, int);
extern int fl_slider_mouse_object(int, FL_Coord, FL_Coord,
	    FL_Coord, FL_Coord, int, float, float, FL_Coord, FL_Coord, int);
extern int fl_get_pos_in_slider(FL_Coord, FL_Coord, FL_Coord, FL_Coord,
			    int, float, FL_Coord, FL_Coord, float, float *);
extern void fl_getimcolor(FL_COLOR, int *, int *, int *);

extern void fl_set_perm_clipping(FL_Coord, FL_Coord, FL_Coord, FL_Coord);
extern void fl_unset_perm_clipping(void);
extern int fl_perm_clip;
extern XRectangle fl_perm_xcr;

/* Application windows */
typedef struct fl_win_
{
    struct fl_win_ *next;
    Window win;
    FL_APPEVENT_CB pre_emptive;	/* if set, always gets called first */
    FL_APPEVENT_CB callback[LASTEvent];
    void *pre_emptive_data;
    void *user_data[LASTEvent];
    FL_APPEVENT_CB default_callback;
    unsigned long mask;
}
FL_WIN;

extern FL_WIN *fl_app_win;

extern void fl_set_object_bw(FL_OBJECT *, int);
extern void fl_set_form_window(FL_FORM *);
extern void fl_hide_canvas(FL_OBJECT *);
extern FL_APPEVENT_CB fl_set_preemptive_callback(Window, FL_APPEVENT_CB, void *);

extern Window fl_swinopen(Window, const char *);
extern Window fl_cmap_swinopen(Window, Colormap);

extern unsigned long fl_xevent_to_mask(int);
extern void fl_dump_state_info(int, const char *);
extern int fl_initialize_program_visual(void);
extern FL_OBJECT *fl_mouseobj, *fl_pushobj;

#define TIMER_RES           50	/* resolution of FL_STEP event */

/*
 * currently only one idle procedure is permitted, so the next
 * field is of no much use
 */
typedef struct fl_idle_cb_
{
    struct fl_idle_cb_ *next;
    FL_APPEVENT_CB callback;
    void *data;
}
FL_IDLE_REC;

typedef struct fl_io_event_
{
    struct fl_io_event_ *next;
    FL_IO_CALLBACK callback;
    void *data;
    unsigned mask;
    int source;
}
FL_IO_REC;

typedef struct fl_freelist_
{
    int nfree;
    int avail;
    void **data;
    int *age;
}
FL_FREE_REC;

typedef FL_SIGRET(*FL_OSSIG_HANDLER) (int);

typedef struct fl_signallist_
{
    struct fl_signallist_ *next;
    FL_SIGNAL_HANDLER callback;
    FL_OSSIG_HANDLER ocallback;	/* default OS signal handler */
    void *data;
    int signum;
    int expired;
}
FL_SIGNAL_REC;

/* timeouts */
typedef struct fl_timeout_
{
    struct fl_timeout_ *next;
    FL_TIMEOUT_CALLBACK callback;
    long msec, msec0;
    void *data;
    long sec;
    long usec;
    int id;
}
FL_TIMEOUT_REC;


extern void fl_addto_freelist(void *);
extern void fl_free_freelist(void);

/*
 *  Intenal controls.
 */
typedef struct fl_context_
{
    struct fl_context_ *next;	/* not used at the moment */

    FL_FORM_ATCLOSE atclose;	/* what to do if WM_DELETE_WINDOW */
    void *close_data;

    FL_IDLE_REC *idle_rec;	/* idle callback record   */
    FL_IO_REC *io_rec;		/* async IO      record   */
    FL_FREE_REC *free_rec;	/* stuff need to be freed */
    FL_SIGNAL_REC *signal_rec;	/* list of app signals    */
    FL_TIMEOUT_REC *timeout_rec;/* timeout callbacks      */
    int idle_delta;		/* timer resolution       */
    long mouse_button;		/* push/release record    */
    Window pup_win;		/* popup window           */
    int pup_id;			/* current active pup id  */
    FL_FORM *modal;		/* current modal form     */
    long max_request_size;	/* max protocol size      */
    int num_io;
    int hscb, vscb;		/* default scrollbar      */
    long ext_request_size;	/* extended request size  */
    int tooltip_time;
#ifdef XlibSpecificationRelease
    XIM    xim ;                /* input method           */
    XIC    xic ;                /* input context          */
#else
    void  *xim;
    void  *xic;
#endif
    unsigned int  navigate_mask;       /* input field           */
    long reserverd[6];
}
FL_CONTEXT;

/* some X info that helps to make the windowing system independent
 * API work (fl_color() etc
 */
typedef struct
{
    Display*      display;
    Window        win;
    GC            gc, textgc;
    GC            miscgc;
    int           isRGBColor;
    int           isMBFont;		/* multi-byte font */
    unsigned long bktextcolor;
    int           newpix;
    int           fdesc;		/* font descent           */
    int           fasc;		/* font ascent            */
    int           fheight;		/* font height            */
    Colormap      colormap;
    XFontStruct*  fs;
    unsigned long color;	/* last color. cache     */
    unsigned long textcolor;	/* last textcolor. cache */
    unsigned long bkcolor;
    int           screen;
}
FL_TARGET;

/* compatibility stuff */
extern int           fl_fdesc_(void), fl_fheight_(void), fl_fasc_(void);
extern Window        fl_cur_win_(void);
extern XFontStruct*  fl_cur_fs_(void);
extern GC fl_gc_(void), fl_textgc_(void);

extern void fl_init_fl_context(void);
extern void fl_free_newpixel(unsigned long pixel);

#include "extern.h"

extern Window fl_winshow(Window);
#define fl_mapwindow fl_winshow

extern void fl_watch_io(FL_IO_REC *, long);
extern int fl_is_valid_window(Window);
extern int fl_wildmat(const char *, const char *);
extern int fl_do_shortcut(FL_FORM *, int, FL_Coord, FL_Coord, XEvent *);
extern void fl_draw_object_label(FL_OBJECT *);
extern void fl_set_object_clip(FL_OBJECT *, int);
extern void fl_compress_motion(XEvent *);

extern void fl_get_hv_align(int, int *, int *);
extern void fl_get_outside_align(int, int, int, int, int, int *, int *, int *);
extern void fl_init_symbols(void);
extern unsigned long fl_fmtime(const char *);
extern char *fix_dirname(char *);
extern int fl_handle_event_callbacks(XEvent *);

#define XK_PageUp    XK_Prior
#define XK_PageDn    XK_Next

#define XK_PageUp    XK_Prior
#define XK_PageDn    XK_Next

#ifdef HAVE_KP_DEFINE
#define IsHome(k)      (k==XK_Home || k==XK_Begin || k==XK_KP_Home)
#define IsLeft(k)      (k==XK_Left || k==XK_KP_Left)
#define IsRight(k)     (k==XK_Right|| k==XK_KP_Right)
#define IsUp(k)        (k==XK_Up   || k==XK_KP_Up)
#define IsDown(k)      (k==XK_Down || k==XK_KP_Down)
#define IsEnd(k)       (k==XK_End  || k==XK_KP_End)
#define IsPageDown(k)  (k==XK_Next || k==XK_Page_Down || k==XK_KP_Page_Down)
#define IsPageUp(k)    (k==XK_Prior || k==XK_Page_Up  || k==XK_KP_Page_Up)
#else
#define IsHome(k)      (k==XK_Home || k== XK_Begin)
#define IsLeft(k)      (k==XK_Left)
#define IsRight(k)     (k==XK_Right)
#define IsDown(k)      (k==XK_Down)
#define IsUp(k)        (k==XK_Up)
#define IsEnd(k)       (k==XK_End)
#define IsPageDown(k)  (k==XK_Next)
#define IsPageUp(k)    (k==XK_Prior)
#endif

#define FL_HALFPAGE_UP         0x10000000
#define FL_HALFPAGE_DOWN       0x20000000
#define FL_NLINES_UP           0x30000000
#define FL_NLINES_DOWN         0x40000000
#define FL_1LINE_UP            0x50000000
#define FL_1LINE_DOWN          0x60000000
#define IsHalfPageUp(k)        ((k)==FL_HALFPAGE_UP)
#define IsHalfPageDown(k)      ((k)==FL_HALFPAGE_DOWN)
#define IsNLinesUp(k)          ((k)==FL_NLINES_UP)
#define IsNLinesDown(k)        ((k)==FL_NLINES_DOWN)
#define Is1LineUp(k)          ((k)==FL_1LINE_UP)
#define Is1LineDown(k)        ((k)==FL_1LINE_DOWN)


extern int fl_convert_shortcut(const char *, long *);
extern int fl_get_underline_pos(const char *, const char *);
extern void fl_get_object_bbox_rect(FL_OBJECT *, XRectangle *);
extern const XRectangle *fl_bounding_rect(const XRectangle *,
					  const XRectangle *);

extern void fl_scale_length(FL_Coord *, FL_Coord *, double);
extern int fl_is_good_form(FL_FORM *);
extern void fl_recount_auto_object(void);
extern FL_OBJECT *fl_object_qtest(void);

extern void (*fl_handle_signal) (void);
extern int fl_get_tabpixels(XFontStruct *);
extern void fl_delete_group(FL_OBJECT *);


extern int fl_get_default_scrollbarsize(FL_OBJECT *);
extern void fl_set_app_name(const char *, const char *);

extern void fl_hide_composite(FL_OBJECT *);
extern void fl_show_composite(FL_OBJECT *);
extern void fl_deactivate_composite(FL_OBJECT *);
extern void fl_activate_composite(FL_OBJECT *);
extern void fl_delete_composite(FL_OBJECT *);
extern void fl_set_composite_gravity(FL_OBJECT * ob, unsigned nw, unsigned se);
extern void fl_set_composite_resize(FL_OBJECT * ob, unsigned);
extern void fl_add_child(FL_OBJECT *, FL_OBJECT *);
extern void fl_delete_child(FL_OBJECT *);

extern void fl_parse_goodies_label(FL_OBJECT * ob, const char *name);
extern int fl_goodies_preemptive(FL_FORM * form, void *ev);
extern int fl_goodies_atclose(FL_FORM * form, void *ev);
extern void fl_get_goodies_font(int *, int *);
extern void fl_handle_goodie_font(FL_OBJECT *, FL_OBJECT *);
extern void fl_init_alert(void);

extern void fl_handle_timeouts(long);

#define FL_IS_NONSQRBOX(t) (t==FL_SHADOW_BOX ||\
                            t==FL_NO_BOX||\
                            t==FL_RFLAT_BOX||\
                            t==FL_ROUNDED_BOX||\
                            t==FL_OVAL_BOX||\
                            t==FL_ROUNDED3D_UPBOX||\
                            t==FL_ROUNDED3D_DOWNBOX )

extern void fl_set_additional_clipping(FL_Coord, FL_Coord, FL_Coord, FL_Coord);
extern const FL_RECT *fl_union_rect(const FL_RECT *, const FL_RECT *);
extern void fl_reparent_pup(int, Window);
extern void fl_getpup_window(int, Window *, Window *);
extern void fl_draw_object_outside_label(FL_OBJECT *);

/* temporary stuff */

extern FL_OBJECT *fl_create_menubar(int, FL_Coord, FL_Coord, FL_Coord,
				    FL_Coord, const char *);

extern FL_OBJECT *fl_add_menubar(int, FL_Coord, FL_Coord, FL_Coord,
				 FL_Coord, const char *);
#define M_test  M_err
#define M_dev   M_err

extern void fl_set_focuswin(Window);

extern void fl_xyplot_nice_label(float, int, float, char *);
extern void fl_xyplot_compute_data_bounds(FL_OBJECT *, int *, int *, int);
extern int fl_xyplot_interpolate(FL_OBJECT *, int, int, int);
extern void fl_insert_composite_after(FL_OBJECT *, FL_OBJECT *);
extern void fl_change_composite_parent(FL_OBJECT *, FL_OBJECT *);
extern void fl_add_composite(FL_FORM *, FL_OBJECT *);
extern FL_OBJECT *fl_get_component(FL_OBJECT *, int, int, int);
extern int fl_is_watched_io(int);
extern char *fl_del_tail_slash(char *);
extern const char *fl_object_class_name(FL_OBJECT *);
extern void fl_print_form_object(FL_FORM *, const char *);
extern void fl_mark_composite_for_redraw(FL_OBJECT *);

#include "textbox.h"		/* from private/ directory */

extern int (*fl_handle_clipboard) (void *event);

extern void fl_set_form_icon_data(FL_FORM *, char **);
extern char *fl_getcwd(char *, int);
extern void fl_get_clipping(FL_Coord *, FL_Coord *, FL_Coord *, FL_Coord *);
extern FL_OBJECT *fl_get_real_focusobj(FL_OBJECT *);
extern void fl_replacepup_text(int, int, const char *);

extern int fl_handle_mouse_wheel(FL_OBJECT *, int *, int *, void *);

#include "pvaluator.h"

extern int fl_valuator_handle_drag(FL_OBJECT *, double);
extern int fl_valuator_handle_release(FL_OBJECT *, double);
extern void *fl_init_valuator(FL_OBJECT *);
extern double fl_valuator_round_and_clamp(FL_OBJECT *, double);
extern int fl_set_valuator_return(FL_OBJECT *, int);
extern double fl_clamp(double, double, double);

extern void fl_inherit_attributes(FL_OBJECT *, FL_OBJECT *);
extern int fl_winreparentxy(Window, Window, int, int);
extern int fl_boxtype2frametype(int btype);
extern void fl_drw_broken_box(int, int, int, int, int, FL_COLOR, int);
extern void fl_xvisual2flstate(FL_State * s, XVisualInfo * xvinfo);
extern int fl_find_closest_color(int, int, int, XColor *, int, unsigned long *);
extern void fl_rgbmask_to_shifts(unsigned long, unsigned int *, unsigned int *);
extern char *fl_basename(char[]);

extern void fl_show_tooltip(const char *, int, int);
extern void fl_hide_tooltip(void);
extern void fl_do_radio_push(FL_OBJECT *, FL_Coord, FL_Coord, int, void *);
extern long fl_getpid(void);
extern void fl_xlinestyle(Display *, GC, int);
extern void fl_xdashedlinestyle(Display *, GC, const char *, int);


#define FL_BROKEN_BOX  (1<<10)

extern FL_TARGET *fl_internal_init(void);
extern void fl_switch_target(FL_TARGET *);
extern void fl_restore_target(void);

extern void fl_draw_text_inside(int align, FL_Coord x, FL_Coord y,
		     FL_Coord w, FL_Coord h, char *istr, int len, int style,
		      int size, FL_COLOR c, FL_COLOR bc, int bk, int angle);

extern void
fl_draw_text_cursor(int align, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
                    char *str, int len, int style, int size,
                    FL_COLOR c, FL_COLOR bc, FL_COLOR cc, int bk, int pos);


extern const char *fl_get_xevent_name(const XEvent *);

extern void fl_set_input_navigate(unsigned mask);
extern void fl_adjust_browser_scrollbar(FL_OBJECT *);

#if XlibSpecificationRelease==6
#  define IsTab(ksym)  (ksym==XK_ISO_Left_Tab||ksym== XK_Tab)
#else
#  define IsTab(ksym)  (ksym== XK_Tab)
#endif

#endif /* !defined FL_INTERNAL_H */
