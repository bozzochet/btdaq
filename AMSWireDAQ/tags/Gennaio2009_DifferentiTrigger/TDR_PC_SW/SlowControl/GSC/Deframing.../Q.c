// file Q.c
//
// reads and displays the list of commands stored in MCC computer (q-file).

#include "forms.h"
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define TIMER_PERIOD 1.0	// secs

typedef struct {
	FL_FORM *QUEU;
	void *vdata;
	long ldata;
} FD_QUEU;

FD_QUEU *QUEU;

FL_OBJECT *LIST_browser;
FL_OBJECT *TIME_text;
FL_OBJECT *MESSAGE_text;
FL_OBJECT *GET_THEM_button;
FL_OBJECT *FMT_button;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *Command_name[3] = { "GET", "SET", "EXE"};

#define NB_OF_SYNONIMS MAX_NB_OF_SYNONIMS
char *Object_name[128][64], *Dataset_name[128][64];

#define integer16 unsigned short
#define integer32 unsigned int

integer16 block_length;
integer16 block[32768];
integer32 block_time;

integer16 *command_block;
integer16 command_block_length;
integer16 command_id;
#define block_ID       command_block[0]
#define command_ID     command_block[1]
integer32 command_time;
int fmt;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void TIMER_callback( FL_OBJECT *, long);
void GET_THEM_button_callback( FL_OBJECT *, long);
void FMT_button_callback( FL_OBJECT *, long);
int get_MCC_command_queu( void);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

FD_QUEU *create_form_QUEU(void) {

  FL_OBJECT *obj;
  int i, j, k, x, y;
  
  FD_QUEU *fdui = (FD_QUEU *) fl_calloc(1, sizeof(*fdui));
//
  fdui->QUEU = fl_bgn_form(FL_NO_BOX, 480, 680);
  obj = fl_add_box(FL_FLAT_BOX,0,0,480,680,"");
//
  obj = fl_add_timer(FL_HIDDEN_TIMER,120,0,100,30,"");
    fl_set_object_color(obj,FL_WHEAT,FL_RED);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_timer(obj,TIMER_PERIOD);
    fl_set_object_callback(obj,TIMER_callback,0);
//
  obj = fl_add_text(FL_NORMAL_TEXT,10,0,460,25,"List of commands stored in MCC:");
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE+FL_EMBOSSED_STYLE);
  TIME_text = obj =
    fl_add_text(FL_NORMAL_TEXT,40,30,400,30,"");
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
  LIST_browser = obj =  
    fl_add_browser(FL_MULTI_BROWSER,10,50,460,580,"");
    fl_set_object_color(obj,FL_WHEAT,FL_DARKORANGE);
    fl_set_browser_fontstyle(obj,FL_FIXEDBOLD_STYLE);
    fl_set_browser_fontsize(obj,FL_NORMAL_SIZE);
  FMT_button = obj =
    fl_add_checkbutton(FL_PUSH_BUTTON,10,0,50,50,"");
    fl_set_object_color(obj,FL_MAGENTA,FL_DODGERBLUE);
    fl_set_object_callback(obj,FMT_button_callback,0);
//
  MESSAGE_text = obj = 
    fl_add_text(FL_NORMAL_TEXT,10,640,340,30,"");
    fl_set_object_color(obj,FL_SPRINGGREEN,FL_MCOL);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_ENGRAVED_STYLE);
//
  GET_THEM_button = obj =
    fl_add_button(FL_NORMAL_BUTTON,360,640,110,30,"GET THEM");
    fl_set_object_color(obj,FL_CYAN,FL_RED);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lstyle(obj,FL_TIMES_STYLE+FL_ENGRAVED_STYLE);
    fl_set_object_callback(obj,GET_THEM_button_callback,0);
    fl_hide_object(obj);
