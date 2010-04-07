//  $Id: richrec.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
#include <math.h>
#include "commons.h"
#include "ntuple.h"
#include "richrec.h"
#include "richradid.h"
#include "mccluster.h"
#include "lipcomm.h"
#ifdef __WRITEROOT__
#include "root.h" 
#endif


//
// TODO: Check that the reconstructions used the right refractive index
//       Currently it simply calls get_tile_index of richradid

void AMSRichRawEvent::mc_build(){
  // Add noise...
  geant mean_noisy=RICnwindows*RICHDB::total*RICHDB::prob_noisy;
  int dummy1;
  integer n_noisy;
  integer hitn=1;


  POISSN(mean_noisy,n_noisy,dummy1);

  for(int i=0;i<n_noisy;i++){
    // Get a random channel number and make it noisy
    integer channel=integer(RICnwindows*RICHDB::total*RNDM(dummy1));
    AMSRichMCHit::noisyhit(channel);
  }


  // Add dark current
  // This is not used until a parametrization exists 
  //
  //geant mean_noisy=RICnwindows*RICHDB::total*RICHDB::prob_dark;
  //POISSN(mean_noisy,n_noisy,dummy1);
  //for(int i=0;i<n_noisy;i++){
  // Get a random channel number and make it noisy
  //integer channel=integer(RICnwindows*RICHDB::total*RNDM(dummy1));
  //AMSRichMCHit::darkhit(channel);???
  //}




  // Construct event: channel signals

   int nhits=0;
   int nnoisy=0;
   //   int ndark=0;

   for(AMSRichMCHit* hits=(AMSRichMCHit *)AMSEvent::gethead()->getheadC("AMSRichMCHit",0,1);hits;hits=hits->next()){ // loop on signals

      integer channel=hits->getchannel();
      if(channel==-1) continue;  //SKIP charged particles

      if(channel>=RICnwindows*RICHDB::total){
         cerr<< "AMSRichRawEvent::mc_build-ChannelNoError "<<channel<<endl;
          break;
      }
      hits->puthit(hitn); // To construct the pointer list

      if(hits->getid()==Cerenkov_photon) nhits++;
      if(hits->getid()==Noise) nnoisy++;
      //if(hits->getid()==Dark_current) ndark++;   // NOT IMPLEMENTED
      
      
      if(hits->testlast()){ // last signal in the hit so construct it
	// Simulate the gainX5 mode
	geant pedestal=nnoisy>0?AMSRichMCHit::noise(channel,1):AMSRichMCHit::adc_empty(channel,1);
	geant signal=AMSRichMCHit::adc_hit(nhits,channel,1);
	AMSRICHIdSoft calibration(channel);
	geant threshold;
	integer mode=1;

	if(integer(signal+pedestal)>calibration.getboundary()){
	  // Change to gainX1 mode
	  mode=0;
	  pedestal=nnoisy>0?AMSRichMCHit::noise(channel,0):AMSRichMCHit::adc_empty(channel,0);
	  signal*=calibration.getgain(0)/calibration.getgain(1);
	}

	threshold=calibration.getthreshold(mode)*calibration.getsped(mode)+calibration.getped(mode);

	nnoisy=0;
	nhits=0;
	//	ndark=0;

	if(integer(signal+pedestal)>threshold && calibration.getstatus()){
	  AMSEvent::gethead()->addnext(AMSID("AMSRichRawEvent",0),
				       new AMSRichRawEvent(channel,integer(signal+pedestal-calibration.getped(mode)),(mode==0?0:gain_mode)));
	  hitn++;
	  
	//        if(nnoisy>0) pedestal=AMSRichMCHit::noise(channel); 
	//	else if(nhits>0) pedestal=AMSRichMCHit::adc_empty();
	//        signal+=AMSRichMCHit::adc_hit(nhits);
	//        nnoisy=0;
	//        nhits=0;
	//        if(integer(signal)>=integer(RICHDB::c_ped)+integer(RICHDB::ped)){
	//	  AMSEvent::gethead()->addnext(AMSID("AMSRichRawEvent",0),
	//				       new AMSRichRawEvent(channel,integer(signal)));
	//          hitn++;
	//          signal=0;
        } else {
	  for(AMSRichMCHit* clean=(AMSRichMCHit *)AMSEvent::gethead()->getheadC("AMSRichMCHit",0,1);clean;clean=clean->next()){
	    if(clean->gethit()==hitn) clean->puthit(0);
	    if(clean->gethit()==-1) break;
	  }
	}
      }
   }
}



/*
void AMSRichRawEvent::build(){
  // Flag the PMTs which are likely to be crossed by a charged particle using the
  // bit number 'bit_crosse_pmt'

  double PMTSignal[RICmaxpmts];

  for(int i=0;i<RICHDB::total;PMTSignal[i++]=0);
  
  int nhits=0;
  for(AMSRichRawEvent* current=(AMSRichRawEvent *)AMSEvent::gethead()->
	getheadC("AMSRichRawEvent",0);current;current=current->next()){
    current->unsetbit(crossed_pmt_bit);
    int pmt=current->getchannel()/16;
    nhits++;
    if(pmt>=RICHDB::total){
      cerr<< "AMSRichRawEvent::build-ChannelNoError "<<pmt<<endl;
      return;
    }
    PMTSignal[pmt]+=current->getnpe();
  }
  
  double mean=0;
  double mean2=0;
  double rms=0;
  double threshold;
  int total=0;
  
  for(int i=0;i<RICHDB::total;i++)
    if(PMTSignal[i]>0){
      total++;
      mean+=PMTSignal[i];
      mean2+=PMTSignal[i]*PMTSignal[i];
    }
  
  if(total>0){
    mean/=total;
    mean2/=total;
    rms=sqrt(mean2-mean*mean);
    threshold=mean+3*rms;
  }
  
  for(AMSRichRawEvent* current=(AMSRichRawEvent *)AMSEvent::gethead()->
	getheadC("AMSRichRawEvent",0);current;current=current->next()){
    int pmt_number=current->getchannel()/16;
    if(PMTSignal[pmt_number]>threshold)
      current->setbit(crossed_pmt_bit);
  }

}



int AMSRichRawEvent::Npart(){
  int PMTFlagged[RICmaxpmts];
  for(int i=0;i<RICmaxpmts;i++) PMTFlagged[i]=0;
  for(AMSRichRawEvent* current=(AMSRichRawEvent *)AMSEvent::gethead()->
        getheadC("AMSRichRawEvent",0);current;current=current->next()){
    int pmt_number=current->getchannel()/16;
    PMTFlagged[pmt_number]+=current->getbit(crossed_pmt_bit);
  }
  int npart=0;
  for(int i=0;i<RICmaxpmts;i++) if(PMTFlagged[i]) npart++;
  return npart;
}
*/

int AMSRichRawEvent::_npart=0;
int AMSRichRawEvent::Npart(){return _npart;}

void AMSRichRawEvent::build(){
  double signal[RICmaxpmts];
  int hid[RICmaxpmts];
  int test[RICmaxpmts];
  double best_mean=0;
  double best_sigma=1e200;

  //
  // Unflag hits and collect the PMT signal
  //
  
  _npart=0;
  for(int i=0;i<RICmaxpmts;signal[i++]=0);

  for(AMSRichRawEvent* current=(AMSRichRawEvent *)AMSEvent::gethead()->
	getheadC("AMSRichRawEvent",0);current;current=current->next()){
    current->unsetbit(crossed_pmt_bit);
    int pmt=current->getchannel()/16;
    if(pmt>=RICHDB::total){
      cerr<< "AMSRichRawEvent::build-ChannelNoError "<<pmt<<endl;
      return;
    }
    signal[pmt]+=current->getnpe();
  }

  //
  // Count nb. of PMTS and fill arrays
  //
  int npmt=0;
  for(int i=0;i<RICmaxpmts;i++){
    if(signal[i]>0) hid[npmt++]=i;
  }
  
  const int minimum_pmts=4;
  const double flag_threshold=10;
  int bootstraps=100;

  if(npmt>=minimum_pmts){
    int pmts=int(npmt*(1-1.0/minimum_pmts));
    
    for(int i=0;i<bootstraps;i++){
      double mean;
      double mean2;
      Select(pmts,npmt,test);
      mean=0;
      mean2=0;
      for(int j=0;j<npmt;j++){
        if(!test[j]) continue;
        mean+=signal[hid[j]];
        mean2+=signal[hid[j]]*signal[hid[j]];
      }
      
      mean/=pmts;
      mean2/=pmts;
      mean2-=mean*mean;
      mean2=sqrt(mean2);
      
      if(mean2<best_sigma){
        best_mean=mean;
        best_sigma=mean2;

	//	cout<<"CURRENT BEST SIGMA IS "<<best_sigma<<" FOR "<<endl;
	//	for(int j=0;j<npmt;j++){
	//	  if(!test[j]) continue;
	//	  cout<<"    pmt "<<hid[j]<<" "<<signal[hid[j]]<<endl;
	//	}
      }
    }

    //
    // Flag hits and pmts and count number of good/bad PMTs
    //
    

    
    for(AMSRichRawEvent* current=(AMSRichRawEvent *)AMSEvent::gethead()->
	  getheadC("AMSRichRawEvent",0);current;current=current->next()){
      current->unsetbit(crossed_pmt_bit);
      int pmt=current->getchannel()/16;
      
      if(signal[pmt]-best_mean>flag_threshold*best_sigma) current->setbit(crossed_pmt_bit);
    }
    
    _npart=0;
    for(int i=0;i<npmt;i++) if(signal[hid[i]]-best_mean>flag_threshold*best_sigma) _npart++;
    
  }

//   if(_npart>0){
//     cout<<"*************************************"<<endl;
//     cout<<" MEAN PER PMT "<<best_mean<<endl;
//     cout<<" SIGMA        "<<best_sigma<<endl;
//     cout<<" DUMPING FLAGGED PMTS "<<endl;
//     for(int i=0;i<npmt;i++) if(signal[hid[i]]-best_mean>flag_threshold*best_sigma) cout<<" PMT "<<hid[i]<<" SIGNAL "<<signal[hid[i]]<<endl;;
//     cout<<"*************************************"<<endl;

//     if(best_sigma<0.5){
//       cout<<"--------- SIGMA TOO LOW DUMPING HITS -------"<<endl;
//       for(int i=0;i<npmt;i++){
// 	int dumped=0;
// 	for(AMSRichRawEvent* current=(AMSRichRawEvent *)AMSEvent::gethead()->
// 	      getheadC("AMSRichRawEvent",0);current;current=current->next()){
// 	  int pmt=current->getchannel()/16;
// 	  if(pmt!=hid[i]) continue;
// 	  cout<<" PMT "<<hid[i]<<" HIT "<<current->getnpe()<<endl;
// 	  dumped++;
// 	}
// 	if(dumped) cout<<"=============="<<endl;
//       }
//     }

//   }

}


