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
 * $Id: fd_attribs.c,v 1.0 2002/05/08 05:27:39 zhao Release $
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 * It contains some routines to deal with attributes of the objects.
 *
 */

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "fd_main.h"

/****************** FORMS AND CALL-BACKS ************************/

int auto_apply = 1;

static void readback_attributes(FL_OBJECT *);

/* Call-back routine to get a color from the user */
void
setcolor_cb(FL_OBJECT * obj, long arg)
{
    int col1 = fl_show_colormap(obj->col1);
    fl_set_object_color(obj, col1, col1);
    auto_apply_cb(0, 0);
}

/* if change attributes automatically */
void
auto_apply_cb(FL_OBJECT * ob, long arg)
{
    if (auto_apply)
	apply_cb(ob, arg);
}

/* the auto-apply setting itself */

void
autoapply_setting_cb(FL_OBJECT * ob, long arg)
{
    if (!(auto_apply = fl_get_button(ob)))
	fl_show_object(fd_attrib->applyobj);
    else
	fl_hide_object(fd_attrib->applyobj);
}

/******* For cancel and restore operation *******{*/

static FL_OBJECT *oldcopy, *curobj;	/* object being changed */
static char oldname[MAX_VAR_LEN];
static char oldcbname[MAX_VAR_LEN], oldargname[MAX_VAR_LEN];

static void
save_object(FL_OBJECT * obj)
{
    char *ol;
    long *os;

    get_object_name(obj, oldname, oldcbname, oldargname);
    if (!oldcopy)
	oldcopy = fl_make_object(0, 0, 0, 0, 0, 0, 0, 0);
    ol = oldcopy->label;
    os = oldcopy->shortcut;
    *oldcopy = *obj;
    oldcopy->label = ol;
    oldcopy->shortcut = os;
    fl_set_object_label(oldcopy, obj->label);
}

/* duplicate everything in oldobj to obj */
static void
restore_object(FL_OBJECT * obj)
{
    char *ol = obj->label;
    long *os = obj->shortcut;

    set_object_name(obj, oldname, oldcbname, oldargname);
    *obj = *oldcopy;
    obj->label = ol;
    obj->shortcut = os;
    fl_set_object_label(obj, oldcopy->label);
}

/********** End of cancel/restore ********}***/

/* really change object attributes */
void
apply_cb(FL_OBJECT * obj, long arg)
{
    readback_attributes(curobj);
    change_selected_objects(curobj);
    redraw_the_form(0);
}

/* restore from the original copy */
void
restore_cb(FL_OBJECT * ob, long data)
{
    restore_object(curobj);
    show_attributes(curobj);
    change_selected_objects(curobj);
    redraw_the_form(0);
}

/****************** GLOBAL INITIALIZATION  ************************/

static FL_OBJECT *fnts;
static void
add_font_choice(const char *p)
{
    fl_addto_choice(fnts, p);
}

/* font sizes. Need to do this because of symbolic names */

typedef struct
{
    int size;
    char *name, *sc;
}
Fsizes;

static Fsizes fsizes[] =
{
    {FL_TINY_SIZE, "Tiny", "Tt#t"},
    {FL_SMALL_SIZE, "Small", "Ss#s"},
    {FL_NORMAL_SIZE, "Normal", "Nn#n"},
    {FL_MEDIUM_SIZE, "Medium", "Mm#m"},
    {FL_LARGE_SIZE, "Large", "Ll#l"},
    {FL_HUGE_SIZE, "Huge", "Hh#h"},
    {11, "Variable", ""}
};
#define NFSIZE (sizeof(fsizes) / sizeof(fsizes[0]))

