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
#include <iostream>
#include "AMSR_GeometrySetter.h"
#include "TList.h"
#include "TCONE.h"



//_____________________________________________________________________________
AMSR_GeometrySetter::AMSR_GeometrySetter()
{
   // Default constructor of AMSR_GeometrySetter
   //

   m_Geometry = 0;
}

AMSR_GeoToggle AMSR_GeometrySetter::m_Toggle[] = {
 { "TRD9210",              kDrawNone, NULL },
  { "AMSG1",              kDrawSonsOnly, NULL },
  { "FMOT1",              kDrawSonsOnly, NULL },
  { "MMOT11",               kDrawNone, NULL },
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
   TNode * node;
   Int_t i = 0;
   while ( m_Toggle[i].name != 0 ) {
     node = m_Geometry->GetNode(m_Toggle[i].name);
     if (node) node->SetVisibility(m_Toggle[i].vis);
     else cerr<<"No Node "<<m_Toggle[i].name<<endl;
     i++;
   }
   

          TNode *first=(TNode*)m_Geometry->GetListOfNodes()->First();
          TObjLink *lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"TRDT");          
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"TOPR");          
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"TRDW");    
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"STK");      
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"PLA");
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"MS");
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"CR");
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"RA1");
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"RA2");
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"RA3");
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"RA4");
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"US");
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"TRDI");
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"TRCO");
           lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"TRDQ");
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"TRDE");
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"TRB");
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"TRR");




}


void    AMSR_GeometrySetter::recur(TObjLink *lnk, char *exclude, bool what){
         while (lnk) {       
          TNode * obj=(TNode*)lnk->GetObject();              
          const char * name=obj->GetName();
          bool off=true;
          for(int k=0;k<strlen(exclude);k++){
           if(strlen(name)<strlen(exclude) || name[k]!=exclude[k]){
            off=false;
            break;
          }
          }
          if(off){
            obj->SetVisibility(what);
            //cout <<" name "<<name <<(what?" inseted ":" removed ")<<endl;
          }
          if(obj->GetListOfNodes())recur(obj->GetListOfNodes()->FirstLink(),exclude,what); 
          lnk = lnk->Next();                                 
         } 
}

//_____________________________________________________________________________
void AMSR_GeometrySetter::TurnOn(char * name)
{
  //
  // turn on drawing of a node, but not its sons
  //
  TNode * node = m_Geometry->GetNode(name);
  if (node) node->SetVisibility(-2);
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



void AMSR_GeometrySetter::UpdateGeometry(EAMSR_View mview){
  if(mview==kTopView){
          TNode *first=(TNode*)m_Geometry->GetListOfNodes()->First();
          TObjLink *lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"T");          
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"EL");          
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"L");          
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"STK");          
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"RAD");          
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"FOA");          
  }
  else{
         TNode *first=(TNode*)m_Geometry->GetListOfNodes()->First();
          TObjLink *lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"T",true);          
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"EL",true);          
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"L",true);          
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"RAD",true);          
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"FOA",true);          
          lnk = first->GetListOfNodes()->FirstLink();
            recur(lnk,"TRDT");
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"TOPR");          
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"TRDW");
/*
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"STK");
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"PLA");
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"MS");
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"CR");
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"RA1");
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"RA2");
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"RA3");
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"RA4");
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"US");
*/
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"TRDI");
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"TRCO");
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"TRDQ");
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"TRDE");
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"TRR");
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"TRB");




}
}
