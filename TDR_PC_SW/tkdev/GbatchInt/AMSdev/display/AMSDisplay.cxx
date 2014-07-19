//  $Id: AMSDisplay.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSDisplay                                                           //
//                                                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <TROOT.h>
#include <TRootCanvas.h>
#include <TButton.h>
#include <TCanvas.h>
#include <TView.h>
#include <TText.h>
#include <TPaveLabel.h>
#include <TPaveText.h>
#include <TList.h>
#include <TDiamond.h>
#include <TNode.h>
#include <TTUBE.h>
#include <TMath.h>
#include <TPolyLine3D.h>
#include <X3DBuffer.h>

#include "AMSDisplay.h"
#include "AMSR_Axis.h"
#include "AMSR_GeometrySetter.h"
#include "TGRunEventDialog.h"
#include "TLatex.h"
#ifndef WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#endif

#include "AMSNtupleV.h"
#include "TAxis3D.h"
#include "Help.h"
AMSDisplay *gAMSDisplay;

AMSDisplay::AMSDisplay(const char *title, TGeometry * geo, AMSNtupleV * ntuple):m_ntuple(ntuple), TObject(){


     fCooDef[0][0]=-115.;
     fCooDef[0][1]=-115.;
     fCooDef[0][2]=-170;
     fCooDef[1][0]=115.;
     fCooDef[1][1]=115.;
     fCooDef[1][2]=170.;
     //double jopa= fCooDef[1][2]+fCooDef[0][2];
     //jopa*=(sqrt(3.)-1)/2.;
     //jopa=0;
     //fCooDef[0][2]+=jopa;
     //fCooDef[1][2]+=jopa;
     ResetCoo();
     m_Geometry     = 0;
     m_selected=0;
     m_scale=1;   
     m_chain=0; 
     m_trclpr=true;
     m_drawrichringfromplex=false;
      m_drawsolid=true;
 
   // Initialize display default parameters
    SetGeometry(geo);

 // Set front view by default
   m_Theta = 90;
   m_Phi   = -90;
   m_View = kTwoView;
   m_zoom=false;
   m_PrevView=kNoView;
   m_DrawParticles = kTRUE;
   m_DrawGeometry  = kTRUE;
   m_DrawMoreGeometry  = kFALSE;

   // Create display canvas
   m_Canvas = new TCanvas("Canvas", (char*)title,1024,768);
   m_Canvas->SetEditable(kFALSE);

   //
   // Create pads on the canvas
   //
   m_Canvas->cd();

   //
   // Logo
   //
   //int const logoSizeX = 127, logoSizeY = 123;
   int const logoSizeX = 130, logoSizeY = 130;
   Int_t logoX0 = m_Canvas->XtoAbsPixel(m_Canvas->GetX1());
   Int_t logoY0 = m_Canvas->YtoAbsPixel(m_Canvas->GetY2()) + logoSizeY;
		// notice the "+" sign.  In AbsPixel large y goes downward
   // Double_t xsep = m_Canvas->AbsPixeltoX(logoSizeX + 4);
   Double_t ysep = m_Canvas->AbsPixeltoY(logoY0 + 4);

   Double_t xsep = 0.;
   m_Canvas->cd();


   //
   // Create title pad
   // ----------------------------
   m_TitlePad = new TPad("TitlePad", "AMS Title", xsep,0.95, 1.0, 1.0);
   m_TitlePad->Draw();
   m_TitlePad->Modified();
   //m_TitlePad->SetFillColor(33);
   m_TitlePad->SetFillColor(33);
   m_TitlePad->SetBorderSize(2);
   m_TitlePad->SetBorderMode(1);
   m_TitlePad->SetEditable(false);
   //
   // Create main display pad
   // ----------------------------
   m_Pad = new TPad("ViewPad", "AMS Event Display",xsep,0.05,1,0.95);
   m_Pad->Modified();
   m_Pad->SetFillColor(0);	//white 
   m_Pad->SetBorderSize(2);
   m_Pad->Draw();
   //
   // Create object info pad
   // ----------------------------
   m_Canvas->cd();
   m_ObjInfoPad = new TPad("ObjInfoPad", "object info pad", xsep, 0, 1, 0.05);
   m_ObjInfoPad->SetFillColor(0);
   m_ObjInfoPad->SetBorderSize(1);
   m_ObjInfoPad->SetBorderMode(2);
   m_ObjInfoPad->Draw();
   m_ObjInfoPad->SetEditable(false);





   m_Canvas->cd();

   m_Canvas->Update();

   gAMSDisplay    = this;



  if(!m_ntuple)OpenFileCB();
  if(!m_ntuple){
   cerr <<"No file opened, exiting "<<endl;
   exit(1);
  } 

}