static void
attrib_init(FD_generic_attrib * ui)
{
    static int attrib_initialized;
    int i;
    char buf[64];
    VN_pair *vp;

    if (attrib_initialized)
	return;

    attrib_initialized = 1;

    fl_clear_choice(ui->boxobj);
    for (i = 1, vp = vn_btype; vp->val >= 0; vp++, i++)
    {
	fl_addto_choice(ui->boxobj, vp->shown);
	fl_set_choice_item_shortcut(ui->boxobj, i, vp->hotkey);
    }

    fl_set_object_return(ui->nameobj, FL_RETURN_END);
    fl_set_object_return(ui->cbnameobj, FL_RETURN_END);

    /* resize */
    fl_set_choice_fontsize(ui->resize, fd_align_fontsize);
    for (vp = vn_resize; vp->val >= 0; vp++)
	fl_addto_choice(ui->resize, vp->name + 3);

    /* gravity. Due to compatibilities issues, there are more than need in
       vn_gravity */
    for (i = 0, vp = vn_gravity; vp->val >= 0 && i < 9; vp++, i++)
    {
	fl_addto_choice(ui->nwgravity, vp->name + 3);
	fl_addto_choice(ui->segravity, vp->name + 3);
    }


    /* align */
    fl_set_choice_fontsize(ui->align, fd_align_fontsize);
    for (vp = vn_align; vp->val >= 0; vp++)
	fl_addto_choice(ui->align, vp->name + 9);
    fl_addto_choice(ui->inside, "Inside|Outside");

    /* font stuff */
    fnts = ui->fontobj;
    fl_enumerate_fonts(add_font_choice, 1);
    fl_addto_choice(ui->styleobj, "Normal|Shadow|Engraved|Embossed");

    /* size */
    for (i = 0; i < NFSIZE; i++)
    {
	if (fsizes[i].size == FL_NORMAL_SIZE)
	{
	    fsizes[i].name = "Normal";
	    fsizes[i].sc = "Nn#n";
	}
	sprintf(buf, "%2d  %s%%r1", fsizes[i].size, fsizes[i].name);
	fl_addto_choice(ui->sizeobj, buf);
	fl_set_choice_item_shortcut(ui->sizeobj, i + 1, fsizes[i].sc);
    }

}

/* check for obvious errors */
#define OK_letter(c)  \
          (*c=='_' || *c=='[' || *c==']' || *c=='.' ||\
          (*c==':' && *++c==':') || (*c=='-' && *++c=='>'))

static int
valid_c_identifier(const char *s)
{
    if (fdopt.lax)
	return 1;

    /* empty is considered to be valid */
    if (!s || !*s || (*s == ' ' && *(s + 1) == '\0'))
	return 1;

    if (!isalpha( ( int ) *s) && *s != '_')
	return 0;
    for (s++; *s; s++)
	if (!isalnum( ( int ) *s) && !OK_letter(s))
	    return 0;
    return 1;
}

static int
validate_cvar_name(FL_OBJECT * ob)
{
    const char *s = fl_get_input(ob);

    if (!valid_c_identifier(s))
    {
	char buf[256];
	sprintf(buf, "Invalid C identifier specified for object %s", ob->label);
	fl_show_alert("Error", buf, s, 0);
	fl_set_focus_object(ob->form, ob);
	return 0;
    }
    return 1;
}

static int
validate_attributes(void)
{
    return (validate_cvar_name(fd_generic_attrib->nameobj) &&
	    validate_cvar_name(fd_generic_attrib->cbnameobj));
}

void
validate_cvar_name_cb(FL_OBJECT * ob, long data)
{
    validate_cvar_name(ob);
}


