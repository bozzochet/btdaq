/*=============================================================================

   Title        : QuickUSB.cpp
   Description  : QuickUSB Library
   Notes        : None
   History      :

   Copyright  2009 Bitwise Systems.  All rights reserved.
   This software contains confidential information and trade secrets of
   Bitwise Systems and is protected by United States and international
   copyright laws.  Use, disclosure, or reproduction is prohibited without
   the prior express written permission of Bitwise Systems, except as agreed
   in the QuickUSB Plug-In Module license agreement.

   Use, duplication or disclosure by the U.S. Government is subject to
   restrictions as provided in DFARS 227.7202-1(a) and 227.7202-3(a)
   (1998), and FAR 12.212, as applicable.  Bitwise Systems, 6489 Calle Real, 
   Suite E, Goleta, CA  93117.

   Bitwise Systems
   6489 Calle Real, Suite E
   Goleta, CA  93117
   Voice: (805) 683-6469
   Fax  : (805) 683-4833
   Web  : www.bitwisesys.com
   email: support@bitwisesys.com

   $History: QuickUsb.cpp $
=============================================================================*/

/*------------------------------------------------------------------------------
   Alon's Notes:
   libusb returns a value less than 0 as a failure but this file sends a 0 out 
   as a failure following QuickUSB documentation.
   bulkread and bulkwrite from libusb returns a value "sent" maybe this is how 
   much was sent out/read in  need to investigate.
   return results when programming bulkread bulkwrite.
   I2c does not return bytesRevieved in linux.... just success/failure.
------------------------------------------------------------------------------*/

#define DRIVER libusb
#warning Compiling for Linux with libusb

// Define dummy structures used in ezusbsys.h so we can compile

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include "usb.h"
#define BOOLEAN               int
#define FALSE                 0
#define TRUE                  1
#define PCHAR                 char
#define ULONG                 unsigned long
#define ANCHOR_LOAD_INTERNAL  0xA0
#define CPUCS_REG_FX2         0xE600
#define USB_BULK_READ         0x0006
#define USB_BULK_WRITE        0x0002

/* Linux usbdevfs has a limit of one page size per read/write. 4096 is */
/* the most portable maximum we can do for now */
#define MAX_WRITE_BLOCKSIZE   4096
#define MAX_READ_BLOCKSIZE    4096
int Timeout = 1000;		// timeout in milliseconds

#include "QuickUSB.h"
#include "QuickUSBVersion.h"
 
#define CYPRESS_VID           0x04B4
#define CY7C68013_PID         0x8613

#define BITWISE_VID           0x0FBB
#define QUSB2_PID             0x0001

#define FIRMWARE_LOAD		   0xA0
#define QUICKUSB_EEPROM		   0xA9
#define QUICKUSB_DEVICE_BASENAME    "QUSB"
#define QUICKUSB_DEVICE_STRING      "Bitwise Systems"
#define MAX_DEVICENUM         128
#define MAX_CNTL_XFER_SIZE    64
#define MAX_DEVICEPATH        128
#define MAX_STRINGDESCRIPTOR  128

// QuickUSB String Descriptor Indices
#define QUICKUSB_SERIAL_PERM  0x05
#define QUICKUSB_MAKE_PERM    0x06

// QuickUSB vendor specific requests
#define QUICKUSB_SETTING      0xB0
#define QUICKUSB_FPGA         0xB1
#define QUICKUSB_COMMAND      0xB2
#define QUICKUSB_PORT         0xB3
#define QUICKUSB_RS232        0xB4
#define QUICKUSB_I2C          0xB5
#define QUICKUSB_SPI          0xB6
#define QUICKUSB_READDATALEN  0xB7
#define QUICKUSB_TIMER  		0xB8
#define QUICKUSB_JTAG         0xB9
#define QUICKUSB_SCRIPT  		0xBF


// QuickUSB FPGA commands (wIndex values for QUICKUSB_FPGA)
#define FPGA_START            0
#define FPGA_WRITE            1
#define FPGA_ISCONFIG         3

// QuickUSB RS-232 Commands 
// (wIndex values for QUICKUSB_RS232, wAddr has the portNum)
#define PORT_GETSET_DIR       0
#define PORT_READWRITE        1

// QuickUSB RS-232 Commands (wIndex values for QUICKUSB_RS232)
#define RS232_SETGET_BAUD     0
#define RS232_SETGET_NUM      1
#define RS232_READWRITE       2

// QuickUSB I2C Commands (wIndex values for QUICKUSB_I2C)
#define I2C_READWRITE         0

// QuickUSB SPI Commands (wIndex values for QUICKUSB_SPI)
#define SPI_SETPINS           0
#define SPI_READWRITE         1
#define SPI_WRITEANDREAD      2

// QuickUSB JTAG Commands (wIndex values for QUICKUSB_FPGA)
#define JTAG_DIRECT           0
#define JTAG_CLKRAW           1
#define JTAG_CLKMULT          2
#define JTAG_RECIRC           3
#define JTAG_FLUSH            4


// QuickUSB Get Last Error Code -- Default to No Error
unsigned long lastError = QUICKUSB_ERROR_NO_ERROR;
unsigned long driverCode;


// An array to hold legacy or CyAPI driver indicator based on bus module number.
// QusbActiveDriver holds the driver type for which QuickUsbOpen was issued.
// OpenDeviceOffset holds the offset into the QUSB_DEVICE_INFO struct for the
// open device.
QUSB_DEVICE_INFO QusbDeviceInfo[MAX_DEVICENUM];

