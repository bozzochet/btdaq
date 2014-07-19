// this is file psb.c
//
// AMS 1553 bus monitor for STS-91 PGSC computer
//
// - functions for PSB data processing and display
//

#include "pgsc.h"               // it has some global data definitions

//------------------------------------------------------------------------------

static int PSB_index[8] = { 0, -1, 2, 3, 1, -1, -1, -1};
static int PSB_node_number[4] = { 0x0, 0x4, 0x2, 0x3};

static char *PSB_name[4] = { "PSB.0", "PSB.3", "PSB.4", "PSB.7"};
static short PSB_col[4]  = {     9,      23,      37,      51  };

static struct {
  char  *name;
  short nb_of_outputs;
  short P_bit;
  short S_bit;
  struct {
    char  *name;
    short row;
    short V_index;
    short I_index;
  } output[3];
  struct {
    short PSB[4];
  } is_present_in;
} DC_converter[10] = {

//DCDC  # of   MDIon    "nominal"  dis-    ADC      is this conv
//conv  out-    bit      voltage   play   index     in this PSB ??
//name  puts   P   S        V      row    V   I     0  3  4  7
                                     
  {"0",  1,   18, 17,   {{"+5.6",    4,   1,  2}}, {Y, Y, Y, Y} },
  {"1",  3,   20, 19,   {{"-5.6",    5,   7,  8},
                        {"+12.5",    6,   3,  4}, 
                        {"-12.5",    7,   5,  6}}, {Y, Y, Y, Y} },
  {"2",  1,   22, 21,  {{"+12.5",    8,   9, 10}}, {Y, Y, Y, Y} },
  {"3",  1,    9, 10,   {{"+5.6",   10,  13, 14}}, {Y, Y, N, Y} },
  {"4",  1,   11, 12,   {{"+5.6",   11,  15, 16}}, {Y, N, N, N} },
  {"5",  1,   16, 15,   {{"+5.6",   12,  20, 21}}, {Y, Y, Y, Y} },
  {" ",  1,   14, 13,   {{"    ",   13,  18, 17}}, {N, N, N, N} },  // fake
  {"7",  1,    7,  8,   {{"+5.6",   14,  32, 31}}, {Y, Y, Y, Y} },
  {"8",  3,    5,  6,   {{"-5.6",   15,  26, 25}, 
                        {"+12.5",   16,  30, 29}, 
                        {"-12.5",   17,  28, 27}}, {Y, Y, Y, Y} },
  {"9",  1,    3,  4,  {{"+12.5",   18,  24, 23}}, {Y, Y, Y, Y} },
};

static struct {
  short P_bit;
  short S_bit;
} IMBP = { 2, 23};

static struct {
  short P_bit;
  short S_bit;
} heater = { 1, 24};

static short PSB_total_power_row = 22;
static short temperature_row     = 20;
  
static short PSB_grand_total_power_row = 22;
static short PSB_grand_total_power_col = 65;

//------------------------------------------------------------------------------

