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
#include "twheel_gui.h"

/* callbacks and freeobj handles for form twheelform */
void valchange_cb(FL_OBJECT *ob, long data)
{
     FD_twheelform *fdui = (FD_twheelform *)ob->form->fdui;
     char buf[128];

     sprintf(buf,"%.3f",fl_get_thumbwheel_value(ob));
     fl_set_object_label(fdui->report,buf);
}

void returnchange_cb(FL_OBJECT *ob, long data)
{
     FD_twheelform *fdui = (FD_twheelform *)ob->form->fdui;
     int n = fl_get_choice(ob)-1;

     fl_set_thumbwheel_return(fdui->vert, n);
     fl_set_thumbwheel_return(fdui->hor, n);
}


int main(int argc, char *argv[])
{
   FD_twheelform *fd_twheelform;

   fl_initialize(&argc, argv, 0, 0, 0);
   fd_twheelform = create_form_twheelform();

   /* fill-in form initialization code */

   /* show the first form */
   fl_show_form(fd_twheelform->twheelform,
                FL_PLACE_CENTERFREE,FL_FULLBORDER,"twheelform");

   fl_do_forms();
   return 0;
}

#include "twheel_gui.c"
