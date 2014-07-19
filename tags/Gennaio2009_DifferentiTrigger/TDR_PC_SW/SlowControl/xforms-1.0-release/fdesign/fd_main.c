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
 * $Id: fd_main.c,v 1.0 2002/05/08 05:27:39 zhao Release $
 *
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 * This is the main module of the Form Designer, a program that
 * helps you to design form in a fully interactive way. It
 * automatically generates the corresponding C-code.
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "forms.h"
#include "ui_theforms.h"
#include "fd_main.h"
#include "fd_logo.xpm"

#if defined(__EMX__) || defined(WIN32)
#define strcasecmp stricmp
#endif

#if defined(__VMS) && __VMS_VER < 70000000
extern int strcasecmp(const char *, const char *);
#endif

/* Info about the program */
static const char *fd_version[] =
{
    "fdesign (FORM Designer)"
    "$State: Release $  $Revision: 1.0 $ of $Date: 2002/05/08 05:27:39 $",
    "Copyright (c) 1996-2002 by T.C. Zhao and Mark Overmars", 0
};


static void
print_version(int die)
{
    const char **p = fd_version;

    for (; *p; p++)
	fprintf(stderr, "%s\n", fl_rm_rcs_kw(*p));
    if (die)
	exit(0);
}

char xform_header[128] = "forms.h";
long main_window = 0;		/* The main screen window */
FL_Coord winw, winh;

/* various options  of the program */
FD_Opt fdopt;
Conv convertor[MAX_CONVERTOR + 1];

int fd_cntlborder;
int help;			/* if convert only */
int fd_bwidth;
int is_pasting;
int fd_trackgeometry = 1;
int fd_show_palette;
int fd_pversion;
int fd_buttonLabelSize;
int fd_helpfontsize = 14;
int fd_align_fontsize = FL_TINY_SIZE;
int fd_type_fontsize = FL_TINY_SIZE;
int no_selection = 0;

GC fd_gc;
Colormap fd_colormap;
unsigned long fd_red, fd_black, fd_white, fd_col;
Display *fd_display;
int changed = 0;		/* Whether the thing has changed. */
char main_name[MAX_VAR_LEN];	/* Name of the main calling routine */
char *loadedfile;
char *loadedfile_fullpath;

/* Lets the user add some object to the form */

static void
add_something(void)
{
    FL_OBJECT *obj;
    float xx, yy, ww = 0.0, hh = 0.0;

    if (cur_form == NULL)
    {
	fl_show_alert("Warning", "Please Add a form first", "", 0);
	return;
    }

    if (cur_class == -1)
    {
	fl_show_alert("Warning", "Please select what object to add first", "", 0);
	return;
    }

    fl_winset(main_window);
    get_mouse_pos(&xx, &yy);
    scale_box(&xx, &yy, &ww, &hh);

    if (ww < 5.0 || hh < 5.0)
	return;

    obj = add_an_object(cur_class, -1, xx, yy, ww, hh);
    clear_selection();
    addto_selection(obj);
    changed = 1;
    /* reset current selection */
    fl_deselect_browser(fd_control->objectbrowser);
    reset_pallette();
    cur_class = -1;
}

/* Changes the main calling name. */
void
mainname_cb(FL_OBJECT * obj, long arg)
{
    const char *s;

    if (!(s = fl_show_input("Main creation routine name:", main_name)))
	return;

    strcpy(main_name, s);
    if (main_name[0] == '\0')
	strcpy(main_name, "create_the_forms");
    changed = 1;
}


static int
double_click(const XEvent * xev)
{
    static Time lasttime;
    static int lastbut, lastx, lasty;
    int dblclk, x = xev->xbutton.x, y = xev->xbutton.y;

    dblclk = xev->xbutton.button == lastbut &&
	(xev->xbutton.time - lasttime) < FL_CLICK_TIMEOUT &&
	lastx == x && lasty == y;

    lasttime = xev->xbutton.time;
    lastbut = xev->xbutton.button;
    lastx = x;
    lasty = y;

    return dblclk;
}

