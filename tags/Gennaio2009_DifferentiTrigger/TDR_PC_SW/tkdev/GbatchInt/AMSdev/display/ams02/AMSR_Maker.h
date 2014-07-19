//  $Id: AMSR_Maker.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef AMSR_Maker_H
#define AMSR_Maker_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Root AMS virtual base class for Makers                               //
//                                                                      //
// Creation:  Mon Nov 10 17:54:25 MET 1997 at ETH                       //
//            Copied from ATLFast header file and modified to suit AMS. //
//                                                                      //
// Ping Yeh, Academia Sinica                                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TNamed
#include <TNamed.h>
#endif

class TList;
class TBrowser;

class AMSR_Maker : public TNamed {

protected:

   Int_t          m_Save;        // = 1 if m-Maker to be saved in the Tree
   TObject       *m_Fruits;      //Pointer to maker fruits (result)
   TString        m_BranchName;  //Name of branch (if any)
   TList         *m_Histograms;  //Pointer to list supporting Maker histograms

public:
   Bool_t         DrawFruits;	 //Draw fruits?

                  AMSR_Maker();
                  AMSR_Maker(const char *name, const char *title);
   virtual       ~AMSR_Maker();
   virtual void   Finish() = 0;			// interface
   virtual void   Browse(TBrowser *b);
   virtual void   Draw(Option_t *option="");
   virtual void   Clear(Option_t *option="");
   Bool_t         IsFolder() {return kTRUE;}

   //
   // Getters
   //
   TList         *Histograms() {return m_Histograms;}
   TObject       *Fruits()     {return m_Fruits;}
   virtual void   PrintInfo();
   virtual Bool_t Enabled(TObject * obj) {return kTRUE;} // object enabled for drawing?

   //
   // Setters
   //
   virtual void   Save(Int_t save=1);
   virtual void   Make() = 0;			// interface

   ClassDef(AMSR_Maker, 1)   //AMS virtual base class for Makers
};

#endif
