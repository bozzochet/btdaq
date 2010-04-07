//  $Id: AMSDisplay.cxx,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSOnDisplay                                                           //
//                                                                      //
// Utility class to display AMSRoot outline, tracks, clusters...        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <TROOT.h>
#include <TButton.h>
#include <TCanvas.h>
#include <TRootCanvas.h>
#include <TView.h>
#include <TArc.h>
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
#include "AMSNtuple.h"
#include "ControlFrame.h"
#ifndef WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#endif
AMSOnDisplay * gAMSDisplay;


AMSOnDisplay * AMSOnDisplay::_Head=0; 


//_____________________________________________________________________________
AMSOnDisplay::AMSOnDisplay() : TObject(){
   // Default constructor of AMSOnDisplay
   //
   m_logx=kFALSE;
   m_logy=kFALSE;
   m_logz=kFALSE;
   m_idle=false;
   m_theapp=0;
   m_chain=0;
   m_ntuple=0;
   m_ControlFrame=0;
   _Head=this;   
   _cursubdet=0;
   _cursubdetb=0;
   _grset[0]='\0';
   _Begin=0;
   _Sample=10000;
   m_scale=1;
   gAMSDisplay=this;
}


//_____________________________________________________________________________
AMSOnDisplay::AMSOnDisplay(const char *title, AMSNtupleR *file):TObject(){
   m_scale=1;
   m_ControlFrame=0;
   m_chain=0;
   m_theapp=0;
   m_logx=kFALSE;
   m_logy=kFALSE;
   m_logz=kFALSE;
   _grset[0]='\0';
   _Begin=0;
   _Sample=10000;
   m_idle=true;
   _Head=this;
   _cursubdet=0;
   _cursubdetb=0;
   gAMSDisplay=this;
   m_ntuple = file;
   // Constructor of AMSOnDisplay
   //


   m_Canvas = new TCanvas("Canvas", (Text_t*)title,1024,768);
//   m_Canvas->SetCanvasSize(2000,2000);
   //
   // Create pads on the canvas
   //
   m_Canvas->cd();


   Float_t xsep = 0.0;
   //
   // Create title pad
   // ----------------------------
   m_TitlePad = new TPad("TitlePad", "AMSRoot Online display", xsep,0.95, 1.0, 1.0);
   m_TitlePad->Draw();
   m_TitlePad->Modified();
   m_TitlePad->SetFillColor(33);
   m_TitlePad->SetBorderSize(2);
   m_TitlePad->SetBorderMode(1);
   //
   // Create main display pad
   // ----------------------------
   m_Pad = new TPad("HistPad", "AMSRoot histos",xsep,0.05,1,0.95);
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

  if(!m_ntuple)OpenFileCB();
  if(!m_ntuple){
   cerr <<"No file opened, exiting "<<endl;
   exit(1);
  } 


}
 void AMSOnDisplay::Init(){  // Create user interface control pad
   m_ControlFrame=0;
   m_Canvas->cd();
   Draw();
   m_Canvas->cd();
   m_Canvas->Update();
   
   // Create Controlframe
   m_ControlFrame= new AMSControlFrame(gClient->GetRoot(), (TRootCanvas*)m_Canvas->GetCanvasImp(), 400, 200);

}






//_____________________________________________________________________________
void AMSOnDisplay::Draw(Option_t *option)
{
//    Insert current event in graphics pad list



   DrawTitle();
   DrawRunInfo();
   m_Pad->cd();
   if(m_logx)m_Pad->SetLogx(1);
   else  m_Pad->SetLogx(0);
   if(m_logy)m_Pad->SetLogy(1);
   else  m_Pad->SetLogy(0);
   if(m_logz)m_Pad->SetLogz(1);
   else  m_Pad->SetLogz(0);
   m_Canvas->cd();
   

}