Int_t AMSDisplay::DistancetoPrimitive(Int_t px, Int_t py){
// Compute distance from point px,py to objects in event
   char *info=m_ntuple->GetObjInfo(px,py);
    AddParticleInfo(info);
       m_ObjInfoPad->Update();
       if(info)return 9999;
       else return -1;
}


void AMSDisplay::Draw(Option_t *option){

//    Insert current event in graphics pad list


   m_ntuple->Prepare();
   m_Pad->cd();
   m_Canvas->Modified();
   if ( m_View == kAllView ) {
      DrawAllViews();
      m_PrevView=m_View;
   }
   else if ( m_View == kTwoView) {
      DrawFrontAndSideViews();
       m_PrevView=m_View;
   }
   else {
      DrawView(m_Theta,m_Phi);
   }

   m_Canvas->cd();
   ((TRootCanvas*)m_Canvas->GetCanvasImp())->Layout();
/*
   static TLine * deco[2]={0,0};

   if ( deco[0] == 0 ) {
     deco[0] = new TLine(0.0,0.0,1.0,0.0);
     deco[0]->SetLineWidth(2);
   }
   if ( deco[1] == 0 ) {
     deco[1] = new TLine(1.0,0.0,1.0,1.0);
     deco[1]->SetLineWidth(2);
   }
   deco[0]->Draw();
   deco[1]->Draw();
*/

    AddParticleInfo();
    DrawTitle();
    m_PrevView=m_View;
}


void AMSDisplay::DrawAllViews(){
//    Draw front,top,side and 30 deg views

  if(m_PrevView!=m_View){
   m_Pad->cd();
   m_Pad->SetFillColor(15);
   m_Pad->Clear();
   m_Pad->Divide(2,2);
  }

   // draw 30 deg view
   m_Pad->cd(1);
   DrawView(30, 30, 0);
   DrawCaption();

   // draw top view
   m_Pad->cd(2);
   DrawView(0, -90, 1);
   DrawCaption("Top");

   // draw front view
   m_Pad->cd(3);

   DrawView(90, 0, 2);
   DrawCaption("Front");

   // draw side view
   m_Pad->cd(4);
   DrawView(90, -90, 3);
   DrawCaption("Side ");

   m_Pad->cd(2);
}

void AMSDisplay::DrawFrontAndSideViews(){

   m_Pad->cd();
   if(m_PrevView!=m_View){
    m_Pad->SetFillColor(15);
    m_Pad->Clear();
    m_Pad->Divide(2,1);
   }
   // draw front view
   m_Pad->cd(1);
   DrawView(90, 0, 0);
   DrawCaption("Front");

   // draw side view
   m_Pad->cd(2);
   DrawView(90, -90, 1);
   DrawCaption("Side");

   m_Pad->cd(1);
}


//_____________________________________________________________________________
void AMSDisplay::DrawTitle(Option_t *option){
   m_TitlePad->SetEditable(true);

   static TText * text=0;
   static char  atext[255];

   

   sprintf(atext,"AMS Event Display         Run %d/ %d %s",m_ntuple->Run(), m_ntuple->Event(),m_ntuple->Time());

   TVirtualPad * gPadSave = gPad;
   m_TitlePad->cd();

   if (! text) {
	text = new TText(0.5, 0.5, atext);
   }
   else
	text->SetText(0.5, 0.5, atext);

   text->SetTextAlign(22);
   text->SetTextSize(0.65);
   text->Draw();
   m_TitlePad->Update();
   m_TitlePad->SetEditable(false);
   gPadSave->cd();
}

