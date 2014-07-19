//  $Id: mceventg.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// Author V. Choutko 24-may-1996
//#undef __ASTRO__ 

#include "mceventg.h"
#include <math.h>
#include "amsdbc.h"
#include "event.h"
#include "commons.h"
#include "ntuple.h"
#include "io.h"
#include "extC.h"
#include "ecaldbc.h"
#include "tofdbc02.h"
#include "astro.h" //ISN 
#ifdef __G4AMS__
#include "CLHEP/Random/Random.h"
#include "g4util.h"
#include <iostream.h>
#endif
orbit AMSmceventg::Orbit;
integer AMSmceventg::_hid=20001;
AMSPoint AMSmceventg::_coorange[2];
AMSPoint AMSmceventg::_dirrange[2];
number   AMSmceventg::_momrange[2];
integer  AMSmceventg::_fixedpoint;
integer  AMSmceventg::_fixedplane;
integer  AMSmceventg::_fixeddir;
integer  AMSmceventg::_fixedmom;
number   AMSmceventg::_albedorate;
number   AMSmceventg::_albedocz; 
number   AMSmceventg::_planesw[6];
int   AMSmceventg::_particle[30];
int   AMSmceventg::_nucleons[30];
geant*   AMSmceventg::_spectra[30];
number   AMSmceventg::_flux[30];


AMSmceventg::AMSmceventg(const AMSIO & io){
_nskip=io._nskip;
_seed[0]=io._seed[0];
_seed[1]=io._seed[1];
init(io._ipart);
_mom=io._mom;
int i;
for(i=0;i<3;i++){
 _coo[i]=io._coo[i];
 _dir[i]=io._dir[i];
}
InitSeed();
if(!CCFFKEY.oldformat)Orbit.UpdateAxis(io.getveltheta(),
   io.getvelphi(),io.getstheta(),io.getsphi());
}


AMSmceventg::AMSmceventg(integer ipart, geant mom, const AMSPoint & coo,
const AMSDir & dir, integer nskip):_nskip(nskip),_mom(mom),_coo(coo),_dir(dir){
init(ipart);
}

