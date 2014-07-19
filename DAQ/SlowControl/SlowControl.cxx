#include <string.h>
#include "SlowControl.h"
#include "amswcom.h"


sTPSFEaddr& operator ++(sTPSFEaddr& addr, int) {
  if (addr==kTPSFE3) return addr=kTPSFE0;
  int temp=addr;
  return addr=static_cast<sTPSFEaddr> (++temp);
}


sTBSaddr& operator ++(sTBSaddr& addr, int) {
  if (addr==kTBS1) return addr=kTBS0;
  int temp=addr;
  return addr=static_cast<sTBSaddr> (++temp);
}


sTBSTDR& operator ++(sTBSTDR& addr, int) {
  if (addr==kTDR11) return addr=kTDR0;
  int temp=addr;
  return addr=static_cast<sTBSTDR> (++temp);
}

sTBSLR& operator ++(sTBSLR& addr, int) {
  if (addr==k1Cold) return addr=k0Hot;
  int temp=addr;
  return addr=static_cast<sTBSLR> (++temp);
}

//------------------------- SlowControl -------------------------//


SlowControl::SlowControl(AMSWcom *node, ushort addr) {
  //  Node=node;
  Bus=0;
  Debug=0;
  ID=0;
  Amswire=node;
  //  cout << "amsw pointers: " << node << " " << Amswire << endl;
  AMSWaddr=addr;
}


SlowControl::~SlowControl() {

}


void SlowControl::SetTrackerBus() {

  if (GetAddress()==3) Bus=0x40+(Half<<5)+(Actel<<4); // S9011...
  else {
    Bus=(Actel<<5);
    if (GetAddress()>10) Bus+=0x10;
  }
}


// void SlowControl::SlowControl(AMSWcom *node) {
//   Node=node;
// }



void SlowControl::SetRegister(ushort r) {
  // in case we need 4 or 5 bits for the register address, we cannot use bit 4, as it is used
  // to indicate if the command is READ or WRITE, thus we use bits 5 and 6 for the address.
  r &= 0x1f; // we keep the 5 last bits
  Register=(r & 7)+((r & 0x18)<<1);
}


ushort SlowControl::GetRegister() {
  ushort r;
  r=Register & 7;
  r+=((Register & 0x30)>>1);

  return r;
}


void SlowControl::BuildCommand(sCommandMode mode, ushort data) {
  //  if (mode) mode=8; // write
  Command=0xa000+Register+Address+(mode<<3);
  DoParity(&Command, data);
}


void SlowControl::DoParity(ushort *x, ushort data) {
  int parity=1;
  //  ushort val=*x;
  for (int cnt=0; cnt<16; cnt++) parity+=((*x & (1<<cnt))!=0)+((data & (1<<cnt))!=0);
  if (parity & 1) *x|=0x4000;
}


ushort SlowControl::ReadRegister(ushort r) {
  SetRegister(r);
  BuildCommand(kRead,0);
  if (Debug) printf("command: 0x%04x 0x%04X 0x%04X\n", Bus, Command,0); 
  uint res=(Amswire->LecroyRead(AMSWaddr, Bus,Command,0));
  ushort val=(res & 0xffff);
  ErrLecroy=res>>16;
  ErrAmsw=Amswire->GetRX_DONE();
  return val;
}


void SlowControl::ReadRegister(ushort r, uint freq, QList *qlist) {
  SetRegister(r);
  BuildCommand(kRead,0);
  if (Debug) printf("command: 0x%04x 0x%04X 0x%04X\n", Bus, Command,0); 

  qlist->AddCommand(Bus,freq, Command,0);
  //uint res=(Amswire->LecroyRead(AMSWaddr, Bus,Command,0));
  //ushort val=(res & 0xffff);
  //ErrLecroy=res>>16;
  //ErrAmsw=Amswire->GetRX_DONE();
  //return val;
}


ushort SlowControl::WriteRegister(ushort r, ushort data) {
  SetRegister(r);
  BuildCommand(kWrite, data);
  if (Debug) printf("command: 0x%04x 0x%04X 0x%04X\n", Bus, Command, data); 
  uint res=(Amswire->LecroyRead(AMSWaddr, Bus,Command,data));
  ushort val=(res & 0xffff);
  ErrLecroy=res>>16;
  ErrAmsw=Amswire->GetRX_DONE();
  return val; // as a return value we get the read register value after execution of the command
}


