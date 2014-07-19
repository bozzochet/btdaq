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
 * $Id: sp_pixmap.c,v 1.0 2002/05/08 05:27:39 zhao Release $
 *
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 * Settting pixmap/bitmap class specific attributes. Should
 * be absorbed into button class.
 *
 */

#include "forms.h"
#include "fd_main.h"
#include "fd_spec.h"
#include <ctype.h>
#include "spec/pixmap_spec.h"

extern FD_pixmapattrib *create_form_pixmapattrib(void);

static FD_pixmapattrib *px_attrib;
static SuperSPEC *pixmap_spec;
static IconInfo *info;
static void show_spec(SuperSPEC *);
static void get_data_name(FL_OBJECT * ob, IconInfo *);
static FL_OBJECT *edited;

void *
get_pixmap_spec_fdform(void)
{
    if (!px_attrib)
    {
	px_attrib = create_form_pixmapattrib();
	fl_addto_choice(px_attrib->pixalign, align_name(FL_ALIGN_CENTER));
	fl_addto_choice(px_attrib->pixalign, align_name(FL_ALIGN_TOP));
	fl_addto_choice(px_attrib->pixalign, align_name(FL_ALIGN_BOTTOM));
	fl_addto_choice(px_attrib->pixalign, align_name(FL_ALIGN_LEFT));
	fl_addto_choice(px_attrib->pixalign, align_name(FL_ALIGN_RIGHT));
	fl_set_input_return(px_attrib->filename, FL_RETURN_END);
    }

    return px_attrib;
}

void
pixmap_spec_restore(FL_OBJECT * ob, long data)
{
    FL_OBJECT *pedited = px_attrib->vdata;

    superspec_to_spec(pedited);
    show_spec(get_superspec(pedited));
    redraw_the_form(0);
}

static void
show_spec(SuperSPEC * spec)
{
    info = spec->cspecv;

    fl_set_button(px_attrib->use_data, spec->use_data);
    fl_set_button(px_attrib->fullpath, spec->fullpath);

    fl_set_choice_text(px_attrib->pixalign, align_name(info->align));

    fl_set_input(px_attrib->filename, info->filename);
}


int
set_pixmap_attrib(FL_OBJECT * ob)
{

    px_attrib->vdata = edited = ob;

    pixmap_spec = get_superspec(ob);
    info = pixmap_spec->cspecv;

    if (!info)
    {
	M_err("PixmapAttrib", "internal error");
	return -1;
    }

    (ob->objclass == FL_PIXMAP ? fl_show_object : fl_hide_object)
	(px_attrib->pixalign);


    show_spec(pixmap_spec);
    return 0;
}

