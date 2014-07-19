//  $Id: amsgeom.C,v 1.2 2008/11/06 19:29:51 zuccon Exp $
// Author V. Choutko 24-may-1996
// TOF Geometry E. Choumilov 22-jul-1996 
// ANTI Geometry E. Choumilov 2-06-1997 
// ECAL 1.0-version Geometry E. Choumilov 15-09-1999 
// Passive Shield Geometry V. Choutko 22-jul-1996
// CTC (Cherenkov Thresh. Counter) geometry E.Choumilov 26-sep-1996
// ATC (Aerogel Threshold Cerenkov) geometry A.K.Gougas 14-Mar-1997
// AMS02 MAGNET geometry v1.0  E.Choumilov 17.11.2000
// AMS02 Radiators/crates/TRD-TOF_supports/USS geometry v1.0  E.Choumilov 4.06.2001
// New ANTI design (AMS-02). E.Choumilov 24.03.03.
// 
//
#include "typedefs.h"
#include "node.h"
#include "snode.h"
#include "amsdbc.h"
#include "gmat.h"
//#include "extC.h"
#include <stdlib.h>
#include "gvolume.h"
#include "amsgobj.h"
#include "AMSJob.h"
#include "commons.h"
#include <strstream>
#include "TrField.h"

namespace amsgeom{
  extern void tkgeom02(AMSgvolume &);
  extern void magnetgeom(AMSgvolume &);
  extern void magnetgeom02(AMSgvolume &);
  extern void ext1structure02(AMSgvolume &);
  extern void magnetgeom02o(AMSgvolume &);
  extern void magnetgeom02Test(AMSgvolume &);
  extern void ext2structure(AMSgvolume &);


  extern void Put_rad(AMSgvolume *,integer);
  extern void Put_pmt(AMSgvolume *,integer);
};
using namespace amsgeom;
//--------------------------------------------------
void AMSgvolume::amsgeom(){
  AMSID amsid;
  geant par[11];
  geant parf[10];
  geant coo[3]={0,0,0};
  number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
  integer gid=1;
  number amss[3];
  
  for(int i=0;i<3;i++)amss[i]=AMSDBc::ams_size[i]*sqrt(2.);

  //
  parf[0]=amss[0]/2;
  parf[1]=amss[1]/2;
  parf[2]=amss[2]/2;
  par[0]=AMSDBc::ams_size[0]/2;
  par[1]=AMSDBc::ams_size[1]/2;
  par[2]=AMSDBc::ams_size[2]/2;
  // to be able rotate ams as a whole
  //
  //----> create mother volumes:
  //
  static AMSgvolume false_mother("VACUUM",0,AMSDBc::ams_name,"BOX ",parf,3,coo,nrm,"ONLY",0,gid,1);  // AMS false mother volume
  char optn[5];
  
  
  strcpy(optn,"ONLY");

  
  
  static AMSgvolume mother("VACUUM",AMSDBc::ams_rotmno,"FMOT","BOX",par,3,AMSDBc::ams_coo,nrm,optn,0,gid,1);  // AMS mother volume
  
  AMSJob::gethead()->addup( &false_mother);
  
  false_mother.add(&mother);
  
  //
  //----> add solar pannels and part of ISS-base around AMS for "BIG" setup:
  //
  
  cout <<" AMSGeom-I-AMS02 setup selected."<<endl;
  magnetgeom02(mother);
  
  ext1structure02(mother);//should be called after tofgeom02 !!!
  tkgeom02(mother);
  ext2structure(mother);

  

  false_mother.MakeG3Volumes();
  
  cout << "AMSGeom::-I-"<<getNpv()<<" Physical volumes, "<<getNlv()<<" logical volumes and "<<getNrm()<<" rotation matrixes have been created "<<endl;
  // Check recursion
  AMSNodeMap geommap;
  geommap.map(false_mother);
  int abort=0;
  int i;
  for( i=0;;i++){
    AMSgvolume* p1=(AMSgvolume*)geommap.getid(i);
    AMSgvolume* p2=(AMSgvolume*)geommap.getid(i+1);
    if(!p1 || !p2)break;
    if(!strcmp(p1->getname(), p2->getname()) && p1->VolumeHasG3Attributes() &&
       p2->VolumeHasG3Attributes()){
      if(p1->down() && p2->down()){
	cerr<<" AMSGeom-F-Geant3RecursionProblemDetectedForVolumes "<<
	  p1->getname()<<" "<<p1->getid()<<" , "<<p2->getname()<<" "<<p2->getid()<<endl;
	abort=1;
      }
      else if(p1->down() || p2->down()){
	//   cerr<<" AMSGeom-W-PossibleGeant3RecursionProblemDetectedForVolumes "<<
	//   p1->getname()<<" "<<p1->getid()<<" , "<<p2->getname()<<" "<<p2->getid()<<endl;
      }
    }
  }
  if(abort){
    cerr<<"AMSGeom-F-Geant3VolumesWithDaugthersMustHaveDifferentNames"<<endl;
    exit(1);
  }
  GGCLOS();
  cout <<" AMSGeom-I-TotalOf "<<GlobalRotMatrixNo()<<" rotation matrixes  created"<<endl;


}

//--------------------------------------------------------------

void amsgeom::magnetgeom(AMSgvolume & mother){
  AMSID amsid;
  geant par[6]={0.,0.,0.,0.,0.,0.};
  char name[5]="MAGN";
  geant coo[3]={0.,0.,0.};
  number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
  integer gid=0;
  AMSNode * cur;
  AMSNode * dau;
  AMSgtmed *p;
  gid=1;
  par[0]=112./2;
  par[1]=129.6/2;
  par[2]=40.;
  dau=mother.add(new AMSgvolume(
				"MAGNET",0,name,"TUBE",par,3,coo,
				nrm, "ONLY",0,gid,1));
  gid=2;
  par[0]=111.4/2.;
  par[1]=112.0/2.;
  par[2]=40.;
  dau=mother.add(new AMSgvolume(
				"1/2ALUM",0,"ALT1","TUBE",par,3,coo,nrm, "ONLY",0,gid,1));
  gid=3;
  par[0]=129.6/2.;
  par[1]=130.4/2.;
  par[2]=40.;
  dau=mother.add(new AMSgvolume(
				"1/2ALUM",0,"ALT2","TUBE",par,3,coo,nrm, "ONLY",0,gid++,1));
  par[0]=112/2.;
  par[1]=165./2.;
  par[2]=0.75;
  coo[2]=40.75;
  dau=mother.add(new AMSgvolume(
				"1/2ALUM",0,"ALT3","TUBE",par,3,coo,nrm, "ONLY",0,gid++,1));
    
  par[0]=112/2.;
  par[1]=165.6/2.;
  par[2]=0.75;
  coo[2]=-40.75;
  dau=mother.add(new AMSgvolume(
				"1/2ALUM",0,"ALT4","TUBE",par,3,coo,nrm, "ONLY",0,gid++,1));
    
}
//-------------------------------------------------------------------
// AMS02d  /long /+86 cm/  perm magnet  setup:
//
//-------------------------------------------------------------------
// for  final(trapezoidal TOF) AMS02 setup:
//



