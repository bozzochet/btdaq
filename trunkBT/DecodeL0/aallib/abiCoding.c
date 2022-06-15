//  abiCoding.c   AMS Block decode and encode routines

//  Revision History
//    May 26 by X.Cai
//      - Initial version in SCH

#include "aallib.h"
#include "abi.h"

#ifdef NOT_LUA
#include "logfile.h"
#endif

#define abiDebug 1

#ifdef LITTLE_ENDIAN
# include <byteswap.h>
# define ByteSwap16(x) __bswap_16(x)
# define ByteSwap32(x) __bswap_32(x)
#else
# define ByteSwap16(x) (x)
# define ByteSwap32(x) (x)
#endif

////////////////////////////////////////////////////////////////////////////
//-Extern: abiSizeWord
int32 abiSizeWord(int8 **data)
//-Synopsis:    Decode the block size word from given location
//-Returns:     Block size and the given pointer will be changed
//-ARG_I:       data        Pointer to block size word
//-Description: Decode the block size word from given location
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
    int32 len;
    int8 *ptr = *data;

    len = (*ptr++);
    if (len & 0x80)
    {
      len &= 0x7F;
      len  = (len << 8) + *ptr++;
      len  = (len << 8) + *ptr++;
    }
    len = (len << 8) + *ptr++;

    *data = ptr;

    return len;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: abiBlockSize
int32 abiBlockSize(pAMSBlock pBlock, bool shf)
//-Synopsis:    Calculate AMSBlock memory size reqired include size word
//-Returns:     Total length include size word
//-ARG_I:       pBlock      Pointer to AMSBlock to release
//-ARG_I:       shf         AMSBlock secondary header flag cntrol
//-Description: Estimate AMSBlock memory size reqired
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
    int32 len;

    // Data and primary header
    len = pBlock->DataCount + 2;
    if ( pBlock->DataType > 0x1E ) len += 2;

    // secondary header
    if ( shf ) {
        len += 2;
        if ( (pBlock->BlockType & BT_REPLY) != 0 ) len += 4;
    }

    return len;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: abiDecode
pAMSBlock abiDecode(int8 *data, int size, bool shf, bool ByteSwap)
//-Synopsis:    Allocate memory for and convert from data to AMSBlock
//-Returns:     Pointer to the created AMSBlock.  Can be NULL in the
//-             event memory is exhausted or format error.
//-ARG_I:       data        Block data to be decoded
//-ARG_I:       size        Size of block data to be decoded
//-ARG_I:       shf         AMSBlock secondary header flag cntrol
//-ARG_I:       ByteSwap    Byte swap control,
//                          True if data is in little endian 16-bit word
//                          False if data is in big endian
//-Description: Allocate memory for and convert from data to AMSBlock
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
    int32 DataType, NodeAddress, BlockType, CommandTag, Error, DataTime;
    int32 len;
    pAMSBlock pBlock;
    int8 *ptr, *pend;
    int16 *p16 = (void *) data;
    int i;
    int16 w;

    // Swap data
    if ( ByteSwap ) {
        for ( i = 0; i < (size + 1) / 2; i++ ) p16[i] = ByteSwap16(p16[i]);
    }

    // Start and end pointers
    ptr  = data;
    pend = data + size;
    len  = size;

    // AMSBlock primary header
    w  = ((*ptr++) << 8);
    w |= (*ptr++);
    BlockType   = (w >> 14);
    NodeAddress = ((w >> 5) & 0x1FF);
    DataType    = (w & 0x1F);
    DataTime    = 0;
    CommandTag  = 0;
    Error       = 0;
    len        -= 2;
    if ( DataType == 0x1F ) {
        DataType = (DataType << 8) | (*ptr++);
        DataType = (DataType << 8) | (*ptr++);
        len     -= 2;
    }
    if ( len < 0 || ptr > pend ) {
#ifdef NOT_LUA
        LogError("abiDecode: Block format error\n");
#else
        printf("abiDecode: Block format error\n");
#endif
        return NULL;
    }

    // ABSBlock secondary header
    if ( shf && len > 0 ) {
        w  = ((*ptr++) << 8);
        w |= (*ptr++);
        CommandTag = (w & 0xFFF);
        Error      = (w >> 12);
        len -= 2;

        if ( (BlockType & BT_REPLY) != 0 ) {
            DataTime = (*ptr++);
            DataTime = ((DataTime << 8) | (*ptr++));
            DataTime = ((DataTime << 8) | (*ptr++));
            DataTime = ((DataTime << 8) | (*ptr++));
            len -= 4;
        }
        if ( len < 0 || ptr > pend ) {
#ifdef NOT_LUA
            LogError("abiDecode: Block format error\n");
#else
            printf("abiDecode: Block format error\n");
#endif
            return NULL;
        }
    }

    //printf("abiDecode: len %d ptr %p pend %p\n", len, ptr, pend);

    // Build block
    pBlock = abiConstruct(NodeAddress, DataType, len, BlockType);
    if ( pBlock == NULL ) {
#ifdef NOT_LUA
        LogError("abiDecode: Out of memory\n");
#else
        printf("abiDecode: Out of memory\n");
#endif
        return NULL;
    }

    // Fill headers
    pBlock->CommandTag = CommandTag;
    pBlock->Error      = Error;
    pBlock->DataTime   = DataTime;

    // Copy data
    if ( len > 0 ) memcpy(abiDataPtr8(pBlock), ptr, len);

    return pBlock;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: abiEncode
