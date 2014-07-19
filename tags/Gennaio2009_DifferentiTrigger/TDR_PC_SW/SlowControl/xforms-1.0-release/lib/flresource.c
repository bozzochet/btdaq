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
 * $Id: flresource.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao
 *  All rights reserved.
 *.
 *
 *  Handle XResources. Only rudimetry support is implemented. However,
 *  using this minimum implementation, more powerful and application
 *  specific resources can be developed easily.
 *
 */
#if defined(F_ID) || defined(DEBUG)
char *fl_id_rsc = "$Id: flresource.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include "forms.h"
#include <X11/Xresource.h>
#include <ctype.h>
#include <locale.h>
#ifdef FL_WIN32
#include <X11/Xw32defs.h>
#endif

/* used to have other uses, currently does almost nothing */
extern int fl_check_it(const char *);

static XrmDatabase fldatabase;	/* final merged database */
static XrmDatabase cmddb;	/* command line database */
static char *fl_app_name, *fl_app_class, *fl_ori_app_name;
static void fl_init_resources(void);

/*
 * Command line options
 */
static XrmOptionDescRec copt[] =
{
    {"-debug", "*debug", XrmoptionSepArg, (caddr_t) "0"},
    {"-flversion", "*flversion", XrmoptionNoArg, (caddr_t) "1"},
    {"-name", ".name", XrmoptionSepArg, (caddr_t) 0},
    {"-display", ".display", XrmoptionSepArg, (caddr_t) 0},
    {"-sync", "*sync", XrmoptionNoArg, (caddr_t) "1"},
    {"-depth", "*depth", XrmoptionSepArg, (caddr_t) 0},
    {"-visual", "*visual", XrmoptionSepArg, (caddr_t) 0},
    {"-private", "*privateColormap", XrmoptionNoArg, (caddr_t) "1"},
    {"-shared", "*sharedColormap", XrmoptionNoArg, (caddr_t) "1"},
    {"-standard", "*standardColormap", XrmoptionNoArg, (caddr_t) "1"},
    {"-stdcmap", "*standardColormap", XrmoptionNoArg, (caddr_t) "1"},
    {"-double", "*doubleBuffer", XrmoptionNoArg, (caddr_t) "1"},
    {"-bw", "*borderWidth", XrmoptionSepArg, (caddr_t) 0},
    {"-vid", "*visualID", XrmoptionSepArg, (caddr_t) 0},

#ifdef DO_GAMMA_CORRECTION
    {"-rgamma", "*rgamma", XrmoptionSepArg, (caddr_t) 0},
    {"-bgamma", "*bgamma", XrmoptionSepArg, (caddr_t) 0},
    {"-ggamma", "*ggamma", XrmoptionSepArg, (caddr_t) 0},
#endif

/*    {"-single", "*doubleBuffer", XrmoptionNoArg, (caddr_t) "0"}, */
    {"-bs", "*backingStore", XrmoptionNoArg, (caddr_t) "0"}
};

#define Ncopt  (sizeof(copt)/sizeof(copt[0]))

/*
 * Other resources
 */
#define PV(a)  &(fl_cntl.a)
#define PS(a)  (fl_cntl.a)
#define NS(a)   #a

/* xform and XForm will be generated on the fly */
typedef char Bop[8];		/* Boolean default */
typedef char Iop[8];		/* Integer default */
static Bop OpPrivateMap, OpSharedMap, OpStandardMap, OpDouble;
static Bop OpSync, OpULW = "1";
static Iop OpDebug, OpDepth, OpULT = "-1";
static char OpBS[12] = "1";	/* whenmapped */
static char OpSafe[12];
static char OpSCBT[16] = "thin";
static char OpBLsize[20] = NS(FL_DEFAULT_SIZE);
static char OpMLsize[20] = NS(FL_DEFAULT_SIZE);
static char OpBrFsize[20] = NS(FL_DEFAULT_SIZE);
static char OpChFsize[20] = NS(FL_DEFAULT_SIZE);
static char OpSLsize[20] = NS(FL_DEFAULT_SIZE);
static char OpLLsize[20] = NS(FL_DEFAULT_SIZE);
static char OpILsize[20] = NS(FL_DEFAULT_SIZE);
static char OpIBW[20] = NS(FL_BOUND_WIDTH);
static char OpPsize[20] = NS(FL_DEFAULT_SIZE);
static char OpVisualID[20] = "0";

#ifdef DO_GAMMA_CORRECTION
static char OpRgamma[12] = "1";
static char OpGgamma[12] = "1";
static char OpBgamma[12] = "1";
#endif

static char OpCoordUnit[32];

