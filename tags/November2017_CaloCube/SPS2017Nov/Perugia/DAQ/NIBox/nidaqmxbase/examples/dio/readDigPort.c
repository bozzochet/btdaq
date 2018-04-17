/*********************************************************************
*
* C Example program:
*    readDigPort.c
*
* Example Category:
*    DI
*
* Description:
*    This example demonstrates how to read values from a digital input
*    channel.
*
* Instructions for Running:
*    1. Select the digital ports on the DAQ device to be read.
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
   int32       error = 0;
   TaskHandle  taskHandle = 0;
   char        errBuff[2048];

   // Channel parameters
   const char  chan[] = "Dev1/port0";

   // Read parameters
   uInt32      r_data [1];
   int32       read;

   // Create Digital Input (DI) Task and Channel
   DAQmxErrChk (DAQmxBaseCreateTask ("", &taskHandle));
   DAQmxErrChk (DAQmxBaseCreateDIChan(taskHandle,chan,"",DAQmx_Val_ChanForAllLines));

   // Start Task (configure port)
   DAQmxErrChk (DAQmxBaseStartTask (taskHandle));

   // Read from port
   DAQmxErrChk (DAQmxBaseReadDigitalU32(taskHandle,1,10.0,DAQmx_Val_GroupByChannel,r_data,1,&read,NULL));

   printf("Data read: 0x%X\n", r_data[0]);

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

