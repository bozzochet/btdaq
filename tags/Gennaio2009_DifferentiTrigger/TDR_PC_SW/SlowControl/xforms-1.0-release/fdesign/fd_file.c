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
 * $Id: fd_file.c,v 1.0 2002/05/08 05:27:39 zhao Release $
 *
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 * This file is part of the Form Designer.
 *
 * It contains the routines to save and load forms in the internal
 * format used by the form designer. This is readable ASCII.
 *
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "forms.h"
#include "fd_main.h"

/* Writes an obj description to a file. Note externally the coordinate
 * system always starts from lower-left corner of the screen */

#if 0
#define OBJ_Y(ob)   ob->form->h - ob->y - ob->h
#else
#define OBJ_Y(ob)   obj->y
#endif

static void
save_object(FILE * fl, FL_OBJECT * obj)
{
    char name[MAX_VAR_LEN], cbname[MAX_VAR_LEN], argname[MAX_VAR_LEN];
    double sc = get_conversion_factor();
    FL_OBJECT fake_obj;

    if (obj->is_child)
	return;

    get_object_name(obj, name, cbname, argname);

    fprintf(fl, "\n--------------------\n");
    fprintf(fl, "class: %s\n", class_name(obj->objclass));
    fprintf(fl, "type: %s\n", find_type_name(obj->objclass, obj->type));

    fake_obj.x = obj->x;
    fake_obj.y = obj->y;
    fake_obj.w = obj->w;
    fake_obj.h = obj->h;
    fl_scale_object(&fake_obj, sc, sc);
    fprintf(fl, "box: %d %d %d %d\n", fake_obj.x, fake_obj.y, fake_obj.w, fake_obj.h);

    fprintf(fl, "boxtype: %s\n", boxtype_name(obj->boxtype));
    fprintf(fl, "colors: %s %s\n", fl_query_colorname(obj->col1),
	    fl_query_colorname(obj->col2));
    fprintf(fl, "alignment: %s\n", align_name(obj->align));
    fprintf(fl, "style: %s\n", style_name(obj->lstyle));
    fprintf(fl, "size: %s\n", lsize_name(obj->lsize));
    fprintf(fl, "lcol: %s\n", fl_query_colorname(obj->lcol));
    fprintf(fl, "label: %s\n", get_label(obj, 0));
    fprintf(fl, "shortcut: %s\n", get_shortcut_string(obj));
    fprintf(fl, "resize: %s\n", resize_name(obj->resize));
    fprintf(fl, "gravity: %s %s\n",
	    gravity_name(obj->nwgravity),
	    gravity_name(obj->segravity));
    fprintf(fl, "name: %s\n", name);
    fprintf(fl, "callback: %s\n", cbname);
    fprintf(fl, "argument: %s\n", argname);

    save_objclass_spec_info(fl, obj);
}

/* My version of fgets, removing heading name:  */
static void
myfgets(char *line, FILE * fl)
{
    char tmpstr[10000];		/* Maximal label length is limited here. */
    int i = 0, j;
    int ch;
    ch = fgetc(fl);
    while (ch != '\n' && ch != EOF)
    {
	tmpstr[i++] = ch;
	ch = fgetc(fl);
    }

    tmpstr[i] = '\0';

    i = 0;
    while (tmpstr[i] != ':' && tmpstr[i + 1] != ' ')
	i++;

    i += 2;
    j = 0;

    do
	line[j++] = tmpstr[i++];
    while (tmpstr[i - 1] != '\0');
}

/*
 * X version changed color systemtically, need to do a translation
 * from old fd files on the fly
 *
 */
typedef struct
{
    int oldval, newval;
}
Trantable;

static Trantable tcolor[] =
{
    {0, FL_BLACK},
    {1, FL_RED},
    {2, FL_GREEN},
    {3, FL_YELLOW},
    {4, FL_BLUE},
    {5, FL_MAGENTA},
    {6, FL_CYAN},
    {7, FL_WHITE},
    {8, FL_BOTTOM_BCOL},	/* approx */

    {9, FL_INDIANRED},
    {10, FL_PALEGREEN},
    {11, FL_PALEGREEN},
    {12, FL_SLATEBLUE},

    {35, FL_RIGHT_BCOL},
    {36, FL_RIGHT_BCOL},	/* approx */
    {37, FL_RIGHT_BCOL},	/* approx  */
    {40, FL_BOTTOM_BCOL},
    {47, FL_COL1},
    {49, FL_MCOL},
    {51, FL_TOP_BCOL},
    {55, FL_LEFT_BCOL}
};

