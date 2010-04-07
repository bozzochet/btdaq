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
 * $Id: fd_printC.c,v 1.0 2002/05/08 05:27:39 zhao Release $
 *
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *  Generate header/C files
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef WIN32
#include <io.h>
#endif

#include "forms.h"
#include "fd_main.h"

void
make_backup(const char *s)
{
    char buf[1024];

    if (access((char *) s, R_OK) == 0)
    {
	sprintf(buf, "%s.bak", s);
#ifdef  __EMX__
	unlink(buf);
#endif
	if (rename(s, buf))
	    M_err("MakeBackup", "%s write failed", buf);
    }
}

/* filename is without extensions */
int
C_output(const char *filename, FRM * forms, int fnumb)
{
    char fname[512], *tmp;
    int i, j;
    FILE *fn;

    strcpy(fname, filename);
    strcat(fname, ".h");

    make_backup(fname);
    if (!(fn = fopen(fname, "w")))
    {
	if (!fdopt.conv_only)
	    fl_show_alert("Cannot create header file!!", fname, "", 1);
	else
	    M_err(0, "Can't create %s", fname);
	return 0;
    }

    fprintf(fn, "/** Header file generated with fdesign on %s.**/\n\n",
	    fl_now());

    fprintf(fn, "#ifndef %s_h_\n#define %s_h_\n",
	    get_fd_name(forms[0].fname),
	    get_fd_name(forms[0].fname));

    fprintf(fn, "\n/** Callbacks, globals and object handlers **/\n");
    for (i = 0; i < fnumb; i++)
	print_callbacks_and_globals(fn, forms[i].form, 0);

    fprintf(fn, "\n/**** Forms and Objects ****/\n");
    for (i = 0; i < fnumb; i++)
	print_header(fn, forms[i].form, forms[i].fname);

    if (fdopt.altformat)
    {
	fprintf(fn, "\n/**** Creation Routine ****/\n");
	fprintf(fn, "extern void %s(void);\n", main_name);
    }
    fprintf(fn, "\n#endif /* %s_h_ */\n", get_fd_name(forms[0].fname));
    fclose(fn);

    /* Make the .c file. */
    strcpy(fname, filename);
    strcat(fname, ".c");

    make_backup(fname);
    if (!(fn = fopen(fname, "w")))
    {
	fl_show_alert("Cannot create C-code file!!", "", "", 1);
	return 0;
    }

    fprintf(fn, "/* Form definition file generated with fdesign. */\n\n");
    fprintf(fn, "#include \"%s\"\n", xform_header);
    fprintf(fn, "#include <stdlib.h>\n");
    strcpy(fname, filename);

    /* strip path */
    j = -1;
    for (i = 0; fname[i] != '\0'; i++)
	if (fname[i] == '/')
	    j = i;
    if (j >= 0)
	for (i = j + 1; fname[i - 1]; i++)
	    fname[i - j - 1] = fname[i];

    fprintf(fn, "#include \"%s.h\"\n\n", fname);

    reset_dupinfo_cache();

    for (i = 0; i < fnumb; i++)
	print_form(fn, forms[i].form, forms[i].fname);

    if (fdopt.altformat)
    {
	fprintf(fn, "void %s(void)\n{\n", main_name);
	for (i = 0; i < fnumb; i++)
	    fprintf(fn, "  create_form_%s();\n", forms[i].fname);
	fprintf(fn, "}\n\n");
    }
    fclose(fn);

    /* check if need to output a template for main program */
    if (fdopt.emit_main)
    {
	strcpy(fname, filename);
	strcat(fname, "_main.c");

	make_backup(fname);
	if (!(fn = fopen(fname, "w")))
	{
	    fl_show_alert("Cannot create main file!!", "", "", 1);
	    return 0;
	}

	/* figure out the path */
	strcat(strcpy(fname, filename), ".h");
	if ((tmp = strrchr(fname, '/')))
	    ++tmp;
	else
	    tmp = fname;
	fprintf(fn, "#include \"%s\"\n#include \"%s\"\n\n",
		xform_header, tmp);
	output_main(fn, forms, fnumb);
	fclose(fn);
    }

    /* output callback stubs */
    if (fdopt.emit_cb)
    {
	strcpy(fname, filename);
	strcat(fname, "_cb.c");

	make_backup(fname);
	if (!(fn = fopen(fname, "w")))
	{
	    fl_show_alert("Cannot create callback file!!", "", "", 1);
	    return 0;
	}

	/* figure out the path */
	strcat(strcpy(fname, filename), ".h");
	if ((tmp = strrchr(fname, '/')))
	    *tmp++ = '\0';
	else
	    tmp = fname;
	fprintf(fn, "#include \"%s\"\n#include \"%s\"\n\n",
		xform_header, tmp);
	output_callbacks(fn, forms, fnumb);
	fclose(fn);
    }
    return 1;
}



