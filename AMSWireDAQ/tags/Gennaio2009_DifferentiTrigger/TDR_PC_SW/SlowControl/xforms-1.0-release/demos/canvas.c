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
 * Demo showing the interaction with a canvas object.   
 *
 *  This file is part of xforms package
 *  T.C. Zhao and M. Overmars  (1997)
 *
 */

#include "forms.h"
#include <stdlib.h>

extern void sensitive_setting(FL_OBJECT *, long);
extern void misc_cb(FL_OBJECT *, long);

/**** Forms and Objects ****/

typedef struct {
	FL_FORM *canvasform;
	void *vdata;
	char *cdata;
	long ldata;
	FL_OBJECT *canvas;
	FL_OBJECT *br;
	FL_OBJECT *keyboard;
	FL_OBJECT *mouse;
	FL_OBJECT *done;
	FL_OBJECT *misc;
	FL_OBJECT *menu;
} FD_canvasform;

extern FD_canvasform * create_form_canvasform(void);


static GC canvasGC;

int 
canvas_expose(FL_OBJECT *ob, Window win, int w, int h, XEvent *ev, void *d)
{
    FD_canvasform *ui = (FD_canvasform *)d;
    XFillRectangle(fl_get_display(), win, canvasGC, 0, 0, w, h);
    fl_addto_browser(ui->br, "Expose");
    return 0;
}

int 
canvas_key(FL_OBJECT *ob, Window win, int w, int h, XEvent *ev, void *d)
{
    FD_canvasform *ui = (FD_canvasform *)d;
    char buf[128];
    sprintf(buf, "KeyPress: keysym=%ld", 
           XKeycodeToKeysym(fl_display,ev->xkey.keycode, 0));
    fl_addto_browser(ui->br, buf);
    return 0;
}

int 
canvas_but(FL_OBJECT *ob, Window win, int w, int h, XEvent *ev, void *d)
{
    FD_canvasform *ui = (FD_canvasform *)d;
    char buf[128];
    sprintf(buf, "Button%s: %d", ev->type==ButtonPress?"Press":"Release",
            ev->xbutton.button);
    fl_addto_browser(ui->br, buf);
    return 0;
}

int 
canvas_misc(FL_OBJECT *ob, Window win, int w, int h, XEvent *ev, void *d)
{
    FD_canvasform *ui = (FD_canvasform *)d;
    fl_addto_browser(ui->br, 
         ev->xcrossing.type == EnterNotify ? "Enter canvas":"Leave canvas");
    return 0;
}


void init_canvas(FD_canvasform *fdui)
{
    fl_add_canvas_handler(fdui->canvas, Expose, canvas_expose, fdui);
    fl_add_canvas_handler(fdui->canvas, KeyPress, canvas_key, fdui);
    fl_add_canvas_handler(fdui->canvas, ButtonPress,canvas_but,fdui);
    fl_add_canvas_handler(fdui->canvas, ButtonRelease,canvas_but,fdui);
    fl_set_button(fdui->mouse, 1);
    fl_set_button(fdui->keyboard, 1);
    canvasGC = XCreateGC(fl_get_display(),fl_state[fl_vmode].trailblazer,0,0);
    XSetForeground(fl_get_display(), canvasGC, fl_get_flcolor(FL_BLACK));
}

FD_canvasform *fd_canvasform;

/* callbacks */
void sensitive_setting(FL_OBJECT *ob, long event)
{
    FL_HANDLE_CANVAS hc = (event == KeyPress) ? canvas_key:canvas_but;

    if(fl_get_button(ob))
       fl_add_canvas_handler(fd_canvasform->canvas, event, hc, fd_canvasform);
    else
       fl_remove_canvas_handler(fd_canvasform->canvas, event, hc);
}

void disable_it(FL_OBJECT *ob, long data)
{
    (fl_get_button(ob) ? fl_deactivate_object:fl_activate_object)
    (fd_canvasform->canvas);
}

