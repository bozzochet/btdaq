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


/* Demo showing object gravity, form minsize etc. (also double click). 
 *
 * With some more work, this could be made into a useful program ...
 *
 */

#ifndef __VMS
#  define WWW_BROWSER "netscape"
#else
#  define WWW_BROWSER "netscape_dir:netscape"
#endif

#include "forms.h"
#include <stdlib.h>

/* Header file generated with fdesign. */

typedef struct {
	FL_FORM *www;
	void *vdata;
	long ldata;
	FL_OBJECT *listbr;
	FL_OBJECT *cat;
	FL_OBJECT *file;
	FL_OBJECT *shown;
} FD_www;

extern FD_www * create_form_www(void);

/* Application program begins */
#define MAX_SITES 30
typedef struct
{
    char *cat;
    char *sites[MAX_SITES][2];
} WWW;

/* real program should have these on disks */
static WWW www_sites[] =
{
    {
      "XForms App.",
      { 
	  {
	      "@b@C4XForms Home Page",
	      "http://world.std.com/~xforms"
	  },
	  {
	      "UnixCockpit - File Manager",
	      "ftp://ftp.uni-wuppertal.de/pub/unix/cockpit"
	  },
	  {
	      "xfmail - mailer ",
	      "http://Burka.NetVision.net.il/xfmail/xfmail.html"
	  },
	  {
	      "XRPlay - RPlay audio",
	      "ftp://ftp.sdsu.edu/pub/rplay"
	  },
	  {
	      "Lyx - LaTeX Frontend",
	      "http://www.lehigh.edu/~dlj0/LyriX.html"
	  },
	  {
	      "XWatch - Sys Monitor",
	      "http://www.icce.rug.nl/docs/programs/xwatch/xwatch.html"
	  },
	  {
	      "xamixer - Sound mixer",
	      "ftp://sunsite.unc.edu/pub/Linux/apps/sound/mixers"
	  },
	  {
	      "pyxforms - A Python binding to xforms",
	      "ftp://ultra7.unl.edu.ar/pub/pyxforms"
	  },
	  {
	      "xirc - An IRC client with GUI",
	      "http://www.bitgate.com/xirc"
	  },
	  {
	      "xdata - An appointment manager",
	      "ftp://jurassic.upr.clu.edu/pub/xforms"
	  },
	  {
	      "xfmixer - Audio mixer",
	      "http://linux.ms.mff.cuni.cz/~rdou3187/xfmix/xfmix.html"
	  },
	  {
	      "CaXmix - Audio mixer",
	      "http://synergy.caltech.edu/~john/programs.html"
	  },
	  {
	      NULL,
	      NULL,
	  }
      }
    },

    {
	"X related",
	{
	    {
		"@b@C4XForms Home Page",
		"http://bragg.phys.uwm.edu/xforms"
	    },
	    {
		"X Journal",
		"http://landru.unx.com/SIGS/TXJ/docs/txjhome.html"
	    },
	    {
		"X Archive Site",
		"ftp://ftp.x.org"
	    },
	    {
		"OpenGL and X",
		"http://www.sgi.com/Technology/openGL/opengl.html",
	    },
	    {
		NULL,
		NULL
	    }
	}
    },



    /* useful FAQS */
    {
	"All FAQs",
	{
	    { 
		"Usenet FAQs",
		"http://www.cis.ohio-state.edu/hypertext/faq/usenet"
	    },
	    {
		NULL,
		NULL
	    }

	}
    },

    /* Sports related www sites */
    {
	"Sports",
	{
	    {
		"SatchelSports",
		"http://www.starwave.com/SatchelSports.html"
	    },
	    {
		"Game Recap",
		"http://www.starwave.com/SatchelSports.html"
	    },
	    {
		"Patricia's NBA Daily News",
		"http://www.primenet.com/~terra/news.html"
	    },
	    {
		"Houston Rockets",
		"http://hyrax.med.uth.tmc.edu/misc/rockets.html"
	    },
	    { 
		NULL,
		NULL
	    }
	}
    },

    /* money */
    {
	"Financial",
	{
	    {
		"@f Holt Report",
		"ftp://ftp.netcom.com/pub/ge/geoholt/holt_rpt.htm"
	    },
	    {
		NULL,
		NULL,
	    }
	}
    },

    {
       "Travel",
       {
	   {
	       "New York",
	       "http://www.panix.com/clay/nyc.html"
	   },
	   {
	       NULL,
	       NULL
	   }
       }
    },

    /* things that do not belong to any of the above */
    {
	"Misc",
	{
	    {
		"Star Trek: Voyager",
		"http://voyager.paramount.com"
	    },
	    {
		"Linux Journal",
		"http://www.ssc.com/lj/mags.html"
	    },
	    {
		NULL,
		NULL
	    }
	}
    },

    /* sentinel */

    {0}
};


FD_www *ui;
static int show_sites;

/* initialize and show how many categories we have got */
void 
init_cat(void)
{
    WWW *www = www_sites;

    for (; www->cat; www++)
	fl_addto_choice(ui->cat, www->cat);
}

/* initialize file menu */
void 
init_file_menu(void)
{
    fl_fit_object_label(ui->file,1,0);
    fl_addto_menu(ui->file, "Save|Load|Quit");
}