double AMSRichRawEvent::RichRandom(){
  static unsigned long int x=1234567;
  static unsigned long int p1=16807;
  static unsigned long int N=2147483647;
  
  x=(p1*x)%N;
  return ((double)x)/((double)(N+1));
}


void AMSRichRawEvent::Select(int howmany,int size,int lista[]){
  int i,j;
  int true_index;

  for(i=0;i<size;i++) lista[i]=0;

  for(i=0;i<howmany;i++){
    int selected;
    selected=int((size-i)*RichRandom());

    j=0;
    for(true_index=0;true_index<size;true_index++){
      if(!lista[true_index])j++;
      if(j>selected) break;
    }
    lista[true_index]=1;
  }
}


void AMSRichRawEvent::_writeEl(){
  
  AMSRICHIdGeom channel(_channel);
  AMSRICHIdSoft calibration(channel);

#ifdef __WRITEROOT__
    float x = channel.x();
    float y = channel.y();
    float z = channel.z();
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this,x,y,z);
#endif
  RICEventNtuple* cluster=AMSJob::gethead()->getntuple()->Get_richevent();
  
  if(cluster->Nhits>=MAXRICHITS) return;

  cluster->channel[cluster->Nhits]=_channel;
  cluster->adc[cluster->Nhits]=_counts;

   cluster->npe[cluster->Nhits]=getnpe();


  cluster->x[cluster->Nhits]=channel.x();
  cluster->y[cluster->Nhits]=channel.y();
  cluster->status[cluster->Nhits]=_status;
  cluster->Nhits++;
}



void AMSRichRawEvent::reconstruct(AMSPoint origin,AMSPoint origin_ref,
                                  AMSDir direction,AMSDir direction_ref,
				  geant betamin,geant betamax,
				  geant *betas,geant index,
				  int kind_of_tile=agl_kind){

  // Reconstruct the beta values for this hit. Assumes direction as unitary
  static const geant z=RICHDB::RICradpos()-RICHDB::rad_height-RICHDB::foil_height-
                       RICradmirgap-RIClgdmirgap-RICHDB::rich_height;
  AMSRICHIdGeom channel(_channel);

  geant x=channel.x();
  geant y=channel.y();

  betas[0]=0;
  betas[1]=0;
  betas[2]=0;
  _beta_hit[0]=betas[0];
  _beta_hit[1]=betas[1];
  _beta_hit[2]=betas[2];

  //---------------------------------------------------------
  
  // * Direct case: it uses the Newton algorithm to find the zero of the
  // function:
  //
  //  f(u)=R-h*u/sqrt(1-u**2)-H*u*n/sqrt(1-n**2*u**2)
  //
  // Being:
  //   u=sin( Cerenkov angle )
  //   R= Distance in the XY plane from teh emission point to the detection
  //      point
  //   h= Distance from the bottom of the radiator to the emision point in the
  //      Z axis
  //   H= Expansion length 
  //   n= radiator refractive index
  //
  
  
  // Compute the initial guess and auxiliar values
  
  geant R=sqrt((origin[0]-x)*(origin[0]-x)+(origin[1]-y)*(origin[1]-y));
  geant theta=atan2(number(R),fabs(origin[2]-z));
  geant h=origin[2]-RICHDB::RICradpos()+RICHDB::rad_height;
  geant H=RICHDB::rich_height+RICHDB::foil_height+
                       RICradmirgap+RIClgdmirgap
                       -RICHDB::foil_height;   // Correction due to high index
  geant n=index;
  
  geant u=fabs(sin(theta)/n);
  integer time_out=0;
  geant delta=1,f,g;


  if(kind_of_tile==naf_kind)
    H+=RICHDB::foil_height;

  while(fabs(delta)>1e-5 && time_out<5){
    f=R-h*u/sqrt(1-u*u)-
      H*u*n/sqrt(1-n*n*u*u);
    
    g=h*(u*u/((1-u*u)*sqrt(1-u*u))+1/sqrt(1-u*u))+
      H*n*(u*u*n*n/((1-n*n*u*u)*sqrt(1-n*n*u*u))+
	   1/sqrt(1-n*n*u*u));
    
    delta=f/g;
    u+=delta;
    time_out++;
  }

  if(u>=0 && u<=1){ // Theta has been reconstructed 
    geant phi=atan2(geant(y-origin[1]),geant(x-origin[0]));
    betas[0]=direction[0]*cos(phi)*u+
      direction[1]*sin(phi)*u-direction[2]*sqrt(1-u*u);
    betas[0]=1/index/betas[0];
    if(betas[0]<betamin){ // If the beta as direct is below threshold
      betas[0]=-2.;       // it is the primary passing through the LG so
      return;}            // stop the recontruction.
    else if(betas[0]>betamax) betas[0]=-1;
  }else{
    betas[0]=0;    // Wrong theta
  }

  //------------------------------------------------------  

  integer j=1;
  AMSPoint puntos[8];

  integer max=reflexo(origin_ref,puntos);

  for(integer i=0;i<max;i++){
    AMSPoint dir;
    dir=puntos[i]-origin_ref;
    dir=dir/sqrt(dir[0]*dir[0]+dir[1]*dir[1]+dir[2]*dir[2]);

    geant phi=atan2(dir[1],dir[0]);
    theta=atan2(sqrt(dir[0]*dir[0]+dir[1]*dir[1]),dir[2]);
    u=sin(theta)/index;
    dir.setp(u*cos(phi),u*sin(phi),-sqrt(1-u*u));
    betas[j]=1/index/(dir[0]*direction_ref[0]+
				  dir[1]*direction_ref[1]+
				  dir[2]*direction_ref[2]);

    //CJD:  PROVISIONAL CORRECTION ON THE RADIATOR INDEX FOR REFLECTED
    betas[j]*=1.0007624;


    if(betas[j]<betamin) betas[j]=-2.;
    else
      if(betas[j]>betamax) betas[j]=-1.; 
      else 
	j++;
#ifndef __SAFE__
    if(j==3) return;
#endif    
  }

  // Fill the current beta hit 
  _beta_hit[0]=betas[0];
  _beta_hit[1]=betas[1];
  _beta_hit[2]=betas[2];


}


integer AMSRichRawEvent::reflexo(AMSPoint origin,AMSPoint *ref_point){
  geant false_height=RICHDB::bottom_radius*
    RICHDB::rich_height/(RICHDB::bottom_radius-RICHDB::top_radius); 


  double zk=(RICHDB::bottom_radius/false_height)*
    (RICHDB::bottom_radius/false_height);
  double c2=zk/(1+zk);
  double c=sqrt(c2);
  double alp=1/sqrt(1+zk);
  
  AMSRICHIdGeom channel(_channel);
  double xf=channel.x();
  double yf=channel.y();

  double zf=-false_height-RIClgdmirgap;
  double x=origin[0],y=origin[1],
    z=origin[3]-RICHDB::RICradpos()+RICHDB::rad_height-false_height+
                (RICHDB::rich_height+RICHDB::foil_height
                   +RICradmirgap)
                -RICHDB::foil_height; // Correction due to high index


  AMSPoint initial(x,y,z),final(xf,yf,zf);

  double f1=x*xf-y*yf,
    f2=x*zf+z*xf,
    f3=x*yf+y*xf,
    f4=y*zf+z*yf;

  double a4=4*(f3*f3+f1*f1),
    a3=4*c*(f1*f2+f3*f4),
    a2=c2*(f2*f2+f4*f4)+4*(c2-1)*(f1*f1+f3*f3),
    a1=2*c*(c2-1)*(f3*f4+2*f1*f2),
    a0=(c2-1)*((c2-1)*f3*f3+c2*f2*f2);

  if(a4==0.) return 0;
  
  a3/=a4;
  a2/=a4;
  a1/=a4;
  a0/=a4;

  integer mt=0;
  double sols[4];
  
  SOLVE(a3,a2,a1,a0,sols,mt);

  //Count the number of solutions
  mt=0;
  for(integer i=0;i<4;i++) if(sols[i]!=0.) mt++;

  AMSPoint planes[8];
  AMSPoint this_plane;
  integer nsols=0;

  for(integer i=0;i<mt;i++){
    double b=1/(1+zk)-sols[i]*sols[i];
    if(b<0) continue;
    b=sqrt(b);
    for(integer j=0;j<2;j++){
      if(j) b*=-1;
      this_plane.setp(sols[i],b,c);

      if(dist(initial,this_plane)>0 && dist(final,this_plane)>0)
	planes[nsols++]=this_plane;
    }
  }

  integer good=0;
  for(integer i=0;i<nsols;i++){
    ref_point[good]=rpoint(initial,final,planes[i]);
    if((ref_point[good])[2]<initial[2] && (ref_point[good])[2]>-false_height
       && fabs(ref_point[good][0]*ref_point[good][0]+
	       ref_point[good][1]*ref_point[good][1]-zk*
	       ref_point[good][2]*ref_point[good][2])<1e-4){

      (ref_point[good]).setp((ref_point[good])[0],
			     (ref_point[good])[1],
			     (ref_point[good])[2]+false_height-
                              RICHDB::rich_height-RICHDB::foil_height-RICradmirgap-
                              RICHDB::rad_height+RICHDB::RICradpos());

      good++;
    }

  }

  return good;
}