/********************* THE ACTUAL ROUTINES ****************************/
static void
readback_attributes(FL_OBJECT * obj)
{
    int spstyle, warn = 0;
    char name[128], cbname[128];
    char tmpbuf[128];
    static char *m[] =
    {"object name", "callback", "object name & callback"};

    obj->boxtype = fl_get_choice(fd_generic_attrib->boxobj) - 1;

    /* label style consists of two parts */
    obj->lstyle = fl_get_choice(fd_generic_attrib->fontobj) - 1;
    spstyle = fl_get_choice(fd_generic_attrib->styleobj) - 1;
    obj->lstyle += spstyle == 3 ? FL_EMBOSSED_STYLE : (spstyle * FL_SHADOW_STYLE);
    obj->col1 = fd_generic_attrib->col1obj->col1;
    obj->col2 = fd_generic_attrib->col2obj->col1;
    obj->lcol = fd_generic_attrib->lcolobj->col1;


    snprintf(tmpbuf,sizeof(tmpbuf),"FL_ALIGN_%s",
                fl_get_choice_text(fd_generic_attrib->align));
    obj->align = align_val(tmpbuf);

    if (fl_get_choice(fd_generic_attrib->inside) == 1 && obj->align != FL_ALIGN_CENTER)
	obj->align |= FL_ALIGN_INSIDE;
    else
	obj->align &= ~FL_ALIGN_INSIDE;

    snprintf(tmpbuf,sizeof(tmpbuf),"FL_%s",
                fl_get_choice_text(fd_generic_attrib->resize));
    obj->resize = resize_val(tmpbuf);

    snprintf(tmpbuf,sizeof(tmpbuf),"FL_%s",
                fl_get_choice_text(fd_generic_attrib->segravity));
    obj->segravity = gravity_val(tmpbuf);

    snprintf(tmpbuf,sizeof(tmpbuf),"FL_%s",
                fl_get_choice_text(fd_generic_attrib->nwgravity));
    obj->nwgravity = gravity_val(tmpbuf);


    obj->lsize = fsizes[fl_get_choice(fd_generic_attrib->sizeobj) - 1].size;

    set_label(obj, fl_get_input(fd_generic_attrib->labelobj));
    set_shortcut(obj, fl_get_input(fd_generic_attrib->scobj));

    strncpy(name, fl_get_input(fd_generic_attrib->nameobj),sizeof(name));
    name[sizeof(name)-1] = '\0';

    strncpy(cbname, fl_get_input(fd_generic_attrib->cbnameobj),sizeof(cbname));
    cbname[sizeof(cbname)-1] = '\0';

    if (!valid_c_identifier(name))
    {
	name[0] = '\0';
	warn = 1;
    }

    if (!valid_c_identifier(cbname))
    {
	cbname[0] = '\0';
	warn += 2;
    }

    if (warn)
	fl_show_alert("Error", "Invalid C identifier specified for",
		      m[warn - 1], 0);
    set_object_name(obj, name, cbname, fl_get_input(fd_generic_attrib->argobj));

    /* change type need to be the last call as it may create objects based on 


       the current object, which need to have the latest attributes */
    if (obj->objclass == FL_BOX)
	change_type(obj, obj->boxtype);
    else
	change_type(obj, fl_get_choice(fd_generic_attrib->typeobj) - 1);
}