void simulate_PSB_block( void) {

  int ii, iii, j, k, l, V_index, I_index;   static long q = 0;
  float voltage, current;
  short temperature;
  unsigned char port[3];
  time_t now;
  static int first = TRUE;
  
  ii = rand() % 4;                                         // PSB index
  q++;  if (q > 20) q = 0; if (q > 10 && ii == 1) return;

//AMS_block_ID = block_type<<13 | node_type<<9 | node_number<<6 | data_type;
  
  iii = rand() % 3;

//*********************************************
//*                                           *
//*      simulate HK block 'header'           *
//*                                           *
//*********************************************

    AMS_block_body[0] = (AMS_block_body[0] + 1) & 0x0FFF;
    if (rand()%32==0) AMS_block_body[0] |= 0x8000;
    now = time( NULL);
    AMS_block_body[1] = (now & 0xFFFF0000) >> 16;
    AMS_block_body[2] =  now & 0x0000FFFF;
                                                      
//*********************************************
//*                                           *
//*      simulate voltage and current data    *
//*                                           *
//*********************************************

  if (iii == 0) {
  
    AMS_block_ID = GET_REPLY_block_type << 13 |
                   PSB_node_type        <<  9 |
                   PSB_node_number[ii]  <<  6 |
                   VOLT_data_type;

    if (first) {
      AMS_block_ID |= 0x01C0;
      first = FALSE;
    }
  
    for (j=0; j<10; j++) {
      for (k=0; k<DC_converter[j].nb_of_outputs; k++) {
        V_index = DC_converter[j].output[k].V_index - 1;
        I_index = DC_converter[j].output[k].I_index - 1;
        if (DC_converter[j].is_present_in.PSB[ii]) {
          voltage = atof( DC_converter[j].output[k].name);
          voltage = voltage - 0.5 + (float)rand() / (float)RAND_MAX;
          //assume P=20Wt
          current = 20.0 / voltage - 0.5 + (float)rand() / (float)RAND_MAX;
        }
        else {
          voltage = 0.2 * (float)rand() / (float)RAND_MAX;
          current = (float)ii / 10.0;
        }
        AMS_block_body[V_index+3] = 1000.0 * voltage;
        AMS_block_body[I_index+3] = 1000.0 * current;
      }
    }

    AMS_block_length = 1 + 3 + 32;
  
    return;
  }
  
//************************************************
//*                                              *
//*  simulate primary/secondary MDI & HE & IMBP  *
//*             (a.k.a. digital I/O)             *
//*                                              *
//************************************************

  if (iii == 1) {
  
    AMS_block_ID = GET_REPLY_block_type << 13 |
                   PSB_node_type        <<  9 |
                   PSB_node_number[ii]  <<  6 |
                   STAT_data_type;
  
    port[0] = port[1] = port[2] = 0;
  
    for (j=0; j<10; j++) {
      if (DC_converter[j].is_present_in.PSB[ii]) {
        if (rand()%4!=0) {
          k = (DC_converter[j].P_bit - 1) / 8;
          l = (DC_converter[j].P_bit - 1) - 8 * k;
          port[k] |= 1 << (7-l);
        }
        if (rand()%4==0) {
          k = (DC_converter[j].S_bit - 1) / 8;
          l = (DC_converter[j].S_bit - 1) - 8 * k;
          port[k] |= 1 << (7-l);
        }
      }
      else {
        if (rand()%40==0) {
          k = (DC_converter[j].P_bit - 1) / 8;
          l = (DC_converter[j].P_bit - 1) - 8 * k;
          port[k] |= 1 << (7-l);
        }
        if (rand()%40==0) {
          k = (DC_converter[j].S_bit - 1) / 8;
          l = (DC_converter[j].S_bit - 1) - 8 * k;
          port[k] |= 1 << (7-l);
        }
      }
    }
    
    if (rand()%2==0) {
      k = (IMBP.P_bit - 1) / 8;
      l = (IMBP.P_bit - 1) - 8 * k;
      port[k] |= 1 << (7-l);
    }
    if (rand()%2==0) {
      k = (IMBP.S_bit - 1) / 8;
      l = (IMBP.S_bit - 1) - 8 * k;
      port[k] |= 1 << (7-l);
    }
    
    if (rand()%2==0) {
      k = (heater.P_bit - 1) / 8;
      l = (heater.P_bit - 1) - 8 * k;
      port[k] |= 1 << (7-l);
    }
    if (rand()%2==0) {
      k = (heater.S_bit - 1) / 8;
      l = (heater.S_bit - 1) - 8 * k;
      port[k] |= 1 << (7-l);
    }
    
    printf("%d %02X %02X %02X\n",ii,port[0],port[1],port[2]);

    AMS_block_body[15] = port[0]<<8 | port[1];
    AMS_block_body[16] = port[2]<<8;
    AMS_block_length = 1 + 3 + 14;
  
    return;
  }
  
//*********************************************
//*                                           *
//*         simulate temperature data         *
//*         (a.k.a. DS1820 or Dallas)         *
//*                                           *
//*********************************************

  if (iii == 2) {

    AMS_block_ID = GET_REPLY_block_type << 13 |
                   PSB_node_type        <<  9 |
                   PSB_node_number[ii]  <<  6 |
                   TEMP_data_type;
    
    temperature = (rand() % 100) - 50;
    if (rand() % 16 == 0) temperature = 0x1FFF / 2;
    AMS_block_body[3] = 2 * temperature;
    temperature = (rand() % 100) - 50;
    if (rand() % 16 == 0) temperature = 0x1FFF / 2;
    AMS_block_body[4] = 2 * temperature;

    printf("temp: %d %03d %03d \n",ii,AMS_block_body[3],AMS_block_body[4]);

    AMS_block_length = 1 + 3 + 64;
    
    return;
  }
  
}

