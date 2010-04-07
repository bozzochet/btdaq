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
 * scrollbar functionality check-out
 */
#include "forms.h"
#include <stdlib.h>
#include "scrollbar_gui.h"   /* from fd/ directory */


void hide_cb(FL_OBJECT *ob, long data)
{
    FD_scb *fdui = (FD_scb *)ob->form->fdui;

    if(fdui->hor_thin->visible)
    {
        fl_set_object_label(fdui->hide,"Show");
        fl_hide_object(fdui->hor_thin);
    }
    else
    {
        fl_set_object_label(fdui->hide,"Hide");
        fl_show_object(fdui->hor_thin);
    }

}

void deactivate_cb(FL_OBJECT *ob, long data)
{
    FD_scb *fdui = (FD_scb *)ob->form->fdui;

    if(fdui->hor_thin->active == 1)
    {
        fl_set_object_label(fdui->deactivate,"Activate");
        fl_deactivate_object(fdui->hor_thin);
    }
    else
    {
        fl_set_object_label(fdui->deactivate,"Deactivate");
        fl_activate_object(fdui->hor_thin);
    }
}

void done_cb(FL_OBJECT *ob, long data)
{
    exit(0);
}

void noop_cb(FL_OBJECT *ob, long data)
{
}

int main(int argc, char *argv[])
{
   FD_scb *fd_scb;

   fl_set_border_width(-2); 
   fl_initialize(&argc, argv, 0, 0, 0);
   fd_scb = create_form_scb();

   /* show the first form */
   fl_show_form(fd_scb->scb,FL_PLACE_CENTERFREE,FL_FULLBORDER,"form0");
   fl_do_forms();
   return 0;
}

#include "scrollbar_gui.c"