void SlowControl::WriteRegister(ushort r, ushort data, uint freq, QList *qlist) {
  SetRegister(r);
  BuildCommand(kWrite, data);
  if (Debug) printf("command: 0x%04x 0x%04X 0x%04X\n", Bus, Command, data); 

  qlist->AddCommand(Bus,freq,Command,data);
  //uint res=(Amswire->LecroyRead(AMSWaddr, Bus,Command,data));
  //ushort val=(res & 0xffff);
  //ErrLecroy=res>>16;
  //ErrAmsw=Amswire->GetRX_DONE();
  //return val; // as a return value we get the read register value after execution of the command
}



void SlowControl::SetActelBrother(sActelMode mode) {
  ushort setting=mode;
  if (!strcmp(CardType,"TPSFE")) setting<<=14;
  WriteRegister(0,setting);
}


void SlowControl::InitLecroy() {
  //printf("Bus %x: init Lecroy...\n", Bus);
  Amswire->LecroyWrite(AMSWaddr,Bus);
  
}


//-------------------------- S9011AT ----------------------------//

S9011AT::S9011AT(AMSWcom *node, ushort amswaddr, sACTEL actel, ushort half) : SlowControl(node, amswaddr) {
  SetCardType("S9011AT");
  SetAddress(3);   // card address set to 3 (TBC)

  // !!!!!!!! TEMPORARY to correct an error in TPD QM !!!!!!!
  //  if (actel==kActelHot) actel=kActelCold;
  //  else if (actel==kActelCold) actel=kActelHot;

  SetActelId(actel); // kActelHot or kActelCold
  SetHalf(half);
  SetTrackerBus();
}


S9011AT::~S9011AT() {};


// void S9011AT::SetActelBrother(sActelMode mode) {
//   WriteRegister(0,mode);
// };


void S9011AT::SetS9051(ushort mode) {
  WriteRegister(1,mode);
}


ushort S9011AT::Convert(sDCDCmode mode) {
  if (mode==kAonBon) return (kClear<<2)+kClear; // 0101=5 
  if (mode==kAonBoff) return (kSet<<2)+kClear;  // 1001=9
  if (mode==kAoffBon) return (kClear<<2)+kSet;  // 0110=6
  if (mode==kAoffBoff) return (kSet<<2)+kSet;   // 1010=10
  return 0;
}

ushort S9011AT::ConvertS9053(sDCDCmode mode) {
  if (mode==kAonBon) return (kSet<<2)+kClear;    // 1001=9  
  if (mode==kAonBoff) return (kClear<<2)+kClear; // 0101=5 
  if (mode==kAoffBon) return (kSet<<2)+kSet;     // 1010=10
  if (mode==kAoffBoff) return (kClear<<2)+kSet;  // 0110=6
  return 0;
}

void S9011AT::SetS9051(sDCDCmode dc0, sDCDCmode dc1, sDCDCmode dc2, sDCDCmode dc3) {
  ushort command=Convert(dc0)+(Convert(dc1)<<4)+(Convert(dc2)<<8)+(Convert(dc3)<<12);
  SetS9051(command);
}

void S9011AT::SetS9051(ushort group, ushort side, sDCDC mode) {
  if (group>3 || side>1) return;
  ushort command=(mode<<(2*(group*2+side)));
  SetS9051(command);
}


void S9011AT::SetS9053(sDCDCmode dc0, sDCDCmode dc1) {  
  ushort command=ConvertS9053(dc0)+(ConvertS9053(dc1)<<4);
  WriteRegister(2,command);
}


void S9011AT::SetS9053(ushort group, sDCDCmode dc) {
  if (group>1) return;

  ushort command=ConvertS9053(dc);
  if (group) command=command<<4;

  WriteRegister(2,command);
}


void S9011AT::SetS9053(ushort group, ushort side, sDCDC mode) {
  if (group>1 || side>1) return;

  if (side==1) {
    // taking into account the convention inversion for the cold circuits of s9053
    if (mode==kOn) mode=kOff;
    else if (mode==kOff) mode=kOn;
  }

  ushort command=(mode<<(2*(group*2+side)));

  WriteRegister(2,command);
}



void S9011AT::SetS9055(sDCDCmode dc0, sDCDCmode dc1) {
  ushort command=(Convert(dc0)<<8)+(Convert(dc1)<<12);
  WriteRegister(2,command);
}

void S9011AT::SetS9055(ushort group, ushort side, sDCDC mode) {
  if (group>1 || side>1) return;
  ushort command=(mode<<(2*(group*2+side)));
  command<<=8;
  WriteRegister(2,command);
}


