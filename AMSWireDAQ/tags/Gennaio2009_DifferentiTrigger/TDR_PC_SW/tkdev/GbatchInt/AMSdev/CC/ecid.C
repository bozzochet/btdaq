#include "ecaldbc.h"
#include "ecid.h"
using namespace ecalconst;
integer AMSECIds::_sidlst[ECRCMX];//swid-list(vx sequential hwch)
integer AMSECIds::_hidlst[ECRCMX];//hwid-list(vs sequential swch)
integer AMSECIds::_hidls[ECRCMX];//hwid-list(vs sequential hwch)
//
//slot(card)-types map in the crates (only data-slots are considered !):
int16u AMSECIds::_sltymap[ECRT][ECSLOTS]={
                                         1,1,1,1,1,1,2, //crate-1
	                                 1,1,1,1,1,1,2  //crate-2
                                         };
//dataSlot(EDR/ETRG)-id(address) map in the crate for indiv.sides: 
int16u AMSECIds::_cardids[2][ECSLOTS]={
                                            0,4,8,12,16,20,22, //side-a
                                            1,5,9,13,17,21,23, //side-b
                                            };
// output channels(rdch's) vs slot(card) type: 
int16u AMSECIds::_ochpsty[ECSTYPS]={243,7};//EDR2,ETRG
//
//EIBid-names map(id=+-Face|Half|sLayer=>F=1/2/3/4->A/B/C/D;H=1/2->-X(Y)/+X(Y);
//L=0-8; in_front +/- => (PMT looking in +X(Y) direction)/opposit
int16 AMSECIds::_eibid[ECRT][ECEDRS][ECCONN]={
                                             -221, 120,-322,  // cr-1, EDR-1, conn.0-2
					     -211,-320, 122,  //  ...  EDR-2
					     -223, 128,-324,  //  ...  EDR-3
					     -213,-328, 124,  //  ...  EDR-4
					     -225,-217,-326,  //  ...  EDR-5
					     -215,-227, 126,  //  ...  EDR-6
					      411,-310, 112,  // cr-2  EDR-1, conn.0-2
					      421, 110,-312,  //  ...  EDR-2
					      413,-318, 114,  //  ...  EDR-3
					      423, 118,-314,  //  ...  EDR-4
					      415, 427, 116,  //  ...  EDR-5
					      425, 417,-316   //  ...  EDR-6
					     };
// PMT HW->SW pixel mapping for faces A,B and C,D : 
int16u AMSECIds::_h2spixmap[2][5]={
                                  3,2,0,1,4,  //face A,B
				  2,3,1,0,4   //face C,D
                                 };