void AMSDisplay::AddParticleInfo(char * obj){

   static char  atext[255] = "";
   TVirtualPad * gPadSave = gPad;
   m_ObjInfoPad->cd();
   m_ObjInfoPad->SetEditable(true);
   m_ObjInfoPad->Clear();

   if(obj){
    static TLatex texto;
    texto.SetTextAlign(22);
    texto.SetTextSize(0.35/sqrt(m_scale));
    texto.DrawLatex(0.5,0.25,obj);
   }


   static TLatex text;
   text.SetTextAlign(22);
   text.SetTextSize(0.4/sqrt(m_scale));
   text.DrawLatex(0.5,0.75,m_ntuple->pParticle(0)?m_ntuple->pParticle(0)->Info(0):atext);

   
   m_ObjInfoPad->SetEditable(false);
      
   gPadSave->cd();
}




//_____________________________________________________________________________
void AMSDisplay::DrawCaption(Option_t *option)
{
//    Draw the event title

   Double_t xmin = gPad->GetX1();
   Double_t xmax = gPad->GetX2();
   Double_t ymin = gPad->GetY1();
   Double_t ymax = gPad->GetY2();
   Double_t dx   = xmax-xmin;
   Double_t dy   = ymax-ymin;

   if (strlen(option) == 0) {

   } else {
      TPaveLabel *label = new TPaveLabel(xmin +0.01*dx, ymax-0.15*dy, xmin +0.25*dx, ymax-0.01*dy,option);
      label->SetBit(kCanDelete);
      label->SetFillColor(42);
      label->Draw();
   }
}


//_____________________________________________________________________________
void AMSDisplay::DrawAxis(Int_t index, Option_t * option)
{
   //
   // Draw x-y-z axes in lowerleft corner of current pad
   //
   // This appends the axisPad to the current pad and then updates the axis

   TView *eventView = gPad->GetView();
   Double_t longitude = eventView->GetLongitude();
   Double_t latitude  = eventView->GetLatitude();


   TPad * axisPad = new TPad("axis","axis", 0.0, 0.0, 0.12, 0.12);
					// this will be deleted when
					// the parent pad calls Clear()

   

   axisPad->SetBorderSize(1);
   axisPad->SetBorderMode(1);
   axisPad->Draw();	// append to current pad

   TVirtualPad * gPadSave = gPad;
   axisPad->cd();

   //
   // axes
   //
     AMSR_Axis * axis[3];
     axis[0] = new AMSR_Axis(1,0,0, "x");
     axis[0]->SetLineColor(2);
     axis[1] = new AMSR_Axis(0,1,0, "y");
     axis[1]->SetLineColor(3);
     axis[2] = new AMSR_Axis(0,0,1, "z");
     axis[2]->SetLineColor(4);

   axis[0]->Draw(option);
   axis[1]->Draw(option);
   axis[2]->Draw(option);


   TView * axisView = new TView(1);
   axisView->Hack=kTRUE;
   m_AxisPadP[index] = (TPad*)gPadSave;
   m_AxisPad[index] = axisPad;
   axisView->SetRange(-2.,-2.,-2., 2.,2.,2.);
   Int_t iret;

   axisView->SetView(longitude, latitude, 0, iret);

   //
   // Force update the drawing
   //
   axisPad->Modified();
   axisPad->Update();

   gPadSave->cd();

}



void AMSDisplay::DrawTrigger(){
   //
   // Aux pad to help see Trigger/etc service info in lowerright corner of current pad
   //
   // This appends the axisPad to the current pad and then updates the axis

   TView *eventView = gPad->GetView();
   Double_t longitude = eventView->GetLongitude();
   Double_t latitude  = eventView->GetLatitude();


   TPad * trigPad = new TPad("trig","trig", 0.85, 0.0, 1.0, 0.15);
					// this will be deleted when
					// the parent pad calls Clear()

   

   trigPad->SetBorderSize(1);
   trigPad->SetBorderMode(1);
   trigPad->Draw();	// append to current pad

   TVirtualPad * gPadSave = gPad;
   trigPad->cd();
   m_ntuple->Draw(ktrig);
       
   //
   // Force update the drawing
   //
   trigPad->Modified();
   trigPad->Update();

   gPadSave->cd();

}