ushort S9011AT::GetActel() {
  ushort res=ReadRegister(0);
  Control_Brother=res & 1;
  Trip_Brother=(res>>13) & 1;
  Status_Brother=(res>>14) & 1;
  PowerOn=(res>>15);
  return res;
}


ushort S9011AT::GetS9051() {
  ushort res=ReadRegister(1);
  LControl_S9051=0xff & res;
  Status_S9051=(res>>8);
  return res;
}


ushort S9011AT::GetS905355() {
  ushort res=ReadRegister(2);
  LControl_S9053=0xf & res;
  LControl_S9055=(0xf0 & res)>>4;
  Status_S9053=(0xf00 & res)>>8;
  Status_S9055=(0xf000 & res)>>12;
  return res;
}


ushort S9011AT::GetGlobalControl() {
  ushort res=ReadRegister(3);
  GControl_S9051=res & 0xff;
  GControl_S9053=(res & 0xf00)>>8;
  GControl_S9055=(res & 0xf000)>>12;
  return res;
}


ushort S9011AT::GetTripStatus() {
  ushort res=ReadRegister(4);
  Trip_S9051=res & 0xff;
  Trip_S9053=(res & 0xf00)>>8;
  Trip_S9055=(res & 0xf000)>>12;
  return res;
}


//---------------------- TPSFE ---------------------------------//

TPSFE::TPSFE(AMSWcom *node, ushort amswaddr, sACTEL actel, sTPSFEaddr address) : SlowControl(node,amswaddr) {
  ushort addr=0;

  //  memset(TDRstatus,0,sizeof(TDRstatus));
  //  memset(LRSstatus,0,sizeof(LRSstatus));
  //  memset(LRKstatus,0,sizeof(LRKstatus));

  SetCardType("TPSFE");

  if (address==kTPSFE0) addr=4;
  if (address==kTPSFE1) addr=6;
  if (address==kTPSFE2) addr=14;
  if (address==kTPSFE3) addr=16;

  SetID(address);
  SetAddress(addr);
  SetActelId(actel); // kActelHot or kActelCold
  SetTrackerBus();

  memset(CntTDR,0,sizeof(CntTDR));
  memset(CntLRS,0,sizeof(CntLRS));
  memset(CntLRK,0,sizeof(CntLRK));
  CntActel=0;
  AutoMode_TDR=0; AutoMode_LRS=0; AutoMode_LRK=0;
  Status_TDR=0; Status_LRS=0; Status_LRK=0;
  LControl_TDR=0; LControl_LRS=0; LControl_LRK=0;
  GControl_LRS=0; GControl_LRK=0;
}


TPSFE::~TPSFE() {};



ushort TPSFE::GetTPSFEId() {
  ushort addr=GetAddress();
  ushort ret=0xffff;
  if (addr==4) ret=kTPSFE0;
  if (addr==6) ret=kTPSFE1;
  if (addr==14) ret=kTPSFE2;
  if (addr==16) ret=kTPSFE3;
  return ret;
}

void TPSFE::FillStatLCtrl(ushort res, ushort *lctrl, ushort *automode, ushort *status) {
  *lctrl=res & 0x3f;
  *automode=(res>>6) & 1;
  *status=(res>>8) & 0x3f;
}


ushort TPSFE::GetActel() {
  ushort res=ReadRegister(0);

  FillStatLCtrl(res, &LControl_TDR, &AutoMode_TDR, &Status_TDR);
  Control_Brother=(res>>7) & 1;
  Status_Brother=(res>>14) & 1;
  PowerOn=(res>>15);
  return res;
}


ushort TPSFE::GetLRS() {
  ushort res=ReadRegister(1);
  FillStatLCtrl(res, &LControl_LRS, &AutoMode_LRS, &Status_LRS);
  return res;
}


ushort TPSFE::GetLRK() {
  ushort res=ReadRegister(2);
  FillStatLCtrl(res, &LControl_LRK, &AutoMode_LRK, &Status_LRK);
  return res;
}


ushort TPSFE::GetGControl() {
  ushort res=ReadRegister(3);
  GControl_LRS=res & 0x3f;
  GControl_LRK=(res>>8) & 0x3f;
  return res;
}


