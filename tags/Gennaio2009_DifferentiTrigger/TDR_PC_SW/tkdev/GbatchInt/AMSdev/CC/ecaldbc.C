//  $Id: ecaldbc.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// Author E.Choumilov 14.07.99.
#include "typedefs.h"
#include "cern.h"
#include "extC.h"
#include <math.h>
#include "commons.h"
#include "job.h"
#include "ecaldbc.h"
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include "ecalcalib.h"
//
using namespace ecalconst;
ECcalib ECcalib::ecpmcal[ECSLMX][ECPMSMX];// mem.reserv.for ECAL indiv.PMcell calib. param.
ECcalibMS ECcalibMS::ecpmcal[ECSLMX][ECPMSMX];// the same for MC-Seeds params
ECPMPeds ECPMPeds::pmpeds[ECSLMX][ECPMSMX];// ..........for ECAL peds,sigmas
ECALVarp ECALVarp::ecalvpar;// .........................for ECAL general run-time param.  
//-----------------------------------------------------------------------
//  =====> ECALDBc class variables definition :
//
integer ECALDBc::debug=1;
//
//
// Initialize ECAL geometry (defaults, REAL VALUES are read FROM geomconfig-file)
//
//---> default structural. data:
//
geant ECALDBc::_gendim[10]={
   65.8,65.8,0., // i=1-2  x,y-dimentions of EC-radiator; 3->spare
   8.2,          //  =4    dx(dy) thickn.of (PMT+electronics)-support(frame)
   0.,0.,        //  =5,6    center shift in x,y   
  -142.3,        //  =7      Radiator(incl.glue) front face Z-pozition
   4.18,         //  =8      top(bot) honeycomb thickness
   1.83,         //  =9      lead thickness of 1 SuperLayer
   0.01          //  =10     Thickness of glue on top(bot) side of SL 
};
//
geant ECALDBc::_fpitch[3]={
   0.135,      // i=1   fiber pitch in X(Y) projection
   0.173,0.    // i=2 fiber pitch in Z (inside super-layer); =3->spare
};
//
geant ECALDBc::_rdcell[10]={
   368.,32.5,0.15,//i=1,3  MC-def fib.att.length slow)/fast/fast_fract(real values from DB !!)
   0.094,        // i=4    fiber_core diameter(0.1-2x0.003cm)
   0.9,          // i=5    size(dx=dz) of "1/4" of PMT-cathod (pixel)
   0.45,         // i=6    abs(x(z)-position) of "1/4" in PMT coord.syst.
   1.8,          // i=7    X(Y)-pitch of PMT's;
   0.00857,      // i=8    fiber_cladd+glue thickn(.003+.00557cm) to have hole diam.=0.11114
   0.,0.         // i=9,10 spare
};
//
integer ECALDBc::_slstruc[6]={
    1,           // i=1   1st super-layer projection(0->X, 1->Y)
   10,           // i=2   numb. of fiber-layers per super-layer
   9,            //  =3   real numb. of super-layers (X+Y)
   36,           //  =4   real numb. of PMT's per super-layer (in X(Y))
  -1,1           //  =5-6 readout dir. in X/Y-proj (=1/-1->along/oppos Y/X-axes) for 1st PM  
};
//
integer ECALDBc::_nfibpl[2]={
   486,487       // i=1,2 numb. of fibers per 1st/2nd fiber-layer in S-layer
};
//
int ECALDBc::_scalef=2;// MC/Data scale factor used in ADC->DAQ-value conversion.
//
//  member functions :
//
// ---> function to read geomconfig-files 
//                  Called from ECALgeom :
  void ECALDBc::readgconf(){

//   add some checks VC
     if(_slstruc[2] >ecalconst::ECSLMX){
      _slstruc[2]=ecalconst::ECSLMX;
      cerr <<"====> ECALDBc::readgconf-W-Resetting ECSLMX to "<<_slstruc[2]<<endl;
     }
    if(_slstruc[2]==8){
     ECCAFFKEY.cfvers=10;

    }
//
    int i;
    char fname[80];
    char name[80]="ecalgeom";
    char vers1[3]="01";
    char vers2[4]="001";
    char vers3[3]="02";
    geant ZShift=-AMSDBc::amsdext;
//
    if(strstr(AMSJob::gethead()->getsetup(),"AMS02D")){
          cout <<"   <--ECALGeom-I-AMS02D setup selected."<<endl;
          strcat(name,vers3);
	  _gendim[6]+=ZShift;
	  cout<<"    <--ZShift="<<ZShift<<endl;
    }
    else if(strstr(AMSJob::gethead()->getsetup(),"AMS02")){
          cout <<"   <--ECALGeom-I-AMS02 setup selected."<<endl;
          strcat(name,vers3);
    }
    else
    {
          cout <<"<----- Error: unknown setup !??"<<endl;
          exit(1);
    }
    strcat(name,".dat");
//    if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
//    if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
//    strcat(fname,name);
//    cout<<"ECALDBc::readgconf: Open file : "<<fname<<'\n';
//    ifstream tcfile(fname,ios::in); // open needed config-file for reading
//    if(!tcfile){
//      cerr <<"ECgeom-read: missing geomconfig-file "<<fname<<endl;
//      exit(1);
//    }
  }
//---
//
  geant ECALDBc::gendim(integer i){
#ifdef __AMSDEBUG__
      if(ECALDBc::debug){
        assert(i>0 && i <= 10);
      }
#endif
    return _gendim[i-1];
  }
//
  geant ECALDBc::fpitch(integer i){
#ifdef __AMSDEBUG__
      if(ECALDBc::debug){
        assert(i>0 && i <= 3);
      }
#endif
    return _fpitch[i-1];
  }
//
  geant ECALDBc::rdcell(integer i){
#ifdef __AMSDEBUG__
      if(ECALDBc::debug){
        assert(i>0 && i <= 10);
      }
#endif
    return _rdcell[i-1];
  }
//

 
  integer ECALDBc::slstruc(integer i){
#ifdef __AMSDEBUG__
      if(ECALDBc::debug){
        assert(i>0 && i <= 6);
      }
#endif
    return _slstruc[i-1];
  }
//
  integer ECALDBc::nfibpl(integer i){
#ifdef __AMSDEBUG__
      if(ECALDBc::debug){
        assert(i>0 && i <= 2);
      }
#endif
    return _nfibpl[i-1];
  }
//
//---
  number ECALDBc::segarea(number r, number ds){//small_segment area fraction (wrt full disk)
//                                    r-radious, ds-horde_distance(from center)
    number sina,cs,sn,a;
    if(ds>=r)return(0.);
    cs=ds/r;
    sn=sqrt(1.-cs*cs);
    sina=2.*cs*sn;
    a=2.*asin(sn);//sector opening angle:(0->pi) when ds:(r->0)
    return ((a-sina)/2./AMSDBc::pi);//0.5->0 when ds  0->r
  }
//---
// fiberID(SSLLFFF) to cellID(SSPPC) conversion
// ("analog" design - fiber_edep division between neigb.pixels/pmts with fraction w)
//
  void ECALDBc::fid2cida(integer fid, integer cid[4], number w[4]){
    integer fidd,fff,ss,ll,ip,nfl,npm,pm,ctbin,czbin,cell,bran,tbc;
    number cleft,tleft,bdis,ztop,ct,cz,ww;
    geant pit,piz,pmdis,dist,pmpit,pmsiz,pxsiz,fr;
    nfl=_slstruc[1];// numb.of fiber-layers per super-layer
    npm=_slstruc[3];// numb.of PM's per super-layer
    pit=_fpitch[0];// fiber pitch(transv)
    piz=_fpitch[1];// fiber pitch(in z)
    pmpit=_rdcell[6];//PM-pitch(transv)
    pmsiz=_rdcell[4]*2.;//PM_size
    pxsiz=_rdcell[4];// SubCell(pixel) size
    fr=_rdcell[3]/2;//   fiber radious 
    fidd=fid/1000;
    fff=fid%1000-1;
    ll=fidd%100-1;
    ss=fidd/100;
    ip=ll%2;
    if(ip==0)cleft=-(_nfibpl[0]-1)*pit/2.;//     fiber from 1st layer of s-layer
    else cleft=-(_nfibpl[1]-1)*pit/2.;//         fiber from 2nd layer of s-layer
    ct=cleft+fff*pit;//       transv.coord. of fiber in ECAL r(eference) s(ystem)
    ztop=(nfl-1)*piz/2.;//       z-pos of 1st(from top) f-layer of s-layer
    cz=ztop-ll*piz;//      z-pos of fiber in Slayer r.s.(z=0->middle of super-layer)
    tleft=-npm*pmsiz/2.;//     low-edge PM-bin transv.position in ECAL r.s.
    dist=ct-tleft;//           fiber-center dist from the 1st PM (its left edge)
    pm=integer(floor(dist/pmsiz));//   number of fired PM  0-(npm-1)   (IMPLY pmpit=pmsiz !!!)
//    cout <<fid<<" "<<pm<<endl;
     if(pm<0 || pm>=npm)return;//    (out of sensitive area - no signal is readout)
//
    if(cz>fr)tbc=0;// below i imply no sharing in Z (even number of f-layers/s-layer)
    else if(cz<-fr)tbc=2;
    else {
      cerr<<"ECALDBc::fid2cida: bad fiber-PM matching in Z, cz="<<cz<<endl;
      exit(1);;
    }
//
    pmdis=dist-pm*pmsiz;//   fiber-center dist from the left edge of current(pm) PM
    if(pmdis<fr)bran=1;
    else if(pmdis>(pmsiz-fr))bran=2;
    else bran=3;
    switch(bran){
      case 1:  //<-- near the left PM-boundary
        cell=0+tbc;
        bdis=pmdis;
	w[0]=1.-segarea(fr,bdis);
	cid[0]=1000*ss+(pm+1)*10+(cell+1);
	if(pm==0)return;// 1st PM
	w[1]=1.-w[0];
	cell=1+tbc;
	cid[1]=1000*ss+(pm)*10+(cell+1);// left neighbour
        break;
//        
      case 2:  //<-- near the right PM-boundary
        cell=1+tbc;
        bdis=pmsiz-pmdis;
	w[0]=1.-segarea(fr,bdis);
	cid[0]=1000*ss+(pm+1)*10+(cell+1);
	if(pm==(npm-1))return;// last PM
	w[1]=1.-w[0];
	cell=0+tbc;
	cid[1]=1000*ss+(pm+2)*10+(cell+1);// right neighbour
        break;
//        
      case 3:  //<-- completely inside PM
        bdis=pmdis-pxsiz;// f-center dist fron vertical pixel boundary
        if(bdis<=-fr){// <-- completely in the left half of PM
          cell=0+tbc;
	  cid[0]=1000*ss+(pm+1)*10+(cell+1);
	  w[0]=1.;
        }
        else if(bdis>=fr){// <-- completely in the right half of PM
          cell=1+tbc;
	  cid[0]=1000*ss+(pm+1)*10+(cell+1);
	  w[0]=1.;
        }
        else{ // <-- lefr-right sharing
	  ww=segarea(fr,fabs(bdis));
          cell=0+tbc;
	  cid[0]=1000*ss+(pm+1)*10+(cell+1);
          cell=1+tbc;
	  cid[1]=1000*ss+(pm+1)*10+(cell+1);
	  if(bdis<0.){
	    w[0]=1.-ww;	    
	    w[1]=ww;	    
	  }
	  else{
	    w[0]=ww;	    
	    w[1]=1.-ww;	    
	  }
        }
	break;
//
      default:
        cerr<<"ECALDBc::fid2cida: problem with PM/Fiber transv.matching, pmdis="<<pmdis<<endl;
	exit(1);
    }
  }
