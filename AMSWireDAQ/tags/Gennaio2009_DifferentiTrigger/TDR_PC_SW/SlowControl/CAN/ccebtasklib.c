// file ccebtasklib.c
//
// Common Functions for CCEB USCM and Commanding Programs
//
// A.Lebedev Jul-2008...

#include "ccebtasklib.h"

struct _ADC    *adc;
struct _cernox *cernox[1000];

uint8 cceb_model;
_osc OSC;

_cc cc_source[4], cc_converted[4];
_dallas_t_table dallas_t_table[2];
_cernox_t_table cernox_t_table[3];

//~============================================================================

void convert_probation(int direction, int32 off) {

  int i;

  for (i=0; i<4; i++) {
    int32 *p_s = ((int32*)&cc_source[i]    + off / sizeof(int32));
    int32 *p_c = ((int32*)&cc_converted[i] + off / sizeof(int32));
    switch (direction) {
      case TO_USCM: *p_c = (float)*p_s * 1000.0; break;
      case BACK:    *p_s = (float)*p_c / 1000.0; break;
    }
  }
}

//~----------------------------------------------------------------------------

void convert_V_RMS(int direction, int32 off) {

  int i;

  for (i=0; i<4; i++) {
    float AV = adc[V_RMS_0+8*i].A;
    int32 *p_s = ((int32*)&cc_source[i]    + off / sizeof(int32));
    int32 *p_c = ((int32*)&cc_converted[i] + off / sizeof(int32));
    switch (direction) {
      case TO_USCM: *p_c = (float)*p_s / AV + 0.5; break;
      case BACK:    *p_s = (float)*p_c * AV + 0.5; break;
    }
  }
}

//~----------------------------------------------------------------------------

void convert_I_RMS(int direction, int32 off) {

  int i;

  for (i=0; i<4; i++) {
    float AI = adc[I_RMS_0+8*i].A;
    int32 *p_s = ((int32*)&cc_source[i]    + off / sizeof(int32));
    int32 *p_c = ((int32*)&cc_converted[i] + off / sizeof(int32));
    switch (direction) {
      case TO_USCM: *p_c = (float)*p_s / 1000.0 / AI + 0.5; break;
      case BACK:    *p_s = (float)*p_c * 1000.0 * AI + 0.5; break;
    }
  }
}

//~----------------------------------------------------------------------------

void convert_Z_RMS(int direction, int32 off) {

  int i;

  for (i=0; i<4; i++) {
    float AV = adc[V_RMS_0+8*i].A;
    float AI = adc[I_RMS_0+8*i].A;
    int32 *p_s = ((int32*)&cc_source[i]    + off / sizeof(int32));
    int32 *p_c = ((int32*)&cc_converted[i] + off / sizeof(int32));
    switch (direction) {
      case TO_USCM: *p_c = (float)*p_s * (real)0x10000 / (AV / AI) + 0.5; break;
      case BACK:    *p_s = (float)*p_c / (real)0x10000 * (AV / AI) + 0.5; break;
    }
  }}

//~----------------------------------------------------------------------------

void convert_dallas_t(int direction, int32 off) {

  int i;

  for (i=0; i<4; i++) {
    char *p_s = ((char*)&cc_source[i]    + off / sizeof(char));
    char *p_c = ((char*)&cc_converted[i] + off / sizeof(char));
    switch (direction) {
      case TO_USCM: *p_c = *p_s * 2; break;
      case BACK:    *p_s = *p_c / 2; break;
    }
  }
}

//~----------------------------------------------------------------------------

void convert_cernox_index(int direction, int32 off) {

  int i;

  for (i=0; i<4; i++) {
    char *p_s = ((char*)&cc_source[i]    + off / sizeof(char));
    char *p_c = ((char*)&cc_converted[i] + off / sizeof(char));
    switch (direction) {
      case TO_USCM: *p_c = *p_s; break;
      case BACK:    *p_s = *p_c; break;
    }
  }
}

//~----------------------------------------------------------------------------

