// this is file p.c
//
// AMS 1553 bus monitor for STS PGSC computer (PCMCIA 1553 interface only)
//
// - various functions for 1553 PCMCIA interface (BU-65550)
//
// MODIFIED BY VK AT THE END OF 1997 *******
//
//  most texts were taken from VK's "k3.c" file
//  modified to run under Windows-95 on 03/10/97 by VK himself
//  modified once more by VK himself on 07/10/97
//

#include "pgsc.h"

static int OPEN = FALSE;
static int cntr = 0;
static FILE *frames;

// #define TIME_TAG_RES 3 // 0->64us, 1->32us, 2->16us, 3->8us, 4->4us, 5->2us

typedef  struct {
  unsigned int block_status;
  unsigned int time_tag;               
  unsigned int data_pointer;
  unsigned int command_word;
} bu_descriptor_t;

static struct bu_registers {
  unsigned int interrupt_mask;
  unsigned int config_1;
  unsigned int config_2;
  unsigned int start_reset;
  unsigned int control_word;
  unsigned int time_tag;
  unsigned int interrupt_status;
  unsigned int config_3;
  unsigned int config_4;
  unsigned int config_5;
  unsigned int data_stack_pointer;
  unsigned int bc_frame_time;
  unsigned int bc_message_time;
  unsigned int mt_trigger;
  unsigned int rt_status;
  unsigned int rt_bit;
} far *reg;

#define command_stack_pointer start_reset

static struct bu_memory {
  bu_descriptor_t stack_A[64];               // 000   0x000
  unsigned int rt_cmd_pointer_A;             // 256   0x100
  unsigned int reserved_1;                   // 257   0x101
  unsigned int mt_cmd_pointer_A;             // 258   0x102
  unsigned int mt_dat_pointer_A;             // 259   0x103
  unsigned int rt_cmd_pointer_B;             // 260   0x104
  unsigned int reserved_2;                   // 261   0x105
  unsigned int mt_cmd_pointer_B;             // 262   0x106
  unsigned int mt_dat_pointer_B;             // 263   0x107
  unsigned int mode_code_interrupt[8];       // 264   0x108
  unsigned int mode_code_data[48];           // 272   0x110
  unsigned int rt_lookup_table_A[128];       // 320   0x140
  unsigned int rt_lookup_table_B[128];       // 448   0x1c0
  unsigned int busy_lookup[8];               // 576   0x240
  unsigned int unused_1[24];                 // 584   0x248
  unsigned int data_block_s[32];             // 608   0x260
  unsigned int mt_lookup_table[128];         // 640   0x280
  unsigned int command_illegal[256];         // 768   0x300
  bu_descriptor_t mt_cmd_A[64];              //1024   0x400
  bu_descriptor_t mt_cmd_B[64];              //1280   0x500
  unsigned int unused_2[512];                //1536   0x600
  unsigned int mt_dat_A[1024];               //2048   0x800
  unsigned int mt_dat_B[1024];               //3072   0xc00
} far *mem;
                                                                    
// static PCCardStruct CardInfo = PC_CARD_DEFAULTS;  //               !!!
static unsigned int get, put, ovf, data_index, bu_irq;

void static interrupt far bu_interrupt_handler(void);
void static (interrupt far *old_interrupt_handler)(void);
unsigned int bu_interrupt_status;         ///// 28/12/97
unsigned int bu_interrupt_vector_set = 0; ///// 28/12/97
//--------------------------------------------------------------------------

unsigned int VK_index_n;

// here comes kk3.h file (PC buffer memory for PCMCIA card)

#define CMD_SIZE 100
#define DAT_SIZE 5000

typedef  struct {
  unsigned int block_status;   // Message Monitor Mode Block Status Word
  unsigned int handler_status; // Interrupt Handler Status Word
  unsigned int data_pointer;   // Index in dat_buffer of first data word
  unsigned int command_word;   // 1553 Command Word
} pc_descriptor_t;

unsigned int    dat_buffer[DAT_SIZE];
pc_descriptor_t cmd_buffer[CMD_SIZE];

