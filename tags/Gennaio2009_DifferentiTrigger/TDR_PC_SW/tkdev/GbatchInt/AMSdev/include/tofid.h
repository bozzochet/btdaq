#ifndef __SC2ID__
#define __SC2ID__
#include "cern.h"
#include "commons.h"
#include "amsdbc.h"
#include "extC.h"
#include <string.h>
#include "typedefs.h"
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include "tofdbc02.h"
#include "antidbc02.h"
#include "job.h"
//
//
class AMSSCIds{
 protected:
  int _dummy;
  int _dtype;//Detector-type(1-TOF,2-ANTI)
  int _swid;//TOF software ID is LBBSPM(Layer|Bar|Side|Pmt|Meas_type)
//         or ANTI .............. BBSPM(LogicBar|Side|Pmt=0|Meas_type)
//                                                                        
  int16u _layer; //0,1,...
  int16u _bar;   //0,1,...
  int16u _side;  //0,1
  int16u _mtyp;  //Meas.type:0/1/2/3/4->LT_Time/Charge/FT_Time/SumHT_Time/SumSHT_time
                 // Mtyp=0-1 for physical paddles; Mtyp=2-4 for 2 fictitious extra paddles,
                 // representing odd(paddles 1,3,..)/even(paddles 2,4,..) half-planes 
  int16u _pmt; // 0->anode, 1-3 ->dynodes
  int16u _swch;//GLOBAL sequential s/w-channels numbering scheme:
// TOF=> for given BB/S=1 1st come all possible anode-mtypes(0,1,[2,3]),next come dynode-mtype(s)
// (1) repeated Ndyn-times(missing for half-pl); next - previous structure for S=2 of the same paddle. 
// Above structure is repeated for each BB of each L.
//
//
// ANTI=> for given BB(sector): BBS(1)P(0)M(0),BBS(1)P(0)M(1), BBS(2)P(0)M(0),BBS(2)P(0)M(1)
//        above structure is repeated for each BB.
//---
  int _hwid;//hardware ID is CSSRR(Crate|Slot|Readout_channel)
  int16u _crate; //0,1,,2,3
  int16u _slot;  //slot# 0,1,...6,7,8,9,10 (last 4 are for SFECs,fictitious)
  int16u _sltyp;  //slot(card) type : 1->SFET(8inp max)/2->SFEA(5inp)
//                                    3->SFEC(10inp max)/4->SDR/5->SPT...
//                    
//  int16u _crdid; //card id(may be different from slot#, = link number)
  int16u _rdch;  //readout channel(internal card-output numbering)
  int16u _inpch; //inp.channel(internal card-input numbering)
  int16u _hwch;//sequential h/w-channels numbering: C(1)SS(1)RR(1),...C(1)SS(1)RR(Nmax for slot_1),
// C(1)SS(2)RR(1),...C(1)SS(2)RR(max for slot_2),..............
// C(Nmax)SS(max slots in this cr)RR(max for this slot)
//--- 
  static int sidlst[TOF2GC::SCRCMX];//swid-list(vs hwch) 
  static int hidlst[TOF2GC::SCRCMX];//hwid-list(vs swch)
  static int hidls[TOF2GC::SCRCMX];//hwid-list(vs hwch)
  static int16u sltymap[TOF2GC::SCCRAT][TOF2GC::SCSLTM];//map of slot-types in crates 
//  static int16u cardids[TOF2GC::SCCRAT][TOF2GC::SCSLTM];
  static int16u cardidc[TOF2GC::SCCRAT][TOF2GC::SCSLTM];
  static int16u cardidt[TOF2GC::SCCRAT][TOF2GC::SCSLTM];
  static int16u ochpsty[TOF2GC::SCSLTY];//outp.channels per slot-type
  static int16u ichpsty[TOF2GC::SCSLTY];//inp.channels per slot-type
  static int totswch[2];//total s/w-channels for TOF, ANTI
  static int16u tmpsln[TOF2GC::SCSLTM];//map of temp-sensor numbers in slots
  static int trpatba[TOF2GC::SCCRAT][TOF2GC::SCMXBR2];//LBBS<->TrigPattBits assignment
 public:
  AMSSCIds():_dummy(1){};
  AMSSCIds(int16u crate, int16u slot, int16u rdch);//used for tof,anti
  AMSSCIds(int16u layer, int16u bar, int16u side, int16u pmt, int16u mtyp);//used for tof
  AMSSCIds(int16u bar, int16u side, int16u pmt, int16u mtyp);//used for anti
  AMSSCIds(int swid);//used for tof,anti
  static void inittable();
  static void selftest();
  bool dummy(){return _dummy==1;}
  bool AchOK(){return TOF2Brcal::scbrcal[_layer][_bar].AchOK(_side) &&
                      TOFBPeds::scbrped[_layer][_bar].PedAchOK(_side);}
  bool DchOK(){return TOF2Brcal::scbrcal[_layer][_bar].DchOK(_side,_pmt) &&
                      TOFBPeds::scbrped[_layer][_bar].PedDchOK(_side,_pmt);}
		      
		 
		 
  integer getshortid()const {return (_layer+1)*1000+(_bar+1)*10+_side+1;}
  int16u getswch(){return _swch;}
  int16u getcrate(){return _crate;}
  int16u getslot(){return _slot;}
  int16u getsltyp(){return _sltyp;}
//  int16u getcrdid(){return _crdid;}
  int16u getrdch(){return _rdch;}
  int16u getinpch(){return _inpch;}
  int16u getpmt(){return _pmt;}
  int16u getmtyp(){return _mtyp;}
  int16u gettempsn(){return tmpsln[_slot];}//1,...5, or "0" if not temper. slot(0,1,...)
  int gethwid(){return _hwid;}
  int getswid(){return _swid;}
  static int gettpbas(int16u crt, int16u bit);//return LBBS for bit bit(0-17) in crate crt(0-3)
  static int16u swseqn(int dt, int16u il, int16u ib, int16u is, int16u ip, int16u im);
  static int16u hwseqn(int16u cr, int16u sl, int16u ch);
  static int hw2swid(int16u cr, int16u sl, int16u ch);
//  static int16  crdid2sl(int16u crate, int16u crdid);
  static int16  crdidc2sl(int16u crate, int16u crdid);
  static int16  crdidt2sl(int16u crate, int16u crdid);
  static int16u sl2tsid(int16u slot){return tmpsln[slot];}//abs.slot#->temp.sensor id(sequential slot#)
  static int16u ich2rdch(int16u crate, int16u slot, int16u ich, int16u mtyp);
};


#endif
