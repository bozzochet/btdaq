/* -*-C-*-
 *  crc.h - Header file for CRC utilities
 *
 *-----------------------------------------------------------------------
 *  Created by X.Cai, on 23.04.2003
 *
 *-----------------------------------------------------------------------
 */

#ifndef __UTILS_CRC_H__
#define __UTILS_CRC_H__

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************
  Includes
 ****************************************************/

//??#include "types.h"
typedef unsigned short UINT16;
typedef int INT;
typedef unsigned char UINT8;

/****************************************************
  Definitions
 ****************************************************/

#define CRC16_POLYNOMIAL	0x1021

/****************************************************
  typedef and struct
 ****************************************************/

/****************************************************
  function declarations
 ****************************************************/

void CRC16_TableInit(void);
UINT16 CRC16_Calc(UINT8 *msg, INT len);
INT CRC16_Check(UINT8 *msg, INT len);
UINT16 CRC16_Attach(UINT8 *msg, INT len);
UINT16 CRC16_Continue(UINT8 *msg, INT len, UINT16 crc);

UINT16 crc_calc(unsigned char *data, int count);

/****************************************************
  End
 ****************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __UTILS_CRC_H__ */
