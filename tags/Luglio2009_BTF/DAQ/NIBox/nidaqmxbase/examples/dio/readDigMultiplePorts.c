/*********************************************************************
*
* C Example program:
*    readDigPort.c
*
* Example Category:
*    DI
*
* Description:
*    This example demonstrates how to read values from  multiple digital
*    ports.
*
* Instructions for Running:
*    1. Select the digital ports on the DAQ device to be written.
*
* Steps:
*    1. Create a task.
*    2. Create a Digital Input channel.
*    3. Call the Start function to start the task.
*    4. Read the digital uInt8 array data. This read function
*       reads a single sample of digital data on demand, so no timeout
*       is necessary.
*    5. Call the Clear Task function to clear the Task.
*    6. Display an error if any..
*
*********************************************************************/
#include "NIDAQmxBase.h"
#include <stdio.h>

#define DAQmxErrChk(functionCall) { if( DAQmxFailed(error=(functionCall)) ) { goto Error; } }

int main (int argc, char *argv[])
{
   // Task parameters
   int32        error = 0;
   TaskHandle   taskHandle = 0;
   int32        i;
   const uInt32 num_ports = 3;
   char         errBuff[2048];

   // Channel parameters
   char         chan1[] = "Dev1/port0";
   char         chan2[] = "Dev1/port1";
   char         chan3[] = "Dev1/port2";

   // Read parameters
   uInt32       r_data [num_ports];
   int32        read;

   // Create Digital Input (DI) Task and Channel
   DAQmxErrChk (DAQmxBaseCreateTask ("", &taskHandle));
   DAQmxErrChk (DAQmxBaseCreateDIChan(taskHandle,chan1,"",DAQmx_Val_ChanForAllLines));
   DAQmxErrChk (DAQmxBaseCreateDIChan(taskHandle,chan2,"",DAQmx_Val_ChanForAllLines));
   DAQmxErrChk (DAQmxBaseCreateDIChan(taskHandle,chan3,"",DAQmx_Val_ChanForAllLines));

   // Start Task (configure port)
   DAQmxErrChk (DAQmxBaseStartTask (taskHandle));

   // Read from port
   DAQmxErrChk (DAQmxBaseReadDigitalU32(taskHandle,1,10.0,DAQmx_Val_GroupByChannel,r_data,num_ports,&read,NULL));

   for(i=0; i<num_ports; i++)
      printf("Data read, port %d: 0x%X\n", i, r_data[i]);

Error:

   if (DAQmxFailed (error))
      DAQmxBaseGetExtendedErrorInfo (errBuff, 2048);

   if (taskHandle != 0)
   {
      DAQmxBaseStopTask (taskHandle);
      DAQmxBaseClearTask (taskHandle);
   }

   if (error)
      printf ("DAQmxBase Error %d: %s\n", error, errBuff);

   return 0;
}

