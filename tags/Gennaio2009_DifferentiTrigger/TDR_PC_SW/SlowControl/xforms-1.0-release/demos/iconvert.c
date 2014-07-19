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
 * Convert an image file using the image support of Forms Library.
 *
 *
 *  Usage: iconvert [-version][-verbose][-help] inputimage outimage [fmt]
 *     output image format is determined by the extension or
 *     by fmt if present.
 *
 *   Exit status:  0 (success) 1 (bad command line)  3 (conversion failed)
 *
 *  T.C. Zhao (03/1999)
 *
 */

#include <stdlib.h>
#include "forms.h"
#include "flimage.h"

static void initialize(void);
static void usage(const char *, int);
static int  parse_command_line(int *, char **);
static char *version="$Id: iconvert.c,v 0.11 1999/03/15 04:13:45 zhao Exp $";

int
main(int argc, char *argv[])
{
    FL_IMAGE *im;
    const char *fmt = 0;
    char *const *args;

    initialize();
    args = argv + parse_command_line(&argc, argv);

    if ( argc < 3 || !(fmt = argc>=4 ? args[3]: strrchr(args[2],'.')))
       usage(argv[0], argc>=3);

    fmt += (fmt[0]=='.'); /* remove . */
    im = flimage_load(strdup(args[1]));
    return flimage_dump(im, args[2], fmt) < 0 ? 3:0;
}


static void usage(const char *cmd, int more)
{
    const FLIMAGE_FORMAT_INFO *info;
    int n, i, k;

    fprintf(stderr,"Usage: %s [-verbose][-help] infile outfile [fmt]\n", cmd);

    if(!more)
       exit(1);

    fputs(" The output format is determined by the file extension ",stderr);
    fputs("or fmt.\n fmt or extension must be one of the following\n",stderr);

    n = flimage_get_number_of_formats();
    for ( i = 1, k = 0; i <= n; i++)
    {
          info = flimage_get_format_info(i);
          if((info->read_write & FLIMAGE_WRITABLE))
          {
               fprintf(stderr,"\t%s", info->extension);
               if((++k%6) == 0)
                 fputc('\n',stderr);
          }
    }

    if(k%6)
        fputc('\n', stderr);
    exit(1);
}

/* shut up visual_cue */
static int noop(FL_IMAGE *im, const char *s)
{
    return 0;
}

static int parse_command_line(int *argc, char *argv[])
{ 
    int i;
    static FLIMAGE_SETUP setup;

    setup.visual_cue = noop;

    for ( i = 1; i < *argc && *argv[i]=='-';)
    {
         if(strncmp(argv[i],"-verb",5)==0)
         {
             setup.visual_cue = 0;
             i++;
         }
         else if(strncmp(argv[i],"-vers",5)==0)
         {
            fprintf(stderr,"%s\n",version);
            exit(0);
         }
         else if(strncmp(argv[i],"-h",2)==0)
         {
             usage(argv[0],1);
             i++;
         }
         else
             usage(argv[0],0);
    }

    flimage_setup(&setup); 
    *argc -= i-1;
    return i-1;
} 

static void initialize(void)
{
    flimage_enable_xpm();
    flimage_enable_gif();
    flimage_enable_bmp();
    flimage_enable_sgi();
    flimage_enable_fits();
    flimage_enable_png();
    flimage_enable_xwd();
    flimage_enable_tiff();
    flimage_enable_ps();
#ifndef NO_JPEG
    flimage_enable_jpeg();
#endif
}
