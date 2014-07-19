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


/* test screen/world conversion in addition to showing the xyplot styles */
#include "forms.h"
#include <stdlib.h>

static FL_FORM *fxyplot;
static float x[6][21], y[6][21];

static FL_OBJECT *xyplot[6];
static void create_form_xyplot(void);

typedef struct
{
   int type;
   char *name;
   FL_COLOR color;
} 
XYType;

#define VN(a,c)   {a,#a,c}

static XYType xytype[] =
{
  VN(FL_NORMAL_XYPLOT,FL_BLACK),
  VN(FL_SQUARE_XYPLOT,FL_RED),
  VN(FL_CIRCLE_XYPLOT,FL_GREEN),
  VN(FL_POINTS_XYPLOT,FL_BLUE),
  VN(FL_DASHED_XYPLOT,FL_INDIANRED),
  VN(FL_FILL_XYPLOT,FL_SLATEBLUE),
  VN(-1,0),
};

#define N (sizeof(xytype) / sizeof(xytype[0]) - 1)


static void
done_xyplot(FL_OBJECT *ob, long q)
{
    fl_hide_form(ob->form); 
    exit(0);
}

#include <math.h>

static int post(FL_OBJECT *ob, int ev, 
               FL_Coord mx, FL_Coord my, int key, void *xev)
{
    if(ev == FL_PUSH || ev == FL_MOUSE)
    {
        float wx, wy;
        char buf[64];

        fl_xyplot_s2w(ob, mx, my, &wx, &wy);
        sprintf(buf,"x=%d y=%d wx=%.1f wy=%.1f", mx, my, wx, wy);
        fl_show_oneliner(buf, ob->x + ob->form->x + 5, ob->y + ob->form->y);
/*       fl_object_ps_dump(ob,"test.ps"); */
        ob->wantkey = FL_KEY_ALL;
        ob->input = 1;
    }
    else if(ev == FL_RELEASE)
    {
        fl_hide_oneliner();
    }
    else if(ev == FL_KEYBOARD)
    {
          fprintf(stderr,"key=%d\n",key);
    }
    return 0;
}

int
main(int argc, char *argv[])
{
     int i, j;

     fl_initialize(&argc, argv, "FormDemo", 0, 0);
     create_form_xyplot();

     /* make sure double buffer also works */
     for ( i = 0; i < 3; i++)
       fl_set_object_dblbuffer(xyplot[i],0);

     for ( i = 0; i < N; i++)
     {
         for(j = 0; j < 21; j++)
         {
            x[i][j] = (float)j * 3.1415 / 10 + 0.2;
            y[i][j] = sin(2*x[i][j]) + cos(x[i][j]);
         }


         fl_set_xyplot_data(xyplot[i], x[i], y[i], 21, "TestTitle", 
                           "X-axis", "Y|axis");
         if(i==0)
            fl_add_xyplot_text(xyplot[i], x[i][15], 0.1,
                 "@2->", FL_ALIGN_TOP, FL_BLUE);
         else
            fl_add_xyplot_text(xyplot[i], x[i][8], 1.4,
                 "Text Inset", FL_ALIGN_CENTER, FL_BLUE);

         if(i == 3)
         {
            fl_set_xyplot_xgrid(xyplot[i],FL_GRID_MAJOR);
            fl_set_xyplot_xgrid(xyplot[i],FL_GRID_MINOR);
         }
         else if(i==0)
         {
             fl_set_xyplot_xtics(xyplot[i], 7, 2);
             fl_set_xyplot_xbounds(xyplot[i],6,0);
         }
         else if(i==1)
         {
             fl_set_xyplot_ytics(xyplot[i], 5, 2);
             fl_set_xyplot_ybounds(xyplot[i], 2.4,-2.4);
         }
        
         fl_set_object_posthandler(xyplot[i], post);
     }

     fl_show_form(fxyplot, FL_PLACE_ASPECT, FL_TRANSIENT, "XYplot");

     while (fl_do_forms())
        ;
     return 0;
}

static
void create_form_xyplot(void)
{
   FL_OBJECT *obj;
   XYType *xy  = xytype;
   int dx = 180, dy = 160;

   if (fxyplot)
      return;

   fxyplot = fl_bgn_form(FL_NO_BOX,570,430);
   obj = fl_add_box(FL_UP_BOX,0,0,570,430,"");

   if(xy->type != -1)
   {
      xyplot[0] = obj = fl_add_xyplot(xy->type,20,40,dx,dy,xy->name);
      fl_set_object_lsize(obj, FL_TINY_SIZE);
      fl_set_object_color(obj, FL_COL1, xy->color);
      xy++;
   }

   if(xy->type != -1)
   {
      xyplot[1] = obj = fl_add_xyplot(xy->type,200,40,dx,dy,xy->name);
      fl_set_object_lsize(obj, FL_TINY_SIZE);
      fl_set_object_color(obj, FL_COL1, xy->color);
      xy++;
   }

   if(xy->type != -1)
   {
      xyplot[2] = obj = fl_add_xyplot(xy->type,380,40,dx,dy,xy->name);
      fl_set_object_lsize(obj, FL_TINY_SIZE);
      fl_set_object_color(obj, FL_COL1, xy->color);
      xy++;
   }

   if(xy->type != -1)
   {
      xyplot[3] = obj = fl_add_xyplot(xy->type,20,210,dx,dy,xy->name);
      fl_set_object_lsize(obj, FL_TINY_SIZE);
      fl_set_object_color(obj, FL_COL1, xy->color);
      xy++;
   }

   if(xy->type != -1)
   {
      xyplot[4] = obj = fl_add_xyplot(xy->type,200,210,dx,dy,xy->name);
      fl_set_object_lsize(obj, FL_TINY_SIZE);
      fl_set_object_color(obj, FL_COL1, xy->color);
      xy++;
   }

   if(xy->type != -1)
   {
      xyplot[5] = obj = fl_add_xyplot(xy->type,380,210,dx,dy,xy->name);
      fl_set_object_lsize(obj, FL_TINY_SIZE);
      fl_set_object_color(obj, FL_COL1, xy->color);
   }

   obj = fl_add_button(FL_NORMAL_BUTTON,230,390,100,30,"Exit");
   fl_set_object_callback(obj, done_xyplot, 0);

   obj = fl_add_text(FL_NORMAL_TEXT,180,15,240,30,"FL_XYPLOT");
   fl_set_object_lcol(obj, FL_SLATEBLUE); 
   fl_set_object_lsize(obj, FL_HUGE_SIZE);
   fl_set_object_lstyle(obj, FL_BOLD_STYLE|FL_EMBOSSED_STYLE);
   fl_set_object_boxtype(obj, FL_FLAT_BOX);

   fl_end_form();
}