//_____________________________________________________________________________
void AMSOnDisplay::DrawTitle(Option_t *option)
{

   static TText * text=0;
   static char atext[255];

   sprintf(atext, "Alpha Magnetic Spectrometer Online Display    %s.Set_%d",gAMSDisplay->getCurSubDet()->GetName(),gAMSDisplay->getCurSubDet()->getCSet());
   sprintf(_grset,"%d",gAMSDisplay->getCurSubDet()->getCSet());
   TVirtualPad * gPadSave = gPad;
   m_TitlePad->cd();

   if (! text) {
	text = new TText(0.5, 0.5, atext);
   }
   else
	text->SetText(0.5, 0.5, atext);

//   text->SetTextFont(7);
   text->SetTextAlign(22);
   text->SetTextSize(0.65);
   text->Draw();

   gPadSave->cd();
}


void AMSOnDisplay::DrawRunInfo(Option_t *option)
{

   static TText * text=0;
   static char atext[255]="Alpha";

   TVirtualPad * gPadSave = gPad;
   m_ObjInfoPad->cd();
   m_ObjInfoPad->Clear();
   atext[0]=0;
   sprintf(atext,"Run %d   EventsProcessed %d / %d / %d %s",m_ntuple->GetRun(),_Begin,m_ntuple->Entries(),m_ntuple->EventNo(),m_ntuple->GetTime());

   if (! text) {
	text = new TText(0.5, 0.38, atext);
   }
   else
	text->SetText(0.5, 0.38,atext);

//   text->SetTextFont(7);
   text->SetTextAlign(22);
   text->SetTextSize(0.6);
   text->Draw();

   gPadSave->cd();
          if(m_ControlFrame){
            m_ControlFrame->SetPosition(_Begin/(m_ntuple->Entries()+1.e-20)*100);
            m_ControlFrame->ShowPosition();
          }
}




void AMSOnDisplay::StartStop(char *buf){
  if(buf)buf[0]='\0';
  if(!m_theapp)return;
   static TText * text=0;
   static char atext2[20]="Stop Timer";
   static char atext1[20]="Start Timer";

   if (! text) {
	if(m_idle)text = new TText(0.5, 0.5, atext1);
	else text = new TText(0.5, 0.5, atext2);
   }
   else{
	if(m_idle)text->SetText(0.5, 0.5, atext1);
	else text = new TText(0.5, 0.5, atext2);
   }
    if(m_idle)m_theapp->RemoveIdleTimer();
    else m_theapp->SetIdleTimer(15,"");
   text->SetTextAlign(22);
   text->SetTextSize(0.55);
   if(!buf){
    gPad->Clear();
    text->Draw();
   }
   else {
	if(m_idle)strcpy(buf, atext1);
	else strcpy(buf,atext2);
   }
   m_idle=!m_idle;
}

void AMSOnDisplay::AddSubDet(  AMSHist & subdet){
   subdet.Book();
   _subdet.push_back(&subdet);
}



Int_t AMSOnDisplay::Dispatch(Int_t subdet, Int_t set){
   if(_subdet[subdet]->getMSet()==0)return subdet; 
   TVirtualPad * gPadSave = gPad;
   m_Pad->cd();
   Int_t retcode;
   _cursubdetb=subdet;
   if(subdet>=0 && subdet<_subdet.size() ){
      int temp=_cursubdet;
      int tempset=_subdet[subdet]->getCSet();
      retcode=_subdet[subdet]->DispatchHist(set);
      _cursubdet=subdet;
      //      cout <<" subdet "<<subdet<<" "<<set<<" "<<tempset<<endl;
      if(set>=0)_subdet[_cursubdet]->getCSet()=set; 
      else _subdet[_cursubdet]->getCSet()=tempset; 
      Draw();
      _subdet[subdet]->getCSet()=retcode;
      _cursubdet=temp;
   }   
   gPadSave->cd();
      m_Canvas->Update();
   return retcode;
}

