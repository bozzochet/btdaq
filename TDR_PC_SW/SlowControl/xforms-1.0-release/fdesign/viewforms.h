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


/** Header file generated with fdesign on Sun Oct 17 13:27:13 1999.**/

#ifndef FD_main_h_
#define FD_main_h_

/** Callbacks, globals and object handlers **/
extern void text_timeout_cb(FL_OBJECT *, long);
extern void file_mnu_cb(FL_OBJECT *, long);
extern void options_mnu_cb(FL_OBJECT *, long);
extern void but_cb(FL_OBJECT *, long);
extern void help_mnu_cb(FL_OBJECT *, long);
extern void movie_slider_cb(FL_OBJECT *, long);

extern void help_nav_cb(FL_OBJECT *, long);
extern void text_timeout_cb(FL_OBJECT *, long);
extern void but_cb(FL_OBJECT *, long);

extern void but_cb(FL_OBJECT *, long);
extern void font_choice_cb(FL_OBJECT *, long);
extern void size_choice_cb(FL_OBJECT *, long);

extern void but_cb(FL_OBJECT *, long);

extern void update_cb(FL_OBJECT *, long);
extern void but_cb(FL_OBJECT *, long);
extern void label_dist_cb(FL_OBJECT *, long);
extern void freq_dist_cb(FL_OBJECT *, long);
extern void anim_time_cb(FL_OBJECT *, long);

extern void x_snap_cb(FL_OBJECT *, long);
extern void y_snap_cb(FL_OBJECT *, long);
extern void r_snap_cb(FL_OBJECT *, long);
extern void z_snap_cb(FL_OBJECT *, long);
extern void but_cb(FL_OBJECT *, long);

extern void but_cb(FL_OBJECT *, long);

extern void but_cb(FL_OBJECT *, long);
extern void x_size_cb(FL_OBJECT *, long);
extern void y_size_cb(FL_OBJECT *, long);

extern void but_cb(FL_OBJECT *, long);
extern void Void(FL_OBJECT *, long);

extern void but_cb(FL_OBJECT *, long);
extern void Void(FL_OBJECT *, long);

extern void col_cb(FL_OBJECT *, long);
extern void but_cb(FL_OBJECT *, long);

extern void but_cb(FL_OBJECT *, long);
extern void interp_freq_cb(FL_OBJECT *, long);


/**** Forms and Objects ****/
typedef struct {
	FL_FORM *main;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *timeout;
	FL_OBJECT *file;
	FL_OBJECT *options;
	FL_OBJECT *button5;
	FL_OBJECT *glcanvas;
	FL_OBJECT *help;
	FL_OBJECT *movie_slider;
	FL_OBJECT *file_grp;
	FL_OBJECT *button2;
	FL_OBJECT *button1;
	FL_OBJECT *player_grp;
	FL_OBJECT *button60;
	FL_OBJECT *button61;
	FL_OBJECT *button62;
	FL_OBJECT *button63;
	FL_OBJECT *button64;
	FL_OBJECT *button9;
	FL_OBJECT *comment;
	FL_OBJECT *struccount;
	FL_OBJECT *framecount;
} FD_main;

extern FD_main * create_form_main(void);
typedef struct {
	FL_FORM *help;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *help_index;
	FL_OBJECT *help_disp;
	FL_OBJECT *timeout;
	FL_OBJECT *button10;
} FD_help;

extern FD_help * create_form_help(void);
typedef struct {
	FL_FORM *fontselect;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *button20;
	FL_OBJECT *font;
	FL_OBJECT *button21;
	FL_OBJECT *size;
	FL_OBJECT *text;
} FD_fontselect;

extern FD_fontselect * create_form_fontselect(void);
typedef struct {
	FL_FORM *about;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *revision;
	FL_OBJECT *date;
	FL_OBJECT *button30;
} FD_about;

extern FD_about * create_form_about(void);
typedef struct {
	FL_FORM *brucc_opt;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *labelfreq;
	FL_OBJECT *button43;
	FL_OBJECT *button42;
	FL_OBJECT *button40;
	FL_OBJECT *spl_brucc;
	FL_OBJECT *button47;
	FL_OBJECT *button44;
	FL_OBJECT *button48;
	FL_OBJECT *but401;
	FL_OBJECT *but400;
	FL_OBJECT *dln_brucc;
	FL_OBJECT *button46;
	FL_OBJECT *button45;
	FL_OBJECT *button49;
	FL_OBJECT *label_dist;
	FL_OBJECT *freq_dist;
	FL_OBJECT *anim_time;
	FL_OBJECT *strip_width;
} FD_brucc_opt;

extern FD_brucc_opt * create_form_brucc_opt(void);
typedef struct {
	FL_FORM *snapshot;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *x_displacmnt;
	FL_OBJECT *y_displacmnt;
	FL_OBJECT *rotation;
	FL_OBJECT *zoom;
	FL_OBJECT *comment;
	FL_OBJECT *button70;
} FD_snapshot;

extern FD_snapshot * create_form_snapshot(void);
typedef struct {
	FL_FORM *log;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *browser;
	FL_OBJECT *button80;
	FL_OBJECT *button81;
} FD_log;

extern FD_log * create_form_log(void);
typedef struct {
	FL_FORM *save;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *button90;
	FL_OBJECT *button91;
	FL_OBJECT *filename;
	FL_OBJECT *xsize;
	FL_OBJECT *ysize;
} FD_save;

extern FD_save * create_form_save(void);
typedef struct {
	FL_FORM *save_as_file;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *button90;
	FL_OBJECT *button91;
	FL_OBJECT *filename;
	FL_OBJECT *xsize;
	FL_OBJECT *ysize;
	FL_OBJECT *button92;
	FL_OBJECT *frames;
	FL_OBJECT *frames_fr_to;
	FL_OBJECT *frames_from;
	FL_OBJECT *frames_to;
	FL_OBJECT *frames_current;
	FL_OBJECT *frames_all;
} FD_save_as_file;

extern FD_save_as_file * create_form_save_as_file(void);
typedef struct {
	FL_FORM *save_as;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *button90;
	FL_OBJECT *button91;
	FL_OBJECT *xsize;
	FL_OBJECT *ysize;
	FL_OBJECT *frames_from;
	FL_OBJECT *frames_to;
	FL_OBJECT *frames;
	FL_OBJECT *frames_fr_to;
	FL_OBJECT *frames_current;
	FL_OBJECT *frames_all;
	FL_OBJECT *format;
	FL_OBJECT *format_gif;
	FL_OBJECT *format_ps;
	FL_OBJECT *format_eps;
	FL_OBJECT *format_2dcoords;
	FL_OBJECT *pathname;
	FL_OBJECT *prefix;
} FD_save_as;

extern FD_save_as * create_form_save_as(void);
typedef struct {
	FL_FORM *color_opt;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *backbone_col;
	FL_OBJECT *background_col;
	FL_OBJECT *nucno_col;
	FL_OBJECT *wc_col;
	FL_OBJECT *gu_col;
	FL_OBJECT *other_bond_col;
	FL_OBJECT *a_col;
	FL_OBJECT *c_col;
	FL_OBJECT *g_col;
	FL_OBJECT *u_col;
	FL_OBJECT *other_nuc_col;
	FL_OBJECT *nuc_un_col;
} FD_color_opt;

extern FD_color_opt * create_form_color_opt(void);
typedef struct {
	FL_FORM *interp;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *interp_freq;
} FD_interp;

extern FD_interp * create_form_interp(void);

#endif /* FD_main_h_ */
