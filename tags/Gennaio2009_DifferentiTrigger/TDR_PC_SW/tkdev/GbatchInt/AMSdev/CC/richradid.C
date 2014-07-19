#include "richradid.h"

/////////////////////////////////////////////////

integer RichRadiatorTileManager::_number_of_rad_tiles=0;
RichRadiatorTile **RichRadiatorTileManager::_tiles=0;


void RichRadiatorTileManager::Init(){  // Default initialization
  if(_number_of_rad_tiles!=0) return; // Not necessary

  cout<<"RichRadiatorTileManager::Init-default radiator"<<endl;
  Init_Default();
}

void RichRadiatorTileManager::Init_Default(){  // Default initialization
  if(RICRADSETUPFFKEY.setup!=1){
    cout<<"RichRadiatorTileManager::Init_Default RICRADSETUPFFKEY.setup!=0 is not longer supported -- forcing default RICRADSETUPFFKEY.setup=1"<<endl;
    RICRADSETUPFFKEY.setup=1;
  }

  const int agl_boxes_number=11;
  int agl_boxes[agl_boxes_number][agl_boxes_number]={
    0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
    0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0
  };

  const int naf_boxes_number=4;
  int naf_boxes[naf_boxes_number][naf_boxes_number]={
    1,1,1,1,
    1,1,1,1,
    1,1,1,1,
    1,1,1,1
  };


  // Count the number of tiles
  _number_of_rad_tiles=0;
  for(int i=0;i<agl_boxes_number;i++)
    for(int j=0;j<agl_boxes_number;j++){
      if(agl_boxes[i][j]==1) _number_of_rad_tiles++;
    }

  for(int i=0;i<naf_boxes_number;i++)
    for(int j=0;j<naf_boxes_number;j++){
      if(naf_boxes[i][j]==1) _number_of_rad_tiles++;
    }
 
  cout<<"   RichRadiatorTileManager::Init_Default-number of radiator tiles: "<<_number_of_rad_tiles<<endl;

  _tiles=new RichRadiatorTile*[_number_of_rad_tiles];

  RICHDB::mat_init();  // Ensure that the agl tables are updated.

  // Fill the information for agl
  int current=0;
  for(int i=0;i<agl_boxes_number;i++)
    for(int j=0;j<agl_boxes_number;j++){
      if(agl_boxes[i][j]!=1) continue;
      double x=(i+0.5*(1-agl_boxes_number))*(RICHDB::rad_length+RICaethk);
      double y=(j+0.5*(1-agl_boxes_number))*(RICHDB::rad_length+RICaethk);
      _tiles[current]=new RichRadiatorTile;
      _tiles[current]->position[0]=x;
      _tiles[current]->position[1]=y;
      _tiles[current]->bounding_box[0][0]=-RICHDB::rad_length/2;
      _tiles[current]->bounding_box[0][1]=RICHDB::rad_length/2;
      _tiles[current]->bounding_box[1][0]=-RICHDB::rad_length/2;
      _tiles[current]->bounding_box[1][1]=RICHDB::rad_length/2;
      _tiles[current]->bounding_box[2][0]=-RICHDB::rad_agl_height/2;
      _tiles[current]->bounding_box[2][1]=RICHDB::rad_agl_height/2;
      _tiles[current]->kind=agl_kind;
      _tiles[current]->id=current;
      _tiles[current]->clarity=RICHDB::eff_rad_clarity;
      _tiles[current]->effective_scattering_probability=0;
      _tiles[current]->effective_scattering_angle=0;
      _tiles[current]->abs_length=RICHDB::abs_length;
      //      _tiles[current]->index_table=RICHDB::index;
      _tiles[current]->index=RICHDB::rad_index;

      _tiles[current]->index_table=new geant[RICmaxentries];
      for(int ii=0;ii<RICmaxentries;ii++)
	_tiles[current]->index_table[ii]=1+(_tiles[current]->index-1)/(RICHDB::rad_index-1)*(RICHDB::index[ii]-1);
      current++;
    }

  // Now we compute the effective height and effective index calling the dedicated routine
  geant eff_index;
  geant eff_height;
  _compute_mean_height(_tiles[current-1]->index_table,
		       _tiles[current-1]->clarity,
		       _tiles[current-1]->abs_length,
		       _tiles[current-1]->bounding_box[2][1]-_tiles[current-1]->bounding_box[2][0],
		       eff_index,
		       eff_height);

  for(int i=0;i<current;i++){
    if(_tiles[i]->kind!=agl_kind) continue;
    _tiles[i]->mean_refractive_index=eff_index;
    _tiles[i]->mean_height=eff_height-0.1; // Corretion by hand
  }
  
  for(int i=0;i<naf_boxes_number;i++)
    for(int j=0;j<naf_boxes_number;j++){
      if(naf_boxes[i][j]!=1) continue;
      double x=(i+0.5*(1-naf_boxes_number))*(RICHDB::naf_length+RICaethk);
      double y=(j+0.5*(1-naf_boxes_number))*(RICHDB::naf_length+RICaethk);
      _tiles[current]=new RichRadiatorTile;
      _tiles[current]->position[0]=x;
      _tiles[current]->position[1]=y;
      _tiles[current]->bounding_box[0][0]=-RICHDB::naf_length/2;
      _tiles[current]->bounding_box[0][1]=RICHDB::naf_length/2;
      _tiles[current]->bounding_box[1][0]=-RICHDB::naf_length/2;
      _tiles[current]->bounding_box[1][1]=RICHDB::naf_length/2;
      _tiles[current]->bounding_box[2][0]=-RICHDB::naf_height/2;
      _tiles[current]->bounding_box[2][1]=RICHDB::naf_height/2;
      _tiles[current]->kind=naf_kind;
      _tiles[current]->id=current;
      _tiles[current]->clarity=0;
      _tiles[current]->effective_scattering_probability=0;
      _tiles[current]->effective_scattering_angle=0;
      _tiles[current]->abs_length=RICHDB::naf_abs_length;
      //      _tiles[current]->index_table=RICHDB::naf_index_table;
      _tiles[current]->index=RICHDB::naf_index;
      
      _tiles[current]->index_table=new geant[RICmaxentries];
      for(int ii=0;ii<RICmaxentries;ii++)
	_tiles[current]->index_table[ii]=1+(_tiles[current]->index-1)/(RICHDB::naf_index-1)*(RICHDB::naf_index_table[ii]-1);
      current++;
    }

  _compute_mean_height(_tiles[current-1]->index_table,
		       _tiles[current-1]->clarity,
		       _tiles[current-1]->abs_length,
		       _tiles[current-1]->bounding_box[2][1]-_tiles[current-1]->bounding_box[2][0],
		       eff_index,
		       eff_height);
  
  for(int i=0;i<_number_of_rad_tiles;i++){
    if(_tiles[i]->kind!=naf_kind) continue;
    _tiles[i]->mean_refractive_index=eff_index;
    _tiles[i]->mean_height=eff_height-0.2; // Corretion by hand
  }
}

