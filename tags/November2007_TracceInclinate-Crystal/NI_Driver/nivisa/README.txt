National Instruments NI-VISA(TM) Software for Linux x86. Version 4.0.
Copyright(c) 1995, 2006 National Instruments Corporation. All rights reserved.

NATIONAL INSTRUMENTS HOLDS MULTIPLE COPYRIGHTS, TRADEMARKS, AND PATENTS
FOR THIS PRODUCT.  BY DOWNLOADING AND/OR INSTALLING THIS PRODUCT, YOU AGREE
TO THE TERMS OF THE LICENSE.txt FILE THAT RESIDES IN THE SAME DIRECTORY AS
THIS readme.txt FILE ON THE CD AND/OR WEB SITE.  FOR PATENTS COVERING
NATIONAL INSTRUMENTS PRODUCTS, REFER TO THE patents.txt FILE ON THE ROOT
DIRECTORY OF YOUR CD (IF APPLICABLE) AND/OR www.ni.com/patents.


CONTENTS OF THIS FILE
---------------------
System Requirements
Installation From the Internet
Installation FAQs
LabWindows(TM)/CVI Run-Time Engine
VISA Support for Multiple GPIB-VXI Controllers
VISA Probes for LabVIEW 7.x
USB Issues on Linux
NI-VISA 2.5 Improvements and Bug Fixes
NI-VISA 2.6 Improvements and Bug Fixes
NI-VISA 3.0 Improvements and Bug Fixes
NI-VISA 3.1 Improvements and Bug Fixes
NI-VISA 3.2 Improvements and Bug Fixes
NI-VISA 3.3 Improvements and Bug Fixes
NI-VISA 3.4 Improvements and Bug Fixes
NI-VISA 3.5 Improvements and Bug Fixes
NI-VISA 4.0 Improvements and Bug Fixes
Credits
Known Issues


SYSTEM REQUIREMENTS
-------------------
NI-VISA 4.0 for Linux software for the Intel x86 architecture requires
one of the following distributions:

    Mandrakelinux 10.1 Official
    Mandriva Linux 2006
    SUSE Linux 10.0
    SUSE Linux 10.1
    Red Hat Enterprise Linux WS 3
    Red Hat Enterprise Linux WS 4

You must use the provided INSTALL script to install NI-VISA 4.0. Manual
installation using rpm is not supported.

At the time of release, National Instruments supports NI-VISA 4.0 only
on the above listed distributions and versions.  For information on
NI-VISA compatibility on other versions of these distributions,
including earlier versions of the distributions listed above, refer
to KB 3XAAMQV4 on ni.com, "What Linux distributions do National
Instruments' drivers and software support?".

After installation, you may need to reboot your machine before attempting
to use NI-VISA 4.0. The installer will prompt you to reboot if it is
required.

Refer to ni.com/linux for the most recent information about Linux
support at National Instruments.


INSTALLATION FROM THE INTERNET
------------------------------
You should be logged in as 'root' to perform this installation.

1) Download the installation script file "INSTALL" and the compressed tar
   file "NICVISA_.TZ" from the ftp site into a temporary directory.

2) Type ./INSTALL to execute the install script, and follow the instructions
   as prompted.

3) Add the following environment variables to your shell setup file as shown
   in the following example for csh. The following commands assume that
   you installed NI-VISA into /usr/local/vxipnp:

   setenv VXIPNPPATH       /usr/local/vxipnp
   setenv LD_LIBRARY_PATH  /usr/local/vxipnp/linux/bin

Note:  If your setup file already contains the LD_LIBRARY_PATH environment
       variable, you need to append /usr/local/vxipnp/linux/bin to the end of
       the statement. The required commands may vary from those shown in the
       previous section if you are not using csh.


INSTALLATION FAQS
-----------------
Q1: What action needs to be taken if I upgrade/change my kernel?

A1: Some action is required to make the existing NI-VISA installation
    work for the new kernel. After upgrading your kernel, run
    updateNIDrivers utility as root. The utility is located in
    /usr/local/bin. Be sure to have sources for your new kernel properly
    installed on your system before running the utility. The utility
    rebuilds the driver using the new kernel sources and copies it to
    the appropriate location for the new kernel. For the changes
    to take place, you will be asked to reboot your machine after the
    utility completes.

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
    6. Run the INSTALL script for the NI-VISA software for Linux from this
       directory.


