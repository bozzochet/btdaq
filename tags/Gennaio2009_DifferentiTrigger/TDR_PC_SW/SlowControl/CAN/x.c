// file x.c
//
// Reanimation of AMS-01 program x
//
// A.Lebedev Oct-2008...


// this is X-forms/X-Windows AMS commanding program

#include "mylib.h"
#include "AMS_commanding_language.h"
#include "x.h"

char command_file_directory[N] = "";	// better keep it = ""
char datafile_directory[N] = "";	// better keep it = ""
char master_datafile_directory[N] = "";
char log_file_directory[N] = "/usr0/users/lebedev/AMS/Commanding../commands/";
char log_file_name[N] = "log_file";

#define NB_OF_SYNONIMS MAX_NB_OF_SYNONIMS  // so you restrict synonim usage

int strequ( char *a, char *b);
int search( TABLE *table, char *name, int command_mode, char *parent);

//-------------------------------------------------------------------------------------

int send_this_damned_command(void) {

  return OK;
}

//-------------------------------------------------------------------------------------

FD_F1 *create_form_F1(void) {

  FL_OBJECT *obj, *group;
  FD_F1 *fdui = (FD_F1 *) fl_calloc(1, sizeof(*fdui));
  
  int i, k, n;

  fdui->F1 = fl_bgn_form(FL_UP_BOX, 1070, 810);
  
  obj = fl_add_clock(FL_ANALOG_CLOCK,40,240,100,110,"TIME");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_color(obj,FL_BLUE,FL_YELLOW);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE+FL_EMBOSSED_STYLE);
    
  obj = fl_add_clock(FL_DIGITAL_CLOCK,40,350,100,30,"");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_color(obj,FL_BLUE,FL_YELLOW);
    
  fdui->CMD_group = group = fl_bgn_group();   //----------------------------------

  obj = fl_add_text(FL_NORMAL_TEXT,40,390,120,50,"Select\nCommand");
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    
  fdui->CMD_browser = obj = fl_add_browser(FL_SELECT_BROWSER,40,440,120,170,"");
    fl_set_object_color(  obj,FL_CYAN,FL_WHEAT);
    fl_set_object_lsize(  obj,FL_LARGE_SIZE);
    fl_set_object_lalign( obj,FL_ALIGN_TOP);
    fl_set_object_lstyle( obj,FL_TIMESBOLD_STYLE);  
    fl_set_browser_fontstyle(  obj, FL_TIMESBOLD_STYLE);
    fl_set_browser_fontsize(   obj, FL_LARGE_SIZE);
    fl_set_object_callback( obj, CMD_browser_callback, 0);

  fdui->CMD_input = obj = fl_add_input(FL_NORMAL_INPUT,40,620,120,30,"COMMAND");
    fl_set_object_color(  obj, FL_YELLOW, FL_WHEAT);
    fl_set_object_lsize(  obj, FL_LARGE_SIZE);
    fl_set_object_lalign( obj, FL_ALIGN_BOTTOM);
    fl_set_object_lstyle( obj, FL_TIMESBOLD_STYLE + FL_EMBOSSED_STYLE);
    fl_set_input_color(  obj, FL_BLACK, FL_RED);
    fl_set_input_return( obj, FL_RETURN_ALWAYS);
    fl_set_object_callback( obj, CMD_input_callback, FL_RETURN_ALWAYS);
    
  fl_end_group();   //------------------------------------------------------------
         
  fdui->OBJ_group = group = fl_bgn_group(); //------------------------------------

  obj = fl_add_text(FL_NORMAL_TEXT,170,190,240,50,"Select\nSubdetector/Subsystem");
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    
  search( subsystem, "\r", ALL, "");
  
  fdui->SUB_a_browser = obj = fl_add_browser(FL_SELECT_BROWSER,170,240,80,150,"");
    fl_set_object_color(  obj,FL_CYAN,FL_WHEAT);
    fl_set_object_lsize(  obj,FL_LARGE_SIZE);
    fl_set_object_lalign( obj,FL_ALIGN_TOP);
    fl_set_object_lstyle( obj,FL_TIMESBOLD_STYLE);  
    fl_set_browser_fontstyle(  obj, FL_TIMESBOLD_STYLE);
    fl_set_browser_fontsize(   obj, FL_LARGE_SIZE);
    fl_set_object_callback( obj, SUB_browser_callback, 1);
    
  fdui->SUB_b_browser = obj = fl_add_browser(FL_SELECT_BROWSER,250,240,80,150,"");
    fl_set_object_color(  obj,FL_CYAN,FL_WHEAT);
    fl_set_object_lsize(  obj,FL_LARGE_SIZE);
    fl_set_object_lalign( obj,FL_ALIGN_TOP);
    fl_set_object_lstyle( obj,FL_TIMESBOLD_STYLE);  
    fl_set_browser_fontstyle(  obj, FL_TIMESBOLD_STYLE);
    fl_set_browser_fontsize(   obj, FL_LARGE_SIZE);
    fl_set_object_callback( obj, SUB_browser_callback, 2);
    
  fdui->SUB_c_browser = obj = fl_add_browser(FL_SELECT_BROWSER,330,240,80,150,"");
    fl_set_object_color(  obj,FL_CYAN,FL_WHEAT);
    fl_set_object_lsize(  obj,FL_LARGE_SIZE);
    fl_set_object_lalign( obj,FL_ALIGN_TOP);
    fl_set_object_lstyle( obj,FL_TIMESBOLD_STYLE);  
    fl_set_browser_fontstyle(  obj, FL_TIMESBOLD_STYLE);
    fl_set_browser_fontsize(   obj, FL_LARGE_SIZE);
    fl_set_object_callback( obj, SUB_browser_callback, 3);
    
  obj = fl_add_text(FL_NORMAL_TEXT,230,390,120,50,"Select\nObject");
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);

  fdui->OBJ_browser = obj = fl_add_browser(FL_SELECT_BROWSER,230,440,120,170,"");
    fl_set_object_color(  obj,FL_CYAN,FL_WHEAT);
    fl_set_object_lsize(  obj,FL_LARGE_SIZE);
    fl_set_object_lalign( obj,FL_ALIGN_TOP);
    fl_set_object_lstyle( obj,FL_TIMESBOLD_STYLE);  
    fl_set_browser_fontstyle(  obj, FL_TIMESBOLD_STYLE);
    fl_set_browser_fontsize(   obj, FL_LARGE_SIZE);
    fl_set_object_callback( obj, OBJ_browser_callback, 0);
    fl_clear_browser( obj);
    
  fdui->OBJ_input = obj = fl_add_input(FL_NORMAL_INPUT,230,620,120,30,"OBJECT");
    fl_set_object_color(  obj, FL_YELLOW, FL_WHEAT);
    fl_set_object_lsize(  obj, FL_LARGE_SIZE);
    fl_set_object_lalign( obj, FL_ALIGN_BOTTOM);
    fl_set_object_lstyle( obj, FL_TIMESBOLD_STYLE + FL_EMBOSSED_STYLE);
    fl_set_input_color(  obj, FL_BLACK, FL_RED);
    fl_set_input_return( obj, FL_RETURN_ALWAYS);
    fl_set_object_callback( obj, OBJ_input_callback, FL_RETURN_ALWAYS);
    
  fl_end_group();  //------------------------------------------------------------
    
  fdui->DAT_group = group = fl_bgn_group();  //----------------------------------

  obj = fl_add_text(FL_NORMAL_TEXT,420,190,120,50,"Select\nDataset");
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);

  fdui->DAT_browser = obj = fl_add_browser(FL_SELECT_BROWSER,420,240,120,370,"");
    fl_set_object_color(  obj, FL_CYAN, FL_WHEAT);
    fl_set_object_lsize(  obj, FL_HUGE_SIZE);
    fl_set_object_lalign( obj, FL_ALIGN_TOP);
    fl_set_object_lstyle( obj, FL_TIMESBOLD_STYLE);
    fl_set_browser_fontstyle(  obj, FL_TIMESBOLD_STYLE);
    fl_set_browser_fontsize(   obj, FL_LARGE_SIZE);
    fl_set_object_callback(obj,DAT_browser_callback,0);
    fl_clear_browser( obj);
    
  fdui->DAT_input = obj = fl_add_input(FL_NORMAL_INPUT,420,620,120,30,"DATASET");
    fl_set_object_color(  obj, FL_YELLOW, FL_WHEAT);
    fl_set_object_lsize(  obj, FL_LARGE_SIZE);
    fl_set_object_lalign( obj, FL_ALIGN_BOTTOM);
    fl_set_object_lstyle( obj, FL_TIMESBOLD_STYLE + FL_EMBOSSED_STYLE);
    fl_set_input_color(  obj, FL_BLACK, FL_RED);
    fl_set_input_return( obj, FL_RETURN_ALWAYS);
    fl_set_object_callback( obj, DAT_input_callback, FL_RETURN_ALWAYS);
    
  fl_end_group();  //------------------------------------------------------------
    
  fdui->FILE_group = group = fl_bgn_group();  //---------------------------------

  obj = fl_add_text(FL_NORMAL_TEXT,550,210,320,30,"Select Data File Name");
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
  fdui->FILE_a_browser = obj = fl_add_browser(FL_SELECT_BROWSER,550,240,160,370,"");
    fl_set_object_color(obj,FL_CYAN,FL_WHITE);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_browser_fontstyle(  obj, FL_TIMESBOLD_STYLE);
    fl_set_browser_fontsize(   obj, FL_LARGE_SIZE);
    fl_set_object_callback(obj,FILE_browser_callback,0);
  fdui->FILE_b_browser = obj = fl_add_browser(FL_SELECT_BROWSER,710,240,160,370,"");
    fl_set_object_color(obj,FL_CYAN,FL_WHITE);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_browser_fontstyle(  obj, FL_TIMESBOLD_STYLE);
    fl_set_browser_fontsize(   obj, FL_LARGE_SIZE);
    fl_set_object_callback(obj,FILE_browser_callback,0);
  fdui->FILE_input = obj = fl_add_input(FL_NORMAL_INPUT,550,620,320,30,"DATAFILE NAME");
    fl_set_object_color(obj,FL_YELLOW,FL_WHEAT);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_BOTTOM);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE+FL_EMBOSSED_STYLE);
    fl_set_input_color(  obj, FL_BLACK, FL_RED);
    fl_set_input_return( obj, FL_RETURN_ALWAYS);
    fl_set_object_callback(obj,FILE_input_callback, FL_RETURN_ALWAYS);
    
  fl_end_group();  //------------------------------------------------------------

  fdui->PRIORITY_group = group = fl_bgn_group();  //---------------------------------

  obj = fl_add_text(FL_NORMAL_TEXT,890,210,150,30,"Select Priority");
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
  obj = fl_add_button(FL_NORMAL_BUTTON,890,240,30,30,"-");
    fl_set_object_color(obj,FL_CYAN,FL_RED);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lstyle(obj,FL_FIXEDBOLD_STYLE+FL_ENGRAVED_STYLE);
    fl_set_object_callback(obj,PRIORITY_button_callback,-1);
  fdui->PRIORITY_input = obj = fl_add_text(FL_NORMAL_TEXT,920,240,90,30,"0");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_color(obj,FL_YELLOW,FL_MCOL);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
  obj = fl_add_button(FL_NORMAL_BUTTON,1010,240,30,30,"+");
    fl_set_object_color(obj,FL_CYAN,FL_RED);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lstyle(obj,FL_FIXEDBOLD_STYLE+FL_ENGRAVED_STYLE);
    fl_set_object_callback(obj,PRIORITY_button_callback,+1);
  obj = fl_add_text(FL_NORMAL_TEXT,890,270,150,30,"PRIORITY");
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE+FL_EMBOSSED_STYLE);
    
  fl_end_group();  //------------------------------------------------------------

  fdui->EDIT_button = obj = fl_add_lightbutton(FL_NORMAL_BUTTON,920,580,110,30,"EDIT");
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lstyle(obj,FL_NORMAL_STYLE+FL_EMBOSSED_STYLE);
    fl_set_object_callback(obj,EDIT_button_callback,0);

  fdui->SEND_button = obj = fl_add_lightbutton(FL_NORMAL_BUTTON,920,620,110,30,"SEND");
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lstyle(obj,FL_NORMAL_STYLE+FL_EMBOSSED_STYLE);
    fl_set_object_callback(obj,SEND_button_callback,0);

  fdui->RESET_button = obj = fl_add_lightbutton(FL_NORMAL_BUTTON,890,50,180,50,"RESET ALL");
    fl_set_object_color(obj,FL_CYAN,FL_BLUE);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lstyle(obj,FL_NORMAL_STYLE+FL_EMBOSSED_STYLE);
    fl_set_object_callback(obj,RESET_button_callback,0);
    
  fdui->STOP_button = obj = fl_add_lightbutton(FL_NORMAL_BUTTON,890,0,180,50,"STOP NOW");
    fl_set_object_color(obj,FL_CYAN,FL_BLUE);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lstyle(obj,FL_NORMAL_STYLE+FL_EMBOSSED_STYLE);
    fl_set_object_callback(obj,STOP_button_callback,0);

