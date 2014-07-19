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
 * $Id: sp_button.c,v 1.0 2002/05/08 05:27:39 zhao Release $
 *
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 * Settting button class specific attributes.
 *
 */

#include "forms.h"
#include "fd_main.h"
#include "fd_spec.h"
#include <ctype.h>
#include "spec/button_spec.h"

extern FD_buttonattrib *create_form_buttonattrib(void);

static FD_buttonattrib *bt_attrib;
static SuperSPEC *button_spec;
static IconInfo *info;
static void show_spec(SuperSPEC *);
static void get_data_name(FL_OBJECT * ob, IconInfo *);
static FL_OBJECT *edited;

void *
get_button_spec_fdform(void)
{
    if (!bt_attrib)
    {
	bt_attrib = create_form_buttonattrib();
	fl_addto_choice(bt_attrib->pixalign, align_name(FL_ALIGN_CENTER));
	fl_addto_choice(bt_attrib->pixalign, align_name(FL_ALIGN_TOP));
	fl_addto_choice(bt_attrib->pixalign, align_name(FL_ALIGN_BOTTOM));
	fl_addto_choice(bt_attrib->pixalign, align_name(FL_ALIGN_LEFT));
	fl_addto_choice(bt_attrib->pixalign, align_name(FL_ALIGN_RIGHT));
	fl_set_input_return(bt_attrib->filename, FL_RETURN_END);
	fl_set_input_return(bt_attrib->focus_filename, FL_RETURN_END);
	fl_set_input_return(bt_attrib->tooltip, FL_RETURN_END);
    }

    return bt_attrib;
}

void
button_spec_restore(FL_OBJECT * ob, long data)
{
    FL_OBJECT *bedited = bt_attrib->vdata;

    superspec_to_spec(bedited);
    show_spec(get_superspec(bedited));
    redraw_the_form(0);
}

static void
show_spec(SuperSPEC * spec)
{
    info = spec->cspecv;

    fl_set_button(bt_attrib->initialval, spec->int_val);
    fl_set_button(bt_attrib->use_data, spec->use_data);
    fl_set_button(bt_attrib->fullpath, spec->fullpath);

    fl_set_button(bt_attrib->showfocus, info->show_focus);
    fl_set_choice_text(bt_attrib->pixalign, align_name(info->align));

    fl_set_input(bt_attrib->filename, info->filename);
    fl_set_input(bt_attrib->focus_filename, info->focus_filename);
    fl_set_input(bt_attrib->tooltip, info->helper);
}

#define IsIconButton(cls) (cls == FL_BITMAPBUTTON || cls==FL_PIXMAPBUTTON)

int
set_button_attrib(FL_OBJECT * ob)
{

    if (!IsIconButton(ob->objclass) &&
	ob->type != FL_PUSH_BUTTON && ob->type != FL_RADIO_BUTTON)
	return -1;

    bt_attrib->vdata = edited = ob;

    button_spec = get_superspec(ob);
    info = button_spec->cspecv;

    if (!info)
    {
	M_err("ButtonAttrib", "internal error");
	return -1;
    }


    if (ob->type == FL_PUSH_BUTTON || ob->type == FL_RADIO_BUTTON)
	fl_show_object(bt_attrib->initialval);
    else
	fl_hide_object(bt_attrib->initialval);

    if (ob->objclass == FL_PIXMAPBUTTON || ob->objclass == FL_BITMAPBUTTON)
    {
	fl_show_object(bt_attrib->filename);
	fl_show_object(bt_attrib->browse);
	fl_show_object(bt_attrib->use_data);
	fl_show_object(bt_attrib->fullpath);
	fl_show_object(bt_attrib->tooltip);
	(ob->objclass == FL_PIXMAPBUTTON ?
	 fl_show_object : fl_hide_object) (bt_attrib->focus_filename);
	(ob->objclass == FL_PIXMAPBUTTON ?
	 fl_show_object : fl_hide_object) (bt_attrib->browse2);
    }
    else
    {
	fl_hide_object(bt_attrib->filename);
	fl_hide_object(bt_attrib->focus_filename);
	fl_hide_object(bt_attrib->browse);
	fl_hide_object(bt_attrib->browse2);
	fl_hide_object(bt_attrib->use_data);
	fl_hide_object(bt_attrib->fullpath);
	fl_hide_object(bt_attrib->tooltip);
    }

    if (ob->objclass == FL_PIXMAPBUTTON)
    {
	fl_show_object(bt_attrib->pixalign);
	fl_show_object(bt_attrib->showfocus);
    }
    else
    {
	fl_hide_object(bt_attrib->pixalign);
	fl_hide_object(bt_attrib->showfocus);
    }

    show_spec(button_spec);
    return 0;
}

