// file x.h
//
// Reanimation of AMS-01 program x
//
// A.Lebedev Oct-2008...

// this is main include file for X-forms/X-Windows AMS commanding program

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include "forms.h"
//#include <ams/types.h>
//#include <ams/tcctcp.h>

typedef struct {
	FL_FORM *F1;
	void *vdata;
	long ldata;
	FL_OBJECT *CMD_browser;
	FL_OBJECT *CMD_input;
	FL_OBJECT *SUB_a_browser;
	FL_OBJECT *SUB_b_browser;
	FL_OBJECT *SUB_c_browser;
	FL_OBJECT *OBJ_browser;
	FL_OBJECT *OBJ_input;
	FL_OBJECT *DAT_browser;
	FL_OBJECT *DAT_input;
	FL_OBJECT *FILE_a_browser;
	FL_OBJECT *FILE_b_browser;
	FL_OBJECT *FILE_input;
	FL_OBJECT *PRIORITY_input;
	FL_OBJECT *EDIT_button;
	FL_OBJECT *SEND_button;
	FL_OBJECT *RESET_button;
	FL_OBJECT *STOP_button;
	FL_OBJECT *SECRET_a_button;
	FL_OBJECT *SECRET_b_button;
	FL_OBJECT *CMD_group;
	FL_OBJECT *OBJ_group;
	FL_OBJECT *DAT_group;
	FL_OBJECT *FILE_group;
	FL_OBJECT *PRIORITY_group;
	FL_OBJECT *ANIMATION_group;
	FL_OBJECT *MY_box;
	FL_OBJECT *FSD_box;
	FL_OBJECT *CIP_box;
	FL_OBJECT *MY_to_FSD_arrow;
	FL_OBJECT *FSD_to_MY_arrow;
	FL_OBJECT *FSD_to_CIP_arrow;
	FL_OBJECT *CIP_to_FSD_arrow;
	FL_OBJECT *CIP_to_AMS_arrow;
	FL_OBJECT *AMS_barrel;
	FL_OBJECT *MISSING_COMMAND_message;
	FL_OBJECT *UNKNOWN_COMMAND_message;
	FL_OBJECT *MISSING_OBJECT_message;
	FL_OBJECT *INVALID_OBJECT_message;
	FL_OBJECT *UNKNOWN_OBJECT_message;
	FL_OBJECT *MISSING_DATASET_message;
	FL_OBJECT *INVALID_DATASET_message;
	FL_OBJECT *UNKNOWN_DATASET_message;
	FL_OBJECT *MISSING_FILE_NAME_message;
	FL_OBJECT *COMMAND_IS_BEING_SENT_message;
	FL_OBJECT *COMMAND_WAS_NOT_SENT_message;
	FL_OBJECT *COMMAND_HAS_BEEN_SENT_message;
	FL_OBJECT *COMMAND_WOULD_BE_SENT_message;
	FL_OBJECT *SEND_message;
} FD_F1;

typedef struct {
	FL_FORM *F2;
	void *vdata;
	long ldata;
	FL_OBJECT *CANCEL_button;
	FL_OBJECT *CONTINUE_button;
	FL_OBJECT *DATAFILE_browser;
	FL_OBJECT *INVALID_DATAFILE_message;
	FL_OBJECT *CHECK_DATAFILE_message;
} FD_F2;

FD_F1 *F1;	// make it global
FD_F2 *F2;	// make it global

FD_F1 *create_form_F1( void);
FD_F2 * create_form_F2(void);
void  CMD_browser_callback( FL_OBJECT *, long);
void  SUB_browser_callback( FL_OBJECT *, long);
void  OBJ_browser_callback( FL_OBJECT *, long);
void  DAT_browser_callback( FL_OBJECT *, long);
void FILE_browser_callback( FL_OBJECT *, long);
void  CMD_input_callback( FL_OBJECT *, long);
void  OBJ_input_callback( FL_OBJECT *, long);
void  DAT_input_callback( FL_OBJECT *, long);
void FILE_input_callback(  FL_OBJECT *, long);

void PRIORITY_button_callback( FL_OBJECT *, long);
void     EDIT_button_callback( FL_OBJECT *, long);
void     SEND_button_callback( FL_OBJECT *, long);
void    RESET_button_callback( FL_OBJECT *, long);
void     STOP_button_callback( FL_OBJECT *, long);
void SECRET_a_button_callback( FL_OBJECT *, long);
void SECRET_b_button_callback( FL_OBJECT *, long);
void   CANCEL_button_callback( FL_OBJECT *, long);
void CONTINUE_button_callback( FL_OBJECT *, long);

void fill_CMD_browser ( void);
void fill_SUB_browser ( void);
void fill_OBJ_browser ( void);
void fill_DAT_browser ( void);
void fill_FILE_browser( void);

void fill_CMD_input ( void);
void fill_OBJ_input ( void);
void fill_DAT_input ( void);
void fill_FILE_input( void);
void fill_PRIORITY_input( void);
int setup_EDIT_button( void);
int setup_SEND_button( void);
void lock_other_than( FL_OBJECT *obj);
void unlock_all_what_was_locked( void);
void fatal_error( int);
int send_this_damned_command();
int get_and_process_data( void);
void split_fields( char *numeric_chars);
void fill_block_CRC( void);
void ctrl_C_handler( void);
void exit_handler( void);

#define integer16 unsigned short
#define integer32 unsigned int

//#define FALSE 0
//#define TRUE  1
#define FAIL 0
#define OK   1

#define CMD 1
#define OBJ 2
#define DAT 4
#define FIL 8

int what_was_locked;
int datafile_checkup_required;

#define N 256
char command_name[N], subsystem_name[N], object_name[N], dataset_name[N];
int  command_index,   subsystem_index,   object_index,   dataset_index;

integer16 command_priority;

integer16 block_type;
integer16 node_type;
integer16 node_number;
integer16 data_type;

int were_data_conversion_warnings;

char data_line[N];
char numbers[N];
char comment[N];

char bb[N];

char command_file_name[N];
char datafile_name[N];
int  datafile_index;
char master_datafile_name[N];

#define GET_BLOCK_TYPE 0x1

FILE *command_stream;
FILE *data_stream;
FILE *log_stream;

#define MAX_AMS_COMMAND_BLOCK_LENGTH 61		// 16-bit words
#define MAX_AMS_COMMAND_BLOCK_BODY_LENGTH 56	// 16-bit words
#define BUFFER_ZONE 256				// 16-bit words

integer16                          AMS_command_block[MAX_AMS_COMMAND_BLOCK_LENGTH+1+BUFFER_ZONE];
#define block_length               AMS_command_block[0]
#define block_ID                   AMS_command_block[1]
#define command_ID                 AMS_command_block[2]
static  integer16 *command_time = &AMS_command_block[3];
static  integer16 *block_body   = &AMS_command_block[5];
#define block_CRC                  AMS_command_block[block_length+1]  // it is outside block !

integer16                          UTU_command_block[MAX_AMS_COMMAND_BLOCK_LENGTH+1+BUFFER_ZONE];

integer16 block_body_index;

char *given_program_name;

