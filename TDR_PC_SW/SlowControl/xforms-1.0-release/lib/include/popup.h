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

/********************** crop here for forms.h **********************/

/*
 * $Id: popup.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *  Prototypes for pop-up menus
 */

#ifndef FL_POPUP_H
#define FL_POPUP_H		/* { */


#define FL_MAXPUPI     128	/* max item each pup        */
#define FL_PUP_PADH    4	/* space between each items */

typedef void (*FL_PUP_ENTERCB) (int, void *);	/* call back prototype  */
typedef void (*FL_PUP_LEAVECB) (int, void *);	/* call back prototype  */

FL_EXPORT int fl_setpup_entries(
		int nm,
		FL_PUP_ENTRY *entries
		);

FL_EXPORT int fl_newpup(
		Window win
		);

FL_EXPORT int fl_defpup(
		Window win,
		const char *str,
		...
		);

FL_EXPORT int fl_addtopup(
		int n,
		const char *str,
		...
		);

FL_EXPORT int fl_setpup_mode(
		int nm,
		int ni,
		unsigned int mode
		);

FL_EXPORT void fl_freepup(
		int n
		);

FL_EXPORT int fl_dopup(
		int n
		);


FL_EXPORT Cursor fl_setpup_default_cursor(
		int cursor
		);


/* setpup_{colors|fontsize} etc will be removed in next revision
 * use fl_set_default_xxx instead
 */
FL_EXPORT void fl_setpup_color(
		FL_COLOR fg,
		FL_COLOR bg
		);

FL_EXPORT void fl_setpup_checkcolor(
		FL_COLOR col
		);

FL_EXPORT int fl_setpup_fontsize(
		int size
		);

FL_EXPORT int fl_setpup_fontstyle(
		int style
		);


#define fl_setpup_default_fontsize    fl_setpup_fontsize
#define fl_setpup_default_fontstyle   fl_setpup_fontstyle
#define fl_setpup_default_color       fl_setpup_color
#define fl_setpup_default_checkcolor  fl_setpup_checkcolor

FL_EXPORT int fl_setpup_default_bw(
		int bw
		);

FL_EXPORT void fl_setpup_shortcut(
		int nm,
		int ni,
		const char *sc
		);

FL_EXPORT void fl_setpup_position(
		int x,
		int y
		);

FL_EXPORT void fl_setpup_selection(
		int nm,
		int ni
		);

FL_EXPORT void fl_setpup_shadow(
		int n,
		int y
		);

FL_EXPORT void fl_setpup_softedge(
		int n,
		int y
		);

FL_EXPORT void fl_setpup_bw(
		int n,
		int bw
		);

FL_EXPORT void fl_setpup_title(
		int nm,
		const char *title
		);

FL_EXPORT FL_PUP_ENTERCB fl_setpup_entercb(
		int nm,
		FL_PUP_ENTERCB cb,
		void *data
		);

FL_EXPORT FL_PUP_LEAVECB fl_setpup_leavecb(
		int nm,
		FL_PUP_LEAVECB cb,
		void *data
		);

FL_EXPORT void fl_setpup_pad(
		int n,
		int padw,
		int padh
		);

FL_EXPORT Cursor fl_setpup_cursor(
		int nm,
		int cursor
		);

FL_EXPORT int fl_setpup_maxpup(
		int n
		);

FL_EXPORT unsigned fl_getpup_mode(
		int nm,
		int ni
		);

FL_EXPORT const char *fl_getpup_text(
		int nm,
		int ni
		);

FL_EXPORT void fl_showpup(
		int n
		);

FL_EXPORT void fl_hidepup(
		int n
		);

FL_EXPORT int fl_getpup_items(
		int n
		);

FL_EXPORT int fl_current_pup(
		void
		);


#define fl_setpup_hotkey    fl_setpup_shortcut

FL_EXPORT FL_PUP_CB fl_setpup_itemcb(
		int nm,
		int ni,
		FL_PUP_CB cb
		);

FL_EXPORT FL_PUP_CB fl_setpup_menucb(
		int nm,
		FL_PUP_CB cb
		);

FL_EXPORT void fl_setpup_submenu(
		int m,
		int i,
		int subm
		);


#define fl_setpup    fl_setpup_mode

#endif /* Xpopup } */