LABWINDOWS/CVI RUN-TIME ENGINE
------------------------------
The INSTALL script now automatically installs the LabWindows/CVI Run-Time
Engine when necessary. The NI-VISA interactive utility, VISAIC, requires the
LabWindows/CVI Run-Time Engine 5.0.1 or later to be installed. When you
install this utility, the INSTALL script also installs the LabWindows/CVI
Run-Time Engine if it is not already present.


VISA SUPPORT FOR MULTIPLE GPIB-VXI CONTROLLERS
----------------------------------------------
NI-VISA supports multiple GPIB-VXI controllers in a single system. Because the
National Instruments GPIB-VXI controller ships with a primary address of 1,
NI-VISA automatically searches for any GPIB-VXI controllers at that address.

If you have changed the primary address (for example, if you added a second
GPIB-VXI controller to your system, and set the address of the second
controller to 9), or if you have a GPIB-VXI controller from another vendor,
then you need to use the VISA configuration utility so NI-VISA can find it.
This app's path is <VXIPNPPATH>/linux/NIvisa/visaconf.  While in the Resource
Editor, click the 'Add' button to add a new resource.  Specify a valid
resource name for the new controller, such as GPIB-VXI2::0::INSTR.  You will
then be prompted for the GPIB board number and the GPIB-VXI's primary and
secondary addresses.  For most users, the only number that will change
(from the default selections) is the primary address (for the above example,
you would change it to 9).  Click 'OK' to add this new resource, and then
select the File>>Save menu option to save this to the NI-VISA database.

If you are using a GPIB-VXI command module from another vendor, you also need
to obtain and install that vendor's VISA GPIB-VXI component.

If you have a GPIB device at primary address 1 and that device does not
recognize "*IDN?", you should disable the GPIB-VXI auto-detect feature in
visaconf on the Settings tab.


VISA PROBES FOR LABVIEW 7.x
---------------------------
The VISA probes for LabVIEW 7.x greatly extend the amount of relevant
information displayed when using LabVIEW probes on VISA refnums. They are used
automatically by LabVIEW in place of the generic probe when they are
installed.

The NI-VISA installer will install the probes for LabVIEW 7.1. However,
if you are using LabVIEW 7.0, you can download and install them yourself.
To install them, download them from "ftp.ni.com/support/visa/labview/probes/"
and copy them into "<LABVIEW_DIR>/vi.lib/_probes/default/VISA/" where
<LABVIEW_DIR> is the installation directory of LabVIEW.


USB ISSUES ON LINUX
-------------------
NI-VISA relies on a Linux kernel feature for its USB support. This feature
is called "usbfs", and on older Linux kernels was referred to as "usbdevfs".
For NI-VISA to support USB devices, this feature must be present and 
mounted (like a virtual filesystem). This is supported in most major Linux
distributions such as Red Hat, SUSE, and Mandrake. You may use the "mount"
command to display what filesystems are currently mounted to see if your
system currently supports this feature.

Also, the VISA user must have write accress to the file that represents
the USB device, which is typically somewhere in a subdirectory within
"/proc/bus/usb". If this is not the case, the USB device is not
accessible by VISA (it will not be found using viFindRsrc and viOpen will
fail). The default configuration on most systems is that the 'root' user
has write access; however, no other user has this access.

There are a number of options that you can take to provide a non-root
user access to a USB device.

1) Use the hotplug package. This package is installed by default on most
distributions including Red Hat, SUSE, and Mandrake. The hotplug package
allows the user to run scripts for a specific USB device based on 
characteristics such as Vendor ID (VID) and Product ID (PID). If the
hotplug package exists, the NI-VISA Installer by default will install
scripts to give all users write access to all USB TMC devices and a 
framework for USB RAW devices. To add write permissions for a specific
USB RAW device, simply run the included script: 
"<VXIPNPPATH>/linux/NIvisa/USB/AddUsbRawPermissions.sh".
For more information on the hotplug package, please visit:
http://linux-hotplug.sourceforge.net