static FL_OBJECT *
create_a_button(FL_OBJECT * ob)
{
    FL_OBJECT *defobj = 0;

    /* create a default object */
    if (ob->objclass == FL_BUTTON)
	defobj = fl_create_button(ob->type, 0, 0, 0, 0, "");
    else if (ob->objclass == FL_BITMAPBUTTON)
	defobj = fl_create_bitmapbutton(ob->type, 0, 0, 0, 0, "");
    else if (ob->objclass == FL_PIXMAPBUTTON)
	defobj = fl_create_pixmapbutton(ob->type, 0, 0, 0, 0, "");
    else if (ob->objclass == FL_ROUNDBUTTON)
	defobj = fl_create_roundbutton(ob->type, 0, 0, 0, 0, "");
    else if (ob->objclass == FL_LABELBUTTON)
	defobj = fl_create_labelbutton(ob->type, 0, 0, 0, 0, "");
    else if (ob->objclass == FL_LIGHTBUTTON)
	defobj = fl_create_lightbutton(ob->type, 0, 0, 0, 0, "");
    else if (ob->objclass == FL_CHECKBUTTON)
	defobj = fl_create_checkbutton(ob->type, 0, 0, 0, 0, "");
    else if (ob->objclass == FL_ROUND3DBUTTON)
	defobj = fl_create_round3dbutton(ob->type, 0, 0, 0, 0, "");
    else
    {
	fprintf(stderr, "Unknown Button Class: %d\n", ob->objclass);
    }
    return defobj;
}

static char *
file_tail(const char *full)
{
    static char tmpbuf[512];
    char *p;

    strcpy(tmpbuf, full);
    if ((p = strrchr(tmpbuf, '/')))
	strcpy(tmpbuf, p + 1);

    return tmpbuf;
}

void
emit_button_header(FILE * fp, FL_OBJECT * ob)
{
    SuperSPEC *spec;
    char buf[512];

    if (ob->objclass != FL_PIXMAPBUTTON && ob->objclass != FL_BITMAPBUTTON)
	return;

    spec = get_superspec(ob);
    info = spec->cspecv;

    if (info->use_data && info->data[0] && info->filename[0])
    {
	if (info->fullpath)
	    sprintf(buf, "#include \"%s\"", info->filename);
	else
	    sprintf(buf, "#include \"%s\"", file_tail(info->filename));

	if (!is_duplicate_info(ob, buf))
	    fprintf(fp, "%s\n", buf);

	if (info->focus_filename[0])
	{
	    if (info->fullpath)
		sprintf(buf, "#include \"%s\"", info->focus_filename);
	    else
		sprintf(buf, "#include \"%s\"", file_tail(info->focus_filename));

	    if (!is_duplicate_info(ob, buf))
		fprintf(fp, "%s\n", buf);
	}

    }
}

void
emit_button_code(FILE * fp, FL_OBJECT * ob)
{
    SuperSPEC *btspec, *defspec;
    FL_OBJECT *defobj;
    IconInfo *definfo;

    if (!ISBUTTON(ob->objclass) || !(defobj = create_a_button(ob)))
	return;

    defspec = get_superspec(defobj);
    btspec = get_superspec(ob);


    if (btspec->int_val != defspec->int_val)
	fprintf(fp, "    fl_set_button(obj, %d);\n", btspec->int_val);

    if (!(info = btspec->cspecv))
	return;

    if (info->helper[0])
	fprintf(fp, "    fl_set_object_helper(obj, \"%s\");\n", info->helper);

    definfo = defspec->cspecv;

    if (info->filename[0] && !info->use_data)
    {
	fprintf(fp, "    fl_set_%sbutton_file(obj, \"%s\");\n",
		ob->objclass == FL_PIXMAPBUTTON ? "pixmap" : "bitmap",
		info->filename);
	if (info->focus_filename[0])
	    fprintf(fp, "    fl_set_%sbutton_focus_file(obj, \"%s\");\n",
		    ob->objclass == FL_PIXMAPBUTTON ? "pixmap" : "bitmap",
		    info->focus_filename);

    }

    if (info->align != definfo->align && ob->objclass == FL_PIXMAPBUTTON)
    {
	fprintf(fp, "    fl_set_pixmapbutton_align(obj, %s, %d, %d);\n",
	     align_name(info->align | FL_ALIGN_INSIDE), info->dx, info->dy);
    }

    if (info->data[0] && info->use_data && info->filename[0])
    {
	if (ob->objclass == FL_PIXMAPBUTTON)
	    fprintf(fp, "    fl_set_pixmapbutton_data(obj,%s);\n",
		    info->data);
	else
	    fprintf(fp, "    fl_set_bitmapbutton_data(obj,%s, %s, (unsigned char *)%s);\n",
		    info->width, info->height, info->data);
	if (info->focus_filename[0])
	    fprintf(fp, "    fl_set_pixmapbutton_focus_data(obj,%s);\n",
		    info->focus_data);

    }

    if (info->show_focus != definfo->show_focus &&
	ob->objclass == FL_PIXMAPBUTTON)
	fprintf(fp, "    fl_set_pixmapbutton_focus_outline(obj,%d);\n",
		info->show_focus);

    fl_free_object(defobj);
}