// Default values... currently unused: filled during reconstruction
number AMSRichRing::_index=1.05;
number AMSRichRing::_height=3.;
AMSPoint AMSRichRing::_entrance_p=AMSPoint(0,0,0);
AMSDir   AMSRichRing::_entrance_d=AMSDir(0,0);
AMSPoint AMSRichRing::_emission_p=AMSPoint(0,0,0);
AMSDir   AMSRichRing::_emission_d=AMSDir(0,0);
geant   AMSRichRing::_clarity=0.0113;
geant   *AMSRichRing::_abs_len=0;
geant   *AMSRichRing::_index_tbl=0;

int     AMSRichRing::_kind_of_tile=0;

void AMSRichRing::build(){
  _Start();
  // Build all the tracks 
  
  AMSTrTrack *track;

  _lipdummy=0;   // LIP

  int j=0,k=0;
  for(int id=0;;){
    track=(AMSTrTrack *)AMSEvent::gethead()->getheadC("AMSTrTrack",id++,1);
    if(!track) break;
    for(;track;track=track->next()) {if(build(track))k++;j++;}
  }

}


AMSRichRing* AMSRichRing::build(AMSTrTrack *track,int cleanup){
  
  // All these arrays are for speed up the reconstruction
  // They should be move to a dynamic list (like the containers)
  // using, for example, a structure (~completely public class)

  int ARRAYSIZE=(AMSEvent::gethead()->getC("AMSRichRawEvent",0))->getnelem();
  if(ARRAYSIZE==0) return 0;

  // Fast but not safe

  geant recs[RICmaxpmts*RICnwindows/2][3];
  geant mean[RICmaxpmts*RICnwindows/2][3];
  geant probs[RICmaxpmts*RICnwindows/2][3];
  integer size[RICmaxpmts*RICnwindows/2][3];
  integer mirrored[RICmaxpmts*RICnwindows/2][3];
  
  AMSPoint point;
  number theta,phi,length;
  AMSPoint pnt(0.,0.,RICHDB::RICradpos()-RICHDB::rad_height);
  AMSDir dir(0.,0.,-1.);

  int bit=(AMSEvent::gethead()->getC("AMSRichRing",0))->getnelem();

  if(bit==crossed_pmt_bit){
    //cout<<" AMSRichRing::build-too-many-tracks "<<endl;
    return 0;
  }


  track->interpolate(pnt,dir,point,theta,phi,length);

  if(fabs(point[2]-pnt[2])>0.01) return 0; // Interpolation failed
      
  //============================================================
  // Here we should check if the track goes through the radiator
  //============================================================
  
  RichRadiatorTileManager crossed_tile(track);
  
  
  if(crossed_tile.getkind()==empty_kind) return 0;
  
  
  _index=crossed_tile.getindex();
  _height=crossed_tile.getheight();
  _entrance_p=crossed_tile.getentrancepoint();
  _entrance_d=crossed_tile.getentrancedir();
  _clarity=crossed_tile.getclarity();
  _abs_len=crossed_tile.getabstable();
  _index_tbl=crossed_tile.getindextable();
  _kind_of_tile=crossed_tile.getkind();

  // LIP RECONSTRUCTION


  if((RICRECFFKEY.recon[0]/10)%10){
    if(RICCONTROLFFKEY.tsplit)AMSgObj::BookTimer.start("RERICHLIP");
    int liprecstat=1;
    buildlip(track);
    if(!LIPVAR.liphused) liprecstat=0;
#ifdef __AMSDEBUG__
    cout << " >>>> LIP status " << liprecstat << endl;
#endif
    if(RICCONTROLFFKEY.tsplit)AMSgObj::BookTimer.stop("RERICHLIP");
  }

  //ENDofLIP

  //============================================================
  // PARAMETRISATION OF THE HIT ERROR
  // Obtained using the same conditions as the reconstruction 
  // height parametrisation. This depends in the light guides 
  // dimensions
  //============================================================
  
  
  // Parameters

  
  geant A=(-2.81+13.5*(_index-1.)-18.*
	   (_index-1.)*(_index-1.))*
    _height/(RICHDB::rich_height+RICHDB::foil_height+
			RICradmirgap+RIClgdmirgap)*40./2.;
  
  geant B=(2.90-11.3*(_index-1.)+18.*
	   (_index-1.)*(_index-1.))*
    _height/(RICHDB::rich_height+RICHDB::foil_height+
			RICradmirgap+RIClgdmirgap)*40./2.;




  if(_kind_of_tile==naf_kind) // For NaF they are understimated
    {A*=3.44;B*=3.44;}

  
  
  // Reconstruction threshold: maximum beta admited
  geant betamax=1.+5.e-2*(A+B);


  // Next obtained by Casaus: minimum beta admited to avoid noise caused
  // by the particle going through the PMTs
  // Value corrected by Carlos D.
  geant betamin=(1.+RICthreshold*(_index-1.))/_index;
  
  
  
  //==================================================
  // The reconstruction starts here
  //==================================================
  

  // Reconstructing it as direct


  AMSPoint dirp,refp;
  AMSDir   dird,refd;

  dirp=crossed_tile.getemissionpoint();
  dird=crossed_tile.getemissiondir();
  refp=crossed_tile.getemissionpoint(1);
  refd=crossed_tile.getemissiondir(1);


  _emission_p=dirp;
  _emission_d=dird;

  if(RICCONTROLFFKEY.tsplit)AMSgObj::BookTimer.start("RERICHHITS"); //DEBUG
  integer actual=0,counter=0;
  AMSRichRawEvent *hitp[RICmaxpmts*RICnwindows/2];
  
  for(AMSRichRawEvent* hit=(AMSRichRawEvent *)AMSEvent::gethead()->
	getheadC("AMSRichRawEvent",0);hit;hit=hit->next()){
    
    // Checks bounds
    if(actual>=RICmaxpmts*RICnwindows/2) {
      cout << "AMSRichRing::build : Event too long."<<endl;
      break;
    }
    
    // Reconstruct one hit
    hit->reconstruct(dirp,refp,
		     dird,refd,
		     betamin,betamax,recs[actual],_index,_kind_of_tile);

    hit->unsetbit(bit);
    
    if(recs[actual][0]>0 || recs[actual][1]>0 || recs[actual][2]>0){	
      hitp[actual]=hit;
      actual++;
    }
  }
  // Look for clusters
  if(RICCONTROLFFKEY.tsplit)AMSgObj::BookTimer.stop("RERICHHITS"); //DEBUG
  uinteger current_ring_status=_kind_of_tile==naf_kind?naf_ring:0;


  AMSRichRing *first_done=0;

  do{
    if(RICCONTROLFFKEY.tsplit)AMSgObj::BookTimer.start("RERICHBETA"); //DEBUG
    integer best_cluster[2]={0,0};
    geant best_prob=-1;

    for(integer i=0;i<actual;i++)
      for(integer j=0;j<3;j++)
	if(recs[i][j]>0){
	  mean[i][j]=recs[i][j];
	  size[i][j]=1;mirrored[i][j]=j>0?1:0;
	  probs[i][j]=0;}
    
    
    
    for(integer i=0;i<actual;i++){
      if(recs[i][0]==-2.) continue; // Jump if direct is below threshold
      if(cleanup && current_ring_status&dirty_ring) 
	if(hitp[i]->getbit(crossed_pmt_bit)) continue;

      for(integer k=0;k<3;k++){
	if(recs[i][k]<betamin) continue; 
	for(integer j=0;j<actual;j++){
	  if(recs[j][0]==-2.) continue;
	  if(i==j) continue;
	  if(cleanup && current_ring_status&dirty_ring)
            if(hitp[j]->getbit(crossed_pmt_bit)) continue;

	  integer better=AMSRichRing::closest(recs[i][k],recs[j]);
	  
	  geant prob=(recs[i][k]-
		      recs[j][better])*
	    (recs[i][k]-
	     recs[j][better])/
	    AMSRichRing::Sigma(recs[i][k],A,B)/
	    AMSRichRing::Sigma(recs[i][k],A,B);
	  if(prob<9){ //aprox. (3 sigmas)**2
	    probs[i][k]+=exp(-.5*prob);
	    mean[i][k]+=recs[j][better];
	    if(better>0) mirrored[i][k]++;
	  size[i][k]++;
	  }
	}
	if(best_prob<probs[i][k]){ 
	  best_prob=probs[i][k];	
	  best_cluster[0]=i;
	  best_cluster[1]=k;
	}
	
      }
    }
    uinteger cleaning=current_ring_status;
    current_ring_status&=~dirty_ring;

    
    if(best_prob>0 || LIPVAR.liphused>2){
      // This piece is a bit redundant: computes chi2 and weighted beta
      geant wsum=0,wbeta=0;      
      geant chi2=0.;
      geant beta_track=0;
      integer beta_used=0;
      integer mirrored_used=0;

      if(best_prob>0){
	beta_track=recs[best_cluster[0]][best_cluster[1]];
	
	for(integer i=0;i<actual;i++){
	  hitp[i]->unsetbit(bit);
	  if(recs[i][0]==-2.) continue;
	  if(cleanup && cleaning&dirty_ring)
	    if(hitp[i]->getbit(crossed_pmt_bit)) continue;
	  
	  integer closest=
	    AMSRichRing::closest(beta_track,recs[i]);
	  
	  if(recs[i][closest]<betamin) continue;
	  geant prob=(recs[i][closest]-beta_track)*
	    (recs[i][closest]-beta_track)/
	    AMSRichRing::Sigma(beta_track,A,B)/
	    AMSRichRing::Sigma(beta_track,A,B);
	  // Store resolution per hit
#ifdef __AMSDEBUG__
	  {
	    AMSmceventg *pmcg=(AMSmceventg*)AMSEvent::gethead()->getheadC("AMSmceventg",0);
	    number mass=pmcg->getmass();
	    number mom=pmcg->getmom();
	    number beta=mom/sqrt(mom*mom+mass*mass);
	    HF1(123456+(closest>0),100.*(recs[i][closest]/beta-1),1.);
	  }
#endif
	  if(prob>=9) continue;
	  hitp[i]->setbit(bit);
	  if(cleanup) current_ring_status|=hitp[i]->getbit(crossed_pmt_bit)?dirty_ring:0; 
	  chi2+=prob;
	  wsum+=hitp[i]->getnpe();
	  wbeta+=recs[i][closest]*hitp[i]->getnpe();
	}
	
	beta_used=size[best_cluster[0]][best_cluster[1]];
	mirrored_used=mirrored[best_cluster[0]][best_cluster[1]];
	beta_track=mean[best_cluster[0]][best_cluster[1]]/geant(beta_used);
	
	if(wsum>0) wbeta/=wsum; else wbeta=0.;       
      }

      // Event quality numbers:
      // 0-> Number of used hits
      // 1-> chi2/Ndof
      
      // Fill the container
      if(RICCONTROLFFKEY.tsplit)AMSgObj::BookTimer.stop("RERICHBETA"); //DEBUG

      AMSRichRing* done=(AMSRichRing *)AMSEvent::gethead()->addnext(AMSID("AMSRichRing",0),
								    new AMSRichRing(track,
										    beta_used,
										    mirrored_used,
										    beta_track,
										    chi2/geant(beta_used),
										    wbeta,
										    LIPVAR.liphused,
										    LIPVAR.lipthc,
										    LIPVAR.lipbeta,
										    LIPVAR.lipebeta,
										    LIPVAR.liplikep,
										    LIPVAR.lipchi2,
										    LIPVAR.liprprob,
										    current_ring_status,  //Status word
										    (RICRECFFKEY.recon[1]%10)
										    ));  //LIP
      if(!first_done) first_done=done;
      bit++;  
    } else {
      if(RICCONTROLFFKEY.tsplit)AMSgObj::BookTimer.stop("RERICHBETA"); //DEBUG
      //	// Add empty ring to keep track of no recostructed tracks
      //	AMSEvent::gethead()->addnext(AMSID("AMSRichRing",0),
      //				     new AMSRichRing(track,
      //						     0,
      //						     0.,
      //						     0.,
      //						     0. // Unused by now
      //						     ));
    }
  }while(current_ring_status&dirty_ring);   // Do it again if we want to clean it up once
  return first_done;
}