/* mouse click inside the working window */
static int
handle_click(XEvent * xev, void *p)
{
    int dblclk = double_click(xev);

    if (no_selection)
	return 0;

    if (dblclk)
    {
	if (xev->xbutton.button == 1)
	{
	    cut_selection();
	}
	else if (xev->xbutton.button == 3)
	{
	    change_selection();
	}
	redraw_the_form(0);
	return 0;
    }


    if (xev->xbutton.button == 1 && cur_class != -1)
    {
	XEvent ev;

	XNextEvent(fl_display, &ev);
	if (ev.type == MotionNotify)
	    add_something();
    }
    else if (is_pasting)
    {
	;
    }
    else if (within_selection(xev->xbutton.x, xev->xbutton.y) &&
	     xev->xbutton.button != 3)
    {
	XEvent ev;

	XNextEvent(fl_display, &ev);
	if (ev.type == MotionNotify)
	    handle_move(xev);
	else if (ev.type == ButtonRelease)
	    handle_select(&ev);
    }
    else
	handle_select(xev);

    redraw_the_form(0);
    return 0;
}

static void
handle_keypress(const XKeyEvent * xev)
{
    KeySym keysym;
    char keybuf[128];
    static int stp = 5;
    int shift = ShiftIsDown(xev->state);

    XLookupString((XKeyEvent *) xev, keybuf, sizeof(keybuf), &keysym, 0);

    if (keysym >= XK_F1 && keysym <= XK_F12)
	func_cb(0, keysym - XK_F1 + 1);
    else if (IsRight(keysym))
	(shift ? resize_selection : move_selection) (stp, 0);
    else if (IsLeft(keysym))
	(shift ? resize_selection : move_selection) (-stp, 0);
    else if (IsUp(keysym))
	(shift ? resize_selection : move_selection) (0, -stp);
    else if (IsDown(keysym))
	(shift ? resize_selection : move_selection) (0, stp);
    else if (keysym == XK_Escape)
	exit_cb(0, 0);
    else if (keysym == XK_t || keysym == XK_T)
	(fd_test->test->visible ? stoptest_cb : test_cb) (0, 0);
    else if (keysym == XK_s || keysym == XK_S)
	saveforms_as_cb(0, 0);
    else if (IsTab(keysym))
    {
	(shift ? prev_selection : next_selection) ();
    }
    else if (keysym >= '0' && keysym <= '9')
    {
	if ((stp = keysym - '0') == 0)
	    stp = 10;
/*      set_snap_size(stp, 1); */
    }
}

/*
 * Process events on the main window. Since rubber band was implemented
 * badly, can't use buttonpress callback
 */
void
process_xevent(int no_add)
{
    XEvent xev;

    fl_XNextEvent(&xev);
    fl_xevent_name("fd_main", &xev);
    fl_winset(main_window);

    switch (xev.type)
    {
    case KeyPress:
	handle_keypress((XKeyEvent *) & xev);
	break;
    }
}

#if 0
static int
PgroupID(FL_OBJECT * ob, void *d)
{

    fprintf(stderr, "obj %s: class=%s groupID=%d pushed=%d\n",
	ob->label, find_class_name(ob->objclass), ob->group_id, ob->pushed);
    return 0;
}
#endif

/* The main event handling loop. */
static void
main_loop(void)
{
    FL_OBJECT *obj;
    char str[MAX_VAR_LEN], name[MAX_VAR_LEN];
    char cbname[MAX_VAR_LEN], argname[MAX_VAR_LEN];

    while (1)
    {

	obj = fl_do_forms();

	/* Check whether testing a form */
	if (thetestform != NULL && obj != FL_EVENT)
	{
#if 0
	    static ii;
	    if (ii < 5)
	    {
		ii++;
		fl_for_all_objects(obj->form, PgroupID, 0);
	    }
#endif
	    get_object_name(obj, name, cbname, argname);
	    if (name[0] == '\0')
		strcpy(name, "<noname>");
	    if (cbname[0] != '\0')
		sprintf(str, "Called: %s(%s,%s);", cbname, name, argname);
	    else
		sprintf(str, "Returned: %s", name);
	    fl_addto_browser(fd_test->browser, str);
	}

	if (obj == FL_EVENT)
	    process_xevent(0);
    }
}


