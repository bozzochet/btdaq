/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2004 by PADSOFT, Inc.                        */
/*                                                                           */
/*    Developed for use on AMS-2, An International Space Station payload     */
/*                      searching for missing matter                         */
/*                                                                           */
/*                         No rights reserved                                */
/*                                                                           */
/* Developed by:        Peter Armistead Dennett                              */
/*                            PADSOFT, Inc                                   */
/*                          61 Harbor Lane                                   */
/*                        Kemah, Texas 77565                                 */
/*                          (281) 334 3800                                   */
/*                      http://www.padsoft.com                               */
/*                                                                           */
/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/

//  abi.h   Defines for the AMS Block Interface

#ifndef _ABI_H
#define _ABI_H

//  Revision History
//    Jul 2004 by Peter Dennett
//      - Initial version in Lake Howard Hotel, Taiwan
//        based on MDCSoftware coding
//    Apr 2007 by Peter Dennett - CERN
//      - Moved to aal-tcl env
//    Dec 2007 by Peter Dennett
//      - Updated for MinGW for WIN32 operation
//      - Removed dead code
//      - Added support for AMSBlock secondary header
//    Mar 2008 by Peter Dennett at CERN
//      - Updated AMSBlock secondary header names to match AAL's


//-Code: typedef AMSBlock
//-  Defines an AMSBlock
#define BT_REQUEST       0x00
#define BT_REPLY         0x02
#define BT_WRITEREAD     0x00
#define BT_WRITE         0x01   // Better read as write but inhibit read

//  Special bit to trace an AMSBlock in the system
//  Turn off this feature by defining as zero

#define BT_TRACE         0x80

// Error Codes - top four bits of CommandTag if BT_REPLY

#define BLOCK_S_END         0   // End on success
#define BLOCK_S_ABORT       1   // Abort from slave or wrong command
#define BLOCK_S_ERROR       2   // Error reply from slave or CRC error
#define BLOCK_S_ENOSPC      3   // No space
#define BLOCK_S_EEXIST      4   // Device or file not exist
#define BLOCK_S_EBADNODE    5   // Bad node address (timeout after certain tries)
#define BLOCK_S_EIO         8   // Hardware error
#define BLOCK_S_ETIMEOUT    13  // Slave timeout

// Request option bits - top four bits of CommandTag if BT_REQUEST

#define BLOCK_O_NO_HK      (1<<0) // No reply placed in HK data
#define BLOCK_O_IGNORE_ERR (1<<1) // Ignore errors
#define BLOCK_0_FBI_1      (1<<2)
#define BLOCK_0_FBI_2      (1<<3)

#define abiStatusFreeData   1   // Data should be abiFree'd on Destruction

typedef struct _AMSBlock
{
  int32 Status;                 // Block status per abiStatus* above
  int16 Source;                 // Used by AAL

                                // ---PRIMARY HEADER---
  int32 DataType;               // DataType for the block
  int32 DataCount;              // Count of 8 bit bytes
  int16 NodeAddress;            // The node address for the block
  int16 BlockType;              // BlockType BT_ bits above

                                // ---SECONDARY HEADER---
  int16 Error;                  // AKA Status - AMSBlock error status per BLOCK_ above
  int16 CommandTag;             // Command Tag
  int32 DataTime;               // Time
  int32 FineTime;               // Used only by DataType 0x1F0383 which has a fine time

                                // ---BLOCK CONTENTS---
  int32 BufferSize;             // Number of bytes allocated for Data.p8
  void *Contents;               // Pointer to our data area.  Nominally this
                                // Points into the same malloc buffer to
                                // DataStart (next in this structure).  This allows
                                // us to release this memory with a single
                                // call.  In practice this could point to any
                                // memory (see abiStatusFreeData).

  struct {
    int8  *p8;                  // Pointer to data area
    int16 *p16;                 // Pointer to data area
  } Data;

  int16 DataStart[1];           // First 16 bit word of the block (additional is
                                // allocated to hold the entire array).
} AMSBlock, *pAMSBlock;
//-Code_End:

////////////////////////////////////////////////////////////////////////////
//-Extern: abiBlockIsRequest
static __inline int abiBlockIsRequest(pAMSBlock b)
//-Synopsis:    Test to see if an AMSBlock is a request
//-Returns:     The logical result of the test
//-ARG_I:       b              Pointer to the AMSBlock
//-Description: Test to see if an AMSBlock is a request.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  return !(b->BlockType & BT_REPLY);
}

////////////////////////////////////////////////////////////////////////////
//-Extern: abiMakeRequest
static __inline int abiMakeRequest(pAMSBlock b)
//-Synopsis:    Mark an AMSBlock as a request
//-Returns:     BlockType
//-ARG_I:       b              Pointer to the AMSBlock
//-Description: Mark an AMSBlock as a request
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions: {return (b->BlockType |= BT_WRITE);}
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  return (b->BlockType &= ~BT_REPLY);
}

////////////////////////////////////////////////////////////////////////////
//-Extern: abiIsReply
static __inline int abiIsReply(pAMSBlock b)
//-Synopsis:    Test to see if an AMSBlock is a reply
//-Returns:     The logical result of the test
//-ARG_I:       b              Pointer to the AMSBlock
//-Description: Test to see if an AMSBlock is a reply.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  return (b->BlockType & BT_REPLY);
}

