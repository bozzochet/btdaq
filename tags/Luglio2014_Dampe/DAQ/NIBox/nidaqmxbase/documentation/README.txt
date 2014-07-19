README for NI-DAQmx Base 2.1.0f0
27-July-2006

Copyright 2004-2006 National Instruments Corporation. All Rights Reserved.
For patents covering National Instruments products, refer to ni.com/patents.
CVI(TM), LabVIEW(TM), Measurement Studio(TM), National Instruments(TM), NI(TM),
ni.com(TM), NI-DAQ(TM), and SCXI(TM) are trademarks of National Instruments
Corporation.  Linux(r) is the registered trademark of Linus Torvalds in the
U.S. and other countries.  Product and company names mentioned herein are
trademarks or trade names of their respective companies.

--------------------------------------------------------------------------------
This file contains late breaking news about NI-DAQmx Base and supersedes any 
information found in other documentation included in the distribution. Refer to
ni.com/linux for the most recent information about Linux support at National
Instruments.
--------------------------------------------------------------------------------

SUPPORTED FUNCTIONALITY
M Series MIO (PCI/PCIe/PXI only)
  *Analog Input
    -DMA
    -Digital start and reference triggering
    -Analog start and reference triggering
    -External clocking
    -Multi-channel, multi-sample read (scaled)
    -Multi-channel, multi-sample read (raw)
    -Multi-channel, single sample read (scaled)
  *Analog Output
    -External clocking
    -Multi-channel, multi-sample write (scaled)
    -Multi-channel, single-sample write (scaled)
  *Counter Input
    -Simple event counting
    -Buffered event counting (continuous speed is limited by computer speed;
        reading a single large buffer will not cause an error)
    -Period measurement
    -Pulse width measurement
  *Counter Output
    -Single pulse
    -Continuous pulse train
    -Digital start triggering
  *Digital I/O
    -Port input
    -Line input
    -Port output
    -Line Output
    -Correlated DIO (speed is limited to about 100kHz (system dependent)
        because DMA is not supported for DIO)
NI 6508/DIO96 (PCI/PXI only)
  *Supported in NI-DAQmx Base Task Configuration Utility
  *Port Input
  *Port Output
E Series MIO (PCI/PXI only)
  *Supported in NI-DAQmx Base Task Configuration Utility
  *Analog Input
    -DMA
    -Digital start and reference triggering
    -Analog start and reference triggering
    -External clocking
    -Multi-channel, multi-sample read (scaled)
    -Multi-channel, multi-sample read (raw)
    -Multi-channel, single sample read (scaled)
  *Analog Output
    -External clocking
    -Multi-channel, multi-sample write (scaled)
    -Multi-channel, single-sample write (scaled)
  *Counter Input
    -Simple event counting
    -Buffered event counting (continuous speed is limited by computer speed;
        reading a single large buffer will not cause an error)
    -Period measurement
    -Pulse width measurement
  *Counter Output
    -Single pulse
    -Continuous pulse train
    -Digital start triggering
  *Digital I/O (Port0 only)
    -Port input
    -Port output
B Series MIO (PCI/PXI only)
  *Supported in NI-DAQmx Base Task Configuration Utility (6013/6014 only)
  *Analog Input
    -DMA
    -Digital start and reference triggering
    -Analog start and reference triggering
    -External clocking
    -Multi-channel, multi-sample read (scaled)
    -Multi-channel, multi-sample read (raw)
    -Multi-channel, single sample read (scaled)
  *Analog Output
    -External clocking
    -Multi-channel, multi-sample write (scaled)
    -Multi-channel, single-sample write (scaled)
  *Counter Input
    -Simple event counting
    -Buffered event counting (continuous speed is limited by computer speed;
        reading a single large buffer will not cause an error)
    -Period measurement
    -Pulse width measurement
  *Counter Output
    -Single pulse
    -Continuous pulse train
    -Digital start triggering
  *Digital I/O (Port0 only)
    -Port input
    -Port output