2) "usbfs" (formerly known as "usbdevfs") may be mounted with the option
"devmode=0666". This gives all users read and write access to all USB
devices.

3) The 'root' user may add write permissions to the file that represents
the USB device which is typically somehere in a subdirectory within
"/proc/bus/usb". Unfortunately, these permissions will be lost if the
device is unplugged and then plugged back in. Because of this, this
approach is not recommended.


NI-VISA 2.5 IMPROVEMENTS AND BUG FIXES
--------------------------------------
GPIB and Serial performance have been improved over Version 2.01.

If you are using the GPIB-ENET controller, the previous version of NI-VISA
would take a minute or more to time out if a GPIB-ENET controller were not
connected or powered on. The timeout is now much shorter.

This version of NI-VISA incorporates general bug fixes and improvements for
GPIB-VXI. The NI-VISA code instrument will now load properly in configurations
where it previously failed.

If a GPIB-VXI chassis is reset while an application has VISA loaded, the
GPIB-VXI would previously become accessible only as GPIB but not GPIB-VXI.
This release of NI-VISA will now properly re-detect the GPIB-VXI.

Fixed bug with the GPIB-VXI and hardware triggers.

This version of NI-VISA incorporates general bug fixes and improvements for
formatted I/O, especially with very large buffers and with the following
modifiers: "%b", ",", "#", "!ob", "!ol", and "%[".

The documentation for viPrintf and viFlush better describes when formatted
I/O buffers are sent. The documentation for viScanf better describes the ","
and "#" modifiers.

Fixed bug with the Serial End Last Bit mode for reads and writes.


NI-VISA 2.6 IMPROVEMENTS AND BUG FIXES
--------------------------------------
Fixed bug with enabling the exception event on GPIB-VXI.

viScanf %f modifier will now properly scan numbers bigger than 2^31-1.

viGpibPassControl now returns an error if the primary and/or secondary address
is invalid.

In VXI, mapping a trigger line already mapped produced VI_ERROR_LINE_IN_USE 
(0xBFFF0042), instead of VI_SUCCESS_TRIG_MAPPED (0x3FFF007E).  This now
returns the expected status code.

In VXI and GPIB-VXI, using viMoveInXX and viMoveOutXX with a negative 
value for the length parameter would occasionally cause the system to reboot.
This now returns the error code VI_ERROR_INV_LENGTH (0xBFFF0083) instead.

viReadToFile now writes the correct data to the file.

For TCP/IP SOCKET and INSTR resources, the openTimeout specified in viOpen
is now used as the network timeout during the connection sequence.  The
default openTimeout, if not specified, is 2 seconds (2000 milliseconds).

For TCP/IP INSTR resources, trying to do a viRead or viReadAsync on a buffer
with a size larger than a few kilobytes, would either return an error or
occasionally crash.  This has been fixed.

viFindRsrc would previously reject certain valid expressions and accept
certain invalid expressions, usually involving spaces.  This has been fixed.

The NI-VISA driver would not initialize properly on libc version 5.4.44.
This has been fixed.

If an application called fork, the NI-VISA driver didn't initialize
properly in the child process.  Exiting either the child or parent process
caused the other's VISA calls to stop working.

In Serial, we now support baud rates higher than 38400.  Specifically,
the values 57600, 115200, 230400, and 460800 are now valid as long as the
system driver allows them.

In Serial, if data arrives at the same time as viTerminate is called or a
timeout occurs, the data now gets read.  If this satisfies the read request,
then the read operation returns a successful status.

Enabling VI_EVENT_SERVICE_REQ for VI_HNDLR with a GPIB-ENET controller
would cause an application to hang if an SRQ event arrived while I/O was
in progress.  This has been fixed.

Multiple VISA processes using GPIB could not perform I/O simultaneously
even if the I/O was to different controller cards.  This has been fixed.
Multiple I/O requests to the same GPIB controller will still be handled
sequentially due to the nature of GPIB.

Calling viOpen with access mode 1 (exclusive lock) did not work as expected
on GPIB interface sessions.  This has been fixed.