//
  fl_end_form();

  fdui->QUEU->fdui = fdui;

  return fdui;
}
//-------------------------------------------------------------------
void TIMER_callback( FL_OBJECT *obj, long data) {
  FILE *file;
  static int counter = 0;
  integer16 bl_length;
  integer16 bl[32768];
  integer32 bl_time;
  fl_set_timer( obj, TIMER_PERIOD);
  if (counter) fl_set_object_lstyle( MESSAGE_text, FL_BOLD_STYLE   + FL_ENGRAVED_STYLE);
  else         fl_set_object_lstyle( MESSAGE_text, FL_NORMAL_STYLE + FL_ENGRAVED_STYLE);
  counter = ~counter;
  file = fopen( "/stage/STATUS/MCC.LIST", "r");
  fread( &bl_length, sizeof( integer16), 1, file);
  fread( bl, sizeof( integer16), bl_length, file);
  fclose( file);
  bl_time = (bl[2] << 16) | (bl[3] & 0xFFFF);
  if (bl_time != block_time) {
    fl_set_object_label( MESSAGE_text, "NEW DATA ARRIVED!");
    fl_show_object( GET_THEM_button);
  }
  else {
    fl_set_object_label( MESSAGE_text, "working hard...");
    fl_hide_object( GET_THEM_button);
  }
}
//-------------------------------------------------------------------
void GET_THEM_button_callback( FL_OBJECT *obj, long data) {
  get_MCC_command_queu();
}
//-------------------------------------------------------------------
void FMT_button_callback( FL_OBJECT *obj, long data) {
  fmt = fl_get_button( obj);
  list_MCC_command_queu();
}
//-------------------------------------------------------------------
int list_MCC_command_queu( void) {
  int block_type, node_ID, data_type;
  char *command_name, *object_name, *dataset_name;
  int command_priority;
  char text[100];
  char ascii_time[26];
  int p;
  block_time = (block[2] << 16) | (block[3] & 0xFFFF);
  command_block = block + 5;
  fl_freeze_form( QUEU->QUEU);
  fl_clear_browser( LIST_browser);
  strcpy( ascii_time, (char *)(ctime(&block_time)+4));
  ascii_time[15] = '\0';
  sprintf( text, "(as of %s)\n", ascii_time);
  fl_set_object_label( TIME_text, text);
  while (command_block-block <= block_length) {
    command_block_length = command_block[-1];
    command_priority = command_ID >> 12;
    command_id = command_ID & 0x0FFF;
    command_time = (command_block[2] << 16) | (command_block[3] & 0xFFFF);
//command_time = time(NULL);
//block_ID = rand();
    if (decode_AMS_block_ID( block_ID, &command_name, &object_name, &dataset_name)) {
      if (fmt) {
        p = sprintf( text, "%3X %3s %-6s %-9s priority %2d", 
            command_id, command_name, object_name, dataset_name, command_priority);
      }
      else {
        p = sprintf( text, "%3X %3s %s %s priority %d", 
            command_id, command_name, object_name, dataset_name, command_priority);
      }
      if (command_time) {
        struct tm *tm_ptr;
        tm_ptr = gmtime( &command_time);
        sprintf( text+p, " NASA %3.3d:%2.2d:%2.2d:%2.2d", 
            tm_ptr->tm_yday+1,
            tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec);
      }
    }
    else {
      if (fmt) {
        p = sprintf( text, "%3X ??? ?????? ????????? priority %2d", command_id, command_priority);
      }
      else {
        p = sprintf( text, "%3X ??? ???? ?????? priority %d", command_id, command_priority);
      }
      if (command_time) {
        struct tm *tm_ptr;
        tm_ptr = gmtime( &command_time);
        sprintf( text+p, " NASA %3.3d:%2.2d:%2.2d:%2.2d", 
            tm_ptr->tm_yday+1,
            tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec);
      }
    }
    fl_addto_browser( LIST_browser, text);
    command_block = command_block + command_block_length + 1;
  }
  fl_unfreeze_form( QUEU->QUEU);
}
//-------------------------------------------------------------------
int get_MCC_command_queu( void) {
  FILE *file;
  file = fopen( "/stage/STATUS/MCC.LIST", "r");
  fread( &block_length, sizeof( integer16), 1, file);
  fread( block, sizeof( integer16), block_length, file);
  fclose( file);
  list_MCC_command_queu();
}
//-------------------------------------------------------------------

void main( int argc, char *argv[]) {
  setbuf( stdout, NULL);	// set "no buffering" for stdout stream
  fl_initialize( &argc, argv, " ", 0, 0);
  QUEU = create_form_QUEU();
  get_MCC_command_queu();
  fl_set_form_position( QUEU->QUEU, 0, 0);
  fl_show_form( QUEU->QUEU, FL_PLACE_GEOMETRY, FL_FULLBORDER, "Q");
  while(1) {
    fl_do_forms();
  }
}

//-------------------------------------------------------------------
