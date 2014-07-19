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
 * Demo showing the use of FREE Object with color leakage
 *
 */

#include "forms.h"
#include <stdlib.h>
/* Header file generated with fdesign. */

typedef struct {
	FL_FORM *drawfree;
	void *vdata;
	char *cdata;
	long ldata;
	FL_OBJECT *freeobj;
	FL_OBJECT *colorobj;
	FL_OBJECT *hsli;
	FL_OBJECT *wsli;
	FL_OBJECT *rsli;
	FL_OBJECT *gsli;
	FL_OBJECT *bsli;
	FL_OBJECT *drobj[3];
} FD_drawfree;

extern FD_drawfree * create_form_drawfree(void);
static FD_drawfree * drawui;
extern void draw_initialize(FD_drawfree *);

static int max_w = 150, max_h = 150;
static Display *dpy;

int main(int argc, char *argv[])
{
    dpy = fl_initialize(&argc, argv, "FormDemo", 0, 0);
    drawui = create_form_drawfree();
    fl_set_color_leak(1);
    draw_initialize(drawui);
    fl_show_form(drawui->drawfree, FL_PLACE_CENTER, FL_TRANSIENT, "FreeObject");
    fl_do_forms();
    return 0;
}

/* Structure mantainace */

typedef void (*DrawFunc) (int, int, int, int, int, unsigned long);

void draw_triangle(int fill, int x, int y, int w, int h, unsigned long col)
{
     XPoint xpoint[5];
     GC gc = fl_state[fl_vmode].gc[0];
     Window win = fl_winget();

     xpoint[0].x = x;         xpoint[0].y = y + h - 1;
     xpoint[1].x = x + w/2;   xpoint[1].y = y;
     xpoint[2].x = x + w - 1; xpoint[2].y = y + h - 1;
     XSetForeground(dpy, gc, fl_get_flcolor(col));
     if(fill)
       XFillPolygon (dpy, win, gc, xpoint, 3, Nonconvex, Unsorted);
     else
     {
         xpoint[3].x = xpoint[0].x; xpoint[3].y = xpoint[0].y;
         XDrawLines(dpy, win, gc, xpoint, 4, CoordModeOrigin);
     }
}  


static DrawFunc drawfunc[] =
{
    fl_oval, fl_rectangle, draw_triangle
};

typedef struct
{
    DrawFunc drawit;
    int x, y, w, h, fill, c[3];
    int newfig;
} DrawFigure;

static DrawFigure saved_figure[800], *cur_fig;

void draw_initialize(FD_drawfree *ui)
{
    cur_fig = saved_figure;
    cur_fig->c[0] = cur_fig->c[1] = cur_fig->c[2] = 127, 
    cur_fig->w = cur_fig->h = 30;
    cur_fig->drawit = fl_oval;
    cur_fig->fill = 1;

    fl_mapcolor(FL_FREE_COL1, cur_fig->c[0], cur_fig->c[1], cur_fig->c[2]);

    fl_set_slider_bounds(ui->wsli, 1, max_w);
    fl_set_slider_bounds(ui->hsli, 1, max_h);
    fl_set_slider_precision(ui->wsli, 0);
    fl_set_slider_precision(ui->hsli, 0);
    fl_set_slider_value(ui->wsli, cur_fig->w);
    fl_set_slider_value(ui->hsli, cur_fig->h);

    /* intial drawing function */
    fl_set_button(ui->drobj[0], 1);

    /* setup the color slider so we can find out colorobject from
       the callback funtions. This is not necessary as drawui
       is static, this is done to show how to access other objects
       from an object callback function */

    ui->rsli->u_vdata = ui;
    ui->gsli->u_vdata = ui;
    ui->bsli->u_vdata = ui;
}


void switch_object(FL_OBJECT * ob, long which)
{
    cur_fig->drawit = drawfunc[which];
}

void change_color(FL_OBJECT * ob, long which)
{
    cur_fig->c[which] = (int)(fl_get_slider_value(ob) * 255);
    fl_mapcolor(FL_FREE_COL1, cur_fig->c[0], cur_fig->c[1], cur_fig->c[2]);
    fl_redraw_object(((FD_drawfree *)ob->u_vdata)->colorobj);
}

void fill_cb(FL_OBJECT * ob, long notused)
{
    cur_fig->fill = !fl_get_button(ob);
}

void change_size(FL_OBJECT * ob, long which)
{
    if (which == 0)
	cur_fig->w = (int)fl_get_slider_value(ob);
    else
	cur_fig->h = (int)fl_get_slider_value(ob);
}

void refresh_cb(FL_OBJECT * ob, long which)
{
    fl_redraw_object(drawui->freeobj);
}

void clear_cb(FL_OBJECT * ob, long notused)
{
    saved_figure[0] = *cur_fig;
    cur_fig = saved_figure;
    fl_redraw_object(drawui->freeobj);
}