//fdui->SECRET_a_button = obj = fl_add_checkbutton(FL_NORMAL_BUTTON,874,610,50,50,"");
  fdui->SECRET_a_button = obj = fl_add_checkbutton(FL_NORMAL_BUTTON,874,610,20,20,"");
    fl_set_object_color(obj,FL_CYAN,FL_BLUE);
    fl_set_object_callback(obj,SECRET_a_button_callback,0);
//  if (strcmp( "lebedev", getlogin()) != 0) fl_hide_object( obj); // *** ! ***

//fdui->SECRET_b_button = obj = fl_add_checkbutton(FL_NORMAL_BUTTON,1035,624,20,20,"");
  fdui->SECRET_b_button = obj = fl_add_checkbutton(FL_NORMAL_BUTTON, 874,650,20,20,"");
    fl_set_object_color(obj,FL_RED,FL_BLUE);
    fl_set_object_callback(obj,SECRET_b_button_callback,0);
//  fl_hide_object( obj);

  fdui->MISSING_COMMAND_message = group = fl_bgn_group();  //--------------------
  
  obj = fl_add_text(FL_NORMAL_TEXT,40,680,120,100,"@8->");
    fl_set_object_color(obj,FL_COL1,FL_BLACK);
    fl_set_object_lcolor(obj,FL_RED);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,160,720,230,50,"Missing Command!");
    fl_set_object_color(obj,FL_BLACK,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLDITALIC_STYLE+FL_ENGRAVED_STYLE);

  fl_end_group();  //------------------------------------------------------------
  
  fl_hide_object( group);
  
  fdui->UNKNOWN_COMMAND_message = group = fl_bgn_group();  //--------------------
  
  obj = fl_add_text(FL_NORMAL_TEXT,40,680,120,100,"@8->");
    fl_set_object_color(obj,FL_COL1,FL_BLACK);
    fl_set_object_lcolor(obj,FL_RED);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,160,720,230,50,"Unknown Command!");
    fl_set_object_color(obj,FL_BLACK,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLDITALIC_STYLE+FL_ENGRAVED_STYLE);

  fl_end_group();  //------------------------------------------------------------
  
  fl_hide_object( group);
  
  fdui->MISSING_OBJECT_message = group = fl_bgn_group();  //--------------------
  
  obj = fl_add_text(FL_NORMAL_TEXT,230,680,120,100,"@8->");
    fl_set_object_color(obj,FL_COL1,FL_BLACK);
    fl_set_object_lcolor(obj,FL_RED);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,350,720,190,50,"Missing Object!");
    fl_set_object_color(obj,FL_BLACK,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLDITALIC_STYLE+FL_ENGRAVED_STYLE);
  
  fl_end_group();  //------------------------------------------------------------
  
  fl_hide_object( group);
  
  fdui->INVALID_OBJECT_message = group = fl_bgn_group();  //--------------------
  
  obj = fl_add_text(FL_NORMAL_TEXT,230,680,120,100,"@8->");
    fl_set_object_color(obj,FL_COL1,FL_BLACK);
    fl_set_object_lcolor(obj,FL_RED);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,350,720,190,50,"Invalid Object!");
    fl_set_object_color(obj,FL_BLACK,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLDITALIC_STYLE+FL_ENGRAVED_STYLE);
  
  fl_end_group();  //------------------------------------------------------------
  
  fl_hide_object( group);
    
  fdui->UNKNOWN_OBJECT_message = group = fl_bgn_group();  //--------------------
  
  obj = fl_add_text(FL_NORMAL_TEXT,230,680,120,100,"@8->");
    fl_set_object_color(obj,FL_COL1,FL_BLACK);
    fl_set_object_lcolor(obj,FL_RED);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,350,720,190,50,"Unknown Object!");
    fl_set_object_color(obj,FL_BLACK,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLDITALIC_STYLE+FL_ENGRAVED_STYLE);
  
  fl_end_group();  //------------------------------------------------------------
  
  fl_hide_object( group);
  
  fdui->MISSING_DATASET_message = group = fl_bgn_group();  //--------------------
  
  obj = fl_add_text(FL_NORMAL_TEXT,210,720,210,50,"Missing Dataset!");
    fl_set_object_color(obj,FL_BLACK,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLDITALIC_STYLE+FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,420,680,120,100,"@8->");
    fl_set_object_color(obj,FL_COL1,FL_BLACK);
    fl_set_object_lcolor(obj,FL_RED);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_ENGRAVED_STYLE);
  
  fl_end_group();  //------------------------------------------------------------
  
  fl_hide_object( group);
  
  fdui->INVALID_DATASET_message = group = fl_bgn_group();  //--------------------
  
  obj = fl_add_text(FL_NORMAL_TEXT,210,720,210,50,"Invalid Dataset!");
    fl_set_object_color(obj,FL_BLACK,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLDITALIC_STYLE+FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,420,680,120,100,"@8->");
    fl_set_object_color(obj,FL_COL1,FL_BLACK);
    fl_set_object_lcolor(obj,FL_RED);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_ENGRAVED_STYLE);
  
  fl_end_group();  //------------------------------------------------------------
    
  fl_hide_object( group);

  fdui->UNKNOWN_DATASET_message = group = fl_bgn_group();  //--------------------
  
  obj = fl_add_text(FL_NORMAL_TEXT,210,720,210,50,"Unknown Dataset!");
    fl_set_object_color(obj,FL_BLACK,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLDITALIC_STYLE+FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,420,680,120,100,"@8->");
    fl_set_object_color(obj,FL_COL1,FL_BLACK);
    fl_set_object_lcolor(obj,FL_RED);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_ENGRAVED_STYLE);
  
  fl_end_group();  //------------------------------------------------------------
  
  fl_hide_object( group);
  
  fdui->MISSING_FILE_NAME_message = group = fl_bgn_group();  //------------------
  
  obj = fl_add_text(FL_NORMAL_TEXT,405,720,210,50,"Missing File Name!");
    fl_set_object_color(obj,FL_BLACK,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLDITALIC_STYLE+FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,615,680,120,100,"@8->");
    fl_set_object_color(obj,FL_COL1,FL_BLACK);
    fl_set_object_lcolor(obj,FL_RED);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_ENGRAVED_STYLE);

  fl_end_group();  //------------------------------------------------------------

  fl_hide_object( group);
  
  fdui->COMMAND_IS_BEING_SENT_message = group = fl_bgn_group();  //--------------
  
  obj = fl_add_text(FL_NORMAL_TEXT,760,690,280,90,"Command is being sent...");
    fl_set_object_color(obj,FL_YELLOW,FL_MCOL);
    fl_set_object_lcolor(obj,FL_BLACK);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLDITALIC_STYLE+FL_ENGRAVED_STYLE);

  fl_end_group();  //------------------------------------------------------------

  fl_hide_object( group);
  
  fdui->COMMAND_WAS_NOT_SENT_message = group = fl_bgn_group();  //---------------
  
  obj = fl_add_text(FL_NORMAL_TEXT,760,690,280,90,"Command was not sent!");
    fl_set_object_color(obj,FL_BLACK,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLDITALIC_STYLE+FL_ENGRAVED_STYLE);

  fl_end_group();  //------------------------------------------------------------

  fl_hide_object( group);
  
  fdui->COMMAND_HAS_BEEN_SENT_message = group = fl_bgn_group();  //--------------
  
  obj = fl_add_text(FL_NORMAL_TEXT,760,690,280,90,"Command has been sent!");
    fl_set_object_color(obj,FL_CYAN,FL_MCOL);
    fl_set_object_lcolor(obj,FL_BLACK);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLDITALIC_STYLE+FL_ENGRAVED_STYLE);

  fl_end_group();  //------------------------------------------------------------

  fl_hide_object( group);
  
  fdui->COMMAND_WOULD_BE_SENT_message = group = fl_bgn_group();  //--------------
  
  obj = fl_add_text(FL_NORMAL_TEXT,760,690,280,90,"Command would be sent!");
    fl_set_object_color(obj,FL_CYAN,FL_MCOL);
    fl_set_object_lcolor(obj,FL_BLACK);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLDITALIC_STYLE+FL_ENGRAVED_STYLE);

  fl_end_group();  //------------------------------------------------------------

  fl_hide_object( group);
  
  fdui->ANIMATION_group = group = fl_bgn_group();  //----------------------------

  fdui->MY_box = obj = fl_add_box(FL_EMBOSSED_BOX,75,700,90,70,"This\nProgram");
    fl_set_object_color(obj,FL_YELLOW,FL_COL1);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
  fdui->FSD_box = obj = fl_add_box(FL_EMBOSSED_BOX,245,700,90,70,"FSD\nProgram");
    fl_set_object_color(obj,FL_YELLOW,FL_COL1);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
  fdui->CIP_box = obj = fl_add_box(FL_EMBOSSED_BOX,415,700,90,70,"CIP\n(The Wall)");
    fl_set_object_color(obj,FL_YELLOW,FL_COL1);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
  fdui->MY_to_FSD_arrow = obj = fl_add_box(FL_FLAT_BOX,165,710,80,10,"@+96->");
  fdui->FSD_to_MY_arrow = obj = fl_add_box(FL_FLAT_BOX,165,750,80,10,"@+94->");
  fdui->FSD_to_CIP_arrow = obj = fl_add_box(FL_FLAT_BOX,335,710,80,10,"@+96->");
  fdui->CIP_to_FSD_arrow = obj = fl_add_box(FL_FLAT_BOX,335,750,80,10,"@+94->");
  fdui->CIP_to_AMS_arrow = obj = fl_add_box(FL_FLAT_BOX,505,730,80,10,"@+96->");

  fl_end_group();  //------------------------------------------------------------

  fl_hide_object( group);
  
  fdui->AMS_barrel = group = fl_bgn_group();  //---------------------------------

  obj = fl_add_box(FL_OVAL_BOX,585,740,80,50,"");
    fl_set_object_color(obj,FL_WHEAT,FL_COL1);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
  obj = fl_add_box(FL_FLAT_BOX,585,720,80,40,"");
    fl_set_object_color(obj,FL_WHEAT,FL_COL1);
  obj = fl_add_box(FL_OVAL_BOX,585,690,80,50,"AMS");
    fl_set_object_color(obj,FL_YELLOW,FL_COL1);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);

  fl_end_group();  //------------------------------------------------------------

  fl_hide_object( group);
  
  fdui->SEND_message = obj = fl_add_text(FL_NORMAL_TEXT,77,770,427,34,"");
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESITALIC_STYLE);
    
  fl_end_form();  //=============================================================
  
  fdui->F1->fdui = fdui;

  return fdui;
}

