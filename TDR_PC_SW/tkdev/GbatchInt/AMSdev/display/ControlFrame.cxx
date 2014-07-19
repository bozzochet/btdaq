//  $Id: ControlFrame.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#include "ControlFrame.h"
#include "AMSDisplay.h"
#include "AMSNtupleV.h"
#include <TCanvas.h>
#include <TVirtualX.h>

Bool_t AMSControlFrame::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  char buf[255];
  TGHotString *ptg1=0;
  TGHotString *ptg2=0;
  // Process messages sent to this dialog.
  switch (GET_MSG(msg)) {
  case kC_HSLIDER:
  case kC_VSLIDER:
       if(parm1==9001){
    switch (GET_SUBMSG(msg)) {
      case kSL_POS:
         //Zoom
          gAMSDisplay->ReSizeCanvas(parm2,false);
        break;
      case kSL_RELEASE:
          gAMSDisplay->ReSizeCanvas(parm2,true);
       break;
    }
   }   
  return true;
  case kC_COMMAND:
    switch (GET_SUBMSG(msg)) {
     case kCM_COMBOBOX:
     switch(parm1){
      case 8001:
      gAMSDisplay->SetFocus(_pfocus->GetSelected());
      gAMSDisplay->SetView(gAMSDisplay->GetPrevView());
      break;
     }
     break;     
     case kCM_CHECKBUTTON:
      EAMSType  type;
      switch(parm1){
        case 4005:
        gAMSDisplay->StartStop(_pauto->GetState());
        break;
        case 7001:
         type=kusedonly;
         break;
        case 7002:
         type=ktrdtracks;
         break;
        case 7003:
         type=ktrdclusters;
         break;
        case 7004:
         type=kanticlusters;
         break;
        case 7005:
         type=ktofclusters;
         break;
        case 7006:
         type=ktrtracks;
         break;
        case 7007:
         type=ktrclusters;
         break;
        case 7008:
         type=krichrings;
         break;
        case 7009:
         type=krichhits;
         break;
        case 7010:
         type=kecalshowers;
         break;
        case 7011:
         type=kecalclusters;
         break;
        case 7012:
         type=kparticles;
         break;
        case 7013:
         type=kmcinfo;
         break;
        case 7014:
         type=kgeometry;
          break;
      }
      if(parm1/1000==7){
        gAMSDisplay->SetVisible(type, _pvis[parm1%100-1]->GetState());
        gAMSDisplay->GetNtuple()->Prepare(type);
         EAMSR_View mview=gAMSDisplay->GetView();
       if(type==kgeometry){
           gAMSDisplay->ResetView();
        }
       if(gAMSDisplay->DrawObject(type) || type==kusedonly || type==kgeometry)gAMSDisplay->SetView(mview);
        gAMSDisplay->DrawTitle();
       }
      break;
     case kCM_RADIOBUTTON:
      switch(parm1){
        case 9001:
         if(gAMSDisplay->Zoom()){
           gAMSDisplay->Zoom()=false;
           //EAMSR_View mview=gAMSDisplay->GetView();
           //gAMSDisplay->ResetView();
           //gAMSDisplay->SetView(mview);          
        }
        break;
        case 9002:
         if(!gAMSDisplay->Zoom()){
          gAMSDisplay->Zoom()=true;
           EAMSR_View mview=gAMSDisplay->GetView();
           gAMSDisplay->ResetView();
           gAMSDisplay->SetView(mview);          
         }
        break;
  
      }
      break;
     case kCM_BUTTON:
      switch(parm1){
       case 6001:
       gAMSDisplay->SetView(kTopView); 
       break;
       case 6002:
       gAMSDisplay->SetView(kSideView); 
       break;
       case 6003:
       gAMSDisplay->SetView(kFrontView); 
       break;
       case 6004:
       gAMSDisplay->SetView(kTwoView); 
       break;
//     Control
       case 4001:
       gAMSDisplay->ShowNextEvent(1);
       break;
       case 4002:
       gAMSDisplay->ShowNextEvent(-1);
       break;
       case 4003:
       gAMSDisplay->GotoRunEvent();
       break;
       case 4004:
        int i=gAMSDisplay->ReLoad();
        if(i==1){
         ptg2= new TGHotString("UF Load Failed");
	  _pcontrol[3]->SetText(ptg2);
          _pcontrol[3]->SetState(kButtonDisabled);
        
        }
        else if(i==-1){
  	    ptg2 =new TGHotString("UF Comp Failed");
	  _pcontrol[3]->SetText(ptg2);
        }
        else{
  	   ptg2= new TGHotString("Reload UF");
	  _pcontrol[3]->SetText(ptg2);
        }
       break;
      }
      break;
    case kCM_MENU:
      switch   (parm1){
      case 201:
       gAMSDisplay->ShowHelp();
       break;
      case 101:
       gAMSDisplay->ShowTrClProfile()=!gAMSDisplay->ShowTrClProfile();
        fOptionMenu->DeleteEntry(101);
        if(gAMSDisplay->ShowTrClProfile()){
          fOptionMenu->AddEntry("&Draw TrClusters As Simple Boxes",101,0,0,fOptionMenu->GetEntry(102));
        }
        else{
          fOptionMenu->AddEntry("&Draw TrClusters As Clusters",101,0,0,fOptionMenu->GetEntry(102));
        }
         gAMSDisplay->GetNtuple()->Prepare(ktrclusters);
         gAMSDisplay->SetView(gAMSDisplay->GetView());
         gAMSDisplay->DrawTitle();
        break;
      case 102:
        gAMSDisplay->UseSolidStyle()=!gAMSDisplay->UseSolidStyle();
        fOptionMenu->DeleteEntry(102);
        if(gAMSDisplay->UseSolidStyle()){
         fOptionMenu->AddEntry("&Use Hollow Style",102,0,0,fOptionMenu->GetEntry(103));
        }
        else   fOptionMenu->AddEntry("&Use Solid Style",102,0,0,fOptionMenu->GetEntry(103));
        gAMSDisplay->GetNtuple()->Prepare(kall);
        gAMSDisplay->SetView(gAMSDisplay->GetView());
        gAMSDisplay->DrawTitle();
        break;
      case 103:
        gAMSDisplay->DrawRichRingsFromPlex()=!gAMSDisplay->DrawRichRingsFromPlex();
        fOptionMenu->DeleteEntry(103);
        if(gAMSDisplay->DrawRichRingsFromPlex()){
         fOptionMenu->AddEntry("Do Not Draw &Richrings From Plex",103);
        }
        else   fOptionMenu->AddEntry("Draw &RichRings From Plex",103);
        gAMSDisplay->GetNtuple()->Prepare(krichrings);
        gAMSDisplay->SetView(gAMSDisplay->GetView());
        gAMSDisplay->DrawTitle();
        break;
      case 5:
        gApplication->SetReturnFromRun(0);
        gApplication->Terminate(1);
	break;
      case 1:
	gAMSDisplay->SaveParticleCB();
	break;              
      case 2:
	gAMSDisplay->SaveParticleGIF();
	break;              
      case 3:
	gAMSDisplay->PrintCB();
	break;              
      case 4:
	gAMSDisplay->OpenFileCB();
	gAMSDisplay->DrawEvent();
	break;              
      }
    }
  }
  return kTRUE;
}


