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
 * $Id: flps.h,v 0.89 1999/07/17 03:17:52 zhao Beta $
 */

#ifndef FLPS_H
#define FLPS_H

/* postscript stuff */

enum 
{
   FLPS_AUTO,                  /* switch to landscale if does not fit */
   FLPS_LANDSCAPE,             /* landscape always                    */
   FLPS_PORTRAIT,              /* portrait always                     */
   FLPS_BESTFIT                /* even margins/best fit               */
};

enum
{
  FLPS_BW = -1, FLPS_GRAYSCALE, FLPS_COLOR
};

#define FLPS_MEMBERS                                     \
             int        ps_color;                        \
             int        orientation;                     \
             int        auto_fit;                        \
             int        drawbox;                         \
             int        eps;                             \
             float      xdpi, ydpi;                      \
             float      paper_w, paper_h;                \
             float      gamma;                           \
             const char *tmpdir;                         \
             int        printer_dpi;                     \
             float      hm, vm;                          \
             float      xscale, yscale;                  \
             int        scale_text;                      \
             int        first_page_only;                 \
             int        clip;                            \
             int        reserved[10]             

/* configuration infomation */
typedef struct flps_cntl_ { FLPS_MEMBERS; } FLPS_CONTROL;

FL_EXPORT FLPS_CONTROL *flps_init(
		void
		);

FL_EXPORT int fl_object_ps_dump(
		FL_OBJECT *ob,
		const char *fname
		);


#endif