static Trantable tclass[] =
{
    {1, FL_BOX},
    {2, FL_TEXT},
    {3, FL_BITMAP},
    {4, FL_CHART},
    {11, FL_BUTTON},
    {12, FL_LIGHTBUTTON},
    {13, FL_ROUNDBUTTON},
    {21, FL_SLIDER},
    {22, FL_DIAL},
    {23, FL_POSITIONER},
    {24, FL_VALSLIDER},
    {25, FL_COUNTER},
    {31, FL_INPUT},
    {41, FL_MENU},
    {42, FL_CHOICE},
    {61, FL_CLOCK},
    {62, FL_TIMER},
    {71, FL_BROWSER},
    {101, FL_FREE},
    {10000, FL_BEGIN_GROUP},
    {20000, FL_END_GROUP}
};

static Trantable talign[] =
{
    {0, FL_ALIGN_TOP},
    {1, FL_ALIGN_BOTTOM},
    {2, FL_ALIGN_LEFT},
    {3, FL_ALIGN_RIGHT},
    {4, FL_ALIGN_CENTER}
};

static Trantable tbtype[] =
{
    {0, FL_NO_BOX},
    {1, FL_UP_BOX},
    {2, FL_DOWN_BOX},
    {3, FL_FLAT_BOX},
    {4, FL_BORDER_BOX},
    {5, FL_SHADOW_BOX},
    {6, FL_FRAME_BOX},
    {7, FL_ROUNDED_BOX},
    {8, FL_RFLAT_BOX},
    {9, FL_RSHADOW_BOX}
};

static int
do_trans(Trantable * tab, int n, int old)
{
    Trantable *p = tab, *q;
    for (q = p + n; p < q; p++)
	if (p->oldval == old)
	    return p->newval;
    return old;
}

#define new_class(o)  do_trans(tclass, sizeof(tclass)/sizeof(tclass[0]),o)
#define new_color(o)  do_trans(tcolor, sizeof(tcolor)/sizeof(tcolor[0]),o)
#define new_align(o)  do_trans(talign, sizeof(talign)/sizeof(talign[0]),o)
#define new_btype(o)  do_trans(tbtype, sizeof(tbtype)/sizeof(tbtype[0]),o)

static void
fd_skip_comment(FILE * fp)
{
#if 0
    int c, done = 0;

    while (!done)
    {
	if ((c = getc(fp)) == '#' || c == ';')
	    while ((c = getc(fp)) != '\n' && c != EOF)
		;
	else
	{
	    done = 1;
	    ungetc(c, fp);
	}
    }
#endif
}

/* Read lines consisting of keyword: value and split. Return -1 if EOF
   or error */
int
read_key_val(FILE * fp, char *key, char *val)
{
    char buf[1024], *p;

    fd_skip_comment(fp);

    if (!fgets(buf, 1024, fp) || ferror(fp))
	return EOF;

    buf[1023] = 0;
    val[0] = key[0] = 0;

    /* nuke the new line */
    if ((p = strchr(buf, '\n')))
	*p = '\0';

    if (!(p = strchr(buf, ':')))
    {
	strcpy(key, "?");
	return EOF;
    }

    *p = '\0';
    strcpy(key, buf);

    if (*(p + 1))
	strcpy(val, p + 2);
    return 0;
}

/*
 *  Loads an object from the file and returns it.
 *
 *  object coordinates are measured from lower-left corner. fl_add_object
 *  will do the proper conversion but if obj->y is manipulated directly
 *  need to do the transformation manually
 */
