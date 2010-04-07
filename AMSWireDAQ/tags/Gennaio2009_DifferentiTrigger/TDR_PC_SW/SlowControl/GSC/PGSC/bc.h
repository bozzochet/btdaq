#include "dos.h"
#include "conio.h"
#include <stddef.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include "bios.h"

#define PDI_frame_size  160
#define pdi_buffer_size 100

#define PSP_frame_size  128
unsigned char           PSP_frame[PSP_frame_size];
volatile unsigned short PSP_status;
short                   PSP_request;

#define bus_A     0x0080
#define bus_B     0x0000

unsigned short rt_addr;
unsigned short bus_select;
 
