/*********************************************************************
*
* C Example program:
*    writeDigPort.c
*
* Example Category:
*    DO
*
* Description:
*    This example demonstrates how to write values to a digital port.
*
* Instructions for Running:
*    1. Select the digital ports on the DAQ device to be written.
*    2. Select a value to write.
*
* Steps:
*    1. Create a task.
*    2. Create a Digital Output channel.
*    3. Call the Start function to start the task.
*    4. Write the digital uInt32 array data. This write function
*       writes a single sample of digital data on demand, so no timeout
*       is necessary.
*    5. Call the Clear Task function to clear the Task.
*    6. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal output terminals match the Lines I/O
*    Control. In this case wire the item to receive the signal to the
*    first eight digital lines on your DAQ Device.
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
   char        chan[] = "Dev1/port0";

   // Write parameters
   uInt32      w_data [1];
   int32       written;


   // Create Digital Output (DO) Task and Channel
   DAQmxErrChk (DAQmxBaseCreateTask ("", &taskHandle));
   DAQmxErrChk (DAQmxBaseCreateDOChan(taskHandle,chan,"",DAQmx_Val_ChanForAllLines));

   // Start Task (configure port)
   //DAQmxErrChk (DAQmxBaseStartTask (taskHandle));

   //  Write 0x55 to port(s)
   //  Only 1 sample per channel supported for static DIO
   //  Autostart ON

   w_data[0] = 0x55;

   printf("Data to write: 0x%X\n", w_data[0]);

   DAQmxErrChk (DAQmxBaseWriteDigitalU32(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,w_data,&written,NULL));


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

