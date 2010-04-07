// $Id: tkgeom.C,v 1.2 2008/03/17 22:03:34 haino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  tkgeom.cc
///\brief Definition for materials/mediums and geometry for tracker
/// 
/// Tracker-related parts are extracted from gmat.C and amsgeom.C
///
///\date  2007/11/27 SH  First test version
///\date  2007/11/29 SH  First stable version
///$Date: 2008/03/17 22:03:34 $
///
///$Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////////

#include "TrGeom.h"

#include "gvolume.h"  // for AMSgvolume
#include "gmat.h"     // for AMSgmat and AMSgtmed
#include "job.h"      // for AMSJob

namespace gbtrk {

/// Define materials for AMS detector
void amsmat(void)
{
  static AMSgmat mat;
  mat.setname("Materials:");

  mat.add (new AMSgmat("VACUUM",1.01,1., 1.e-21,1.E+22,1.E+22,0.1));
  mat.add (new AMSgmat("VACUUMTRD",1.01,1., 1.e-21,1.E+22,1.E+22,0.1));
  mat.add (new AMSgmat("CARBON", 12.01, 6., 2.265 ,18.8,49.9));
  mat.add (new AMSgmat("SILICON",28.09,14.,2.32,9.36,45.));

  // AL honeycomb structure for Tracker (as for TOF now !) :
  mat.add (new AMSgmat( "AL-HONEYC-Tr",26.98, 13., 0.04, 600., 2660.));

  // Foam structure for Tracker  :
  mat.add (new AMSgmat( "FOAM",12.01, 6., 0.1 , 425.82, 900.));

  // Map objects
  AMSgObj::GTrMatMap.map(mat);
}

/// Define mediums for AMS detector
void amstmed(void)
{
  static AMSgtmed tmed;
  tmed.setname("TrackingMedia:");
  tmed.add (new AMSgtmed("VACUUM","VACUUMTRD",0));
  tmed.add (new AMSgtmed("ACTIVE_SILICON","SILICON",1));
  tmed.add (new AMSgtmed("NONACTIVE_SILICON","SILICON"));
  tmed.add (new AMSgtmed("ELECTRONICS","CARBON",0));
  tmed.add (new AMSgtmed("Tr_Honeycomb","AL-HONEYC-Tr",0));
  tmed.add (new AMSgtmed("Tr_Foam","FOAM",0));

  // Map objects
  AMSgObj::GTrMedMap.map(tmed);
}

/// Define AMS geometry
void amsgeom(void)
{
  AMSID amsid;
  geant par[11];
  geant parf[10];
  geant coo[3]={0,0,0};
  number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
  integer gid=1;
  number amss[3];

  for(int i=0;i<3;i++)amss[i]=AMSDBc::ams_size[i]*sqrt(2.);

  parf[0]=amss[0]/2;
  parf[1]=amss[1]/2;
  parf[2]=amss[2]/2;
  par[0]=AMSDBc::ams_size[0]/2;
  par[1]=AMSDBc::ams_size[1]/2;
  par[2]=AMSDBc::ams_size[2]/2;

//----> create mother volumes:

  // AMS false mother volume
  static AMSgvolume false_mother("VACUUM",0,AMSDBc::ams_name,"BOX ",
				 parf,3,coo,nrm,"ONLY",0,gid,1);
  // AMS mother volume
  static AMSgvolume mother("VACUUM",AMSDBc::ams_rotmno,"FMOT","BOX", par,
			   3,AMSDBc::ams_coo,AMSDBc::ams_nrm,"ONLY",0,gid,1);

  AMSJob::gethead()->addup(&false_mother);
  false_mother.add(&mother);

  // Tracker geometry
  TrGeom::Build(&mother);

  false_mother.MakeG3Volumes();

  std::cout << "AMSGeom::-I-" 
	    << AMSgvolume::getNpv() << " Physical volumes, "
	    << AMSgvolume::getNlv() << " logical volumes and "
	    << AMSgvolume::getNrm() << " rotation matrixes have been created "
	    << std::endl;

  // Close geometry
  GGCLOS();
  std::cout << " AMSGeom-I-TotalOf "
	    << AMSgvolume::GlobalRotMatrixNo()
	    << " rotation matrixes  created" << std::endl;
}

}  // End of namespace gbtrk
