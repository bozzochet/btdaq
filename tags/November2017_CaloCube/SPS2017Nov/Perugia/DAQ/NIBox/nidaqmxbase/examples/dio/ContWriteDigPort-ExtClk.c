/*********************************************************************
*
* ANSI C Example program:
*    ContWriteDigPort-ExtClk.c
*
* Example Category:
*    DO
*
* Description:
*    This example demonstrates how to output a continuous digital
*    pattern using an external clock.
*
* Instructions for Running:
*    1. Select the Physical Channel to correspond to where your
*       signal is output on the DAQ device.
*    2. Select the Clock Source for the generation.
*    3. Specify the Rate of the output digital pattern.
*    4. Enter the digital pattern data.
*
* Steps:
*    1. Create a task.
*    2. Create an Digital Output Channel.
*    3. Call the DAQmxCfgSampClkTiming function which sets the sample
*       clock rate. Additionally, set the sample mode to continuous.
*    4. Write the data to the output buffer.
*    5. Call the Start function to start the task.
*    6. Loop continuously until the user presses the Stop button.
*       Check for errors every 100 ms using the DAQmxIsTaskDone
*       function.
*    7. Call the Clear Task function to clear the Task.
*    8. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal output terminal matches the Physical
*    Channel I/O Control. Also, make sure your external clock
*    terminal matches the Clock Source Control. For further
*    connection information, refer to your hardware reference manual.
*
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "NIDAQmxBase.h"

#define DAQmxErrChk(functionCall) { if( DAQmxFailed(error=(functionCall)) ) { goto Error; } }

int main(void)
{
	int32       error=0;
	time_t		startTime;
	TaskHandle  taskHandle=0;
	uInt32      data[8]={1,2,4,8,16,32,64,128};
	bool32      done=0;
	char        errBuff[2048]={'\0'};

	/*********************************************/
	/*/ DAQmxBase Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxBaseCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxBaseCreateDOChan(taskHandle,"Dev1/port0","",DAQmx_Val_ChanForAllLines));
	DAQmxErrChk (DAQmxBaseCfgSampClkTiming(taskHandle,"/Dev1/PFI0",1000.0,DAQmx_Val_Rising,DAQmx_Val_ContSamps,1000));

	/*********************************************/
	/*/ DAQmxBase Write Code
	/*********************************************/
	DAQmxErrChk (DAQmxBaseWriteDigitalU32(taskHandle,8,0,10.0,DAQmx_Val_GroupByChannel,data,NULL,NULL));

	/*********************************************/
	/*/ DAQmxBase Start Code
	/*********************************************/
	DAQmxErrChk (DAQmxBaseStartTask(taskHandle));

	printf("Generating digital output continuously.  Press Ctrl-C to exit\n");
	
    startTime = time(NULL);
    while( !done && (time(NULL)<startTime+10) ) {	

		/*********************************************/
		/*/ DAQmxBase IsDone Code
		/*********************************************/
		DAQmxErrChk (DAQmxBaseIsTaskDone(taskHandle,&done));

		if( !done )
			usleep(100000);
	}

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
