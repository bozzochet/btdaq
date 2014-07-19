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
 * $Id: goodies.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 */
#ifndef FL_GOODIES_H
#define FL_GOODIES_H		/* { */

/***** Resources and misc. goodie routines ******/
#define FLAlertDismissLabel     "flAlert.dismiss.label"
#define FLAlertTitle            "flAlert.title"

#define FLQuestionYesLabel      "flQuestion.yes.label"
#define FLQuestionNoLabel       "flQuestion.no.label"
#define FLQuestionTitle         "flQuestion.title"

#define FLOKLabel               "flInput.ok.label"
#define FLInputClearLabel       "flInput.clear.label"
#define FLInputCancelLabel      "flInput.cancel.label"
#define FLInputTitle            "flInput.title"

#define FLChoiceTitle           "flChoice.title"

FL_EXPORT void fl_set_goodies_font(
		int style,
		int size
		);


/*********** messages and questions **************/

FL_EXPORT void fl_show_message(
		const char *s1,
		const char *s2,
		const char *s3
		);

FL_EXPORT void fl_show_messages(
		const char *str
		);

FL_EXPORT void fl_hide_message(
		void
		);


FL_EXPORT int fl_show_question(
		const char *str,
		int ans
		);

FL_EXPORT void fl_hide_question(
		void
		);


FL_EXPORT void fl_show_alert(
		const char *title,
		const char *str1,
		const char *str2,
		int c
		);

FL_EXPORT void fl_hide_alert(
		void
		);


FL_EXPORT const char *fl_show_input(
		const char *str1,
		const char *defstr
		);

FL_EXPORT void fl_hide_input(
		void
		);

FL_EXPORT const char *fl_show_simple_input(
		const char *str1,
		const char *defstr
		);


FL_EXPORT int fl_show_colormap(
		int oldcol
		);

FL_EXPORT int fl_goodie_atclose(FL_FORM *, FL_OBJECT *);


/********* choices *****************/
FL_EXPORT int fl_show_choices(
		const char *msg,
		int numb,
		const char *c1,
		const char *c2,
		const char *c3,
		int def
		);


FL_EXPORT int fl_show_choice(
		const char *m1,
		const char *m2,
		const char *m3,
		int numb,
		const char *c1,
		const char *c2,
		const char *c3,
		int def
		);

FL_EXPORT void fl_hide_choice(
		void
		);


FL_EXPORT void fl_set_choice_shortcut(
		const char *a,
		const char *b,
		const char *c
		);

#define fl_set_choices_shortcut fl_set_choice_shortcut

/************ one liner ***************/

FL_EXPORT void fl_show_oneliner(
		const char *s,
		int x,
		int y
		);

FL_EXPORT void fl_hide_oneliner(
		void
		);

FL_EXPORT void fl_set_oneliner_font(
		int style,
		int size
		);

FL_EXPORT void fl_set_oneliner_color(
		FL_COLOR tc,
		FL_COLOR bc
		);


FL_EXPORT void fl_set_tooltip_font(
		int style,
		int size
		);

FL_EXPORT void fl_set_tooltip_color(
		FL_COLOR tc,
		FL_COLOR bc
		);

FL_EXPORT void fl_set_tooltip_boxtype(
		int bt
		);

FL_EXPORT void fl_set_tooltip_lalign(
		int align
		);


/************* command log **************/

typedef struct
{
    FL_FORM *form;
    void *vdata;
    char *cdata;
    long  ldata;
    FL_OBJECT *browser;
    FL_OBJECT *close_browser;
    FL_OBJECT *clear_browser;
}
FD_CMDLOG;

#ifdef FL_WIN32
#define FL_PID_T HANDLE
#else
#define FL_PID_T long
#endif

FL_EXPORT long fl_exe_command(
		const char *cmd,
		int block
		);

FL_EXPORT int fl_end_command(
		long pid
		);

FL_EXPORT int fl_check_command(
		long pid
		);

FL_EXPORT FILE *fl_popen(
		const char *cmd,
		const char *type
		);

FL_EXPORT int fl_pclose(
		FILE *stream
		);

FL_EXPORT int fl_end_all_command(
		void
		);

FL_EXPORT void fl_show_command_log(
		int border
		);

FL_EXPORT void fl_hide_command_log(
		void
		);

FL_EXPORT void fl_clear_command_log(
		void
		);

FL_EXPORT void fl_addto_command_log(
		const char *s
		);

FL_EXPORT void fl_set_command_log_position(
		int x,
		int y
		);

FL_EXPORT FD_CMDLOG *fl_get_command_log_fdstruct(
		void
		);


/* aliases */
#define fl_open_command    fl_exe_command
#define fl_close_command   fl_end_command

/******* file selector *****************/

#define FL_MAX_FSELECTOR  6

typedef struct
{
    FL_FORM *fselect;
    void *vdata;
    void *cdata;
    long  ldata;
    FL_OBJECT *browser, *input, *prompt, *resbutt;
    FL_OBJECT *patbutt, *dirbutt, *cancel, *ready;
    FL_OBJECT *dirlabel, *patlabel;
    FL_OBJECT *appbutt[3];
}
FD_FSELECTOR;

FL_EXPORT int fl_use_fselector(
		int n
		);

FL_EXPORT const char *fl_show_fselector(
		const char *message,
		const char *dir,
		const char *pat,
		const char *fname
		);

FL_EXPORT void fl_hide_fselector(
		void
		);


FL_EXPORT void fl_set_fselector_fontsize(
		int fsize
		);

FL_EXPORT void fl_set_fselector_fontstyle(
		int fstyle
		);

FL_EXPORT void fl_set_fselector_placement(
		int place
		);

FL_EXPORT void fl_set_fselector_border(
		int b
		);


#define fl_set_fselector_transient(b)   \
                     fl_set_fselector_border((b)?FL_TRANSIENT:FL_FULLBORDER)

FL_EXPORT void fl_set_fselector_callback(
		FL_FSCB fscb,
		void *data
		);

FL_EXPORT const char *fl_get_filename(
		void
		);

FL_EXPORT const char *fl_get_directory(
		void
		);

FL_EXPORT const char *fl_get_pattern(
		void
		);

FL_EXPORT int fl_set_directory(
		const char *p
		);

FL_EXPORT void fl_set_pattern(
		const char *s
		);

FL_EXPORT void fl_refresh_fselector(
		void
		);

FL_EXPORT void fl_add_fselector_appbutton(
		const char *label,
		void (*cb)(void *),
		void *data
		);

FL_EXPORT void fl_remove_fselector_appbutton(
		const char *label
		);

FL_EXPORT void fl_disable_fselector_cache(
		int yes
		);

FL_EXPORT void fl_invalidate_fselector_cache(
		void
		);

FL_EXPORT FL_FORM *fl_get_fselector_form(
		void
		);

FL_EXPORT FD_FSELECTOR *fl_get_fselector_fdstruct(
		void
		);

FL_EXPORT void fl_hide_fselector(
		void
		);


FL_EXPORT void fl_set_fselector_filetype_marker(
		int dir,
		int fifo,
		int sock,
		int cdev,
		int bdev
		);


#define fl_show_file_selector     fl_show_fselector
#define fl_set_fselector_cb       fl_set_fselector_callback
#define fl_set_fselector_title(s) fl_set_form_title(fl_get_fselector_form(),s)

FL_EXPORT int fl_goodies_atclose(
		FL_FORM *form,
		void *data
		);


#endif /* } */