void
save_button_attrib(FILE * fp, FL_OBJECT * ob)
{
    FL_OBJECT *defobj;
    SuperSPEC *defspec, *btspec;
    IconInfo *definfo;

    if (!ISBUTTON(ob->objclass) || !(defobj = create_a_button(ob)))
	return;

    defspec = get_superspec(defobj);
    definfo = defspec->cspecv;
    btspec = get_superspec(ob);
    info = btspec->cspecv;

    if (btspec->int_val != defspec->int_val)
	fprintf(fp, "\tvalue: %d\n", btspec->int_val);

    if (!info || !definfo)
    {
	M_err("SaveButtonAttrib", "internal error");
	return;
    }

    get_data_name(ob, info);

    if (info->filename[0])
    {
	fprintf(fp, "\tfile: %s\n", info->filename);
	if (info->focus_filename[0])
	    fprintf(fp, "\tfocus_file: %s\n", info->focus_filename);
	fprintf(fp, "\tfullpath: %d\n", info->fullpath);
    }

    if (info->align != definfo->align)
	fprintf(fp, "\talign: %s\n", align_name(info->align | FL_ALIGN_INSIDE));

    if (info->show_focus != definfo->show_focus)
	fprintf(fp, "\tfocus: %d\n", info->show_focus);

    if (info->data[0] && info->filename[0])
    {
	fprintf(fp, "\tdata: %s\n", info->data);
	if (info->focus_data[0])
	    fprintf(fp, "\tfocus_data: %s\n", info->focus_data);
    }

    if (info->width[0])
	fprintf(fp, "\twidth: %s\n", info->width);
    if (info->height[0])
	fprintf(fp, "\theight: %s\n", info->height);

    if (info->helper[0])
	fprintf(fp, "\thelper: %s\n", info->helper);

    fl_free_object(defobj);
}


void
usedata_change(FL_OBJECT * ob, long data)
{
    info->use_data = fl_get_button(ob);
}

void
fullpath_cb(FL_OBJECT * ob, long data)
{
    info->fullpath = fl_get_button(ob);
}

void
initialval_change(FL_OBJECT * ob, long data)
{
    fl_set_button(edited, fl_get_button(bt_attrib->initialval));

    if (auto_apply)
	redraw_the_form(0);
}

void
showfocus_change(FL_OBJECT * ob, long data)
{
    info->show_focus = fl_get_button(ob);
    fl_set_pixmapbutton_focus_outline(edited, info->show_focus);
}

void
iconbutton_filename_change(FL_OBJECT * ob, long data)
{
    if (!IsIconButton(edited->objclass))
	return;

    strcpy(info->filename, fl_get_input(ob));

    if (info->filename[0])
    {
	(edited->objclass == FL_PIXMAPBUTTON ?
	 fl_set_pixmapbutton_file : fl_set_bitmapbutton_file)
	    (edited, info->filename);
    }
    else
    {
	/* show the default crab */
	if (edited->objclass == FL_PIXMAPBUTTON)
	    set_testing_pixmap(edited);
    }

    if (auto_apply)
	redraw_the_form(0);
}

void
focusiconbutton_filename_change(FL_OBJECT * ob, long data)
{
    if (!IsIconButton(edited->objclass))
	return;

    strcpy(info->focus_filename, fl_get_input(ob));

    if (info->filename[0])
    {
	(edited->objclass == FL_PIXMAPBUTTON ?
	 fl_set_pixmapbutton_focus_file : fl_set_bitmapbutton_file)
	    (edited, info->focus_filename);
    }
}