void
show_attributes(const FL_OBJECT * obj)
{
    char objname[MAX_VAR_LEN], cbname[MAX_VAR_LEN], argname[MAX_VAR_LEN];
    char buf[MAX_VAR_LEN];
    int i, lstyle, spstyle, oksize, align = obj->align & ~FL_ALIGN_INSIDE;
    static char othersize[32];

    fl_freeze_form(fd_generic_attrib->generic_attrib);

    /* Fill in list of types */
    fl_clear_choice(fd_generic_attrib->typeobj);
    fl_set_choice_fontsize(fd_generic_attrib->typeobj, fd_type_fontsize);
    if (obj->objclass != FL_BOX)
    {
	for (i = 0; i < find_class_maxtype(obj->objclass); i++)
	{
	    strcat(strcpy(buf, find_type_name(obj->objclass, i)), "%r1");
	    fl_addto_choice(fd_generic_attrib->typeobj, buf);
	}
	fl_set_choice(fd_generic_attrib->typeobj, obj->type + 1);
    }

    /* Fil in settings */
    fl_set_choice(fd_generic_attrib->boxobj, obj->boxtype + 1);
    fl_set_choice_text(fd_generic_attrib->align, align_name(align) + 9);
    fl_set_choice(fd_generic_attrib->inside, (obj->align == align) + 1);


    lstyle = obj->lstyle % FL_SHADOW_STYLE;
    spstyle = (obj->lstyle / FL_SHADOW_STYLE);

    if (spstyle >= 3)
	spstyle = 3;

    fl_set_choice(fd_generic_attrib->fontobj, lstyle + 1);
    fl_set_choice(fd_generic_attrib->styleobj, spstyle + 1);

    for (oksize = i = 0; !oksize && i < NFSIZE; i++)
	if ((oksize = obj->lsize == fsizes[i].size))
	    fl_set_choice(fd_generic_attrib->sizeobj, i + 1);

    if (!oksize)
    {
	sprintf(othersize, "%d (Variable)", obj->lsize);
	fsizes[NFSIZE - 1].size = obj->lsize;
	fsizes[NFSIZE - 1].name = othersize;
	fl_replace_choice(fd_generic_attrib->sizeobj, NFSIZE, othersize);
	fl_set_choice(fd_generic_attrib->sizeobj, NFSIZE);
    }

    /* gravity stuff */
    fl_set_choice_text(fd_generic_attrib->resize, resize_name(obj->resize) + 3);
    fl_set_choice_text(fd_generic_attrib->nwgravity, gravity_name(obj->nwgravity) + 3);
    fl_set_choice_text(fd_generic_attrib->segravity, gravity_name(obj->segravity) + 3);

    get_object_name(obj, objname, cbname, argname);
    fl_set_input(fd_generic_attrib->labelobj, get_label(obj, 0));
    fl_set_input(fd_generic_attrib->nameobj, objname);
    fl_set_input(fd_generic_attrib->cbnameobj, cbname);
    fl_set_input(fd_generic_attrib->argobj, argname);

    fl_set_input(fd_generic_attrib->scobj, get_shortcut_string(obj));

    fl_set_object_color(fd_generic_attrib->col1obj, obj->col1, obj->col1);
    fl_set_object_color(fd_generic_attrib->col2obj, obj->col2, obj->col2);
    fl_set_object_color(fd_generic_attrib->lcolobj, obj->lcol, obj->lcol);
    fl_unfreeze_form(fd_generic_attrib->generic_attrib);
}


/* Displays the form to change the attributes. all indicates whether
   all label, name, etc. should also be changed */
int
change_object(FL_OBJECT * obj, int all)
{
    FL_OBJECT *retobj;
    FD_generic_attrib *ui = fd_generic_attrib;

    attrib_init(fd_generic_attrib);

    /* save current attributes for later restore */
    curobj = obj;
    save_object(obj);


    /* Show only required parts */
    if (all)
    {
	fl_show_object(ui->labelobj);
	fl_show_object(ui->scobj);
	fl_show_object(ui->nameobj);
	fl_show_object(ui->cbnameobj);
	fl_show_object(ui->argobj);
    }
    else
    {
	fl_hide_object(ui->labelobj);
	fl_hide_object(ui->scobj);
	fl_hide_object(ui->nameobj);
	fl_hide_object(ui->cbnameobj);
	fl_hide_object(ui->argobj);
    }

    /* show attributes of the current object */
    show_attributes(obj);

    /* Do interaction */
    fl_deactivate_all_forms();


    /* disable selection */
    no_selection = 1;

    /* always come up with Generic */
    fl_set_folder_bynumber(fd_attrib->attrib_folder, 1);

    if (fd_attrib->attrib->y < 55)
	fd_attrib->attrib->y = 25;

    fl_show_form(fd_attrib->attrib, FL_PLACE_GEOMETRY, FL_FULLBORDER, "Attributes");
    fl_set_app_mainform(fd_attrib->attrib);

    /* Both cancel and readyobj should have their own callbacks, so we don't
       need this do_form stuff, but since attribute editing can't be invoked
       for more than once item at a time, need to block the proces_xevent.
       TODO */

    do
    {
	XEvent xev;
	retobj = fl_do_forms();
	if (retobj == FL_EVENT)
	{
	    fl_XNextEvent(&xev);
	}
    }
    while ((retobj != fd_attrib->readyobj || !validate_attributes()) &&
	   retobj != fd_attrib->cancelobj);

    fl_hide_form(fd_attrib->attrib);
    fl_set_app_mainform(fd_control->control);
    fl_activate_all_forms();

    no_selection = 0;

    if (retobj == fd_attrib->cancelobj)
    {
	restore_object(obj);
	/* change_selected_objects(obj); */
	redraw_the_form(0);
	return FALSE;
    }
    else
    {
	/* Read back form. Caller will do change_selected_objects */
	readback_attributes(obj);
	return TRUE;
    }
}

