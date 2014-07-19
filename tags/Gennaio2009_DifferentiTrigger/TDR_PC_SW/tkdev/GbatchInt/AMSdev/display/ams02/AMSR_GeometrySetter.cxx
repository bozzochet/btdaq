//  $Id: AMSR_GeometrySetter.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_GeometrySetter                                                    //
//                                                                      //
// Utility class to set various options of  AMS geometry                //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TNode.h>
#include <TDialogCanvas.h>
#include "Debugger.h"
#include <iostream.h>
#include "TSwitch.h"
#include "AMSR_GeometrySetter.h"


ClassImp(AMSR_GeometrySetter)



//_____________________________________________________________________________
AMSR_GeometrySetter::AMSR_GeometrySetter()
{
   // Default constructor of AMSR_GeometrySetter
   //

   m_Geometry = 0;
}

AMSR_GeoToggle AMSR_GeometrySetter::m_Toggle[] = {
  { "AMSG1",              kDrawSonsOnly, NULL },
  { "FMOT1",              kDrawSonsOnly, NULL },
  { "CRB11231",                kDrawNone, NULL },
  { "CRB21241",                kDrawNone, NULL },
  { "CRB31251",                kDrawNone, NULL },
  { "CRB41261",                kDrawNone, NULL },
  { "CRB41261",                kDrawNone, NULL },
  { "CRS11271",                kDrawNone, NULL },
  { "CRS21281",                kDrawNone, NULL },
  { "MMOT11",               kDrawNone, NULL },
  { "MSBO1301",             kDrawNone, NULL },
  { "MSBO1312",             kDrawNone, NULL },
  { "MSBO1323",             kDrawNone, NULL },
  { "MSBO1334",             kDrawNone, NULL },
  { "MSBO1345",             kDrawNone, NULL },
  { "MSBO1356",             kDrawNone, NULL },
  { "MSBO1367",             kDrawNone, NULL },
  { "MSBO1378",             kDrawNone, NULL },
  { "MSFR1291",             kDrawNone, NULL },
  { "RA1B1161",             kDrawNone, NULL },
  { "RA1T1172",             kDrawNone, NULL },
  { "RA2B1181",             kDrawNone, NULL },
  { "RA2M1192",             kDrawNone, NULL },
  { "RA2T1203",             kDrawNone, NULL },
  { "RA3T1211",             kDrawNone, NULL },
  { "RA4T1221",             kDrawNone, NULL },
  { "ASTB100",              kDrawNone,     NULL },
  { "PLA2632",              kDrawNone,     NULL },
  { "PLA7637",              kDrawNone,     NULL },
  { "AMOT200",               kDrawNone, NULL },
  { "RICH1",              kDrawImmediateSonsOnly, NULL },
  { "ECMO1",              kDrawImmediateSonsOnly, NULL },
  { "ECEB31",              kDrawNone,     NULL },
  { "ECEB42",              kDrawNone,     NULL },
  { "ECEB53",              kDrawNone,     NULL },
  { "ECEB64",              kDrawNone,     NULL },
  { "ECHN11",              kDrawNone,     NULL },
  { "ECHN22",              kDrawNone,     NULL },
  { "STK11",              kDrawImmediateSonsOnly, NULL },
  { "STK22",              kDrawImmediateSonsOnly, NULL },
  { "STK33",              kDrawImmediateSonsOnly, NULL },
  { "STK44",              kDrawImmediateSonsOnly, NULL },
  { "STK55",              kDrawImmediateSonsOnly, NULL },
  { "STK66",              kDrawImmediateSonsOnly, NULL },
  { "STK77",              kDrawImmediateSonsOnly, NULL },
  { "STK88",              kDrawImmediateSonsOnly, NULL },
  { "US1B1381",              kDrawNone,     NULL },
  { "US1B1392",              kDrawNone,     NULL },
  { "US1B1403",              kDrawNone,     NULL },
  { "US1B1414",              kDrawNone,     NULL },
  { "US2B1421",              kDrawNone,     NULL },
  { "US2B1432",              kDrawNone,     NULL },
  { "US2B1443",              kDrawNone,     NULL },
  { "US2B1454",              kDrawNone,     NULL },
  { "US4B1461",              kDrawNone,     NULL },
  { "US4B1472",              kDrawNone,     NULL },
  { "US4B1483",              kDrawNone,     NULL },
  { "US4B1494",              kDrawNone,     NULL },
  { "USLF1",              kDrawNone,     NULL },
  { "USS23",              kDrawNone,     NULL },
  { "USS32",              kDrawNone,     NULL },
  { 0,                    kDrawNone,     NULL }
};

//_____________________________________________________________________________
AMSR_GeometrySetter::AMSR_GeometrySetter(TGeometry * geo)
{
   // Constructor of AMSR_GeometrySetter
   //
   m_Geometry = geo;
   debugger.Print("+++ AMSR_GeometrySetter::AMSR_GeometrySetter() geo = %lx\n", geo);
   TNode * node;
   Int_t i = 0;
   while ( m_Toggle[i].name != 0 ) {
     node = m_Geometry->GetNode(m_Toggle[i].name);
   debugger.Print("Looking for node %s\n", m_Toggle[i].name);
     if (node) node->SetVisibility(m_Toggle[i].vis);
     else cerr<<"No Node "<<m_Toggle[i].name<<endl;
     i++;
   }
   
}


