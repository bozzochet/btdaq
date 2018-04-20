/*********************************************************************
*
* ANSI C Example program:
*    acquireNScans-AnlgStart.c
*
* Example Category:
*    AI
*
* Description:
*    This example demonstrates how to acquire a finite amount of data
*    using the DAQ device's internal clock, started by an analog edge
*    condition.
*
* Instructions for Running:
*    1. Select the physical channel to correspond to where your
*       signal is input on the DAQ device.
*    2. Enter the minimum and maximum voltage range.
*    Note: For better accuracy try to match the input range to the
*          expected voltage level of the measured signal.
*    3. Set the number of samples to acquire per channel.
*    4. Set the rate of the acquisiton.
*    Note: The rate should be AT LEAST twice as fast as the maximum
*          frequency component of the signal being acquired.
*    5. Set the source of the start trigger. By default this is
*       analog input channel 0.
*    6. Set the slope and level of desired analog edge condition.
*
* Steps:
*    1. Create a task.
*    2. Create an analog input voltage channel.
*    3. Set the rate for the sample clock. Additionally, define the
*       sample mode to be finite.
*    4. Define the parameters for an Analog Slope Start Trigger.
*    5. Call the Start function to start the acquistion.
*    6. Read all of the waveform data.
*    7. Call the Clear Task function to stop the acquistion.
*    8. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal input terminal matches the Physical
*    Channel I/O Control. In this case wire your signal to the ai0
*    pin on your DAQ Device. The analog start trigger must be connected
*    to PFI0.
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
    char        triggerSource[] = "PFI0";
    uInt32      triggerSlope = DAQmx_Val_RisingSlope;
    float64     triggerLevel = 1.0;

    // Data read parameters
    #define     bufferSize (uInt32)1000
    float64     data[bufferSize];
    int32       pointsToRead = bufferSize;
    int32       pointsRead;
    float64     timeout = 10.0;


    DAQmxErrChk (DAQmxBaseCreateTask("",&taskHandle));
    DAQmxErrChk (DAQmxBaseCreateAIVoltageChan(taskHandle,chan,"",DAQmx_Val_Cfg_Default,min,max,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxBaseCfgSampClkTiming(taskHandle,clockSource,sampleRate,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,samplesPerChan));
    DAQmxErrChk (DAQmxBaseCfgAnlgEdgeStartTrig(taskHandle,triggerSource,triggerSlope,triggerLevel));
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
