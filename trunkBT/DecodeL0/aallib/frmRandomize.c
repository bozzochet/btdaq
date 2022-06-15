/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2004 by PADSOFT, Inc.                        */
/*                                                                           */
/*    Developed for use on AMS-2, An International Space Station payload     */
/*                      searching for missing matter                         */
/*                                                                           */
/*                        All rights reserved                                */
/*                                                                           */
/* Developed by:        Peter Armistead Dennett                              */
/*                            PADSOFT, Inc                                   */
/*                          61 Harbor Lane                                   */
/*                        Kemah, Texas 77565                                 */
/*                          (281) 334 3800                                   */
/*                      http://www.padsoft.com                               */
/*                                                                           */
/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/

//  frmRandomize.c   AMS Frame randomizer

//  Revision History
//    Apr 2002 by Peter Dennett
//      - Initial version in Kemah office (AALFRM)
//    Sep 2003 by Peter Dennett
//      - Conversion for use in payload simulator

#include "aallib.h"
#include "frm.h"
 
#if 0

//  From Alexei's AMS-1 deframing program

static int16 PsuedoRandom[255] = 
{
    0xFF48, 0x0EC0, 0x9A0D, 0x70BC, 0x8E2C, 0x93AD, 0xA7B7, 0x46CE,
    0x5A97, 0x7DCC, 0x32A2, 0xBF3E, 0x0A10, 0xF188, 0x94CD, 0xEAB1,
    0xFE90, 0x1D81, 0x341A, 0xE179, 0x1C59, 0x275B, 0x4F6E, 0x8D9C,
    0xB52E, 0xFB98, 0x6545, 0x7E7C, 0x1421, 0xE311, 0x299B, 0xD563,
    0xFD20, 0x3B02, 0x6835, 0xC2F2, 0x38B2, 0x4EB6, 0x9EDD, 0x1B39,
    0x6A5D, 0xF730, 0xCA8A, 0xFCF8, 0x2843, 0xC622, 0x5337, 0xAAC7,
    0xFA40, 0x7604, 0xD06B, 0x85E4, 0x7164, 0x9D6D, 0x3DBA, 0x3672,
    0xD4BB, 0xEE61, 0x9515, 0xF9F0, 0x5087, 0x8C44, 0xA66F, 0x558F,
    0xF480, 0xEC09, 0xA0D7, 0x0BC8, 0xE2C9, 0x3ADA, 0x7B74, 0x6CE5,
    0xA977, 0xDCC3, 0x2A2B, 0xF3E0, 0xA10F, 0x1889, 0x4CDE, 0xAB1F,
    0xE901, 0xD813, 0x41AE, 0x1791, 0xC592, 0x75B4, 0xF6E8, 0xD9CB,
    0x52EF, 0xB986, 0x5457, 0xE7C1, 0x421E, 0x3112, 0x99BD, 0x563F,
    0xD203, 0xB026, 0x835C, 0x2F23, 0x8B24, 0xEB69, 0xEDD1, 0xB396,
    0xA5DF, 0x730C, 0xA8AF, 0xCF82, 0x843C, 0x6225, 0x337A, 0xAC7F,
    0xA407, 0x604D, 0x06B8, 0x5E47, 0x1649, 0xD6D3, 0xDBA3, 0x672D,
    0x4BBE, 0xE619, 0x515F, 0x9F05, 0x0878, 0xC44A, 0x66F5, 0x58FF,
    0x480E, 0xC09A, 0x0D70, 0xBC8E, 0x2C93, 0xADA7, 0xB746, 0xCE5A,
    0x977D, 0xCC32, 0xA2BF, 0x3E0A, 0x10F1, 0x8894, 0xCDEA, 0xB1FE,
    0x901D, 0x8134, 0x1AE1, 0x791C, 0x5927, 0x5B4F, 0x6E8D, 0x9CB5,
    0x2EFB, 0x9865, 0x457E, 0x7C14, 0x21E3, 0x1129, 0x9BD5, 0x63FD,
    0x203B, 0x0268, 0x35C2, 0xF238, 0xB24E, 0xB69E, 0xDD1B, 0x396A,
    0x5DF7, 0x30CA, 0x8AFC, 0xF828, 0x43C6, 0x2253, 0x37AA, 0xC7FA,
    0x4076, 0x04D0, 0x6B85, 0xE471, 0x649D, 0x6D3D, 0xBA36, 0x72D4,
    0xBBEE, 0x6195, 0x15F9, 0xF050, 0x878C, 0x44A6, 0x6F55, 0x8FF4,
    0x80EC, 0x09A0, 0xD70B, 0xC8E2, 0xC93A, 0xDA7B, 0x746C, 0xE5A9,
    0x77DC, 0xC32A, 0x2BF3, 0xE0A1, 0x0F18, 0x894C, 0xDEAB, 0x1FE9,
    0x01D8, 0x1341, 0xAE17, 0x91C5, 0x9275, 0xB4F6, 0xE8D9, 0xCB52,
    0xEFB9, 0x8654, 0x57E7, 0xC142, 0x1E31, 0x1299, 0xBD56, 0x3FD2,
    0x03B0, 0x2683, 0x5C2F, 0x238B, 0x24EB, 0x69ED, 0xD1B3, 0x96A5,
    0xDF73, 0x0CA8, 0xAFCF, 0x8284, 0x3C62, 0x2533, 0x7AAC, 0x7FA4,
    0x0760, 0x4D06, 0xB85E, 0x4716, 0x49D6, 0xD3DB, 0xA367, 0x2D4B,
    0xBEE6, 0x1951, 0x5F9F, 0x0508, 0x78C4, 0x4A66, 0xF558
};

////////////////////////////////////////////////////////////////////////////
//-Extern: frmRandomize
void frmRandomize(int8 *Frame, int32 Bytes)
//-Synopsis:  Perform psuedo randomization funtion on data
//-Returns: N/A
//-ARG_I:       Framer     Place to perform function.
//-ARG_I:       Bytes      Number of bytes to for the frame
//-Description: Perform psuedo randomization function on a data block.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:       Orginal code from Alexei's AMS-1 de-framing software
//-Bugs:
//-Revisions:
//-End:
{
int i;
int16 *p, *endp;

#ifndef AMS_FLIGHT_SOFTWARE
  if (((int32) Frame) & 1)  //??
    Panic("frmRandomize: Data alignment problem in frame");
#endif

  i = 0;
  endp = (unsigned short *)Frame + (Bytes/2);

  for (p=(unsigned short *) Frame; p<endp; p++) 
  {
    // *p = ((*p) >> 8) | ((*p) << 8); //? Byte swap I assume
    *p ^= PsuedoRandom[i++];
    i %= 255;  
  }
  return;
}

#else

void frmRandomize(int8 *data, int32 size)
{
static int8 Init = 0;
static int8 random_table[510];             // Pseudo-randomize loop-up table
int i;

  if ( ! Init) { 
    int8 data = 0xff;      // Initial value
    int i, j;

    Init = 1;

    /* Loop to fill table */
    for ( i = 0; i < 510; i++ ) {
      /* fill data */
      random_table[i] = data;
        
      /* Calculate the next byte */
      for ( j = 0; j < 8; j++ )
        data = ((data ^ (data << 3) ^ (data << 5) ^ (data << 7)) & 0x80) ?
          ((data << 1) + 1) : (data << 1);
    }
  }

  for (i = 0; i < size; i++) data[i] ^= random_table[i % 510];

  return;
}
#endif