//----------
//                            0-1            0-1           0-5          0-242
AMSECIds::AMSECIds(int16u crate, int16u side, int16u slot, int16u rdch):
                        _crate(crate),_aside(side),_slot(slot),_rdch(rdch){//used in RD buildRaw
//cout<<"===> InConstr:crat/side/slot/rdch="<<crate<<" "<<side<<" "<<slot<<" "<<rdch<<endl;
 _hwch=hwseqn(crate,slot,rdch);//0->2915
//cout<<" hwch="<<_hwch<<endl;
 _swid=_sidlst[_hwch];//LTTPG(slayer|tube|pixel|gain)
 _slay=_swid/10000;
 _slay=_slay-1;//0-8
 _pmt=(_swid%10000)/100-1;//0-35
 _pixel=(_swid%100)/10;
 _pixel=_pixel-1;//0-4 (4->dynode)
 _gainf=(_swid%10)-1;//0/1->high/low
//cout<<" swid="<<_swid<<" sl/pmt/pix/gn="<<_slay<<" "<<_pmt<<" "<<_pixel<<" "<<_gainf<<endl;
 _swch=swseqn(_slay,_pmt,_pixel,_gainf);//sw sequential number(0->2915)
//cout<<" swch="<<_swch<<endl;
 _hwid=(_crate+1)*100000+(_slot+1)*1000+(_rdch+1);//CSSRRR(crate|slot|readout_ch)
 _sltyp=_sltymap[_crate][_slot];//1-2=>EDR2/ETRG
//cout<<" hwid/sltyp="<<_hwid<<_sltyp<<endl;
//cout<<"    leave constr...."<<endl;
}
//-----------
//                            LTTP
AMSECIds::AMSECIds(integer sswid):_aside(0),_gainf(0){//used in MC buildRaw
  _slay=sswid/1000-1;
  _pmt=(sswid%1000)/10-1;
  _pixel=sswid%10-1;//0-4 (4->dynode)
  _swid=sswid*10+1;//LTTPG(_gainf=1=anode_hi)
  _swch=swseqn(_slay,_pmt,_pixel,_gainf);//sw sequential number(0->2915)
  _hwid=_hidlst[_swch];//CSSRRR
  _crate=(_hwid/100000)-1;//0-1
  _slot=(_hwid%100000)/1000-1;//0-5
  _rdch=(_hwid%1000)-1;//0-242
  _sltyp=_sltymap[_crate][_slot];//1-2=>EDR2/ETRG(really only 1, because _slot<=5)
  _hwch=hwseqn(_crate,_slot,_rdch);//0->2915
}
//-----------
//                            0-17            0-71  
AMSECIds::AMSECIds(integer layer, integer cell):_aside(0),_gainf(0){//used in 1D-Cluster search
//                        
  _slay=layer/2;//0-8
  _pmt=cell/2;//0-35
  if(_slay<ECSLMX && _pmt<ECPMSMX){
   _pixel=2*(layer%2)+cell%2;//0-3
   _swid=(_slay+1)*10000+(_pmt+1)*100+(_pixel+1)*10+_gainf+1;//swid=LTTPG
   _swch=swseqn(_slay,_pmt,_pixel,_gainf);//sw sequential number(0->...2915)
   _hwid=_hidlst[_swch];//CSSRRR
   _crate=(_hwid/100000)-1;//0-1
   _slot=(_hwid%100000)/1000-1;//0-5
   _rdch=(_hwid%1000)-1;//0-242
   _sltyp=_sltymap[_crate][_slot];//1-2=>EDR2/ETRG(really only 1, because _slot<=5)
   _hwch=hwseqn(_crate,_slot,_rdch);//0->2915
  }
  else{
   cerr<<"AMSECIds::AMSECIds-S-ParametersOutOfBound "<<layer<<" "<<cell<<endl;
//  _dead=1;
  }
}
//-----------
// constructor for buildDAQ:
//                          0-8         0-35          0-4          0-1
AMSECIds::AMSECIds(int16u sl, int16u pmt, int16u pix, int16u gnf, int16u side):
                                        _aside(side),_slay(sl),_pmt(pmt),_pixel(pix),_gainf(gnf){
//
  _swid=(_slay+1)*10000+(_pmt+1)*100+(_pixel+1)*10+_gainf+1;//swid=LTTPG
  _swch=swseqn(_slay,_pmt,_pixel,_gainf);//sw sequential number(0->...2915)
  _hwid=_hidlst[_swch];//CSSRRR
  _crate=(_hwid/100000)-1;//0-1
  _slot=(_hwid%100000)/1000-1;//0-5
  _rdch=(_hwid%1000)-1;//0-242
  _sltyp=_sltymap[_crate][_slot];//1-2=>EDR2/ETRG(really only 1, because _slot<=5)
  _hwch=hwseqn(_crate,_slot,_rdch);//0->2915
}
//--------------
//                                 0-1           0,1,4,5,...
int16 AMSECIds::crdid2sl(int16u side, int16u crdid){
//card(slot)_id(node_addr(link#) to sequential slot# vs side(known from JINF_id)
//                                             slot#=0-5 =>EDRs; =6 =>ETRG
  for(int i=0;i<ECSLOTS;i++){
    if(_cardids[side][i]==crdid)return(i);
  }
  return(-1);//if illegal(not existing) crdid
}
//----------
void AMSECIds::_pixelh2s(){
}
//----------
void AMSECIds::_pixels2h(){//my(softw)->hw pixel_number conversion
}
//----------
//
void AMSECIds::inittable(){
//
 int16u crat,edr,rdch,slay,conn,cpmt,epmt,hpix,spix;
 int16u gnfl,face,half;
 int16 eibid,fdir,fhal;
 integer swid,hwid;
//
 int16u hwch(0),swch(0);
//cout<<"---------> create tables:"<<endl;
 for(crat=0;crat<ECRT;crat++){//crates(JINFs) loop
//   cout<<"  crate="<<crat<<endl;
   for(edr=0;edr<ECEDRS;edr++){//EDRs loop
//     cout<<"     edr="<<edr<<endl;
     for(rdch=0;rdch<ECEDRC;rdch++){//EDR_channels loop
//       cout<<"       rdch="<<rdch<<endl;
       conn=2-(rdch/9)%3;//EDR->EIB connector#(2-0)
       eibid=_eibid[crat][edr][conn];//FHL(face|half|lay)
       fdir=1;
       if(eibid<0)fdir=-1;//pm_readout direction for given face (+1/-1 -> along/oppos X(Y))
       eibid=abs(eibid);
//       cout<<"         conn/eibid/fdir="<<conn<<" "<<eibid<<" "<<fdir<<endl;
       face=eibid/100;//face#(1-4 => A-D)
       half=(eibid%100)/10;//face's half#(1/2 -> -/+)
       fhal=2*half-3;//same but -1/+1
//       cout<<"         face/half/fhal="<<face<<" "<<half<<" "<<fhal<<endl;
       slay=eibid%10;//slay#(0-8)
       epmt=8-(rdch%9);//EIB pmt#(8-0)
       hpix=rdch/54;//hw pixel#(0-3 for anodes, 4 for dynode)
       if(hpix<4)gnfl=1-((rdch/27)%2);//gain flag (0/1 -> hi/low, dynode only hi=no_atten)
       else gnfl=0;//dynode only hi
//       cout<<"         slay/epmt/hpix/gnfl="<<slay<<" "<<epmt<<" "<<hpix<<" "<<gnfl<<endl;
       if(face<=2){//A/B
         spix=_h2spixmap[0][hpix];//0->3,1->2,2->0,3->1
	 if(fhal<0)cpmt=2*epmt;//abs(column) pmt#(0,2,4,...16)
	 else cpmt=2*(8-epmt)+18;//abs(column) pmt#(18,20,...34) 
       }
       else{//C/D
         spix=_h2spixmap[1][hpix];//0->2,1->3,2->1,3->0
	 if(fhal<0)cpmt=2*epmt+1;//abs(column) pmt#(1,3,5,...17)
	 else cpmt=2*(8-epmt)+19;//abs(column) pmt#(19,21,23,...35) 
       }
//       cout<<"         cpmt/spix="<<cpmt<<spix<<endl;
       swid=(slay+1)*10000+(cpmt+1)*100+(spix+1)*10+gnfl+1;//swid=LTTPG
//
       swch=slay*ECPMMX*(4*2+1)+cpmt*(4*2+1)+spix*2;//global sequential sw-channel#(0-2915)
       if(spix<4)swch+=gnfl;
//       cout<<"         swid/swch="<<swid<<" "<<swch<<" hwch="<<hwch<<endl;
// 
       hwid=(crat+1)*100000+(edr+1)*1000+(rdch+1);//CSSRRR(crate|slot|readout_ch)
       _sidlst[hwch]=swid;//hwch->swid
       _hidlst[swch]=hwid;//inverse table, i.e.swch->hwid
       _hidls[hwch]=hwid;//hwch->hwid
       hwch+=1;
     }//-->endof EDR_channels-loop
   }//-->endof EDRs-loop
 }//-->endof crates-loop
//
 cout<<"AMSECIds::init: total hw-channels="<<hwch<<endl;
}
//-----------------
int16u AMSECIds::hwseqn(int16u crate, int16u slot, int16u rdch){
//crate=0,1,...;slot(edr)=0,1,...5;rdch=0,1,...242
// calc. global sequential hw-channel(0,1,...242;no empty chan !!):
  int16u hwch(0),slt;
//
  assert(crate<ECRT && slot<ECEDRS && rdch<ECEDRC);
//  for(int cr=0;cr<crate;cr++){//1st->count in crates befor current one
//    for(int sl=0;sl<ECEDRS;sl++){
//      slt=_sltymap[cr][sl];
//      hwch+=_ochpsty[slt-1];
//    }
//  }
//  for(int sl=0;sl<slot;sl++){//2nd->count in slots befor current one(in crate crate)
//    slt=_sltymap[crate][sl];
//    hwch+=_ochpsty[slt-1];
//  }
//  hwch+=rdch;//3rd->count in current slot
  hwch=crate*ECEDRS*ECEDRC+slot*ECEDRC+rdch;//simple formula due to the same ch/edr, no empty
  return hwch;//0,1,...2915
}
//----------------- 
int16u AMSECIds::swseqn(int16u slay, int16u tube, int16u pix, int16u gn){
//                                0-8           0-35          0-4          0-1
  int16u swch(0);//global sequential sw-channel(incl gainf)=0,1,...2915(no empty chan !!)
    assert(slay<ECSLMX && tube<ECPMMX && pix<5);
    if(pix==4)assert(gn==0);//dynode only with high(no attenuation)-gain
    swch=slay*ECPMMX*(4*2+1)+tube*(4*2+1)+pix*2;
    if(pix<4)swch+=gn;//if anode - count gains befor current one
    return swch;//0,1,...2915
}
//-----------------
integer AMSECIds::hw2swid(int16u crate, int16u slot, int16u rdch){
//crate=0,1,...;slot=0,1,...;rdch=0,1,...
// transform h/w-id(address) into s/w-id:
  int16u hwch;
  integer swid;
  hwch=hwseqn(crate,slot,rdch);
  swid=_sidlst[hwch];
  return swid;
//
}
