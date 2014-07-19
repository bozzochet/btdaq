/*********************************************************************
*
* ANSI C Example program:
*    contAcquireNChan.c
*
* Example Category:
*    AI
*
* Description:
*    This example demonstrates how to continuously acquire data on
*    multiple channels using the DAQ device's internal clock.
*
* Instructions for Running:
*    1. Select the physical channels to correspond to where your
*       signals are input on the DAQ device.
*    2. Enter the minimum and maximum voltage range.
*    Note: For better accuracy try to match the input range to the
*          expected voltage level of the measured signal.
*    3. Set the rate of the acquisition. Also set the Samples to Read
*       control. This will determine how many samples are read each
*       time the while loop iterates. This also determines how many
*       points are plotted on the graph each iteration.
*    Note: The rate should be at least twice as fast as the maximum
*          frequency component of the signal being acquired.
*
* Steps:
*    1. Create a task.
*    2. Create an analog input voltage channel.
*    3. Set the rate for the sample clock. Additionally, define the
*       sample mode to be continuous.
*    4. Call the Start function to start the acquistion.
*    5. Read the data in a loop until 10 seconds or an
*       error occurs.
*    6. Call the Clear Task function to clear the task.
*    7. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal input terminal matches the Physical
*    Channel I/O control. Also, make sure your analog trigger
*    terminal matches the Trigger Source Control. For further
*    connection information, refer to your hardware reference manual.
*
* Recommended Use:
*    1. Call Configure and Start functions.
*    2. Call Read function in a loop.
*    3. Call Stop function at the end.
*
*********************************************************************/

#include "NIDAQmxBase.h"
#include <stdio.h>
#include <time.h>

#define DAQmxErrChk(functionCall) { if( DAQmxFailed(error=(functionCall)) ) { goto Error; } }


int main(int argc, char *argv[])
{
    // Task parameters
    int32       error = 0;
    TaskHandle  taskHandle = 0;
    char        errBuff[2048]={'\0'};
    int32       i,j;
    time_t      startTime;
    bool32      done=0;

    // Channel parameters
    char        chan[] = "Dev1/ai0, Dev1/ai1";
    float64     min = -10.0;
    float64     max = 10.0;

    // Timing parameters
    char        clockSource[] = "OnboardClock";
    uInt64      samplesPerChan = 1000;
    float64     sampleRate = 5000.0;

    // Data read parameters
    #define     bufferSize (uInt32)1000
    float64     data[bufferSize * 2];
    int32       pointsToRead = bufferSize;
    int32       pointsRead;
    float64     timeout = 10.0;
    int32       totalRead = 0;


    printf("Press Ctrl-C to exit\n");
    DAQmxErrChk (DAQmxBaseCreateTask("",&taskHandle));
    DAQmxErrChk (DAQmxBaseCreateAIVoltageChan(taskHandle,chan,"",DAQmx_Val_Cfg_Default,min,max,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxBaseCfgSampClkTiming(taskHandle,clockSource,sampleRate,DAQmx_Val_Rising,DAQmx_Val_ContSamps,samplesPerChan));
    DAQmxErrChk (DAQmxBaseCfgInputBuffer(taskHandle,200000)); //use a 100,000 sample DMA buffer
    DAQmxErrChk (DAQmxBaseStartTask(taskHandle));

    // The loop will quit after 10 seconds

    startTime = time(NULL);
    while( time(NULL)<startTime+10 ) {
        DAQmxErrChk (DAQmxBaseReadAnalogF64(taskHandle,pointsToRead,timeout,DAQmx_Val_GroupByScanNumber,data,bufferSize*2,&pointsRead,NULL));
        totalRead += pointsRead;
        printf("Acquired %d samples. Total %d\n",pointsRead,totalRead);
        // Just print out the first 10 points of the last data read
        for (i = 0; i < 10; ++i)
            printf ("data0[%d] = %f\tdata1[%d] = %f\n",i,data[2*i],i,data[2*i+1]);

    }
    printf("\nAcquired %d total samples.\n",totalRead);

Error:
    if( DAQmxFailed(error) )
        DAQmxBaseGetExtendedErrorInfo(errBuff,2048);
    if(taskHandle != 0) {
        DAQmxBaseStopTask (taskHandle);
        DAQmxBaseClearTask (taskHandle);
    }
    if( DAQmxFailed(error) )
        printf("DAQmxBase Error: %s\n",errBuff);
    return 0;
}