/*** Some attributes query routines ***{*/

#define VN(v)   {v,#v}
#define PVN(v)   v,#v


VN_pair vn_btype[] =
{
    {PVN(FL_NO_BOX), "No box%r1", "Nn#n"},
    {PVN(FL_UP_BOX), "Up box%r1", "Uu#u"},
    {PVN(FL_DOWN_BOX), "Down box%r1", "Dd#d"},
    {PVN(FL_BORDER_BOX), "Border box%r1", "Bb#b"},
    {PVN(FL_SHADOW_BOX), "Shadow box%r1", "Ss#s"},
    {PVN(FL_FRAME_BOX), "Frame box%r1", "Ff#f"},
    {PVN(FL_ROUNDED_BOX), "Rounded box%r1", "Rr#r"},
    {PVN(FL_EMBOSSED_BOX), "Embossed box%r1", "Ee#e"},
    {PVN(FL_FLAT_BOX), "Flat box%r1", "Ff#f"},
    {PVN(FL_RFLAT_BOX), "Rflat box%r1", "lL#l"},
    {PVN(FL_RSHADOW_BOX), "Rshadow box%r1", "wW#w"},
    {PVN(FL_OVAL_BOX), "Oval box%r1", "Oo#o"},
    {PVN(FL_ROUNDED3D_UPBOX), "rounded3d upbox%r1", "nN#n"},
    {PVN(FL_ROUNDED3D_DOWNBOX), "rounded3d downbox%r1", "oO#o"},
    {PVN(FL_OVAL3D_UPBOX), "Oval3d upbox%r1", "vV#v"},
    {PVN(FL_OVAL3D_DOWNBOX), "Oval3d downbox%r1", "lL#l"},
    {PVN(FL_OVAL3D_FRAMEBOX), "Oval3d framebox%r1", "eE#e"},
    {PVN(FL_OVAL3D_EMBOSSEDBOX), "Oval3d embossed%r1", "eE#e"},
#if 0
    {PVN(FL_TOPTAB_UPBOX), "toptab%r1", "tT#t"},
    {PVN(FL_SELECTED_TOPTAB_UPBOX), "selected toptab%r1", "sS#s"},
#endif
    {PVN(-1), 0, 0}
};

VN_pair vn_align[] =
{
    VN(FL_ALIGN_TOP),
    VN(FL_ALIGN_BOTTOM),
    VN(FL_ALIGN_LEFT),
    VN(FL_ALIGN_RIGHT),
    VN(FL_ALIGN_CENTER),
    VN(FL_ALIGN_TOP_RIGHT),
    VN(FL_ALIGN_TOP_LEFT),
    VN(FL_ALIGN_BOTTOM_RIGHT),
    VN(FL_ALIGN_BOTTOM_LEFT),
    VN(-1)
};

static VN_pair vn_lsize[] =
{
    VN(FL_DEFAULT_SIZE),
    VN(FL_TINY_SIZE), VN(FL_SMALL_SIZE),
    VN(FL_NORMAL_SIZE), VN(FL_MEDIUM_SIZE),
    VN(FL_LARGE_SIZE), VN(FL_HUGE_SIZE),

    VN(FL_DEFAULT_FONT),
    VN(FL_TINY_FONT), VN(FL_SMALL_FONT),
    VN(FL_NORMAL_FONT), VN(FL_MEDIUM_FONT),
    VN(FL_LARGE_FONT), VN(FL_HUGE_FONT),
    {FL_SMALL_FONT, "FL_NORMAL_FONT1"},
    {FL_NORMAL_FONT, "FL_NORMAL_FONT2"},
    VN(-1)
};

static VN_pair vn_lstyle[] =
{
    VN(FL_NORMAL_STYLE), VN(FL_BOLD_STYLE),
    VN(FL_ITALIC_STYLE), VN(FL_BOLDITALIC_STYLE),
    VN(FL_FIXED_STYLE), VN(FL_FIXEDBOLD_STYLE),
    VN(FL_FIXEDITALIC_STYLE), VN(FL_FIXEDBOLDITALIC_STYLE),
    VN(FL_TIMES_STYLE), VN(FL_TIMESBOLD_STYLE),
    VN(FL_TIMESITALIC_STYLE), VN(FL_TIMESBOLDITALIC_STYLE),
    VN(FL_SHADOW_STYLE), VN(FL_ENGRAVED_STYLE), VN(FL_EMBOSSED_STYLE),
    VN(-1)
};

