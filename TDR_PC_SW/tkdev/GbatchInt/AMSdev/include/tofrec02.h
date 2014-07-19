//  $Id: tofrec02.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// June, 23, 1996. ak. add getNumbers function
//
// Oct  04, 1996.  ak _ContPos is moved to AMSLink
//
// Last Edit Oct 04, 1996. ak.
// Last edit 8.1.97 EC (raw cluster format changed)
// Removed gain-5 logic, E.Choumilov 22.08.2005
//
#ifndef __AMSTOF2REC__
#define __AMSTOF2REC__
#include "tofdbc02.h"
#include "typedefs.h"
#include "point.h"
#include "link.h"
#include "event.h"
#include "trrec.h"
//========================================================
class TOF2RawCluster: public AMSlink{
protected:
 static integer trflag; // =1/2/3 -> "z>=1"/"z>1"/"z>2", =0->no trig.
 static uinteger trpatt[TOF2GC::SCLRS]; // triggered bars pattern(z>=1)
 static uinteger trpatt1[TOF2GC::SCLRS]; // triggered bars pattern(z>=2)
 static geant fttime[TOF2GC::SCCRAT];//FTtime
 integer _ntof;    // number of TOF-plane(layer) (1-top,...,4-bot)
 integer _plane;   //  number of sc. bar in given plane(1->...)
 number _z;        // z coord of sc.bar
 number _adca[2]; // Anode ADC for 2 sides (ADC-chan in float)
 number _adcd[2]; // Dynode(pmts-combined, i.e. equilized sum) ADC(s1/2, float adc-chan))
 number _adcdr[2][TOF2GC::PMTSMX];// separate Dynode-pmts ADC(s1/2, float adc-chan)
 number _edepa;    // reco. via Anode channel(no angle correction)
 number _edepd;    // reco. via Dynode channel (..................)
 number _sdtm[2];  // A-noncorrected side times (ns, for TZSL-calibr) 
 number _time;   // A-corrected time=0.5*(t1+t2) (ns);
 number _timeD; // Y-coo(long)/(cm,loc.r.s,A-corrected) calc.from 0.5(t1-t2)
 number _etimeD;// Y-coord.error(cm)

 void _copyEl();
 void _printEl(ostream & stream);
 void _writeEl();
public:
 static integer Out(integer);
 integer getstatus() const{return _status;}
 integer getntof()const {return _ntof;}
 integer getplane()const {return _plane;}
 number gettime()const {return _time;}
 number gettimeD()const {return _timeD;}
 number getetimeD()const {return _etimeD;}
 number getedepa()const {return _edepa;}
 number getedepd()const {return _edepd;}
 number getz()const {return _z;}
 TOF2RawCluster(integer status, integer xy, integer plane, 
   number z, number adca[2], number adcd[2],
   number adcdr[2][TOF2GC::PMTSMX],
   number de, number ded, 
   number sdtm[2], number time, number timed, number etimed):
   AMSlink(status,0), _ntof(xy),_plane(plane),_z(z),
   _edepa(de), _edepd(ded),
   _time(time), _timeD(timed), _etimeD(etimed){
   for(int i=0;i<2;i++){
     _adca[i]=adca[i];
     _adcd[i]=adcd[i];
     for(int ip=0;ip<TOF2GC::PMTSMX;ip++)_adcdr[i][ip]=adcdr[i][ip];
     _sdtm[i]=sdtm[i];
   }
 }
 void getadca(number adc[2]){
   adc[0]=_adca[0];
   adc[1]=_adca[1];
 }
 void getadcd(number adc[2]){
   adc[0]=_adcd[0];
   adc[1]=_adcd[1];
 }
 void getadcdr(int is, number adc[TOF2GC::PMTSMX]){
   for(int ipm=0;ipm<TOF2GC::PMTSMX;ipm++)adc[ipm]=_adcdr[is][ipm];
 }
 void getsdtm(number sdtm[2]){
   sdtm[0]=_sdtm[0];
   sdtm[1]=_sdtm[1];
 }
 void recovers(number x);
//
 TOF2RawCluster *  next(){return (TOF2RawCluster*)_next;}
//
 static void setpatt(uinteger patt[]){
   for(int i=0;i<TOF2GC::SCLRS;i++)trpatt[i]=patt[i];
 }
 static void setpatt1(uinteger patt[]){
   for(int i=0;i<TOF2GC::SCLRS;i++)trpatt1[i]=patt[i];
 }
 static void getpatt(uinteger patt[]){
   for(int i=0;i<TOF2GC::SCLRS;i++)patt[i]=trpatt[i];
 }
 static void getpatt1(uinteger patt[]){
   for(int i=0;i<TOF2GC::SCLRS;i++)patt[i]=trpatt1[i];
 }
 static void settrfl(integer trfl){trflag=trfl;}
 static integer gettrfl(){return trflag;}
 static geant getfttime(int icr){return fttime[icr];} 
 static void build(int &);   // RawEvent->RawCluster
#ifdef __WRITEROOT__
 friend class TofRawClusterR;
#endif
};
//===================================================================
class AMSTOFCluster: public AMSlink{
protected:
 static AMSTOFCluster *_Head[4];
 integer _ntof;  // TOF plane number where cluster was found(1->) 
 integer _plane; // bar number of the "peak" bar in cluster(1->)  
 number _edep;   // clust. Etot/nmemb (MeV) (A/D/h/l-combined)
 number _edepd;  // clust. Etot/nmemb (MeV) (D h/l-combined)
 number _time;   // average cluster time (sec)
 number _etime;  // error on cluster time (sec)
 AMSPoint _Coo;  // cluster center of gravity coordinates (cm)
 AMSPoint _ErrorCoo; // Error to coordinates
 integer _nmemb; // number of TOFRawCluster members(really 1-2, max.3=nonphysical)
 AMSlink * _mptr[3]; // list of pointers to members(raw clust)
 static integer _planes; // real number of planes
 static integer _padspl[TOF2GC::SCLRS]; // real number of paddles/plane
 
