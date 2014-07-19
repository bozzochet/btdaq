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


/* Demo: complete pop-ups. 
 * test font/cursor change
 * test attaching pup to menu 
 */

#include "forms.h"
#include <stdlib.h>

FL_FORM *pup;
FL_OBJECT *done, *pret, *b1, *b2, *b3, *menu;
void create_form_pup(void);
void init_menu(void);

int post(FL_OBJECT *, int, FL_Coord, FL_Coord, int, void *);

int main(int argc,char *argv[])
{
    FL_IOPT aa;
    unsigned mask = FL_PDVisual;

    aa.vclass = FL_DefaultVisual;
    fl_set_defaults(mask, &aa);

    fl_initialize(&argc, argv, "FormDemo", 0, 0);

    create_form_pup();

    /* initialize */

    fl_set_object_posthandler(b1, post);
    fl_set_object_posthandler(b2, post);
    fl_set_object_helper(b3,"button3");
/*    fl_set_object_posthandler(b3, post); */

    fl_show_form(pup, FL_PLACE_MOUSE, FL_TRANSIENT,"PupDemo");
    init_menu();

    fl_do_forms();
    return 0;
}

static int subm = -1, m = -1, ssm; 

/* post-handler */
int 
post(FL_OBJECT *ob, int ev, FL_Coord mx, FL_Coord y, int key, void *xev)
{
    static int n1 = -1,n2 = -1;

    if(n1 == -1)
    {    
      n1 = fl_defpup(FL_ObjWin(ob),"line1|line2");
      fl_setpup_shadow(n1,0);
      fl_setpup_bw(n1,0);
      fl_setpup_pad(n1,3,0);

      n2 = fl_defpup(FL_ObjWin(ob),"button1|button2");
      fl_setpup_shadow(n2,0);
      fl_setpup_bw(n2,-1);
      fl_setpup_pad(n2,3,0);
    }

    if(ev == FL_ENTER)
    {
       if(ob==b3)
         fl_show_oneliner("button3",ob->form->x+ob->x,
                         ob->form->y+ob->y + ob->h + 5);
       else
       {
          fl_setpup_position(ob->form->x+ob->x, ob->form->y+ob->y+ob->h + 5);
          fl_showpup((ob==b1) ? n1:n2);
       }
    }
    else if(ev != FL_MOTION)
    {
       if(ob==b3)
         fl_hide_oneliner();
       else
         fl_hidepup((ob==b1) ?  n1:n2);
    }

    return 0;
}


void
show_return_val(int i)
{
    char buf[128];

    if(i >= 0)
       sprintf(buf,"Returned %d(%s)",i, fl_getpup_text(m,i));
    else
       sprintf(buf,"Returned %d",i);

    fl_set_object_label(pret, buf);
}

int ssm_cb(int a)
{
   show_return_val(a);
   return a;
}

void
do_pup(FL_OBJECT *ob, long q)
{
   static int n;

   if(subm == -1)
   {
      ssm  = fl_newpup(FL_ObjWin(ob));
      subm = fl_newpup(FL_ObjWin(ob));
      m    = fl_newpup(FL_ObjWin(ob));

/*      fl_addtopup(ssm,"SubSubM%F%t",ssm_cb);*/
      fl_addtopup(ssm,"SSMItem20%x20%R1");
      fl_addtopup(ssm,"SSMItem21%x21%r1");
      fl_addtopup(ssm,"SSMItem22%x22%r1%l");
      fl_addtopup(ssm,"SSMitem30%x30%R2");
      fl_addtopup(ssm,"SSMItem31%x31%r2");
      fl_addtopup(ssm,"SSMItem32%x32%r2");

/*      fl_addtopup(subm,"SubMenu%t");*/
      fl_addtopup(subm,"SMItemA\tAlt-A%x10%h","#a"); 
      fl_addtopup(subm,"SMItemB\tAlt-B%x11%h","#b");
      fl_addtopup(subm,"SMItemC\tAlt-C%x12%h","#c");
      fl_addtopup(subm,"SMItemD\tAlt-F5%x13%h%m","#&5",ssm);
      fl_addtopup(subm,"SMItemE\tAlt-E%x14","#E");

      fl_setpup_mode(subm, 14, FL_PUP_GREY);

      fl_addtopup(m,"PopUP%t");
      fl_addtopup(m,"MenuItem1%h","1#1");

      fl_addtopup(m,"MenuItem2%h","2#2");
      fl_setpup_submenu(m, 2, subm);
      fl_addtopup(m,"MenuItem3%h","3#3");
      fl_addtopup(m,"MenuItem4%h","4#4");
   }


   if(fl_get_button_numb(ob) >= FL_SHORTCUT)
      fl_setpup_position(ob->form->x + ob->x, 
                      ob->form->y + ob->y + ob->h); 

   show_return_val(fl_dopup(m));

   /* test if changing size/style ok */
   n = !n;
   fl_setpup_fontsize(n ? 14:12);
   fl_setpup_fontstyle(n ? FL_TIMES_STYLE:FL_BOLDITALIC_STYLE);
   fl_setpup_cursor(m, n ? XC_hand2:XC_sb_right_arrow);
}

void init_menu(void)
{
    int mm,smm;

    mm = fl_newpup(fl_default_win());
    fl_setpup_bw(mm, -2);
    fl_setpup_shadow(mm, 0);
    smm = fl_newpup(0);
    fl_setpup_shadow(smm, 0);

    fl_addtopup(mm,"MenuItem1|MenuItem2%m|MenuItem3",smm);
    fl_addtopup(smm,"SubItem1%x11|SubItem2%x12|SubItem3%x13");


    /* attach pup to menu */
    fl_set_menu_popup(menu, mm);
}


void
do_menu(FL_OBJECT *ob, long data)
{
    char buf[128];

    if(fl_get_menu(ob) >= 0)
       sprintf(buf,"%d (%s)", fl_get_menu(ob), fl_get_menu_text(ob));
    else
       sprintf(buf,"%d", fl_get_menu(ob));

    fl_set_object_label(pret, buf);


}

void
done_cb(FL_OBJECT *ob, long data)
{
   exit(0);
}

void create_form_pup(void)
{
  FL_OBJECT *obj;

  if (pup)
     return;
  pup = fl_bgn_form(FL_UP_BOX,260,210);
  done = obj = fl_add_button(FL_NORMAL_BUTTON,150,150,90,35,"Done");
    fl_set_object_callback(obj,done_cb, 0);
  obj = fl_add_button(FL_MENU_BUTTON,30,90,100,30,"PopUp");
  fl_set_button_shortcut(obj,"Pp#p",1);
  fl_set_object_callback(obj, do_pup, 0);
  menu = obj = fl_add_menu(FL_PULLDOWN_MENU,160,95,60,25,"Menu");
  fl_set_object_callback(obj, do_menu, 0);
  pret = obj = fl_add_text(FL_NORMAL_TEXT,20,60,220,30,"");
    fl_set_object_lalign(obj,FL_ALIGN_CENTER);
  b1 = fl_add_button(FL_NORMAL_BUTTON, 20, 10, 60, 30,"Button1");
  b2 = fl_add_button(FL_NORMAL_BUTTON, 90, 10, 60, 30,"Button2");
  b3 = fl_add_button(FL_NORMAL_BUTTON, 160, 10, 60, 30,"Button3");
  fl_end_form();
}