void convert_cernox_t(int direction, int j) {

  int i;
  int ADC_cha[4] = {CC0_COLD_TIP, CC1_COLD_TIP, CC2_COLD_TIP, CC3_COLD_TIP};

  for (i=0; i<4; i++) {
    _cernox_t_table *p_s = &cc_source[i].cernox_t_table[j];
    _cernox_t_table *p_c = &cc_converted[i].cernox_t_table[j];
    struct _cernox *p;
    real M0 = adc[ADC_cha[i]+0].A;
    real M1 = adc[ADC_cha[i]+1].A;
    switch (direction) {
      case TO_USCM:
        p = cernox[cc_source[i].Cold_tip_t.index];
        p_c->Cold_tip_t_min = cernox_T_to_code((real)p_s->Cold_tip_t_min, M0, p);
        p_c->Cold_tip_t_max = cernox_T_to_code((real)p_s->Cold_tip_t_max, M0, p);
        p = cernox[cc_source[i].VCS_t.index];
        p_c->VCS_t_min      = cernox_T_to_code((real)p_s->VCS_t_min,      M1, p);
        p_c->VCS_t_max      = cernox_T_to_code((real)p_s->VCS_t_max,      M1, p);
      break;
      case BACK:
        p = cernox[cc_converted[i].Cold_tip_t.index];
        p_s->Cold_tip_t_min = cernox_code_to_T((real)p_c->Cold_tip_t_min, M0, p);
        p_s->Cold_tip_t_max = cernox_code_to_T((real)p_c->Cold_tip_t_max, M0, p);
        p = cernox[cc_converted[i].VCS_t.index];
        p_s->VCS_t_min      = cernox_code_to_T((real)p_c->VCS_t_min,      M1, p);
        p_s->VCS_t_max      = cernox_code_to_T((real)p_c->VCS_t_max,      M1, p);
      break;
    }
  }
}

//~----------------------------------------------------------------------------

void convert_power(int direction, int32 off) {

  int i;

  for (i=0; i<4; i++) {
    float AV = adc[V_BUS_0+8*i].A;
    float AI = adc[I_BUS_0+8*i].A;
    int32 *p_s = ((int32*)&cc_source[i]    + off / sizeof(int32));
    int32 *p_c = ((int32*)&cc_converted[i] + off / sizeof(int32));
    switch (direction) {
      case TO_USCM: *p_c = (float)*p_s / (AV * AI) + 0.5; break;
      case BACK:    *p_s = (float)*p_c * (AV * AI) + 0.5; break;
    }
  }
}

//~----------------------------------------------------------------------------

void convert_parameters(int direction) {

  int j;

  switch (direction) {
    case TO_USCM:
      memcpy(&cc_converted[0], &cc_source[0], sizeof(cc_source));
    break;
    case BACK:
      memcpy(&cc_source[0], &cc_converted[0], sizeof(cc_source));
    break;
  }

  convert_power(    direction, O(power.min.value));
  convert_probation(direction, O(power.min.probation));
  convert_power(    direction, O(power.max.value));
  convert_probation(direction, O(power.max.probation));

  convert_power(    direction, O(P_bus.min.value));
  convert_probation(direction, O(P_bus.min.probation));
  convert_power(    direction, O(P_bus.max.value));
  convert_probation(direction, O(P_bus.max.probation));

  convert_V_RMS(    direction, O(V_RMS.min.value));
  convert_probation(direction, O(V_RMS.min.probation));
  convert_V_RMS(    direction, O(V_RMS.max.value));
  convert_probation(direction, O(V_RMS.max.probation));

  convert_I_RMS(    direction, O(I_RMS.min.value));
  convert_probation(direction, O(I_RMS.min.probation));
  convert_I_RMS(    direction, O(I_RMS.max.value));
  convert_probation(direction, O(I_RMS.max.probation));

  convert_Z_RMS(    direction, O(Z_RMS.min.value));
  convert_probation(direction, O(Z_RMS.min.probation));
  convert_Z_RMS(    direction, O(Z_RMS.max.value));
  convert_probation(direction, O(Z_RMS.max.probation));

  convert_probation(direction, O(Reject_t.probation));
  convert_dallas_t( direction, O(Reject_t.min.value));
  convert_probation(direction, O(Reject_t.min.probation));
  convert_dallas_t( direction, O(Reject_t.max.value));
  convert_probation(direction, O(Reject_t.max.probation));

  convert_probation(direction, O(Case_t.probation));
  convert_dallas_t( direction, O(Case_t.min.value));
  convert_probation(direction, O(Case_t.min.probation));
  convert_dallas_t( direction, O(Case_t.max.value));
  convert_probation(direction, O(Case_t.max.probation));

  for (j=0; j<2; j++) {
    convert_dallas_t( direction, O(dallas_t_table[j].t_min));
    convert_dallas_t( direction, O(dallas_t_table[j].t_max));
    convert_power(    direction, O(dallas_t_table[j].P_max));
  }

  convert_cernox_index(direction, O(Cold_tip_t.index));
  convert_cernox_index(direction, O(VCS_t.index));

  for (j=0; j<3; j++) {
    convert_cernox_t( direction, j);
    convert_power(    direction, O(cernox_t_table[j].P_max));
  }
  
  convert_power(    direction, O(power_target));

  convert_V_RMS(    direction, O(V_RMS_shutdown));
}