ULONG OpenDeviceOffset;

// Structure for QuickUSB async test for completion
typedef struct _QUSB_ASYNC {
   unsigned char transaction;
   unsigned char immediate;
} QUSB_ASYNC, *PQUSB_ASYNC;


// user defined types
struct quickusb_device;

struct quickusb_device {
   struct quickusb_device  *next, *previous;
   struct usb_device *dev; 
   char devname[9];
};

// Linux variables
struct quickusb_device *qusb_devListHead = NULL;
struct quickusb_device *qusb_devListTail = NULL;

typedef struct quickusb_device quickusb_device;

// Define SWAPBYTES macro, per our operating system and endian
#ifdef __i386__
#define SWAPBYTES(source, dest, length) swab(source, dest, length)
#else
#define SWAPBYTES(source, dest, length) memcpy(dest, source, length)
#endif



QRETURN QuickUsbInitialize(QULONG initValues) {
/*!-----------------------------------------------------------------------------
 Purpose : 
 Input   : 
 Output  :
 Notes   :
-----------------------------------------------------------------------------*/
   driverCode = 0;
   return TRUE;
}



QRETURN QuickUsbOpen(QHANDLE *hDevice, QCHAR *deviceName) {
/*-----------------------------------------------------------------------------
Purpose : Open a QuickUsb module
Input   : 
*deviceName - Points to a valid QiuckUsb device name
Output  :   - Offset into the QUSB_DEVICE_INFO structure
Notes   :
-----------------------------------------------------------------------------*/   
   int result;
   quickusb_device *curr;
   QHANDLE tmpdev;
  
   // search through the link list to find a QuickUsb module with the deviceName parameter given
   curr = qusb_devListHead;
   while (curr != NULL) {
      if (strcmp(curr->devname, deviceName) == 0) {
         // found a match now try to open
         tmpdev = usb_open(curr->dev);
         if (!tmpdev) {
            lastError = QUICKUSB_ERROR_CANNOT_OPEN_MODULE;
	    return FALSE;
         }

         // We must claim an interface before we can use it
         result = usb_claim_interface(tmpdev, 0);
         if (result < 0) {
           printf("Cannot claim interface for %s: Check device privileges\n", deviceName);
           lastError = QUICKUSB_ERROR_CANNOT_OPEN_MODULE;
           return FALSE;
         }

         // We were able to open the device and claim the interface
         *hDevice = tmpdev;
         return TRUE;
      }
      curr = curr->next;
   }
  
  // If no matches were found return FALSE
  return FALSE;
}



QRETURN QuickUsbClose(QHANDLE hDevice) {
/*-----------------------------------------------------------------------------
 Purpose : To close a handle to a QuickUsb module
 Input   : valid offset into QUSB_DEVICE_INFO structure
 Output  : none
 Notes   : none
-----------------------------------------------------------------------------*/
   usb_release_interface(hDevice, 0);
   usb_close(hDevice);
   // Libusb always returns 0 for close which is false in our case just return TRUE

   return TRUE;
}



QRETURN QuickUsbGetStringDescriptor(QHANDLE hDevice, QBYTE index, QCHAR *buffer, QWORD length) {
/*-----------------------------------------------------------------------------
 Purpose : 
 Input   : 
 Output  :
 Notes   :
-----------------------------------------------------------------------------*/
   BOOLEAN result;

   // Query the device for interface information
   result = usb_get_string_simple(hDevice, index, buffer, length );
   if (result < 0){
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
      return FALSE;
   }

   return TRUE;
}



