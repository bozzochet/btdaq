#ifndef SLOWCONTROL_H 
#define SLOWCONTROL_H

#include <stdio.h>
#include "SlowControlTypes.h"
#include "amswcom.h"
#include "QList.h"

class SlowControl {
  
 private:
  ushort Command, Debug, Register, Address, Half, Bus, AMSWaddr, ID, ErrLecroy, ErrAmsw;
  char CardType[20];
  char CardName[40];
  sACTEL Actel;
  AMSWcom *Amswire;

 protected:
  //ushort ;
  //  AMSWcom *AMSW;
  ushort Trip_Brother, Status_Brother, PowerOn, Control_Brother;



 public:
  SlowControl(AMSWcom *node, ushort addr=0xffff);
  ~SlowControl();
  ushort GetErrLecroy() { return ErrLecroy; }
  ushort GetErrAmsw() { return ErrAmsw; }
  void SetAMSWaddr(unsigned int addr) { AMSWaddr=addr; }
  unsigned int GetAMSWaddr() { return AMSWaddr; }
  void SetCardType(char *name) { sprintf(CardType,"%s", name); }
  const char* GetCardType() { return CardType; }
  void SetName(char *name) { sprintf(CardName,"%s", name); }
  const char* GetName() { return CardName; }
  void SetDebug(int mode) { Debug=mode; }
  void SetRegister(ushort r); 
  ushort GetRegister();
  void SetTrackerBus();
  ushort GetBus() { return Bus; }
  void SetHalf(ushort h) { Half=(h>0); }
  ushort GetHalf() { return Half; }
  void SetID(ushort i) { ID=i; }
  ushort GetID() { return ID; }
  void SetAddress(ushort a) { Address=(0x1f & a)<<7; }
  ushort GetAddress() {return Address>>7;}
  void SetActelId(sACTEL a) { Actel=a; }
  sACTEL GetActelId() { return Actel; }
  void SetActelBrother(sActelMode mode);
  void InitLecroy();
  ushort ReadRegister(ushort r);
  void ReadRegister(ushort r, uint freq, QList *qlist);
  void BuildCommand(sCommandMode mode, ushort data);
  void DoParity(ushort *x, ushort data);
  //  void ReadActel();
  ushort WriteRegister(ushort r, ushort data);
  void WriteRegister(ushort r, ushort data, uint freq, QList *qlist);
  ushort GetStatus_Brother() { return Status_Brother; }
  ushort GetControl_Brother() { return Control_Brother; }
  ushort GetTrip_Brother() { return Trip_Brother; }
  ushort GetPowerOn() { return PowerOn; }

};


class S9011AT : public SlowControl {

 private:
  ushort Trip_S9051, LControl_S9051, GControl_S9051, Status_S9051;
  ushort Trip_S9053, LControl_S9053, GControl_S9053, Status_S9053;
  ushort Trip_S9055, LControl_S9055, GControl_S9055, Status_S9055;
  //ushort Trip_Brother, Control_Brother, Status_Brother;
  //ushort PowerOn;

 public:
  S9011AT(AMSWcom *node, ushort amswaddr, sACTEL actel, ushort half);
  virtual ~S9011AT();
  //  void SetActelBrother(sActelMode mode);
  void SetS9051(ushort mode);
  ushort Convert(sDCDCmode mode);
  ushort ConvertS9053(sDCDCmode mode);
  void SetS9051(sDCDCmode dc0, sDCDCmode dc1, sDCDCmode dc2, sDCDCmode dc3);
  void SetS9051(ushort group, ushort side, sDCDC mode);
  void SetS9053(sDCDCmode dc0, sDCDCmode dc1);
  void SetS9053(ushort group, sDCDCmode dc);
  void SetS9053(ushort group, ushort side, sDCDC mode);
  void SetS9055(sDCDCmode dc0, sDCDCmode dc1);
  void SetS9055(ushort group, ushort side, sDCDC mode);
  ushort GetActel();
  ushort GetS9051();
  ushort GetS905355();
  ushort GetGlobalControl();
  ushort GetTripStatus();


  ushort GetStatus_S9051() { return Status_S9051; }
  ushort GetLControl_S9051() { return LControl_S9051; }
  ushort GetGControl_S9051() { return GControl_S9051; }
  ushort GetTrip_S9051() { return Trip_S9051; }

  ushort GetStatus_S9053() { return Status_S9053; }
  ushort GetLControl_S9053() { return LControl_S9053; }
  ushort GetGControl_S9053() { return GControl_S9053; }
  ushort GetTrip_S9053() { return Trip_S9053; }

