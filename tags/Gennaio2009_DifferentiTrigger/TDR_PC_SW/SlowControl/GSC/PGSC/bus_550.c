// file bus_550.c 
// low-level functions for 1553 BUS CONTROLLER PGSC program
// taken from VK and reworked

#include <sys\timeb.h>
#include "bc.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// this was "550.h" file

#define time_tag_res 3 // 0->64us, 1->32us, 2->16us, 3->8us, 4->4us, 5->2us

typedef struct  {
  unsigned int interrupt_mask;
  unsigned int config_1;
  unsigned int config_2;
  unsigned int start_reset;
  unsigned int bc_control;
  unsigned int time_tag;
  unsigned int interrupt_status;
  unsigned int config_3;
  unsigned int config_4;
  unsigned int config_5;
  unsigned int data_stack_addr;
  unsigned int bc_frame_time_remaining;
  unsigned int bc_message_time_remaining;
  unsigned int bc_frame_time;
  unsigned int rt_status;
  unsigned int rt_bit;
} bu_registers;

static bu_registers far *reg;

#define command_stack_pointer start_reset

typedef  struct {
  unsigned int block_status;
  unsigned int time_tag;
  unsigned int gap_time;
  unsigned int block_addr;
} bc_descriptor;

typedef struct  {
  bc_descriptor stack_A[64];
  unsigned int  stack_pointer_A;
  unsigned int  message_count_A;
  unsigned int  initial_stack_pointer_A;
  unsigned int  initial_message_count_A;
  unsigned int  stack_pointer_B;
  unsigned int  message_count_B;
  unsigned int  initial_stack_pointer_B;
  unsigned int  initial_message_count_B;
  unsigned int  Brdcast_Sync_With_Data[4];
  unsigned int  Self_Test[6];
  unsigned int  Broadcast_Time[11];
  unsigned int  PSP_1[36];
  unsigned int  PSP_2[36];
  unsigned int  PDI_1[36];
  unsigned int  PDI_2[36];
} bu_memory;

static bu_memory far *mem;
static unsigned short bu_irq;

#define self_test 0x0040        
#define frame Brdcast_Sync_With_Data[2]

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

void set_broadcast_time( void);
void set_OIU_time( void);
void increment_OIU_time( void);
static void interrupt far bu_interrupt_handler( void);
static void (interrupt far *old_interrupt_handler)( void);
short enable_bus_controller( void);
void disable_bus_controller( void);
void send_PSP( unsigned char *, volatile unsigned short *);
unsigned short take_PDI( unsigned char **);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

static unsigned char  pdi_buffer[pdi_buffer_size][PDI_frame_size];
static unsigned short pdi_status_buffer[pdi_buffer_size];
static unsigned short bu_enabled=0, OIU_counter=0;
static unsigned short psp_flag=0, psp_run=0, pdi_bsw_index;              
static unsigned short put=0, get=pdi_buffer_size-1, last_f=0;
volatile unsigned short static *tx_status;
static unsigned char *tx_p;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

struct OIU_time_t {
  unsigned char  yr_high;
  unsigned char  yr_low;
  unsigned char  months;
  unsigned char  days;
  unsigned char  hours;
  unsigned char  minutes;
  unsigned char  seconds;
  unsigned short milli_secs;
} OIU_time;