/* Message Monitor Mode Block Status Word
bit
15 - End Of Message (EOM)
14 - Start Of Message (SOM)
13 - Channel B/A*
12 - Error Flag
11 - RT-to-RT Transfer
10 - Format Error
 9 - No Response Timeout
 8 - Good Data Block Transfer
 7 - Data Stack Rollover
 6 - Reserved
 5 - Word Count Error
 4 - Incorrect Sync
 3 - Invalid Word
 2 - RT-to-RT Gap/Sync/Address Error
 1 - RT-to-RT 2nd Command Error
 0 - Command Word Contents Error
*/
/* Interrupt Handler Status Word
bit
 2 - PC Command Buffer Overrun
 1 - Command Stack Rollover
 0 - Data Stack Rollover
*/

int nb_of_simulated_1553_words;
int index_of_simulated_1553_word;

// ---- function prototypes (may be seperate include file should be used)

//      functions defined in file "p.c"

void terminate_1553_bus_monitor( void);
int initialize_1553_bus_monitor( int slot);
void prepare_simulated_1553_data( void);
void initialize_1553_hardware( void);
int get_1553_message( void);
int new_1553_message( void);
void process_1553_message( void);

void COPY_A( void);
void COPY_B( void);
//------------------------------------------------------------------------------

void terminate_1553_bus_monitor( void) {
// this is almost unmodified VK's function "mt_disable"
/*
  resets BU65550,
  disable IRQ in i8259 interrupt controller,
  restors old IRQ vector,
  releases Memory & Register Windows,
  releases Interrupt Request Level (IRQ),             **** obsolete
  deregisters application (Client),                   **** obsolete
*/
  reg->start_reset = 1;
// the code below was inserted by VK himself on 03/10/97
  if (bu_irq > 7) {
    _disable();
    outp( 0xa1, (inp( 0xa1) | (1<<(bu_irq-8))));
    _enable();
    if (bu_interrupt_vector_set)   //// 28/12/97
      _dos_setvect( bu_irq + 0x68, old_interrupt_handler);
  }
  else {
    _disable();
    outp( 0x21, (inp( 0x21) | (1<<bu_irq)));
    _enable();
    if (bu_interrupt_vector_set)   //// 28/12/97
      _dos_setvect( bu_irq + 0x08, old_interrupt_handler);
  }

/*   the following was removed on 03/10/97 following VK's advices over my shoulder

  Disable_BU65550( &CardInfo);
  BU_DeregisterClient();
  
*/

}

//------------------------------------------------------------------------------