void AMSControlFrame::CloseWindow()
{
   // Close dialog in response to window manager close.

   delete this;
}

AMSControlFrame::~AMSControlFrame(){

  

}



AMSControlFrame::AMSControlFrame(const TGWindow *p, const TGWindow *main,
				 UInt_t w, UInt_t h, UInt_t options) :
  TGTransientFrame(p, main, w, h, options){
    int i, ax, ay;
    
    ChangeOptions((GetOptions() & ~kHorizontalFrame) | kVerticalFrame);
    
    //create menus



    fSaveAsMenu=new TGPopupMenu(fClient->GetRoot());
    fSaveAsMenu->AddEntry("Save As run.event.&pdf",1);
    fSaveAsMenu->AddEntry("Save As run.event.&png",2);
    fSaveAsMenu->AddSeparator();
    fSaveAsMenu->AddEntry("P&rint",3);
    fSaveAsMenu->AddSeparator();
    fSaveAsMenu->AddEntry("&New File",4);
    fSaveAsMenu->AddSeparator();
    fSaveAsMenu->AddEntry("&Quit",5);
    
    fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1);
    fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
    fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);
    

    fSaveAsMenu->Associate(this);
    
    fOptionMenu=new TGPopupMenu(fClient->GetRoot());
    fOptionMenu->AddEntry("&Draw TrClusters As Simple Boxes",101);
    fOptionMenu->AddEntry("&Use Hollow Style",102);
    fOptionMenu->AddEntry("Draw &RichRings From Plex",103);
    fOptionMenu->Associate(this);

    fHelpMenu=new TGPopupMenu(fClient->GetRoot());
    fHelpMenu->AddEntry("&Help",201);
    fHelpMenu->Associate(this);
    


   // Create menubar
    fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
    
    fMenuBar->AddPopup("&File", fSaveAsMenu, fMenuBarItemLayout);
    fMenuBar->AddPopup("&Options", fOptionMenu, fMenuBarItemLayout);
    fMenuBar->AddPopup("&Help", fHelpMenu, fMenuBarItemLayout);
    AddFrame(fMenuBar, fMenuBarLayout);
    ULong_t tggcolor,tgbcolor,tgccolor,tgblcolor;
    gClient->GetColorByName("#808080",tggcolor);
    gClient->GetColorByName("#e0e0e0",tgbcolor);
    gClient->GetColorByName("#e08080",tgccolor);
    gClient->GetColorByName("LightBlue",tgblcolor);


 // creat layouts

    fL1 = new TGLayoutHints(kLHintsLeft | kLHintsExpandX |kLHintsExpandY, 0, 0, 2, 2);
    fL2 = new TGLayoutHints(kLHintsLeft | kLHintsExpandX |kLHintsExpandY, 1, 1, 1, 1);
    fL3 = new TGLayoutHints(kLHintsLeft | kLHintsExpandX |kLHintsExpandY, 0,0,0,0);
    fL4 = new TGLayoutHints(kLHintsLeft | kLHintsExpandX |kLHintsExpandY, 1, 1, 1, 1);
    fL5 = new TGLayoutHints(kLHintsLeft | kLHintsExpandX , 1,1,1,1);