//------------------------------------------------------------------------------

void initialize_PSB_page( void) {
  int i, j;
  static short first = TRUE;
  if (first) {
    first = FALSE;
    PSB_grand_total.power.capacity = 0;
    for (i=0; i<4; i++) {
      PSB[i].total.power.capacity = 0;
      for (j=0; j<10; j++) {
        if (DC_converter[j].is_present_in.PSB[i] &&
            !((i == 2) && (j == 5))) {
          if (DC_converter[j].nb_of_outputs == 1) {
            PSB[i].total.power.capacity += 30.0 * 2.0;
          }
          else {
            PSB[i].total.power.capacity += 17.5 * 2.0;
          }
        }
      }
      PSB_grand_total.power.capacity += PSB[i].total.power.capacity;
    }
  }
}

//------------------------------------------------------------------------------

void process_PSB_block( void) {

  int i, ii, j, k, l, delta, V_index, I_index;
  unsigned char port[3];
  
//*******************************
//*                             *
//*  PSB block structure checks *
//*                             *
//*******************************

  if ((block_type != GET_REPLY_block_type) &&
      (block_type != EXE_REPLY_block_type) &&
      (block_type != HK_DATA_block_type  )) {
    display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
    sprintf( b, "PSB block with wrong block type %X(16)", block_type);
    write_log_pipe( b);
    return;
  }

  if (node_type != PSB_node_type) {
    display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
    sprintf( b, "PSB block with wrong node type %X(16)", node_type);
    write_log_pipe( b);
    return;
  }  

  ii = PSB_index[node_number];
  
  if (ii < 0) {
    display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
    sprintf( b, "PSB block with wrong node number %X(16)", node_number);
    write_log_pipe( b);
    return;
  }
  
//*********************************************
//*                                           *
//*       process voltage and current data    *
//*                                           *
//*********************************************
  
  if ((data_type & 0x1F) == VOLT_data_type) {
  
    if (AMS_block_length != (1+3+32)) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
      sprintf( b, "PSB VOLT_data_type block with wrong length %d(10)",
               AMS_block_length);
      write_log_pipe( b);
      return;
    }

    if (SIMULATION) {
//    if (time(NULL)%60<5 && ii==1) return; // simulate old data
    }
    
    PSB[ii].ADC_time = time(NULL); 
    PSB[ii].total.power.used = 0.0;
    
    for (j=0; j<10; j++) {
      for (k=0; k<DC_converter[j].nb_of_outputs; k++) {
        V_index = DC_converter[j].output[k].V_index - 1;
        I_index = DC_converter[j].output[k].I_index - 1;
        PSB[ii].DC_converter[j].output[k].voltage =
          (float)(short)AMS_block_body[V_index+3] / 1000.;
        PSB[ii].DC_converter[j].output[k].current = 
          fabs((float)(short)AMS_block_body[I_index+3] / 1000.);
        if (DC_converter[j].is_present_in.PSB[ii]) {
          PSB[ii].total.power.used +=  
            PSB[ii].DC_converter[j].output[k].power = 
            fabs(PSB[ii].DC_converter[j].output[k].voltage) *
            PSB[ii].DC_converter[j].output[k].current;
        }
      }
    }
    
    PSB_grand_total.power.used = 0.0;
    for (i=0; i<4; i++) {
      PSB_grand_total.power.used += PSB[i].total.power.used;
    }

    for (j=0; j<10; j++) {
      if (DC_converter[j].is_present_in.PSB[ii]) {
        for (k=0; k<DC_converter[j].nb_of_outputs; k++) {
          PSB[ii].DC_converter[j].output[k].status = NORMAL;
        }
      }
    }
    PSB[ii].total.power.status = NORMAL;
    PSB_grand_total.power.status = NORMAL;
  
    return;
  }