//*******************************************************************************

void fill_CMD_browser( void) {
  int i;
//printf( "start fill_CMD_browser\n");
  fl_clear_browser( F1->CMD_browser);
  search( command, "\r", ALL, "");
  while ((i = search( command,
                      "\n",
                      ALL,
                      "")) >= 0) {
    fl_add_browser_line( F1->CMD_browser, command[i].name[0]);
//  printf( " %d %s\n", i, command[i].name[0]);
  }
//printf( "exit from fill_CMD_browser: command_index=%d\n", command_index);
  fill_CMD_input();
}

//-------------------------------------------------------------------------------------

void fill_CMD_input( void) {
  fl_hide_object( F1->MISSING_COMMAND_message);
  fl_hide_object( F1->UNKNOWN_COMMAND_message);
  if (command_index < 0) {
    fl_set_input( F1->CMD_input, "");
  }
  else {
    fl_set_input( F1->CMD_input, command[command_index].name[0]);
    unlock_all_what_was_locked();
  }
  fill_SUB_browser();
}

//-------------------------------------------------------------------------------------

void fill_SUB_browser( void) {

  FL_OBJECT *browser[3];
  int i, j, k;
//printf( "start fill_SUB_browser\n");

  browser[0] = F1->SUB_a_browser;
  browser[1] = F1->SUB_b_browser;
  browser[2] = F1->SUB_c_browser;
  
  for (j=0; j<3; j++) fl_clear_browser( browser[j]);
  
  if (command_index >= 0) {
    for (j=0; j<3; j++) {
      search( subsystem, "\r", ALL, "");
      for (k=0; k<fl_get_browser_screenlines( browser[j]); k++) {
        i = search( subsystem,
                    "\n",
                    command[command_index].command_mode,
                    subsystem_group[j].name[0]);
        if (i < 0) break;
        fl_add_browser_line( browser[j], subsystem[i].name[0]);
//      printf( "  %d %s\n", k, subsystem[i].name[0]);
      }
    }
    if (subsystem_index >= 0) {
      subsystem_index = search( subsystem,
                                subsystem[subsystem_index].name[0],
                                command[command_index].command_mode,
                                "");
    }
  }
  else {
    subsystem_index =-1;
  }
//printf( "exit from fill_SUB_browser: subsystem_index=%d\n", subsystem_index);
  fill_OBJ_browser();
}

//-------------------------------------------------------------------------------------

void fill_OBJ_browser( void) {
  int i;
//printf( "start fill_OBJ_browser\n");
  fl_clear_browser( F1->OBJ_browser);
  if (command_index >= 0 && subsystem_index >= 0) {
    search( object, "\r", ALL, "");
    while ((i = search( object, 
                        "\n",
                        command[command_index].command_mode,
                        subsystem[subsystem_index].name[0])) >= 0) {
      fl_add_browser_line( F1->OBJ_browser, object[i].name[0]);
//    printf( "     %d %s\n", i, object[i].name[0]);
    }
    if (object_index >= 0) {
      object_index = search( object,
                             object[object_index].name[0],
                             command[command_index].command_mode,
                             subsystem[subsystem_index].name[0]);
    }
  }
  else {
    object_index = -1;
  }
//printf( "exit from fill_OBJ_browser: object_index=%d\n", object_index);
  fill_OBJ_input();
}