//  Create EventControlFrame

/*
     const TString ac("Control");
    _pcontrolfr= new TGButtonGroup(this, 1,4,0,0,ac);
    _pcontrolfr->ChangeBackground(tggcolor);
    _pcontrolfr->SetLayoutHints(fL1);

*/
    _pcontrolfr= new TGGroupFrame(this, new TGString("Controls"));
    _pcontrolfr->ChangeBackground(tggcolor);


//  Add buttons to it

    _pcontrol.push_back(new TGTextButton(_pcontrolfr,"Next Event",4001));
    _pcontrol.push_back(new TGTextButton(_pcontrolfr,"Previous Event",4002));
    _pcontrol.push_back(new TGTextButton(_pcontrolfr,"Select Event",4003));
    _pcontrol.push_back(new TGTextButton(_pcontrolfr,"Load UF",4004));
    _pauto=new TGCheckButton(_pcontrolfr,"AutoScan",4005);
    for(int i=0;i<_pcontrol.size();i++){
         _pcontrol[i]->ChangeBackground(tgccolor);
         _pcontrolfr->AddFrame(_pcontrol[i],fL1);
         _pcontrol[i]->Associate(this);
    }
    _pauto->ChangeBackground(tggcolor);
    _pcontrolfr->AddFrame(_pauto,fL1);
    _pauto->Associate(this);
    
      



//  Create ViewsFrame

    _pviewfr= new TGGroupFrame(this, new TGString("Views"));
    _pviewfr->ChangeBackground(tggcolor);