static FL_OBJECT *
load_object(FILE * fl)
{
    FL_OBJECT *obj;
    int objclass, type;
    float x, y, w, h;
    char name[MAX_VAR_LEN], cbname[MAX_VAR_LEN], argname[MAX_VAR_LEN];
    char cn1[MAX_VAR_LEN], cn2[MAX_VAR_LEN];
    char objcls[MAX_VAR_LEN];
    char key[MAX_VAR_LEN], val[10000];

    /* Must demand the vital info */
    if (fscanf(fl, "\n--------------------\n") == EOF)
    {
	M_err("LoadObject", "Error reading input file");
	return 0;
    }

    fscanf(fl, "class: %s\n", objcls);
    fscanf(fl, "type: %s\n", val);
    fscanf(fl, "box: %f %f %f %f\n", &x, &y, &w, &h);

    objclass = class_val(objcls);
    type = find_type_value(objclass, val);

    if (fd_magic == MAGIC2)
	objclass = new_class(objclass);

    if (cur_form && fd_magic != FD_V1)
	y = (cur_form->h - y - h);

    /* create this object */
    obj = add_an_object(objclass, type, x, y, w, h);

    if (obj == NULL)
    {
	char tmpbuf[128];
	sprintf(tmpbuf, "Object (class=%s(%d) type=%s) discarded",
		objcls, objclass, val);
	fl_show_alert("UnknownObject", tmpbuf, "", 1);
	return NULL;
    }

    /* now parse the attributes */
    while (read_key_val(fl, key, val) != EOF)
    {
	if (strcmp(key, "boxtype") == 0)
	    obj->boxtype = boxtype_val(val);
	else if (strcmp(key, "colors") == 0)
	{
	    cn1[0] = cn2[0] = '\0';
	    sscanf(val, "%s %s", cn1, cn2);
	    obj->col1 = fl_query_namedcolor(cn1);
	    obj->col2 = fl_query_namedcolor(cn2);
	    if (obj->col1 == 0x8fffffff)
		obj->col1 = FL_NoColor;
	}
	else if (strcmp(key, "alignment") == 0)
	    obj->align = align_val(val);
	else if (strcmp(key, "style") == 0 || strcmp(key, "lstyle") == 0)
	    obj->lstyle = style_val(val);
	else if (strcmp(key, "size") == 0 || strcmp(key, "lsize") == 0)
	    obj->lsize = lsize_val(val);
	else if (strcmp(key, "lcol") == 0)
	    obj->lcol = fl_query_namedcolor(val);
	else if (strcmp(key, "resize") == 0)
	    obj->resize = resize_val(val);
	else if (strcmp(key, "label") == 0)
	    set_label(obj, val);
	else if (strcmp(key, "shortcut") == 0)
	    set_shortcut(obj, val);
	else if (strcmp(key, "callback") == 0)
	    strcpy(cbname, val);
	else if (strcmp(key, "name") == 0)
	    strcpy(name, val);
	else if (strcmp(key, "gravity") == 0)
	{
	    cn1[0] = cn2[0] = '\0';
	    sscanf(val, "%s %s", cn1, cn2);
	    obj->nwgravity = gravity_val(cn1);
	    obj->segravity = gravity_val(cn2);
	}
	else if (strcmp(key, "argument") == 0)
	{
	    strcpy(argname, val);
	    goto done;
	}
	else
	{
	    fprintf(stderr, "Unknown keyword %s ignored\n", key);
	}
    }

  done:
    /* do the translation from old fdesign on the fly */
    if (fd_magic == MAGIC2)
    {
	obj->col1 = new_color(obj->col1);
	obj->col2 = new_color(obj->col2);
	obj->lcol = new_color(obj->lcol);
	obj->align = new_align(obj->align);
	obj->boxtype = new_btype(obj->boxtype);
    }

    set_object_name(obj, name, cbname, argname);

    /* load object specific info */
    fd_skip_comment(fl);
    load_objclass_spec_info(fl, obj);

    return obj;
}

/* Saves a form definition to the file. */
void
write_form(FILE * fl, FL_FORM * form, char fname[])
{
    int onumb;
    FL_OBJECT *obj;

    fprintf(fl, "\n=============== FORM ===============\n");
    fprintf(fl, "Name: %s\n", fname);
    fprintf(fl, "Width: %d\n", convert_u(form->w));
    fprintf(fl, "Height: %d\n", convert_u(form->h));

    /* print the object number */
    for (onumb = 0, obj = form->first->next; obj; obj = obj->next)
	onumb += obj->is_child == 0;

    fprintf(fl, "Number of Objects: %d\n", onumb);

    /* print the objects */
    obj = form->first->next;
    while (obj != NULL)
    {
	save_object(fl, obj);
	obj = obj->next;
    }
}

/* Loads a form definition from the file */
int
read_form(FILE * fl, char *fname)
{
    double w, h;
    int onumb, i, ok;
    char buf[256], *s;

    /* skip until we get ===, the form seperator */
    while (fgets(buf, sizeof(buf) - 1, fl) && strncmp(buf, "===", 3))
	;

    myfgets(fname, fl);
    fscanf(fl, "Width: %lf\n", &w);
    fscanf(fl, "Height: %lf\n", &h);

    if (w <= 0.0 || h <= 0.0 || feof(fl))
    {
	M_err("LoadForm", " Invalid Width/Height: %f %f", w, h);
	return -1;
    }

    cur_form = fl_bgn_form(FL_NO_BOX, (FL_Coord) w, (FL_Coord) h);
    fl_end_form();

    /* between width/height and number of objects, we can put anything we
       want. */
    while ((s = fgets(buf, sizeof(buf) - 1, fl)) &&
	   strncmp(s, "Number of O", 11))
    {
	/* see what this is */

    }


    sscanf(buf, "Number of Objects: %d", &onumb);

    for (ok = 1, i = 0; i < onumb && ok; i++)
	ok = (load_object(fl) != 0);

    return 0;
}
