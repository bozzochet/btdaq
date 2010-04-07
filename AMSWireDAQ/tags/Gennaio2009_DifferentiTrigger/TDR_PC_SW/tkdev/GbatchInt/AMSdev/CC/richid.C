#include"richid.h"
#include"commons.h"
#include "job.h"
#include <fstream.h>

geant *AMSRICHIdGeom::_pmt_p[3]={0,0,0};
integer *AMSRICHIdGeom::_pmt_orientation=0;
integer *AMSRICHIdGeom::_pmt_crate=0;
integer *AMSRICHIdGeom::_npmts_crate=0;
integer *AMSRICHIdGeom::_crate_pmt=0;
integer AMSRICHIdGeom::_ncrates=12;
integer AMSRICHIdGeom::_npmts=1000;


void AMSRICHIdGeom::Init(){
    _ncrates=8;
    _npmts=680;


    // Alloc space for pmt position
    _pmt_p[0]=new geant[_npmts];
    _pmt_p[1]=new geant[_npmts];
    _pmt_p[2]=new geant[_npmts];

    _pmt_orientation=new integer[_npmts];
  
    // Alloc space for pmt->crate table
    _pmt_crate=new integer[_npmts];
  
    // Alloc space fot crate->first pmt table
    _crate_pmt=new integer[_ncrates];

    // Alloc scape for number of pmts per crate
    _npmts_crate=new integer[_ncrates];
 

    // Fill tables: manual filling


    // Crate positioning is according to next drawing
    //
    //          3| 2 |1
    //          -+---+-
    //          4|   |0
    //          -+---+-
    //          5| 6 |7


    _npmts_crate[0]=143;
    _npmts_crate[1]=27;
    _npmts_crate[2]=143;
    _npmts_crate[3]=27;
    _npmts_crate[4]=143;
    _npmts_crate[5]=27;
    _npmts_crate[6]=143;
    _npmts_crate[7]=27;

    // Setups the look-up table 
    _crate_pmt[0]=0;
    for(int i=1;i<8;i++) _crate_pmt[i]=_crate_pmt[i-1]+_npmts_crate[i-1];

    integer pmtnb=0;

    // Setup crates 0,4: It's a bit tricky due to the pmts arrangement
    {
      const int nrows=17;
      const int pmts[nrows]={6,8,8,9,9,9,9,9,9,9,9,9,9,9,8,8,6};
      const int offset[nrows]={1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
      const int max_pmts=10;
      const int block_number=6; 
      
      //      geant xedge=RICHDB::hole_radius+pitch/2+RICpmtsupport;
      //      geant yedge=-RICHDB::hole_radius+pitch/2;  


      // Block 0 and 4 specifics
      geant xedge=34.65;
      geant yedge=-29.6;

      for(int i=0;i<nrows;i++)
	
	for(int j=0;j<pmts[i];j++){
	  
          *(_pmt_p[0]+_crate_pmt[0]+pmtnb)=xedge+(j+offset[i])*pitch;
          *(_pmt_p[1]+_crate_pmt[0]+pmtnb)=yedge+i*pitch;
          *(_pmt_p[2]+_crate_pmt[0]+pmtnb)=-RICHDB::pmt_pos();
	  
	  
          *(_pmt_p[0]+_crate_pmt[4]+pmtnb)=-(xedge+(j+offset[i])*pitch);
          *(_pmt_p[1]+_crate_pmt[4]+pmtnb)=-(yedge+i*pitch);
          *(_pmt_p[2]+_crate_pmt[4]+pmtnb)=-RICHDB::pmt_pos();
	  
	  
	  pmtnb++;
	  
        }
    }


    pmtnb=0;


    // Setup crates 2,6: It's a bit tricky due to the pmts arrangement
    {
      const int nrows=17;
      const int pmts[nrows]={6,8,8,9,9,9,9,9,9,9,9,9,9,9,8,8,6};
      const int offset[nrows]={1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
      const int max_pmts=10;
      const int block_number=6; 
      
      //      geant xedge=RICHDB::hole_radius+pitch/2+RICpmtsupport;
      //      geant yedge=-RICHDB::hole_radius+pitch/2;  


      // Block 2 and 6 specifics
      geant xedge=29.85;
      geant yedge=34.4;
      geant shift=0;

      for(int i=0;i<nrows;i++)
	for(int j=0;j<pmts[i];j++){
	  
	  shift=i<9?0:-.5;
	  
          *(_pmt_p[0]+_crate_pmt[2]+pmtnb)=xedge-i*pitch+shift;
          *(_pmt_p[1]+_crate_pmt[2]+pmtnb)=yedge+(j+offset[i])*pitch;
          *(_pmt_p[2]+_crate_pmt[2]+pmtnb)=-RICHDB::pmt_pos();
	  
	  
          *(_pmt_p[0]+_crate_pmt[6]+pmtnb)=-(xedge-i*pitch+shift);
	  *(_pmt_p[1]+_crate_pmt[6]+pmtnb)=-(yedge+(j+offset[i])*pitch);
          *(_pmt_p[2]+_crate_pmt[6]+pmtnb)=-RICHDB::pmt_pos();
	  
	  
	  pmtnb++;
	  
        }
    }




    pmtnb=0;


    { //Setup crates 1,3,5,7
      const int nrows=7;
      const int pmts[nrows]={6,6,5,4,3,2,1};
      const int offset[nrows]={1,0,0,0,0,0,0};
      const int max_pmts=6;
          
      //      geant xedge=RICHDB::hole_radius+pitch/2+RICpmtsupport;
      //      geant yedge=RICHDB::hole_radius+pitch/2+RICpmtsupport;

      geant xedge=34.65;
      geant yedge=34.4;

      for(int i=0;i<nrows;i++)
       for(int j=0;j<pmts[i];j++){

        *(_pmt_p[0]+_crate_pmt[1]+pmtnb)=xedge+(j+offset[i])*pitch;
        *(_pmt_p[1]+_crate_pmt[1]+pmtnb)=yedge+i*pitch;
        *(_pmt_p[2]+_crate_pmt[1]+pmtnb)=-RICHDB::pmt_pos();

        *(_pmt_p[0]+_crate_pmt[3]+pmtnb)=-(xedge+(j+offset[i])*pitch);
        *(_pmt_p[1]+_crate_pmt[3]+pmtnb)=yedge+i*pitch;       
        *(_pmt_p[2]+_crate_pmt[3]+pmtnb)=-RICHDB::pmt_pos();

        *(_pmt_p[0]+_crate_pmt[5]+pmtnb)=-(xedge+(j+offset[i])*pitch);
        *(_pmt_p[1]+_crate_pmt[5]+pmtnb)=-(yedge+i*pitch);       
        *(_pmt_p[2]+_crate_pmt[5]+pmtnb)=-RICHDB::pmt_pos();

        *(_pmt_p[0]+_crate_pmt[7]+pmtnb)=xedge+(j+offset[i])*pitch;
        *(_pmt_p[1]+_crate_pmt[7]+pmtnb)=-(yedge+i*pitch);
        *(_pmt_p[2]+_crate_pmt[7]+pmtnb)=-RICHDB::pmt_pos();

        pmtnb++;

       }

    }

    for(integer crate=0;crate<_ncrates;crate++)
      for(pmtnb=0;pmtnb<_npmts_crate[crate];pmtnb++)
        _pmt_crate[pmtnb+_crate_pmt[crate]]=crate;
        
    for(pmtnb=0;pmtnb<_npmts;pmtnb++)
      _pmt_orientation[pmtnb]=0;  // To be correctly filled in the future


    RICHDB::total=_npmts;

}




AMSRICHIdGeom::AMSRICHIdGeom(integer channel){
  channel=channel<0?0:channel%(_npmts*RICnwindows);

  _channel=channel;
  _pmt=channel/16;
  _pixel=channel%16;
  _crate=_pmt_crate[_pmt];
  _pmt_in_crate=_pmt-_crate_pmt[_crate];

}

AMSRICHIdGeom::AMSRICHIdGeom(integer pmtnb,geant x,geant y){
  // Better simulation of photocathode effective area


//  x-=AMSRICHIdGeom::pmt_pos(pmtnb,0)-RICcatolength/2;
//  y-=AMSRICHIdGeom::pmt_pos(pmtnb,1)-RICcatolength/2;
//  x/=RICcatolength/sqrt(RICnwindows);
//  y/=RICcatolength/sqrt(RICnwindows);



  x-=AMSRICHIdGeom::pmt_pos(pmtnb,0);
  y-=AMSRICHIdGeom::pmt_pos(pmtnb,1);

  if(fabs(x)>RICcatolength/2 || fabs(y)>RICcatolength/2){_channel=-1;return;}  //error
  if(fabs(x)<RICcatogap/2. || fabs(y)<RICcatogap/2.){_channel=-1;return;}
  if(fabs(fabs(x)-(RICcatogap+cato_inner_pixel))<RICcatogap/2. ||
     fabs(fabs(y)-(RICcatogap+cato_inner_pixel))<RICcatogap/2.) {_channel=-1;return;}

  integer nx,ny;

  if(fabs(x)>RICcatogap+cato_inner_pixel){
    nx=x>0?3:0;
  }else{
    nx=x>0?2:1;
  }
  
  if(fabs(y)>RICcatogap+cato_inner_pixel){
    ny=y>0?3:0;
  }else{
    ny=y>0?2:1;
  }


#ifdef __AMSDEBUG__
  //  cout<<"nx and ny according to new routine "<<nx<<" "<<ny<<endl;
#endif

  integer channel=RICnwindows*pmtnb+integer(sqrt(number(RICnwindows)))*
    ny+nx;
//    integer(y)+integer(x);
  
  _channel=channel;
  _pmt=channel/16;
  _pixel=channel%16;
  _crate=_pmt_crate[_pmt];
  _pmt_in_crate=_pmt-_crate_pmt[_crate];
}



integer AMSRICHIdGeom::get_channel_from_top(geant x,geant y){
  // First, decide in which crate to look for the guy. To do such thing it uses the hole radious
  // Second look for it and decide the PMTnb


  integer crate=-1;

  if(x>RICHDB::hole_radius[0]-RICpmtsupport/2.){
    if(y>RICHDB::hole_radius[1]-RICpmtsupport/2.) crate=1;
    else if(-y>RICHDB::hole_radius[1]-RICpmtsupport/2) crate=7;
    else crate=0;
  } else if(-x>RICHDB::hole_radius[0]-RICpmtsupport/2.){
    if(y>RICHDB::hole_radius[1]-RICpmtsupport/2.) crate=3;
    else if(-y>RICHDB::hole_radius[1]-RICpmtsupport/2) crate=5;
    else crate=4;
  }else{
    if(y>RICHDB::hole_radius[1]-RICpmtsupport/2.) crate=2;
    else if(-y>RICHDB::hole_radius[1]-RICpmtsupport/2) crate=6;
    else return -1;
  }


#ifdef __AMSDEBUG__
  /*
  static int first=1;

  if(first){
    first=0;
    integer crate;

    for(float y=67;y>=-67;y-=2.){
      for(float x=-67;x<=67;x+=2.){
	
		
	if(x>RICHDB::hole_radius[0]-RICpmtsupport/2.){
	  if(y>RICHDB::hole_radius[1]-RICpmtsupport/2.) crate=1;
	  else if(-y>RICHDB::hole_radius[1]-RICpmtsupport/2) crate=7;
	  else crate=0;
	} else if(-x>RICHDB::hole_radius[0]-RICpmtsupport/2.){
	  if(y>RICHDB::hole_radius[1]-RICpmtsupport/2.) crate=3;
	  else if(-y>RICHDB::hole_radius[1]-RICpmtsupport/2) crate=5;
	  else crate=4;
	}else{
	  if(y>RICHDB::hole_radius[1]-RICpmtsupport/2.) crate=2;
	  else if(-y>RICHDB::hole_radius[1]-RICpmtsupport/2) crate=6;
	  else crate=-1;
	}
	
	cout <<crate+1;
	
      }
      cout<<endl;
    }
    }*/
#endif

  
  // Now we now in which crate to look for... doit

  integer pmt_chosen=-1;

  /*
  for(int n=0;n<_npmts;n++){

    if(fabs(x-_pmt_p[0][n])<RICHDB::lg_length/2. &&
       fabs(y-_pmt_p[1][n])<RICHDB::lg_length/2.){
      pmt_chosen=n;
      break;
    }
    }
  */

  for(int n=_crate_pmt[crate];n<_crate_pmt[crate]+_npmts_crate[crate];n++){
    // Check if this guy is within a given pmt light guide group 

    if(fabs(x-_pmt_p[0][n])<RICHDB::lg_length/2. &&
       fabs(y-_pmt_p[1][n])<RICHDB::lg_length/2.){
      pmt_chosen=n;
      break;
    }
  }


#ifdef __AMSDEBUG__
  //  cout<<"    @ get channel: chosen "<<pmt_chosen<<endl;
#endif


  if(pmt_chosen==-1) return -1;

#ifdef __AMSDEBUG__
  //  cout<<"    @ get channel: x,y"<<
  //    _pmt_p[0][pmt_chosen]<<" "<<_pmt_p[1][pmt_chosen]<<endl;
  //  cout <<"   @ get channel: input x,y:"<<
  //    x<<" "<<y<<endl;
#endif


  x=(x-_pmt_p[0][pmt_chosen]+RICHDB::lg_length/2.)*
    4/RICHDB::lg_length;
  y=(y-_pmt_p[1][pmt_chosen]+RICHDB::lg_length/2.)*
    4/RICHDB::lg_length;
  
#ifdef __AMSDEBUG__
  //  cout<<"    @ get channel: nx,ny"<<
  //    int(x)<<" "<<int(y)<<endl;
#endif
  
  //  if(int(y)*integer(sqrt(RICnwindows))+int(x)>15 || 
  //       int(y)*integer(sqrt(RICnwindows))+int(x)<0) {cout <<"ERRORRRRRR"<<endl;exit(1);}

  return int(y)*integer(sqrt(number(RICnwindows)))+int(x);
  

  


}





// The rotation is not implemented yet

geant AMSRICHIdGeom::x(){

// WARNING This to be mapped to the new setup

  geant x=(2*(_pixel%integer(sqrt(number(RICnwindows))))-3)*
    RICHDB::lg_length/8.+*(_pmt_p[0]+_pmt);

#ifdef __AMSDEBUG__
  /*  if(x>1e6){
    cout<<endl<<"--------"<<endl
	<<"PMT position in x "<<*(_pmt_p[0]+_pmt)<<endl
	<<"Factor "<<2*(_pixel%integer(sqrt(number(RICnwindows))))-3<<endl
	<<"--------"<<endl;
	}*/
#endif

  return x;
}

geant AMSRICHIdGeom::y(){
  geant y=(2*(_pixel/integer(sqrt(number(RICnwindows))))-3)*
    RICHDB::lg_length/8.+*(_pmt_p[1]+_pmt);

  return y;
}


geant AMSRICHIdGeom::z(){
  geant z=RICHDB::RICradpos()+*(_pmt_p[2]+_pmt)+RICHDB::pmtb_height()/2.;

  return z;
}




///////////////////////////////////////////////////////////


uinteger AMSRICHIdSoft::_nchannels=0;
geant * AMSRICHIdSoft::_ped=0;
geant *AMSRICHIdSoft::_sig_ped=0;

geant *AMSRICHIdSoft::_lambda=0;
geant *AMSRICHIdSoft::_scale=0;
geant *AMSRICHIdSoft::_transparency=0;
geant *AMSRICHIdSoft::_lambda_dyn=0;
geant *AMSRICHIdSoft::_scale_dyn=0;


geant *AMSRICHIdSoft::_threshold=0;
integer *AMSRICHIdSoft::_gain_mode_boundary=0;
integer *AMSRICHIdSoft::_status=0;

uinteger AMSRICHIdSoft::_nbins=1000;  // default value
geant *AMSRICHIdSoft::_cumulative_prob=0;
geant *AMSRICHIdSoft::_step=0;

void AMSRICHIdSoft::Init(){
  _nchannels=AMSRICHIdGeom::getpmtnb()*16;  // Assume 16 channels per PMT
  _ped=new geant[2*_nchannels];
  _sig_ped=new geant[2*_nchannels];

  _lambda=new geant[2*_nchannels];
  _scale=new geant[2*_nchannels];
  _transparency=new geant[2*_nchannels];
  _lambda_dyn=new geant[2*_nchannels];
  _scale_dyn=new geant[2*_nchannels];

  _threshold=new geant[2*_nchannels];
  _gain_mode_boundary=new integer[_nchannels];
  _status=new integer[_nchannels];

  _cumulative_prob=new geant[2*_nchannels*_nbins];
  _step=new geant[2*_nchannels];

  // Check

  assert(_ped && _sig_ped && _lambda && _scale && _transparency && 
         _lambda_dyn && _scale_dyn && _threshold && _gain_mode_boundary &&
         _status && _cumulative_prob);


  // Here we should fill all the values

  if(RICFFKEY.ReadFile){
    char name[80];

    strcpy(name,"richcal");

    if(AMSJob::gethead()->isMCData()){ // McData 
      strcat(name,"mc");
    }else{  // Real data
      strcat(name,"dt");
    }


    strcat(name,".001");  // Version


    cout<<"AMSRICHIdSoft::Init: Loading default calibration ..."<<endl;
    fstream calib(name,ios::in); // open  file for reading

    if(calib){
      cout<<"AMSRICHIdSoft::Init: Local "<<name<<" Opened"<<endl;
    }else{
      char newname[80];
      strcpy(newname,AMSDATADIR.amsdatadir);
      strcat(newname,name);
      strcpy(name,newname);
      calib.open(newname,ios::in);
      if(calib){
	cout<<"AMSRICHIdSoft::Init: "<<name<<" Opened"<<endl;
      }else{
	cerr <<"AMSRICHIdSoft::Init:: missing "<<name<<endl;
	exit(1);
      }

    }


    for(int i=0;i<_nchannels;i++){
      for(int hl=0;hl<2;hl++){
	calib >> _ped[2*i+hl];
	calib >> _sig_ped[2*i+hl];
	calib >> _threshold[2*i+hl];
	calib >> _lambda[2*i+hl];
	calib >> _scale[2*i+hl];
	calib >> _transparency[2*i+hl];
	calib >> _lambda_dyn[2*i+hl];
	calib >> _scale_dyn[2*i+hl];
      }
      
      calib >> _gain_mode_boundary[i];
      calib >> _status[i];
      
    }
    
    calib.close();


#ifdef __AMSDEBUG__
    /*    for(int i=0;i<_nchannels;i++){
      for(int hl=0;hl<2;hl++){
	
	cout <<"Channel "<<i<<" mode "<<(hl==0?"low":"high")<<
	  _ped[2*i+hl]<<" "<<_sig_ped[2*i+hl]<<" "<<_threshold[2*i+hl]<<" "<<
	  _lambda[2*i+hl]<<" "<<_scale[2*i+hl]<<" "<<_transparency[2*i+hl]<<" "<<
	  _lambda_dyn[2*i+hl]<<" "<<_scale_dyn[2*i+hl]<<endl;
      }
      cout <<"   --> "<< _gain_mode_boundary[i]<<" "<<_status[i]<<endl;

      }*/
#endif    


    
    
  }else{

    cout<<"AMSRICHIdSoft::Init: Using nominal calibration."<<endl;
    for(int i=0;i<_nchannels;i++){
      for(int hl=0;hl<2;hl++){
	// OLD PMT simulation 
	//        _ped[2*i+hl]=0.;
	//        _sig_ped[2*i+hl]=4.;
	//        _threshold[2*i+hl]=4.;
	//        _lambda[2*i+hl]=1.23;
	//        _scale[2*i+hl]=30./(5.-4.*hl);;
	//        _transparency[2*i+hl]=0.;
	//        _lambda_dyn[2*i+hl]=0.;
	//        _scale_dyn[2*i+hl]=0.;

	// New PMT simulation
        _ped[2*i+hl]=0.;
        _sig_ped[2*i+hl]=4.;
        _threshold[2*i+hl]=4.;
        _lambda[2*i+hl]=4.92;
        _scale[2*i+hl]=12.25/(5.-4.*hl);;
        _transparency[2*i+hl]=0.;
        _lambda_dyn[2*i+hl]=0.;
        _scale_dyn[2*i+hl]=0.;

      }
      
      _gain_mode_boundary[i]=3000;
      _status[i]=1;
      
    }
    


  }



  // Fill the probability function for each channel
  fill_probability();

}


void AMSRICHIdSoft::fill_probability(){
  if(RICFFKEY.ReadFile){
    for(int channel=0;channel<_nchannels;channel++)
      for(int mode=0;mode<2;mode++){
	geant gain,sigma;
	geant lambda,scale;
	
	lambda=_lambda[2*channel+mode];
	scale=_scale[2*channel+mode];	
	GETRMURSG(lambda,scale,gain,sigma);
	
	geant upper_limit=gain*10;
	_step[2*channel+mode]=upper_limit/_nbins;
	
	// Use the trapezoid rule to compute the integral
	_cumulative_prob[2*channel*_nbins+mode*_nbins]=0;
	for(int i=1;i<_nbins;i++){
	  float value,x1,x2;
	  
	  x1=i*_step[2*channel+mode];
	  x2=(i-1)*_step[2*channel+mode];
	  value=PDENS(x1,lambda,scale)+PDENS(x2,lambda,scale);
	  value*=_step[2*channel+mode]/2;
	  _cumulative_prob[2*channel*_nbins+mode*_nbins+i]=_cumulative_prob[2*channel*_nbins+mode*_nbins+i-1]+value;
	}

	for(int i=0;i<_nbins;i++)
	  _cumulative_prob[2*channel*_nbins+mode*_nbins+i]/=
	    _cumulative_prob[2*channel*_nbins+mode*_nbins+_nbins-1];

      }
  }
  else{
    // Nominal calibration


      for(int mode=0;mode<2;mode++){
	int channel=0;
	geant gain,sigma;
	geant lambda,scale;
	
	lambda=_lambda[2*channel+mode];
	scale=_scale[2*channel+mode];	
	GETRMURSG(lambda,scale,gain,sigma);
	
	geant upper_limit=gain*10;
	_step[2*channel+mode]=upper_limit/_nbins;
	for(int chan=1;chan<_nchannels;chan++) _step[2*chan+mode]=_step[2*channel+mode];

	// Use the trapezoid rule to compute the integral
	_cumulative_prob[2*channel*_nbins+mode*_nbins]=0;
	for(int i=1;i<_nbins;i++){
	  float value,x1,x2;
	  
	  x1=i*_step[2*channel+mode];
	  x2=(i-1)*_step[2*channel+mode];
	  value=PDENS(x1,lambda,scale)+PDENS(x2,lambda,scale);
	  value*=_step[2*channel+mode]/2;
	  _cumulative_prob[2*channel*_nbins+mode*_nbins+i]=_cumulative_prob[2*channel*_nbins+mode*_nbins+i-1]+value;
	}
	
	for(int i=0;i<_nbins;i++){
	  _cumulative_prob[2*channel*_nbins+mode*_nbins+i]/=
	    _cumulative_prob[2*channel*_nbins+mode*_nbins+_nbins-1];
	  for(int chan=1;chan<_nchannels;chan++)
	    _cumulative_prob[2*chan*_nbins+mode*_nbins+i]=_cumulative_prob[2*channel*_nbins+mode*_nbins+i];
	}
	
      }
  }
  
}



geant AMSRICHIdSoft::simulate_single_pe(int mode){
  // Sample from the _cumulative_prob for _address and mode
  geant dummy=0;
  geant value=RNDM(dummy);
  for(int i=0;i<_nbins;i++){
    if(value<=_cumulative_prob[2*_nbins*_address+(mode<=0?0:1)*_nbins+i])
      return i*_step[2*_address+(mode<=0?0:1)];
  }
}

