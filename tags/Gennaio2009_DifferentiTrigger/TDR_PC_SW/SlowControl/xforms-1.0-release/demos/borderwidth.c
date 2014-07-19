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
 * demo showing the effect of different border widths 
 *
 * This file is part of xforms packge
 * T.C. Zhao and M. Overmars  (1997)
 *
 */

#include "forms.h"
#include <stdlib.h>


extern void done_callback(FL_OBJECT *, long);
extern void bw_callback(FL_OBJECT *, long);

/**** Forms and Objects ****/
typedef struct {
	FL_FORM *bwform;
	void *vdata;
	char *cdata;
	long ldata;
	FL_OBJECT *done;
	FL_OBJECT *bw_choice;
	FL_OBJECT *bwgroup;
	FL_OBJECT *pmobj;
} FD_bwform;

extern FD_bwform * create_form_bwform(void);

/* callbacks for form bwform */
void done_callback(FL_OBJECT *ob, long data)
{
   fl_finish();
   exit(0);
}

FD_bwform *fd_bwform;

void bw_callback(FL_OBJECT *ob, long data)
{
  /* fill-in code for callback */
   static int bws[] = { -3,-2,-1,1,2,3,4,5};
   int bw = bws[fl_get_choice(ob)-1];

   fl_set_object_bw(fd_bwform->bwgroup, bw);
   /* since bwgroup includes the backface, it wipes out the done button*/  
   fl_redraw_object(fd_bwform->done);
   fl_redraw_object(fd_bwform->bw_choice);
}


int main(int argc, char *argv[])
{

   /* application default. Can be overriden by the command line options */
   fl_set_border_width(-2); 

   fl_initialize(&argc, argv, "FormDemo", 0, 0);
   fd_bwform = create_form_bwform();

   /* fill-in form initialization code */
   fl_set_pixmapbutton_file(fd_bwform->pmobj, "crab.xpm");

   fl_addto_choice(fd_bwform->bw_choice,"-3 Pixel|-2 Pixel|-1 Pixel");
   fl_addto_choice(fd_bwform->bw_choice," 1 Pixel| 2 Pixel| 3 Pixel");
   fl_addto_choice(fd_bwform->bw_choice," 4 Pixel| 5 Pixel");
   fl_set_choice_text(fd_bwform->bw_choice, "-2 Pixel");

   /* show the first form */
   fl_show_form(fd_bwform->bwform,FL_PLACE_CENTER,FL_TRANSIENT,"bwform");
   while (fl_do_forms())
     ;
   return 0;
}

/* Form definition file generated with fdesign. */

FD_bwform *create_form_bwform(void)
{
  FL_OBJECT *obj;
  FD_bwform *fdui = (FD_bwform *) fl_calloc(1, sizeof(FD_bwform));

  fdui->bwform = fl_bgn_form(FL_NO_BOX, 380, 340);

  fdui->bwgroup = fl_bgn_group();
  obj = fl_add_box(FL_UP_BOX,0,0,380,340,"");
  obj = fl_add_frame(FL_EMBOSSED_FRAME,220,60,135,145,"");
  obj = fl_add_frame(FL_ENGRAVED_FRAME,15,60,185,145,"");
  obj = fl_add_slider(FL_HOR_SLIDER,25,70,160,20,"");
  obj = fl_add_valslider(FL_HOR_BROWSER_SLIDER,25,105,160,20,"");
  obj = fl_add_scrollbar(FL_HOR_THIN_SCROLLBAR,25,140,160,20,"");
    fl_set_scrollbar_size(obj, 0.2);
  obj = fl_add_counter(FL_NORMAL_COUNTER,25,175,160,20,"");
  fdui->pmobj = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,305,145,40,35,"");
  obj = fl_add_positioner(FL_NORMAL_POSITIONER,30,225,100,80,"");
  obj = fl_add_button(FL_NORMAL_BUTTON,230,65,100,28,"Button");
  obj = fl_add_lightbutton(FL_PUSH_BUTTON,230,98,100,28,"LightButton");
  obj = fl_add_roundbutton(FL_PUSH_BUTTON,230,128,80,32,"Button");
  obj = fl_add_round3dbutton(FL_PUSH_BUTTON,230,152,80,32,"Button");
    fl_set_object_color(obj,FL_COL1, FL_BLUE);
  obj = fl_add_checkbutton(FL_PUSH_BUTTON,230,175,80,32,"Button");
  obj = fl_add_input(FL_NORMAL_INPUT,195,240,160,28,"Input");
  fdui->bw_choice = obj = fl_add_choice(FL_NORMAL_CHOICE2,105,20,80,28,
					"Border Width");
    fl_set_object_callback(obj,bw_callback,0);
  fl_end_group();

  fdui->done = obj = fl_add_button(FL_NORMAL_BUTTON,270,290,75,30,"Done");
    fl_set_object_callback(obj,done_callback,0);
  fl_end_form();
  fdui->bwform->fdui  = fdui;

  return fdui;
}