VN_pair vn_gravity[] =
{
    VN(FL_NoGravity),
    VN(FL_NorthWest),
    VN(FL_North),
    VN(FL_NorthEast),
    VN(FL_West),

    VN(FL_East),
    VN(FL_South),
    VN(FL_SouthEast),
    VN(FL_SouthWest),

    VN(FL_ForgetGravity),
    VN(ForgetGravity),
    VN(NorthWestGravity),
    VN(NorthGravity),
    VN(NorthEastGravity),
    VN(WestGravity),

    VN(EastGravity),
    VN(SouthGravity),
    VN(SouthEastGravity),
    VN(SouthWestGravity),

    VN(-1)
};

VN_pair vn_resize[] =
{
    VN(FL_RESIZE_NONE), VN(FL_RESIZE_X), VN(FL_RESIZE_Y), VN(FL_RESIZE_ALL),
    VN(-1)
};

static VN_pair vn_unit[] =
{
    VN(FL_COORD_PIXEL),
    VN(FL_COORD_MM),
    VN(FL_COORD_centiMM),
    VN(FL_COORD_POINT),
    VN(FL_COORD_centiPOINT),
    {FL_COORD_PIXEL, "pixel"},
    {FL_COORD_MM, "mm"},
    {FL_COORD_POINT, "point"},
    {FL_COORD_centiPOINT, "cp"},
    {FL_COORD_centiMM, "cmm"},
    {FL_COORD_centiPOINT, "cpoint"},
    VN(-1)
};

int
get_vn_val(VN_pair * vn, const char *name)
{
    for (; vn->val >= 0; vn++)
	if (strcmp(name, vn->name) == 0)
	    return vn->val;
    return atoi(name);
}

char *
get_vn_name(VN_pair * vn, int val)
{
    static char buf[MAX_TYPE_NAME_LEN];

    for (; vn->val >= 0; vn++)
	if (vn->val == val)
	    return vn->name;
    sprintf(buf, "%d", val);
    return buf;
}

static void output_object(FILE * fn, FL_OBJECT * obj, int);
static void pre_form_output(FILE * fn);
static void post_form_output(FILE * fn);

static void
emit_attrib(FILE * fp, int a, VN_pair * vn, const char *aname)
{
    const char *s;
    if (vn == vn_align)
	s = align_name(a);
    else
	s = get_vn_name(vn, a);

    fprintf(fp, "    %s(obj,%s);\n", aname, s);
}

static char *
pure_style_name(int val)
{
    VN_pair *vn = vn_lstyle;
    static char buf[64];

    for (; vn->val >= 0 && vn->val != val; vn++)
	;
    if (vn->val == val)
	return vn->name;
    else
    {
	sprintf(buf, "%d", val);
	return buf;
    }
}

static int
pure_style_val(const char *cc)
{
    VN_pair *vn = vn_lstyle;

    for (; vn->val >= 0 && strcmp(cc, vn->name); vn++)
	;
    return (strcmp(cc, vn->name) == 0) ? vn->val : atoi(cc);
}

char *
style_name(int style)
{
    static char buf[64];
    int lstyle = style % FL_SHADOW_STYLE;
    int spstyle = (style / FL_SHADOW_STYLE) * FL_SHADOW_STYLE;

    strcpy(buf, pure_style_name(lstyle));
    if (spstyle)
	strcat(strcat(buf, "+"), pure_style_name(spstyle));
    return buf;
}

int
style_val(const char *cc)
{
    char lstyle[MAX_TYPE_NAME_LEN], spstyle[MAX_TYPE_NAME_LEN], *p;

    strcpy(lstyle, cc);
    spstyle[0] = '\0';
    if ((p = strchr(lstyle, '+')))
    {
	strcpy(spstyle, p + 1);
	*p = 0;
    }
    return pure_style_val(lstyle) + pure_style_val(spstyle);
}

char *
lsize_name(int val)
{
    return get_vn_name(vn_lsize, val);
}

int
lsize_val(const char *cc)
{
    return get_vn_val(vn_lsize, cc);
}

char *
gravity_name(int val)
{
    return get_vn_name(vn_gravity, val);
}

int
gravity_val(const char *cc)
{
    return get_vn_val(vn_gravity, cc);
}

char *
resize_name(int val)
{
    return get_vn_name(vn_resize, val);
}

int
resize_val(const char *cc)
{
    return get_vn_val(vn_resize, cc);
}

const char *
align_name(int val)
{
    static char buf[128];
    strcpy(buf, get_vn_name(vn_align, val % FL_ALIGN_INSIDE));
    if (val >= FL_ALIGN_INSIDE)
	strcat(buf, "|FL_ALIGN_INSIDE");
    return buf;
}