/* Sets the attributes of an object */
void
set_attribs(FL_OBJECT * obj, int boxtype, int col1, int col2,
	    int lcol, int align, float lsize, int lstyle, char label[])
{
    obj->boxtype = boxtype;
    obj->col1 = col1;
    obj->col2 = col2;
    obj->lcol = lcol;
    obj->align = align;
    obj->lsize = lsize;
    obj->lstyle = lstyle;
    fl_set_object_label(obj, label);
}

/* more attributes */
void
set_miscattribs(FL_OBJECT * obj, unsigned nw, unsigned se, unsigned re)
{
    obj->nwgravity = nw;
    obj->segravity = se;
    obj->resize = re;
}

#define NL	10

#define STRLEN  1024


/* Sets the label, turning \n in NL */
void
set_label(FL_OBJECT * obj, const char *str)
{
    int i = 0, j = 0;
    char tmpstr[STRLEN];	/* Limit on label length introduced here */

    tmpstr[0] = '\0';

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
		tmpstr[j++] = *fl_ul_magic_char;
	    i += 3;

	}
	else
	    tmpstr[j++] = str[i];
    }
    while (str[i++] != 0);
    fl_set_object_label(obj, tmpstr);
}

void
set_shortcut(FL_OBJECT * obj, const char *sc)
{
    if (obj->type != FL_RETURN_BUTTON &&
	obj->type != FL_HIDDEN_RET_BUTTON)
	fl_set_object_shortcut(obj, sc, 1);
}

/* if \ preceeds c, \ does not need quote */
#define Ok(c) (c=='"'||c=='\\'||c=='t'||c=='n'||isdigit(c))

/* decide if label need quote \ */
static int
need_quote(const char *s, int i)
{
    int c = s[i], p, n;

    p = i ? s[i - 1] : 0;	/* prev char */
    n = *s ? s[i + 1] : 0;	/* next char */

    if (c == '"' && p != '\\')
	return 1;
    else if (c == '\\' && p != '\\')
	return (!isdigit(n) && !Ok(n));
    else
	return 0;
}

/* Read the label, turning NL into \n */
char *
get_label(const FL_OBJECT * obj, int c_source)
{
    int i = 0, j = 0;
    const char *label = obj->label;
    int len = strlen(label);
    static char tmpstr[STRLEN];

    for (i = 0; i < len; i++)
    {
	if (label[i] == NL)
	{
	    tmpstr[j++] = '\\';
	    tmpstr[j++] = 'n';
	}
	else if (label[i] == *fl_ul_magic_char)
	{
	    if (!obj->shortcut || !*obj->shortcut)
	    {
		tmpstr[j++] = '\\';
		tmpstr[j++] = '0';
		tmpstr[j++] = '1';
		tmpstr[j++] = '0';
	    }
	}
	else if (c_source && need_quote(label, i))
	{
	    tmpstr[j++] = '\\';
	    tmpstr[j++] = label[i];
	}
	else
	    tmpstr[j++] = label[i];
    }
    tmpstr[j] = '\0';

    return tmpstr;
}

/* convert shortcut into string representation.
 * ESC -> ^[, F1 -> &1 etc.
 */
