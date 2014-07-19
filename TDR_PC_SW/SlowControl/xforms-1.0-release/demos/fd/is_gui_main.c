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
#include "is_gui.h"

/*** callbacks and freeobj handles for form is_mainform ***/
void vscroll_callback(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void hscroll_callback(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void filemenu_callback(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void progressbar(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}



/*** callbacks and freeobj handles for form viewform ***/
void doublebuffer_callback(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}

void windowlevel_callback(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}



/*** callbacks and freeobj handles for form processform ***/
void scaling_callback(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
}



int main(int argc, char *argv[])
{
   FD_is_mainform *fd_is_mainform;
   FD_viewform *fd_viewform;
   FD_processform *fd_processform;

   fl_initialize(&argc, argv, 0, 0, 0);
   fd_is_mainform = create_form_is_mainform();
   fd_viewform = create_form_viewform();
   fd_processform = create_form_processform();

   /* fill-in form initialization code */

   /* show the first form */
   fl_show_form(fd_is_mainform->is_mainform,FL_PLACE_CENTERFREE,FL_FULLBORDER,"is_mainform");
   fl_do_forms();
   return 0;
}