NI 671x/673x (PCI/PXI only)
  *Supported in NI-DAQmx Base Task Configuration Utility
  *Analog Output
    -External clocking
    -Multi-channel, multi-sample write (scaled)
    -Multi-channel, single-sample write (scaled)
  *Counter Input
    -Simple event counting
    -Buffered event counting (continuous speed is limited by computer speed;
        reading a single large buffer will not cause an error)
    -Period measurement
    -Pulse width measurement
  *Counter Output
    -Single pulse
    -Continuous pulse train
    -Digital start triggering
  *Digital I/O (Port0 only)
    -Port input
    -Port output
NI USB-9211
  *Supported in NI-DAQmx Base Task Configuration Utility
  *Analog Input
    -Multi-channel, multi-sample read (scaled)
    -Multi-channel, single sample read (scaled)
NI USB-9215
  *Supported in NI-DAQmx Base Task Configuration Utility
  *Analog Input
    -Multi-channel, multi-sample read (scaled)
    -Multi-channel, single sample read (scaled)
NI USB-6008/9
  *Supported in NI-DAQmx Base Task Configuration Utility
  *Analog Input
    -Digital start triggering
    -Multi-channel, multi-sample read (scaled)
    -Multi-channel, single sample read (scaled)
  *Analog Output
    -Multi-channel, multi-sample write (scaled)
    -Multi-channel, single-sample write (scaled)
  *Counter Input
    -Simple event counting (falling edge only)
  *Digital I/O
    -Line input
    -Port input
    -Port output
    -Line output
NI USB-6501
  *Supported in NI-DAQmx Base Task Configuration Utility
  *Counter Input
    -Simple event counting (falling edge only)
  *Digital I/O
    -Line input
    -Port input
    -Port output
    -Line output
NI S Series
  *Boards Supported
    -NI 6110
    -NI 6111
    -NI 6115
    -NI 6120
    -NI 6143
  *Analog Input
    -DMA
    -Digital start and reference triggering
    -Analog start and reference triggering
    -External clocking
    -Multi-channel, multi-sample read (scaled)
    -Multi-channel, multi-sample read (raw)
    -Multi-channel, single sample read (scaled)

Please check installed documentation for a list of supported functions and
parameters.


OPERATING SYSTEM SUPPORT
NI-DAQmx Base 2.1.0f0 supports Windows 2000/XP,
Mandrakelinux 10.1 Official, Mandriva Linux 2006,
SUSE LINUX Professional 10.0 and 10.1,
Red Hat Enterprise Linux WS 3 and 4,
and Mac OS X v10.3.9 and v10.4.

NOTE: For information about NI-DAQmx Base for Windows and Mac OS X, refer to
ni.com/info and enter rddqld.


SYSTEM REQUIREMENTS (Minimum System)
256 MB Memory
236 MB Disk Space for full install
NI-DAQmx Base 2.1.0f0 for Linux software for the Intel x86 architecture
requires one of the following distributions:

    Red Hat Enterprise Linux WS 3
    Red Hat Enterprise Linux WS 4
    Mandrakelinux 10.1 Official
    Mandriva Linux 2006
    SUSE Linux 10.0
    SUSE Linux 10.1

You must use the provided INSTALL script to install NI-DAQmx Base 2.1.0f0.

Refer to ni.com/linux for the most recent information about Linux
support at National Instruments.


INSTALLER NOTES
Install your hardware first before installing the driver software.
To install NI-DAQmx Base, run as root:
#./INSTALL

