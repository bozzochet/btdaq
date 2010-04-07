// file crisalib.h
//
// see file crisalib.c
//
// A.Lebedev - Dec-2008...

//~----------------------------------------------------------------------------

#ifndef _CRISALIB_H
#define _CRISALIB_H

#include "cablib.h"

void get_CAB_command(FILE *file, int *siz, int8 *cmd);
void send_command_to_CAB(char *directory, char *input, int16 *err);
void select_CAB_CPU(char *input, int16 *err);
void select_REQUEST_BUS(char *input, int16 *err);
void select_REPLY_BUS(char *input, int16 *err);
int wait_n_seconds(char *input, int16 *err);

//~============================================================================

#endif // _CRISALIB_H
