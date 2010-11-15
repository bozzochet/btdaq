#ifndef __GSE__
#define __GSE__

#define CHRDEV      "gse"
#define GSE_BASE    0x378
#define GSE_IRQ     7

#define EVENTMAX      393
#define EVENTDEPTH    385
#define EVENTCAL      393 
#define CHANNELDEPTH  101
#define TEMPDEPTH     19

#define SPP_DATA      (0+GSE_BASE)
#define SPP_STATUS    (1+GSE_BASE)
#define SPP_CONTROL   (2+GSE_BASE)
#define EPP_ADDRESS   (3+GSE_BASE)
#define EPP_DATA      (4+GSE_BASE)
#define EPP_DATA16    (5+GSE_BASE)
#define EPP_DATA32    (7+GSE_BASE)
#define ECR_REG       (0x402+GSE_BASE)

#define CONTROL_nWRITE     0x1      /* Inverted at conector */  
#define CONTROL_nDSTRB     0x2      /* Inverted at conector */
#define CONTROL_nINIT      0x4      
#define CONTROL_nASTRB     0x8      /* Inverted at conector */
#define CONTROL_ENIRQ      0x10     /* 1=IRQ enable */  
#define CONTROL_DIR        0x20     /* 0=write to peripheral */
                                    /* 1=read from peripheral */

#define STATUS_TIMEOUT     0x1 
#define STATUS_S3          0x8
#define STATUS_S4          0x10
#define STATUS_S5          0x20
#define STATUS_IRQ         0x40
#define STATUS_nWAIT       0x80     /* Inverted at conector */ 


/* Define ioctl numbers according with new kernel convention, we will use 
   just the numbers free after the parport project:
       magic number = 'p'
       ordinal numbers = A0-FF
   See /usr/src/linux/Documentation/ioctl-number.txt */
#define GSE_IOCTL         'p'
#define GSE_NUMBER        0xC0

#define GSE_R_DATA        _IOR(GSE_IOCTL,GSE_NUMBER+0,unsigned char)
#define GSE_W_DATA        _IOW(GSE_IOCTL,GSE_NUMBER+1,unsigned char)

#define GSE_R_STATUS      _IOR(GSE_IOCTL,GSE_NUMBER+2,unsigned char)
#define GSE_W_STATUS      _IOW(GSE_IOCTL,GSE_NUMBER+3,unsigned char)

#define GSE_R_CONTROL     _IOR(GSE_IOCTL,GSE_NUMBER+4,unsigned char)
#define GSE_W_CONTROL     _IOW(GSE_IOCTL,GSE_NUMBER+5,unsigned char)
#define GSE_F_CONTROL     _IOW(GSE_IOCTL,GSE_NUMBER+6,unsigned char)

#define GSE_R_EPP         _IOR(GSE_IOCTL,GSE_NUMBER+7,unsigned char)
#define GSE_W_EPP         _IOW(GSE_IOCTL,GSE_NUMBER+8,unsigned char)

#define GSE_READADC       _IOR(GSE_IOCTL,GSE_NUMBER+10,unsigned char) 
#define GSE_READEVENT     _IOR(GSE_IOCTL,GSE_NUMBER+11,unsigned int *) 
#define GSE_READGAIN      _IOR(GSE_IOCTL,GSE_NUMBER+12,unsigned int *)
#define GSE_READCHANNEL   _IOR(GSE_IOCTL,GSE_NUMBER+13,unsigned int *) 
#define GSE_READTEMP      _IOR(GSE_IOCTL,GSE_NUMBER+14,unsigned short *) 

#define GSE_CLRIRQ        _IOR(GSE_IOCTL,GSE_NUMBER+20, int)
#define GSE_CTLONIRQ      _IOW(GSE_IOCTL,GSE_NUMBER+21, unsigned char)
#define GSE_READTIMEOUT   _IOR(GSE_IOCTL,GSE_NUMBER+22, int)


#endif







