//  $Id: AMSR_Maker.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS virtual base class for Makers                                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include <TTree.h>
#include <TList.h>
#include <TClonesArray.h>
#include <TBrowser.h>

#include "AMSR_Maker.h"
#include "AMSR_Root.h"

ClassImp(AMSR_Maker)

//_____________________________________________________________________________
AMSR_Maker::AMSR_Maker()
{
   // should never be explicitly used because m_Histograms will stay 0
   //

   m_Save       = 0;
   m_Fruits     = 0;
   m_BranchName = "";
   m_Histograms = 0;
   DrawFruits   = kTRUE;
}

//_____________________________________________________________________________
AMSR_Maker::AMSR_Maker(const char *name, const char *title)
       :TNamed(name,title)
{
   m_Save       = 0;
   m_BranchName = "";
   m_Histograms = new TList();
   DrawFruits   = kTRUE;
   // m_Fruits should be constructed by derived classes
}

//_____________________________________________________________________________
AMSR_Maker::~AMSR_Maker()
{
   //dummy
}

//______________________________________________________________________________
void AMSR_Maker::Browse(TBrowser *b)
{
//  Insert Maker objects in the list of objects to be browsed.

  char name[64];
  if( b == 0  || m_Fruits == 0) return;
  TObject *obj;

// If m_Fruits is a ClonesArray, insert all the objects in the list
// of browsable objects
  if (m_Fruits->InheritsFrom("TClonesArray")) {
     TClonesArray *clones = (TClonesArray*)m_Fruits;
     Int_t nobjects = clones->GetEntries();
     for (Int_t i=0;i<nobjects;i++) {
        obj = clones->At(i);
        sprintf(name,"%s_%d",obj->GetName(),i);
        if (strstr(name,"AMS")) b->Add(obj, &name[4]);
        else                    b->Add(obj, &name[0]);
     }
// m_Fruits points to an object in general. Insert this object in the browser
  } else {
      b->Add( m_Fruits, m_Fruits->GetName());
  }
}

//_____________________________________________________________________________
void AMSR_Maker::Draw(Option_t *option)
{
//    Insert products of this maker in graphics pad list

  if (! DrawFruits) return;

  TObject *obj;
  char* info=0;

// If m_Fruits is a ClonesArray, insert all the objects in the list
// of objects to be painted
  if (m_Fruits->InheritsFrom("TClonesArray")) {
     TClonesArray *clones = (TClonesArray*)m_Fruits;
     Int_t nobjects = clones->GetEntries();
     debugger.Print("AMSR_Maker::Draw():  nobjects = %d\n", nobjects);
     for (Int_t i=0;i<nobjects;i++) {
        obj = clones->At(i);
	if (obj && i==0){ debugger.Print("AMSR_Maker::Draw obj class = %s\n", obj->ClassName());}
        // kludge to skip tracks
	// TString name(obj->ClassName());
        // if (name != TString("AMSR_Track") && obj) obj->AppendPad();
        if (obj && Enabled(obj)) 
	{
	   info = obj->GetObjectInfo(0,0);
//	     debugger.Print("AMSR_Maker::Draw():  info = %s\n", info);
	   obj->AppendPad();
	}
     }
// m_Fruits points to an object in general. Insert this object in the pad
  } else {
     m_Fruits->AppendPad();
  }
}

//_____________________________________________________________________________
void AMSR_Maker::Clear(Option_t *option)
{
  if (m_Fruits) m_Fruits->Delete(option);
}


//_____________________________________________________________________________
void AMSR_Maker::Save(Int_t save)
{
  m_Save = save; 
}


//_____________________________________________________________________________
void AMSR_Maker::PrintInfo()
{
   printf("*********************************\n");
   printf("*                               *\n");
   printf("*     %23s   *\n",GetName());
   printf("*                               *\n");
   printf("*********************************\n");

   Dump();
}


//______________________________________________________________________________
void AMSR_Maker::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSR_Maker.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion();
      TNamed::Streamer(R__b);
      R__b >> m_Save;
      R__b >> m_Fruits;
      m_BranchName.Streamer(R__b);
      R__b >> m_Histograms;
      //this is an addition to the standard rootcint version of Streamer
      //branch address for this maker is set automatically
      TTree *tree = gAMSR_Root->Tree();
      if (tree == 0  || m_Fruits == 0  || m_BranchName.Length() == 0) return;
      if (m_Fruits->InheritsFrom("TClonesArray")) {
         tree->SetBranchAddress(m_BranchName.Data(),  m_Fruits);
      } else {
         tree->SetBranchAddress(m_BranchName.Data(), &m_Fruits);
      }
   } else {
      R__b.WriteVersion(AMSR_Maker::IsA());
      TNamed::Streamer(R__b);
      R__b << m_Save;
      R__b << m_Fruits;
      m_BranchName.Streamer(R__b);
      R__b << m_Histograms;
   }
}