#define DEFGeom       "320x250+1-293"	/* working area geometry     */
#define DEFcntlGeom   "+150+1"	/* control panel position    */
#define DEFattribGeom "+210+25"	/* Attributes panel position */
#define DEFtestGeom   "-275+2"	/* Test panel position       */
#define DEFalignGeom  "-1+1"	/* align panel position       */
#define DEFhelpGeom   "-1+1"	/* Help panel position       */

static FL_CMD_OPT fd_cmdopt[] =
{
    {"-geometry", "*geometry", XrmoptionSepArg, 0},
    {"-border", ".XForm.Border", XrmoptionNoArg, "1"},
    {"-convert", ".convert", XrmoptionNoArg, "1"},
    {"-unit", "*unit", XrmoptionSepArg, 0},
    {"-altformat", "*altformat", XrmoptionNoArg, "1"},
    {"-I", "*xformHeader", XrmoptionSepArg, 0},
    {"-main", "*main", XrmoptionNoArg, "1"},
    {"-callback", "*callback", XrmoptionNoArg, "1"},
    {"-lax", "*lax", XrmoptionNoArg, "1"},
    {"-nocode", "*nocode", XrmoptionNoArg, "0"},
    {"-version", ".fdversion", XrmoptionNoArg, "1"},
    {"-compensate", ".compensate", XrmoptionNoArg, "1"},
    {"-ada", ".language", XrmoptionNoArg, "ada95"},
    {"-ada95", ".language", XrmoptionNoArg, "ada95"},
    {"-perl", ".language", XrmoptionNoArg, "perl"},
    {"-python", ".language", XrmoptionNoArg, "python"},
    {"-fortran", ".language", XrmoptionNoArg, "fortran"},
    {"-pascal", ".language", XrmoptionNoArg, "pascal"},
    {"-scm", ".language", XrmoptionNoArg, "scm"},
    {"-ps", ".language", XrmoptionNoArg, "ps"},
    {"-filter", ".filter", XrmoptionSepArg, 0},
    {"-help", ".help", XrmoptionNoArg, "1"}
};
#define Ncopt (sizeof(fd_cmdopt)/sizeof(fd_cmdopt[0]))

/* resources */
#define NG     64
static char fdgeom[NG] = DEFGeom;
static char fdcntlgeom[NG] = DEFcntlGeom;
static char fdattribgeom[NG] = DEFattribGeom;
static char fdtestgeom[NG] = DEFtestGeom;
static char fdaligngeom[NG] = DEFalignGeom;
static char fdhelpgeom[NG] = DEFhelpGeom;
static char fd_attrib_col[NG];
static char fd_sunit[32];
static char fd_slanguage[32];
static char fd_sfilter[32];

static FL_resource fdres[] =
{
    {"formDesign.geometry", "Geometry", FL_STRING, fdgeom, 0, NG},
    {"workingArea.geometry", "Geometry", FL_STRING, fdgeom, 0, NG},
    {"control.geometry", "Control.Geometry", FL_STRING, fdcntlgeom, 0, NG},
    {"attributes.geometry", "Attributes.Geometry", FL_STRING, fdattribgeom, 0, NG},
    {"attributes.background", "Attributes.Background", FL_STRING, fd_attrib_col, 0, NG},
    {"test.geometry", "Test.Geometry", FL_STRING, fdtestgeom, 0, NG},
    {"help.geometry", "Help.Geometry", FL_STRING, fdhelpgeom, 0, NG},
    {"align.geometry", "Align.Geometry", FL_STRING, fdaligngeom, 0, NG},
    {"control.border", "XForm.Border", FL_BOOL, &fd_cntlborder, "0"},
    {"convert", "Convert", FL_BOOL, &fdopt.conv_only, "0"},
    {"compensate", "Compensate", FL_BOOL, &fdopt.compensate, "0"},
    {"unit", "Unit", FL_STRING, fd_sunit, "pixel", 30},
    {"language", "Language", FL_STRING, fd_slanguage, "C", 30},
    {"filter", "Filter", FL_STRING, fd_sfilter, 0, 30},
    {"xformHeader", "XFormHeader", FL_STRING, xform_header, "forms.h", 128},
    {"altformat", "AltFormat", FL_BOOL, &fdopt.altformat, "0"},
    {"helpFontSize", "HelpFontSize", FL_INT, &fd_helpfontsize, "12"},
    {"nocode", "NoCode", FL_BOOL, &fdopt.emit_code, "1"},
    {"main", "Main", FL_BOOL, &fdopt.emit_main, "0"},
    {"callback", "Callback", FL_BOOL, &fdopt.emit_cb, "0"},
    {"lax", "Lax", FL_BOOL, &fdopt.lax, "0"},
};

