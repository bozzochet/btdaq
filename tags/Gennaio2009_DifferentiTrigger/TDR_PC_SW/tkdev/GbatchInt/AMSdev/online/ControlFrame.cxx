//  $Id: ControlFrame.cxx,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#include "ControlFrame.h"
#include "AMSDisplay.h"
#include "AMSTOFHist.h"
#include <TCanvas.h>
#include <TVirtualX.h>
ClassImp(AMSControlFrame)

Bool_t AMSControlFrame::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  char buf[255];
  TGHotString *ptg1=0;
  TGHotString *ptg2=0;
  // Process messages sent to this dialog.
  switch (GET_MSG(msg)) {
  case kC_HSLIDER:
  case kC_VSLIDER:
       if(parm1==601){
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
          
    case kCM_CHECKBUTTON:
    case kCM_RADIOBUTTON:
    case kCM_BUTTON:
      if(parm1/100000 == 2){
	gAMSDisplay->Dispatch(parm1%1000);
	return kTRUE;
      }
      if(parm1/100000 == 3){
	(gAMSDisplay->getSubDet(parm1%1000))->SetActive(_pcycle[parm1%1000]->GetState());
	return kTRUE;
      }
      if(parm1/100 == 5){
	switch(parm1%10){
	case 1:
          gAMSDisplay->IsLogX()=_plogx->GetState();
	  break;
	case 2:
          gAMSDisplay->IsLogY()=_plogy->GetState();
	  break;
	case 3:
          gAMSDisplay->IsLogZ()=_plogz->GetState();
	  break;
	  
	}
	gAMSDisplay->RDispatch();
	return kTRUE;
      }
      switch(parm1){
      case 401:
	gAMSDisplay->StartStop(buf);
	if(strlen(buf)){
	  ptg1 =new TGHotString(buf);
	  _pcontrol[0]->SetText(ptg1);
	}
	break;
      case 402:
	gAMSDisplay->Filled(buf);
	if(strlen(buf)){
	  ptg2 =new TGHotString(buf);
	  _pcontrol[1]->SetText(ptg2);
	}
	break;
      case 403:
	gAMSDisplay->Reset();
         _pcontrol[1]->SetText("Fill");
	break;
      case 404:
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
      if(parm1/100000 ==1){
	int sdet=(parm1/1000)%100;
	int set=parm1%1000;
//        cout <<"  get det/set "<<sdet<<" "<<set<<endl;
        gAMSDisplay->Dispatch(sdet,set);
        gAMSDisplay->GetCanvas()->Update();         

	return kTRUE;
      } 
      switch   (parm1){
      case 4:
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

  delete fDetMenu;
  delete fSaveAsMenu;
  for(int i=0;i<_pcontrol.size();i++)delete _pcontrol[i];
  for(int i=0;i<_pbutton.size();i++)delete _pbutton[i];    
  for(int i=0;i<_pcycle.size();i++)delete _pcycle[i];    
  delete _plogx;    
  delete _plogy;
  delete _plogz;
  delete fL1;
  delete fL2;
  delete fL3;
  delete fL4;
  if(fL5)delete fL5;
  delete _pbutfr;
  delete _pcontrolfr;
  delete _plogfr;
  delete _pcyclefr;
  delete _pcombofr;
  
  delete fMenuBar;
  delete fMenuBarLayout;
  delete fMenuBarItemLayout;
  delete fMenuBarHelpLayout;
  

}



AMSControlFrame::AMSControlFrame(const TGWindow *p, const TGWindow *main,
				 UInt_t w, UInt_t h, UInt_t options) :
  TGTransientFrame(p, main, w, h, options){
    int i, ax, ay;
    
    ChangeOptions((GetOptions() & ~kHorizontalFrame) | kVerticalFrame);
    
    //create menus



    fSaveAsMenu=new TGPopupMenu(fClient->GetRoot());
    fSaveAsMenu->AddEntry("Save As SubDetector.Set.&pdf",1);
    fSaveAsMenu->AddEntry("Save As SubDetector.Set.&gif",2);
    fSaveAsMenu->AddSeparator();
    fSaveAsMenu->AddEntry("P&rint",3);
    fSaveAsMenu->AddSeparator();
    fSaveAsMenu->AddEntry("&Quit",4);
    fDetMenu=new TGPopupMenu(fClient->GetRoot());
//    for(i=0;i<gAMSDisplay->getMaxSubDet();i++)
//      fSubDetCascadeMenu.push_back(new TGPopupMenu(fClient->GetRoot()));
    
    for(i=0;i<gAMSDisplay->getMaxSubDet();i++)fSubDetMenu.push_back(new TGPopupMenu(fClient->GetRoot()));
    
    for(i=0;i<fSubDetMenu.size();i++){
      if(i<fSubDetMenu.size()-1){
      for(int j=0;j<gAMSDisplay->getSubDet(i)->getMSet();j++){
        int kpar=100000+1000*i+j;
        fSubDetMenu[i]->AddEntry(gAMSDisplay->getSubDet(i)->GetSetName(j),kpar);
      }
      }
      else{
        TGPopupMenu *fmenu=0;
      for(int j=0;j<gAMSDisplay->getSubDet(i)->getMSet();j++){
        int kpar=100000+1000*i+j;
        if(gAMSDisplay->getSubDet(i)->GetSetName(j)[0]=='_'){
//         cout <<"  fmenu "<<gAMSDisplay->getSubDet(i)->GetSetName(j)<<endl;
         fmenu= new TGPopupMenu(fClient->GetRoot());
         fSubDetMenu[i]->AddPopup(gAMSDisplay->getSubDet(i)->GetSetName(j)+1,fmenu);
         fmenu->AddEntry(gAMSDisplay->getSubDet(i)->GetSetName(j)+1,kpar);
        }
        else{
         fmenu->AddEntry(gAMSDisplay->getSubDet(i)->GetSetName(j),kpar);
        }
      }       
      }      
    }
    
    
    fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1);
    fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
    fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);
    

    fDetMenu->Associate(this);
    fSaveAsMenu->Associate(this);
    for(i=0;i<fSubDetMenu.size();i++)fSubDetMenu[i]->Associate(this);
//    for(i=0;i<fSubDetMenu.size();i++)fSubDetCascadeMenu[i]->Associate(this);
    
    for(i=0;i<fSubDetMenu.size();i++)fDetMenu->AddPopup(gAMSDisplay->getSubDet(i)->GetName(), fSubDetMenu[i]);
     

   // Create menubar
    fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
    
    fMenuBar->AddPopup("&File", fSaveAsMenu, fMenuBarItemLayout);
    fMenuBar->AddPopup("&Control", fDetMenu, fMenuBarItemLayout);
    AddFrame(fMenuBar, fMenuBarLayout);
    ULong_t tggcolor,tgbcolor,tgccolor;
    gClient->GetColorByName("#808080",tggcolor);
    gClient->GetColorByName("#e0e0e0",tgbcolor);
    gClient->GetColorByName("#e08080",tgccolor);

   // Create Group frames
   //  TGCompositeFrame * f2 = new TGCompositeFrame(this, 80, 40, kHorizontalFrame| kSunkenFrame);
    SetWindowAttributes_t wattr;
    _pbutfr= new TGGroupFrame(this, new TGString("Subdetectors"),kVerticalFrame );
    wattr.fMask = kWABackPixel | kWAEventMask;
    wattr.fBackgroundPixel = tggcolor;
    wattr.fEventMask = kExposureMask;
    gVirtualX->ChangeWindowAttributes(_pbutfr->GetId(), &wattr);
    

    _pcontrolfr= new TGGroupFrame(this, new TGString("Controls"));
    _pcontrolfr->ChangeBackground(tggcolor);
/* 
    wattr.fMask = kWABackPixel | kWAEventMask;
    wattr.fBackgroundPixel = tggcolor;
    wattr.fEventMask = kExposureMask;
    gVirtualX->ChangeWindowAttributes(_pcontrolfr->GetId(), &wattr);
*/    
    _pcyclefr= new TGGroupFrame(this, new TGString("Cycle"));
    wattr.fMask = kWABackPixel | kWAEventMask;
    wattr.fBackgroundPixel = tggcolor;
    wattr.fEventMask = kExposureMask;
    
    gVirtualX->ChangeWindowAttributes(_pcyclefr->GetId(), &wattr);
//     const TString a("Scale");
//    _plogfr= new TGButtonGroup(this, 1,3,0,0,a);
    _plogfr= new TGGroupFrame(this, new TGString("Scale"));
    _plogfr->ChangeBackground(tggcolor);
    
    fL1 = new TGLayoutHints(kLHintsLeft | kLHintsExpandX |kLHintsExpandY, 0, 0, 2, 2);
    fL2 = new TGLayoutHints(kLHintsLeft | kLHintsExpandX |kLHintsExpandY, 1, 1, 1, 1);
    fL3 = new TGLayoutHints(kLHintsLeft | kLHintsExpandX |kLHintsExpandY, 0,0,0,0);
    fL4 = new TGLayoutHints(kLHintsLeft | kLHintsExpandX |kLHintsExpandY, 1, 1, 1, 1);
    fL5 = new TGLayoutHints(kLHintsLeft | kLHintsExpandX , 1,1,1,1);
    
   
    for(i=0;i<fSubDetMenu.size();i++){
       _pbutton.push_back(new TGTextButton(_pbutfr, gAMSDisplay->getSubDet(i)->GetName(), 200000+i));
/*
      wattr.fMask = kWABackPixel | kWAEventMask;
      wattr.fBackgroundPixel = tgbcolor;
      wattr.fEventMask = kExposureMask;
      gVirtualX->ChangeWindowAttributes(_pbutton[i]->GetId(), &wattr);
*/      
    }
    //  get subdet from amsdisplay
    for(i=0;i<_pbutton.size();i++){
      _pcycle.push_back(new TGCheckButton(_pcyclefr, gAMSDisplay->getSubDet(i)->GetName(), 300000+i));
/*
      wattr.fMask = kWABackPixel | kWAEventMask;
      wattr.fBackgroundPixel = tggcolor;
      wattr.fEventMask = kExposureMask;
      gVirtualX->ChangeWindowAttributes(_pcycle[i]->GetId(), &wattr);
*/
       _pcycle[i]->ChangeBackground(tggcolor);
    }
    _plogx=new TGCheckButton(_plogfr,"LogX",501);
    _plogy=new TGCheckButton(_plogfr,"LogY",502);
    _plogz=new TGCheckButton(_plogfr,"LogZ",503);
/*
      wattr.fMask = kWABackPixel | kWAEventMask;
      wattr.fBackgroundPixel = tggcolor;
      wattr.fEventMask = kExposureMask;
      gVirtualX->ChangeWindowAttributes(_plogx->GetId(), &wattr);
      gVirtualX->ChangeWindowAttributes(_plogy->GetId(), &wattr);
      gVirtualX->ChangeWindowAttributes(_plogz->GetId(), &wattr);
*/
     _plogx->ChangeBackground(tggcolor);
     _plogy->ChangeBackground(tggcolor);
     _plogz->ChangeBackground(tggcolor);
    for(i=0;i<_pbutton.size();i++)_pbutton[i]->Associate(this);
    for(i=0;i<_pbutton.size();i++)_pcycle[i]->Associate(this);
    _plogx->Associate(this);   
    _plogy->Associate(this);   
    _plogz->Associate(this);   
    for(i=0;i<gAMSDisplay->getMaxSubDet();i++){
      if((gAMSDisplay->getSubDet(i))->IsActive())_pcycle[i]->SetState(kButtonDown);
    }
    _pcontrol.push_back(new TGTextButton(_pcontrolfr,"Stop Timer",401));
    _pcontrol.push_back(new TGTextButton(_pcontrolfr,"Fill",402));
    _pcontrol.push_back(new TGTextButton(_pcontrolfr,"Reset",403));
    _pcontrol.push_back(new TGTextButton(_pcontrolfr,"Load UF",404));

    
    for(i=0;i<_pcontrol.size();i++){
/*
      wattr.fMask = kWABackPixel | kWAEventMask;
      wattr.fBackgroundPixel = tgccolor;
      wattr.fEventMask = kExposureMask;
      gVirtualX->ChangeWindowAttributes(_pcontrol[i]->GetId(), &wattr);
*/
      _pcontrol[i]->ChangeBackground(tgccolor);
    }
    for(i=0;i<_pcontrol.size();i++)_pcontrol[i]->Associate(this);
    for(i=0;i<_pcontrol.size();i++)_pcontrolfr->AddFrame(_pcontrol[i],fL1);
    for(i=0;i<_pbutton.size();i++)_pbutfr->AddFrame(_pbutton[i],fL1);
// slider
//   TGLabel *flabel=new TGLabel(_pcontrolfr,"Zoom");
   fslider=new TGHSlider(_pcontrolfr,150,kSlider1 | kScaleDownRight,601);
//   fslider=new TGHSlider(_pcontrolfr,150,kSlider2 | kScaleDownRight,601);
   
   fslider->SetRange(0,100*log(4.));
   fslider->SetPosition(0);
   fslider->ChangeBackground(tggcolor);
   fslider->Associate(this);
  //   _pcontrolfr->AddFrame(flabel,fL1);
   _pcontrolfr->AddFrame(fslider,fL1);


    for(i=0;i<_pcycle.size();i++)_pcyclefr->AddFrame(_pcycle[i],fL1);
    _plogfr->AddFrame(_plogx,fL1);
    _plogfr->AddFrame(_plogy,fL1);
    _plogfr->AddFrame(_plogz,fL1);
    
    
    AddFrame(_pcontrolfr,fL5);
    AddFrame(_plogfr,fL5);
    AddFrame(_pbutfr,fL4);
    AddFrame(_pcyclefr,fL3);
    fbar=0; 
//  Progress bar
    fbar=new TGHProgressBar(this,TGProgressBar::kStandard,300);
    fbar->SetFillType(TGProgressBar::kBlockFill);
    fbar->SetBarColor("yellow");
    AddFrame(fbar,fL5);
    MapSubwindows();
    Resize(110,768);
   //    _pcontrolfr->Resize(_pcontrolfr->GetWidth(),_pcontrolfr->GetHeight()/2);
   //_plogfr->Resize(GetDefaultSize());

   // position relative to the parent's window
    Window_t wdum;
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
    //Move(ax,ay-26);
    
    SetWindowName("Control");
    MapWindow();
    gVirtualX->MoveWindow(fId,ax,ay-26);
}