//*********************************************
//*                                           *
//* process primary/secondary MDI & HE & IMBP *
//*           (a.k.a. digital I/O)            *
//*                                           *
//*********************************************
  
  if ((data_type & 0x1F) == STAT_data_type) {

//  if (AMS_block_length != (1+3+14)) {
    if (AMS_block_length != (1+3+16)) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
      sprintf( b, "PSB STAT_data_type block with wrong length %d(10)",
               AMS_block_length);
      write_log_pipe( b);
      return;
    }

    if (SIMULATION) {
//    if (time(NULL)%60<5 && ii==3) return; // simulate old data
    }
    
    PSB[ii].digital_IO_time = time( NULL);
     
    port[0] = AMS_block_body[15] >> 8;
    port[1] = AMS_block_body[15] & 0xFF;
    port[2] = AMS_block_body[16] >> 8;
    
    if (ii == 1 || ii == 3) PSB[ii].total.power.enabled = 60.0;
    else                    PSB[ii].total.power.enabled =  0.0;

    for (j=0; j<10; j++) {
      PSB[ii].DC_converter[j].state = 0x0;
      k = (DC_converter[j].P_bit - 1) / 8;
      l = (DC_converter[j].P_bit - 1) - 8 * k;
      if (port[k] & (1 << (7-l))) {
        if (DC_converter[j].is_present_in.PSB[ii] &&
            !((ii == 1 || ii == 3) && (j == 5))   &&
            !((ii == 2)            && (j == 5))) {
        PSB[ii].DC_converter[j].state |= 0x1;
          if (DC_converter[j].nb_of_outputs == 1) {
            PSB[ii].total.power.enabled += 30.0;
          }
          else {
            PSB[ii].total.power.enabled += 17.5;
          }
        }
      }
      k = (DC_converter[j].S_bit - 1) / 8;
      l = (DC_converter[j].S_bit - 1) - 8 * k;
      if (port[k] & (1 << (7-l))) {
        PSB[ii].DC_converter[j].state |= 0x2;
        if (DC_converter[j].is_present_in.PSB[ii] &&
            !((ii == 1 || ii == 3) && (j == 5))   &&
            !((ii == 2)            && (j == 5))) {
          if (DC_converter[j].nb_of_outputs == 1) {
            PSB[ii].total.power.enabled += 30.0;
          }
          else {
            PSB[ii].total.power.enabled += 17.5;
          }
        }
      }
      if (PSB[ii].DC_converter[j].state == 0x3) {
        PSB[ii].DC_converter[j].status = WARNING;
      }
      else {
        PSB[ii].DC_converter[j].status = NORMAL;
      }
    }

//-- IMBP bit

    PSB[ii].IMBP.state = 0x0;
    k = (IMBP.P_bit - 1) / 8;
    l = (IMBP.P_bit - 1) - 8 * k;
    if (port[k] & (1 << (7-l))) PSB[ii].IMBP.state |= 0x1;
    k = (IMBP.S_bit - 1) / 8;
    l = (IMBP.S_bit - 1) - 8 * k;
    if (port[k] & (1 << (7-l))) PSB[ii].IMBP.state |= 0x2;

    if (PSB[ii].IMBP.state == 0x0) PSB[ii].IMBP.status = ALARM;
    if (PSB[ii].IMBP.state == 0x1) PSB[ii].IMBP.status = WARNING;
    if (PSB[ii].IMBP.state == 0x2) PSB[ii].IMBP.status = WARNING;
    if (PSB[ii].IMBP.state == 0x3) PSB[ii].IMBP.status = NORMAL;