void AMSDisplay::DrawView(Double_t theta, Double_t phi, Int_t index){
//    Draw a view of AMS

   static TPaveText * angle=0;
   m_Theta=theta;
   m_Phi=phi; 
   gPad->SetFillColor(10);	//white for easy printing
   TView *view=gPad->GetView();
   if(  m_PrevView!=m_View){
    m_geosetter->UpdateGeometry(m_View);
    gPad->Clear();
    // add itself to the list
   m_ntuple->Draw();
   AppendPad();
   view = new TView(1);
   view->Hack=kTRUE;
   // add the geomtry to the pad
    if(DrawGeometry())m_Geometry->Draw("same");
    view->SetRange(fCooCur[0][0], fCooCur[0][1],fCooCur[0][2],fCooCur[1][0],fCooCur[1][1],fCooCur[1][2]);
           Int_t iret;
     if ( theta != 9999 && phi != 9999 ) view->SetView(phi, theta, 0, iret);
     if(m_zoom){
      TAxis3D *axis=TAxis3D::ToggleZoom(gPad);
     if(axis){
        axis->StickyZoom()=true;
        axis->SetNdivisions(0);
        axis->SetAxisColor(0);
     } 
   }
   }
   else {
    gPad->GetListOfPrimitives()->Remove(this);
    m_ntuple->Draw();
    AppendPad();
   }
     Int_t iret;
     if ( theta != 9999 && phi != 9999 ) view->SetView(phi, theta, 0, iret);

   if( m_PrevView!=m_View){
     DrawAxis(index);
   }
   else if ( theta != 9999 && phi != 9999 ){
    m_AxisPad[index]->GetView()->SetView(phi,theta,0,iret);
    m_AxisPad[index]->Modified();
   }
   gPad->Paint();
}

void AMSDisplay::DrawViewGL(){
//    Draw current view using OPENGL

   TPad *pad = (TPad*)gPad->GetPadSave();
   pad->cd();
   TView *view = pad->GetView();
   if (!view) return;
   pad->x3d("OPENGL");
}

//_____________________________________________________________________________
void AMSDisplay::DrawViewX3D()
{
//    Draw current view using X3D

   TPad *pad = (TPad*)gPad->GetPadSave();
   pad->cd();
   TView *view = pad->GetView();
   if (!view) return;
   pad->x3d();
}

//______________________________________________________________________________
void AMSDisplay::ExecuteEvent(Int_t event, Int_t px, Int_t py){
//*-*-*-*-*-*-*-*-*-*-*Execute action corresponding to one event*-*-*-*
//*-*                  =========================================
   if (gPad->GetView()) {
       if(m_zoom){
         TAxis3D *axis=TAxis3D::GetPadAxis();
         if(axis){
           axis->ExecuteEvent(event,px,py);
           return;
         }
       }
      gPad->GetView()->ExecuteRotateView(event, px, py);
      if (event == kButton1Up) {
        for(int i=0;i<4;i++){
         if(m_AxisPadP[i]==gPad){
          int iret;
          m_AxisPad[i]->GetView()->SetView(gPad->GetView()->GetLatitude(), gPad->GetView()->GetLatitude(),0,iret);
          m_AxisPad[i]->Modified();
//          m_AxisPad[i]->Update();
          break;
         }
        }
      }
   }
     ((TPad*)gPad)->SetMaxPickDistance(0);
}

char *AMSDisplay::GetObjectInfo(Int_t px, Int_t py) const{
   static char info[80];
   sprintf(info, "%d, %d", px, py);
   return info;
}



//_____________________________________________________________________________
void AMSDisplay::SetGeometry(TGeometry * geo){
//  Set AMS in/out outline parameters

   if (m_Geometry!=0 && m_Geometry!=geo) delete m_Geometry;
   m_Geometry = geo;
   m_geosetter=new AMSR_GeometrySetter(geo);
}