int initialize_1553_bus_monitor( int slot) {
// this is almost unmodified VK's function "mt_enable"
/* 
   registers application (Client) to work with PC Card in slot "slot", **** obsolete
   gets Interrupt Request Level (IRQ),                                 **** obsolete
   maps BU65550 Interrupts to IRQ,                                     **** obsolete
   gets Memory & Register Windows,                                     **** obsolete
   maps Memory & Register Windows to available PC address space,       **** obsolete
   initializes BU65550 to work in Messages Monitor Mode,
   set interrupt handler variables,
   saves IRQ vector,
   set IRQ vector to point to mt_interrupt_handler,
   enable IRQ in i8259 interrupt controller,
   starts BU65550 Message Monitor.
   returns:
   OK   - monitor is started
   FAIL - error was found
*/
  int  interrupt_request = 9;
  long memory_address    = 0xD2000000;
  long register_address  = 0xD1000000;
  FILE *configuration_file;
  char string[81];
  long value;
  int i;
    
  atexit( terminate_1553_bus_monitor);  // is it right place for this call ?

//   the following was removed on 03/10/97 following VK's advices over my shoulder
/*
  if (BU_RegisterClient()) {
    printf( "CS Register Client error\n");
    return( FAIL);
  }

  CardInfo.Socket = slot;
  if (Enable_BU65550( &CardInfo)) {
    printf( "65550 Enable error\n");
    return( FAIL);
  }

  bu_irq = CardInfo.IrqLevel;
  mem = (struct bu_memory far *)CardInfo.MemPointer;
  reg = (struct bu_registers far *)CardInfo.RegPointer;

*/

//bu_irq = 9;
//mem = (struct bu_memory    far *) 0xd2000000;
//reg = (struct bu_registers far *) 0xd1000000;

  configuration_file = fopen( "C:\\ACESOFT\\ACE.CFG", "r");
  if (configuration_file != NULL) {
    printf( "Reading configuration file...");
    while (fgets( b, 80, configuration_file) != NULL) {
      sscanf( b, "%s = %lx", string, &value);
      if (!strcmp( string, "interrupt")) interrupt_request = value;
      if (!strcmp( string, "register"))  register_address  = value << 16;
      if (!strcmp( string, "memory"))    memory_address    = value << 16;
    }
  }  
  printf( "interrupt = %d\nregister  = %8.8lX\nmemory    = %8.8lX\n",
           interrupt_request, register_address, memory_address);
 
  bu_irq = interrupt_request;
  mem    = (struct bu_memory    far *) memory_address;
  reg    = (struct bu_registers far *) register_address;

  reg->start_reset = 0x0001;
  reg->interrupt_mask = 0x55aa;
  if (reg->interrupt_mask != 0x55aa) {
    printf( "BU-65550 register read/write error\n");
    return( FAIL);
  }

  mem->reserved_1 = 0xaa55;
  if (mem->reserved_1 != 0xaa55) {
    printf( "BU-65550 memory read/write error\n");
    return( FAIL);
  }

  reg->config_3 = 0x8000; // enhanced mode
  reg->config_3 = 0x8600; // 256W monitor command stack, 1KW monitor data stack
  reg->config_1 = 0x5000; // message monitor
  /*
  reg->config_2 = 0x8018 | (TIME_TAG_RES<<7);
                          // interrupt status auto clear,level interrupt request
  */                        
/*  ///// 28/12/97
  reg->config_2 = 0x8018; // interrupt status auto clear,level interrupt request
*/
  reg->config_2 = 0x8318; // interrupt status auto clear,level interrupt request
                          // time tag in test mode
  reg->config_4 = 0x0008;
  reg->config_5 = 0x0002; // rt #1
/*  ///// 28/12/97
  reg->interrupt_mask = 0x0001;
                          // end of message interrupt
*/
  reg->interrupt_mask = 0x0041;
                          // end of message interrupt
                          // enable interrupt on time tag rollover
  mem->mt_cmd_pointer_A = offsetof( struct bu_memory, mt_cmd_A) / 2;
  mem->mt_dat_pointer_A = offsetof( struct bu_memory, mt_dat_A) / 2;
  mem->mt_cmd_pointer_B = offsetof( struct bu_memory, mt_cmd_B) / 2;
  mem->mt_dat_pointer_B = offsetof( struct bu_memory, mt_dat_B) / 2;

  for (i=0; i<128; i++) mem->mt_lookup_table[i] = 0xffff;

  get = CMD_SIZE - 1;
  put = 0;
  data_index = 0;
  ovf = 0;

// the code below was inserted by VK himself on 03/10/97
  if (bu_irq > 7) {
   old_interrupt_handler = _dos_getvect( bu_irq + 0x68);
    _dos_setvect( bu_irq + 0x68, bu_interrupt_handler);
    _disable();
    outp( 0xa1, (inp( 0xa1) & ~(1<<(bu_irq-8))) );
    _enable();
  }
  else {
    old_interrupt_handler = _dos_getvect( bu_irq + 0x08);
    _dos_setvect( bu_irq + 0x08, bu_interrupt_handler);
    _disable();
    outp( 0x21, (inp( 0x21) & ~(1<<bu_irq)) );
    _enable();
  }
//// 28/12/97  
  bu_interrupt_vector_set = 1;
  bu_interrupt_status = 0;
  // load time tag register with full scale value
  reg->time_tag = 0xffff;
  // increment time tag register (time tag will rollover to 0)
  reg->start_reset = 0x10;
  // verify that time tag register rolled over
  if (reg->time_tag != 0) {
    printf( "BU-65550 time tag increment error\n");
    return( FAIL);
  }
  if ((bu_interrupt_status & 0x8040) != 0x8040) {
    printf( "BU-65550 time tag rolled over interrupt error\n");
    return( FAIL);
  }
  reg->config_2 = 0x8018; 
                       // interrupt status auto clear,level interrupt request
  reg->interrupt_mask = 0x0001;
                       // enable end of message interrupt only
                       
  reg->start_reset = 0x0002; // start monitor
  return( OK);
}

//------------------------------------------------------------------------------