void AMSmceventg::gener(){
  char cdir[256];
  char *R="R";
  HCDIR(cdir,R);
  char hpawc[256]="//PAWC/GEN";
  char *BLANK=" ";
  HCDIR (hpawc,BLANK);
  if(CCFFKEY.low==-2){
//
//  wrong method do not use  (VC)
//
    geant mom,themu,phimu,chmu,xmu,ymu,zmu;
    while(1){
      CMGENE(mom,themu,phimu,chmu,xmu,ymu,zmu);
      if(TFMCFFKEY.fast==0)break;
      if(fastcheck(xmu,ymu,zmu,themu,phimu))break;
    }
    _mom=mom;
    _coo[0]=xmu;
    _coo[1]=ymu;
    _coo[2]=zmu;
    _dir[0]=sin(themu)*cos(phimu);
    _dir[1]=sin(themu)*sin(phimu);
    _dir[2]=-cos(themu);
    if(chmu < 0)_ipart=6;
    else _ipart=5;
    init(_ipart);
  }
  else {
    number _gamma=0;
    static integer ini=0;
    integer curp=0;
    number phi;
    number theta;
    geant d(-1);
    if(_fixedmom)_mom=_momrange[0];
    else if(CCFFKEY.low==2){
     geant mom,gamma,chmu;
      CMGENE2(mom,gamma,chmu);
      _mom=mom;
      _gamma=gamma; 
      if(chmu < 0)_ipart=6;
      else _ipart=5;
//      cout <<"  mom "<<_mom<<" "<<_gamma<<" "<<_ipart<<endl;
      init(_ipart);
  }
    else if(CCFFKEY.low==4){
      _mom=_momrange[0]+(_momrange[1]-_momrange[0])*RNDM(d);
      if(_momrange[1]>1000000){
        cerr<<"AMSmceventg::gener-F-MomentumRangeIsLimitiedTo 1000 TeV "<<_momrange[1]<<endl;
        exit(1);
      }
    }
    else if(CCFFKEY.low==5 ){
      if(_momrange[0]<=0 ){
        cerr<<"AMSmceventg::gener-F-LogInfinity "<<endl;
        exit(1);
      }              
      number lrange=log(fabs(_momrange[1]))-log(fabs(_momrange[0]));
      _mom=_momrange[0]*exp(lrange*RNDM(d));
    }
    
    else if(CCFFKEY.low==6){
     //  get particle id
     int xp=HRNDM1(_hid);
     GCKINE.ikine=_particle[xp];
     if(GCKINE.ikine<0){
      cerr<<"  AMSmceventg::gener-F-WrongParticle "<<GCKINE.ikine<<endl;
      abort();
     }
     GCKINE.ipart=GCKINE.ikine;
     init(GCKINE.ikine);      
     _mom=exp(HRNDM1(_hid+xp+1))/1000.;
     number etot=_mom*_nucleons[xp]+_mass;
     _mom=sqrt(etot*etot-_mass*_mass);
//     if(_ipart>50)cout <<"  got "<<_ipart<<" "<<_charge<<" "<<_mom<<" "<<_nucleons[xp]<<" "<<xp<<endl;     
    }
     
    else{
#ifdef __G4AMS__
      if(MISCFFKEY.G4On){
        _mom=AMSRandGeneral::hrndm1(_hid)/1000.;
        // cout <<" mom    .... "<<_mom<<endl; 
      }
      else
#endif
        _mom=HRNDM1(_hid)/1000.;  // momentum in GeV
//cout<<"mom="<<_mom<<" hid="<<_hid<<endl;
    }
again:
    geant th,ph; // photon incidence angle (normal to gamma source generation plane)
    if(_fixeddir){ // when GammaSource>0 -> fixeddir=1 (this is done elsewhere)
      if(!(GMFFKEY.GammaSource==0)){  
    number ra,dec;  // AMS zenithal pointing direction
    number rai,deci; //ISS zenithal pointing direction (AMS not tilted)
        AMSEvent::gethead()->GetISSCoo(rai,deci); //non-tilted case      
        AMSEvent::gethead()->GetAMSCoo(ra,dec);
        skyposition isspos(rai,deci); //non-tilted case
        skyposition amspos(ra,dec); // amspos is filled up      
        
        if(GMFFKEY.GammaSource==1){ // user-defined source 
          skyposition sourcepos(GMFFKEY.SourceCoo[0],GMFFKEY.SourceCoo[1]);
          //isspos.CalcDir(sourcepos,th,ph); //non-tilted case
          amspos.CalcDir(sourcepos,th,ph);
        }
        else { // tabulated source
          geant rs,ds; // to store output from lookupsourcecoo
          lookupsourcecoo(GMFFKEY.GammaSource,rs,ds);
          skyposition sourcepos(rs,ds);
          //isspos.CalcDir(sourcepos,th,ph); //non-tilted case
          amspos.CalcDir(sourcepos,th,ph);
        }
        
        if(fabs(th)<GMFFKEY.SourceVisib){
          _dir=AMSDir(-cos(ph)*sin(th),-sin(ph)*sin(th),-cos(th)); // invert direction TO source to direction FROM source.
        } 
        else _dir=AMSDir(cos(ph)*sin(th),sin(ph)*sin(th),cos(th));
      }// ISN
      else _dir=AMSDir(_dirrange[0]);

      if(_fixedpoint){
        _coo=_coorange[0];
      }
      else {
        number x0=_coorange[0][0]>-AMSDBc::ams_size[0]/2?_coorange[0][0]:-AMSDBc::ams_size[0]/2;
        number y0=_coorange[0][1]>-AMSDBc::ams_size[1]/2?_coorange[0][1]:-AMSDBc::ams_size[1]/2;
        number z0=_coorange[0][2]>-AMSDBc::ams_size[2]/2?_coorange[0][2]:-AMSDBc::ams_size[2]/2;
        number x1=_coorange[1][0]<AMSDBc::ams_size[0]/2?_coorange[1][0]:AMSDBc::ams_size[0]/2;
        number y1=_coorange[1][1]<AMSDBc::ams_size[1]/2?_coorange[1][1]:AMSDBc::ams_size[1]/2;
        number z1=_coorange[1][2]<AMSDBc::ams_size[2]/2?_coorange[1][2]:AMSDBc::ams_size[2]/2;
        number lx=x1-x0;
        number ly=y1-y0;
        number lz=z1-z0;      
        //number lx=_coorange[1][0]-_coorange[0][0];
        //number ly=_coorange[1][1]-_coorange[0][1];
        //number lz=_coorange[1][2]-_coorange[0][2];
        //number x=_coorange[0][0];
        //number y=_coorange[0][1];
        //number z=_coorange[0][2];
        geant d(-1);
        if(!(GMFFKEY.GammaSource==0)){
          number coox,cooy,cooz;
          //x0 = 0; // the following values are used to create the particle 
          //y0 = 0; // in the middle of the generating plane
          //z1 = 195; // to check orbit generation
          //lx = 0;
          //ly = 0;
          //lz = 0; 
         
          // generating plane has the orientation given by dir
          // transforming to AMS coordinates (coox,cooy,cooz) from generation plane local coordinates (x0+lx*RNDM(d),y0+ly*RNDM(d),0). z1=MCGEN[1][2] is the distance to the plane from AMS coordinate origin.
          //CAREFUL: MCGEN[0][2] is not used for gamma sources         
         // MCGEN[1][2]=195 for std generation
//            coox=(z+lz)*cos(ph)*sin(th)+(x+lx*RNDM(d))*cos(th)*cos(ph)-(y+ly*RNDM(d))*sin(ph);
//            cooy=(z+lz)*sin(ph)*sin(th)+(x+lx*RNDM(d))*cos(th)*sin(ph)+(y+ly*RNDM(d))*cos(ph);
//            cooz=(z+lz)*cos(th)-(x+lx*RNDM(d))*sin(th);
          coox=z1*cos(ph)*sin(th)+(x0+lx*RNDM(d))*cos(th)*cos(ph)-(y0+ly*RNDM(d))*sin(ph);
          cooy=z1*sin(ph)*sin(th)+(x0+lx*RNDM(d))*cos(th)*sin(ph)+(y0+ly*RNDM(d))*cos(ph);
          cooz=z1*cos(th)-(x0+lx*RNDM(d))*sin(th);
          
          _coo=AMSPoint(coox,cooy,cooz);
      
      // these transformations are obtained by adding:
      // a translation in R=z1 cm 
      // a rotation: theta around y; -phi around z
      // the orientation was chosen so that cooy is coplanar with y

      
          if(GMFFKEY.GammaBg==1){ //to generate a background photon around the source
            number thprime,phprime; //in radians
            geant d(-1);
            phprime=2*AMSDBc::pi*RNDM(d);
            
            //this is to generate randomly between BgAngle and 0.
            thprime=1-pow(sin(GMFFKEY.BgAngle),2)*(double)RNDM(d);
            thprime=sqrt(thprime);
            thprime=acos(thprime);
            
        
            // direction vector respect to inclined plane
            number xprime=cos(phprime)*sin(thprime);
            number yprime=sin(phprime)*sin(thprime);
            number zprime=cos(thprime);
            
            // direction vector respect to AMS ref. system
            number xbg=(xprime*cos(th)+zprime*sin(th))*cos(ph)-yprime*sin(ph);
            number ybg=(xprime*cos(th)+zprime*sin(th))*sin(ph)+yprime*cos(ph);
            number zbg=-xprime*sin(th)+zprime*cos(th);
            
            xbg=xbg/sqrt(pow(xbg,2)+pow(ybg,2)+pow(zbg,2)); // to make unitary vector
            ybg=ybg/sqrt(pow(xbg,2)+pow(ybg,2)+pow(zbg,2));
            zbg=zbg/sqrt(pow(xbg,2)+pow(ybg,2)+pow(zbg,2));
            
            // now we rewrite the direction vector _dir 
            // from: "perpendicular to generating plane"(source direction) to "random direction around source"
            if(acos(zbg)<GMFFKEY.SourceVisib){
              _dir=AMSDir(-xbg,-ybg,-zbg);
            }
            else _dir=AMSDir(xbg,ybg,zbg);
          }
        } // ISN
        else if(_fixedplane==1){
//          cout << "fixed plane" << endl;
          _coo=AMSPoint(x0+lx*RNDM(d),y0+ly*RNDM(d),z0+lz);
        }
        else{
          _coo=AMSPoint(x0+lx*RNDM(d),y0+ly*RNDM(d),z0+lz*RNDM(d));
        }
      }
    }//--->endof fixdir>0
    else {   // <--- random dir
      geant d(-1);
      phi=2*AMSDBc::pi*RNDM(d);
      theta=sqrt((double)RNDM(d));
      theta=acos(theta);
      if(_fixedpoint){
        _dir=AMSDir(cos(phi)*sin(theta),sin(phi)*sin(theta),-cos(theta));
        _coo=_coorange[0];
        char hp[9]="//PAWC";
        HCDIR(hp," ");
        HCDIR (cdir, " ");
        return;
      }
     
      if(_fixedplane){
        curp=_fixedplane;
      }
      else {
        geant r=RNDM(d)-1.e-5;
       int i;
       for ( i=0;i<6;i++){
         if(r<_planesw[i]){
           curp=i+1;
           break;
         }
       }
      }
      number xa=_coorange[0][0]>-AMSDBc::ams_size[0]/2?_coorange[0][0]:-AMSDBc::ams_size[0]/2;
      number ya=_coorange[0][1]>-AMSDBc::ams_size[1]/2?_coorange[0][1]:-AMSDBc::ams_size[1]/2;
      number za=_coorange[0][2]>-AMSDBc::ams_size[2]/2?_coorange[0][2]:-AMSDBc::ams_size[2]/2;
      number xb=_coorange[1][0]<AMSDBc::ams_size[0]/2?_coorange[1][0]:AMSDBc::ams_size[0]/2;
      number yb=_coorange[1][1]<AMSDBc::ams_size[1]/2?_coorange[1][1]:AMSDBc::ams_size[1]/2;
      number zb=_coorange[1][2]<AMSDBc::ams_size[2]/2?_coorange[1][2]:AMSDBc::ams_size[2]/2;
      number lx=xb-xa;
      number ly=yb-ya;
      number lz=zb-za;
      geant xin,yin;
     //
     
      switch(curp){
      case 1:
        //
        // <-- try(if asked) to increas "from top" gener. effic.(Not for accept. calc.!!!)
       if(TFMCFFKEY.fast>0){
         while(1){
           xin=xa+lx*RNDM(d);
           yin=ya+ly*RNDM(d);
           phi=2*AMSDBc::pi*RNDM(d);
           theta=sqrt((double)RNDM(d));
           theta=acos(theta);
           if(fastcheck(xin,yin,zb,theta,phi))break;
         }
         _coo=AMSPoint(xin,yin,zb);
       }
       else{
         _coo=AMSPoint(xa+lx*RNDM(d),ya+ly*RNDM(d),zb);
//cout <<" coo "<<xa<<" "<<ya<<" "<<lx<<" "<<ly<<" "<<_coo<<endl;
       }
       //
       _dir=AMSDir(cos(phi)*sin(theta),sin(phi)*sin(theta),-cos(theta));
       break;
      case 2:  
        _dir=AMSDir(cos(phi)*sin(theta),sin(phi)*sin(theta),cos(theta));
        _coo=AMSPoint(xa+lx*RNDM(d),ya+ly*RNDM(d),za);
        break;
      case 3:  
        _dir=AMSDir(-cos(theta),cos(phi)*sin(theta),sin(phi)*sin(theta));
        _coo=AMSPoint(xb,ya+ly*RNDM(d),za+lz*RNDM(d));
        break;
      case 4:  
        _dir=AMSDir(cos(theta),cos(phi)*sin(theta),sin(phi)*sin(theta));
        _coo=AMSPoint(xa,ya+ly*RNDM(d),za+lz*RNDM(d));
       break;
      case 5:  
        _dir=AMSDir(cos(phi)*sin(theta),-cos(theta),sin(phi)*sin(theta));
        _coo=AMSPoint(xa+lx*RNDM(d),yb,za+lz*RNDM(d));
        break;
      case 6:  
        _dir=AMSDir(cos(phi)*sin(theta),cos(theta),sin(phi)*sin(theta));
        _coo=AMSPoint(xa+lx*RNDM(d),ya,za+lz*RNDM(d));
        break;
      default:  
        cerr <<" AMSmceventg-F-plane problem "<<curp<<endl;
        abort();
      }//<--- end of switch
      //if(_fixedplane == 0)_coo=_coo/2;
//
// add cmuouns (low==2)
//
    if(CCFFKEY.low==2){
     if(_dir[2]>0)goto again;
     geant dummy;
     number u=RNDM(dummy);
     if(u>=pow(fabs(_dir[2]),_gamma))goto again;
//      cout <<"accepted  mom "<<_mom<<" "<<_gamma<<" "<<_ipart<<endl;
    }
    }//--->endof "random dir"
  }//--->endof "low!=2"
  char hp[9]="//PAWC";
  HCDIR(hp," ");
  HCDIR (cdir, " ");
  
}