//  Add buttons to it

    _pview.push_back(new TGTextButton(_pviewfr,"Top View",6001));
    _pview.push_back(new TGTextButton(_pviewfr,"Side View",6002));
    _pview.push_back(new TGTextButton(_pviewfr,"Front View",6003));
    _pview.push_back(new TGTextButton(_pviewfr,"Two Views",6004));
    for(int i=0;i<_pview.size();i++){
         _pview[i]->ChangeBackground(tgblcolor);
         _pviewfr->AddFrame(_pview[i],fL1);
         _pview[i]->Associate(this);
    }
    
      
//  Create VisualizationFrame

    _pvisfr= new TGGroupFrame(this, new TGString("Draw"));
    _pvisfr->ChangeBackground(tggcolor);


//  Add buttons to it

     int marker=7001;
    _pvis.push_back(new TGCheckButton(_pvisfr,"UsedObjectsOnly",marker++));
    _pvis.push_back(new TGCheckButton(_pvisfr,"TrdTracks",marker++));
    _pvis.push_back(new TGCheckButton(_pvisfr,"TrdClusters",marker++));
    _pvis.push_back(new TGCheckButton(_pvisfr,"AntiClusters",marker++));
    _pvis.push_back(new TGCheckButton(_pvisfr,"TofClusters",marker++));
    _pvis.push_back(new TGCheckButton(_pvisfr,"TrTracks",marker++));
    _pvis.push_back(new TGCheckButton(_pvisfr,"TrClusters",marker++));
    _pvis.push_back(new TGCheckButton(_pvisfr,"RichRings",marker++));
    _pvis.push_back(new TGCheckButton(_pvisfr,"RichHits",marker++));
    _pvis.push_back(new TGCheckButton(_pvisfr,"EcalShowers",marker++));
    _pvis.push_back(new TGCheckButton(_pvisfr,"EcalClusters",marker++));
    _pvis.push_back(new TGCheckButton(_pvisfr,"Particles",marker++));
    _pvis.push_back(new TGCheckButton(_pvisfr,"MC Info",marker++));
    _pvis.push_back(new TGCheckButton(_pvisfr,"Geometry",marker++));
//    _pvis.push_back(new TGCheckButton(_pvisfr,"SolidStyle",marker++));
//    _pvis.push_back(new TGCheckButton(_pvisfr,"TrClustersAsBoxes",marker++));
    for(int i=0;i<_pvis.size();i++){
         _pvis[i]->ChangeBackground(tggcolor);
         _pvisfr->AddFrame(_pvis[i],fL1);
         _pvis[i]->Associate(this);
    }
    for(int i=1;i<_pvis.size();i++){
     _pvis[i]->SetState(kButtonDown);
    }
     gAMSDisplay->SetVisible(kusedonly,_pvis[0]->GetState());
     gAMSDisplay->SetVisible(ktrdtracks,_pvis[1]->GetState());
     gAMSDisplay->SetVisible(ktrdclusters,_pvis[2]->GetState());
     gAMSDisplay->SetVisible(kanticlusters,_pvis[3]->GetState());
     gAMSDisplay->SetVisible(ktofclusters,_pvis[4]->GetState());
     gAMSDisplay->SetVisible(ktrtracks,_pvis[5]->GetState());
     gAMSDisplay->SetVisible(ktrclusters,_pvis[6]->GetState());
     gAMSDisplay->SetVisible(krichrings,_pvis[7]->GetState());
     gAMSDisplay->SetVisible(krichhits,_pvis[8]->GetState());
     gAMSDisplay->SetVisible(kecalshowers,_pvis[9]->GetState());
     gAMSDisplay->SetVisible(kecalclusters,_pvis[10]->GetState());
     gAMSDisplay->SetVisible(kparticles,_pvis[11]->GetState());
     gAMSDisplay->SetVisible(kmcinfo,_pvis[12]->GetState());
     gAMSDisplay->SetVisible(kgeometry,_pvis[13]->GetState());

//  Create FocusFrame

    _pfocusfr= new TGGroupFrame(this, new TGString("Focus"));
    _pfocusfr->ChangeBackground(tggcolor);


