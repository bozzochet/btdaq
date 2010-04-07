// file xformslib.c
//
// My "Standard" Forms Library 
//
// A.Lebedev Oct-2003...

#include "xformslib.h"

int               c_good = 0, c_warning = 1, c_alarm = 2, c_error = 3;
int colors[4]  = {FL_GREEN,   FL_YELLOW,     FL_RED,      FL_BLACK};
int lcolors[4] = {FL_BLACK,   FL_BLACK,      FL_YELLOW,   FL_WHITE};

static int convert_size(int size);
static int convert_align(int align);
static void switch_btn_callback(FL_OBJECT *obj, long data);
static void hidden_btn_callback(FL_OBJECT* obj, long data);
static void update_btn_related_data(SW_BTN *B);

//~============================================================================

void set_object_color(FL_OBJECT *obj, int c1, int c2, int lc) {

  fl_set_object_color(obj, c1, c2);
  fl_set_object_lcolor(obj, lc);
}

//~----------------------------------------------------------------------------

void set_object_label(FL_OBJECT *obj, char *t, int c1, int c2, int lc) {

  if (t) fl_set_object_label(obj, t);
  set_object_color(obj, c1, c2, lc);
}

//~============================================================================

static int convert_size(int size) {

  if      (size == 'T') return FL_TINY_SIZE;
  else if (size == 'S') return FL_SMALL_SIZE;
  else if (size == 'N') return FL_NORMAL_SIZE;
  else if (size == 'M') return FL_MEDIUM_SIZE;
  else if (size == 'L') return FL_LARGE_SIZE;
  else if (size == 'H') return FL_HUGE_SIZE;
  return size;
}

//~----------------------------------------------------------------------------

static int convert_align(int align) {

  if      (align == 'L') return FL_ALIGN_LEFT   | FL_ALIGN_INSIDE;
  else if (align == 'C') return FL_ALIGN_CENTER | FL_ALIGN_INSIDE;
  else if (align == 'R') return FL_ALIGN_RIGHT  | FL_ALIGN_INSIDE;
  return align;
}

//~============================================================================

FL_OBJECT *add_plain_text(int x, int y, int w, int h, char *l, int ls) {

  FL_OBJECT *obj;
  
  obj = fl_add_text(FL_NORMAL_TEXT, x, y, w, h, l);
    fl_set_object_boxtype(obj, FL_NO_BOX);
    fl_set_object_color(obj, FL_BLACK, 0);
    fl_set_object_lsize(obj, convert_size(ls));
    fl_set_object_lalign(obj, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);

  return obj;
}

//~----------------------------------------------------------------------------

