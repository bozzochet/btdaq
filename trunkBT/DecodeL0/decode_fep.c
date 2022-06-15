/***********************************************************
 *  File: decode_fep.c
 *
 *  C code for AMS-02 decode functions for FEP server datatypes
 *
 *  History:
 *    20190817 - created by X.Cai
 ***********************************************************/

#include "aallib/logfile.h"
#include "aallib/al_lib.h"
#include "aallib/abi.h"

#include "node_address.h"
#include "decode_funcs.h"
#include "influxdb_client.h"

//----------------------------------------------------------
//  DB entries list
//----------------------------------------------------------

//----------------------------------------------------------
//  Function: decode_fep
//      Decode function for FEP servers
//
//  Inputs:
//      block ---- AMS block
//      utime ---- Time in microseconds
//
//  Return:
//      None
//----------------------------------------------------------
void decode_fep(AMSBlock *block, uint64_t utime)
{
    uint8_t *p = abiDataPtr8(block);

    // Datatypes
    switch ( block->DataType ) {
        // CHD
        case 0x1FEED3:
        case 0x1FEEDA:
            // Change node address to JMDC ID inside CHD
            block->NodeAddress = 0xC + (p[2] >> 6);
            decode_jmdc_chd(block, utime);
            break;
    }
}

