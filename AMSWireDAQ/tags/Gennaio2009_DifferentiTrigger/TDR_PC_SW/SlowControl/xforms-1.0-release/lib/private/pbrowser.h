/*
 *
 *  This file is part of the XForms library package.
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


#ifndef PBROWSER_H_
#define PBROWSER_H_

#ifndef FL_BROWSER_SCROLL_CALLBACKt
#define FL_BROWSER_SCROLL_CALLBACKt
typedef void (*FL_BROWSER_SCROLL_CALLBACK)(FL_OBJECT *ob, int, void *);
#endif

typedef struct
{
    FL_OBJECT *br;		/* the browser - container    */
    FL_OBJECT *tb;		/* the textbox                */
    FL_OBJECT *hsl;		/* horizontal scrollbar       */
    FL_OBJECT *vsl;		/* vertical scrollbar         */
    FL_CALLBACKPTR callback;
    long callback_data;
    float hsize, vsize;
    float hval, vval;
    float hinc1, hinc2;
    float vinc1, vinc2;
    int dead_area;
    int attrib;
    int v_on, h_on;		/* scrollbar on/off state     */
    int v_pref, h_pref;		/* on/off prefererence        */
    int vw, vw_def;
    int hh, hh_def;
    int user_set;
    FL_BROWSER_SCROLL_CALLBACK hcb, vcb;
    void *hcb_data, *vcb_data;
} FL_BROWSER_SPEC;

#endif