//-- PSB heaters
    
    PSB[ii].heater.ENABLED = FALSE;
    PSB[ii].heater.ON = FALSE;
    k = (heater.P_bit - 1) / 16;
    l = (heater.P_bit - 1) % 16;
    if (AMS_block_body[11+k] & (1 << (15-l))) PSB[ii].heater.ENABLED = TRUE;
    if (AMS_block_body[15+k] & (1 << (15-l))) PSB[ii].heater.ON = TRUE;
    k = (heater.S_bit - 1) / 16;
    l = (heater.S_bit - 1) % 16;
    if (AMS_block_body[11+k] & (1 << (15-l))) PSB[ii].heater.ENABLED = TRUE;
    if (AMS_block_body[15+k] & (1 << (15-l))) PSB[ii].heater.ON = TRUE;

    if (PSB[ii].heater.ON && !PSB[ii].heater.ENABLED) {
      PSB[ii].heater.status = WARNING;
    }
    else {
      PSB[ii].heater.status = NORMAL;
    }
    
    PSB_grand_total.power.enabled = 0.0;
    for (i=0; i<4; i++) {
      PSB_grand_total.power.enabled += PSB[i].total.power.enabled;
    }

    return;
  }
 
//*********************************************
//*                                           *
//*         process temperature data          *
//*         (a.k.a. DS1820 or Dallas)         *
//*                                           *
//*********************************************

  if ((data_type & 0x1F) == TEMP_data_type) {

    if (AMS_block_length != (1+3+64)) {
      display_message( 25, error_in_BLK_col, error_in_BLK_col+2, alarm, "BLK");
      sprintf( b, "PSB TEMP_data_type block with wrong length %d(10)",
               AMS_block_length);
      write_log_pipe( b);
      return;
    }

    if (SIMULATION) {
//    if (time(NULL)%60<5 && ii==3) return; // simulate old data
    }

    PSB[ii].temperature_time = time(NULL);
    PSB[ii].temperature[0].value = ((short)AMS_block_body[3]) / 2;
    PSB[ii].temperature[1].value = ((short)AMS_block_body[4]) / 2;

    PSB[ii].total.temperature.delta = 
      abs( PSB[ii].temperature[0].value -
           PSB[ii].temperature[1].value);

    for (j=0; j<2; j++) {
      PSB[ii].temperature[j].status = NORMAL;
      if (PSB[ii].temperature[j].value < -20) PSB[ii].temperature[j].status = WARNING;
      if (PSB[ii].temperature[j].value < -10) PSB[ii].temperature[j].status = ALARM;
      if (PSB[ii].temperature[j].value > +60) PSB[ii].temperature[j].status = WARNING;
      if (PSB[ii].temperature[j].value > +65) PSB[ii].temperature[j].status = ALARM;
    }

    PSB[ii].total.temperature.status = NORMAL;
    if (PSB[ii].total.temperature.delta > 30) PSB[ii].total.temperature.status = WARNING;
    if (PSB[ii].total.temperature.delta > 50) PSB[ii].total.temperature.status = ALARM;
  
    return;
  }

}

//------------------------------------------------------------------------------

void display_PSB_right_panel( short row, short col) {

  int i;
  
  set_colors( headline); 
  
  for (i=row; i<PSB_total_power_row-1; i++) display_line(i, col, 80, " ");

  _settextposition( ++row, col+1);
  _outtext("Color coding:");
  
  _settextposition( ++row, col+1);
  set_colors( frame); 
  _outtext("7 x");  
  set_colors( headline); 
  _outtext(" - constant");

  _settextposition( ++row, col+1);
  set_colors( old_data); 
  _outtext("   ");  
  set_colors( headline); 
  _outtext(" - no data");  
  
  _settextposition( ++row, col+1);
  set_colors( normal); 
  _outtext("0.1");  
  set_colors( headline); 
  _outtext(" - normal");    

  _settextposition( ++row, col+1);
  set_colors( warning); 
  _outtext("0.5");  
  set_colors( headline); 
  _outtext(" - warning");

  _settextposition( ++row, col+1);
  set_colors( alarm); 
  _outtext("0.9");  
  set_colors( headline); 
  _outtext(" - alarm");  

  _settextposition( ++row, col+1);
  set_colors( old_data); 
  _outtext("1.5");  
  set_colors( headline); 
  _outtext(" - old/bad");  
  
  _settextposition( row+=2, col+1);
  _outtext("Ps  - Power");
  _settextposition( ++row, col+1);
  _outtext("      supply");
  _settextposition( ++row, col+1);
  _outtext("St  - State:");
  _settextposition( ++row, col+1);
  _outtext("  P = Primary");
  _settextposition( ++row, col+1);
  _outtext("  S = Secondary");  

  _settextposition( ++row, col+1);
//  _outtext("HE  - Heater");
  _outtext("H   - Heater");
//  _settextposition( ++row, col+1);
//  _outtext("      Elements");

  _settextposition( ++row, col+1);
  _outtext("UPS - UPS");

  _settextposition( ++row, col+1);
  _outtext("CAN - CAN");
}

