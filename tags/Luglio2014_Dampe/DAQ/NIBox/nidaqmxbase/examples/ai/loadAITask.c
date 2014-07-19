/*********************************************************************
*
* ANSI C Example program:
*    loadAITask.c
*
* Example Category:
*    AI & Task Loading
*
* Description:
*    This example demonstrates how to load a task created with the
*    NI-DAQmx Base Task Configuration Utility.
*
* Instructions for Running:
*    1. Run the NI-DAQmx Base Task Configuration Utility executable.
*    2  Create/modify a task.
*    3. Select the created/modified task to load.
*
* Steps:
*    1. Load a task.
*    2. Start the task.
*    3. Use the Read function to read data from the task.
*       Set a timeout so an error is returned if the sample is
*       not returned in the specified time limit.
*    4. Stop the task.
*    5. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your input and output signals match the
*    created/modified task.
*
* Recommended Use:
*    Loading a specialized task that was created using the
*    NI-DAQmx Base Task Configuration Utility.
*
*********************************************************************/

#include "NIDAQmxBase.h"
#include <stdio.h>

#define DAQmxErrChk(functionCall) { if( DAQmxFailed(error=(functionCall)) ) { goto Error; } }

int main(int argc, char *argv[])
{
    // Task parameters
    int32       error = 0;
    TaskHandle  taskHandle = 0;
    char        errBuff[2048]={'\0'};
    char        taskName[] = "ai finite buffered";
    int32       i;

    // Data read parameters
    #define     bufferSize (uInt32)1000
    float64     data[bufferSize];
    int32       pointsToRead = bufferSize;
    int32       pointsRead;
    float64     timeout = 10.0;


    DAQmxErrChk (DAQmxBaseLoadTask(taskName,&taskHandle));
    DAQmxErrChk (DAQmxBaseStartTask(taskHandle));
    DAQmxErrChk (DAQmxBaseReadAnalogF64(taskHandle,pointsToRead,timeout,DAQmx_Val_GroupByChannel,data,bufferSize,&pointsRead,NULL));

    printf ("Acquired %d samples\n", pointsRead);

    // Just print out the first 10 points
    for (i = 0; i < 10; ++i)
       printf ("data[%d] = %f\n", i, data[i]);

Error:
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
