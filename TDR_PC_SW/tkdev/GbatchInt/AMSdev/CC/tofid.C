#include "tofid.h"
using namespace TOF2GC;
using namespace ANTI2C;
  int AMSSCIds::totswch[2];//total s/w-channels for TOF and ANTI
  int AMSSCIds::sidlst[SCRCMX];//swid-list(vs hwch) 
  int AMSSCIds::hidlst[SCRCMX];//hwid-list(vs swch)
  int AMSSCIds::hidls[SCRCMX];//hwid-list(vs hwch)
//outp.channels for each slot(card) type: 
  int16u AMSSCIds::ochpsty[TOF2GC::SCSLTY]={13,11,10,0,0};
  int16u AMSSCIds::ichpsty[TOF2GC::SCSLTY]={8,6,10,0,0};
//slot(card)-type map in crates  
  int16u AMSSCIds::sltymap[SCCRAT][SCSLTM]={4,1,1,5,1,1,2,3,3,3,3,  //top cr-1
                                            4,1,1,5,1,1,2,3,3,3,3,  //top cr-2
					    4,1,1,5,1,1,2,3,3,3,3,  //bot cr-3
					    4,1,1,5,1,1,2,3,3,3,3   //bot cr-4
					    };
//map of temp-sensors ids in crate's slots:
  int16u AMSSCIds::tmpsln[SCSLTM]={0,1,2,0,3,4,5,0,0,0,0};// same for all crates(id=sensor_seq.number)
//  (because each SFET(A) has 1 temp-sensor, above array gives also SFET(A) sequential number =1,2,3,4,5)
//----
// sequence of cards in crate: SDR, SFET_0(A), SFET_1(B), SPT, SFET_2(C), SFET_3(D), SFEA, SFEC1-4(fict)
//                      slot#:  1       2         3        4      5         6         7     8  9 10 11
//    "a la Q" slot_id(link#): 10*      9         8       11*     7         6         1     4  5  2  3
//    "a la T" slot_id(link#):  9*      0         1       10*     2         3         4     5* 6* 7* 8*
//     "*" means that given link# is fictitious and should not be found in data !!!
//----
//slot-id's map for ChargeLinks( link(=slot_id) numbers vs seq.slot): 
  int16u AMSSCIds::cardidc[SCCRAT][SCSLTM]={10, 9, 8,11, 7, 6, 1, 4, 5, 2, 3, //cr-1
                                            10, 9, 8,11, 7, 6, 1, 4, 5, 2, 3, //cr-2 
                                            10, 9, 8,11, 7, 6, 1, 4, 5, 2, 3, //cr-3 
                                            10, 9, 8,11, 7, 6, 1, 4, 5, 2, 3  //cr-4 
					   };
//slot-id's map for TimeLinks( link(=slot_id) numbers vs seq.slot): 
  int16u AMSSCIds::cardidt[SCCRAT][SCSLTM]={ 9, 0, 1,10, 2, 3, 4, 5, 6, 7, 8, //cr-1
                                             9, 0, 1,10, 2, 3, 4, 5, 6, 7, 8, //cr-2 
                                             9, 0, 1,10, 2, 3, 4, 5, 6, 7, 8, //cr-3 
                                             9, 0, 1,10, 2, 3, 4, 5, 6, 7, 8  //cr-4 
					   };
//Trig.patt.bits assignment to LBBS  in SPT2 info:
  int AMSSCIds::trpatba[TOF2GC::SCCRAT][TOF2GC::SCMXBR2]={
                           2012,2032,2052,2072,2022,2042,2062,2082,           //cr-1
                           1011,1031,1051,1071,1021,1041,1061,1081,   0,   0,
			   2011,2031,2051,2071,2021,2041,2061,2081,           //cr-2
			   1012,1032,1052,1072,1022,1042,1062,1082,   0,   0,
			   4012,4032,4052,4072,4022,4042,4062,4082,           //cr-3			   
			   3012,3032,3052,3072,3022,3042,3062,3082,3092,3102, 
			   4011,4031,4051,4071,4021,4041,4061,4081,           //cr-4
			   3011,3031,3051,3071,3021,3041,3061,3081,3091,3101
			   };