FL_OBJECT *add_frame_box(int x, int y, int w, int h, char *l, int ls) {

  FL_OBJECT *obj;
  
  obj = fl_add_text(FL_NORMAL_TEXT, x, y, w, h, l);
    fl_set_object_boxtype(obj, FL_EMBOSSED_BOX);
    fl_set_object_color(obj, FL_BLUE, 0);
    fl_set_object_lcolor(obj, FL_YELLOW);
    fl_set_object_lsize(obj, convert_size(ls));
    fl_set_object_lalign(obj, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
  
  return obj;
}

//~----------------------------------------------------------------------------

FL_OBJECT *add_data_box(int x, int y, int w, int h, char *l, int ls) {

  FL_OBJECT *obj;
  
  obj = fl_add_text(FL_NORMAL_TEXT, x, y, w, h, l);
    fl_set_object_boxtype(obj, FL_FRAME_BOX);
    fl_set_object_color(obj, FL_MCOL, 0);
    fl_set_object_lsize(obj, convert_size(ls));
    fl_set_object_lalign(obj, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj, FL_FIXED_STYLE + FL_BOLD_STYLE);
  
  return obj;
}

//~----------------------------------------------------------------------------


FL_OBJECT *add_message_box(
          int x, int y, int w, int h, char *l, int ls, int c, int lc) {

  FL_OBJECT *obj;

  obj = fl_add_text(FL_NORMAL_TEXT, x, y, w, h, l);
    fl_set_object_color(obj, c, 0);
    fl_set_object_lcolor(obj, lc);
    fl_set_object_lsize(obj, convert_size(ls));
    fl_set_object_lalign(obj, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj, FL_TIMESBOLDITALIC_STYLE + FL_ENGRAVED_STYLE);
    fl_hide_object(obj);
    
  return obj;
}

//~============================================================================

FL_OBJECT *add_normal_button(int x, int y, int w, int h, char *l, int ls) {

  FL_OBJECT *obj;
  
  obj = fl_add_button(FL_NORMAL_BUTTON, x, y, w, h, l);
    fl_set_object_color(obj, FL_CYAN, FL_RED);
    fl_set_object_lsize(obj, convert_size(ls));
    fl_set_object_lstyle(obj, FL_TIMES_STYLE + FL_EMBOSSED_STYLE);
  
  return obj;
}

//~----------------------------------------------------------------------------

void set_normal_button(FL_OBJECT *obj, int ctrl) {

  if (obj) {
    switch (ctrl) {
      case 0:
        fl_set_object_boxtype(obj, FL_UP_BOX);
        set_object_color(obj, FL_CYAN, FL_RED, FL_BLACK);
        fl_activate_object(obj);
      break;
      case 1:
        fl_set_object_boxtype(obj, FL_DOWN_BOX);
        set_object_color(obj, FL_RED, FL_RED, FL_YELLOW);
        fl_activate_object(obj);
      break;
      case 2:
        fl_set_object_boxtype(obj, FL_UP_BOX);
        set_object_color(obj, FL_COL1, FL_COL1, FL_COL1);
        fl_deactivate_object(obj);
      break;
    }
    if (ctrl) {
    }
    else {
    }
  }
}

//~----------------------------------------------------------------------------

FL_OBJECT *add_push_button(int x, int y, int w, int h, char *l, int ls) {

  FL_OBJECT *obj;
  
  obj = fl_add_button(FL_PUSH_BUTTON, x, y, w, h, l);
    fl_set_object_color(obj, FL_CYAN, FL_RED);
    fl_set_object_lsize(obj, convert_size(ls));
    fl_set_object_lstyle(obj, FL_TIMES_STYLE + FL_EMBOSSED_STYLE);
  
  return obj;
}

//~----------------------------------------------------------------------------

FL_OBJECT *add_switch_button(int x, int y, int w, int h, char *l, int ls) {

  FL_OBJECT *obj;
  
  obj = fl_add_button(FL_NORMAL_BUTTON, x, y, w, h, l);
    fl_set_object_boxtype(obj, FL_EMBOSSED_BOX);
    fl_set_object_color(obj, FL_WHEAT, FL_RED);
    fl_set_object_lsize(obj, convert_size(ls));
    fl_set_object_lstyle(obj, FL_FIXED_STYLE + FL_ENGRAVED_STYLE);
  
  return obj;
}

//~============================================================================

SW_BTN *add_switch_btn(
               int x, int y, int w, int h, char **l, int ls, int n, int i) {

  SW_BTN *B = malloc(sizeof(SW_BTN));
  FL_OBJECT *obj;
 
  B->obj = 
  obj = add_switch_button(x, y, w, h, "x", convert_size(ls));
    fl_set_object_callback(obj, switch_btn_callback, (long)B);
  B->new  = FALSE;
  B->d    = NULL;
  B->n    = n;
  B->l    = l;
  B->i    = i;
  B->p    = NULL;
  B->p10  = NULL;
  B->m    = 0xFFFFFFFF;
  B->m10  = 0;
  B->cnv  = NULL;
  B->cnv1 = NULL;
  B->cnv2 = NULL;
  B->call = NULL;
  
  if      (B->i < 0)   fl_set_object_label(B->obj, "?");
  else if (B->i > n-1) fl_set_object_label(B->obj, "??");
  else {
    if    (B->l)       fl_set_object_label(B->obj, B->l[B->i]);
    else               fl_set_object_label(B->obj, "-");
  }

  return B;
}

//~----------------------------------------------------------------------------

static void switch_btn_callback(FL_OBJECT *obj, long data) {

  SW_BTN *B = (SW_BTN*)data;
  int d = 0;
  int btn = fl_get_button_numb(obj);

  if (btn == FL_LEFT_MOUSE)  d =  1;
  if (btn == FL_RIGHT_MOUSE) d = -1;
  B->i = MOD(B->i+d, B->n);

  if (B->l) while (!B->l[B->i]) B->i = MOD(B->i+d, B->n);

  if (B->l) fl_set_object_label(B->obj, B->l[B->i]);

  update_btn_related_data(B);

  if (B->call) B->call(B->data);
}

//~----------------------------------------------------------------------------

void add_hidden_btn_zones(SW_BTN *B, ...) {

  va_list ap;
  FL_OBJECT *obj;
  int i, n;
  int x, y, w, h;
  bool array = FALSE;
  int *d = NULL;
  char t[80];

  fl_get_object_geometry(B->obj, &x, &y, &w, &h);

  va_start(ap, B);

  n = (int)va_arg(ap, int);

  if (ABS(n) > 10) {
    array = TRUE;
    d = (int*)n;
    n = d[0];
  }

  for (i=0; i<n; i++) {
    sprintf(t, "%d", array ? d[i+1] : (int)va_arg(ap, int));
    obj = fl_add_button(FL_HIDDEN_BUTTON, x+(n-1-i)*w/n, y, w/n, h, t);
      fl_set_object_callback(obj, hidden_btn_callback, (long)B);
  }

  va_end(ap);
}

//~----------------------------------------------------------------------------

static void hidden_btn_callback(FL_OBJECT* obj, long data) {

  SW_BTN *B = (SW_BTN*)data;
  int d = atoi(fl_get_object_label(obj));

  d = (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) ? -d : d;
  B->i = MOD(B->i+d, B->n);
  
  update_btn_related_data(B);

  if (B->call) B->call(B->data);
}

//~----------------------------------------------------------------------------

static void update_btn_related_data(SW_BTN *B) {

  int32 code, value;

  if (B->new) {
    B->p = ((_ww*)B->d)->inp + B->k;
    code  = unpack32(*B->p, B->m);
    if (B->cnv1) B->cnv1(code, &value);
    else         value = code;
    if (B->m10) pack10(&value, B->m10-1,             B->i);
    else        pack32(&value, unpack32(B->m, B->m), B->i);
    if (B->cnv2) B->cnv2(value, &code);
    else         code = value;
    pack32(B->p, B->m, code);
  }
  else {
    if (B->p)   pack32(B->p,   B->m, B->i);
    if (B->p10) pack10(B->p10, B->m, B->i);
  }
}

//~============================================================================

void invalidate_r(bool first, _r *r, int n) {

  r->unk = TRUE;
  r->sta = status_NA;
  if (first) r->was = malloc(n*4);
  if (first) r->rea = malloc(n*4);
}

//~----------------------------------------------------------------------------

void invalidate_w(_w *w, int n, int32 val) {

  int i;
  
  w->unk = TRUE;
  for (i=0; i<n; i++) w->inp = val;
}

//~----------------------------------------------------------------------------

void invalidate_ww(bool first,_ww *ww, int n, int32 val) {

  int i;
  
  ww->unk = TRUE;
  if (first) ww->inp = malloc(n*4);
  if (first) ww->wri = malloc(n*4);
  for (i=0; i<n; i++) ww->inp[i] = val;
}

//~============================================================================
//~============================================================================
//
//                  SMALL LIBRARY-2
//
//~============================================================================
//~============================================================================

void create_dat_box_string(bool H,
     int n, int x0, int y0, int w, int h, int s, _r *d, _b *b, DAT_BOX **B) {

  int i;
  int size = convert_size(s % 100);
  int jump = (s / 100) ? (s / 100) : 1000;

  for (i=0; i<n; i++) {
    int x = x0, y = y0;
    if (H) x = x0 + i * w + 5 * (i / jump);
    else   y = y0 + i * h + 5 * (i / jump);
    B[i] = malloc(sizeof(DAT_BOX));
    B[i]->obj = add_data_box(x, y, w, h, " ", size);
    B[i]->d = d;
    B[i]->k = b[i].ind;
    B[i]->m = b[i].msk;
    B[i]->n = b[i].n;
    B[i]->l = NULL;
    B[i]->cnv = NULL;
  }
}

//~----------------------------------------------------------------------------

void create_dat_box_vstring(
     int n, int x0, int y0, int w, int h, int s, _r *d, _b *b, DAT_BOX **B) {

  create_dat_box_string(0, n, x0, y0, w, h, s, d, b, B); 
}

//~----------------------------------------------------------------------------

void create_dat_box_hstring(
     int n, int x0, int y0, int w, int h, int s, _r *d, _b *b, DAT_BOX **B) {

  create_dat_box_string(1, n, x0, y0, w, h, s, d, b, B); 
}

//~----------------------------------------------------------------------------

void show_dat_box_string(int n, char **t, char *fmt, DAT_BOX **B) {

  FL_OBJECT *o;
  int i, c;
  char b[80];
  _r *d;

  for (i=0; i<n; i++) {
    if (!B[i]) continue;
    o = B[i]->obj;
    d = B[i]->d;
    if (d->unk) {
      sprintf(b, " ");
      c = c_warning;
    }
    else {
      int32 d_rea = unpack32((int32)d->rea[B[i]->k], B[i]->m);
      int32 d_was = unpack32((int32)d->was[B[i]->k], B[i]->m);
      if (B[i]->cnv) B[i]->cnv(d_rea, &d_rea);
      if (B[i]->cnv) B[i]->cnv(d_was, &d_was);
      if (FALSE) {
      }
      else if (t)   {
        if (d_rea < B[i]->n) sprintf(b, t[d_rea]);
        else                 sprintf(b, "?");
      }
      else if (fmt) {
        if (strstr(fmt, "f")) sprintf(b, fmt, *(float*)&d_rea);
        else                  sprintf(b, fmt,           d_rea);
      }
      else                    sprintf(b,      B[i]->l);
      if (d_was != d_rea) c = c_warning;
      else                c = c_good;
    }
    if (d->sta == status_NA) c = c_warning;
    if (d->sta == status_KO) c = c_error;
    set_object_label(o, b, colors[c], 0, lcolors[c]);
  }
}

//~----------------------------------------------------------------------------

void create_btn_string(bool H,
     int n, int x0, int y0, int w, int h, char **t, int s, _ww *d, _b *b, SW_BTN **B) {

  int i;
  int size = convert_size(s % 100);
  int jump = (s / 100) ? (s / 100) : 1000;

  for (i=0; i<n; i++) {
    int x = x0, y = y0;
    if (H) x = x0 + i * w + 5 * (i / jump);
    else   y = y0 + i * h + 5 * (i / jump);
    B[i] = 
    add_switch_btn(x, y, w, h, t, size, b[i].n, unpack32(d->inp[b[i].ind], b[i].msk));
    B[i]->new = TRUE;
    B[i]->d = d;
    B[i]->k = b[i].ind;
    B[i]->p = ((_ww*)B[i]->d)->inp + B[i]->k;
    B[i]->m = b[i].msk;
    B[i]->call = SW_BTN_CALLBACK;
  }
}

//~----------------------------------------------------------------------------

void create_btn_vstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _ww *d, _b *b, SW_BTN **B) {

  create_btn_string(0, n, x0, y0, w, h, t, s, d, b, B);
}

