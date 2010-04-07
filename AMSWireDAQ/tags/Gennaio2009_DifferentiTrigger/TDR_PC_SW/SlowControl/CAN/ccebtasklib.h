// file ccebtasklib.h
//
// see ccebtasklib.c
//
// A.Lebedev Jul-2008...

#ifndef _CCEBTASKLIB_H
#define _CCEBTASKLIB_H

#define BUILD_FOR_CC

#include "mylib.h"
#include "cceb.h"
#include "cceblib.h"

#define SOURCE    1
#define CONVERTED 2
#define TO_USCM   1    // convertion direction
#define BACK      2    // convertion direction

void convert_probation(int direction, int32 off);
void convert_V_RMS(int direction, int32 off);
void convert_I_RMS(int direction, int32 off);
void convert_Z_RMS(int direction, int32 off);
void convert_dallas_t(int direction, int32 off);
void convert_cernox_index(int direction, int32 off);
void convert_cernox_t(int direction, int j);
void convert_power(int direction, int32 off);
void convert_parameters(int direction);

void pack_item(const uint8 siz, const void *adr, uint8 **p);
void unpack_item(const uint8 siz, const void *adr, uint8 **p);

void read_CCEB_task_parameters(int16 *err);
void write_CCEB_task_parameters(_osc *OSC, _cc *cc, int16 *err);

//~============================================================================

#endif // _CCEBTASKLIB_H
