//  $Id: AMSR_TrdTrack.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef AMSR_TrdTrack_H
#define AMSR_TrdTrack_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Root TrdTrack class                                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#include <THelix.h>

class AMSR_TrdTrackReader;


class AMSR_TrdTrack : public THelix {
//class AMSR_TrdTrack : public TObject {

private:
   Int_t        m_ID;			//TrdTrack ID
   Int_t	m_Status;       	//Status word

   //
   // Hits
   //
   Int_t	m_Pattern;		//Hit pattern, see datacards.doc

   //
   // Some Chi^2
   //
   Float_t	m_Chi2;


     Float_t	m_Theta;		//Theta 
     Float_t	m_Phi;			//Phi
     Float_t	m_Coo[3];		//Coordinates 
     Float_t	m_ErCoo[3];		//Coordinates 


public:
                  AMSR_TrdTrack() {;}
                  AMSR_TrdTrack(Int_t status, Int_t pattern);
   virtual       ~AMSR_TrdTrack() {;}
   virtual char  *GetObjectInfo(Int_t px, Int_t py) const;
           void   SetHelix();
   virtual void   Paint(Option_t *option="");

   //
   // Getters and Setters...
   //
   friend class AMSR_TrdTrackReader;

   ClassDef(AMSR_TrdTrack, 1)   //AMSR_Root TrdTrack class
};

#endif