int
align_val(const char *cc)
{
    char s[128], *p;
    int val;

    strcpy(s, cc);
    if ((p = strchr(s, '|')))
	*p = '\0';
    val = get_vn_val(vn_align, s);
    return p ? (val | FL_ALIGN_INSIDE) : val;
}

char *
boxtype_name(int val)
{
    return get_vn_name(vn_btype, val);
}

int
boxtype_val(const char *cc)
{
    return get_vn_val(vn_btype, cc);
}

char *
unit_name(int val)
{
    return get_vn_name(vn_unit, val);
}

int
unit_val(const char *s)
{
    return get_vn_val(vn_unit, s);
}

/******** End of attributes query routines ********}****/

/*------------- Keeping track of array names. -------------------*/

#define MAXARNAME	100

static char *arnames[MAXARNAME];
static int arsizes[MAXARNAME];
static int anumb = 0;

/* Initializes the aray names. */
static void
init_array_names(void)
{
    anumb = 0;
}

/* Checks whether this name is an array name and remembers it */
static int
check_array_name(char aname[])
{
    char tmpstr[MAX_VAR_LEN];
    int i, j, ind;
    strcpy(tmpstr, aname);

    for (i = 0; tmpstr[i] != '[' && tmpstr[i]; i++)
	;
    if (!tmpstr[i])
	return FALSE;

    ind = 0;
    for (j = i + 1; tmpstr[j] != ']' && tmpstr[j]; j++)
	if (tmpstr[j] >= '0' && tmpstr[j] <= '9')
	    ind = 10 * ind + tmpstr[j] - '0';

    tmpstr[i] = 0;
    for (i = 0; i < anumb; i++)
	if (strcmp(arnames[i], tmpstr) == 0)
	{
	    if (ind + 1 > arsizes[i])
		arsizes[i] = ind + 1;
	    return TRUE;
	}

    if (anumb == MAXARNAME)
	return FALSE;

    arnames[anumb] = (char *) malloc(MAX_VAR_LEN);
    strcpy(arnames[anumb], tmpstr);
    arsizes[anumb] = ind + 1;
    anumb++;
    return TRUE;
}

static int
arethere_array_names(void)
{
    return (anumb > 0);
}

/* Prints the array names of the file. */
static void
print_array_names(FILE * fn, int newf)
{
    int i;

    for (i = 0; i < anumb; i++)
    {
	if (!newf)
	{
	    fprintf(fn, "\t*%s[%i]", arnames[i], arsizes[i]);
	    if (i < anumb - 1)
		fprintf(fn, ",\n");
	}
	else
	{
	    fprintf(fn, "\tFL_OBJECT *%s[%i];\n", arnames[i], arsizes[i]);
	}
    }
}

/*----------------------- Printing the C-code --------------------*/

/*
 * Prints the form description in C-code. Note that no matter what
 * the internal coordinate system is, externally we always have
 * positive y pointing upward from the lower-left corner of the
 * screen
 */

const char *
get_fd_name(const char *form_name)
{
    static char fdtname[MAX_VAR_LEN];
    sprintf(fdtname, "FD_%s", form_name);
    return fdtname;
}

double
get_conversion_factor(void)
{
    double sc = 1.0;

    if (fdopt.unit == FL_COORD_POINT)
	sc = 72.00 / fl_dpi;
    else if (fdopt.unit == FL_COORD_MM)
	sc = 25.40 / fl_dpi;
    else if (fdopt.unit == FL_COORD_centiPOINT)
	sc = 7200.00 / fl_dpi;
    else if (fdopt.unit == FL_COORD_centiMM)
	sc = 2540.00 / fl_dpi;

    return sc;
}

/* unit indicates what kind of output is desired. */
int
convert_u(FL_Coord l)
{
    FL_Coord len = l, t = 0;
    fl_scale_length(&t, &len, get_conversion_factor());
    return len;
}

/*
 * emit fl_set_xxxx_shortcut instead of fl_set_object_shortcut
 */
static VN_pair scclass[] =
{
    {FL_BUTTON, "button"},
    {FL_LIGHTBUTTON, "button"},
    {FL_ROUNDBUTTON, "button"},
    {FL_CHECKBUTTON, "button"},
    {FL_BITMAPBUTTON, "button"},
    {FL_PIXMAPBUTTON, "button"},
    {FL_INPUT, "input"},
 /* sentinel */
    {-1, 0}
};

static const char *
supported_shortcut(int objclass)
{
    VN_pair *vn = scclass;
    for (; vn->val >= 0; vn++)
	if (vn->val == objclass)
	    return vn->name;
    return "object";
}

static int
need_resize(int nw, int se)
{
    return (nw != se) ||
	(nw == EastGravity || nw == SouthGravity ||
	 nw == WestGravity || nw == NorthGravity || nw == ForgetGravity);
}