//~----------------------------------------------------------------------------

void create_btn_hstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _ww *d, _b *b, SW_BTN **B) {

  create_btn_string(1, n, x0, y0, w, h, t, s, d, b, B);
}

//~----------------------------------------------------------------------------

void create_btn10_vstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _ww *d, _b *b, SW_BTN **B) {

  int i;

  create_btn_string(0, n, x0, y0, w, h, t, s, d, b, B);
  for (i=0; i<n; i++) {
    B[i]->m10  = n - i;
  }
}

//~----------------------------------------------------------------------------

void create_btn10_hstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _ww *d, _b *b, SW_BTN **B) {

  int i;

  create_btn_string(1, n, x0, y0, w, h, t, s, d, b, B);
  for (i=0; i<n; i++) {
    B[i]->m10  = n - i;
  }
}

//~----------------------------------------------------------------------------

void show_btn_string(int n, char *fmt, SW_BTN **BTN) {

  SW_BTN *B;
  int i, c;
  _ww *d;
  
  for (i=0; i<ABS(n); i++) {
    int32 d_inp;
    int32 d_wri;
    int32 c_inp, v_inp;
    int32 c_wri, v_wri;
    int B_i_old;

    if (!(B = BTN[i])) continue;
    
    d = B->d;
    d_inp = d->inp[B->k];
    d_wri = d->wri[B->k];
    c_inp = unpack32(d_inp, B->m);

    if (B->cnv1) B->cnv1(c_inp, &v_inp);
    else         v_inp = c_inp;
    if (B->m10) B->i = unpack10(v_inp, B->m10-1);
    else        B->i = unpack32(v_inp, unpack32(B->m, B->m));

    c_wri = unpack32(d_wri, B->m);
    if (B->cnv1) B->cnv1(c_wri, &v_wri);
    else         v_wri = c_wri;
    if (B->m10) B_i_old = unpack10(v_wri, B->m10-1);
    else        B_i_old = unpack32(v_wri, unpack32(B->m, B->m));

    if (B->l) {
      fl_set_object_label(B->obj, B->l[B->i]);
    }
    else {
      if (B->cnv) B->cnv(c_inp, &v_inp);
      if (fmt) {
        char b[80];
        if (strstr(fmt, "f")) sprintf(b, fmt, *(float*)&v_inp);
        else                  sprintf(b, fmt,           v_inp);
        fl_set_object_label(B->obj, b);
      }
    }
    if (B_i_old != B->i) c = c_warning;
    else                 c = c_good;
    if (d->unk)          c = c_warning;
    set_object_label(B->obj, NULL, colors[c], FL_RED, lcolors[c]);
  }
}