//
//---------
// <--- function to get info about SubCell("analog" design) :
//
//input: isl->S-layer(0-...);pmc->PMCell(0-...);sc->SubCell(0-3)
//output:pr->Proj(0-X,1-Y);pl->Plane(0-..);cell->0-...;ct/l/z-coord. in AMS.r.s
//
//  PmCells/SubCells(pixels) numbering:
//                |0|1|
// PM1(at -X/Y)>> ----- >>PM36(at +X/Y)
//                |2|3|
//
  void ECALDBc::getscinfoa(integer isl, integer pmc, integer sc,
         integer &pr, integer &pl, integer &cell, number &ct, number &cl, number &cz){
//
  int i,ip,sl,fl,fb,fi,pm,ce,nf[4];
  number z,z11,t,tleft;
  static int first=0;
  static number cool[2];
  static number coot[ECPMSMX][4];
  static number cooz[ECSLMX][4];
//
  int nsl=_slstruc[2];// numb.of super-layers
  int nfl=_slstruc[1];// numb.of fiber-layers per super-layer
  int npm=_slstruc[3];// numb.of PM's per super-layer
  int nfb;
  geant dzrad1=_gendim[8];// z-size of 1SL EC radiator(lead)
  geant alpth=_gendim[9];// Al-plate(on rad. top/bot) thickness
  geant pmpit=_rdcell[6];//PM-pitch(transv)
  geant pxsiz=_rdcell[4];// SubCell(pixel) size
  geant piz=_fpitch[1];
  geant pizz,dz;
//
  #ifdef __AMSDEBUG__
    if(ECALDBc::debug){
      assert(isl>=0 && isl<nsl);
      assert(pmc>=0 && isl<npm);
      assert(sc>=0 && sc<4);
    }
  #endif
//
    if(first==0){// <--- first call actions(prepare some static tables):
      if(nfl%2 != 0){// imply even number (no fiber sharing between top/bot pixels)
        cerr<<"ECALDBc::getscinfoa: wrong number of fiber layers per super layer, nfl="<<nfl<<endl;
	exit(1);
      }
      first=1;
      pizz=dzrad1-(nfl-1)*piz+2.*alpth;// fib-layer dist. in two adjacent super-layers
      dz=(dzrad1+2.*alpth)*nsl;//EC rad. total thickness(incl. Al-plates, excl. Honeyc)
      z11=(nsl*(nfl-1)*piz+(nsl-1)*pizz)/2.;//zpos(ECAL r.s.) of 1st f-layer of 1st S-layer
      z=z11+(_gendim[6]-dz/2.);//zpos(AMS r.s.) of 1st f-layer of 1st S-layer
//      cout<<"ECALDBc::getscinfoa: 1st fiber-layer Zpos="<<z<<endl;
      for(sl=0;sl<nsl;sl++){// <--- S-layer loop for Z-calc.
        for(i=0;i<4;i++){
	  nf[i]=0;
	  cooz[sl][i]=0.;
	}
        for(fl=0;fl<nfl;fl++){// <--- F-layer loop
	  if(fl < nfl/2)fi=1;
	  else fi=3;
	  nf[fi-1]+=1;
	  cooz[sl][fi-1]+=z;
	  nf[fi]+=1;
	  cooz[sl][fi]+=z;
	  z-=piz;
	}
	z+=piz;//remove extra piz-shift
	z-=pizz;//add pizz-shift while going from one s-layer to the next one
	for(i=0;i<4;i++)cooz[sl][i]/=geant(nf[i]);// z-COG's (AMS r.s.)
      }
// <--- now calc. tranv. COG's for all SubCells of ONE S-layer:
//      (neglecting fiber sharing effects)
//
      tleft=-(npm-1)*pmpit/2.;//1st PM-center transv.position in ECAL r.s.
      for(pm=0;pm<npm;pm++){// <--- PMCell loop for t-coo. calc.
        t=tleft+pm*pmpit;
	coot[pm][0]=t-pxsiz/2;
	coot[pm][2]=coot[pm][0];
	coot[pm][1]=t+pxsiz/2;
	coot[pm][3]=coot[pm][1];
      }
#ifdef __AMSDEBUG__
      cout<<"ECALDBc::getscinfo-test:"<<endl;
      cout<<"1st SubCell in 1st plane coot(EC r.s.)/cooz="<<coot[0][0]<<" "<<cooz[0][0]<<endl;      
      cout<<"last SubCell in 1st plane coot(EC r.s.)/cooz="<<coot[npm-1][1]<<" "<<cooz[0][1]<<endl;      
      cout<<"1st SubCell in 2nd plane coot(EC r.s.)/cooz="<<coot[0][2]<<" "<<cooz[0][2]<<endl;      
      cout<<"1st SubCell in last plane coot(EC r.s.)/cooz="<<coot[0][2]<<" "<<cooz[nsl-1][2]<<endl;      
      cout<<"last SubCell in last plane coot(EC r.s.)/cooz="<<coot[npm-1][3]<<" "<<cooz[nsl-1][3]<<endl;      
//
#endif
    }
//----------- each call actions :
    else{
      ip=isl%2;
      if(ip==0)pr=_slstruc[0];// proj (0/1->X/Y)
      else pr=1-_slstruc[0];
      pl=isl*2+(sc/2);// plane (continious numbering of SubCell-planes over 2 proj)
      cell=pmc*2+(sc%2);// Cell in SubCell-plane
      cz=cooz[isl][sc];// z-coo (AMS r.s.)
      ct=coot[pmc][sc];// transv.coo (ECAL r.s.)
      if(pr==0){
        ct+=_gendim[4];//t-coo for X-proj (AMS r.s.)
	cl=_gendim[5];// l-coo .....
      }
      else{
        ct+=_gendim[5];//t-coo for Y-proj (AMS r.s.)
	cl=_gendim[4];// l-coo .....
      }
    }
// 
}
//----------------
// this function return Cell abs.coo: Ctransv/Clongit/Cz->ico=0/1/2;
// plane:0-...; cell:0-...; coo: in cm
  number ECALDBc::CellCoo(integer plane, integer cell, integer ico){
    integer isl,pmc,sc,pr,pl,cel;
    number ct,cl,cz,coo(0);
    isl=plane/2;
    pmc=cell/2;
    if(plane%2==0)sc=cell%2;
    else sc=cell%2+2;
    ECALDBc::getscinfoa(isl, pmc, sc, pr, pl, cel, ct, cl, cz);
    if(ico==0)coo=ct;
    if(ico==1)coo=cl;
    if(ico==2)coo=cz;
    return coo;
  }


  number ECALDBc::CellSize(integer plane, integer cell, integer ico){
  // icoo - 0 transverse
  //        1  longit
  //        2  z
// temporary -> work for eugeni
  
    if(ico==0){    //transverse
     return _rdcell[4];
    }
    else if(ico==1){  //longit
     return (gendim(1)+gendim(2))/4;  //wrong - ask eugeni to correct
    }
    else{
     cerr<<"ECALDBc::CellSize-S-NotYetImplemented "<<ico<<endl;
     abort();
     return 0;
    }
  }



