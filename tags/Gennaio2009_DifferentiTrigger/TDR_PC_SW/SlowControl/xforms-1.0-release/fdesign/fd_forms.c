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
 * $Id: fd_forms.c,v 1.0 2002/05/08 05:27:39 zhao Release $
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 * This file is part of the Forms Designer.
 *
 * It contains the routines that maintain the collection of
 * forms on which the program is working. It contains the callback
 * routines to add forms, change their name, remove them ,etc.
 * It also contains the routine to draw them and the  basic routines
 * for loading and saving forms.
 *
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "forms.h"
#include "fd_main.h"

static FRM forms[MAXFRM];	/* The forms */
int fnumb = 0;

FL_FORM *cur_form = NULL;	/* The current form */

/* Returns the number of the form */
static int
get_form_numb(FL_FORM * form)
{
    int i;
    for (i = 0; i < fnumb; i++)
	if (forms[i].form == form)
	    return i;
    return -1;
}

/* Sets the current form to numb, to NULL when numb = -1. */
static void
set_form(int numb)
{
    if (numb == -1)
    {
	cur_form = NULL;
	fl_deselect_browser(fd_control->formbrowser);
    }
    else
    {
	cur_form = forms[numb].form;
	set_bounding_box(0, 0, cur_form->w, cur_form->h);
	fl_select_browser_line(fd_control->formbrowser, numb + 1);
	fl_winstepunit(main_window, 1, 1);
	fl_winresize(main_window, cur_form->w, cur_form->h);
	fl_winstepunit(main_window,
	      (int) (get_step_size() + 0.1), (int) (get_step_size() + 0.1));
	if (fl_display)
	    XSync(fl_display, 0);
    }
    clear_selection();
    fillin_groups();
    redraw_the_form(1);
}


/* change current form background. Called when the main window is resized */
void
reshape_form_background(FL_Coord neww, FL_Coord newh)
{
    if (cur_form && cur_form->first)
    {
	cur_form->w = cur_form->first->next->w = neww;
	cur_form->h = cur_form->first->next->h = newh;
	set_bounding_box(0, 0, neww, newh);
    }
}


/****
  CALLBACK ROUTINES
****/

/* Callback routine that is called when the user selects another form
   to work on. */
void
form_cb(FL_OBJECT * obj, long arg)
{
    set_form(fl_get_browser(fd_control->formbrowser) - 1);
}

/* Callback routine called when the user adds a form. */
void
addform_cb(FL_OBJECT * obj, long arg)
{
    float w = 0, h = 0;
    FL_Coord xx, yy;
    static int form_seq;
    const char *s;

    if (fnumb == MAXFRM)
    {
	Bark(0, "Reached Maximum Number of forms:%d", MAXFRM);
	return;
    }

    /* Make old form invisible */
    set_form(-1);

    /* Get boundary */
    fl_deactivate_form(fd_control->control);

    fl_get_win_size(main_window, &xx, &yy);
    set_bounding_box(0, 0, (float) xx, (float) yy);

    w = xx;
    h = yy;

    if (!(s = fl_show_input("Give form name:", "")))
    {
	fl_activate_form(fd_control->control);
	return;
    }

    /* Create the form */
    cur_form = forms[fnumb].form = fl_bgn_form(FL_NO_BOX, w, h);
    fl_end_form();
    add_an_object(FL_BOX, FL_UP_BOX, 0, 0, w, h);
    fl_set_form_dblbuffer(cur_form, 1);

    /* Get form name and add it */
    strcpy(forms[fnumb].fname, s);

    if (!forms[fnumb].fname[0])
	sprintf(forms[fnumb].fname, "form%d", form_seq++);
    fl_add_browser_line(fd_control->formbrowser, forms[fnumb].fname);

    /* finish off */
    set_form(fnumb++);
    fl_activate_form(fd_control->control);
    changed = 1;
}

/* Callback routine called when the user wants to change the name */
void
changename_cb(FL_OBJECT * obj, long arg)
{
    int fn = get_form_numb(cur_form);
    const char *s;

    if (cur_form == NULL || fn == -1)
	return;

    if (!(s = fl_show_input("Give form name:", forms[fn].fname)) || !*s)
	return;

    strcpy(forms[fn].fname, s);

    fl_replace_browser_line(fd_control->formbrowser, fn + 1, forms[fn].fname);
    changed = 1;
}

/* Callback routine called to delete a form */
void
deleteform_cb(FL_OBJECT * obj, long arg)
{
    int i, fn = get_form_numb(cur_form);

    if (cur_form == NULL || fn == -1)
	return;
    if (!fl_show_question("Delete current form?", 1))
	return;
    fl_delete_browser_line(fd_control->formbrowser, fn + 1);
    for (i = fn; i < fnumb - 1; i++)
	forms[i] = forms[i + 1];
    fnumb--;
    set_form(-1);
    changed = 1;
}

/****
  DRAWING FORMS
****/