/*  The routine that does drawing */
int freeobject_handler(FL_OBJECT * ob, int event, FL_Coord mx, FL_Coord my,
		   int key, void *xev)
{
    DrawFigure *dr;

    switch (event)
    {
    case FL_DRAW:
        if (cur_fig->newfig == 1)
	    cur_fig->drawit(cur_fig->fill, 
	                    cur_fig->x + ob->x,
	                    cur_fig->y + ob->y, 
	                    cur_fig->w, cur_fig->h, FL_FREE_COL1); 
        else
	{
           fl_drw_box(ob->boxtype, ob->x, ob->y, ob->w, ob->h, ob->col1,
                      FL_BOUND_WIDTH);

           for (dr = saved_figure; dr < cur_fig; dr++)
	   {
	      fl_mapcolor(FL_FREE_COL1, dr->c[0], dr->c[1], dr->c[2]);
	      dr->drawit(dr->fill, dr->x + ob->x,
	                            dr->y + ob->y, 
	                            dr->w, dr->h, FL_FREE_COL1);
	   }
	}
	cur_fig->newfig = 0;
	break;
    case FL_PUSH:
	if (key != 2)
	{
	    cur_fig->x = mx - cur_fig->w/2;
	    cur_fig->y = my - cur_fig->h/2;

            /* convert position to relative to the free object */
	    cur_fig->x -= ob->x;
	    cur_fig->y -= ob->y;

	    cur_fig->newfig = 1;
	    fl_redraw_object(ob);
	    *(cur_fig+1) = *cur_fig;
	    cur_fig++;
	    cur_fig->newfig = 0;
	}
	break;
    }
    return 0;
}

FD_drawfree *create_form_drawfree(void)
{
  FL_OBJECT *obj;
  FD_drawfree *fdui = (FD_drawfree *) fl_calloc(1, sizeof(FD_drawfree));

  fdui->drawfree = fl_bgn_form(FL_NO_BOX, 530, 490);
  obj = fl_add_box(FL_UP_BOX,0,0,530,490,"");
  obj = fl_add_frame(FL_DOWN_FRAME,145,55,370,405,"");
  fdui->freeobj = obj = fl_add_free(FL_NORMAL_FREE,
                        145, 55, 370, 405, "", freeobject_handler);
/*    fl_set_object_dblbuffer(obj, 1); */
  fl_set_object_boxtype(obj, FL_FLAT_BOX); 
  fdui->colorobj = obj = fl_add_box(FL_BORDER_BOX,25,325,90,25,"");
    fl_set_object_color(obj, FL_FREE_COL1, FL_COL1), 
  fdui->hsli = obj = fl_add_valslider(FL_HOR_SLIDER,15,55,120,25,"Height");
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_callback(obj,change_size,1);
  obj = fl_add_button(FL_NORMAL_BUTTON,395,15,105,30,"Quit");
    fl_set_object_shortcut(obj,"Qq#q",1);
  obj = fl_add_checkbutton(FL_PUSH_BUTTON,15,430,100,35,"Outline");
    fl_set_object_color(obj,FL_MCOL,FL_BLUE);
    fl_set_object_callback(obj,fill_cb,0);
  fdui->wsli = obj = fl_add_valslider(FL_HOR_SLIDER,15,95,120,25,"Width");
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_callback(obj,change_size,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,280,15,105,30,"Refresh");
    fl_set_object_callback(obj,refresh_cb,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,165,15,105,30,"Clear");
    fl_set_object_callback(obj,clear_cb,0);

  fl_bgn_group();
  fdui->drobj[0] = obj = fl_add_button(FL_RADIO_BUTTON,10,390,40,40,"@#circle");
    fl_set_object_lcol(obj,FL_YELLOW);
    fl_set_object_callback(obj,switch_object,0);
  fdui->drobj[1] = obj = fl_add_button(FL_RADIO_BUTTON,50,390,40,40,"@#square");
    fl_set_object_lcol(obj,FL_YELLOW);
    fl_set_object_callback(obj,switch_object,1);
  fdui->drobj[2] = obj = fl_add_button(FL_RADIO_BUTTON,90,390,40,40,"@#8>");
    fl_set_object_lcol(obj,FL_YELLOW);
    fl_set_object_callback(obj,switch_object,2);
  fl_end_group();

  fdui->rsli = obj = fl_add_slider(FL_VERT_FILL_SLIDER,25,195,30,125,"");
    fl_set_object_color(obj,FL_COL1,FL_RED);
    fl_set_object_callback(obj,change_color,0);
  fdui->gsli = obj = fl_add_slider(FL_VERT_FILL_SLIDER,55,195,30,125,"");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_callback(obj,change_color,1);
  fdui->bsli = obj = fl_add_slider(FL_VERT_FILL_SLIDER,85,195,30,125,"");
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_callback(obj,change_color,2);
  fl_end_form();

  return fdui;
}