//_____________________________________________________________________________
void AMSR_GeometrySetter::TurnOn(char * name)
{
  //
  // turn on drawing of a node, but not its sons
  //
  TNode * node = m_Geometry->GetNode(name);
  debugger.Print("got node %lx\n", node);
  if (node) node->SetVisibility(-2);
  if (node) debugger.Print("name = %s\n", node->GetName());
}


//_____________________________________________________________________________
void AMSR_GeometrySetter::TurnOnWithSons(char * name)
{
  //
  // turn on drawing of a node, but not its sons
  //
  TNode * node = m_Geometry->GetNode(name);
  if (node) node->SetVisibility(3);
}


//_____________________________________________________________________________
void AMSR_GeometrySetter::TurnOff(char * name)
{
  //
  // turn off drawing of a node but not its sons
  //
  TNode * node = m_Geometry->GetNode(name);
  if (node) node->SetVisibility(2);
}


//_____________________________________________________________________________
void AMSR_GeometrySetter::TurnOffWithSons(char * name)
{
  //
  // turn off drawing of a node and its sons
  //
  TNode * node = m_Geometry->GetNode(name);
  if (node) node->SetVisibility(-1);
}


//_____________________________________________________________________________
void AMSR_GeometrySetter::CheckVisibility(char * name)
{
  TNode * node = m_Geometry->GetNode(name);
  Int_t vis;
  if (node) printf("%s vis %d\n", node->GetName(), node->GetVisibility());
}


static TDialogCanvas * dialog;
/*
static const Int_t nToggle = 6;
struct myToggle {  
   Bool_t drawIt; 
   TSwitch * sw; 
   char * name; 
};
myToggle toggle[nToggle];
*/
static TButton * okButton;


//______________________________________________________________________________
void AMSR_GeometrySetter::SetGeo()
{
  TVirtualPad * gPadSave = gPad;

  dialog = new TDialogCanvas("setgeo", "Geometry Component On/Off", 740,650);
  if (dialog == 0) return;

  dialog->BuildStandardButtons();

  gPadSave->cd();

/*
  dialog->cd();


  toggle[0].name = "TOF";
  toggle[1].name = "TRACKER";
  toggle[2].name = "CTC";
  toggle[3].name = "ANTI";
  toggle[4].name = "MAGNET";
  toggle[5].name = "SHUTTLE";

  char onMethod[80], offMethod[80];
  for (Int_t i = 0; i < nToggle; i++) {
    sprintf(onMethod, 
      "display->GetGeometrySetter()->TurnOnWithSons(\"%s\")",
	toggle[i].name);
    sprintf(offMethod, 
      "display->GetGeometrySetter()->TurnOffWithSons(\"%s\")",
	toggle[i].name);
    toggle[i].sw= new TSwitch(toggle[i].name, &toggle[i].drawIt,
		onMethod, 0.0, 0.9-i*0.1, 1.0, 1.0-i*0.1, offMethod);
    toggle[i].sw->Draw();
  }

  okButton = new TButton("OK", "display->OkGeo()", 0.3, 0.1, 0.7, 0.2);
  okButton->Draw();
*/
/*
  sw[0] = new TSwitch("ToF", 
   sw[6] = new TSwitch("Geometry", &m_DrawGeometry, 
			"gAMSR_Root->Display()->Draw()", 0.0, y-0.1, 1.0, y);
*/

}

/*
//______________________________________________________________________________
void AMSR_GeometrySetter::SetGeo()
{
  gPadSave = gPad;
  dialog = new TCanvas("setgeo", "Geometry Component On/Off", 14,47,740,650);
  if (dialog == 0) return;

  dialog->cd();


  toggle[0].name = "TOF";
  toggle[1].name = "TRACKER";
  toggle[2].name = "CTC";
  toggle[3].name = "ANTI";
  toggle[4].name = "MAGNET";
  toggle[5].name = "SHUTTLE";

  char onMethod[80], offMethod[80];
  for (Int_t i = 0; i < nToggle; i++) {
    sprintf(onMethod, 
      "display->GetGeometrySetter()->TurnOnWithSons(\"%s\")",
	toggle[i].name);
    sprintf(offMethod, 
      "display->GetGeometrySetter()->TurnOffWithSons(\"%s\")",
	toggle[i].name);
    toggle[i].sw= new TSwitch(toggle[i].name, &toggle[i].drawIt,
		onMethod, 0.0, 0.9-i*0.1, 1.0, 1.0-i*0.1, offMethod);
    toggle[i].sw->Draw();
  }

  okButton = new TButton("OK", "display->OkGeo()", 0.3, 0.1, 0.7, 0.2);
  okButton->Draw();


}
*/