void AMSmceventg::setspectra(integer begindate, integer begintime, 
                             integer enddate, integer endtime, 
                              integer ipart,  integer low){

  Orbit.Begin.tm_year  =  begindate%10000-1900;
  Orbit.Begin.tm_mon = (begindate/10000)%100-1;
  Orbit.Begin.tm_mday   = (begindate/1000000)%100;
  Orbit.Begin.tm_hour  = (begintime/10000)%100;
  Orbit.Begin.tm_min= (begintime/100)%100;
  Orbit.Begin.tm_sec=(begintime)%100;
  //  Orbit.Begin.tm_isdst =  Orbit.Begin.tm_mon>=3 &&  Orbit.Begin.tm_mon<=8;
  Orbit.Begin.tm_isdst =  0;

  Orbit.End.tm_year  =  enddate%10000-1900;
  Orbit.End.tm_mon = (enddate/10000)%100-1;
  Orbit.End.tm_mday   = (enddate/1000000)%100;
  Orbit.End.tm_hour  = (endtime/10000)%100;
  Orbit.End.tm_min=(endtime/100)%100;
  Orbit.End.tm_sec=(endtime)%100;
  //  Orbit.End.tm_isdst = Orbit.End.tm_mon>=3 &&  Orbit.End.tm_mon<=8;
  Orbit.End.tm_isdst = 0;
  Orbit.FlightTime=difftime(mktime(&Orbit.End),mktime(&Orbit.Begin));
  if(Orbit.FlightTime < 0){
    cerr <<"AMSmceventg::setspectra-F-FlighTime < 0 "<<Orbit.FlightTime<<endl;
    exit(1);
  }  
  Orbit.ThetaI=CCFFKEY.theta/AMSDBc::raddeg;
  Orbit.PhiI=fmod(CCFFKEY.phi/AMSDBc::raddeg+AMSDBc::twopi,AMSDBc::twopi);
  const number MIR=51.65;
  Orbit.AlphaTanThetaMax=tan(MIR/AMSDBc::raddeg);
  Orbit.UpdateOrbit(Orbit.ThetaI,Orbit.PhiI,CCFFKEY.sdir);
  Orbit.AlphaSpeed=AMSDBc::twopi/90.8/60.;
  Orbit.EarthSpeed=AMSDBc::twopi/24/3600;
  Orbit.PolePhi=CCFFKEY.polephi/AMSDBc::raddeg;
  Orbit.EarthR=6371.2e5;                      
  Orbit.AlphaAltitude=Orbit.EarthR+380.e5;  
  Orbit.DipoleR=534.259e5;                       // Dipole Shift Distance  (cm)
  Orbit.DipoleTheta=21.687/AMSDBc::raddeg;       //              Latitude  (rad)
  Orbit.DipolePhi=144.280/AMSDBc::raddeg;        //              Longitude (rad)
  Orbit.PoleTheta=-79.397/AMSDBc::raddeg;        // Dipole Direction Lat   (rad)
  Orbit.PolePhiStatic=108.392/AMSDBc::raddeg;    //                  Lon   (rad)
  Orbit.Nskip=0;
  Orbit.Nskip2=0;
  Orbit.Ntot=AMSIO::getntot();

  if(AMSJob::gethead()->isSimulation()){
  if(ipart==0){
    if(IOPA.mode!=1){
      cerr << "AMSmceventg::setspectra-F-PARTNotDefined"<<endl;
      exit(1);
    }
    return;
  }
    char cdir[256];
    HCDIR(cdir,"R");
    HMDIR("//PAWC/GEN","S");
    if(low ==2)ipart=5;
    char chp[22]="";
    integer itrtyp;
    geant mass;
    geant charge;
    geant tlife;
    geant ub[1];
    integer nwb=0;
    GFPART(ipart,chp,itrtyp,mass,charge,tlife,ub,nwb);
    charge=fabs(charge);

    if(ipart == 3 && low==3 ){
      HBOOK1(_hid,"Low Electron Spectrum",12,0.,6.,0.);
      HF1(_hid,0.75,12900.);
      HF1(_hid,1.25,4550.);
      HF1(_hid,1.75,1810.);
      HF1(_hid,2.25,846.);
      HF1(_hid,2.75,376.);
      HF1(_hid,3.25,177.);
      HF1(_hid,3.75,67.);
      HF1(_hid,4.25,29.);
      HF1(_hid,4.75,7.3);
      HF1(_hid,5.25,2.7);
      HF1(_hid,5.75,.4);
    }
    else if(low ==2){
      cout <<"AMSMceventg::setspectra-W-Sea Level muons Generator Chosen"<<endl;
      CMINIT();
    }
    else if(low==1){
      cout <<"AMSMceventg::setspectra-W-UnderCutoffParticleGeneratorChosen"<<endl;
      if(ipart!=14){
        cerr<<"AMSMCeventg::setspectra-F-OnlyProtonsAreCurrentlySupprted"<<endl;
       abort();
      }
      integer nchan=500;
      geant binw;
      binw=40;
      geant al=binw/2;
      geant bl=binw/2+nchan*binw;
      HBOOK1(_hid,"Spectrum",nchan,al,bl,0.);
      for(int i=0;i<nchan;i++){
        geant xm=i*binw+al+binw/2;
        number xmom=xm/1000;
       number xr=xmom/charge;
        number xkin=(sqrt(xmom*xmom+mass*mass)-mass);
        geant y=2.4*exp(-0.8*xkin)+100*exp(-4.26*xkin);
        if(xkin>0.005)HF1(_hid,xm,y);
     }
          
          HPRINT(_hid);
    }
    else if (low==0 || !(GMFFKEY.GammaSource==0)){//ISN
      integer nchan=10000;
      geant binw;
      if(mass < 0.938)binw=100;
      else  binw=100*mass/0.938;
      geant al=binw/2;
      geant bl=binw/2+nchan*binw;
      HBOOK1(_hid,"Spectrum",nchan,al,bl,0.);
      HBOOK1(_hid+1,"Spectrum",nchan,al,bl,0.);
      //
      // find a modulation
      //
      geant modul[14]={400.,350.,550.,650.,950.,1000.,1300.,1200.,1000.,900.,800.,600.,500.,500.};
      integer year=(begindate%10000+enddate%10000)/2-1997; 
      if(year <=0 || year > 13){
        cerr<<"AMSmceventg::setspectra-F-year not supported yet: "<<year<<endl;
        exit(1);
      }
      integer i; 
      for(i=0;i<nchan;i++){
        geant xm=i*binw+al+binw/2;
        number xmom=xm/1000;
        number xkin=(sqrt(xmom*xmom+mass*mass)-mass)*1000;
        number amass=mass*1000;
        number z=charge;
        number xkm=xkin+z*modul[year];
        number xt=xkm/1000+mass;
        number beta=sqrt(1.-mass*mass/xt/xt);
        number y;
        number xrig=z!=0?beta*xt/z:0;
        number constant=0.0137;
        number index=-2.10;
        if(ipart ==1&&(GMFFKEY.GammaSource==0||GMFFKEY.GammaBg==1)){//ISN
          // EGDB gamma-ray photon energy spec.(EGRET) per (m^2-sr-s-GeV)

          y= 0.0137/pow(xt,2.10);	            
        }
        else if((ipart==1)&&(!(GMFFKEY.GammaSource==0))&&GMFFKEY.GammaBg==0){

          lookupsourcesp(GMFFKEY.GammaSource,constant,index);
          y=constant*pow(xt,index);
          //ISN 
        }

        else if(ipart ==2){
          // positron          
          y=700./1.5/pow(xt,3.3)*(0.02+0.1/sqrt(xt));
          y=y*(xkin*xkin+2*amass*xkin)/(xkm*xkm+2*amass*xkm);
          
          
        }
        else if(ipart==3){
          //electron
          y=700./1.5/pow(xt,3.3);
          y=y*(xkin*xkin+2*amass*xkin)/(xkm*xkm+2*amass*xkm);
        }
        else if (ipart ==14 ){
          y=1.7e4/beta/pow(xrig,2.77);
          y=y*(xkin*xkin+2*amass*xkin)/(xkm*xkm+2*amass*xkm);
        }    
        else if (ipart > 15 && ipart < 100){
          // He etc...
          y=.252e4/beta/pow(xrig,2.74);
          y=y*(xkin*xkin+2*amass*xkin)/(xkm*xkm+2*amass*xkm);
        }    
        else if (ipart == 15 || ipart > 100){
          number xb=1.5;
          number xa=3.0;
          number fact=pow(xa/xb,1./(xa+xb));
          fact=2.7/fact;
          number zz=xrig/fact;
          number ampl=0.1;
          y=ampl*pow(zz,xa)/(pow(zz,xa+xb)+1.);
          y=y*(xkin*xkin+2*amass*xkin)/(xkm*xkm+2*amass*xkm);
        }
        else if(!_fixedmom){
	
         cerr<<"AMSmceventg::setspectra-F-NoPrimarySpectraDefinedForParticle "<<ipart<<endl;
         exit(1);
        }
        else{
          char hp[9]="//PAWC";
          HCDIR(hp," ");
          HCDIR (cdir, " ");
          return;
        }
        HF1(_hid,xm,y);
        HF1(_hid+1,xm,y);
      }//--->endof "nchan"-loop
    }//--->endof "low=0"
//
    else if(low==6){
    //read fluxfile
      char fname[161];
      UHTOC(CCFFKEY.FluxFile,40,fname,160);
      char fnam[256];
      strcpy(fnam,AMSDATADIR.amsdatadir);
      strcat(fnam,fname);  
      ifstream iftxt(fnam,ios::in);
      if(!iftxt){
       cerr <<"MCEventg::setcuts-F-UnableToOpenFluxFile "<<fnam<<endl;
       exit(1);
      }
      while(iftxt.get()!='Z'  && iftxt.good()){
      iftxt.ignore(1024,'%');
      }      
      if(iftxt.good()){
       iftxt.ignore(1024,'=');
       int zmin,zmax;
       iftxt>>zmin;
       iftxt.ignore(1024,'=');
       iftxt>>zmax;
       iftxt.ignore(1024,'%');
       if(iftxt.good() && iftxt.get()=='P'){
         iftxt.ignore(1024,'=');
         if(zmax-zmin+1>sizeof(_particle)/sizeof(_particle[0])){
          cerr <<"MCEventg::setcuts-F-ToManySpectraIn "<<fnam<<endl;
          exit(1);
         }
         else {
          for(int i=0;i<sizeof(_particle)/sizeof(_particle[0]);i++){
           _particle[i]=-1;          
           _flux[i]=0;          
           _spectra[i]=0;          
           }
           HBOOK1(_hid,"Fluxes",zmax-zmin+1,0,zmax-zmin+1,0.);
          }
         for(int i=0;i<zmax-zmin+1;i++)iftxt>>_particle[i];
         iftxt.ignore(1024,'%');
         if(iftxt.good() && iftxt.get()=='N'){
          iftxt.ignore(1024,'=');
          for(int i=0;i<zmax-zmin+1;i++)iftxt>>_nucleons[i];
         }
         iftxt.ignore(1024,'%');
         iftxt.ignore(1024,'%');
         iftxt.ignore(1024,'\n');
         double emin,emax;
         int nbin;          
         iftxt>>emin;
         iftxt>>emax;
         iftxt>>nbin;
          double a=log(emin);
          double b=log(emax);
          double* ene=new double[nbin];
         for(int i=0;i<nbin;i++){
          ene[i]=exp(a+(b-a)*(float(i)/float(nbin-1)));
         }        
         for(int i=0;i<zmax-zmin+1;i++){
          _spectra[i]=new float[nbin];
          HBOOK1(_hid+i+1,"Spectra",nbin-1,a,b,0.);
         }
         int ntry=nbin/6;
         iftxt.ignore(1024,'\n');
         iftxt.ignore(1024,'\n');
         iftxt.ignore(1024,'\n');
         for (int i=0;i<zmax-zmin+1;i++){
         for(int j=0;j<ntry;j++){
           for(int k=0;k<6;k++){
            iftxt>>_spectra[i][j*6+k];

           }
           iftxt.ignore(1024,'\n');
         }
         for(int j=1;j<nbin;j++){
          double x=a+(b-a)/float(nbin-1)*float(j-1)+(b-a)/(nbin-1)/2.;
          double y=(_spectra[i][j]+_spectra[i][j-1])*0.5*(ene[j]-ene[j-1]);
          _flux[i]+=y;
          HF1(_hid+i+1,float(x),float(y));
         }
         cout <<"  AMSmceventg::setspectra-I-Charge "<<zmin+i<<" Flux "<<_flux[i]<<endl;
         HF1(_hid,float(i)+0.5,float(_flux[i]));
         iftxt.ignore(1024,'\n');
        }
        HPRINT(_hid);
        
        delete[] ene;
        if(!iftxt.good()){
         cerr <<"MCEventg::setspectra-F-ProblemsToReadSpectraIn "<<fnam<<endl;
         exit(1);
        }
        else{
         cout <<"MCEventg::setspectra-I-"<<zmax-zmin+1<<" SpectraReadIn "<<fnam<<endl;
        }
       }
       else{
       cerr <<"MCEventg::setspectra-F-ProblemsToFindParticlesIn "<<fnam<<endl;
       exit(1);

       }              
      }
      else{
       cerr <<"MCEventg::setspectra-F-ProblemsToReadFluxFile "<<fnam<<endl;
       exit(1);
      }
      for(int i=0;i<sizeof(_spectra)/sizeof(_spectra[0]);i++){
         if(_spectra[i])delete[] _spectra[i];
      }        

    }
#ifdef __AMSDEBUG__
    //HPRINT(_hid);
#endif
#ifdef __G4AMS__
if(MISCFFKEY.G4On)AMSRandGeneral::book(_hid);
#endif
     char hp[9]="//PAWC";
     HCDIR(hp," ");
     HCDIR (cdir, " ");
  }
}
void AMSmceventg::lookupsourcesp(int sourceid, number & constant, number & index){
  switch(sourceid){
  case 2: //Crab approximate pulsed emission (at GeV, from EGRET)
    constant=226.2E-9;
    index=-2.1;
    break;
  case 3: //faint source at galactic center
    constant=5.E-9;
    index=-2.;
    break; 
  case 4: //Vela approximate pulsed emission (at GeV, from EGRET)
    constant=2100.E-9;
    index=-1.6;
    break;  
  default: // using average index for default;
    constant=100.2E-9;
    index=-2.;
  }
}