NI-VISA 3.0 IMPROVEMENTS AND BUG FIXES
--------------------------------------
When you open a VISA session to GPIB0::INTFC and immediately query the
attribute VI_ATTR_GPIB_SRQ_STATE, it previously returned the value
VI_STATE_UNASSERTED.  It now returns VI_STATE_UNKNOWN because the SRQ line
cannot be monitored if the interface is not yet CIC.  The correct state is
returned after calling viGpibSendIFC.

For a TCPIP INSTR resource, calling viTerminate did not work correctly.

For a TCPIP INSTR resource, calling viRead or viWrite with a large buffer
occasionally would return an error without transmitting the entire buffer.

For a TCPIP INSTR resource, opening and closing sessions to multiple host
addresses would cause NI-VISA to crash.

For a TCPIP INSTR resource, calling viLock with a timeout longer than the
I/O timeout for that session could cause NI-VISA to return VI_ERROR_TMO.

For a TCPIP SOCKET resource, viReadAsync was not implemented.  Now it is.

For a Serial INSTR resource, viOpen will now open a port if the process
that had been using that port exited ungracefully (such as with Ctrl-C).
The previous behavior had been that viOpen would return VI_ERROR_RSRC_BUSY.

For a Serial INSTR resource, if you enabled RTS/CTS flow control and then
manually tried to set the state of the RTS line, some operating systems
would generate VI_ERROR_NSUP_ATTR while others would return VI_SUCCESS but
not actually modify the state of the line.  Neither of these behaviours was
correct.  Now NI-VISA will return VI_ERROR_INV_SETUP in this case on all
operating systems.

For a Serial INSTR resource, NI-VISA now supports the baud rates 50, 75,
134, 200, 1800, and 576000.  This list is now identical to the OS list.

For a Serial INSTR resource, if a binding existed to a valid file/device
but that file/device was not really a serial port, then NI-VISA would
not close the handle.  Repeated calls to viFindRsrc or viOpen would cause
the process to eventually run out of file handles.  This has been fixed.

If you had a GPIB device at primary address 1 that generated a response to
*IDN? that was longer than 100 characters, viFindRsrc would occasionally
crash.  This has been fixed.

viFindRsrc would crash on a complex expression with multiple "|" symbols.
This has been fixed.

viScanf %# modifier will now output the number of characters put into the
buffer excluding the terminating null character.  This applies to the %#s, 
%#t, %#T, and %#[] modifiers. NI-VISA treats the maximum array size 
(specified on input) as the total buffer size including the terminating null
character.

Once viScanf returned VI_ERROR_TMO, it would no longer work properly on
that session.

Calling viFindRsrc repeatedly when logged in as root would eventually run
out of file descriptors.  This has been fixed.

The attributes VI_ATTR_MANF_NAME and VI_ATTR_MODEL_NAME are now correctly
implemented for GPIB-VXI regardless of whether NI-VXI is installed.

The maximum simultaneous number of GPIB-VXI controllers supported by
NI-VISA has changed from 10 to 32.

When calling viWaitOnEvent repeatedly with a timeout of 0 (immediate), some
interfaces and event types would still consume excessive CPU time.  Now doing
this will yield the CPU regardless of the interface or event type.

viFindRsrc would return VI_ERROR_INV_EXPR if the expression exceeded 255
characters.  Now it will accept resource expressions of any length.

Calling viFindRsrc would always assert REN on each GPIB board after finding
devices.  Now viFindRsrc will re-assert REN on each board only if there is
at least 1 open VISA INSTR session on that board.

For a TCPIP SOCKET resource, using viRead with termchar enabled was much
slower than reading the exact number of bytes.  The performance of using
termchar with viRead is now much better than it was.


NI-VISA 3.1 IMPROVEMENTS AND BUG FIXES
--------------------------------------
Reading data from a Serial port was slow.  The performance has improved.

Using the Serial Break VI from LabVIEW did not work with remote NI-VISA.
This has been fixed.

For a Serial INSTR resource, if a parity or framing error occurred, NI-VISA
would not return the correct error condition.  This has been fixed.

The hostname "localhost" or the IP address "127.0.0.1" did not work properly
with remote NI-VISA.  Only the DHCP name or network IP address would work.
This has been fixed. 

