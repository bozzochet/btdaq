#include "trdsim.h"
#include "event.h"
#include "extC.h"
#include "tofsim02.h"
#include "ntuple.h"
#include "mceventg.h"

using namespace trdsim;
void AMSTRDRawHit::lvl3CompatibilityAddress(int16u ptr,uinteger& udr, uinteger & ufe,uinteger& ute,uinteger& chan){
 chan = ptr & 15;
 ute = (ptr>>4) & 3;
 ufe = (ptr>>6) & 7;
 udr = (ptr>>9) & 7;
}

number AMSTRDRawHit::delay_sf_table[numvawf];
number AMSTRDRawHit::va_waveform[numvawf]={0.,-420.,-740.,-950.,-1080.,-1150.,-1180.,-1200.,-1180.,-1160., -1132., -1085., -1040., -1000., -960., -920., -878., -830., -790., -750., -700.,-650., -615., -583., -552., -510., -480., -440., -420., -400., -380., -360.};


void AMSTRDRawHit::init_delay_table(){

  // Initialize delay table based on VA waveform

  number sumall=0;
  for (int i=0;i<numvawf;i++){
      sumall+=va_waveform[i];
  }
  for (int i=0;i<numvawf;i++){
      number sum=0;
      for (int j=i;j<numvawf;j++){
	  sum+=va_waveform[j];
	}
      if (fabs(sumall)>0.){
	delay_sf_table[i]=sum/sumall;
      }
    }
}

number AMSTRDRawHit::delay_sf(number usec){
  // Returns scale factor for delay in microsecs
  // Very simple table

  if (usec>0. && usec <10.){
    int index=(int)(31.*usec/10.);
    return delay_sf_table[index];
  }
  else{
    return 1.;
  }
}

bool AMSTRDRawHit::_HardWareCompatibilityMode=false;
void AMSTRDRawHit::sitrddigi(){

    AMSTRDMCCluster * ptr=
    (AMSTRDMCCluster*)AMSEvent::gethead()->getheadC("AMSTRDMCCluster",0,1);    

    geant itrack=0;
    geant edep=0;
    while(ptr){

      int i=0;
      number delay=0.;
      itrack=ptr->getitra();
      // Get delay info for the track corresponding to this cluster
      AMSmceventg *pmcg=(AMSmceventg*)AMSEvent::gethead()->getheadC("AMSmceventg",0);
      while(pmcg && i<itrack) {
	// Get mcg for this track (at the beginning of the list)
	if (pmcg){
	  delay = pmcg->getdelay();
	}
        pmcg=pmcg->next();
	i++;
      }
    
     edep+=ptr->getedep()*TRDMCFFKEY.GeV2ADC;
     //Put in scale factor for delayed event
     if (delay>0) edep *= AMSTRDRawHit::delay_sf(delay);

     if(ptr->testlast()){
           AMSTRDIdSoft idsoft(ptr->getid());
           number amp=(edep*idsoft.getgain()+idsoft.getped()+idsoft.getsig()*rnormx());
           if (amp>idsoft.overflow())amp=idsoft.overflow();

           if(amp-idsoft.getped()>TRDMCFFKEY.Thr1R*idsoft.getsig())
        AMSEvent::gethead()->addnext(AMSID("AMSTRDRawHit",idsoft.getcrate()),
        new AMSTRDRawHit(idsoft,(amp-idsoft.getped())*TRDMCFFKEY.f2i));
        edep=0;       
     }

     ptr=ptr->next();
    }
    int cl=AMSEvent::gethead()->getC("AMSTRDRawHit",0)->getnelem();

  if(TRDMCFFKEY.NoiseOn && TOF2RawSide::GlobFasTrigOK())AMSTRDRawHit::sitrdnoise();


}


void AMSTRDRawHit::sitrdnoise(){
//   brute force now
//   if necessary should be changed a-la sitknoise

for ( int i=0;i<TRDDBc::TRDOctagonNo();i++){
      for (int j=0;j<TRDDBc::LayersNo(i);j++){
	  for(int k=0;k<TRDDBc::LaddersNo(i,j);k++){
            for (int l=0;l<TRDDBc::TubesNo(i,j,k);l++){
             float amp=rnormx();
             if(amp>TRDMCFFKEY.Thr1R){
              AMSTRDIdGeom id(j,k,l,i);
              AMSTRDIdSoft idsoft(id);
              AMSTRDRawHit *p =new AMSTRDRawHit(idsoft,amp*idsoft.getsig()*TRDMCFFKEY.f2i);
              AMSTRDRawHit *ph= (AMSTRDRawHit *)AMSEvent::gethead()->getheadC(AMSID("AMSTRDRawHit",idsoft.getcrate()));
              while(ph){
                if(ph<p){
                  AMSEvent::gethead()->addnext(AMSID("AMSTRDRawHit",idsoft.getcrate()),p);
                  break;
                }
                else if(!(p<ph)){
                  delete p;
                  break;
                }
                ph=ph->next();
              }
             }
            }
           }
          }
}

}





void AMSTRDRawHit::builddaq(int n, int16u* p){

}

void AMSTRDRawHit::buildraw(int i, int n, int16u*p){
}


void AMSTRDRawHit::_writeEl(){
  integer flag =    (IOPA.WriteAll%10==1)
                 || ( checkstatus(AMSDBc::USED));
  if(AMSTRDRawHit::Out( flag  )){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
    TRDRawHitNtuple* TrN = AMSJob::gethead()->getntuple()->Get_trdht();
    if (TrN->Ntrdht>=MAXTRDRHT) return;
    TrN->Layer[TrN->Ntrdht]=_id.getlayer();
    TrN->Ladder[TrN->Ntrdht]=_id.getladder();
    TrN->Tube[TrN->Ntrdht]=_id.gettube();
    TrN->Amp[TrN->Ntrdht]=Amp();
    TrN->Ntrdht++;
  }

}

void AMSTRDRawHit::_copyEl(){
}

void AMSTRDRawHit::_printEl(ostream & o){
o<<_id<<" "<<_Amp<<endl;
}



integer AMSTRDRawHit::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
  for(int n=0;n<ntrig;n++){
    if(strcmp("AMSTRDRawHit",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
    }
  }
}
return (WriteAll || status);
}






integer AMSTRDRawHit::lvl3format(int16 * adc, integer nmax){
  //
  // convert my stupid format to lvl3 one for ams02 flight 
  // (pure fantasy, not yet defined
  //  word 1:   tube no in address format
  //  word 2:   tube amp  (-ped)


  if (nmax < 2)return 0;
  adc[0]=_id.gethaddr();
  adc[1]=_Amp;
  return 2;
}