void AMSmceventg::lookupsourcecoo(int sourceid, geant & rasource, geant & decsource){
  switch(sourceid){ //if sourceid=GMFFKEY.GammaSource=1 --> user defined coo
  case 2: //Crab aproximate pulsed emission (at GeV, from EGRET)
    rasource=1.46;
    decsource=0.384;
    break;
  case 3: // faint source at galactic center
    rasource=4.65;
    decsource=-0.505;
    break;
  case 4: //Vela approximate pulsed emission (at GeV, from EGRET)
    rasource=2.24;
    decsource=-0.789;
    break;
  default: //Crab aproximate pulsed emission (at GeV, from EGRET)
    rasource=1.46;
    decsource=0.384;
  }
}


void AMSmceventg::setcuts(geant coo[6], geant dir[6],
   geant momr[2],integer fxp=0,geant albedor=0.1 ,geant albedocz=0.05){
    _albedorate=albedor;
    _albedocz=albedocz;
    _coorange[0]=AMSPoint(coo[0],coo[1],coo[2]);
    _coorange[1]=AMSPoint(coo[3],coo[4],coo[5]);
    _dirrange[0]=AMSPoint(dir[0],dir[1],dir[2]);
    _dirrange[1]=AMSPoint(dir[3],dir[4],dir[5]);
    _fixedplane=fxp;
    _momrange[0]=momr[0];
    _momrange[1]=momr[1];
    _fixedmom=0;
    _fixedpoint=0;
    _fixeddir=0;
    if(momr[0]>=momr[1])_fixedmom=1;
    if(coo[0]>=coo[3] && coo[1]>=coo[4] && coo[2]>=coo[5])_fixedpoint=1;
    if(dir[0]>=dir[3] && dir[1]>=dir[4] && dir[2]>=dir[5])_fixeddir=1;
    if(!(GMFFKEY.GammaSource==0))_fixeddir=1; //ISN 

    number area[6];
  number xa=_coorange[0][0]>-AMSDBc::ams_size[0]/2?_coorange[0][0]:-AMSDBc::ams_size[0]/2;
  number ya=_coorange[0][1]>-AMSDBc::ams_size[1]/2?_coorange[0][1]:-AMSDBc::ams_size[1]/2;
  number za=_coorange[0][2]>-AMSDBc::ams_size[2]/2?_coorange[0][2]:-AMSDBc::ams_size[2]/2;
  number xb=_coorange[1][0]<AMSDBc::ams_size[0]/2?_coorange[1][0]:AMSDBc::ams_size[0]/2;
  number yb=_coorange[1][1]<AMSDBc::ams_size[1]/2?_coorange[1][1]:AMSDBc::ams_size[1]/2;
  number zb=_coorange[1][2]<AMSDBc::ams_size[2]/2?_coorange[1][2]:AMSDBc::ams_size[2]/2;
  number lx=xb-xa;
  number ly=yb-ya;
  number lz=zb-za;
    area[0]=lx*ly;
    area[1]=lx*ly;
    area[2]=ly*lz;
    area[3]=ly*lz;
    area[4]=lx*lz;
    area[5]=lx*lz;
    int i;
    for ( i=1;i<6;i++)area[i]=area[i]+area[i-1];
    for ( i=0;i<6;i++)_planesw[i]=area[5]>0?area[i]/area[5]:(i+1.)/6.;

 

}