/*
 * Generate the C file for the forms we've defined. Header is generated
 * elsewhere. All default attributes are omitted.
 */
static void
print_form_newformat(FILE * fn, FL_FORM * form, const char *fname)
{
    FL_OBJECT *obj;
    char fdtname[MAX_VAR_LEN], fdvname[MAX_VAR_LEN];

    strcpy(fdtname, get_fd_name(fname));
    strcpy(fdvname, "fdui");

    /* check if object specific stuff wants to write anything */
    for (obj = form->first; (obj = obj->next);)
	emit_objclass_spec_header(fn, obj);

    fprintf(fn, "%s *create_form_%s(void)\n{\n  FL_OBJECT *obj;\n",
	    fdtname, fname);

    fprintf(fn, "  %s *%s = (%s *) fl_calloc(1, sizeof(*%s));\n",
	    fdtname, fdvname, fdtname, fdvname);

    /* take care of unit, borderwidth etc that affect the entire form */
    pre_form_output(fn);

    fprintf(fn, "  %s->%s = fl_bgn_form(FL_NO_BOX, %d, %d);\n",
	    fdvname, fname, convert_u(form->w), convert_u(form->h));

    for (obj = form->first; (obj = obj->next);)
	output_object(fn, obj, 0);

    fprintf(fn, "  fl_end_form();\n\n");

    if (fdopt.compensate)
	fprintf(fn, "  fl_adjust_form_size(%s->%s);\n", fdvname, fname);

    fprintf(fn, "  %s->%s->%s = %s;\n", fdvname, fname, fdvname, fdvname);

    /* restore */
    post_form_output(fn);

    fprintf(fn, "\n  return %s;\n}\n", fdvname);
    fprintf(fn, "/*---------------------------------------*/\n\n");
}

/*
 * Output C files for the forms defined, but slightly different from
 * the earlier routines. Activated by -altformat on the command line
 */
static void
print_form_altformat(FILE * fn, FL_FORM * form, const char *fname)
{
    FL_OBJECT *obj;
    char name[MAX_VAR_LEN], cbname[MAX_VAR_LEN], argname[MAX_VAR_LEN];
    int first = 1;

    /* print the form declaration */
    fprintf(fn, "FL_FORM *%s;\n\n", fname);

    /* print the object declarations */
    init_array_names();
    obj = form->first;

    while ((obj = obj->next) != NULL)
    {
	get_object_name(obj, name, cbname, argname);
	if (name[0] != 0)
	{
	    if (!check_array_name(name))
	    {
		fprintf(fn, first ? "FL_OBJECT\n" : ",\n");
		first = 0;
		fprintf(fn, "        *%s", name);
	    }
	}
    }

    if (arethere_array_names())
    {
	fprintf(fn, first ? "FL_OBJECT\n" : ",\n");
	first = 0;
	print_array_names(fn, 0);
    }

    fprintf(fn, first ? "\n" : ";\n\n");

    /* check if object specific stuff wants to write anything */
    for (obj = form->first; (obj = obj->next);)
	emit_objclass_spec_header(fn, obj);

    /* Print the defining procedure */
    fprintf(fn, "void create_form_%s(void)\n{\n  FL_OBJECT *obj;\n", fname);

    pre_form_output(fn);

    fprintf(fn, "  if (%s)\n     return;\n\n", fname);
    fprintf(fn, "  %s = fl_bgn_form(FL_NO_BOX,%d,%d);\n",
	    fname, convert_u(form->w), convert_u(form->h));

    for (obj = form->first; (obj = obj->next);)
	output_object(fn, obj, 1);

    fprintf(fn, "  fl_end_form();\n");
    if (fdopt.compensate)
	fprintf(fn, "  fl_adjust_form_size(%s);\n", fname);

    post_form_output(fn);

    fprintf(fn, "\n}\n/*---------------------------------------*/\n\n");
}

/*** This is the routine called the output rotuine in fd_forms.c */
void
print_form(FILE * fn, FL_FORM * form, const char *fname)
{
    (!fdopt.altformat ? print_form_newformat : print_form_altformat)
	(fn, form, fname);
}

/* check if a callback stub has already been emitted so we can suppress
 * duplicates as callbacks can be bound to multiple objects.
 * Potential problems: this check is only for a particular form.
 * if a callback is bound to multiple objects across many forms,
 * the callback will be output at least once per form.
 */

static int
already_emitted(FL_OBJECT * first, FL_OBJECT * curobj, const char *cb)
{
    char name[MAX_VAR_LEN], cbname[MAX_VAR_LEN], argname[MAX_VAR_LEN];
    FL_OBJECT *ob = first;

    for (ob = first->next; ob && ob != curobj; ob = ob->next)
    {
	get_object_name(ob, name, cbname, argname);
	if (cbname[0] && strcmp(cbname, cb) == 0)
	    return 1;
    }
    return 0;
}

