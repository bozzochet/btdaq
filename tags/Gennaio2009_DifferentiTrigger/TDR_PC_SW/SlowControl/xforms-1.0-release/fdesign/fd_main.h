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
 * $Id: fd_main.h,v 1.0 2002/05/08 05:27:39 zhao Release $
 *
 * This file is the header file used in the Form Designer
 *
 */

#ifndef FD_MAIN_H_
#define FD_MAIN_H_

#include <stdio.h>
#include "forms.h"

#ifndef FALSE
#define FALSE    0
#define TRUE    (!FALSE)
#endif

#define MAGIC2   12321
#define MAGIC3   12322

/* general limits */
#define MAX_CLASS_NAME_LEN 64	/* FL_BUTTON etc.                     */
#define MAX_TYPE_NAME_LEN  64	/* FL_RADIO_BUTTON etc                */
#define MAX_VAR_LEN       128	/* max. length of a variable/function */

/* various options  of the program */
typedef struct
{
    int language;
    int emit_code;		/* if emit UI code */
    int emit_main;
    int emit_cb;
    int lax;
    int altformat;
    int conv_only;
    int show_pallete;
    int compensate;		/* if emit font size compensation code */
    int unit;
}
FD_Opt;


enum
{
    FD_C = 1, FD_ADA95, FD_PERL, FD_PASCAL,
    FD_PYTHON, FD_FORTRAN, FD_PS, FD_SCM, FD_EXP
};

/* we use different file selector for different tasks */
enum
{
    SAVE_FSELECTOR, LOAD_FSELECTOR, XPM_FSELECTOR
};

#define MAX_CONVERTOR  (FD_EXP+1)

/* structure that will hole all forms created */
typedef struct
{
    FL_FORM *form;		/* The form */
    char fname[MAX_VAR_LEN];	/* Its name */
}
FRM;

typedef struct
{
    const char *lang_name;
    int (*convert) (const char *s, FRM *, int);		/* built-in convertot 



							 */
    const char *extern_convertor;	/* the converter itself         */
    const char *contact_info;	/* who to contact in case of error      */
    int need_c_code;		/* if translator works on C or .fd file */
    void (*init) (void);	/* additional initialization            */
}
Conv;

extern Conv convertor[];
extern FD_Opt fdopt;


/* since V1, all coordinates are relative to the top-left */
#define FD_V1    13000
/*#define FD_V2    13001 */
#if 0
#define TRANY(obj,form) obj->y = form->h - obj->h - obj->y
#else
#define TRANY(obj,form)
#endif


#define ShiftIsDown(mask)    (mask & ShiftMask)


/******** in fd_main.c    *********/

extern long main_window;	/* The identifier of the main window */
extern FL_Coord winw, winh;
extern int changed;		/* Whether the file has changed. */
extern char main_name[];	/* Main creation routine name */

/******** in fd_rubber.c  *********/

/* Contains routines for drawing rubberband boxes. */

extern void set_step_size(float);	/* sets the stepsize */
extern float get_step_size(void);	/* returns the stepsize */

/* sets the bounding box in which the drawing occurs */
extern void set_bounding_box(float, float, float, float);

/* returns the position of the mouse in world coordinates */
extern void get_mouse_pos(float *, float *);

/* drag a box around until the user releases a mouse button */
extern void move_box(float *, float *, float *, float *, int);

/* scales a box until the user releases a left mouse button */
extern void scale_box(float *, float *, float *, float *);

/******** in fd_select.c *********/

/* Contains the routines that deal with the maintenance of the
   current selection of objects */

extern int is_selected(FL_OBJECT *);	/* Returns whether the object is
					   selected. */
extern void addto_selection(FL_OBJECT *);	/* Adds an object to a
						   selection */
extern void addgroupto_selection(FL_OBJECT *);	/* Adds a group of objects to
						   a selection */
extern void deletefrom_selection(FL_OBJECT *);	/* Deletes an object from a
						   selection */
extern void clear_selection(void);	/* Clears the current selection */
extern void resize_selection(FL_Coord, FL_Coord);
extern void move_selection(FL_Coord, FL_Coord);
extern void deletegroupfrom_selection(FL_OBJECT *);	/* Deletes a group of
							   objects from a
							   selection */
extern void draw_selbox(void);	/* Draws the box around the selected objects */
extern void handle_select(const XEvent *);	/* Change the selection when
						   user puhed mouse. */
extern void handle_move(const XEvent *);	/* Moves or scales the
						   selection. */