static FL_VN_PAIR vn_coordunit[] =
{
    {FL_COORD_PIXEL, "pixel"},
    {FL_COORD_MM, "mm"},
    {FL_COORD_POINT, "point"},
    {FL_COORD_centiPOINT, "centipoint"},
    {FL_COORD_centiPOINT, "cp"},
    {FL_COORD_centiMM, "centimm"},
    {FL_COORD_centiMM, "cmm"},
    {-1, "Invalid"}
};



#define SetR(name,rclass,type,deft, buflen) \
       {#name,rclass,type,&(fl_cntl.name),deft, buflen}

static FL_resource internal_resources[] =
{
    SetR(debug, "Debug", FL_INT, OpDebug, 0),
    SetR(sync, "Sync", FL_BOOL, OpSync, 0),

#ifdef DO_GAMMA_CORRECTION
    SetR(rgamma, "Gamma", FL_FLOAT, OpRgamma, 0),
    SetR(ggamma, "Gamma", FL_FLOAT, OpGgamma, 0),
    SetR(bgamma, "Gamma", FL_FLOAT, OpBgamma, 0),
#endif

    SetR(depth, "Depth", FL_INT, OpDepth, 0),
    {"visual", "Visual", FL_STRING, PS(vname), PS(vname), 20},
    {"scrollbarType", "ScrollbarType", FL_STRING, OpSCBT, OpSCBT, 16},
    SetR(doubleBuffer, "DoubleBuffer", FL_BOOL, OpDouble, 0),
    SetR(ulThickness, "ULThickness", FL_INT, OpULT, 0),
    SetR(privateColormap, "PrivateColormap", FL_BOOL, OpPrivateMap, 0),
    SetR(sharedColormap, "SharedColormap", FL_BOOL, OpSharedMap, 0),
    SetR(standardColormap, "StandardColormap", FL_BOOL, OpStandardMap, 0),
    SetR(buttonFontSize, "FontSize", FL_INT, OpBLsize, 0),
    SetR(menuFontSize, "FontSize", FL_INT, OpMLsize, 0),
    SetR(choiceFontSize, "FontSize", FL_INT, OpChFsize, 0),
    SetR(browserFontSize, "FontSize", FL_INT, OpBrFsize, 0),
    SetR(labelFontSize, "FontSize", FL_INT, OpLLsize, 0),
    SetR(sliderLabelSize, "FontSize", FL_INT, OpSLsize, 0),
    SetR(inputLabelSize, "FontSize", FL_INT, OpILsize, 0),
    SetR(pupFontSize, "PupFontSize", FL_INT, OpPsize, 0),
    SetR(borderWidth, "BorderWidth", FL_INT, OpIBW, 0),
    SetR(ulPropWidth, "ULWidth", FL_BOOL, OpULW, 0),
    SetR(backingStore, "BackingStore", FL_INT, OpBS, 0),
    SetR(safe, "Safe", FL_INT, OpSafe, 0),
    {"coordUnit", "CoordUnit", FL_STRING, OpCoordUnit, OpCoordUnit, 32},
    {"visualID", "VisualID", FL_LONG, &fl_requested_vid, OpVisualID, 0}
};

#define Niopt (sizeof(internal_resources)/sizeof(internal_resources[0]))

/*
 * Program can set its own default, overriding XForms default
 */

#define SetMember(a)   fl_cntl.a = cntl->a
#define GetMember(a)   cntl->a

void
fl_set_defaults(unsigned long mask, FL_IOPT * cntl)
{
    if ((mask & FL_PDPrivateMap))
    {
	SetMember(privateColormap);
	sprintf(OpPrivateMap, "%d", (cntl->privateColormap != 0));
    }

    if ((mask & FL_PDSharedMap))
    {
	SetMember(sharedColormap);
	sprintf(OpSharedMap, "%d", (cntl->sharedColormap != 0));
    }

    if ((mask & FL_PDStandardMap))
    {
	SetMember(standardColormap);
	sprintf(OpStandardMap, "%d", (cntl->standardColormap != 0));
    }

    if ((mask & FL_PDDouble))
    {
	SetMember(doubleBuffer);
	sprintf(OpDouble, "%d", (cntl->doubleBuffer != 0));
    }

    if ((mask & FL_PDDepth))
    {
	SetMember(depth);
	sprintf(OpDepth, "%d", cntl->depth);
    }

    if ((mask & FL_PDVisual))
    {
	SetMember(vclass);
	strcpy(fl_cntl.vname, fl_vclass_name(cntl->vclass));
    }

    if ((mask & FL_PDButtonFontSize))
    {
	SetMember(buttonFontSize);
	sprintf(OpBLsize, "%d", cntl->buttonFontSize);
    }

    if ((mask & FL_PDBrowserFontSize))
    {
	SetMember(browserFontSize);
	sprintf(OpBrFsize, "%d", cntl->browserFontSize);
    }

    if ((mask & FL_PDMenuFontSize))
    {
	SetMember(menuFontSize);
	sprintf(OpMLsize, "%d", cntl->menuFontSize);
    }

    if ((mask & FL_PDChoiceFontSize))
    {
	SetMember(choiceFontSize);
	sprintf(OpChFsize, "%d", cntl->choiceFontSize);
    }

    if ((mask & FL_PDSliderFontSize))
    {
	SetMember(sliderFontSize);
	sprintf(OpSLsize, "%d", cntl->sliderFontSize);
    }

    if ((mask & FL_PDInputFontSize))
    {
	SetMember(inputFontSize);
	sprintf(OpILsize, "%d", cntl->inputFontSize);
    }

    if ((mask & FL_PDLabelFontSize))
    {
	SetMember(labelFontSize);
	sprintf(OpLLsize, "%d", cntl->labelFontSize);
    }

    if ((mask & FL_PDBorderWidth))
	fl_set_border_width(cntl->borderWidth);

    if ((mask & FL_PDScrollbarType))
	fl_set_scrollbar_type(cntl->scrollbarType);

    if ((mask & FL_PDPupFontSize))
    {
	SetMember(pupFontSize);
	sprintf(OpPsize, "%d", cntl->pupFontSize);
    }

    if ((mask & FL_PDSafe))
    {
	SetMember(safe);
	sprintf(OpSafe, "%d", cntl->safe);
    }

    if ((mask & FL_PDBS))
    {
	SetMember(backingStore);
	sprintf(OpBS, "%d", cntl->backingStore);
    }

    if ((mask & FL_PDCoordUnit))
	fl_set_coordunit(cntl->coordUnit);

    if ((mask & FL_PDDebug))
	fl_set_debug_level(cntl->debug);
}

void
fl_get_defaults(FL_IOPT * cntl)
{
    if (!fl_display)
    {
	M_err("GetDefault", "You forgot to call fl_initialize");
	exit(1);
    }

    *cntl = fl_cntl;
}

/* convenience functions */
void
fl_set_coordunit(int u)
{
    fl_cntl.coordUnit = u;
    strcpy(OpCoordUnit, fl_get_vn_name(vn_coordunit, fl_cntl.coordUnit));
}

void
fl_set_border_width(int bw)
{
    fl_cntl.borderWidth = bw;
    sprintf(OpIBW, "%d", bw);
}

void
fl_set_scrollbar_type(int t)
{
    fl_cntl.scrollbarType = t;
    if (t == FL_NORMAL_SCROLLBAR)
	strcpy(OpSCBT, "normal");
    else if (t == FL_NICE_SCROLLBAR)
	strcpy(OpSCBT, "nice");
    else if (t == FL_PLAIN_SCROLLBAR)
	strcpy(OpSCBT, "plain");
    else
	strcpy(OpSCBT, "thin");
}

void
fl_set_visualID(long id)
{
    fl_requested_vid = id;
    sprintf(OpVisualID, "0x%lx", id);
}

int
fl_get_border_width(void)
{
    return fl_cntl.borderWidth;
}

int
fl_get_coordunit(void)
{
    return fl_cntl.coordUnit;
}

void
fl_set_debug_level(int l)
{
    fl_cntl.debug = l;
    sprintf(OpDebug, "%d", fl_cntl.debug);
    set_msg_threshold(fl_cntl.debug);
}

/*
 * Handle XAPPLRESDIR (colon seperated directories) specification.
 * Ignore the %T %N stuff
 */

static void
handle_applresdir(const char *rstr, const char *appclass)
{
    char *tok;
    char rbuf[512], buf[512];
    XrmDatabase fdb = 0;

    strcpy(rbuf, rstr);
    for (tok = strtok(rbuf, ":"); tok; tok = strtok(0, ":"))
    {
        snprintf(buf,sizeof(buf),"%s/%s",tok,appclass);
	M_info(0, "Trying XAPPLRESDIR: %s", buf);
	if ((fdb = XrmGetFileDatabase(buf)))
	{
	    XrmMergeDatabases(fdb, &fldatabase);
	    M_warn(0, "  XAPPLRESDIR %s loaded", buf);
	}
    }
}

/*
 * Routine to merge all resource databases, excluding the commandline,
 * which is  done in fl_initialize.
 */
static void
init_resource_database(const char *appclass)
{
    char buf[FL_PATH_MAX + 127], *rstr;
    XrmDatabase fdb = 0;

    if (!fl_display)
    {
	M_err("InitResource", "fl_initialize is not called");
	return;
    }

    if (fldatabase)
	return;

    XrmInitialize();


#ifdef __VMS
/* For the VMS version try to load the resources from, in this order,

   DECW$SYSTEM_DEFAULTS:appclass.DAT
   DECW$USER_DEFAULTS:appclass.DAT
   DECW$USER_DEFAULTS:DECW$XDEFAULTS.DAT
   The window resource manager for this display

 */
    snprintf(buf, sizeof(buf), "DECW$SYSTEM_DEFAULTS:%s.DAT", appclass);
    M_info(0, "Trying Sys_default: %s", buf);
    if ((fdb = XrmGetFileDatabase(buf)))
    {
	XrmMergeDatabases(fdb, &fldatabase);
	M_warn(0, "   system default %s loaded", buf);
    }

    snprintf(buf, sizeof(buf), "DECW$USER_DEFAULTS:%s.DAT", appclass);
    M_info(0, "Trying User_default: %s", buf);
    if ((fdb = XrmGetFileDatabase(buf)))
    {
	XrmMergeDatabases(fdb, &fldatabase);
	M_warn(0, "   system default %s loaded", buf);
    }


    snprintf(buf, sizeof(buf), "DECW$USER_DEFAULTS:DECW$XDEFAULTS.DAT");
    M_info(0, "Trying Sys_default: %s", buf);
    if ((fdb = XrmGetFileDatabase(buf)))
    {
	XrmMergeDatabases(fdb, &fldatabase);
	M_warn(0, "   system default %s loaded", buf);
    }

    M_info(0, "Trying RESOURCE_MANAGER");
    if ((rstr = XResourceManagerString(fl_display)))
    {
	if ((fdb = XrmGetStringDatabase(rstr)))
	{
	    XrmMergeDatabases(fdb, &fldatabase);
	    M_warn(0, "   RESOURCE_MANAGER loaded");
	}
    }
#else /* !VMS */
    snprintf(buf, sizeof(buf), "/usr/lib/X11/app-defaults/%s", appclass);
    M_info(0, "Trying Sys_default: %s", buf);
    if ((fdb = XrmGetFileDatabase(buf)))
    {
	XrmMergeDatabases(fdb, &fldatabase);
	M_warn(0, "   system default %s loaded", buf);
    }

    /* try XAPPLRESDIR */
    M_info(0, "Trying XAPPLRESDIR");
    if ((rstr = getenv("XAPPLRESDIR")))
	handle_applresdir(rstr, appclass);

    /* try server defined resources */
    M_info(0, "Trying RESOURCE_MANAGER");
    if ((rstr = XResourceManagerString(fl_display)))
    {
	if ((fdb = XrmGetStringDatabase(rstr)))
	{
	    XrmMergeDatabases(fdb, &fldatabase);
	    M_warn(0, "   RESOURCE_MANAGER loaded");
	}
    }
    else
    {
	/* try ~/.Xdefaults   */
	if ((rstr = getenv("HOME")))
	{
            snprintf(buf,sizeof(buf),"%s/.Xdefaults",rstr);
	    M_info(0, "Trying %s", buf);
	    if ((fdb = XrmGetFileDatabase(buf)))
	    {
		XrmMergeDatabases(fdb, &fldatabase);
		M_warn(0, "   %s loaded", buf);
	    }
	}
    }

    /* load file XENVIRONMENT */
    M_info(0, "Trying $XEVIRONMENT");
    if ((rstr = getenv("XENVIRONMENT")))
    {
	if ((fdb = XrmGetFileDatabase(rstr)))
	{
	    XrmMergeDatabases(fdb, &fldatabase);
	    M_warn(0, "   %s loaded", rstr);
	}
    }
    else
    {
	/* ~/.Xdefaults-<hostname> */
	M_info(0, "Trying ~/.Xdefaults-<hostname>");
	if ((rstr = getenv("HOME")))
	{
	    int l;
            snprintf(buf,sizeof(buf),"%s/.Xdefaults",rstr);
	    l = strlen(strcat(buf, "-"));
	    gethostname(buf + l, sizeof(buf) - l);
	    M_info(0, "Trying %s", buf);
	    if ((fdb = XrmGetFileDatabase(buf)))
	    {
		XrmMergeDatabases(fdb, &fldatabase);
		M_warn(0, "   %s loaded", buf);
	    }
	}
    }
#endif /* VMS */

    errno = 0;

    if (!fldatabase)
    {
	M_warn("InitResource", "Can't find any resource databases!");
	return;
    }
}

static int
is_true(const char *s)
{
    return (strncmp(s, "True", 4) == 0 || strncmp(s, "true", 4) == 0 ||
	    strncmp(s, "Yes", 3) == 0 || strncmp(s, "yes", 3) == 0 ||
	    strncmp(s, "On", 2) == 0 || strncmp(s, "on", 2) == 0 ||
	    *s == '1');
}

/*
 * The resource routine of the lowest level. Only adds the application
 * name.
 *
 * Return the string representation of the resource value
 */

const char *
fl_get_resource(const char *rname,	/* resource name */
		const char *cname,	/* class name  */
		FL_RTYPE dtype,	/* data type          */
		const char *defval,	/* default            */
		void *val,	/* variable           */
		int size)	/* buffer size if string */
{
    XrmValue entry;
    char *type = 0;
    char res_name[256], res_class[256];

    res_name[0] = res_class[0] = '\0';
    snprintf(res_name,sizeof(res_name),"%s.%s",fl_app_name,rname);
    if (cname)
        snprintf(res_class,sizeof(res_class),"%s.%s",fl_app_class,cname);

    entry.addr = 0;
    entry.size = 0;
    XrmGetResource(fldatabase, res_name, res_class, &type, &entry);

    M_warn(0, "GetResource %s(%s): %s ", res_name, res_class,
	   entry.addr ? entry.addr : "None");

    /* use the default if not set */
    if (!entry.addr)
	entry.addr = (void *) defval;	/* entry.add can be XPointer/caddr_t */

    if (dtype == FL_NONE || !entry.addr)
	return entry.addr;

    if (entry.addr)
    {
	switch (dtype)
	{
	case FL_SHORT:
	    *(short *) val = atoi(entry.addr);
	    break;
	case FL_INT:
	    *(int *) val = atoi(entry.addr);
	    break;
	case FL_BOOL:
	    *(int *) val = is_true(entry.addr);
	    break;
	case FL_LONG:
	    *(long *) val = strtol(entry.addr, (char **) NULL, 0);
	    break;
	case FL_FLOAT:
	    *(float *) val = (float)atof(entry.addr);
	    break;
	case FL_STRING:
	    strncpy(val, entry.addr, size);
	    ((char *) val)[size - 1] = '\0';
	    break;
	default:
	    M_err("GetResource", "Unknown type %d", dtype);
	    break;
	}
    }
    return (entry.addr);
}


void
fl_set_resource(const char *str, const char *val)
{
    char res_name[256];
    snprintf(res_name,sizeof(res_name),"%s.%s",fl_app_name,str);
    XrmPutStringResource(&fldatabase, res_name, (char *) val);
}



/* internal resource initialization */
static void
fl_init_resources(void)
{
    char res[256], cls[256], ores[256];
    char *appname = fl_app_name, *appclass = fl_app_class;
    char *ori_appname = fl_ori_app_name;

    /* internal resources need to be prefixed xform and XForm. need to
       generate for all names */

    snprintf(cls,sizeof(cls),"%s.XForm",fl_app_class);
    fl_app_class = cls;
    snprintf(res,sizeof(res),"%s.xform",fl_app_name);
    fl_app_name = res;
    snprintf(ores,sizeof(ores),"%s.xform",fl_ori_app_name);
    fl_ori_app_name = ores;
    fl_get_app_resources(internal_resources, Niopt);

    fl_app_name = appname;
    fl_app_class = appclass;
    fl_ori_app_name = ori_appname;

    if (fl_cntl.sync)
    {
	XSynchronize(fl_display, 1);
	M_err(0, "**** Synchronous Mode ********");
	fl_set_debug_level(4);
    }
}

static int fl_argc;
static char **fl_argv;

static void
dup_argv(char **argv, int n)
{
    int i;

    if (!argv)
	return;

    if (!fl_argv)
	fl_argv = fl_malloc(512 * sizeof(*fl_argv));

    for (i = 0; i < 511 && i < n; i++)
	fl_argv[i] = fl_strdup(argv[i]);

    fl_argv[i] = 0;
}

char **
fl_get_cmdline_args(int *n)
{
    *n = fl_argc;
    return fl_argv;
}

/* find the command name from arg[0] and return a copy of it */
static char *
get_command_name(const char *arg0)
{
    char *p;
    char *s = fl_strdup(arg0);
    char *cmd_name = s;

#ifdef __VMS
    /* vms command disk:[path]command.exe  */
    if ((p = strrchr(s, ']')))
	cmd_name = p + 1;
#else
#ifdef FL_WIN32
    _splitpath(arg0, NULL, NULL, cmd_name, NULL);
#else
    if ((p = strrchr(s, '/')))
	cmd_name = p + 1;
#endif
#endif

    /* remove the extension and the period */
    if ((p = strrchr(cmd_name, '.')))
	*p = '\0';

    return cmd_name;
}

/*
 * Global Routines to do the initialization and resource management
 *
 */

#define DumpD(a)    fprintf(stderr,"\t%s:%d\n",#a,fl_cntl.a)
#define DumpS(a)    fprintf(stderr,"\t%s:%s\n",#a,fl_cntl.a)
#define DumpF(a)    fprintf(stderr,"\t%s:%.3f\n",#a,fl_cntl.a)

static Window GetVRoot(Display *, int);

void
fl_init_fl_context(void)
{
    if (!fl_context)
    {
	/* initialize context */
	fl_context = fl_calloc(1, sizeof(*fl_context));
	fl_context->next = 0;
	fl_context->io_rec = 0;
	fl_context->idle_rec = 0;
	fl_context->atclose = 0;
	fl_context->free_rec = 0;
	fl_context->signal_rec = 0;
	fl_context->idle_delta = TIMER_RES;
	fl_context->hscb = FL_HOR_THIN_SCROLLBAR;
	fl_context->vscb = FL_VERT_THIN_SCROLLBAR;
        fl_context->navigate_mask = ShiftMask; /* to navigate input field */
    }
}


void fl_set_input_navigate(unsigned mask)
{
     fl_init_fl_context();

     if (mask == ShiftMask || mask == Mod1Mask || mask == ControlMask)
          fl_context->navigate_mask = mask;
}
          

Display *
fl_initialize(int *na, char *arg[], const char *appclass,
	      XrmOptionDescList appopt, int nappopt)
{
    char disp_name[256], disp_cls[256], buf[256];
    XrmValue xval;
    char *type;
    char *expire = "NC_PA_@X";
    float xdpi, ydpi;

    if (fl_display)
    {
	Bark("fl_initialize", "Already initialized");
	return fl_display;
    }

    setlocale(LC_ALL,"");

    fl_internal_init();

    XrmInitialize();

    /* be paranoid */
    if (!na || !*na)
    {
	fprintf(stderr, "XForms: argc==0 detected\n");
	exit(1);
    }


    /* save a copy of the command line for later WM hints */
    fl_argc = *na;
    dup_argv(arg, *na);


    /* get appname and class, but without any leading paths  */

    fl_ori_app_name = fl_app_name = get_command_name(arg[0]);
    fl_app_class = fl_strdup((appclass && *appclass) ? appclass : fl_app_name);

    /* make class name upper case if non supplied */
    if (!appclass || !*appclass)
    {
	fl_app_class[0] = toupper(fl_app_class[0]);
	if (fl_app_class[0] == 'X')
	    fl_app_class[1] = toupper(fl_app_class[1]);
    }

    /* do form internal resouces first */
    XrmParseCommand(&cmddb, copt, Ncopt, fl_app_name, na, arg);

    /* if there are still more left and  appopt is not zero */
    if (appopt && (na && *na))
	XrmParseCommand(&cmddb, appopt, nappopt,
			(char *) fl_ori_app_name, na, arg);

    /* check version request */
    snprintf(disp_name,sizeof(disp_name), "%s.fl_version", fl_app_name);
    snprintf(disp_cls,sizeof(disp_cls), "%s.FLversion", fl_app_name);

    if (XrmGetResource(cmddb, disp_name, disp_cls, &type, &xval))
	fl_print_version(0);

    /* get the display name first before doing anything */
    snprintf(disp_name,sizeof(disp_name), "%s.display",fl_ori_app_name);
    snprintf(disp_cls ,sizeof(disp_cls ), "%s.Display",fl_app_class);

    buf[0] = '\0';
    if (XrmGetResource(cmddb, disp_name, disp_cls, &type, &xval))
    {
	strncpy(buf, xval.addr, sizeof(buf));
	buf[sizeof(buf)-1] = '\0';
    }

    /* open display and quit if failure */
    if (!(fl_display = XOpenDisplay(buf)))
    {
        /* if no display is set, there is no guarantee that buf
           is long enough to contain the DISPLAY setting
         */
	M_err(0, "%s: Can't open display %s", fl_argv[0], 
              XDisplayName(buf[0] ? buf:0));
	return 0;
    }

    flx->display = fl_display;
    flx->screen  = fl_screen;

    /* get debug level settings since all error reporting will be controled
       by it */
    snprintf(disp_name,sizeof(disp_name),"%s.debug",fl_app_name);
    snprintf(disp_cls, sizeof(disp_cls ),"%s.Debug",fl_app_class);
    if (XrmGetResource(cmddb, disp_name, disp_cls, &type, &xval))
	fl_set_debug_level(atoi(xval.addr));

    /* check if -name is present */
    snprintf(disp_name,sizeof(disp_name),"%s.name",fl_app_name);
    snprintf(disp_cls, sizeof(disp_cls ),"%s.Name",fl_app_class);
    M_warn(0, "trying %s", disp_name);
    if (XrmGetResource(cmddb, disp_name, disp_cls, &type, &xval))
    {
	fl_app_name = fl_strdup(xval.addr);
	M_warn(0, "Change AppName from %s to %s", fl_ori_app_name, fl_app_name);
    }

    /* merge  other resource database */
    init_resource_database(fl_app_class);

    /* override any options in the database with the command line opt */
    XrmMergeDatabases(cmddb, &fldatabase);

    /* load FL resources */
    fl_init_resources();

    fl_cntl.vclass = fl_vclass_val(fl_cntl.vname);
    fl_cntl.coordUnit = fl_get_vn_value(vn_coordunit, OpCoordUnit);


#if (FL_DEBUG >= ML_WARN)	/* { */
    if (fl_cntl.debug)
    {
	fprintf(stderr, "Options Set\n");
	DumpD(debug);
	fprintf(stderr, "\tVisual:%s (%d)\n",
	 fl_cntl.vclass >= 0 ? fl_vclass_name(fl_cntl.vclass) : "To be set",
		fl_cntl.vclass);
	DumpD(depth);
	DumpD(privateColormap);
	DumpD(sharedColormap);
	DumpD(standardColormap);
	DumpD(doubleBuffer);
	DumpD(ulPropWidth);
	DumpD(ulThickness);
	DumpD(scrollbarType);
	DumpD(backingStore);
	fprintf(stderr, "\t%s:%s\n", "coordUnit",
		fl_get_vn_name(vn_coordunit, fl_cntl.coordUnit));
	fprintf(stderr, "\t%s:0x%lx\n", "VisualId", fl_requested_vid);

#ifdef DO_GAMMA_CORRECTION
	DumpF(rgamma);
	DumpF(ggamma);
	DumpF(bgamma);
#endif
    }
#endif /* DEBUG *} */

#ifdef DO_GAMMA_CORRECTION
    fl_set_gamma(fl_cntl.rgamma, fl_cntl.ggamma, fl_cntl.bgamma);
#endif

    fl_set_ul_property(fl_cntl.ulPropWidth, fl_cntl.ulThickness);

    fl_cntl.vclass = fl_vclass_val(fl_cntl.vname);

    if (fl_check_it(expire))
	exit(1);

    /* get the current keyboard state */
    {
	XKeyboardState xks;
	XGetKeyboardControl(fl_display, &xks);
	fl_keybdcontrol.auto_repeat_mode = xks.global_auto_repeat;
	fl_keybdmask = KBAutoRepeatMode;
    }

    /* other initializations */
    fl_screen = DefaultScreen(fl_display);
    fl_root = RootWindow(fl_display, fl_screen);
    fl_vroot = GetVRoot(fl_display, fl_screen);
    fl_wmstuff.pos_request = USPosition;
    flx->screen  = fl_screen;

    if (fl_root != fl_vroot)
    {
	M_warn("FlInit", "fl_root=%lu fl_vroot=%lu", fl_root, fl_vroot);

	/* tvtwm requires this to position a window relative to the current
	   desktop */
	fl_wmstuff.pos_request = PPosition;
    }

    fl_scrh = DisplayHeight(fl_display, fl_screen);
    fl_scrw = DisplayWidth(fl_display, fl_screen);

    xdpi = fl_scrh * 25.4f / DisplayHeightMM(fl_display, fl_screen);
    ydpi = fl_scrw * 25.4f / DisplayWidthMM(fl_display, fl_screen);

    if ((xdpi / ydpi) > 1.05f || (ydpi / xdpi) < 0.95f)
	M_warn("FlInit", "NonSquarePixel %.1f %.1f", xdpi, ydpi);

    fl_dpi = (xdpi + ydpi) / 2;
    fl_dpi = ((int) (fl_dpi * 10.0 + 0.3)) * 0.1;

    M_info(0, "screen DPI=%f", fl_dpi);

    fl_vmode = fl_initialize_program_visual();
    fl_init_colormap(fl_vmode);
    fl_init_font();
    fl_init_fl_context();

#ifdef XlibSpecificationRelease
    if (XSupportsLocale())
    {
         XSetLocaleModifiers("");
        /* use the same input method throughout xforms */
        fl_context->xim = XOpenIM(fl_display, 0, 0, 0);
        /* also use the same input context */
        if (fl_context->xim)
        {
            int style =  XIMPreeditNothing|XIMStatusNothing;
            fl_context->xic = XCreateIC(fl_context->xim,
                                 XNInputStyle, style,
                                 0);
        }
    }
#endif
    fl_default_xswa();
    fl_init_stipples();
    set_err_msg_func(fl_show_alert);

    /* if using non-default visual or private colormap, need a window with
       correct visual/colormap and depth */

    fl_state[fl_vmode].trailblazer = fl_root;

    if (fl_visual(fl_vmode) != DefaultVisual(fl_display, fl_screen) ||
	fl_state[fl_vmode].pcm)
    {
	fl_state[fl_vmode].trailblazer =
	    fl_create_window(fl_root, fl_colormap(fl_vmode), "trailblazer");
    }


    if (strcmp(OpSCBT, "plain") == 0)
    {
	fl_context->hscb = FL_HOR_PLAIN_SCROLLBAR;
	fl_context->vscb = FL_VERT_PLAIN_SCROLLBAR;
    }
    else if (strcmp(OpSCBT, "normal") == 0)
    {
	fl_context->hscb = FL_HOR_SCROLLBAR;
	fl_context->vscb = FL_VERT_SCROLLBAR;
    }
    else if (strcmp(OpSCBT, "thin") == 0)
    {
	fl_context->hscb = FL_HOR_THIN_SCROLLBAR;
	fl_context->vscb = FL_VERT_THIN_SCROLLBAR;
    }
    else if (strcmp(OpSCBT, "nice") == 0)
    {
	fl_context->hscb = FL_HOR_NICE_SCROLLBAR;
	fl_context->vscb = FL_VERT_NICE_SCROLLBAR;
    }

    fl_context->max_request_size = XMaxRequestSize(fl_display);

    if (fl_context->max_request_size < 4096)
    {
	M_err("init", "Something is wrong with max_request_size:%ld",
	      fl_context->max_request_size);
	fl_context->max_request_size = 4096;
    }


#if XlibSpecificationRelease >= 6
    fl_context->ext_request_size = XExtendedMaxRequestSize(fl_display);
#else
    fl_context->ext_request_size = 0;
#endif

    if (fl_context->ext_request_size == 0)
	fl_context->ext_request_size = fl_context->max_request_size;

    fl_context->max_request_size -= 8;
    fl_context->ext_request_size -= 8;
    fl_context->tooltip_time = 600;

    fl_add_io_callback(ConnectionNumber(fl_display), FL_READ, 0, 0);

    /* has to be here, otherwise fl_add_symbol won't be able to replace the
       built-in */
    fl_init_symbols();
    fl_init_goodies();

    /* hang the database on the display so application can get it */
    XrmSetDatabase(fl_display, fldatabase);

    return fl_display;
}

/*
 * Find out about  virtual root. Taken from XFaq
 */
#include <X11/Xatom.h>
static Window
GetVRoot(Display * dpy, int scr)
{
    Window rootReturn, parentReturn, *children;
    unsigned int numChildren;
    Window root = RootWindow(dpy, scr);
    Atom __SWM_VROOT = None;
    unsigned int i;

    __SWM_VROOT = XInternAtom(dpy, "__SWM_VROOT", False);
    XQueryTree(dpy, root, &rootReturn, &parentReturn, &children, &numChildren);

    for (i = 0; i < numChildren; i++)
    {
	Atom actual_type;
	int actual_format;
	unsigned long nitems, bytesafter;
	Window *newRoot = NULL;

	if (XGetWindowProperty(dpy, children[i], __SWM_VROOT, 0, 1,
			       False, XA_WINDOW, &actual_type,
			       &actual_format, &nitems, &bytesafter,
			 (unsigned char **) &newRoot) == Success && newRoot)
	{
	    root = *newRoot;
	    break;
	}

    }

    XFree((char *) children);
    return root;
}

/*
 * For Application programs
 */
static void
get_app_resource(FL_resource * appresource, int n)
{
    FL_resource *flr = appresource, *flrs = flr + n;

    for (; flr < flrs; flr++)
	if (flr->type == FL_STRING && flr->nbytes == 0)
	    M_err("AppResource", "%s: buflen==0", flr->res_name);
	else
	    fl_get_resource(flr->res_name, flr->res_class,
			    flr->type, flr->defval, flr->var, flr->nbytes);
}

void
fl_get_app_resources(FL_resource * appresource, int n)
{
    char *tmp = fl_app_name;

    fl_app_name = fl_ori_app_name;
    get_app_resource(appresource, n);

    if (fl_app_name != tmp)
    {
	fl_app_name = tmp;
	get_app_resource(appresource, n);
    }
}

void
fl_flip_yorigin(void)
{
    if (!fl_display)
	fl_inverted_y = 1;
    else
	M_err("YCoord", "Only supported before fl_initialize");
}

void
fl_set_app_name(const char *n, const char *c)
{
    if (n)
	fl_app_name = fl_strdup(n);
    if (c)
	fl_app_class = fl_strdup(c);
}


Display *
fl_init(void)
{
    int argc = 1;
    static char *tmp = "random_command";
    return fl_initialize(&argc, &tmp, 0, 0, 0);
}
