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
 * popup tester
 *
 * This file is part of xforms package
 * T.C. Zhao and Mark Overmars
 */
#include "forms.h"

typedef struct {
	FL_FORM *pupform;
	void *vdata;
	char *cdata;
	long ldata;
	FL_OBJECT *menu;
	FL_OBJECT *button;
	FL_OBJECT *choice;
	FL_OBJECT *status;
	FL_OBJECT *done_cb;
} FD_pupform;

extern FD_pupform * create_form_pupform(void);
extern void menu_callback(FL_OBJECT *, long );
extern void dopup_callback(FL_OBJECT *, long );
extern void choice_callback(FL_OBJECT *, long );

static void init_menu(FL_OBJECT *);
static void init_choice(FL_OBJECT *);

FD_pupform *fd_pupform;

int main(int argc, char *argv[])
{

   fl_initialize(&argc, argv, 0, 0, 0);
   fd_pupform = create_form_pupform();

   /* fill-in form initialization code */
   init_menu(fd_pupform->menu);
   init_choice(fd_pupform->choice);

   /* show the first form */
   fl_show_form(fd_pupform->pupform,FL_PLACE_CENTER,FL_FULLBORDER,"pupform");

   fl_do_forms();

   return 0;
}

/********* MENU ***********************************************/

void menu_callback(FL_OBJECT *ob, long data)
{
    FD_pupform *fdui = (FD_pupform *)ob->form->fdui;
    char buf[128];

    sprintf(buf,"item %d (%s) selected", 
       fl_get_menu(ob), fl_get_menu_text(ob));

    fl_set_object_label(fdui->status, buf);
}

/** menu initialization entries. No callbacks for the item */
FL_PUP_ENTRY menu_entries[] = 
{
     {"Menu Item1"},
     {"Menu Item2"},
     {"_Menu Item3"},
     {"/_Menu Item 4"},
        {"Menu Item 5"},
        {"Menu Item 6"},
        {"Menu Item 7"},
        {"Menu Item 8"},
     {0},
     {"Menu Item10"},
     {"menu Item11"},
     {"menu Item12"},
     {0}
};

static void menuitem_entercb(int n, void *data)
{
      char buf[128];
      FD_pupform *fdui = (FD_pupform *)data;

      sprintf(buf,"Entered %d (%s)", n, fl_get_menu_item_text(fdui->menu, n));
      fl_set_object_label(fdui->status, buf);
}


static void
init_menu(FL_OBJECT *menu)
{
    /* if no entercb is desired, set_menu_entries is much simpler */
#if 0
     fl_set_menu_entries(menu, menu_entries);
#else
     int n = fl_newpup(0);
     fl_setpup_entries(n, menu_entries);
     fl_setpup_entercb(n, menuitem_entercb, menu->form->fdui);
     fl_setpup_bw(n, -2);
     fl_set_menu_popup(menu, n);
#endif
}

/*********** End of menu *************************/

/******* PopUP ***********************************/

static int pupID = -1;

static int pupitem_cb(int selected)
{
     char buf[128];

     sprintf(buf,"Item %d (%s) selected",
         selected, fl_getpup_text(pupID, selected));
     fl_set_object_label(fd_pupform->status, buf);
     return selected;
}

FL_PUP_ENTRY pup_entries[] = 
{
     {"Popup Item1", pupitem_cb, "1", FL_PUP_RADIO},
     {"Popup Item2", pupitem_cb, "2", FL_PUP_RADIO},
     {"Popup Item3", pupitem_cb, "3", FL_PUP_RADIO},
     {"/Popup Item 4", pupitem_cb},
        {"Popup Item 5", pupitem_cb},
        {"Popup Item 6", pupitem_cb},
        {"Popup Item 7", pupitem_cb},
        {"Popup Item 8", pupitem_cb},
     {0},
     {"Popup Item10", pupitem_cb, 0, FL_PUP_GRAY},
     {"Popup Item11", pupitem_cb},
     {0}
};

static void pup_entercb(int n, void *data)
{ 
      FD_pupform *fdui = (FD_pupform *)data;
      char buf[128];
      sprintf(buf,"Entered %d (%s)", n, pup_entries[n-1].text);
      fl_set_object_label(fdui->status, buf);
}

void 
dopup_callback(FL_OBJECT *ob, long data)
{
    if(pupID < 0)
    {
        pupID = fl_newpup(0);
        fl_setpup_entries(pupID, pup_entries);
        fl_setpup_entercb(pupID, pup_entercb, ob->form->fdui);
    }

    /* since each item has its own callback, ignore the return value */
    fl_dopup(pupID);
}

/********* End of pup *****************/ 

static void
init_choice(FL_OBJECT *ob)
{
    fl_addto_choice(ob,"Choice1|Choice2|Choice3");
    fl_addto_choice(ob,"Choice4|Choice5|Choice6");
    fl_addto_choice(ob,"Choice7|Choice8|Choice9");
}

void
choice_callback(FL_OBJECT *ob, long data)
{
    char buf[128];
    FD_pupform *fdui = (FD_pupform *)ob->form->fdui;

    sprintf(buf,"%d (%s) selected",
      fl_get_choice(ob), fl_get_choice_text(ob));
    fl_set_object_label(fdui->status, buf);
}

/* Form definition file generated with fdesign. */

#include <stdlib.h>

FD_pupform *create_form_pupform(void)
{
  FL_OBJECT *obj;
  FD_pupform *fdui = (FD_pupform *) fl_calloc(1, sizeof(*fdui));

  fdui->pupform = fl_bgn_form(FL_NO_BOX, 320, 250);
  obj = fl_add_box(FL_UP_BOX,0,0,320,250,"");
  fdui->menu = obj = fl_add_menu(FL_PULLDOWN_MENU,20,95,60,20,"Menu");
    fl_set_object_boxtype(obj,FL_FLAT_BOX);
    fl_set_object_callback(obj, menu_callback,0);
  fdui->button = obj = fl_add_button(FL_MENU_BUTTON,100,90,75,30,"Button");
    fl_set_object_callback(obj, dopup_callback, 0);
    fl_set_object_shortcut(obj, "#BB", 1);
  fdui->choice = obj = fl_add_choice(FL_NORMAL_CHOICE2,195,90,105,30,"");
    fl_set_object_callback(obj,choice_callback, 0);
  fdui->status = obj = fl_add_text(FL_NORMAL_TEXT,25,30,265,30,"");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER);
    fl_set_object_dblbuffer(obj, 1);
  fdui->done_cb = obj = fl_add_button(FL_NORMAL_BUTTON,210,200,85,30,"Done");
  fl_end_form();

  fl_adjust_form_size(fdui->pupform);
  fdui->pupform->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