void TPSFE::GetTripCounters() {

  ushort cnt=0;
  cnt=ReadRegister(4);
  CntTDR[0]=cnt & 0xf;
  CntTDR[1]=(cnt>>4) & 0xf;
  CntTDR[2]=(cnt>>8) & 0xf;
  CntTDR[3]=cnt>>12;

  cnt=ReadRegister(5);  
  CntTDR[4]=cnt & 0xf;
  CntTDR[5]=(cnt>>4) & 0xf;
  CntLRS[0]=(cnt>>8) & 0xf;
  CntLRS[1]=cnt>>12;

  cnt=ReadRegister(6);
  CntLRS[2]=cnt & 0xf;
  CntLRS[3]=(cnt>>4) & 0xf;
  CntLRS[4]=(cnt>>8) & 0xf;
  CntLRS[5]=cnt>>12;

  cnt=ReadRegister(7);
  CntLRK[0]=cnt & 0xf;
  CntLRK[1]=(cnt>>4) & 0xf;
  CntLRK[2]=(cnt>>8) & 0xf;
  CntLRK[3]=cnt>>12;

  cnt=ReadRegister(8);
  CntLRK[4]=cnt & 0xf;
  CntLRK[5]=(cnt>>4) & 0xf;
  CntActel=(cnt>>8) & 0xf;

  Trip_Brother=(CntActel>0);
}


ushort TPSFE::GetCntTDR(ushort tdr) {
  if (tdr>5) tdr=5;
  return CntTDR[tdr];
}

ushort TPSFE::GetCntLRS(ushort tdr) {
  if (tdr>5) tdr=5;
  return CntLRS[tdr];
}

ushort TPSFE::GetCntLRK(ushort tdr) {
  if (tdr>5) tdr=5;
  return CntLRK[tdr];
}

// void TPSFE::SetActelBrother(sActelMode brother) {
//   ushort setting=brother<<14;
//   SetTDR(setting); // register 0 also stores the actel brother command.
// }


void TPSFE::SetTDR(sDCDC tdr0, sDCDC tdr1, sDCDC tdr2, sDCDC tdr3, sDCDC tdr4, sDCDC tdr5, sDCDC automode) {
  if (automode==kOn) automode=kOff;
  else if (automode==kOff) automode=kOn;

  ushort setting=tdr0+(tdr1<<2)+(tdr2<<4)+(tdr3<<6)+(tdr4<<8)+(tdr5<<10)+(automode<<12);
  SetTDR(setting);
}


void TPSFE::SetTDR(ushort tdrnum, sDCDC mode) {
  if (tdrnum>5) return;
  ushort setting=(mode<<(2*tdrnum));
  SetTDR(setting);
}


void TPSFE::SetTDR(ushort setting) {
  WriteRegister(0,setting);
}


void TPSFE::SetLRS(sDCDC lr0, sDCDC lr1, sDCDC lr2, sDCDC lr3, sDCDC lr4, sDCDC lr5, sDCDC automode) {
  if (automode==kOn) automode=kOff;
  else if (automode==kOff) automode=kOn;

  ushort setting=lr0+(lr1<<2)+(lr2<<4)+(lr3<<6)+(lr4<<8)+(lr5<<10)+(automode<<12);
  SetLRS(setting);
}


void TPSFE::SetLRS(ushort tdrnum, sDCDC mode) {
  if (tdrnum>5) return;
  ushort setting=(mode<<(2*tdrnum));
  SetLRS(setting);
}


void TPSFE::SetLRS(ushort setting) {
  WriteRegister(1,setting);
}


void TPSFE::SetLRK(sDCDC lr0, sDCDC lr1, sDCDC lr2, sDCDC lr3, sDCDC lr4, sDCDC lr5, sDCDC automode) {
  if (automode==kOn) automode=kOff;
  else if (automode==kOff) automode=kOn;

  ushort setting=lr0+(lr1<<2)+(lr2<<4)+(lr3<<6)+(lr4<<8)+(lr5<<10)+(automode<<12);
  SetLRK(setting);
}


void TPSFE::SetLRK(ushort tdrnum, sDCDC mode) {
  if (tdrnum>5) return;
  ushort setting=(mode<<(2*tdrnum));
  SetLRK(setting);
}


void TPSFE::SetLRK(ushort setting) {
  WriteRegister(2,setting);
}


void TPSFE::SetAutoModeTDR(sDCDC automode) {
  SetTDR(kNoDCDCChange0,kNoDCDCChange0,kNoDCDCChange0,kNoDCDCChange0,kNoDCDCChange0,kNoDCDCChange0, automode);
} 