bool AMSmceventg::SpecialCuts(integer cut){
  if(cut ==1 ) {
   //  EcalFocusing
   static bool InitDone=false;
   static AMSPoint cross;
   if(!InitDone){
    AMSgvolume *ecal=AMSJob::gethead()->getgeomvolume(AMSID("ECMO",1));
    if(ecal){
     InitDone=true;
     AMSPoint cooA,par;
     for(int i=0;i<3;i++)cooA[i]=ecal->getcooA(i);
     for(int i=0;i<3;i++)par[i]=ecal->getpar(i);
      cross[2]=cooA[2]+par[2];
      cross[0]=par[0];
      cross[1]=par[1];
      cout <<" EcalFocusing-I-CrossingParameters: "<<cross<<endl;
    }
    else{
     cerr<<"AMSmceventg::SpecialCuts-S-NoEcalVulmefound "<<endl;
     return true;
    }   
   }
    if(_dir[2]){
     AMSPoint extrap=_coo+_dir*((cross[2]-_coo[2])/_dir[2]);
     if(fabs(extrap[0])<fabs(cross[0]) && fabs(extrap[1])<fabs(cross[1]))return true;
     else return false; 
    }
    else return false;
  }
  else if(cut==2){
    // TRD Top HC Focusing
   static bool InitDone=false;
   static number ZI,RI,ZO,RO;
   if(!InitDone){
    AMSgvolume *trdhc=AMSJob::gethead()->getgeomvolume(AMSID("TRD5",6));
    if(trdhc){
     InitDone=true;
     AMSPoint cooA;
     number par[10];
     for(int i=0;i<3;i++)cooA[i]=trdhc->getcooA(i);
     for(int i=0;i<10;i++)par[i]=trdhc->getpar(i);
      ZI=cooA[2]+par[4];
      RI=par[5];
      ZO=cooA[2]+par[7];
      RO=par[8];
      cout <<" TRDHCFocusing-I-CrossingParameters: "<<ZI<<" "<<RI<<" "<<ZO<<" "<<RO<<endl;
    }
    else{
     cerr<<"AMSmceventg::SpecialCuts-S-NoTRD5Volumefound "<<endl;
     return true;
    }   
   }
     AMSPoint extrapI=_coo+_dir*((ZI-_coo[2])/_dir[2]);
     AMSPoint extrapO=_coo+_dir*((ZO-_coo[2])/_dir[2]);
     if(extrapI[0]>RI && extrapO[0]>RO)return false;
     if(extrapI[0]<-RI && extrapO[0]<-RO)return false;
     if(extrapI[1]>RI && extrapO[1]>RO)return false;
     if(extrapI[1]<-RI && extrapO[1]<-RO)return false;
     if(extrapI[0]>RI)extrapI[0]=RI;
     if(extrapI[0]<-RI)extrapI[0]=-RI;
     if(extrapI[1]>RI)extrapI[1]=RI;
     if(extrapI[1]<-RI)extrapI[1]=-RI;
     if(extrapO[0]>RO)extrapO[0]=RO;
     if(extrapO[0]<-RO)extrapO[0]=-RO;
     if(extrapO[1]>RO)extrapO[1]=RO;
     if(extrapO[1]<-RO)extrapO[1]=-RO;
     number path=(extrapO-extrapI).norm();
     if(path<(RO+RI)/2){
      return false;
     }
     else return true; 
  }
  else if(cut==3){
   // solar panel - ams focusing
   static bool InitDone=false;
     static AMSDir n;
     static AMSPoint p;
     static AMSPoint ps;
     static AMSPoint amss;
   if(!InitDone){
    AMSgvolume *spa=AMSJob::gethead()->getgeomvolume(AMSID("SPA1",1));
    if(spa){
     InitDone=true;
     
     for(int i=0;i<3;i++)n[i]=spa->getnrmA(i,0);
     for(int i=0;i<3;i++)p[i]=spa->getcooA(i);
     for(int i=0;i<3;i++)ps[i]=spa->getpar(i);
    AMSgvolume *ams=AMSJob::gethead()->getgeomvolume(AMSID("FMOT",1));
     number para[3];
     for(int i=0;i<3;i++)amss[i]=ams->getpar(i);
     }  
     else{ 
       cerr<<"AMSmceventg::SpecialCuts-S-NoSolarPanelVolumefound "<<endl;
       return true;
     }
   }
     // check if track pass by solar panel
     
     number ns=n.prod(_dir);
     if(ns){
      number t=(_coo-p).prod(n)/(ns);
      AMSPoint psec=_coo-_dir*t;
      if(t<0 && fabs(psec[1]-p[1])<ps[1] && fabs(psec[2]-p[2])<ps[2]){
       
         // check if pass in the vicinity of 0 
         number t=_coo.prod(_dir);
         if(t<0 && (_coo-_dir*t).norm()<amss[0]){
//           cout <<" passed 1 "<<(_coo-_dir*t).norm()<<endl;
           return true;
         }
         else return false;
      }
      else return false;
     } 
     else return true;
  }
     return true;
}

integer AMSmceventg::accept(){
  _nskip=Orbit.Ntot;
  if(!(GMFFKEY.GammaSource==0)) return 1; //ISN
  if(_coo >= _coorange[0] && _coo <= _coorange[1]){
    if(_fixeddir || (_dir >= _dirrange[0] && _dir<= _dirrange[1])){
      if(_mom>=_momrange[0] && _mom <= _momrange[1]){
         geant d;
          if(SpecialCuts(CCFFKEY.SpecialCut)){
//        if(CCFFKEY.low || _fixeddir || _dir[2]<_albedocz || RNDM(d)< _albedorate)
            if(!CCFFKEY.low  && CCFFKEY.earth == 1 && !_fixeddir && !_fixedmom) 
            return EarthModulation();
           else return 1;
          }
      }
    }
  }
  ++Orbit.Nskip;   
  ++Orbit.Nskip2;   
  return 0;
}
integer AMSmceventg::acceptio(){
  if(_coo >= _coorange[0] && _coo <= _coorange[1]){
    if(_fixeddir || (_dir >= _dirrange[0] && _dir<= _dirrange[1])){
      if(_mom>=_momrange[0] && _mom <= _momrange[1]){
          if(AMSJob::gethead()->isRealData()){
            if(!MISCFFKEY.BeamTest || (_ipart==GCKINE.ikine || (_ipart>0  && GCKINE.ikine==-1)))return 1;
          }
//          else return EarthModulation();
          else return 1;
      }
    }
  }
  return 0;
}