 void _copyEl();
 void _printEl(ostream & stream);
 void _writeEl();
#ifdef __PRIVATE__
 static integer goodch[4][14];
 static geant   tcoef[4][14];
#endif

public:

  integer operator < (AMSlink & o)const{
   AMSTOFCluster *p =dynamic_cast<AMSTOFCluster*>(&o);
   if (checkstatus(AMSDBc::USED) && !(o.checkstatus(AMSDBc::USED)))return 1;
   else if(!checkstatus(AMSDBc::USED) && (o.checkstatus(AMSDBc::USED)))return 0;
   else return !p || _time < p->_time;
  }


 static integer Out(integer);
 static void init();
 static integer planes(){return _planes;}
 static integer padspl(integer npl){return _padspl[npl];}
 AMSID crgid(integer i=0);
 integer getntof()const {return _ntof;}
 integer getplane()const {return _plane;}
 number gettime()const {return _time;}
 number getetime()const {return _etime;}
 void setetime(number et){_etime=et;}
 number getedep()const {return _edep;}
 number getedepd()const {return _edepd;}
 integer getnmemb()const {return _nmemb;}
 integer getmemb(AMSlink *ptr[]){
   for(int i=0;i<_nmemb;i++)ptr[i]=_mptr[i];
   return _nmemb;
 }
 void recovers(AMSTrTrack * ptr);
 void recovers2(AMSTrTrack * ptr);
 AMSPoint getcoo()const {return _Coo;}
 AMSPoint getecoo()const {return _ErrorCoo;}
//+
 AMSTOFCluster(): AMSlink() {}
//-
 AMSTOFCluster(integer status, integer ntof, integer plane, number edep, 
    number edepd, AMSPoint coo, AMSPoint ecoo, number time, number etime, integer nm,
                                                                     AMSlink * ptr[]):
 AMSlink(status,0),_ntof(ntof),_plane(plane),_edep(edep),_edepd(edepd),
 _Coo(coo),_ErrorCoo(ecoo),_time(time), _etime(etime), _nmemb(nm){
   for(int i=0;i<nm;i++)_mptr[i]=ptr[i];
 }
//
 AMSTOFCluster(integer status, integer ntof, integer plane):
        AMSlink(status,0),_ntof(ntof),_plane(plane),_edep(0),_edepd(0),
                 _Coo(0,0,0),_ErrorCoo(0,0,0),_time(0), _etime(0), _nmemb(0){
   for(int i=0;i<2;i++)_mptr[i]=0;
 }
//
 AMSTOFCluster *  next(){return (AMSTOFCluster*)_next;}
 ~AMSTOFCluster(){for(int i=0;i<4;i++)_Head[i]=0;};
 static void build(int &);
 static void build2(int &);
 static void build();   // fast algo build
 static void print();
 void  resethash(integer i, AMSlink *head){
   if(i>=0 && i <4)_Head[i]=(AMSTOFCluster*)head;
 }

 static AMSTOFCluster * gethead(integer i=0){
  if(i>=0 && i<4){
    if(!_Head[i])_Head[i]=(AMSTOFCluster*)AMSEvent::gethead()
    ->getheadC("AMSTOFCluster",i,1);
    return _Head[i];
  }
  else {
#ifdef __AMSDEBUG__
    cerr <<"AMSTOFCluster:gethead-S-Wrong Head "<<i;
#endif
    return 0;
  }
 }
#ifdef __WRITEROOT__
 friend class TofClusterR;
#endif
};
//===================================================================
class TOF2User {
 public:
  void static Event();
  void static InitJob();
  void static EndJob();
};
#endif