/* initialize sites given category cat */
void 
init_sites(int cat)
{
    WWW *www = www_sites + cat - 1;
    int i;

    if (cat <= 0)
	return;

    /* cat changed. Update sites */
    fl_freeze_form(ui->www);
    fl_clear_browser(ui->listbr);
    for (i = 0; www->sites[i][0]; i++)
	fl_addto_browser(ui->listbr, www->sites[i][show_sites]);
    fl_unfreeze_form(ui->www);
}

/* new catogrey is selected */
void 
change_cat(FL_OBJECT * ob, long data)
{
    init_sites(fl_get_choice(ob));
}

/* we can show either the subject a site carries or the site name*/
void init_shown(void)
{
    fl_addto_choice(ui->shown, "Subject|Sites");
}

/* request a change of what is shown, site name or subject */ 
void 
change_shown(FL_OBJECT * ob, long data)
{
    int oldshow = show_sites;
    if ((show_sites = fl_get_choice(ob) == 2) != oldshow)
	init_sites(fl_get_choice(ui->cat));
}

/* file menu is selected. Currently we do nothing except exit */ 
void 
do_file_menu(FL_OBJECT * ob, long data)
{
      if(fl_get_menu(ob)== 3)
      {
        fl_end_all_command();
        exit(1);
      }
      else
        fl_show_alert("Sorry","Not implemented yet","", 0);
}

/* give category, cat, and the line no. within the browser, get sitename*/
char *
get_site(int cat, int thesite)
{
    return www_sites[cat - 1].sites[thesite - 1][1];
}

/* either double-clicked or connect button is pressed */
void 
connect_cb(FL_OBJECT * ob, long data)
{
    char cmdbuf[1024];
    int i;

    if ((i = fl_get_browser(ui->listbr)) > 0)
    {
	fl_set_cursor(ui->www->window, XC_watch);
	XFlush(fl_get_display());
	snprintf(cmdbuf, 
		 sizeof( cmdbuf), 
		 "%s %s",
		 WWW_BROWSER,
		 get_site(fl_get_choice(ui->cat), i));
	if(fl_exe_command(cmdbuf,0) < 0)
	  fprintf(stderr,"Error running %s\n",cmdbuf);
	fl_reset_cursor(ui->www->window);
    }
}

void 
change_sites(FL_OBJECT * ob, long data)
{
   /* no-op */
}

int 
main(int argc, char *argv[])
{

    fl_initialize(&argc, argv, "FormDemo", 0, 0);

    ui = create_form_www();

    /* some basic resizing policy */
    fl_set_form_minsize(ui->www, 320, 330);
    init_file_menu();
    init_cat();
    init_sites(1);
    init_shown();
    ui->listbr->u_vdata = (void*)100;

    fl_set_browser_dblclick_callback(ui->listbr, connect_cb, 0);

    fl_show_form(ui->www, FL_PLACE_CENTER|FL_FREE_SIZE, 
                          FL_FULLBORDER, "www launcher");
    fl_do_forms();
    fl_end_all_command();
    fl_finish();
    return 0;
}

FD_www *create_form_www(void)
{
  FL_OBJECT *obj;
  FD_www *fdui = (FD_www *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(-3);
  fdui->www = fl_bgn_form(FL_NO_BOX, 371, 331);
  obj = fl_add_box(FL_UP_BOX,0,0,371,331,"");
  obj = fl_add_frame(FL_UP_FRAME,3,2,365,25,"");
    fl_set_object_gravity(obj, NorthGravity, NorthGravity);
  fdui->listbr = obj = fl_add_browser(FL_HOLD_BROWSER,20,90,250,220,"");
    fl_set_object_lalign(obj,FL_ALIGN_BOTTOM|FL_ALIGN_INSIDE);
    fl_set_object_gravity(obj, NorthWestGravity, SouthEastGravity);
    fl_set_object_callback(obj,change_sites,0);
  fdui->cat = obj = fl_add_choice(FL_NORMAL_CHOICE,20,50,112,25,"");
    fl_set_object_gravity(obj, NorthGravity, NorthGravity);
    fl_set_object_callback(obj,change_cat,0);
  fdui->file = obj = fl_add_menu(FL_PULLDOWN_MENU,15,5,42,20,"File");
    fl_set_object_shortcut(obj,"Ff#f",1);
    fl_set_object_boxtype(obj,FL_FLAT_BOX);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, NorthWestGravity, NorthWestGravity);
    fl_set_object_callback(obj,do_file_menu,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,280,280,75,30,"Done");
    fl_set_button_shortcut(obj,"#Dd]",1);
    fl_set_object_gravity(obj, SouthEastGravity, SouthEastGravity);
  obj = fl_add_button(FL_NORMAL_BUTTON,280,90,75,30,"Connect");
    fl_set_button_shortcut(obj,"C#cc",1);
    fl_set_object_gravity(obj, NorthEastGravity, NorthEastGravity);
    fl_set_object_callback(obj,connect_cb,0);
  fdui->shown = obj = fl_add_choice(FL_NORMAL_CHOICE,156,50,112,25,"");
    fl_set_object_gravity(obj, NorthGravity, NorthGravity);
    fl_set_object_callback(obj,change_shown,0);
  fl_end_form();
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