Int_t AMSOnDisplay::RDispatch(){
  Int_t subdet=_cursubdetb;
   TVirtualPad * gPadSave = gPad;
   m_Pad->cd();
   Int_t retcode;
   _cursubdetb=subdet;
   if(subdet>=0 && subdet<_subdet.size() ){
      int temp=_cursubdet;
      int tempset=_subdet[subdet]->getCSetl();
      if(tempset<0)tempset=_subdet[subdet]->getMSet()-1;
      retcode=_subdet[subdet]->DispatchHist(tempset);
      _cursubdet=subdet;
      //      cout <<" subdet "<<subdet<<" "<<" "<<tempset<<endl;
      Draw();
      _cursubdet=temp;
   }   
   gPadSave->cd();
      m_Canvas->Update();
   return retcode;
}

void AMSOnDisplay::DispatchProcesses(){
  // Check some of the subdet are active
  static Int_t change=1;
  if(_subdet.size()==0){
    cerr <<" AMSOnDisplay::DispatchProcesses-E-NoSubDetectorsFound"<<endl;
    return;
  }
  int active=0;
  for(int i=0;i<_subdet.size();i++){
    if(_subdet[i]->IsActive()){
      active++;
      break;
    }
  }
  if(!active){
    cerr <<"no active subproc"<<endl;
    _cursubdet=0;
    _cursubdetb=0;
    return;
  }
   if(change==0)_cursubdet=(_cursubdet+1)%_subdet.size();
   while(!_subdet[_cursubdet]->IsActive()){
    _cursubdet=(_cursubdet+1)%_subdet.size();
   }
   change=Dispatch(_cursubdet);
}

void AMSOnDisplay::Filled(char *buf){
  if(buf)buf[0]='\0';
   bool filled=Fill();
  static int state=0;
   static TText * text=0;
   static char atext2[20]="Fill";
   static char atext1[20]="Filled";
   sprintf(atext2,"Fill/%d",_Begin);
   sprintf(atext1,"Filled/%d",_Begin);
   if (! text) {
	if(filled)text = new TText(0.5, 0.5, atext1);
	else text = new TText(0.5, 0.5, atext2);
   }
   else{
	if(filled)text->SetText(0.5, 0.5, atext1);
	else text = new TText(0.5, 0.5, atext2);
   }
   text->SetTextAlign(22);
   text->SetTextSize(0.55);
   if(!buf){
    gPad->Clear();
    text->Draw();
   }
   else {
	if(filled)strcpy(buf,atext1);
	else strcpy(buf,atext2);
   }
}



bool AMSOnDisplay::Fill(bool checkonly){
  int _End=m_ntuple->Entries();
  DrawRunInfo();
  if(checkonly)return _Begin>=_End;
  int retcode=1;
  time_t timett1,timett2;
  time(&timett1);
   int cur=0;
   bool filled=false;
  for(int i=_Begin;i<_End;i++){
   int ret= m_ntuple->ReadOneEvent(i);
   if(ret<0){
     cout <<"  interrupt received"<<endl;
     return false;
   }
     _Begin++;
     time(&timett2);
     if(timett2-timett1>0 && (i-cur)>(_End)/100){
         timett1=timett2;
         if(!(_Begin%GetScale())){
           DrawRunInfo();
           m_Canvas->Update();
          }
          else if(m_ControlFrame){
            m_ControlFrame->SetPosition(_Begin/(m_ntuple->Entries()+1.e-20)*100);
            m_ControlFrame->ShowPosition();
          }

         cur=i;
     }
//     if(_Begin>=_End){
//         _Begin=0;
//         i=-1;
//         cout <<"  reset we jopu"<<endl;
//     }
     if(ret){
      filled=true;
      for(int j=0;j<_subdet.size();j++){
       _subdet[j]->Fill(m_ntuple);
      }
    }
  }
  DrawRunInfo();
  m_Canvas->Update();
  return filled;
}


void AMSOnDisplay::Reset(){
     for(int j=0;j<_subdet.size();j++){
      _subdet[j]->Reset();
     }
     _Begin=0;
}