//-------------------------------------------------------------------------------------

void fill_OBJ_input( void) {
  fl_hide_object( F1->MISSING_OBJECT_message);
  fl_hide_object( F1->INVALID_OBJECT_message);
  fl_hide_object( F1->UNKNOWN_OBJECT_message);
  if (command_index < 0)
      fl_deactivate_object( F1->OBJ_group);
  else
      fl_activate_object(   F1->OBJ_group);
  if (object_index < 0) {
    fl_set_input( F1->OBJ_input, "");
  }
  else {
    fl_set_input( F1->OBJ_input, object[object_index].name[0]);
    unlock_all_what_was_locked();
  }
  fill_DAT_browser();
}

//-------------------------------------------------------------------------------------

void fill_DAT_browser( void) {
  int i; int j;
//printf( "start fill_DAT_browser\n");
  fl_clear_browser( F1->DAT_browser);
  if (command_index >= 0 && subsystem_index >= 0 && object_index >= 0) {
    search( dataset, "\r", ALL, "");
    while ((i = search( dataset, 
                        "\n",
                        command[command_index].command_mode,
                        subsystem[subsystem_index].name[0])) >= 0) {
      if (dataset[i].name[0][0] == '\0')	// do not fill empty string
          fl_add_browser_line( F1->DAT_browser, "<no dataset>");
      else
          fl_add_browser_line( F1->DAT_browser, dataset[i].name[0]);
//    printf( "       %d %s\n", i, dataset[i].name[0]);
    }
    if (dataset_index >= 0) {
      dataset_index = search( dataset,
                              dataset[dataset_index].name[0],
                              command[command_index].command_mode,
                              subsystem[subsystem_index].name[0]);
    }
  }
  else {
    dataset_index = -1;
  }
//printf( "exit from fill_DAT_browser: dataset_index=%d\n", dataset_index);
  fill_DAT_input();
}  

//-------------------------------------------------------------------------------------

void fill_DAT_input( void) {
  int i;
  
  fl_hide_object( F1->MISSING_DATASET_message);
  fl_hide_object( F1->INVALID_DATASET_message);
  fl_hide_object( F1->UNKNOWN_DATASET_message);
  
  if (command_index < 0 || object_index < 0)
      fl_deactivate_object( F1->DAT_group);
  else
      fl_activate_object(   F1->DAT_group);
        
  if (command_index>=0 && subsystem_index>=0 && object_index>=0 && dataset_index<0) {
    i = search( dataset,
                "",
                command[command_index].command_mode,
                subsystem[subsystem_index].name[0]);
    if (i >= 0) dataset_index = i;
  }
      
  if (dataset_index < 0) {
    fl_set_input( F1->DAT_input, "");
  }
  else {
    fl_set_input( F1->DAT_input, dataset[dataset_index].name[0]);
    unlock_all_what_was_locked();
  }
  
  fill_FILE_browser();
}

//-------------------------------------------------------------------------------------

void fill_FILE_browser( void) {
  FL_OBJECT *browser[2];
  DIR  *dirp;
  struct dirent *dp;
  char directory_name[N];
  char this_line[N], next_line[N];
  char *name;
  int i, j, k;
//printf( "start fill_FILE_browser: c,o,d=%d,%d,%d\n",
//    command_index, object_index, dataset_index);
  browser[0] = F1->FILE_a_browser;
  browser[1] = F1->FILE_b_browser;
  for (j=0; j<2; j++) fl_clear_browser( browser[j]);
  j = 0;
  k = 0;
  if (command_index >= 0 && object_index >= 0 && dataset_index >= 0) {
    if (command[command_index].data != GET_BLOCK_TYPE) {
      if (dataset[dataset_index].name[0][0] != '\0') {
        sprintf( master_datafile_name, "%s.%s",
            object[object_index].name[0], dataset[dataset_index].name[0]);
      }
      else {
        sprintf( master_datafile_name, "%s", object[object_index].name[0]);
      }
      fl_add_browser_line( browser[j], master_datafile_name);
      k++;
      if (datafile_index >=0 ) {
        if (strcmp(datafile_name,master_datafile_name) != 0) {
//        fl_add_browser_line( browser[0], datafile_name);
//        k++;
        }
      }
      else {
        datafile_index = 0;
      }
      sprintf( directory_name, getcwd( bb, N));
//    printf( "directory_name = %s\n", directory_name);
      this_line[0] = '\0';
      while (1) {
        if ((dirp = opendir( directory_name)) == NULL) fatal_error( 12301);
        sprintf( bb, "%s*", object[object_index].name[0]);
        next_line[0] = '\0';
        while ((dp = readdir( dirp)) != NULL) {
          name = dp->d_name;
          if (fnmatch( bb, name, 0) == 0) {
            if (strcmp( name, master_datafile_name)) {
              if (strcmp( name, this_line) > 0) {
                if (
                    next_line[0] == '\0' || 
                   (next_line[0] != '\0' && strcmp( name, next_line) < 0)
                   ) {
                  sprintf( next_line, name);
                }
              }
            }
          }
        }
        if (closedir( dirp)) fatal_error( 12302);
        if (next_line[0] == '\0') break;
        if (k >= fl_get_browser_screenlines( browser[0])) j = 1;
        fl_add_browser_line( browser[j], next_line);
        k++;
        sprintf( this_line, next_line);
      }
//    fl_add_browser_line( F1->FILE_browser, "<temporary working file>");
      sprintf( datafile_name, master_datafile_name);
    }
    else {
      datafile_index = -1;
    }
  }
  else {
    datafile_index = -1;
  }
//printf( "exit from fill_FILE_browser: datafile_index=%d\n", datafile_index);
  fill_FILE_input();
}

//-------------------------------------------------------------------------------------

void fill_FILE_input( void) {
  int pos, xpos, ypos;

//printf( "start fill_FILE_input: c,o,d=%d,%d,%d\n",
//    command_index, object_index, dataset_index);
  fl_hide_object( F1->MISSING_FILE_NAME_message);

  if (command_index >= 0 && object_index >= 0 && dataset_index >= 0) {
    if (command[command_index].data != GET_BLOCK_TYPE) {
      fl_activate_object(   F1->FILE_group);
    }
    else {
      fl_deactivate_object( F1->FILE_group);
    }
  }
  else {
    fl_deactivate_object(   F1->FILE_group);
  }
  
  pos = fl_get_input_cursorpos( F1->FILE_input, &xpos, &ypos);
  
  if (datafile_index < 0) {
    fl_set_input( F1->FILE_input, "");
    if (pos != -1) fl_set_input_cursorpos( F1->FILE_input, xpos, ypos);
  }
  else {
    datafile_index = access( datafile_name, F_OK) + 1;	// replace F_OK later
    fl_set_input( F1->FILE_input, datafile_name);
    if (pos != -1) fl_set_input_cursorpos( F1->FILE_input, xpos, ypos);
    unlock_all_what_was_locked();
  }
  
//printf( "exit from fill_FILE_input: datafile_index=%d\n", datafile_index);
  fill_PRIORITY_input();
}

//-------------------------------------------------------------------------------------

void fill_PRIORITY_input( void) {
  sprintf( bb, "%X", command_priority);
  fl_set_object_label( F1->PRIORITY_input, bb);
  setup_EDIT_button();
}

//*******************************************************************************

int setup_EDIT_button( void) {
  if (datafile_index < 0) {		// file name missing
    fl_set_object_color(  F1->EDIT_button, FL_COL1, FL_COL1);
    fl_deactivate_object( F1->EDIT_button);
  }
  if (datafile_index == 0) {		// file name given, file missing
    fl_set_object_color(  F1->EDIT_button, FL_RED, FL_BLUE);
    fl_activate_object(   F1->EDIT_button);
  }
  if (datafile_index > 0) {		// file name given, file exists
    fl_set_object_color(  F1->EDIT_button, FL_CYAN, FL_BLUE);
    fl_activate_object(   F1->EDIT_button);
  }
  setup_SEND_button();
}

//-------------------------------------------------------------------------------------