#define Nropt  (sizeof(fdres)/sizeof(fdres[0]))

char *helps[] =
{
    "-help                     this message",
    "-display host:dpy         display name",
    "-name appname             change app name",
    "-border                   add border to control panel",
    "-unit {mm|point|pixel|cp|cmm} unit of measure. Default pixel",
    "-nocode                   supress code generation",
    "-main                     emit main program template",
    "-callback                 emit callback stubs",
    "-altformat                select alternative output format",
    "-compensate               emit font/server compensation code",
    "-lax                      go easy on syntax checking",
    "-I headername             alternate header file (forms.h default)",
    "-convert file-list        convert .fd to code non-interactively",
    "-geometry geom            initial working area geometry",
    0,
};

static void
usage(char *s, int die)
{
    char **q = helps;
    Conv *cv = convertor;

    fprintf(stderr, "usage: %s: [options] [file.fd]\n", s);
    fprintf(stderr, "  where options are\n");
    for (; *q; q++)
	fprintf(stderr, "   %s\n", *q);

    fprintf(stderr, "  Language Options\n   ");
    for (cv += 2; cv->lang_name; cv++)
	fprintf(stderr, "-%s ", cv->lang_name);

    fprintf(stderr, "\n   -filter f       filter name");
    fprintf(stderr, "\n\n See also the online Help for resources info\n");

    if (die)
	exit(1);
}

static int
lang_val(const char *s)
{
    int i;

    for (i = 1; i < MAX_CONVERTOR; i++)
	if (strcasecmp(s, convertor[i].lang_name) == 0 ||
	    strncmp(s, convertor[i].lang_name, 3) == 0)		/* compatibility 







								 */
	    return i;
    return FD_C;
}


static int ignored_fake_configure;

static int
handle_configure(XEvent * xev, void *p)
{
    /* some window managers sends bogus configure events. Make a note of it */
    if (xev->xconfigure.send_event)
	ignored_fake_configure = (winw != xev->xconfigure.width &&
				  winh != xev->xconfigure.height);
    else
    {
	winw = xev->xconfigure.width;
	winh = xev->xconfigure.height;
    }
    return 1;
}

static int
handle_expose(XEvent * xev, void *p)
{
    if (xev->xexpose.count == 0)
    {
	M_warn("FD_Expose", "");

	/* if we have ignored a previous configure event, need the window
	   size might not be correct. Re-read */

	if (ignored_fake_configure)
	    fl_get_winsize(main_window, &winw, &winh);

	/* change form background size */
	if (cur_form && (cur_form->w != winw || cur_form->h != winh))
	    reshape_form_background(winw, winh);

	redraw_the_form(1);
	ignored_fake_configure = 0;
	return 1;
    }
    return 0;
}

/*
 * Need the ability to convert/help without requiring an X connection.
 */