QRETURN QuickUsbFindModules(QCHAR *nameList, QULONG length) {
/*-----------------------------------------------------------------------------
Purpose : Build a list of valid device names
Input   : 
*nameList - Points to a buffer in which to place the names.
length - The length of the buffer
Output  :
Notes   : devName size changed from 64 to 9 by Alon max Qusb-127\0 is only 9 chars long.
          An array holds the marker for choosing the legacy driver or CyAPI driver.for
          module access.
-----------------------------------------------------------------------------*/
   char devName[64];
   char *namePtr; 
   UCHAR index;
   struct usb_bus *bus;
   struct usb_device *dev;
   usb_dev_handle *udev;
   quickusb_device *qusb;
   quickusb_device *next;

   // find all busses and all devices 
   usb_init();
   usb_find_busses();
   usb_find_devices();
   index = 0;

   // Check the length of the buffer
   if (length < 1) {
      lastError = QUICKUSB_ERROR_CANNOT_FIND_DEVICE;
      return(FALSE);
   }

   // Delete all previous QuickUSB device entries, if any
   if (qusb_devListHead != NULL) {
      qusb = qusb_devListHead;

      while (qusb) {
         if (strncmp(qusb->devname, "QUSB", 4) != 0) {
            break;
         }

         next = qusb->next;             // next entry on the list
         free (qusb);                   // free current entry

         if (next == qusb) {
            break;
         }

         qusb = next;                   // check next entry
      }

      qusb_devListHead = qusb_devListTail = NULL;
   }

   namePtr = nameList;

   // Iterate through all devices to find Qusb modules
   for (bus = usb_busses; bus; bus = bus->next) {
      for (dev = bus->devices; dev; dev = dev->next) {
         if ((dev->descriptor.idVendor == BITWISE_VID) ||      // Programmed QuickUsb module
            (dev->descriptor.idVendor == CYPRESS_VID && dev->descriptor.idProduct == CY7C68013_PID)) { // unProgrammed QuickUsb module
            // Open the device and return false if it was a failure
            udev = usb_open(dev);
            if (!udev) {
               lastError = QUICKUSB_ERROR_CANNOT_OPEN_MODULE;
               return FALSE;
            }

            // Creating a new qusb device and attaching it to the tail
            qusb = (quickusb_device * ) malloc(sizeof(quickusb_device));
            if (qusb == NULL) {
               lastError = QUICKUSB_ERROR_OUT_OF_MEMORY;
               return FALSE;
            }

            // Get the string descriptor
#ifdef __APPLE__
            // Mac OS X 1.4 does not like the sprintf() call below that Linux accepts
			// This means only 9 QuickUSB devices will work on a Mac
			sprintf(devName, "%s", QUICKUSB_DEVICE_BASENAME);
			devName[4] = '-';
			devName[5] = index + '0';
			devName[6] = 0;
#else
            sprintf(devName, "%s-%d", QUICKUSB_DEVICE_BASENAME, index);
#endif
            index++;

            // Append the devName to the nameList
            if (strlen(devName) < (length - (namePtr-nameList))) {
               sprintf(namePtr, "%s", devName);  
               namePtr += strlen(devName) + 1;  
            }

            // Close the device
            usb_close(udev);
            // Start to assign the struct variables
            qusb->dev = dev;
            strcpy(qusb->devname, devName);

            // If this is the first device then attach the list pointer to this device
            if (qusb_devListHead  == NULL) {
               qusb->next = NULL;
               qusb->previous = NULL;
               qusb_devListHead = qusb;
               qusb_devListTail = qusb;
            }
            // Else just continue creating a link list of QuickUsb Modules. qusb_devListCurr is at previous node in the list
            else {
               qusb_devListTail->next = qusb;
               qusb->previous = qusb_devListTail;
               qusb_devListTail = qusb;
            }
         }
      }
   }

   // Add a terminating null
   *namePtr = '\0';

   // Return true if the list is not empty
   if (strlen(nameList) > 0) {
      return(TRUE);
   }
   else {
      lastError = QUICKUSB_ERROR_CANNOT_FIND_DEVICE;
      return(FALSE);
   }
}



QRETURN QuickUsbSetResetState(QHANDLE hDevice, unsigned char resetBit) {
/*-----------------------------------------------------------------------------
 Purpose : 
 Input   : 
 Output  :
 Notes   :
-----------------------------------------------------------------------------*/
   BOOLEAN result;
   char buffer;

   buffer = resetBit;
   result = usb_control_msg(hDevice,
			    USB_TYPE_VENDOR | USB_RECIP_DEVICE, // RequestType
			    ANCHOR_LOAD_INTERNAL,               // Request
			    CPUCS_REG_FX2,                      // Write to the CPUCS register
			    0,                                  // Index 
			    &buffer,
			    1,                                  // Size
			    Timeout
			    );
   if (result < 0) {
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
      return FALSE;
   }

   return TRUE;
}



QRETURN QuickUsbReadSetting(QHANDLE hDevice, unsigned short address, unsigned short *value) {
/*-----------------------------------------------------------------------------
 Purpose : Return the FPGA configuration state
 Input   : None
 Output  : Returns TRUE (1) if configured, FALSE if not.
 Notes   : Return value is the actual state of the CONF_DONE line.
-----------------------------------------------------------------------------*/
   unsigned int result;

   
   result = usb_control_msg(hDevice,
			    USB_ENDPOINT_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE,  // Direction
			    QUICKUSB_SETTING, // Vendor request configure FPGAs
			    0,                // Value
			    address,          // Index 
			    (char *) value, 
			    sizeof(unsigned short),
			    Timeout
			    );
   if (result < 0) {
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
      return FALSE;
   }

   return(TRUE);
}



QRETURN QuickUsbWriteSetting(QHANDLE hDevice, unsigned short address, unsigned short value) {
/*-----------------------------------------------------------------------------
 Purpose : Return the FPGA configuration state
 Input   : None
 Output  : Returns TRUE (1) if configured, FALSE if not.
 Notes   : Return value is the actual state of the CONF_DONE line.
-----------------------------------------------------------------------------*/
   unsigned int result;

   result = usb_control_msg(hDevice,
			    USB_ENDPOINT_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE,  // Sending data out
			    QUICKUSB_SETTING,                                       // Vendor request configure FPGAs 
			    0,                                                      // Value
			    address,                                                //  Index
			    (char *) &value,
			    sizeof(unsigned short),
			    Timeout
			    );
   if (result < 0) {
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
      return FALSE;
   }

   return(TRUE);
}



QRETURN QuickUsbStartFpgaConfiguration(QHANDLE hDevice) {
/*-----------------------------------------------------------------------------
 Purpose : 
 Input   : 
 Output  :
 Notes   :
-----------------------------------------------------------------------------*/
   unsigned int result;
   unsigned short status;

   result = usb_control_msg(hDevice,
			    USB_ENDPOINT_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE, // Requesttype
			    QUICKUSB_FPGA,                                        // request  
			    0,                                                    // Value
			    FPGA_START,                                           // index
			    (char *) &status,                                     // bytes
			    sizeof(status),                                       // size
			    Timeout
			    );

   if (result < 0) {
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
   }

   // Common WINDOWS and LINUX code below
   if (status != 0) {
      return(TRUE);
   }
   else {
      return(FALSE);
   }
}