static int
special_key(int key, char *outbuf)
{
    char *start = outbuf;
    if (key >= XK_F1 && key <= XK_F30)
    {
	int p = (key - XK_F1 + 1) / 10, q = (key - XK_F1 + 1) % 10;
	*outbuf++ = '&';
	if (p)
	    *outbuf++ = '0' + p;
	*outbuf++ = '0' + q;
    }
    else if (IsUp(key))
    {
	*outbuf++ = '&';
	*outbuf++ = 'A';
    }
    else if (IsDown(key))
    {
	*outbuf++ = '&';
	*outbuf++ = 'B';
    }
    else if (IsRight(key))
    {
	*outbuf++ = '&';
	*outbuf++ = 'C';
    }
    else if (IsLeft(key))
    {
	*outbuf++ = '&';
	*outbuf++ = 'D';
    }
    else
    {
	*outbuf++ = key;
    }

    *outbuf = '\0';
    return (outbuf - start);
}

char *
get_shortcut_string(const FL_OBJECT * obj)
{
    static char tmps[127];
    char *p = tmps;
    long *sc = obj->shortcut;
    int n;

    for (*p = '\0'; sc && *sc; sc++)
    {
	if (*sc >= FL_ALT_VAL)
	{
	    *p++ = '#';
	    n = special_key((*sc - FL_ALT_VAL), p);
	    p += n;
	}
	else if (*sc == '#' || *sc == '&' || *sc == '^')	/* prefixed
								   w/ ^ */
	{
	    *p++ = '^';
	    *p++ = *sc;
	}
	else if (*sc < 30)
	{
	    *p++ = '^';
	    if (*sc <= 'Z')
		*p++ = 'A' + *sc - 1;
	    else if (*sc == 27)	/* Escape */
		*p++ = '[';
	}
	else if (*sc > 255)
	{
	    n = special_key(*sc, p);
	    p += n;
	}
	else
	    *p++ = *sc;
    }
    *p = '\0';
    return tmps;
}

/* Makes a copy of the object. If not exact, shortcut
 * and object name are not copied.
 */
FL_OBJECT *
copy_object(FL_OBJECT * obj, int exact)
{
    char name[MAX_VAR_LEN], cbname[MAX_VAR_LEN], argname[MAX_VAR_LEN];
    FL_OBJECT *obj2;

    obj2 = add_an_object(obj->objclass, obj->type, obj->x, obj->y,
			 obj->w, obj->h);
    get_object_name(obj, name, cbname, argname);
    set_object_name(obj2, exact ? name : "", cbname, argname);

    set_attribs(obj2, obj->boxtype, obj->col1, obj->col2,
		obj->lcol, obj->align, obj->lsize, obj->lstyle, obj->label);

    set_miscattribs(obj2, obj->nwgravity, obj->segravity, obj->resize);

    /* also copy the object specific info */
    copy_superspec(obj2, obj);

    fl_delete_object(obj2);

    return obj2;
}

#if 0

static void
copy_class_spec(FL_OBJECT * src, FL_OBJECT * dest)
{
    if (src->objclass == dest->objclass && src->spec_size == dest->spec_size)
	memcpy(dest->spec, src->spec, src->spec_size);
}

#endif

/* Changes the type of an object by reconstructing it. A quite nasty
   procedure that delves into the form structure in a bad way. */