#define max(x,y) ((x)<(y)?(y):(x))
#define min(x,y) ((x)<(y)?(x):(y))



void RichRadiatorTileManager::Finish(){
  //
  // Decide how to finish as a function of the kind of job
  //

  Finish_Default();
}


void RichRadiatorTileManager::Finish_Default(){
  cout<<" RichTileManager finishing"<<endl;
}

integer RichRadiatorTileManager::get_tile_number(geant x,geant y){
  int closer=-1;
  double dist=1e6;
  for(int i=0;i<_number_of_rad_tiles;i++){
    double dx=x-_tiles[i]->position[0];
    double dy=y-_tiles[i]->position[1];

    if(dx<max(_tiles[i]->bounding_box[0][0],_tiles[i]->bounding_box[0][1]) &&
       dx>min(_tiles[i]->bounding_box[0][0],_tiles[i]->bounding_box[0][1]) &&
       dy<max(_tiles[i]->bounding_box[1][0],_tiles[i]->bounding_box[1][1]) &&
       dy>min(_tiles[i]->bounding_box[1][0],_tiles[i]->bounding_box[1][1])){
      return i;
    }
  }

  return -1;
}



RichRadiatorTileManager::RichRadiatorTileManager(AMSTrTrack *track){
  if(_number_of_rad_tiles==0){
    cerr<<"RichRadiatorTileManager::RichRadiatorTileManager -- tiles not initialized -- doing it"<<endl;
    Init();
  }

  // First decide wich kind of radiator is current
  AMSPoint pnt(0.,0.,RICHDB::RICradpos()-RICHDB::rad_height),
    point;
  AMSDir dir(0.,0.,-1.);
  
  number theta,phi,length;

  track->interpolate(pnt,dir,point,
		     theta,phi,length);

  _current_tile=get_tile_number(point[0],point[1]);
  
  if(_current_tile<0){
    _p_direct=AMSPoint(0.,0.,0.);
    _p_reflected=AMSPoint(0.,0.,0.);
    _d_direct=AMSDir(0.,0.);
    _d_reflected=AMSDir(0.,0.);
    return;
  }

  pnt.setp(0.,0.,RICHDB::RICradpos()-RICHDB::rad_height+getheight());
  track->interpolate(pnt,dir,point,
		     theta,phi,
		     length);
  
  if(getkind()!=get_tile_kind(get_tile_number(point[0],point[1]))){
    _current_tile=-1;
    _p_direct=AMSPoint(0.,0.,0.);
    _p_reflected=AMSPoint(0.,0.,0.);
    _d_direct=AMSDir(0.,0.);
    _d_reflected=AMSDir(0.,0.);
    return;
  }
  
  _p_entrance=point;
  _d_entrance=AMSDir(theta,phi);


  // Direct photons
  pnt.setp(0.,0.,RICHDB::RICradpos()-RICHDB::rad_height+_tiles[_current_tile]->mean_height);
  track->interpolate(pnt,dir,point,theta,phi,length);
  
  _p_direct=point;
  _d_direct=AMSDir(theta,phi);
  
  // Reflected photons
  pnt.setp(0.,0.,RICHDB::RICradpos()-RICHDB::rad_height+_tiles[_current_tile]->mean_height);
  track->interpolate(pnt,dir,point,theta,phi,length);
  
  _p_reflected=point;
  _d_reflected=AMSDir(theta,phi);

  // Use the mean position for the direct photons to estimate the
  // local mean index
  double dx=_p_direct[0]-_tiles[_current_tile]->position[0];
  double dy=_p_direct[1]-_tiles[_current_tile]->position[1];

  _local_index=1+(_tiles[_current_tile]->mean_refractive_index-1)*
    (_tiles[_current_tile]->LocalIndex(dx,dy)-1)/
    (_tiles[_current_tile]->index-1);
} 




