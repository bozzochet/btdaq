/*********************************************************************
*
* ANSI C Example program:
*    acquireNScans-DigRef.c
*
* Example Category:
*    AI
*
* Description:
*    This example demonstrates how to acquire a finite amount of data
*    using a digital reference trigger.
*
* Instructions for Running:
*    1. Select the Physical Channel to correspond to where your
*       signal is input on the DAQ device.
*    2. Enter the Minimum and Maximum Voltage Ranges.
*    Note: For better accuracy try to match the Input Ranges to the
*          expected voltage level of the measured signal.
*    3. Select how many Samples to Acquire on Each Channel.
*    4. Set the Rate of the Acquisiton.
*    Note: The Rate should be AT LEAST twice as fast as the maximum
*          frequency component of the signal being acquired.
*    5. Select the Source and Edge of the Digital Reference Trigger
*       for the acquisition.
*
* Steps:
*    1. Create a task.
*    2. Create an analog input voltage channel.
*    3. Define the parameters for an Internal Clock Source.
*       Additionally, define the sample mode to be Finite.
*    4. Define the parameters for a Digital Edge Reference Trigger.
*    5. Call the Start function to begin the acquisition.
*    6. Use the Read function to retrieve the waveform. Set a timeout
*       so an error is returned if the samples are not returned in
*       the specified time limit.
*    7. Call the Clear Task function to clear the Task.
*    8. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal input terminal matches the Physical
*    Channel I/O Control. Also, make sure your digital trigger
*    terminal matches the Trigger Source Control. For further
*    connection information, refer to your hardware reference manual.
*
* Recommended Use:
*    1. Call Configure and Start functions.
*    2. Call Read function.
*    3. Call Stop function at the end.
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
    int32       i;

    // Channel parameters
    char        chan[] = "Dev1/ai0";
    float64     min = -10.0;
    float64     max = 10.0;

    // Timing parameters
    char        clockSource[] = "OnboardClock";
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


    DAQmxErrChk (DAQmxBaseCreateTask("",&taskHandle));
    DAQmxErrChk (DAQmxBaseCreateAIVoltageChan(taskHandle,chan,"",DAQmx_Val_Cfg_Default,min,max,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxBaseCfgSampClkTiming(taskHandle,clockSource,sampleRate,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,samplesPerChan));
    DAQmxErrChk (DAQmxBaseCfgDigEdgeRefTrig(taskHandle,triggerSource,triggerSlope,triggerSamples));
    DAQmxErrChk (DAQmxBaseStartTask(taskHandle));
    DAQmxErrChk (DAQmxBaseReadAnalogF64(taskHandle,pointsToRead,timeout,DAQmx_Val_GroupByChannel,data,bufferSize,&pointsRead,NULL));

    printf ("Acquired %d samples\n", pointsRead);

    // Just print out the first 10 points
    for (i = 0; i < 10; ++i)
        printf ("data[%d] = %f\n", i, data[i]);

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
