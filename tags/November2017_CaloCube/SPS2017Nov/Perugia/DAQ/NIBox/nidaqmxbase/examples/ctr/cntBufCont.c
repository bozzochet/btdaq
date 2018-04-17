/*********************************************************************
*
* ANSI C Example program:
*    cntBufCont.c
*
* Example Category:
*    CI
*
* Description:
*    This example demonstrates how to count buffered digital events on
*    a Counter Input Channel. The Count Direction, Edge,
*    and Sample Clock Source are all configurable.
*
*    Edges are counted on the counter's default input terminal
*    (see I/O Connections for more information), but could easily be
*    modified to count edges on a PFI or RTSI line.
*
*    Note: For buffered event counting, an external sample clock is
*    necessary to signal when a sample should be inserted into the buffer.
*    This is set by the Sample Clock Source control.
*
* Instructions for Running:
*    1. Select the Physical Channel which corresponds to the counter
*       you want to count edges on the DAQ device.
*    2. Enter the Count Direction and measurement Edge to specify how
*       you want the counter to count. Set the Sample Clock Source.
*       Note:  An external sample clock must be used.  Counters do not
*       have an internal sample clock available.  You can use the
*       genDigPulseTrainCont example to generate a pulse train
*       on another counter and connect it to the Sample Clock Source you
*       are using in this example.
*
* Steps:
*    1. Create a task.
*    2. Create a Counter Input channel to Count Events. The Edge
*       parameter is used to determine if the counter will increment
*       on rising or falling edges.
*    3. Call the DAQmx Base Timing function (Sample Clock) to configure
*       the external sample clock timing parameters such as Sample Mode
*       and Sample Clock Source. The Sample Clock Source determines
*       when a sample will be inserted into the buffer. The 100kHz
*       and 20MHz timebases cannot be used as the Sample Clock Source.
*       The Edge parameter can be used to determine when a sample is
*       taken.
*    4. Call the Start function to arm the counter and begin counting.
*       The counter will be preloaded with the Initial Count.
*    5. For continuous measurements, the counter will continually read
*       all available data until the Stop button is pressed on the
*       front panel.
*    6. Call the Clear Task function to clear the Task.
*    7. Display an error if any.
*
* I/O Connections Overview:
*   The counter will count edges on the input terminal of the counter
*   specified in the Physical Channel I/O control.
*
*   For E-Series counters, the default input terminal for edge
*   measurements on each physical channel is:
*   ctr0 - PFI8
*   ctr1 - PFI3
*
*   In this example the counter will count edges on the default input
*   terminal on ctr0 (PFI8). The counter will take measurements on
*   valid edges of the external Sample Clock Source which is PFI9 in
*   this example.  For more information on default input terminals for
*   counters, refer to the "Making signal Connections for Counters"
*   section of the NI-DAQmx Base help.
*
* Recommended use:
*    Call Configure and Start functions. Call Read function in a loop.
*    Call Stop function at the end.
*
*********************************************************************/
#include "NIDAQmxBase.h"
#include <stdio.h>
#include <time.h>

#define DAQmxErrChk(functionCall) { if( DAQmxFailed(error=(functionCall)) ) { goto Error; } }

static int gRunning;

int main(void)
{
    // Task parameters
    int32       error = 0;
    TaskHandle  taskHandle = 0;
    char        errBuff[2048]={'\0'};
    time_t      startTime;

    // Channel parameters
    char        chan[] = "Dev1/ctr0";

    // Timing parameters
    #define     bufferSize 100
    char        clockSource[] = "/Dev1/PFI9";
    uInt64      samplesPerChan = bufferSize;
    float64     sampleRate = 100.0;

    // Data read parameters
    uInt32      data[bufferSize];
    int32       samplesToRead = 100;
    float64     timeout = 10.0;
	int32       read;


    DAQmxErrChk (DAQmxBaseCreateTask("",&taskHandle));
    DAQmxErrChk (DAQmxBaseCreateCICountEdgesChan(taskHandle,chan,"",DAQmx_Val_Rising,0,DAQmx_Val_CountUp));
    DAQmxErrChk (DAQmxBaseCfgSampClkTiming(taskHandle,clockSource,sampleRate,DAQmx_Val_Rising,DAQmx_Val_ContSamps,samplesPerChan));

    DAQmxErrChk (DAQmxBaseStartTask(taskHandle));
	gRunning = 1;
	// The loop will quit after 10 seconds
	startTime = time(NULL);
	while( gRunning && time(NULL)<startTime+10 ) {
        DAQmxErrChk (DAQmxBaseReadCounterU32(taskHandle,samplesToRead,timeout,data,samplesToRead,&read,NULL));
		printf("Acquired %d samples\n",read);
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