Calling viEnableEvent on a remote VISA session would leak memory in the 
server.  This has been fixed.

The remote NI-VISA server did not properly validate a hostname with an 
underscore.  This has been fixed.

Calling viOpen on a remote NI-VISA resource did not honor the openTimeout
parameter.  This has been fixed.

Calling viClose on an ethernet resource would leak a socket handle.  This 
has been fixed.

The global attributes on a GPIB INTFC resource were not properly shared 
across sessions.  This has been fixed.

Using the system call "fork" on a process with VISA loaded would sometimes
cause the forked process to hang.  This has been fixed.


NI-VISA 3.2 IMPROVEMENTS AND BUG FIXES
--------------------------------------
Added complete support for USB (both RAW and INSTR).

Added a new UNINSTALL script for Linux.  This UNINSTALL script is installed
in the /usr/local/vxipnp/linux/NIvisa directory.

Calling viPrintf on a remote VISA session would return VI_ERROR_IO instead
of VI_ERROR_RSRC_LOCKED when the remote resource was locked.  This is fixed.

The VISA Interactive Control utility (NIvisaic) now displays an error message
box when viFindRsrc returns an error.


NI-VISA 3.3 IMPROVEMENTS AND BUG FIXES
--------------------------------------
The PXI/PCI INSTR Resource is now supported on Linux.  PXI specific
functionality is not supported at this time.  You must generate an INF file
for your PXI/PCI device with the VISA Driver Development Wizard on Windows
using NI-VISA 3.3 or higher.  INF files generated with older versions of
NI-VISA are not supported on Linux.  You can find installation instructions
in the generated file.  This is a new feature.

For USB RAW sessions, now interrupt-out endpoints may be used for viWrite.

For USB RAW sessions, viReadAsync is now implemented.

Opening a VISA session to a resource and then exiting the application
without closing either the VISA session or the Resource Manager
could crash the application.  This is fixed.

viFindRsrc could be very slow if the Serial Passport is enabled.
This is fixed.

viFindRsrc would crash when given long strings that use remote NI-VISA.
This is fixed.

Calling viMoveAsync on a remote VISA session would return an error.
This is fixed.

For Serial INSTR sessions, viClear previously would flush (discard) the I/O
buffers and then send a break.  According to the VISA spec, VISA must flush
(discard) the I/O output buffer, send a break, and then flush (discard) the
I/O input buffer.  This is fixed.

viScanf and variants did not support the "%u" identifier for arrays.
This is fixed.

viPrintf and variants did not support the "%zy" and "%Zy" identifiers for
floating point data.  This is fixed.

viPrintf and variants previously generated "NaN", "+Inf", and "-Inf".  It 
now generates "NAN", "INF", and "NINF", respectively.

viScanf and variants did not correctly interpret the input values "NAN",
"INF", and "NINF". This is fixed.

The function viUsbControlIn would report an incorrect return count when it
read fewer bytes than requested.  This is fixed.


NI-VISA 3.4 IMPROVEMENTS AND BUG FIXES
--------------------------------------
Added support for PXI system identification, trigger routing, and
reservation. PXI-1006 trigger routing is not supported.
Type "man nipxiconfig" for more information.

The VISA Driver Development Wizard is now supported on Linux. It is
located in /usr/local/vxipnp/linux/NIvisa. Previously, the wizard was
supported on Windows only.

For PXI INSTR sessions, non National Instruments PXI/PCI devices would not
be correctly detected and hence communication to them was not possible.
This is fixed.

Performing asynchronous I/O to a remote VISA resource would return an
incorrect transfer count and/or data. This is fixed.

For USB RAW sessions, communication with non-zero control endpoints is now
supported with the use of VI_ATTR_USB_CTRL_PIPE.

For USB RAW sessions, the default setting for VI_ATTR_USB_END_IN has been
changed from VI_USB_END_SHORT to VI_USB_END_SHORT_OR_COUNT.

For USB RAW sessions, viReadAsync would fail if the transfer size was over
8KB and not a multiple of the maximum packet size of the endpoint.
This is fixed.