extern void select_all(void);	/* Selects all objectsin the form */
extern void change_selection(void);	/* Changes the atributes of the
					   selection */
extern void change_selected_objects(FL_OBJECT *);
extern void align_selection(int);	/* Aligns the objects in the
					   selection */
extern void show_selection(void);	/* Makes the selection visible. */
extern void hide_selection(void);	/* Makes the selection invisible. */
extern void raise_selection(void);	/* Raises the selection */
extern void lower_selection(void);	/* Lower the selection */
extern void cut_selection(void);	/* Deletes all objects in the
					   selection */
extern void paste_selection(void);	/* Puts elements from buffer into
					   form */
extern void copy_selection(void);	/* Copies objects in selection to
					   buffer */
extern void *dup_selection(void);
extern void set_selection(void *);
extern void free_dupped_selection(void *);
extern void next_selection(void), prev_selection(void);
extern void group_selection(void);	/* Turns objects in the selection
					   into a group. */
extern void flatten_selection(void);	/* Removes begin and end groups from
					   the selection. */

/******** in fd_attribs.c **********/

/* Contains routines to set and change attributes of objects. */

int change_object(FL_OBJECT *, int);	/* changes the settings for object */

void change_type(FL_OBJECT *, int);	/* Changes the type of a particular
					   object. */

/* sets all attributes for an object */
void set_attribs(FL_OBJECT *, int, int, int, int, int, float, int, char[]);
void set_miscattribs(FL_OBJECT *, unsigned, unsigned, unsigned);

void set_label(FL_OBJECT *, const char *);	/* Sets the object label
						   (interpreting \n's). */
void set_shortcut(FL_OBJECT *, const char *);
char *get_label(const FL_OBJECT *, int);	/* Returns the object label
						   (turning NL's into \n's). */
char *get_shortcut_string(const FL_OBJECT *);

FL_OBJECT *copy_object(FL_OBJECT *, int keep);	/* Make an exact copy of an
						   object */

/******** in fl_file.c    ********/

/* Contains all routines that deal with saving and loading forms. */

extern int read_form(FILE *, char *);
extern void write_form(FILE *, FL_FORM *, char[]);

/******** in fl_print.c   ********/

/* Contains all routines that deal with printing forms in C format. */

void print_form(FILE *, FL_FORM *, const char *);
void print_callbacks_and_globals(FILE *, FL_FORM *, int);

/* For making the header file */
void print_header(FILE *, FL_FORM *, const char *);
void output_main(FILE *, FRM *, int);
void output_callbacks(FILE *, FRM *, int);

/******** in fd_forms.c   *********/

/* Contains the routines that maintain the collection of forms. */

#define MAXFRM	   64		/* Maximal number of forms allowed. */
#define MAXGROUP   64		/* Maximal number of groups in a form. */
#define MAXOBJS    64		/* max object on a form               */

extern FL_FORM *cur_form;
/* The current form under construction */

extern void redraw_the_form(int);
/* Redraws the current form. The argument indicates whether the background
   should be redrawn. */

extern void reshape_form_background(FL_Coord, FL_Coord);

/* loads the forms from a file */
extern int load_forms(int, char *, int);

extern int save_forms(char *);
/* saves the forms to a file, returns whether actually saved */

/******** fd_groups.c *********/

/* Contains the routines that deal with groups. */


/* Fills in the groups in the current form. */
void fillin_groups(void);

/******** fd_names.c  **********/

/* Contains the routines that keep track of the object names */

void get_object_name(const FL_OBJECT *, char *, char *, char *);
/* returns the names of an object */

void set_object_name(FL_OBJECT *, const char *, const char *, const char *);
/* returns the names of an object */

/******** fd_objects.c *********/

/* Contains the different object classes and types. */

#define MAXCLASS	40	/* Max number of object class handled */
#define MAXTYPE		24	/* Max number of types per class.     */

/* Maximal number of object classes. */

/* The current class of object to be added */
extern int cur_class;

/* Initializes all the classes and types. */
extern void init_classes(void);

/* adds an object to the current form */
extern FL_OBJECT *add_an_object(int, int, FL_Coord, FL_Coord, FL_Coord, FL_Coord);

/* Returns a pointer to the name of the class */
extern char *find_class_name(int);
extern char *find_class_name(int);

/* Returns the number of types in the class */
extern int find_class_maxtype(int);

/* Returns the default object in the class */
extern FL_OBJECT *find_class_default(int, int);