void RichRadiatorTileManager::_compute_mean_height(geant *index,
						   geant clarity,
						   geant *abs_len,
						   geant rheight,
						   geant &eff_index,
						   geant &height){
  // Computes the mean emission point inside the radiator
  // of the detected photons.
  // The credits go to .... Elisa Lanciotti
  const integer steps=100;    // Number of steps for the approximation
  geant lambda,qeff,n,dl,l_scat=0,l_abs_rad,l_abs_lg;
  geant sum=0,densum=0;
  geant sum_index=0;

  for(integer i=0;i<RICmaxentries-1;i++){ // Integration in wave length
    lambda=(RICHDB::wave_length[i]+RICHDB::wave_length[i+1])/2.;
    qeff=(RICHDB::eff[i]+RICHDB::eff[i+1])/2.;
    n=(index[i]+index[i+1])/2.;
    dl=RICHDB::wave_length[i]-RICHDB::wave_length[i+1];
    l_scat=clarity==0?1e6:(lambda/1000.)*(lambda/1000.)*(lambda/1000.)*(lambda/1000.)/
	clarity;
    l_abs_rad=(abs_len[i]+abs_len[i+1])/2.;
    l_abs_lg=(RICHDB::lg_abs[i]+RICHDB::lg_abs[i+1])/2.;


    for(integer j=0;j<steps;j++){ // Integration in radiador thicknes
      geant x=rheight*(geant(j)+0.5)/geant(steps);
      geant g=qeff/lambda/lambda/exp((rheight-x)*
				     (1/l_scat+1/l_abs_rad))/
	exp(RICHDB::lg_height/l_abs_lg);
      sum+=dl*g*x;
      densum+=dl*g;
      sum_index+=dl*g*n;
    }
  }
  if(!densum){
    cout<<"RichRadiatorTileManager::_mean_height : Error"<<endl;
  }else{
    height=rheight-sum/densum;

    eff_index=sum_index/densum;


  }
} 

geant RichRadiatorTileManager::get_refractive_index(geant x,geant y,geant wavelength){
  int tile_number=get_tile_number(x,y);
  if(tile_number<0){
    // From time to time it gets a 0 due to the PORON. In that
    // case we should continue the photon propagation
    // This can be checked by ploting the photons producing this problem
    //    cout<<">><< "<<x<<" "<<y<<endl;
    //    cout<<"-- WARNING: RichRadiatorTileManager::get_refractive_index: out of tile"<<endl;
        return 0.0;  
  }
  
  // For using the future parameterization
  double dx=x-_tiles[tile_number]->position[0];
  double dy=y-_tiles[tile_number]->position[1];
  

  // Get the wavelength bin using binary search
  int wl_bin=RICHDB::get_wavelength_bin(wavelength);

  // Use a linear interpolation to get the refractive index
  double ratio=(wavelength-RICHDB::wave_length[wl_bin])/(RICHDB::wave_length[wl_bin+1]-RICHDB::wave_length[wl_bin]);
  geant index=ratio*_tiles[tile_number]->index_table[wl_bin+1]+(1-ratio)*_tiles[tile_number]->index_table[wl_bin];

  // Correction taking into account the local refractive index
  geant local_index=_tiles[tile_number]->LocalIndex(dx,dy);
  geant final_index=1+(index-1)*(local_index-1)/(_tiles[tile_number]->index-1);

  // Return the computed index
  return final_index;
}