integer AMSmceventg::EarthModulation(){
  // Get current station position from event bank
  number polephi,theta,phi;
  
  AMSEvent::gethead()->GetGeographicCoo(polephi, theta, phi);
  geant pitch=AMSEvent::gethead()->getpitch();
  geant roll=AMSEvent::gethead()->getroll();
  geant yaw=AMSEvent::gethead()->getyaw();
  //
  // Dipole direction
  //
  number um=sin(AMSDBc::pi/2-Orbit.PoleTheta)*cos(polephi);
  number vm=sin(AMSDBc::pi/2-Orbit.PoleTheta)*sin(polephi);
  number wm=cos(AMSDBc::pi/2-Orbit.PoleTheta);

  //
  // Direction Dipole to Station 
  //
  number dphi=Orbit.DipolePhi+polephi-Orbit.PolePhiStatic;
  number xc=Orbit.AlphaAltitude*sin(AMSDBc::pi/2-theta)*cos(phi)-
   Orbit.DipoleR*sin(AMSDBc::pi/2-Orbit.DipoleTheta)*cos(dphi);
  number yc=Orbit.AlphaAltitude*sin(AMSDBc::pi/2-theta)*sin(phi)-
   Orbit.DipoleR*sin(AMSDBc::pi/2-Orbit.DipoleTheta)*sin(dphi);
  number zc=Orbit.AlphaAltitude*cos(AMSDBc::pi/2-theta)-
   Orbit.DipoleR*cos(AMSDBc::pi/2-Orbit.DipoleTheta);
  number rl=sqrt(xc*xc+zc*zc+yc*yc);
  number  up=xc/rl;
  number  vp=yc/rl;
  number  wp=zc/rl;

  number cts=um*up+vm*vp+wm*wp;
  number xl=acos(cts);
  number cl=fabs(sin(xl));
  number rgm=rl;
  // Magnetic East
  xc=vm*wp-wm*vp;
  yc=wm*up-um*wp;
  zc=um*vp-vm*up;
  rl=sqrt(xc*xc+yc*yc+zc*zc);

  number  uv=xc/rl;
  number  vv=yc/rl;
  number  wv=zc/rl;

  //
  // particle dir in global system
  // AMS x along the shuttle/station flight
  //
  number amsxg[3],amsyg[3],amszg[3];
  amszg[0]=sin(AMSDBc::pi/2-theta)*cos(phi);
  amszg[1]=sin(AMSDBc::pi/2-theta)*sin(phi);
  amszg[2]=cos(AMSDBc::pi/2-theta);
  amsyg[0]=Orbit.Axis[0];
  amsyg[1]=Orbit.Axis[1];
  amsyg[2]=Orbit.Axis[2];
  amsxg[0]=amsyg[1]*amszg[2]-amsyg[2]*amszg[1];
  amsxg[1]=amsyg[2]*amszg[0]-amsyg[0]*amszg[2];
  amsxg[2]=amsyg[0]*amszg[1]-amsyg[1]*amszg[0];

  number amsx[3],amsy[3],amsz[3];
  number cp=cos(pitch); 
  number sp=sin(pitch);
  number cy=cos(yaw);
  number sy=sin(yaw);
  number cr=cos(roll);
  number sr=sin(roll);
  number l1=cy*cp;
  number m1=-sy;
  number n1=cy*sp;
  number l2=cr*sy*cp-sr*sp;
  number m2=cr*cy;
  number n2=cr*sy*sp+sr*cp;
  number l3=-sr*sy*cp-cr*sp;
  number m3=-sr*cy;
  number n3=-sr*sy*sp+cr*cp;
  amsx[0]=-(l1*amsxg[0]+l2*amsyg[0]+l3*amszg[0]);
  amsx[1]=-(l1*amsxg[1]+l2*amsyg[1]+l3*amszg[1]);
  amsx[2]=-(l1*amsxg[2]+l2*amsyg[2]+l3*amszg[2]);
  amsy[0]=-(m1*amsxg[0]+m2*amsyg[0]+m3*amszg[0]);
  amsy[1]=-(m1*amsxg[1]+m2*amsyg[1]+m3*amszg[1]);
  amsy[2]=-(m1*amsxg[2]+m2*amsyg[2]+m3*amszg[2]);
  amsz[0]=n1*amsxg[0]+n2*amsyg[0]+n3*amszg[0];
  amsz[1]=n1*amsxg[1]+n2*amsyg[1]+n3*amszg[1];
  amsz[2]=n1*amsxg[2]+n2*amsyg[2]+n3*amszg[2];
  number ue=_dir[0]*amsx[0]+_dir[1]*amsy[0]+_dir[2]*amsz[0];
  number ve=_dir[0]*amsx[1]+_dir[1]*amsy[1]+_dir[2]*amsz[1];
  number we=_dir[0]*amsx[2]+_dir[1]*amsy[2]+_dir[2]*amsz[2];
  
  number cth=ue*uv+ve*vv+we*wv;
  number xfac=57.576*Orbit.EarthR/rgm*Orbit.EarthR/rgm;
  number chsgn=_charge/fabs(_charge);
  number cl3=cl*cl*cl;
  number cl4=cl*cl*cl*cl;
  number mom=CCFFKEY.Fast==0?
  xfac*cl4/(sqrt(1.-chsgn*cth*cl3)+1)/(sqrt(1.-chsgn*cth*cl3)+1)*fabs(_charge):
  xfac*cl4/4;
  if (_mom > mom)return 1;
  else {
  //cout <<xfac<<" "<<_mom<<" "<<mom<<endl;
  ++Orbit.Nskip;
   return 0;
  }
}
  
void AMSmceventg::gendelay(){

  // Generate a random delay for a track between 0 and 10 microsecs

  geant d(-1);
  _delay=RNDM(d)*10.;

}


void AMSmceventg::init(integer ipart){
_ipart=ipart;
_delay = 0.;
char chn[22]="";
integer itr=0;
geant amass=0;
geant charge=0;
geant tlife=0;
geant ubb[1];
integer one=1;
GFPART(abs(_ipart)%256,chn,itr,amass,charge,tlife,ubb,one);
_mass=amass;
_charge=charge;
  }


  void AMSmceventg::run(integer ipart){
   init(ipart);
   geant plab[3],vertex[3];
   integer nvert=0;
   integer nt=0;
   do{
      gener();
    }while(!accept());
    // Set seed
#ifdef __G4AMS__
if(!MISCFFKEY.G3On){
_seed[0]=HepRandom::getTheSeeds()[0];
_seed[1]=HepRandom::getTheSeeds()[1];
}
else
#endif
   GRNDMQ(_seed[0],_seed[1],0,"G");
//   cout <<"seed[ "<<_seed[0]<<" "<<_seed[1]<<endl;
//   cout << "  direction "<<_dir/_dir[2]<<" coo "<<_coo<<endl;
   vertex[0]=_coo[0];
   vertex[1]=_coo[1];
   vertex[2]=_coo[2];
   plab[0]=_mom*_dir[0];
   plab[1]=_mom*_dir[1];
   plab[2]=_mom*_dir[2];
   GSVERT(vertex,0,0,0,0,nvert);
   GSKINE(plab,_ipart,nvert,0,0,nt);
  }

  void AMSmceventg::run(){
   geant plab[3],vertex[3];
   integer nvert=0;
   integer nt=0;
   if(acceptio()){
    vertex[0]=_coo[0];
    vertex[1]=_coo[1];
    vertex[2]=_coo[2];
    plab[0]=_mom*_dir[0];
    plab[1]=_mom*_dir[1];
    plab[2]=_mom*_dir[2];
    GSVERT(vertex,0,0,0,0,nvert);
    GSKINE(plab,_ipart,nvert,0,0,nt);
   }   
  }

void AMSmceventg::InitSeed(){

  //C
  //C             Initialise the random number generator
  //C
  //      IF(NRNDM(2).NE.0)THEN
  //         CALL GRNDMQ(NRNDM(1),NRNDM(2),1,'S')
  //      ELSEIF(NRNDM(1).GT.0)THEN
  //         ISEQ=NRNDM(1)
  //         CALL GRNDMQ(NRNDM(1),NRNDM(2),ISEQ,'Q')
  //         CALL GRNDMQ(NRNDM(1),NRNDM(2),ISEQ,'S')
  //      ENDIF
  //
  GCFLAG.NRNDM[0]=_seed[0];
  GCFLAG.NRNDM[1]=_seed[1];
#ifdef __G4AMS__
if(!MISCFFKEY.G3On){
 long seedl[3]={0,0,0};
 seedl[0]=_seed[0];
 seedl[1]=_seed[1];
 HepRandom::setTheSeeds(seedl);
}
else{
#endif
  if(_seed[1] !=0)GRNDMQ(_seed[0],_seed[1],1,"S");
  else if(_seed[0] > 0){
   integer ISEQ=_seed[0];
   GRNDMQ(_seed[0],_seed[1],ISEQ,"Q");
   GRNDMQ(_seed[0],_seed[1],ISEQ,"S");
  }
#ifdef __G4AMS__
}
#endif
}  

void AMSmceventg::_printEl(ostream &stream){
geant phi=atan2(_dir[1],_dir[0]);
geant theta=acos(_dir[2]);
stream <<" Particle " << _ipart <<" Theta "<<theta<<" Phi "<<phi<<
       " Ridgidity " << _mom/_charge <<endl;
stream <<" Coo "<<_coo[0]<<" "<<_coo[1]<<" "<<_coo[2]<<endl;
stream <<" Random numbers "<<_seed[0]<<" "<<_seed[1]<<endl;
}

void AMSmceventg::_copyEl(){
  if(IOPA.mode ==2 || IOPA.mode==3){
  // write elem  for simple i/o
   integer run,event;
   if (Primary()){
   run=AMSEvent::gethead()->getrun();
   event=AMSEvent::gethead()->getEvent();
   number pole,theta,phi;
   AMSEvent::gethead()->GetGeographicCoo(pole,theta,phi);
   AMSIO io(run,event,AMSEvent::gethead()->gettime(),AMSEvent::gethead()->getusec(),_ipart,_seed,_coo,_dir,
   _mom,pole,theta,phi,_nskip,AMSEvent::gethead()->GetStationRad(),
   AMSEvent::gethead()->getveltheta(),AMSEvent::gethead()->getvelphi(),
   AMSEvent::gethead()->getyaw(),AMSEvent::gethead()->getroll(),
   AMSEvent::gethead()->getpitch(),AMSEvent::gethead()->getangvel());
   io.write();
  }
  }
}

