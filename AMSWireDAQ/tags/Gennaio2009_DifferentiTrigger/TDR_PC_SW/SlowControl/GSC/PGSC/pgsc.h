#include <stddef.h>
#include "dos.h"
#include "conio.h"
#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>

#define TRUE  1
#define FALSE 0

#define YES 1
#define NO  0

#define Y 1
#define N 0

#define OK   1
#define FAIL 0

#define NORMAL   0
#define WARNING  1
#define ALARM    2
#define OLD_DATA 3
#define NO_DATA  4

/* some global flags */

short I_am_DEC_ALPHA;
short I_am_in_BM_mode;
short I_am_in_BC_mode;
short I_am_in_RT_mode;
short write_N_tuples;
short error_in_1553_data;
short new_frame_number;
short new_time_broadcast;
short new_command_box_car[2], new_PSP_frame;
short new_HK_data_box_car[2], new_PDI_frame;

/* some global data */

#define TIME_BROADCAST_LENGTH 8
#define BOX_CAR_LENGTH 32
#define HK_FRAME_BODY_LENGTH 60
#define MAX_ALLOWED_AMS_BLOCK_LENGTH 2000
#define PDI_FRAME_LENGTH 2*BOX_CAR_LENGTH
#define PSP_FRAME_LENGTH 2*BOX_CAR_LENGTH

long frame_file_number_to_start;
long frame_file_number;

unsigned short data_word; /* this is what comes from 1553 bus */
unsigned char  OIU_frame_number;
unsigned short OIU_frame_number_block_status;
unsigned short time_broadcast[TIME_BROADCAST_LENGTH];
unsigned short time_broadcast_block_status;
unsigned short block_status[2];
unsigned short status_word[2];

unsigned char HK_frame_ID; 
unsigned char HK_frame_first_block_link;

union {
  unsigned short byte[4*BOX_CAR_LENGTH];
  unsigned short word[2*BOX_CAR_LENGTH];
  unsigned short box_car[2][BOX_CAR_LENGTH];
  struct {
    unsigned short offset[2];
    unsigned short header;
    unsigned short body[HK_FRAME_BODY_LENGTH];
    unsigned short CRC;
  } frame;
} HK;

//
//----- AMS block data structure description ----------------
//  (it is so funny to comply with MicroSoft C restrictions)
//

#define integer16 unsigned short
#define integer32 unsigned long
#define MAX_AMS_BLOCK_LENGTH   65535     // 16-bit words

integer16                           AMS_block[MAX_AMS_BLOCK_LENGTH+1];

#define           AMS_block_length  AMS_block[0]
#define           AMS_block_ID      AMS_block[1]
static integer16 *AMS_block_data = &AMS_block[1];
static integer16 *AMS_block_body = &AMS_block[2];
    
//
//----- yet another AMS block data structure description ----
//  (it is so funny to comply with MicroSoft C restrictions)
//

integer16                    bl[MAX_AMS_BLOCK_LENGTH+1];

#define           bl_length  bl[0]
#define           bl_ID      bl[1]
static integer16 *bl_data = &bl[1];
static integer16 *bl_body = &bl[2];

//-----------------------------------------------------------

struct {
  long time;
  unsigned short status_word[2];
  unsigned short block_status[2];
  unsigned short frame[PSP_FRAME_LENGTH];
} PSP;

struct {
  long time;
  unsigned short status_word[2];
  unsigned short block_status[2];
  unsigned short frame[PDI_FRAME_LENGTH];
} PDI;

struct color {
  long  bkcolor;
  short txcolor;
};

struct color headline, frame, bottomline;

struct color normal, old_data, warning, alarm, no_data;

short page_frozen, stop_now;
char page_message[80], page_prompt[80], main_page_prompt[80];
short keyboard_hit;   /* use this flag to avoid VERY SLOW kbhit() call */
short data_are_old;   /* it is NOT a flag, just a COMMON variable */
short data_are_absent;/* it is NOT a flag, just a COMMON variable */
float bit_rate;
float frame_rate;
short new_page;
time_t page_time;
short SIMULATION;

integer16 block_type;
integer16 node_type;
integer16 node_number;
integer16 data_type;

time_t block_time;

#define GET_REPLY_block_type 0x1
#define SET_REPLY_block_type 0x2
#define EXE_REPLY_block_type 0x3
#define HK_DATA_block_type   0x4

