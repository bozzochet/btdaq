//  $Id: ecid.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef __AMSECID__
#define __AMSECID__
#include "typedefs.h"
#include "amsdbc.h"
#include <iostream.h>
#include "amsstl.h"
#include "commons.h"
#include "link.h"
#include "job.h"
#include "ecaldbc.h"
class AMSECIds{
 protected:
//  int _dead;
  integer _swid;//long sw_id=LTTPG=>superLayer|pmTube|Pixel|Gain(high/low)=1/2;
//                                     P=1-4->Anode,5->Dynode; Dyn.has only G=1(hi)
  int16u _slay;//0->8
  int16u _pmt;// 0->35(per 1sl)
  int16u _pixel;//sw pixel number(0->4, 0-3=>anodes, 4=>dynode)
  int16u _gainf;//gain_flag(0/1->high/low, dynode is always "high"=no_attenuation)
  int16u _swch;//GLOBAL sequential s/w-channel, numbering scheme:(L=0|TT=00|P=0|G=0),(0|00|0|1),
//(0|00|1|0),(0|00|1|1),...(0|00|3|1),(0|00|4|0){dyn only hi-gain, same sequence for (L=0/TT=01),...}
  integer _hwid;//CSSRRR=>Crate|Slot|Readout_channel
  int16u _pixelh;// hardware pixel number(0->4)
  int16u _crate;//0-1
  int16u _aside;//0-1, active side of the crate(no side-mixing is implied)
  int16u _slot;//sequential active(EDR2/ETRG) slot number (0-6)
  int16u _sltyp;//active(data) slot-type, 1/2->EDR2/ETRG
  int16u _rdch;//readout channel within slot(0-242)
  int16u _hwch;//sequential h/w-channels numbering: C(1)SS(1)RR(1),...C(1)SS(1)RR(Nmax for slot_1),
// C(1)SS(2)RR(1),...C(1)SS(2)RR(max for slot_2),..............
// C(Nmax)SS(max slots in this cr)RR(max for this slot)
//
  void _pixelh2s(); // hardware->software pixel_number conversion
  void _pixels2h(); // software->hardware .............
  static int16u _h2spixmap[2][5];//hw->sf pixel correspondence
  static integer _sidlst[ecalconst::ECRCMX];//swid-list(vs sequential hwch) 
  static integer _hidlst[ecalconst::ECRCMX];//hwid-list(vs sequential swch)
  static integer _hidls[ecalconst::ECRCMX];//hwid-list(vs sequential hwch)
  static int16u _sltymap[ecalconst::ECRT][ecalconst::ECSLOTS];//slot-types map in crates 
  static int16u _cardids[2][ecalconst::ECSLOTS];//card ids(link numbers, do not depend on crate) 
  static int16u _ochpsty[ecalconst::ECSTYPS];//outp.channels per slot-type
  static int16u _ichpsty[ecalconst::ECSTYPS];//inp.channels per slot-type
  static int16 _eibid[ecalconst::ECRT][ecalconst::ECEDRS][ecalconst::ECCONN];//EIB-id(name) map
 public:
  uinteger makesswid()const {return (_slay+1)*1000+(_pmt+1)*10+_pixel+1;}//short swid=LTTP
//  bool dead(){return _dead==1;}

  bool CHisBad(){return ECPMPeds::pmpeds[getslay()][getpmt()].HCHisBad(getpixel())
                 || ECcalib::ecpmcal[getslay()][getpmt()].HCHisBad(getpixel());}
		 
  bool HCHisBad(){return ECPMPeds::pmpeds[getslay()][getpmt()].HCHisBad(getpixel())
                 || ECcalib::ecpmcal[getslay()][getpmt()].HCHisBad(getpixel());}
		 
  bool LCHisBad(){return ECPMPeds::pmpeds[getslay()][getpmt()].LCHisBad(getpixel())
                 || ECcalib::ecpmcal[getslay()][getpmt()].LCHisBad(getpixel());}
		 
  bool DCHisBad(){return ECPMPeds::pmpeds[getslay()][getpmt()].DCHisBad()
                 || ECcalib::ecpmcal[getslay()][getpmt()].DCHisBad();}
		 
int16u getcrate(){return _crate;}
int16u getrdch(){return _rdch;}
int16u getslay(){return _slay;}
int16u getslot(){return _slot;}
int16u getlayer(){return _slay*2+_pixel/2;}//pixel-level layers numbering(0-17)
int16u getcell(){return _pmt*2+_pixel%2;}//pixel(cell) numbering within pixel-layer(0-71)
int16u getpmt(){return _pmt;}
int16u getpixel(){return _pixel;}
int16u getaside(){return _aside;}
integer getswid(){return _swid;}
integer gethwid(){return _hwid;}
int16u getswch(){return _swch;}
geant getped(int16u gain){return gain<2?ECPMPeds::pmpeds[getslay()][getpmt()].ped(getpixel(),gain):0;}
geant getsig(int16u gain){return gain<2?ECPMPeds::pmpeds[getslay()][getpmt()].sig(getpixel(),gain):0;}
integer getsta(int16u gain){return gain<2?ECPMPeds::pmpeds[getslay()][getpmt()].sta(getpixel(),gain):0;}
geant getpedd(){return ECPMPeds::pmpeds[getslay()][getpmt()].pedd();}
geant getsigd(){return ECPMPeds::pmpeds[getslay()][getpmt()].sigd();}
geant gethi2lowr(){return ECcalib::ecpmcal[getslay()][getpmt()].hi2lowr(getpixel());}
geant getan2dyr(){return ECcalib::ecpmcal[getslay()][getpmt()].an2dyr();}
geant getadc2mev(){return ECcalib::ecpmcal[getslay()][getpmt()].adc2mev();}
geant getgain(){return ECcalib::ecpmcal[getslay()][getpmt()].pmscgain(getpixel())*
             ECcalib::ecpmcal[getslay()][getpmt()].pmrgain();}
AMSECIds(){};
AMSECIds(int16u crate, int16u side, int16u slot, int16u rdch);//used for RD buildRaw
AMSECIds(int16u sl, int16u pmt, int16u pix, int16u gain, int16u sid);//for MC buildDaq(i.e. _aside(0))
AMSECIds(int lay, int cell);//used for 1d-cluster search  (i.e. _aside(0),_gainf(0))
AMSECIds(integer shortswid);//used for MC-buildRaw, EcalHit (i.e. _aside(0),_gainf(0)) 
static integer ncrates(){return ecalconst::ECRT;}
static void inittable();
static int16u hwseqn(int16u crat, int16u slot, int16u rdch);
static int16u swseqn(int16u slay, int16u tube, int16u pix, int16u gn);
static integer hw2swid(int16u crate, int16u slot, int16u rdch);
static int16 crdid2sl(int16u side, int16u crdid);
};


#endif