AMSRichRing* AMSRichRing::rebuild(AMSTrTrack *ptrack){

  AMSRichRing *ring=(AMSRichRing *)AMSEvent::gethead()->getheadC("AMSRichRing",0);
  if(ring && ring->_ptrack->getpattern()>=0)return 0;
  return build(ptrack);


}



void AMSRichRing::_writeEl(){
  
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
  RICRing* cluster=AMSJob::gethead()->getntuple()->Get_ring();
  
  if(cluster->NRings>=MAXRICHRIN) return;

  if(_ptrack->checkstatus(AMSDBc::NOTRACK))cluster->track[cluster->NRings]=-1;
  else if(_ptrack->checkstatus(AMSDBc::TRDTRACK))cluster->track[cluster->NRings]=-1;
  else if(_ptrack->checkstatus(AMSDBc::ECALTRACK))cluster->track[cluster->NRings]=-1;
  else cluster->track[cluster->NRings]=_ptrack->getpos();
  cluster->used[cluster->NRings]=_used;
  cluster->mused[cluster->NRings]=_mused;
  cluster->beta[cluster->NRings]=_beta;
  cluster->errorbeta[cluster->NRings]=_errorbeta;
  cluster->quality[cluster->NRings]=_quality;
  cluster->status[cluster->NRings]=_status;
  // All the stuff related to the new routines
  cluster->npexp[cluster->NRings]=_npexp;
  cluster->collected_npe[cluster->NRings]=_collected_npe;
  cluster->probkl[cluster->NRings]=_probkl;
  //+LIP
  //variables
  cluster->liphused[cluster->NRings]=_liphused;
  cluster->lipthc[cluster->NRings]=_lipthc;
  cluster->lipbeta[cluster->NRings]=_lipbeta;
  cluster->lipebeta[cluster->NRings]=_lipebeta;
  cluster->liplikep[cluster->NRings]=_liplikep;
  cluster->lipchi2[cluster->NRings]=_lipchi2;
  cluster->liprprob[cluster->NRings]=_liprprob;
  //ENDofLIP

  cluster->NRings++;
}
void AMSRichRing::_copyEl(){
#ifdef __WRITEROOT__
 if(PointerNotSet())return;
 RichRingR & ptr = AMSJob::gethead()->getntuple()->Get_evroot02()->RichRing(_vpos);
   if (_ptrack) ptr.fTrTrack= _ptrack->GetClonePointer();
   else ptr.fTrTrack=-1;

   // Add the hit used status
   for(AMSRichRawEvent* hit=(AMSRichRawEvent *)AMSEvent::gethead()->getheadC("AMSRichRawEvent",0);hit;hit=hit->next())
     if((hit->getstatus()>>_vpos)%2){
       ptr.fRichHit.push_back(hit->GetClonePointer());   
     }
   
#endif
}
void AMSRichRing::CalcBetaError(){
  geant A=(-2.81+13.5*(_index-1.)-18.*
	   (_index-1.)*(_index-1.))*
    _height/(RICHDB::rich_height+RICHDB::foil_height+
                                   RICradmirgap+RIClgdmirgap)*40./2.;
      
  geant B=(2.90-11.3*(_index-1.)+18.*
	   (_index-1.)*(_index-1.))*
    _height/(RICHDB::rich_height+RICHDB::foil_height+
                                   RICradmirgap+RIClgdmirgap)*40./2.;


  if(_kind_of_tile==naf_kind){A*=3.44;B*=3.44;}


  _errorbeta=_used>0?
    sqrt(AMSRichRing::Sigma(_beta,A,B)*
	 AMSRichRing::Sigma(_beta,A,B)/geant(_used)+0.016e-2*0.016e-2):1;
}









////////////////////////////////////
/// charge reconstruction
/// CREDITS TO JORGE CASAUS AND ELISA LANCIOTTI and moi

#define PI 3.14159265359
#define SQR(x) ((x)*(x))
#define ESC(x,y) ((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])
#define MOD(x) sqrt(ESC(x,x))


geant AMSRichRing::_Time=0;