//  Add buttons to it
/*
     marker=8001;
    _pfocus.push_back(new TGRadioButton(_pfocusfr,"AMS",marker++));
    _pfocus.push_back(new TGRadioButton(_pfocusfr,"TRD",marker++));
    _pfocus.push_back(new TGRadioButton(_pfocusfr,"TOF Top",marker++));
    _pfocus.push_back(new TGRadioButton(_pfocusfr,"Tracker",marker++));
    _pfocus.push_back(new TGRadioButton(_pfocusfr,"TOF Bottom",marker++));
    _pfocus.push_back(new TGRadioButton(_pfocusfr,"RICH",marker++));
    _pfocus.push_back(new TGRadioButton(_pfocusfr,"ECAL",marker++));
    for(int i=0;i<_pfocus.size();i++){
         _pfocus[i]->ChangeBackground(tggcolor);
         _pfocusfr->AddFrame(_pfocus[i],fL1);
         _pfocus[i]->Associate(this);
    }
*/

     _pfocus= new TGComboBox(_pfocusfr,8001);
     int kk=0;
     _pfocus->AddEntry("AMS",kk++);
     _pfocus->AddEntry("TRD",kk++);
     _pfocus->AddEntry("TOF Top",kk++);
     _pfocus->AddEntry("Tracker",kk++);
     _pfocus->AddEntry("TOF Bottom",kk++);
     _pfocus->AddEntry("RICH",kk++);
     _pfocus->AddEntry("ECAL",kk++);
     _pfocus->AddEntry("UserDefined",kk++);
     _pfocus->Select(0);
     _pfocusfr->AddFrame(_pfocus,fL1);
     _pfocus->Associate(this);
// slider
   TGLabel *flabel=new TGLabel(_pfocusfr,"Zoom");
     _pfocusfr->AddFrame(flabel,fL1);
   flabel->ChangeBackground(tggcolor);
   fslider=new TGHSlider(_pfocusfr,150,kSlider1 | kScaleDownRight,9001);
//   fslider=new TGHSlider(_pfocusfr,150,kSlider2 | kScaleDownRight,9001);
   
   fslider->SetRange(0,100*log(3.));
   fslider->SetPosition(0);
   fslider->ChangeBackground(tggcolor);
   fslider->Associate(this);
   _pfocusfr->AddFrame(fslider,fL1);
    
      

//  Create MouseFrame

     const TString am("MouseDrag");
    _pmousefr= new TGButtonGroup(this, 2,1,0,0,am);
    _pmousefr->ChangeBackground(tggcolor);
    

//  Add buttons to it

     marker=9001;
    _pmouse.push_back(new TGRadioButton(_pmousefr,"Rotate",marker++));
    _pmouse.push_back(new TGRadioButton(_pmousefr,"Zoom",marker++));
    for(int i=0;i<_pmouse.size();i++){
         _pmouse[i]->ChangeBackground(tggcolor);
         _pmouse[i]->Associate(this);
    }
    _pmouse[0]->SetState(kButtonDown);    

    
    
    AddFrame(_pcontrolfr,fL5);
    AddFrame(_pviewfr,fL5);
    AddFrame(_pvisfr,fL3);
    AddFrame(_pfocusfr,fL5);
    AddFrame(_pmousefr,fL5);
  
   
    MapSubwindows();
    Resize(158,768);

   // position relative to the parent's window
    Window_t wdum;
    cout <<" main "<<main <<" "<<GetParent()<<endl;
    gVirtualX->TranslateCoordinates(main->GetId(), GetParent()->GetId(),
			       (((TGFrame *) main)->GetWidth()+2 ),0,
			       //(((TGFrame *) main)->GetHeight() ) >> 1,
			       //(((TGFrame *) main)->GetWidth() + fWidth) >> 1,
			       //(((TGFrame *) main)->GetHeight() + fHeight) >> 1,
			       ax, ay, wdum);
    if(ax +fWidth> 1280){
      gVirtualX->TranslateCoordinates(main->GetId(), GetParent()->GetId(),
				 -fWidth-12,0,
				 ax, ay, wdum);
    }
//    Move(ax,ay-26);
    SetWindowName("ControlFrame");
    MapWindow();
    gVirtualX->MoveWindow(fId,ax,ay-26);
}