QRETURN QuickUsbWriteFpgaData(QHANDLE hDevice, unsigned char *fpgadata, unsigned long datalength) {
/*-----------------------------------------------------------------------------
 Purpose : 
 Input   : 
 Output  :
 Notes   :
-----------------------------------------------------------------------------*/
   unsigned int result = 0;
   unsigned long bytecount;
   unsigned long bufferSize;

   // Check for datalength of 0
   if (datalength == 0) {
      lastError = QUICKUSB_ERROR_INVALID_PARAMETER;
      return FALSE;
   }

   //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   // Send configuration data to the system
   //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

   // Send the config file to the FPGA in blocks of MAX_XFER_SIZE
   bytecount = 0;
   while (bytecount < datalength) {
      // Send the block
     if (MAX_CNTL_XFER_SIZE < (datalength - bytecount)) {
        bufferSize = MAX_CNTL_XFER_SIZE;
     }
     else {
       bufferSize = datalength - bytecount;
     }

     result = usb_control_msg(hDevice,
			      USB_ENDPOINT_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE,    // RequestType
			      QUICKUSB_FPGA,                                            // Request  
			      0,                                                        // Value
			      FPGA_WRITE,                                               // Index
			      (char *)fpgadata + bytecount,                             // Data
			      (int) bufferSize, 
			      Timeout
			      );
     bytecount += bufferSize;
   }

   // return the transaction result
   if (result < 0) {
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
      return FALSE;
   }

   return TRUE;   
}



QRETURN QuickUsbIsFpgaConfigured(QHANDLE hDevice, unsigned short *isConfigured) {
/*-----------------------------------------------------------------------------
 Purpose : Return the FPGA configuration state
 Input   : None
 Output  : Returns TRUE (1) if configured, FALSE if not.
 Notes   : Return value is the actual state of the CONF_DONE line.
-----------------------------------------------------------------------------*/
   unsigned short status;
   unsigned int result;

   result = usb_control_msg(hDevice,
			    USB_ENDPOINT_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE,  // RequestType
			    QUICKUSB_FPGA,    // Request  
			    0,                // Value
			    FPGA_ISCONFIG,    // Index
			    (char *) &status,  // char *
			    sizeof(status),   // size
			    Timeout
			    );
  
   // Return the configuration status
   if (status == TRUE) {
      *isConfigured = TRUE;
   }
   else {
      *isConfigured = FALSE;
   }

   // return the transaction result
   if (result < 0) {
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
      return FALSE;
   }
   else {
      return (result);
   }

}





QRETURN QuickUsbWriteCommand(QHANDLE hDevice, unsigned short address, unsigned char *data, unsigned short length) {
/*-----------------------------------------------------------------------------
 Purpose : 
 Input   : 
 Output  :
 Notes   :
-----------------------------------------------------------------------------*/
   int result;
     
   result = usb_control_msg(hDevice,
			    USB_ENDPOINT_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE, // RequestType
			    QUICKUSB_COMMAND,                                      // Request  
			    (int) length,                                          // Value
			    (int) address,                                         // Index
			    (char *)data,                                          // char *
			    (int) length,                                          // size
			    Timeout
			    );
   if (result  < 0) {
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
      return FALSE;
   }
   else {
      return TRUE;   
   }
}



QRETURN QuickUsbReadCommand(QHANDLE hDevice, unsigned short address, unsigned char *data, unsigned short *length) {
/*-----------------------------------------------------------------------------
 Purpose : 
 Input   : 
 Output  :
 Notes   :
-----------------------------------------------------------------------------*/
   int result;

   result = usb_control_msg(hDevice,
			    USB_ENDPOINT_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE, // RequestType
			    QUICKUSB_COMMAND,                                     // Request  
			    (int) *length,                                        // Value
			    (int) address,                                        // Index
			    (char *)data,                                         // char *
			    (int) *length,                                        // size
			    Timeout
			    );
   if (result < 0) {
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
      return FALSE;
   }
   else {
      return TRUE;
   }

}



QRETURN QuickUsbWriteData(QHANDLE hDevice, unsigned char *data, unsigned long length) {
/*-----------------------------------------------------------------------------
 Purpose : To transmit data synchronously to a USB Module Bulk Endpoint
 Input   : Device handle offset, data buffer and buffer length
 Output  : Error Codes
 Notes   : Transfers of more than 262144 (256K) Bytes handled in driver if
           later than version 1.0.0.
-----------------------------------------------------------------------------*/
   int result = TRUE;
   unsigned long bytesWritten;
   unsigned long blockSize;
   unsigned char *block;

   // Set up the initial values
   result = TRUE;
   block = data;
   bytesWritten = 0;

   // Send the data one block at a time until sent 
   while (result > 0) {
      // Calculate the current block size
      if ((length - (block - data)) < MAX_WRITE_BLOCKSIZE){
         blockSize = length - (block - data);
      }
      else {
         blockSize = MAX_WRITE_BLOCKSIZE;
      }	    
      if (blockSize == 0) {
         break;
      }

      // Send the block
      result = usb_bulk_write(hDevice, USB_BULK_WRITE, (char *)block, (int) blockSize, Timeout); 

      // Increment the block pointer
      if (result > 0) {
         block += blockSize;
      }
   }

   if (result < 0) {
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
      return FALSE;
   }
   else {
      return TRUE;
   }
}