#define MAXFREEOBJ 16


static char *
get_free_handle(FL_OBJECT * ob, const char *name)
{
    static int n;
    static char buf[1024];
    static FL_OBJECT *freeobj[MAXFREEOBJ];
    int i, k;


    if (ob->c_vdata)
	strcpy(buf, ob->c_vdata);
    else if (*name)
	sprintf(buf, "freeobj_%s_handle", name);
    else if (*ob->label)
	sprintf(buf, "freeobj_%s_handle", ob->label);
    else
    {
	for (k = -1, i = 0; i < MAXFREEOBJ && k < 0; i++)
	{
	    if (freeobj[i] == ob)
		k = i;
	}

	if (k < 0)
	{
	    k = ++n;
	    freeobj[k] = ob;
	}
	sprintf(buf, "freeobj%d_handle", k);
    }

    return buf;
}

/* Prints the callback routines used in form */
void
print_callbacks_and_globals(FILE * fn, FL_FORM * form, int code)
{
    FL_OBJECT *obj;
    char name[MAX_VAR_LEN], cbname[MAX_VAR_LEN], argname[MAX_VAR_LEN];

    obj = form->first;

    while ((obj = obj->next) != NULL)
    {
	get_object_name(obj, name, cbname, argname);

	if (obj->objclass == FL_FREE)
	{
	    if (!code)
	    {
		fprintf(fn, "extern int %s(FL_OBJECT *, int, FL_Coord,"
			" FL_Coord,\n\t\t\tint, void *);\n",
			get_free_handle(obj, name));
	    }
	    else
		fprintf(fn, "int %s(FL_OBJECT *ob, int ev, FL_Coord mx,"
			" FL_Coord my,\n\t\t\tint key, void *xev)\n"
		      "{\n   /* free obj handler code */\n  return 0;\n}\n",
			get_free_handle(obj, name));
	}


	if (cbname[0] != 0 && !strstr(cbname, "::") &&
	    !already_emitted(form->first, obj, cbname))
	{
	    if (!code)
	    {
		fprintf(fn, "extern void %s(FL_OBJECT *, long);\n", cbname);
	    }
	    else
	    {
		fprintf(fn, "void %s(FL_OBJECT *ob, long data)\n{\n", cbname);
		fprintf(fn, "  /* fill-in code for callback */\n}\n\n");
	    }
	}

	if (!code)
	    emit_objclass_spec_global(fn, obj);
    }

    fprintf(fn, "\n");
}

/*
 * Output header file that contains the form definition.
 */
static void
print_header_newformat(FILE * fn, FL_FORM * form, const char *fname)
{
    FL_OBJECT *obj;
    char name[MAX_VAR_LEN], cbname[MAX_VAR_LEN];
    char argname[MAX_VAR_LEN], fdtname[MAX_VAR_LEN];

    sprintf(fdtname, "FD_%s", fname);
    fprintf(fn, "typedef struct {\n\tFL_FORM *%s;\n", fname);
    fprintf(fn, "\tvoid *vdata;\n\tchar *cdata;\n\tlong  ldata;\n");

    init_array_names();

    for (obj = form->first; (obj = obj->next);)
    {
	get_object_name(obj, name, cbname, argname);
	if (name[0] && !check_array_name(name))
	    fprintf(fn, "\tFL_OBJECT *%s;\n", name);
    }

    if (arethere_array_names())
	print_array_names(fn, 1);

    fprintf(fn, "} %s;\n", fdtname);

    fprintf(fn, "\nextern %s * create_form_%s(void);\n",
	    fdtname, fname);
}


/* Prints the object declarations used in form */
static void
print_header_altformat(FILE * fn, FL_FORM * form, const char *fname)
{
    FL_OBJECT *obj;
    char name[MAX_VAR_LEN], cbname[MAX_VAR_LEN], argname[MAX_VAR_LEN];
    int first = 1;

    fprintf(fn, "extern FL_FORM *%s;\n\n", fname);
    init_array_names();
    obj = form->first;

    while ((obj = obj->next) != NULL)
    {
	get_object_name(obj, name, cbname, argname);
	if (name[0] != 0)
	{
	    if (!check_array_name(name))
	    {
		fprintf(fn, first ? "extern FL_OBJECT\n" : ",\n");
		first = 0;
		fprintf(fn, "        *%s", name);
	    }
	}
    }

    if (arethere_array_names())
    {
	fprintf(fn, first ? "extern FL_OBJECT\n" : ",\n");
	first = 0;
	print_array_names(fn, 0);
    }

    if (!first)
	fprintf(fn, ";\n\n");
}