  ushort GetStatus_S9055() { return Status_S9055; }
  ushort GetLControl_S9055() { return LControl_S9055; }
  ushort GetGControl_S9055() { return GControl_S9055; }
  ushort GetTrip_S9055() { return Trip_S9055; }


};


class TPSFE : public SlowControl {

 private:
  ushort CntTDR[6], CntLRS[6], CntLRK[6], CntActel;
  //ushort Control_Brother, Status_Brother;
  ushort AutoMode_TDR, AutoMode_LRS, AutoMode_LRK;
  ushort Status_TDR, Status_LRS, Status_LRK;
  ushort LControl_TDR, LControl_LRS, LControl_LRK;
  ushort GControl_LRS, GControl_LRK;

  void FillStatLCtrl(ushort res, ushort *lctrl, ushort *automode, ushort *status);

 public:
  //ushort TDRstatus[6], LRSstatus[6], LRKstatus[6]; // 1=on , 0=off

  TPSFE(AMSWcom *node, ushort amswaddr, sACTEL actel, sTPSFEaddr address);
  virtual ~TPSFE();

  ushort GetGControlLRS() { return GControl_LRS; }
  ushort GetGControlLRK() { return GControl_LRK; }
  ushort GetStatusTDR() { return Status_TDR; }
  ushort GetAutoModeTDR() { return AutoMode_TDR; }
  ushort GetAutoModeLRS() { return AutoMode_LRS; }
  ushort GetAutoModeLRK() { return AutoMode_LRK; }

  ushort GetCntTDR(ushort tdr);
  ushort GetCntLRS(ushort tdr);
  ushort GetCntLRK(ushort tdr);

  ushort GetActel();
  ushort GetLRS();
  ushort GetLRK();
  ushort GetGControl();
  ushort GetTPSFEId();
  void GetTripCounters();
  //void SetActelBrother(sActelMode brother);
  void SetTDR(sDCDC tdr0, sDCDC tdr1, sDCDC tdr2, sDCDC tdr3, sDCDC tdr4, sDCDC tdr5, sDCDC automode);
  void SetTDR(ushort setting);
  void SetTDR(ushort tdrnum, sDCDC mode);
  void SetLRS(sDCDC lr0, sDCDC lr1, sDCDC lr2, sDCDC lr3, sDCDC lr4, sDCDC lr5, sDCDC automode);
  void SetLRS(ushort setting);
  void SetLRS(ushort tdrnum, sDCDC mode);
  void SetLRK(sDCDC lr0, sDCDC lr1, sDCDC lr2, sDCDC lr3, sDCDC lr4, sDCDC lr5, sDCDC automode);
  void SetLRK(ushort setting);
  void SetLRK(ushort tdrnum, sDCDC mode);

  void SetAutoModeTDR(sDCDC mode);
  void SetAutoModeLRS(sDCDC mode);
  void SetAutoModeLRK(sDCDC mode);
};


class TBS : public SlowControl {

 private:
  ushort Mode_LR;
  ushort LControl_LR, GControl_LR;
  ushort Voltage[4];
  ushort Current[12];

 public:
  TBS(AMSWcom *node, ushort amswaddr, sACTEL actel, sTBSaddr address);
  virtual ~TBS();
  //  ushort GetTBSId();
  ushort GetGControlLR() { return GControl_LR; }
  ushort GetActel();
  ushort GetLRState();
  ushort GetBiasVoltage(sTBSLR lr);
  void GetBiasVoltages();
  ushort BiasCurrent(sTBSTDR tdr);
  ushort GetBiasCurrent(sTBSTDR tdr);
  void GetBiasCurrents();
  ushort BiasVoltage(sTBSLR lr);
  sVOLT GetSetVoltage(sTBSLR);
  //void SetActelBrother(sActelMode brother);
  void SetVoltage(ushort setting);
  void SetVoltage(sVOLT h0, sVOLT h1, sVOLT c0, sVOLT c1);
  void SetVoltage(sVOLT lr0, sVOLT lr1); 
  void SetVoltage(ushort group, sVOLT volt);
  void SetLR(ushort setting);
  void SetLR(sDCDC h0, sDCDC h1, sDCDC c0, sDCDC c1);
  void SetLR(ushort group, sDCDC hot, sDCDC cold);
};

#endif /*SLOWCONTROL_H*/ 
