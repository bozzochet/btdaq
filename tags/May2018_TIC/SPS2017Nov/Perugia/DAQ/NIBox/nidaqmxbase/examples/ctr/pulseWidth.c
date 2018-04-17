/*********************************************************************
*
* ANSI C Example program:
*    pulseWidth.c
*
* Example Category:
*    CI
*
* Description:
*    This example demonstrates how to measure pulse width on a
*    Counter Input Channel. The Starting Edge, Minimum Value and
*    Maximum Value are all configurable.

*
*    This example shows how to measure pulse width on the counter's
*    default input terminal (see I/O connections for more
*    information), but could easily be expanded to measure pulse
*    width on any PFI, RTSI, or internal signal.
*
* Instructions for Running:
*    1. Select the Physical Channel which corresponds to the counter
*       you want to measure pulse width on the DAQ device.
*    2. Enter the Measurement Edge to specify what type of pulse you
*       want the counter to measure. Enter the Maximum and Minimum
*       Value to specify the range of your unknown pulse width.
*    Note: It is important to set the Maximum and Minimum Values of
*          your unknown pulse as accurately as possible so the best
*          internal timebase can be chosen to minimize measurement
*          error. The default values specify a range that can be
*          measured by the counter using the 20MHzTimebase. Use the
*          genDigPulseTrainCont example to verify that you are
*          measuring correctly on the DAQ device.
*
* Steps:
*    1. Create a task.
*    2. Create a Counter Input channel to measure Pulse Width. The
*       Edge parameter is used to determine if the counter will
*       measure high or low pulses. It is important to set the
*       Maximum and Minimum Values of your unknown pulse as
*       accurately aspossible so the best internal timebase can be
*       chosen to minimize measurement error. The default values
*       specify a range that can be measured by the counter using the
*       20MHzTimebase.
*    3. Call the Start function to arm the counter and begin
*       measuring.
*    4. Call the Read function to return the next pulse width
*       measurement. Set a timeout so an error is returned if a pulse
*       is not returned in the specified time limit.
*    5. Call the Clear Task function to clear the task.
*    6. Display an error if any.
*
* I/O Connections Overview:
*    The counter will measure pulses on the input terminal of the
*    counter specified in the Physical Channel I/O control.
*
*    In this example the pulse width will be measured on the default
*    input terminal on ctr0.
*
*    For more information on the default counter input and output
*    terminals for your device, open the NI-DAQmx Base Help, and refer
*    to Counter Signal Connections found under the Device
*    Considerations book in the table of contents.
*
* Recommended Use:
*    Call Configure and Start functions.
*    Call Read function.
*    Call Stop function at the end.
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

    // Channel parameters
    char        chan[] = "Dev1/ctr0";
    float64     min = 0.000000100;
    float64     max = 0.830000000;

    // Data read parameters
    float64     data = 0.0;
    float64     timeout = 10.0;


    DAQmxErrChk (DAQmxBaseCreateTask("",&taskHandle));
    DAQmxErrChk (DAQmxBaseCreateCIPulseWidthChan(taskHandle,chan,"",min,max,DAQmx_Val_Seconds,DAQmx_Val_Rising,""));
    DAQmxErrChk (DAQmxBaseStartTask(taskHandle));
    DAQmxErrChk (DAQmxBaseReadCounterScalarF64(taskHandle,timeout,&data,NULL));
    printf("Measured Pulse Width: %.9f sec\n",data);

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