void
change_type(FL_OBJECT * obj, int type)
{
    FL_OBJECT *ttt, *defobj;
    int boxtype, reshow = 0, is_focus;
    FL_FORM *form = obj->form;
    FL_OBJECT *prev;

    if (obj->type == type)
	return;

    /* Create the new object. */
    ttt = add_an_object(obj->objclass, type, obj->x, obj->y, obj->w, obj->h);

    /* Remove it from the form */
    fl_delete_object(ttt);

    /* create a default object from which we obtain info on if the user has
       changed boxtype and other attributes. This is done primarily to get
       around the type change problem with types having different default
       boxtype. */

    defobj = find_class_default(obj->objclass, obj->type);

    if (defobj->boxtype != obj->boxtype)
	boxtype = obj->boxtype;
    else
    {
	boxtype = ttt->boxtype;
	reshow = 1;
    }

    /* don't need to free the defobj as it is managed by find_class_default */

    /* Set the attributes */
    set_attribs(ttt, boxtype, obj->col1, obj->col2,
		obj->lcol, obj->align, obj->lsize, obj->lstyle, obj->label);

    set_miscattribs(ttt, obj->nwgravity, obj->segravity, obj->resize);

    obj->type = ttt->type;
    prev = obj->prev;
    is_focus = obj->focus;

    if (!ttt->child)
    {
	ttt->form = form;
	if (obj->child)
	{
	    clear_selection();
	    obj->focus = 0;
	    fl_delete_object(obj);
	    ttt->prev = prev;
	    ttt->next = prev ? prev->next : 0;
	    if (prev)
		prev->next = ttt;
	    *obj = *ttt;
	    fl_handle_object(obj, FL_ATTRIB, 0, 0, 0, 0);
	    addto_selection(obj);
	}
	else
	{
	    ttt->prev = obj->prev;
	    ttt->next = obj->next;
	}
    }
    else
    {
	clear_selection();
	fl_delete_object(obj);
	fl_change_composite_parent(ttt, obj);
	ttt->parent = obj;
	ttt->form = form;
	*obj = *ttt;
	prev->form = form;
	fl_insert_composite_after(obj, prev);
	fl_handle_object(obj, FL_ATTRIB, 0, 0, 0, 0);
	addto_selection(obj);
    }


    /* Correct the object focus if required. */
    if (is_focus)
	fl_set_object_focus(obj->form, ttt);

    if (ttt->child)
	redraw_the_form(0);
    else
    {
	copy_superspec(ttt, obj);
	*obj = *ttt;
    }

    if (reshow)
	show_attributes(obj);
}


static void
accept_spec(FL_OBJECT * ob, long data)
{
    long tmp;

    spec_to_superspec(curobj);
    fl_set_folder_bynumber(fd_attrib->attrib_folder, 1);
    tmp = fd_attrib->attrib_folder->argument;
    fd_attrib->attrib_folder->argument = -1;
    fl_call_object_callback(fd_attrib->attrib_folder);
    fd_attrib->attrib_folder->argument = tmp;
}

static void
spec_apply_cb(FL_OBJECT * ob, long data)
{
    redraw_the_form(0);
}

/*
 * switch between GENERIC and SPEC
 */

void
folder_switch_cb(FL_OBJECT * ob, long data)
{
    FD_attrib *ui = ob->form->fdui;

    int active = fl_get_active_folder_number(ui->attrib_folder);

    if (active == 1)
    {
	if (data != -1)		/* -1 indicates manual call */
	    fl_call_object_callback(fd_attrib->readyobj);
	fd_attrib->readyobj->type = FL_RETURN_BUTTON;	/* yuk! */
	fl_set_object_shortcut(fd_attrib->readyobj, "^M", 0);
	fl_redraw_object(fd_attrib->readyobj);
	fl_set_object_callback(fd_attrib->readyobj, 0, 0);
	fl_set_object_callback(fd_attrib->applyobj, apply_cb, 0);
	fl_set_object_callback(fd_attrib->restoreobj, restore_cb, 0);
	cleanup_spec(curobj);
    }
    else
    {
	fd_attrib->readyobj->type = FL_NORMAL_BUTTON;	/* yuk! */
	fl_set_object_shortcut(fd_attrib->readyobj, "", 0);
	fl_redraw_object(fd_attrib->readyobj);
	fl_redraw_object(fd_attrib->readyobj);
	fl_set_object_callback(fd_attrib->readyobj, accept_spec, 0);
	fl_set_object_callback(fd_attrib->applyobj, spec_apply_cb, 0);
	set_objclass_spec_attributes(curobj, 0);
    }
}