//==========================================================================
//
//   EcalJobStat static variables definition (memory reservation):
//
integer EcalJobStat::mccount[ECJSTA];
integer EcalJobStat::recount[ECJSTA];
integer EcalJobStat::cacount[ECJSTA];
integer EcalJobStat::srcount[20];
number EcalJobStat::zprmc1[ECSLMX];// mc-hit average Z-profile(SL-layers) 
number EcalJobStat::zprmc2[ECSLMX];// mc-hit(+att) average Z-profile(SL(PM-assigned)-layers) 
number EcalJobStat::zprofa[2*ECSLMX];// average Z-profile(scPlanes) 
number EcalJobStat::zprofapm[ECSLMX];// average Z-profile(pm-layers) 
number EcalJobStat::zprofac[ECSLMX];// SL Edep profile (punch-through events)
geant EcalJobStat::nprofac[ECSLMX];// SL profile (punch-through events)
integer EcalJobStat::daqc1[ECJSTA];//daq-decoding counters
integer EcalJobStat::daqc2[ecalconst::ECRT][ECJSTA];
integer EcalJobStat::daqc3[ecalconst::ECRT][ECSLOTS][ECJSTA];
//
//------------------------------------------
void EcalJobStat::clear(){
  int i,j,k;
  for(i=0;i<ECJSTA;i++){
    daqc1[i]=0;
    for(j=0;j<ECRT;j++){
      daqc2[j][i]=0;
      for(k=0;k<ECSLOTS;k++){
        daqc3[j][k][i]=0;
      }
    }
  }
  for(i=0;i<ECJSTA;i++)mccount[i]=0;
  for(i=0;i<ECJSTA;i++)recount[i]=0;
  for(i=0;i<ECJSTA;i++)cacount[i]=0;
  for(i=0;i<20;i++)srcount[i]=0;
  for(i=0;i<ECSLMX;i++)zprmc1[i]=0;
  for(i=0;i<ECSLMX;i++)zprmc2[i]=0;
  for(i=0;i<2*ECSLMX;i++)zprofa[i]=0.;
  for(i=0;i<ECSLMX;i++)zprofapm[i]=0.;
  for(i=0;i<ECSLMX;i++)zprofac[i]=0.;
  for(i=0;i<ECSLMX;i++)nprofac[i]=0;
}
//------------------------------------------
// function to print Job-statistics at the end of JOB(RUN):
void EcalJobStat::printstat(){
//
  printf("\n");
  printf("    =================== ECAL-JOB statistics report ==================\n");
  printf("\n");
  if(daqc1[0]>0){
   printf("     Decoding report :\n");
   printf(" JINF entries                      : % 7d\n",daqc1[0]);
   printf(" non empty                         : % 7d\n",daqc1[1]);
   printf(" no assembly errors                : % 7d\n",daqc1[2]);
   printf(" Crates appearence frequency         crate-1:   crate-2:\n");
   printf(" with side-A readout               : % 7d    % 7d\n",daqc1[3],daqc1[5]);
   printf(" with side-B readout               : % 7d    % 7d\n",daqc1[4],daqc1[6]);
   printf(" RawFMT EDR/ETRG blocks            : % 7d    % 7d\n",daqc2[0][0],daqc2[1][0]);
   printf(" ComprFMT EDR/ETRG blocks          : % 7d    % 7d\n",daqc2[0][1],daqc2[1][1]);
   printf(" RawFMT EDR/ETRG no_assembly_err   : % 7d    % 7d\n",daqc2[0][2],daqc2[1][2]);
   printf(" Illegal EDR/ETRG ID(raw)          : % 7d    % 7d\n",daqc2[0][4],daqc2[1][4]);
   printf(" Illegal EDR/ETRG ID(raw)          : % 7d    % 7d\n",daqc2[0][4],daqc2[1][4]);
   cout<<"---> Crate-1: entries per slot:"<<endl;
   for(int sl=0;sl<7;sl++)cout<<daqc3[0][sl][0]<<" ";
   cout<<endl;
   cout<<"  + block length OK (rawFMT):"<<endl;
   for(int sl=0;sl<7;sl++)cout<<daqc3[0][sl][1]<<" ";
   cout<<endl;
   cout<<"  + block length OK (compFMT):"<<endl;
   for(int sl=0;sl<7;sl++)cout<<daqc3[0][sl][2]<<" ";
   cout<<endl;
//
   cout<<"---> Crate-2: entries per slot:"<<endl;
   for(int sl=0;sl<7;sl++)cout<<daqc3[1][sl][0]<<" ";
   cout<<endl;
   cout<<"  + block length OK (rawFMT):"<<endl;
   for(int sl=0;sl<7;sl++)cout<<daqc3[1][sl][1]<<" ";
   cout<<endl;
   cout<<"  + block length OK (compFMT):"<<endl;
   for(int sl=0;sl<7;sl++)cout<<daqc3[1][sl][2]<<" ";
   cout<<endl;
  }
//
  printf("\n");
//
  if(MISCFFKEY.dbwrbeg>0)return;//dbwriter job, don't need statistics print
//
  printf(" MC: entries                       : % 6d\n",mccount[0]);
  printf(" MC: MCHit->RawEven(ECTrigfl>0) OK : % 6d\n",mccount[1]);
  printf(" MCTrigBuild: entries              : % 6d\n",srcount[0]);
  printf("              Etot>=MIP            : % 6d\n",srcount[1]);
  printf("              Mult>=Low            : % 6d\n",srcount[2]);
  printf("              Mult>=EM(FT OK)      : % 6d\n",srcount[3]);
  printf("              Angle(whenFT) OK     : % 6d\n",srcount[4]);
  printf(" RECO-entries                      : % 6d\n",recount[0]);
  printf(" LVL1-trigs(TOF-FT || EC-FT || Ext): % 6d\n",recount[1]);
  printf(" Validation OK                     : % 6d\n",recount[2]);
  printf(" RawEvent->EcalHit OK              : % 6d\n",recount[3]);
  printf(" EcalHit->EcalCluster OK           : % 6d\n",recount[4]);
  printf(" CatastrRearLeak detected          : % 6d\n",recount[5]);
  number rrr(0);
  if(recount[3]>0)rrr=number(srcount[10])/number(recount[3]);
  printf(" Saturated PMTs per EcalHitOK-event: % 6.4f\n",rrr);
  printf("\n\n");
  if(ECREFFKEY.relogic[1]==1 || ECREFFKEY.relogic[1]==2){
    printf("    ============== RLGA/FIAT part of REUN_Clibration-statistics ===============\n");
    printf("\n");
    printf(" REUN: entries(tof/ec_trigflag OK) : % 6d\n",cacount[0]);
    printf(" REUN: Track found                 : % 6d\n",cacount[1]);
    printf(" REUN: Track charge OK             : % 6d\n",cacount[2]);
    printf(" REUN: Track quality OK            : % 6d\n",cacount[3]);
    printf(" REUN: Track hits EC               : % 6d\n",cacount[4]);
    printf(" REUN: Recognized as PunchTrough   : % 6d\n",cacount[5]);
    printf(" REUN: Etruncated in limits        : % 6d\n",cacount[6]);
    printf(" REUN: At least one PM matched     : % 6d\n",cacount[7]);
    printf("\n\n");
  }
  if(ECREFFKEY.relogic[1]==3){
    printf("    ============== ANOR part of REUN_Clibration-statistics ===============\n");
    printf("\n");
    printf(" REUN: entries(tof/ec_trigflag OK) : % 6d\n",cacount[0]);
    printf(" REUN: ANTI OK                     : % 6d\n",cacount[1]);
    printf(" REUN: Track found                 : % 6d\n",cacount[2]);
    printf(" REUN: Track quality OK            : % 6d\n",cacount[3]);
    printf(" REUN: Track hits ECAL             : % 6d\n",cacount[4]);
    printf(" REUN: FiredSubCells pattern OK    : % 6d\n",cacount[5]);
    printf(" REUN: Efront/Epeak/Etail/Etot OK  : % 6d\n",cacount[6]);
    printf(" REUN: Plane1/6 COG matching OK    : % 6d\n",cacount[7]);
    printf(" REUN: Plane1/6 Ebcg/Esig OK       : % 6d\n",cacount[8]);
    printf("\n\n");
  }
//
}
//------------------------------------------
void EcalJobStat::bookhist(){
  int i,j,k,ich,maxcl,maxpl,maxsl;
  char htit1[60];
  char inum[11];
  char in[2]="0";
//
  maxpl=2*ECSLMX;//MAX planes
  maxcl=2*ECPMSMX;//MAX SubCells per plane
  maxsl=ECSLMX;
  strcpy(inum,"0123456789");
    if(ECREFFKEY.reprtf[0]!=0){ // Book reco-hist
      HBOOK1(ECHISTR+10,"ECRE: RawEvent-hits number",80,0.,400.,0.);
      HBOOK1(ECHISTR+11,"ECRE: RawEvent-hits ADCtot(gain-corr)",200,0.,100000.,0.);
      HBOOK1(ECHISTR+12,"ECRE: RawEvent-hits ADCtot(gain-corr)",100,0.,500.,0.);
      HBOOK1(ECHISTR+13,"ECRE: EcalHit-hits number",80,0.,160.,0.);
      HBOOK1(ECHISTR+14,"ECRE: EcalHit-hits Etot(NoDynCorr,Mev)",200,0.,200000,0.);
      HBOOK1(ECHISTR+9,"ECRE: EcalHit-hit Energy(Mev)",100,0.,100.,0.);
      HBOOK1(ECHISTR+15,"ECRE: EcalHit-hits DyCorrectionEn(tot,Mev)",100,0.,100000,0.);
      HBOOK1(ECHISTR+16,"ECRE: RawEvent-hits value(adc,gain-corr)",200,0.,10000.,0.);
      HBOOK1(ECHISTR+17,"ECRE: RawEvent-hits value(adc,gain-corr)",100,0.,100.,0.);
//      HBOOK1(ECHISTR+18,"ECRE: EcalClust per event",60,0.,120.,0.);
      if(ECREFFKEY.reprtf[1]==1){//<--- to store t-profiles, z-prof
        HBOOK1(ECHISTR+19,"ECRE: T-prof in plane 8(X)",maxcl,1.,geant(maxcl+1),0.);
        HBOOK1(ECHISTR+20,"ECRE: T-prof in plane 9(Y)",maxcl,1.,geant(maxcl+1),0.);
        HBOOK1(ECHISTR+21,"ECRE: Z-profile",maxpl,1.,geant(maxpl+1),0.);
      }
//      HBOOK1(ECHISTR+22,"ECRE: EcalClust value(tot,Mev)",200,0.,1000000,0.);//now not my responsib.
//      HBOOK1(ECHISTR+23,"ECRE: EcalClust value(tot,Mev)",100,0.,50000,0.);
//      HBOOK1(ECHISTR+24,"ECRE: SubCelLayer En-profile(ECHits)",maxpl,1.,geant(maxpl+1),0.);//not implemented
//      HBOOK1(ECHISTR+25,"ECRE: SuperLayer En-profile(ECHits)",maxsl,1.,geant(maxsl+1),0.);
      HBOOK1(ECHISTR+28,"ECRE: TriggerPatternProjX(validate,whenLVL1)",120,1.,121.,0.);
      HBOOK1(ECHISTR+29,"ECRE: TriggerPatternProjY(validate,whenLVL1)",120,1.,121.,0.);
      HBOOK1(ECHISTR+30,"ECRE: Trigger flag(validate,whenLVL1)",40,0.,40.,0.);
      HBOOK1(ECHISTR+31,"ECLVL3: Etot(mev)",100,0.,100000.,0.);
      HBOOK1(ECHISTR+32,"ECLVL3: Efront",80,0.,1600.,0.);
      HBOOK1(ECHISTR+33,"ECLVL3: Epeak/Ebase",80,0.,40.,0.);
      HBOOK1(ECHISTR+34,"ECLVL3: Epeak/Efront",80,0.,40.,0.);
      HBOOK1(ECHISTR+35,"ECLVL3: X-width",80,0.,80.,0.);
      HBOOK1(ECHISTR+36,"ECLVL3: Y-width",80,0.,80.,0.);
      HBOOK1(ECHISTR+37,"ECLVL3: PM-colx signals(mev)",80,0.,240.,0.);
      HBOOK1(ECHISTR+38,"ECLVL3: PM-coly signals(mev)",80,0.,240.,0.);
      HBOOK1(ECHISTR+39,"ECLVL3: EM-flag",3,-1.,2.,0.);
      HBOOK1(ECHISTR+40,"ECLVL3: Z-cog",50,-180.,-130.,0.);
      HBOOK1(ECHISTR+41,"ECLVL3: T-cog,sl1",80,-40.,40.,0.);
      HBOOK1(ECHISTR+42,"ECLVL3: T-cog,sl2",80,-40.,40.,0.);
      HBOOK1(ECHISTR+43,"ECLVL3: chi2y",50,0.,2.5,0.);
      HBOOK1(ECHISTR+44,"ECLVL3: tany",50,-1.,1.,0.);
      HBOOK1(ECHISTR+45,"ECLVL3: DxClosest",80,-80.,80.,0.);
      HBOOK1(ECHISTR+46,"ECLVL3: chi2x",50,0.,2.5,0.);
      HBOOK1(ECHISTR+47,"ECLVL3: tanx",50,-1.,1.,0.);
      HBOOK1(ECHISTR+48,"ECLVL3: DyClosest",80,-80.,80.,0.);
      HBOOK1(ECHISTR+49,"ECLVL3: SL1-rms",50,0.,20.,0.);
      HBOOK1(ECHISTR+50,"ECLVL3: SL2-rms",50,0.,20.,0.);
      HBOOK1(ECHISTR+51,"ECLVL3: SL8-rms",50,0.,20.,0.);
      HBOOK1(ECHISTR+52,"ECLVL3: SL9-rms",50,0.,20.,0.);
      HBOOK1(ECHISTR+53,"ECLVL3: Xhigh-Xlow",50,0.,50.,0.);
      HBOOK1(ECHISTR+54,"ECLVL3: Yhigh-Ylow",50,0.,50.,0.);
      HBOOK1(ECHISTR+55,"ECLVL3: Xec-Xtr",80,-80.,80.,0.);
      HBOOK1(ECHISTR+56,"ECLVL3: Yec-Ytr",80,-80.,80.,0.);
//
      if(ECREFFKEY.relogic[1]==1 || ECREFFKEY.relogic[1]==2){// RLGA/FIAT part of REUN-calibration
        HBOOK1(ECHISTC,"ECCA: Track COS(theta) at EC front",100,-1.,1.,0.);
        HBOOK1(ECHISTC+1,"ECCA: Track Imp.point X, SL1",70,-70.,70.,0.);
        HBOOK1(ECHISTC+2,"ECCA: Track Imp.point Y, SL1",70,-70.,70.,0.);
        HBOOK1(ECHISTC+3,"ECCA: PM-Track Transv-dist,SL1",50,-5.,5.,0.);
        HBOOK1(ECHISTC+4,"ECCA: PM-Track Transv-dist,SL2",50,-5.,5.,0.);
        HBOOK1(ECHISTC+5,"ECCA: PM-Track Longit-dist,SL1,PM18 ",70,0.,70.,0.);
        HBOOK1(ECHISTC+6,"ECCA: Track-fit Chi2 ",80,0.,20.,0.);
//    hist # +7 is booked inside mfit !!!
        HBOOK1(ECHISTC+8,"ECCA: SubCell Efficiency",50,0.2,1.2,0.);
        HBOOK1(ECHISTC+9,"ECCA: SubCell RelativeGain",50,0.5,1.5,0.);
        HBOOK1(ECHISTC+10,"ECCA: RefPmResp. uniformity",ECCLBMX,1.,geant(ECCLBMX+1),0.);
        HBOOK1(ECHISTC+11,"ECCA: PM relat.gains",100,0.5,1.5,0.);
        HBOOK1(ECHISTC+12,"ECCA: Rigidity (gv)",100,0.,100.,0.);
        HBOOK1(ECHISTC+13,"ECCA: PM-RelGain L-profile",ECSLMX,1.,geant(ECSLMX+1),0.);
	HMINIM(ECHISTC+13,0.9);
	HMAXIM(ECHISTC+13,1.1);
        HBOOK1(ECHISTC+14,"ECCA: SubCell Efficiency L-profile",maxpl,1.,geant(maxpl+1),0.);
        HBOOK1(ECHISTC+15,"ECCA: PM Eff vs SL(full fib.length)",maxsl,1.,geant(maxsl+1),0.);
	if(ECCAFFKEY.truse==1){//He4
	  HMINIM(ECHISTC+14,0.8);
	  HMAXIM(ECHISTC+14,1.);
	  HMINIM(ECHISTC+15,0.85);
	  HMAXIM(ECHISTC+15,1.05);
	}
	else{//prot
	  HMINIM(ECHISTC+14,0.4);
	  HMAXIM(ECHISTC+14,0.8);
	  HMINIM(ECHISTC+15,0.5);
	  HMAXIM(ECHISTC+15,1.1);
	}
        HBOOK1(ECHISTC+16,"ECCA: Edep/SLayer(PunchThrough,mev)",100,0.,200.,0.);
        HBOOK1(ECHISTC+17,"ECCA: Bad(non PunchThrough) scLayers",maxpl+1,0.,geant(maxpl+1),0.);
        HBOOK1(ECHISTC+18,"ECCA: SLayerEdep prof(punch-through)",maxsl,1.,geant(maxsl+1),0.);
	HBOOK2(ECHISTC+19,"ECCA: RefPmSc Alow vs Ahigh",80,20.,260.,30,0.,30.,0.);
        HBOOK1(ECHISTC+20,"ECCA: Slop(h2lcalib,all chan)",80,8.,24.,0.);
        HBOOK1(ECHISTC+21,"ECCA: Offs(h2lcalib,all chan)",80,-40.,40.,0.);
        HBOOK1(ECHISTC+22,"ECCA: Chi2(h2lcalib,all chan)",80,0.,8.,0.);
        HBOOK1(ECHISTC+23,"ECCA: LowChBinRMS/Aver(h2lcalib,all chan)",80,0.,1.,0.);
        HBOOK1(ECHISTC+24,"ECCA: EcalHit Energy(in adc-units)",100,0.,100.,0.);
        HBOOK1(ECHISTC+25,"ECCA: Fired(above thr) SubCells/Layer",80,0.,80.,0.);
        HBOOK1(ECHISTC+26,"ECCA: SubCell eff(even SL) ",80,0.5,1.3,0.);
        HBOOK1(ECHISTC+27,"ECCA: SubCell eff( odd SL) ",80,0.5,1.3,0.);
        HBOOK1(ECHISTC+28,"ECCA: SL number(punch-through)",maxsl,1.,geant(maxsl+1),0.);
        HBOOK1(ECHISTC+29,"ECCA: PM spectrum(trk-matched,X-pr)",100,0.,100.,0.);
        HBOOK1(ECHISTC+30,"ECCA: PM spectrum(trk-matched,Y-pr)",100,0.,100.,0.);
        HBOOK1(ECHISTC+31,"ECCA: PM eff(even SL) ",80,0.5,1.3,0.);
        HBOOK1(ECHISTC+32,"ECCA: PM eff( odd SL) ",80,0.5,1.3,0.);
// test	HBOOK1(ECHISTC+33,"ECCA: TRK imppoint X-accur",60,-0.3,0.3,0.); 
// test	HBOOK1(ECHISTC+34,"ECCA: TRK imppoint Y-accur",60,-0.3,0.3,0.); 
        HBOOK1(ECHISTC+35,"ECCA: Track beta",96,-1.2,1.2,0.);
	HBOOK1(ECHISTC+36,"ECCA: Track Z (from tracker)",16,0.,16.,0.);
      }
      if(ECREFFKEY.relogic[1]==3){// =====> ANOR part of REUN-calibration
        HBOOK1(ECHISTC,"ECCA: Track COS(theta) at EC front",100,-1.,1.,0.);
        HBOOK1(ECHISTC+1,"ECCA: Track Chi2(FastFit) ",80,0.,40.,0.);
        HBOOK1(ECHISTC+2,"ECCA: Track rigidity (Gv,FastFit)",100,2.,12.,0.);
        HBOOK1(ECHISTC+3,"ECCA: Track dR/R(FastFit)",100,0.,0.05,0.);
        HBOOK1(ECHISTC+4,"ECCA: Track half-rig. assimetry(AdvFit)",80,-0.4,0.4,0.);
        HBOOK1(ECHISTC+5,"ECCA: EcalHit Energy(in adc-units)",100,0.,100.,0.);
        HBOOK1(ECHISTC+6,"ECCA: Fired(above thr) SubCells/Layer",80,0.,80.,0.);
        HBOOK1(ECHISTC+7,"ECCA: Fired(above thr) SubCells/Layer1/2/3/4/5/6",120,0.,120.,0.);
        HBOOK1(ECHISTC+8,"ECCA: Bad SC-Layers(spikes,high multipl.",maxpl+1,0.,geant(maxpl+1),0.);
        HBOOK1(ECHISTC+9,"ECCA: Edep total(mev)",200,0.,20000.,0.);
        HBOOK1(ECHISTC+10,"ECCA: Side Eleak/Etot",100,0.,0.5,0.);
        HBOOK1(ECHISTC+11,"ECCA: Edep in 1st 4X0(mev)",100,0.,1000.,0.);
        HBOOK1(ECHISTC+12,"ECCA: Etail/Epeak",100,0.,1.,0.);
        HBOOK1(ECHISTC+13,"ECCA: Edep/Mom-1(Efr,Epeak/Etail cuts)",100,-1.,1.,0.);
        HBOOK1(ECHISTC+14,"ECCA: Max. dist of Track and SC",100,0.,50.,0.);
        HBOOK1(ECHISTC+15,"ECCA: Track-COG(planes 5,6) dist",100,-10.,10.,0.);
        HBOOK1(ECHISTC+16,"ECCA: Edep/Mom(all cuts)",100,0.,2.,0.);
        HBOOK1(ECHISTC+17,"ECCA: Back Eleak/Etot",100,0.,0.5,0.);
        HBOOK1(ECHISTC+18,"ECCA: Mom/Edep(all cuts)",100,0.,2.,0.);
        HBOOK1(ECHISTC+19,"ECCA: Max.value of SubCell ADC",100,0.,5000.,0.);
        HBOOK1(ECHISTC+20,"ECCA: SCplane1/2/3/4/5/6 holes",120,0.,120.,0.);
        HBOOK1(ECHISTC+21,"ECCA: Max. dist of Track and SC(pl 1)",100,0.,50.,0.);
        HBOOK1(ECHISTC+22,"ECCA: Track-COG(plane 1,2) dist",100,-10.,10.,0.);
        HBOOK1(ECHISTC+23,"ECCA: Track-COG(plane 3,4) dist",100,-10.,10.,0.);
        HBOOK1(ECHISTC+24,"ECCA: Spikes/plane",80,0.,80.,0.);
        HBOOK1(ECHISTC+25,"ECCA: Layers with early showering(holes)",10,0.,10.,0.);
        HBOOK1(ECHISTC+26,"ECCA: Edep(all cuts,attf corr,mev)",100,0.,20000.,0.);
        HBOOK1(ECHISTC+27,"ECCA: Edep(all cuts,no attf.corr,mev)",100,0.,20000.,0.);
        HBOOK1(ECHISTC+28,"ECCA: Rigidity(all cuts,mev)",100,2000.,12000.,0.);
        HBOOK1(ECHISTC+29,"ECCA: Z-profile(all cuts)",maxpl,0.,geant(maxpl),0.);
        HBOOK1(ECHISTC+30,"ECCA: Tracker backgr. Ycl/layer",50,0.,50.,0.);
        HBOOK1(ECHISTC+31,"ECCA: Tracker backgr. Xcl/layer",50,0.,50.,0.);
        HBOOK1(ECHISTC+32,"ECCA: Tracker X-layer backgr.",50,0.,10.,0.);
        HBOOK1(ECHISTC+33,"ECCA: Tracker Y-layer backgr.",50,0.,10.,0.);
        HBOOK1(ECHISTC+34,"ECCA: Track X-layer ampl.",80,0.,400.,0.);
        HBOOK1(ECHISTC+35,"ECCA: Track Y-layer ampl.",80,0.,400.,0.);
        HBOOK1(ECHISTC+36,"ECCA: Track rigidity (Gv,FastFit,BadClam)",100,2.,12.,0.);
        HBOOK1(ECHISTC+37,"ECCA: Track Xcross At ECTop",70,-35.,35.,0.);
        HBOOK1(ECHISTC+38,"ECCA: Track Ycross At ECTop",70,-35.,35.,0.);
	HBOOK1(ECHISTC+39,"ECCA: ANTI sector energy",80,0.,40.,0.);
	HBOOK1(ECHISTC+40,"ECCA: ANTI fired sectors",17,0.,17.,0.);
        HBOOK1(ECHISTC+41,"ECCA: Track rigidity (Gv,FastFit,TrackOK)",100,2.,12.,0.);
        HBOOK1(ECHISTC+42,"ECCA: Track Layer ampl.",80,0.,400.,0.);
        HBOOK1(ECHISTC+43,"ECCA: Track rigidity (Gv,FastFit,BadEbkg)",100,2.,12.,0.);
        HBOOK1(ECHISTC+44,"ECCA: Track beta",96,-1.2,1.2,0.);
        HBOOK1(ECHISTC+50,"ECCA: Dist. of Track and SC(pl1)",80,-8.,8.,0.);
        HBOOK1(ECHISTC+51,"ECCA: Dist. of Track and SC(pl2)",80,-8.,8.,0.);
        HBOOK1(ECHISTC+52,"ECCA: Dist. of Track and SC(pl3)",80,-8.,8.,0.);
        HBOOK1(ECHISTC+53,"ECCA: Dist. of Track and SC(pl4)",80,-8.,8.,0.);
        HBOOK1(ECHISTC+54,"ECCA: Dist. of Track and SC(pl5)",80,-8.,8.,0.);
        HBOOK1(ECHISTC+55,"ECCA: Dist. of Track and SC(pl6)",80,-8.,8.,0.);
	HBOOK1(ECHISTC+56,"ECCA: Emism/Ematch(pl1)",80,0.,4.,0.);
	HBOOK1(ECHISTC+57,"ECCA: Emism/Ematch(pl2)",80,0.,4.,0.);
	HBOOK1(ECHISTC+58,"ECCA: Emism/Ematch(pl3)",80,0.,4.,0.);
	HBOOK1(ECHISTC+59,"ECCA: Emism/Ematch(pl4)",80,0.,4.,0.);
	HBOOK1(ECHISTC+60,"ECCA: Emism/Ematch(pl5)",80,0.,4.,0.);
	HBOOK1(ECHISTC+61,"ECCA: Emism/Ematch(pl6)",80,0.,4.,0.);
//
//  WARNING: Hist.ECHISTC+100->ECHISTC+189 are reserved for Ped-Calibration 
//gchen
	if(ECCAFFKEY.ecshswit==1){// =====> ecal_shower used.
	  HBOOK1(ECHISTC+62,"ECCA: shower energy, track ok",80,0.,18.,0.);
	  HBOOK1(ECHISTC+63,"ECCA: chi2 of dir fit",100,0.,100.,0.);
	  HBOOK1(ECHISTC+64,"ECCA: Energy of Front layers",125,0.,250.,0.);
	  HBOOK1(ECHISTC+65,"ECCA: Dif. over sum",80,-1.,0.6,0.);
	  HBOOK1(ECHISTC+66,"ECCA: Rel. Side leak",40,0.,0.4,0.);
	  HBOOK1(ECHISTC+67,"ECCA: Rel. Rear Leak",40,0.,0.4,0.);
	  HBOOK1(ECHISTC+68,"ECCA: Rel. Dead Cell Leak",20,0.,0.2,0.);
	  HBOOK1(ECHISTC+69,"ECCA: out of core leak",50,0.,0.5,0.);
	  HBOOK1(ECHISTC+70,"ECCA: chi2 of prof. fit",50,0.,35.,0.);
	  HBOOK1(ECHISTC+71,"ECCA: chi2 of trans. fit",50,0.,35.,0.);
	  HBOOK1(ECHISTC+72,"ECCA: Mom/Edep",100,0.,2.,0.);
	  HBOOK1(ECHISTC+73,"ECCA: Eshower/Mom-1",100,-1.,1.,0.);
	  HBOOK1(ECHISTC+74,"ECCA: Track-shower entry x dist",100,-10.,10.,0.);
	  HBOOK1(ECHISTC+75,"ECCA: Track-shower entry y dist",100,-10.,10.,0.);
	  HBOOK1(ECHISTC+76,"ECCA: Track-shower exit x dist",100,-10.,10.,0.);
	  HBOOK1(ECHISTC+77,"ECCA: Track-shower exit y dist",100,-10.,10.,0.);
	  HBOOK1(ECHISTC+78,"ECCA: Edep/Mom(all cuts)",100,0.,2.,0.);
	  HBOOK1(ECHISTC+79,"ECCA: Edep/Mom",100,0.,2.,0.);
	  HBOOK1(ECHISTC+80,"ECCA: Mom/Edep(all cuts)",100,0.,2.,0.);
	  HBOOK1(ECHISTC+81,"ECCA: Track beta",96,-1.2,1.2,0.);
	  HBOOK1(ECHISTC+82,"ECCA: Track rigidity (Gv,FastFit,BadEbkg)",100,2.,12.,0.);
	  HBOOK1(ECHISTC+83,"ECCA: Track Chi2(FastFit) ",80,0.,40.,0.);
	  HBOOK1(ECHISTC+84,"ECCA: Track rigidity (Gv,FastFit)",100,2.,12.,0.);
	  HBOOK1(ECHISTC+85,"ECCA: Track dR/R(FastFit)",100,0.,0.05,0.);
	  HBOOK1(ECHISTC+86,"ECCA: Track half-rig. assymetry(AdvFit)",80,-0.4,0.4,0.);
	  HBOOK1(ECHISTC+87,"ECCA: Track COS(theta) at EC front",100,-1.,1.,0.);
	  HBOOK1(ECHISTC+88,"ECCA: Track rigidity (Gv,FastFit,TrackOK)",100,2.,12.,0.);
	  HBOOK1(ECHISTC+89,"ECCA: Track Layer ampl.",80,0.,400.,0.);
	  HBOOK1(ECHISTC+90,"ECCA: Rigidity(all cuts,mev)",100,2000.,12000.,0.);
	  HBOOK1(ECHISTC+91,"ECCA: ANTI fired sectors",17,0.,17.,0.);
	  HBOOK1(ECHISTC+92,"ECCA: Tracker backgr. Ycl/layer",50,0.,50.,0.);
	  HBOOK1(ECHISTC+93,"ECCA: Tracker backgr. Xcl/layer",50,0.,50.,0.);
	  HBOOK1(ECHISTC+94,"ECCA: Tracker X-layer backgr.",50,0.,10.,0.);
	  HBOOK1(ECHISTC+95,"ECCA: Tracker Y-layer backgr.",50,0.,10.,0.);
	  HBOOK1(ECHISTC+96,"ECCA: Track X-layer ampl.",80,0.,400.,0.);
	  HBOOK1(ECHISTC+97,"ECCA: Track Y-layer ampl.",80,0.,400.,0.);
	  HBOOK1(ECHISTC+98,"ECCA: Track rigidity (Gv,FastFit,BadClam)",100,2.,12.,0.);
	  HBOOK1(ECHISTC+99,"ECCA: Track Xcross At ECTop",70,-35.,35.,0.);
	  HBOOK1(ECHISTC+100,"ECCA: Track Ycross At ECTop",70,-35.,35.,0.);
	  HBOOK1(ECHISTC+101,"ECCA: ANTI sector energy",80,0.,40.,0.);
	  HBOOK1(ECHISTC+102,"ECCA: energy error of shower",50,0.,5.,0.);
	  HBOOK1(ECHISTC+103,"ECCA: Rigidity(track ok)",100,2000.,12000.,0.);
	  HBOOK1(ECHISTC+104,"ECCA: shower energy(chi2 ok)",80,0.,18.,0.);
	  HBOOK1(ECHISTC+105,"ECCA: Rigidity(chi2 ok)",100,2000.,12000.,0.);
	  HBOOK1(ECHISTC+106,"ECCA: shower energy(s/b leak ok)",80,0.,18.,0.);
	  HBOOK1(ECHISTC+107,"ECCA: Rigidity(s/b leak ok)",100,2000.,12000.,0.);
	  HBOOK1(ECHISTC+108,"ECCA: shower energy(d/o leak ok)",80,0.,18.,0.);
	  HBOOK1(ECHISTC+109,"ECCA: Rigidity(d/o leak ok)",100,2000.,12000.,0.);
	  HBOOK1(ECHISTC+110,"ECCA: shower energy(difsum ok)",80,0.,18.,0.);
	  HBOOK1(ECHISTC+111,"ECCA: Rigidity(difsum ok)",100,2000.,12000.,0.);
	  HBOOK1(ECHISTC+112,"ECCA: shower energy(all cuts ok)",80,0.,18.,0.);
	  HBOOK1(ECHISTC+113,"ECCA: Rigidity(all cuts ok)",100,2000.,12000.,0.);
	}
      }
    }
//
}
//-----------------------------
void EcalJobStat::bookhistmc(){
    if(ECMCFFKEY.mcprtf!=0){ // Book mc-hist
      HBOOK1(ECHIST+1,"Geant-hits number",100,0.,5000.,0.);
      HBOOK1(ECHIST+2,"ECMC: GeantdE/dX-hits Etot(MeV)",100,0.,500,0.);
      HBOOK1(ECHIST+3,"ECMC: GeantdE/dX-hits Etot(+FiberAtt,MeV)",100,0.,500.,0.);
      HBOOK1(ECHIST+4,"ECMC: GeantEvisTot(after mev2mev+NpeFluct)",200,0.,200000.,0.);
      HBOOK1(ECHIST+5,"ECMC: Indiv.Dynode signals(FTinput, mev)",100,0.,50.,0.);
      HBOOK1(ECHIST+6,"ECMC: 4xA/D(FT)-signal ratio",50,0.,10.,0.);
      HBOOK1(ECHIST+7,"ECMC: EmcHits SL-profile",ECSLMX,1.,geant(ECSLMX+1),0.);
      HBOOK1(ECHIST+8,"ECMC: EmcHits SL(PM-assigned)-profile",ECSLMX,1.,geant(ECSLMX+1),0.);
      HBOOK1(ECHIST+9,"ECMC: Etot(DynodeTrigSum,mev)",200,0.,100000.,0.);
      HBOOK1(ECHIST+10,"ECMC: ECTriggerConf(IJ->Energ|Angle Proj-Or(1)/And(2))",30,0.,30.,0.);
      HBOOK1(ECHIST+19,"ECMC: ECTriggerFlag(IJ->Mult(0-3)|Angle(0-2))",40,0.,40.,0.);
      HBOOK1(ECHIST+20,"ECMC: Tot.Anode charge(4subc.sum, pC)",100,0.,20.,0.);
      HBOOK1(ECHIST+21,"ECMC: Max TotAnodeCharge(4subc.sum, pC)",100,0.,1000.,0.);
      HBOOK1(ECHIST+22,"ECMC: Max ADC-H(incl.ped, No ovfl.limit)",100,0.,4100.,0.);
      HBOOK1(ECHIST+23,"ECMC: Max ADC-L(incl.ped, No ovfl.limit)",100,0.,4100.,0.);
      HBOOK1(ECHIST+24,"ECMC: Max ADC-D(incl.ped, No ovfl.limit)",100,0.,4100.,0.);
      HBOOK1(ECHIST+30,"ECMC: TrgDynSignalsMax(mev),SL1",80,0.,400.,0.);
      HBOOK1(ECHIST+31,"ECMC: TrgDynSignalsMax(mev),SL2",80,0.,400.,0.);
      HBOOK1(ECHIST+32,"ECMC: TrgDynSignalsMax(mev),SL3",80,0.,400.,0.);
      HBOOK1(ECHIST+33,"ECMC: TrgDynSignalsMax(mev),SL4",80,0.,400.,0.);
      HBOOK1(ECHIST+34,"ECMC: TrgDynSignalsMax(mev),SL5",80,0.,400.,0.);
      HBOOK1(ECHIST+35,"ECMC: TrgDynSignalsMax(mev),SL6",80,0.,400.,0.);
      HBOOK1(ECHIST+36,"ECMC: TrgDynSignalsMax(mev),SL7",80,0.,400.,0.);
      HBOOK1(ECHIST+37,"ECMC: TrgDynSignalsMax(mev),SL8",80,0.,400.,0.);
      HBOOK1(ECHIST+38,"ECMC: TrgDynSignalsMax(mev),SL9",80,0.,400.,0.);
      HBOOK1(ECHIST+39,"ECMC: NonEmptySLs(BendYproj)",10,0.,10.,0.);
      HBOOK1(ECHIST+40,"ECMC: NonEmptySLs(NonBendXproj)",10,0.,10.,0.);
      HBOOK1(ECHIST+41,"ECMC: MaxCOGShift(Yproj)",50,0.,10.,0.);
      HBOOK1(ECHIST+42,"ECMC: MaxCOGShift(Xproj)",50,0.,10.,0.);
      HBOOK1(ECHIST+43,"ECMC: DyTrigEnerTot(mev)",80,0.,2000.,0.);
    }
}
//----------------------------
void EcalJobStat::outp(){
  geant rzprofa[2*ecalconst::ECSLMX],rzprofapm[ecalconst::ECSLMX],rzprofac[ecalconst::ECSLMX];
    if(ECREFFKEY.reprtf[0]!=0){ // print RECO-hists
      HPRINT(ECHISTR+10);
      HPRINT(ECHISTR+11);
      HPRINT(ECHISTR+12);
      HPRINT(ECHISTR+13);
      HPRINT(ECHISTR+14);
      HPRINT(ECHISTR+9);
      HPRINT(ECHISTR+15);
      HPRINT(ECHISTR+16);
      HPRINT(ECHISTR+17);
//      HPRINT(ECHISTR+18);
//      HPRINT(ECHISTR+22);
//      HPRINT(ECHISTR+23);
      if(recount[2]>0){
//        for(int i=0;i<2*ECSLMX;i++)rzprofa[i]=geant(zprofa[i]/recount[2]);
//        for(int i=0;i<ECSLMX;i++)rzprofapm[i]=geant(zprofapm[i]/recount[2]);
//        HPAK(ECHISTR+24,rzprofa);
//        HPAK(ECHISTR+25,rzprofapm);
//        HPRINT(ECHISTR+24);
//        HPRINT(ECHISTR+25);
      }
      HPRINT(ECHISTR+28);
      HPRINT(ECHISTR+29);
      HPRINT(ECHISTR+30);
      HPRINT(ECHISTR+31);
      HPRINT(ECHISTR+32);
      HPRINT(ECHISTR+33);
      HPRINT(ECHISTR+34);
      HPRINT(ECHISTR+35);
      HPRINT(ECHISTR+36);
      HPRINT(ECHISTR+37);
      HPRINT(ECHISTR+38);
      HPRINT(ECHISTR+39);
      HPRINT(ECHISTR+40);
      HPRINT(ECHISTR+41);
      HPRINT(ECHISTR+42);
      HPRINT(ECHISTR+43);
      HPRINT(ECHISTR+44);
      HPRINT(ECHISTR+45);
      HPRINT(ECHISTR+46);
      HPRINT(ECHISTR+47);
      HPRINT(ECHISTR+48);
      HPRINT(ECHISTR+49);
      HPRINT(ECHISTR+50);
      HPRINT(ECHISTR+51);
      HPRINT(ECHISTR+52);
      HPRINT(ECHISTR+53);
      HPRINT(ECHISTR+54);
      HPRINT(ECHISTR+55);
      HPRINT(ECHISTR+56);
    }
    if(ECREFFKEY.relogic[1]==1 || ECREFFKEY.relogic[1]==2){ // print RLGA/FIAT-hists
      HPRINT(ECHISTC);
      HPRINT(ECHISTC+6);
      HPRINT(ECHISTC+35);
      HPRINT(ECHISTC+36);
      HPRINT(ECHISTC+1);
      HPRINT(ECHISTC+2);
      HPRINT(ECHISTC+3);
      HPRINT(ECHISTC+4);
      HPRINT(ECHISTC+5);
      ECREUNcalib::mfit();
      HPRINT(ECHISTC+8);
      HPRINT(ECHISTC+9);
//      HPRINT(ECHISTC+10);//already printed inside mfit
      HPRINT(ECHISTC+11);
      HPRINT(ECHISTC+12);
      HPRINT(ECHISTC+13);
      HPRINT(ECHISTC+14);
      HPRINT(ECHISTC+15);
      HPRINT(ECHISTC+16);
      HPRINT(ECHISTC+17);
      for(int i=0;i<ECSLMX;i++){
        if(nprofac[i]>0)rzprofac[i]=geant(zprofac[i]/nprofac[i]);
	else rzprofac[i]=0;
      }
      HPAK(ECHISTC+18,rzprofac);
      HPRINT(ECHISTC+18);
      HPRINT(ECHISTC+28);
      HPRINT(ECHISTC+19);
      HPRINT(ECHISTC+20);
      HPRINT(ECHISTC+21);
      HPRINT(ECHISTC+22);
      HPRINT(ECHISTC+23);
      HPRINT(ECHISTC+24);
      HPRINT(ECHISTC+25);
      HPRINT(ECHISTC+26);
      HPRINT(ECHISTC+27);
      HPRINT(ECHISTC+29);
      HPRINT(ECHISTC+30);
      HPRINT(ECHISTC+31);
      HPRINT(ECHISTC+32);
// test      HPRINT(ECHISTC+33);
// test      HPRINT(ECHISTC+34);
    }
    if(ECREFFKEY.relogic[1]==3){ // print ANOR-hists
      HPRINT(ECHISTC+39);
      HPRINT(ECHISTC+40);
      HPRINT(ECHISTC);
      HPRINT(ECHISTC+1);
      HPRINT(ECHISTC+2);
      HPRINT(ECHISTC+3);
      HPRINT(ECHISTC+4);
      HPRINT(ECHISTC+44);
      HPRINT(ECHISTC+30);
      HPRINT(ECHISTC+31);
      HPRINT(ECHISTC+32);
      HPRINT(ECHISTC+33);
      HPRINT(ECHISTC+34);
      HPRINT(ECHISTC+35);
      HPRINT(ECHISTC+36);
      HPRINT(ECHISTC+43);
      HPRINT(ECHISTC+42);
      HPRINT(ECHISTC+41);
      HPRINT(ECHISTC+37);
      HPRINT(ECHISTC+38);
      HPRINT(ECHISTC+5);
      HPRINT(ECHISTC+6);
      HPRINT(ECHISTC+7);
      HPRINT(ECHISTC+8);
      HPRINT(ECHISTC+9);
      HPRINT(ECHISTC+10);
      HPRINT(ECHISTC+17);
      HPRINT(ECHISTC+11);
      HPRINT(ECHISTC+12);
      HPRINT(ECHISTC+13);
      HPRINT(ECHISTC+14);
      HPRINT(ECHISTC+16);
      HPRINT(ECHISTC+18);
      HPRINT(ECHISTC+19);
      HPRINT(ECHISTC+20);
      HPRINT(ECHISTC+25);
      HPRINT(ECHISTC+21);
      HPRINT(ECHISTC+22);
      HPRINT(ECHISTC+23);
      HPRINT(ECHISTC+15);
      HPRINT(ECHISTC+24);
      HPRINT(ECHISTC+50);
      HPRINT(ECHISTC+51);
      HPRINT(ECHISTC+52);
      HPRINT(ECHISTC+53);
      HPRINT(ECHISTC+54);
      HPRINT(ECHISTC+55);
      HPRINT(ECHISTC+56);
      HPRINT(ECHISTC+57);
      HPRINT(ECHISTC+58);
      HPRINT(ECHISTC+59);
      HPRINT(ECHISTC+60);
      HPRINT(ECHISTC+61);
      HPRINT(ECHISTC+26);
      HPRINT(ECHISTC+27);
      HPRINT(ECHISTC+28);
      HPRINT(ECHISTC+29);
//gchen
      if(ECCAFFKEY.ecshswit==1){// =====> ecal_shower used.
	HPRINT(ECHISTC+62);
	HPRINT(ECHISTC+63);
	HPRINT(ECHISTC+64);
	HPRINT(ECHISTC+65);
	HPRINT(ECHISTC+66);
	HPRINT(ECHISTC+67);
	HPRINT(ECHISTC+68);
	HPRINT(ECHISTC+69);
	HPRINT(ECHISTC+70);
	HPRINT(ECHISTC+71);
	HPRINT(ECHISTC+72);
	HPRINT(ECHISTC+73);
	HPRINT(ECHISTC+74);
	HPRINT(ECHISTC+75);
	HPRINT(ECHISTC+76);
	HPRINT(ECHISTC+77);
	HPRINT(ECHISTC+78);
	HPRINT(ECHISTC+79);
	HPRINT(ECHISTC+80);
	HPRINT(ECHISTC+81);
	HPRINT(ECHISTC+82);
	HPRINT(ECHISTC+83);
	HPRINT(ECHISTC+84);
	HPRINT(ECHISTC+85);
	HPRINT(ECHISTC+86);
	HPRINT(ECHISTC+87);
	HPRINT(ECHISTC+88);
	HPRINT(ECHISTC+89);
	HPRINT(ECHISTC+90);
	HPRINT(ECHISTC+91);
	HPRINT(ECHISTC+92);
	HPRINT(ECHISTC+93);
	HPRINT(ECHISTC+94);
	HPRINT(ECHISTC+95);
	HPRINT(ECHISTC+96);
	HPRINT(ECHISTC+97);
	HPRINT(ECHISTC+98);
	HPRINT(ECHISTC+99);
	HPRINT(ECHISTC+100);
	HPRINT(ECHISTC+101);
	HPRINT(ECHISTC+102);
	HPRINT(ECHISTC+103);
	HPRINT(ECHISTC+104);
	HPRINT(ECHISTC+105);
	HPRINT(ECHISTC+106);
	HPRINT(ECHISTC+107);
	HPRINT(ECHISTC+108);
	HPRINT(ECHISTC+109);
	HPRINT(ECHISTC+110);
	HPRINT(ECHISTC+111);
	HPRINT(ECHISTC+112);
	HPRINT(ECHISTC+113);
	HPRINT(ECHISTC+114);
	HPRINT(ECHISTC+115);
	HPRINT(ECHISTC+116);
	HPRINT(ECHISTC+117);
	HPRINT(ECHISTC+118);
	HPRINT(ECHISTC+119);
	HPRINT(ECHISTC+120);
	HPRINT(ECHISTC+121);
	HPRINT(ECHISTC+122);
	HPRINT(ECHISTC+123);
	HPRINT(ECHISTC+124);
	HPRINT(ECHISTC+125);
	HPRINT(ECHISTC+126);
	HPRINT(ECHISTC+127);
	HPRINT(ECHISTC+128);
	HPRINT(ECHISTC+129);
	HPRINT(ECHISTC+130);
	HPRINT(ECHISTC+131);
	HPRINT(ECHISTC+132);
	HPRINT(ECHISTC+133);
      }
      ECREUNcalib::mfite();
    }
}
//----------------------------
void EcalJobStat::outpmc(){
  geant rzprmc1[ecalconst::ECSLMX],rzprmc2[ecalconst::ECSLMX];
    if(ECMCFFKEY.mcprtf!=0){ // print MC-hists
      HPRINT(ECHIST+1);
      HPRINT(ECHIST+2);
      HPRINT(ECHIST+3);
      HPRINT(ECHIST+4);
      HPRINT(ECHIST+20);
      HPRINT(ECHIST+21);
      HPRINT(ECHIST+22);
      HPRINT(ECHIST+23);
      HPRINT(ECHIST+24);
      HPRINT(ECHIST+5);
      HPRINT(ECHIST+6);
      if(mccount[1]>0){
        for(int i=0;i<ECSLMX;i++){
          rzprmc1[i]=geant(zprmc1[i]/mccount[1]);
          rzprmc2[i]=geant(zprmc2[i]/mccount[1]);
	}
        HPAK(ECHIST+7,rzprmc1);
        HPAK(ECHIST+8,rzprmc2);
        HPRINT(ECHIST+7);
        HPRINT(ECHIST+8);
      }
      HPRINT(ECHIST+9);
      HPRINT(ECHIST+30);
      HPRINT(ECHIST+31);
      HPRINT(ECHIST+32);
      HPRINT(ECHIST+33);
      HPRINT(ECHIST+34);
      HPRINT(ECHIST+35);
      HPRINT(ECHIST+36);
      HPRINT(ECHIST+37);
      HPRINT(ECHIST+38);
      HPRINT(ECHIST+39);
      HPRINT(ECHIST+40);
      HPRINT(ECHIST+41);
      HPRINT(ECHIST+42);
      HPRINT(ECHIST+43);
      HPRINT(ECHIST+10);
      HPRINT(ECHIST+19);
    }
}
//==========================================================================
//  ECcalib class functions :
//
void ECcalib::build(){// <--- create MC/RealData ecpmcal-objects
  int i,isl,ipm,isc,cnum;
  integer sid,sta[4],stad,endflab;
  geant pmrg,scrg[4],h2lr[4],a2m,a2dr,lfs,lsl,ffr;
  integer slmx,pmmx;
  integer scmx=4;// max.SubCells(pixels) in PM
  slmx=ECSLMX;//max.S-layers
  pmmx=ECPMSMX;//max.PM's in S-layer
  cout<<endl<<"====> ECcalib_build: start with total PMTs="<<ECPMSL<<endl;
//
  char fname[80];
  char name[80];
  char vers1[3]="mc";
  char vers2[3]="rl";
  char in[2]="0";
  char inum[11];
  int ctyp,ntypes,mcvern[10],rlvern[10];
  int mcvn,rlvn,dig;
//
  integer status[ECPMSL][4],statusd[ECPMSL];
  geant pmrgn[ECPMSL],pmscgn[ECPMSL][4],sch2lr[ECPMSL][4],an2dyr[ECPMSL],adc2mev;
  geant lfast[ECPMSL],lslow[ECPMSL],fastf[ECPMSL];
//
  strcpy(inum,"0123456789");
//
// ---> read list of calibration-type-versions list (menu-file) :
//
  integer cfvn;
  cfvn=ECCAFFKEY.cfvers%1000;
  strcpy(name,"ecalcvlist");// basic name for file of cal-files list 
  dig=cfvn/100;
  in[0]=inum[dig]; 
  strcat(name,in);
  dig=(cfvn%100)/10;
  in[0]=inum[dig]; 
  strcat(name,in);
  dig=cfvn%10;
  in[0]=inum[dig]; 
  strcat(name,in);
  strcat(name,".dat");
//
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Open verslist-file: "<<fname<<'\n';
  ifstream vlfile(fname,ios::in); // open needed tdfmap-file for reading
  if(!vlfile){
    cerr <<"<---- ECcalib_build:Error: missing verslist-file "<<fname<<endl;
    exit(1);
  }
  vlfile >> ntypes;// total number of calibr. file types in the list
  for(i=0;i<ntypes;i++){
    vlfile >> mcvern[i];// first number - for mc
    vlfile >> rlvern[i];// second number - for real
  }
  vlfile.close();
  cout<<"   <--Verslist-file is successfully read !"<<endl;
//
//------------------------------
//
//   --->  Read status/rel.gains calib. file :
//
  ctyp=1;//1st type of calibration (really may be single)
  strcpy(name,"ecalrlga");
  mcvn=mcvern[ctyp-1]%1000;
  rlvn=rlvern[ctyp-1]%1000;
  if(AMSJob::gethead()->isMCData()) //      for MC-event
  {
       cout <<"      MC: reading real(not Seed!)RLGA-calib file..."<<endl;
       dig=mcvn/100;
       in[0]=inum[dig];
       strcat(name,in);
       dig=(mcvn%100)/10;
       in[0]=inum[dig];
       strcat(name,in);
       dig=mcvn%10;
       in[0]=inum[dig];
       strcat(name,in);
       strcat(name,vers1);
  }
  else                              //      for Real events
  {
       cout <<"      RD: reading RLGA-calib file..."<<endl;
       dig=rlvn/100;
       in[0]=inum[dig];
       strcat(name,in);
       dig=(rlvn%100)/10;
       in[0]=inum[dig];
       strcat(name,in);
       dig=rlvn%10;
       in[0]=inum[dig];
       strcat(name,in);
       strcat(name,vers2);
  }
//
  strcat(name,".dat");
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Open file : "<<fname<<'\n';
  ifstream rlgfile(fname,ios::in); // open  file for reading
  if(!rlgfile){
    cerr <<"<---- ECcalib_build: Error: missing status/rel.gains file !!? "<<fname<<endl;
    exit(1);
  }
//
// ---> read PM-status:
//
  for(isl=0;isl<slmx;isl++){   
    for(isc=0;isc<4;isc++){   
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        rlgfile >> status[cnum][isc];//pixel's status
      }
    }
    for(ipm=0;ipm<pmmx;ipm++){
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        rlgfile >> statusd[cnum];//dynode's status
    }
  } 