int setup_SEND_button( void) {
//printf( "start setup_SEND_button: c,o,d,d,p=%d,%d,%d,%d,%d\n",
//        command_index, object_index, dataset_index, datafile_index, command_priority);
  fl_hide_object( F1->COMMAND_IS_BEING_SENT_message);
  fl_hide_object( F1->COMMAND_WAS_NOT_SENT_message);
  fl_hide_object( F1->COMMAND_HAS_BEEN_SENT_message);
  fl_hide_object( F1->COMMAND_WOULD_BE_SENT_message);
  fl_hide_object( F1->ANIMATION_group);
  fl_set_object_color( F1->MY_box, FL_YELLOW, FL_COL1);
  fl_set_object_lcolor( F1->MY_box, FL_BLACK);
  fl_set_object_color( F1->FSD_box, FL_YELLOW, FL_COL1);
  fl_set_object_lcolor( F1->FSD_box, FL_BLACK);
  fl_set_object_color( F1->CIP_box, FL_YELLOW, FL_COL1);
  fl_set_object_lcolor( F1->CIP_box, FL_BLACK);
  fl_set_object_lcolor( F1->MY_to_FSD_arrow, FL_BLACK);
  fl_set_object_lcolor( F1->FSD_to_MY_arrow, FL_BLACK);
  fl_set_object_lcolor( F1->FSD_to_CIP_arrow, FL_BLACK);
  fl_set_object_lcolor( F1->CIP_to_FSD_arrow, FL_BLACK);
  fl_set_object_lcolor( F1->CIP_to_AMS_arrow, FL_BLACK);
  fl_hide_object( F1->AMS_barrel);
//fl_hide_object( F1->SECRET_b_button);
  if (command_index   >= 0 &&
      object_index    >= 0 &&
      dataset_index   >= 0 &&
      what_was_locked == 0) {
    if (command[command_index].data == GET_BLOCK_TYPE || datafile_index > 0) {
      fl_set_object_color( F1->SEND_button, FL_CYAN, FL_BLUE);
      fl_activate_object(  F1->SEND_button);
    }
    else {
      fl_set_object_color(  F1->SEND_button, FL_YELLOW, FL_BLUE);
      fl_deactivate_object( F1->SEND_button);
    }
  }
  else {
    fl_set_object_color(  F1->SEND_button, FL_COL1, FL_COL1);
    fl_deactivate_object( F1->SEND_button);
  }
}

//*******************************************************************************

void CMD_browser_callback( FL_OBJECT *obj, long data) {
  int line;
  line = fl_get_browser( obj);
  if (line) {
    fl_set_focus_object( F1->F1, F1->CMD_input);
    sprintf( command_name, fl_get_browser_line( obj, line));
    command_index = search( command,
                            command_name,
                            ALL,
                            "");
//  printf( "CMD_browser_callback: command_name=%s, command_index=%d\n",
//           command_name, command_index);
    if (command_index < 0) fatal_error( 219);
    fill_CMD_input();
  }
}

//-------------------------------------------------------------------------------------

void SUB_browser_callback( FL_OBJECT *obj, long data) {
  int line;
  if (command_index < 0) fatal_error( 221);
  line = fl_get_browser( obj);
  if (line) {
    fl_set_focus_object( F1->F1, F1->OBJ_input);
    sprintf( subsystem_name, fl_get_browser_line( obj, line));
    subsystem_index = search( subsystem,
                              subsystem_name,
                              command[command_index].command_mode,
                              "");
//  printf( "SUB_browser_callback: subsystem_name=%s, subsystem_index=%d\n",
//           subsystem_name, subsystem_index);
    if (subsystem_index < 0) fatal_error( 229);
    fill_OBJ_browser();
  }
}

//-------------------------------------------------------------------------------------

void OBJ_browser_callback( FL_OBJECT *obj, long data) {
  int line;
  if (command_index   < 0) fatal_error( 231);
  if (subsystem_index < 0) fatal_error( 232);
  line = fl_get_browser( obj);
  if (line) {
    fl_set_focus_object( F1->F1, F1->OBJ_input);
    sprintf( object_name, fl_get_browser_line( obj, line));
    object_index = search( object,
                           object_name,
                           command[command_index].command_mode,
                           subsystem[subsystem_index].name[0]);
//  printf( "OBJ_browser_callback: object_name=%s, object_index=%d\n",
//           object_name, object_index);
    if (object_index < 0) fatal_error( 239);
    fill_OBJ_input();
  }
}


//-------------------------------------------------------------------------------------

void DAT_browser_callback( FL_OBJECT *obj, long data) {
  int line;
  if (command_index   < 0) fatal_error( 241);
  if (subsystem_index < 0) fatal_error( 242);
  if (object_index    < 0) fatal_error( 243);
  line = fl_get_browser( obj);
  if (line) {
    fl_set_focus_object( F1->F1, F1->DAT_input);
    sprintf( dataset_name, fl_get_browser_line( obj, line));
    
    if (strcmp(dataset_name,"<no dataset>") == 0)	 //no empty string from browser
        dataset_name[0] = '\0';
    dataset_index = search( dataset,
                            dataset_name,
                            command[command_index].command_mode,
                            subsystem[subsystem_index].name[0]);
//  printf( "DAT_browser_callback: dataset_name=%s, dataset_index=%d\n",
//           dataset_name, dataset_index);
    if (dataset_index < 0) fatal_error( 249);
    fill_DAT_input();
  }
}

//-------------------------------------------------------------------------------------

void FILE_browser_callback( FL_OBJECT *obj, long data) {
  int line;
  if (command_index   < 0) fatal_error( 251);
  if (subsystem_index < 0) fatal_error( 252);
  if (object_index    < 0) fatal_error( 253);
  if (dataset_index   < 0) fatal_error( 253);
  line = fl_get_browser( obj);
  if (line) {
    fl_set_focus_object( F1->F1, F1->FILE_input);
    sprintf( datafile_name, fl_get_browser_line( obj, line));
//  printf( "FILE_browser_callback: datafile_name=%s\n",
//           datafile_name);
    fill_FILE_input();
  }
}

//*******************************************************************************

void CMD_input_callback( FL_OBJECT *obj, long data) {
  int i, l;
//printf( "start CMD_input_callback\n");
  command_index = -1;
  lock_other_than( F1->CMD_group);
  sprintf( command_name, fl_get_input( F1->CMD_input));
  l = strlen( command_name);
  for (i=0; i<=l; i++) command_name[i] = toupper( command_name[i]);
//printf( "CMD_input_callback: command_name = %s, l = %d\n", command_name, l);
  command_index = search( command,
                          command_name,
                          ALL,
                          "");
//printf( "CMD_input_callback: command_index = %d\n", command_index);
  if (command_index >= 0) {
    fill_CMD_input();
    return;
  }
  if (l == 0) {
//  printf( "Missing Command!\n");
    fl_show_object( F1->MISSING_COMMAND_message);
  }
  else {
//  printf( "Unknown Command '%s'!\n", command_name);
    fl_show_object( F1->UNKNOWN_COMMAND_message);
  }
  fl_set_focus_object( F1->F1, F1->CMD_input);
  return;
}

//-------------------------------------------------------------------------------------

void OBJ_input_callback( FL_OBJECT *obj, long data) {
  int i, l;
//printf( "start OBJ_input_callback\n");
  if (command_index < 0) fatal_error( 321);
  object_index = -1;
  lock_other_than( F1->OBJ_group);
  sprintf( object_name, fl_get_input( F1->OBJ_input));
  l = strlen( object_name);
  for (i=0; i<=l; i++) object_name[i] = toupper( object_name[i]);
//printf( "OBJ_input_callback: object_name = %s, l = %d\n", object_name, l);
  object_index = search( object,
                         object_name,
                         command[command_index].command_mode,
                         "");
//printf( "OBJ_input_callback: object_index = %d\n", object_index);
  if (object_index >= 0) {
    subsystem_index = search( subsystem,
                              object[object_index].parent,
                              command[command_index].command_mode,
                              "");
//  printf( "OBJ_input_callback: subsystem_index = %d\n", subsystem_index);
    if (subsystem_index >= 0) {
      fill_SUB_browser();
      return;
    }
  }
  if (l == 0) {
//  printf( "Missing Object!\n");
    fl_show_object( F1->MISSING_OBJECT_message);
  }
  else {
    if (search( object, object_name, ALL, "") < 0) {
//    printf( "Unknown Object '%s'!\n", object_name);
      fl_show_object( F1->UNKNOWN_OBJECT_message);
    }
    else {
//    printf( "Invalid Object '%s'!\n", object_name);
      fl_show_object( F1->INVALID_OBJECT_message);
    }
  }
  fl_set_focus_object( F1->F1, F1->OBJ_input);
  return;
}

//-------------------------------------------------------------------------------------

