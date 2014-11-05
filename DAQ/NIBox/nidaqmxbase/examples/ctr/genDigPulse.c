/*********************************************************************
*
* ANSI C Example program:
*    genDigPulse.c
*
* Example Category:
*    CO
*
* Description:
*    This example demonstrates how to generate a single digital pulse
*    from a Counter Output Channel. The Frequency, and Duty Cycle
*    are all configurable.
*
* Instructions for Running:
*    1. Select the Physical Channel which corresponds to the counter
*       you want to output your signal to on the DAQ device.
*    2. Enter the Frequency and Duty Cycle to define the pulse
*       parameters. Additionally, you can set the Initial Delay
*       (in seconds) which will delay the beginning of the pulse from
*       the start call; this is currently set to 0.0 in the code.
*    Note: Use the pulseWidth example to verify you are
*          outputting the pulse on the DAQ device.
*
* Steps:
*    1. Create a task.
*    2. Create a Counter Output channel to produce a Pulse in terms
*       of Frequency.
*    3. Call the Start function to arm the counter and begin the
*       pulse generation. The pulse would not begin until after the
*       Initial Delay (in seconds) has expired.
*    4. Use the IsTaskDone function to ensure the entire pulse
*       is generated before ending the task.
*    5. Call the Clear Task function to clear the Task.
*    6. Display an error if any.
*
* I/O Connections Overview:
*    The counter will output the pulse on the output terminal of the
*    counter specified in the Physical Channel I/O control.
*
*    In this example the output will be sent to the default output
*    terminal on ctr0.
*
*    For more information on the default counter input and output
*    terminals for your device, open the NI-DAQmx Base Help, and refer
*    to Counter Signal Connections found under the Device Considerations
*    book in the table of contents.
*
* Recommended Use:
*    Call Configure and Start functions.
*    Call Stop function at the end.
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
    time_t      startTime;

    // Channel parameters
    char        chan[] = "Dev1/ctr0";
    float64     freq = 1.0;
    float64     duty = 0.5;
    float64     delay = 0.0;

    // Data write parameters
    float64     timeout = 10.0;
    bool32      done = 0;


    DAQmxErrChk (DAQmxBaseCreateTask("",&taskHandle));
    DAQmxErrChk (DAQmxBaseCreateCOPulseChanFreq(taskHandle,chan,"",DAQmx_Val_Hz,DAQmx_Val_Low,delay,freq,duty));
    DAQmxErrChk (DAQmxBaseStartTask(taskHandle));

    // The loop will quit after 10 seconds
    startTime = time(NULL);
    while( !done && time(NULL)<startTime+10 ) {
        DAQmxErrChk (DAQmxBaseIsTaskDone(taskHandle,&done));
        if( !done )
            usleep(100000);
    }

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
