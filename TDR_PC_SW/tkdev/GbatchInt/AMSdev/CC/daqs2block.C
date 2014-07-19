//  $Id: daqs2block.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// 1.0 version 2.07.97 E.Choumilov
// AMS02 version 7.11.06 by E.Choumilov : TOF/ANTI RawFormat preliminary decoding is provided
#include "typedefs.h"
#include <iostream.h>
#include "link.h"
#include <fstream.h>
#include "cern.h"
#include "commons.h"
#include "event.h"
#include <time.h>
#include "timeid.h"
#include "tofdbc02.h"
#include "tofid.h"
#include "daqs2block.h"
#include "tofrec02.h"
#include "tofsim02.h"
#include "antidbc02.h"
#include "antirec02.h"
#include "tofcalib02.h"
#include "anticalib02.h"
//
using namespace TOF2GC;
using namespace ANTI2C;
//
int16u DAQS2Block::format=0; // default format (raw)
//
int16u DAQS2Block::nodeids[2*TOF2GC::SCCRAT]=//valid SC_node(SDR) id(link#)(*2 due to 2 halfs) 
  {
    14,16,18,20, //  a-sides, crates 1->4 
    15,17,19,21  //  b-sides, ...........
  };
//
integer DAQS2Block::totbll=0;
//
// functions for S-blocks class:
//
void DAQS2Block::node2crs(int16u nodeid, int16u &crat, int16u &sid){
//  on input: nodeid=(blid&0x001F)
//  on output: crate=1-4, =0 if ID is not found; side=(1,2) -> (a,b)
  crat=0;
  sid=0;
  for(int i=0;i<2*TOF2GC::SCCRAT;i++){
    if(nodeid==nodeids[i]){
      crat=int16u(i%4)+1;//seq.crate# 1,...4
      sid=int16u(floor(geant(i)/4)+1);//side# 1,2(a,b)
    }
  }
}
//
integer DAQS2Block::checkblockid(int16u blid){//have to be redefined !!!
  int valid(0);
  for(int i=0;i<2*TOF2GC::SCCRAT;i++)if((blid & (0x001F)) == nodeids[i])valid=1;
  return valid;
}
//-------------------------------------------------------
void DAQS2Block::buildraw(integer leng, int16u *p){
// it is implied that event_fragment is SDR2's one (checked in calling procedure)
// on input: len=tot_block_length as given at the beginning of block
//           *p=pointer_to_beggining_of_block_data(i.e. to len-word) ?????
// Length counting does not includes length-word itself !!! 
//
  integer i,j,il,ib,is,ic,icn,nh,lent,im,irl1,irl2,irs1,irs2;
  int swid,swidn,pmmx,hwid;
  int16u sswid,sswidn,shwid,shwida,shwidd;
  int16u mt,pmt,mtyp,swch,bpnt,rdch,inch,slot,crat,slid,csid,val16,tsens;
  int16u blid,dtyp,naddr,datyp,len,lenraw(0),lencom(0),sdrlen,formt,evnum;
  int16u word,nword,nnword,lastw,bias,dlink,plink,tostim,toscha,parerr,fmt;
  int16u osbit,wtyp,tdcerr(0);
  int16u tdcerfl,mxtw,hts2tdc(0);
  int16u iw,qchmsk,qlowchf;
  int16u biasmx;
//
  int16u rfmttrf(0);//raw-fmt truncation flag
  int16u ltmoutf(0);//SFET/SFEA/SFEC(link) time-out flags (from raw-eos or from compressed-part)
  int16u wcount(0);//sequencer Word-counter
  int16u svermsk(0);//stat.verification mask in TrPatt/Stat-block of ComprFMT 
// for onboard ped-cal tables:
  bool pedblk(false);//tempor:ped-calib_data flag(shoulld be passed here from header !!!)
  geant ped,sig,pedc,sigc;
  int16u sta,stac,nblkok;
  AMSTimeID *ptdv;
  time_t begin,end,insert,BeginTime,CurTime;
// for PedCalTable(onboard calib)
  integer static FirstPedBlk(0);
  integer static TotPedBlks(0);
  integer static PedBlkCrat[SCCRAT]={0,0,0,0};
  bool PedBlkOK(false);
// for classic ped-run events or for DownScaled events
  bool PedCal(false);//means PedCal job, using event-by-event in RawFMT or DownScaled 
  bool DownScal(false);//tempor:  how to recognize it ???
  static int FirstDScalBlk(0);
//  int16u PedSubt[SCCRAT]={1,1,1,1};//0/1->no/yes PedSubtr at RawEvent creation
  int16u PedSubt[SCCRAT]={0,0,0,0};//temporTest 0/1->no/yes PedSubtr at RawEvent creation
//
  int16u tdcbfn[SCFETA];//buff. counters for each TDC(link# 1-5)
  int16u tdcbfo[SCFETA];//TDC-buff OVFL FLAGS
  uinteger tdcbfh[SCFETA][8*SCTHMX2+4];//Raw-fmt TDC-buff,"+4" to count temper,header,error and trailer words
  uinteger wds2tdc,wttem,wthed,wterr,wttrl,tem1,tem2,htime;
  int16u nwtdcb,eventID,eventIDt,bunchID,ltedge,ntimbh;
  bool tmout;
//
  int16u *pr;
  integer bufpnt(0),lbbs;
  uinteger val32;
  geant temp,charge;
//
  int16u eoslenr(10);//length of end-of-segment record for raw format
//
  uinteger swcbuf[2*SCRCMX][SCTHMX2];
// keep: 1st half->LT/FT/HT-time+ampl values for raw/compr. fmt; 2nd->for raw if mixed fmt   
  integer  swnbuf[2*SCRCMX],swibuf[2*SCRCMX],hwibuf[2*SCRCMX];
//keep:     LT/FT/HT hits#      swids=LBBSPM         hwids=CSSRR   for raw format
// these buffers will be filled with TOF/ANTI sw-channels hits in current crate as 
// arrays of sequential sw-channel (number of hits in each channel will be ovfl-protected !!!).
// static Tof2JobStat temperature-array is also filled;
// Also filled at TOF(ANTI)RawEvent objects creation the following static arrays:
// TOF2RawSide::FTtime[SCCRAT][SCFETA][SCTHMX1];//FT-channel time-hits(incl.ANTI)
// TOF2RawSide::FThits[SCCRAT][SCFETA];//number of FT-channel hits ....... 
// TOF2RawSide::SumHTt[SCCRAT][SCFETA-1][SCTHMX2];//TOF SumHT-channel time-hits
// TOF2RawSide::SumHTh[SCCRAT][SCFETA-1];// number of SumHT-channel time-hits 
// TOF2RawSide::SumSHTt[SCCRAT][SCFETA-1][TOF2GC::SCTHMX2];//TOF SumSHT-channel time-hits
// TOF2RawSide::SumSHTh[SCCRAT][SCFETA-1];// number of SumSHT-channel time-hits
//-----
//-----
  TOF2JobStat::daqsfr(0);//count entries
  len=*p;//fragment's 1st word(block length not including length word itself)
  blid=*(p+len);// fragment's last word: Status+slaveID
//  cout<<"    blid="<<hex<<blid<<dec<<endl;
  bool dataf=((blid&(0x8000))>0);//data-fragment
  bool crcer=((blid&(0x4000))>0);//CRC-error
  bool asser=((blid&(0x2000))>0);//assembly-error
  bool amswer=((blid&(0x1000))>0);//amsw-error   
  bool timoer=((blid&(0x0800))>0);//timeout-error   
  bool fpower=((blid&(0x0400))>0);//FEpower-error   
  bool seqer=((blid&(0x0400))>0);//sequencer-error
  bool cdpnod=((blid&(0x0020))>0);//CDP-node(like SDR2-node with no futher fragmentation)
  bool noerr;
  naddr=blid&(0x001F);//slaveID(="NodeAddr"=SDR_link#)(14-21)
  datyp=(blid&(0x00C0))>>6;//0,1,2,3
//cout<<"====> InBuildRaw: len="<<len<<" data-type:"<<datyp<<" node-addr:"<<naddr<<endl;
  if(datyp>0 && len>1){
    if(pedblk)TOF2JobStat::daqsfr(8);//<=== count non-empty fragments of PedTable-type
    else TOF2JobStat::daqsfr(1+datyp);//<=== count non-empty fragments of given DATA-type
  }
  else goto BadExit;
  noerr=(dataf && !crcer && !asser && !amswer 
                                       && !timoer && !fpower && !seqer && cdpnod);
  if(noerr){
    if(pedblk)TOF2JobStat::daqsfr(9);//<=== count non-empty fragments of PedTble-type
    else TOF2JobStat::daqsfr(4+datyp);//<=== count no-errors fragments for given DATA-type
  }     
  else goto BadExit;
  node2crs(naddr,crat,csid);//get crate#(1-4),card_side(1-2<->a-b)
//---------
  if(TFREFFKEY.relogic[0]==5
             || ATREFFKEY.relogic==2)PedCal=true;//tempor(use later info on ClassPedData presence from header ?)
  if(!PedCal && (TFREFFKEY.relogic[1]==5 || ATREFFKEY.relogic==2)){
    cout<<"DAQS2Block::buildraw-W-Not ClassicPedCalibData when classic PedCal job is requested !!!"<<endl;
    return;
  }
//
  if(TFREFFKEY.relogic[0]==6 || ATREFFKEY.relogic==3)PedCal=true;// DownScaledEvents PedCalib job is requested 
//---------
  if(datyp==1)setrawf();
  else if(datyp==2)setcomf();
  else if(datyp==3)setmixf();
  if(pedblk)setpedf();//tempor: pedblk flag is known only from header and should be passed here
  formt=getformat();//0/1/2/3->raw/compr/mixt/onboard_pedcal_table
//cout<<"    format="<<formt<<" crate="<<crat<<endl;     
  if(formt<=1)TOF2JobStat::daqscr(formt,crat-1,0);//count crate-entries with stand-alone  raw/comp format
  else if(formt==2){//mix.form
    TOF2JobStat::daqscr(0,crat-1,0);//count crate-entries with raw format
    TOF2JobStat::daqscr(1,crat-1,0);//count crate-entries with compr. format
  }
  else TOF2JobStat::daqscr(2,crat-1,0);//=3 -> count crate-entries with pedcal_table format
  totbll+=len;//summing to have event(scint) data length
//
//-----
// reset buffers (here SCRCMX is used as max. number(even higher) of sw-channels)
  for(i=0;i<2*SCRCMX;i++){
    swnbuf[i]=0;
    swibuf[i]=0;
    hwibuf[i]=0;
    for(j=0;j<SCTHMX2;j++)swcbuf[i][j]=0;
  }
//
  if(formt==2)bufpnt=SCRCMX;//pointer to 2nd half of decoded buffer to store Raw format
// data if mixed format; if non-mixed format - decoded data are stored in 1st half of the buffer
//----- 
#ifdef __AMSDEBUG__
  if(TFREFFKEY.reprtf[1]>=1 || ATREFFKEY.reprtf[1]>=1){
    cout<<"-----------------------------------------------------------"<<endl;
    cout<<"   DAQS2Block::decoding: Segment_id="<<hex<<blid<<dec<<endl; 
    cout<<"      node_addr(link#)="<<naddr<<" data_type(fmt)="<<datyp<<" crate="<<crat<<endl;
    cout<<"      block_length="<<len<<endl<<endl;
//
    cout<<"  Block hex/binary dump follows :"<<endl<<endl;
    int16u tstw,tstb;
    for(i=0;i<len+1;i++){
      tstw=*(p+i);
      cout<<hex<<setw(4)<<tstw<<"  |"<<dec;
      for(j=0;j<16;j++){
        tstb=(1<<(15-j));
        if((tstw&tstb)!=0)cout<<"x"<<"|";
        else cout<<" "<<"|";
      }
      cout<<endl;
    }
    cout<<dec<<endl<<endl;
  }
#endif
//============================> "On_Board_PedCalib_Table" processing (if present):
  bias=1;//tempor: here len=90x3+1(blid)
  if(formt==3 && (TFREFFKEY.relogic[0]==7 || ATREFFKEY.relogic==4)){
    TOF2JobStat::daqscr(2,crat-1,0);// PedCalFormat entries/crate
    if(FirstPedBlk==0){
//      BeginTime=AMSEvent::gethead()->gettime();
      TOFPedCalib::BTime()=AMSEvent::gethead()->gettime();
      TOFPedCalib::BRun()=AMSEvent::gethead()->getrun();
      if(TFREFFKEY.relogic[0]==7)TOFPedCalib::resetb();
      if(ATREFFKEY.relogic==4)ANTPedCalib::resetb();
    }
    if((len-1)!=(90*3)){//
      TOF2JobStat::daqscr(2,crat-1,1);// PedCalFormat length error
      goto BadExit;    
    }
    PedBlkOK=true;
    while(bias<(len-1)){
      word=*(p+bias);//ped
      nword=*(p+bias+1);//sig
      nnword=*(p+bias+2);//stat
      slid=(bias-1)%9+1;//position(in block)  defined link# (1,...9)
      val16=word&(0x0FFF);// charge value (=0, if link problem)
      slot=AMSSCIds::crdidc2sl(crat-1,slid)+1;//slot-id to abs.slot-number(solid,sequential, 1,...,11)
      if(slot<=0 || slot==1 || slot==4 || slot>11){//check slot# validity
#ifdef __AMSDEBUG__
	cout<<"DAQS2Block::Error:PedCal_InvalidSlot , crat/slot_id/slot="<<crat<<" "<<slid<<" "<<slot<<endl;
#endif
	TOF2JobStat::daqscr(2,crat-1,2);//invalid slot number
	goto BadExit;    
      }
      inch=int16u(floor(float(bias-1)/9)+1);// slot's input channel(1,...10)
      mtyp=1;//for subr. ich2rdch only mtyp=0/1 has meaning(==>time/charge measuring channel)
      rdch=AMSSCIds::ich2rdch(crat-1,slot-1,inch-1,mtyp);//outp(=readout) ch:0(if empty),1,2...
      ped=geant(word&0xFFFF)/4;//tempor
      sig=geant(nword&0xFFFF)/4;//tempor
      sta=nnword;//tempor 1/0->bad(empty)/ok
      if(rdch>0){//<-- valid, not empty channel
        AMSSCIds scinid(crat-1,slot-1,rdch-1);
        swid=scinid.getswid();
        mtyp=scinid.getmtyp();//0->LTtime, 1->Q, 2->FT, 3->sumHT, 4->sumSHT
        pmt=scinid.getpmt();//0->anode, 1-3->dynode#
        il=swid/100000;//0,1,2,3,4
        mtyp=swid%10;
        if(il==0)dtyp=2;//anti
        else{
          dtyp=1;//tof
          il-=1;
        }
        ib=(swid%100000)/1000-1;//0,1,..
        is=(swid%1000)/100-1;//0,1
//---
        if(dtyp==1 && TFREFFKEY.relogic[0]==7){//TOFped-processing
	  TOFPedCalib::filltb(il, ib, is, pmt, ped, sig, sta);//store values
	}
//---
        if(dtyp==2 && ATREFFKEY.relogic==4){//ANTIped-processing
	  ANTPedCalib::filltb(ib, is, ped, sig, sta);//store values
	}
      }//--->endof "rdch>0" check
      bias+=3; 
    }//--->endof ped-block loop
    if(PedBlkOK)PedBlkCrat[crat-1]=1;//mark good processed crate
    TotPedBlks+=1;//counts tot# of processed PedBlocks
    FirstPedBlk=1;
//--- declare(make) DB-updates if last :
    if(TotPedBlks==4){//<---last block processed
      nblkok=0;
      for(i=0;i<4;i++)if(PedBlkCrat[i]==1)nblkok+=1;
      if(nblkok==4){//all blocks OK
        if(TFREFFKEY.relogic[0]==7){
	  TOFPedCalib::outptb(TFCAFFKEY.pedoutf);//0/1/2->only_histos/write2db+file/write2file
	}
	if(ATREFFKEY.relogic==4){
	  ANTPedCalib::outptb(ATCAFFKEY.pedoutf);//0/1/2->only_histos/write2db+file/write2file
	} 
      }//--->endof "all 4 blocks OK"
      TotPedBlks=0;//be ready for next calib.blocks sequence
      FirstPedBlk=0;
      for(i=0;i<SCCRAT;i++)PedBlkCrat[i]=0;
    }//---<endof "last PedBlock processed"
    return;
  }//--->endof formt=3(ped-block)
//-----------------------------------------------------------------------------------------------------
// !!!!!! Warning: Switching-Off of PedSuppression(for classic PedRun or DownScalled events) 
//  does not(?) automatically means usage of RAW-format (Format may still be Compressed(addr+value)???)
//-----------------------------------------------------------------------------------------------------
//
  if(formt==0 || formt==2){// <=========================  RawFMT is present - process it:
//bias_min=1 (pr+bias_min always points to 1st word of raw (sub-)segment) (i.e. 1st word of Q-block !!!)
// pr+bias_max always points to wcount-word (last word of raw (sub-)segment) 
    evnum=*(p+1);//event number
    if(formt==0){//(0) raw alone
      pr=p+1;//points to evnum (next word is beginning of Q-blk)
      biasmx=len-2;//bias_max (-2 to exclude evnum and blid words)
    }
    else{//(2) raw in mixt
      pr=p+2;//points to lenraw-word (next word is beginning of Q-blk)
      lenraw=*pr;
      biasmx=lenraw;//bias_max
//cout<<" MixtFMT:RawSubSegm length="<<lenraw<<endl;
    }
//cout<<" decoded RawSubSegm data will be put in glob.buffer starting from location="<<bufpnt<<endl; 
    wcount=*(pr+biasmx);//raw-subsegm words counter 
//cout<<" RawSubSegm_WordCounter(hex)="<<hex<<wcount<<dec<<" biasmx="<<biasmx<<endl;
    if((wcount&(0x8000))!=0){//raw-subsegm. is truncated
      TOF2JobStat::daqscr(0,crat-1,1);// RawForm max.length ovfl
      rfmttrf=1;//set raw-subseg truncation flag
    }
    wcount=(wcount&(0x7FF));//counts raw-subseg length=lenraw-evnum-itself(ovfl-bit removed)
//    cout<<"    wcount(dec)="<<wcount<<endl;  
    if(biasmx!=(wcount+1)){//<---length mismatch check,"+1" stay because wcount not incl. itself
      TOF2JobStat::daqscr(0,crat-1,2);// RawForm length error
      goto BadExit;    
    }
//
    for(i=0;i<SCFETA;i++){
      tdcbfn[i]=0;//clear TDC-buff counters
      tdcbfo[i]=0;//clear TDC-buff ovfl flags
    }
//
//-------------> start raw-subsegment processing:
//
    bias=1;//bias for RAW-pointer(i.e. to current rawdata-word, now 1st raw-DATA word (next to evnum))
    while(bias<=biasmx){//<--- loop over raw-subsegment words(excl. lenraw,evnum words)
      word=*(pr+bias);
//      cout<<" --> word/bias="<<hex<<word<<dec<<" "<<bias<<endl;
//-----   
      if(bias<=90){//<================= charge-words reading(incl the ones in SFET/SFEA !!!)
	dlink=(word&(0xF000))>>12;//decoded link# (1,...)
	slid=dlink;
	plink=(bias-1)%9+1;//"in block" position defined link# (1,...9)
//	if(dlink!=plink || slid>9){tempor removed because link numbering disorder
	if(slid>9){
#ifdef __AMSDEBUG__
	  cout<<"DAQS2Block::Error:link<->position mismatch, crate/dlink/plink="<<crat<<" "<<dlink<<" "<<plink<<endl;
#endif
          TOF2JobStat::daqscr(0,crat-1,3);//charge_link <-> position mismatch
	  goto BadExit;    
	}
	val16=word&(0x0FFF);// charge value (=0, if link problem)
	slot=AMSSCIds::crdidc2sl(crat-1,slid)+1;//slot-id to abs.slot-number(solid,sequential, 1,...,11)
	if(slot<=0 || slot==1 || slot==4 || slot>11){//check slot# validity
#ifdef __AMSDEBUG__
	  cout<<"DAQS2Block::Error:NotChargeSlotNumber , crat/slot_id/slot="<<crat<<" "<<slid<<" "<<slot<<endl;
#endif
	  TOF2JobStat::daqscr(0,crat-1,4);//invalid slot number
	  goto BadExit;    
	}
	TOF2JobStat::daqssl(0,crat-1,slot-1,0);//count legal charge-slots
	inch=int16u(floor(float(bias-1)/9)+1);// slot's input channel(1,...10)
        mtyp=1;//charge
	rdch=AMSSCIds::ich2rdch(crat-1,slot-1,inch-1,mtyp);//outp.ch:0(if empty),1,2...
	if(rdch>0 && val16>0){//<-- valid, not empty channel
//	  TOF2JobStat::daqsch(0,crat-1,slot-1,rdch-1,0);//count charge channels entries
	  TOF2JobStat::daqsch(0,crat-1,slot-1,inch-1,0);//count charge channels entries
          AMSSCIds scinid(crat-1,slot-1,rdch-1);
//          mtyp=scinid.getmtyp();//0->LTtime, 1->Q, 2->FT, 3->sumHT, 4->sumSHT
//          pmt=scinid.getpmt();//0->anode, 1-3->dynode#
          swch=scinid.getswch();//seq. sw-channel (0,1,...)
	  swid=scinid.getswid();
	  hwid=scinid.gethwid();
	  bpnt=bufpnt+swch;//swch true address in the buffer(buff.half directed)
	  if(swnbuf[bpnt]<1){
	    swcbuf[bpnt][swnbuf[bpnt]]=val16;//store value
	    swibuf[bpnt]=swid;
	    hwibuf[bpnt]=hwid;
	    swnbuf[bpnt]+=1;//increase counter of given swch
	  }
	  else{//should not happen, normally 1hit/channel
#ifdef __AMSDEBUG__
	    cout<<"DAQS2Block::RawFmtErr:ChargeHits > 1 for hwid/swid="<<hwid<<" "<<swid<<endl;
#endif
	  }
	}//--->endof empty chan. check
	bias+=1;
      }//--->endof charge words reading
//-----
      else if(bias>90 && bias<=94){//<===== trig.patt words reading
//        cout<<"  TP-decoding:"<<endl;
        tmout=0;
        for(i=0;i<4;i++){//check presence of TrPat-type info
          word=*(pr+bias+i);
	  if((word&(0xF800))!=0 && (word&(0xF800))!=0xF800){//5 msb should be 0 or 1(timeout)
	    TOF2JobStat::daqscr(0,crat-1,5);//notTrPat word in TrPat-area
            goto BadExit;
	  }
	  else if((word&(0xF800))==0xF800){
	    tmout=1;
	    *(pr+bias+i)=0;//reset pattern
	  }
	}
	if(tmout==1){
	  TOF2JobStat::daqscr(0,crat-1,6);//time-out in TrPatt-block
	  goto SkipTPpr;//skip TP-processing
	}
        word=*(pr+bias);
        nword=*(pr+bias+1);
	val32=uinteger(nword&(0x07FF));//HT-trigpatt least sign.bits
	val32|=(uinteger(word&(0x007F))<<11);//add HT-trigpatt most sign.bits
//	cout<<"   1st 16bits paire was combined into:"<<hex<<val32<<dec<<endl;
	for(i=0;i<18;i++){//HT-trigpatt bits loop
	  lbbs=AMSSCIds::gettpbas(crat-1,i);
	  if(lbbs>0){//valid lbbs for given bit
	    is=lbbs%10;//plane side(1,2)
	    ib=(lbbs/10)%100;//paddle(1-8(10))
	    il=lbbs/1000;//plane(1-4)
	    if(i==3){//use "middl.bit" to save il/is of 1st layer(side) contributing to given crate
	      irl1=il;
	      irs1=is;
	    } 
	    if(i==11){//use "middle.bit" to save il/is of 2nd layer(side) contributing to given crate
	      irl2=il;
	      irs2=is;
	    }
	    if((val32&(1<<i))>0)
	      TOF2RawSide::addtpb(il-1,ib-1,is-1);//set bit in RawSide's TPs(reseted in retof2initevent() !
	  }
	}
	osbit=(word&(0x0080));//other card-side CT-bit of 1st layer(side)
	if(osbit>0)TOF2RawSide::addtpb(irl1-1,13,irs1-1);//set as fictitious paddle-14
	osbit=(word&(0x0100));//other card-side CT-bit of 2nd layer(side)
	if(osbit>0)TOF2RawSide::addtpb(irl2-1,13,irs2-1);//set as fictitious paddle-14
	osbit=(word&(0x0200));//other card-side CP-bit of 1st layer(side)
	if(osbit>0)TOF2RawSide::addtpb(irl1-1,12,irs1-1);//set as fictitious paddle-13
	osbit=(word&(0x0400));//other card-side CP-bit of 2nd layer(side)
	if(osbit>0)TOF2RawSide::addtpb(irl2-1,12,irs2-1);//set as fictitious paddle-13
//         
        word=*(pr+bias+2);
        nword=*(pr+bias+3);
	val32=uinteger(nword&(0x07FF));//SHT-trigpatt least sign.bits
	val32|=(uinteger(word&(0x007F))<<11);//add SHT-trigpatt most sign.bits
//	cout<<"   2nd 16bits paire was combined into:"<<hex<<val32<<dec<<endl;
	for(i=0;i<18;i++){//SHT-trigpatt bits loop
	  lbbs=AMSSCIds::gettpbas(crat-1,i);
	  if(lbbs>0){
	    is=lbbs%10;//plane side(1,2)
	    ib=(lbbs/10)%100;//paddle(1-8(10))
	    il=lbbs/1000;//plane(1-4)
	    if(i==3){//save il/is of 1st layer(side) contributing to given crate
	      irl1=il;
	      irs1=is;
	    } 
	    if(i==11){//save il/is of 2nd layer(side) contributing to given crate
	      irl2=il;
	      irs2=is;
	    }
	    if((val32&(1<<i))>0)TOF2RawSide::addtpzb(il-1,ib-1,is-1);//SHT trigpatt bits setting
	  }
	}
	osbit=(word&(0x0080));//other card-side BZ-bit of 1st layer(side)
	if(osbit>0)TOF2RawSide::addtpzb(irl1-1,12,irs1-1);//set as fictitious paddle-13
	osbit=(word&(0x0100));//other card-side BZ-bit of 2nd layer(side)
	if(osbit>0)TOF2RawSide::addtpzb(irl2-1,12,irs2-1);//set as fictitious paddle-13
//
SkipTPpr:
        bias+=4;
      }//--->endof tr.pat.words reading
//-----
      else if(bias>94 && bias<=(biasmx-eoslenr)){//<===== time(+temper) words reading
//        cout<<"  Temp+Time-decoding:"<<endl;
        nword=*(pr+bias+1);
	dlink=(word&(0xF00))>>8;//decoded link# (0,...4)
	wtyp=((word&(0xF000))>>12);
	slid=dlink;//0,1,2,3,4 => SFET_0,_1,_2,_3,SFEA
	slid+=1;//for backw.compartib.
	if(slid>5){
#ifdef __AMSDEBUG__
	  cout<<"DAQS2Block::TimeBlErr: invalid link, crate/dlink="<<crat<<" "<<dlink<<endl;
#endif
          TOF2JobStat::daqscr(0,crat-1,7);//invalid link number
	  goto BadExit;    
        }
	slot=AMSSCIds::crdidt2sl(crat-1,slid-1)+1;//slot-id to slot-number(solid,sequential, 1,...,11)
	if(slot<=0 || slot==1 || slot==4 || slot>7){//check slot# validity
#ifdef __AMSDEBUG__
	  cout<<"DAQS2Block::TimeBlErr: invalid slot, crat/slot_id/slot="<<crat<<" "<<slid<<" "<<slot<<endl;
#endif
	  TOF2JobStat::daqscr(0,crat-1,8);//invalid slot number
	  goto BadExit;    
	}
	if(wtyp==8 || wtyp==2 || wtyp==3 || wtyp==4 ||wtyp==5){//time(+temper) wtype ?
	  TOF2JobStat::daqssl(0,crat-1,slot-1,1);//count TDC-buff entries(pairs of 16bits words)
	  if(tdcbfn[slid-1]<(8*SCTHMX2+4)){//fill TDC-buffer
	    val32=(uinteger(word)<<16);//msb
	    val32|=uinteger(nword);//lsb
	    tdcbfh[slid-1][tdcbfn[slid-1]]=val32;//store 32-bits word in TDC-buff
	    tdcbfn[slid-1]+=1;
	  }
	  else{//TDC-buff overflow
#ifdef __AMSDEBUG__
	    cout<<"DAQS2Block::TimeErr: TDC-buff Ovfl, crate/link="<<crat<<" "<<slid<<endl;
#endif
	    if(tdcbfo[slid-1]==0)TOF2JobStat::daqssl(0,crat-1,slot-1,2);//counts Events with TDC-buff ovfl
	    tdcbfo[slid-1]=1;
	  }
	}
	else{
#ifdef __AMSDEBUG__
	  cout<<"DAQS2Block::TimeErr: InvalidWordType, crate/link/wtyp="<<crat<<" "<<slid<<" "<<wtyp<<endl;
#endif
	  TOF2JobStat::daqssl(0,crat-1,slot-1,3);//count  invalid wtyp
	  goto BadExit;    
	}
	bias+=2;
      }//--->endof time(+temper)-words reading block
//-----
      else{//<-- SeqStatusBlock words reading
        ltmoutf=word;//link time-out flags
//cout<<"RawSubSegm:SeqStatusBlock: timeout_flags word="<<hex<<ltmoutf<<" wcount_word="
//                                  <<*(pr+bias+eoslenr-1)<<dec<<" bias="<<bias<<endl;
        bias+=eoslenr;//to skip the rest of eos-record;
      }
//------
    }//--->endof raw-subsegment words reading loop
//-------------> now start TDC-buffers inspection and decoding:
    for(slid=1;slid<=SCFETA;slid++){//<---loop over TDC-buffers
      wttem=0;
      wthed=0;
      wterr=0;
      wttrl=0;
      wds2tdc=0;
      tdcbfo[slid-1]=0;
      nwtdcb=tdcbfn[slid-1];//number of filled 32b-words in TDC(slid)
      if(nwtdcb==0)continue;//empty buffer->skip it
      wttem=((tdcbfh[slid-1][0]&(0xF0000000L))>>28);//wtyp of 1st word
      if(nwtdcb>1){
        wthed=((tdcbfh[slid-1][1]&(0xF0000000L))>>28);//wtyp of 2nd word
	wterr=((tdcbfh[slid-1][nwtdcb-2]&(0xF0000000L))>>28);//wtyp of prev. to last word
      }
      wttrl=((tdcbfh[slid-1][nwtdcb-1]&(0xF0000000L))>>28);//wtyp of last word
      if(wttrl==3)wds2tdc=(tdcbfh[slid-1][nwtdcb-1]&(0xFFFL));//nwords given by trailer(last word)
      tmout=((ltmoutf&(1<<(slid-1)))>0);//time-out flag from eos
      slot=AMSSCIds::crdidt2sl(crat-1,slid-1)+1;//slot-id to slot-number(solid,sequential, 1,...,11)
      if(wttem!=8 || wthed!=2 || wttrl!=3 || tmout || nwtdcb!=(wds2tdc+1)){//broken structure
        TOF2JobStat::daqssl(0,crat-1,slot-1,4);//count links with broken struct
	continue;//skip link(TDC) with broken structure (or time-out)
      }
      if(wterr==6){
        TOF2JobStat::daqssl(0,crat-1,slot-1,5);//count links with TDC-error
	continue;//skip link(TDC) with internal error
      }
      TOF2JobStat::daqssl(0,crat-1,slot-1,6);//count good links
//
      val32=tdcbfh[slid-1][0];//lst word(temperature)
      tem1=((val32&(0xFFF000L))>>12);
      tem2=(val32&(0xFFFL));
      temp=0;
      if(tem2>0)temp=geant(tem1)/geant(tem2);//tempor : missing calibration formula
      tsens=AMSSCIds::sl2tsid(slot-1);//seq.slot#->temp.sensor#; 1,2,3,4,5
      TOF2JobStat::puttemp(crat-1,tsens-1,temp);
//
      val32=tdcbfh[slid-1][1];//2nd word(header)
      eventID=int16u((val32&(0xFFF000L))>>12);
      bunchID=int16u((val32&(0xFFFL)));
      val32=tdcbfh[slid-1][nwtdcb-1];//last word(trailer)
      eventIDt=int16u((val32&(0xFFF000L))>>12);
      ntimbh=nwtdcb-3;//number of time-hits(all channels in current TDC)
      for(int ih=0;ih<ntimbh;ih++){//<---loop over all time-hits
        val32=tdcbfh[slid-1][ih+2];//time-word
	inch=int16u((val32&(0xE00000L))>>21);//inp.channel#(0-7)
	htime=((val32&(0x7FFFFL))<<2);//19 msb
	htime|=((val32&(0x180000L))>>19);//add 2 lsb
	mtyp=0;//"0" for ich2rdch-routine means "any Time-ch like LT,FT,sHT,sSHT"
	rdch=AMSSCIds::ich2rdch(crat-1,slot-1,inch,mtyp);//outp.ch:0(if empty),1,2...
	if(rdch>0){//<-- not empty channel
          AMSSCIds scinid(crat-1,slot-1,rdch-1);//
//	  TOF2JobStat::daqsch(0,crat-1,slot-1,rdch-1,1);//counts time channels entries
	  TOF2JobStat::daqsch(0,crat-1,slot-1,inch,1);//counts time channels entries
          swch=scinid.getswch();//seq. sw-channel (0,1,...)
	  swid=scinid.getswid();
	  bpnt=bufpnt+swch;//swch true address in the buffer(buff.half directed)
	  hwid=scinid.gethwid();
	  if(swnbuf[bpnt]<SCTHMX2){
	    swcbuf[bpnt][swnbuf[bpnt]]=htime;//store time-value
	    swibuf[bpnt]=swid;
	    hwibuf[bpnt]=hwid;
	    swnbuf[bpnt]+=1;//increase counter of given swch
	  }
	  else{
#ifdef __AMSDEBUG__
	    if(tdcbfo[slid-1]==0)cout<<"DAQS2Block::Error:SWCH_Buf_Ovfl, hwid/swid="<<hwid<<" "<<swid<<endl;
	    tdcbfo[slid-1]=1;
#endif
	    TOF2JobStat::daqsch(0,crat-1,slot-1,rdch-1,2);//count Nhit overflows in time channels
	  }
	}//--->endof "rdch>0" check
      }//--->endof time-hits-loop
    }//--->endof TDC-buffers loop
//-------------------------
    if(PedCal)PedSubt[crat-1]=0;//flag to subtr(1)/not(0) peds at RawEvent-creation(for Raw only by whole crate)
//------
//cout<<"*******> endof RawFMT-decoding !"<<endl;
  }//=========================> endof "RawFMT presence check/processing
//
//
//============================> ComprFMT processing (if present) :
//                     (always 1st half of the glob.buffer will be filled)
//
  if(formt>0){//<========== Mixt/ComprFMT check(pedcalFormat(3) is already processed)
//
    int16u *pc;
    int16u *pss;//sub-sect pointer (pss+bias always points2 1st word (when bias=1)of sub-sector)
    int16u nnzch,nthts,tlhead,tlerrf(0);
    int16u nqwrds(0),ntwrds(0);
//
    if(formt==2){//<============= ComprFMT is inside of MixtFMT
      TOF2JobStat::daqscr(1,crat-1,1);//ComprFMT entries inside MixedFMT(after RawFMT ok)
      pc=p+2+lenraw;//points to last raw-subsegm. word(wcount)
      if((len-lenraw-3)<=0){//this is compr-subsection length 
        TOF2JobStat::daqscr(1,crat-1,2);//count truncated_segments cases
	goto BadExit;    
      }
//        TOF2JobStat::daqscr(1,crat-1,3);//spare
      lencom=len-lenraw-3;//tot.length of compr.subgegment("-3" to excl. evnum,lenraw,blid words)
      nqwrds=*(pc+8)+1;//q-group words, "+1"->nwords-word itself
      ntwrds=*(pc+8+nqwrds)+1;//t-group wordfs,......
//cout<<" CompInMixFMT:lenraw/lencom="<<lenraw<<" "<<lencom<<" nQwrds/nTwrds="<<nqwrds<<" "<<ntwrds<<endl;
      if(lencom!=(7+nqwrds+ntwrds)){
        TOF2JobStat::daqscr(1,crat-1,4);//count length mismatch inside ComprFMT
	goto BadExit;    
      }
    }
    else{//<=============== ComprFMT is stand-alone
      pc=p+1;//points to evnum of stand-alone ComprFMT segment
      evnum=*pc;
      lencom=len-2;//tot.length of compressed subsegment(excl. evnum and (Stat+SlaveId)- word)
      nqwrds=*(pc+8)+1;//q-group words, "+1"->nwords-word itself
      ntwrds=*(pc+8+nqwrds)+1;//t-group wordfs,......
//cout<<" CompAloneFMT:lencom="<<lencom<<" nQwrds/nTwrds="<<nqwrds<<" "<<ntwrds<<endl;
      if(lencom!=(7+nqwrds+ntwrds)){
        TOF2JobStat::daqscr(1,crat-1,5);//count length mismatch inside ComprFMT
	goto BadExit;    
      }
    }
//                                                                        
//
//<==================== TrPatt/Status section:
//        cout<<"  ComprSegment::TrPatt/Status-decoding:"<<endl;
        pss=pc;
	bias=1;//pss+bias points2 1st word of TrPatt-section
        tmout=0;
        for(i=0;i<4;i++){//check presence of TrPat-type info
          word=*(pss+bias+i);
	  if((word&(0xF800))!=0 && (word&(0xF800))!=0xF800){//5 msb should be 0 or 1(timeout)
	    TOF2JobStat::daqscr(1,crat-1,6);//notTrPat word in TrPat-area
            goto BadExit;
	  }
	  else if((word&(0xF800))==0xF800){
	    tmout=1;
	    *(pss+bias+i)=0;//reset pattern
	  }
	}
	if(tmout==1){
	  TOF2JobStat::daqscr(1,crat-1,7);//time-out in TrPatt-block
	  goto SkipTPpr1;//skip TP-processing
	}
        word=*(pss+bias);
        nword=*(pss+bias+1);
	val32=uinteger(nword&(0x07FF));//HT-trigpatt least sign.bits
	val32|=(uinteger(word&(0x007F))<<11);//add HT-trigpatt most sign.bits
//	cout<<"   1st 16bits paire was combined into:"<<hex<<val32<<dec<<endl;
	for(i=0;i<18;i++){//HT-trigpatt bits loop
	  lbbs=AMSSCIds::gettpbas(crat-1,i);
	  if(lbbs>0){//valid lbbs for given bit
	    is=lbbs%10;//plane side(1,2)
	    ib=(lbbs/10)%100;//paddle(1-8(10))
	    il=lbbs/1000;//plane(1-4)
	    if(i==3){//use "middl.bit" to save il/is of 1st layer(side) contributing to given crate
	      irl1=il;
	      irs1=is;
	    } 
	    if(i==11){//use "middle.bit" to save il/is of 2nd layer(side) contributing to given crate
	      irl2=il;
	      irs2=is;
	    }
	    if((val32&(1<<i))>0)
	      TOF2RawSide::addtpb(il-1,ib-1,is-1);//set bit in RawSide's TPs(reseted in retof2initevent() !
	  }
	}
	osbit=(word&(0x0080));//other card-side CT-bit of 1st layer(side)
	if(osbit>0)TOF2RawSide::addtpb(irl1-1,13,irs1-1);//set as fictitious paddle-14
	osbit=(word&(0x0100));//other card-side CT-bit of 2nd layer(side)
	if(osbit>0)TOF2RawSide::addtpb(irl2-1,13,irs2-1);//set as fictitious paddle-14
	osbit=(word&(0x0200));//other card-side CP-bit of 1st layer(side)
	if(osbit>0)TOF2RawSide::addtpb(irl1-1,12,irs1-1);//set as fictitious paddle-13
	osbit=(word&(0x0400));//other card-side CP-bit of 2nd layer(side)
	if(osbit>0)TOF2RawSide::addtpb(irl2-1,12,irs2-1);//set as fictitious paddle-13
//         
        word=*(pss+bias+2);
        nword=*(pss+bias+3);
	val32=uinteger(nword&(0x07FF));//SHT-trigpatt least sign.bits
	val32|=(uinteger(word&(0x007F))<<11);//add SHT-trigpatt most sign.bits
//	cout<<"   2nd 16bits paire was combined into:"<<hex<<val32<<dec<<endl;
	for(i=0;i<18;i++){//SHT-trigpatt bits loop
	  lbbs=AMSSCIds::gettpbas(crat-1,i);
	  if(lbbs>0){
	    is=lbbs%10;//plane side(1,2)
	    ib=(lbbs/10)%100;//paddle(1-8(10))
	    il=lbbs/1000;//plane(1-4)
	    if(i==3){//save il/is of 1st layer(side) contributing to given crate
	      irl1=il;
	      irs1=is;
	    } 
	    if(i==11){//save il/is of 2nd layer(side) contributing to given crate
	      irl2=il;
	      irs2=is;
	    }
	    if((val32&(1<<i))>0)TOF2RawSide::addtpzb(il-1,ib-1,is-1);//SHT trigpatt bits setting
	  }
	}
	osbit=(word&(0x0080));//other card-side BZ-bit of 1st layer(side)
	if(osbit>0)TOF2RawSide::addtpzb(irl1-1,12,irs1-1);//set as fictitious paddle-13
	osbit=(word&(0x0100));//other card-side BZ-bit of 2nd layer(side)
	if(osbit>0)TOF2RawSide::addtpzb(irl2-1,12,irs2-1);//set as fictitious paddle-13
//
SkipTPpr1:
// decode status part:
        ltmoutf=*(pss+bias+4);//link's time-out flags(sequencer Timout flag as in rawFMT)
	wcount=*(pss+bias+5);//sequencer Counter (as in rawFMT)
	svermsk=*(pss+bias+6);//status verif.mask
	if((svermsk&(0x3FF))>0){//problems in st.verif.mask
	  TOF2JobStat::daqscr(1,crat-1,8);//StatVerifMask problems
//          goto BadExit;//tempor
	}
//
        pss+=7;//shift by 7 words, now pss points to last(7th) word of TrPatt-section
//=========>endof TrPatt/Status section
//
//<====================== Charge-section:
//cout<<"  ComprSegment::Qsection-decoding:"<<endl;
      bias=1;
// !!! here pss+bias points to nwords-word
      while(bias<nqwrds){//q-block words loop(nqwrds=1 if Kunin's nwords=0
	word=*(pss+bias+1);// current link header(+1 to bypass nwords-word)
	slid=(word&0x000F);
	qlowchf=0;
	if((word&(0x4000))>0)qlowchf=1;//set negat.(adc-ped) presence flag
	if((word&(0x8000))>0 && slid>0 && slid<=9){//header's marker,link# OK
	  slot=AMSSCIds::crdidc2sl(crat-1,slid)+1;//slot-id to abs.slot-number(solid,sequential, 1,...,11)
	  if(slot<=0 || slot==1 || slot==4 || slot>11){//check slot# validity
	    TOF2JobStat::daqscr(1,crat-1,10);//invalid slot number
	    goto BadExit;
	  }    
	  TOF2JobStat::daqssl(1,crat-1,slot-1,0);//count legal charge-slot entries
	  if(qlowchf)TOF2JobStat::daqssl(1,crat-1,slot-1,1);//count charge-slots with (adc-ped)<0
	  qchmsk=((word&(0x3FF0))>>4);//mask of "above-ped" channels
	  nnzch=0;
	  for(inch=0;inch<10;inch++){//<--- current_link input channels loop
	    if((qchmsk&(512>>inch))>0){//non-empty inp.channel
// now fill global buffer:
              mtyp=1;//charge
	      rdch=AMSSCIds::ich2rdch(crat-1,slot-1,inch,mtyp);//outp.ch:0(if empty),1,2...
	      val16=*(pss+bias+2+nnzch);//tempor imply that TDC-values(12bits) are not modified
	      if(rdch>0 && val16>0){//<-- valid, not empty channel
//	        TOF2JobStat::daqsch(1,crat-1,slot-1,rdch-1,0);//count charge channels entries
	        TOF2JobStat::daqsch(1,crat-1,slot-1,inch,0);//count charge channels entries
                AMSSCIds scinid(crat-1,slot-1,rdch-1);
//          mtyp=scinid.getmtyp();//0->LTtime, 1->Q, 2->FT, 3->sumHT, 4->sumSHT
//          pmt=scinid.getpmt();//0->anode, 1-3->dynode#
                swch=scinid.getswch();//seq. sw-channel (0,1,...)
	        swid=scinid.getswid();
	        hwid=scinid.gethwid();
	        bpnt=swch;//swch true address in the buffer(1st buff.half)
	        if(swnbuf[bpnt]<1){
	          swcbuf[bpnt][swnbuf[bpnt]]=val16;
	          swibuf[bpnt]=swid;
	          hwibuf[bpnt]=hwid;
	          swnbuf[bpnt]+=1;//increase counter of given swch
	        }
	        else{//should not happen, normally 1hit/channel
#ifdef __AMSDEBUG__
	          cout<<"DAQS2Block::CompFmtErr:ChargeHits > 1 for hwid/swid="<<hwid<<" "<<swid<<endl;
#endif
	        }
	      }//--->endof empty chan. check
              nnzch+=1;//counts hits
            }//--->endof "non-empty" inp.channel check
	  }//--->endof inp.channels loop
	  bias+=(nnzch+1);//+1 for link-header itself
	}//--->endof "link-header OK"
	else{
	  TOF2JobStat::daqscr(1,crat-1,9);//wrong link-header in Q-section
	  goto BadExit;    
	}
      }//--->endof q-block words loop
//
      pss+=nqwrds;//shift by nQwords, now pss points to last word of Q-section 
//============>endof Charge-section
//
//<====================== TempTime-section:
//
//cout<<"  ComprSegment::Tsection-decoding:"<<endl;
      bias=1;
      word=*(pss+bias);//nwords-word
//cout<<" Tblock nwords(hex)="<<hex<<word<<dec<<endl;
      if((word&(0x8000))>0 || (word&(0x4000))>0){
	TOF2JobStat::daqscr(1,crat-1,11);//was link# problems during Kunin's raw->comp conversion 
      }
      while(bias<ntwrds){//<---temp/time-block words loop
	tlhead=*(pss+bias+1);//current time-link header
	slid=(tlhead&(0x0007));//slot_id(link#) 0,1,2,3,4 => SFET_0,_1,_2,_3,SFEA
	nthts=((tlhead&(0x0FF0))>>4);//time-hits in current link
//cout<<" lhead="<<hex<<tlhead<<dec<<" slid/nthts="<<slid<<" "<<nthts<<endl;
	if((tlhead&(0x8000))==0 || (tlhead&(0x4000))==0 || (tlhead&(0x0008))>0){//
	  TOF2JobStat::daqscr(1,crat-1,12+slid);//fatal err during raw->comp
	  goto BadExit; 
	}
        slot=AMSSCIds::crdidt2sl(crat-1,slid)+1;//slot-id to slot-number(solid,sequential, 1,...,11)
	if(slot<=0 || slot==1 || slot==4 || slot>7){//check slot# validity
	  TOF2JobStat::daqscr(1,crat-1,17);//invalid slot number
	  goto BadExit;    
	}
	TOF2JobStat::daqssl(1,crat-1,slot-1,2);//count legal time-slot entries
//
	bias+=1;//to point to 1st tempr-word(if present), or to 1st word of the 1st time-hit otherwise
	if((tlhead&(0x2000))>0)nthts+=1;//consider temper. as additional(1st) time-hit
	else TOF2JobStat::daqssl(1,crat-1,slot-1,3);//count missing temperature cases
//cout<<"   new nthts="<<nthts<<endl;
//
	for(nh=0;nh<nthts;nh++){//<---hits loop
//cout<<"---> nh="<<nh<<" bias="<<bias<<endl;
	  word=*(pss+bias+1);
	  nword=*(pss+bias+2);
	  nnword=*(pss+bias+3);
//cout<<"     w/nw/nnw="<<hex<<word<<" "<<nword<<" "<<nnword<<dec<<endl;
	  if(nh%2==0){//1st,3rd,...hit
	    if(nh==0 && (tlhead&(0x2000))>0){//1st hit is the temperature - decode it
	      val32=(uinteger(word)<<8);//16 msb
	      val32|=(uinteger(nword)>>8);//8 lsb
              tem1=((val32&(0xFFF000L))>>12);
              tem2=(val32&(0xFFFL));
//cout<<"    temp="<<tem1<<" "<<tem2<<endl;
              temp=0;
              if(tem2>0)temp=geant(tem1)/geant(tem2);//tempor : missing calibration formula
              tsens=AMSSCIds::sl2tsid(slot-1);//seq.slot#->temp.sensor#; 1,2,3,4,5
              TOF2JobStat::puttemp(crat-1,tsens-1,temp);
	      continue;//temp-hit is not stored in swcbuf, so skip storing 
	    }
	    else{
	      inch=((word&(0xE000))>>13);//inp.ch# 0-7
	      val32=uinteger((nword&(0xFF00))>>8);// lsb
	      val32|=(uinteger(word&(0x1FFF))<<8);// msb
//cout<<"    inch/val32="<<inch<<" "<<val32<<endl;
	    }
	  }
	  else{//2nd,4th,...hit
	    inch=((nword&(0x00E0))>>5);//inp.ch# 0-7
	    val32=uinteger(nnword);// lsb
	    val32|=(uinteger(nword&(0x001F))<<16);// msb
//cout<<"    inch/val32="<<inch<<" "<<val32<<endl;
	    bias+=3;//now points to next "3x16bits" or to err-patt word(if any) or to next link-header
	  }
// now fill global buffer:
	  mtyp=0;//"0" for ich2rdch-routine means "any Time-ch like LT,FT,sHT,sSHT"
	  rdch=AMSSCIds::ich2rdch(crat-1,slot-1,inch,mtyp);//outp.ch:0(if empty),1,2...
	  if(rdch>0){//<-- not empty channel
            AMSSCIds scinid(crat-1,slot-1,rdch-1);//
//	    TOF2JobStat::daqsch(1,crat-1,slot-1,rdch-1,1);//counts time channels entries
	    TOF2JobStat::daqsch(1,crat-1,slot-1,inch,1);//counts time channels entries
            swch=scinid.getswch();//seq. sw-channel (0,1,...)
	    swid=scinid.getswid();
	    bpnt=swch;//swch true address in the buffer(1st buff.half)
	    hwid=scinid.gethwid();
	    htime=((val32&(0x7FFFFL))<<2);//19 msb
	    htime|=((val32&(0x180000L))>>19);//add 2 lsb
//cout<<"    time-2-buff="<<htime<<endl;
	    if(swnbuf[bpnt]<SCTHMX2){
	      swcbuf[bpnt][swnbuf[bpnt]]=htime;//store time-value
	      swibuf[bpnt]=swid;
	      hwibuf[bpnt]=hwid;
	      swnbuf[bpnt]+=1;//increase counter of given swch
	    }
	    else{
//	      TOF2JobStat::daqsch(1,crat-1,slot-1,rdch-1,2);//count time-channels hits ovfls
	      TOF2JobStat::daqsch(1,crat-1,slot-1,inch,2);//count time-channels hits ovfls
	    }
	  }//--->endof "rdch>0" check
//
	}//--->endof hits loop
//
	if((nthts%2)==1)bias+=2;//now points to err-patt word(if any) or to next link-header
	if((tlhead&(0x1000))>0){//error-word is present
	  tlerrf=*(pss+bias+1);
//cout<<"     Err:tlhead/tlerrf="<<hex<<tlhead<<" "<<tlerrf<<dec<<endl;
	  TOF2JobStat::daqssl(1,crat-1,slot-1,4);//count time-slots with error
	  bias+=1;//now poits to to next link-header
	}
      }//--->endof while(temp+time)-block words loop
//
//===========>endof TempTime-section
//cout<<"*******> endof ComprFMT-decoding !"<<endl;
  }//-->endof "Compr or Mixt"-datatype check
//
//<=========================== Endof CompressedFormat processing
//
//===========================> compare Raw/Comp-halfs of the decoded buffer(if mixed format):
  if(formt==2){
    TOF2JobStat::daqscr(3,crat-1,0);//count crate-entries with mixt-format(raw/com-subsegm OK)
//cout<<endl;
//cout<<"----> DAQS2Block:: MixFMT processed: Compare Raw- and Compr-subsegments decoding results:"<<endl;
    integer ihr,ihc,nhraw,nhcom,swidr,swidc,hwidr,hwidc,nhitsc(0),nhitsr(0);
    integer vmism,err1c(0),err2c(0);
    uinteger valr,valc;
    for(ic=0;ic<SCRCMX;ic++){//compr half-buf. loop (all swch)
      nhcom=swnbuf[ic];
      nhraw=swnbuf[ic+bufpnt];
      if(nhcom>0){
        swidc=swibuf[ic];
	hwidc=hwibuf[ic];
        swidr=swibuf[ic+bufpnt];
	hwidr=hwibuf[ic+bufpnt];
	if(nhraw<nhcom || swidc!=swidr || hwidc!=hwidr){
//	  cout<<"     Error_1: nhits/id mismatch for hwidc/swidc="<<hwidc<<" "<<swidc<<endl;
//	  cout<<"     nhcom/nhraw="<<nhcom<<" "<<nhraw<<" hwidr/swidr="<<hwidr<<" "<<swidr<<" swch="<<ic<<endl;
	  err1c+=1;
          TOF2JobStat::daqscr(3,crat-1,1);//count Err-1
	}
	vmism=1;
        for(ihc=0;ihc<nhcom;ihc++){//<--- loop over compFMT-hits to find confirmation in rawFMT
	  valc=swcbuf[ic][ihc];
	  for(ihr=0;ihr<nhraw;ihr++){
	    valr=swcbuf[ic+bufpnt][ihr];
	    if(valc==valr)vmism=0;//OK: compFMT-hit confirmed in rawFMT 
	  }
	  if(vmism==1){
	    err2c+=1;//count mism.hits
            TOF2JobStat::daqscr(3,crat-1,2);//count Err-2
//cout<<"     Error_2: unconfirmed CompFMT-hit="<<valc<<" for swid/hwid="<<swidc<<" "<<hwidc<<endl;
	  }
        }//--->endof confirm.loop
	nhitsc+=nhcom;//count compFMT-hits
      }//-->endof nhcom>0 check
//
      if(nhraw>0){
	nhitsr+=nhraw;//count rawFMT-hits
      }
    }//-->endof buff-loop
//
    if(err1c==0 && err2c==0)TOF2JobStat::daqscr(3,crat-1,3);//count errorless crate-entries
//cout<<"<---- MixFMT check completed:total Raw/CompFMT Q+T(LT,FT,sHT,sSHT)-hits :"<<nhitsr<<" "<<nhitsc<<endl;
//cout<<"       Total Nhit/swid/hwid-mismatch errors:"<<err1c<<endl;
//cout<<"       Total unconfirmed CompFMT-hits:"<<err2c<<endl;
//cout<<endl; 
  }
//<=========================== End of Comparison section
//
// Now scan buffers and create tof/anti raw-event obj.:
//
  integer nftdc;         // number of FT-tdc hits
  integer ftdc[SCTHMX1]; // FT-tdc" hits (in TDC channels)
  integer nstdc;         // number of LT-tdc hits
  integer stdc[SCTHMX3]; // LT-tdc" hit (in TDC channels)
  geant adca; // Anode-channel ADC hit (ADC-counts, float)
  integer nadcd;         // number of NONZERO Dynode-channels(max PMTSMX)
  geant adcd[PMTSMX]; // ALL Dynodes ADC hits(ADC-counts, positional, float)
  int16u aslt,sslt;
  integer nsumh,nsumsh,sumht[TOF2GC::SCTHMX2],sumsht[TOF2GC::SCTHMX2];
  integer crsta;
  bool subtrped;
  geant athr,dthr,anthr;
  athr=TOF2Varp::tofvpar.daqthr(3);//tof daq readout thr(ped sigmas) for anode
  dthr=TOF2Varp::tofvpar.daqthr(4);//tof daq-thr. for dynode
//
//for(ic=0;ic<SCRCMX;ic++){
//  if(swibuf[ic]>0){
//    cout<<"ic="<<ic<<" swid/hwid="<<swibuf[ic]<<" "<<hwibuf[ic]<<" nhits="<<swnbuf[ic]<<" Hits:";
//    for(i=0;i<swnbuf[ic];i++)cout<<" "<<swcbuf[ic][i];
//    cout<<endl;
//  }
//}
//  
  adca=0;
  nftdc=0;
  nstdc=0;
  nadcd=0;
  for(i=0;i<PMTSMX;i++){
    adcd[i]=0;
  }
  sswid=0;
  sswidn=0;
  shwida=0;
  shwidd=0;
  for(ic=0;ic<SCRCMX;ic++){//1-pass scan
    swid=swibuf[ic];//LBBSPM
//    cout<<"----> scanning buff: ic="<<ic<<" swid="<<swid<<endl;
    if(swid>0){//!=0 LBBSPM found
      sswid=swid/100;//LBBS ("short swid")
      for(icn=ic+1;icn<SCRCMX;icn++){//find next !=0 LBBSPM
        swidn=swibuf[icn];
        if(swidn>0)break;
      }
      if(swidn>0)sswidn=swidn/100;//next LBBS
      else sswidn=9999;//means all icn>ic are "0"
      il=swid/100000;
      mtyp=swid%10;
      if(il==0)dtyp=2;//anti
      else{
        dtyp=1;//tof
        il-=1;
      }
      ib=(swid%100000)/1000-1;
      is=(swid%1000)/100-1;
      pmt=(swid%100)/10;
      if(dtyp==1)pmmx=TOF2DBc::npmtps(il,ib);
      if(dtyp==2)pmmx=1;
      nh=swnbuf[ic];//#of hits for given sw-chan(in accordance with mtyp)
      hwid=hwibuf[ic];//CSSRR
      shwid=hwid/100;//CSS("short hwid")
      if(pmt==0)shwida=shwid;//store shwid if anode measurement
      if(pmt>0)shwidd=shwid;//store shwid if dynode measurement
      aslt=shwid%100;//abs. Slot #(1,2,...11)
      sslt=AMSSCIds::sl2tsid(aslt-1);//temp.sens.id=SFET+A seq.slot#(if temp.slot or 0, if any other slot)
      if(mtyp>=2)assert(sslt>0 && sslt<=SCFETA);//SFET+SFEA seq.slot# is used for FT/sHT/sSHT storing
      subtrped=(PedSubt[crat-1]==1);//true/false->subtr/not peds
//
      switch(mtyp){//fill RawEvent arrays
        case 0:
	  for(i=0;i<nh;i++)stdc[i]=swcbuf[ic][i];
	  nstdc=nh;
	  break;
        case 1:
	  if(pmt>pmmx)cout<<"scan: Npm>Max in ADC-hit, swid="<<swid<<endl;
	  else{
	    if(pmt==0){//anode
	      adca=geant(swcbuf[ic][0])+0.5;//"+0.5" to be at ADC-bin center
	      if(dtyp==1){//tof
	        ped=TOFBPeds::scbrped[il][ib].apeda(is);
	        sig=TOFBPeds::scbrped[il][ib].asiga(is);
		if(subtrped){
		  if((adca-ped)>athr*sig)adca-=ped;
		  else adca=0;
		}
	      }
	      if(dtyp==2 ){//anti
                ped=ANTIPeds::anscped[ib].apeda(is);
                sig=ANTIPeds::anscped[ib].asiga(is);
                anthr=ANTI2SPcal::antispcal[ib].getdqthr();
		if(subtrped){
		  if((adca-ped)>anthr*sig)adca-=ped;
		  else adca=0;
		}
	      }    
	    }
	    else{//dynode
	      adcd[pmt-1]=geant(swcbuf[ic][0])+0.5;
	      if(subtrped){
	        ped=TOFBPeds::scbrped[il][ib].apedd(is,pmt-1);
	        sig=TOFBPeds::scbrped[il][ib].asigd(is,pmt-1);
		if((adcd[pmt-1]-ped)>dthr*sig){
		  adcd[pmt-1]-=ped;
	          nadcd+=1;
		}
		else adcd[pmt-1]=0;
	      }
	      else nadcd+=1;
	    }
	  }
	  break;
        case 2:
	  TOF2RawSide::FThits[crat-1][sslt-1]=nh;
	  for(i=0;i<nh;i++)TOF2RawSide::FTtime[crat-1][sslt-1][i]=swcbuf[ic][i];
	  break;
	case 3:
          TOF2RawSide::SumHTh[crat-1][sslt-1]=nh; 
          for(i=0;i<nh;i++)TOF2RawSide::SumHTt[crat-1][sslt-1][i]=swcbuf[ic][i];
	  break;
	case 4:
          TOF2RawSide::SumSHTh[crat-1][sslt-1]=nh; 
          for(i=0;i<nh;i++)TOF2RawSide::SumSHTt[crat-1][sslt-1][i]=swcbuf[ic][i];
	  break;
        default:
	  cout<<"Buf-scan:unknown measurement type ! swid="<<swid<<endl;
      }//-->endof switch
    }//-->endof "!=0 LBBSPM found"
//
    if(sswid!=sswidn){//new/last LBBS found -> create RawEvent-obj for current LBBS
// (after 1st swid>0 sswid is = last filled LBBS, sswidn is = LBBS of next nonempty channel or =9999)
//  at this stage temp may be not defined, it will be redefined at validation-stage using static job-store)
      crsta=0;
      if(dtyp==1){//TOF
	if(nstdc>0 || adca>0 || nadcd>0){//create tof-raw-side obj
	  if(nstdc>0 || adca>0)shwid=shwida;
	  else shwid=shwidd;
	  if(PedSubt[crat-1])sta=0;//ok(normal TOF2RawSide object with subtracted ped)
	  else sta=1;//for the moment it is a flag for Validate-stage that Peds was not subtracted !!!
	  
	  nftdc=0;//dummy(filled later at valid. stage from [cr][sl] static stores)
	  nsumh=0;
	  nsumsh=0;
//	  cout<<endl;
//	  cout<<"    ==> Create TOFRawSide: short swid/hwid="<<sswid<<" "<<shwid<<endl;
//	  cout<<"                               shwida/shwidd="<<shwida<<" "<<shwidd<<endl;
//	  cout<<"    nLT-hits="<<nstdc;
//	  for(i=0;i<nstdc;i++)cout<<" "<<stdc[i];
//	  cout<<endl;
//	  cout<<"    adca="<<adca<<" nadcd="<<nadcd<<"  dynh="<<adcd[0]<<" "<<adcd[1]<<" "<<adcd[2]<<endl;
          if(AMSEvent::gethead()->addnext(AMSID("TOF2RawSide",0),
                 new TOF2RawSide(sswid,shwid,sta,charge,temp,nftdc,ftdc,nstdc,stdc,
		                                                   nsumh,sumht,
								   nsumsh,sumsht,
                                                                   adca,
			                                           nadcd,adcd)))crsta=1;
	}
      }
      else{//ANTI 
	if(nstdc>0 || adca>0){
	  if(PedSubt[crat-1])sta=0;//ok(normal Anti2RawEvent object with subtracted ped)
	  else sta=1;//for the moment it is a flag for Validate-stage that Peds was not subtracted !!!
	  nftdc=0;//dummy(filled later at valid. stage from [cr][sl] static stores)
//	  cout<<"   ==> Create AntiRawEvent: swid="<<sswid<<endl;
//	  cout<<"    nLT-hits="<<nstdc;
//	  for(i=0;i<nstdc;i++)cout<<" "<<stdc[i];
//	  cout<<endl;
//	  cout<<"    adca="<<adca<<endl;
//	  cout<<endl;
          AMSEvent::gethead()->addnext(AMSID("Anti2RawEvent",0),
                                    new Anti2RawEvent(sswid,sta,temp,adca,nftdc,ftdc,nstdc,stdc));
	}
      }
      adca=0;//reset RawEvent buffer
      nftdc=0;
      nstdc=0;
      nadcd=0;
      shwida=0;
      shwidd=0;
      for(i=0;i<PMTSMX;i++){
        adcd[i]=0;
      }
    }//-->endof next/last LBBS check
  }//-->endof scan
//
/*
  cout<<"    FT-time hits report:"<<endl;
  for(int isla=0;isla<5;isla++){
    cout<<"SFETA-slot="<<isla+1<<" hits:"<<endl;
    cout<<" nhits="<<TOF2RawSide::FThits[crat-1][isla];
    for(ic=0;ic<TOF2RawSide::FThits[crat-1][isla];ic++)cout<<" "<<TOF2RawSide::FTtime[crat-1][isla][ic];
    cout<<endl;
  }
  cout<<"    sumHT-time hits report:"<<endl;
  for(int isla=0;isla<4;isla++){
    cout<<"SFETA-slot="<<isla+1<<" hits:"<<endl;
    cout<<" nhits="<<TOF2RawSide::SumHTh[crat-1][isla];
    for(ic=0;ic<TOF2RawSide::SumHTh[crat-1][isla];ic++)cout<<" "<<TOF2RawSide::SumHTt[crat-1][isla][ic];
    cout<<endl;
  }
  cout<<"    sumSHT-time hits report:"<<endl;
  for(int isla=0;isla<4;isla++){
    cout<<"SFETA-slot="<<isla+1<<" hits:"<<endl;
    cout<<" nhits="<<TOF2RawSide::SumSHTh[crat-1][isla];
    for(ic=0;ic<TOF2RawSide::SumSHTh[crat-1][isla];ic++)cout<<" "<<TOF2RawSide::SumSHTt[crat-1][isla][ic];
    cout<<endl;
  }
//
  cout<<"Temperature report:"<<endl;
  for(int its=0;its<5;its++){
    cout<<"  SFETA-slot:"<<its+1<<" temper:"<<TOF2JobStat::gettemp(crat-1,its)<<endl;
  }
//
  integer trpat[TOF2GC::SCLRS],trpatz[TOF2GC::SCLRS];
  TOF2RawSide::getpatt(trpat);
  cout<<"TrigPatt report(HT):"<<endl;
  for(il=0;il<TOF2GC::SCLRS;il++){// pattern histogr
    cout<<"   layer="<<il+1<<endl;
    cout<<"     trpatt="<<hex<<trpat[il]<<dec<<endl;
    cout<<"      side1-pads:";
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      if((trpat[il]&(1<<ib))>0)cout<<" "<<ib+1;
    }
    cout<<endl;
    if((trpat[il]&(1<<12))>0)cout<<"       other-SPTside CPbit-on";
    if((trpat[il]&(1<<13))>0)cout<<"       other-SPTside CTbit-on";
    cout<<endl;
    cout<<"      side2-pads:";
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      if((trpat[il]&(1<<(16+ib)))>0)cout<<" "<<ib+1;
    }
    cout<<endl;
    if((trpat[il]&(1<<28))>0)cout<<"       other-SPTside CPbit-on";
    if((trpat[il]&(1<<29))>0)cout<<"       other-SPTside CTbit-on";
    cout<<endl;
  }
  TOF2RawSide::getpattz(trpatz);
  cout<<"TrigPatt report(SHT):"<<endl;
  for(il=0;il<TOF2GC::SCLRS;il++){// pattern histogr
    cout<<"   layer="<<il+1<<endl;
    cout<<"     trpatt="<<hex<<trpatz[il]<<dec<<endl;
    cout<<"      side1-pads:";
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      if((trpatz[il]&(1<<ib))>0)cout<<" "<<ib+1;
    }
    cout<<endl;
    if((trpat[il]&(1<<12))>0)cout<<"       other-SPTside BZbit-on";
    cout<<endl;
    cout<<"      side2-pads:";
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      if((trpatz[il]&(1<<(16+ib)))>0)cout<<" "<<ib+1;
    }
    cout<<endl;
    if((trpat[il]&(1<<28))>0)cout<<"       other-SPTside BZbit-on";
    cout<<endl;
  }
*/
//--------
  return;
BadExit:
  TOF2JobStat::daqsfr(1);//count rejected entries(segments)    
//  
}
//----------------------------------------------------
integer DAQS2Block::calcblocklength(integer ibl){//tempor, have to be changed !
  integer tofl(0),antil(0),fmt,lent;
  int16u blid,msk;
//
  fmt=TFMCFFKEY.daqfmt;
  format=fmt;// class variable is redefined by data card 
  blid=nodeids[ibl];// valid block_id
//  tofl=TOF2RawSide::calcdaqlength(blid);
  antil=Anti2RawEvent::calcdaqlength(blid);
  lent=(1+tofl+antil);//"1" for block-id word.
  return lent;
}
//----------------------------------------------------
integer DAQS2Block::getmaxblocks(){return 2*TOF2GC::SCCRAT;}//"2" to include a-b sides
//----------------------------------------------------
void DAQS2Block::buildblock(integer ibl, integer len, int16u *p){//tempor, have to be changed !
//
// on input: len=tot_block_length as was defined by call to calcblocklength
//           *p=pointer_to_beggining_of_block_data (block-id word)
//
  integer i,dlen,clen,fmt,lent(0);
  int16u *next=p;
  int16u blid;
//
  blid=nodeids[ibl];// valid block_id
// ---> wrire block-id :
  *p=blid;
  next+=1;//now points to first subdet_data word (usually TOF)
  lent+=1;
//
//--------------
  dlen=0;
//  TOF2RawSide::builddaq(blid, dlen, next);
//
  lent+=dlen;
  next+=dlen;
//---------------
  dlen=0;
  Anti2RawEvent::builddaq(blid, dlen, next);
  lent+=dlen;
//---------------
  if(len != lent){
    cout<<"DAQS2Block::buildblock: length-mismatch !!! for block "<<ibl<<endl;
    cout<<"Initially declared length="<<len<<" but was written "<<lent<<endl;
  }
//---------------
  if(ibl==(2*TOF2GC::SCCRAT-1))  //clear RawEvent/Hit container after last block processed
  {
#ifdef __AMSDEBUG__

    AMSContainer *ptr1=AMSEvent::gethead()->getC("TOF2RawSide",0);
    if(ptr1)ptr1->eraseC();
//
    AMSContainer *ptr2=AMSEvent::gethead()->getC("Anti2RawEvent",0);
    if(ptr2)ptr2->eraseC();
//
//
#endif
  }
//--------------
}