QRETURN QuickUsbReadData(QHANDLE hDevice, unsigned char *data, unsigned long *length) {
/*-----------------------------------------------------------------------------
 Purpose : To acquire data synchronously from a USB Module Bulk Endpoint
 Input   : Device handle offset, data buffer and buffer length
 Output  : Error Codes
 Notes   : We first need to tell the module how much to read, Transfers of more
           than 262144 (256K) Bytes handled in driver if later than version 1.0
-----------------------------------------------------------------------------*/
   int result;
   unsigned long bytesWritten;
   unsigned long blockSize;
   unsigned char *block;

   // Set up the initial values
   result = TRUE;
   block = data;
   bytesWritten = 0;
   
   // Read the data one block at a time until sent
   while (result > 0) {
     // Calculate the current block size
     if ((*length - (block - data)) <  MAX_READ_BLOCKSIZE) {
        blockSize = *length - (block - data);
     }
     else {
        blockSize =  MAX_READ_BLOCKSIZE;   
     }
     if (blockSize == 0) {
       break;
     }
     
     // Send the read data block size
     result = usb_control_msg(hDevice,
			      USB_ENDPOINT_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE, // RequestType
			      QUICKUSB_READDATALEN,                                  // Request  
			      0,                                                     // Value
			      0,                                                     // Index
			      (char *) &blockSize,                                   // char *
			      sizeof(unsigned long),                                 // size
			      Timeout
			      );
     if (result < 0) {
       break;
     }

     // Receive the data block
     result =  usb_bulk_read(hDevice, (int)USB_BULK_READ, (char *)block, (int) blockSize, Timeout);
     
     // Increment the block pointer
     if (result > 0) {
       block += blockSize;
     }
   } 
   
   // send error message
   if (result < 0) {
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
     return FALSE;
   }
   else {
     *length = (unsigned long) (block - data);
     return TRUE;
   }
}




QRETURN QuickUsbReadPortDir(QHANDLE hDevice, unsigned short address, unsigned char *data) {
/*-----------------------------------------------------------------------------
 Purpose : 
 Input   : 
 Output  :
 Notes   :
-----------------------------------------------------------------------------*/
   int result;

   result = usb_control_msg(hDevice,
			    USB_ENDPOINT_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE, // RequestType
			    QUICKUSB_PORT,                                        // Request  
			    (int) address,                                        // Value
			    PORT_GETSET_DIR,                                      // Index
			    (char *)data,                                         // char *
			    1,                                                    // size
			    Timeout
			    );
   if (result < 0) {
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
     return FALSE;
   }
   else {
      return TRUE;
   }
}



QRETURN QuickUsbWritePortDir(QHANDLE hDevice, unsigned short address, unsigned char data) {
/*-----------------------------------------------------------------------------
 Purpose : 
 Input   : 
 Output  :
 Notes   :
-----------------------------------------------------------------------------*/
   int result;

   char linux_data = data;
   result = usb_control_msg(hDevice,
			    USB_ENDPOINT_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE,// RequestType
			    QUICKUSB_PORT,                                        // Request  
			    (int)address,                                         // Value
			    PORT_GETSET_DIR,                                      // Index
			    &linux_data,                                          // char *
			    1,                                                    // size
			    Timeout
			    );
   if (result < 0) {
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
      return FALSE;
   }
   else {
      return TRUE;
   }
}



QRETURN QuickUsbReadPort(QHANDLE hDevice, unsigned short address, unsigned char *data, unsigned short *length) {
/*-----------------------------------------------------------------------------
 Purpose : 
 Input   : 
 Output  :
 Notes   :
-----------------------------------------------------------------------------*/
   int result;

   result = usb_control_msg(hDevice,
			    USB_ENDPOINT_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE, // RequestType
			    QUICKUSB_PORT,                                        // Request  
			    (int) address,                                        // Value
			    PORT_READWRITE,                                       // Index
			    (char *)data,                                         // char *
			    (int) *length,                                        // size
			    Timeout
			    );
   if (result < 0) {
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
     return FALSE;
   }
   else {
      return TRUE;
   }
}



QRETURN QuickUsbWritePort(QHANDLE hDevice, unsigned short address, unsigned char *data, unsigned short length) {
/*-----------------------------------------------------------------------------
 Purpose : 
 Input   : 
 Output  :
 Notes   :
-----------------------------------------------------------------------------*/
   int result;

   result = usb_control_msg(hDevice,
			    USB_ENDPOINT_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE, // RequestType
			    QUICKUSB_PORT,                                        // Request  
			    (int) address,                                        // Value
			    PORT_READWRITE,                                       // Index
			    (char *)data,                                         // char *
			    (int) length,                                         // size
			    Timeout
			    );
   if (result < 0) {
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
      return FALSE;
   }
   else {
      return TRUE;
   }
}



QRETURN QuickUsbSetRS232BaudRate(QHANDLE hDevice, unsigned long baudRate) {
/*-----------------------------------------------------------------------------
 Purpose : 
 Input   : 
 Output  :
 Notes   :
-----------------------------------------------------------------------------*/
   int result;

   result = usb_control_msg(hDevice,
			    USB_ENDPOINT_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE, // RequestType
			    QUICKUSB_RS232,                                       // Request  
			    0,                                                    // Value
			    RS232_SETGET_BAUD,                                    // Index
			    (char *) &baudRate,
			    sizeof(unsigned long),                                           
			    Timeout
			    );
   if (result < 0) {
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
      return FALSE;
   }
   else {
      return TRUE;
   }
}