void AMSDisplay::SetView(EAMSR_View newView){
   m_PrevView=m_View;
   m_View = newView;
   m_Pad->cd();
   switch (m_View) {
     case kFrontView:  DrawView(90,0,0);   break;
     case kSideView:   DrawView(90,-90,0); break;
     case kTopView:    DrawView(0,-90,0); break;
     case kAllView:    DrawAllViews();  break;
     case kTwoView:    DrawFrontAndSideViews();  break;
     default:          DrawView(90,0,0);   break;
   }
}

Bool_t AMSDisplay::GotoRunEvent(){
   //
   //Prompt a dialog for user to input run/event number and goto
   //
   Int_t run=0, event=0;

   //
   //create a dialog to input run/event
   //
   
   if(m_idle)m_theapp->StopIdleing();
   const TGWindow *main = gClient->GetRoot();
   TRootCanvas *own = (TRootCanvas*)m_Canvas->GetCanvasImp();
   new TGRunEventDialog(main, own, &run, &event);
   if (run==0 && event<=0) {
   if(m_idle)m_theapp->StartIdleing();
     return kFALSE;
   }
   if (run==0) run = m_ntuple->Run();
   bool retn=false;
   if (m_ntuple->GetEvent(run, event)) {
      m_Pad->cd();
      Draw();
      retn=true;
   } 
      if(m_idle)m_theapp->StartIdleing();
   return retn;
}


void AMSDisplay::ShowNextEvent(Int_t delta){
//  Display (current event_number+delta)
//    delta =  1  shown next event
//    delta = -1 show previous event



      int entry=m_ntuple->CurrentEntry()+delta;      
      while(m_ntuple->ReadOneEvent(entry)==0){
         entry+=delta;
      }
      if(entry>=0)DrawEvent();
   }



void AMSDisplay::DrawEvent()
{
  m_Pad->cd(); 
  Draw();
}





void AMSDisplay::SaveParticleCB(){
   char fnam[255];
   sprintf(fnam, "%d.%d.pdf",m_ntuple->Run(),m_ntuple->Event());
   GetCanvas()->SaveAs(fnam);
   GetCanvas()->Update();          // refresh the screen
}


void AMSDisplay::SaveParticleGIF(){
   char fnam[255];
   sprintf(fnam, "%d.%d.png",m_ntuple->Run(),m_ntuple->Event());
   GetCanvas()->SaveAs(fnam);
   GetCanvas()->Update();          // refresh the screen
}


void AMSDisplay::PrintCB(){
#ifndef WIN32
   pid_t pid = getpid();
#else
   int pid=0;
#endif
   char filename[80];
   sprintf(filename, "/tmp/AMSDisplay.%u.ps",pid);
   GetCanvas()->SaveAs(filename);
   GetCanvas()->Update();          // refresh the screen
   char cmd[255];
   sprintf(cmd, "lpr /tmp/AMSDisplay.%u.ps",pid);
   system(cmd);
   sprintf(cmd, "rm /tmp/AMSDisplay.%u.ps",pid);
   system(cmd);
}