//
// ---> read PM(sum of 4 SubCells) relative(to some Ref.PM) gains
//
  for(isl=0;isl<slmx;isl++){   
    for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      rlgfile >> pmrgn[cnum];
    }
  }
//
// ---> read PM-SubCell relative gains:
//
  for(isl=0;isl<slmx;isl++){   
    for(isc=0;isc<4;isc++){   
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        rlgfile >> pmscgn[cnum][isc];
      }
    }
  }
//
// ---> read PM-SubCell hi2low ratious:
//
  for(isl=0;isl<slmx;isl++){   
    for(isc=0;isc<4;isc++){   
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        rlgfile >> sch2lr[cnum][isc];
      }
    }
  }
//
// ---> read PM anode(sum of 4-SubCells)-to-Dynode ratious:
//
  for(isl=0;isl<slmx;isl++){   
    for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      rlgfile >> an2dyr[cnum];
    }
  }
// ---> read endfile-label:
//
  rlgfile >> endflab;
//
  rlgfile.close();
  if(endflab==12345){
    cout<<"   <--RLGA-calibration file is successfully read !"<<endl;
  }
  else{cout<<"<---- ECcalib::build: ERROR: problems while reading RLGA-calib.file !!?"<<endl;
    exit(1);
  }
//==================================================================
//
//   --->  Read fiber-attenuation calib. file :
//
  ctyp=2;//2nd type of calibration 
  strcpy(name,"ecalfiat");
  mcvn=mcvern[ctyp-1]%1000;
  rlvn=rlvern[ctyp-1]%1000;
  if(AMSJob::gethead()->isMCData()) //      for MC-event
  {
       cout <<"      MC: reading FIAT-calib file..."<<endl;
       dig=mcvn/100;
       in[0]=inum[dig];
       strcat(name,in);
       dig=(mcvn%100)/10;
       in[0]=inum[dig];
       strcat(name,in);
       dig=mcvn%10;
       in[0]=inum[dig];
       strcat(name,in);
       strcat(name,vers1);
  }
  else                              //      for Real events
  {
       cout <<"      RD: reading FIAT-calib file..."<<endl;
       dig=rlvn/100;
       in[0]=inum[dig];
       strcat(name,in);
       dig=(rlvn%100)/10;
       in[0]=inum[dig];
       strcat(name,in);
       dig=rlvn%10;
       in[0]=inum[dig];
       strcat(name,in);
       strcat(name,vers2);
  }
