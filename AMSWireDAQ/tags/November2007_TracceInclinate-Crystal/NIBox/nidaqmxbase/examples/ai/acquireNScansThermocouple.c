/*********************************************************************
*
* ANSI C Example program:
*    acquireNScansThermocouple.c
*
* Example Category:
*    AI
*
* Description:
*    This example demonstrates how to acquire a finite amount of data
*    from a thermocouple channel.
*    The NI-DAQmx Base API only supports thermocouple measurements 
*    with devices that use built-in cold junction compensation.
*
* Instructions for Running:
*    1. Select the physical channel to correspond to where your signal
*       is input on the DAQ device.
*    2. Enter the minimum and maximum temperature range.
*       Note: For better accuracy try to match the input range to the
*       expected temperature level of the measured signal.
*    3. Set the thermocouple type and requested temperature units
*    4. Set the number of samples to acquire per channel.
*    5. Set the rate of the acquisiton.
*       Note: The rate should be AT LEAST twice as fast as the maximum
*       frequency component of the signal being acquired.
*
* Steps:
*    1. Create a task.
*    2. Create an analog input thermocouple channel.
*    3. Set the rate for the sample clock. Additionally, define the
*       sample mode to be finite.
*    4. Call the Start function to start the acquistion.
*    5. Read all of the waveform data.
*    6. Call the Clear Task function to stop the acquisition.
*    7. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal input terminal matches the Physical Channel
*    I/O Control. In this case wire your signal to the ai0 pin on your
*    DAQ Device. By default, this will also be the signal used as the
*    analog start trigger.
*
* Recommended use:
*    Call Configure and Start functions.
*    Call Read function.
*    Call Stop function at the end.
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
    char        errBuff[2048]={'\0'};
    int32       i;

    // Channel parameters
    char        chan[] = "Dev1/ai0";
    float64     min = 0;
    float64     max = 100.0;

    // Timing parameters
    char        source[] = "OnboardClock";
    uInt64      samplesPerChan = 10;
    float64     sampleRate = 4.0;

    // Data read parameters
    #define     bufferSize (uInt32)10
    float64     data[bufferSize];
    int32       pointsToRead = 10;
    int32       pointsRead;
    float64     timeout = 10.0;


    DAQmxErrChk (DAQmxBaseCreateTask ("", &taskHandle));
    DAQmxErrChk (DAQmxBaseCreateAIThrmcplChan (taskHandle, chan, "", min, max, DAQmx_Val_DegC , DAQmx_Val_K_Type_TC, DAQmx_Val_BuiltIn , 0, ""));
    DAQmxErrChk (DAQmxBaseCfgSampClkTiming (taskHandle, source, sampleRate, DAQmx_Val_Rising, DAQmx_Val_FiniteSamps, samplesPerChan));

    DAQmxErrChk (DAQmxBaseStartTask (taskHandle));

    DAQmxErrChk (DAQmxBaseReadAnalogF64 (taskHandle, pointsToRead, timeout, 0, data, bufferSize, &pointsRead, NULL));

    printf ("Acquired %d samples\n", pointsRead);

    // print out the first 10 points
    for (i = 0; i < 10; ++i)
       printf ("data[%d] = %f\n", i, data[i]);

Error:
    if (DAQmxFailed (error))
       DAQmxBaseGetExtendedErrorInfo (errBuff, 2048);

    if (taskHandle != 0)
    {
       DAQmxBaseStopTask (taskHandle);
       DAQmxBaseClearTask (taskHandle);
    }

    if (error)
       printf ("DAQmxBase Error: %d\n", error);

    return 0;
}