static void
pre_connect(int ac, char *av[])
{
    int i, s;
    static char filter[128];

    for (i = 1; i < ac && *av[i] == '-'; i++)
    {
	if (strncmp(av[i] + 1, "help", 1) == 0)
	    usage(av[0], 1);
	else if (strncmp(av[i] + 1, "version", 4) == 0)
	    print_version(1);
	else if (strncmp(av[i] + 1, "altformat", 3) == 0)
	    fdopt.altformat = 1;
	else if (strncmp(av[i] + 1, "callback", 3) == 0)
	    fdopt.emit_cb = 1;
	else if (strncmp(av[i] + 1, "compensate", 4) == 0)
	    fdopt.compensate = 1;
	else if (strncmp(av[i] + 1, "main", 3) == 0)
	    fdopt.emit_main = 1;
	else if (strncmp(av[i] + 1, "noc", 3) == 0)
	    fdopt.emit_code = 0;
	else if (strncmp(av[i] + 1, "unit", 4) == 0 && (i + 1) < ac)
	    fdopt.unit = unit_val(av[++i]);
	else if (strncmp(av[i] + 1, "filter", 4) == 0 && (i + 1) < ac)
	    strcpy(filter, av[++i]);
	else if (strcmp(av[i], "-I") == 0 && (i + 1) < ac)
	    strcpy(xform_header, av[++i]);
	else if (strcmp(av[i], "-bw") == 0 && (i + 1) < ac)
	    fd_bwidth = atoi(av[++i]);
	else if (strncmp(av[i] + 1, "convert", 1) == 0)
	{
	    fl_set_no_connection(1);
	    fdopt.conv_only = 1;
	}
	else if (strncmp(av[i] + 1, "lang", 4) == 0 && (i + 1) < ac)
	    fdopt.language = lang_val(av[++i]);
	else
	{
	    fdopt.language = lang_val(av[i] + 1);
	}
    }

    if (fdopt.language < FD_C || fdopt.language > FD_EXP)
	fdopt.language = FD_C;

    if (filter[0])
	convertor[fdopt.language].extern_convertor = filter;

    if (convertor[fdopt.language].init)
	convertor[fdopt.language].init();

    /* no need to check for C syntax if not c program */
    if (fdopt.language != FD_C)
	fdopt.lax = 1;

    if (fdopt.conv_only)
    {
	fl_set_app_name(av[0], "Fdesign");	/* resource routine want this 







						 */
	fl_init_fl_context();
	create_the_forms();
	init_classes();

	fl_dpi = 95;

	if (i > ac - 1)
	{
	    fprintf(stderr, "-convert requires arguments\n");
	    usage(av[0], 1);
	}

	for (s = i; s < ac; s++)
	{
	    if (load_forms(FALSE, av[s], 0) >= 0)
		save_forms(av[s]);
	}
	exit(0);
    }
}

static int
parse_geometry(const char *gstr, int *x, int *y,
	       unsigned int *w, unsigned int *h)
{
    int s;

    s = XParseGeometry((char *) gstr, x, y, w, h);
    if ((s & XValue) && (s & XNegative))
	*x = fl_scrw - FL_abs(*x);
    if ((s & YValue) && (s & YNegative))
	*y = fl_scrh - FL_abs(*y);
    return s;
}

#include <signal.h>
static void
interrupted(int s, void *data)
{
    exit_cb(0, 0);
}

/*
 * default action for delete window client message
 */
static int
delete_handler(FL_FORM * form, void *data)
{
    if (changed)
    {
	if (fl_show_question("Changes have not been saved\nSave them now ?", 1))
	{
	    if (!save_forms(NULL))
		return FL_IGNORE;
	}
    }
    exit(0);
    return 0;			/* shut compiler */
}

/* always out perl code in altformat */
static void
perl_init(void)
{
    fdopt.altformat = 1;
}