//
  strcat(name,".dat");
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Open file : "<<fname<<'\n';
  ifstream fatfile(fname,ios::in); // open  file for reading
  if(!fatfile){
    cerr <<"<---- ECcalib_build::Error: missing fiber_att.param. file !!? "<<fname<<endl;
    exit(1);
  }
//
// ---> read PM-fibers att.parameters(Latt_fast/Latt_slow/Fast_fraction): 
//
  for(isl=0;isl<slmx;isl++){   
    for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      fatfile >> lfast[cnum];
    }
  }
//
  for(isl=0;isl<slmx;isl++){   
    for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      fatfile >> lslow[cnum];
    }
  }
//
  for(isl=0;isl<slmx;isl++){   
    for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      fatfile >> fastf[cnum];
    }
  }
// ---> read endfile-label:
//
  fatfile >> endflab;
//
  fatfile.close();
  if(endflab==12345){
    cout<<"   <--FIAT-calibration file is successfully read !"<<endl;
  }
  else{cout<<"<---- ECcalib_build: ERROR:problems while reading FIAT-calib.file !!?"<<endl;
    exit(1);
  }
//================================================================== 
//
//   --->  Read abs.norm. calib. file :
//
  ctyp=3;//3rd type of calibration 
  strcpy(name,"ecalanor");
  mcvn=mcvern[ctyp-1]%1000;
  rlvn=rlvern[ctyp-1]%1000;
  if(AMSJob::gethead()->isMCData()) //      for MC-event
  {
       cout <<"      MC: reading ANOR-calib file..."<<endl;
       dig=mcvn/100;
       in[0]=inum[dig];
       strcat(name,in);
       dig=(mcvn%100)/10;
       in[0]=inum[dig];
       strcat(name,in);
       dig=mcvn%10;
       in[0]=inum[dig];
       strcat(name,in);
       strcat(name,vers1);
  }
  else                              //      for Real events
  {
       cout <<"      RD: reading ANOR-calib file..."<<endl;
       dig=rlvn/100;
       in[0]=inum[dig];
       strcat(name,in);
       dig=(rlvn%100)/10;
       in[0]=inum[dig];
       strcat(name,in);
       dig=rlvn%10;
       in[0]=inum[dig];
       strcat(name,in);
       strcat(name,vers2);
  }
