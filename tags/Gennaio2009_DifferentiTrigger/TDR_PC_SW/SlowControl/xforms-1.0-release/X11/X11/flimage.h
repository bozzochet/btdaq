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
 * $Id: flimage.h,v 0.89 1999/07/17 03:17:52 zhao Beta $
 *
 * Image related routines
 *
 */

#ifndef FL_IMAGE_H
#define FL_IMAGE_H

#if defined(__cplusplus)
extern "C"
{
#endif

#ifndef FL_EXPORT
#define FL_EXPORT extern
#endif


#define FL_RGB2GRAY(r,g,b)  (unsigned)((78*(r)+150*(g)+28*(b))>>8)

enum
{
    FL_IMAGE_NONE,
    FL_IMAGE_MONO = 1,		/* b&w. 1bit. 0=white 1=black */
    FL_IMAGE_GRAY = 2,		/* 8 bit gray image           */
    FL_IMAGE_CI = 4,		/* colormmaped image          */
    FL_IMAGE_RGB = 8,		/* RGBA image. 8bit each      */
    FL_IMAGE_PACKED  = 16,	/* RGBA packed into an int    */
    FL_IMAGE_GRAY16 = 32,	/* 12bit gray scale image     */
    FL_IMAGE_RGB16 = 64,	/* 36bits color image         */
    FL_IMAGE_FLEX = 1023,       /* all formats                */
    /* aliases */
    FLIMAGE_NONE   = FL_IMAGE_NONE,
    FLIMAGE_MONO   = FL_IMAGE_MONO,
    FLIMAGE_GRAY   = FL_IMAGE_GRAY,
    FLIMAGE_CI     = FL_IMAGE_CI,
    FLIMAGE_RGB    = FL_IMAGE_RGB,
    FLIMAGE_PACKED = FL_IMAGE_PACKED,
    FLIMAGE_GRAY16 = FL_IMAGE_GRAY16,
    FLIMAGE_RGB16  = FL_IMAGE_RGB16,
    FLIMAGE_FLEX   = FL_IMAGE_FLEX
};

#ifndef FL_PCBITS
typedef unsigned char FL_PCTYPE;	/* primary color type */
#define FL_PCBITS          8	/* primary color bits */
#define FL_PCMAX          ((1<<FL_PCBITS)-1)
#define FL_PCCLAMP(a)     ((a)>(FL_PCMAX) ? (FL_PCMAX):((a) < 0 ? 0:(a)))
typedef unsigned int       FL_PACKED4;
#define FL_PACKED          FL_PACKED4

#define FL_RMASK          0x000000ff
#define FL_RSHIFT         0
#define FL_GMASK          0x0000ff00
#define FL_GSHIFT         8
#define FL_BMASK          0x00ff0000
#define FL_BSHIFT         16
#define FL_AMASK          0xff000000
#define FL_ASHIFT         24
/* if PCBITS is not 8, we need to apply the RGBmask */
#define FL_GETR(packed)   ((packed)&FL_RMASK)
#define FL_GETG(packed)   (((packed)>>FL_GSHIFT)&FL_PCMAX)
#define FL_GETB(packed)   (((packed)>>FL_BSHIFT)&FL_PCMAX)
#define FL_GETA(packed)   (((packed)>>FL_ASHIFT)&FL_PCMAX)

#define FL_PACK3(r,g,b)   (((r)<<FL_RSHIFT)|((g)<<FL_GSHIFT)|((b)<<FL_BSHIFT))
#define FL_PACK            FL_PACK3
#define FL_PACK4(r,g,b,a) (FL_PACK3(r,g,b)|((a)<<FL_ASHIFT))

#define FL_UNPACK(p,r,g,b) do {r=FL_GETR(p);g=FL_GETG(p),b=FL_GETB(p);} while(0)
#define FL_UNPACK3         FL_UNPACK
#define FL_UNPACK4(p,r,g,b,a) do {FL_UNPACK3(p,r,g,b);a=FL_GETA(p);} while(0) 
#endif

#define FL_LUTBITS        12    /* max colormap bits. 4096 entries */

#define FL_IsRGB(im)      (im->type == FL_IMAGE_RGB)
#define FL_IsPacked(im)   (im->type == FL_IMAGE_PACKED)

enum 
{ 
    FLIMAGE_AUTOCOLOR = 0x7fffffff,
    FLIMAGE_BADCOLOR = FLIMAGE_AUTOCOLOR 
};



typedef Window FL_WINDOW;   /* unsigned long */

typedef struct flimage_text_
{
    char *str;		        /* the string itself             */
    int len;			/* string length                 */
    int x, y;			/* starting location of text (wrt image) */
    unsigned int color;		/* color of the text             */
    unsigned int bcolor;	/* background color of the text  */
    int nobk;                   /* no background                 */
    int size, style;		/* font size & style             */
    int angle;                  /* in 1/10th of a degrees        */
    int align;                  /* alignment wrt to (x,y)        */
    int reserved[6];
} FLIMAGE_TEXT;

typedef struct flimage_marker_
{
    const char *name;           /* marker name                  */
    int w, h;                   /* size                         */
    int x, y;                   /* location                     */
    unsigned int color;		/* color of the marker          */
    unsigned int bcolor;	/* aux. color of the marker     */
    int angle;                  /* in 1/10th of a degree        */
    int fill;
    int thickness;              /* line thickness               */
    int style;                  /* line style                   */
    /* the following is filled by the library */
    void *display;
    void *gc;
    FL_WINDOW win;
    const char *psdraw;
    int reserved[6];
} FLIMAGE_MARKER;
    

#define FLIMAGE_REPFREQ  0x1f   /* report every 32 lines */

#ifndef FL_RGB2PIXEL
typedef struct
{
   unsigned int rshift, rmask, rbits;
   unsigned int gshift, gmask, gbits;
   unsigned int bshift, bmask, bbits;
   int bits_per_rgb;
   int colormap_size;
   int reserved[2];
} FL_RGB2PIXEL_;

#define FL_RGB2PIXEL  FL_RGB2PIXEL_
#endif


typedef struct flimage_setup_ *FLIMAGESETUP;

struct flimage_src_;
struct flimage_dest_;

typedef struct flimage_
{
    int type;			/* image type                    */
    int w, h;			/* image size                    */
    void *app_data;		/* for application at setup time */
    void *u_vdata;		/* for application               */
    long  u_ldata;		/* for application               */
    unsigned char **red;        
    unsigned char **green;
    unsigned char **blue;
    unsigned char **alpha;
    unsigned char **rgba[4];    /* alias                         */
    unsigned short **ci;
    unsigned short **gray;
    FL_PACKED4 **packed;
    unsigned short **red16;    /* not currently supported */
    unsigned short **green16;  /* not currently supported */
    unsigned short **blue16;   /* not currently supported */
    unsigned short **alpha16;  /* not currently supported */
    unsigned char  **ci8;  /* not currently supported */
    int *red_lut;              /* red lookup tables                */
    int *green_lut;            /* green lookup tables              */
    int *blue_lut;             /* blue lookup tables               */
    int *alpha_lut;            /* alpha lookup tables              */
    int *lut[4];               /* alias                            */
    int map_len;               /* lut length                       */
    int colors;                /* actual colors used in displaying */
    int gray_maxval;           /* indicate the range of gray16     */
    int ci_maxval;             /* max value of ci. not used. use map_len */
    int rgb_maxval;            /* max value for rgb16 image       */
    int level, wwidth;
    unsigned short *wlut;      /* lut for window levelling         */
    int wlut_len;
    int app_background;        /* transparent color: in RGB        */
    char *comments;
    int comments_len;
    int available_type;
    struct flimage_ *next;
    int sx, sy;                 /* display subimage origin          */
    int sw, sh;                 /* display subimage width           */
    int wx, wy;                 /* display location relative to win */
    int modified;
    int (*display)(struct flimage_ *, FL_WINDOW);
    int double_buffer;
    int sxd, syd, swd, shd;
    int wxd, wyd;
    const char *fmt_name;       /* format name (ppm,jpg etc)     */
    int bi_reserved[8];

    /* annotation stuff */
    FLIMAGE_TEXT *text;
    int ntext, max_text;
    int dont_display_text;
    void (*display_text)(struct flimage_ *);
    void (*free_text)(struct flimage_ *);
    FLIMAGE_MARKER *marker;
    int nmarkers, max_markers;
    int dont_display_marker;
    void (*display_markers)(struct flimage_ *);
    void (*free_markers)(struct flimage_ *);
    int an_reserved[8];

    /* physicalValue = poffset + pixelValue * pscale  */
    double pmin, pmax;        /* physical data range             */
    double poffset;   
    double pscale;
    /* pixel grid distance */
    double xdist_offset;
    double xdist_scale;
    double ydist_offset;
    double ydist_scale;
    int px_reserved[8];

    char *infile;
    char *outfile;
    long foffset;
    int original_type;
     /* hooks for application to have a chance to set some options.
        if pre_write returns -1, the output will be canceled 
     */
    int (*pre_write)(struct flimage_ *);
    int (*post_write)(struct flimage_ *); 
    int f_reserved[16];

    /* image processing stuff */
    int subx, suby;           /* subimage origin       */
    int subw, subh;           /* subimage size         */
    int sub_shape;            /* shape of the subimage */
    unsigned int fill_color;  /* fill color            */
    int force_convert;
    int *llut[3];            /* linear lut            */
    int llut_len; 
    unsigned int *hist[4];
    int ip_reserved[16];

    /* application handlers */ 
    int total, completed;
    int (*visual_cue) (struct flimage_*,  const char *);
    void (*error_message) (struct flimage_*, const char *);
    int error_code;       /* not currently used */

    int display_type;		/* just before handing it to X      */
    unsigned short **pixels;
    void *image_spec;		/* additional image info            */
    void *xdisplay;             /* the X connection                 */
    int tran_rgb;           /* RGB color that should be transparent */
    int tran_index;         /* index that should be transparent     */
    int matr, matc;
    /* multi-frame images */
    int more;
    int current_frame;
    int total_frames;
    int (*next_frame)(struct flimage_ *);
    int (*prev_frame)(struct flimage_ *);
    int (*random_frame)(struct flimage_ *, int);
    int (*rewind_frame)(struct flimage_ *);
    void (*cleanup)(struct flimage_ *);
    int stop_looping;
    int mi_reserved[16];

    /* the following are for internal use */
    FILE *fpin;
    FILE *fpout;
    void *image_io;
    void *io_spec;		/* io operation helper       */
    int spec_size;
    int depth;                  /* the depth we actually use */
    int vclass;
    void *visual;
    unsigned long xcolormap;
    FL_RGB2PIXEL rgb2p;
    void *ximage;
    FL_WINDOW win;
    void *gc;
    int sdepth;                 /* depth the server says     */
    void *textgc;
    void *markergc;
    void *extra_io_info;
    unsigned long pixmap;
    int pixmap_w, pixmap_h, pixmap_depth;
    int isPixmap;
    FLIMAGESETUP setup;
    char *info;
    struct flimage_src_  *src;  /* src other than file         */
    struct flimage_dest_ *dest; /* destination other than file */
    int internal_reserved[14];
} 
FL_IMAGE;

/* some configuration stuff */
typedef struct flimage_setup_
{
    void *app_data;
    int (*visual_cue)(FL_IMAGE *, const char *);
    void (*error_message)(FL_IMAGE *, const char *);
    int (*display)(FL_IMAGE *, unsigned long);
    const char *rgbfile;
    int do_not_clear;
    void *xdisplay;
    int max_frames;
    int delay;
    int no_auto_extension;
    int report_frequency;
    int double_buffer;
    /* internal use */
    unsigned long trailblazer;
    int header_info;
    int reserved[8];
} FLIMAGE_SETUP;

FL_EXPORT void flimage_setup(
		FLIMAGE_SETUP *setup
		);


/* possible errors from the library. Not currently (v0.89) used */
enum
{
   FLIMAGE_ERR_NONE = 0,
   FLIMAGE_ERR_ALLOC = -50,     /* allocation error     */
   FLIMAGE_ERR_INVALID,         /* invalid image        */
   FLIMAGE_ERR_ARGUMENT,        /* bad argument/request */
   FLIMAGE_ERR_FILE,            /* io error             */
   FLIMAGE_ERR_INTERNAL,        /* bugs                 */
   FLIMAGE_ERR_UNKNOWN
};


typedef int (*FLIMAGE_Identify) (FILE *);
typedef int (*FLIMAGE_Description) (FL_IMAGE *);
typedef int (*FLIMAGE_Read_Pixels) (FL_IMAGE *);
typedef int (*FLIMAGE_Write_Image) (FL_IMAGE *);

/* basic IO routines */
FL_EXPORT FL_IMAGE *flimage_load(
		const char *file
		);

FL_EXPORT FL_IMAGE *flimage_read(
		FL_IMAGE *im
		);

FL_EXPORT int flimage_dump(
		FL_IMAGE *image,
		const char *filename,
		const char *fmt
		);

FL_EXPORT int flimage_close(
		FL_IMAGE *im
		);

FL_EXPORT FL_IMAGE *flimage_alloc(
		void
		);

FL_EXPORT int flimage_getmem(
		FL_IMAGE *im
		);


FL_EXPORT int flimage_is_supported(
		const char *file
		);


FL_EXPORT int flimage_description_via_filter(
		FL_IMAGE *im,
		char *const *cmds,
		const char *what,
		int verbose
		);

FL_EXPORT int flimage_write_via_filter(
		FL_IMAGE *im,
		char *const *cmds,
		char *const formats[],
		int verbose
		);



FL_EXPORT FL_IMAGE *flimage_alloc(
		void
		);

FL_EXPORT int flimage_free(
		FL_IMAGE *image
		);

FL_EXPORT int flimage_display(
		FL_IMAGE *in_image,
		Window win
		);

FL_EXPORT int flimage_sdisplay(
		FL_IMAGE *im,
		Window win
		);

FL_EXPORT int flimage_convert(
		FL_IMAGE *image,
		int newtype,
		int ncolors
		);

FL_EXPORT const char *flimage_type_name(
		int type
		);

FL_EXPORT int flimage_add_text(
		FL_IMAGE *im,
		const char *str,
		int len,
		int style,
		int size,
		unsigned int tcol,
		unsigned int bcol,
		int tran,
		double tx,
		double ty,
		int rot
		);

FL_EXPORT int flimage_add_text_struct(
		FL_IMAGE *im,
		const FLIMAGE_TEXT *txt
		);

FL_EXPORT void flimage_delete_all_text(
		FL_IMAGE *im
		);


FL_EXPORT int flimage_add_marker(
		FL_IMAGE *im,
		const char *name,
		double x,
		double y,
		double w,
		double h,
		int style,
		int fill,
		int rot,
		unsigned int col,
		unsigned bcol
		);


FL_EXPORT int flimage_add_marker_struct(
		FL_IMAGE *im,
		const FLIMAGE_MARKER *min
		);

FL_EXPORT int flimage_define_marker(
		const char *name,
		void (*draw)(FLIMAGE_MARKER *),
		const char *psdraw
		);

FL_EXPORT void flimage_delete_all_markers(
		FL_IMAGE *im
		);

FL_EXPORT int flimage_render_annotation(
		FL_IMAGE *im,
		FL_WINDOW win
		);


FL_EXPORT void flimage_error(
		FL_IMAGE *im,
		const char *fmt,
		...
		);


/* built-in format supports */
FL_EXPORT void flimage_enable_pnm(
		void
		);

FL_EXPORT int flimage_set_fits_bits(
		int bits
		);


/* output options       */
typedef struct
{
    int quality;
    int smoothing;
    int reserved[6];
} FLIMAGE_JPEG_OPTION;

FL_EXPORT void flimage_jpeg_options(
		FLIMAGE_JPEG_OPTION *op
		);

FL_EXPORT void flimage_pnm_options(
		int raw
		);

FL_EXPORT void flimage_gif_options(
		int inter
		);

struct flps_cntl_; /* the same as FLPS_CONTROL */
typedef struct flps_cntl_ FLIMAGE_PS_OPTION;
FL_EXPORT FLPS_CONTROL *flimage_ps_options(
		void
		);


#define flimage_jpeg_output_options   flimage_jpeg_options
#define flimage_pnm_output_options    flimage_pnm_options
#define flimage_gif_output_options    flimage_gif_options

enum
{
    FLIMAGE_WRITABLE = FL_WRITE, FLIMAGE_READABLE = FL_READ
};

typedef struct
{
    const char *formal_name;
    const char *short_name;
    const char *extension;
    int type;
    int read_write;
    int annotation;
    int reserved[5];
} FLIMAGE_FORMAT_INFO;

FL_EXPORT int flimage_get_number_of_formats(
		void
		);

FL_EXPORT const FLIMAGE_FORMAT_INFO *flimage_get_format_info(
		int n
		);


FL_EXPORT void *fl_get_matrix(
		int nrows,
		int ncols,
		unsigned int esize
		);

FL_EXPORT void *fl_make_matrix(
		int nrows,
		int ncols,
		unsigned int esize,
		void *mem
		);

FL_EXPORT void fl_free_matrix(
		void *p
		);

FL_EXPORT char *fl_basename(
		char name[]
		);

FL_EXPORT int fl_init_RGBdatabase(
		const char *f
		);

FL_EXPORT int fl_lookup_RGBcolor(
		const char *req_name,
		int *r,
		int *g,
		int *b
		);


FL_EXPORT int flimage_add_format(
		const char *formal_name,
		const char *short_name,
		const char *extension,
		int type,
		FLIMAGE_Identify identify,
		FLIMAGE_Description description,
		FLIMAGE_Read_Pixels read_pixels,
		FLIMAGE_Write_Image write_image
		);


FL_EXPORT void flimage_set_annotation_support(
		int in,
		int flag
		);


FL_EXPORT int flimage_getcolormap(
		FL_IMAGE *im
		);


FL_EXPORT void fl_select_octree_quantizer(void);

FL_EXPORT void fl_select_mediancut_quantizer(
		void
		);



/* simple image processing routines */

#define FLIMAGE_SHARPEN        (int**)(-1)
#define FLIMAGE_SMOOTH         (int**)(-2)
#define FL_SMOOTH              FLIMAGE_SMOOTH  
#define FL_SHARPEN             FLIMAGE_SHARPEN  

enum
{ 
   FLIMAGE_NOSUBPIXEL  = 0,    /* scale with no subpixel sampling */         
   FLIMAGE_SUBPIXEL  = 1,      /* scale with  subpixel sampling */         
   FLIMAGE_CENTER = 2,         /* center warped image. default  */
   FLIMAGE_RIGHT = 8,          /* flush right the warped image  */
   FLIMAGE_ASPECT = 32,        /* fit the size */
   FLIMAGE_NOCENTER = FL_ALIGN_TOP_LEFT 
};

FL_EXPORT int flimage_convolve(
		FL_IMAGE *im,
		int **kernel,
		int krow,
		int kcol
		);

FL_EXPORT int flimage_convolvea(
		FL_IMAGE *im,
		int *kernel,
		int krow,
		int kcol
		);

FL_EXPORT int flimage_tint(
		FL_IMAGE *im,
		unsigned int packed,
		double opacity
		);

FL_EXPORT int flimage_rotate(
		FL_IMAGE *im,
		int deg,
		int subp
		);

FL_EXPORT int flimage_flip(
		FL_IMAGE *im,
		int axis
		);

FL_EXPORT int flimage_scale(
		FL_IMAGE *im,
		int nw,
		int nh,
		int option
		);

FL_EXPORT int flimage_warp(
		FL_IMAGE *im,
		float m[][2],
		int nw,
		int nh,
		int option
		);

FL_EXPORT int flimage_autocrop(
		FL_IMAGE *im,
		unsigned int bk
		);

FL_EXPORT int flimage_get_autocrop(
		FL_IMAGE *im,
		unsigned int bk,
		int *xl,
		int *yt,
		int *xr,
		int *yb
		);

FL_EXPORT int flimage_crop(
		FL_IMAGE *im,
		int xl,
		int yt,
		int xr,
		int yb
		);

FL_EXPORT int flimage_replace_pixel(
		FL_IMAGE *im,
		unsigned int target,
		unsigned int repl
		);

FL_EXPORT int flimage_transform_pixels(
		FL_IMAGE *im,
		int *red,
		int *green,
		int *blue
		);

FL_EXPORT int flimage_windowlevel(
		FL_IMAGE *im,
		int level,
		int width
		);

FL_EXPORT int flimage_enhance(
		FL_IMAGE *im,
		int delta
		);


FL_EXPORT int flimage_from_pixmap(
		FL_IMAGE *im,
		Pixmap pixmap
		);

FL_EXPORT Pixmap flimage_to_pixmap(
		FL_IMAGE *im,
		FL_WINDOW win
		);

FL_EXPORT FL_IMAGE *flimage_dup(
		FL_IMAGE *sim
		);

/* Miscellaneous prototypes */

FL_EXPORT void *fl_get_submatrix(
		void *in,
		int rows,
		int cols,
		int r1,
		int c1,
		int rs,
		int cs,
		unsigned int esize
		);

FL_EXPORT int fl_j2pass_quantize_packed(
		unsigned int **packed,
		int w,
		int h,
		int max_color,
		unsigned short **ci,
		int *actual_color,
		int *red_lut,
		int *green_lut,
		int *blue_lut,
		FL_IMAGE *im
		);

FL_EXPORT int fl_j2pass_quantize_rgb(
		unsigned char **red,
		unsigned char **green,
		unsigned char **blue,
		int w,
		int h,
		int max_color,
		unsigned short **ci,
		int *actual_color,
		int *red_lut,
		int *green_lut,
		int *blue_lut,
		FL_IMAGE *im
		);

FL_EXPORT void *fl_make_submatrix(
		void *in,
		int rows,
		int cols,
		int r1,
		int c1,
		int rs,
		int cs,
		unsigned int esize
		);

FL_EXPORT int fl_object_ps_dump(
		FL_OBJECT *ob,
		const char *fname
		);

FL_EXPORT void fl_pack_bits(
		unsigned char *out,
		unsigned short *in,
		int len
		);

FL_EXPORT void fl_unpack_bits(
		unsigned short *out,
		unsigned char *in,
		int len
		);

FL_EXPORT unsigned int fl_value_to_bits(
		unsigned int val
		);

FL_EXPORT void flimage_add_comments(
		FL_IMAGE *im,
		const char *s,
		int len
		);

FL_EXPORT unsigned long flimage_color_to_pixel(
		FL_IMAGE *im,
		int r,
		int g,
		int b,
		int *newpix
		);

FL_EXPORT FL_IMAGE *flimage_combine(
		FL_IMAGE *im1,
		FL_IMAGE *im2,
		double alpha
		);

FL_EXPORT int flimage_define_marker(
		const char *name,
		void (*draw)(FLIMAGE_MARKER *),
		const char *psdraw
		);

FL_EXPORT void flimage_display_markers(
		FL_IMAGE *im
		);

FL_EXPORT FL_IMAGE *flimage_dup_(
		FL_IMAGE *sim,
		int pix
		);

FL_EXPORT void flimage_enable_bmp(
		void
		);

FL_EXPORT void flimage_enable_fits(
		void
		);

FL_EXPORT void flimage_enable_genesis(
		void
		);

FL_EXPORT void flimage_enable_gif(
		void
		);

FL_EXPORT void flimage_enable_gzip(
		void
		);

FL_EXPORT void flimage_enable_jpeg(
		void
		);

FL_EXPORT void flimage_enable_png(
		void
		);

FL_EXPORT void flimage_enable_ps(
		void
		);

FL_EXPORT void flimage_enable_sgi(
		void
		);

FL_EXPORT void flimage_enable_tiff(
		void
		);

FL_EXPORT void flimage_enable_xbm(
		void
		);

FL_EXPORT void flimage_enable_xpm(
		void
		);

FL_EXPORT void flimage_enable_xwd(
		void
		);

FL_EXPORT void flimage_free_ci(
		FL_IMAGE *im
		);

FL_EXPORT void flimage_free_gray(
		FL_IMAGE *im
		);

FL_EXPORT void flimage_free_linearlut(
		FL_IMAGE *im
		);

FL_EXPORT void flimage_free_rgb(
		FL_IMAGE *im
		);

FL_EXPORT void flimage_freemem(
		FL_IMAGE *image
		);

FL_EXPORT int flimage_get_closest_color_from_map(
		FL_IMAGE *im,
		unsigned int col
		);

FL_EXPORT int flimage_get_linearlut(
		FL_IMAGE *im
		);

FL_EXPORT void flimage_invalidate_pixels(
		FL_IMAGE *im
		);

FL_EXPORT FL_IMAGE *flimage_open(
		const char *file
		);

FL_EXPORT int flimage_read_annotation(
		FL_IMAGE *im
		);

FL_EXPORT void flimage_replace_image(
		FL_IMAGE *im,
		int w,
		int h,
		void *r,
		void *g,
		void *b
		);

FL_EXPORT int flimage_swapbuffer(
		FL_IMAGE *im
		);

FL_EXPORT int flimage_to_ximage(
		FL_IMAGE *im,
		FL_WINDOW win,
		XWindowAttributes *xwa
		);

FL_EXPORT int flimage_write_annotation(
		FL_IMAGE *im
		);

FL_EXPORT void flps_apply_gamma(
		float gamma
		);

FL_EXPORT void flps_arc(
		int fill,
		int x,
		int y,
		int r,
		int t1,
		int t2,
		long col
		);

FL_EXPORT void flps_circ(
		int fill,
		int x,
		int y,
		int r,
		long col
		);

FL_EXPORT void flps_color(
		long color
		);

FL_EXPORT void flps_draw_box(
		int style,
		int x,
		int y,
		int w,
		int h,
		long col,
		int bw_in
		);

FL_EXPORT void flps_draw_checkbox(
		int type,
		int x,
		int y,
		int w,
		int h,
		long col,
		int bw
		);

FL_EXPORT void flps_draw_frame(
		int style,
		int x,
		int y,
		int w,
		int h,
		long col,
		int bw
		);

FL_EXPORT int flps_draw_symbol(
		const char *label,
		int x,
		int y,
		int w,
		int h,
		long col
		);

FL_EXPORT void flps_draw_tbox(
		int style,
		int x,
		int y,
		int w,
		int h,
		long col,
		int bw
		);

FL_EXPORT void flps_draw_text(
		int align,
		int x,
		int y,
		int w,
		int h,
		long col,
		int style,
		int size,
		const char *istr
		);

FL_EXPORT void flps_draw_text_beside(
		int align,
		int x,
		int y,
		int w,
		int h,
		long col,
		int style,
		int size,
		const char *str
		);

FL_EXPORT void flps_emit_header(
		const char *title,
		int n,
		int xi,
		int yi,
		int xf,
		int yf
		);

FL_EXPORT void flps_emit_prolog(
		void
		);

FL_EXPORT int flps_get_gray255(
		long color
		);

FL_EXPORT int flps_get_linestyle(
		void
		);

FL_EXPORT int flps_get_linewidth(
		void
		);

FL_EXPORT int flps_get_namedcolor(
		const char *s
		);

FL_EXPORT FLPS_CONTROL *flps_init(
		void
		);

FL_EXPORT void flps_invalidate_color_cache(
		void
		);

FL_EXPORT void flps_invalidate_font_cache(
		void
		);

FL_EXPORT void flps_invalidate_linewidth_cache(
		void
		);

FL_EXPORT void flps_invalidate_symbol_cache(
		void
		);

FL_EXPORT void flps_line(
		int xi,
		int yi,
		int xf,
		int yf,
		long col
		);

FL_EXPORT void flps_lines(
		FL_POINT *xp,
		int n,
		long col
		);

FL_EXPORT void flps_linestyle(
		int n
		);

FL_EXPORT void flps_linewidth(
		int lw
		);

FL_EXPORT void flps_log(
		const char *s
		);

FL_EXPORT void flps_output(
		const char *fmt,
		...
		);

FL_EXPORT void flps_oval(
		int fill,
		int x,
		int y,
		int w,
		int h,
		long col
		);

FL_EXPORT void flps_pieslice(
		int fill,
		int x,
		int y,
		int w,
		int h,
		int t1,
		int t2,
		long col
		);

FL_EXPORT void flps_poly(
		int fill,
		FL_POINT *xp,
		int n,
		long col
		);

FL_EXPORT void flps_rectangle(
		int fill,
		int x,
		int y,
		int w,
		int h,
		long col
		);

FL_EXPORT void flps_reset_cache(
		void
		);

FL_EXPORT void flps_reset_linewidth(
		void
		);

FL_EXPORT void flps_restore_flps(
		void
		);

FL_EXPORT void flps_rgbcolor(
		int r,
		int g,
		int b
		);

FL_EXPORT void flps_roundrectangle(
		int fill,
		int x,
		int y,
		int w,
		int h,
		long col
		);

FL_EXPORT void flps_set_clipping(
		int x,
		int y,
		int w,
		int h
		);

FL_EXPORT void flps_set_font(
		int style,
		int size
		);

FL_EXPORT void flps_unset_clipping(
		void
		);

#ifdef MAKING_FORMS
#include "flimage_int.h"
#endif

#if defined(__cplusplus)
}
#endif

#endif