void DAT_input_callback( FL_OBJECT *obj, long data) {
  int i, l;
//printf( "start DAT_input_callback\n");
  if (command_index   < 0) fatal_error( 331);
  if (subsystem_index < 0) fatal_error( 332);
  if (object_index    < 0) fatal_error( 333);
  dataset_index = -1;
  lock_other_than( F1->DAT_group);
  sprintf( dataset_name, fl_get_input( F1->DAT_input));
  l = strlen( dataset_name);
  for (i=0; i<=l; i++) dataset_name[i] = toupper( dataset_name[i]);
//printf( "DAT_input_callback: dataset_name = %s, l = %d\n", dataset_name, l);
  dataset_index = search( dataset,
                          dataset_name,
                          command[command_index].command_mode,
                          subsystem[subsystem_index].name[0]);
//printf( "DAT_input_callback: dataset_index = %d\n", dataset_index);
  if (dataset_index >= 0) {
    fill_DAT_input();
    return;
  }
  if (l == 0) {
//  printf("Missing Dataset!\n");
    fl_show_object( F1->MISSING_DATASET_message);
  }
  else {
    if (search( dataset, dataset_name, ALL, "") < 0) {
//    printf( "Unknown Dataset '%s'!\n", command_name);
      fl_show_object( F1->UNKNOWN_DATASET_message);
    }
    else {
//    printf( "Invalid Dataset '%s'!\n", command_name);
      fl_show_object( F1->INVALID_DATASET_message);
    }
  }
  fl_set_focus_object( F1->F1, F1->DAT_input);
  return;
}

//*******************************************************************************

void FILE_input_callback( FL_OBJECT *obj, long data) {
  int l;
//printf( "start FILE_input_callback\n");
  if (command_index   < 0) fatal_error( 341);
  if (subsystem_index < 0) fatal_error( 342);
  if (object_index    < 0) fatal_error( 343);
  if (dataset_index   < 0) fatal_error( 344);
  datafile_index = -1;
  lock_other_than( F1->FILE_group);
  sprintf( datafile_name, fl_get_input( F1->FILE_input));
  l = strlen( datafile_name);
//printf( "FILE_input_callback: datafile_name = '%s', l = %d\n", datafile_name, l);
  if (l > 0) {
    datafile_index = 0;
    fill_FILE_input();
    return;
  }
//printf( "Missing File!\n");
  fl_show_object( F1->MISSING_FILE_NAME_message);
  fl_set_focus_object( F1->F1, F1->FILE_input);
  return;
}

//-------------------------------------------------------------------------------------

void PRIORITY_button_callback( FL_OBJECT *obj, long data) {
//printf( "+ PRIORITY_button_callback: data = %d\n", data);
  command_priority = (command_priority + data) & 0xF;
  fill_PRIORITY_input();
//printf( "- PRIORITY_button_callback\n");
}


//-------------------------------------------------------------------------------------

void EDIT_button_callback( FL_OBJECT *obj, long data) {
//printf( "+ EDIT_button_callback\n");
  setup_EDIT_button();
//printf( "- from EDIT_button_callback\n");
}

//-------------------------------------------------------------------------------------

void SECRET_a_button_callback( FL_OBJECT *obj, long data) {
//printf( "+ SECRET_a_button_callback\n");
  if (datafile_checkup_required) {
    datafile_checkup_required = FALSE;
    fl_set_object_color(obj,FL_RED,FL_BLUE);
  }
  else {
    datafile_checkup_required = TRUE;
    fl_set_object_color(obj,FL_CYAN,FL_BLUE);
  }
//printf( "- SECRET_a_button_callback\n");
}

//-------------------------------------------------------------------------------------

void SECRET_b_button_callback( FL_OBJECT *obj, long data) {
//printf( "+ SECRET_b_button_callback\n");
  command_priority = 0;
  setup_SEND_button();
//printf( "- SECRET_b_button_callback\n");
}

//-------------------------------------------------------------------------------------

void SEND_button_callback( FL_OBJECT *obj, long data) {
  int datafile_status;
  fl_set_object_color(  F1->SEND_button, FL_COL1, FL_COL1);
  fl_deactivate_object( F1->SEND_button);
  if (command_index<0 || subsystem_index<0 || object_index<0 || dataset_index<0) {
    fatal_error( 711);
  }
  
  block_length    = 4;
  block_type      = command[command_index].data;
  node_type       = subsystem[subsystem_index].data;
  node_number     = object[object_index].data;
  data_type       = dataset[dataset_index].data;
  block_ID        = block_type<<13 | node_type<<9 | node_number<<6 | data_type;
  command_ID      = (++command_ID & 0x0FFF) | (command_priority << 12);
  command_time[0] = 0x0000;
  command_time[1] = 0x0000;
  fill_block_CRC();
    
  if (command[command_index].data != GET_BLOCK_TYPE) {
    if (datafile_index < 0) fatal_error( 712);
    if ((data_stream = fopen( datafile_name, "r")) == NULL) fatal_error( 713);
    fl_hide_object( F2->INVALID_DATAFILE_message);
    fl_hide_object( F2->CHECK_DATAFILE_message);
    fl_set_object_color( F2->CANCEL_button,   FL_CYAN, FL_BLUE);
    fl_activate_object(  F2->CANCEL_button);
    fl_set_object_color( F2->CONTINUE_button, FL_CYAN, FL_BLUE);
    fl_activate_object(  F2->CONTINUE_button);
    fl_clear_browser( F2->DATAFILE_browser);
    
    block_body_index = 0;
    datafile_status = get_and_process_data();
    block_length += block_body_index;
    fill_block_CRC();
    
    if (datafile_status != OK) {
      fl_show_object( F2->INVALID_DATAFILE_message);
      fl_set_object_color(  F2->CANCEL_button, FL_RED, FL_BLUE);
      fl_set_object_color(  F2->CONTINUE_button, FL_COL1, FL_COL1);
      fl_deactivate_object( F2->CONTINUE_button);
    }
    else if (were_data_conversion_warnings) {
      fl_show_object( F2->CHECK_DATAFILE_message);
    }
    if (datafile_checkup_required || datafile_status != OK) {
      fl_set_form_position( F2->F2, -F2->F2->w, 0);
      fl_show_form( F2->F2, FL_PLACE_GEOMETRY, FL_FULLBORDER,
                   "AMS COMMANDING PROGRAM  -  DATAFILE CHECKUP");
      fl_hide_form( F1->F1);
      fl_show_form( F1->F1, FL_PLACE_ICONIC, FL_FULLBORDER, "ICON");
    }
    else {
      fl_trigger_object( F2->CONTINUE_button);
    }
  }
  else {
    if (send_this_damned_command() == OK) command_priority = 0;
  }
}

//-------------------------------------------------------------------------------------

void test_messages() {
  fl_clear_browser( F2->DATAFILE_browser);
  
  fl_addto_browser( F2->DATAFILE_browser, " ");

  fl_addto_browser( F2->DATAFILE_browser,   "@C0Your line : 1234 5678   - this is your input");
  fl_addto_browser( F2->DATAFILE_browser, "@C4@iMy line   : 1234 5678   - this is my playback for you to check");
  
  fl_addto_browser( F2->DATAFILE_browser,   "@C0Your line : 1234 5678   - this is your input");
  fl_addto_browser( F2->DATAFILE_browser, "@C5@iMy line   ? 1234 5678   - this is my playback + warning message");
  
  fl_addto_browser( F2->DATAFILE_browser,   "@C0Your line : 1234 5678   - this is your input");
  fl_addto_browser( F2->DATAFILE_browser, "@C1@iMy line   ! 1234 5678   - this is my playback + error message");

  fl_set_form_position( F2->F2, -F2->F2->w, 0);
  fl_show_form( F2->F2, FL_PLACE_GEOMETRY, FL_FULLBORDER, "AMS COMMANDING PROGRAM");
}

//-------------------------------------------------------------------------------------

void RESET_button_callback( FL_OBJECT *obj, long data) {
  static int first = TRUE;
  
  if (first) {
    first = FALSE;
    what_was_locked = 0;
  }

  command_index    = -1;
  subsystem_index  = -1;
  object_index     = -1;
  dataset_index    = -1;
  datafile_index   = -1;
  command_priority =  0;
  command_ID       = 0x0200;
  
  unlock_all_what_was_locked();
  
  datafile_checkup_required = TRUE;
  
  fill_CMD_browser();
    
  fl_set_focus_object( obj->form, F1->CMD_input);
}

//-------------------------------------------------------------------------------------

void STOP_button_callback( FL_OBJECT *obj, long data) {
//fatal_error( 999);
  exit( 0);
}

//*******************************************************************************

void lock_other_than( FL_OBJECT *obj) {
  if (obj == F1->CMD_group) {
      what_was_locked |= OBJ;
      what_was_locked |= DAT;
      what_was_locked |= FIL;
      fl_deactivate_object( F1->OBJ_group);
      fl_deactivate_object( F1->DAT_group);
      fl_deactivate_object( F1->FILE_group);
  }
  if (obj == F1->OBJ_group) {
      what_was_locked |= DAT;
      what_was_locked |= FIL;
      what_was_locked |= CMD;
      fl_deactivate_object( F1->DAT_group);
      fl_deactivate_object( F1->FILE_group);
      fl_deactivate_object( F1->CMD_group);
  }
  if (obj == F1->DAT_group) {
      what_was_locked |= FIL;
      what_was_locked |= CMD;
      what_was_locked |= OBJ;
      fl_deactivate_object( F1->FILE_group);
      fl_deactivate_object( F1->CMD_group);
      fl_deactivate_object( F1->OBJ_group);
  }
  if (obj == F1->FILE_group) {
      what_was_locked |= CMD;
      what_was_locked |= OBJ;
      what_was_locked |= DAT;
      fl_deactivate_object( F1->CMD_group);
      fl_deactivate_object( F1->OBJ_group);
      fl_deactivate_object( F1->DAT_group);
  }
  setup_EDIT_button();
}