QRETURN QuickUsbGetNumRS232(QHANDLE hDevice, unsigned char portNum, unsigned short *length) {
/*-----------------------------------------------------------------------------
 Purpose : 
 Input   : 
 Output  :
 Notes   :
-----------------------------------------------------------------------------*/
   int result;
   unsigned short numChars;

   result = usb_control_msg(hDevice,
			    USB_ENDPOINT_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE, // RequestType
			    QUICKUSB_RS232,                                       // Request  
			    portNum,                                              // Value
			    RS232_SETGET_NUM,                                     // Index
			    (char *) &numChars,
			    sizeof(unsigned short),                                           
			    Timeout
			    );
   if (result < 0) {
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
      return FALSE;
   }
   else {
      *length = numChars;
      return TRUE;
   }
}



QRETURN QuickUsbFlushRS232(QHANDLE hDevice, unsigned char portNum) {
/*-----------------------------------------------------------------------------
 Purpose : 
 Input   : 
 Output  :
 Notes   :
-----------------------------------------------------------------------------*/
   int result;
   unsigned short numChars;

   result = usb_control_msg(hDevice,
			    USB_ENDPOINT_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE, // RequestType
			    QUICKUSB_RS232,                                       // Request  
			    portNum,                                              // Value
			    RS232_SETGET_NUM,                                     // Index
			    (char *) &numChars,
			    sizeof(unsigned short),                                           
			    Timeout
			    );

   // Check the transaction result
   if (result < 0) {
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
      return FALSE;
   }
   else {
       return TRUE;
   }
}



QRETURN QuickUsbReadRS232(QHANDLE hDevice, unsigned char portNum, unsigned char *data, unsigned short *length) {
/*-----------------------------------------------------------------------------
 Purpose : 
 Input   : 
 Output  :
 Notes   :
-----------------------------------------------------------------------------*/
   int result;
   long blockSize;
   unsigned long bytesReceived;
   unsigned char *block;

   // Set up the initial values
   result = TRUE;
   block = data;
   bytesReceived = 0;
   
   // Read the data one block at a time until sent
   while (result != FALSE) {
      // Calculate the current block size
      if ((*length - (block - data)) <  64) {
         blockSize = *length - (block - data);
      }
      else {
         blockSize = 64;
      }

      if (blockSize == 0) {
         break;
      }

     // Read the block
     result = usb_control_msg(hDevice,
			      USB_ENDPOINT_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE, // RequestType
			      QUICKUSB_RS232,                                       // Request  
			      portNum,                                              // Value
			      RS232_READWRITE,                                      // Index
			      (char *)block,
			      (int)blockSize,
			      Timeout
			      );

      // Increment the block pointer if the transaction was a success
      if (result > 0) {
         block += blockSize;
      }
   }
   
   if (result < 0) {
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
      return FALSE;
   }
   else {
      *length = (unsigned short) bytesReceived;
      return TRUE;
   }
}



QRETURN QuickUsbWriteRS232(QHANDLE hDevice, unsigned char portNum, unsigned  char *data, unsigned short length) {
/*-----------------------------------------------------------------------------
 Purpose : 
 Input   : 
 Output  :
 Notes   :
-----------------------------------------------------------------------------*/
   int result;
   long blockSize;
   unsigned long bytesWritten;
   unsigned char *block; 

   // Set up the initial values
   result = TRUE;
   block = data;
   bytesWritten = 0;
   
   while (result > 0) {
      // Calculate the current block size
      if (length - (block - data) <  64) {   // Control endpoint len
         blockSize = length - (block - data);
      }
      else {
        blockSize = 64;
      }
     
      if (blockSize == 0) {
         break;
      }
     
     // Send the block
     result = usb_control_msg(hDevice,
			      USB_ENDPOINT_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE, // RequestType
			      QUICKUSB_RS232,                                       // Request  
			      portNum,                                              // Value
			      RS232_READWRITE,                                      // Index
			      (char *)block,
			      (int) blockSize,                                           
			      Timeout
			      );
     // Increment the block pointer if the transaction was a success
     if (result > 0) {
        block += blockSize;
     }
   }

   // Check transaction result
   if (result < 0) {
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
      return FALSE;
   }
   else {
      return TRUE;
   }
}



QRETURN QuickUsbReadSpi(QHANDLE hDevice, unsigned char portNum, unsigned char *data, unsigned short *length) {
/*-----------------------------------------------------------------------------
 Purpose : 
 Input   : 
 Output  :
 Notes   :
-----------------------------------------------------------------------------*/
   int result;
   // Check for valid length
   if ((*length < 1) || (*length > 64)) {
      lastError = QUICKUSB_ERROR_INVALID_PARAMETER;
      return FALSE;
   }

   result = usb_control_msg(hDevice,
			    USB_ENDPOINT_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE,  // RequestType
			    QUICKUSB_SPI,                                          // Request  
			    (int) portNum,                                         // Value
			    SPI_READWRITE,                                         // Index
			    (char *)data,
			    (int) *length,
			    Timeout
			    );

   // Check the transactino result
   if (result < 0) {
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
      return FALSE;
   }
   else {
      return TRUE;
   }
}