void AMSRichRing::ReconRingNpexp(geant window_size,int cleanup){ // Number of sigmas used 
const integer freq=10;
static integer trig=0;
trig=(trig+1)%freq;
           if(trig==0 && _NoMoreTime()){
            throw amsglobalerror(" AMSRichRing::ReconRingNpexp-E-Cpulimit Exceeded ");
           }

  AMSPoint local_pos=_entrance_p;
  AMSDir   local_dir=_entrance_d;

  local_pos[2]=RICHDB::rad_height-_height;
  local_dir[2]*=-1;

  const integer NSTL=15,NSTP=70; // Z=1 optimized
  const geant dphi=2*PI/NSTP;


  static geant dfphi[NSTP],dfphih[NSTP];
  static geant hitd[RICmaxpmts*RICnwindows/2],hitp[RICmaxpmts*RICnwindows/2];
  static AMSRichRawEvent *used_hits[RICmaxpmts*RICnwindows/2];

  static geant unused_hitd[RICmaxpmts*RICnwindows/2];
  static AMSRichRawEvent *unused_hits[RICmaxpmts*RICnwindows/2];


  for(int i=0;i<NSTP;i++)
    dfphi[i]=dfphih[i]=0;

  // For the probkl stuff
  integer nh=0,nu=0;
  geant dmax=0.;
  integer nh_unused=0;

  for(AMSRichRawEvent* hit=(AMSRichRawEvent *)AMSEvent::gethead()->
	getheadC("AMSRichRawEvent",0);hit;hit=hit->next()){
    if(hit->getbit((AMSEvent::gethead()->getC("AMSRichRing",0))->
		   getnelem())){
      used_hits[nh]=hit;
      hitd[nh++]=1.e5;
    }else
      if(!(hit->getbit(crossed_pmt_bit))){
	unused_hits[nh_unused]=hit;
	unused_hitd[nh_unused++]=1.e5;
      }
  }
  

  geant l,dL,phi;
  geant efftr,xb,yb,lentr,lfoil,lguide,geftr,
    reftr,beftr,ggen,rgen,bgen;
  geant nexp,nexpg,nexpr,nexpb,prob,vice,pvice;
  geant dfnrm=0,dfnrmh=0;
  integer tflag;

  AMSPoint r;

  l=_height/local_dir[2];
  dL=l/NSTL;
  integer i,j,k;

  for(nexp=0,nexpg=0,nexpr=0,nexpb=0,j=0;j<NSTL;j++){
    l=(j+.5)*dL;

    r=local_pos+local_dir*l;
    

    for(phi=0,i=0;phi<2*PI;phi+=dphi,i++){

      efftr=trace(r,local_dir,phi,&xb,&yb,&lentr,
		  &lfoil,&lguide,&geftr,&reftr,&beftr,&tflag);
	
      if(geftr){
	float cnt=generated(lentr,lfoil,lguide,
			    &ggen,&rgen,&bgen)*dL/NSTP;


	dfphi[i]+=efftr*cnt;

	nexp+=efftr*cnt;
	nexpg+=geftr*dL/NSTP*ggen;
	nexpr+=reftr*dL/NSTP*rgen;
	nexpb+=beftr*dL/NSTP*bgen;
      }

      for(k=0;k<nh_unused;k++){
	geant d=sqrt(SQR(xb-unused_hits[k]->getpos(0))+
		     SQR(yb-unused_hits[k]->getpos(1)));
	if(d<unused_hitd[k])  unused_hitd[k]=d;
      }


      for(k=0;k<nh;k++){
	geant d=sqrt(SQR(xb-used_hits[k]->getpos(0))+
		     SQR(yb-used_hits[k]->getpos(1)));
	if(d<hitd[k]){
	  hitd[k]=d;
	  hitp[k]=phi;
	}
      }
    }
  }


#ifdef __AMSDEBUG__
  HF1(123000,nexpg,1.);
  HF1(123001,nexpr,1.);
  HF1(123002,nexpb,1.);
  HF1(123003,nexp,1.);
#endif



  for(i=0;i<nh;i++){
    if(hitd[i]<1.){
      nu++;
      j=int(hitp[i]/dphi);
      dfphih[j]+=used_hits[i]->getnpe();
    }
  }

  for(i=0;i<NSTP;i++){
    dfnrm+=dfphi[i];
    dfnrmh+=dfphih[i];
  }
  
  // Compute the spread on azimuthal distance for used hits
  Double_t sum2=0;
  for(k=0;k<nh;k++)
    sum2+=(hitp[k]-hitp[0])*(hitp[k]-hitp[0]);
  if(nh>0) _phi_spread=sum2/nh; else _phi_spread=0;


  // Compute the mean of the distribution of 1/distance**2 for not used hits
  sum2=0;
  for(k=0;k<nh_unused;k++)
    sum2+=1/unused_hitd[k]/unused_hitd[k];
  _unused_dist=sum2;

  // Compute Kullbak distance
  double KullbackD=0;
  if(dfnrmh)
    for(int i=0;i<NSTP;i++){
      if(!dfphih[i]) continue;
      if(!dfphi[i]){
	KullbackD=1e5;
	break;
      }
      KullbackD+=dfphih[i]/dfnrmh*log(dfphih[i]*dfnrm/dfnrmh/dfphi[i])+
	0.5/dfnrmh*log(2*PI*dfphih[i]);  // Correction for low n
      //      KullbackD+=dfphi[i]/dfnrm*log(dfphi[i]*(dfnrmh+1)/dfnrm/(dfphih[i]+1.0/NSTP));
    }else KullbackD=1e5;

  if(KullbackD!=1e5) KullbackD-=0.5/dfnrmh*log(2*PI*dfnrmh); // Correction for low n
  _kdist=KullbackD;


  if(dfnrm>0 && dfnrmh>0){
    dfphi[0]/=dfnrm;
    dfphih[0]/=dfnrmh;
    for(i=1;i<NSTP;i++){
      dfphi[i]=dfphi[i-1]+dfphi[i]/dfnrm;
      dfphih[i]=dfphih[i-1]+dfphih[i]/dfnrmh;
      if(fabs(dfphih[i]-dfphi[i])>dmax) 
      	dmax=fabs(dfphih[i]-dfphi[i]);
    }
  } else dmax=1;


  // Probability for the ring
  if(nu){
    float z=sqrt(geant(nu))*dmax;
    _probkl=PROBKL(z);
  }
  else _probkl=0;
  
  // Charge related variables
  _npexp=nexp;

  _npexpg=nexpg;
  _npexpr=nexpr;
  _npexpb=nexpb;

  _collected_npe=0.;
  
  geant A=(-2.81+13.5*(_index-1.)-18.*
	   (_index-1.)*(_index-1.))*
    _height/(RICHDB::rich_height+RICHDB::foil_height+
                                   RICradmirgap+RIClgdmirgap)*40./2.;
      
  geant B=(2.90-11.3*(_index-1.)+18.*
	   (_index-1.)*(_index-1.))*
    _height/(RICHDB::rich_height+RICHDB::foil_height+
                                   RICradmirgap+RIClgdmirgap)*40./2.;

  if(_kind_of_tile==naf_kind) // For NaF they are understimated
    {A*=3.44;B*=3.44;}

  geant sigma=Sigma(_beta,A,B);

  for(AMSRichRawEvent* hit=(AMSRichRawEvent *)AMSEvent::gethead()->
	getheadC("AMSRichRawEvent",0);hit;hit=hit->next()){  
    
    geant betas[3];
    for(int n=0;n<3;n++) betas[n]=hit->getbeta(n);
    geant value=fabs(_beta-betas[closest(_beta,betas)])/sigma;
    if(value<window_size){
      _collected_npe+=(cleanup && hit->getbit(crossed_pmt_bit))?0:hit->getnpe();
    }
  }

}