//~============================================================================
//~============================================================================
//
//                  SMALL LIBRARY
//
//~============================================================================
//~============================================================================

void create_bit_num_string(int n, int f, int x0, int y0, int w, int h, int s) {

  int i;
  int size = convert_size(s % 100);
  int jump = (s / 100) ? (s / 100) : 1000;

  for (i=0; i<ABS(n); i++) {
    char b[80];
    if ( n > 0) sprintf(b, "%d", i+f);
    else        sprintf(b, "%d", f-i);
    add_plain_text(x0+i*w+5*(i/jump), y0, MAX(w,20), h, b, size);
  }
}

//~============================================================================

void create_frame_box_hstring(
     int n, int x0, int y0, int w, int h, char **t, int s, int a) {

  FL_OBJECT *obj;
  int i;
  int size = convert_size(s % 100);
  int jump = (s / 100) ? (s / 100) : 1000;

  for (i=0; i<n; i++) {
    obj = add_frame_box(x0+i*w+5*(i/jump), y0, w, h, t[i], size);
    fl_set_object_lalign(obj, convert_align(a));
  }
}

//~----------------------------------------------------------------------------

void create_frame_box_vstring(
     int n, int x0, int y0, int w, int h, char **t, int s, int a) {

  FL_OBJECT *obj;
  int i;
  int size = convert_size(s % 100);
  int jump = (s / 100) ? (s / 100) : 1000;

  for (i=0; i<n; i++) {
    obj = add_frame_box(x0, y0+i*h+5*(i/jump), w, h, t[i], size);
    fl_set_object_lalign(obj, convert_align(a));
  }
}

//~============================================================================

void create_data_box_hstring(
     int n, int x0, int y0, int w, int h, int s, FL_OBJECT **obj) {

  int i;
  int size = convert_size(s % 100);
  int jump = (s / 100) ? (s / 100) : 1000;

  for (i=0; i<n; i++) obj[i] = add_data_box(x0+i*w+5*(i/jump), y0, w, h, " ", size);
}

//~----------------------------------------------------------------------------

void create_data_box_vstring(
     int n, int x0, int y0, int w, int h, int s, FL_OBJECT **obj) {

  int i;
  int size = convert_size(s % 100);
  int jump = (s / 100) ? (s / 100) : 1000;

  for (i=0; i<n; i++) obj[i] = add_data_box(x0, y0+i*h+5*(i/jump), w, h, " ", size);
}

//~============================================================================

void create_bit_btn_hstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _w *d, SW_BTN **B) {

  int i;
  int size = convert_size(s % 100);
  int jump = (s / 100) ? (s / 100) : 1000;

  for (i=0; i<ABS(n); i++) {
    int32 m;
    if (n>0) m = 0x00000001 << (ABS(n) - 1 - i);
    else     m = 0x00000001 << i;
    B[i] = add_switch_btn(x0+i*w+5*(i/jump), y0, w, h, t, size, 2, unpack32(d->inp, m));
    B[i]->p = &d->inp;
    B[i]->m = m;
    B[i]->call = SW_BTN_CALLBACK;
  }
}

//~----------------------------------------------------------------------------

void create_bit_btn_vstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _w *d, SW_BTN **B) {

  int i;
  int size = convert_size(s % 100);
  int jump = (s / 100) ? (s / 100) : 1000;

  for (i=0; i<ABS(n); i++) {
    int32 m;
    if (n>0) m = 0x00000001 << (ABS(n) - 1 - i);
    else     m = 0x00000001 << i;
    B[i] = add_switch_btn(x0, y0+i*h+5*(i/jump), w, h, t, size, 2, unpack32(d->inp, m));
    B[i]->p = &d->inp;
    B[i]->m = m;
    B[i]->call = SW_BTN_CALLBACK;
  }
}

//~----------------------------------------------------------------------------

void create_bit_btn_hsstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _w *d, int f, SW_BTN **B) {

  int i;
  int size = convert_size(s % 100);
  int jump = (s / 100) ? (s / 100) : 1000;

  for (i=0; i<ABS(n); i++) {
    int k = i + f;
    int32 m;
    if (n>0) m = 0x00000001 << (ABS(n) - 1 - k);
    else     m = 0x00000001 << k;
    B[i] = add_switch_btn(x0+i*w+5*(i/jump), y0, w, h, t, size, 2, unpack32(d->inp, m));
    B[i]->p = &d->inp;
    B[i]->m = m;
    B[i]->call = SW_BTN_CALLBACK;
  }
}

//~----------------------------------------------------------------------------

void create_bit_btn_vsstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _w *d, int f, SW_BTN **B) {

  int i;
  int size = convert_size(s % 100);
  int jump = (s / 100) ? (s / 100) : 1000;

  for (i=0; i<ABS(n); i++) {
    int k = i + f;
    int32 m;
    if (n>0) m = 0x00000001 << (ABS(n) - 1 - k);
    else     m = 0x00000001 << k;
    B[i] = add_switch_btn(x0, y0+i*h+5*(i/jump), w, h, t, size, 2, unpack32(d->inp, m));
    B[i]->p = &d->inp;
    B[i]->m = m;
    B[i]->call = SW_BTN_CALLBACK;
  }
}

//~----------------------------------------------------------------------------

void create_hex_btn_hstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _w *d, SW_BTN **B) {

  int i;
  int size = convert_size(s % 100);
  int jump = (s / 100) ? (s / 100) : 1000;

  for (i=0; i<ABS(n); i++) {
    int32 m;
    if (n>0) m = 0x0000000F << (4 * (n - 1 - i));
    else     m = 0x0000000F << (4 * i);
    B[i] = add_switch_btn(x0+i*w+5*(i/jump), y0, w, h, t, size, 16, unpack32(d->inp, m));
    B[i]->p = &d->inp;
    B[i]->m = m;
    B[i]->call = SW_BTN_CALLBACK;
  }
}

//~----------------------------------------------------------------------------

