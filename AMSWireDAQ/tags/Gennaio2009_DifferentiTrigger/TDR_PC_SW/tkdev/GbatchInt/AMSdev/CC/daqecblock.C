// AMS02 version 16.11.06 by E.Choumilov
#include "typedefs.h"
#include <iostream.h>
#include "link.h"
#include <fstream.h>
#include "cern.h"
#include "commons.h"
#include "event.h"
#include "ecaldbc.h"
#include "ecid.h"
#include "daqecblock.h"
#include "ecalrec.h"
#include "ecalcalib.h"
//
using namespace ecalconst;
//
int16u DAQECBlock::format=0; // default format (raw)
//
int16u DAQECBlock::nodeids[2*ecalconst::ECRT]=//valid EC_nodes(JINFs) id(link#)(*2 due to 2 halfs) 
  {
    0,1, //  a-sides, crates 1->2 (tempor !!!) 
    2,3  //  b-sides, ...........
  };
//
integer DAQECBlock::totbll=0;
//
// functions for EC-blocks class:
//
void DAQECBlock::node2crs(int16u nodeid, int16u &crat, int16u &sid){
//  on input: nodeid(JINF-nodes only!!!)=(blid&0x001F)
//  on output: crate=1-2, =0 if ID is not found; side=(1,2) -> (a,b)
  crat=0;
  sid=0;
  for(int i=0;i<2*ecalconst::ECRT;i++){
    if(nodeid==nodeids[i]){
      crat=int16u(i%2)+1;//seq.crate# 1,...2
      sid=int16u(floor(geant(i)/2)+1);//side# 1,2(a,b)
    }
  }
}
//
integer DAQECBlock::checkblockid(int16u blid){//have to be redefined !!!
  int valid(0);
  for(int i=0;i<2*ecalconst::ECRT;i++)if((blid & (0x001F)) == nodeids[i])valid=1;
//            (this "2*" due to redundancy)
  return valid;
}
//-------------------------------------------------------
void DAQECBlock::buildraw(integer leng, int16u *p){
// it is implied that event_fragment is EC-JINF's one (validity is checked in calling procedure)
//           *p=pointer_to_beggining_of_block_data(i.e. to len-word) ?????
// this event fragment may keep EDR2,ETRG blocks in arbitrary order;
// Length counting does not include length-word itself !!!
//
  integer i,j,ic,icn;
  integer swid,hwid,crsta(0);
  int16u swch,rdch,slot,aslt,crat,val16,ibit,slay,pmt,pix,gain,trppmt;
  int16u dtyp,datyp,lenraw(0),lencom(0),formt,evnum;
  int16u jbias,ebias,jblid,eblid,jleng,eleng,jaddr,eaddr,csid;
  int16u word,nword,nnword;
  int16u fmt;
  int16u osbit;
  int16 slots;
  integer bufpnt(0),lbbs;
  uinteger val32;
// for class/DownScaled ped-cal
  bool PedCal(false);//means PedCal job, using event-by-event in RawFMT or DownScaled 
  bool DownScal(false);
  static int FirstDScalBlk(0);
  int16u PedSubt[ECSLOTS]={1,1,1,1,1,1,1};//0/1->no/yes PedSubtr at RawEvent creation
// for PedCalTable(onboard calib)
  integer static FirstPedBlk(0);
  integer static TotPedBlks(0);
  integer static PedBlkCrat[ECRT][ECEDRS]={0,0,0,0,0,0,0,0,0,0,0,0};
  bool PedBlkOK(false);
  geant ped,sig;
  int16u sts,nblkok;
//
  uinteger swvbuf[ECRCMX];
// keep:       ADC-values   
  integer  swibuf[ECRCMX],hwibuf[ECRCMX];
//keep:        swids=LTTPG         hwids=CSSRRR   
// these buffers will be filled with Ecal sw-channels hits in current JINF(crate) as 
// arrays of sequential sw-channel index 
// Also filled at EcalRawEvent objects creation the dynode static arrays:
// AMSEcalRawEvent::dynadc[ECSLMX][ECPMSMX];//Dunode hits vs (SL,PMT)
//
  for(ic=0;ic<ECRCMX;ic++){//clear buffs
    swibuf[ic]=0;
    hwibuf[ic]=0;
    swvbuf[ic]=0;
  }
//
  EcalJobStat::daqs1(0);//count entries
  jleng=*p;//fragment's 1st word(block length excluding length-word itself)
  jblid=*(p+jleng);// JINF fragment's last word: Status+slaveID(its id)
//
  bool dataf=((jblid&(0x8000))>0);//data-fragment
  bool crcer=((jblid&(0x4000))>0);//CRC-error
  bool asser=((jblid&(0x2000))>0);//assembly-error
  bool amswer=((jblid&(0x1000))>0);//amsw-error   
  bool timoer=((jblid&(0x0800))>0);//timeout-error   
  bool fpower=((jblid&(0x0400))>0);//FEpower-error   
  bool seqer=((jblid&(0x0400))>0);//sequencer-error
  bool cdpnod=((jblid&(0x0020))>0);//CDP-node(like EDR2-node with no futher fragmentation)
  bool noerr;
  jaddr=jblid&(0x001F);//slaveID(="NodeAddr"=JINFaddr here)(one of 4 permitted)
//  cout<<"====> InBuildRaw: JIN_length="<<jleng<<" JINF-addr:"<<jaddr<<endl;
  if(jleng>1)EcalJobStat::daqs1(1);//<=== count non-empty fragments
  else goto BadExit;
  noerr=(dataf && !crcer && !asser && !amswer 
                                       && !timoer && !fpower && !seqer && !cdpnod);
  if(noerr)EcalJobStat::daqs1(2);//<=== count no_ass_errors JINF-fragments for given DATA-type     
  else goto BadExit;
  node2crs(jaddr,crat,csid);//get crate#(1-2, =0,if notfound),card_side(1-2<->a-b)
//  cout<<"     crate/side="<<crat<<" "<<csid<<endl;
  if(crat>0)EcalJobStat::daqs1(3+2*(crat-1)+csid-1);//crates/sides sharing of "no_ass_err" entries
  else goto BadExit;
//-----------
  if(ECREFFKEY.relogic[1]==4)PedCal=true;//tempor(use later info about presence of ClassPedData from header ?)
  if(!PedCal && ECREFFKEY.relogic[1]==4){
    cout<<"DAQECBlock::buildraw-W-Not ClassicPedCalibData when classic PedCal job is requested !!!"<<endl;
    return;
  }
//
  if(ECREFFKEY.relogic[1]==5)PedCal=true;// DownScaledEvents PedCalib job is requested 
//-----------
  jbias=1;
  while(jbias<jleng){//<---- JINF-words loop
    eleng=*(p+jbias);//deeper level (EDR2/ETRG) fragment's length
    eblid=*(p+jbias+eleng);//deeper level (EDR2/ETRG) fragment's Status+slaveID(EDR/ETRG id) 
    eaddr=eblid&(0x001F);//slaveID(="NodeAddr"=ERD/ETRGaddr here)(one of 7/side permitted)
    datyp=(eblid&(0x00C0))>>6;//0,1,2(only raw or compr)
    if(datyp==1)setrawf();
    if(datyp==2)setcomf();
    formt=getformat();//0/1->raw/compr flag for current EDR
//    cout<<"    XDR_length/addr="<<eleng<<" "<<eaddr<<"  datyp="<<datyp<<endl;
    if(eleng>1 && datyp>0)EcalJobStat::daqs2(crat-1,formt);//entries per crate/format
    else goto NextBlock;
//edr/etrg status-bits:
    dataf=((eblid&(0x8000))>0);//data-fragment
    crcer=((eblid&(0x4000))>0);//CRC-error
    asser=((eblid&(0x2000))>0);//assembly-error
    amswer=((eblid&(0x1000))>0);//amsw-error   
    timoer=((eblid&(0x0800))>0);//timeout-error   
    fpower=((eblid&(0x0400))>0);//FEpower-error   
    seqer=((eblid&(0x0400))>0);//sequencer-error
    cdpnod=((eblid&(0x0020))>0);//CDP-node(like EDR2/ETRG-node with no futher fragmentation)
    noerr=(dataf && !crcer && !asser && !amswer 
                                       && !timoer && !fpower && !seqer && cdpnod);
//    if(noerr)cout<<" status-bits OK..."<<endl;
    if(noerr)EcalJobStat::daqs2(crat-1,2+formt);//"no_ass_err" entries per crate/format
    else goto NextBlock;
//
    slots=AMSECIds::crdid2sl(csid-1,eaddr);//get seq.slot#(0-5 =>EDRs; =6 =>ETRG;-1==>not_found)
//    cout<<"    SeqSlot="<<slots<<endl;
    if(slots<0){
#ifdef __AMSDEBUG__
      cout<<"ECBlock::Error:illegal CardID, crate/side/fmt/id="<<crat<<" "<<csid<<" "<<formt
                                                                <<" "<<hex<<eaddr<<dec<<endl;
#endif
      EcalJobStat::daqs2(crat-1,4);//illegal CardId
      goto NextBlock;    
    }
    slot=int16u(slots);
    EcalJobStat::daqs3(crat-1,slot,0);//entries per crate/slot
    ebias=1;
//------
    if(slot==6){//<===== ETRG-block processing(modifying ebias)
      if(eleng==8)EcalJobStat::daqs3(crat-1,slot,2);//lengthOK
      else goto BadExit;
//cout<<"      ETRG-len OK"<<endl;
      for(int16u iwd=0;iwd<eleng-1;iwd++){//ETRG-words loop
	val16=*(p+jbias+ebias);//
        for(int16u ibt=0;ibt<16;ibt++){
	  ibit=16*iwd+ibt;//0-111(really used 0-107(3sl*36pm),108(XA),109(XF))
	  slay=ibit/36;//0-2
	  if(crat==1)slay=2*slay;//trig.layers counting: 0,2,4(X-proj,Face B(D))
	  if(crat==2)slay=2*slay+1;//trig.layers counting: 1,3,5(Y-proj,Face A(C))
	  pmt=ibit%36;//0-35
	  if((val16&(1<<ibt))>0){
//	    cout<<"   SettingTRPbits:ProjLay/pmt="<<slay<<" "<<pmt<<" crat="<<crat<<endl;
	    AMSEcalRawEvent::settrpbit(slay,pmt);//set bit in static trpatt[6][3]-array)
	  }
	  if(ibit==107)break;
	}
	if(ebias==(eleng-1)){//this last word contains Fast/Lev1(angle) decision bits
	  if((val16 & 0x8000)>0){//was FastTr-bit
	    if(crat==1)trppmt=38;//X-pr
	    if(crat==2)trppmt=39;//Y-pr
//	  cout<<"     SettingFastTrig-bit, trppmt="<<trppmt<<endl;
	    AMSEcalRawEvent::settrpbit(5,trppmt);
	  }
	  if((val16 & 0x4000)>0){//was Lev1Tr-bit(angle)
	    if(crat==1)trppmt=36;//X-pr
	    if(crat==2)trppmt=37;//Y-pr
//	  cout<<"     SettingLVL1Trig-bit, trppmt="<<trppmt<<endl;
	    AMSEcalRawEvent::settrpbit(5,trppmt);
	  }
	} 
        ebias+=1;
      }//--->endof ETRG-words loop
    }
//------
    if(slot<=5){//<===== EDR-block processing
//--
      if(formt==0){//<===================== RawFormat
//-------
	if(eleng==(3*ECEDRC+1)){//<-------- PedTable found
	  EcalJobStat::daqs3(crat-1,slot,3);//PedTable entrie with length OK
	  if(ECREFFKEY.relogic[1]==6){//PedTable was requested ==> processing...
            if(FirstPedBlk==0){
              ECPedCalib::BTime()=AMSEvent::gethead()->gettime();
              ECPedCalib::BRun()=AMSEvent::gethead()->getrun();
              ECPedCalib::resetb();
            }
            PedBlkOK=true;
            while(ebias<eleng){//<---- EDR-words loop (3*243 ADC-values)
              word=*(p+jbias+ebias);//ped, ADC-value(multiplied by 16 in DSP)
              nword=*(p+jbias+ebias+1);//sig, ADC-value(multiplied by 16 in DSP)
              nnword=*(p+jbias+ebias+2);//stat, x16 ???
	      rdch=(ebias-1)/3;//0-242
	      AMSECIds ecid(crat-1,csid-1,slot,rdch);//create ecid-obj
	      swid=ecid.getswid();//long sw_id=LTTPG
              ped=geant(word&0xFFFF)/16;//tempor
              sig=geant(nword&0xFFFF)/16;//tempor
              sts=nnword;//tempor 1/0->bad(empty)/ok
	      ECPedCalib::filltb(swid, ped, sig, sts);//tempor
	      ebias+=3;
	    }//--->endof EDR-words loop(PedTable)
            if(PedBlkOK)PedBlkCrat[crat-1][slot]=1;//mark good processed crate
            TotPedBlks+=1;//counts tot# of processed PedBlocks
            FirstPedBlk=1;
//               call DB- and pedfile-writing if last :
            if(TotPedBlks==(ECRT*ECEDRS)){//<---last(12th) ped-block processed
              nblkok=0;
              for(i=0;i<ECRT;i++)for(j=0;j<ECEDRS;j++)if(PedBlkCrat[i][j]==1)nblkok+=1;
              if(nblkok==(ECRT*ECEDRS)){// all found blocks OK
	        ECPedCalib::outptb(ECCAFFKEY.pedoutf);//0/1/2->noactions(only_histos)/write2db+file/write2file
	      }
              TotPedBlks=0;//be ready for next calib.blocks sequence
              FirstPedBlk=0;
              for(i=0;i<ECRT;i++)for(j=0;j<ECEDRS;j++)PedBlkCrat[i][j]=0;
            }//---<endof "last PedBlock processed"
	  }//--->endof PedTable processing
          goto NextBlock;//(if any)
	}//--->endof PedTable presence check
//-------
        else if(eleng==(ECEDRC+1))EcalJobStat::daqs3(crat-1,slot,1);//true RawSegment & length OK
        else goto BadExit;//wrong length ==> stop any further processing
//cout<<"    RawFMT,EDR_length OK"<<endl;
//             <======================== Normal RawFormat :
        while(ebias<eleng){//<---- EDR-words loop (243 ADC-values)
	  rdch=ebias-1;//0-242
	  val16=*(p+jbias+ebias);//ADC-value(multiplied by 16 in DSP)
//	  cout<<"  ebias="<<ebias<<" rdch/val="<<rdch<<" "<<val16<<endl;
	  AMSECIds ecid(crat-1,csid-1,slot,rdch);//create ecid-obj
	  swid=ecid.getswid();//long sw_id=LTTPG
	  swch=ecid.getswch();//glob.seq. sw-chan(0-2915)
	  hwid=ecid.gethwid();//hw_id=CSSRRR
//	  cout<<"   swid/hwid/swch="<<swid<<" "<<hwid<<" "<<swch<<endl;
	  swibuf[swch]=swid;//fill buffs
	  hwibuf[swch]=hwid;
	  swvbuf[swch]=val16;
	  ebias+=1;
	}//--->endof EDR-words loop(RawFmt)
        if(PedCal)PedSubt[slot]=0;//my internal flag to subtr(1)/not(0) peds at RawEvent-creation
      }
//-------
      else{//<======================== ComprFormat : 
        if(eleng<=(2*ECEDRC+1) && (eleng%2==1))EcalJobStat::daqs3(crat-1,slot,2);//lengthOK
        else goto BadExit;
//cout<<"    ComprFMT,EDR_length OK"<<endl;
        DownScal=(eleng==(2*ECEDRC+1));//EDR-block is "downscaled",i.e. comprFMT,but no "0" suppression
        while(ebias<eleng){//<---- EDR-words loop (max 2*243 (rdch# + ADC-valie))
	  rdch=*(p+jbias+ebias);//rdch(0-242)
	  val16=*(p+jbias+ebias+1);//ADC-value(multiplied by 16 in DSP)
//	  cout<<"  ebias="<<ebias<<" rdch/val="<<rdch<<" "<<val16<<endl;
	  AMSECIds ecid(crat-1,csid-1,slot,rdch);//create ecid-obj
	  swid=ecid.getswid();//long sw_id=LTTPG
	  swch=ecid.getswch();//glob.seq. sw-chan(0-2915)
	  hwid=ecid.gethwid();//hw_id=CSSRRR
//	  cout<<"   swid/hwid/swch="<<swid<<" "<<hwid<<" "<<swch<<endl;
	  swibuf[swch]=swid;//fill buffs
	  hwibuf[swch]=hwid;
	  swvbuf[swch]=val16;
//	  cout<<"    rdch/val="<<rdch<<" "<<val16<<"  swid/hwid="<<swid<<" "<<hwid<<"  swch="<<swch<<endl;
	  ebias+=2;
//
	  if(DownScal && PedCal){//"downscaled" block ped-processing (if ordered !)
            PedSubt[slot]=0;//my internal flag to subtr(1)/not(0) peds at RawEvent-creation
            if(FirstDScalBlk==0){
              ECPedCalib::BTime()=AMSEvent::gethead()->gettime();//store Begin-time
	      FirstDScalBlk=1;
            }
	  }//--->endof "DownScaled" block ped-processing
//
	}//--->endof EDR-words loop(comprF)
      }//--->endof ComprFMT
    }
//----
NextBlock:
    jbias+=eleng+1;//"+1" to include eleng-word itself
  }//---->endof JINF-level loop
//
//----------------------------> scan swch-buffers and create RawEvent-Objects:
//
  integer sswid,swidn,sswidn,shwid,subtrped,sta;
  geant peda,siga,pedd,sigd,athr,dthr,padc[3],adca,adcd;
//
  athr=ECALVarp::ecalvpar.daqthr(0);//daq-thr. for anode(hi/low)
  dthr=ECALVarp::ecalvpar.daqthr(4);//daq-thr. for dynode
//
  for(i=0;i<3;i++){//reset RawEvent adc-buff
    padc[i]=0;
  }
  sswid=0;
  sswidn=0;
  for(ic=0;ic<ECRCMX;ic++){//1-pass scan
    swid=swibuf[ic];//LTTPG
    if(swid>0){//!=0 LTTPG found
      sswid=swid/10;//LTTP ("short swid")
      for(icn=ic+1;icn<ECRCMX;icn++){//find next !=0 LTTPG
        swidn=swibuf[icn];
        if(swidn>0)break;
      }
      if(swidn>0)sswidn=swidn/10;//next LTTP
      else sswidn=9999;//means all icn>ic are "0"
      slay=swid/10000;//1-9
      gain=swid%10;//1-2(hi/low)
      pmt=(swid%10000)/100;//1-36
      pix=(swid%100)/10;//1-4(=5->dyn)
      hwid=hwibuf[ic];//CSSRRR
      shwid=hwid/1000;//CSS("short hwid")
      aslt=shwid%100;//abs. Slot #(1,2,...5)
      subtrped=PedSubt[aslt-1];//1/0->subtr/not peds at RawEvent creation
      peda=ECPMPeds::pmpeds[slay-1][pmt-1].ped(pix-1,gain-1);//current Aped
      siga=ECPMPeds::pmpeds[slay-1][pmt-1].sig(pix-1,gain-1);//current Asig
      pedd=ECPMPeds::pmpeds[slay-1][pmt-1].pedd();//current Dped
      sigd=ECPMPeds::pmpeds[slay-1][pmt-1].pedd();//current Dsig
//
      switch(gain){//fill RawEvent arrays
        case 1:
	  if(pix<=4){//anode-adc(hi)
	    adca=geant(swvbuf[ic])/16+0.5;//"16" to go back to true ADC-value
	    if(subtrped){
	      if((adca-peda)>athr*siga)padc[0]=adca-peda;//subtr.ped and apply DAQ-threshold
	      else padc[0]=0;
	    }
	    else padc[0]=adca;
	  }
	  else{//dynode(pix=5)
	    adcd=geant(swvbuf[ic])/16+0.5;
	    if(subtrped){
	      if((adcd-pedd)>dthr*sigd)padc[2]=adcd-pedd;//subtr.ped and apply DAQ-threshold
	      else padc[2]=0;
	    }
	    else padc[2]=adcd;
	    AMSEcalRawEvent::setadcd(slay-1,pmt-1,padc[2]);
	  }
	  break;
//
        case 2:
	  adca=geant(swvbuf[ic])/16+0.5;//anode-adc(low)
	  if(subtrped){
	    if((adca-peda)>athr*siga)padc[1]=adca-peda;//subtr.ped and apply DAQ-threshold
	    else padc[1]=0;
	  }
	  else padc[1]=adca;
	  break;
//
        default:
	  cout<<"EcalDAQBuf-scan:unknown measurement type(gain) ! swid="<<swid<<endl;
      }//-->endof switch
    }//-->endof "!=0 LTTPG found"
//
    if(sswid!=sswidn){//new/last LTTP found -> create RawEvent for current LTTP(i.e. all gains for given pixel)
// (after 1st swid>0 sswid is = last filled LTTP, sswidn is = LTTP of next nonempty channel or =9999)
//General: it is implied that given pixel info(ah/al/d) is always contained within one slot(EDR)
      crsta=0;
      if(padc[0]>0 || padc[1]>0){//create EcalRawEvent obj
        if(ECREFFKEY.reprtf[2]>0){//debug-prints
	  cout<<"    ==> Create EcalRawEvent: short swid/hwid="<<sswid<<" "<<shwid<<endl;
	  cout<<"    Aadc(hi/low)="<<padc[0]<<" "<<padc[1]<<endl;
	  cout<<endl;
        }
	if(PedSubt[aslt-1])sta=0;//ok(normal RawEvent object with subtracted ped)
	else sta=1;//NOW it is flag for Validate-stage that Peds was not subtracted !!!
        padc[2]=0;//no valid Dyn-info in RawEvent for the moment(will be added in Validation)
        if(AMSEvent::gethead()->addnext(AMSID("AMSEcalRawEvent",crat-1),
                 new AMSEcalRawEvent(sswid,sta,csid,padc)))crsta=1;
      }
      for(i=0;i<3;i++){//reset RawEvent adc-buffer
        padc[i]=0;
      }
    }//-->endof next/last LTTP check
  }//-->endof scan
//
  if(ECREFFKEY.reprtf[2]>0){//debug-prints
    cout<<"===> Fired Dynodes map after processing of crate="<<crat<<":"<<endl;
    cout<<"           (reading direction: pm=1--->pm=36)"<<endl<<endl;
    for(slay=0;slay<ECSLMX;slay++){
      for(pmt=0;pmt<ECPMMX;pmt++){
        cout<<AMSEcalRawEvent::getadcd(slay,pmt)<<" ";
        if(pmt==17)cout<<endl;
      }
      cout<<endl<<endl;
    }
    cout<<"===> TrigPattern(slayer=1-6, pmt=1-36):"<<endl<<endl;
    for(slay=0;slay<ECSLMX-3;slay++){
      for(pmt=0;pmt<ECPMMX;pmt++){
        if(AMSEcalRawEvent::gettrpbit(slay,pmt)>0)cout<<1<<"|";
        else cout<<0<<"|";
      }
      cout<<endl;
    }
    cout<<"FastTrigBits(XF/YF)="<<AMSEcalRawEvent::gettrpbit(5,38)<<" "<<AMSEcalRawEvent::gettrpbit(5,39)<<endl;
    cout<<"Lvl1TrigBits(XA/YA)="<<AMSEcalRawEvent::gettrpbit(5,36)<<" "<<AMSEcalRawEvent::gettrpbit(5,37)<<endl;
  }
//
  return;
BadExit:
  EcalJobStat::daqs1(ECJSTA-1);//count rejected JINF-entries(segments)    
//  
}
//------------------------------------
void frbdump(int16u *p, int16u len, char name){//fragment bits-dump
  int16u tstw,tstb;
  int i,j;
  cout<<"-----------------------------------------------------------"<<endl;
//
  cout<<"  EC_Block"<<name<<" hex/binary dump follows :"<<endl<<endl;
  for(i=0;i<len;i++){
    tstw=*(p+i);
    cout<<hex<<setw(4)<<tstw<<"  |"<<dec;
    for(j=0;j<16;j++){
      tstb=(1<<(15-j));
      if((tstw&tstb)!=0)cout<<"x"<<"|";
      else cout<<" "<<"|";
    }
    cout<<endl;
  }
  cout<<dec<<endl;
  cout<<"-----------------------------------------------------------"<<endl;
} 
//------------------------------------
integer DAQECBlock::getmaxblocks(){return 2;}//for MC where only one side in any slot is implied(no redundancy)
integer DAQECBlock::calcblocklength(integer ibl){return 0;}
void DAQECBlock::buildblock(integer ibl, integer len, int16u *p){
  int leng=*p;
}