void hide_it(FL_OBJECT *ob, long all)
{
      if(all)
      {
         fl_hide_form(fd_canvasform->canvasform);
         fl_show_form(fd_canvasform->canvasform,
                      FL_PLACE_CENTER, FL_TRANSIENT, "canvas");
#if 0
         fl_remove_selected_xevent(fd_canvasform->canvasform->window,
            OwnerGrabButtonMask);
#endif
      }
      else
      {
         if(fd_canvasform->canvas->visible)
         {
            fl_hide_object(fd_canvasform->canvas);
            fl_set_object_label(ob,"ShowCanvas");
         }
         else
         {
            fl_show_object(fd_canvasform->canvas);
            fl_set_object_label(ob,"HideCanvas");
         }
      }
}

void misc_cb(FL_OBJECT *ob, long data)
{ 
    if(fl_get_button(ob))
    {
       fl_add_canvas_handler(fd_canvasform->canvas, EnterNotify, 
                             canvas_misc, fd_canvasform);
       fl_add_canvas_handler(fd_canvasform->canvas, LeaveNotify,
                             canvas_misc, fd_canvasform);
    }
    else
    {
       fl_remove_canvas_handler(fd_canvasform->canvas, 
                                EnterNotify, canvas_misc);
       fl_remove_canvas_handler(fd_canvasform->canvas, 
                                LeaveNotify, canvas_misc);
    }
}

int main(int argc, char *argv[])
{

   fl_set_border_width(-2);

   fl_initialize(&argc, argv, "FormDemo", 0, 0);
   fd_canvasform = create_form_canvasform();

   /* fill-in form initialization code */

   init_canvas(fd_canvasform);


   fl_addto_menu(fd_canvasform->menu,"Item1|Item2|Item3|Item4");

   fl_show_form(fd_canvasform->canvasform,
                FL_PLACE_FREE,FL_FULLBORDER,"canvasform");
#if 0
         fl_remove_selected_xevent(fd_canvasform->canvasform->window,
            OwnerGrabButtonMask);
#endif

   while (fl_do_forms() != fd_canvasform->done) 
      ;

   return 0;
}

/*******************/
FD_canvasform *create_form_canvasform(void)
{
  FL_OBJECT *obj;
  FD_canvasform *fdui = (FD_canvasform *) fl_calloc(1, sizeof(*fdui));

  fdui->canvasform = fl_bgn_form(FL_NO_BOX, 450, 280);
  obj = fl_add_box(FL_UP_BOX,0,0,450,280,"");
  fdui->canvas = obj = fl_add_canvas(FL_NORMAL_CANVAS,20,40,155,187,"");
  fdui->br = obj = fl_add_browser(FL_NORMAL_BROWSER,188,40,152,187,"");
  obj = fl_add_button(FL_PUSH_BUTTON,30,236,90,27,"Deactivate");
    fl_set_object_callback(obj,disable_it,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,120,236,90,27,"HideCanvas");
    fl_set_object_callback(obj, hide_it, 0);
  obj = fl_add_button(FL_NORMAL_BUTTON,210,236,90,27,"HideForm");
    fl_set_object_callback(obj, hide_it, 1);
  fdui->done = obj = fl_add_button(FL_NORMAL_BUTTON,300,236,90,27,"Done");
  obj = fl_add_text(FL_NORMAL_TEXT,130,10,120,20,"Canvas");
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  fdui->menu = obj = fl_add_menu(FL_PULLDOWN_MENU, 20,10, 45,22,"Menu");
    fl_set_object_shortcut(obj,"#m", 1);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
  fdui->keyboard = obj = fl_add_checkbutton(FL_PUSH_BUTTON,345,40,76,26,"Keyboard");
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_callback(obj,sensitive_setting,KeyPress);
  fdui->mouse = obj = fl_add_checkbutton(FL_PUSH_BUTTON,345,70,76,26,"Mouse");
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_callback(obj,sensitive_setting,ButtonPress);
  fdui->misc = obj = fl_add_checkbutton(FL_PUSH_BUTTON,345,100,74,26,"Enter/Leave");
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_callback(obj,misc_cb,ButtonPress);
  fl_end_form();

  return fdui;
}
