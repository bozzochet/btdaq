// this is file dialog.c
//
// AMS 1553 bus monitor for STS-91 PGSC computer
//
//

#include "pgsc.h"               // it has some global data definitions

#define bus_B     0x0000
#define bus_A     0x0080
unsigned short rt_addr;
unsigned short bus_select;

char line[45];
int indx_line;
int RT;

void display_dialog( void);
void display_dialog_page( void);

//------------------------------------------------------------------------------

void display_dialog( void) {
  set_colors( headline);
  if (input_symbol == '\n' || input_symbol == '\r') {
    line[indx_line] = '\0';
    sprintf( b, "**** ERROR: invalid RT/BUS, no changes made.");
    RT = atoi( line);
    if (RT == 4 || RT == 2 || RT == 28) {
      if (RT == 2) RT = 28;
      rt_addr = RT;
      sprintf( b, "---- RT address changed to %d.", RT);
    }
    if (toupper(line[0]) == 'A' || toupper(line[0]) == 'B') {
      if (toupper(line[0]) == 'A') bus_select = bus_A;
      else                         bus_select = bus_B;
      sprintf( b, "---- 1553 BUS changed to %c.", toupper(line[0]));
    }
    
    display_line( dialog_cursor.row+3, 15, 60, b);
    indx_line = 0;
    new_page = TRUE;
    display_dialog_page();
    new_page = FALSE;
  }
  else {
    if (isprint(input_symbol)) {
      line[indx_line] = input_symbol;
      if (indx_line < 40) {
        indx_line++;
        _settextposition( dialog_cursor.row, dialog_cursor.col);
        sprintf( b, "%c", input_symbol);
        _outtext( b);
        dialog_cursor = _gettextposition();
        if (dialog_cursor.col > 60) dialog_cursor.col = 60;
      }
    }
    else {
      if (input_symbol == '\b' || (int)input_symbol == 127) {
        if (indx_line > 0 ) {
          indx_line--;
          dialog_cursor.col--;
          _settextposition( dialog_cursor.row, dialog_cursor.col);
          sprintf( b, " ");
          _outtext( b);
        }
      }
    }
  }
}

//------------------------------------------------------------------------------

void display_dialog_page( void) {
  short row = 8;
  set_colors( headline);
  if (new_page) {
    indx_line = 0;
    set_colors( headline);
    display_headline( "DIALOG PAGE");
    display_line( row++, 15, 60, " RT MENU - Please select an RT or 1553 Bus:");
    display_line( row++, 15, 60, " 4 - Select Primary RT #4");
    display_line( row++, 15, 60, " 2 - Select Alternate RT #28");
    display_line( row++, 15, 60, " A - Select 1553 Bus A");
    display_line( row++, 15, 60, " B - Select 1553 Bus B");
    display_line( row++, 15, 60, " else exit this menu with no change");
    display_line( row++, 15, 60, " RT: ");
    dialog_cursor = _gettextposition();
  }
}
 
//------------------------------------------------------------------------------