//
void amsgeom::magnetgeom02o(AMSgvolume & mother){
  // old (a la AMS01) design.
  AMSID amsid;
  geant par[6]={0.,0.,0.,0.,0.,0.};
  char name[5]="MAGN";
  geant coo[3]={0.,0.,0.};
  number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
  integer gid=0;
  AMSNode * cur;
  AMSgvolume * dau;
  AMSgtmed *p;
  geant magnetl=86.+86.;
  gid=1;
  par[0]=113.2/2;
  par[1]=129.6/2;
  par[2]=magnetl/2.;
  dau=(AMSgvolume*)mother.add(new AMSgvolume(
					     "MAGNET",0,name,"TUBE",par,3,coo,
					     nrm, "ONLY",0,gid,1));
  gid=2;
  coo[0]=0;
  coo[1]=0;
  coo[2]=0;
  par[0]=111.5/2.;
  par[1]=112.0/2.;
  par[2]=magnetl/2.;
  mother.add(new AMSgvolume(
			    "1/2ALUM",0,"ALT1","TUBE",par,3,coo,nrm, "ONLY",0,gid,1));


  /*
    gid=3;
    par[0]=129.6/2.;
    par[1]=130.4/2.;
    par[2]=magnetl/2.;
    dau=mother.add(new AMSgvolume(
    "1/2ALUM",0,"ALT2","TUBE",par,3,coo,nrm, "ONLY",0,gid++,1));
    par[0]=112/2.;
    par[1]=165./2.;
    par[2]=0.75;
    coo[2]=magnetl/2.+par[2];
    dau=mother.add(new AMSgvolume(
    "1/2ALUM",0,"ALT3","TUBE",par,3,coo,nrm, "ONLY",0,gid++,1));
    
    par[0]=112/2.;
    par[1]=165.6/2.;
    par[2]=0.75;
    coo[2]=-magnetl/2.-par[2];
    dau=mother.add(new AMSgvolume(
    "1/2ALUM",0,"ALT4","TUBE",par,3,coo,nrm, "ONLY",0,gid++,1));
  */
    
}
//------------------------------------------------
void amsgeom::magnetgeom02(AMSgvolume & mother){
  // "real" AMS02 design, but the shape of coils/he-vessel is rectang. 
  // with "the same weight(cross-section area)" dimensions.
  AMSID amsid;
  geant par[15]={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
  geant coo[3]={0.,0.,0.};
  number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
  int ii,kk;
  char inum[11];
  char in[2]="0";
  char vname[5];
  integer gid;
  AMSNode * mmoth ;
  AMSNode * mivol;
  AMSNode * coil;
  AMSNode * lhves;
  AMSNode * cur;
  geant casl1=40.;     //vac.case half-length at inner radious
  geant casl2;//      .......................... outer .......
  geant casl1i,casl2i; // same for its internal surface
  geant tancon,coscon;
  geant conang=27.;    //flange angl(degr,wrt horizont)
  geant casr1=55.75;    //vac.case inner radious
  geant casr2=134.;    //vac.case outer radious
  geant cylit=0.3;     //inner cyl. thickness
  geant cylot=0.2;     //outer cyl. thickness
  geant flant=0.2;     //flange thickness
  //
  strcpy(inum,"0123456789");
  //
  tancon=tan(conang/AMSDBc::raddeg);
  coscon=cos(conang/AMSDBc::raddeg);
  casl2=casl1+(casr2-casr1)*tancon;
  casl1i=casl1+cylit*tancon-flant/coscon;
  casl2i=casl1i+(casr2-casr1-cylit-cylot)*tancon;
  par[0]=0.;//phi0
  par[1]=360.;//dphi
  par[2]=4;
  par[3]=-casl2;
  par[4]=casr2;
  par[5]=casr2;
  par[6]=-casl1;
  par[7]=casr1;
  par[8]=casr2;
  par[9]=casl1;
  par[10]=casr1;
  par[11]=casr2;
  par[12]=casl2;
  par[13]=casr2;
  par[14]=casr2;
  coo[0]=0.; 
  coo[1]=0.; 
  coo[2]=0.; 
  gid=1;
  mmoth=mother.add(new AMSgvolume(
				  "MVCASEMED",0,"MMOT","PCON",par,15,coo,nrm,"ONLY",1,gid,1));//magnet case vol.
  par[3]=-casl2i;
  par[4]=casr2-cylot;
  par[5]=casr2-cylot;
  par[6]=-casl1i;
  par[7]=casr1+cylit;
  par[8]=casr2-cylot;
  par[9]=casl1i;
  par[10]=casr1+cylit;
  par[11]=casr2-cylot;
  par[12]=casl2i;
  par[13]=casr2-cylot;
  par[14]=casr2-cylot;
  mivol=mmoth->add(new AMSgvolume(
				  "MVACMED",0,"MVOL","PCON",par,15,coo,nrm,"ONLY",1,gid,1));//inner vac.volume
  //
  // -----> return coils:
  //
  geant rcrads=33.8;        //ret-coil radial size(Rout-Rint) incl.supp.
  geant rctrth=10.3;        //ret-coil trancv.(x/y) thickness(at mid.radious)
  geant rcradp=77.5;        //ret-coil radial pos.(in x-y plane)
  geant rchozs=39.3;        //ret-coil half out.z-size(rect! shape, 260+pi*169/4 mm)
  geant rchizs=32.8;        //ret-coil half inn.z-size(rect! shape, 260+pi*87/4 mm)
  geant rcrth=8.2;          //ret-coil tot.radial thickness of turns+supp(169-87mm)
  integer nrcoils=12;       //tot.number of ret-coils
  geant rcphis=360.*rctrth/rcradp/AMSDBc::twopi; //ret-coil phi-size (degree)
  geant rcphip[12]={60.,72.,84.,96.,108.,120.,240.,252.,264.,276.,288.,300.};
  //                (ret-coils phi-positions)
  for(int nc=0;nc<nrcoils;nc++){
    strcpy(vname,"MR");
    kk=nc+1;
    ii=kk/10;
    in[0]=inum[ii];
    strcat(vname,in);
    ii=kk%10;
    in[0]=inum[ii];
    strcat(vname,in);
    par[0]=rcradp-rcrads/2.;//rmin
    par[1]=rcradp+rcrads/2.;//rmax
    par[2]=rchozs;//dz
    par[3]=rcphip[nc]-rcphis/2.;//phi1  
    par[4]=rcphip[nc]+rcphis/2.;//phi2  
    gid=nc+1;
    coil=mivol->add(new AMSgvolume(
				   "MCOILMED",0,vname,"TUBS",par,5,coo,nrm,"ONLY",1,gid,1));//ret-coil
    par[0]=rcradp-rcrads/2.+rcrth;//rmin
    par[1]=rcradp+rcrads/2.-rcrth;//rmax
    par[2]=rchizs;//dz
    cur=coil->add(new AMSgvolume(
				 "MVACMED",0,"MRHO","TUBS",par,5,coo,nrm,"ONLY",gid==1?1:-1,gid,1));//ret-coil hole
  }
  //
  // -----> dipole coils:
  //
  geant dczth=19.;       //dip-coil tot.z-thick. of turns+supp(365-175mm)
  geant dczhs=36.5;      //dip-coil z half-size,incl.supp
  geant dcxpos=66.4;     //dip-coil x-pos
  geant dcxth=8.76;      //dip-coil x thickness
  geant dcohys=48.7;     //dip-coil out.half y-size,incl.supp(200+pi*365/4 mm)
  geant dcihys=33.7;     //dip-coil inn.half y-size,incl.supp(200+pi*175/4 mm)
  //
  par[0]=dcxth/2.;//dx
  par[1]=dcohys;  //dy
  par[2]=dczhs;   //dz
  coo[0]=dcxpos;//x-pos
  coo[1]=0.;//y-pos
  coo[2]=0.;//z-pos 
  gid=1;
  coil=mivol->add(new AMSgvolume(
				 "MCOILMED",0,"MDC1","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//main coil-1
  par[1]=dcihys;//dy
  par[2]=dczhs-dczth;//dz
  coo[0]=0.;
  gid=1;
  cur=coil->add(new AMSgvolume(
			       "MVACMED",0,"MDH1","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//coil-1 hole
  //----
  par[0]=dcxth/2.;//dx
  par[1]=dcohys;   //dy
  par[2]=dczhs;   //dz
  coo[0]=-dcxpos;//x-pos
  gid=1;
  coil=mivol->add(new AMSgvolume(
				 "MCOILMED",0,"MDC2","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//main coil-2
  par[1]=dcihys;//dy
  par[2]=dczhs-dczth;//dz
  coo[0]=0.;
  gid=1;
  cur=coil->add(new AMSgvolume(
			       "MVACMED",0,"MDH2","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//coil-2 hole
  //
  // -----> inter-fixation of dipole and return coils:
  //
  geant drfrmn=casr1+cylit+1.5;//inner radious of fixation
  geant drfrmx=dcxpos-dcxth/2.-1.5;//outer .......
  geant drfdz=dczhs-dczth+1.;//z half size(to cover hole of dipole coil)
  geant drfopang=55.;//half opening angle(degr)
  par[0]=drfrmn;
  par[1]=drfrmx;
  par[2]=drfdz;
  par[3]=360.-drfopang;
  par[4]=360.+drfopang;
  coo[0]=0.;
  coo[1]=0.;
  coo[2]=0.;
  gid=1;
  cur=mivol->add(new AMSgvolume(
				"MVCASEMED",0,"DRF1","TUBS",par,5,coo,nrm,"ONLY",1,gid,1));//dip/ret.coil fix.1
  par[3]=180.-drfopang;
  par[4]=180.+drfopang;
  cur=mivol->add(new AMSgvolume(
				"MVCASEMED",0,"DRF2","TUBS",par,5,coo,nrm,"ONLY",1,gid,1));//dip/ret.coil fix.2
  //
  // -----> liq.helium vessel + helium/vacuum:
  //
  geant hvrpos=112.5;    //he_vessel radial pos.
  geant hvrs=33.;        //he_vessel radial size
  geant hvohzs=55.5;     //he_vessel out.half z-size(425+pi*165/4 mm)
  geant hvihzs=55.26;     //he_vessel inn.half z-size(425+pi*162.5/4 mm)
  geant hvwth=0.25;      //he_vessel average wall(radial) thickness
  //
  par[0]=hvrpos-hvrs/2.;//rmin
  par[1]=hvrpos+hvrs/2.;//rmax
  par[2]=hvohzs;//dz
  coo[0]=0.; 
  coo[1]=0.; 
  coo[2]=0.; 
  gid=1;
  lhves=mivol->add(new AMSgvolume(
				  "MLHVMED",0,"MLHV","TUBE",par,3,coo,nrm,"ONLY",1,gid,1));//vessel
  //     
  par[0]=hvrpos-hvrs/2.+hvwth;//rmin
  par[1]=hvrpos+hvrs/2.-hvwth;//rmax
  par[2]=hvihzs;//dz
  if(MAGFIELDFFKEY.magstat>=0){
    cur=lhves->add(new AMSgvolume(
				  "MLHEMED",0,"MLHE","TUBE",par,3,coo,nrm,"ONLY",1,gid,1));//liq.helium
    cout<<"      magnet cold state selected !"<<endl;
  }
  else{
    cur=lhves->add(new AMSgvolume(
				  "MVACMED",0,"MLHE","TUBE",par,3,coo,nrm,"ONLY",1,gid,1));//vacuum
    cout<<"      mafnet warm state selected"<<endl;
  }
  //
  cout<<"<---- Amsgeom::magnetgeom02: G3/G4-compatible geometry is successfully done!"<<endl<<endl;
}

//-----------------------------------------------------------------
void amsgeom::ext1structure02(AMSgvolume & mother){
  // external radiators, crates, TOF-TRD interfaces, USS exept bottom frame and 
  //triangle, by E.C.
  //
  AMSID amsid;
  geant par[15]={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
  geant coo[3]={0.,0.,0.};
  geant cro[3]={0.,0.,0.};
  number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};//"no rotation" matrix
  number nrm0[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};//working array
  int i,j,ii;
  char inum[11];
  char in[2]="0";
  char vname[5];
  integer gid,nrot;
  AMSNode * cur;
  AMSNode * dau;
  //
  // ----------------------------> Radiators:
  // -------------> #1(vertical at +Y(Wake)):
  //
  geant r13dx=184.9;//section 13(top) width(Should match to us1x1 !!!)
  geant r1dy=2.6;//thickness
  geant r13dz=58.;//height
  geant r1cy=(173.5+0.5+r1dy/2.);//Y0 ("0.5 is my corr.to avoid intersection with ISS)
  geant r1cx=0.;//X0 
  geant r13cz=81.5;//Z0
  //
  geant r12xcut=29.;//x-width of the hole
  geant r12dx=r13dx-r12xcut;//section 12(mid) width
  geant r12dz=82.;//height
  geant r12cz=11.5;//Z0
  geant r12cx=r12xcut/2;//X0
  //
  geant r11dx=209.9;//section 11(bot) width
  geant r11dz=81.;//height
  geant r11cz=-70.;//Z0
  //
  //nrot=TOF2GC::SCROTN+200;
  //
  par[0]=r11dx/2.;
  par[1]=r1dy/2.;
  par[2]=r11dz/2.;
  coo[0]=r1cx;
  coo[1]=r1cy;
  coo[2]=r11cz;  
  gid=1;
  dau=mother.add(new AMSgvolume(
				"RADMED1",0,"RA1B","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//radiator #1,bott.
  //
  gid=2;
  par[0]=r12dx/2.;
  par[2]=r12dz/2.;
  coo[0]=r12cx;
  coo[2]=r12cz;  
  dau=mother.add(new AMSgvolume(
				"RADMED1",0,"RA1M","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//radiator #1,mid.
  //
  gid=3;
  par[0]=r13dx/2.;
  par[2]=r13dz/2.;
  coo[0]=r1cx;
  coo[2]=r13cz;  
  dau=mother.add(new AMSgvolume(
				"RADMED1",0,"RA1T","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//radiator #1,top.
  //
  // ------------> #2(vertical at -Y(Ram)):
  //
  geant r21dx=r11dx;//section 21(bot) width
  geant r2dy=r1dy;//thickness
  geant r21dz=r11dz;//height
  geant r2cx=0.;//X0
  geant r2cy=-r1cy;//Y0
  geant r21cz=r11cz;//Z0
  //
  geant r22dx=r13dx;// section 22(top) width
  geant r22dz=140.;
  geant r22cz=40.5;//Z0
  //
  par[0]=r21dx/2.;
  par[1]=r2dy/2.;
  par[2]=r21dz/2.;
  coo[0]=r2cx;
  coo[1]=r2cy;
  coo[2]=r21cz;  
  gid=1;
  dau=mother.add(new AMSgvolume(
				"RADMED1",0,"RA2B","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//radiator #2,bott.
  //
  gid=2;
  par[0]=r22dx/2.;
  par[2]=r22dz/2.;
  coo[2]=r22cz;  
  dau=mother.add(new AMSgvolume(
				"RADMED1",0,"RA2T","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//radiator #2,top.
  //
  // ------------> #3(inclined at +Y(Wake)):
  //
  geant r3dz=51.86;//height of TRD1(dz)
  geant r3cy2=173.15;// bot/inner corner y-pos.
  geant r3cz2=112.46;//bot/inner corner z-pos
  geant r3dy=1.5;// thickness
  geant r3dx1=260;//top dx of TRD1(at -z)
  geant r3dx2=212.7;//bot dx of TRD1(at +z)
  geant alp3=45.1;//slope(degr)
  geant cosa=cos(alp3/AMSDBc::raddeg);
  geant sina=sin(alp3/AMSDBc::raddeg);
  number nrm3[3][3]={-1.,0.,0.,0.,sina,cosa,0.,cosa,-sina};//rot matrix.v#3
  //
  par[0]=r3dx1/2;//dxloc at -Zloc
  par[1]=r3dx2/2;//dxloc at +Zloc
  par[2]=r3dy/2;// thickn(dyloc)
  par[3]=r3dz/2;// height(dzloc)
  coo[0]=0.;
  coo[1]=r3cy2-0.5*r3dz*cosa+0.5*r3dy*sina;
  coo[2]=r3cz2+0.5*r3dz*sina+0.5*r3dy*cosa;
  gid=1;
  dau=mother.add(new AMSgvolume(
				"RADMED1",nrot++,"RA3T","TRD1",par,4,coo,nrm3,"ONLY",1,gid,1));//radiator #3,top.
  //
  // ------------> #4(inclined at -Y):
  //
  number nrm4[3][3]={1.,0.,0.,0.,-sina,-cosa,0.,cosa,-sina};//rot matrix.v#4
  //
  coo[1]=-coo[1];
  gid=1;
  dau=mother.add(new AMSgvolume(
				"RADMED1",nrot++,"RA4T","TRD1",par,4,coo,nrm4,"ONLY",1,gid,1));//radiator #4,top.
  //
  // -----------------------------> Crates:
  //
  //----> Horizontal top crates(Attached to vert.radiators):
  //
  geant cr1dx=r13dx;// +Y(Wake) top crate-1(2)
  geant cr1dy=23.;//tempor
  geant cr1dz=26.3;
  geant cr1cy=r1cy-r1dy/2-cr1dy;//inner wall y-pos.
  geant cr1cz=(r11dz+r12dz+r13dz)/2-cr1dz-4.4;// bot wall z-pos
  //
  par[0]=cr1dx/2.;
  par[1]=cr1dy/2.;
  par[2]=cr1dz/2.;
  coo[0]=0.;
  coo[1]=cr1cy+cr1dy/2.;
  coo[2]=cr1cz+cr1dz/2.;
  gid=1;
  dau=mother.add(new AMSgvolume(
				"CRATEMED1",0,"CRH1","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//crate +Y/top
  coo[1]=-coo[1];
  dau=mother.add(new AMSgvolume(
				"CRATEMED1",0,"CRH2","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//crate -Y/top
  //
  //----> Horizontal bot crates(Attached to vert.radiators):
  //
  geant cr3dx=r11dx;// +Y(Wake) bot crate-3(4)
  par[0]=cr3dx/2.;
  coo[1]=-coo[1];//imply symmetric +-y/+-z pos !!! 
  coo[2]=-coo[2];
  dau=mother.add(new AMSgvolume(
				"CRATEMED1",0,"CRH3","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//crate +Y/bot
  coo[1]=-coo[1];
  dau=mother.add(new AMSgvolume(
				"CRATEMED1",0,"CRH4","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//crate -Y/bot
  //
  //----> Vertical +X(-X) crates(attached to vert.radiators):
  //
  geant crh1dx=18.5;//x-width
  geant crh1dz=2*(cr1cz-3);//y-height(and imply symm. +-z pos)
  geant crh1dy=cr1dy;// tempor
  geant crh1cx=38.9;//inner(wrt rad-midd) wall pos
  geant crh1cy=r1cy-r1dy/2-crh1dy;//inner(wrt AMS"0") wall y-pos.
  par[0]=crh1dx/2;
  par[1]=crh1dy/2;
  par[2]=crh1dz/2;
  coo[0]=crh1cx+crh1dx/2;
  coo[1]=crh1cy+crh1dy/2;
  coo[2]=0;
  gid=1;
  dau=mother.add(new AMSgvolume(
				"CRATEMED1",0,"CRV1","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//crate +Y/+X
  coo[0]=-coo[0];    
  dau=mother.add(new AMSgvolume(
				"CRATEMED1",0,"CRV2","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//crate +Y/-X
  coo[1]=-coo[1];    
  dau=mother.add(new AMSgvolume(
				"CRATEMED1",0,"CRV4","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//crate -Y/-X
  coo[0]=-coo[0];    
  dau=mother.add(new AMSgvolume(
				"CRATEMED1",0,"CRV3","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//crate -Y/+X
  //
  //---------------
  //
  //----> small horizontal crates near ECAL(29.01.03. update):
  //
  geant cr2dx=134.;//for +Y crate
  geant cr2dy=19.5;
  geant cr2dz=24.3;
  geant cr2cy=96.;//center y-pos
  // cr2cy+=15.;//tempor solution of ISS overlaping problem. Do not need now ??
  geant cr2cz=-106.4;//top wall z-pos
  // cr2cz+=10.;//tempor solution of ISS overlaping problem. Do not need now ??
  //
  par[0]=cr2dx/2.;
  par[1]=cr2dy/2.;
  par[2]=cr2dz/2.;
  coo[0]=0.;
  coo[1]=cr2cy;
  coo[2]=cr2cz-cr2dz/2.;
  gid=1;
  dau=mother.add(new AMSgvolume(
				"CRATEMED1",0,"CRS1","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//small crate +Y
  coo[1]=-coo[1];
  dau=mother.add(new AMSgvolume(
				"CRATEMED1",0,"CRS2","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//small crate -Y
  //
  //------------> TOF/TRD-interfaces(10.02.2003):
  //--- MSF
  geant msfr1=101.2;// M-struct. frame (MSF) internal radious
  geant msfz1=93.5;// MSF Z-pos 
  geant msfdz=6.;// MSF z-thickness
  geant msfdr=6.;// MSF radial thickness
  //--- MS
  geant msdy=6.;// M-structure legs thickness (MSL)
  geant msr2=112.;// MS-plane-top radial pos.(near bot.edge of TRD top honeyc)
  geant msz2=146.1;// MS ...............z-pos.(near bot.edge of TRD top honeyc)
  geant msdx1=93.1;// "M"-dx(outer half size) at Z-frame 
  geant msdx2=47.7;// "M"-dx(outer half size) at Z-top
  geant msdz=46.4;//  "M"-dz(height)
  geant msang1=atan((msr2-msfr1-msfdr)/(msz2-msfz1-msfdz));
  geant msang=atan(msdy/msdz)+msang1;//"M"-plane slope angle(~13.25degr)
  geant msalp1=atan(msdz/(msdx1-msdx2));//outer "M"-leg angle wrt frame
  // hole-3
  geant msh3dx2=36.;//hole-3 half size at top
  geant msdz2=msdz-6.;// hole-3 height  
  geant msalp2=atan(msdz/msh3dx2);//inner "M"-leg angle wrt frame
  geant msh3dx1=(msdz-msdz2)*msh3dx2/msdz;//hole-3 half size at bot
  geant msh3cy=(msdz-msdz2)/2.;//hole-3 "0" Y-pos in "M" ref.syst
  // hole-1
  geant msh1dx1=msdx1-msdy/sin(msalp1)-msdy/sin(msalp2);//hole-1 size at bot
  geant msdz1=msdz-6.;//hole-1 height
  geant msh1dx2=msh1dx1-msdz1*(1./tan(msalp1)+1./tan(msalp2));//hole-1 size at top
  geant msh1x1=msdy/sin(msalp2)+0.5*msh1dx1;//hole-1 bot base(center) x-pos
  geant msh1x2=msdy/sin(msalp2)+msdz1/tan(msalp2)+0.5*msh1dx2;//hole-1 top base x-pos
  geant msh1cx=0.5*(msh1x1+msh1x2);//hole-1(2) "0" |X-pos| in "M" ref.syst
  geant msh1cy=-(msdz-msdz1)/2.;//hole-1(2) "0" Y-pos in "M" ref.syst
  geant msgam=atan((msh1x1-msh1x2)/msdz1);//assim.angle of hole-1(2) trapezoid
  // 
  geant cosb=cos(msang);
  geant sinb=sin(msang);
  geant mscr=0.5*(msr2+msfr1+msfdr);//radial dist. of "M" origin in AMS r.s.
  geant mscz=0.5*(msfz1+msfdz+msz2);//Z-pos of "M" origin in AMS r.s.
  geant mscx[4]={0.,-mscr,0.,mscr};//X-pos for 4 M's origines
  geant mscy[4]={mscr,0.,-mscr,0.};//Y-pos for 4 M's origines
  number rmms1[3][3]={-1.,0.,0.,0.,sinb,cosb,0.,cosb,-sinb};//M1-rot.matr.at +Y
  number rmms2[3][3]={0.,-sinb,-cosb,-1.,0.,0.,0.,cosb,-sinb};//M2-rot.matr.at -X
  number rmms3[3][3]={1.,0.,0.,0.,-sinb,-cosb,0.,cosb,-sinb};//M3-rot.matr.at -Y
  number rmms4[3][3]={0.,sinb,cosb,1.,0.,0.,0.,cosb,-sinb};//M4-rot.matr.at +X
  //
  // TFHS(TOF-hoheycomb support,updated 30.01.03)
  //
  geant thsdx1=47.8;//radial bot-size of TRAPezoid(in AMS-XY plane)
  geant thsdx2=24.;//radial top-size of TRAP
  geant thsdz=20.8;//"width" of TRAP(in AMS-XY plane)
  geant thsdy=15.8;//"height" of TRAP(in AMS-Z direction)
  geant thsr0=83.5;//radial position of internal(closer to AMS "0") edge of TRAP
  geant thsdr=0.5*(1.5*thsdx1-thsdx2);//rad.dist. from int.edge of TRAP to its "0"
  geant thsr=thsr0+thsdr;//rad.pos. of TRAP "0"
  geant thsalp=atan(0.5*(thsdx1-thsdx2)/thsdy);//TRAP alpha-angle (rad)
  geant thsfi=45;//phi-angle of 1st TRAP in xy-plane(degr)
  geant sinfi=sin(thsfi/AMSDBc::raddeg);
  geant cosfi=cos(thsfi/AMSDBc::raddeg);
  geant thscx[4]={cosfi,-cosfi,-cosfi,cosfi};//TRAP "0" X-pos for 4 fi-positions
  geant thscy[4]={sinfi,sinfi,-sinfi,-sinfi};//TRAP "0" Y-pos for 4 fi-positions
  number rmtfs1[3][3]={cosfi,0.,sinfi,sinfi,0.,-cosfi,0.,1.,0.};//rot.matr.for pos-1(+x+y)
  number rmtfs2[3][3]={-sinfi,0.,cosfi,cosfi,0.,sinfi,0.,1.,0.};//rot.matr.for pos-1(-x+y)
  number rmtfs3[3][3]={-cosfi,0.,-sinfi,-sinfi,0.,cosfi,0.,1.,0.};//rot.matr.for pos-1(-x-y)
  number rmtfs4[3][3]={sinfi,0.,-cosfi,-cosfi,0.,-sinfi,0.,1.,0.};//rot.matr.for pos-1(+x-y)
  //
  //----> Create MSF(M-structure frame,PGON):
  //
  par[0]=45.;//begin phi
  par[1]=360;//dphi
  par[2]=4;//sides
  par[3]=2;// Nz
  par[4]=-msfdz/2.;//z1
  par[5]=msfr1;//r1
  par[6]=msfr1+msfdr;//r2
  par[7]=msfdz/2.;//z2
  par[8]=par[5];
  par[9]=par[6];
  coo[0]=0.;
  coo[1]=0.;
  coo[2]=msfz1+0.5*msfdz;
  gid=1;
  dau=mother.add(new AMSgvolume(
				"MSFMED1",0,"MSFR","PGON",par,10,coo,nrm,"ONLY",1,gid,1));// M-structure frame
  //
  //----> create TFHS(TOF-honeycomb supports, TRAP shape): 
  //
  par[0]=thsdz/2.;//dz
  par[1]=0.;//the
  par[2]=0.;//phi
  par[3]=thsdy/2.;//h1
  par[4]=thsdx1/2.;//bl1
  par[5]=thsdx2/2.;//tl1
  par[6]=thsalp*AMSDBc::raddeg;//alp1
  par[7]=par[3];//h2
  par[8]=par[4];//bl2
  par[9]=par[5];//tl2
  par[10]=par[6];//alp2
  //coo[2]=TOF2DBc::supstr(1)+TOF2DBc::supstr(9)+0.5*thsdy+0.001;//Z of TRAP "0" in AMS r.s.
  for(int is=0;is<4;is++){// loop over 4 supports
    coo[0]=thsr*thscx[is];
    coo[1]=thsr*thscy[is];
    if(is==0)for(i=0;i<3;i++)for(j=0;j<3;j++)nrm0[i][j]=rmtfs1[i][j];
    if(is==1)for(i=0;i<3;i++)for(j=0;j<3;j++)nrm0[i][j]=rmtfs2[i][j];
    if(is==2)for(i=0;i<3;i++)for(j=0;j<3;j++)nrm0[i][j]=rmtfs3[i][j];
    if(is==3)for(i=0;i<3;i++)for(j=0;j<3;j++)nrm0[i][j]=rmtfs4[i][j];
    gid=is+1;
    nrot+=is;
    dau=mother.add(new AMSgvolume(
				  "TFSUPMED1",nrot,"TFHS","TRAP",par,11,coo,nrm0,"ONLY",gid==1?1:-1,gid,1));//TOF-honeyc.supp.
  }
  //
  //----> create M-structure(TRAP with 3 TRAP-holes) :
  //
  for(int is=0;is<4;is++){//loop over 4 "M"-trapezoids
    par[0]=msdy/2.;//dz
    par[1]=0.;//the
    par[2]=0.;//phi
    par[3]=msdz/2.;//dy/2 at -z
    par[4]=msdx1;//dx/2 at -y,-z
    par[5]=msdx2;//dx/2 at +y,-z
    par[6]=0.;//alp1
    par[7]=par[3];//dy/2 at +z
    par[8]=par[4];//dx/2 at -y,+z
    par[9]=par[5];//dx/2 at +y,+z
    par[10]=par[6];//alp2
    coo[2]=mscz+0.001;//Z of TRAP-"0" in AMS r.s.
    coo[0]=mscx[is];
    coo[1]=mscy[is];
    if(is==0)for(i=0;i<3;i++)for(j=0;j<3;j++)nrm0[i][j]=rmms1[i][j];
    if(is==1)for(i=0;i<3;i++)for(j=0;j<3;j++)nrm0[i][j]=rmms2[i][j];
    if(is==2)for(i=0;i<3;i++)for(j=0;j<3;j++)nrm0[i][j]=rmms3[i][j];
    if(is==3)for(i=0;i<3;i++)for(j=0;j<3;j++)nrm0[i][j]=rmms4[i][j];
    gid=is+1;
    nrot+=is;
    dau=mother.add(new AMSgvolume(
				  "MSFMED1",nrot,"MSMB","TRAP",par,11,coo,nrm0,"ONLY",gid==1?1:-1,gid,1));//M-structure M-body
#ifndef __G4AMS__
    if(is==0)
#else
      if(MISCFFKEY.G4On || (is==0))
#endif
	{
	  par[0]=msdy/2.;//dz
	  par[1]=0.;//the
	  par[2]=0.;//phi
	  par[3]=msdz1/2.;//dy/2 at -z
	  par[4]=msh1dx1/2.;//dx/2 at -y,-z
	  par[5]=msh1dx2/2.;//dx/2 at +y,-z
	  par[6]=msgam*AMSDBc::raddeg;//alp1
	  par[7]=par[3];//dy/2 at +z
	  par[8]=par[4];//dx/2 at -y,+z
	  par[9]=par[5];//dx/2 at +y,+z
	  par[10]=par[6];//alp2
	  cro[0]=-msh1cx;
	  cro[1]=msh1cy;
	  cro[2]=0.;
	  cur=dau->add(new AMSgvolume(
				      "MVACMED",0,"MSH1","TRAP",par,11,cro,nrm,"ONLY",0,gid,1));// hole-1(vacuum)
	  //
	  par[6]=-msgam*AMSDBc::raddeg;//alp1
	  par[10]=par[6];//alp2
	  cro[0]=msh1cx;
	  cur=dau->add(new AMSgvolume(
				      "MVACMED",0,"MSH2","TRAP",par,11,cro,nrm,"ONLY",0,gid,1));// hole-2(vacuum)
	  //
	  par[0]=msdy/2.;//dz
	  par[1]=0.;//the
	  par[2]=0.;//phi
	  par[3]=msdz2/2.;//dy/2 at -z
	  par[4]=msh3dx1;//dx/2 at -y,-z
	  par[5]=msh3dx2;//dx/2 at +y,-z
	  par[6]=0.;//alp1
	  par[7]=par[3];//dy/2 at +z
	  par[8]=par[4];//dx/2 at -y,+z
	  par[9]=par[5];//dx/2 at +y,+z
	  par[10]=par[6];//alp2
	  cro[0]=0.;
	  cro[1]=msh3cy;
	  cur=dau->add(new AMSgvolume(
				      "MVACMED",0,"MSH3","TRAP",par,11,cro,nrm,"ONLY",0,gid,1));// hole-3(vacuum)
	  //
#ifdef __G4AMS__
	  ((AMSgvolume*)cur )->Smartless()=-2;
#endif            
	}
  }//--->endof loop
  //
  //----> create M-struct. brackets, attached to TRD-honeycomb:
  //
  geant msbrdr=4.;// dR
  geant msbrzb=msz2+0.001;//Zbot
  geant msbrdz=8.;//dZ
  geant msbrdf=10.;//dphi(degr)
  geant msbf1=360./16;//1st bracket phi-pos.
  geant msbrmn=msr2/cos(msbf1/AMSDBc::raddeg);//bracket Rmin
  geant msbfs=360./8;//phi spacing
  geant msbfi;
  // 
  par[0]=msbrmn;//rmin
  par[1]=msbrmn+msbrdr;//rmax
  par[2]=0.5*msbrdz;//dz/2
  //
  coo[0]=0.;
  coo[1]=0.;
  coo[2]=msbrzb+0.5*msbrdz;//cz
  //
  for(int ib=0;ib<8;ib++){//loop over 8 brackets
    msbfi=msbf1+ib*msbfs;
    par[3]=msbfi-0.5*msbrdf;
    par[4]=msbfi+0.5*msbrdf;
    gid=ib+1;
    dau=mother.add(new AMSgvolume(
				  "MSBRMED",0,"MSBR","TUBS",par,5,coo,nrm,"ONLY",1,gid,1));//M-structure top-brackets
  }
  //
  // ------> M-structure antivibration rods(22.09.03 RB) :
  //
  geant rodd=2.5;//rod diameter
  geant rodh=2.;//rod int. hole diam
  geant rodpxy=52.8;//rod-end dist from MS-frame edge(intern.angle)
  geant rodpz=0;//rod z-pos wrt MS-frame top surface(<0 if below it)
  geant rodl=sqrt(2.)*rodpxy-rodd-0.01;//rod length +safety margine
  geant roda=45;//rod-rotation angle in xy-plane(only 45degr is supported !) 
  geant sinra=sin(roda/AMSDBc::raddeg);
  geant cosra=cos(roda/AMSDBc::raddeg);
  number rmrd1[3][3]={0.,sinra,cosra,0.,cosra,-sinra,-1.,0.,0.};//rm  +x+y
  number rmrd2[3][3]={0.,sinra,-cosra,0.,-cosra,-sinra,-1.,0.,0.};//rm +x-y
  number rmrd3[3][3]={0.,-sinra,-cosra,0.,-cosra,sinra,-1.,0.,0.};//rm -x-y
  number rmrd4[3][3]={0.,-sinra,cosra,0.,cosra,sinra,-1.,0.,0.};//rm -x+y
  //
  par[0]=rodh/2;//rmin
  par[1]=rodd/2;//rmax
  par[2]=rodl/2;//length
  coo[2]=msfz1+msfdz-rodpz;
  geant rdcx=msfr1-rodpxy/2;//rod-center x-pos for #1
  geant rdcy=msfr1-rodpxy/2;//rod-center y-pos for #1
  //
  for(int ir=0;ir<4;ir++){
    if(ir==0){
      for(i=0;i<3;i++)for(j=0;j<3;j++)nrm0[i][j]=rmrd1[i][j];
      coo[0]=rdcx;
      coo[1]=rdcy;
    }
    if(ir==1){
      for(i=0;i<3;i++)for(j=0;j<3;j++)nrm0[i][j]=rmrd2[i][j];
      coo[0]=rdcx;
      coo[1]=-rdcy;
    }
    if(ir==2){
      for(i=0;i<3;i++)for(j=0;j<3;j++)nrm0[i][j]=rmrd3[i][j];
      coo[0]=-rdcx;
      coo[1]=-rdcy;
    }
    if(ir==3){
      for(i=0;i<3;i++)for(j=0;j<3;j++)nrm0[i][j]=rmrd4[i][j];
      coo[0]=-rdcx;
      coo[1]=rdcy;
    }
    gid=ir+1;
    nrot+=ir;
    dau=mother.add(new AMSgvolume(
				  "USSALLOY",nrot,"AVRD","TUBE",par,3,coo,nrm0,"ONLY",gid==1?1:-1,gid,1));
  }
  //------------------------------------------ 
  //
  //------------> USS :
  //
  //----> bar-1 type(attached to magnet top + TOF/TRD interface):
  //
  geant us1dz=18.4;//final
  geant us1dx=12.7;//final
  geant us1x1=99.9-us1dx/2.;//(final)inner(wrt AMS "0") edge x-pos
  geant us1y1=99.9;//(final) inner edge y-pos(with us1x1 are defined by attach to VC
  //                             at R=141.3cm and angle 45 degr.)                            
  geant us1z1=68.12-us1dz/2.;//(final) inner/bot edge z-pos 
  geant us1x2=us1x1;// outer edge x-pos
  geant us1y2=192.;//(final+cutted) outer edge y-pos
  geant us1z2=59.5-us1dz/2.;//(final+cutted) outer edge z-pos
  geant us1dy=sqrt(pow(us1y2-us1y1,2)+pow(us1z1-us1z2,2));
  geant us1ang=atan((us1z1-us1z2)/(us1y2-us1y1));//slope(>0)
  geant cosb1=cos(us1ang);
  geant sinb1=sin(us1ang);
  number rmus1[3][3]={1.,0.,0.,0.,cosb1,sinb1,0.,-sinb1,cosb1};
  number rmus1a[3][3]={1.,0.,0.,0.,cosb1,-sinb1,0.,sinb1,cosb1};
  //
  //----> bar-2 type(attached to magnet bottom):
  //
  geant us2aan=46.3/180.*AMSDBc::pi;//beams angle abs.value
  geant us2dx=12.6;//(final)
  geant us2dz=15.2;//(final)
  geant us2x1=99.9-us2dx/2.;//(final)inner(wrt AMS "0") edge x-pos
  geant us2y1=us1y1;// inner edge y-pos
  geant us2z1=-90.14+0.5*us2dz*cos(us2aan);//(final) inner edge  z-pos
  geant us2x2=us2x1;// outer edge x-pos
  geant us2z2=us1z2-48.;//(final+cutted) outer edge z-pos
  geant us2y2=us2y1+(us2z2-us2z1)/tan(us2aan);//(final) outer edge y-pos
  // (this y-pos is calc.from hand-fixed z-pos and known angle 46.3)
  geant us2dy=sqrt(pow(us2y2-us2y1,2)+pow(us2z2-us2z1,2));
  geant us2ang=atan((us2z1-us2z2)/(us2y2-us2y1));// slope(<0) for +Y beams
  geant cosb2=cos(us2ang);
  geant sinb2=sin(us2ang);
  number rmus2[3][3]={1.,0.,0.,0.,cosb2,sinb2,0.,-sinb2,cosb2};
  number rmus2a[3][3]={1.,0.,0.,0.,cosb2,-sinb2,0.,sinb2,cosb2};
  //
  //----> bar-3 type:
  //
  geant us3x=0.;//center x-pos
  geant us3y=211.4;//(final)center y-pos
  geant us3z=57.6;//(final)center z-pos
  geant us3dx=2.*us1x1-0.001;
  geant us3dy=8.;// tempor
  geant us3dz=8.;
  //
  //------------> create 4 bars of type-1:
  //
  par[0]=us1dx/2.;
  par[1]=us1dy/2.;
  par[2]=us1dz/2.;
  coo[0]=-(us1x1+us1dx/2.);
  coo[1]=0.5*(us1y1+us1y2)+sinb1*us1dz/2.;
  coo[2]=0.5*(us1z1+us1z2)+cosb1*us1dz/2.;
  gid=1;
  nrot+=1;
  dau=mother.add(new AMSgvolume(
				"US1BMED",nrot,"US1B","BOX",par,3,coo,rmus1,"ONLY",gid==1?1:-1,gid,1));//at (-x,+y)
  coo[0]=-coo[0];
  gid+=1;
  nrot+=1;
  dau=mother.add(new AMSgvolume(
				"US1BMED",nrot,"US1B","BOX",par,3,coo,rmus1,"ONLY",gid==1?1:-1,gid,1));//at (+x,+y)
  coo[1]=-coo[1];
  gid+=1;
  nrot+=1;
  dau=mother.add(new AMSgvolume(
				"US1BMED",nrot,"US1B","BOX",par,3,coo,rmus1a,"ONLY",gid==1?1:-1,gid,1));//at (+x,-y)
  coo[0]=-coo[0];
  gid+=1;
  nrot+=1;
  dau=mother.add(new AMSgvolume(
				"US1BMED",nrot,"US1B","BOX",par,3,coo,rmus1a,"ONLY",gid==1?1:-1,gid,1));//at (-x,-y)
  //
  //-----------> create 4 bars of type-2
  //
  par[0]=us2dx/2.;
  par[1]=us2dy/2.;
  par[2]=us2dz/2.;
  coo[0]=-(us2x1+us2dx/2.);
  coo[1]=0.5*(us2y1+us2y2)-sinb2*us2dz/2.;
  coo[2]=0.5*(us2z1+us2z2)-cosb2*us2dz/2.;
  gid=1;
  nrot+=1;
  dau=mother.add(new AMSgvolume(
				"US2BMED",nrot,"US2B","BOX",par,3,coo,rmus2,"ONLY",gid==1?1:-1,gid,1));//at (-x,+y)
  coo[0]=-coo[0];
  gid+=1;
  nrot+=1;
  dau=mother.add(new AMSgvolume(
				"US2BMED",nrot,"US2B","BOX",par,3,coo,rmus2,"ONLY",gid==1?1:-1,gid,1));//at (+x,+y)
  coo[1]=-coo[1];
  gid+=1;
  nrot+=1;
  dau=mother.add(new AMSgvolume(
				"US2BMED",nrot,"US2B","BOX",par,3,coo,rmus2a,"ONLY",gid==1?1:-1,gid,1));//at (+x,-y)
  coo[0]=-coo[0];
  gid+=1;
  nrot+=1;
  dau=mother.add(new AMSgvolume(
				"US2BMED",nrot,"US2B","BOX",par,3,coo,rmus2a,"ONLY",gid==1?1:-1,gid,1));//at (-x,-y)
  //
  //-----------> create 2 bars of type-3 (innored now !!!)
  //
  par[0]=us3dx/2.;
  par[1]=us3dy/2.;
  par[2]=us3dz/2.;
  coo[0]=us3x;
  coo[1]=us3y;
  coo[2]=us3z;
  gid=1;
  // dau=mother.add(new AMSgvolume(
  //   "US3BMED",0,"US3B","BOX",par,3,coo,nrm,"ONLY",gid==1?1:-1,gid,1));//at (+y)
  coo[1]=-coo[1];
  gid+=1;
  // dau=mother.add(new AMSgvolume(
  //   "US3BMED",0,"US3B","BOX",par,3,coo,nrm,"ONLY",gid==1?1:-1,gid,1));//at (-y)
  //-------------------------------------
  //----> bar-4 type(attached to magnet bottom and frame around EC):
  //
  geant us4dx=11.5;//final
  geant us4dz=14.2;//final
  geant us4r1=106.5;//(final)radial pos. of low end(center)(check matching with low frame)
  geant us4r2=141.3;//(final)radial pos. of high end(center)(match top/bot USS joint radious)
  geant us4z1=-135.9;//final                               ................................
  geant us4z2=-95.;//final(to have 50.1 degr. slope)        ................................
  geant us4dy=sqrt(pow(us4r2-us4r1,2)+pow(us4z2-us4z1,2));
  geant us4bet=atan((us4z2-us4z1)/(us4r2-us4r1));//vert.slope
  geant cosb4=cos(us4bet);
  geant sinb4=sin(us4bet);
  number rmus4[3][3];
  geant us4alf[4]={45.,135.,225.,315.};//bar rot.angle in xy-plane
  geant sina4,cosa4;
  //
  //-----------> create 4 bars of type-4:
  //
  par[0]=us4dx/2.;
  par[1]=us4dy/2.;
  par[2]=us4dz/2.;
  coo[2]=0.5*(us4z1+us4z2);
  for(i=0;i<4;i++){
    sina4=sin(us4alf[i]/AMSDBc::raddeg);
    cosa4=cos(us4alf[i]/AMSDBc::raddeg);
    coo[0]=0.5*(us4r2+us4r1)*sina4;
    coo[1]=0.5*(us4r2+us4r1)*cosa4;
    rmus4[0][0]=cosa4;
    rmus4[0][1]=sina4*cosb4;
    rmus4[0][2]=-sina4*sinb4;
    rmus4[1][0]=-sina4;
    rmus4[1][1]=cosa4*cosb4;
    rmus4[1][2]=-cosa4*sinb4;
    rmus4[2][0]=0.;
    rmus4[2][1]=sinb4;
    rmus4[2][2]=cosb4;
    gid=i+1;
    nrot+=1;
    dau=mother.add(new AMSgvolume(
				  "US4BMED",nrot,"US4B","BOX",par,3,coo,rmus4,"ONLY",gid==1?1:-1,gid,1));
  }
  //
  cout<<"<---- Amsgeom::ext1structure02:Rads/crates/M_struct/USS G3/G4-compatible geometry done!"<<endl<<endl;
}
//------------------------------------------------------------------------
void amsgeom::magnetgeom02Test(AMSgvolume & mother){
  AMSID amsid;
  geant par[6]={0.,0.,0.,0.,0.,0.};
  char name[5]="MAGN";
  geant coo[3]={0.,0.,0.};
  number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
  integer gid=0;
  AMSNode * cur;
  AMSNode * dau;
  AMSgtmed *p;
  geant magnetl=80.;
  gid=1;
  par[0]=113.2/2;
  par[1]=129.6/2;
  par[2]=magnetl/2.;
  dau=mother.add(new AMSgvolume(
				"MAGNET",0,name,"TUBE",par,3,coo,
				nrm, "ONLY",0,gid,1));
  gid=2;
  par[0]=111.4/2.;
  par[1]=112.0/2.;
  par[2]=magnetl/2.;
  dau=mother.add(new AMSgvolume(
				"1/2ALUM",0,"ALT1","TUBE",par,3,coo,nrm, "ONLY",0,gid,1));
  gid=3;
  par[0]=129.6/2.;
  par[1]=130.4/2.;
  par[2]=magnetl/2.;
  dau=mother.add(new AMSgvolume(
				"1/2ALUM",0,"ALT2","TUBE",par,3,coo,nrm, "ONLY",0,gid++,1));
  par[0]=112/2.;
  par[1]=165./2.;
  par[2]=0.75;
  coo[2]=magnetl/2.+par[2];
  dau=mother.add(new AMSgvolume(
				"1/2ALUM",0,"ALT3","TUBE",par,3,coo,nrm, "ONLY",0,gid++,1));
    
  par[0]=112/2.;
  par[1]=165.6/2.;
  par[2]=0.75;
  coo[2]=-magnetl/2.-par[2];
  dau=mother.add(new AMSgvolume(
				"1/2ALUM",0,"ALT4","TUBE",par,3,coo,nrm, "ONLY",0,gid++,1));
    
}
//-------------------------------------------------------------------
void amsgeom::ext2structure(AMSgvolume & mother){
  //USS low frame + triang., by V.Choutko(frame parameters modified by E.C
  AMSID amsid;
  geant par[10]={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
  number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
  number inrm[3][3];
  integer nrot;
  char name[5];
  geant coo[3]={0.,0.,0.};
  integer gid=0;
  AMSNode * cur;
  AMSNode * dau;
  AMSgtmed *p;
  gid=1;

  //  nrot=TOF2GC::SCROTN+300;
  // Low quadrant(frame)
  geant frameztop=-135.95;//(final)Z-pos of the frame(top face)
        
  par[0]=45;   //  phi angle
  par[1]=360;   // open angle
  par[2]=4;     // quadrant
  par[3]=2;
  par[4]=-5.08;//final
  par[5]=130.22/2.;//final
  par[6]=par[5]+5.08*2.;//final
  par[7]=-par[4];
  par[8]=par[5];
  par[9]=par[6];
  coo[0]=0;
  coo[1]=0;
  coo[2]=frameztop+par[4];//z-center of the frame
  mother.add(new AMSgvolume("US5BMED",nrot++,"USLF","PGON",par,10,coo,nrm,"ONLY",0,gid++,1));


  // Low Triangle ( two para)

  coo[1]=-(par[5]+par[6])/2/2;
  coo[0]=-(par[5]+par[6])/2;
  coo[2]=coo[2]+par[4];
  par[0]=4.5*2.54/2;
  par[1]=4.5*2.54/2;
  par[2]=4.5*2.54/2 * 3.6;
  coo[1]=-par[6]+par[2]+par[0];   //  implicit 45 deg assumption
  par[3]=0;
  par[4]=45;
  par[5]=0;

  coo[2]=coo[2]-par[2];
  number nrm1[3][3]={ 0, 1,0,
		      -1, 0,0,
		      0, 0,1
  };
  mother.add(new AMSgvolume("USSALLOY",nrot++,"USS3","PARA",par,6,coo,nrm1,"ONLY",0,gid++,1));

  coo[1]=-coo[1];
  par[5]=180;


  mother.add(new AMSgvolume("USSALLOY",nrot++,"USS2","PARA",par,6,coo,nrm1,"ONLY",0,gid++,1));
  //
  cout<<"<---- Amsgeom::ext2structure: Low_USS_frame geometry(G3/G4-compatible) done!"<<endl<<endl;
}







//------------------------------
