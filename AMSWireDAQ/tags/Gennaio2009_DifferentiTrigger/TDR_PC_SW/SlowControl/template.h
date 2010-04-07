// file template.h
//
// see template.c
//
// A.Lebedev Apr-2005...

//~----------------------------------------------------------------------------

#ifndef _TEMPLATE_H
#define _TEMPLATE_H

#include "mylib.h"
#include "xformslib.h"

#undef USE_CAN_OR_AMSW
#ifdef USE_CAN
#define USE_CAN_OR_AMSW
#endif
#ifdef USE_AMSW
#define USE_CAN_OR_AMSW
#endif

#ifdef USE_CAN
#include "uscmlib.h"
#endif

#ifdef USE_AMSW
#include "amswlib.h"
#endif

void (*AFTER_ADDRESS_CHANGED_CALL)(void);
void (*AFTER_RUN_FLASH_CALL)(void);

/*
#ifdef USE_AMSW
_ww w_jinf_path;                         // Global Variable
#endif
*/

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void template(int argc, char **argv);
#ifdef USE_CAN
void bgn_form(FL_FORM **fdui, int X0, int Y0, int W, int H);
#else
#ifdef USE_AMSW
void bgn_form(FL_FORM **fdui, int X0, int Y0, int W, int H, char *pathname);
#else
void bgn_form(FL_FORM **fdui, int X0, int Y0, int W, int H);
#endif
#endif

void rob_form(char *name);
void end_form(FL_FORM *fdui);
void set_message_text(char *txt);
void update_template_objects(int *msg, int16 *err, bool *chk, int *P);

bool go_on(int16 err);

#ifdef USE_CAN
void create_USCM_control_H2_panel(int x0, int y0);
void create_USCM_control_H3_panel(int x0, int y0);
void create_USCM_control_H3S_panel(int x0, int y0);
void create_USCM_control_V_panel(int x0, int y0);
void create_USCM_status_panel(int x0, int y0);
void create_USCM_control_panel(int x0, int y0);
void USCM_boot_callback(FL_OBJECT *obj, long data);
#endif

//~============================================================================

#endif // _TEMPLATE_H