The installer performs the following:
  1.  Installs NI-VISA.
  2.  Installs all needed shared object files and documentation for supported
      PCI/PXI hardware.
  3.  Installs shared tools for device detection and configuration.
  4.  Installs LabVIEW 8.0, 8.2, and/or C API functionality.
  5.  Installs USB device support.
  
  For NI-DAQmx Base 2.1.0f0, the LabVIEW 8.0 support is mass compiled
  for LabVIEW 8.0.1. The VI support will function properly in LabVIEW 8.0,
  but should be mass compiled for faster load times. For a faster mass compile,
  just mass compile the following subdirectories of LabVIEW 8.0:
    examples/daqmxbase
    project
    vi.lib/addons
    vi.lib/_DAQmxBase

To uninstall NI-DAQmx Base, run as root:
#./UNINSTALL


SUSE LINUX PROFESSIONAL INSTALLATION
Before installing NI-DAQmx Base on SUSE LINUX Professional, please complete
the steps below. If they are not completed, the installer will fail with an
error.
These steps require you to be running the kernel for which you wish to configure
NI-KAL and its client kernel modules.

    1. Ensure you have installed kernel-source and kernel-syms packages
       corresponding to the version of the currently running kernel. The version
       of the currently running kernel can be determined by issuing the command
       `uname -r`.
    2. Change the directory to the /usr/src/linux-<version> directory, where
       <version> corresponds to the currently running kernel version.
    3. Run "make cloneconfig" as root to configure the sources for the
       currently running kernel.
    4. Run "make modules_prepare" as root to prepare the headers for
       compilation.
    5. THIS STEP IS STRICTLY OPTIONAL. Completing this step removes the
       warning:
       WARNING: Symbol version dump /usr/src/linux/Module.symvers is missing,
                modules will have CONFIG_MODVERSIONS disabled.
       Run "make modules" as root to compile all modules and generate
       Module.symvers; this step may take 30 minutes or more to complete.
    6. Run the INSTALL script for the NI-DAQmx Base software for Linux from this
       directory.


INSTALLATION FAQS
Q1: What action needs to be taken if I upgrade/change my kernel?

A1: An action is required to make the existing NI-DAQmx Base installation work
    for the new kernel. Run the updateNIDrivers utility located in
    /usr/local/bin as root. By default the updateNIDrivers utility rebuilds
    the driver for the currently running kernel using the kernel sources
    and copies it to the appropriate location. To verify the version of the
    currently running kernel, run `uname -r` at the command prompt. Be sure
    to have sources for your kernel properly installed and configured on
    your system before running the utility.  To compile the drivers for a
    different kernel then you are currently running, pass the kernel
    version as an argument to the utility:

         updateNIDrivers <kernelVersion>

    The kernel version format varies between distributions. Check the
    contents of /lib/modules or refer to your distribution documentation
    for more information. For the changes to take place, you must restart
    your computer after the utility completes.
    
Q2: During installation I get a message similar to the following one:
    "Kernel source does not appear to be configured for the running kernel.
     Configuration of kernel source is required to continue installation."
    I do have kernel sources installed; is there anything else that needs to
    be done?

A2: This problem has been seen on SUSE LINUX Professional 9.1 running the 2.6.5
    kernel or newer versions of SUSE LINUX Professional. However, it is possible
    that other distributions will require similar steps to resolve this problem.

    On SUSE LINUX Professional complete the following steps:
    1. Ensure you have installed kernel-source and kernel-syms packages
       corresponding to the version of the currently running kernel. The version
       of the currently running kernel can be determined by issuing the command
       `uname -r`.
    2. Change the directory to the /usr/src/linux-<version> directory, where
       <version> corresponds to the currently running kernel version.
    3. Run "make cloneconfig" as root to configure the sources for the
       currently running kernel.
    4. Run "make modules_prepare" as root to prepare the headers for
       compilation.
    5. THIS STEP IS STRICTLY OPTIONAL. Completing this step removes the
       warning:
       WARNING: Symbol version dump /usr/src/linux/Module.symvers is missing,
                modules will have CONFIG_MODVERSIONS disabled.
       Run "make modules" as root to compile all modules and generate
       Module.symvers; this step may take 30 minutes or more to complete.
    6. Run the INSTALL script for the NI-DAQmx Base software for Linux from this
       directory.