//------------------------------------------------------------------------------
    
void display_PSB_data_frame( void) {

  int i, j, k;
  
  set_colors( frame); 

  _settextposition( DC_converter[0].output[0].row-2, 1);
  sprintf( b, 
  "       %s--- %5s ---%s--- %5s ---%s--- %5s ---%s--- %5s ---%s",
  VBAR, PSB_name[0], VBAR, PSB_name[1], 
  VBAR, PSB_name[2], VBAR, PSB_name[3], VBAR); 
  _outtext( b);

  _settextposition( DC_converter[0].output[0].row-1, 1);
  sprintf( b,
  "Ps Vnom%s St   V   I  %s St   V   I  %s St   V   I  %s St   V   I  %s",
  VBAR, VBAR, VBAR, VBAR, VBAR);
  _outtext( b);

  _settextposition( DC_converter[9].output[0].row+1, 1);
  sprintf( b, 
  "       %s--- %5s ---%s--- %5s ---%s--- %5s ---%s--- %5s ---%s",
  VBAR, PSB_name[0], VBAR, PSB_name[1], 
  VBAR, PSB_name[2], VBAR, PSB_name[3], VBAR); 
  _outtext( b);

 
  for (j=0; j<10; j++) {
    for (k=0; k<DC_converter[j].nb_of_outputs; k++) {
      sprintf( b, "%1.1s %5.5s",
      DC_converter[j].name,
      DC_converter[j].output[k].name);
      _settextposition( DC_converter[j].output[k].row, 1);
      _outtext(b);
      for (i=0; i<4; i++) {
        _settextposition( DC_converter[j].output[k].row, PSB_col[i]-1);
        if (DC_converter[j].is_present_in.PSB[i]) {
          if ((i == 1 || i == 3) && (j == 5)) sprintf( b, "%sUPS", VBAR);
          else if ((i == 2) && (j == 5))      sprintf( b, "%sCAN", VBAR);
               else                           sprintf( b, "%s", VBAR);
        }
        else {
          sprintf( b, "%s             ", VBAR);
        }
        _outtext( b);
      }
      _settextposition( DC_converter[j].output[k].row, PSB_col[3]+13);
      sprintf( b, "%s", VBAR);
      _outtext( b);
    }
  }

  _settextposition( DC_converter[2].output[0].row+1, 1);
  _outtext( "       ");
  for (i=0; i<4; i++) {
    _settextposition( DC_converter[2].output[0].row+1, PSB_col[i]-1);
    sprintf( b, "%s             ", VBAR);
    _outtext( b);
  }
  _settextposition( DC_converter[2].output[0].row+1, PSB_col[3]+13);
  sprintf( b, "%s", VBAR);
  _outtext( b);
  
  _settextposition( temperature_row, 1);
//  sprintf( b, "Temp&HE%s", VBAR);
  sprintf( b, "T,H,IMB%s", VBAR);
  _outtext( b);
  for ( i=0; i<4; i++) {
    _settextposition( temperature_row, PSB_col[i]+13);
    sprintf( b, "%s", VBAR);
    _outtext( b);
  }

  _settextposition( PSB_total_power_row-1, 1);
  sprintf( b, "Power: %s", VBAR);
  _outtext( b);
  for ( i=0; i<4; i++) {
    _settextposition( PSB_total_power_row-1, PSB_col[i]);
    sprintf( b, "enabld / used%s", VBAR);
    _outtext( b);
  }
  sprintf( b, " ENABLED / USED ");
  _outtext( b);
  
  _settextposition( PSB_total_power_row, 1);
  sprintf( b, "       %s", VBAR);
  _outtext( b);
  for ( i=0; i<4; i++) {
    _settextposition( PSB_total_power_row, PSB_col[i]+6);
    sprintf( b, " / ");
    _outtext( b);
    _settextposition( PSB_total_power_row, PSB_col[i]+13);
    sprintf( b, "%s", VBAR);
    _outtext( b);
  }
  
  _settextposition( PSB_grand_total_power_row, PSB_grand_total_power_col);
  sprintf( b, "%s =", SIGMA);
  _outtext( b);

  _settextposition( PSB_grand_total_power_row, PSB_grand_total_power_col+8);
  sprintf( b, " / ");
  _outtext( b);
  
}