/* Redraws the form in main window. back indicates whether the background
   should be redrawn (when not doublebuffering). This avoids flashing. */
void
redraw_the_form(int back)
{
    if (main_window == 0)
	return;

    fl_winset(main_window);

    /* it is possible to have a null cur_form such as adding */
    if (back)
    {
	if (!cur_form)
	    fd_clear(0, 0, winw + 1, winh + 1);
    }

    if (cur_form != NULL)
    {
	cur_form->window = main_window;
	cur_form->visible = 1;
	fl_set_form_dblbuffer(cur_form, 1);
	fl_redraw_form(cur_form);
	cur_form->window = 0;
	cur_form->visible = 0;
	draw_selbox();
    }
}

/****
  LOADING AND SAVING
****/

int fd_magic;

char *
append_fd_suffix(const char *ff)
{
    static char fname[512];
    int i;

    strcpy(fname, ff);
    i = strlen(fname) - 1;
    if (i < 3 || fname[i] != 'd' || fname[i - 1] != 'f' || fname[i - 2] != '.')
	strcat(fname, ".fd");
    return fname;
}

/* loads or merges a file with form definitions */
int
load_forms(int merge, char *str, int record)
{
    int i, saved_unit = fdopt.unit, ok, nforms;
    FILE *fn;
    char fname[256], buf[256], ubuf[32];
    const char *ff, *fname_nopath = 0;

    fl_use_fselector(LOAD_FSELECTOR);

    /* Get the filename */
    if (str != NULL && str[0] != '\0')
	ff = str;
    else if (merge)
	ff = fl_show_fselector("Filename to merge forms from", "", "*.fd", "");
    else
    {
	ff = fl_show_fselector("Filename to load forms from", "", "*.fd", "");
	fname_nopath = fl_get_filename();
    }

    if (ff == NULL || ff[0] == '\0')
	return -1;

    /* Append .fd if required. */
    strcpy(fname, append_fd_suffix(ff));

    /* Open the file for reading */
    if (!(fn = fopen(fname, "r")))
    {
	if (!fdopt.conv_only)
	    fl_show_alert("No such file", fname ? fname : "", "", 0);
	else
	    M_err("LoadForm", "can't open %s", fname);
	return -1;
    }

    /* Read in the definitions */
    if (!merge)
    {
	fnumb = 0;
	fl_clear_browser(fd_control->formbrowser);
    }

    fscanf(fn, "Magic: %d\n\n", &fd_magic);

    if (fd_magic != MAGIC2 && fd_magic != MAGIC3 && fd_magic != FD_V1)
    {
	if (!fdopt.conv_only)
	    fl_show_alert("Wrong type of file!!", "", "", 1);
	else
	    M_err("LoadForm", "wrong type of file ID=%d", fd_magic);
	return -1;
    }

    fscanf(fn, "Internal Form Definition File\n");
    fscanf(fn, "    (do not change)\n\n");
    fgets(buf, sizeof(buf) - 1, fn);
    sscanf(buf, "Number of forms: %d", &nforms);
    /* fscanf(fn, "Number of forms: %d ", &nforms); */

    if (nforms >= MAXFRM || nforms <= 0)
    {
	M_err("LoadForms", "Input file %s can't be loaded", fname);
	return -1;
    }

    /* from here until we hit seperator newline, we are free to do whatever
       we want here */

    while (fgets(buf, sizeof(buf) - 1, fn) && buf[0] != '\n')
    {
	if (strncmp(buf, "Unit", 4) == 0)
	{
	    sscanf(buf, "Unit of measure: %s", ubuf);
	    fdopt.unit = unit_val(ubuf);
	    fl_cntl.coordUnit = fdopt.unit;	/* make_obj uses this */
	    M_warn("Load", "unit=%s %d", ubuf, fdopt.unit);
	}
	else if (strncmp(buf, "Border", 6) == 0)
	{
	    int bw;
	    sscanf(buf, "Border Width: %s", ubuf);
	    if ((bw = atoi(ubuf)) != FL_BOUND_WIDTH)
		fl_set_border_width(fd_bwidth = bw);
	    M_warn("Load", "BW=%d", fd_bwidth);
	}
	else if (strncmp(buf, "Snap", 4) == 0)
	{
	    sscanf(buf, "SnapGrid: %s", ubuf);
	    set_snap_size(atoi(ubuf), 1);
	}
	else
	{
	    M_warn("Load", "skipped %s", buf);
	}
    }

    for (ok = 1, i = 0; i < nforms && ok; i++)
    {
	if ((ok = read_form(fn, forms[fnumb].fname) >= 0))
	{
	    forms[fnumb].form = cur_form;
	    fl_add_browser_line(fd_control->formbrowser, forms[fnumb].fname);
	    fnumb++;
	}
    }

    if (!ok)
    {
	if (!fdopt.conv_only)
	    fl_show_alert("Not all forms could be loaded!!", "", "", 1);
	else
	    M_err(0, "not all forms could be loaded");
    }

    if (ok && !merge && !feof(fn))
    {
	fscanf(fn, "\n==============================\n");
	fgets(main_name, MAX_VAR_LEN, fn);
	main_name[strlen(main_name) - 1] = '\0';
    }

    set_form(fnumb > 0 ? 0 : -1);

    if (merge)
	changed = 1;
    else if (record && fname_nopath)
    {
	if (loadedfile)
	    fl_free(loadedfile);
	loadedfile = fl_strdup(fname_nopath);
    }
    fclose(fn);

    if (!merge)
    {
	if (loadedfile_fullpath)
	    fl_free(loadedfile_fullpath);
	loadedfile_fullpath = fl_strdup(fname);
    }

    /* reset active coordinate system to pixel */
    fl_cntl.coordUnit = FL_COORD_PIXEL;

    /* force output the same as input when converting directly. The reason is
       we don't know the screen DPI. */
    if (!fdopt.conv_only)
	fdopt.unit = saved_unit;

    fd_magic = 0;
    return 0;
}