DEVICE CONFIGURATION
Anytime a new device is added or removed, you should run the following
(does not need to be root):
#lsdaq

This detects all NI-DAQmx Base supported devices.  Make note of the device name
(dev1 for example) as this is the name you need to specify when creating
channels in your program.


EXAMPLES
This release includes examples for each measurement type that demonstrates
appropriate functionality.  Examples for LabVIEW are located in
<LabVIEW>/examples/daqmxbase. They are not included in the NI Example Finder.
C examples are installed by default to /usr/local/natinst/nidaqmxbase/examples.
The make files create the links to the necessary libraries.  Be sure to run the
device configuration utility (lsdaq) before running the examples.  To compile
and run the AI example:
#cd /usr/local/natinst/nidaqmxbase/examples/ai
#make
#./acquireNScans


DOCUMENTATION
Documentation located in /usr/local/natinst/nidaqmxbase/documentation includes
the DAQmx Base 2.x C Function Reference, the hardware references, and supporting
PDFs.  LabVIEW help files are accessible from the LabVIEW Help menu.


KNOWN ISSUES

LabVIEW API:

Using DAQmxBase CreateTask, DAQmxBase Create Virtual Channel, and DAQmxBase
Clear Task in a loop leaks approximately 110 bytes per iteration.  If you pass
in a string for the taskName parameter in DAQmxBase Create Task, the memory
leak does not occur.

DAQmxBase Is Task Done does not currently support analog input.

Some parameters that aren't available in the LabVIEW API interface can be
accessed through the NI-DAQmx Base Task Configuration Utility (trigger
hysteresis, convert clock rate, etc.).  The utility can be launched from the
Tools menu and allows you to statically configure tasks.  To use static tasks,
call DAQmxBase Create Task with the 'task to copy' input with the name of your
static task.  You can then choose to modify the static task with configuration
functions or just call Start, Read/Write, Stop afterwards.  Refer to the
SUPPORTED FUNCTIONALITY section for devices the utility supports.

If, when building an executable or shared object project using NI-DAQmx Base
VIs, the Application Builder catches an error, you must restart LabVIEW before
running any NI-DAQmx Base VI.

C API:

Using DAQmxBaseCreateTask, a DAQmxBaseCreateXXChannel function, and
DAQmxBaseClearTask in a loop leaks approximately 110 bytes per iteration.
If you pass in a string for the taskName parameter in DAQmxBaseCreateTask,
the memory leak does not occur.

DAQmxBaseIsTaskDone does not currently support analog input.

Some parameters that aren't available in the C API interface can be accessed
through the NI-DAQmx Base Task Configuration Utility (trigger hysteresis,
convert clock rate, etc.).  The utility is named mxbaseconfig and allows you to
statically configure tasks.  To use static tasks, call DAQmxBaseLoadTask instead
of DAQmxBaseCreateTask.  You can then choose to modify the static task with
configuration functions or just call Start, Read/Write, Stop afterwards.  Refer
to the SUPPORTED FUNCTIONALITY section for devices the utility supports.

When linking, the following library must be linked for full functionality of
NI-DAQmx Base:
     libnidaqmxbase.so
To create a link to this library, include this line in your Makefile:
nilibs=-lnidaqmxbase
and in your compile or link step add $(nilibs). Creating this link also
builds the C API examples.

All APIs:
NI-DAQmx Base uses polynomial scaling (voltage to temperature) for thermocouple 
measurements, but the temperature range that the software supports is not 
consistent with the minimum accurate range of the inverse polynomial coefficients 
used for several thermocouple types. For more information about this, 
visit ni.com/info and enter "rditml".

NI-DAQmx Base is not multi-thread safe. In LabVIEW, use the error cluster to
force execution order. In C, use only one thread to make API calls; especially
for task/channel creation and deletion.