//------------------------------------------------------------------------------

void display_PSB_data( void) {

  int i, j, k, m;
  time_t now;

  if (page_frozen) return;
  
  now = time( NULL);

//  --- display DC_converters state (P,S,PS,UPS) ---

  for (i=0; i<4; i++) {
    if (PSB[i].digital_IO_time) data_are_absent = FALSE;
    else                        data_are_absent = TRUE;
    if (now - PSB[i].digital_IO_time > PSB_digital_IO_time_staleness) data_are_old = TRUE;
    else                                                              data_are_old = FALSE;
    for (j=0; j<10; j++) {
      for (k=0; k<DC_converter[j].nb_of_outputs; k++) {
        if (DC_converter[j].is_present_in.PSB[i] &&
            !((i == 1 || i == 3) && (j == 5))    &&
            !((i == 2)           && (j == 5))) {
          if (PSB[i].DC_converter[j].state & 0x1) m  = sprintf( b,   " P");
          else                                    m  = sprintf( b,   "  ");
          if (PSB[i].DC_converter[j].state & 0x2) m += sprintf( b+m, "S");
          else                                    m += sprintf( b+m, " ");
          set_value_colors( PSB[i].DC_converter[j].status);
          _settextposition( DC_converter[j].output[k].row, PSB_col[i]);
          _outtext( b);
        }
      }
    }
  }

//  --- display HE state (D+ etc) amd IMBP state ---

  for (i=0; i<4; i++) {
    if (PSB[i].digital_IO_time) data_are_absent = FALSE;
    else                        data_are_absent = TRUE;
    if (now - PSB[i].digital_IO_time > PSB_digital_IO_time_staleness) data_are_old = TRUE;
    else                                                              data_are_old = FALSE;
    if (PSB[i].heater.ENABLED) m  = sprintf( b,   "H=E");
    else                       m  = sprintf( b,   "H=D");
    if (PSB[i].heater.ON)      m += sprintf( b+m, "+ ");
    else                       m += sprintf( b+m, "- ");
    set_value_colors( PSB[i].heater.status);
    _settextposition( temperature_row, PSB_col[i]+7);
    _outtext( b);
    set_value_colors( PSB[i].IMBP.status);
    _settextposition( temperature_row, PSB_col[i]+12);
    if (PSB[i].IMBP.state == 0x3) sprintf( b, " ");
    else                          sprintf( b, "%d", PSB[i].IMBP.state);
    _outtext( b);
  }

//  --- display DC-converters voltages and currents ---

  for (i=0; i<4; i++) {
    if (PSB[i].ADC_time) data_are_absent = FALSE;
    else                 data_are_absent = TRUE;
    if (now - PSB[i].ADC_time > PSB_ADC_time_staleness) data_are_old = TRUE;
    else                                                data_are_old = FALSE;
    for (j=0; j<10; j++) {
      for (k=0; k<DC_converter[j].nb_of_outputs; k++) {
        if (DC_converter[j].is_present_in.PSB[i]) {
          sprintf( b, "%5.1f%4.1f ",
                   PSB[i].DC_converter[j].output[k].voltage,
                   PSB[i].DC_converter[j].output[k].current);
          set_value_colors( PSB[i].DC_converter[j].output[k].status);
          _settextposition( DC_converter[j].output[k].row, PSB_col[i]+3);
          _outtext( b);
        }
      }
    }
  }
      
//  --- display temperatures ---

  for (i=0; i<4; i++) {
    if (PSB[i].temperature_time) data_are_absent = FALSE;
    else                         data_are_absent = TRUE;
    if (now - PSB[i].temperature_time > PSB_temperature_time_staleness) data_are_old = TRUE;
    else                                                                data_are_old = FALSE;
    _settextposition( temperature_row, PSB_col[i]);
    for ( j=0; j<2; j++) {
      if (abs(PSB[i].temperature[j].value) > 99) {
        sprintf( b, "XXX");
      }
      else {
        sprintf( b, "%+3d", PSB[i].temperature[j].value);
      }
      set_value_colors( PSB[i].temperature[j].status);
      _outtext( b);
    }
    set_value_colors( PSB[i].total.temperature.status);
    if (PSB[i].total.temperature.status == NORMAL) _outtext( " ");
    else                                           _outtext( "!");
  }

//  --- display total PSBs powers enabled ---

  for (i=0; i<4; i++) {
    if (PSB[i].digital_IO_time) data_are_absent = FALSE;
    else                        data_are_absent = TRUE;
    if (now - PSB[i].digital_IO_time > PSB_digital_IO_time_staleness) data_are_old = TRUE;
    else                                                              data_are_old = FALSE;
    sprintf( b, " %4.0fW", PSB[i].total.power.enabled);
    set_value_colors( NORMAL);
    _settextposition( PSB_total_power_row, PSB_col[i]);
    _outtext( b);
  }

//  --- display total PSBs powers used ---

  for (i=0; i<4; i++) {
    if (PSB[i].ADC_time) data_are_absent = FALSE;
    else                 data_are_absent = TRUE;
    if (now - PSB[i].ADC_time > PSB_ADC_time_staleness) data_are_old = TRUE;
    else                                                data_are_old = FALSE;
    sprintf( b, "%3.0fW", PSB[i].total.power.used);
    set_value_colors( PSB[i].total.power.status);
    _settextposition( PSB_total_power_row, PSB_col[i]+9);
    _outtext( b);
  }

//  --- display grand total (all PSB) power enabled ---
  
  data_are_absent = TRUE;
  data_are_old = FALSE;
  for (i=0; i<4; i++) {
    if (PSB[i].digital_IO_time) data_are_absent = FALSE;
    if (now - PSB[i].digital_IO_time > PSB_digital_IO_time_staleness) data_are_old = TRUE;
  }
  sprintf (b, "%4.0fW", PSB_grand_total.power.enabled);
  _settextposition( PSB_grand_total_power_row, PSB_grand_total_power_col+3);
  set_value_colors( NORMAL);
  _outtext( b);

//  --- display grand total (all PSB) power used ---

  data_are_absent = TRUE;
  data_are_old = FALSE;
  for (i=0; i<4; i++) {
    if (PSB[i].ADC_time) data_are_absent = FALSE;
    if (now - PSB[i].ADC_time > PSB_ADC_time_staleness) data_are_old = TRUE;
  }
  sprintf( b, "%4.0fW", PSB_grand_total.power.used);
  _settextposition( PSB_grand_total_power_row, PSB_grand_total_power_col+11);
  set_value_colors( PSB_grand_total.power.status);
  _outtext( b);

}

//------------------------------------------------------------------------------

void display_PSB_page( void) {
  short row, col;
  display_headline( "CRATE POWER SUPPLY PAGE");
  if (new_page) {
    display_PSB_data_frame();
    display_PSB_right_panel( row=2, col=65);
  }
  display_PSB_data();
}
 
//------------------------------------------------------------------------------