/* saves the form definitions, retunrs whether saved */
int
save_forms(char *str)
{
    int i, snap;
    FILE *fn;
    char fname[1024], filename[1024], *ff;
    Conv *conv;

    fl_use_fselector(SAVE_FSELECTOR);

    /* Get the filename */
    if (str != NULL && str[0] != '\0')
	ff = str;
    else
	ff = (char *) fl_show_fselector("Filename to save forms to", "",
					"*.fd", loadedfile);

    if (!ff)
	return 0;		/* cancel */

    if (ff[0] == '\0')
    {
	fl_show_alert("No forms were saved.", "", "", 0);
	return 0;
    }

    /* Remove .fd if required */
    strcpy(filename, ff);
    i = strlen(filename) - 1;
    if (filename[i] == 'd' && filename[i - 1] == 'f' && filename[i - 2] == '.')
	filename[i - 2] = '\0';

    strcpy(fname, filename);

    /* if conversion only, no need to (re)save  the .fd file. It messes up
       Makefile rules */
    if (fdopt.conv_only)
	goto emit_code;

    /* Make the .fd file. */
    strcat(fname, ".fd");
    make_backup(fname);

    if ((fn = fopen(fname, "w")) == 0)
    {
	fl_show_alert("Cannot create definition file!!", "", "", 1);
	return 0;
    }

    snap = (get_step_size() + 0.1);
    fprintf(fn, "Magic: %d\n\n", snap == 10 ? FD_V1 : FD_V1);
    fprintf(fn, "Internal Form Definition File\n");
    fprintf(fn, "    (do not change)\n\n");
    fprintf(fn, "Number of forms: %d\n", fnumb);

    fprintf(fn, "Unit of measure: %s\n", unit_name(fdopt.unit));

    if (fd_bwidth != FL_BOUND_WIDTH && fd_bwidth)
	fprintf(fn, "Border Width: %d\n", fd_bwidth);

    if (snap != 10)
	fprintf(fn, "SnapGrid: %d\n", snap);

    for (i = 0; i < fnumb; i++)
	write_form(fn, forms[i].form, forms[i].fname);

    fprintf(fn, "\n==============================\n%s\n", main_name);
    fclose(fn);

  emit_code:

    /* if no code is desired, return */
    if (!fdopt.emit_code)
	return 1;

    conv = convertor + fdopt.language;

    if ((ff = strrchr(fname, '.')))
	*ff = '\0';

    /* some converter works on the c code */
    if (conv->need_c_code)
	convertor[FD_C].convert(fname, forms, fnumb);

    if (conv->convert)
	return conv->convert(fname, forms, fnumb);
    else if (conv->extern_convertor)
    {
	char cmdbuf[1024];
	char optbuf[512];
	int status;

	optbuf[0] = '\0';

	if (fdopt.emit_main)
	    strcat(optbuf, "-main ");
	if (fdopt.emit_cb)
	    strcat(optbuf, "-callback ");
	if (fdopt.altformat)
	    strcat(optbuf, "-altformat ");
	if (fdopt.compensate)
	    strcat(optbuf, "-compensate ");

	sprintf(cmdbuf, "%s %s%s", conv->extern_convertor, optbuf, fname);
	M_warn("Convert", "Executing %s", cmdbuf);

	if (fdopt.conv_only)
	{
	    if ((status = system(cmdbuf)))
		M_err("Output", "Error executing %s\n", cmdbuf);
	}
	else
	{
	    fl_clear_command_log();
	    if ((status = fl_exe_command(cmdbuf, 1)))
	    {
		fl_addto_command_log("\nerror executing ");
		fl_addto_command_log(cmdbuf);
		fl_show_command_log(FL_FULLBORDER);
	    }
	}

	return status == 0;

    }
    else
    {
	fprintf(stderr, "Convertor %s for %s not found\n",
		conv->extern_convertor, conv->lang_name);
	return 0;
    }

    return 1;
}