//
  strcat(name,".dat");
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Open file : "<<fname<<'\n';
  ifstream anrfile(fname,ios::in); // open  file for reading
  if(!anrfile){
    cerr <<"<---- ECcalib_build:Error: missing ANOR(abs.calib) file !!? "<<fname<<endl;
    exit(1);
  }
//
// ---> read common(hope) adc2mev convertion factor(abs.norm):
//
  anrfile >> adc2mev;
//
// ---> read endfile-label:
//
  anrfile >> endflab;
//
  anrfile.close();
  if(endflab==12345){
    cout<<"   <--ANOR-calibration file is successfully read !"<<endl;
  }
  else{cout<<"<---- ECcalib_build: ERROR: problems while reading ANOR-calib.file !!?"<<endl;
    exit(1);
  }
    
//------------------------------
//
//   
//
  for(isl=0;isl<slmx;isl++){
    for(ipm=0;ipm<pmmx;ipm++){
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(32)
      sid=(ipm+1)+100*(isl+1);
      for(isc=0;isc<4;isc++)sta[isc]=status[cnum][isc];//anode pixels
      stad=statusd[cnum];//dynode
      pmrg=pmrgn[cnum];
      for(isc=0;isc<4;isc++)scrg[isc]=1/(pmrg*pmscgn[cnum][isc]);
//(1/(..) ->  to have multiplication instead of deviding in RECO(for speed); pmrg included
//  in pmscgn because in simu/reco product of pmrg*pmscgn is used really(or pmrg alone)
      for(isc=0;isc<4;isc++)h2lr[isc]=sch2lr[cnum][isc];
      a2dr=an2dyr[cnum];
      lfs=lfast[cnum];
      lsl=lslow[cnum];
      ffr=fastf[cnum];
      a2m=adc2mev;
      ecpmcal[isl][ipm]=ECcalib(sid,sta,stad,pmrg,scrg,h2lr,a2dr,lfs,lsl,ffr,a2m);
    }
  }
  cout<<"<---- ECcalib::build: successfully done !"<<endl<<endl;
}
//---------------
number ECcalib::pmsatf1(int dir,number q){//simulate PM-anode saturation, i.e. Qmeas/Qin
//   return 1;
//(imply satur.origin as voltage drop on last devider resistor,i.e. depends on TOTAL
// PM charge(4xSubc. peak current)); dir=0/1->(Qin->Qmeas)/(Qmeas->Qin)
//                                return Qmeas/Qin(dir=0) or  Qin/Qmeas(dir=1)
  int npnt=11;
  int i;
  geant qin[11]={750.,800.,900.,1000.,1250.,1500.,2000.,3000.,4000.,6000.,10000.};
  geant qme[11]={750.,785.,854., 920.,1071.,1210.,1450.,1835.,2125.,2507., 2747.};
  if(dir==0){//Qin->Qmeas
     if(q<=qin[0])return 1;
     else if(q>=qin[npnt-1])return qme[npnt-1]/q;
     else{
       for(i=1;i<npnt;i++){
         if(q<qin[i]){
           return (qme[i-1]+(qme[i]-qme[i-1])*(q-qin[i-1])/(qin[i]-qin[i-1]))/q;//r=Qme/Qin<1
         }
       }
     }
  }
  else if(dir==1){//Qmeas->Qin
    if(q<=qme[0])return 1;
    else if(q>(0.99*qme[npnt-1])){
//      cerr<<"ECcalib::pmsatf1:Qmeas saturation !"<<endl;
      return -qin[npnt-1]/q;//r=Qin/Qmeas
    }
    else{
      for(i=1;i<npnt;i++){
         if(q<qme[i]){
           return (qin[i-1]+(qin[i]-qin[i-1])*(q-qme[i-1])/(qme[i]-qme[i-1]))/q;//r=Qin/Qmeas>1
         }
      }
    }
  } 
    cerr<<"<---- ECcalib_pmsatf1:Error: wrong dir parameter or logic error"<<dir<<endl;
    abort();
    return(0);
}  
//==========================================================================
//  ECcalibMS class functions :
//
void ECcalibMS::build(){// <--- create ecpmcal-objects used as "MC-Seeds"
  int i,isl,ipm,isc,cnum;
  integer sid,sta[4],stad,endflab;
  geant pmrg,scrg[4],h2lr[4],a2m,a2dr,lfs,lsl,ffr;
  integer slmx,pmmx;
  integer scmx=4;// max.SubCells(pixels) in PM
  slmx=ECSLMX;//max.S-layers
  pmmx=ECPMSMX;//max.PM's in S-layer
  cout<<endl<<"====> ECcalibMS::build: start build with total PMs="<<ECPMSL<<endl;
//
  char fname[80];
  char name[80];
  char vers1[3]="mc";
  char vers2[3]="sd";
  char in[2]="0";
  char inum[11];
  int ctyp,ntypes,mcvern[10],rlvern[10];
  int mcvn,rlvn,dig;
//
  integer status[ECPMSL][4],statusd[ECPMSL];
  geant pmrgn[ECPMSL],pmscgn[ECPMSL][4],sch2lr[ECPMSL][4],an2dyr[ECPMSL],adc2mev;
  geant lfast[ECPMSL],lslow[ECPMSL],fastf[ECPMSL];
//
  strcpy(inum,"0123456789");
//
// ---> read list of calibration-type-versions list (menu-file) :
//
  integer cfvn;
  cfvn=ECCAFFKEY.cfvers%1000;
  strcpy(name,"ecalcvlist");// basic name for file of cal-files list 
  dig=cfvn/100;
  in[0]=inum[dig]; 
  strcat(name,in);
  dig=(cfvn%100)/10;
  in[0]=inum[dig]; 
  strcat(name,in);
  dig=cfvn%10;
  in[0]=inum[dig]; 
  strcat(name,in);
  strcat(name,".dat");
//
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Open verslist-file  "<<fname<<'\n';
  ifstream vlfile(fname,ios::in); // open needed tdfmap-file for reading
  if(!vlfile){
    cerr <<"<---- ECcalibMS_build::Error: missing verslist-file !!? "<<fname<<endl;
    exit(1);
  }
  vlfile >> ntypes;// total number of calibr. file types in the list
  for(i=0;i<ntypes;i++){
    vlfile >> mcvern[i];// first number - for mc
    vlfile >> rlvern[i];// second number - for real
  }
  vlfile.close();
  cout<<"   <--Verslist-file is successfully read !"<<endl;
//
//------------------------------
//
// --->  Read copy of RealData RLGA calib. file(to use as MC-Seed) :
//                   (or MC, if silogic[1]=1)
  ctyp=1;//1st type of calibration 
  strcpy(name,"ecalrlga");
  mcvn=mcvern[ctyp-1]%1000;
  rlvn=rlvern[ctyp-1]%1000;
// 
       cout<<"      MC: reading Real/Seed RLGA(stat/rel.gains)-calibr. file..."<<endl;
       cout<<"      (normally Real(*mc.dat)-file is the CalibProg outp.file when the Seed"<<endl;
       cout<<"      -file(*sd.dat) was chosen to be read at this stage !!!)"<<endl;
       dig=rlvn/100;
       in[0]=inum[dig];
       strcat(name,in);
       dig=(rlvn%100)/10;
       in[0]=inum[dig];
       strcat(name,in);
       dig=rlvn%10;
       in[0]=inum[dig];
       strcat(name,in);
       if(ECMCFFKEY.silogic[1]==0)strcat(name,vers1);//mc
       else strcat(name,vers2);//sd = copy of rl
//
  strcat(name,".dat");
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Open file : "<<fname<<'\n';
  ifstream rlgfile(fname,ios::in); // open  file for reading
  if(!rlgfile){
    cerr <<"<---- ECcalibMS_build:Error: missing Real/Seed RLGA file !!? "<<fname<<endl;
    exit(1);
  }
//
// ---> read PM-status:
//
  for(isl=0;isl<slmx;isl++){   
    for(isc=0;isc<4;isc++){   
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        rlgfile >> status[cnum][isc];//pixel's status
      }
    }
    for(ipm=0;ipm<pmmx;ipm++){
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        rlgfile >> statusd[cnum];//dynode's status
    }
  } 