QRETURN QuickUsbWriteSpi(QHANDLE hDevice, unsigned char portNum, unsigned char *data, unsigned short length) {
/*-----------------------------------------------------------------------------
 Purpose : 
 Input   : 
 Output  :
 Notes   :
-----------------------------------------------------------------------------*/
   int result;
   // Check for valid length
   if ((length < 1) || (length > 64)) {
      lastError = QUICKUSB_ERROR_INVALID_PARAMETER;
      return FALSE;
   }

   result = usb_control_msg(hDevice,
			    USB_ENDPOINT_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE, // RequestType
			    QUICKUSB_SPI,                                          // Request  
			    (int) portNum,                                         // Value
			    SPI_READWRITE,                                         // Index
			    (char *)data,
			    (int) length,
			    Timeout
			   );

   // Check the transaction result
   if (result < 0) {
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
      return FALSE;
   }
   else {
      return TRUE;
   }
}



QRETURN QuickUsbWriteReadSpi(QHANDLE hDevice, unsigned char portNum, unsigned char *data, unsigned short length) {
/*-----------------------------------------------------------------------------
 Purpose : 
 Input   : 
 Output  :
 Notes   :
-----------------------------------------------------------------------------*/
   int result;
   // Send the block of data
   int i;

      // Check for valid length
   if ((length < 1) || (length > 64)) {
      lastError = QUICKUSB_ERROR_INVALID_PARAMETER;
      return FALSE;
   }

   result = usb_control_msg(hDevice,
			    USB_ENDPOINT_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE, // RequestType
			    QUICKUSB_SPI,                                          // Request  
			    (int) portNum,                                         // Value
			    SPI_WRITEANDREAD,                                      // Index
			    (char *)data,
			    (int) length,
			    Timeout
			    );
   if (result < 0) {
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
      return FALSE;
   }

   for ( i = 0; i < length; i++ ) {
     data[i] = 1;
   }
   
   // Get the block of data 
   result = usb_control_msg(hDevice,
			    USB_ENDPOINT_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE, // RequestType
			    QUICKUSB_SPI,                                          // Request  
			    (int) portNum,                                         // Value
			    SPI_WRITEANDREAD,                                      // Index
			    (char *)data,
			    (int) length,
			    Timeout
			    );

   // Check transaction result
   if (result < 0) {
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
      return FALSE;
   }
   else {
      return TRUE;
   }
}



// This function is called when an error is detected from an I2C read or write.
// It gets the I2C error status from the firmware, and translates it to a 
// QuickUSB error code.
// NOTE this feature requires firmware 2.11 RC 7 or later to work correctly.
static void GetI2CError(QHANDLE hDevice) {
   unsigned short I2CStatus;
   unsigned long status;

   // The I2C error status is in the high order byte of setting 8, starting
   // with firmware 2.11 RC 7.
   status = QuickUsbReadSetting(hDevice, SETTING_I2CTL, &I2CStatus);

   if (status == TRUE) {
      // These values must match the values in FX2.H from the firmware.
      I2CStatus = I2CStatus >> 8;
      switch (I2CStatus) {
      case 6:                          // I2C_BERROR
         lastError = QUICKUSB_ERROR_I2C_BUS_ERROR;
         return;
      case 7:                          // I2C_NACK
         lastError = QUICKUSB_ERROR_I2C_NO_ACK;
         return;
      case 10:                         // I2C_SLAVE_WAIT
         lastError = QUICKUSB_ERROR_I2C_SLAVE_WAIT;
         return;
      case 11:                         // I2C_TIMEOUT
         lastError = QUICKUSB_ERROR_I2C_TIMEOUT;
         return;
      }
   }
}



QRETURN QuickUsbReadI2C(QHANDLE hDevice, unsigned short address, unsigned char *data, unsigned short *length) {
/*-----------------------------------------------------------------------------
 Purpose : 
 Input   : 
 Output  :
 Notes   :
-----------------------------------------------------------------------------*/
   int result;
   // Check for valid length
   if ((*length < 1) || (*length > 64)) {
      lastError = QUICKUSB_ERROR_INVALID_PARAMETER;
      return FALSE;
   }

   result = usb_control_msg(hDevice,
			    USB_ENDPOINT_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE,  // RequestType
			    QUICKUSB_I2C,                                          // Request  
			    address,                                               // Value
			    I2C_READWRITE,                                         // Index
			    (char *)data,
			    (int) *length,
			    Timeout
			    );

   // Check transaction result
   if (result < 0) {
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
      return FALSE;
   }
   else {
      return TRUE;
   }

   if (result == FALSE) {
      GetI2CError(hDevice);
   }

   return result;
}



QRETURN QuickUsbWriteI2C(QHANDLE hDevice, unsigned short address, unsigned char *data, unsigned short length) {
/*-----------------------------------------------------------------------------
 Purpose : 
 Input   : 
 Output  :
 Notes   :
-----------------------------------------------------------------------------*/
   int result;

   // Check for valid length
   if ((length < 1) || (length > 64)) {
      lastError = QUICKUSB_ERROR_INVALID_PARAMETER;
      return FALSE;
   }

   result = usb_control_msg(hDevice,
			    USB_ENDPOINT_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE, // RequestType
			    QUICKUSB_I2C,                                          // Request  
			    address,                                               // Value
			    I2C_READWRITE,                                         // Index
			    (char *)data,
			    (int) length,
			    Timeout
			    );
   if (result < 0) {
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
      result = FALSE;
   }
   else {
      result = TRUE;
   }

   if (result == FALSE) {
      GetI2CError(hDevice);
   }

   return result;
}