/* Returns a pointer to the name of the type in the class */
extern const char *find_type_name(int, int);
extern int find_type_value(int, const char *);

extern unsigned long fd_red, fd_black, fd_white, fd_col;
extern int fd_magic;
extern int fd_border;
extern int fd_trackgeometry, fd_buttonLabelSize, fd_testborder;
extern void color(unsigned long);
extern int getbutton(int);
extern void fd_clear(int, int, int, int);
extern void rect(FL_Coord, FL_Coord, FL_Coord, FL_Coord);
extern void rectf(FL_Coord, FL_Coord, FL_Coord, FL_Coord);
extern int fl_qtest(void);

/* attribute query routines */
extern char *class_name(int);
extern int class_val(const char *);

extern char *gravity_name(int);
extern int gravity_val(const char *);

extern char *resize_name(int);
extern int resize_val(const char *);

extern char *style_name(int);
extern int style_val(const char *);

extern const char *align_name(int);
extern int align_val(const char *);

extern char *lsize_name(int);
extern int lsize_val(const char *);

extern char *boxtype_name(int);
extern int boxtype_val(const char *);
extern char *loadedfile;
extern char *loadedfile_fullpath;

typedef struct
{
    int val;			/* value       */
    char *name;			/* formal name */
    char *shown;		/* name shown  */
    char *hotkey;		/* hotkeys     */
}
VN_pair;

extern char *get_vn_name(VN_pair *, int);
extern int get_vn_val(VN_pair *, const char *);

extern VN_pair vn_btype[];
extern VN_pair vn_gravity[];
extern VN_pair vn_align[];
extern VN_pair vn_resize[];

extern int fd_align_fontsize, fd_type_fontsize;
extern int fd_convert, fd_show_palette;
extern const char *get_fd_name(const char *);
extern int fd_bwidth;
extern char *unit_name(int);
extern int unit_val(const char *);
extern int convert_u(int);
extern double get_conversion_factor(void);
extern char xform_header[];
extern void show_geometry(const char *, int, int, int, int);
extern void show_selmessage(FL_OBJECT *[], int);

enum
{
    FD_LEFT = 1,
    FD_HCENTER = 2,
    FD_RIGHT = 3,
    FD_HEQUAL = 4,
    FD_BOTTOM = 5,
    FD_VCENTER = 6,
    FD_TOP = 7,
    FD_VEQUAL = 8
};

enum
{
    HELP_VERSION,
    HELP_GENERAL,
    HELP_MOUSE,
    HELP_KEYBOARD,
    HELP_GROUP,
    HELP_TEST,
    HELP_SAVE
};

extern FL_FORM *thetestform;

extern FL_OBJECT *fl_create_simu_canvas(int, FL_Coord, FL_Coord,
					FL_Coord, FL_Coord, const char *);
extern FL_OBJECT *fl_add_simu_canvas(int, FL_Coord, FL_Coord,
				     FL_Coord, FL_Coord, const char *);

extern FL_OBJECT *fl_create_simu_glcanvas(int, FL_Coord, FL_Coord,
					  FL_Coord, FL_Coord, const char *);
extern FL_OBJECT *fl_add_simu_glcanvas(int, FL_Coord, FL_Coord,
				       FL_Coord, FL_Coord, const char *);

/* object class specific info handler */
extern void save_objclass_spec_info(FILE *, FL_OBJECT *);
extern void load_objclass_spec_info(FILE *, FL_OBJECT *);
extern void emit_objclass_spec_info(FILE *, FL_OBJECT *);
extern void emit_objclass_spec_header(FILE *, FL_OBJECT *);
extern void emit_objclass_spec_global(FILE *, FL_OBJECT *);
extern void set_objclass_spec_attributes(FL_OBJECT *, long);
extern int has_class_spec_info(FL_OBJECT *);
extern void modify_attrib_basic_color(FL_COLOR, FL_COLOR);
extern void show_attributes(const FL_OBJECT *);
extern int read_key_val(FILE * fp, char *key, char *val);

/* Control panel etc */
#include "ui_theforms.h"
#include "ui_attrib.h"

extern FD_control *fd_control;
extern FD_attrib *fd_attrib;
extern FD_generic_attrib *fd_generic_attrib;
extern FD_test *fd_test;
extern FD_align *fd_align;
extern FD_helpform *fd_help;
extern FD_nullattrib *fd_nullattrib;