//
// Here we define a function to get the refractive index with a fortran call giving the position
//

extern "C" geant rrtmindex_(geant *x,geant *y,geant *wvl){
  return  RichRadiatorTileManager::get_refractive_index(*x,*y,*wvl);
}


//
// Get the number of Cherenkov photons. Prepare integration table
//

double _cherenkov_integral[RICmaxentries];
double _cherenkov_index[RICmaxentries];
int _cherenkov_border;

extern "C" geant getphotons_(geant *charge,geant *min_index,geant *vect,geant *step){
  const geant rfact=369.81E9;
  const geant cvpwl=1.2398E-6;  //m*eV=nm*GeV

  if(*charge==0) return 0;  // Simple test

  //
  // Get the refractive index in the xtremes
  //

  double mean=0;
  double dp=0;
  double ge=0;
  _cherenkov_integral[0]=0;
  _cherenkov_border=1;
  for(int i=1;i<RICmaxentries;i++){
    //
    // Get the momentum range
    //
    geant initial_momentum=cvpwl/(RICHDB::wave_length[i-1]);
    geant final_momentum=cvpwl/(RICHDB::wave_length[i]);

    //
    // Get the mean wavelength
    //
    double wavelength=0.5*(RICHDB::wave_length[i-1]+RICHDB::wave_length[i]);

    //
    // Get the refractive indexes in the middle of the step
    //
    geant index=RichRadiatorTileManager::get_refractive_index(vect[0]-0.5*(*step)*vect[3],vect[1]-0.5*(*step)*vect[4],wavelength);

    if(index==0.0) return 0;

    //
    // Store the current best limit
    // 
    
    _cherenkov_border=i+1;
    if(index<*min_index) break;

    
    //
    // Compute the first integral part
    //

    _cherenkov_integral[i]=_cherenkov_integral[i-1]+(final_momentum-initial_momentum)*
      (1-(*min_index)*(*min_index)/(index*index));

    mean=_cherenkov_integral[i];
  }

  if(mean==0) return 0; // If no photons, skip further computation

  //
  // Fill tables to sample from them
  //
  for(int i=0;i<RICmaxentries;i++){
    //
    // Prepare table of integrals
    //
    _cherenkov_integral[i]/=mean;

    //
    // Prepare table of indexes
    //
    double wavelength=RICHDB::wave_length[i];
    _cherenkov_index[i]=RichRadiatorTileManager::get_refractive_index(vect[0]-0.5*(*step)*vect[3],vect[1]-0.5*(*step)*vect[4],wavelength);
  }

  mean*=(*charge)*(*charge)*rfact*(*step); 
  return mean;
}


//
// Get the photon momentum and corresponding refractive index
//
extern "C" geant getmomentum_(geant *index){
  const geant cvpwl=1.2398E-6;  //m*eV=nm*GeV
  int dummy; 
  geant integral=RNDM(dummy);
  
  //
  // Use binary search to find the photon
  int bin_max=_cherenkov_border-1;
  int bin_min=0;

  for(;;){
    int bin_med=(bin_max+bin_min)/2;
    if(integral<_cherenkov_integral[bin_med]) bin_max=bin_med; else bin_min=bin_med;
    if(bin_max-bin_min<=1) break;
  }

  //
  // Get the linear interpolation wavelength
  //
  geant ratio=(integral-_cherenkov_integral[bin_min])/
    (_cherenkov_integral[bin_max]-_cherenkov_integral[bin_min]);
  geant wl=RICHDB::wave_length[bin_min]*(1-ratio)+RICHDB::wave_length[bin_max]*ratio;

  geant momentum=cvpwl/wl;
  
  //
  // Get the linear interpolation index
  // 
  *index=_cherenkov_index[bin_min]*(1-ratio)+_cherenkov_index[bin_max]*ratio;

  // This a test to look for problems
  if(*index<1 || *index>2){
    cerr<<"BIG PROBLEM AT getphoton_@richradid.C "<<bin_min<<" "<<bin_max<<" "<<ratio<<endl;
    cerr<<endl<<"DUMPING TABLE "<<endl;
    for(int i=0;i<_cherenkov_border-1;i++)
      cerr<<"WV "<<RICHDB::wave_length[i]<<" INDEX "<<_cherenkov_index[i]<<" INTEGRAL "<<_cherenkov_integral[i]<<endl;
    exit(1);
  }

  return momentum;
}
