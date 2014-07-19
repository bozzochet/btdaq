/*********************************************************************
*
* ANSI C Example program:
*    buffPeriodFinite.c
*
* Example Category:
*    CI
*
* Description:
*    This example demonstrates how to measure a finite number of
*    periods on a Counter Input Channel. The Minimum Value,
*    Maximum Value, and Samples per Channel are all configurable.
*
*    This example shows how to measure period on the counter's
*    default input terminal (see I/O Connections for more
*    information) but could easily be expanded to measure semi-period
*    on any PFI, RTSI, or internal signal.
*
*    Period measurement differs from pulse width measurement in
*    that it measures both the high and the low pulses of a given
*    signal. So for every period, two data points will be returned.
*
* Instructions for Running:
*    1. Select the Physical Channel which corresponds to the counter
*       you want to measure period on the DAQ device.
*    2. Enter the Maximum and Minimum Value to specify the range of
*       your unknown periods.
*    Note: It is important to set the Maximum and Minimum Values of
*          your unknown semi-period as accurately as possible so the
*          best internal timebase can be chosen to minimize
*          measurement error. The default values specify a range that
*          can be measured by the counter using the 20MHzTimebase.
*          Use the genDigPulseTrainCont example to verify that
*          you are measuring correctly on the DAQ device.
*
* Steps:
*    1. Create a task.
*    2. Create a Counter Input channel to measure Period. It is
*       important to set the Maximum and Minimum Values of your
*       unknown period as accurately as possible so the best internal
*       timebase can be chosen to minimize measurement error. The
*       default values specify a range that can be measured by the
*       counter using the 20MHzTimebase. The Maximum Value will be
*       the largest amount of time between 2 adjacent edges. The
*       Minimum Value will be the smallest amount of time between 2
*       adjacent edges.
*    3. Call the DAQmx Base Timing function (Implicit) to configure the
*       Sample Mode and Samples per Channel.
*    Note: For time measurements with counters, the implicit timing
*          function is used because the signal being measured itself
*          determines the sample rate.
*    4. Call the Start function to arm the counter and begin
*       measuring.
*    5. For finite measurements, the counter will stop reading data
*       when the Samples per Channel have been received. For
*       continuous measurements, the counter will continually read
*       all available data until told to stop.
*    6. Call the Clear Task function to clear the Task.
*    7. Display an error if any.
*
* I/O Connections Overview:
*    The counter will measure period on the input terminal of
*    the counter specified in the Physical Channel I/O control.
*
*    In this example the period will be measured on the default
*    input terminal on ctr0.

*
*    For more information on the default counter input and output
*    terminals for your device, open the NI-DAQmx Base Help, and refer
*    to Counter Signal Connections found under the Device
*    Considerations book in the table of contents.
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

int main(void)
{
    // Task parameters
    int32       error = 0;
    TaskHandle  taskHandle = 0;
    char        errBuff[2048]={'\0'};
    int32       i;

    // Channel parameters
    char        chan[] = "Dev1/ctr0";
    float64     min = 0.000000100;
    float64     max = 0.838860750;

    // Timing parameters
    #define     bufferSize 100
    uInt64      samplesPerChan = bufferSize;


    // Data read parameters
    float64     data[bufferSize];
    float64     timeout = 10.0;
    int32       pointsRead;


    DAQmxErrChk (DAQmxBaseCreateTask("",&taskHandle));
    DAQmxErrChk (DAQmxBaseCreateCIPeriodChan(taskHandle,chan,"",min,max,DAQmx_Val_Seconds,DAQmx_Val_Rising,DAQmx_Val_LowFreq1Ctr,0.0,0,""));
    DAQmxErrChk (DAQmxBaseCfgImplicitTiming(taskHandle,DAQmx_Val_FiniteSamps,samplesPerChan));
    DAQmxErrChk (DAQmxBaseStartTask(taskHandle));
    DAQmxErrChk (DAQmxBaseReadCounterF64(taskHandle,samplesPerChan,timeout,data,samplesPerChan,&pointsRead,NULL));
    printf("Acquired %d samples\n",pointsRead);

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