void create_hex_btn_hsstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _w *d, int f, SW_BTN **B) {

  int i;
  int size = convert_size(s % 100);
  int jump = (s / 100) ? (s / 100) : 1000;

  for (i=0; i<ABS(n); i++) {
    int k = i + f;
    int32 m;
    if (n>0) m = 0x0000000F << (4 * (n - 1 - k));
    else     m = 0x0000000F << (4 * k);
    B[i] = add_switch_btn(x0+i*w+5*(i/jump), y0, w, h, t, size, 16, unpack32(d->inp, m));
    B[i]->p = &d->inp;
    B[i]->m = m;
    B[i]->call = SW_BTN_CALLBACK;
  }
}

//~----------------------------------------------------------------------------

void create_hex_btn_vstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _w *d, SW_BTN **B) {

  int i;
  int size = convert_size(s % 100);
  int jump = (s / 100) ? (s / 100) : 1000;

  for (i=0; i<ABS(n); i++) {
    int32 m;
    if (n>0) m = 0x0000000F << (4 * (n - 1 - i));
    else     m = 0x0000000F << (4 * i);
    B[i] = add_switch_btn(x0, y0+i*h+5*(i/jump), w, h, t, size, 16, unpack32(d->inp, m));
    B[i]->p = &d->inp;
    B[i]->m = m;
    B[i]->call = SW_BTN_CALLBACK;
  }
}

//~----------------------------------------------------------------------------

void create_hex_btn_vsstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _w *d, int f, SW_BTN **B) {

  int i;
  int size = convert_size(s % 100);
  int jump = (s / 100) ? (s / 100) : 1000;

  for (i=0; i<ABS(n); i++) {
    int k = i + f;
    int32 m;
    if (n>0) m = 0x0000000F << (4 * (n - 1 - k));
    else     m = 0x0000000F << (4 * k);
    B[i] = add_switch_btn(x0, y0+i*h+5*(i/jump), w, h, t, size, 16, unpack32(d->inp, m));
    B[i]->p = &d->inp;
    B[i]->m = m;
    B[i]->call = SW_BTN_CALLBACK;
  }
}

//~----------------------------------------------------------------------------

void create_dec_btn_hstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _w *d, SW_BTN **B) {

  int i;
  int size = convert_size(s % 100);
  int jump = (s / 100) ? (s / 100) : 1000;

  for (i=0; i<ABS(n); i++) {
    int m;
    if (n>0) m = n - 1 - i;
    else     m = i;
    B[i] = add_switch_btn(x0+i*w+5*(i/jump), y0, w, h, t, size, 10, unpack10(d->inp, m));
    B[i]->p10 = &d->inp;
    B[i]->m   = m;
    B[i]->call = SW_BTN_CALLBACK;
  }
}

//~----------------------------------------------------------------------------

void create_dec_btn_vstring(
     int n, int x0, int y0, int w, int h, char **t, int s, _w *d, SW_BTN **B) {

  int i;
  int size = convert_size(s % 100);
  int jump = (s / 100) ? (s / 100) : 1000;

  for (i=0; i<ABS(n); i++) {
    int m;
    if (n>0) m = n - 1 - i;
    else     m = i;
    B[i] = add_switch_btn(x0, y0+i*h+5*(i/jump), w, h, t, size, 10, unpack10(d->inp, m));
    B[i]->p10 = &d->inp;
    B[i]->m   = m;
    B[i]->call = SW_BTN_CALLBACK;
  }
}

//~----------------------------------------------------------------------------

void create_num_btn_hstring(
     int n, int x0, int y0, int w, int h, char **t, int s, int nn, _w *d, SW_BTN **B) {

  int i;
  int size = convert_size(s % 100);
  int jump = (s / 100) ? (s / 100) : 1000;

  for (i=0; i<n; i++) {
    int m = 0xFFFFFFFF;
    B[i] = add_switch_btn(x0+i*w+5*(i/jump), y0, w, h, t, size, nn, unpack32(d[i].inp, m));
    B[i]->p = &d[i].inp;
    B[i]->m = m;
    B[i]->call = SW_BTN_CALLBACK;
  }
}

//~----------------------------------------------------------------------------

void create_num_btn_vstring(
     int n, int x0, int y0, int w, int h, char **t, int s, int nn, _w *d, SW_BTN **B) {

  int i;
  int size = convert_size(s % 100);
  int jump = (s / 100) ? (s / 100) : 1000;

  for (i=0; i<n; i++) {
    int m = 0xFFFFFFFF;
    B[i] = add_switch_btn(x0, y0+i*h+5*(i/jump), w, h, t, size, nn, unpack32(d[i].inp, m));
    B[i]->p = &d[i].inp;
    B[i]->m = m;
    B[i]->call = SW_BTN_CALLBACK;
  }
}

//~============================================================================
//
//                    SHOW BOXES
//
//~============================================================================

void show_bit_box_string(int n, _r *d, char **txt, FL_OBJECT **obj) {

  int i, c;
  char b[80];
    
  for (i=0; i<ABS(n); i++) {
    FL_OBJECT *o = obj[i];
    if (d->unk) {
      sprintf(b, " ");
      c = c_warning;
    }
    else {
      int32 m = n > 0 ? 0x00000001 << (n - 1 - i) : 0x00000001 << i;
      int32 d_rea = unpack32((int32)*d->rea, m);
      int32 d_was = unpack32((int32)*d->was, m);
      if (txt) sprintf(b, txt[d_rea]);
      else     sprintf(b, "%d", d_rea);
      if (d_was != d_rea) c = c_warning;
      else                c = c_good;
    }
    if (d->sta == status_NA) c = c_warning;
    if (d->sta == status_KO) c = c_error;
    set_object_label(o, b, colors[c], 0, lcolors[c]);
  }
}

//~----------------------------------------------------------------------------