geant AMSRichRing::trace(AMSPoint r, AMSDir u, 
			 geant phi, geant *xb, geant *yb, 
			 geant *lentr, geant *lfoil, 
			 geant *lguide, geant *geff, 
			 geant *reff, geant *beff, 
			 integer *tflag,float beta_gen)
{
  static geant bx,by;
  static geant kc,ac;
  geant r0[3],u0[3],r1[3],u1[3],r2[3],u2[3],r3[3],n[3];
  geant cc,sc,cp,sp,cn,sn,f,l,a,b,c,d,maxxy,rbase;
  static int first=1;
  static geant mir_eff,exp_len; 
  int i,ed;

  *xb=0;
  *yb=0;
  *lentr=0;
  *lfoil=0;
  *lguide=0;
  *geff=0;
  *reff=0;
  *beff=0;
  *tflag=-1;


  if(first){
    first=0;
    exp_len=RICHDB::rich_height+RICradmirgap+RIClgdmirgap; /* expansion length */
    kc=(RICHDB::bottom_radius-RICHDB::top_radius)/RICHDB::rich_height;
    ac=RICHDB::rad_height+RICHDB::foil_height+RICradmirgap-RICHDB::top_radius/kc;
    bx=RICHDB::hole_radius[0];
    by=RICHDB::hole_radius[1];
    mir_eff=RICmireff;
  }


  cc=1./_beta/_index;
  sc=sqrt(1-SQR(cc));
  cp=cos(phi);
  sp=sin(phi);
  f=sqrt(SQR(u[0])+SQR(u[1]));

  /* initial values*/
  for(i=0;i<3;i++) r0[i]=r[i];

  if(f>0){
    u0[0]=sc/f*(sp*u[0]*u[2]+cp*u[1])+cc*u[0];
    u0[1]=sc/f*(sp*u[1]*u[2]-cp*u[0])+cc*u[1];
    u0[2]=-f*sc*sp+u[2]*cc;}
  else{
    u0[0]=sc*cp;
    u0[1]=sc*sp;
    u0[2]=cc;}

  if(tile(r0))*geff=1;

  /* propagate to radiator end */
  l=(RICHDB::rad_height-r0[2])/u0[2];


  for(i=0;i<3;i++) r1[i]=r0[i]+l*u0[i];

  // Check if the ray crossed the tiles-wall.

  if (!tile(r0) || tile(r1)!=tile(r0)){
    *tflag=6;
    return 0;
  }
  *lentr=l;

  /* exit radiator volume */
  n[0]=0;
  n[1]=0;
  n[2]=1;
  cn=ESC(u0,n);
  sn=sqrt(1-SQR(cn));
  if (RICHDB::foil_height>0){
  /* radiator -> foil */

    if (_index*sn>RICHDB::foil_index){  // Check total reflection
      *tflag=0;
      return 0;
    }
    f=sqrt(1-SQR(_index/RICHDB::foil_index*sn))-_index/RICHDB::foil_index*cn;
    for(i=0;i<3;i++) u1[i]=_index/RICHDB::foil_index*u0[i]+f*n[i];

  /* propagate to foil end */
    l=RICHDB::foil_height/u1[2];
    for(i=0;i<3;i++) r1[i]=r1[i]+l*u1[i];

    if (sqrt(SQR(r1[0])+SQR(r1[1]))>RICHDB::top_radius){
      *tflag=2;
      return 0;
    }
    *lfoil=l;
  /* foil -> vacuum */
    cn=ESC(u1,n);
    sn=sqrt(1-SQR(cn));
    if (RICHDB::foil_index*sn>1){  // Check total reflexion
      *tflag=0;
      return 0;
    }
    f=sqrt(1-SQR(RICHDB::foil_index*sn))-RICHDB::foil_index*cn;
    for(i=0;i<3;i++) u1[i]=RICHDB::foil_index*u1[i]+f*n[i];}
  else{
  /* radiator -> vacuum */
    if (_index*sn>1){
      *tflag=0;
     return 0;
    }
    f=sqrt(1-SQR(_index*sn))-_index*cn;
    for(i=0;i<3;i++) u1[i]=_index*u0[i]+f*n[i];}

  *reff=1;
  
  
  /* propagate to top of mirror */
  l=RICradmirgap/u1[2];
  for(i=0;i<3;i++) r1[i]+=l*u1[i];
  rbase=sqrt(SQR(r1[0])+SQR(r1[1]));
  if (rbase>RICHDB::top_radius){   
    *tflag=1;
    return 0;
  }
  

  
  /* propagate to base of mirror*/
  l=RICHDB::rich_height/u1[2];
  
  for(i=0;i<3;i++) r2[i]=r1[i]+l*u1[i];

  
  /* hole, direct or reflected */
  rbase=sqrt(SQR(r2[0])+SQR(r2[1]));
  
  
  if (rbase<RICHDB::bottom_radius){   // All right
    // Propagate to end
    l=RIClgdmirgap/u1[2];
    for(i=0;i<3;i++) r2[i]+=l*u1[i];
    *xb=r2[0];
    *yb=r2[1];

    *beff=AMSRICHIdGeom::get_channel_from_top(r2[0],r2[1])<0?0:1;
    *tflag=*beff?3:5;
    return (*beff)*lgeff(r2,u1,lguide); 
  }
  else{   // It intersects the mirror
    a=1-(SQR(kc)+1)*SQR(u1[2]);
    b=2*(r1[0]*u1[0]+r1[1]*u1[1]-SQR(kc)*(r1[2]-ac)*u1[2]);
    c=SQR(r1[0])+SQR(r1[1])-SQR(kc*(r1[2]-ac));
    d=SQR(b)-4*a*c;
    if(d<0){
      printf("AMSRichRing::trace Crossing Point not found\n");
      printf(" kc %f, ac %f\n",kc,ac);
      printf(" a %f, b %f, c %f\n",a,b,c);
      return 0;}
    l=(-b+sqrt(d))/2./a;
    if(l<0){
#ifdef __AMSDEBUG__
      printf("AMSRichRing::trace Crossing Point negative \n");
      printf(" kc %f, ac %f\n",kc,ac);
      printf(" a %f, b %f, c %f\n",a,b,c);
      printf(" l %f\n",l);
#endif
      return 0;}
    
    for(i=0;i<3;i++) r2[i]=r1[i]+l*u1[i];
    
    f=1./sqrt(1+SQR(kc));
    n[0]=-f*r2[0]/sqrt(SQR(r2[0])+SQR(r2[1]));
    n[1]=-f*r2[1]/sqrt(SQR(r2[0])+SQR(r2[1]));
    n[2]=f*kc;
    
    f=2*ESC(u1,n);
    for(i=0;i<3;i++) u2[i]=u1[i]-f*n[i];
    
    l=(exp_len+RICHDB::rad_height+RICHDB::foil_height-r2[2])/u2[2];


    for(i=0;i<3;i++) r3[i]=r2[i]+l*u2[i];
    rbase=sqrt(SQR(r3[0])+SQR(r3[1]));

    if(rbase>RICHDB::bottom_radius){
      *tflag=8;
      return 0;
    }


    *xb=r3[0];
    *yb=r3[1];
    
    //    //    if (fabs(r3[0])<bx && fabs(r3[1])<by){
    //    //      *tflag=1;
    //    //      return 0;
    //    //    }
    

    *beff=mir_eff*(AMSRICHIdGeom::get_channel_from_top(r3[0],r3[1])==0?0:1);
    *tflag=*beff?4:5;
    return *beff*lgeff(r3,u2,lguide);
    
  }
  
  
}



int AMSRichRing::tile(AMSPoint r){ // Check if a track hits the radator support struycture
  integer tile=RichRadiatorTileManager::get_tile_number(r[0],r[1]);
  if(RichRadiatorTileManager::get_tile_kind(tile)==empty_kind) return 0;
  // Next should be guaranteed by RichRadiatorTileManager
  //  if(fabs(RichRadiatorTileManager::get_tile_x(tile)-r[0])>RICHDB::rad_length/2.-RICaethk/2.
  //     ||fabs(RichRadiatorTileManager::get_tile_y(tile)-r[1])>RICHDB::rad_length/2.-RICaethk/2.) return 0;
  return tile;
}