QRETURN QuickUsbSetTimeout(QHANDLE hDevice, unsigned long timeOut) {
/*-----------------------------------------------------------------------------
 Purpose : To set a timeout for synchronous bulk read/write calls to the
           Driver. The timeout is to the nearest millisecond
 Input   : 
 Output  :
 Notes   : Driver version 2.0
-----------------------------------------------------------------------------*/
   Timeout = timeOut;
   return (TRUE);
}



QRETURN QuickUsbGetDriverVersion(USHORT *MajorDriverVersion, 
                                    USHORT *MinorDriverVersion,
                                    USHORT *BuildDriverVersion) {
/*-----------------------------------------------------------------------------
 Purpose : 
 Input   : 
 Output  :
 Notes   : 
-----------------------------------------------------------------------------*/
   return QuickUsbGetDllVersion(MajorDriverVersion, MinorDriverVersion, BuildDriverVersion);
}



QRETURN QuickUsbGetDllVersion(USHORT *MajorDllVersion, 
                                 USHORT *MinorDllVersion,
                                 USHORT *BuildDllVersion) {
/*-----------------------------------------------------------------------------
 Purpose : 
 Input   : 
 Output  :
 Notes   : 
-----------------------------------------------------------------------------*/
   *MajorDllVersion = EZUSB_DLL_MAJOR_VERSION;
   *MinorDllVersion = EZUSB_DLL_MINOR_VERSION;
   *BuildDllVersion = EZUSB_DLL_BUILD_VERSION;
   return(1);
}


QRETURN QuickUsbGetFirmwareVersion(QHANDLE hDevice, USHORT *MajorVersion, 
                                 USHORT *MinorVersion,
                                 USHORT *BuildVersion) {
/*-----------------------------------------------------------------------------
 Purpose : 
 Input   : 
 Output  :
 Notes   :
-----------------------------------------------------------------------------*/
   BOOLEAN result;
   unsigned short buildVer = 0;

   struct usb_device_descriptor UsbDeviceDescriptor;

   // Query the device for interface information
   result = usb_get_descriptor(hDevice, USB_DT_DEVICE, 0, &UsbDeviceDescriptor, sizeof(UsbDeviceDescriptor));
   if (result < 0){
      lastError = QUICKUSB_ERROR_IOCTL_FAILED;
      return FALSE;
   }
   else {
      // Get the build version.  This requires firmware 2.11rc9 or later.
      QuickUsbReadSetting(hDevice, SETTING_VERSIONBUILD, &buildVer);

      *MajorVersion = (((UsbDeviceDescriptor.bcdDevice & 0xF000) >> 12) * 10) +
                       ((UsbDeviceDescriptor.bcdDevice & 0x0F00) >> 8);
      *MinorVersion = (((UsbDeviceDescriptor.bcdDevice & 0x00F0) >> 4) * 10) +
                        (UsbDeviceDescriptor.bcdDevice & 0x000F);
      *BuildVersion = buildVer;
      return TRUE;
   }
}


QRETURN QuickUsbGetLastError(unsigned long *error) {
/*-----------------------------------------------------------------------------
 Purpose : To obtain USB and Driver errors as well as Driver State information.
 Input   : 
 Output  :
 Notes   :
-----------------------------------------------------------------------------*/
   *error = lastError;
   return TRUE;
}



QRETURN QuickUsbReadDataAsync(QHANDLE hDevice, unsigned char *data, unsigned long *length, unsigned char *transaction) {
/*-----------------------------------------------------------------------------
 Purpose : To send a group of partial IRPs describing 256K transfers to the
           USBD stack and return control to the application software.
 Input   : device handle offset, pointers to data buffer and its length
 Output  : error codes
 Notes   : 
-----------------------------------------------------------------------------*/
   // Async I/O is not supported in libusb version 0.1.11
   lastError = QUICKUSB_ERROR_FUNCTION_NOT_SUPPORTED;
   return (FALSE);
}



QRETURN QuickUsbWriteDataAsync(QHANDLE hDevice, unsigned char *data, unsigned long length, unsigned char *transaction) {
/*-----------------------------------------------------------------------------
 Purpose : To send a group of partial IRPs describing 256K transfers to the
           USBD stack and return control to the application software.
 Input   : device handle offset, pointers to data buffer and its length
 Output  : error codes
 Notes   :
-----------------------------------------------------------------------------*/
   // Async I/O is not supported in libusb version 0.1.11
   lastError = QUICKUSB_ERROR_FUNCTION_NOT_SUPPORTED;
   return (FALSE);
}



QRETURN QuickUsbAsyncWait(QHANDLE hDevice, unsigned long *bytecount, unsigned char transaction, unsigned char immediate) {
/*-----------------------------------------------------------------------------
 Purpose : To test completion of an async read or write function
 Input   : none
 Output  : bytes transferred
 Notes   : If a timeout was sensed, the timeout error code is set.
           If the function succeeds, the number of bytes transferred is set.
-----------------------------------------------------------------------------*/
   // Async I/O is not supported in libusb version 0.1.11
   lastError = QUICKUSB_ERROR_FUNCTION_NOT_SUPPORTED;
   return (FALSE);
} //End QuickUsbAsyncWait





/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Section header
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*-----------------------------------------------------------------------------
 Purpose : 
 Input   : 
 Output  :
 Notes   :
-----------------------------------------------------------------------------*/