void TPSFE::SetAutoModeLRS(sDCDC automode) {
 SetLRS(kNoDCDCChange0,kNoDCDCChange0,kNoDCDCChange0,kNoDCDCChange0,kNoDCDCChange0,kNoDCDCChange0, automode);
} 

void TPSFE::SetAutoModeLRK(sDCDC automode) {
  SetLRK(kNoDCDCChange0,kNoDCDCChange0,kNoDCDCChange0,kNoDCDCChange0,kNoDCDCChange0,kNoDCDCChange0, automode);
} 

//---------------------- TBS ----------------------//

TBS::TBS(AMSWcom *node, ushort amswaddr, sACTEL actel, sTBSaddr address) : SlowControl(node,amswaddr) {

  SetCardType("TBS");

  memset(Voltage,0,sizeof(Voltage));
  memset(Current,0,sizeof(Current));

  ushort addr=0xffff;
  if (address==kTBS0) addr=5;
  if (address==kTBS1) addr=15;

  SetID(address);
  SetAddress(addr);
  SetActelId(actel); // kActelHot or kActelCold
  SetTrackerBus();
}


TBS::~TBS() {}


ushort TBS::GetActel() {
  ushort res=ReadRegister(0);

  Mode_LR=(res>>8) & 3;

  Trip_Brother=(res>>13) & 1;
  Control_Brother=res & 1;
  Status_Brother=(res>>14) & 1;
  PowerOn=(res>>15);
  return res;
}

// ushort TBS::GetTBSId() {
//   ushort addr=GetAddress();
//   ushort ret=0xffff;
//   if (addr==5) ret=kTBS0;
//   if (addr==15) ret=kTBS1;
//   return ret;
// }

ushort TBS::GetLRState() {
  ushort res=ReadRegister(1);

  LControl_LR=res & 0xff;
  GControl_LR=(res>>8);
  return res;
}


ushort TBS::GetBiasVoltage(sTBSLR lr) {
  Voltage[lr]=ReadRegister(0x10+lr);
  return Voltage[lr];
}


void TBS::GetBiasVoltages() {
  for (sTBSLR lr=k0Hot; lr<=k1Cold; lr++) {
    GetBiasVoltage(lr);
    if (lr==k1Cold) break;
  }
}


ushort TBS::BiasVoltage(sTBSLR lr) {
  return Voltage[lr];
}


ushort TBS::GetBiasCurrent(sTBSTDR tdr) {
  Current[tdr]=ReadRegister(0x14+tdr);
  return Current[tdr];
}


void TBS::GetBiasCurrents() {
  for (sTBSTDR tdr=kTDR0; tdr<=kTDR11; tdr++) {
    GetBiasCurrent(tdr);
    if (tdr==kTDR11) break;
  }
}


ushort TBS::BiasCurrent(sTBSTDR tdr) {
  return Current[tdr];
}

// void TBS::SetActelBrother(sActelMode brother) {
//   ushort setting=brother;
//   WriteRegister(0,setting);
// }


void TBS::SetVoltage(ushort setting) {
  setting<<=8;
  WriteRegister(1,setting);
}


void TBS::SetVoltage(sVOLT h0, sVOLT h1, sVOLT c0, sVOLT c1) {
  // k80V, k60V
  ushort setting=h0+(h1<<2)+(c0<<4)+(c1<<6);
  SetVoltage(setting);
}


void TBS::SetVoltage(sVOLT lr0, sVOLT lr1) {
  SetVoltage(lr0, lr1, lr0, lr1);
}


void TBS::SetVoltage(ushort group, sVOLT volt) {
  if (group>1) return;
  SetVoltage((group)?kNoVolt0:volt, (group)?volt:kNoVolt0);
}


void TBS::SetLR(ushort setting) {
  WriteRegister(1,setting);
}


void TBS::SetLR(sDCDC h0, sDCDC h1, sDCDC c0, sDCDC c1) {
  // kOff, kOn
  ushort setting=h0+(h1<<2)+(c0<<4)+(c1<<6);
  SetLR(setting);
}


void TBS::SetLR(ushort group, sDCDC hot, sDCDC cold) {
  if (group>1) return;
  if (group) SetLR(kNoDCDCChange0, hot, kNoDCDCChange0, cold);
  else SetLR(hot, kNoDCDCChange0, cold, kNoDCDCChange0);
}


sVOLT TBS::GetSetVoltage(sTBSLR lr) {
  ushort volt=GControl_LR>>4;
  volt>>=lr;
  volt&=1;
  return (volt)?k60V:k80V;
}