float AMSRichRing::generated(geant length,
			   geant lfoil,
			   geant lguide,
			   geant *fg,
			   geant *fr,
			   geant *fb){

  const int NRAD=400;
  const int NFOIL=10;
  const int NGUIDE=14;
  const float ALPHA=0.0072973530764; 

  static float factor=1.;
  static int veryfirst=1;
  static int first[radiator_kinds];
  static float k,abslref,tl;
  const int ENTRIES=RICmaxentries;
  static float l[ENTRIES][radiator_kinds],
    r[ENTRIES][radiator_kinds],
    a[ENTRIES][radiator_kinds],
    b[ENTRIES][radiator_kinds],
    g[ENTRIES][radiator_kinds],
    t[ENTRIES][radiator_kinds];
  static float effg[NRAD][radiator_kinds],
    ring[NRAD][radiator_kinds];
  static float effr[NRAD][NFOIL][radiator_kinds],
    rinr[NRAD][NFOIL][radiator_kinds];
  static float effb[NRAD][NFOIL][radiator_kinds],
    rinb[NRAD][NFOIL][radiator_kinds];
  static float effd[NRAD][NFOIL][NGUIDE][radiator_kinds],
    rind[NRAD][NFOIL][NGUIDE][radiator_kinds];
  float rmn=0,rmx=2.0*_height;
  float fmn=RICHDB::foil_height,fmx=1.5*RICHDB::foil_height;
  float gmn=RICHDB::lg_height,gmx=1.7*RICHDB::lg_height;
  int i,lr,lf,lg,nf;
  float beta;
  float f=0.;


  if(veryfirst){for(int i=0;i<radiator_kinds;i++) first[i]=1;veryfirst=0;}


  if(_kind_of_tile==naf_kind){
    rmx=6.0*_height;
    fmx=3.0*RICHDB::foil_height;}
  



  if(first[_kind_of_tile-1]){
    first[_kind_of_tile-1]=0;
    k=2*PI*ALPHA;
    tl=4*RICHDB::foil_index/SQR(1+RICHDB::foil_index);
    abslref=(RICHDB::lg_abs[0]+RICHDB::lg_abs[1])/2;
#ifdef __AMSDEBUG__
    printf("\nLight Guide Absorption Parameter\n"
             "--------------------------------\n"
             "       Ref. AbsLength :  %f\n",
             abslref);
#endif
    for(i=0;i<ENTRIES-1;i++){
      float dl=1.e-3*(RICHDB::wave_length[i]-RICHDB::wave_length[i+1]);
      float q=1.e-2*(RICHDB::eff[i]+RICHDB::eff[i+1])/2;
      l[i][_kind_of_tile-1]=1.e-3*(RICHDB::wave_length[i]+RICHDB::wave_length[i+1])/2;
      r[i][_kind_of_tile-1]=(_index_tbl[i]+_index_tbl[i+1])/2;
      a[i][_kind_of_tile-1]=(_abs_len[i]+_abs_len[i+1])/2;
      b[i][_kind_of_tile-1]=(RICHDB::lg_abs[i]+RICHDB::lg_abs[i+1])/2;
      g[i][_kind_of_tile-1]=q*dl/SQR(l[i][_kind_of_tile-1]);
      t[i][_kind_of_tile-1]=4*r[i][_kind_of_tile-1]/SQR(1+r[i][_kind_of_tile-1]);
      if(RICHDB::foil_height>0) t[i][_kind_of_tile-1]*=RICHDB::foil_index*(1+r[i][_kind_of_tile-1])/(r[i][_kind_of_tile-1]+SQR(RICHDB::foil_index));
    }
    nf=RICHDB::foil_height>0?NFOIL:1;
#ifdef __AMSDEBUG__
    printf("\nTabulating Effective no. Photons & Refractive Index\n"
             "---------------------------------------------------\n"
             "      %4d bins in Radiator Thickness\n"
             "      %4d bins in Foil     Thickness\n"
             "      %4d bins in LGuide   Thickness\n",
            NRAD,nf,NGUIDE);
#endif

    for(lr=0;lr<NRAD;lr++){
     float rl=rmn+lr*(rmx-rmn)/NRAD;
     effg[lr][_kind_of_tile-1]=0;
     ring[lr][_kind_of_tile-1]=0;
     for(lf=0;lf<nf;lf++){
      float fl=fmn+lf*(fmx-fmn)/nf;
      effr[lr][lf][_kind_of_tile-1]=0;
      rinr[lr][lf][_kind_of_tile-1]=0;
      effb[lr][lf][_kind_of_tile-1]=0;
      rinb[lr][lf][_kind_of_tile-1]=0;
      for(lg=0;lg<NGUIDE;lg++){
       float gl=gmn+lg*(gmx-gmn)/NGUIDE;
       effd[lr][lf][lg][_kind_of_tile-1]=0;
       rind[lr][lf][lg][_kind_of_tile-1]=0;
       for(i=0;i<ENTRIES-1;i++){
         float cr=1./exp(rl*_clarity/SQR(SQR(l[i][_kind_of_tile-1])));
         float ar=1./exp(rl/a[i][_kind_of_tile-1]);
         float af=1./exp(fl/b[i][_kind_of_tile-1]);
         float al=1./exp(gl*(1./b[i][_kind_of_tile-1]-1./abslref));
         effd[lr][lf][lg][_kind_of_tile-1]+=g[i][_kind_of_tile-1]*t[i][_kind_of_tile-1]*cr*ar*af*al;
         rind[lr][lf][lg][_kind_of_tile-1]+=r[i][_kind_of_tile-1]*g[i][_kind_of_tile-1]*t[i][_kind_of_tile-1]*cr*ar*af*al;
         if(!lg){
          if(!lf){
           effg[lr][_kind_of_tile-1]+=g[i][_kind_of_tile-1];
           ring[lr][_kind_of_tile-1]+=r[i][_kind_of_tile-1]*g[i][_kind_of_tile-1];}
          effr[lr][lf][_kind_of_tile-1]+=g[i][_kind_of_tile-1]*t[i][_kind_of_tile-1]*cr*ar*af;
          rinr[lr][lf][_kind_of_tile-1]+=r[i][_kind_of_tile-1]*g[i][_kind_of_tile-1]*t[i][_kind_of_tile-1]*cr*ar*af;
          effb[lr][lf][_kind_of_tile-1]+=g[i][_kind_of_tile-1]*t[i][_kind_of_tile-1]*cr*ar*af*tl;
          rinb[lr][lf][_kind_of_tile-1]+=r[i][_kind_of_tile-1]*g[i][_kind_of_tile-1]*t[i][_kind_of_tile-1]*cr*ar*af*tl;}
       }
       rind[lr][lf][lg][_kind_of_tile-1]/=effd[lr][lf][lg][_kind_of_tile-1];
      }
      rinr[lr][lf][_kind_of_tile-1]/=effr[lr][lf][_kind_of_tile-1];
      rinb[lr][lf][_kind_of_tile-1]/=effb[lr][lf][_kind_of_tile-1];
     }
     ring[lr][_kind_of_tile-1]/=effg[lr][_kind_of_tile-1];
    }
#ifdef __AMSDEBUG__
    printf("....End of Tables!\n\n");
#endif
  }

  lr=int((floor)(NRAD*(length-rmn)/(rmx-rmn)+.5));

  if(lr>NRAD-1){
    static char guard=0;
    if(!guard){
      printf("AMSRichRing::generated WARNING: length too big %f\nMore warning messages suppressed\n",length);
      guard=1;
    }
    lr=NRAD-1;}
  else if(lr<0)lr=0;

  lf=RICHDB::foil_height>0?int((floor)(NFOIL*(lfoil-fmn)/(fmx-fmn)+.5)):0;

  if(lf>NFOIL-1){
    static char guard=0;
    if(!guard){
      printf("AMSRichRing::generated WARNING: lfoil  too big %f\nMore warning messages suppressed\n",lfoil);
      guard=1;
    }
    lf=NFOIL-1;}
  else if(lf<0)lf=0;
  lg=int((floor)(NGUIDE*(lguide-gmn)/(gmx-gmn)+.5));


  if(lg>NGUIDE-1){
    static char guard=0;
    if(!guard){
      printf("AMSRichRing::generated WARNING: lguide too big %f\nMore warning messages suppressed\n",lguide);
      guard=1;
    }
    lg=NGUIDE-1;}
  else if(lg<0)lg=0;

  f=1.e4*k*(1.-1./SQR(_beta*rind[lr][lf][lg][_kind_of_tile-1]))*factor*effd[lr][lf][lg][_kind_of_tile-1];
  *fg=1.e4*k*(1.-1./SQR(_beta*ring[lr][_kind_of_tile-1]))*effg[lr][_kind_of_tile-1];
  *fr=1.e4*k*(1.-1./SQR(_beta*rinr[lr][lf][_kind_of_tile-1]))*effr[lr][lf][_kind_of_tile-1];
  *fb=1.e4*k*(1.-1./SQR(_beta*rinb[lr][lf][_kind_of_tile-1]))*effb[lr][lf][_kind_of_tile-1];
  return f;

}


geant AMSRichRing::lgeff(AMSPoint r, 
			 float u[3],
			 geant *lguide)
{
  float v[3],w[3];
  float f=0;
  int wnd,iw;
  int i,j,k;

  static float LG_Tran,Eff_Area;
  static float bwd=0.04;
  static int first=1;

#ifdef __AMSDEBUG__
  if(first){
    // Check the get_from_top stuff

    cout <<"DUMPING OF MAP"<<endl<<"***************"<<endl;

    AMSRICHIdGeom basura(4805);
    for(geant y=basura.y()-3.4/2-2.;y<=basura.y()+3.4/2+2;y+=0.1){
      for(geant x=basura.x()-3.4/2-2;x<=basura.x()+3.4/2+2;x+=0.1){
	integer wnd=AMSRICHIdGeom::get_channel_from_top(x,y);
	char c=32;

	if(wnd>=0) c='A'+wnd; else c=' ';
	cout <<c;

      }
      cout <<endl;
    }
  }

#endif




  if(first){
    first=0;
    LG_Tran=4*RICHDB::foil_index/SQR(1+RICHDB::foil_index); /*=0.96*/          
    Eff_Area=SQR(RICHDB::lg_length/pitch);
#ifdef __AMSDEBUG__
    printf("\nLight Guide Parameters\n"
             "---------------------------\n"
             "LG_Tran   (Transmittance): %f\n"
             "Eff_Area  (LG-size/Pitch): %f\n",
             LG_Tran,Eff_Area);



    for(int i=0;i<RIC_NWND;i++)
      for(int j=0;j<RIC_NPHI;j++)
	for(int k=0;k<RIC_NTH;k++){

	  cout<<i<<" "<<j<<" "<<k<<" "<<RICHDB::lg_dist_tbl[i][j][k]<<" "<<
	      RICHDB::lg_eff_tbl[i][j][k]<<endl;

	}
#endif	  


  }

  wnd=AMSRICHIdGeom::get_channel_from_top(r[0],r[1]);


  if(wnd==-1){
    *lguide=0;
    return 0;
  }


  refract(1.,RICHDB::foil_index,u,v);

  if(locsmpl(wnd,&iw,v,w)){
   float x=acos(-w[2]);
   if(x<RIC_NTH*bwd){
    int phi,tl,th;
    float y=atan2(-w[1],-w[0]);
    phi=int((floor)(RIC_NPHI*(y+PI)/(2*PI)));
    if(phi==12) phi=0; /*EL*/
    tl=int((floor)((x-bwd/2.)/bwd));
    if(tl<0)tl=0;
    th=tl<RIC_NTH-1?tl+1:tl;
    f=RICHDB::lg_eff_tbl[iw][phi][tl]+(x-(tl+.5)*bwd)*
      (RICHDB::lg_eff_tbl[iw][phi][th]-
       RICHDB::lg_eff_tbl[iw][phi][tl])/bwd;
    *lguide=RICHDB::lg_height*(
     RICHDB::lg_dist_tbl[iw][phi][tl]+(x-(tl+.5)*bwd)*
     (RICHDB::lg_dist_tbl[iw][phi][th]-RICHDB::lg_dist_tbl[iw][phi][tl])/bwd);
   }
  }else *lguide=0;
#ifdef __AMSDEBUG__

  if(locsmpl(wnd,&iw,v,w)){
   HF1(1231001,f,1.);
HF1(1231002,*lguide,1.);
  }
  *lguide=RICHDB::lg_height;
  return 1;

#endif


  return LG_Tran*f;
}


