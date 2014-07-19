//  $Id: xform_proc_10comp.h,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
/* Making X-forms for monitoring Offline processes */

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <forms.h>
#include <math.h>
#include <unistd.h>

// Definitions for X-form
#define X_size  1040
#define Y_size  790
#define Y_head  83
#define X_headl 50
#define X_clock 80
#define N_comp  10 /*number of computers */

#define N 3  /* number of processes */

char a_name[80], temp_name[80];
int Y_line, Y_current, ind=0, i_log[N_comp*2], ind_l=-1;
time_t ti,  N_lim_log_length;

typedef char ch80[80];
typedef char ch256[256];

ch80 names[50][N_comp], host_name[N_comp];

typedef FL_OBJECT *Obje;
typedef FL_FORM *Forms;

FL_FORM *form, *spare_f;
Forms log_f[14];
FL_OBJECT *quit, *help, *but, *b_open_log[N_comp], *b_open_elog[N_comp], *heade;
Obje check_[10][N_comp+1], numb_[20][N_comp+1], spare[N_comp+1], ind_[2], 
  stop_h[N_comp*2], stati_[N_comp+2], log_check[N_comp];

  /*-------------------------------------*/


FL_OBJECT * add_check_field(int i, int j, int X, int Y, int width, int heigth, const char * label) {
  FL_OBJECT *obj, *obj_buf;
  obj_buf = fl_add_box(FL_FRAME_BOX,X,Y,width,heigth,"");
  spare[j] = fl_add_box(FL_FLAT_BOX,X+2,Y+2,width-150,heigth-4,label);
  fl_set_object_color(spare[j],FL_TOP_BCOL,FL_TOP_BCOL);
  fl_set_object_lsize(spare[j],FL_MEDIUM_SIZE);
  numb_[i][j]=fl_add_box(FL_DOWN_BOX,X+width-150,Y+2,115,heigth-4,"0");
  fl_set_object_color(numb_[i][j],FL_WHITE,FL_WHITE);
  obj = fl_add_box(FL_OVAL_BOX,(X+width-35),Y+5,30,30,"");
  fl_set_object_color(obj,FL_GREEN,FL_GREEN);

  return(obj);
}

/*---------------------------------*/

FL_OBJECT * add_comp_box(int i, int j, int X, int Y, int width, int heigth, const char * label) {
  FL_OBJECT *obj_buf;
  /* add box */
  obj_buf=fl_add_box(FL_EMBOSSED_BOX,X,Y,width,heigth,label);
  fl_set_object_lalign(obj_buf, FL_ALIGN_INSIDE|FL_ALIGN_TOP);
  fl_set_object_lstyle(obj_buf, FL_BOLDITALIC_STYLE);
  fl_set_object_lsize(obj_buf,FL_MEDIUM_SIZE);
  fl_set_object_bw(obj_buf,6);

  check_[0][j]= add_check_field(0,j,X+10,Y+20,320,40,"gbatch.exe");
  check_[1][j]= add_check_field(1,j,X+10,Y+60,320,40,"used disk space");
  check_[2][j]= add_check_field(2,j,X+10,Y+100,320,40,"run");

  b_open_log[j] =fl_add_button(FL_NORMAL_BUTTON,X+10,Y+140,100,25,"log");
  b_open_elog[j]=fl_add_button(FL_NORMAL_BUTTON,X+width-110,Y+140,100,25,"err log");
  log_check[j]=fl_add_box(FL_FLAT_BOX,X+115,Y+140,width-225,25,"");
  fl_set_object_lstyle(log_check[j], FL_BOLDITALIC_STYLE);
  fl_set_object_lsize(log_check[j],FL_MEDIUM_SIZE);
  fl_set_object_lcol(log_check[j],FL_RED);
  return(obj_buf);

}

FL_OBJECT * add_stat(int j, int X,int Y, int width, int heigth, const char *label) {
FL_OBJECT *obj_buf, *obj;
int shif=255;

  obj_buf = fl_add_box(FL_FRAME_BOX,X,Y,width,heigth,"");
  obj_buf = fl_add_box(FL_FLAT_BOX,X+2,Y+2,width-shif,heigth-4,label);
  fl_set_object_color(obj_buf,FL_TOP_BCOL,FL_TOP_BCOL);
  fl_set_object_lsize(obj_buf,FL_MEDIUM_SIZE);
  if (j<N_comp+1) 
    obj=fl_add_box(FL_FLAT_BOX,X+width-shif,Y+2,shif-5,heigth-4,"----");
  else 
    obj=fl_add_box(FL_FLAT_BOX,X+width-shif,Y+2,shif-5,heigth-4,
                   "events(expect/proc)     el.t      cpu t    rate(real/cpu)");
  fl_set_object_color(obj,FL_WHITE,FL_WHITE);

  return(obj);

}

FL_OBJECT * add_stat_box(int X, int Y, int width, int heigth, const char * label) {
FL_OBJECT *obj_buf;
int Y_c, Y_step=22, i;

  obj_buf=fl_add_box(FL_EMBOSSED_BOX,X,Y,width,heigth,label);
  fl_set_object_lalign(obj_buf, FL_ALIGN_INSIDE|FL_ALIGN_TOP);
  fl_set_object_lstyle(obj_buf, FL_BOLDITALIC_STYLE);
  fl_set_object_lsize(obj_buf,FL_MEDIUM_SIZE);
  fl_set_object_bw(obj_buf,6);
  fl_set_object_color(obj_buf,FL_TOP_BCOL,FL_TOP_BCOL);
  stati_[N_comp+1] = add_stat(N_comp+1,X+10,Y+20,320,Y_step,"host");
  Y_c=Y+20+Y_step;
  for (i=0; i<N_comp; i++) {
    stati_[i] = add_stat(i,X+10,Y_c,320,Y_step,host_name[i]);
    Y_c += Y_step;
  }
  stati_[N_comp] = add_stat(N_comp,X+10,Y_c,320,Y_step,"sum");

  return (obj_buf);
}

