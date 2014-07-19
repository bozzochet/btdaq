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


/*
 * $Id: image_text.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *  .
 *  This file is part of the XForms library package.
 *  Copyright (c) 1998-2002  T.C. Zhao
 *  All rights reserved.
 *
 *  Handling image annotation (text)
 */

#include "forms.h"
#include "flinternal.h"
#include "flimage.h"
#include "flimage_int.h"
#include <stdlib.h>

static void display_text(FL_IMAGE * im);

int
flimage_add_text(FL_IMAGE * im, const char *str, int len,
		 int style, int size,
		 unsigned int tcol, unsigned int bcol,
		 int tran, double tx, double ty, int rot)
{
    FLIMAGE_TEXT *text;

    if (!str || !*str || !len || !size || !im)
	return -1;

    if (im->text)
	im->text = fl_realloc(im->text, sizeof(*im->text) * (im->ntext + 1));
    else
	im->text = fl_malloc(sizeof(*im->text) * (im->ntext + 1));

    if (!im->text)
    {
	flimage_error(im, "AddText: malloc failed");
	return -1;
    }

    text = im->text + im->ntext;
    memset(text, 0, sizeof(*text));
    text->str = fl_strdup(str);
    text->len = len;
    text->angle = rot;
    text->size = size;
    text->style = style;
    text->color = tcol;
    text->bcolor = bcol;
    text->nobk = tran;
    text->x = im->xdist_offset + tx * im->xdist_scale;
    text->y = im->ydist_offset + ty * im->ydist_scale;

    im->free_text = flimage_delete_all_text;
    im->display_text = display_text;

    return ++im->ntext;
}

int
flimage_add_text_struct(FL_IMAGE * im, const FLIMAGE_TEXT * txt)
{
    FLIMAGE_TEXT *text;

    if (!txt || !im || !txt->str)
	return -1;

    if (txt->len <= 0)
    {
	flimage_error(im, "AddTextStruct: bad text length (%d)", txt->len);
	return -1;
    }

    if (im->text)
	im->text = fl_realloc(im->text, sizeof(*im->text) * (im->ntext + 1));
    else
	im->text = fl_malloc(sizeof(*im->text) * (im->ntext + 1));

    if (!im->text)
	return -1;

    text = im->text + im->ntext;
    memcpy(text, txt, sizeof(*text));
    text->str = fl_malloc(txt->len + 1);
    memcpy(text->str, txt->str, txt->len);
    text->str[txt->len] = '\0';

    im->free_text = flimage_delete_all_text;
    im->display_text = display_text;

    return ++im->ntext;
}

void
flimage_delete_all_text(FL_IMAGE * im)
{
    int i;

    if (!im || !im->ntext || !im->text)
	return;

    for (i = 0; i < im->ntext; i++)
	fl_free(im->text[i].str);

    fl_free(im->text);
    im->ntext = 0;
    im->text = 0;
}

static void
display_text(FL_IMAGE * im)
{
    FLIMAGE_TEXT *t, *tend;
    FL_TARGET target;

    if (im->dont_display_text || im->ntext == 0)
	return;

    if (!im->textgc)
	im->textgc = XCreateGC(im->xdisplay, im->win, 0, 0);

    memcpy(&target, fl_internal_init(), sizeof(target));

    target.display = im->xdisplay;
    target.win = im->win;
    target.isRGBColor = 1;
    target.textgc = im->textgc;
    target.gc = im->gc;

    fl_switch_target(&target);

    for (t = im->text, tend = t + im->ntext; t < tend; t++)
    {
	fl_draw_text_inside(t->align, 
                            t->x + im->wxd - im->sxd -1, 
                            t->y + im->wyd - im->syd -1,
                            2, 2, t->str, t->len,
			    t->style, t->size, t->color, t->bcolor, !t->nobk,
                            t->angle);
    }

    fl_restore_target();
}