////////////////////////////////////////////////////////////////////////////
//-Extern: abiMakeReply
static __inline int abiMakeReply(pAMSBlock b)
//-Synopsis:    Mark an AMSBlock as a reply
//-Returns:     BlockType
//-ARG_I:       b              Pointer to the AMSBlock
//-Description: Mark an AMSBlock as a reply
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  return (b->BlockType |= BT_REPLY);
}

////////////////////////////////////////////////////////////////////////////
//-Extern: abiReplyError
static __inline pAMSBlock abiReplyError(pAMSBlock b, int16 Error)
//-Synopsis:    Mark an AMSBlock as a request
//-Returns:     BlockType
//-ARG_I:       b              Pointer to the AMSBlock
//-Description: Mark an AMSBlock as a request
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  b->Error = Error;
  b->DataCount = 0;
  abiMakeReply(b);
  return (b);
}

////////////////////////////////////////////////////////////////////////////
//-Extern: abiIsWriteRead
static __inline int abiIsWriteRead(pAMSBlock b)
//-Synopsis:    Test to see if an AMSBlock is a write then read
//-Returns:     The logical result of the test
//-ARG_I:       b              Pointer to the AMSBlock
//-Description: Test to see if an AMSBlock is a put.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  return  !(b->BlockType & BT_WRITE);
}

////////////////////////////////////////////////////////////////////////////
//-Extern: abiMakeWriteRead
static __inline int abiMakeWriteRead(pAMSBlock b)
//-Synopsis:    Make an AMSBlock a write then read
//-Returns:     The logical result of the test
//-ARG_I:       b              Pointer to the AMSBlock
//-Description: Make an AMSBlock a write then read
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  return (b->BlockType &= ~BT_WRITE);
}

////////////////////////////////////////////////////////////////////////////
//-Extern: abiIsWrite
static __inline int abiIsWrite(pAMSBlock b)
//-Synopsis:    Test to see if an AMSBlock is a write
//-Returns:     The logical result of the test
//-ARG_I:       b              Pointer to the AMSBlock
//-Description: Test to see if an AMSBlock is a write.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  return (b->BlockType & BT_WRITE);
}

////////////////////////////////////////////////////////////////////////////
//-Extern: abiMakeWrite
static __inline int abiMakeWrite(pAMSBlock b)
//-Synopsis:    Make an AMSBlock a write
//-Returns:     New BlockType
//-ARG_I:       b              Pointer to the AMSBlock
//-Description: Make an AMSBlock a write
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  return (b->BlockType |= BT_WRITE);
}

static __inline int8  *abiDataPtr8 (pAMSBlock b) {return (int8  *)b->Contents;}
static __inline int16 *abiDataPtr16(pAMSBlock b) {return (int16 *)b->Contents;}
static __inline int32 *abiDataPtr32(pAMSBlock b) {return (int32 *)b->Contents;}

pAMSBlock abiConstruct(int32 NodeAddress, int32 DataType, int32 DataCount,
                       int32 BlockType);
void abiDestruct(pAMSBlock Block);
pAMSBlock abiClone(pAMSBlock Block, int32 DataCount);
pAMSBlock abiDuplicate(pAMSBlock pBlock);

bool abiShow(pAMSBlock pBlock, int NumberBytesToDump);
bool abiPrint(char *Tag, pAMSBlock pBlock, int NumberBytesToDump);
bool abiPrint2(char *Tag1, char *Tag2, pAMSBlock pBlock, int NumberBytesToDump);

bool abiLog(FILE *fh, pAMSBlock pBlock);

pAMSBlock abiFileRead(FILE *file, char *FileName, bool SecondaryHeader);
bool      abiFileWrite(FILE *file, char *FileName, bool SecondaryHeader,
                       pAMSBlock pBlock);
FILE *abiFileOpenTemp(char *Path, int *FileNumber);
int abiFileCommitTemp(char *Path, int FileNumber, char *user, char *group);
int abiFileUnlinkTemp(char *Path, int FileNumber);

typedef bool abicbRoutine(void *caller_data, pAMSBlock pBlock);
pAMSBlock abiEnvelope(int n, pAMSBlock pBlocks[]);
int abiProcessEnvelope(abicbRoutine *callback, void *caller_data, pAMSBlock pBlock);
int abiCountEnvelope(pAMSBlock pBlock);
int abiPrintEnvelope(char *tag, pAMSBlock pBlock);

pAMSBlock abiEnvelope2(bool shf, int BlockType, int NodeAddress, int DataType,
                       int n, pAMSBlock pBlocks[]);
int abiProcessEnvelope2(abicbRoutine *callback, void *caller_data,
                        pAMSBlock pBlock, bool shf);
int abiCountEnvelope2(pAMSBlock pBlock, bool shf);
int abiPrintEnvelope2(char *tag, pAMSBlock pBlock, bool shf);

int32 abiSizeWord(int8 **data);
int32 abiBlockSize(pAMSBlock pBlock, bool shf);
pAMSBlock abiDecode(int8 *data, int size, bool shf, bool ByteSwap);
int abiEncode(pAMSBlock Block, int8 *data, int max, bool shf, bool ByteSwap);

#endif // _ABI_H