//
// ---> read PM(sum of 4 SubCells) relative(to some Ref.PM) gains
//
  for(isl=0;isl<slmx;isl++){   
    for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      rlgfile >> pmrgn[cnum];
    }
  }
//
// ---> read PM-SubCell relative gains:
//
  for(isl=0;isl<slmx;isl++){   
    for(isc=0;isc<4;isc++){   
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        rlgfile >> pmscgn[cnum][isc];
      }
    }
  }
//
// ---> read PM-SubCell hi2low ratious:
//
  for(isl=0;isl<slmx;isl++){   
    for(isc=0;isc<4;isc++){   
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        rlgfile >> sch2lr[cnum][isc];
      }
    }
  }
//
// ---> read PM anode(sum of 4-SubCells)-to-Dynode ratious:
//
  for(isl=0;isl<slmx;isl++){   
    for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      rlgfile >> an2dyr[cnum];
    }
  }
// ---> read endfile-label:
//
  rlgfile >> endflab;
//
  rlgfile.close();
  if(endflab==12345){
    cout<<"   <--MC Real/Seed RLGA-calib file is successfully read as MCSeed !"<<endl;
  }
  else{cout<<"<---- ECcalibMS_build:Error reading MC Real/Seed RLGA-calib.file as MCSeed !!?"<<endl;
    exit(1);
  }
