//  $Id: antirec02.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
//
// July 18 1997 E.Choumilov RawEvent added + RawCluster/Cluster modified
// 10.11.2005 E.Choumilov, complete revision
//
#ifndef __AMSANTI2REC__
#define __AMSANTI2REC__
#include "typedefs.h"
#include "point.h"
#include "link.h"
#include "event.h"
#include "antidbc02.h"
//
//---------------------------------------
class Anti2RawEvent: public AMSlink{
private:
  static integer nscoinc;//# of logic sectors in coinc.with FT
  static uinteger trpatt; //bits 1-8 => log.sect# in coinc.with FT
  int16u _idsoft; // BBS (Bar/Side)
  int16u _status; // status (0/1/... -> alive/dead/...)
  geant _temp;//temperature
  geant _adca;// anode pulse-charge hit(ADC-counts, ped-subtracted if not PedCalibJob)
  integer _nftdc;//number of FastTrig(FT) hits, =1 in MC(filled at validation stage !!)
  integer _ftdc[TOF2GC::SCTHMX1]; // FT-hits(tdc-chan),1/4-plane specific, but stored for each sect
  integer _ntdct; // number of TimeHist(LT-chan) hits 
  integer _tdct[ANTI2C::ANTHMX];// TimeHist hits
public:
  Anti2RawEvent(int16u idsoft, int16u status, geant temp,  geant adca,
                  integer nftdc, integer ftdc[], 
                  integer ntdct, integer tdct[]):_idsoft(idsoft),_status(status)
  {
    int i;
    _temp=temp;
    _adca=adca;
    _ntdct=ntdct;
    _nftdc=nftdc;
    for(i=0;i<_ntdct;i++)_tdct[i]=tdct[i];
    for(i=0;i<_nftdc;i++)_ftdc[i]=ftdc[i];
  };
  ~Anti2RawEvent(){};
  Anti2RawEvent * next(){return (Anti2RawEvent*)_next;}
//
  integer operator < (AMSlink & o)const{
                return _idsoft<((Anti2RawEvent*)(&o))->_idsoft;}
//
  int16u getid() const {return _idsoft;}
  int16u getstat() const {return _status;}
  void updstat(int16u sta){_status=sta;}
  int16u getnzchn(){if(_adca>0)return(1);
                          else return(0);}
//
  geant getadca(){
    return _adca;
  }
  void putadca(geant arr){
    _adca=arr;
  }
//
  integer gettdct(integer arr[]){
    for(int i=0;i<_ntdct;i++)arr[i]=_tdct[i];
    return _ntdct;
  }
  void puttdct(integer nelem, integer arr[]){
    _ntdct=nelem;
    for(int i=0;i<_ntdct;i++)_tdct[i]=arr[i];
  }
  
  integer getftdc(integer arr[]){
    for(int i=0;i<_nftdc;i++)arr[i]=_ftdc[i];
    return _nftdc;
  }
  void putftdc(integer nelem, integer arr[]){
    _nftdc=nelem;
    for(int i=0;i<_nftdc;i++)_ftdc[i]=arr[i];
  }
//
 geant gettemp(){return _temp;}
 void settemp(geant tmp){_temp=tmp;}
//
  static void setpatt(uinteger patt){
    trpatt=patt;
  }
  static void setncoinc(integer nc){nscoinc=nc;}
  static uinteger getpatt(){
    return trpatt;
  }
  static integer getncoinc(){return nscoinc;}
  static void mc_build(int &stat);
  static void validate(int &stat);
//
// interface with DAQ :
static integer calcdaqlength(int16u blid);
static void builddaq(int16u blid, integer &len, int16u *p);
static void buildraw(int16u blid, integer &len, int16u *p);
//
protected:
void _printEl(ostream &stream){
  int i;
  stream <<"Anti2RawEvent: id="<<dec<<_idsoft<<endl;
  stream <<dec<<_adca<<endl;
  stream <<"ntdct="<<dec<<_ntdct<<endl;
  for(i=0;i<_ntdct;i++)stream <<dec<<_tdct[i]<<endl;
  stream <<"_nftdc="<<dec<<_nftdc<<endl;
  for(i=0;i<_nftdc;i++)stream <<dec<<_ftdc[i]<<endl;
  stream <<dec<<endl<<endl;
}
void _writeEl(){};
void _copyEl(){};
//
};
//===================================================================
class AMSAntiCluster: public AMSlink{
protected:
 integer _sector;  // Sector number 1 - MAXANTI
 integer _ntimes;  //number of time-hits
 integer _npairs;  //number of true(paired) time-hits
 number  _times[ANTI2C::ANTHMX*2];//time-hits(ns)
 number  _etimes[ANTI2C::ANTHMX*2];// approx.time errors(ns)
 number _edep;    // SectorEdep (MeV)
 AMSPoint _coo;   // R, phi , Z
 AMSPoint _ecoo;  // Error to _coo 
 void _copyEl();
 void _printEl(ostream & stream);
 void _writeEl();
public:

 static integer Out(integer);
 AMSID crgid(int kk=0){
   return AMSID("ANTS",_sector);
 }
//
 AMSAntiCluster(integer status, integer sector, integer ntimes,
                integer npairs, number times[ANTI2C::ANTHMX*2], 
                number etimes[ANTI2C::ANTHMX*2],number edep, 
		AMSPoint coo, AMSPoint ecoo):
                            AMSlink(status,0), _sector(sector),
		               _ntimes(ntimes),_npairs(npairs),
                             _edep(edep),_coo(coo),_ecoo(ecoo){
    for(int i=0;i<_ntimes;i++){
      _times[i]=times[i];
      _etimes[i]=etimes[i];
    }
 }
//
 AMSAntiCluster(integer status, integer sector):
                AMSlink(status,0), _sector(sector),_ntimes(0),_npairs(0),
		_edep(0),_coo(0,0,0),_ecoo(0,0,0){}
//
 AMSAntiCluster *  next(){return (AMSAntiCluster*)_next;}
//
 integer getsector()const {return _sector;}
 number getedep()const {return _edep;}
//
 integer gettimes(number arr[]){
   for(int i=0;i<_ntimes;i++)arr[i]=_times[i];
   return _ntimes;
 }
 integer getntimes(){return _ntimes;}
 number gettime(int i){return _times[i];}
 number getetime(int i){return _etimes[i];}
 integer getnpairs(){return _npairs;}
//
 static void build2(int &stat); 
 static void print();
#ifdef __WRITEROOT__
 friend class AntiClusterR;
#endif
};
#endif