For USB RAW sessions, changing the USB Alternate Setting on a USB
Interface number other than 0 would fail. This is fixed.

For TCP/IP SOCKET sessions, VI_ATTR_SUPPRESS_END_EN set to VI_TRUE did not
work in all cases. This is fixed. The default attribute setting for
VI_ATTR_SUPPRESS_END_EN on TCP/IP SOCKET sessions is now VI_TRUE.

For PXI INSTR sessions, when enabling for VI_EVENT_PXI_INTR using VI_QUEUE,
viDiscardEvents did not work properly. This is fixed.

For GPIB INSTR sessions, when using a GPIB-USB controller, if you unplug
and replug the GPIB-USB cable, you would have to exit and restart your
application to communicate with it again. This is fixed.


NI-VISA 3.5 IMPROVEMENTS AND BUG FIXES
--------------------------------------
For TCP/IP INSTR sessions, viWrite would fail if the transfer size was over
8KB. This is fixed.

Significantly optimized PCI interrupts and block moves.

Significantly optimized calls to viFindRsrc and viOpen for all interface
types.

In some cases, floating point formatted I/O would hang. This is fixed.

For Serial INSTR sessions, a call to viSetBuf, which did not modify the
buffer size, still returned VI_SUCCESS. viSetBuf now returns a warning
indicating the operation is not supported on the current platform.


NI-VISA 4.0 IMPROVEMENTS AND BUG FIXES
--------------------------------------
Added support for 64-bit data transfers in register-based operations, to
support the VISA 4.0 specification.

On some Linux desktop environments, visaconf and NIvisaic would not respond
to keyboard input while 'Num Lock' is on. This is fixed.

The I/O control in LabVIEW 8.0 would not always honor the parameters you
specified in the filtering dialog, depending on the order in which you made
changes. This is fixed.

viScanf would crash when reading indefinite-length block data. This is
fixed.

For ENET-Serial INSTR sessions, framing errors were occasionally not
detected. This is fixed.

Improved performance of NI Spy.

For TCP/IP SOCKET sessions, re-enabling the termination character between
read operations would incorrectly flush the internal buffer. This is fixed.

For TCP/IP SOCKET sessions, reading less than the entire amount of data that
the device sent would not work correctly for asynchronous read operations.
This is fixed.

The new PXI/PCI Express attributes are now implemented.


CREDITS
-------
This product includes components that use the socket++ library.  The library
is Copyright(C) 1992-1995 Greg Lavender (University of Texas) and
Gnanasekaran Swaminathan (University of Virginia).  All rights reserved.


KNOWN ISSUES
------------
If your application crashes or terminates abnormally, NI-VISA cannot always
clean up properly.  In order to run another VISA application, you should
first run the application <VXIPNPPATH>/linux/NIvisa/viclean.  This deletes
any remaining shared memory segments and system semaphores.

The GPIB INSTR and GPIB INTFC resources depend on the presence of an NI-488
driver installed on the system.  For PCI-GPIB products on Linux, NI-VISA
requires NI-488.2 for Linux version 2.3 or higher.  For GPIB ethernet
products on Linux, NI-VISA requires NI-488.2 for Linux version 2.2 or higher.

For USB resources, there are issues using interrupt-in endpoints. These
issues vary depending on the version of the Linux kernel and the USB
controller hardware that is on the system. On 2.4.x kernels, only one
packet may be requested at a time. Hence, if the value for the attribute 
VI_ATTR_USB_MAX_INTR_SIZE exceeds the maximum packet size of the current
interrupt-in endpoint, interrupts will not be received. Also, depending
on the chipset of the USB controller hardware, it is possible for a
kernel panic (system crash) to occur while attempting an interrupt-in
transfer. These issues have been fixed in 2.6.x kernels. However, in some
2.6.x kernels, interrupt-in transfers will not work on EHCI (USB 2.0)
controllers with USB 2.0 devices.

In SUSE Linux 10.1, when using USB TMC (Test & Measurement Class) 
Instruments, they may not be accessible by a user other than 'root' if the
class of the instrument is defined at the interface level. If this is the
case, access may be granted by using the "AddUsbRawPermissions.sh" script.
See the "USB ISSUES ON LINUX" section for details.