/*-----------------------------------*/
int Xf_init(void) {
  FL_OBJECT *obj_buf;
  char *cti;

  Y_line = Y_head+3;

  form = fl_bgn_form(FL_FLAT_BOX,X_size,Y_size);

 /*HeaD OF form */
  ti=time(NULL);
  cti=ctime(&ti);
  heade = fl_add_box(FL_FLAT_BOX,X_headl, 0, X_size-X_headl, Y_head,cti);
  fl_set_object_color(heade, FL_WHEAT, FL_WHEAT);
  fl_set_object_lsize(heade,FL_MEDIUM_SIZE);
  fl_set_object_lalign(heade,FL_ALIGN_CENTER);
  /*Add indicator */
  ind_[0]=fl_add_box(FL_OVAL_BOX,X_headl+10,Y_line-15,7,7,"");
  fl_set_object_color(ind_[0],FL_RED,FL_RED);
  ind_[1]=fl_add_box(FL_OVAL_BOX,X_headl+30,Y_line-15,7,7,"");
  fl_set_object_color(ind_[1],FL_GREEN,FL_GREEN);
  /* Add line */
  obj_buf=fl_add_box(FL_UP_BOX,0,(Y_line-3),X_size,3,"");
  fl_set_object_color(obj_buf, FL_BLACK, FL_BLACK);
  /* Add QUIT button */
  quit = fl_add_button(FL_NORMAL_BUTTON,0,0,X_headl,2*Y_head/3,"QUIT");
  fl_set_object_color(quit,FL_RED,FL_RED);
  help = fl_add_button(FL_NORMAL_BUTTON,0,2*Y_head/3,X_headl,Y_head/3,"HELP");
  fl_set_object_color(help,FL_GREEN,FL_GREEN);
  /* Add coo Check */
  Y_current =Y_line+10;
  /*Add boxes */

  add_comp_box(0,0,5,Y_current,340,170,host_name[0]);
  add_comp_box(0,1,350,Y_current,340,170,host_name[1]);
  add_comp_box(0,2,695,Y_current,340,170,host_name[2]);
  Y_current += 170;
  add_comp_box(0,3,5,Y_current,340,170,host_name[3]);
  add_comp_box(0,4,350,Y_current,340,170,host_name[4]);
  add_comp_box(0,5,695,Y_current,340,170,host_name[5]);
  Y_current += 170;
  add_comp_box(0,6,5,Y_current,340,170,host_name[6]);
  add_comp_box(0,7,350,Y_current,340,170,host_name[7]);
  add_stat_box(695,Y_current,340,300,"statistics");
  Y_current +=170;
  add_comp_box(0,8,5,Y_current,340,170,host_name[8]);
  add_comp_box(0,9,350,Y_current,340,170,host_name[9]);
  Y_current +=170;

  check_[1][N_comp]= add_check_field(1,N_comp,710,Y_current-30,320,40,"DB disk used");

  fl_end_form();

  return(0);

}

/*-------------------*/
int add_sp(int j) {
  FL_OBJECT *obj_buf, *but_buf, *b;
  const char *chbuf;
  
  spare_f = fl_bgn_form(FL_FLAT_BOX, 420, 50);
  obj_buf = fl_add_input(FL_FLAT_BOX,100,5,200,40,"Input process");
  but_buf = fl_add_button(FL_NORMAL_BUTTON,320,5,40,40,"GO");
  fl_end_form();
 
  fl_show_form(spare_f,FL_PLACE_MOUSE,FL_FULLBORDER, "Add process");

  while (1) {
  if ((b=fl_check_only_forms()) == but_buf) {
    chbuf=fl_get_input(obj_buf);
    sprintf(names[N][j],"%s",chbuf);
    fl_hide_form(spare_f);
    if (j<2)
      fl_set_object_label(spare[j],names[N][j]);
    else 
      fl_set_object_label(spare[j],names[0][j]);
    return(0);
  }
  }
}



int log_(int j, const char *label) {
  FL_OBJECT *obj_buf, *b;
  struct stat f_s;
  int count, X_s=550, Y_s=600;

  log_f[ind_l] = fl_bgn_form(FL_FLAT_BOX, X_s, Y_s);
  stop_h[ind_l] = fl_add_button(FL_NORMAL_BUTTON,0,(Y_s-33),X_s,33,"H I D E    t h i s    f o r m ");
  obj_buf = fl_add_browser(FL_NORMAL_BROWSER,0,0,X_s,(Y_s-33),"");
  fl_set_object_color(obj_buf,FL_TOP_BCOL,FL_TOP_BCOL);
  fl_set_browser_fontsize(obj_buf,FL_NORMAL_SIZE);
  fl_set_browser_fontstyle(obj_buf,FL_FIXED_STYLE);
  fl_clear_browser(obj_buf);
  count = stat(temp_name,&f_s);
  if (count < 0) {
    fl_add_browser_line(obj_buf,"\n\nfile cannot be found");
  }
  else {
    fl_load_browser(obj_buf,temp_name);
  }
  fl_end_form();
  fl_show_form(log_f[ind_l],FL_PLACE_FREE,FL_FULLBORDER, label);
  i_log[ind_l]=1;
 
  if (((b=fl_check_only_forms()) == stop_h[ind_l]) || (b == quit)) {
    i_log[ind_l]=0;
    fl_hide_form(log_f[ind_l]);
  }
  return(0);
}