unsigned char days_in_month[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
unsigned char bcd[100] = {
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
  0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,
  0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,
  0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,
  0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,
  0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,
  0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,
  0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,
  0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,
};

//---------------------------------------------------------------------------
#pragma check_stack( off )
void set_broadcast_time(void) {
  mem->Broadcast_Time[2] =                  0x50<<8 | bcd[OIU_time.yr_high ];
  mem->Broadcast_Time[3] = bcd[OIU_time.yr_low ]<<8 | bcd[OIU_time.months ];
  mem->Broadcast_Time[4] = bcd[OIU_time.days   ]<<8 | bcd[OIU_time.hours  ];
  mem->Broadcast_Time[5] = bcd[OIU_time.minutes]<<8 | bcd[OIU_time.seconds];
}  
#pragma check_stack()
//---------------------------------------------------------------------------
void set_OIU_time(void) {
  struct timeb timebuffer;
  struct tm *newtime;
  ftime( &timebuffer );
  newtime = localtime( &timebuffer.time );
  OIU_time.milli_secs = 100 * (timebuffer.millitm / 100);
  OIU_time.seconds    = newtime->tm_sec;
  OIU_time.minutes    = newtime->tm_min;
  OIU_time.hours      = newtime->tm_hour;
  OIU_time.days       = newtime->tm_mday;
  OIU_time.months     = newtime->tm_mon + 1;
  OIU_time.yr_low     = newtime->tm_year % 100;
  OIU_time.yr_high    = (1900 + newtime->tm_year)/100;
  set_broadcast_time();
  mem->frame = 10 * (OIU_time.seconds % 10) + OIU_time.milli_secs / 100;  
}
//---------------------------------------------------------------------------
#pragma check_stack( off )
void increment_OIU_time(void) {
  struct OIU_time_t *p = &OIU_time;
  if ((p->milli_secs += 100) >= 1000) {
    p->milli_secs = 0;
    if (++p->seconds >= 60) {
      p->seconds = 0;
      if (++p->minutes >= 60) {
        p->minutes = 0;
        if (++p->hours >= 24) {
          p->hours = 0;
          if (++p->days > days_in_month[p->months]) {
            if (!(p->months   == 2 && // April
                  p->yr_low%4 == 0 && // Leap
                  !(p->yr_low == 0 && p->yr_high%4 != 0))) {
              p->days = 1;
              if (++p->months > 12) {
                p->months = 1;
                if (++p->yr_low >= 100) {
                  p->yr_low = 0;
                  ++p->yr_high;
                }
              }  
            }
          }
        }
      }
    }
  }
  set_broadcast_time();
}
#pragma check_stack()
//---------------------------------------------------------------------------
void send_PSP(unsigned char PSP[], volatile unsigned short *status) { 
  tx_status = status;
  *status   = 0;
  tx_p = PSP;
  psp_flag = 1;
}
//---------------------------------------------------------------------------
unsigned short take_PDI(unsigned char **p) {
  unsigned int t;
  t = get + 1;
  if (t == pdi_buffer_size) t = 0;
  if (t == put) return 0;
  *p = pdi_buffer[t];
  get = t;
  return pdi_status_buffer[t];
}
//---------------------------------------------------------------------------
// BU 65550 interrupt service
static void interrupt far bu_interrupt_handler(void) {
  static unsigned short completion_status=1;
  unsigned short register t,i,j;
  int count;
  unsigned int *p;
  volatile unsigned int status;
  status = reg->interrupt_status;
  if ((status & 0x0010) == 0) ; //? error, this bit should be on
  _enable();
// make pdi frame    
  if (put==get) {
    completion_status = 2;
  }
  else {
    i = 12;                                  
    for (j=0; j<32; j++) {
      pdi_buffer[put][i++] = mem->PDI_1[j+4]>>8;
      pdi_buffer[put][i++] = mem->PDI_1[j+4];
    }
    for (j=0; j<32; j++) {
      pdi_buffer[put][i++] = mem->PDI_2[j+4]>>8;
      pdi_buffer[put][i++] = mem->PDI_2[j+4];
    }
    pdi_buffer[put][i++] = mem->stack_A[pdi_bsw_index].block_status>>8;
    pdi_buffer[put][i++] = mem->stack_A[pdi_bsw_index].block_status;
    pdi_buffer[put][i++] = mem->stack_A[pdi_bsw_index+1].block_status>>8;
    pdi_buffer[put][i++] = mem->stack_A[pdi_bsw_index+1].block_status;
    pdi_status_buffer[put] = completion_status;
    if (++put == pdi_buffer_size) put = 0;
    completion_status = 1;
  }   
//
  increment_OIU_time();
// set in next frame frame number
  i = mem->frame;
  last_f = i;
  if (++i == 100) i = 0; 
  mem->frame = i;
// set in next frame     
//mem->Broadcast_Time[0] = (mem->frame%10) ? 0x00E2 : 0x00A2;
  if (mem->frame % 10) mem->Broadcast_Time[0] = 0x0022 | bus_select | self_test;
  else                 mem->Broadcast_Time[0] = 0x0022 | bus_select;
//
  mem->PDI_1[0] = bus_select;
  mem->PDI_2[0] = 0x0010 | bus_select;       // EOM Interrupt
  
  mem->PDI_1[1] = 0x0420 | rt_addr<<11;
  mem->PDI_2[1] = 0x0440 | rt_addr<<11;
  
  mem->PSP_1[1] = 0x0360 | rt_addr<<11;
  mem->PSP_2[1] = 0x0380 | rt_addr<<11;
  
  mem->Brdcast_Sync_With_Data[0] = 0x0006 | bus_select;
  
  mem->Self_Test[0] = bus_select | self_test;
       
  if (psp_run) {
    psp_run  = 0;
    OIU_counter++;
//  mem->PSP_1[ 0] = 0x00C0;
//  mem->PSP_2[ 0] = 0x00C0;
    mem->PSP_1[ 0] = bus_select | self_test;
    mem->PSP_2[ 0] = bus_select | self_test;
  }
  if (psp_flag) {
//  mem->PSP_1[ 0] = 0x0080;
//  mem->PSP_2[ 0] = 0x0080;
    mem->PSP_1[ 0] = bus_select;
    mem->PSP_2[ 0] = bus_select;
    count = tx_p[1];
    for (j=0, t=2; j<32; j++, t+=2)
       mem->PSP_1[j+2] = (t < count) ? (tx_p[t]<<8 | tx_p[t+1]) : 0;
    for (j=0; j<31; j++,t+=2)
       mem->PSP_2[j+2] = (t < count) ? (tx_p[t]<<8 | tx_p[t+1]) : 0;
       mem->PSP_2[33] = OIU_counter;
    psp_flag = 0;
    psp_run  = 1; 
    *tx_status = 1;
  }
  _disable();
  outp( 0x20, 0x20);
  if (bu_irq > 7) outp( 0xA0, 0x20);
  _enable();
}
//---------------------------------------------------------------------------
// Initialize BU-65550 as Bus Controller
short enable_bus_controller( void) {
  unsigned int register q;
  int  interrupt_request = 9;
  long memory_address    = 0xD2000000;
  long register_address  = 0xD1000000;
  FILE *configuration_file;
  char b[81], string[81];
  long value;

  rt_addr = 4;
  bus_select = bus_A;
  
  PSP_status  = 1;
  PSP_request = 0;


  configuration_file = fopen( "C:\\ACESOFT\\ACE.CFG", "r");
  if (configuration_file != NULL) {
//  printf( "Reading configuration file...");
    while (fgets( b, 80, configuration_file) != NULL) {
      sscanf( b, "%s = %lx", string, &value);
      if (!strcmp( string, "interrupt")) interrupt_request = value;
      if (!strcmp( string, "register"))  register_address  = value << 16;
      if (!strcmp( string, "memory"))    memory_address    = value << 16;
    }
  }  
//printf( "interrupt = %d\nregister  = %8.8lX\nmemory    = %8.8lX\n",
//         interrupt_request, register_address, memory_address);
 
  bu_irq = interrupt_request;
  mem    = (bu_memory    far *) memory_address;
  reg    = (bu_registers far *) register_address;
  
  reg->start_reset = 1;
  reg->interrupt_mask = 0x55AA;
  if (reg->interrupt_mask != 0x55AA) {
    printf( "BU-65550 register read/write error\n");
    return 0;
  }

  mem->Self_Test[0] = 0xAA55;
  if (mem->Self_Test[0] != 0xAA55) {
    printf( "BU-65550 memory read/write error\n");
    return 0;
  }

  reg->start_reset    = 1;             /* software reset               */
  reg->config_3       = 0x8000;        /* Enhanced Mode                */
  reg->interrupt_mask = 0x0010;        /* BC Message Interrupt         */
  reg->config_1       = 0x0160;        /* Frame Auto Repeat            */
                                       /* Internal Trigger             */
                                       /* Message Gap Timer            */
  reg->config_2       = 0x8418;        /* Enhanced Interrupt           */
                                       /* 256-WD Bound Disable         */
                                       /* Interrupt Status Auto Clear  */
                                       /* Level Interrupt Request      */
  reg->config_4       = 0x1040;        /* Expanded BC Control Word     */
                                       /* Valid if ME, no DATA         */
  reg->config_5       = 0x0800;        /* Expanded Zero-Crossing       */
  reg->bc_frame_time  = 1000;          /* 100 msec frame               */
 
  q=0;
  mem->stack_A[q].gap_time   = 12500-1; 
  mem->stack_A[q].block_addr = offsetof(bu_memory,Brdcast_Sync_With_Data)/2;
  q++;
  mem->stack_A[q].gap_time   = 12500-1; 
  mem->stack_A[q].block_addr = offsetof(bu_memory,Self_Test)/2;
  q++;  
  mem->stack_A[q].gap_time   = 12500-1;
  mem->stack_A[q].block_addr = offsetof(bu_memory,Broadcast_Time)/2;
  q++; 
  mem->stack_A[q].gap_time   = 812-1; 
  mem->stack_A[q].block_addr = offsetof(bu_memory,PSP_1)/2;
  q++; 
  mem->stack_A[q].gap_time   = 12500-812;
  mem->stack_A[q].block_addr = offsetof(bu_memory,PSP_2)/2;
  q++;  
  mem->stack_A[q].gap_time   = 12500-1;
  mem->stack_A[q].block_addr = offsetof(bu_memory,Self_Test)/2;
  q++;
  pdi_bsw_index = q;
  mem->stack_A[q].gap_time   = 812-1;
  mem->stack_A[q].block_addr = offsetof(bu_memory,PDI_1)/2;
  q++;  
  mem->stack_A[q].gap_time   = 12500-812;
  mem->stack_A[q].block_addr = offsetof(bu_memory,PDI_2)/2;
  q++;
  mem->stack_A[q].gap_time   = 12500-1;
  mem->stack_A[q].block_addr = offsetof(bu_memory,Self_Test)/2;
  q++;
  mem->stack_A[q].gap_time   = 12500-1;
  mem->stack_A[q].block_addr = offsetof(bu_memory,Self_Test)/2;
  q++;
  mem->initial_stack_pointer_A = offsetof(bu_memory,stack_A)/2;
  mem->initial_message_count_A = ~q;
 
//mem->Broadcast_Time[0] = 0x00E2;
  mem->Broadcast_Time[0] = 0x0022 | bus_select | self_test;
  mem->Broadcast_Time[1] = 0xFBA8;
//
  mem->Broadcast_Time[6] = 0;
  mem->Broadcast_Time[7] = 0x61A8;
  mem->Broadcast_Time[8] = 0;
  mem->Broadcast_Time[9] = 0;
  mem->Broadcast_Time[10]= 0;
  
//mem->PSP_1[ 0] = 0x00C0;
  mem->PSP_1[ 0] = bus_select | self_test;
  mem->PSP_1[ 1] = 0x0360 | rt_addr<<11;
  mem->PSP_1[34] = 0;
  mem->PSP_1[35] = 0;
 
//mem->PSP_2[ 0] = 0x00C0;
  mem->PSP_2[ 0] = bus_select | self_test;
  mem->PSP_2[ 1] = 0x0380 | rt_addr<<11;
  mem->PSP_2[34] = 0;
  mem->PSP_2[35] = 0;
  
//mem->PDI_1[ 0] = 0x0080;
  mem->PDI_1[ 0] = bus_select;
  mem->PDI_1[ 1] = 0x0420 | rt_addr<<11;
  mem->PDI_1[ 2] = 0;
  mem->PDI_1[ 3] = 0;
 
//mem->PDI_2[ 0] = 0x0090; /* EOM Interrupt */
  mem->PDI_2[ 0] = 0x0010 | bus_select;     // EOM Interrupt
  mem->PDI_2[ 1] = 0x0440 | rt_addr<<11;
  mem->PDI_2[ 2] = 0;
  mem->PDI_2[ 3] = 0;
 
/* BC Control:  Bus A, Mode Code, Broadcast               */
/* Command: Broadcast Mode Code Synchronize With Data     */
//mem->Brdcast_Sync_With_Data[0] = 0x0086;
  mem->Brdcast_Sync_With_Data[0] = 0x0006 | bus_select;
  mem->Brdcast_Sync_With_Data[1] = 0xF811;
  mem->Brdcast_Sync_With_Data[2] = 0;
  mem->Brdcast_Sync_With_Data[3] = 0;
  
//mem->Self_Test[0] = 0x00C0; /* BC Control: OFF Line Self Test */
  mem->Self_Test[0] = bus_select | self_test;  // BC Control: OFF Line Self Test
  mem->Self_Test[1] = 0x0822;                  // Command: BC->RT=1 SA=1 WC=2
  mem->Self_Test[2] = 0xAAAA;                  // First Data Word
  mem->Self_Test[3] = 0x5555;                  // Second Data Word
  mem->Self_Test[4] = 0;                       // Looped Back
  mem->Self_Test[5] = 0;                       // Status Word
  

  if (bu_irq > 7) {
   old_interrupt_handler = _dos_getvect( bu_irq + 0x68);
    _dos_setvect( bu_irq + 0x68, bu_interrupt_handler);
    _disable();
    outp( 0xa1, (inp( 0xA1) & ~(1<<(bu_irq-8))) );
    _enable();
  }
  else {
    old_interrupt_handler = _dos_getvect( bu_irq + 8);
    _dos_setvect( bu_irq + 0x08, bu_interrupt_handler);
    _disable();
    outp( 0x21, (inp( 0x21) & ~(1<<bu_irq)) );
    _enable();
  }
  bu_enabled = 1;
  set_OIU_time();
  reg->start_reset = 2;       /* BC start */
  
  return 1;

}
//---------------------------------------------------------------------------
void disable_bus_controller(void) {
  reg->start_reset = 1;       /* BC reset */
  if (bu_enabled) {
    if (bu_irq > 7) {
      _disable();
      outp( 0xA1, (inp( 0xA1) | (1<<(bu_irq-8))));
      _enable();
      _dos_setvect( bu_irq + 0x68, old_interrupt_handler);
    }
    else {
      _disable();
      outp( 0x21, (inp( 0x21) | (1<<bu_irq)));
      _enable();
      _dos_setvect( bu_irq + 0x08, old_interrupt_handler);
    }
    bu_enabled = 0;
  }
}
//---------------------------------------------------------------------------
 



