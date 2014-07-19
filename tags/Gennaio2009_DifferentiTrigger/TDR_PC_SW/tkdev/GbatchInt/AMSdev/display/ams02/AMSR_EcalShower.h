//  $Id: AMSR_EcalShower.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef AMSR_EcalShower_H
#define AMSR_EcalShower_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Root track class                                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#include <THelix.h>

class AMSR_EcalShowerReader;


class AMSR_EcalShower : public THelix {
//class AMSR_EcalShower : public TObject {

private:
   Int_t        m_ID;			//Track ID
   Int_t	m_Status;       	//Status word

   //
   // Hits
   //

   Float_t      m_Energy;
   Float_t      m_ErrEnergy;
   Float_t	m_FChi2;		//Chi2 fast nonlinear fit
   Float_t	m_FTheta;		//Theta fast nonlinear fit
   Float_t	m_FPhi;			//Phi fast nonlinear fit
   Float_t	m_FP0[3];		//Coordinates fast nonlinear fit


public:
                  AMSR_EcalShower() {;}
   virtual       ~AMSR_EcalShower() {;}
   virtual char  *GetObjectInfo(Int_t px, Int_t py) const;
           void   SetHelix();
   virtual void   Paint(Option_t *option="");

   //
   // Getters and Setters...
   //
   friend class AMSR_EcalShowerReader;

   ClassDef(AMSR_EcalShower, 1)   //AMSR_Root track class
};

#endif