void AMSmceventg::_writeEl(){
if( Out(_ipart>0 || IOPA.WriteAll%10==1 )){
#ifdef __WRITEROOT__
  AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
  MCEventGNtuple02* GN = AMSJob::gethead()->getntuple()->Get_mcg02();

  if (GN->Nmcg>=MAXMCG02) return;
// added specifically to reduce ntuple size

// Fill the ntuple
  int i;
  GN->Nskip[GN->Nmcg]=_nskip;
  GN->Particle[GN->Nmcg]=_ipart;
  for(i=0;i<3;i++)GN->Coo[GN->Nmcg][i]=_coo[i];
  for(i=0;i<3;i++)GN->Dir[GN->Nmcg][i]=_dir[i];
  GN->Momentum[GN->Nmcg]=_mom;
  GN->Mass[GN->Nmcg]=_mass;
  GN->Charge[GN->Nmcg]=_charge;
  GN->Nmcg++;
}
}


integer AMSmceventg::checkdaqid(int16u id){
if(id==getdaqid())return 1;
else return 0;
}

void AMSmceventg::builddaq(integer i, integer length, int16u *p){
  AMSmceventg *ptr=(AMSmceventg*)AMSEvent::gethead()->
  getheadC("AMSmceventg",0);
 *p=getdaqid();
while(ptr){ 
if(ptr->Primary()){
 const uinteger c=65535;
 *(p+1)=ptr->_ipart;
 uinteger momentum=uinteger(ptr->_mom*1000);
 *(p+3)=int16u(momentum&c);
 *(p+2)=int16u((momentum>>16)&c);
 int16u d=int16u((ptr->_dir[0]+1)*10000);
 *(p+4)=d;
 d=int16u((ptr->_dir[1]+1)*10000);
 *(p+5)=d;
 d=int16u((ptr->_dir[2]+1)*10000);
 *(p+6)=d;
 integer big=10000;
 for(int k=0;k<3;k++){
 if(ptr->_coo[k]+big<0){
  cerr<<"AMSmceventg::builddaq-E-NegativeCoo "<<ptr->_coo[k]<<endl;
 }
 uinteger cd=(ptr->_coo[k]+big)*10000;
 *(p+8+2*k)=int16u(cd&c);
 *(p+7+2*k)=int16u((cd>>16)&c);
 }
 p+=12;
}
 ptr=ptr->next();
}

}


void AMSmceventg::buildraw(integer n, int16u *p){
 integer ip;
 geant mom;
 for(int16u *ptr=p+1;ptr<p+n;ptr+=12){ 
  ip=*(ptr);
  uinteger momi= (*(ptr+2)) | (*(ptr+1))<<16;
  mom=momi/1000.;
  AMSDir dir(integer(*(ptr+3))-10000,integer(*(ptr+4))-10000,integer(*(ptr+5))-10000);
  uinteger cdx=  (*(ptr+7)) | (*(ptr+6))<<16;  
  uinteger cdy=  (*(ptr+9)) | (*(ptr+8))<<16;  
  uinteger cdz=  (*(ptr+11)) | (*(ptr+10))<<16;  
  AMSPoint coo(cdx/10000.-10000.,cdy/10000.-10000.,cdz/10000.-10000.);
        AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), new
        AMSmceventg(ip,mom,coo,dir));

}

} 

integer AMSmceventg::calcdaqlength(integer i){
 AMSContainer *p = AMSEvent::gethead()->getC("AMSmceventg");
   integer l=1;
    for(AMSmceventg* pm=
      (AMSmceventg*)AMSEvent::gethead()->getheadC("AMSmceventg",0);pm;pm=pm->next()){
       if(pm->Primary())l+=12;
      }
 return l;
}

void orbit::UpdateOrbit(number theta, number phi, integer sdir){
  ThetaI=theta;
  PhiI=phi;
  number MIR=atan(AlphaTanThetaMax);
  number r= tan(ThetaI)/AlphaTanThetaMax;
  if(r > 1 || r < -1){
    cerr <<"AMSMCEVENTG::setspectra-ThetaI too high "<<ThetaI<<endl;
    if(ThetaI < 0 )ThetaI = -MIR;
    else ThetaI= MIR;
    cerr <<"AMSMCEVENTG::setspectra-ThetaI brought to "<<ThetaI<<endl;
    r=tan(ThetaI)/AlphaTanThetaMax;
  }
  PhiZero=PhiI-atan2(r,sdir*sqrt(1-r*r));
  Axis[0]=sin(MIR)*sin(PhiZero);
  Axis[1]=-sin(MIR)*cos(PhiZero);
  Axis[2]=cos(MIR);
  AMSDir ax1(AMSDBc::pi/2-ThetaI,PhiI);
  AMSDir ax2(AMSDBc::pi/2,PhiZero);
  if(ax1.prod(Axis)>1.e-5){
   cerr<<"AMSmceventg::setspectra-F-OrbitParametersWrong "<<ax1.prod(Axis);
  exit(1);
  }
}

integer orbit::UpdateOrbit(number xsec, geant & ThetaS, geant & PhiS, geant & PolePhi, number & RaS, number & DecS, number & GLatS, number & GLongS, time_t & time){ 
    number t2=
      AlphaTanThetaMax*AlphaTanThetaMax; 
    number theta=ThetaI;
    number philocal=
      atan2(sin(PhiI-PhiZero)*sqrt(1+t2),
            cos(PhiI-PhiZero));    
    number pole=PolePhi;

    pole=fmod(pole+EarthSpeed*xsec,AMSDBc::twopi);

    philocal=fmod(philocal+AlphaSpeed*xsec,AMSDBc::twopi); 

    number phi=atan2(sin(philocal),cos(philocal)*sqrt(1+t2));
    if(phi < 0)phi=phi+AMSDBc::twopi;
    theta=asin(sin(atan(AlphaTanThetaMax))*sin(philocal));

    time=integer(mktime(&Begin)+xsec); 
    ThetaS=theta;
    PolePhi=pole;
    PhiS=fmod(phi+PhiZero+NodeSpeed*xsec,AMSDBc::twopi);
     ///// NodeSpeed is  ascending node precession due to oblateness
    number alt = AlphaAltitude;

    //number correction=fmod(1.848384291E-7*xsec,AMSDBc::twopi);
    //number correction=fmod(1.98694E-7*xsec,AMSDBc::twopi);
    //number correction=0;
    //number correction=fmod((6.27894582393474/(365*86400))*xsec,AMSDBc::twopi);
    number truephi=fmod(PhiS-(PolePhi-AMSmceventg::Orbit.PolePhiStatic)+AMSDBc::twopi,AMSDBc::twopi);  
   
    skyposition isspos(ThetaS,truephi,alt,time); // calculate celestial position 

    isspos.GetRa(RaS); // Values are stored in radians through Get**
    isspos.GetDec(DecS); 
    isspos.GetLat(GLatS); 
    isspos.GetLong(GLongS); 
    return cos(PhiS-PhiZero)>0?1:-1;
}
//ISN
orbit::orbit(geant Th,geant Ph, geant Pole, integer Dir):
ThetaI(Th),PolePhi(Pole){
  PhiI=fmod(Ph+AMSDBc::twopi,AMSDBc::twopi);
  const number MIR=51.65;
  AlphaTanThetaMax=tan(MIR/AMSDBc::raddeg);
  UpdateOrbit(ThetaI,PhiI,Dir);
  AlphaSpeed=AMSDBc::twopi/90.8/60.;
  EarthSpeed=AMSDBc::twopi/86164.091; //ISN rad/s
  NodeSpeed=9.88E-7; // ISN ascending node precession due to oblateness
  Begin.tm_year  =  98;
  Begin.tm_mon = 05;
  Begin.tm_mday   = 2;
  Begin.tm_hour  = 12;
  Begin.tm_min= 0;
  Begin.tm_sec=0;
  Begin.tm_isdst =  0;

}
void orbit::UpdateAxis(number vt, number vp, number t, number p){
  AMSDir ax1(AMSDBc::pi/2-t,p);
  AMSDir ax2(AMSDBc::pi/2-vt,vp);
  Axis=ax1.cross(ax2);
}

#ifdef __G4AMS__
#include "geant4.h"
  void AMSmceventg::runG4(integer ipart){
  if(ipart){
   init(ipart);
    do{
      gener();
    }while(!accept());
    // Set seed
#ifdef __G4AMS__
if(!MISCFFKEY.G3On){
_seed[0]=HepRandom::getTheSeeds()[0];
_seed[1]=HepRandom::getTheSeeds()[1];
}
else
#endif
   GRNDMQ(_seed[0],_seed[1],0,"G");
// cout <<"seed[ "<<_seed[0]<<" "<<_seed[1]<<endl;
// cout <<_coo<<" "<<_dir<<endl;
   AMSJob::gethead()->getg4generator()->SetParticleGun(ipart,_mom,_coo,_dir);
  }
  else{
   if(acceptio()){
    AMSJob::gethead()->getg4generator()->SetParticleGun(_ipart,_mom,_coo,_dir);
   }   
  }
}

