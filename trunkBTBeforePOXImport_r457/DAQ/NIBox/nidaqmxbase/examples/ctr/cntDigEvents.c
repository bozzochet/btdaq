/*********************************************************************
*
* ANSI C Example program:
*    cntDigEvents.c
*
* Example Category:
*    CI
*
* Description:
*    This example demonstrates how to count digital events on a
*    Counter Input Channel. The Count Direction and
*    Edge are all configurable.
*
*    Edges are counted on the counter's default input terminal
*    (see I/O Connections for more information), but could easily
*    be modified to count edges on a PFI or RTSI line.
*
* Instructions for Running:
*    1. Select the Physical Channel which corresponds to the counter
*       you want to count edges on the DAQ device.
*    2. Enter the Count Direction and measurement Edge to specify how
*       you want the counter to count.
*       Note: Use the genDigPulseTrainCont example to
*       verify that you are counting correctly on the DAQ device.
*
* Steps:
*    1. Create a task.
*    2. Create a Counter Input channel to Count Events. The Edge
*       parameter is used to determine if the counter will increment
*       on rising or falling edges.
*    3. Call the Start function to arm the counter and begin counting.
*       The counter will be preloaded with the Initial Count.
*    4. The counter will be continually polled until the Stop button
*       is pressed on the front panel.
*    5. Call the Clear Task function to clear the Task.
*    6. Display an error if any.
*
* I/O Connections Overview:
*    The counter will count edges on the input terminal of the counter
*    specified in the Physical Channel I/O control.
*
*    For E-Series counters, the default input terminal for edge
*    measurements on each physical channel is:
*    ctr0 - PFI8
*    ctr1 - PFI3
*
*    In this example the counter will count edges on the default input
*    terminal on ctr0 (PFI8). For more information on default input
*    terminals for counters, refer to the "Making signal Connections
*    for Counters" section of the NI-DAQmx Base help.
*
* Recommended use:
*    Call Configure and Start functions. Call Read function in a loop.
*    Call Stop function at the end.
*
*********************************************************************/
#include "NIDAQmxBase.h"
#include <stdio.h>
#include <time.h>

#define DAQmxErrChk(functionCall) { if( DAQmxFailed(error=(functionCall)) ) { goto Error; } }

static int gRunning;

int main(int argc, char *argv[])
{
    // Task parameters
    int32       error = 0;
    TaskHandle  taskHandle = 0;
    char        errBuff[2048]={'\0'};
    time_t      startTime;

    // Channel parameters
    char        chan[] = "Dev1/ctr0";

    // Data read parameters
    uInt32      data;
    float64     timeout = 10.0;


    DAQmxErrChk (DAQmxBaseCreateTask("",&taskHandle));
    DAQmxErrChk (DAQmxBaseCreateCICountEdgesChan(taskHandle,chan,"",DAQmx_Val_Falling,0,DAQmx_Val_CountUp));
    DAQmxErrChk (DAQmxBaseStartTask(taskHandle));
	gRunning = 1;
	// The loop will quit after 10 seconds
	startTime = time(NULL);
	while( gRunning && time(NULL)<startTime+10 ) {
        DAQmxErrChk (DAQmxBaseReadCounterScalarU32(taskHandle,timeout,&data,NULL));
		printf("\rCount: %d",data);
	}

Error:
	puts("");
	if( DAQmxFailed(error) )
		DAQmxBaseGetExtendedErrorInfo(errBuff,2048);
	if( taskHandle!=0 ) {
        DAQmxBaseStopTask(taskHandle);
        DAQmxBaseClearTask(taskHandle);
    }
	if( DAQmxFailed(error) )
		printf("DAQmxBase Error: %s\n",errBuff);
	return 0;
}