#define NN (62*( 1+1 +1+1+32 +1+1+32))

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
  if (input == NULL) {
    printf("File 'HKframes' is absent.\nProgram terminated.\n");
    exit( 0);
  }
  for (i=0; i<NN; i++) {
    if (fgets( line, 80, input) == NULL) {
      printf("EOF after %d lines\n",nb_of_simulated_1553_words);
      return;
    }
    else {
      ++nb_of_simulated_1553_words;
      sscanf( line,   "%d", &simulated_1553_tag[i]);
      sscanf( line+2, "%x", &simulated_1553_word[i]);
//    printf( "line=%s", line);
//    printf( "tag = %d         ", tag);
//    printf( "number = %4.4x \n", simulated_1553_word[i]);
//    if (i==2) exit(0);
    }
  }
  printf("all %d words will be used\n",nb_of_simulated_1553_words);
}

//------------------------------------------------------------------------------

void initialize_1553_hardware( void) {
  if (initialize_1553_bus_monitor( 0) == FAIL) {
    terminate_1553_bus_monitor();  // termination call again at exit - ?
    SIMULATION = TRUE;
/*
    prepare_simulated_1553_data();
*/
    I_am_in_BM_mode = FALSE;
    I_am_in_BC_mode = TRUE;
    I_am_in_RT_mode = FALSE;
  }
  else {
    SIMULATION = FALSE;
    I_am_in_BM_mode = TRUE;
    I_am_in_BC_mode = FALSE;
    I_am_in_RT_mode = FALSE;
  }
}

//------------------------------------------------------------------------------

int get_1553_message( void) {
// this is almost unmodified VK's function "get_monitor_message"
  unsigned int t;
  t = get + 1;
  if (t >= CMD_SIZE) t = 0;
  if (t == put) return( FAIL);
  VK_index_n = t;
  get = t;
//************************************************************************
//
//if (cmd_buffer[VK_index_n].handler_status) return( 2); else return( 1);
//
// I simply ignore the PC-buffer overrun... (is it safe for a meanwhile - ?)
//
//************************************************************************
  return( OK);
}

//------------------------------------------------------------------------------

int new_1553_message( void) {

  int l1, l2, l3, l4;
  integer16 junk[12];
  integer16 *p;
/*
  if (SIMULATION) {
//  put some data into PC-buffer...
    while (simulated_1553_tag[index_of_simulated_1553_word] != 1) {
      index_of_simulated_1553_word = 
      (index_of_simulated_1553_word + 1) % nb_of_simulated_1553_words;
    }
    cmd_buffer[put].command_word = simulated_1553_word[index_of_simulated_1553_word];
//  printf("%4.4x %i %i\n", cmd_buffer[put].command_word, index_of_simulated_1553_word, put);
    index_of_simulated_1553_word = 
    (index_of_simulated_1553_word + 1) % nb_of_simulated_1553_words;
    cmd_buffer[put].block_status = 0x8100;
    cmd_buffer[put].handler_status = ovf;
    cmd_buffer[put].data_pointer = data_index;
    while (simulated_1553_tag[index_of_simulated_1553_word] != 1) {
      dat_buffer[data_index] = simulated_1553_word[index_of_simulated_1553_word];
//    printf(" %4.4x %i %i\n", dat_buffer[data_index], index_of_simulated_1553_word, data_index);
      data_index = (data_index + 1) % DAT_SIZE;
      index_of_simulated_1553_word = 
      (index_of_simulated_1553_word + 1) % nb_of_simulated_1553_words;
    }
    put = (put + 1) % CMD_SIZE;
  }
*/
  if (SIMULATION) {
    if (!OPEN) {
      if ((frames = fopen( "HKframes", "rb")) == NULL) {
        printf( "\nFile 'HKFRAMES' is absent.  Program terminated.\n");
        exit( 0);
      }
      OPEN = TRUE;
    }
    l1 = l2 = l3 = l4 = 0;
                  l1 = fread(         junk, sizeof( integer16),  6, frames);
    if (l1 ==  6) l2 = fread(      HK.word, sizeof( integer16), 64, frames);
    if (l2 == 64) l3 = fread( block_status, sizeof( integer16),  2, frames);
    if (l3 ==  2) l4 = fread(         junk, sizeof( integer16),  8, frames);
    if (l1 == 6 && l2 == 64 && l3 == 2 && l4 == 8) {
      cntr++;
      if (cntr >= 10) {
        cntr = 0;
        wait( 1);
      }
      p = block_status; 
      (*p) = ((*p) >> 8) | ((*p) << 8);
      p++;
      (*p) = ((*p) >> 8) | ((*p) << 8);
      for (p=HK.word; p<HK.word+64; p++) {
        (*p) = ((*p) >> 8) | ((*p) << 8);
      }
      return TRUE;
    }
    else {
      fclose( frames);
      OPEN = FALSE;
      return FALSE;
    }
  }
  
  if (get_1553_message() == OK) return TRUE;
  return FALSE;
}