int  AMSDisplay::ReLoad(){
#ifndef WIN32
        static void *handle=0;
        char *CC=getenv("CC");
        if(!CC){
          setenv("CC","g++",0);
        }
        char cmd[]="$CC -I$ROOTSYS/include -I../include -c AMSNtupleSelect.C";
        int $i=system(cmd);
        if(!$i){
#ifdef __APPLE__
         char cmd1[]="ld -init _fgSelect -dylib -ldylib1.o -undefined dynamic_lookup AMSNtupleSelect.o -o libuser.so";
#else
         char cmd1[]="ld -init fgSelect  -shared AMSNtupleSelect.o -o libuser.so";
#endif
         $i=system(cmd1);
         if( !$i){  
           if(handle){
             dlclose(handle);
           }
           if(handle=dlopen("libuser.so",RTLD_NOW)){
              return 0;
           }
           cout <<dlerror()<<endl;
           return 1;
            
        }
        }
#else
       static HINSTANCE handle=0;
       typedef AMSNtupleHelper * (*MYPROC)(VOID);
       if(handle){
         FreeLibrary(handle);
         handle=0;
       }
      int i=system("cl.exe -c AMSNtupleSelect.C -I%ROOTSYS%\\include  /EHsc /TP");
      if(!i){
       i=system("cl.exe AMSNtupleSelect.obj -o libuser.so /LD /link -nologo -export:gethelper");
      if(!i){
      handle=LoadLibrary(".\\libuser.so");
       if(!handle){
          cout <<"  Unable to load lib "<<endl; 
          return 1;
       }
       else {
        MYPROC pa=(MYPROC)GetProcAddress(handle,"gethelper");
        if(pa){
         AMSNtupleHelper::fgHelper= ((pa)()); 
         cout << " ok "<< ((pa)())<<endl;
         return 0;
        }  
        return 1;
       }
       }
       }
        
#endif
        return -1;

}



void AMSDisplay::StartStop(bool setidle){
  m_idle=setidle;
  if(m_idle)m_theapp->SetIdleTimer(10,"");
  else  m_theapp->RemoveIdleTimer();
}


void AMSDisplay::SetVisible(EAMSType type, bool visible){
 switch(type){
   case kusedonly:
     d_usedonly=visible;
     break;
   case ktrdtracks:
     d_trdtracks=visible;
     break;
   case ktrdclusters:
     d_trdclusters=visible;
     break;
   case kanticlusters:
     d_anticlusters=visible;
     break;
   case ktofclusters:
     d_tofclusters=visible;
     break;
   case ktrtracks:
     d_trtracks=visible;
     break;
   case ktrclusters:
     d_trclusters=visible;
     break;
   case krichrings:
     d_richrings=visible;
     break;
   case krichhits:
     d_richhits=visible;
     break;
   case kecalshowers:
     d_ecalshowers=visible;
     break;
   case kecalclusters:
     d_ecalclusters=visible;
     break;
   case kparticles:
     d_particles=visible;
     break;
   case kmcinfo:
     d_mcinfo=visible;
     break;
   case kgeometry:
     d_geometry=visible;
     break;
 }
}


bool AMSDisplay::DrawObject(EAMSType type) const{
switch (type){
   case kusedonly:
     return d_usedonly;
   case ktrdtracks:
     return d_trdtracks;
   case ktrdclusters:
     return d_trdclusters;
   case kanticlusters:
     return d_anticlusters;
   case ktofclusters:
     return d_tofclusters;
   case ktrtracks:
     return d_trtracks;
   case ktrclusters:
     return d_trclusters;
   case krichrings:
     return d_richrings;
   case krichhits:
     return d_richhits;
   case kecalshowers:
     return d_ecalshowers;
   case kecalclusters:
     return d_ecalclusters;
   case kparticles:
     return d_particles;
   case kmcinfo:
     return d_mcinfo;
   case kgeometry:
     return d_geometry;
}
    return false;
}



void AMSDisplay::Init(){
   m_ControlFrame= new AMSControlFrame(gClient->GetRoot(), (TRootCanvas*)m_Canvas->GetCanvasImp(), 400, 220);
   m_Canvas->Update();
}