void AMSRichRing::refract(geant r1,
			  geant r2, 
			  geant *u,
			  geant *v)
{
  int i;
  float f,cn,s2n;
  static float n[3] = { 0., 0., 1. };
  float rr=r1/r2;
  cn=ESC(u,n);

  s2n=1-SQR(cn);
  f=sqrt(1.-SQR(rr)*s2n)-rr*cn;
  for(i=0;i<3;i++)v[i]=rr*u[i]+f*n[i];
  // v=rr*u+f*n;
}

int AMSRichRing::locsmpl(int id,
			 int *iw, 
			 geant *u, 
			 geant *v)
{
  static int a[16][2][2] = {
   -1, 0, 0,-1,   0,-1,-1, 0,   0,-1, 1, 0,   0,-1, 1, 0,
   -1, 0, 0,-1,  -1, 0, 0,-1,   0,-1, 1, 0,   1, 0, 0,-1,
   -1, 0, 0, 1,   0, 1,-1, 0,   1, 0, 0, 1,   1, 0, 0, 1,
    0, 1,-1, 0,   0, 1,-1, 0,   0, 1, 1, 0,   1, 0, 0, 1};
  static int wnd[16] = { 
    2, 1, 1, 2,   1, 0, 0, 1,   1, 0, 0, 1,   2, 1, 1, 2};
  int i,j;
  int ok=0;
  if(id>=0 && id<16){
    *iw=wnd[id];
    for(i=0;i<2;i++)
     for(j=0,v[i]=0;j<2;j++) v[i]+=a[id][i][j]*u[j];
    v[2]=-u[2];
    ok=1;}
  return ok;
}



# undef ESC
# undef SQR
# undef PI

//+LIP
///////////////////////////////////////////////////////////////////////////
///////////////////// LIP RECONSTRUCTION     //////////////////////////////
///////////////////////////////////////////////////////////////////////////

integer AMSRichRing::_lipdummy=0;

void AMSRichRing::buildlip(AMSTrTrack *trk){

#define PI 3.14159265359
#define SQR(x) ((x)*(x))

  LIPVAR.liphused=0;
  LIPVAR.lipbeta=0.;
  LIPVAR.lipthc =0.;
  LIPVAR.liplikep =99999.;
  LIPVAR.lipchi2  =99999.;
  LIPVAR.liprprob =99999.;

  int lipdummy=_lipdummy;  // fill geometry and hit control variable

  // ------------------------------------------------------------------------
  // geometry : radiator parameters (may change between tracks)
  // ------------------------------------------------------------------------

  LIPRAD.hrad_c      = _height;            // radiator height
  LIPRAD.refindex_c  = _index;             // current radiator ref. index
  LIPRAD.clarity_c   = _clarity;           //  "        "      clarity (0 for NAF)
  LIPRAD.radtype_c   = _kind_of_tile;      // 0(empty),1(agl),2(naf)
  LIPRAD.nabslen_c   = RICmaxentries;      //nr of. elements of pabslen
  for(int i=0; i<RICmaxentries ; i++){
    LIPRAD.labsrad_c[i]   = *(_abs_len+i);
  }

  if (!lipdummy){
    // ------------------------------------------------------------------------
    // geometry : fixed parameters
    // ------------------------------------------------------------------------

    // radiator :
    LIPRAD.rrad_c      = RICHDB::rad_radius; // radiator radius
    LIPRAD.ltile_c     = RICHDB::rad_length; // tile size

    // foil

    LIPRAD.hpgl_c      = RICHDB::foil_height;
    LIPRAD.pglix_c     = RICHDB::foil_index;

    // dimensions:

    LIPGEO.ztoprad_ams = RICHDB::RICradpos();   //Z coord at the radiator top in the AMS frame
    LIPGEO.zpmtdet_c   = RICHDB::rad_height+RICHDB::foil_height+RICradmirgap+RICHDB::rich_height+RIClgdmirgap;
    LIPGEO.hmir_c      = RICHDB::rich_height;
    LIPGEO.rtmir_c     = RICHDB::top_radius;
    LIPGEO.rbmir_c     = RICHDB::bottom_radius;
    LIPGEO.emcxlim_c   = RICHDB::hole_radius[0];
    LIPGEO.emcylim_c   = RICHDB::hole_radius[1];
    LIPGEO.ztmirgap_c  = RICradmirgap;
    LIPGEO.zbmirgap_c  = RIClgdmirgap;
    LIPGEO.reflec_c    = RICmireff;

    //light guides

    LIPGEO.lg_length_c = RICHDB::lg_length;
    LIPGEO.lg_height_c = RICHDB::lg_height;

    //pmts

    LIPGEO.pmt_suptk_c = RICpmtsupport;
    LIPGEO.pmt_shdtk_c = RICpmtshield;
    LIPGEO.pmt_sidel_c = PMT_electronics;

    // ------------------------------------------------------------------------
    // hit parameters
    // ------------------------------------------------------------------------

    LIPDAT.nbhitsmax_c=RICmaxpmts*RICnwindows/2;

    int actual=0;
    LIPDAT.nbhits_c=0;

    for(AMSRichRawEvent* hit=(AMSRichRawEvent *)AMSEvent::gethead()->
	  getheadC("AMSRichRawEvent",0);hit;hit=hit->next()){

      if(actual>=LIPDAT.nbhitsmax_c) {
	cout << "AMSRichRing::build : Event too long."<<endl;
	break;
      }

      LIPDAT.nbhits_c++;

      LIPDAT.hitsadc_c[actual]=hit->getcounts();
      LIPDAT.hitsnpe_c[actual]=hit->getnpe();

      AMSRICHIdGeom  hitch(hit->getchannel());

      LIPDAT.hitscoo_c[actual][0]=hitch.x();
      LIPDAT.hitscoo_c[actual][1]=hitch.y();
      LIPDAT.hitscoo_c[actual][2]=RICHDB::RICradpos()-hitch.z(); // Z : AMS frame -> RICH frame

      int hgain=hit->getbit(gain_mode_bit);

      LIPDAT.hitshid_c[actual]=10*(16*hitch.getpmt()+hitch.getpixel())+hgain;

      actual++;
    }
    _lipdummy=1;
  }

  // ------------------------------------------------------------------------
  // track parameters
  // ------------------------------------------------------------------------

  // particle ip in radiator
  LIPTRK.pimp_c[0]=_entrance_p[0];
  LIPTRK.pimp_c[1]=_entrance_p[1];           // y.ip(trk) RICH frame
  LIPTRK.pimp_c[2]=RICHDB::RICradpos()-_entrance_p[2];  // z.ip(trk) RICH frame

  // particle direction
  LIPTRK.pphi_c = atan2(_entrance_d[1],_entrance_d[0]);
  if ( LIPTRK.pphi_c<0 ) LIPTRK.pphi_c=LIPTRK.pphi_c+2*PI;
  LIPTRK.pthe_c = acos(-_entrance_d[2]);

  // --- extrapolated point at PMT matrix

  LIPTRK.pcoopmt_c[0]=_entrance_p[0]+tan(LIPTRK.pthe_c)*cos(LIPTRK.pphi_c)*(LIPGEO.zpmtdet_c-LIPTRK.pimp_c[2]);
  LIPTRK.pcoopmt_c[1]=_entrance_p[1]+tan(LIPTRK.pthe_c)*sin(LIPTRK.pphi_c)*(LIPGEO.zpmtdet_c-LIPTRK.pimp_c[2]);
  LIPTRK.pcoopmt_c[2]=LIPGEO.zpmtdet_c;

  // get simulation information for particle

  // NOTE: HERE WE SHOUL CHECK IF THIS IS SIMULATION 

  AMSmceventg *pmcg=(AMSmceventg*)AMSEvent::gethead()->getheadC("AMSmceventg",0);
  number pmass=pmcg->getmass();
  LIPTRK.pmom_c   = pmcg->getmom();
  LIPTRK.pchg_c   = pmcg->getcharge();
  LIPTRK.pbeta_c  = LIPTRK.pmom_c/sqrt(SQR(LIPTRK.pmom_c)+SQR(pmass));
  LIPTRK.cerang_c = acos(1./(_index*LIPTRK.pbeta_c));

  int ievnumb=AMSEvent::gethead()->getEvent();

  //  cout << "pcoo " << LIPTRK.pcoopmt_c[0] <<" " <<LIPTRK.pcoopmt_c[1] <<" " <<LIPTRK.pcoopmt_c[2] <<    endl;
  // cout << "pmom   "<< LIPTRK.pmom_c << endl;
  //cout << "pchg   "<< LIPTRK.pchg_c << endl;
  //cout << "pbeta  "<< LIPTRK.pbeta_c << endl;
  //cout << "cerang "<< LIPTRK.cerang_c << endl;
  //cout << "ppos   " << _entrance_p[0] << " " << _entrance_p[1] << " " << _entrance_p[2] << endl\;
  //cout << "pdir   " << _entrance_d[0] << " " << _entrance_d[1] << " " << _entrance_d[2] << endl\;

  RICHRECLIP(ievnumb);

  //output variables

#ifdef __AMSDEBUG__
  cout << "lip_hused   "  << LIPVAR.liphused << endl;
  cout << "lip_beta    "  << LIPVAR.lipbeta << endl;
  cout << "lip_thc     "  << LIPVAR.lipthc << endl;
  cout << "lip_likep   "  << LIPVAR.liplikep << endl;
  cout << "lip_chi2rec "  << LIPVAR.lipchi2 << endl;
#endif
}
//ENDofLIP