int  AMSOnDisplay::ReLoad(){

#ifdef WIN32
        char cmd[]="cl.exe -c AMSNtupleSelect.C -I%ROOTSYS%\\include  /EHsc /TP";
#else
        static void *handle=0;
        char *CC=getenv("CC");
        if(!CC){
          setenv("CC","g++",0);
        }
        char cmd[]="$CC -I$ROOTSYS/include  -c AMSNtupleSelect.C";
#endif
        int $i=system(cmd);
        if(!$i){
#ifdef WIN32
	char cmd1[]="cl.exe AMSNtupleSelect.obj -o libuser.so /LD /link -nologo -export:gethelper";
#else
#ifdef __APPLE__
         char cmd1[]="ld -init _fgSelect -dylib -ldylib1.o -undefined dynamic_lookup AMSNtupleSelect.o -o libuser.so";
#else
         char cmd1[]="ld -init fgSelect -shared AMSNtupleSelect.o -o libuser.so";
#endif
#endif
         $i=system(cmd1);
         if(!$i){  
#ifdef WIN32
       static HINSTANCE handle=0;
       typedef AMSNtupleHelper * (*MYPROC)(VOID);
      handle=LoadLibrary(".\\libuser.so");
       if(!handle){
          cout <<"  Unable to load lib "<<endl; 
          return 1;
       }
       else {
        MYPROC pa=(MYPROC)GetProcAddress(handle,"gethelper");
        if(pa){
         AMSNtupleHelper::fgHelper= ((pa)()); 
         return 0;
        }  
        return 1;
       }
	 
#else
           if(handle)dlclose(handle);
           if(handle=dlopen("libuser.so",RTLD_NOW)){
              return 0;
           }
           cout <<dlerror()<<endl;
           return 1;
//            gSystem->Load("libuser.so");  //redundant to 
//           gSystem->Unload("libuser.so");  
//           cout <<"result "<<$i<<endl;
//           if($i==-1)$i=1;
//           else $i==0;
//           return $i;
#endif          
  
	 }
        }
        return -1;

}
void AMSOnDisplay::SaveParticleCB()
{
   char fnam[255];
   sprintf(fnam, "%s.%s.pdf",getCurSubDet()->GetName(),getGrSet());
   for(int i=0;i<strlen(fnam);i++){
     if(fnam[i] =='&'){
       for(int j=i;j<strlen(fnam);j++){
         fnam[j]=fnam[j+1];
       }
       i--;
     }
    }

   GetCanvas()->SaveAs(fnam);
   GetCanvas()->Update();          // refresh the screen
}


void AMSOnDisplay::SaveParticleGIF()
{
   char fnam[255];
   sprintf(fnam, "%s.%s.gif",getCurSubDet()->GetName(),getGrSet());
   for(int i=0;i<strlen(fnam);i++){
     if(fnam[i] =='&'){
       for(int j=i;j<strlen(fnam);j++){
         fnam[j]=fnam[j+1];
       }
       i--;
     }
    }
   GetCanvas()->SaveAs(fnam);
   GetCanvas()->Update();          // refresh the screen
}


void AMSOnDisplay::PrintCB()
{
#ifndef WIN32
   pid_t pid = getpid();
#else
   int pid=0;
#endif
   char filename[80];
   sprintf(filename, "/tmp/AMSOnDisplay.%u.ps",pid);
   GetCanvas()->SaveAs(filename);
   GetCanvas()->Update();          // refresh the screen
   char cmd[255];
   sprintf(cmd, "lpr /tmp/AMSOnDisplay.%u.ps",pid);
   system(cmd);
   sprintf(cmd, "rm /tmp/AMSOnDisplay.%u.ps",pid);
   system(cmd);
}


void AMSOnDisplay::ReSizeCanvas(Long_t zoom, bool draw){
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
void AMSOnDisplay::OpenFileCB(){
static const char *gOpenTypes[] = { "Root files", "*.root*",
                                    "All files",    "*",
                                     0,              0 };

  //if(m_idle)m_theapp->StopIdleing();
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
    //if(m_idle)m_theapp->StartIdleing();
    return;
  }
  if(m_ntuple)delete m_ntuple;
  if(m_chain)delete m_chain;
  m_chain=new TChain("AMSRoot");
  m_chain->Add(filename);
  m_ntuple=new AMSNtupleR(m_chain);
  //if(m_idle)m_theapp->StartIdleing();

}