#define DAQ_node_type 0x2
#define MCC_node_type 0x8
#define   MAP_data_type  0x01
#define   MASK_data_type 0x02
#define JL3_node_type 0x9
#define   JL3_status_data_type 0x01
#define   JL1_scaler_data_type 0x08
#define JDQS_node_type 0xA
#define CNT_node_type  0xA
#define   TOF_HV_data_type      0x01
#define   TOF_THR_data_type     0x02
#define   TOF_temp_data_type    0x07
#define   ACC_HV_data_type      0x03
#define   ACC_THR_data_type     0x04
#define   ATC_HV_data_type      0x05
#define   JDQS_status_data_type 0x06
#define TRA_node_type 0xB
#define HTC_node_type 0xC
#define PSB_node_type 0xD
#define   STAT_data_type 0x14
#define   VOLT_data_type 0x1C
#define   TEMP_data_type 0x12
#define   AWR_data_type  0x13
#define MON_node_type 0xE
#define   MAG_data_type 0x1E
#define TASE_node_type 0xF

// page_level(s)

#define MAIN_page      0
#define DETAILED_pages 1
#define TEST_pages     2
#define SUMMARY_pages  3
#define DIALOG_page    4

// DETAILED_pages

#define PSB_page              0
#define HTC_page              1
#define CNT_1_page            2
#define CNT_2_page            3
#define CNT_3_page            4
#define MON_page              5
#define MCC_page              6
#define DAQ_1_page            7
#define DAQ_2_page            8
#define DAQ_3_page            9
#define DAQ_4_page            10
#define DAQ_5_page            11
#define DAQ_6_page            12
#define DAQ_7_page            13
#define DAQ_8_page            14
#define CAN_page              15
#define NB_OF_DETAILED_PAGES  16

// TEST_pages

#define frame_number_page   0
#define time_broadcast_page 1
#define PSP_frame_page      2
#define PDI_frame_page      3
#define AMS_block_page      4
#define NB_OF_TEST_PAGES    5

// SUMMARY_pages

#define MCDS_page           0
#define log_page            1
#define EHTS_page           2
#define IHTS_page           3
#define TPLT_page           4
#define NB_OF_SUMMARY_PAGES 5

short page_level;
short page_to_display;

// error/information line (line 25) layout

#define error_in_1553_col  43
#define error_in_PRS_col   48
#define error_in_CRC_col   52
#define error_in_DFR_col   56
#define error_in_BLK_col   60

unsigned char command, input_symbol;       
short wrong_command;

struct rccoord cursor_home, dialog_cursor;
  
char b[100];

//----------------------------------------------------------------------------------
//
//---- declaration of functions defined in file pgsc.c
//
//
//                LOW-LEVEL display control functions
//
void clear_display( void);
void set_colors( struct color c);
void set_value_colors( short status);
void display_line( short row, short col_from, short col_to, char *text);
void display_message( short r, short c1, short c2, struct color c, char *t);
void setup_display( void);
//
//                 GENERAL display control functions
//
void display_headline( char *text);
void display_page_body( void);
void display_bottomline( void);
//
//         AMS HEALTH AND STATUS (ao) display functions
//
void display_AMS_health_and_status();
void display_log_page( void);
//
//                    TEST PAGE display functions
//
void display_frame_number_page( void);
void display_time_broadcast_page( void);
void display_PSP_frame_page( void);
void display_PDI_frame_page( void);
void display_AMS_block_page( void);
//
//                 GENERAL data processing functions
//
void process_frame_number( void);
void process_time_broadcast( void);
void process_PSP_frame( void);
void process_PDI_frame( void);
void deframe_HK_frame( void);
void process_AMS_block( void);
//
//                     GENERAL control functions
//
void process_keyboard_hit( void);
void display_page_if_necessary( void);
//
//                   various junk-printing functions
//
void print_frame_number( void);
void print_time_broadcast( void);
void print_PSP_frame( void);
void print_PDI_frame( void);
void print_HK_frame( void);
void print_AMS_block( void);
//
//                        "library" functions
//
char *mytime( void);
void wait( int seconds);
void open_log_pipe_for_writing( void);
void write_log_pipe( char *message);
int open_log_pipe_for_reading( void);
char *read_log_pipe( void);

//
//===================== global variables ==================================
//

#define STALENESS 30	// in sec

#define MCDS_time_staleness STALENESS

#define PSB_ADC_time_staleness         STALENESS
#define PSB_digital_IO_time_staleness  STALENESS
#define PSB_temperature_time_staleness STALENESS

