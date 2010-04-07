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


#include "forms.h"
#include "viewforms.h"

/*** callbacks and freeobj handles for form main ***/
void text_timeout_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void file_mnu_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void options_mnu_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void but_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void help_mnu_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void movie_slider_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}



/*** callbacks and freeobj handles for form help ***/
void help_nav_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void text_timeout_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void but_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}



/*** callbacks and freeobj handles for form fontselect ***/
void but_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void font_choice_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void size_choice_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}



/*** callbacks and freeobj handles for form about ***/
void but_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}



/*** callbacks and freeobj handles for form brucc_opt ***/
void update_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void but_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void label_dist_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void freq_dist_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void anim_time_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}



/*** callbacks and freeobj handles for form snapshot ***/
void x_snap_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void y_snap_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void r_snap_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void z_snap_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void but_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}



/*** callbacks and freeobj handles for form log ***/
void but_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}



/*** callbacks and freeobj handles for form save ***/
void but_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void x_size_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void y_size_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}



/*** callbacks and freeobj handles for form save_as_file ***/
void but_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void Void(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}



/*** callbacks and freeobj handles for form save_as ***/
void but_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void Void(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}



/*** callbacks and freeobj handles for form color_opt ***/
void col_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void but_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}



/*** callbacks and freeobj handles for form interp ***/
void but_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void interp_freq_cb(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}



int main(int argc, char *argv[])
{
   FD_main *fd_main;
   FD_help *fd_help;
   FD_fontselect *fd_fontselect;
   FD_about *fd_about;
   FD_brucc_opt *fd_brucc_opt;
   FD_snapshot *fd_snapshot;
   FD_log *fd_log;
   FD_save *fd_save;
   FD_save_as_file *fd_save_as_file;
   FD_save_as *fd_save_as;
   FD_color_opt *fd_color_opt;
   FD_interp *fd_interp;

   fl_initialize(&argc, argv, 0, 0, 0);
   fd_main = create_form_main();
   fd_help = create_form_help();
   fd_fontselect = create_form_fontselect();
   fd_about = create_form_about();
   fd_brucc_opt = create_form_brucc_opt();
   fd_snapshot = create_form_snapshot();
   fd_log = create_form_log();
   fd_save = create_form_save();
   fd_save_as_file = create_form_save_as_file();
   fd_save_as = create_form_save_as();
   fd_color_opt = create_form_color_opt();
   fd_interp = create_form_interp();

   /* fill-in form initialization code */

   /* show the first form */
   fl_show_form(fd_main->main,FL_PLACE_CENTERFREE,FL_FULLBORDER,"main");
   fl_do_forms();
   return 0;
}