NI-DAQmx Base is designed to complete one process/task per board functionality
at a time. You can have one task that that does DIO and another task that does
AI on the same board at the same time. You cannot, however, run two AI tasks on
the same board at the same time.

Currently, only the factory calibration constants are loaded and used.
Self-calibration and the use of self-calibration constants are not supported.

For E Series and 67xx boards, only Port0 of DIO is currently supported.

There are three ways to configure tasks in NI-DAQmx Base. The first method,
which is preferred, involves dynamically creating the task using Create Task
followed by Create Virtual Channel. The second and third methods both use the
NI-DAQmx Base Configuration Utility; the difference between the second and third
method is how the utility is launched and used.  Refer to the SUPPORTED
FUNCTIONALITY section for devices the utility supports.
To use the second method, launch the utility from the Tools menu in LabVIEW.
Launching the utility from the Tools menu in LabVIEW includes any other tasks
that may be in memory in LabVIEW at the time the utility is launched. Any
changes made with the LabVIEW launched utility immediately takes effect in
LabVIEW and on any C program launched after the utility is exited. 
The third method involves running 'mxbaseconfig' from the command line. The
'mxbaseconfig' utility is a separate application with the same features as the
LabVIEW launched utility, but is designed for use with the C API and also as a
tool for LabVIEW built executables. The 'mxbaseconfig' utility saves its data to
the same configuration file as the LabVIEW launched version, but changes made
with this version are only be seen in LabVIEW if LabVIEW is restarted.

If your machine enters Hibernate/Sleep, restart your C or LabVIEW-built
application, or LabVIEW to properly communicate with your DAQ device.

When doing Analog Triggering for M Series, use either APFI0 or APFI1 as the
trigger source.  Any other text will result in an error.

Hardware:
USB-9215 Maximum Sample Rate
The USB-9215 maximum sample rate is determined by the highest channel number
read, not by the number of channels read.
Highest Channel Number  Maximum Sample Rate
0                       20 kS/sec
1                       10 kS/sec
2                       6.67 kS/sec
3                       5 kS/sec

USB-6009 Aggregate Scan Rates
When sampling more than 2 channels, the maximum aggregate scan rate is
determined by the number of channels.
Number of Channels  Maximum Aggregate Sample Rate
1                   48 kS/sec
2                   48 kS/sec
3                   45.5 kS/sec
4                   44 kS/sec
5                   43.5 kS/sec
6                   43 kS/sec
7                   42 kS/sec
8                   40.5 kS/sec

USB-9211 Sample Process
When performing a thermocouple task, the USB-9211 automatically reads both the
Cold-Junction and Autozero channels and uses these values when calculating the
temperature every time the scan list is sampled. Both reads count towards the
overall sample rate. Thus the effective maximum sample rate for thermocouple
tasks is 12/(N+2) S/sec where N equals the number of channels being read. For
example, if three channels are being read by a thermocouple task, the maximum
sample rate is then 12/(3 + 2) = 2.4 S/sec. Higher sample rates might be
obtained by reading the desired thermocouple channel as a voltage channel in an
Analog Input task. However, as an Analog Input task, the resulting units are
volts, and temperature calculation is not performed by the driver.
The Cold-Junction Compensation sensor can be read by using physical channel 4
as a voltage channel. The Autozero channel is not available as a physical
channel.

USB-9211 and USB-9215 Channel Voltage Range
The USB-9211 and USB-9215 devices do not provide a user-configurable channel
range. The following lists the supported channel ranges per device:
USB-9211    80 mV
USB-9215    10 V

USB-92xx LED Might Blink Sporadically
The USB-92xx LED might not blink at a steady rate when acquiring data. Sporadic
blinking is normal and does not affect data acquisition.


SUPPORT
National Instruments would appreciate feedback and bug reports about this
release.  Please start correspondence at:
http://www.ni.com/ask