//==================================================================
//
//   --->  Read copy of RealData FIAT calib. file(used as MC-Seeds) :
//                      (or MC)
  ctyp=2;//2nd type of calibration 
  strcpy(name,"ecalfiat");
  mcvn=mcvern[ctyp-1]%1000;
  rlvn=rlvern[ctyp-1]%1000;
       cout <<"      MC: reading Real(as Seed)/Seed(true) FIAT(fiber_att)-calibr. file..."<<endl;
       dig=rlvn/100;
       in[0]=inum[dig];
       strcat(name,in);
       dig=(rlvn%100)/10;
       in[0]=inum[dig];
       strcat(name,in);
       dig=rlvn%10;
       in[0]=inum[dig];
       strcat(name,in);
       if(ECMCFFKEY.silogic[1]==0)strcat(name,vers1);//mc
       else strcat(name,vers2);//sd = copy of rl
//
  strcat(name,".dat");
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Open file : "<<fname<<'\n';
  ifstream fatfile(fname,ios::in); // open  file for reading
  if(!fatfile){
    cerr <<"<---- ECcalibMS_build:Error: missing Real/Seed FIATcalib file !!? "<<fname<<endl;
    exit(1);
  }
//
// ---> read PM-fibers att.parameters(Latt_fast/Latt_slow/Fast_fraction): 
//
  for(isl=0;isl<slmx;isl++){   
    for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      fatfile >> lfast[cnum];
    }
  }
//
  for(isl=0;isl<slmx;isl++){   
    for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      fatfile >> lslow[cnum];
    }
  }
//
  for(isl=0;isl<slmx;isl++){   
    for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      fatfile >> fastf[cnum];
    }
  }
// ---> read endfile-label:
//
  fatfile >> endflab;
//
  fatfile.close();
  if(endflab==12345){
    cout<<"   <--Real/Seed FIAT-calibr. file is successfully read as MCSeed !"<<endl;
  }
  else{cout<<"<---- ECcalibMS_build:Error: reading MC Real/Seed FIAT-calib.file as MCSeed !"<<endl;
    exit(1);
  }
//------------------------------
//
//   
//
  for(isl=0;isl<slmx;isl++){
    for(ipm=0;ipm<pmmx;ipm++){
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(32)
      sid=(ipm+1)+100*(isl+1);
      for(isc=0;isc<4;isc++)sta[isc]=status[cnum][isc];//anode pixels
      stad=statusd[cnum];//dynode
      pmrg=pmrgn[cnum];
      for(isc=0;isc<4;isc++)scrg[isc]=1/(pmrg*pmscgn[cnum][isc]);
//(1/(..) ->  to have mult. instead of dev. in RECO(for speed); pmrg included
//  in pmscgn because in simu/reco product of pmrg*pmscgn is used really(or pmrg alone)
      for(isc=0;isc<4;isc++)h2lr[isc]=sch2lr[cnum][isc];
      a2dr=an2dyr[cnum];
      lfs=lfast[cnum];
      lsl=lslow[cnum];
      ffr=fastf[cnum];
      ecpmcal[isl][ipm]=ECcalibMS(sid,sta,stad,pmrg,scrg,h2lr,a2dr,lfs,lsl,ffr);
    }
  }
//
  cout<<"<---- ECcalibMS::build: successfully done !"<<endl<<endl;
}  
//==========================================================================
//  ECALVarp class functions :
//
void ECALVarp::init(geant daqth[20], geant cuts[5]){

    int i;
    for(i=0;i<20;i++)_daqthr[i]=daqth[i];
    for(i=0;i<5;i++)_cuts[i]=cuts[i];
}
//==========================================================================
//
void ECPMPeds::build(){// create ECPeds-objects for each cell
//
  int i,isl,ipm,isc,cnum;
  integer sid,endflab(0);
  char fname[80];
  char name[80];
  geant pedh[4],sigh[4],pedl[4],sigl[4],pedd,sigd;
  uinteger stah[4],stal[4],stad;
  geant pmpedh[ECPMSL][4],pmsigh[ECPMSL][4],pmpedl[ECPMSL][4],pmsigl[ECPMSL][4];
  geant pmpedd[ECPMSL],pmsigd[ECPMSL];
  uinteger pmstah[ECPMSL][4],pmstal[ECPMSL][4],pmstad[ECPMSL];
  integer slmx,pmmx;
  integer scmx=4;// max.SubCells(pixels) in PM
  slmx=ECSLMX;//max.S-layers
  pmmx=ECPMSMX;//max.PM's in S-layer
//
//   --->  Read high/low pedestals file :
//
  strcpy(name,"eclp_df");
  integer cfvn;
  cfvn=ECCAFFKEY.cfvers%1000;
  char in[2]="0";
  char inum[11];
  int mcvn,rlvn,dig;
  strcpy(inum,"0123456789");
  dig=cfvn/100;
  in[0]=inum[dig];
//  strcat(name,in);
  dig=(cfvn%100)/10;
  in[0]=inum[dig];
//  strcat(name,in);
  dig=cfvn%10;
  in[0]=inum[dig];
//  strcat(name,in);
  if(AMSJob::gethead()->isMCData())           // for MC-event
  {
    cout <<"====> ECPMPeds_build: Default Real_MC-peds file is used..."<<endl;
    strcat(name,"_mc");
  }
  else                                       // for Real events
  {
    cout <<"====> ECPMPeds_build: Default Real_RD-peds file is used..."<<endl;
    strcat(name,"_rl");
  }
  strcat(name,".dat");
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Open file : "<<fname<<'\n';
  ifstream icfile(fname,ios::in); // open pedestals-file for reading
  if(!icfile){
    cerr <<"<---- missing default pedestals-file !!? "<<fname<<endl;
    exit(1);
  }
//
// ---> read HighGain peds/sigmas:
//
  for(isl=0;isl<slmx;isl++){   
    for(isc=0;isc<4;isc++){   
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        icfile >> pmpedh[cnum][isc];
      }
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        icfile >> pmsigh[cnum][isc];
      }
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        icfile >> pmstah[cnum][isc];
      }
    }
  } 
//
// ---> read LowGain peds/sigmas:
//
  for(isl=0;isl<slmx;isl++){   
    for(isc=0;isc<4;isc++){   
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        icfile >> pmpedl[cnum][isc];
      }
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        icfile >> pmsigl[cnum][isc];
      }
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        icfile >> pmstal[cnum][isc];
      }
    }
  } 
//
// ---> read Dynode peds/sigmas:
//
  for(isl=0;isl<slmx;isl++){   
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        icfile >> pmpedd[cnum];
      }
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        icfile >> pmsigd[cnum];
      }
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        icfile >> pmstad[cnum];
      }
  } 
//
// ---> read EndFileLabel :
//
  icfile >> endflab;
//
  icfile.close();
//
  if(endflab==12345){
    cout<<"<---- ECPMPeds::build: successfully done !"<<endl;
  }
  else{cout<<"<---- ECPMPeds::build:Error: problem to read peds-file"<<endl;
    exit(1);
  }
//---------------------------------------------
//
  for(isl=0;isl<slmx;isl++){
    for(ipm=0;ipm<pmmx;ipm++){
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      sid=(ipm+1)+100*(isl+1);
      for(isc=0;isc<4;isc++)stah[isc]=pmstah[cnum][isc];
      for(isc=0;isc<4;isc++)stal[isc]=pmstal[cnum][isc];
      for(isc=0;isc<4;isc++)pedh[isc]=pmpedh[cnum][isc];
      for(isc=0;isc<4;isc++)sigh[isc]=pmsigh[cnum][isc];
      for(isc=0;isc<4;isc++)pedl[isc]=pmpedl[cnum][isc];
      for(isc=0;isc<4;isc++)sigl[isc]=pmsigl[cnum][isc];
      pedd=pmpedd[cnum];
      sigd=pmsigd[cnum];
      stad=pmstad[cnum];
      pmpeds[isl][ipm]=ECPMPeds(sid,stah,stal,stad,pedh,sigh,pedl,sigl,pedd,sigd);
    }
  }
}
//==========================================================================
//
void ECPMPeds::mcbuild(){// create default ECPeds-objects(MC) for each cell
//
  int i,isl,ipm,isc,cnum;
  integer sid,endflab(0);
  geant pedh[4],sigh[4],pedl[4],sigl[4],pedd,sigd;
  uinteger stah[4],stal[4],stad;
  geant pmpedh[ECPMSL][4],pmsigh[ECPMSL][4],pmpedl[ECPMSL][4],pmsigl[ECPMSL][4];
  geant pmpedd[ECPMSL],pmsigd[ECPMSL];
  uinteger pmstah[ECPMSL][4],pmstal[ECPMSL][4];
  uinteger pmstad[ECPMSL];
  integer slmx,pmmx;
  integer scmx=4;// max.SubCells(pixels) in PM
  slmx=ECSLMX;//max.S-layers
  pmmx=ECPMSMX;//max.PM's in S-layer
//
// ---> create High(Low,dynode)Gain peds/sigmas/sta:
//
  for(isl=0;isl<slmx;isl++){   
      for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      for(isc=0;isc<4;isc++){   
        pmpedh[cnum][isc]=ECMCFFKEY.pedh*(1+0.01*ECMCFFKEY.pedvh*rnormx());
	if(pmpedh[cnum][isc]<1)pmpedh[cnum][isc]=1;
        pmsigh[cnum][isc]=ECMCFFKEY.pedsh*(1+0.01*ECMCFFKEY.pedsvh*rnormx());
	if(pmsigh[cnum][isc]<0.1)pmsigh[cnum][isc]=0.1;
        pmstah[cnum][isc]=0;
//
        pmpedl[cnum][isc]=ECMCFFKEY.pedl*(1+0.01*ECMCFFKEY.pedvl*rnormx());
	if(pmpedl[cnum][isc]<1)pmpedl[cnum][isc]=1;
        pmsigl[cnum][isc]=ECMCFFKEY.pedsl*(1+0.01*ECMCFFKEY.pedsvl*rnormx());
	if(pmsigl[cnum][isc]<0.1)pmsigl[cnum][isc]=0.1;
        pmstal[cnum][isc]=0;
      }
      pmpedd[cnum]=ECMCFFKEY.pedd*(1+0.01*ECMCFFKEY.peddv*rnormx());
      if(pmpedd[cnum]<1)pmpedd[cnum]=1;
      pmsigd[cnum]=ECMCFFKEY.pedds*(1+0.01*ECMCFFKEY.peddsv*rnormx());
      if(pmsigd[cnum]<0.1)pmsigd[cnum]=0.1;
      pmstad[cnum]=0;
    }
  } 
//---------------
//
  for(isl=0;isl<slmx;isl++){
    for(ipm=0;ipm<pmmx;ipm++){
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      sid=(ipm+1)+100*(isl+1);
      for(isc=0;isc<4;isc++)stah[isc]=pmstah[cnum][isc];
      for(isc=0;isc<4;isc++)stal[isc]=pmstal[cnum][isc];
      for(isc=0;isc<4;isc++)pedh[isc]=pmpedh[cnum][isc];
      for(isc=0;isc<4;isc++)sigh[isc]=pmsigh[cnum][isc];
      for(isc=0;isc<4;isc++)pedl[isc]=pmpedl[cnum][isc];
      for(isc=0;isc<4;isc++)sigl[isc]=pmsigl[cnum][isc];
      pedd=pmpedd[cnum];
      sigd=pmsigd[cnum];
      stad=pmstad[cnum];
      pmpeds[isl][ipm]=ECPMPeds(sid,stah,stal,stad,pedh,sigh,pedl,sigl,pedd,sigd);
    }
  }
  cout<<"<---- ECPMPeds::MCbuild: Default peds/sigmas are created !"<<endl; 
}
//
//==========================================================================