void
print_header(FILE * fn, FL_FORM * form, const char *fname)
{
    (!fdopt.altformat ? print_header_newformat : print_header_altformat)
	(fn, form, fname);
}

/*
 * All callback stubs
 */

void
output_callbacks(FILE * fn, FRM * fdform, int nform)
{
    int i;
    for (i = 0; i < nform; i++)
    {
	fprintf(fn, "/*** callbacks and freeobj handles for form %s ***/\n",
		fdform[i].fname);
	print_callbacks_and_globals(fn, fdform[i].form, 1);
	fprintf(fn, "\n");
    }
}


const char *
get_placement(FL_FORM * form)
{
    if (!form->first)
	return "FL_PLACE_CENTER";
    else if (!form->first->next)
	return "FL_PLACE_CENTER";
    else if (form->first->next->resize)
	return "FL_PLACE_CENTERFREE";
    else
	return "FL_PLACE_CENTER";
}

/*
 * A Main stub.
 */
static void
output_main_newformat(FILE * fn, FRM * fdform, int nform)
{
    char fdtname[MAX_VAR_LEN], *fname;
    int i;

    /* only output callback stubs if not already output */
    if (!fdopt.emit_cb)
	output_callbacks(fn, fdform, nform);

    fprintf(fn, "int main(int argc, char *argv[])\n{\n");	/* } */

    for (i = 0; i < nform; i++)
    {
	fname = fdform[i].fname;
	sprintf(fdtname, "FD_%s", fname);
	fprintf(fn, "   %s *fd_%s;\n", fdtname, fname);
    }
    fprintf(fn, "\n");

    fprintf(fn, "   fl_initialize(&argc, argv, 0, 0, 0);\n");

    for (i = 0; i < nform; i++)
    {
	fname = fdform[i].fname;
	fprintf(fn, "   fd_%s = create_form_%s();\n", fname, fname);
    }

    fprintf(fn, "\n   /* fill-in form initialization code */\n\n");

    fprintf(fn, "   /* show the first form */\n");
    fprintf(fn, "   fl_show_form(fd_%s->%s,%s,FL_FULLBORDER,\"%s\");\n",
	    fdform[0].fname, fdform[0].fname,
	    get_placement(fdform[0].form),
	    fdform[0].fname);	/* { */

    fprintf(fn, "   fl_do_forms();\n   return 0;\n}\n");
}

static void
output_main_altformat(FILE * fn, FRM * fdform, int nform)
{
    /* only output callback stubs if not already output */
    if (!fdopt.emit_cb)
	output_callbacks(fn, fdform, nform);

    fprintf(fn, "int main(int argc, char *argv[])\n{\n");	/* } */
    fprintf(fn, "\n");

    fprintf(fn, "   fl_initialize(&argc, argv, %s, 0, 0);\n", "0");
    fprintf(fn, "\n   create_the_forms();\n");
    fprintf(fn, "\n   /* fill-in form initialization code */\n\n");

    fprintf(fn, "   /* show the first form */\n");
    fprintf(fn, "   fl_show_form(%s,FL_PLACE_CENTER,FL_FULLBORDER,\"%s\");\n",
	    fdform[0].fname, fdform[0].fname);	/* { */

    fprintf(fn, "   fl_do_forms();\n   return 0;\n}\n");
}

void
output_main(FILE * fn, FRM * fdform, int nform)
{
    (fdopt.altformat ? output_main_altformat : output_main_newformat)
	(fn, fdform, nform);
}

/* set some defaults, unit/bw etc. */
static void
pre_form_output(FILE * fn)
{
    /* some global defaults */
    if (fdopt.unit != FL_COORD_PIXEL)
	fprintf(fn, "  int old_unit = fl_get_coordunit();\n");

    if (fd_bwidth != FL_BOUND_WIDTH && fd_bwidth != 0)
	fprintf(fn, "  int old_bw = fl_get_border_width();\n");

    if (fdopt.unit != FL_COORD_PIXEL)
	fprintf(fn, "\n  fl_set_coordunit(%s);", unit_name(fdopt.unit));

    if (fd_bwidth != FL_BOUND_WIDTH && fd_bwidth != 0)
	fprintf(fn, "\n  fl_set_border_width(%d);", fd_bwidth);

    fprintf(fn, "\n");
}

static void
post_form_output(FILE * fn)
{
    if (fdopt.unit != FL_COORD_PIXEL)
	fprintf(fn, "  fl_set_coordunit(old_unit);\n");
    if (fd_bwidth != FL_BOUND_WIDTH && fd_bwidth)
	fprintf(fn, "  fl_set_border_width(old_bw);\n");

}