CHANGES FROM PREVIOUS VERSIONS
+2.0.0 --> 2.1.0
  * Added LabVIEW 8.2 support.
  * Removed LabVIEW 7.0 and LabVIEW 7.1 support.
+1.5.0 --> 2.0.0
  * Added PCIe M Series support.
  * Added LabVIEW 8.0 support.
  * LabVIEW 8.0.1 Run-time Engine is now used for the C API and utility apps.
  * Added DAQmxBase Export Signal.vi and DAQmxBase Wait for Next
    Sample Clock.vi to the LabVIEW API. 
+1.4.0 --> 1.5.0
  * Added M Series support.
+1.3.1 --> 1.4.0
  * Added NI USB-6008/9 support.
  * Added NI USB-6501 support.
+1.2b3 --> 1.3.1
  * Added support for the 671x and 673x boards.
  * Added NI USB-9215 support.
  * Added NI USB-9211 support.
  * Added beta support for S Series boards (AI functionality only).


ERROR & WARNING CODES
These errors and warnings represent the NI-DAQmx Base supported subset of the
Windows version of NI-DAQmx.

223700
Internal software warning.

-223700
Internal software error.

-200797
An empty string was specified as a terminal name which is not supported.
Specify a valid terminal name.

-200745
Frequency and Initial Delay property values are inconsistent with one or more
counter timebase properties.  The conflicting properties must satisfy the
following constraints:
CounterTimebaseRate/CounterMaximumCount<=Frequency<=CounterTimebaseRate/4
CounterTimebaseRate/CounterMaximumCount<=1/InitialDelay<=CounterTimebaseRate/2
If the Counter Timebase Rate is not specified, it is inferred from the Counter
Timebase Source selection.

-200608
Acquisition cannot be started, because the selected buffer size is too small.
Increase the buffer size.

-200565 Specified digital channel contains more bits than supported by the
8-bit version of DAQmxBase Port Write. Use the version of DAQmxBase Port Write
that supports wider digital ports.

-200564 Specified digital channel contains more bits than supported by the
32-bit version of DAQmxBase Port Read.

-200561
Attempted writing analog data that is too large or too small.

-200559
Task cannot contain a channel with the specified channel type, because the task
already contains channels with a different channel type.  Create one task for
each channel type.

-200558
One task cannot contain multiple independent devices.  Create one task for each
independent device.

-200552
Specified string is not valid, because it contains an invalid character.

-200536
Specified value is smaller than the minimum value supported for this property.

-200527
Requested values of the Minimum and Maximum properties for the counter channel
are not supported for the given type of device.  The values that can be
specified for Minimum and Maximum depend on the counter timebase rate.

-200526
Write cannot be performed because this version of DAQmxBase Write does not
match the type of channels in the task.
Use the version of DAQmxBase Write that corresponds to the channel type.

-200525
Read cannot be performed because this version of DAQmxBase Read does not match
the type of channels in the task.
Use the version of DAQmxBase Read that corresponds to the channel type.

-200524
Write cannot be performed, because the data does not contain data for all
channels in the task.  When writing, supply data for all channels in the task
Alternatively, modify the task to contain the same number of channels as the
data written.

-200523
Read cannot be performed because this version of DAQmxBase Read only returns
data from a single channel, and there are multiple channels in the task.  Use
the multichannel version of DAQmxBase Read.

-200498
Syntax for a range of objects in the input string is invalid.
For ranges of objects, specify a number immediately before and after every
colon (":") in the input string. Or, if a name is specified after the colon, it
must be identical to the name specified immediately before the colon. Colons
are not allowed within the names of the individual objects.

-200489
Specified channel cannot be added to the task, because a channel with the same
name is already in the task.

-200485 
The specified task cannot be loaded, because it is not a valid task.

-200478
Specified operation cannot be performed when there are no channels in the task.