#endif
extern "C" void getscanfl_(int &scan);
void AMSmceventg::FillMCInfo(){
static number radl=0;
static number absl=0;
static integer init=0;
static number ECALZ=-FLT_MAX;
if(!init){
 init=1;
 AMSgvolume *p =AMSJob::gethead()->getgeomvolume(AMSID("PMS1",1));
if(p){
 ECALZ=p->getcooA(2)*0.95;
 cout <<"AMSmceventg::FillMCInfo-I-LowestZSetTo "<<ECALZ<<endl;
}
else{
 cerr<<"AMSmceventg::FillMCInfo-W-NoPMS1VolumeFound"<<endl;
}
}

if(GCTRAK.sleng==0){
 radl=0;
 absl=0;
 
}
int scan=0;
getscanfl_(scan);
if(GCKINE.itra==1 && scan && GCKINE.ikine==GCKINE.ipart){
 radl+=GCTRAK.step/GCMATE.radl;
 absl+=GCTRAK.step/GCMATE.absl;
 char nvol[5]="EXIT";
 if(GCTRAK.inwvol==1 && GCVOLU.nlevel<500){
       for(int i=0;i<4;i++)nvol[i]=GCVOLU.names[GCVOLU.nlevel-1][i];
       AMSmctrack* genp=new AMSmctrack(radl,absl,GCTRAK.vect,nvol);
       AMSEvent::gethead()->addnext(AMSID("AMSmctrack",0), genp);

 }
 else if(GCTRAK.inwvol==3){
       AMSmctrack* genp=new AMSmctrack(radl,absl,GCTRAK.vect,nvol);
       AMSEvent::gethead()->addnext(AMSID("AMSmctrack",0), genp);
 }
}
     for (int i=0;i<GCKING.ngkine;i++){
       number mom=(GCKING.gkin[i][0]*GCKING.gkin[i][0]+
                       GCKING.gkin[i][1]*GCKING.gkin[i][1]+
                       GCKING.gkin[i][2]*GCKING.gkin[i][2]);
       mom=sqrt(mom);
       if(GCKIN3.GPOS[i][2]>ECALZ){
        AMSmceventg* genp=new AMSmceventg(-GCKING.gkin[i][4],mom,
        AMSPoint(GCKIN3.GPOS[i][0],GCKIN3.GPOS[i][1],GCKIN3.GPOS[i][2]),
        AMSDir(GCKING.gkin[i][0],GCKING.gkin[i][1],GCKING.gkin[i][2]));
        AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), genp);
       }
     }

}

integer AMSmceventg::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSmceventg",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}

bool AMSmceventg::Primary(){
 return _ipart>0 && _ipart<256;
}

void AMSmceventg::endjob(){
    char cdir[256];
    char *R="R";
    HCDIR(cdir,R);
    char hpawc[256]="//PAWC/GEN";
    char *BLANK=" ";
    HCDIR (hpawc,BLANK);
    if(CCFFKEY.low==2){
     CMEND();
    }
    char hp[9]="//PAWC";
    HCDIR(hp," ");
    HCDIR (cdir, " ");
}
//
integer AMSmceventg::fastcheck(geant xin, geant yin, geant zb, geant theta, geant phi){
  static integer first=1;
  geant ztof1t=66.5;//tempor
  geant ztof1b=64.0;//tempor
  geant ztof2t=63.5;//tempor
  geant ztof2b=61.0;//tempor
  geant xtof1=131/2;//tempor
  geant ytof1=114/2;//tempor
  geant xtof2=120/2;//tempor
  geant ytof2=133/2;//tempor
  geant zanti;
  geant ranti;
  geant zcal=ECALDBc::gendim(7);
  geant calhs=ECALDBc::gendim(1)/2.+ECMCFFKEY.safext;//cal.half-size(+ extention)
  geant dxy,xcr,ycr;
//
     zanti=ATGEFFKEY.scleng/2.;
     ranti=ATGEFFKEY.scradi;
     if(first){
       first=0;
       cout<<"AMSmceventg::fastcheck:z/r-anti="<<zanti<<" "<<ranti<<" zcal="<<zcal<<endl;
     }
     dxy=(zb-zanti)*sin(theta); // cr.with anti-top
     xcr=xin+dxy*cos(phi);
     ycr=yin+dxy*sin(phi);
     if((xcr*xcr+ycr*ycr)>(ranti*ranti))return 0;
     dxy=(zb+zanti)*sin(theta); // cr.with anti-bot
     xcr=xin+dxy*cos(phi);
     ycr=yin+dxy*sin(phi);
//     if((xcr*xcr+ycr*ycr)>(ranti*ranti))return 0;
     if(TFMCFFKEY.fast==2){//<-- check ECAL sens.volume if needed
       dxy=(zb-zcal)*sin(theta); // cr.with calor-top
       xcr=xin+dxy*cos(phi);
       ycr=yin+dxy*sin(phi);
       if((fabs(xcr)>calhs) || (fabs(ycr)>calhs))return 0;

//       dxy=(zb-ztof1t)*sin(theta); // cr.with tof1top
//       xcr=xin+dxy*cos(phi);
//       ycr=yin+dxy*sin(phi);
//       if((fabs(xcr)>xtof1) || (fabs(ycr)>ytof1))return 0;
       
//       dxy=(zb-ztof1b)*sin(theta); // cr.with tof1bot
//       xcr=xin+dxy*cos(phi);
//       ycr=yin+dxy*sin(phi);
//       if((fabs(xcr)<xtof1) && (fabs(ycr)<ytof1))return 0;
       
//       dxy=(zb-ztof2t)*sin(theta); // cr.with tof2top
//       xcr=xin+dxy*cos(phi);
//       ycr=yin+dxy*sin(phi);
//       if((fabs(xcr)<xtof2) && (fabs(ycr)<ytof2))return 0;
       
//       dxy=(zb-ztof2b)*sin(theta); // cr.with tof2bot
//       xcr=xin+dxy*cos(phi);
//       ycr=yin+dxy*sin(phi);
//       if((fabs(xcr)<xtof2) && (fabs(ycr)<ytof2))return 0;
     }
//
     TOF2JobStat::addmc(14);
     return 1;
}

void AMSmceventg::SaveSeeds(){
#ifdef __G4AMS__
if(!MISCFFKEY.G3On){
GCFLAG.NRNDM[0]=HepRandom::getTheSeeds()[0];
GCFLAG.NRNDM[1]=HepRandom::getTheSeeds()[1];
}
else
#endif
   GRNDMQ(GCFLAG.NRNDM[0],GCFLAG.NRNDM[1],0,"G");
}

void AMSmceventg::PrintSeeds(ostream & o){
long tmp[2];
tmp[0]=GCFLAG.NRNDM[0];
tmp[1]=GCFLAG.NRNDM[1];
SaveSeeds();
o<<GCFLAG.NRNDM[0]<<" "<<GCFLAG.NRNDM[1]<<endl;
GCFLAG.NRNDM[0]=tmp[0];
GCFLAG.NRNDM[1]=tmp[1];
}
void AMSmceventg::RestoreSeeds(){
#ifdef __G4AMS__
if(!MISCFFKEY.G3On){
 long seedl[3]={0,0,0};
 seedl[0]=GCFLAG.NRNDM[0];
 seedl[1]=GCFLAG.NRNDM[1];
 HepRandom::setTheSeeds(seedl);
}
else
#endif
  GRNDMQ(GCFLAG.NRNDM[0],GCFLAG.NRNDM[1],1,"S");
}


void AMSmceventg::SetSeed( int seed){

#ifdef __G4AMS__
if(!MISCFFKEY.G3On){
 HepRandom::setTheSeed(seed);
}
else{
#endif
   integer __seed[2];
   GRNDMQ(__seed[0],__seed[1],seed+1,"Q");
   GRNDMQ(__seed[0],__seed[1],seed+1,"S");
#ifdef __G4AMS__
}

#endif


}



void AMSmctrack::_writeEl(){
#ifdef __WRITEROOT__
  AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
// Fill the ntuple
  MCTrackNtuple* GN = AMSJob::gethead()->getntuple()->Get_mct();

  if (GN->Nmct>=MAXMCVOL) return;
  GN->radl[GN->Nmct]=_radl;
  GN->absl[GN->Nmct]=_absl;
  for(int i=0;i<3;i++)GN->pos[GN->Nmct][i]=_pos[i];
  for(int i=0;i<4;i++)GN->vname[GN->Nmct][i]=_vname[i];
  GN->Nmct++;
}