#define  PDD_stat_time_staleness        STALENESS
#define  HTC_digital_IO_time_staleness  STALENESS              
#define  HTC_temperature_time_staleness STALENESS
#define  HTC_AWR_time_staleness         STALENESS

#define T_sensor_time_staleness STALENESS
#define B_sensor_time_staleness STALENESS

#define TOF_HV_time_staleness   STALENESS
#define TOF_THR_time_staleness  STALENESS
#define TOF_temp_time_staleness STALENESS
#define ACC_HV_time_staleness   STALENESS
#define ATC_HV_time_staleness   STALENESS

#define CAN_map_time_staleness  STALENESS
#define CAN_mask_time_staleness STALENESS

#define JL3_time_staleness        STALENESS
#define JL1_scaler_time_staleness STALENESS
#define JDQS_time_staleness       STALENESS

#define MCC_status_time_staleness STALENESS
#define MCC_delay_time_staleness  STALENESS
#define MCC_expand_time_staleness STALENESS
#define MCC_list_time_staleness   STALENESS



//------ PIPE ----------- PIPE ----------- PIPE ----------- PIPE ----------

#define LOG_PIPE_SIZE 21

struct {
  char body[LOG_PIPE_SIZE][81];
  int put_counter;
  int put_pointer;
  int get_counter;
  int get_pointer;
} log_pipe;

//------ MCDS ----------- MCDS ----------- MCDS ----------- MCDS ----------

time_t MCDS_time;

struct MCDS {
  short health_discreets;
  short GPC_command_count;
  short total_command_count;
  short heartbeat;
  short heaters_state;
  short power_step;
  short RT_MCC;
  short pre_under_temperature;
  short pre_over_temperature;
  short under_temperature;
  short over_temperature;
};

struct MCDS MCDS_data, MCDS_status;

//------ PSB ------------ PSB ------------ PSB ------------ PSB -----------

struct { 
  time_t ADC_time;
  time_t digital_IO_time;
  time_t temperature_time;
  struct {
    short status;
    short state;
    struct {
      short status;
      short state;
      float voltage;
      float current;
      float power;
    } output[3];
  } DC_converter[10];
  struct {
    short status;
    short state;
  } IMBP;
  struct {
    short ENABLED;
    short ON;
    short status;
  } heater;
  struct { 
    struct { 
      short status;
      float capacity;
      float enabled;
      float used;
    } power;
    struct { 
      short status;
      short delta;
    } temperature;
  } total;
  struct {
    short status;
    short value;
  } temperature[2];
} PSB[4];

struct {
  struct {
    short status;
    float capacity;
    float enabled;
    float used;
  } power;
  struct {
    short status;
    float delta;
  } temperature;
} PSB_grand_total;

//-------- HTC/PDD ------------ HTC/PDD ------------ HTC/PDD --------------

time_t PDD_stat_time;
integer16 PDD_stat;

struct {
  time_t digital_IO_time;                
  time_t temperature_time;
  time_t AWR_time;
  struct {
    short PDD_ON;
    short ENABLED;
    short ON;
    short AWR;
    short status;
    short heater_temperature;
    short temperature_status;
  } chain[16];
  struct {
    struct {
      short status;
      float capacity;
      float enabled;
      float used;
    } power;
  } total;
} HTC[5];

struct {
  struct {
    short status;
    float capacity;
    float enabled;
    float used;
  } power;
} HTC_grand_total;

//------ MON ------------ MON ------------ MON ------------ MON -----------

int    T_sensor[3][32];
int    T_sensor_status[3][32];
time_t T_sensor_time[3];

float  B_sensor[2][11];
int    B_sensor_status[2][11];
time_t B_sensor_time[2];

//------ CNT ------------ CNT ------------ CNT ------------ CNT -----------

//- - - - - - - - - - - - - - - - - - TOF - - - - - - - - - - - - - - - - -

int    TOF_HV[8][14][3];
int    TOF_HV_status[8][14][3];
time_t TOF_HV_time[8];

int    TOF_THR[8][14][4];
int    TOF_THR_status[8][14][4];
time_t TOF_THR_time[8];

float  TOF_temp[8][4];
int    TOF_temp_status[8][4];
time_t TOF_temp_time[8];

//- - - - - - - - - - - - - - - - - - ACC - - - - - - - - - - - - - - - - -

int    ACC_HV[2][4][8];
int    ACC_code[2][4];
int    ACC_HV_status[2][4];
time_t ACC_HV_time[2];