static void
initialize(void)
{
    fdopt.conv_only = 0;
    fdopt.language = FD_C;
    fdopt.emit_code = 1;

    convertor[FD_EXP].lang_name = "exp";
    convertor[MAX_CONVERTOR].lang_name = 0;

    convertor[FD_C].lang_name = "C";
    convertor[FD_C].convert = C_output;

    convertor[FD_ADA95].lang_name = "ada";
    convertor[FD_ADA95].extern_convertor = "flc2ada";
    convertor[FD_ADA95].need_c_code = 1;

    convertor[FD_PASCAL].lang_name = "pascal";
    convertor[FD_PASCAL].extern_convertor = "fd2pascal";

    convertor[FD_PS].lang_name = "ps";
    convertor[FD_PS].extern_convertor = "fd2ps";

    convertor[FD_PERL].lang_name = "perl";
    convertor[FD_PERL].need_c_code = 1;
    convertor[FD_PERL].extern_convertor = "fd2perl";
    convertor[FD_PERL].init = perl_init;

    convertor[FD_PYTHON].lang_name = "python";
    convertor[FD_PYTHON].extern_convertor = "fd2python";

    convertor[FD_FORTRAN].lang_name = "fortran";
    convertor[FD_FORTRAN].extern_convertor = "fd2fortran";

    convertor[FD_SCM].lang_name = "scm";
    convertor[FD_SCM].extern_convertor = "fd2scm";
}

