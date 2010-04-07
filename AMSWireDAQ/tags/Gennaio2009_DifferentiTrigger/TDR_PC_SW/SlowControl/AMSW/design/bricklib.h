// file bricklib.h
//
// A.Lebedev Apr-2005...
//

//~----------------------------------------------------------------------------

#ifndef _BRICKLIB_H
#define _BRICKLIB_H

//~----------------------------------------------------------------------------

#include "daqlib.h"

void brick_channel_reset(int cha, bool *chk, int16 *err);
void brick_status_read(int8 *sta, bool *chk, int16 *err);
void brick_shutdown(bool *chk, int16 *err);
void brick_channel_read(int cha, int16 *cod, bool *chk, int16 *err);
void brick_channel_write(int cha, int16 cod, bool *chk, int16 *err);
void brick_set_DCDC(int DCDC, bool ONOFF, int8 *sta, bool *chk, int16 *err);

//~============================================================================

#endif // _BRICKLIB_H