//-------------------------------------------------------------------------------------

void CANCEL_button_callback( FL_OBJECT *obj, long data) {
  fl_hide_form( F1->F1);
  fl_set_form_position( F1->F1, 0, -F1->F1->h);
  fl_show_form( F1->F1, FL_PLACE_GEOMETRY, FL_FULLBORDER,
               "AMS COMMANDING PROGRAM  -  KEEP AWAY!");
  fl_hide_form( F2->F2);
  setup_SEND_button();
  fl_show_object( F1->COMMAND_WAS_NOT_SENT_message);
}

//-------------------------------------------------------------------------------------

void CONTINUE_button_callback( FL_OBJECT *obj, long data) {
  if (datafile_checkup_required) {
    fl_hide_form( F1->F1);
    fl_set_form_position( F1->F1, 0, -F1->F1->h);
    fl_show_form( F1->F1, FL_PLACE_GEOMETRY, FL_FULLBORDER,
                 "AMS COMMANDING PROGRAM  -  KEEP AWAY!");
    fl_hide_form( F2->F2);
  }
  if (send_this_damned_command() == OK) command_priority = 0;
}

//-------------------------------------------------------------------------------------

void unlock_all_what_was_locked( void) {
  if (what_was_locked & CMD) {
    fl_activate_object( F1->CMD_group);
    what_was_locked &= ~CMD;
  }
  if (what_was_locked & OBJ) {
    fl_activate_object( F1->OBJ_group);
    what_was_locked &= ~OBJ;
  }
  if (what_was_locked & DAT) {
    fl_activate_object( F1->DAT_group);
    what_was_locked &= ~DAT;
  }
  if (what_was_locked & FIL) {
    fl_activate_object( F1->FILE_group);
    what_was_locked &= ~FIL;
  }
}

//**********************************************************************************

FD_F2 *create_form_F2(void) {

  FL_OBJECT *obj, *group;
  FD_F2 *fdui = (FD_F2 *) fl_calloc(1, sizeof(*fdui));

  fdui->F2 = fl_bgn_form(FL_UP_BOX, 1170, 790);
  
  obj = fl_add_text(FL_NORMAL_TEXT,0,0,1170,30,"Watch Datafile Contents");
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    
  fdui->DATAFILE_browser = obj = fl_add_browser(FL_SELECT_BROWSER,0,30,1170,720,"");
    fl_set_object_color(obj,FL_WHITE,FL_WHITE);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_browser_fontstyle( obj, FL_FIXEDBOLD_STYLE);
    fl_set_browser_fontsize(  obj, FL_MEDIUM_SIZE);

  fdui->CANCEL_button = obj = fl_add_lightbutton(FL_NORMAL_BUTTON,180,754,130,30,"CANCEL");
    fl_set_object_color(obj,FL_CYAN,FL_BLUE);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lstyle(obj,FL_NORMAL_STYLE+FL_EMBOSSED_STYLE);
    fl_set_object_callback(obj,CANCEL_button_callback,0);
    
  fdui->CONTINUE_button = obj = fl_add_lightbutton(FL_NORMAL_BUTTON,860,754,130,30,"CONTINUE");
    fl_set_object_color(obj,FL_CYAN,FL_BLUE);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lstyle(obj,FL_NORMAL_STYLE+FL_EMBOSSED_STYLE);
    fl_set_object_callback(obj,CONTINUE_button_callback,0);
    
  fdui->INVALID_DATAFILE_message = group = fl_bgn_group();  //-------------------
  
  obj = fl_add_text(FL_NORMAL_TEXT,490,750,190,36,"Invalid Datafile!");
    fl_set_object_color(obj,FL_BLACK,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLDITALIC_STYLE+FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,420,750,70,35,"@8->");
    fl_set_object_color(obj,FL_COL1,FL_BLACK);
    fl_set_object_lcolor(obj,FL_RED);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,680,750,70,35,"@8->");
    fl_set_object_color(obj,FL_COL1,FL_BLACK);
    fl_set_object_lcolor(obj,FL_RED);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_ENGRAVED_STYLE);

  fl_end_group();  //------------------------------------------------------------
  
  fl_hide_object( group);
  
  fdui->CHECK_DATAFILE_message = group = fl_bgn_group();  //---------------------

  obj = fl_add_text(FL_NORMAL_TEXT,490,750,190,36,"Check Datafile!");
    fl_set_object_color(obj,FL_BLACK,FL_MCOL);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLDITALIC_STYLE+FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,420,750,70,35,"@8->");
    fl_set_object_color(obj,FL_COL1,FL_BLACK);
    fl_set_object_lcolor(obj,FL_MAGENTA);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,680,750,70,35,"@8->");
    fl_set_object_color(obj,FL_COL1,FL_BLACK);
    fl_set_object_lcolor(obj,FL_MAGENTA);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_ENGRAVED_STYLE);

  fl_end_group();  //------------------------------------------------------------
  
  fl_hide_object( group);
  
  fl_end_form();

  fdui->F2->fdui = fdui;

  return fdui;
}

//**********************************************************************************

void fatal_error( int error_number) {
  printf( "FATAL ERROR #%d:\n", error_number);
  printf( "  command_index = %d", command_index);
  if (command_index >= 0)
      printf( "\r\t\t\tcommand_name = %s", command[command_index].name[0]);
  printf( "\n");
  printf( "  subsystem_index = %d", subsystem_index);
  if (subsystem_index >= 0)
      printf( "\r\t\t\tsubsystem_name = %s", subsystem[subsystem_index].name[0]);
  printf( "\n");
  printf( "  object_index = %d", object_index);
  if (object_index >= 0)
      printf( "\r\t\t\tobject_name = %s", object[object_index].name[0]);
  printf( "\n");
  printf( "  dataset_index = %d", dataset_index);
  if (dataset_index >= 0)
      printf( "\r\t\t\tdataset_name = %s", dataset[dataset_index].name[0]);
  printf( "\n");
  printf( "  datafile_index = %d", datafile_index);
  if (datafile_index > 0)
      printf( "\r\t\t\tdatafile_name = %s", datafile_name);
  printf( "\n");
  printf( "Call A.Lebedev and tell him what is printed above.\n");
  printf( "Do not use this program - it may kill AMS!!!!!!!!!\n");
  exit( 0);
}

//-------------------------------------------------------------------------------------

main( int argc, char *argv[]) {
  given_program_name = argv[0]; 
  setbuf( stdout, NULL);	// set "no buffering" for stdout stream
  atexit( exit_handler);
  signal( SIGINT, ctrl_C_handler);
  fl_initialize( &argc, argv, "AMS COMMANDING version 0.3", 0, 0);
  F1 = create_form_F1();
  F2 = create_form_F2();
  fl_set_form_position( F1->F1, 0, -F1->F1->h);
  fl_show_form( F1->F1, FL_PLACE_GEOMETRY, FL_FULLBORDER, "AMS COMMANDING PROGRAM  -  KEEP AWAY!");
  fl_call_object_callback( F1->RESET_button);	// - this is initialization
  while(1) {
    fl_do_forms();
  }
}

//-------------------------------------------------------------------------------------

int strequ( char *a, char *b) {
//  if (strlen(a)==0 && strlen(b)==0 || strcmp(a,b)==0) return 1; else return 0;
  if (strcmp(a,b)==0) return 1; else return 0;
}

//---------------------------------------------------------------------------

int search( TABLE *table, char *name, int command_mode, char *parent) {
  int i, k;
  if (name[0] == '\r') {
    table[0].q = -1;  // needed for nested usage
    return -1;
  }
  for (i=0; i<10000; i++) {
    if (table[i].command_mode == 0) break;
    if (table[i].command_mode & command_mode) {
      if (parent[0]=='\0' || strequ(table[i].parent,parent)) {
        for (k=0; k<NB_OF_SYNONIMS; k++) {
          if (k>0 && strlen(table[i].name[k])==0) break;
          if (name[0]=='\n' && i>table[0].q) return (table[0].q=i);
          if (name[0]!='\n' && strequ(table[i].name[k],name)) return i;
        }
      }
    }
  }
  return -1;
}

//---------------------------------------------------------------------------

