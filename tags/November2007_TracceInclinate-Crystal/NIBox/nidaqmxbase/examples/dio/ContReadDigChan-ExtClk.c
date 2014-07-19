/*********************************************************************
*
* ANSI C Example program:
*    ContReadDigChan-ExtClk.c
*
* Example Category:
*    DI
*
* Description:
*    This example demonstrates how to input a continuous digital
*    waveform using an external clock.
*
* Instructions for Running:
*    1. Select the physical channel to correspond to where your
*       signal is input on the DAQ device.
*    2. Select the Clock Source for the acquistion.
*    3. Set the approximate Rate of the external clock. This allows
*       the internal characteristics of the acquisition to be as
*       efficient as possible. Also set the Samples to Read control.
*       This will determine how many samples are read each time the
*       while loop iterates. This also determines how many points are
*       plotted on the graph each iteration.
*
* Steps:
*    1. Create a task.
*    2. Create a Digital Input channel.
*    3. Define the parameters for an External Clock Source.
*       Additionally, set the sample mode to be continuous.
*    4. Call the Start function to start the acquisition.
*    5. Read the waveform data in a loop until the user hits the stop
*       button or an error occurs.
*    6. Call the Clear Task function to clear the Task.
*    7. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal input terminal matches the Physical
*    Channel I/O control. Also, make sure your external clock
*    terminal matches the Physical Channel I/O Control. For further
*    connection information, refer to your hardware reference manual.
*
*********************************************************************/

#include <stdio.h>
#include <time.h>
#include "NIDAQmxBase.h"

#define DAQmxErrChk(functionCall) { if( DAQmxFailed(error=(functionCall)) ) { goto Error; } }

int main(int argc, char *argv[])
{
	int32		error=0;
	time_t		startTime; 
	TaskHandle	taskHandle=0;
	uInt32		data[1000];
	int32		sampsRead,totalRead=0;
	char		errBuff[2048]={'\0'};

	/*********************************************/
	/*/ DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxBaseCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxBaseCreateDIChan(taskHandle,"Dev1/port0","",DAQmx_Val_ChanPerLine));
	DAQmxErrChk (DAQmxBaseCfgSampClkTiming(taskHandle,"/Dev1/PFI0",10000.0,DAQmx_Val_Rising,DAQmx_Val_ContSamps,1000));

	/*********************************************/
	/*/ DAQmxBase Start Code
	/*********************************************/
	DAQmxErrChk (DAQmxBaseStartTask(taskHandle));

	printf("Acquiring samples continuously.  Press Ctrl-C to exit\n");
	
    startTime = time(NULL);
    while( time(NULL)<startTime+10 ) {	
		/*********************************************/
		/*/ DAQmxBase Read Code
		/*********************************************/
		DAQmxErrChk (DAQmxBaseReadDigitalU32(taskHandle,1000,10.0,DAQmx_Val_GroupByChannel,data,1000,&sampsRead,NULL));

		if( sampsRead>0 ) {
			totalRead += sampsRead;
			printf("Acquired Sample[0]: 0x%08X\n", data[0]);
		}
	}
	printf("\nAcquired %d total samples.\n",totalRead);

Error:
	if( DAQmxFailed(error) )
		DAQmxBaseGetExtendedErrorInfo(errBuff,2048);
	if( taskHandle!=0 ) {
		/*********************************************/
		/*/ DAQmxBase Stop Code
		/*********************************************/
		DAQmxBaseStopTask(taskHandle);
		DAQmxBaseClearTask(taskHandle);
	}
	if( DAQmxFailed(error) )
		printf("DAQmx Base Error: %s\n",errBuff);
	return 0;
}