static void
output_object(FILE * fn, FL_OBJECT * obj, int altfmt)
{
    FL_OBJECT *defobj, fakeobj;
    char name[MAX_VAR_LEN], cbname[MAX_VAR_LEN], argname[MAX_VAR_LEN];
    char *p, fdvname[MAX_VAR_LEN];
    double sc = get_conversion_factor();

    if (obj->is_child)
	return;

    strcpy(fdvname, "fdui");
    get_object_name(obj, name, cbname, argname);
    if (obj->objclass == FL_BEGIN_GROUP)
    {
	if (name[0] != '\0')
	{
	    if (!altfmt)
		fprintf(fn, "\n  %s->%s =", fdvname, name);
	    else
		fprintf(fn, "\n  %s =", name);
	}
	fprintf(fn, " fl_bgn_group();\n");
    }
    else if (obj->objclass == FL_END_GROUP)
    {
	fprintf(fn, "  fl_end_group();\n\n");
    }
    else
    {
	defobj = find_class_default(obj->objclass, obj->type);
	if (!defobj)
	{
	    M_err("OutputObj", "Failed to create default (class=%s,type=%s)",
		  find_class_name(obj->objclass),
		  find_type_name(obj->objclass, obj->type));
	    exit(1);
	}

	fprintf(fn, "  ");

	if (name[0] != '\0')
	{
	    if (!altfmt)
		fprintf(fn, "%s->%s = ", fdvname, name);
	    else
		fprintf(fn, "%s = ", name);
	}

	fprintf(fn, "obj = ");
	fprintf(fn, "fl_add_%s(", find_class_name(obj->objclass));
	fprintf(fn, "FL_%s,", find_type_name(obj->objclass, obj->type));

	fakeobj.x = obj->x;
	fakeobj.y = obj->y;
	fakeobj.w = obj->w;
	fakeobj.h = obj->h;
	fl_scale_object(&fakeobj, sc, sc);

	if (obj->objclass != FL_FREE)
	    /* ( */
	    fprintf(fn, "%d,%d,%d,%d,\"%s\");\n", fakeobj.x, fakeobj.y,
		    fakeobj.w, fakeobj.h, get_label(obj, 1));
	else
	    /* ( */
	    fprintf(fn, "%d,%d,%d,%d,\"%s\",\n\t\t\t%s);\n",
		    fakeobj.x, fakeobj.y, fakeobj.w, fakeobj.h,
		    get_label(obj, 1), get_free_handle(obj, name));


	if ((p = get_shortcut_string(obj)) && *p && obj->type != FL_RETURN_BUTTON)
	    fprintf(fn, "    fl_set_%s_shortcut(obj,\"%s\",1);\n",
	       supported_shortcut(obj->objclass), get_shortcut_string(obj));

	if (obj->boxtype != defobj->boxtype && obj->objclass != FL_BOX)
	{
	    if (obj->objclass != FL_CANVAS && obj->objclass != FL_FRAME)
		emit_attrib(fn, obj->boxtype, vn_btype, "fl_set_object_boxtype");
	}

	if (obj->col1 != defobj->col1 || obj->col2 != defobj->col2)
	{
	    if (obj->objclass != FL_CANVAS)
		fprintf(fn, "    fl_set_object_color(obj,%s,%s);\n",
			fl_query_colorname(obj->col1),
			fl_query_colorname(obj->col2));
	}

	if (obj->lcol != defobj->lcol)
	{
	    fprintf(fn, "    fl_set_object_lcolor(obj,%s);\n",
		    fl_query_colorname(obj->lcol));
	}

	if (obj->lsize != defobj->lsize)
	    emit_attrib(fn, obj->lsize, vn_lsize, "fl_set_object_lsize");

	if (obj->align != defobj->align)
	    emit_attrib(fn, obj->align, vn_align, "fl_set_object_lalign");

	if (obj->lstyle != defobj->lstyle)
	    fprintf(fn, "    fl_set_object_lstyle(obj,%s);\n",
		    style_name(obj->lstyle));

	if (obj->nwgravity != defobj->nwgravity ||
	    obj->segravity != defobj->segravity)
	    fprintf(fn, "    fl_set_object_gravity(obj, %s, %s);\n",
		    gravity_name(obj->nwgravity),
		    gravity_name(obj->segravity));

	/* resize is only used if default gravity */
	if (obj->resize != defobj->resize &&
	    need_resize(obj->nwgravity, obj->segravity))
	    fprintf(fn, "    fl_set_object_resize(obj, %s);\n",
		    resize_name(obj->resize));

	if (cbname[0] != 0)
	    fprintf(fn, "    fl_set_object_callback(obj,%s,%s);\n",
		    cbname, argname);
    }

    /* generate object class specifc settings */
    emit_objclass_spec_info(fn, obj);
}