//------
AMSSCIds::AMSSCIds(int swid):_dummy(0){
//
int16u slt;
  _swid=swid;//LBBSPM(tof) or BSPM(anti)
  _layer=swid/100000;
  _mtyp=swid%10;
  if(_layer==0)_dtype=2;//anti
  else{
    _dtype=1;//tof
    _layer-=1;
  }
  _bar=(swid%100000)/1000-1;
  _side=(swid%1000)/100-1;
  _pmt=(swid%100)/10;

//_swch => glogal sequential sw-channel(TOF+ANTI, incl pmts and mtyps)
  _swch=swseqn(_dtype,_layer,_bar,_side,_pmt,_mtyp);
  
  _hwid=hidlst[_swch];//CSSRR
  if(_hwid>0){
    _crate=_hwid/10000-1;
    _slot=(_hwid%10000)/100-1;
    _rdch=_hwid%100-1;
    _sltyp=sltymap[_crate][_slot];
//    _crdid=cardids[_crate][_slot];
    _hwch=hwseqn(_crate,_slot,_rdch);//global sequential hw/channel
  }
  else{
    _dummy=1;
    cout<<" *RunTimeError: TofAntConstructor: non-phys s/w-id,(L)BBSPM="<<swid<<" swch="<<_swch<<endl;
  }
}
//------
AMSSCIds::AMSSCIds(int16u layer, int16u bar, int16u side, int16u pmt, int16u mtyp):_dummy(0){
//constructor for TOF
  _dtype=1;
  _swid=100000*(layer+1)+1000*(bar+1)+100*(side+1)+10*pmt+mtyp;//LBBSPM
  _layer=layer;//0,1,...
  _bar=bar;//0,1,...
  _side=side;
  _pmt=pmt;//0,1,2,3(Anode,Dyn1,Dyn2,Dyn3)
  _mtyp=mtyp;//0,1,2,3,4(t,Q,ft,sumHT,sumSHT)
  
//_swch => global sequential sw-channel(TOF+ANTI, incl pmts and mtyps)
  _swch=swseqn(_dtype,_layer,_bar,_side,_pmt,_mtyp);
  
  _hwid=hidlst[_swch];//CSSRR
  if(_hwid>0){
    _crate=_hwid/10000-1;
    _slot=(_hwid%10000)/100-1;
    _rdch=_hwid%100-1;
    _sltyp=sltymap[_crate][_slot];
//    _crdid=cardids[_crate][_slot];
    _hwch=hwseqn(_crate,_slot,_rdch);//global sequential hw/channel
  }
  else{
    _dummy=1;
    cout<<" *RunTimeError: TofConstructor:non-phys. s/w-chan: L/BB/S/P/M="<<layer<<"/"<<bar<<"/"<<side<<"/"<<pmt<<"/"<<mtyp<<endl;
    cout<<"                         swid="<<_swid<<" swch="<<_swch<<endl;
  }
}
//------
AMSSCIds::AMSSCIds(int16u bar, int16u side, int16u pmt, int16u mtyp):_dummy(0){
//constructor for ANTI
  _dtype=2;
  _swid=1000*(bar+1)+100*(side+1)+10*pmt+mtyp;//BSPM
  _layer=0;//0
  _bar=bar;//0,1,...
  _side=side;//0,1
  _pmt=pmt;//0(Anode only)
  _mtyp=mtyp;//0,1,2(t,Q,ft)
  
//_swch => global sequential sw-channel(TOF+ANTI, incl pmts and mtyps)
  _swch=swseqn(_dtype,_layer,_bar,_side,_pmt,_mtyp);

  _hwid=hidlst[_swch];//CSSRR
  if(_hwid>0){
    _crate=_hwid/10000-1;
    _slot=(_hwid%10000)/100-1;
    _rdch=_hwid%100-1;
    _sltyp=sltymap[_crate][_slot];
//    _crdid=cardids[_crate][_slot];
    _hwch=hwseqn(_crate,_slot,_rdch);//global sequential hw-channel
  }
  else{
    _dummy=1;
    cout<<" *RunTimeError: AntiConstructor:non-phys. s/w-chan: bar/side/pm/mtyp="<<bar<<"/"<<side<<"/"<<pmt<<"/"<<mtyp<<endl;
    cout<<"   swid="<<_swid<<" swch="<<_swch<<" hwid="<<_hwid<<endl;
  }
}
//------
AMSSCIds::AMSSCIds(int16u crate, int16u slot, int16u rdch):_crate(crate),_slot(slot),
                                          _rdch(rdch),_dummy(0){
//
  _hwid=(_crate+1)*10000+(_slot+1)*100+(_rdch+1);//CSSRR
  _sltyp=sltymap[_crate][_slot];
//  _crdid=cardids[_crate][_slot];
  _hwch=hwseqn(crate,slot,rdch);//global sequential hw-channel
  _swid=sidlst[_hwch];//LBBSPM or BSPM

  if(_swid>0){
    _layer=_swid/100000;
    _mtyp=_swid%10;
    if(_layer==0)_dtype=2;//anti
    else{
      _dtype=1;//tof
      _layer-=1;
    }
    _bar=(_swid%100000)/1000-1;
    _side=(_swid%1000)/100-1;
    _pmt=(_swid%100)/10;
//  _swch => global sequential sw-channel(TOF+ANTI, incl pmts and mtyps)
    _swch=swseqn(_dtype,_layer,_bar,_side,_pmt,_mtyp);
  }//endof "swid>0" check
  else{
    _dummy=1;
    cout<<" *RunTimeError: TofAntiConstructor: empty readout-channel, crat/slot/rdch="<<crate<<"/"<<slot<<"/"<<rdch<<endl;
  }
}
//--------------
int16 AMSSCIds::crdidc2sl(int16u crate, int16u crdid){//card(slot)_id(ChargeLinks) to sequential slot# 
  for(int i=0;i<SCSLTM;i++){
    if(cardidc[crate][i]==crdid)return(i);
  }
  return(-1);//if illegal(not existing) crdid
}
//--------------
int16 AMSSCIds::crdidt2sl(int16u crate, int16u crdid){//card(slot)_id(TimeLinks) to sequential slot# 
  for(int i=0;i<SCSLTM;i++){
    if(cardidt[crate][i]==crdid)return(i);
  }
  return(-1);//if illegal(not existing) crdid
}
//--------------
int AMSSCIds::gettpbas(int16u crt, int16u bit){//return LBBS for bit bit(0-17) in crate crt(0-3)
  return trpatba[crt][bit];
}
//--------------
int16u AMSSCIds::ich2rdch(int16u crate, int16u slot, int16u inpch, int16u mtyp){
//Input:SC-crate(0-3);slot(0-10),InpCh(0-(maxv-1));mtyp(1/!=1)->Q/Any_kind_of_Time, imply
// the inp.ch. structure 5x(T+Q) + 3xT(FT,sHT,sSHT) ==> certain seq.of rdch(as in sidlst[]-array) !!!!!
// Output:ReadoutCh(1-maxv),if =0 -> empty
// or illegal(no corresponding output(rdch)) input channel
//
  int16u rdch(0),sltyp,hwch;
  integer swid;
  sltyp=sltymap[crate][slot];//1/2/3/4/5->SFET/SFEA/SFEC/SDR/SPT
  if(sltyp>3)return(0);//not direct_input data-slot(ie.SPT or SDR)
  if(inpch>=ichpsty[sltyp-1])return(0);//illegal for given sltyp
  if(sltyp==3){//SFEC
    rdch=inpch;//for SFEC inpch=outch(rdch),(0,...9)
  }
  else{//SFET,SFEA(imply structure 5x(T+Q) + 3xT as in sidlst[]-array 
    if(mtyp==1){
      if(inpch<5)rdch=2*inpch+1;//Q: 1,3,5,7,9
      else return(0);//not Q-measuring inp.channels
    }
    else{//time-measurements
      if(inpch<5)rdch=2*inpch;//LT-time:0,2,4,6,8
      else rdch=10+(inpch-5);//10,11,12->FT,sHT,sSHT-times
    }
  }
  hwch=hwseqn(crate,slot,rdch);//glob. hw outp.seq.# 
  swid=sidlst[hwch];
  if(swid>0)return rdch+1;//1,...,maxv
  else return(0);//empty inp.ch
}
//--------------
void AMSSCIds::inittable(){
//
//--->count total TOF s/w-channels:
//
  int ttswch(0);//total TOF sequential sw-channels(incl pmt and mtyp)
//
  cout<<"====> AMSSCIds::inittable: starting build..."<<endl;
  for(int il=0;il<TOF2DBc::getnplns();il++){
    for(int ib=0;ib<TOF2DBc::getbppl(il)+2;ib++){//"+2"-> 2 fict.paddles for 2 
//                                                 group-measurements(1st and 2nd half of the plane)
      if(ib>=TOF2DBc::getbppl(il))ttswch+=SCHPMTS;//measur.types for fict.paddles
      else ttswch+=(TOF2DBc::npmtps(il,ib)*SCDMTS+SCAMTS);
    }
  }
  ttswch*=2;//2sides
  totswch[0]=ttswch;
//
//--->count total ANTI s/w-channels:
//
  int taswch(0);//total ANTI sequential sw-channels(incl mtyp)
  taswch=MAXANTI*ANAMTS+2*ANAGMTS;//"2" means 2 fict.sectors for 2 group-measurements(cylinder's 1st/2nd half)
  taswch*=2;//2sides
  totswch[1]=taswch;
//
  cout<<"      total s/w-channels(real tof/anti) "<<totswch[0]<<" "<<totswch[1]<<" must be initialised !"<<endl;
//
//--->fill sid's as function of sequential number of h/w-channel
//below used: tof_swid->LBBSPM, anti_swid->BSPM.
//
  int bias(0);
// <=========================================== crate-SC0 (top/wake)
//--->TOF(anodes,SFET):
  sidlst[bias]=   201200;//t   l2/b1/s2  cr0/sl_2  
  sidlst[bias+1]= 201201;//q
  sidlst[bias+2]= 203200;//t      b3/s2
  sidlst[bias+3]= 203201;//q
  sidlst[bias+4]= 205200;//t      b5/s2
  sidlst[bias+5]= 205201;//q
  sidlst[bias+6]= 207200;//t      b7/s2
  sidlst[bias+7]= 207201;//q
  sidlst[bias+8]= 0;
  sidlst[bias+9]= 0;
  sidlst[bias+10]=209202;//ft     fb9/s2
  sidlst[bias+11]=209203;//SumHT, fb9/s2
  sidlst[bias+12]=209204;//SumSHT fb9/s2
  bias+=13;
  sidlst[bias]=   202200;//t    l2/b2/s2 cr0/sl_3 
  sidlst[bias+1]= 202201;//q
  sidlst[bias+2]= 204200;//t       b4/s2
  sidlst[bias+3]= 204201;//q
  sidlst[bias+4]= 206200;//t       b6/s2
  sidlst[bias+5]= 206201;//q
  sidlst[bias+6]= 208200;//t       b8/s2
  sidlst[bias+7]= 208201;//q
  sidlst[bias+8]= 0;
  sidlst[bias+9]= 0;
  sidlst[bias+10]=210202;//ft     fb10/s2
  sidlst[bias+11]=210203;//SumHT, fb10/s2
  sidlst[bias+12]=210204;//SumSHT,fb10/s2
  bias+=13;
  sidlst[bias]=   101100;//t   l1/b1/s1  cr0/sl_5  
  sidlst[bias+1]= 101101;//q
  sidlst[bias+2]= 103100;//t      b3/s1
  sidlst[bias+3]= 103101;//q
  sidlst[bias+4]= 105100;//t      b5/s1
  sidlst[bias+5]= 105101;//q
  sidlst[bias+6]= 107100;//t      b7/s1
  sidlst[bias+7]= 107101;//q
  sidlst[bias+8]= 0;
  sidlst[bias+9]= 0;
  sidlst[bias+10]=109102;//ft     fb9/s1
  sidlst[bias+11]=109103;//SumHT, fb9/s1
  sidlst[bias+12]=109104;//SumSHT fb9/s1
  bias+=13;
  sidlst[bias]=   102100;//t    l1/b2/s1 cr0/sl_6 
  sidlst[bias+1]= 102101;//q
  sidlst[bias+2]= 104100;//t       b4/s1
  sidlst[bias+3]= 104101;//q
  sidlst[bias+4]= 106100;//t       b6/s1
  sidlst[bias+5]= 106101;//q
  sidlst[bias+6]= 108100;//t       b8/s1
  sidlst[bias+7]= 108101;//q
  sidlst[bias+8]= 0;
  sidlst[bias+9]= 0;
  sidlst[bias+10]=110102;//ft     fb10/s1
  sidlst[bias+11]=110103;//SumHT, fb10/s1
  sidlst[bias+12]=110104;//SumSHT,fb10/s1 
  bias+=13;
//--->ANTI(anodes,SFEA):
  sidlst[bias]=   1200;//t       b1/s2  cr0/sl_7
  sidlst[bias+1]= 1201;//q  
  sidlst[bias+2]= 2200;//t       b2/s2 
  sidlst[bias+3]= 2201;//q  
  sidlst[bias+4]= 3200;//t       b3/s2 
  sidlst[bias+5]= 3201;//q  
  sidlst[bias+6]= 4200;//t       b4/s2 
  sidlst[bias+7]= 4201;//q  
  sidlst[bias+8]= 0;//            empty  
  sidlst[bias+9]= 0;  
  sidlst[bias+10]=9202;//ft     fb9/s2
  bias+=11;
//--->TOF(dynodes,SFEC):
  sidlst[bias]=   101111;//q  l1/b1/s1/d1  cr0/sl_8(aux)
  sidlst[bias+1]= 102111;//q     b2/s1/d1 
  sidlst[bias+2]= 103111;//q     b3/s1/d1 
  sidlst[bias+3]= 104111;//q     b4/s1/d1 
  sidlst[bias+4]= 105111;//q     b5/s1/d1 
  sidlst[bias+5]= 106111;//q     b6/s1/d1 
  sidlst[bias+6]= 107111;//q     b7/s1/d1 
  sidlst[bias+7]= 108111;//q     b8/s1/d1 
  sidlst[bias+8]= 101131;//q     b1/s1/d3 
  sidlst[bias+9]= 0;//             empty
  bias+=10;  
  sidlst[bias]=   101121;//q     b1/s1/d2  cr0/sl_9(aux) 
  sidlst[bias+1]= 102121;//q     b2/s1/d2 
  sidlst[bias+2]= 103121;//q     b3/s1/d2 
  sidlst[bias+3]= 104121;//q     b4/s1/d2 
  sidlst[bias+4]= 105121;//q     b5/s1/d2 
  sidlst[bias+5]= 106121;//q     b6/s1/d2 
  sidlst[bias+6]= 107121;//q     b7/s1/d2 
  sidlst[bias+7]= 108121;//q     b8/s1/d2 
  sidlst[bias+8]= 108131;//q     b8/s1/d3 
  sidlst[bias+9]= 0;//             empty
  bias+=10;  
  sidlst[bias]=   201211;//q  l2/b1/s2/d1  cr0/sl_10(aux)
  sidlst[bias+1]= 202211;//q     b2/s2/d1 
  sidlst[bias+2]= 203211;//q     b3/s2/d1 
  sidlst[bias+3]= 204211;//q     b4/s2/d1 
  sidlst[bias+4]= 205211;//q     b5/s2/d1 
  sidlst[bias+5]= 206211;//q     b6/s2/d1 
  sidlst[bias+6]= 207211;//q     b7/s2/d1 
  sidlst[bias+7]= 208211;//q     b8/s2/d1 
  sidlst[bias+8]= 0;//              empty  
  sidlst[bias+9]= 0;//              empty  
  bias+=10;  
  sidlst[bias]=   201221;//q    b1/s2/d2  cr0/sl_11(aux) 
  sidlst[bias+1]= 202221;//q    b2/s2/d2 
  sidlst[bias+2]= 203221;//q    b3/s2/d2 
  sidlst[bias+3]= 204221;//q    b4/s2/d2 
  sidlst[bias+4]= 205221;//q    b5/s2/d2 
  sidlst[bias+5]= 206221;//q    b6/s2/d2 
  sidlst[bias+6]= 207221;//q    b7/s2/d2 
  sidlst[bias+7]= 208221;//q    b8/s2/d2 
  sidlst[bias+8]= 0;//              empty  
  sidlst[bias+9]= 0;//              empty  
  bias+=10;  
//
// <======================================== crate-SC1 (top/ram=L1/S1)  
//--->TOF(anodes,SFET):
  sidlst[bias]=   201100;//t   l2/b1/s1  cr1/sl_2  
  sidlst[bias+1]= 201101;//q
  sidlst[bias+2]= 203100;//t      b3/s1
  sidlst[bias+3]= 203101;//q
  sidlst[bias+4]= 205100;//t      b5/s1
  sidlst[bias+5]= 205101;//q
  sidlst[bias+6]= 207100;//t      b7/s1
  sidlst[bias+7]= 207101;//q
  sidlst[bias+8]= 0;
  sidlst[bias+9]= 0;
  sidlst[bias+10]=209102;//ft     fb9/s1
  sidlst[bias+11]=209103;//SumHT, fb9/s1
  sidlst[bias+12]=209104;//SumSHT fb9/s1
  bias+=13;
  sidlst[bias]=   202100;//t    l2/b2/s1 cr1/sl_3 
  sidlst[bias+1]= 202101;//q
  sidlst[bias+2]= 204100;//t       b4/s1
  sidlst[bias+3]= 204101;//q
  sidlst[bias+4]= 206100;//t       b6/s1
  sidlst[bias+5]= 206101;//q
  sidlst[bias+6]= 208100;//t       b8/s1
  sidlst[bias+7]= 208101;//q
  sidlst[bias+8]= 0;
  sidlst[bias+9]= 0;
  sidlst[bias+10]=210102;//ft     fb10/s1
  sidlst[bias+11]=210103;//SumHT, fb10/s1
  sidlst[bias+12]=210104;//SumSHT,fb10/s1
  bias+=13;
  sidlst[bias]=   101200;//t   l1/b1/s2  cr1/sl_5  
  sidlst[bias+1]= 101201;//q
  sidlst[bias+2]= 103200;//t      b3/s2
  sidlst[bias+3]= 103201;//q
  sidlst[bias+4]= 105200;//t      b5/s2
  sidlst[bias+5]= 105201;//q
  sidlst[bias+6]= 107200;//t      b7/s2
  sidlst[bias+7]= 107201;//q
  sidlst[bias+8]= 0;
  sidlst[bias+9]= 0;
  sidlst[bias+10]=109202;//ft     fb9/s2
  sidlst[bias+11]=109203;//SumHT, fb9/s2
  sidlst[bias+12]=109204;//SumSHT fb9/s2
  bias+=13;
  sidlst[bias]=   102200;//t    l1/b2/s2 cr1/sl_6 
  sidlst[bias+1]= 102201;//q
  sidlst[bias+2]= 104200;//t       b4/s2
  sidlst[bias+3]= 104201;//q
  sidlst[bias+4]= 106200;//t       b6/s2
  sidlst[bias+5]= 106201;//q
  sidlst[bias+6]= 108200;//t       b8/s2
  sidlst[bias+7]= 108201;//q
  sidlst[bias+8]= 0;
  sidlst[bias+9]= 0;
  sidlst[bias+10]=   110202;//ft fb10/s2
  sidlst[bias+11]=110203;//SumHT, fb10/s2
  sidlst[bias+12]=110204;//SumSHT fb10/s2 
  bias+=13;
//--->ANTI(anodes,SFEA):
  sidlst[bias]=   5200;//t       b1/s2  cr1/sl_7
  sidlst[bias+1]= 5201;//q  
  sidlst[bias+2]= 6200;//t       b2/s2 
  sidlst[bias+3]= 6201;//q  
  sidlst[bias+4]= 7200;//t       b3/s2 
  sidlst[bias+5]= 7201;//q  
  sidlst[bias+6]= 8200;//t       b4/s2 
  sidlst[bias+7]= 8201;//q  
  sidlst[bias+8]= 0;//            empty  
  sidlst[bias+9]= 0;  
  sidlst[bias+10]=10202;//ft    fb10/s2
  bias+=11;
//--->TOF(dynodes,SFEC):
  sidlst[bias]=   101211;//q  l1/b1/s2/d1  cr1/sl_8(aux)
  sidlst[bias+1]= 102211;//q     b2/s2/d1 
  sidlst[bias+2]= 103211;//q     b3/s2/d1 
  sidlst[bias+3]= 104211;//q     b4/s2/d1 
  sidlst[bias+4]= 105211;//q     b5/s2/d1 
  sidlst[bias+5]= 106211;//q     b6/s2/d1 
  sidlst[bias+6]= 107211;//q     b7/s2/d1 
  sidlst[bias+7]= 108211;//q     b8/s2/d1 
  sidlst[bias+8]= 101231;//q     b1/s2/d3 
  sidlst[bias+9]= 0;//             empty
  bias+=10;  
  sidlst[bias]=   101221;//q     b1/s2/d2  cr1/sl_9(aux) 
  sidlst[bias+1]= 102221;//q     b2/s2/d2 
  sidlst[bias+2]= 103221;//q     b3/s2/d2 
  sidlst[bias+3]= 104221;//q     b4/s2/d2 
  sidlst[bias+4]= 105221;//q     b5/s2/d2 
  sidlst[bias+5]= 106221;//q     b6/s2/d2 
  sidlst[bias+6]= 107221;//q     b7/s2/d2 
  sidlst[bias+7]= 108221;//q     b8/s2/d2 
  sidlst[bias+8]= 108231;//q     b8/s2/d3 
  sidlst[bias+9]= 0;//             empty
  bias+=10;  
  sidlst[bias]=   201111;//q  l2/b1/s1/d1  cr1/sl_10(aux)
  sidlst[bias+1]= 202111;//q     b2/s1/d1 
  sidlst[bias+2]= 203111;//q     b3/s1/d1 
  sidlst[bias+3]= 204111;//q     b4/s1/d1 
  sidlst[bias+4]= 205111;//q     b5/s1/d1 
  sidlst[bias+5]= 206111;//q     b6/s1/d1 
  sidlst[bias+6]= 207111;//q     b7/s1/d1 
  sidlst[bias+7]= 208111;//q     b8/s1/d1 
  sidlst[bias+8]= 0;//              empty  
  sidlst[bias+9]= 0;//              empty  
  bias+=10;  
  sidlst[bias]=   201121;//q    b1/s1/d2  cr1/sl_11(aux) 
  sidlst[bias+1]= 202121;//q    b2/s1/d2 
  sidlst[bias+2]= 203121;//q    b3/s1/d2 
  sidlst[bias+3]= 204121;//q    b4/s1/d2 
  sidlst[bias+4]= 205121;//q    b5/s1/d2 
  sidlst[bias+5]= 206121;//q    b6/s1/d2 
  sidlst[bias+6]= 207121;//q    b7/s1/d2 
  sidlst[bias+7]= 208121;//q    b8/s1/d2 
  sidlst[bias+8]= 0;//              empty  
  sidlst[bias+9]= 0;//              empty  
  bias+=10;  
//  
// <=========================================== crate-SC2 (bot/wake)
//--->TOF(anodes,SFET):
  sidlst[bias]=   401200;//t   l4/b1/s2  cr2/sl_2  
  sidlst[bias+1]= 401201;//q
  sidlst[bias+2]= 403200;//t      b3/s2
  sidlst[bias+3]= 403201;//q
  sidlst[bias+4]= 405200;//t      b5/s2
  sidlst[bias+5]= 405201;//q
  sidlst[bias+6]= 407200;//t      b7/s2
  sidlst[bias+7]= 407201;//q
  sidlst[bias+8]= 0;
  sidlst[bias+9]= 0;
  sidlst[bias+10]=409202;//ft     fb9/s2
  sidlst[bias+11]=409203;//SumHT, fb9/s2
  sidlst[bias+12]=409204;//SumSHT fb9/s2
  bias+=13;
  sidlst[bias]=   402200;//t    l4/b2/s2 cr2/sl_3 
  sidlst[bias+1]= 402201;//q
  sidlst[bias+2]= 404200;//t       b4/s2
  sidlst[bias+3]= 404201;//q
  sidlst[bias+4]= 406200;//t       b6/s2
  sidlst[bias+5]= 406201;//q
  sidlst[bias+6]= 408200;//t       b8/s2
  sidlst[bias+7]= 408201;//q
  sidlst[bias+8]= 0;
  sidlst[bias+9]= 0;
  sidlst[bias+10]=410202;//ft     fb10/s2
  sidlst[bias+11]=410203;//SumHT, fb10/s2
  sidlst[bias+12]=410204;//SumSHT,fb10/s2
  bias+=13;
  sidlst[bias]=   301200;//t    l3/b1/s2  cr2/sl_5  
  sidlst[bias+1]= 301201;//q
  sidlst[bias+2]= 303200;//t       b3/s2
  sidlst[bias+3]= 303201;//q
  sidlst[bias+4]= 305200;//t       b5/s2
  sidlst[bias+5]= 305201;//q
  sidlst[bias+6]= 307200;//t       b7/s2
  sidlst[bias+7]= 307201;//q
  sidlst[bias+8]= 309200;//t       b9/s2
  sidlst[bias+9]= 309201;//q
  sidlst[bias+10]=311202;//ft     fb11/s2
  sidlst[bias+11]=311203;//SumHT, fb11/s2
  sidlst[bias+12]=311204;//SumSHT fb11/s2
  bias+=13;
  sidlst[bias]=   302200;//t    l3/b2/s2 cr2/sl_6 
  sidlst[bias+1]= 302201;//q
  sidlst[bias+2]= 304200;//t       b4/s2
  sidlst[bias+3]= 304201;//q
  sidlst[bias+4]= 306200;//t       b6/s2
  sidlst[bias+5]= 306201;//q
  sidlst[bias+6]= 308200;//t       b8/s2
  sidlst[bias+7]= 308201;//q
  sidlst[bias+8]= 310200;//t      b10/s2
  sidlst[bias+9]= 310201;//q
  sidlst[bias+10]=312202;//ft     fb12/s2
  sidlst[bias+11]=312203;//SumHT, fb12/s2
  sidlst[bias+12]=312204;//SumSHT,fb12/s2 
  bias+=13;
//--->ANTI(anodes,SFEA):
  sidlst[bias]=   1100;//t       b1/s1  cr2/sl_7
  sidlst[bias+1]= 1101;//q  
  sidlst[bias+2]= 2100;//t       b2/s1 
  sidlst[bias+3]= 2101;//q 
  sidlst[bias+4]= 3100;//t       b3/s1 
  sidlst[bias+5]= 3101;//q  
  sidlst[bias+6]= 4100;//t       b4/s1 
  sidlst[bias+7]= 4101;//q  
  sidlst[bias+8]= 0;//            empty 
  sidlst[bias+9]= 0;  
  sidlst[bias+10]=9102;//ft     fb9/s1
  bias+=11;
//--->TOF(dynodes,SFEC):
  sidlst[bias]=   301211;//q  l3/b1/s2/d1  cr2/sl_8(aux)
  sidlst[bias+1]= 302211;//q     b2/s2/d1 
  sidlst[bias+2]= 303211;//q     b3/s2/d1 
  sidlst[bias+3]= 304211;//q     b4/s2/d1 
  sidlst[bias+4]= 305211;//q     b5/s2/d1 
  sidlst[bias+5]= 306211;//q     b6/s2/d1 
  sidlst[bias+6]= 307211;//q     b7/s2/d1 
  sidlst[bias+7]= 308211;//q     b8/s2/d1 
  sidlst[bias+8]= 309211;//q     b9/s2/d1 
  sidlst[bias+9]= 310211;//q    b10/s2/d1
  bias+=10;  
  sidlst[bias]=   301221;//q     b1/s2/d2  cr2/sl_9(aux) 
  sidlst[bias+1]= 302221;//q     b2/s2/d2 
  sidlst[bias+2]= 303221;//q     b3/s2/d2 
  sidlst[bias+3]= 304221;//q     b4/s2/d2 
  sidlst[bias+4]= 305221;//q     b5/s2/d2 
  sidlst[bias+5]= 306221;//q     b6/s2/d2 
  sidlst[bias+6]= 307221;//q     b7/s2/d2 
  sidlst[bias+7]= 308221;//q     b8/s2/d2 
  sidlst[bias+8]= 309221;//q     b9/s2/d2 
  sidlst[bias+9]= 310221;//q    b10/s2/d2
  bias+=10;  
  sidlst[bias]=   401211;//q  l4/b1/s2/d1  cr2/sl_10(aux)
  sidlst[bias+1]= 402211;//q     b2/s2/d1 
  sidlst[bias+2]= 403211;//q     b3/s2/d1 
  sidlst[bias+3]= 404211;//q     b4/s2/d1 
  sidlst[bias+4]= 405211;//q     b5/s2/d1 
  sidlst[bias+5]= 406211;//q     b6/s2/d1 
  sidlst[bias+6]= 407211;//q     b7/s2/d1 
  sidlst[bias+7]= 408211;//q     b8/s2/d1 
  sidlst[bias+8]= 401231;//q     b1/s2/d3  
  sidlst[bias+9]= 0;//              empty  
  bias+=10;  
  sidlst[bias]=   401221;//q    b1/s2/d2  cr2/sl_11(aux) 
  sidlst[bias+1]= 402221;//q    b2/s2/d2 
  sidlst[bias+2]= 403221;//q    b3/s2/d2 
  sidlst[bias+3]= 404221;//q    b4/s2/d2 
  sidlst[bias+4]= 405221;//q    b5/s2/d2 
  sidlst[bias+5]= 406221;//q    b6/s2/d2 
  sidlst[bias+6]= 407221;//q    b7/s2/d2 
  sidlst[bias+7]= 408221;//q    b8/s2/d2 
  sidlst[bias+8]= 408231;//q    b8/s2/d3  
  sidlst[bias+9]= 0;//             empty  
  bias+=10;  
//
// <======================================== crate-SC3 (bot/ram)  
//--->TOF(anodes,SFET):
  sidlst[bias]=   401100;//t   l4/b1/s1  cr3/sl_2  
  sidlst[bias+1]= 401101;//q
  sidlst[bias+2]= 403100;//t      b3/s1
  sidlst[bias+3]= 403101;//q
  sidlst[bias+4]= 405100;//t      b5/s1
  sidlst[bias+5]= 405101;//q
  sidlst[bias+6]= 407100;//t      b7/s1
  sidlst[bias+7]= 407101;//q
  sidlst[bias+8]= 0;
  sidlst[bias+9]= 0;
  sidlst[bias+10]=409102;//ft     fb9/s1
  sidlst[bias+11]=409103;//SumHT, fb9/s1
  sidlst[bias+12]=409104;//SumSHT fb9/s1
  bias+=13;
  sidlst[bias]=   402100;//t    l4/b2/s1 cr3/sl_3 
  sidlst[bias+1]= 402101;//q
  sidlst[bias+2]= 404100;//t       b4/s1
  sidlst[bias+3]= 404101;//q
  sidlst[bias+4]= 406100;//t       b6/s1
  sidlst[bias+5]= 406101;//q
  sidlst[bias+6]= 408100;//t       b8/s1
  sidlst[bias+7]= 408101;//q
  sidlst[bias+8]= 0;
  sidlst[bias+9]= 0;
  sidlst[bias+10]=410102;//ft     fb10/s1
  sidlst[bias+11]=410103;//SumHT, fb10/s1
  sidlst[bias+12]=410104;//SumSHT,fb10/s1
  bias+=13;
  sidlst[bias]=   301100;//t    l3/b1/s1  cr3/sl_5  
  sidlst[bias+1]= 301101;//q
  sidlst[bias+2]= 303100;//t       b3/s1
  sidlst[bias+3]= 303101;//q
  sidlst[bias+4]= 305100;//t       b5/s1
  sidlst[bias+5]= 305101;//q
  sidlst[bias+6]= 307100;//t       b7/s1
  sidlst[bias+7]= 307101;//q
  sidlst[bias+8]= 309100;//t       b9/s1
  sidlst[bias+9]= 309101;//q
  sidlst[bias+10]=311102;//ft     fb11/s1
  sidlst[bias+11]=311103;//SumHT, fb11/s1
  sidlst[bias+12]=311104;//SumSHT fb11/s1
  bias+=13;
  sidlst[bias]=   302100;//t    l3/b2/s1 cr3/sl_6 
  sidlst[bias+1]= 302101;//q
  sidlst[bias+2]= 304100;//t       b4/s1
  sidlst[bias+3]= 304101;//q
  sidlst[bias+4]= 306100;//t       b6/s1
  sidlst[bias+5]= 306101;//q
  sidlst[bias+6]= 308100;//t       b8/s1
  sidlst[bias+7]= 308101;//q
  sidlst[bias+8]= 310100;//t      b10/s1
  sidlst[bias+9]= 310101;//q
  sidlst[bias+10]=312102;//ft     fb12/s1
  sidlst[bias+11]=312103;//SumHT, fb12/s1
  sidlst[bias+12]=312104;//SumSHT,fb12/s1 
  bias+=13;
//--->ANTI(anodes,SFEA):
  sidlst[bias]=   5100;//t       b5/s1  cr3/sl_7
  sidlst[bias+1]= 5101;//q  
  sidlst[bias+2]= 6100;//t       b6/s1 
  sidlst[bias+3]= 6101;//q 
  sidlst[bias+4]= 7100;//t       b7/s1 
  sidlst[bias+5]= 7101;//q  
  sidlst[bias+6]= 8100;//t       b8/s1 
  sidlst[bias+7]= 8101;//q  
  sidlst[bias+8]= 0;//  
  sidlst[bias+9]= 0;  
  sidlst[bias+10]=10102;//ft   fb10/s1
  bias+=11;
//--->TOF(dynodes,SFEC):
  sidlst[bias]=   301111;//q  l3/b1/s1/d1  cr3/sl_8(aux)
  sidlst[bias+1]= 302111;//q     b2/s1/d1 
  sidlst[bias+2]= 303111;//q     b3/s1/d1 
  sidlst[bias+3]= 304111;//q     b4/s1/d1 
  sidlst[bias+4]= 305111;//q     b5/s1/d1 
  sidlst[bias+5]= 306111;//q     b6/s1/d1 
  sidlst[bias+6]= 307111;//q     b7/s1/d1 
  sidlst[bias+7]= 308111;//q     b8/s1/d1 
  sidlst[bias+8]= 309111;//q     b9/s1/d1 
  sidlst[bias+9]= 310111;//q    b10/s1/d1
  bias+=10;  
  sidlst[bias]=   301121;//q     b1/s1/d2  cr3/sl_9(aux) 
  sidlst[bias+1]= 302121;//q     b2/s1/d2 
  sidlst[bias+2]= 303121;//q     b3/s1/d2 
  sidlst[bias+3]= 304121;//q     b4/s1/d2 
  sidlst[bias+4]= 305121;//q     b5/s1/d2 
  sidlst[bias+5]= 306121;//q     b6/s1/d2 
  sidlst[bias+6]= 307121;//q     b7/s1/d2 
  sidlst[bias+7]= 308121;//q     b8/s1/d2 
  sidlst[bias+8]= 309121;//q     b9/s1/d2 
  sidlst[bias+9]= 310121;//q    b10/s1/d2
  bias+=10;  
  sidlst[bias]=   401111;//q  l4/b1/s1/d1  cr3/sl_10(aux)
  sidlst[bias+1]= 402111;//q     b2/s1/d1 
  sidlst[bias+2]= 403111;//q     b3/s1/d1 
  sidlst[bias+3]= 404111;//q     b4/s1/d1 
  sidlst[bias+4]= 405111;//q     b5/s1/d1 
  sidlst[bias+5]= 406111;//q     b6/s1/d1 
  sidlst[bias+6]= 407111;//q     b7/s1/d1 
  sidlst[bias+7]= 408111;//q     b8/s1/d1 
  sidlst[bias+8]= 401131;//q     b1/s1/d3  
  sidlst[bias+9]= 0;//              empty  
  bias+=10;
  sidlst[bias]=   401121;//q    b1/s1/d2  cr3/sl_11(aux) 
  sidlst[bias+1]= 402121;//q    b2/s1/d2 
  sidlst[bias+2]= 403121;//q    b3/s1/d2 
  sidlst[bias+3]= 404121;//q    b4/s1/d2 
  sidlst[bias+4]= 405121;//q    b5/s1/d2 
  sidlst[bias+5]= 406121;//q    b6/s1/d2 
  sidlst[bias+6]= 407121;//q    b7/s1/d2 
  sidlst[bias+7]= 408121;//q    b8/s1/d2 
  sidlst[bias+8]= 408131;//q    b8/s1/d3  
  sidlst[bias+9]= 0;//             empty  
  bias+=10;
  cout<<"      total TOF+ANTI h/w-channels was initialized(incl.empty) : "<<bias<<endl;
//
//---> prepare hwch->hwid table(incl. empty channels):
//
  int swid,hwid;
  int16u slt,chmx,hwch(0);
  for(int cr=0;cr<SCCRAT;cr++){
    for(int sl=0;sl<SCSLTM;sl++){
      slt=sltymap[cr][sl];
      chmx=ochpsty[slt-1];//outp.channels per slot(incl.empty)
      for(int ch=0;ch<chmx;ch++){
        hwid=(cr+1)*10000+(sl+1)*100+(ch+1);//CSSRR
        hidls[hwch]=hwid;
        hwch+=1;
      }
    }
  }
  cout<<"      S-crates capacity="<<hwch<<" h/w-channels(incl.empty)"<<endl;
//
//--->create invers table (hwid=F(swch)):
//
  int mtmn,mtmx,pmt,hch,ipmx;
  int16u swch=0;//1st- for TOF-channels
  for(int il=0;il<TOF2DBc::getnplns();il++){
//    cout<<"L="<<il+1<<endl;
    for(int ib=0;ib<TOF2DBc::getbppl(il)+2;ib++){//"+2" -> 2 half-planes
//      cout<<"   Bar="<<ib+1<<endl;
      for(int is=0;is<2;is++){//<--- sides loop
//        cout<<"       Side="<<is+1<<endl;
        if(ib<TOF2DBc::getbppl(il)){//single paddle channels
          ipmx=TOF2DBc::npmtps(il,ib);//max index for anode+dynodes counting(2(3))
	  mtmn=0;//min. anode mtyp
	  mtmx=SCAMTS-1;//max. anode mtyp (1)(i.e. 0/1->LT/Q)
        }
        else{//half-plane channels
          ipmx=0;//only anode
	  mtmn=SCAMTS;//mtyp-range from 2
	  mtmx=mtmn+SCHPMTS-1;// to 4 (i.e. 2/3/4->FT/sumHT/sumSHT)
        }
        for(int ip=0;ip<=ipmx;ip++){//<--- anode,dynodes loop
//	  cout<<"          pm="<<ip<<endl;
	  if(ip>0)mtmn=1;//mtyp=1 for dynodes
	  for(int mt=mtmn;mt<=mtmx;mt++){//<--- mtyp loop
//	    cout<<"               mtyp="<<mt<<endl;
	    swid=100000*(il+1)+1000*(ib+1)+100*(is+1)+10*ip+mt;//LBBSPM
	    for(int hc=0;hc<bias;hc++){//<-search in sidlst
	      if(swid==sidlst[hc]){
	        hch=hc;
	        goto Found1;
	      }
	    }//hwch
	    cout<<"<---- AMSSCIds::init:Error: missing TOF-swid in sidlst, swid="<<swid<<endl;
	    exit(1);
Found1:
	    hwid=hidls[hch];
	    hidlst[swch]=hwid;
	    swch+=1;
//	    cout<<"               swid="<<swid<<" hwid="<<hwid<<" swch="<<swch<<endl;
	  }//mtyp
	}//an+dynodes
      }//side
    }//bar
  }//layer
//----
  if(swch!=totswch[0]){
    cout<<"<---- AMSSCIds::init:Error: in TOF-swch counting !!!"<<endl;
    exit(1);
  }
//2nd- for ANTI-channels
  for(int ib=0;ib<MAXANTI+2;ib++){//<---sectors loop, "+2" for two half-cylinders
    for(int is=0;is<2;is++){//<---sides loop
      if(ib<MAXANTI){
        mtmn=0;//0(t)
	mtmx=ANAMTS-1;//1(q)
      }
      else{
        mtmn=ANAMTS;//2(FT)
	mtmx=mtmn+ANAGMTS-1;//2(FT)
      }
      pmt=0;
      for(int mt=mtmn;mt<=mtmx;mt++){//<--- mtyps loop
        swid=1000*(ib+1)+100*(is+1)+10*pmt+mt;//BSPM
	for(int hc=0;hc<bias;hc++){//<-search in sidlst
	  if(swid==sidlst[hc]){
	    hch=hc;
	    goto Found2;
	  }
	}//hwch
	cout<<"<---- AMSSCIds::init:Error: missing ANTI-swid in sidlst, swid="<<swid<<endl;
	exit(1);
Found2:
	hwid=hidls[hch];
	hidlst[swch]=hwid;
	swch+=1;
      }//mtyp
    }//side
  }//logic-sector
//  
  if(swch!=(totswch[0]+totswch[1])){
    cout<<"<---- AMSSCIds::init:Error: wrong ANTI-swch counting !!!"<<endl;
    exit(1);
  }
  cout<<"<---- AMSSCIds::init: successfully done !!!"<<endl<<endl;
}
//---------------
void AMSSCIds::selftest(){
  int swid,hwid,hwid1,dtyp;
  int16u cr,sl,ch,slt,chmx,hwch(0),swch(0),errors(0),ehwch(0);
  int16u lay,bar,side,pmt,mtyp;
  cout<<"====> Starting AMSSCIds selftest...."<<endl;
  for(cr=0;cr<SCCRAT;cr++){
    for(sl=0;sl<SCSLTM;sl++){
      slt=sltymap[cr][sl];
      chmx=ochpsty[slt-1];//outp.channels per slot(incl.empty)
      for(ch=0;ch<chmx;ch++){
        hwid=(cr+1)*10000+(sl+1)*100+(ch+1);//CSSRR
	swid=hw2swid(cr,sl,ch);//using hwseqn(...) and sidlst[hwch]
	if(swid>0){
          lay=swid/100000;
          mtyp=swid%10;
          if(lay==0)dtyp=2;//anti
          else{
            dtyp=1;//tof
            lay-=1;
          }
          bar=(swid%100000)/1000-1;
          side=(swid%1000)/100-1;
          pmt=(swid%100)/10;
	  swch=swseqn(dtyp,lay,bar,side,pmt,mtyp);//swch direct counting
	  hwid1=hidlst[swch];//backward to sidlst[]
	  if(hwid!=hwid1){
	    cout<<"      problem at hw/sw_id="<<hwid<<" "<<swid<<",backw.hwid="<<hwid1<<endl;
	    cout<<"      crate/slot/ochan="<<cr+1<<" "<<sl+1<<" "<<ch+1<<" sw/hw_ch="<<swch<<" "<<hwch<<endl;
	    errors+=1;
	  }
	}
	else{
	  ehwch+=1;
//	  cout<<"  warning: empty hwch at cr/sl/ch="<<cr+1<<" "<<sl+1<<" "<<ch+1<<endl;
	}
        hwch+=1;
      }
    }
  }
  cout<<"<---- AMSSCIds selftest done, errors : "<<errors<<" , empty hw-channels : "<<ehwch<<endl<<endl;
  if(errors>0)exit(1);
}
//---------------
int16u AMSSCIds::swseqn(int dtype, int16u layer, int16u bar, int16u side,
                                                 int16u pmt, int16u mtyp){
//
//dtype=1/2;layer=0,1...;bar=0,1,...;side=0,1;pmt=0,1,2,3;mtyp=0,1,2,3,4
//
  int16u swch(0);//glogal sequential sw-channel(incl pmts and mtyps)=0,1,...
  if(dtype==1){//TOF  
    assert(bar<(TOF2DBc::getbppl(layer)+2) && mtyp<(SCAMTS+SCHPMTS));
    if(bar>=TOF2DBc::getbppl(layer))assert(pmt==0 && mtyp>(SCAMTS-1));
//
    for(int il=0;il<layer;il++){// 1->count in layers befor current one
      for(int ib=0;ib<TOF2DBc::getbppl(il);ib++){
        swch+=(TOF2DBc::npmtps(il,ib)*SCDMTS+SCAMTS)*2;//2 sides
      }
      swch+=(SCHPMTS*2*2);//(2 half-planes FT/HT/SHT-measurements)*2sides
    }
    for(int ib=0;ib<bar;ib++){// 2->count in curr.layer (bars befor current one)
      if(ib<TOF2DBc::getbppl(layer))swch+=(TOF2DBc::npmtps(layer,ib)*SCDMTS+SCAMTS)*2;//2 sides
      else swch+=SCHPMTS*2;//2 sides of half-plane
    }
    for(int is=0;is<side;is++){// 3->count in curr.bar (compl.sides befor current one)
        if(bar<TOF2DBc::getbppl(layer))swch+=(TOF2DBc::npmtps(layer,bar)*SCDMTS+SCAMTS);
	else swch+=SCHPMTS;
    }
// 4->count in current side pmts*mtypes:
    if(pmt==0){//anode
      if(bar<TOF2DBc::getbppl(layer))swch+=mtyp;
      else swch+=(mtyp-SCAMTS);
    } 
    else{//dynode(means automatically that it is normal bar)
      swch+=SCAMTS;//all normal bar anode-measurements
      for(int pm=1;pm<pmt;pm++)swch+=SCDMTS;//+ prev.dynodes
      swch+=(mtyp-SCDMTS);//current_dynode mtypes befor current mtyp
    }
  }
//----
  else{//ANTI
    assert(bar<(MAXANTI+2) && mtyp<(ANAMTS+ANAGMTS));
    if(bar>=MAXANTI)assert(mtyp>(ANAMTS-1));
//
    swch+=totswch[0];//ANTI-channels i count after TOF ones
//
    if(bar<MAXANTI){//normal bar
//count in bars befor current one:
      swch+=(bar*2*ANAMTS);
//count in sides befor current one:
      if(side==1)swch+=ANAMTS;//mtypes in prev.side
      swch+=mtyp;//add mtypes befor the current mtyp
    }
    else{//half-cylinder
//count in bars befor current one:
      swch+=(MAXANTI*2*ANAMTS+(bar-MAXANTI)*2*ANAGMTS);
//count in sides befor current one:
      if(side==1)swch+=ANAGMTS;//mtypes in prev.side
      swch+=(mtyp-ANAMTS);//add mtypes befor the current mtyp
    }
  }
  return swch;//0,1,...
}
//-----------------
int16u AMSSCIds::hwseqn(int16u crate, int16u slot, int16u rdch){
//crate=0,1,...;slot=0,1,...;rdch=0,1,...
// calc. global sequential hw-channel(0,1,...;incl.empty):
  int16u hwch(0),slt;
//
  for(int cr=0;cr<crate;cr++){//1st->count in crates befor current one
    for(int sl=0;sl<SCSLTM;sl++){
      slt=sltymap[cr][sl];
      hwch+=ochpsty[slt-1];
    }
  }
  for(int sl=0;sl<slot;sl++){//2nd->count in slots befor current one(in crate crate)
    slt=sltymap[crate][sl];
    hwch+=ochpsty[slt-1];
  }
  hwch+=rdch;//3rd->count in current slot
  return hwch;//0,1,...
} 
//-----------------
int AMSSCIds::hw2swid(int16u crate, int16u slot, int16u rdch){
//crate=0,1,...;slot=0,1,...;rdch=0,1,...
// transform h/w-id(address) into s/w-id:
  int16u hwch;
  int swid;
  hwch=hwseqn(crate,slot,rdch);
  swid=sidlst[hwch];
  return swid;
//
}



