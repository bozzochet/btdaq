// this is file pp_alpha.c *** to be used on ALPHA computers ONLY ***
//
// AMS 1553 bus monitor for STS PGSC computer (ISA card 1553 interface only)
//
// - various functions for 1553 interface (BU-65529)
//

#include "pgsc.h"

unsigned short pointer;

int nb_of_simulated_1553_words;
int index_of_simulated_1553_word;


void terminate_1553_bus_monitor( void) {
//should I do something else for this card ?
  printf("\n 1553 BUS MONITOR terminated \n");
}

int get_1553_word( void) {
  printf( "this should not be called\n");
  return FAIL;
}

int initialize_1553_bus_monitor( void) {
  printf( " this is alpha-computer, no BU cards available\n");
  return FAIL;
}

#define NN (200*( 1+1 +1+1+32 +1+1+32))

unsigned short simulated_1553_word[NN];
unsigned short simulated_1553_tag[NN];

void prepare_simulated_1553_data( void) {

  FILE *input;
  char line[80];
  int i;
  printf("\n Simulation started... ");
  nb_of_simulated_1553_words = 0;
  index_of_simulated_1553_word = 0;
  input = fopen( "HKframes", "r");
  for (i=0; i<NN; i++) {
    if (fgets( line, 80, input) == NULL) {
      printf("EOF after %d lines\n",nb_of_simulated_1553_words);
      return;
    }
    else { int q;
      ++nb_of_simulated_1553_words;
//    sscanf( line,   "%d", &simulated_1553_tag[i]);
//    sscanf( line+2, "%x", &simulated_1553_word[i]);
      sscanf( line,   "%d", &q); simulated_1553_tag[i] = q;
      sscanf( line+2, "%x", &q); simulated_1553_word[i] = q;
//    printf( "line=%s", line);
//    printf( "tag = %d         ", tag);
//    printf( "number = %4.4x \n", simulated_1553_word[i]);
//    if (i==2) exit(0);
    }
  }
}


void initialize_1553_hardware( void) {

  I_am_in_BM_mode = TRUE;
  I_am_in_BC_mode = FALSE;
  I_am_in_RT_mode = FALSE;
  
  if (initialize_1553_bus_monitor() == FAIL) {
    SIMULATION = TRUE;
    prepare_simulated_1553_data();
  }
  else {
    SIMULATION = FALSE;
  }
}

int new_1553_word( void) {

  if (SIMULATION) {
    OIU_frame_number_block_status = 0x8100;
    block_status[0]               = 0x8100;
    block_status[1]               = 0x8100;
    data_word = simulated_1553_word[index_of_simulated_1553_word];
    index_of_simulated_1553_word = 
      (++index_of_simulated_1553_word) % nb_of_simulated_1553_words;
    return TRUE;
  }
  
  if (get_1553_word() == OK) return TRUE;
  return FALSE;
}


void process_1553_word( void) {

  static int parser_status = 10;
  static int i, j;
  static int first = TRUE;
  
  if (first) {
    first = FALSE;
    new_frame_number = 0;
    new_time_broadcast = 0;
    new_command_box_car[0] = 0;
    new_command_box_car[1] = 0;
    new_PSP_frame = 0;
    new_HK_data_box_car[0] = 0;
    new_HK_data_box_car[1] = 0;
    new_PDI_frame = 0;
  }
  
  switch (parser_status) {
    case 10:
      if (data_word == 0xf811) {
        parser_status = 20;
        goto EXIT;
      }
      display_message( 25, error_in_PRS_col, error_in_PRS_col+3, alarm, "PRS");
      write_log_pipe( "Parser is not locked...");
      goto EXIT;
    case 20:
      OIU_frame_number = data_word;
      new_frame_number++;
      parser_status = 30;
      goto EXIT;
    case 30:
      if (data_word == 0xf811) {
        parser_status = 20;
        goto EXIT;
      }
      if (data_word == 0xfba8) {
        i = 0;
        parser_status = 40;
        goto EXIT;
      }
      if (data_word == 0x2360) {
        i = 0;
        j = 0;
        parser_status = 50;
        goto EXIT;
      }
      if (data_word == 0x2380) {
        i = 0;
        j = 1;
        parser_status = 50;
        goto EXIT;
      }
      if (data_word == 0x2420) {
        i = 0;
        j = 0;
        parser_status = 70;
        goto EXIT;
      }
      if (data_word == 0x2440) {
        i = 0;
        j = 1;
        parser_status = 70;
        goto EXIT;
      }
      goto ERROR;
    case 40:
      time_broadcast[i++] = data_word;
      if (i == TIME_BROADCAST_LENGTH) {
        new_time_broadcast++;
        parser_status = 30;
        goto EXIT;
      }
      goto EXIT;
    case 50:
      HK.box_car[j][i++] = data_word;
      if (i == BOX_CAR_LENGTH) {
        parser_status = 60;
        goto EXIT;
      }
      goto EXIT;          
    case 60:
      status_word[j] = data_word;
      new_command_box_car[j]++;
      if (new_command_box_car[0] && new_command_box_car[1]) {
        new_command_box_car[0] = 0;
        new_command_box_car[1] = 0;
        new_PSP_frame++;
      }
      parser_status = 30;
      goto EXIT;
    case 70:
      status_word[j] = data_word;
      parser_status = 80;
      goto EXIT;
    case 80:
      HK.box_car[j][i++] = data_word;
      if (i == BOX_CAR_LENGTH) {
        new_HK_data_box_car[j]++;
        if (new_HK_data_box_car[0] && new_HK_data_box_car[1]) {
          new_HK_data_box_car[0] = 0;
          new_HK_data_box_car[1] = 0;
          new_PDI_frame++;
        }
        parser_status = 30;
        goto EXIT;
      }
      goto EXIT;          
    default:
      printf(" Impossible syntax error at %d. \n", parser_status);
      goto ERROR;
  }
  printf(" Another impossible syntax error at %d. \n", parser_status);
  parser_status = 10;
  return;
EXIT:
/*printf(" Debug:: parser_status = %d. \n", parser_status);*/
  return;
ERROR:
  printf(" Syntax error at %d. \n", parser_status);
  parser_status = 10;
  return;
}