void show_bit_box_sstring(int n, _r *d, int f, char **txt, FL_OBJECT **obj) {

  int i, c;
  int ff  = f % 100;
  int fff = f / 100;
  char b[80];
    
  for (i=0; i<ABS(n); i++) {
    FL_OBJECT *o = obj[i];
    if (d->unk) {
      sprintf(b, " ");
      c = c_warning;
    }
    else {
      int k = i + fff;
      int32 m = n > 0 ? 0x00000001 << (n - 1 - k) : 0x00000001 << k;
      int32 d_rea = unpack32((int32)d->rea[ff], m);
      int32 d_was = unpack32((int32)d->was[ff], m);
      if (txt) sprintf(b, txt[d_rea]);
      else     sprintf(b, "%d", d_rea);
      if (d_was != d_rea) c = c_warning;
      else                c = c_good;
    }
    if (d->sta == status_NA) c = c_warning;
    if (d->sta == status_KO) c = c_error;
    set_object_label(o, b, colors[c], 0, lcolors[c]);
  }
}

//~----------------------------------------------------------------------------

void show_hex_box_string(int n, _r *d, char **txt, FL_OBJECT **obj) {

  int i, c;
  char b[80];
    
  for (i=0; i<ABS(n); i++) {
    FL_OBJECT *o = obj[i];
    if (d->unk) {
      sprintf(b, " ");
      c = c_warning;
    }
    else {
      int32 m = n > 0 ? 0x0000000F << (4 * (n - 1 - i)) : 0x0000000F << (4 * i);
      int32 d_rea = unpack32((int32)*d->rea, m);
      int32 d_was = unpack32((int32)*d->was, m);
      if (txt) sprintf(b, txt[d_rea]);
      else     sprintf(b, "%X", d_rea);
      if (d_was != d_rea) c = c_warning;
      else                c = c_good;
    }
    if (d->sta == status_NA) c = c_warning;
    if (d->sta == status_KO) c = c_error;
    set_object_label(o, b, colors[c], 0, lcolors[c]);
  }
}

//~----------------------------------------------------------------------------

void show_hex_box_sstring(int n, _r *d, int f, char **txt, FL_OBJECT **obj) {

  int i, c;
  int ff  = f % 100;
  int fff = f / 100;
  char b[80];
    
  for (i=0; i<ABS(n); i++) {
    FL_OBJECT *o = obj[i];
    if (d->unk) {
      sprintf(b, " ");
      c = c_warning;
    }
    else {
      int k = i + fff;
      int32 m = n > 0 ? 0x0000000F << (4 * (n - 1 - k)) : 0x0000000F << (4 * k);
      int32 d_rea = unpack32((int32)d->rea[ff], m);
      int32 d_was = unpack32((int32)d->was[ff], m);
      if (txt) sprintf(b, txt[d_rea]);
      else     sprintf(b, "%X", d_rea);
      if (d_was != d_rea) c = c_warning;
      else                c = c_good;
    }
    if (d->sta == status_NA) c = c_warning;
    if (d->sta == status_KO) c = c_error;
    set_object_label(o, b, colors[c], 0, lcolors[c]);
  }
}

//~----------------------------------------------------------------------------

void show_dec_box_string(int n, _r *d, char **txt, FL_OBJECT **obj) {

  int i, c;
  char b[80];
    
  for (i=0; i<ABS(n); i++) {
    FL_OBJECT *o = obj[i];
    if (d->unk) {
      sprintf(b, " ");
      c = c_warning;
    }
    else {
      int m = n > 0 ? n - 1 - i : i;
      int d_rea = unpack10(*d->rea, m);
      int d_was = unpack10(*d->was, m);
      if (txt) sprintf(b, txt[d_rea]);
      else     sprintf(b, "%d", d_rea);
      if (d_was != d_rea) c = c_warning;
      else                c = c_good;
    }
    if (d->sta == status_NA) c = c_warning;
    if (d->sta == status_KO) c = c_error;
    set_object_label(o, b, colors[c], 0, lcolors[c]);
  }
}

//~----------------------------------------------------------------------------

void show_num_box_string(int n, _r *d, char **txt, char *fmt, FL_OBJECT **obj) {

  int i, c;
  char b[80];
    
  for (i=0; i<ABS(n); i++) {
    FL_OBJECT *o = n > 0 ? obj[i] : obj[ABS(n)-1-i];
    if (d->unk) {
      sprintf(b, " ");
      c = c_warning;
    }
    else {
      if (txt) sprintf(b, txt[d->rea[i]]);
      else     sprintf(b, fmt, d->rea[i]);
      if (d->was[i] != d->rea[i]) c = c_warning;
      else                        c = c_good;
    }
    if (d->sta == status_NA) c = c_warning;
    if (d->sta == status_KO) c = c_error;
    set_object_label(o, b, colors[c], 0, lcolors[c]);
  }
}

//~----------------------------------------------------------------------------

void show_num_box(_r *d, int i, char **txt, char *fmt, FL_OBJECT *o) {

  int c;
  char b[80];

  if (d->unk) {
    sprintf(b, " ");
    c = c_warning;
  }
  else {
    if (txt) sprintf(b, txt[d->rea[i]]);
    else     sprintf(b, fmt, d->rea[i]);
    if (d->was[i] != d->rea[i]) c = c_warning;
    else                        c = c_good;
  }
  if (d->sta == status_NA) c = c_warning;
  if (d->sta == status_KO) c = c_error;
  set_object_label(o, b, colors[c], 0, lcolors[c]);
}

//~----------------------------------------------------------------------------

void show_float_num_box(_r *d, int i, char *fmt, FL_OBJECT *o) {

  int c;
  char b[80], b_was[80], b_rea[80];

  if (d->unk) {
    sprintf(b, " ");
    c = c_warning;
  }
  else {
    sprintf(b, fmt, *(float*)&d->rea[i]);
    sprintf(b_was, fmt, *(float*)&d->was[i]);
    sprintf(b_rea, fmt, *(float*)&d->rea[i]);
    if (strcmp(b_was, b_rea)) c = c_warning;
    else                      c = c_good;
  }
  if (d->unk) sprintf(b, " ");
  if (d->sta == status_NA) c = c_warning;
  if (d->sta == status_KO) c = c_error;
  set_object_label(o, b, colors[c], 0, lcolors[c]);
}

//~----------------------------------------------------------------------------

