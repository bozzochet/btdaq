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
 * $Id: load.c,v 0.36 1999/07/17 03:32:50 zhao beta $
 *
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1997-2000  by T.C. Zhao
 *  All rights reserved.
 *.
 *
 * Read a form defination file and call PS output routines
 *
 */

#include "forms.h"
#include "fd2ps.h"
#include <stdlib.h>
#include <ctype.h>

#define MAGIC3  12322
#define MAGIC4  13000

extern char *de_space(char *s);

static FL_FORM *
make_form(int type, int w, int h)
{
    FL_FORM *form;

    form = calloc(1, sizeof(*form));
    form->w = w;
    form->h = h;
    form->first = form->last = 0;
    form->label = 0;

    return form;
}

#define sl_focus(t) (t==FL_HOR_BROWSER_SLIDER   ||   \
		     t==FL_VERT_BROWSER_SLIDER  ||   \
		     t==FL_HOR_BROWSER_SLIDER2  ||   \
		     t==FL_VERT_BROWSER_SLIDER2 ||   \
		     t==FL_HOR_THIN_SLIDER      ||   \
		     t==FL_VERT_THIN_SLIDER )

FL_OBJECT *
flps_make_object(int objclass, int type, int x, int y,
		 int w, int h, const char *label, void *p)
{
    FL_OBJECT *ob = calloc(1, sizeof(*ob));
    ALLSPEC *sp;

    ob->objclass = objclass;
    ob->type = type;
    ob->boxtype = FL_UP_BOX;
    ob->x = x;
    ob->y = y;
    ob->w = w;
    ob->h = h;

    ob->label = fl_strdup(label ? label : "");

    /* default. Must agree with FL default */
    ob->align = FL_ALIGN_CENTER;
    ob->col1 = FL_COL1;
    ob->col2 = FL_MCOL;
    ob->lcol = FL_BLACK;

    ob->lsize = FL_DEFAULT_FONT;
    ob->lstyle = FL_NORMAL_STYLE;
    ob->shortcut = calloc(1, sizeof(*(ob->shortcut)));
    ob->shortcut[0] = 0;

    ob->next = ob->prev = 0;
    ob->form = 0;
    ob->bw = (psinfo.bw && FL_abs(psinfo.bw) <= 6) ?
	psinfo.bw : FL_BOUND_WIDTH;

    sp = ob->spec = calloc(1, sizeof(ALLSPEC));
    sp->content = calloc(MAX_CONTENT + 1, sizeof(*sp->content));
    sp->mode = calloc(MAX_CONTENT + 1, sizeof(*sp->mode));

    sp->fontstyle = FL_NORMAL_STYLE;
    sp->fontsize = FL_DEFAULT_FONT;

    /* set object specific defaults */
    if (ob->objclass == FL_SLIDER || ob->objclass == FL_VALSLIDER ||
	ob->objclass == FL_THUMBWHEEL)
    {
	sp->val = 0.5;
	sp->prec = 2;
	sp->max = 1.0;
	sp->min = 0.0;
	sp->slsize = FL_SLIDER_WIDTH;
	if (sl_focus(ob->type))
	    sp->slsize *= 1.5;
    }
    else if (ob->objclass == FL_SCROLLBAR)
    {
	ob->col1 = ob->col2 = FL_COL1;

	sp->val = 0.5;
	sp->prec = 2;
	sp->max = 1.0;
	sp->min = 0.0;
	sp->slsize = FL_SLIDER_WIDTH;
	if (ob->type == FL_HOR_THIN_SCROLLBAR ||
	    ob->type == FL_VERT_THIN_SCROLLBAR ||
	    ob->type == FL_HOR_SCROLLBAR ||
	    ob->type == FL_VERT_SCROLLBAR)
	    sp->slsize *= 1.6;
    }
    else if (ob->objclass == FL_POSITIONER)
    {
	sp->xmin = sp->ymin = 0.0;
	sp->xmax = sp->ymax = 1.0;
	sp->xval = sp->yval = 0.5;
    }
    else if (ob->objclass == FL_COUNTER)
    {
	sp->val = 0.0;
	sp->prec = 1;
	sp->min = -1000000.0;
	sp->max = 1000000.0;
    }
    else if (ob->objclass == FL_DIAL)
    {
	sp->min = 0.0;
	sp->max = 1.0;
	sp->val = 0.5;
	sp->thetai = 0.0;
	sp->thetaf = 360;
	sp->origin = 270;
	sp->direction = FL_DIAL_CW;
    }
    else if (ob->objclass == FL_PIXMAP || ob->objclass == FL_PIXMAPBUTTON)
    {
	sp->align = FL_ALIGN_CENTER;
	sp->dx = sp->dy = 3;
	sp->show_focus = 1;
	psinfo.epsf_import = 1;
    }
    else if (ob->objclass == FL_CHOICE)
    {
	sp->align = FL_ALIGN_CENTER;
	sp->fontsize = FL_DEFAULT_FONT;
    }
    else if (ob->objclass == FL_BROWSER)
    {
	sp->fontsize = FL_DEFAULT_FONT;
	sp->h_pref = FL_AUTO;
	sp->v_pref = FL_AUTO;
    }

    return ob;
}