//~----------------------------------------------------------------------------

void pack_item(const uint8 siz, const void *adr, uint8 **p) {

  if (siz == 0) {
    uint8 *pp = (uint8*)adr;
    *(*p)    = (*pp++ & 0x0F) << 4;
    *(*p)++ |= (*pp   & 0x0F) << 0;
  }
  if (siz == 1) {
    uint8 *pp = (uint8*)adr;
    *(*p)++ = *pp >>  0;
  }
  if (siz == 2) {
    uint16 *pp = (uint16*)adr;
    *(*p)++ = *pp >>  8;
    *(*p)++ = *pp >>  0;
  }
  if (siz == 4) {
    uint32 *pp = (uint32*)adr;
    *(*p)++ = *pp >> 24;
    *(*p)++ = *pp >> 16;
    *(*p)++ = *pp >>  8;
    *(*p)++ = *pp >>  0;
  }
}

//~----------------------------------------------------------------------------

void unpack_item(const uint8 siz, const void *adr, uint8 **p) {

  if (siz == 0) {
    uint8 *pp = (uint8*)adr;
    *pp++ = (*(*p)   >> 4) & 0x0F;
    *pp   = (*(*p)++ >> 0) & 0x0F;
  }
  if (siz == 1) {
    uint8 *pp = (uint8*)adr;
    *pp  = *(*p)++ >>  0;
  }
  if (siz == 2) {
    uint16 *pp = (uint16*)adr;
    *pp  = (uint16)*(*p)++ <<  8;
    *pp |= (uint16)*(*p)++ <<  0;
  }
  if (siz == 4) {
    uint32 *pp = (uint32*)adr;
    *pp  = (uint32)*(*p)++ << 24;
    *pp |= (uint32)*(*p)++ << 16;
    *pp |= (uint32)*(*p)++ <<  8;
    *pp |= (uint32)*(*p)++ <<  0;
  }
}

//~----------------------------------------------------------------------------