void show_msk_box(int32 m, _r *d, char **txt, FL_OBJECT *obj) {

  int c, lc;
  char b[80];
  int d_rea, d_was;
  d_rea = unpack32((int32)*d->rea, m);
  d_was = unpack32((int32)*d->was, m);

  if (d->unk) {
    sprintf(b, " ");
    c = FL_YELLOW;
  }
  else {
    if (txt) sprintf(b, txt[d_rea]);
    else     sprintf(b, "%d", d_rea);
    if (d_was != d_rea) c = FL_YELLOW;
    else                c = FL_GREEN;
  }
  lc = FL_BLACK;
  if (d->sta == status_NA) c = FL_YELLOW;
  if (d->sta == status_KO) {
    lc = FL_WHITE;
    c  = FL_BLACK;
  }
  set_object_label(obj, b, c, 0, lc);
}

//~----------------------------------------------------------------------------

void show_mask_box(int32 m, _r *d, char *txt, FL_OBJECT *obj) {

  int c, lc;
  char b[80];
  int d_rea, d_was;

  d_rea = *d->rea & m;
  d_was = *d->was & m;
  sprintf(b, txt);
  if (d->unk) {
    sprintf(b, " ");
    c = FL_YELLOW;
  }
  else {
    if (d_was != d_rea) c = FL_YELLOW;
    else                c = FL_GREEN;
  }
  lc = FL_BLACK;
  if (d->sta == status_NA) c = FL_YELLOW;
  if (d->sta == status_KO) {
    lc = FL_WHITE;
    c  = FL_BLACK;
  }
  set_object_label(obj, b, c, 0, lc);
}

//~============================================================================

void show_bin_btn_string(int n, _w *d, SW_BTN **BTN) {

  int i, c;
  
  for (i=0; i<ABS(n); i++) {
    SW_BTN *B = BTN[i];
    B->i = unpack32(d->inp, B->m);
    if (B->l) fl_set_object_label(B->obj, B->l[B->i]);
    if ((d->inp ^ d->wri) & B->m) c = FL_YELLOW;
    else                          c = FL_GREEN;
    if (d->unk)                   c = FL_YELLOW;
    fl_set_object_color(B->obj, c, FL_RED);
    fl_set_object_lcolor(B->obj, FL_BLACK);
  }
}

//~----------------------------------------------------------------------------

void show_dec_btn_string(int n, _w *d, SW_BTN **BTN) {

  int i, c;
  
  for (i=0; i<ABS(n); i++) {
    SW_BTN *B = BTN[i];
    B->i = unpack10(d->inp, B->m);
    if (B->l) fl_set_object_label(B->obj, B->l[B->i]);
    if (B->i != unpack10(d->wri, B->m)) c = FL_YELLOW;
    else                                c = FL_GREEN;
    if (d->unk)                         c = FL_YELLOW;
    fl_set_object_color(B->obj, c, FL_RED);
    fl_set_object_lcolor(B->obj, FL_BLACK);
  }
}

//~----------------------------------------------------------------------------

void show_msk_btn_string(int n, _w *d, SW_BTN **BTN) {

  int i, c;

  for (i=0; i<ABS(n); i++) {
    SW_BTN *B = BTN[i];
    B->i = ((d->inp & B->m) == B->m) ? 1 : 0;
    if (B->l) fl_set_object_label(B->obj, B->l[B->i]);
    if ((d->inp ^ d->wri) & B->m) c = FL_YELLOW;
    else                          c = FL_GREEN;
    if (d->unk)                   c = FL_YELLOW;
    fl_set_object_color(B->obj, c, FL_RED);
    fl_set_object_lcolor(B->obj, FL_BLACK);
  }
}

//~----------------------------------------------------------------------------

void show_num_btn_string(int n, _w *d, SW_BTN **BTN) {

  int i, c;

  for (i=0; i<ABS(n); i++) {
    SW_BTN *B = BTN[i];
    B->i = d->inp & B->m;
    if (B->l) fl_set_object_label(B->obj, B->l[B->i]);
    if ((d->inp ^ d->wri) & B->m) c = FL_YELLOW;
    else                          c = FL_GREEN;
    if (d->unk)                   c = FL_YELLOW;
    fl_set_object_color(B->obj, c, FL_RED);
    fl_set_object_lcolor(B->obj, FL_BLACK);
  }
}

//~============================================================================
//
// USCM & JMDC Address Selector for My Commanders & Controllers
//
// A.Lebedev Oct-2003...

int USCM_ID;                                      // global variables
int JMDC_ID;                                      // global variables
char *config_file_name;                           // global variables

static FL_OBJECT *USCM_DIGIT[2], *USCM_KIND;

static FL_OBJECT *JMDC_DIGIT;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void USCM_zero_callback(FL_OBJECT *obj, long data);
static void USCM_digit_callback(FL_OBJECT *obj, long data);
static void USCM_kind_callback(FL_OBJECT *obj, long data);

static void JMDC_zero_callback(FL_OBJECT *obj, long data);
static void JMDC_digit_callback(FL_OBJECT *obj, long data);

//~============================================================================

void create_USCM_JMDC_panels(void) {

  create_USCM_ID_panel(5, 5);
  create_JMDC_ID_panel(75, 5);
}

//~----------------------------------------------------------------------------

void update_USCM_JMDC_panels(void) {

  update_USCM_ID_panel();
  update_JMDC_ID_panel();
}

//~----------------------------------------------------------------------------

void update_config_file(void) {

  FILE *file;

  file = fopen(config_file_name, "w");
  if (file != NULL) {
    fwrite(&USCM_ID, sizeof(USCM_ID), 1, file);
    fwrite(&JMDC_ID, sizeof(JMDC_ID), 1, file);
    fclose(file);
  }
}

//~============================================================================