void AMSDisplay::SetFocus(int selected){
  if(m_selected!=selected){
   m_selected=selected;
   if(m_View==kTwoView && selected!=0){
    m_PrevView=kFrontView;
   }
   else m_PrevView=m_View;
   m_View=kNoView;
   m_ntuple->Prepare();
 switch (selected){
  case 0:   //AMS
   ResetCoo();
   break;
  case 1:   //TRD
   fCooCur[0][0]=-110; 
   fCooCur[0][1]=-110; 
   fCooCur[0][2]=80; 
   fCooCur[1][0]=110; 
   fCooCur[1][1]=110; 
   fCooCur[1][2]=165; 
   break;
  case 2:   //TOF Top
   fCooCur[0][0]=-90; 
   fCooCur[0][1]=-90; 
   fCooCur[0][2]=52; 
   fCooCur[1][0]=90; 
   fCooCur[1][1]=90; 
   fCooCur[1][2]=75; 
   break;
  case 3:   //Tracker
   fCooCur[0][0]=-58; 
   fCooCur[0][1]=-58; 
   fCooCur[0][2]=-65; 
   fCooCur[1][0]=58; 
   fCooCur[1][1]=58; 
   fCooCur[1][2]=65; 
   break;
  case 4:   //TOF Bottom
   fCooCur[0][0]=-90; 
   fCooCur[0][1]=-90; 
   fCooCur[0][2]=-75; 
   fCooCur[1][0]=90; 
   fCooCur[1][1]=90; 
   fCooCur[1][2]=-52; 
   break;
  case 5:   //RICH
   fCooCur[0][0]=-69; 
   fCooCur[0][1]=-69; 
   fCooCur[0][2]=-131; 
   fCooCur[1][0]=69; 
   fCooCur[1][1]=69; 
   fCooCur[1][2]=-70; 
   break;
  case 6:   //ECAL
   fCooCur[0][0]=-38; 
   fCooCur[0][1]=-38; 
   fCooCur[0][2]=-162; 
   fCooCur[1][0]=38; 
   fCooCur[1][1]=38; 
   fCooCur[1][2]=-140; 
   break;
 }
}
}



void AMSDisplay::ReSizeCanvas(Long_t zoom, bool draw){
   static double scaleprev=m_scale;
   if(!draw){
    m_scale=exp(double(zoom)/100);
   }
   else if(m_scale){
    UInt_t w=((TRootCanvas*)m_Canvas->GetCanvasImp())->GetCwidth();
    UInt_t h=((TRootCanvas*)m_Canvas->GetCanvasImp())->GetCheight();
    UInt_t wz=w*m_scale/scaleprev;
    UInt_t hz=h*m_scale/scaleprev;
    m_Canvas->SetCanvasSize(wz,hz);
    scaleprev=m_scale;      
//  move canvas say at the center
//
    TGCanvas* frf=((TRootCanvas*)m_Canvas->GetCanvasImp())->GetCanvasWindow();
    frf->SetHsbPosition(wz*(1.-1./m_scale)/2);
    frf->SetVsbPosition(hz*(1.-1./m_scale)/2);
}
}




#include <TGFileDialog.h>
void AMSDisplay::OpenFileCB(){
static const char *gOpenTypes[] = { "Root files", "*.root*",
                                    "All files",    "*",
                                     0,              0 };

  if(m_idle)m_theapp->StopIdleing();
  static TGFileInfo * m_fi = new TGFileInfo;
  //
  // Open a dialog window to select or input filename
  //
  m_fi->fFileTypes = (const char **) gOpenTypes;
  m_fi->fFilename = 0;
  const TGWindow *main = gClient->GetRoot();
  new TGFileDialog(main, (TRootCanvas*)m_Canvas->GetCanvasImp(), kFDOpen, m_fi);


  char *filename = m_fi->fFilename;

  if ( filename==0 || strlen(filename)==0 ) {			// user cancelled
    if(m_idle)m_theapp->StartIdleing();
    return;
  }
  if(m_ntuple)delete m_ntuple;
  if(m_chain)delete m_chain;
  m_chain=new TChain("AMSRoot");
  m_chain->Add(filename);
  m_ntuple=new AMSNtupleV(m_chain);
  if(m_idle)m_theapp->StartIdleing();

}



void AMSDisplay::ShowHelp(){
  static char* fname=0;
  if(!fname){
   char *geo_dir=getenv("AMSGeoDir");
   if ( geo_dir && strlen(geo_dir)  ) {
    fname = new char[256];
   strcpy(fname,geo_dir);
   strcat(fname,"/../00index.txt");
  }
  }
   const TGWindow *main = gClient->GetRoot();
   TRootCanvas *own = (TRootCanvas*)m_Canvas->GetCanvasImp();
   new AMSHelp(main, own, fname);
}