void read_CCEB_task_parameters(int16 *err) {

  int i, j;

  uint8 *p;
  int32 RQ_typ = 0x1D;
  int8  RP_dat[10000];
  int32 RP_siz = 790;
  int32 RP_cnt;

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ,      0,   NULL, RP_siz, &RP_cnt, RP_dat,     err);

  if (*err) return;

  if (RP_cnt != RP_siz) {
    *err = 0x0100;
    return;
  }

  p = RP_dat;

  unpack_item(1, &cceb_model,   &p);
  unpack_item(1, &OSC.selected, &p);

  for (i=0; i<4; i++) {
    _cc *c = &cc_converted[i];
    
    unpack_item(1, &c->CCIF, &p);
    unpack_item(1, &c->feed, &p);
    
    unpack_item(2, &c->check.fail, &p);
    for (j=0; j<8; j++) {
      unpack_item(0, &c->fail.limit[2*j], &p);
    }
    
    unpack_item(2, &c->check.alarm, &p);
    for (j=0; j<8; j++) {
      unpack_item(0, &c->alarm.limit[2*j], &p);
    }

    unpack_item(4, &c->power.min.value, &p);
    unpack_item(4, &c->power.min.probation, &p);
    unpack_item(4, &c->power.max.value, &p);
    unpack_item(4, &c->power.max.probation, &p);

    unpack_item(4, &c->P_bus.min.value, &p);
    unpack_item(4, &c->P_bus.min.probation, &p);
    unpack_item(4, &c->P_bus.max.value, &p);
    unpack_item(4, &c->P_bus.max.probation, &p);

    unpack_item(4, &c->V_RMS.min.value, &p);
    unpack_item(4, &c->V_RMS.min.probation, &p);
    unpack_item(4, &c->V_RMS.max.value, &p);
    unpack_item(4, &c->V_RMS.max.probation, &p);

    unpack_item(4, &c->I_RMS.min.value, &p);
    unpack_item(4, &c->I_RMS.min.probation, &p);
    unpack_item(4, &c->I_RMS.max.value, &p);
    unpack_item(4, &c->I_RMS.max.probation, &p);

    unpack_item(4, &c->Z_RMS.min.value, &p);
    unpack_item(4, &c->Z_RMS.min.probation, &p);
    unpack_item(4, &c->Z_RMS.max.value, &p);
    unpack_item(4, &c->Z_RMS.max.probation, &p);

    unpack_item(1, &c->Reject_t.use, &p);
    unpack_item(4, &c->Reject_t.probation, &p);
    unpack_item(1, &c->Reject_t.min.value, &p);
    unpack_item(4, &c->Reject_t.min.probation, &p);
    unpack_item(1, &c->Reject_t.max.value, &p);
    unpack_item(4, &c->Reject_t.max.probation, &p);

    unpack_item(1, &c->Case_t.use, &p);
    unpack_item(4, &c->Case_t.probation, &p);
    unpack_item(1, &c->Case_t.min.value, &p);
    unpack_item(4, &c->Case_t.min.probation, &p);
    unpack_item(1, &c->Case_t.max.value, &p);
    unpack_item(4, &c->Case_t.max.probation, &p);

    for (j=0; j<2; j++) {
      unpack_item(1, &c->dallas_t_table[j].t_min, &p);
      unpack_item(1, &c->dallas_t_table[j].t_max, &p);
      unpack_item(4, &c->dallas_t_table[j].P_max, &p);
    }

    unpack_item(1, &c->Cold_tip_t.index, &p);
    unpack_item(1, &c->Cold_tip_t.use,   &p);
    unpack_item(1, &c->VCS_t.index, &p);
    unpack_item(1, &c->VCS_t.use,   &p);

    for (j=0; j<3; j++) {
      unpack_item(2, &c->cernox_t_table[j].Cold_tip_t_min, &p);
      unpack_item(2, &c->cernox_t_table[j].Cold_tip_t_max, &p);
      unpack_item(2, &c->cernox_t_table[j].VCS_t_min, &p);
      unpack_item(2, &c->cernox_t_table[j].VCS_t_max, &p);
      unpack_item(4, &c->cernox_t_table[j].P_max, &p);
    }
  
    unpack_item(1, &c->phase_startup, &p);
    unpack_item(1, &c->phase_shutdown, &p);
    unpack_item(1, &c->phase_target, &p);
    unpack_item(1, &c->phase_min, &p);
    unpack_item(1, &c->phase_max, &p);

    unpack_item(4, &c->power_target, &p);

    unpack_item(4, &c->V_RMS_shutdown, &p);
  }
}

//~----------------------------------------------------------------------------