static void
myfgets(char *line, FILE * fl)
{
    char tmpstr[10000];		/* Maximal label length is limited here. */
    int i = 0, j, ch;

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

static int
read_key_val(FILE * fp, char *key, char *val)
{
    char buf[1024], *p;

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



/* Adds an object to the form. */
static void
flps_add_object(FL_FORM * form, FL_OBJECT * obj)
{
    /* Checking for correct behaviour. */
    if (obj == NULL)
    {
	fprintf(stderr, "fl_add_object:Trying to add NULL object.\n");
	return;
    }

    if (form == NULL)
    {

	fprintf(stderr, "fl_add_object:Trying to add object to NULL form\n");
	return;
    }

    obj->prev = obj->next = NULL;

    if (psinfo.inverted)
	obj->y = form->h - obj->h - obj->y;

    if (form->first == NULL)
	form->first = form->last = obj;
    else
    {
	obj->prev = form->last;
	form->last->next = obj;
	form->last = obj;
    }
    obj->form = form;
}


#define NL '\n'


/* Sets the label, turning \n in NL */
static void
set_label(FL_OBJECT * obj, const char *str)
{
    int i = 0, j = 0;
    static char tmpstr[1024];

    do
    {
	if (str[i] == '\\' && str[i + 1] == 'n')
	{
	    tmpstr[j++] = NL;
	    i++;
	}
	else if (str[i] == '\\' && strncmp(str + i + 1, "010", 3) == 0)
	{
	    if (!obj->shortcut || !*obj->shortcut)
		tmpstr[j++] = *ul_magic_char;
	    i += 3;

	}
	else
	    tmpstr[j++] = str[i];
    }
    while (str[i++] != 0);

    tmpstr[j] = '\0';

    obj->label = fl_strdup(tmpstr);
}

extern int class_val(char *);
extern int boxtype_val(char *);
extern int align_val(const char *);
extern int style_val(const char *);
extern int lsize_val(const char *);
extern int pupmode_val(const char *);
extern int scbpref_val(const char *);

#if 0
static void (*modify_object) (FL_OBJECT *);

void
set_object_modifier(void (*m) (FL_OBJECT *))
{
    modify_object = m;
}
#endif

/***********************************************************************
 * Routines to load object  specific initialization code
 **********************************************************************/


#define ISSLIDER(cls)   (cls==FL_SLIDER||cls==FL_VALSLIDER)

/* lump together all spec info into one routine. We really
 * should include fd_spec here so we don't write the code
 * twice
 */
static void
load_objclass_spec_info(FILE * fp, FL_OBJECT * ob)
{
    char key[128], val[128];
    SPEC *sp = ob->spec;
    int c;

    while (ungetc((c = getc(fp)), fp) != '-' && c != '=')
    {
	read_key_val(fp, key, val);
	if (strlen(de_space(key)) < 2)
	    ;
	else if (strcmp(key, "bounds") == 0)
	    sscanf(val, "%f %f", &(sp->min), &(sp->max));
	else if (strcmp(key, "xbounds") == 0)
	    sscanf(val, "%f %f", &(sp->xmin), &(sp->xmax));
	else if (strcmp(key, "ybounds") == 0)
	    sscanf(val, "%f %f", &(sp->ymin), &(sp->ymax));
	else if (strcmp(key, "precision") == 0)
	    sscanf(val, "%d", &(sp->prec));
	else if (strcmp(key, "initial_val") == 0)
	    sscanf(val, "%f", &(sp->val));
	else if (strcmp(key, "value") == 0)
	{
	    if (ISBUTTON(ob->objclass) || ISCHOICE(ob->objclass))
		sp->int_val = atoi(val);
	    else
		sp->val = atof(val);
	}
	else if (strcmp(key, "xvalue") == 0)
	    sscanf(val, "%f", &(sp->xval));
	else if (strcmp(key, "yvalue") == 0)
	    sscanf(val, "%f", &(sp->yval));
	else if (strcmp(key, "xstep") == 0)
	    sscanf(val, "%f", &(sp->xstep));
	else if (strcmp(key, "ystep") == 0)
	    sscanf(val, "%f", &(sp->ystep));
	else if (strcmp(key, "sstep") == 0)
	    sscanf(val, "%f", &sp->sstep);
	else if (strcmp(key, "lstep") == 0)
	    sscanf(val, "%f", &sp->lstep);
	else if (strcmp(key, "file") == 0)
	    sp->file = fl_strdup(val);
	else if (strcmp(key, "focus") == 0)
	    sp->show_focus = atoi(val);
	else if (strcmp(key, "align") == 0)
	    sp->align = align_val(val);
	else if (strcmp(key, "slsize") == 0)
	    sscanf(val, "%f", &(sp->slsize));
	else if (strcmp(key, "angles") == 0)
	    sscanf(val, "%f %f", &(sp->thetai), &(sp->thetaf));
	else if (strcmp(key, "content") == 0)
	    sp->content[++sp->lines] = fl_strdup(val);
	else if (strcmp(key, "mode") == 0)
	    sp->mode[sp->lines] = pupmode_val(val);
	else if (strcmp(key, "h_pref") == 0)
	    sp->h_pref = scbpref_val(val);
	else if (strcmp(key, "v_pref") == 0)
	    sp->v_pref = scbpref_val(val);
	else if (strcmp(key, "dir") == 0)
	{
	    if (strcmp(val, "FL_DIAL_CCW") == 0)
		sp->direction = FL_DIAL_CCW;
	    else
		sp->direction = FL_DIAL_CW;
	}
	/* known but don't care */
	else if (strcmp(key, "return") == 0 || strcmp(key, "data") == 0 ||
		 strcmp(key, "ytics") == 0 || strcmp(key, "xtics") == 0 ||
		 strcmp(key, "xscale") == 0 || strcmp(key, "yscale") == 0 ||
		 strcmp(key, "struct") == 0 ||
		 strcmp(key, "global") == 0 ||
		 strcmp(key, "shortcut") == 0 ||
		 strcmp(key, "increment") == 0 ||
		 strcmp(key, "handler") == 0 ||
		 strcmp(key, "fullpath") == 0)
	    ;
	else
	    fprintf(stderr, "%s: Unknown spec (%s=%s) -- Ignored\n",
		    find_class_name(ob->objclass), key, val);
    }
}


static FL_OBJECT *
load_object(FL_FORM * form, FILE * fl)
{
    FL_OBJECT *obj;
    int objclass, type;
    float x, y, w, h;
    char name[64], cbname[64];
    char cn1[64], cn2[64];
    char objcls[64];
    char key[64], val[10000];

    /* Must demand the vital info */
    fscanf(fl, "\n--------------------\n");
    fscanf(fl, "class: %s\n", objcls);
    fscanf(fl, "type: %s\n", val);
    fscanf(fl, "box: %f %f %f %f\n", &x, &y, &w, &h);

    objclass = find_class_val(objcls);
    type = find_type_val(objclass, val);
    obj = flps_make_object(objclass, type, x, y, w, h, 0, 0);
    flps_add_object(form, obj);

    /* now parse the attributes */
    while (read_key_val(fl, key, val) != EOF)
    {
	if (strcmp(key, "boxtype") == 0)
	    obj->boxtype = boxtype_val(val);
	else if (strcmp(key, "colors") == 0)
	{
	    cn1[0] = cn2[0] = '\0';
	    sscanf(val, "%s %s", cn1, cn2);
	    obj->col1 = fl_get_namedcolor(cn1);
	    obj->col2 = fl_get_namedcolor(cn2);
	}
	else if (strcmp(key, "alignment") == 0)
	    obj->align = align_val(val);
	else if (strcmp(key, "style") == 0 || strcmp(key, "lstyle") == 0)
	    obj->lstyle = style_val(val);
	else if (strcmp(key, "size") == 0 || strcmp(key, "lsize") == 0)
	    obj->lsize = lsize_val(val);
	else if (strcmp(key, "lcol") == 0)
	    obj->lcol = fl_get_namedcolor(val);
	else if (strcmp(key, "label") == 0)
	    set_label(obj, val);
	else if (strcmp(key, "shortcut") == 0)
	    flps_set_object_shortcut(obj, val, 1);
	else if (strcmp(key, "callback") == 0)
	    strcpy(cbname, val);
	else if (strcmp(key, "name") == 0)
	    strcpy(name, val);
	else if (strcmp(key, "argument") == 0)
	    goto done;
	/* ignore uninteresting keywords */
    }
  done:
    load_objclass_spec_info(fl, obj);
    return obj;
}



static FL_FORM *
read_form(FILE * fl)
{
    double w, h;
    int onumb;
    FL_FORM *cur_form = 0;
    char fname[1024];

    fscanf(fl, "\n=============== FORM ===============\n");
    fname[0] = '\0';
    myfgets(fname, fl);
    fscanf(fl, "Width: %lf\n", &w);
    fscanf(fl, "Height: %lf\n", &h);

    cur_form = make_form(FL_NO_BOX, w, h);
    cur_form->label = fl_strdup(fname);

    fscanf(fl, "Number of Objects: %d\n", &onumb);

    for (; --onumb >= 0;)
	load_object(cur_form, fl);

    return cur_form;
}

int
load_form_definition(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    int fd_magic, nf;
    FL_FORM *form;
    char buf[256], ubuf[32];

    if (!fp)
    {
	perror(filename);
	return -1;
    };

    fscanf(fp, "Magic: %d\n\n", &fd_magic);

    if (fd_magic != MAGIC3 && fd_magic != MAGIC4)
    {
	fprintf(stderr, "Unknown file %s (magic %d)\n", filename, fd_magic);
	return -1;
    }

    psinfo.inverted = (fd_magic == MAGIC4);

    fscanf(fp, "Internal Form Definition File\n");
    fscanf(fp, "    (do not change)\n\n");
    fscanf(fp, "Number of forms: %d\n", &nf);
    psinfo.pages = nf;

    if (nf > 1 && psinfo.eps)
    {
	fprintf(stderr, "Requesting EPS output with more than one form.\n");
    }

    if (psinfo.verbose)
	fprintf(stderr, "%d forms will be converted\n", nf);

    /* read until we hit a seperator line */
    while (fgets(buf, sizeof(buf) - 1, fp) && buf[0] != '\n')
    {
	if (strncmp(buf, "Unit", 4) == 0)
	{
	    float xs, ys;
	    sscanf(buf, "Unit of measure: %s", ubuf);
	    psinfo.unit = unit_val(ubuf);
	    get_scale_unit(psinfo.unit, &xs, &ys);
	}
	else if (strncmp(buf, "Border", 6) == 0)
	{
	    sscanf(buf, "Border Width: %s", ubuf);
	    psinfo.bw = atoi(ubuf);
	}
	else if (strncmp(buf, "Snap", 4) == 0)
	{
	    ;
	}
	else
	{
	    fprintf(stderr, "Unknown token: %s", buf);
	}
    }

    if (psinfo.user_bw)
	psinfo.bw = psinfo.user_bw;

    for (; --nf >= 0;)
    {
	form = read_form(fp);
	ps_show_form(form);
    }

    fclose(fp);
    return 0;
}



/* attributes query routines */

#define VN(v)   {v,#v}

typedef struct
{
    int val;
    char *name;
}
VN_pair;


static VN_pair vn_btype[] =
{
    VN(FL_NO_BOX), VN(FL_UP_BOX), VN(FL_DOWN_BOX),
    VN(FL_FLAT_BOX), VN(FL_BORDER_BOX), VN(FL_SHADOW_BOX),
    VN(FL_FRAME_BOX), VN(FL_ROUNDED_BOX), VN(FL_RFLAT_BOX),
    VN(FL_RSHADOW_BOX), VN(FL_OVAL_BOX), VN(FL_EMBOSSED_BOX),
    VN(FL_ROUNDED3D_UPBOX), VN(FL_ROUNDED3D_DOWNBOX),
    VN(FL_OVAL3D_UPBOX), VN(FL_OVAL3D_DOWNBOX),
    VN(FL_TOPTAB_UPBOX), VN(FL_SELECTED_TOPTAB_UPBOX),
    VN(FL_BOTTOMTAB_UPBOX), VN(FL_SELECTED_BOTTOMTAB_UPBOX),
    VN(-1)
};


int
boxtype_val(char *cc)
{
    VN_pair *vn = vn_btype;
    for (; vn->val >= 0 && strcmp(cc, vn->name); vn++)
	;
    if (strcmp(cc, vn->name) == 0)
	return vn->val;
    fprintf(stderr, "unknown boxtype %s\n", cc);
    return atoi(cc);
}

static VN_pair vn_align[] =
{
    VN(FL_ALIGN_CENTER), VN(FL_ALIGN_TOP),
    VN(FL_ALIGN_BOTTOM), VN(FL_ALIGN_LEFT),
    VN(FL_ALIGN_RIGHT),
    VN(FL_ALIGN_TOP_LEFT),
    VN(FL_ALIGN_TOP_RIGHT),
    VN(FL_ALIGN_BOTTOM_LEFT),
    VN(FL_ALIGN_BOTTOM_RIGHT),
    VN(-1)
};

static VN_pair vn_lsize[] =
{
    VN(FL_DEFAULT_FONT), VN(FL_DEFAULT_SIZE),
    VN(FL_TINY_FONT), VN(FL_TINY_SIZE),
    VN(FL_SMALL_FONT), VN(FL_SMALL_SIZE),
    VN(FL_NORMAL_FONT), VN(FL_NORMAL_SIZE),
    VN(FL_MEDIUM_FONT), VN(FL_MEDIUM_SIZE),
    VN(FL_LARGE_FONT), VN(FL_LARGE_SIZE),
    VN(FL_HUGE_FONT), VN(FL_HUGE_SIZE),
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

static VN_pair vn_pupmode[] =
{
    VN(FL_PUP_NONE),
    VN(FL_PUP_GRAY),
    VN(FL_PUP_BOX),
    VN(FL_PUP_CHECK),
    VN(FL_PUP_RADIO),
    VN(-1)
};

static VN_pair vn_scbpref[] =
{
    VN(FL_OFF),
    VN(FL_ON),
    VN(FL_AUTO),
    VN(-1)
};


static int
pure_style_val(const char *cc)
{
    VN_pair *vn = vn_lstyle;

    for (; vn->val >= 0 && strcmp(cc, vn->name); vn++)
	;
    return (strcmp(cc, vn->name) == 0) ? vn->val : atoi(cc);
}


int
style_val(const char *cc)
{
    char lstyle[40], spstyle[40], *p;

    strcpy(lstyle, cc);
    spstyle[0] = '\0';
    if ((p = strchr(lstyle, '+')))
    {
	strcpy(spstyle, p + 1);
	*p = 0;
    }
    return pure_style_val(lstyle) + pure_style_val(spstyle);
}

int
lsize_val(const char *cc)
{
    VN_pair *vn = vn_lsize;

    for (; vn->val >= 0 && strcmp(cc, vn->name); vn++)
	;
    return (strcmp(cc, vn->name) == 0) ? vn->val : atoi(cc);

}

int
pupmode_val(const char *cc)
{
    VN_pair *vn = vn_pupmode;

    for (; vn->val >= 0 && strcmp(cc, vn->name); vn++)
	;
    return (strcmp(cc, vn->name) == 0) ? vn->val : atoi(cc);
}

int
scbpref_val(const char *cc)
{
    VN_pair *vn = vn_scbpref;

    for (; vn->val >= 0 && strcmp(cc, vn->name); vn++)
	;
    return (strcmp(cc, vn->name) == 0) ? vn->val : atoi(cc);
}


int
align_val(const char *cc)
{
    VN_pair *vn = vn_align;
    char s[128], *p;

    strcpy(s, cc);

    if ((p = strchr(s, '|')))
	*p = '\0';

    for (; vn->val >= 0 && strcmp(s, vn->name); vn++)
	;

    if (strcmp(s, vn->name) == 0)
	return p ? (vn->val | FL_ALIGN_INSIDE) : vn->val;

    if (!isdigit( ( int ) cc[0]))
	fprintf(stderr, "Unknown alignment %s\n", cc);
    return atoi(cc);
}


/** Remove leading space */
char *
de_space(char *s)
{
    register char *p;
    /* not all isspace considers '\t' a white space */
    for (p = s; p && ( isspace( ( int ) *p) || *p == '\t'); p++)
	;
    return (p == s) ? s : strcpy(s, p);
}