//- - - - - - - - - - - - - - - - - - ATC - - - - - - - - - - - - - - - - -

int    ATC_HV[2][6][2];
int    ATC_HV_status[2][6][2];
time_t ATC_HV_time[8];

//------ CAN ------------ CAN ------------ CAN ------------ CAN -----------

time_t CAN_map_time;
time_t CAN_mask_time;

//- - - - - - - - - - - - - - - - - - global counters - - - - - - - - - - -

int nb_of_CAN_logical_nodes_failed;             // comes from MAP
int nb_of_CAN_physical_nodes_failed;            // comes from MAP
int nb_of_CAN_links_failed;                     // comes from MAP

int nb_of_CAN_masks_modified;                   // comes from MASK

int nb_of_CAN_logical_nodes_lost;               // comes from MAP & MASK

//- - - - - - - - - - - - - - - - - - - global flags - - - - - - - - - - - 

int CAN_map_corrupted;                          // comes from MAP

//- - - - - - - - - - - - - - - - - - detailed flags & actual values - - -

struct {
  unsigned char map;            // comes from MAP
  int map_corrupted;            // comes from MAP
  int failed;                   // comes from MAP
  int has_bad_nodes;            // comes from MAP
  int has_bad_links;            // comes from MAP

  unsigned char mask;           // comes from MASK
  int access_modified;          // comes from MASK
  
  int lost;                     // comes from MAP & MASK
  
} actual_CAN_node[64];

//-------- JL3/DAQ ------------ JL3/DAQ ------------ JL3/DAQ --------------

//- - -  JL3 - - - - - -  JL3 - - - - - -  JL3 - - - - - -  JL3 - - - - - -

time_t JL3_time[6];

integer32 JL3_status[6][100];

integer32 event_number;
short     event_number_status;
time_t    event_number_time;

integer32 accepted_event_number;
short     accepted_event_number_status;
time_t    accepted_event_number_time;

integer32 data_frame_number;
short     data_frame_number_status;
time_t    data_frame_number_time;

integer32 run_number;
short     run_number_status;
time_t    run_number_time;

integer32 run_tag;
short     run_tag_status;
time_t    run_tag_time;

short  run_type;
short  run_type_status;
time_t run_type_time;

float  L1_rate;
short  L1_rate_status;
time_t L1_rate_time;

float  L3_rate;
short  L3_rate_status;
time_t L3_rate_time;

float  data_frame_rate;
short  data_frame_rate_status;
time_t data_frame_rate_time;

float  event_size;
short  event_size_status;

integer32 crate_found;
short     crate_found_status;
time_t    crate_found_time;

integer32 JBU_frame_counter[6];
short     JBU_frame_counter_status[6];
time_t    JBU_frame_counter_time;

char   JBU_output[4];
short  JBU_output_status;
time_t JBU_output_time;

char   JFO_used[3];
short  JFO_used_status;
time_t JFO_used_time;

char   JL1_used[3];
short  JL1_used_status;
time_t JL1_used_time;

struct {
  integer32 tag;
  long nb_of_events;
} this_run;

//- - -  JL1 - - - - - -  JL1 - - - - - -  JL1 - - - - - -  JL1 - - - - - -

time_t JL1_scaler_time;

integer16 JL1_scalers_status;
float     JL1_scaler_livetime;
short     JL1_scaler_livetime_status;
integer16 JL1_scaler_data[96];
short     JL1_scaler_status[96];
integer32 JL1_scaler_sums[8];
short     JL1_scaler_sums_status[8];

//- - - JDQS - - - - - - JDQS - - - - - - JDQS - - - - - - JDQS - - - - - -

time_t    JDQS_time[8];
integer16 JDQS_status[8][42];
integer16 JDQS_status_status[8][42];
int       JDQS_event_size[8];

//----- MCC --------- MCC --------- MCC --------- MCC --------- MCC -------

time_t    MCC_status_time;
time_t    MCC_alivetime;
integer16 MCC_alivetime_status;

time_t MCC_delay_time;
int    MCC_delay[2];
short  MCC_delay_status;

unsigned int MCC_status[7];
int    MCC_status_status[7];

time_t MCC_expand_time;
int    MCC_expand[15];
short  MCC_expand_status;

time_t MCC_list_time;
int    MCC_list_total;
int    MCC_list_subt[16];
short  MCC_list_status;

char   RT_BUS;


