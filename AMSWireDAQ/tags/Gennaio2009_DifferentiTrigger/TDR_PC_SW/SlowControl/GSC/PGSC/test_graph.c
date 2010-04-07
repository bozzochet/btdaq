#include "pgsc.h"
#include <signal.h>

void exit_handler( void) {
  printf( "\033[0m this is exit handler\n");
}

void ctrl_C_handler( void) {
  printf( "\033[0m this is ctrl_C handler\n");
  exit( 0);
}

void main( void) {

  printf("\033[0m");
  atexit( exit_handler);
  signal( SIGINT, ctrl_C_handler);

  while (1) {
    if (kbhit() != 0) {
      printf("KeyBoard HiT\n");
      printf("Char was '%c'\n", getch());
    }
  }
  
  _clearscreen( _GCLEARSCREEN);
  setup_display();
  display_main_page();
} 
