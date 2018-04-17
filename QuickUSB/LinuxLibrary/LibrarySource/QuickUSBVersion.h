/*=============================================================================
 Title        : QuickUSBVersion.h
 Description  : QuickUSB Version / Driver Version Information
 Notes        : None
 History      :

 Copyright  2003 Bitwise Systems.  All rights reserved.
 This software contains confidential information and trade secrets of
 Bitwise Systems and is protected by United States and international
 copyright laws.  Use, disclosure, or reproduction is prohibited without
 the prior express written permission of Bitwise Systems, except as agreed
 in the QuickUSB Plug-In Module license agreement.

 Use, duplication or disclosure by the U.S. Government is subject to
 restrictions as provided in DFARS 227.7202-1(a) and 227.7202-3(a)
 (1998), and FAR 12.212, as applicable.  Bitwise Systems, 6489 Calle Real, Suite E,
 Goleta, CA  93117.

 Bitwise Systems
 6489 Calle Real, Suite E
 Goleta, CA  93117
 Voice: (805) 683-6469
 Fax  : (805) 683-4833
 Web  : www.bitwisesys.com
 email: support@bitwisesys.com
=============================================================================*/

//#ifndef	VERSION_INCLUDED
//#define	VERSION_INCLUDED

// These are definded in Visual C++, but not for gcc.
#if DRIVER==libusb
typedef unsigned char BYTE;
typedef unsigned char CHAR;
typedef unsigned char UCHAR;
typedef unsigned char * PUCHAR;
typedef unsigned short WORD;
typedef unsigned short USHORT;
typedef signed long LONG;
#endif


//
// Make sure to keep these in sync with the version
// information in ezusbsys.rc
//
#define EZUSB_DLL_MAJOR_VERSION		02
#define EZUSB_DLL_MINOR_VERSION		12
#define EZUSB_DLL_BUILD_VERSION		0000

//
//QusbDriverType designations
//
typedef enum QusbDriverType {
   OTHER_DRIVER = 0,
   QUSB_DRIVER,
   CYAPI_DRIVER,
   CYUSB_DRIVER
} QusbDriverType;



//Cypress Driver timeout structure
typedef struct _CYUSB_TIMEOUT
{
   BOOLEAN bTimeoutSet;
   ULONG BulkOutTimeout;
   ULONG BulkInTimeout;
} CYUSB_TIMEOUT, *PCYUSB_TIMEOUT;


typedef struct _QUSB_DEVICE_INFO {
   QCHAR devName[9];                 // The device name in the form QUSB-XXX
   HANDLE hDevHandle;                // The actual file handle returned by OpenFile
   QusbDriverType devType;            
   BOOLEAN deviceOpen;
   int iRefCount;
   unsigned short DefaultOffset;
} QUSB_DEVICE_INFO, *PQUSB_DEVICE_INFO;

//#endif	//	VERSION_INCLUDED
