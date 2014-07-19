// file xformslib.h
//
// Include File for My "Standard" Forms Library
//
// A.Lebedev Oct-2003...

//~----------------------------------------------------------------------------

#ifndef _XFORMSLIB_H
#define _XFORMSLIB_H

#include "mylib.h"
#include "forms.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define status_NA -1
#define status_KO  0
#define status_OK  1

#define WVAL(w,b) (MIN((b).n, unpack32((w).inp[(b).ind], (b).msk)))
#define RVAL(r,b) (MIN((b).n, unpack32((r).rea[(b).ind], (b).msk)))

typedef struct {
  bool unk;
  int  sta;
  int32 *was;
  int32 *rea;
} _r;

typedef struct {
  bool unk;
  int  sta;
  int32 inp;
  int32 wri;
} _w;

typedef struct {
  bool   unk;
  int    sta;
  int32 *inp;
  int32 *wri;
} _ww;

typedef struct {
  int   ind;
  int32 msk;
  int   n;
} _b;

typedef struct {
  FL_OBJECT *obj;
  _r   *d;
  int   k;
  int32 m;
  int   n;
  char *l;
  void (*cnv)(int32 code, int32 *value);
} DAT_BOX;

typedef struct {
  FL_OBJECT *obj;
  bool new;
  char **l;
  int    n;
  int    i;
  void  *d;
  int32 *p;
  int32 *p10;
  int    k;
  int32  m;
  int    m10;
  void   (*cnv)(int32 code, int32 *value);
  void   (*cnv1)(int32 code, int32 *value);
  void   (*cnv2)(int32 value, int32 *code);
  void   (*call)(int32 data);
  int32 data;
} SW_BTN;

void (*SW_BTN_CALLBACK)(int32 data);

//~----------------------------------------------------------------------------

FL_OBJECT *add_plain_text(int x, int y, int w, int h, char *l, int ls);
FL_OBJECT *add_frame_box(int x, int y, int w, int h, char *l, int ls);
FL_OBJECT *add_data_box(int x, int y, int w, int h, char *l, int ls);
FL_OBJECT *add_message_box(
  int x, int y, int w, int h, char *l, int ls, int c, int lc);
FL_OBJECT *add_normal_button(int x, int y, int w, int h, char *l, int ls);
void      set_normal_button(FL_OBJECT *obj, int ctrl);
FL_OBJECT *add_push_button(int x, int y, int w, int h, char *l, int ls);
FL_OBJECT *add_switch_button(int x, int y, int w, int h, char *l, int ls);

SW_BTN    *add_switch_btn(
  int x, int y, int w, int h, char **l, int ls, int n, int i);
void      add_hidden_btn_zones(SW_BTN *B, ...);

//small library - 2

void set_object_color(FL_OBJECT *obj, int c1, int c2, int lc);
void set_object_label(FL_OBJECT *obj, char *t, int c1, int c2, int lc);

void invalidate_r(bool first, _r *r, int n);
void invalidate_w(_w *w, int n, int32 val);
void invalidate_ww(bool first,_ww *ww, int n, int32 val);

void create_dat_box_string(bool H,
     int n, int x0, int y0, int w, int h, int s, _r *d, _b *b, DAT_BOX **B);
void create_dat_box_vstring(
     int n, int x0, int y0, int w, int h, int s, _r *d, _b *b, DAT_BOX **B);
void create_dat_box_hstring(
     int n, int x0, int y0, int w, int h, int s, _r *d, _b *b, DAT_BOX **B);

void show_dat_box_string(int n, char **t, char *fmt, DAT_BOX **B);

void create_btn_string(bool H,
     int n, int x0, int y0, int w, int h, char **t, int s, _ww *d, _b *b, SW_BTN **B);
void create_btn_vstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _ww *d, _b *b, SW_BTN **B);
void create_btn_hstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _ww *d, _b *b, SW_BTN **B);
void create_btn10_vstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _ww *d, _b *b, SW_BTN **B);
void create_btn10_hstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _ww *d, _b *b, SW_BTN **B);

void show_btn_string(int n, char *fmt, SW_BTN **BTN);

//small library

void create_bit_num_string(int n, int f, int x0, int y0, int w, int h, int s);

void create_frame_box_hstring(
     int n, int x0, int y0, int w, int h, char **t, int s, int a);
void create_frame_box_vstring(
     int n, int x0, int y0, int w, int h, char **t, int s, int a);

void create_data_box_hstring(
     int n, int x0, int y0, int w, int h, int s, FL_OBJECT **obj);
void create_data_box_vstring(
     int n, int x0, int y0, int w, int h, int s, FL_OBJECT **obj);

void create_bit_btn_hstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _w *d, SW_BTN **B);
void create_bit_btn_vstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _w *d, SW_BTN **B);
void create_bit_btn_hsstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _w *d, int f, SW_BTN **B);
void create_bit_btn_vsstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _w *d, int f, SW_BTN **B);
void create_hex_btn_hstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _w *d, SW_BTN **B);
void create_hex_btn_vstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _w *d, SW_BTN **B);
void create_hex_btn_hsstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _w *d, int f, SW_BTN **B);
void create_hex_btn_vsstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _w *d, int f, SW_BTN **B);
void create_dec_btn_hstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _w *d, SW_BTN **B);
void create_dec_btn_vstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _w *d, SW_BTN **B);
void create_num_btn_hstring(
     int n, int x0, int y0, int w, int h, char **t, int s, int nn, _w *d, SW_BTN **B);
void create_num_btn_vstring(
     int n, int x0, int y0, int w, int h, char **t, int s, int nn, _w *d, SW_BTN **B);

void show_bit_box_string(int n, _r *d, char **t, FL_OBJECT **obj);
void show_bit_box_sstring(int n, _r *d, int f, char **txt, FL_OBJECT **obj);
void show_hex_box_string(int n, _r *d, char **t, FL_OBJECT **obj);
void show_hex_box_sstring(int n, _r *d, int f, char **txt, FL_OBJECT **obj);
void show_dec_box_string(int n, _r *d, char **t, FL_OBJECT **obj);
void show_num_box_string(int n, _r *d, char **t, char *fmt, FL_OBJECT **obj);
void show_num_box(_r *d, int i, char **txt, char *fmt, FL_OBJECT *obj);
void show_float_num_box(_r *d, int i, char *fmt, FL_OBJECT *obj);
void show_msk_box(int32 m, _r *d, char **txt, FL_OBJECT *obj);
void show_mask_box(int32 m, _r *d, char *txt, FL_OBJECT *obj);

void show_bin_btn_string(int n, _w *d, SW_BTN **BTN);
void show_dec_btn_string(int n, _w *d, SW_BTN **BTN);
void show_msk_btn_string(int n, _w *d, SW_BTN **BTN);
void show_num_btn_string(int n, _w *d, SW_BTN **BTN);


void update_config_file(void);

void create_USCM_ID_panel(int x0, int y0);
void update_USCM_ID_panel(void);

void create_JMDC_ID_panel(int x0, int y0);
void update_JMDC_ID_panel(void);

void create_USCM_JMDC_panels(void);
void update_USCM_JMDC_panels(void);

void create_P_panel(int x0, int y0);
void update_P_panel(void);

//~============================================================================

#endif // _XFORMSLIB_H