static FL_OBJECT *
create_a_pixmap(FL_OBJECT * ob)
{
    FL_OBJECT *defobj = 0;

    /* create a default object */
    if (ob->objclass == FL_PIXMAP)
	defobj = fl_create_pixmap(ob->type, 0, 0, 0, 0, "");
    else if (ob->objclass == FL_BITMAP)
	defobj = fl_create_bitmapbutton(ob->type, 0, 0, 0, 0, "");
    else
    {
	fprintf(stderr, "Unknown pixmap/bitmap Class: %d\n", ob->objclass);
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
emit_pixmap_header(FILE * fp, FL_OBJECT * ob)
{
    SuperSPEC *spec;
    char buf[512];

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
    }

}

void
emit_pixmap_code(FILE * fp, FL_OBJECT * ob)
{
    SuperSPEC *btspec, *defspec;
    FL_OBJECT *defobj;
    IconInfo *definfo;

    if (!(defobj = create_a_pixmap(ob)))
	return;

    defspec = get_superspec(defobj);
    btspec = get_superspec(ob);

    if (!(info = btspec->cspecv))
	return;

    definfo = defspec->cspecv;

    if (info->filename[0] && !info->use_data)
    {
	fprintf(fp, "    fl_set_%s_file(obj, \"%s\");\n",
		ob->objclass == FL_PIXMAP ? "pixmap" : "bitmap",
		info->filename);
    }

    if (info->align != definfo->align && ob->objclass == FL_PIXMAP)
    {
	fprintf(fp, "    fl_set_pixmap_align(obj, %s, %d, %d);\n",
	     align_name(info->align | FL_ALIGN_INSIDE), info->dx, info->dy);
    }

    if (info->data[0] && info->use_data && info->filename[0])
    {
	if (ob->objclass == FL_PIXMAP)
	    fprintf(fp, "    fl_set_pixmap_data(obj,%s);\n",
		    info->data);
	else
	    fprintf(fp, "    fl_set_bitmap_data(obj,%s, %s, (unsigned char *)%s);\n",
		    info->width, info->height, info->data);
    }

    fl_free_object(defobj);
}


void
save_pixmap_attrib(FILE * fp, FL_OBJECT * ob)
{
    FL_OBJECT *defobj;
    SuperSPEC *defspec, *btspec;
    IconInfo *definfo;

    if (!(defobj = create_a_pixmap(ob)))
	return;

    defspec = get_superspec(defobj);
    definfo = defspec->cspecv;
    btspec = get_superspec(ob);
    info = btspec->cspecv;

    if (!info || !definfo)
    {
	M_err("SavePixmapAttrib", "internal error");
	return;
    }

    get_data_name(ob, info);

    if (info->filename[0])
    {
	fprintf(fp, "\tfile: %s\n", info->filename);
	fprintf(fp, "\tfullpath: %d\n", info->fullpath);
    }

    if (info->align != definfo->align)
	fprintf(fp, "\talign: %s\n", align_name(info->align | FL_ALIGN_INSIDE));

    if (info->data[0] && info->filename[0])
	fprintf(fp, "\tdata: %s\n", info->data);
    if (info->width[0])
	fprintf(fp, "\twidth: %s\n", info->width);
    if (info->height[0])
	fprintf(fp, "\theight: %s\n", info->height);

    fl_free_object(defobj);
}


void
pixmapusedata_change(FL_OBJECT * ob, long data)
{
    info->use_data = fl_get_button(ob);
}

void
pixmapfullpath_cb(FL_OBJECT * ob, long data)
{
    info->fullpath = fl_get_button(ob);
}


void
pixmap_filename_change(FL_OBJECT * ob, long data)
{
    strcpy(info->filename, fl_get_input(ob));

    if (info->filename[0])
    {
	(edited->objclass == FL_PIXMAP ?
	 fl_set_pixmap_file : fl_set_bitmap_file)
	    (edited, info->filename);
    }
    else
    {
	/* show the default crab */
	set_testing_pixmap(edited);
    }

    if (auto_apply)
	redraw_the_form(0);
}

void
pixmapalign_change(FL_OBJECT * ob, long data)
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

    if (!s)
	s = file;
    else
	s = file + strlen(cwd) + 1;

    fl_set_input(px_attrib->filename, s);
    fl_call_object_callback(px_attrib->filename);
    return 1;
}


void
pixmaplookfor_pixmapfile_cb(FL_OBJECT * ob, long data)
{

    fl_use_fselector(XPM_FSELECTOR);
    fl_set_fselector_placement(FL_PLACE_MOUSE);
    fl_set_fselector_callback(loadfile, 0);
    fl_show_fselector("XPM file", "",
		      edited->objclass == FL_PIXMAP ? "*.xpm" : "*.xbm", "");
}

/* read the specified xpm/xbm filename, and return the data name
 * and size
 */
static void
get_xpm_stuff(IconInfo * in, FILE * fp)
{
    char buf[128], *p;
    int done = 0;

    while (fgets(buf, sizeof(buf) - 1, fp) && !done)
    {
	if ((p = strstr(buf, "static char")))
	{
	    char *q = in->data;

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
    FILE *fp;

    if (ob->objclass != FL_BITMAP && ob->objclass != FL_PIXMAP)
    {
	inf->filename[0] = '\0';
	inf->width[0] = '\0';
	inf->height[0] = '\0';
	return;
    }

    if (!inf->use_data)
    {
	inf->data[0] = inf->width[0] = inf->height[0] = '\0';
	return;
    }

    if (!(fp = fopen(inf->filename, "r")) && inf->filename[0])
    {
	fprintf(stderr, "can't open %s\n", inf->filename);
	inf->filename[0] = '\0';
	return;
    }


    if (ob->objclass == FL_PIXMAP)
	get_xpm_stuff(inf, fp);
    else
	get_xbm_stuff(inf, fp);

    fclose(fp);
}

#include "spec/pixmap_spec.c"