-200474
Specified operation did not complete, because the specified timeout expired.

-200473
Read cannot be performed when the Auto Start property is false and the task is
not running or committed.

-200459
Write failed, because there are no output channels in this task to which data
can be written.

-200452
Specified property is not supported by the device or is not applicable to the
task.

-200430
I/O type of the physical channel does not match the I/O type required for the
virtual channel you are creating.

-200428
Value passed to the Task/Channels In control is invalid.
The value must refer to a valid task or valid virtual channels.

-200361
Onboard device memory overflow. Because of system and/or bus-bandwidth
limitations, the driver could not read data from the device fast enough to keep
up with the device throughput.  Reduce the sample rate, or reduce the number of
programs your computer is executing concurrently.

-200341
Generation was configured to use only onboard memory, but the corresponding
buffer is larger than onboard memory. Buffer size is provided implicitly when
data is written or explicitly when the buffer is configured.

-200330
An attempt has been made to use the PFI0 terminal of the device for both an
analog and digital source.
Use a terminal other than PFI0 as the source of your digital signal.

-200326
An attempt has been made to perform a route when the source and the destination
are the same terminal.  In many cases, such as when configuring an external
clock or a counter source, you must select a PFI, PXI Trigger, or RTSI line as
the source terminal.

-200287
Attempted to write to an invalid combination of position and offset. The
position and offset specified a sample prior to the first sample generated
(sample 0).  Make sure any negative write offset specified will select a valid
sample when combined with the write position.

-200284
Some or all of the samples requested have not yet been acquired.
To wait for the samples to become available use a longer read timeout or read
later in your program. To make the samples available sooner, increase the
sample rate. If your task uses a start trigger, make sure that your start
trigger is configured correctly. It is also possible that you configured the
task for external timing, and no clock was supplied. If this is the case, supply
an external clock.

-200278 
Attempted to read a sample beyond the final sample acquired. The acquisition has
stopped, therefore the sample specified by the combination of position and
offset will never be available.
Specify a position and offset which selects a sample up to, but not beyond, the
final sample acquired. The final sample acquired can be determined by querying
the total samples acquired after an acquisition has stopped.

-200265
An attempt has been made to use an invalid analog trigger source.
If you explicitly named the virtual channel using DAQmxBase Create Channel, you
must use the name assigned to that channel.

-200261
An attempt has been made to use an analog trigger in multiple situations with
differing properties.
Change the analog trigger properties so they are the same, or do not use an
analog trigger for all situations.

-200254
Terminal for the device is invalid.

-200220 
Device identifier is invalid.

-200214
Analog trigger circuitry unavailable on the device.
Select a non-analog trigger type, or use a device with analog triggering
hardware.

-200170
Physical channel specified does not exist on this device.
Refer to the documentation for channels available on this device.

-200099
Physical channel not specified.

-200096
Number of samples to read must be -1 or greater.

-200089
Task name specified conflicts with an existing task name.

-200082
Minimum is greater than or equal to the maximum.

-200077
Requested value is not a supported value for this property.

-200040
Source terminal to be routed could not be found on the device.
Make sure the terminal name is valid for the specified device. Refer to help
for valid terminal names.

-200012
Specified physical channel does not support digital output.

-89136
Specified route cannot be satisfied, because the hardware does not support it.

-89131
An attempt has been made to perform a route when the source and the destination
are the same terminal.  In many cases, such as when configuring an external
clock or a counter source, you must select a PFI, PXI Trigger, or RTSI line as
the source terminal.

-89120
Source terminal to be routed could not be found on the device.  Make sure the
terminal name is valid for the specified device.

-50256
USB Device error: Feature not supported.

MHDDK ERROR & WARNING CODES
15
Resource Not Found - The application cannot get access to the hardware. Occurs
if the DAQ device is not installed on the PDA or the reference to the hardware
is invalid. For an invalid reference, restart LabVIEW.