extern void loadforms_cb(FL_OBJECT *, long);
extern void saveforms_cb(FL_OBJECT *, long);
extern void saveforms_as_cb(FL_OBJECT *, long);
extern void mergeforms_cb(FL_OBJECT *, long);
extern void exit_cb(FL_OBJECT *, long);
extern void changegroupname_cb(FL_OBJECT *, long);
extern void mainname_cb(FL_OBJECT *, long);

extern void create_the_forms(void);
extern void control_init(FD_control *);

extern FL_FORM *thetestform;
extern void process_xevent(int);
extern int within_selection(float, float);
extern int no_selection;
extern int C_output(const char *, FRM *, int);
extern int is_pasting;
extern void init_align(void);
extern int auto_apply;
extern void cleanup_spec(FL_OBJECT *);

/* fake stuff */
#define FL_NTABFOLDER  50
extern FL_OBJECT *fl_add_ntabfolder(int, FL_Coord, FL_Coord, FL_Coord,
				    FL_Coord, const char *);
extern FL_OBJECT *fl_create_ntabfolder(int, FL_Coord, FL_Coord, FL_Coord,
				       FL_Coord, const char *);
extern FL_OBJECT *fl_add_nformbrowser(int, FL_Coord, FL_Coord, FL_Coord,
				      FL_Coord, const char *);
extern FL_OBJECT *fl_create_nformbrowser(int, FL_Coord, FL_Coord, FL_Coord,
					 FL_Coord, const char *);
extern void make_backup(const char *);
extern int is_var_boxtype(int cn);
extern void set_testing_pixmap(FL_OBJECT *);

extern void set_snap_size(int, int);
extern int get_snap_size(void);
extern void show_pallette(void), hide_pallette(void);
extern void reset_pallette_menu_status(void);
extern void select_pallette_entry(int);
extern void reset_pallette(void);

#define MAX_CONTENT  128

typedef struct
{
    float min, max, xmin, xmax, ymin, ymax;
    float val, xval, yval;
    float step, sstep, lstep, xstep, ystep;
    float slsize;
    float thetai, thetaf;
    float ldelta, rdelta;
    int prec;
    int direction;
    int int_val;
    int how_return;
    int align;
    int dx, dy;
    int show_focus;
    int h_pref, v_pref;
    int nlines;
    char filename[512];
    char focus_filename[512];
    char data[64];
    char focus_data[64];
    char width[64], height[64];
    void *cspecv;
    int use_data;
    int fullpath;
    int xmajor, xminor;
    int ymajor, yminor;
    int xscale, yscale;
    float xbase, ybase;
    int xgrid, ygrid;
    char **content;
    char **shortcut;
    unsigned char *mode;
    int new_menuapi;
    int global_scope;
    int showsymbol;
    int cspecv_size;
    char *misc_char;
    int grid_linestyle;
    int no_title;
    int mark_active;
    char helper[512];
    int reserverd[12];
}
SuperSPEC;

extern SuperSPEC *get_superspec(FL_OBJECT *);
extern SuperSPEC *spec_to_superspec(FL_OBJECT *);
extern void *superspec_to_spec(FL_OBJECT *);
extern void copy_superspec(FL_OBJECT * dest, FL_OBJECT * src);

typedef struct
{
    int align;
    int dx, dy;
    int show_focus;
    char filename[512];
    char focus_filename[512];
    char data[64];
    char focus_data[64];
    char width[64];
    char height[64];
    int use_data;
    int fullpath;
    char helper[512];
}
IconInfo;

#define ISBUTTON(cls)  (cls==FL_BUTTON       || cls==FL_LIGHTBUTTON ||\
                        cls==FL_CHECKBUTTON  || cls==FL_ROUNDBUTTON ||\
                        cls==FL_ROUND3DBUTTON|| cls==FL_PIXMAPBUTTON||\
                        cls==FL_BITMAPBUTTON || cls==FL_SCROLLBUTTON||\
                        cls==FL_LABELBUTTON)
#define ISCHOICE(cls)  (cls==FL_CHOICE || cls==FL_MENU || cls==FL_BROWSER)

extern void select_object_by_class(int);
extern char *append_fd_suffix(const char *);
extern const char *get_placement(FL_FORM *);

extern void reset_dupinfo_cache(void);
extern int is_duplicate_info(FL_OBJECT *, const char *);
extern int noop_handle(FL_OBJECT *, int, FL_Coord, FL_Coord, int, void *);
extern char *get_helper(char *);


#endif /* FD_MAIN_H */