//------------------------------------------------------------------------------

void process_1553_message( void) {
// this is **very** modified VK's function "show_message"

  int i;
  static int parser_locked = FALSE;
  static int first = TRUE;
  
  unsigned int data=cmd_buffer[VK_index_n].data_pointer%DAT_SIZE;
  unsigned int command=cmd_buffer[VK_index_n].command_word;
  
  if (SIMULATION) {
    new_frame_number = 1;
    new_time_broadcast = 0;
    new_PSP_frame = 0;
    new_PDI_frame = 1;
    return;
  }
  
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
  
  if (!parser_locked && (command != 0xf811)) goto EXIT;
  
  switch (command) {

    case 0xf811:   // frame number
      parser_locked = TRUE;
      OIU_frame_number = dat_buffer[data];
      OIU_frame_number_block_status = cmd_buffer[VK_index_n].block_status;
      new_frame_number++;
      goto EXIT;

    case 0xfba8:   // time broadcast
      for (i=0; i<TIME_BROADCAST_LENGTH; i++) {
        time_broadcast[i] = dat_buffer[(data++)%DAT_SIZE];
      }
      time_broadcast_block_status = cmd_buffer[VK_index_n].block_status;
      new_time_broadcast++;
      goto EXIT;

    case 0x2360:   // PSP frame first box car RT#4
    case 0xe360:   // PSP frame first box car RT#28
      new_command_box_car[0]++;
      status_word[0] = dat_buffer[(data+32)%DAT_SIZE];
      block_status[0] = cmd_buffer[VK_index_n].block_status;
      for (i=0; i<BOX_CAR_LENGTH; i++) {
        HK.box_car[0][i] = dat_buffer[(data++)%DAT_SIZE];
      }
      goto EXIT;

    case 0x2380:   // PSP frame second box car RT#4
    case 0xe380:   // PSP frame second box car RT#28
      new_command_box_car[1]++;
      status_word[1] = dat_buffer[(data+32)%DAT_SIZE];
      block_status[1] = cmd_buffer[VK_index_n].block_status;
      for (i=0; i<BOX_CAR_LENGTH; i++) {
        HK.box_car[1][i] = dat_buffer[(data++)%DAT_SIZE];
      }
      if (new_command_box_car[0] && new_command_box_car[1]) {
        new_command_box_car[0] = 0;
        new_command_box_car[1] = 0;
        new_PSP_frame++;
      }
      goto EXIT;

    case 0x2420:   // PDI frame first box car RT#4
    case 0xe420:   // PDI frame first box car RT#28
      new_HK_data_box_car[0]++;
      status_word[0] = dat_buffer[(data++)%DAT_SIZE];
      block_status[0] = cmd_buffer[VK_index_n].block_status;
      for (i=0; i<BOX_CAR_LENGTH; i++) {
        HK.box_car[0][i] = dat_buffer[(data++)%DAT_SIZE];
      }
      goto EXIT;

    case 0x2440:   // PDI frame second box car RT#4
    case 0xe440:   // PDI frame second box car RT#28
      new_HK_data_box_car[1]++;
      status_word[1] = dat_buffer[(data++)%DAT_SIZE];
      block_status[1] = cmd_buffer[VK_index_n].block_status;
      for (i=0; i<BOX_CAR_LENGTH; i++) {
        HK.box_car[1][i] = dat_buffer[(data++)%DAT_SIZE];
      }
      if (new_HK_data_box_car[0] && new_HK_data_box_car[1]) {
        new_HK_data_box_car[0] = 0;
        new_HK_data_box_car[1] = 0;
        new_PDI_frame++;
      }
      goto EXIT;

    default:
      parser_locked = FALSE;
      display_message( 25, error_in_PRS_col, error_in_PRS_col+3, alarm, "PRS");
      write_log_pipe( "Parser error!");
      return;
  }
EXIT: 
  if (!parser_locked) {
    display_message( 25, error_in_PRS_col, error_in_PRS_col+3, alarm, "PRS");
    write_log_pipe( "Parser not locked...");
  }
  return;
}

