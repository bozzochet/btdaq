/*********************************************************************
*
* ANSI C Example program:
*    contAcquire-ExtClk-DigStart.c
*
* Example Category:
*    AI
*
* Description:
*    This example demonstrates how to acquire a continuous amount of
*    data using an external sample clock, started by a digital edge.
*
* Instructions for Running:
*    1. Select the physical channel to correspond to where your
*       signal is input on the DAQ device.
*    2. Enter the minimum and maximum voltage ranges.
*    Note: For better accuracy try to match the Input Ranges to the
*          expected voltage level of the measured signal.
*    3. Select a source for the sample clock.
*    4. Set the approximate Rate of the external clock. This allows
*       the internal characteristics of the acquisition to be as
*       efficient as possible. Also set the Samples to Read.
*    5. Select a source for the digital edge start trigger.
*    6. Select the edge, rising or falling, on which to trigger.
*
* Steps:
*    1. Create a task.
*    2. Create an analog input voltage channel.
*    3. Define the parameters for an External Clock Source.
*       Additionally, define the sample mode to be continuous. The
*       external clock rate is given to allow the internal
*       characteristics of the acquisition to be as efficient as
*       possible.
*    4. Set the parameters for a digital edge start trigger.
*    5. Call the Start function to start the acquisition.
*    6. Read the waveform data in a loop until 10 seconds or an
*       error occurs.
*    7. Call the Clear Task function to clear the Task.
*    8. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal input terminal matches the Physical
*    Channel I/O control. Also, make sure that your digital trigger
*    signal is connected to the terminal specified in Trigger Source.
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
#include <unistd.h>

#define DAQmxErrChk(functionCall) { if( DAQmxFailed(error=(functionCall)) ) { goto Error; } }

int main(void)
{
    // Task parameters
    int32       error = 0;
    TaskHandle  taskHandle = 0;
    char        errBuff[2048]={'\0'};
    int32       i;
    time_t      startTime;

    // Channel parameters
    char        chan[] = "Dev1/ai0";
    float64     min = -10.0;
    float64     max = 10.0;

    // Timing parameters
    char        clockSource[] = "/Dev1/PFI7";
    uInt64      samplesPerChan = 1000;
    float64     sampleRate = 10000.0;

    // Triggering parameters
    char        triggerSource[] = "/Dev1/PFI0";
    uInt32      triggerSlope = DAQmx_Val_RisingSlope;
    uInt32      triggerSamples = 100;

    // Data read parameters
    #define     bufferSize (uInt32)1000
    float64     data[bufferSize];
    int32       pointsToRead = bufferSize;
    int32       pointsRead;
    float64     timeout = 10.0;
    int32       totalRead = 0;


    DAQmxErrChk (DAQmxBaseCreateTask("",&taskHandle));
    DAQmxErrChk (DAQmxBaseCreateAIVoltageChan(taskHandle,chan,"",DAQmx_Val_Cfg_Default,min,max,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxBaseCfgSampClkTiming(taskHandle,clockSource,sampleRate,DAQmx_Val_Rising,DAQmx_Val_ContSamps,1000));
    DAQmxErrChk (DAQmxBaseCfgDigEdgeStartTrig(taskHandle,triggerSource,triggerSlope));
    DAQmxErrChk (DAQmxBaseStartTask(taskHandle));

    // The loop will quit after 10 seconds

    startTime = time(NULL);
    while( time(NULL)<startTime+10 ) {
        DAQmxErrChk (DAQmxBaseReadAnalogF64(taskHandle,pointsToRead,timeout,DAQmx_Val_GroupByChannel,data,bufferSize,&pointsRead,NULL));
        totalRead += pointsRead;
        printf("Acquired %d samples. Total %d\n",pointsRead,totalRead);
        // Just print out the first 10 points of the last data read
        for (i = 0; i < 10; ++i)
            printf ("data[%d] = %f\n", i, data[i]);

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
