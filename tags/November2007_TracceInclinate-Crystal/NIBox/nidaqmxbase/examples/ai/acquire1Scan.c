/*********************************************************************
*
* ANSI C Example program:
*    acquire1Scan.c
*
* Example Category:
*    AI
*
* Description:
*    This example demonstrates how to take a single Measurement
*    (Sample) from an Analog Input Channel.
*
* Instructions for Running:
*    1. Select the Physical Channel to correspond to where your
*       signal is input on the DAQ device.
*    2. Enter the Minimum and Maximum Voltage Ranges.
*    Note: For better accuracy try to match the Input Ranges to the
*          expected voltage level of the measured signal.
*    Note: Use the voltUpdate example to provide a signal on
*          your DAQ device to measure.
*
* Steps:
*    1. Create a task.
*    2. Create an Analog Input Voltage Channel.
*    3. Use the Read function to Measure 1 Sample from 1 Channel on
*       the Data Acquistion Card. Set a timeout so an error is
*       returned if the sample is not returned in the specified time
*       limit.
*    4. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal input terminal matches the Physical
*    Channel I/O Control.
*
* Recommended Use:
*    Call the Read function.
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

    // Channel parameters
    char        chan[] = "Dev1/ai0";
    float64     min = -10.0;
    float64     max = 10.0;

    // Timing parameters
    uInt64      samplesPerChan = 1;

    // Data read parameters
    float64     data;
    int32       pointsToRead = 1;
    int32       pointsRead;
    float64     timeout = 10.0;


    DAQmxErrChk (DAQmxBaseCreateTask("",&taskHandle));
    DAQmxErrChk (DAQmxBaseCreateAIVoltageChan(taskHandle,chan,"",DAQmx_Val_Cfg_Default,min,max,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxBaseStartTask(taskHandle));
    DAQmxErrChk (DAQmxBaseReadAnalogF64(taskHandle,pointsToRead,timeout,DAQmx_Val_GroupByChannel,&data,samplesPerChan,&pointsRead,NULL));

    printf ("Acquired reading: %f\n", data);

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