int abiEncode(pAMSBlock pBlock, int8 *data, int max, bool shf, bool ByteSwap)
//-Synopsis:    Encode AMSBlock to given location
//-Returns:     Total length after encoding or error if < 0
//-ARG_I:       Block       Pointer to AMSBlock to release
//-ARG_I:       data        Encoded data location
//-ARG_I:       max         Maximum space in data
//-ARG_I:       shf         AMSBlock secondary header flag cntrol
//-ARG_I:       ByteSwap    Byte swap control
//                          True if data is in little endian 16-bit word
//                          False if data is in big endian
//-Description: Encode AMSBlock to given location
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
    int32 len, blen;
    int i;
    int8 *ptr = data;
    int16 *p16 = (void *) data;
    int16 w;

    // Total length
    len = abiBlockSize(pBlock, shf);
    if ( len > 0x7FFFFFF8 ) {
#ifdef NOT_LUA
        LogError("abiEncode: Block too big\n");
#else
        printf("abiEncode: Block too big\n");
#endif
        return -1;
    }
    if ( len > 0x7FFF ) {
        blen = len + 4;
        *ptr++ = (int8) (len >> 24) | 0x80;
        *ptr++ = (int8) (len >> 16);
    }
    else blen = len + 2;
    *ptr++ = (int8) (len >> 8);
    *ptr++ = (int8) len;
    if ( blen > max ) {
#ifdef NOT_LUA
        LogError("abiEncode: No enough memory\n");
#else
        printf("abiEncode: No enough memory\n");
#endif
        return -1;
    }

    // AMSBlock primary header
    w  = (pBlock->BlockType << 14);
    w |= (pBlock->NodeAddress << 5);
    if ( pBlock->DataType > 0x1E ) {
        w |= 0x1F;
        *ptr++ = (int8) (w >> 8);
        *ptr++ = (int8) w;
        *ptr++ = (int8) (pBlock->DataType >> 8);
        *ptr++ = (int8) pBlock->DataType;
        len -= 4;
    }
    else {
        w |= (pBlock->DataType & 0x1F);
        *ptr++ = (int8) (w >> 8);
        *ptr++ = (int8) w;
        len -= 2;
    }

    // AMSBlock secondary header
    if ( shf ) {
        w = ((pBlock->Error << 12) | (pBlock->CommandTag & 0xFFF));
        *ptr++ = (int8) (w >> 8);
        *ptr++ = (int8) w;
        len -= 2;
        if ( (pBlock->BlockType & BT_REPLY) != 0 ) {
            *ptr++ = (int8) (pBlock->DataTime >> 24);
            *ptr++ = (int8) (pBlock->DataTime >> 16);
            *ptr++ = (int8) (pBlock->DataTime >> 8);
            *ptr++ = (int8) pBlock->DataTime;
            len -= 4;
        }
    }
    if ( len != pBlock->DataCount ) {
#ifdef NOT_LUA
        LogError("abiEncode: Length error\n");
#else
        printf("abiEncode: Length error\n");
#endif
        return -1;
    }

    // Copy data
    if ( len != 0 ) memcpy(ptr, abiDataPtr8(pBlock), len);

    // Swap data
    if ( ByteSwap ) {
        blen += (blen & 1);
        len = (blen / 2);
        for ( i = 0; i < len; i++ ) p16[i] = ByteSwap16(p16[i]);
    }

    return blen;
}
