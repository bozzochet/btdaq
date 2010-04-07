#include "pgsc.h"

void main( void) {
  int row = 1, col = 1;
  char screen[24][82] = {
  
//" 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789\n",
  "                             \n",
  " RRRRRR RRRRRR RRRRRR RRRRRR RRRRRR RRRRRR RRRRRR RRRRRR \n",
  " RRRRRR RRRRRR RRRRRR RRRRRR RRRRRR RRRRRR RRRRRR RRRRRR \n",
  "                                                         \n",
  "       RRRRR     RRRRR             RRRRR     RRRRR       \n",
  "       RRRRR     RRRRR             RRRRR     RRRRR       \n",
  "       RRRRR     RRRRR             RRRRR     RRRRR       \n",
  "                                                         \n",
  " R R R RRRRR R R RRRRR R R R R R R RRRRR R R RRRRR R R R \n",
  " R R R RRRRR R R RRRRR R R R R R R RRRRR R R RRRRR R R R \n",
  " R R R RRRRR R R RRRRR R R R R R R RRRRR R R RRRRR R R R \n",
  "                                                         \n",
  " R R R RRRRR R R RRRRR R R R R R R RRRRR R R RRRRR R R R \n",
  " R R R RRRRR R R RRRRR R R R R R R RRRRR R R RRRRR R R R \n",
  " R R R RRRRR R R RRRRR R R R R R R RRRRR R R RRRRR R R R \n",
  "                                                         \n",
  "       RRRRR     RRRRR             RRRRR     RRRRR       \n",
  "       RRRRR     RRRRR             RRRRR     RRRRR       \n",
  "       RRRRR     RRRRR             RRRRR     RRRRR       \n",
  "                                                         \n",
  " RRRRRR RRRRRR RRRRRR RRRRRR RRRRRR RRRRRR RRRRRR RRRRRR \n",
  " RRRRRR RRRRRR RRRRRR RRRRRR RRRRRR RRRRRR RRRRRR RRRRRR \n",
  " \0",
  };
  
  setup_display();
  _clearscreen( _GCLEARSCREEN);
  for (row=1; row<=23; row++) {
    if (screen[row][1] == '\0') break;
    for (col=1; col<=80; col++) {
      if (screen[row][col] == '\n') break;
      if (screen[row][col] == ' ') set_colors( frame);
      if (screen[row][col] == 'R') set_colors( value.warning);
      _settextposition( row, col);
      _outtext( " ");
    }
  }
  _settextposition( 20, 1);
}