char *
get_helper(char *s)
{
    static char helper[256];
    char *p;

    for (p = helper; *s; s++, p++)
    {
	if (*s == '\\' && *(s + 1) == 'n')
	{
	    *p = '\n';
	    s++;
	}
	else
	    *p = *s;
    }

    return helper;
}

void
helper_change_cb(FL_OBJECT * ob, long data)
{
    strcpy(info->helper, fl_get_input(ob));
    if (info->helper[0])
	fl_set_object_helper(edited, get_helper(info->helper));
}

void
pixalign_change(FL_OBJECT * ob, long data)
{
    const char *s = fl_get_choice_text(ob);

    if (!s)
	return;

    info->align = align_val(s);
    /* don't allow outside align */
    fl_set_pixmap_align(edited, info->align | FL_ALIGN_INSIDE,
			info->dx, info->dy);
    if (auto_apply)
	redraw_the_form(0);
}

static int
loadfile(const char *file, void *data)
{
    char buf[512];
    char *cwd = fl_getcwd(buf, sizeof(buf) - 2);
    const char *s = strstr(file, cwd);
    FL_OBJECT *ob;

    if (!s)
	s = file;
    else
	s = file + strlen(cwd) + 1;

    ob = (data && *(long *) data) ? bt_attrib->focus_filename : bt_attrib->filename;

    fl_set_input(ob, s);
    fl_call_object_callback(ob);
    return 1;
}


void
lookfor_pixmapfile_cb(FL_OBJECT * ob, long data)
{
    fl_use_fselector(XPM_FSELECTOR);
    fl_set_fselector_placement(FL_PLACE_MOUSE);
    fl_set_fselector_callback(loadfile, &data);
    fl_show_fselector("XPM file", "",
	       edited->objclass == FL_PIXMAPBUTTON ? "*.xpm" : "*.xbm", "");
}

/* read the specified xpm/xbm filename, and return the data name
 * and size
 */
static void
get_xpm_stuff(char *inbuf, FILE * fp)
{
    char buf[256], *p;
    int done = 0;

    if (!fp)
    {
	M_warn("GetXPMStuff", "fp==0");
	return;
    }

    while (fgets(buf, sizeof(buf) - 1, fp) && !done)
    {
	if ((p = strstr(buf, "static char")))
	{
	    char *q = inbuf;

	    *p += 11;
	    while (*p && *++p != '*')
		;
	    while (*p && *++p != '[')	/* ] */
	    {
		if (!isspace( ( int ) *p))
		    *q++ = *p;
	    }
	    *q = '\0';
	}
    }
}

static void
get_xbm_stuff(IconInfo * in, FILE * fp)
{
    char buf[512], *p;

    strcpy(buf, in->filename);

    if ((p = strrchr(buf, '/')))
	strcpy(buf, ++p);

    if ((p = strrchr(buf, '.')))
	*p = '\0';

    strcat(strcpy(in->width, buf), "_width");
    strcat(strcpy(in->height, buf), "_height");
    strcat(strcpy(in->data, buf), "_bits");
}

static void
get_data_name(FL_OBJECT * ob, IconInfo * inf)
{
    FILE *fp = 0, *focus_fp = 0;

    if (ob->objclass != FL_BITMAPBUTTON && ob->objclass != FL_PIXMAPBUTTON)
    {
	inf->filename[0] = '\0';
	inf->focus_filename[0] = '\0';
	inf->width[0] = '\0';
	inf->height[0] = '\0';
	return;
    }

    if (!inf->use_data)
    {
	inf->data[0] = inf->width[0] = inf->height[0] = '\0';
	return;
    }

    if (inf->filename[0] && !(fp = fopen(inf->filename, "r")))
    {
	fprintf(stderr, "can't open %s\n", inf->filename);
	/* wipe the icon file only if there isn't anything we can do */
	if (!inf->use_data || inf->data[0] == '\0')
	    inf->filename[0] = '\0';
    }

    if (inf->focus_filename[0] && !(focus_fp = fopen(inf->focus_filename, "r")))
    {
	fprintf(stderr, "can't open focusfile %s\n", inf->focus_filename);
	if (!inf->use_data || inf->focus_data[0] == '\0')
	    inf->focus_filename[0] = '\0';
    }

    if (ob->objclass == FL_PIXMAPBUTTON)
    {
	get_xpm_stuff(inf->data, fp);
	get_xpm_stuff(inf->focus_data, focus_fp);
    }
    else
    {
	get_xbm_stuff(inf, fp);
    }

    if (fp)
	fclose(fp);
    if (focus_fp)
	fclose(focus_fp);
}

#include "spec/button_spec.c"