int
main(int ac, char *av[])
{
    unsigned int w = 0, h = 0;
    int x = 0, y = 0, s, version, revision;
    FL_IOPT cntl;
    unsigned mask;

    /* before doing anything, check to make sure the library is made
       correctly */

    if (FL_INCLUDE_VERSION != fl_library_version(&version, &revision))
    {
	fprintf(stderr, "Library and header mismatch\n");
	fprintf(stderr, "  Header Version:%d.%d\n", FL_VERSION, FL_REVISION);
	fprintf(stderr, " Library version:%d.%d\n", version, revision);
    }

    initialize();

    /* For conversion and usage help, we don't need a connection */
    pre_connect(ac, av);

    /* force fdesign to come up in default visual */
    cntl.vclass = FL_DefaultVisual;
    cntl.doubleBuffer = 0;
    cntl.browserFontSize = FL_NORMAL_FONT;
    mask = FL_PDVisual | FL_PDDouble | FL_PDBrowserFontSize;

    cntl.sharedColormap = 1;
    mask |= FL_PDSharedMap;

    fl_set_defaults(mask, &cntl);

    if (!(fd_display = fl_initialize(&ac, av, 0, fd_cmdopt, Ncopt)))
	exit(1);

    fl_get_app_resources(fdres, Nropt);
    fl_add_signal_callback(SIGINT, interrupted, 0);

    if (fd_pversion)
	print_version(1);

    if (help)
	usage(av[0], 1);

    if (av[1] && av[1][0] == '-')
    {
	fprintf(stderr, " Unknown option: %s\n", av[1]);
	usage(av[0], 1);
    }


    fl_cntl.coordUnit = FL_COORD_PIXEL;
    fdopt.unit = unit_val(fd_sunit);
    fdopt.language = lang_val(fd_slanguage);

    if (fd_sfilter[0])
	convertor[fdopt.language].extern_convertor = fd_sfilter;

    /* no need to check for C syntax if not c program */
    if (fdopt.language != FD_C)
	fdopt.lax = 1;

    if (convertor[fdopt.language].init)
	convertor[fdopt.language].init();

    fd_bwidth = fl_cntl.borderWidth;
    M_warn("fdesign", "UNIT=%s BW=%d", unit_name(fdopt.unit), fd_bwidth);

    create_the_forms();
    fl_set_atclose(delete_handler, 0);

    /* must suspend user requested button label font throughout fdesign and
       enable it only when testing */

    fd_buttonLabelSize = fl_cntl.buttonFontSize;
    fl_cntl.buttonFontSize = 0;


    /* Initialize stuff */
    init_classes();

    if (fdopt.conv_only)
    {
	if (ac == 1)
	    fprintf(stderr, "%s: -convert requires arguments\n", av[0]);

	for (s = 1; s < ac; s++)
	{
	    load_forms(FALSE, av[s], 0);
	    save_forms(av[s]);
	}
	exit(0);
    }

    fl_set_counter_bounds(fd_align->snapobj, 0.0, 500.0);
    fl_set_counter_step(fd_align->snapobj, 1.0, 5.0);
    fl_set_counter_value(fd_align->snapobj, 10.0);
    fl_set_counter_precision(fd_align->snapobj, 0);
    thetestform = NULL;
    strcpy(main_name, "create_the_forms");

    /* load files */

    /* if only on argument and the file does not exist, we can assume that
       the intention is to create a new file so we don't bother to try to
       load it */
    if (ac >= 2 && access(append_fd_suffix(av[ac - 1]), R_OK) == 0)
    {
	for (s = 1; s < ac; s++)
	    load_forms(s == 1 ? FALSE : TRUE, av[s], 0);
    }
    else
    {
	select_object_by_class(FL_BUTTON);
	select_pallette_entry(FL_BUTTON);
    }

    /* do auto-naming for single file only */
    if (ac == 2)
	loadedfile = fl_strdup(av[1]);

    /* see if color change */
    if (fd_attrib_col[0])
    {
	long c = FL_FREE_COL1 - 10;
	fl_mapcolor_name(c, fd_attrib_col);
	modify_attrib_basic_color(c, c);
    }

    parse_geometry(fdcntlgeom, &x, &y, &w, &h);
    fl_set_form_position(fd_control->control, x, y);

    fl_set_app_mainform(fd_control->control);
    fl_set_form_icon_data(fd_control->control, fd_logo_pixels);
    fl_show_form(fd_control->control, FL_PLACE_GEOMETRY,
	    (1 || fd_cntlborder) ? FL_FULLBORDER : FL_TRANSIENT, "Control");

    /* other geometries */
    parse_geometry(fdattribgeom, &x, &y, &w, &h);
    fl_set_form_position(fd_attrib->attrib, x, y);

    parse_geometry(fdtestgeom, &x, &y, &w, &h);
    fl_set_form_position(fd_test->test, x, y);

    parse_geometry(fdaligngeom, &x, &y, &w, &h);
    fl_set_form_position(fd_align->align, x, y);

    parse_geometry(fdhelpgeom, &x, &y, &w, &h);
    fl_set_form_position(fd_help->helpform, x, y);

    s = parse_geometry(fdgeom, &x, &y, &w, &h);
    if ((s & XValue) || (s & YValue))
	fl_winposition(x, y);
    if ((s & WidthValue) && (s & HeightValue))
	fl_initial_winsize(w, h);

    /* if a form is already loaded, use that size */
    if (cur_form)
    {
	y -= (cur_form->h - h);
	w = cur_form->w;
	h = cur_form->h;
	fl_initial_wingeometry(x, y, w, h);
    }

    winw = w;
    winh = h;
    fd_colormap = fl_state[fl_vmode].colormap;
    fd_white = fl_get_flcolor(FL_WHITE);
    fd_red = fl_get_flcolor(FL_RED);
    fd_black = fl_get_flcolor(FL_BLACK);
    fd_col = fl_get_flcolor(FL_COL1);

    main_window = fl_cmap_winopen(fl_root, fd_colormap, "Form Design");

    /* set default constraint for the window and snap */
    fl_winstepunit(main_window,
	     (int) (get_step_size() + 0.1), (int) (get_step_size() + 0.01));
    fl_set_fselector_transient(1);

    fl_add_event_callback(main_window, Expose, handle_expose, 0);
    fl_add_event_callback(main_window, ConfigureNotify, handle_configure, 0);
    fl_add_event_callback(main_window, ButtonPress, handle_click, 0);
    fl_addto_selected_xevent(main_window, ButtonMotionMask | PointerMotionHintMask);
    fl_addto_selected_xevent(main_window, ButtonReleaseMask);

    redraw_the_form(1);

    /* GC is valid only after at least one window is created */
    fd_gc = fl_state[fl_vmode].gc[8];

    main_loop();
    return 0;
}

void
set_snap_size(int n, int cb)
{
    fl_set_counter_value(fd_align->snapobj, (double) (n > 0 ? n : 5));
    if (cb)
	fl_call_object_callback(fd_align->snapobj);
}

int
get_snap_size(void)
{
    return (int) (0.01 + fl_get_counter_value(fd_align->snapobj));
}
