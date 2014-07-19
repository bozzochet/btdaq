//  $Id: AMSR_Track.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef AMSR_Track_H
#define AMSR_Track_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Root track class                                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#include <THelix.h>

class AMSR_TrackReader;


class AMSR_Track : public THelix {
//class AMSR_Track : public TObject {

private:
   Int_t        m_ID;			//Track ID
   Int_t	m_Status;       	//Status word

   //
   // Hits
   //
   Int_t	m_Pattern;		//Hit pattern, see datacards.doc
   Int_t	m_NHits;		//Number of hits
   Int_t	m_PHits[6];		//Pointers to trrechit (object now?)

   //
   // Fitting Algorithms that are done
   //
   Int_t	m_FastFitDone;		//non-zero if done
   Int_t	m_GeaneFitDone;		//non-zero if done
   Int_t	m_AdvancedFitDone;	//non-zero if done

   //
   // Some Chi^2
   //
   Float_t	m_Chi2StrLine;		//Chi2 sz fit (what is sz?)
   Float_t	m_Chi2Circle;		//Chi2 circular fit
   Float_t	m_CircleRigidity;	//Circular rigidity

   //
   // Track parameters from fast fit
   //
     Float_t	m_FChi2;		//Chi2 fast nonlinear fit
     Float_t	m_FRigidity;		//Rigidity fast nonlinear fit
     Float_t	m_FErrRigidity;		//Error of Rigidity fast nonlinear fit
     Float_t	m_FTheta;		//Theta fast nonlinear fit
     Float_t	m_FPhi;			//Phi fast nonlinear fit
     Float_t	m_FP0[3];		//Coordinates fast nonlinear fit

   //
   // Track parameters from Geane fit
   //
     Float_t	m_GChi2;		//Chi2 Geane fit
     Float_t	m_GRigidity;		//Rigidity Geane fit
     Float_t	m_GErrRigidity;		//Error of Rigidity Geane fit
     Float_t	m_GTheta;		//Theta Geane fit
     Float_t	m_GPhi;			//Phi Geane fit
     Float_t	m_GP0[3];		//Coordinates Geane fit

   //
   // Track parameters from half-half fit
   //
   Float_t	m_HChi2[2];		//Chi2 half-half fit
   Float_t      m_HRigidity[2];		//Rigidity half-half fit
   Float_t      m_HErrRigidity[2];	//Error of Rigidity half-half fit
   Float_t      m_HTheta[2];		//Theta half-half fit
   Float_t      m_HPhi[2];		//Phi half-half fit
   Float_t      m_HP0[2][3];		//Coordinates half-half fit

   Float_t      m_FChi2MS;		//Fast chi2 mscat off
   Float_t      m_GChi2MS;		//Geane chi2 mscat off
   Float_t      m_RigidityMS;		//Fast rigidity mscat off
   Float_t      m_GRigidityMS;		//Geane rigidity mscat off


   //
   // Links to Monte Carlo particles should be implemented in a class
   // that derives from this class
   //


public:
                  AMSR_Track() {;}
                  AMSR_Track(Int_t status, Int_t pattern);
   virtual       ~AMSR_Track() {;}
   virtual char  *GetObjectInfo(Int_t px, Int_t py) const;
           void   SetHelix();
   virtual void   Paint(Option_t *option="");

   //
   // Getters and Setters...
   //
   friend class AMSR_TrackReader;

   ClassDef(AMSR_Track, 1)   //AMSR_Root track class
};

#endif