void write_CCEB_task_parameters(_osc *OSC, _cc *cc, int16 *err) {

  int i, j;
  uint8 *p;
  int32 RQ_typ = 0x1D;
  int8  RQ_dat[1000];
  int16 RQ_cnt;

  p = RQ_dat;

  pack_item(1, &cceb_model, &p);  
  pack_item(1, &OSC->selected, &p);
  
  for (i=0; i<4; i++) {
    _cc *c = &cc[i];
    
    pack_item(1, &c->CCIF, &p);
    pack_item(1, &c->feed, &p);

    pack_item(2, &c->check.fail, &p);
    for (j=0; j<8; j++) {
      pack_item(0, &c->fail.limit[2*j], &p);
    }
    
    pack_item(2, &c->check.alarm, &p);
    for (j=0; j<8; j++) {
      pack_item(0, &c->alarm.limit[2*j], &p);
    }

    pack_item(4, &c->power.min.value, &p);
    pack_item(4, &c->power.min.probation, &p);
    pack_item(4, &c->power.max.value, &p);
    pack_item(4, &c->power.max.probation, &p);

    pack_item(4, &c->P_bus.min.value, &p);
    pack_item(4, &c->P_bus.min.probation, &p);
    pack_item(4, &c->P_bus.max.value, &p);
    pack_item(4, &c->P_bus.max.probation, &p);

    pack_item(4, &c->V_RMS.min.value, &p);
    pack_item(4, &c->V_RMS.min.probation, &p);
    pack_item(4, &c->V_RMS.max.value, &p);
    pack_item(4, &c->V_RMS.max.probation, &p);

    pack_item(4, &c->I_RMS.min.value, &p);
    pack_item(4, &c->I_RMS.min.probation, &p);
    pack_item(4, &c->I_RMS.max.value, &p);
    pack_item(4, &c->I_RMS.max.probation, &p);

    pack_item(4, &c->Z_RMS.min.value, &p);
    pack_item(4, &c->Z_RMS.min.probation, &p);
    pack_item(4, &c->Z_RMS.max.value, &p);
    pack_item(4, &c->Z_RMS.max.probation, &p);

    pack_item(1, &c->Reject_t.use, &p);
    pack_item(4, &c->Reject_t.probation, &p);
    pack_item(1, &c->Reject_t.min.value, &p);
    pack_item(4, &c->Reject_t.min.probation, &p);
    pack_item(1, &c->Reject_t.max.value, &p);
    pack_item(4, &c->Reject_t.max.probation, &p);

    pack_item(1, &c->Case_t.use, &p);
    pack_item(4, &c->Case_t.probation, &p);
    pack_item(1, &c->Case_t.min.value, &p);
    pack_item(4, &c->Case_t.min.probation, &p);
    pack_item(1, &c->Case_t.max.value, &p);
    pack_item(4, &c->Case_t.max.probation, &p);

    for (j=0; j<2; j++) {
      pack_item(1, &c->dallas_t_table[j].t_min, &p);
      pack_item(1, &c->dallas_t_table[j].t_max, &p);
      pack_item(4, &c->dallas_t_table[j].P_max, &p);
    }

    pack_item(1, &c->Cold_tip_t.index, &p);
    pack_item(1, &c->Cold_tip_t.use,   &p);
    pack_item(1, &c->VCS_t.index, &p);
    pack_item(1, &c->VCS_t.use,   &p);

    for (j=0; j<3; j++) {
      pack_item(2, &c->cernox_t_table[j].Cold_tip_t_min, &p);
      pack_item(2, &c->cernox_t_table[j].Cold_tip_t_max, &p);
      pack_item(2, &c->cernox_t_table[j].VCS_t_min, &p);
      pack_item(2, &c->cernox_t_table[j].VCS_t_max, &p);
      pack_item(4, &c->cernox_t_table[j].P_max, &p);
    }
  
    pack_item(1, &c->phase_startup, &p);
    pack_item(1, &c->phase_shutdown, &p);
    pack_item(1, &c->phase_target, &p);
    pack_item(1, &c->phase_min, &p);
    pack_item(1, &c->phase_max, &p);

    pack_item(4, &c->power_target, &p);

    pack_item(4, &c->V_RMS_shutdown, &p);
  }

  RQ_cnt = p - RQ_dat;
  
//to_USCM(R_W,   RQ_typ, RQ_cnt, RQ_dat, RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(WRITE, RQ_typ, RQ_cnt, RQ_dat,      0,    NULL,   NULL,     err);
}

//~============================================================================