int get_and_process_data( void) {
  time_t t;
  int i, l, m, n, it_is_comment_line;
  float scale, default_scale = 1000.0;
  char tag, c;
  static int eight_bit_mode_on, byte_number;
  int bit_pos[24];
  integer16 v[20];	// here sit hexadecimals and/or integers
  float     V[20];	// here sit floating point numbers
  integer32 number;	// here sits binary number
  int data_conversion_warning;
  data_conversion_warning = FALSE;
  were_data_conversion_warnings = FALSE;
  eight_bit_mode_on = FALSE;
  byte_number = 0;
  while (fgets( data_line, N, data_stream) != NULL) {
    data_line[strcspn( data_line, "\n")] = '\000';
    sprintf( bb, "@C0Your line: %s", data_line);
    fl_addto_browser( F2->DATAFILE_browser, bb);
    were_data_conversion_warnings |= data_conversion_warning;
    data_conversion_warning = FALSE;
    it_is_comment_line = FALSE;
    tag = data_line[0];
//  if ((tag != '8') && eight_bit_mode_on) {
    if ((c == 'H' || c == 'X' || c == 'F' || c == 'R' ||
         c == 'D' || c == 'I' || c == 'B' || c == 'T' ) 
         &&
         eight_bit_mode_on) {
      sprintf( bb, "@C1@i@M8-bit binary input problems. Sorry!");
      fl_addto_browser( F2->DATAFILE_browser, bb);
      return FAIL;
    }
    switch (toupper(tag)) {
    case 'T':
      split_fields( "");
      t = time( NULL);
      v[0] =         t >> 16;
      v[1] = (t << 16) >> 16;
      block_body[block_body_index++] = v[0];
      block_body[block_body_index++] = v[1];
      sprintf( numbers, " %6.4hX %6.4hX", v[0], v[1]);
      break;
    case 'H':
    case 'X':
      split_fields( " \t0123456789abcdefABCDEF");
      n = sscanf( numbers, 
        " %hx %hx %hx %hx %hx %hx %hx %hx %hx %hx %hx %hx %hx %hx %hx %hx %hx %hx %hx %hx",
        &v[0], &v[1], &v[2], &v[3], &v[4], &v[5], &v[6], &v[7], &v[8], &v[9],
        &v[10], &v[11], &v[12], &v[13], &v[14], &v[15], &v[16], &v[17], &v[18], &v[19]);
      m = 0;
      for (i=0; i<n; i++) {
        block_body[block_body_index++] = v[i];
        m += sprintf( numbers+m, " %6.4hX", v[i]);
      }
      m += sprintf( numbers+m, "  ");
      if (n <= 0) data_conversion_warning = TRUE;
      break;
    case 'D':
    case 'I':
      split_fields( " \t0123456789+-");
      n = sscanf( numbers, 
        " %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd",
        &v[0], &v[1], &v[2], &v[3], &v[4], &v[5], &v[6], &v[7], &v[8], &v[9],
        &v[10], &v[11], &v[12], &v[13], &v[14], &v[15], &v[16], &v[17], &v[18], &v[19]);
      m = 0;
      for (i=0; i<n; i++) {
        block_body[block_body_index++] = v[i];
        m += sprintf( numbers+m, " %6hd", v[i]);
      }
      m += sprintf( numbers+m, "  ");
      if (n <= 0) data_conversion_warning = TRUE;
      break;
// Mike demanded to remove this piece - "No floating point numbers in AMS!"    
    case 'F':
    case 'R':
      split_fields( " \t0123456789+-.");
      n = sscanf( numbers, 
        " %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
        &V[0], &V[1], &V[2], &V[3], &V[4], &V[5], &V[6], &V[7], &V[8], &V[9],
        &V[10], &V[11], &V[12], &V[13], &V[14], &V[15], &V[16], &V[17], &V[18], &V[19]);
      scale = default_scale;
      l = strcspn( &comment[0], "*");
      if (comment[l] == '*') {
        if (sscanf( &comment[l+1], " %f", &scale) == 1) {
          l = strspn( &comment[l+1], " \t");
          l = strcspn( &comment[l+1], " \t");
          strcpy( &comment[0], &comment[l+2]);  // remove scale factor from comment field
        }
      }
      m = 0;
      for (i=0; i<n; i++) {
        v[i] = V[i] * scale;
        block_body[block_body_index++] = v[i];
        m += sprintf( numbers+m, " %6hd", v[i]);
      }
      m += sprintf( numbers+m, "  ");
      if (n <= 0) data_conversion_warning = TRUE;
      break;
//* *//      
    case 'B':		// also 'b' will jump here
    case '8':
      split_fields( " \t01");
      n = 0;
      number = 0;
      for (i=0; i<strlen(numbers); i++) {
        c = numbers[i];
        if (c == '0' || c == '1') {
          if (n<24) bit_pos[n] = i;
          n++;
          number = (number << 1) | (integer32)(c - '0');
        }
      }
      if ((tag=='b' && n!=16) || (tag=='B' && n!=24) || (tag=='8' && n!=8)) {
        data_conversion_warning = TRUE;
        numbers[0] = '\000';
      }
      if (!data_conversion_warning) {
        if (tag == '8') {
          if (byte_number % 2) {
            if (eight_bit_mode_on) {
              eight_bit_mode_on = FALSE;
            }
            else {
              data_conversion_warning = TRUE;
              eight_bit_mode_on = FALSE;
              break;
            }
            block_body[block_body_index++]  |= number & 0x000000FF;
          }
          else {
            if (eight_bit_mode_on) {
              data_conversion_warning = TRUE;
              eight_bit_mode_on = FALSE;
              break;
            }
            else {
              eight_bit_mode_on = TRUE;
            }
            block_body[block_body_index  ]   = (number & 0x000000FF) << 8;
          }
          byte_number++;
          for (i=0; i<strlen(numbers); i++) numbers[i] = ' ';
          for (i=0; i<8; i++) {
            if ((number >> (7-i)) & 0x00000001) numbers[bit_pos[i]] = '1';
            else                                numbers[bit_pos[i]] = '0';
          }
        }
        if (tag == 'b') {
          block_body[block_body_index++] = number & 0x0000FFFF;
          for (i=0; i<strlen(numbers); i++) numbers[i] = ' ';
          for (i=0; i<16; i++) {
            if ((number >> (15-i)) & 0x00000001) numbers[bit_pos[i]] = '1';
            else                                 numbers[bit_pos[i]] = '0';
          }
        }
        if (tag == 'B') {
          block_body[block_body_index++] =  number >> 8;
          block_body[block_body_index++] = (number << 8) & 0x0000FF00;
          for (i=0; i<strlen(numbers); i++) numbers[i] = ' ';
          for (i=0; i<24; i++) {
            if ((number >> (23-i)) & 0x00000001) numbers[bit_pos[i]] = '1';
            else                                 numbers[bit_pos[i]] = '0';
          }
        }
      }
      break;
    default:
      it_is_comment_line = TRUE;
      split_fields( "");
      break;
    }
    if (block_body_index > MAX_AMS_COMMAND_BLOCK_BODY_LENGTH) {
      sprintf( bb, "@C1@i@MCommand data block has tooooo many data!  Only %d 16-bit words are allowed!",
          MAX_AMS_COMMAND_BLOCK_BODY_LENGTH);
      fl_addto_browser( F2->DATAFILE_browser, bb);
      return FAIL;
    }    
    if (!it_is_comment_line) {
      if (data_conversion_warning) {
        sprintf( bb, "@C5@iMy line  : %c%s  %s", data_line[0], numbers, comment);
        fl_addto_browser( F2->DATAFILE_browser, bb);
      }
      else {
        sprintf( bb, "@C4@iMy line  : %c%s%s", data_line[0], numbers, comment);
        fl_addto_browser( F2->DATAFILE_browser, bb);
      }
    }
  }
  if (eight_bit_mode_on) {
    sprintf( bb, "@C1@i@M8-bit binary input problems. Sorry!");
    fl_addto_browser( F2->DATAFILE_browser, bb);
    return FAIL;
  }
  return OK;
}

//---------------------------------------------------------------------------

void split_fields( char *numeric_chars) {
  int l;
  l = strspn( &data_line[1], numeric_chars);
  strncpy( numbers, &data_line[1], l);
  numbers[l] = '\000';
  strcpy( comment, &data_line[l+1]);
}

//---------------------------------------------------------------------------

/****************************************************************************/
/*                                                                          */
/*  cyclic redundancy check source was "borrowed" from VK (and "reworked")  */
/*                                                                          */
/****************************************************************************/

void fill_block_CRC( void) {
  integer16 *p, FCS, d, i;
  integer16 g = 0x1021;
  FCS = 0xFFFF;
  for (p=AMS_command_block; p<&block_CRC; p++) {
    d = *p;
    for (i=0; i<16; i++) {
      if ((FCS ^ d) & 0x8000)
        FCS = (FCS << 1) ^ g;
      else
        FCS = (FCS << 1);
      d <<= 1;
    }
  }
  block_CRC = FCS;
}

//---------------------------------------------------------------------------
void ctrl_C_handler( void) {
  exit( 0);
}
//-----------------------------------------------------------------------------
void exit_handler( void) {
  printf( "\033[0m\n\n");	// colors off
  printf( "\033]1;dtterm\007"); // icon title off
  printf( "\033]2;dtterm\007");	// window title off
}
//-------------------------------------------------------------------