void create_USCM_ID_panel(int x0, int y0) {

  FL_OBJECT *obj;
  FILE *file;
  int x, y;
  
  file = fopen(config_file_name, "r");
  if (file != NULL) {
    fread(&USCM_ID, sizeof(USCM_ID), 1, file);
    fread(&JMDC_ID, sizeof(JMDC_ID), 1, file);
    fclose(file);
  }

  x = x0;
  y = y0;
  obj = fl_add_box(FL_DOWN_BOX,x,y,70,45,"");
  
  x = x0 + 5;
  y = y0;
  obj = add_plain_text(x, y, 60, 20, "USCM", FL_NORMAL_SIZE);
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, 55, 20, "");
    fl_set_object_callback(obj, USCM_zero_callback, 0);

  x = x0 + 5;
  y = y0 + 20;
  USCM_DIGIT[0] =
  obj = add_switch_button(x, y, 15, 20, "d", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, USCM_digit_callback, 0);

  x = x0 + 5 + 15;
  y = y0 + 20;
  USCM_DIGIT[1] =
  obj = add_switch_button(x, y, 15, 20, "d", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, USCM_digit_callback, 1);

 
  x = x0 + 5 + 15 + 15;
  y = y0 + 20;
  USCM_KIND =
  obj = add_switch_button(x, y, 30, 20, "n", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, USCM_kind_callback, 0);
}

//~----------------------------------------------------------------------------

static void USCM_zero_callback(FL_OBJECT *obj, long data) {

  if (USCM_ID & 0x0100) USCM_ID = 0x0100;
  else                  USCM_ID = 0x0000;

  update_config_file();
  update_USCM_ID_panel();
}

//~----------------------------------------------------------------------------

static void USCM_digit_callback(FL_OBJECT *obj, long data) {

  int d[2];
  int i = data;

  d[0] = unpack16(USCM_ID, 0x00F0);
  d[1] = unpack16(USCM_ID, 0x000F);
  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  d[i] = MOD(d[i]+1, 16);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) d[i] = MOD(d[i]-1, 16);

  USCM_ID = (USCM_ID & 0x0100) + 16 * d[0] + d[1];

  update_config_file();
  update_USCM_ID_panel();
}

//~----------------------------------------------------------------------------

static void USCM_kind_callback(FL_OBJECT *obj, long data) {

  USCM_ID ^= 0x0100;

  update_config_file();
  update_USCM_ID_panel();
}

//~----------------------------------------------------------------------------

void update_USCM_ID_panel(void) {

  char b[80];

  sprintf(b, "%01X", unpack16(USCM_ID, 0x00F0));
  fl_set_object_label(USCM_DIGIT[0], b);
  sprintf(b, "%01X", unpack16(USCM_ID, 0x000F));
  fl_set_object_label(USCM_DIGIT[1], b);

  if (USCM_ID & 0x0100) fl_set_object_label(USCM_KIND, "NB");
  else                  fl_set_object_label(USCM_KIND, "L");
}

//~============================================================================

void create_JMDC_ID_panel(int x0, int y0) {

  FL_OBJECT *obj;
  FILE *file;
  int x, y;
  
  file = fopen(config_file_name, "r");
  if (file != NULL) {
    fread(&USCM_ID, sizeof(USCM_ID), 1, file);
    fread(&JMDC_ID, sizeof(JMDC_ID), 1, file);
    fclose(file);
  }

  x = x0;
  y = y0;
  
  obj = fl_add_box(FL_DOWN_BOX,x,y,50,45,"");

  x = x0 + 5;
  y = y0;

  obj = add_plain_text(x, y, 40, 20, "JMDC", FL_NORMAL_SIZE);
  obj = fl_add_button(FL_HIDDEN_BUTTON, x, y, 40, 20, "");
    fl_set_object_callback(obj, JMDC_zero_callback, 0);

  x = x0 + 5;
  y = y0 + 20;
  
  JMDC_DIGIT =
  obj = add_switch_button(x, y, 40, 20, "", FL_MEDIUM_SIZE);
    fl_set_object_callback(obj, JMDC_digit_callback, 0);
}

//~----------------------------------------------------------------------------

static void JMDC_zero_callback(FL_OBJECT *obj, long data) {

  JMDC_ID = 0;

  update_config_file();
  update_JMDC_ID_panel();
}

//~----------------------------------------------------------------------------

static void JMDC_digit_callback(FL_OBJECT *obj, long data) {

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  JMDC_ID = MOD(JMDC_ID+1, 4);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) JMDC_ID = MOD(JMDC_ID-1, 4);

  update_config_file();
  update_JMDC_ID_panel();
}

//~----------------------------------------------------------------------------

void update_JMDC_ID_panel(void) {

  char b[80];

  sprintf(b, "%1d", JMDC_ID);
  fl_set_object_label(JMDC_DIGIT, b);
}

//~============================================================================
//
//
//            DEBUG PRINTING LEVEL SWITCH FOR MY COMMANDERS & CONTROLLERS
//
//
//~============================================================================

int P;                                     // global variables

static FL_OBJECT *P_BOX;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void P_callback(FL_OBJECT *obj, long data);

//~----------------------------------------------------------------------------

void create_P_panel(int x0, int y0) {

  FL_OBJECT *obj;
  int x, y;
  int w = 20, h = 20;
  
  x = x0;
  y = y0;
  
  obj = fl_add_box(FL_DOWN_BOX,x,y,w+10,2*h+10,"");
  
  x = x0 + 5;
  y = y0 + 5;
  add_frame_box(x, y, w, h, "P", FL_NORMAL_SIZE);

  y = y + h;
  P_BOX = 
  obj = add_switch_button(x, y, w, h, "?", FL_NORMAL_SIZE);
    fl_set_object_callback(obj, P_callback, 0);
}

//~----------------------------------------------------------------------------

void P_callback(FL_OBJECT *obj, long data) {

  if (fl_get_button_numb(obj) == FL_LEFT_MOUSE)  P = MOD(P+1, 10);
  if (fl_get_button_numb(obj) == FL_RIGHT_MOUSE) P = MOD(P-1, 10);

  update_P_panel();
}

//~----------------------------------------------------------------------------

void update_P_panel(void) {

  FL_OBJECT *obj = P_BOX;
  char b[80];
  
  if (!obj) return;

  sprintf(b, "%d", P);
  fl_set_object_label(obj, b);
}

//~============================================================================