//------------------------------------------------------------------------------

#pragma check_stack(off)  // otherwise there will be stack overflow
void COPY_A( void) {
  int i;
  for (i=0; i<(0xff & mem->mt_cmd_pointer_A)/4; i++) {
    if (put == get) {
      mem->mt_dat_pointer_A = mem->mt_cmd_A[i].data_pointer;
      ovf |= 4;
      break;
    }
    cmd_buffer[put].block_status = mem->mt_cmd_A[i].block_status;
    cmd_buffer[put].handler_status = ovf;
    ovf = 0;
    cmd_buffer[put].data_pointer =
     (data_index + (0x3ff & mem->mt_cmd_A[i].data_pointer)) % DAT_SIZE;
    cmd_buffer[put].command_word = mem->mt_cmd_A[i].command_word;
    put = (put + 1) % CMD_SIZE;
  }
  for (i=0; i<(0x3ff & mem->mt_dat_pointer_A); i++) {
    dat_buffer[data_index] = mem->mt_dat_A[i];
    data_index = (data_index + 1) % DAT_SIZE;
  }
  mem->mt_cmd_pointer_A = offsetof(struct bu_memory, mt_cmd_A)/2;
  mem->mt_dat_pointer_A = offsetof(struct bu_memory, mt_dat_A)/2;
}
#pragma check_stack()   // back to default

//------------------------------------------------------------------------------

#pragma check_stack(off)  // otherwise there will be stack overflow
void COPY_B( void) {
  int i;
  for (i=0; i<(0xff & mem->mt_cmd_pointer_B)/4; i++) {
    if (put == get) {
      mem->mt_dat_pointer_B = mem->mt_cmd_B[i].data_pointer;
      ovf |= 4;
      break;
    }
    cmd_buffer[put].block_status = mem->mt_cmd_B[i].block_status;
    cmd_buffer[put].handler_status = ovf;
    ovf = 0;
    cmd_buffer[put].data_pointer =
     (data_index + (0x3ff & mem->mt_cmd_B[i].data_pointer)) % DAT_SIZE;
    cmd_buffer[put].command_word = mem->mt_cmd_B[i].command_word;
    put = (put + 1) % CMD_SIZE;
  }
  for (i=0; i<(0x3ff & mem->mt_dat_pointer_B); i++) {
    dat_buffer[data_index] = mem->mt_dat_B[i];
    data_index = (data_index + 1) % DAT_SIZE;
  }
  mem->mt_cmd_pointer_B = offsetof(struct bu_memory, mt_cmd_B)/2;
  mem->mt_dat_pointer_B = offsetof(struct bu_memory, mt_dat_B)/2;
}
#pragma check_stack()   // back to default

//------------------------------------------------------------------------------

void static interrupt far bu_interrupt_handler( void) {
//
// this is interrupt handler. it is new AL's version.  VK's version discarded.
//
//  unsigned int status, AB, new_AB;          ///// 28/12/97
  unsigned int AB, new_AB;

//  status = reg->interrupt_status;           ///// 28/12/97
  bu_interrupt_status = reg->interrupt_status;
  
  ovf = (0x0C00 & bu_interrupt_status) >> 9;  ///// 28/12/97

  AB = 0x2000 & reg->config_1;           // AB==0->A buffers active,
                                         // AB!=0->B buffers active
  if (AB) COPY_A(); else COPY_B();       // get data out of non-active buffers

  reg->config_1 ^=  0x2000;              // switch buffers A<->B

  new_AB = 0x2000 & reg->config_1;

  if (new_AB != AB) {                    // if buffers had switched
    if (new_AB) COPY_A(); else COPY_B(); // then get data out of non-active buffers
  }

  _disable();
  outp( 0x20, 0x20);
  if (bu_irq > 7) outp( 0xa0, 0x20);
  _enable();
}

//------------------------------------------------------------------------------


