//  $Id: AMSDisplay.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef AMSDisplay_H
#define AMSDisplay_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSDisplay                                                           //
//                                                                      //
// Utility class to display AMSR_Root outline, tracks, and clusters.      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TVirtualX.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGMsgBox.h>
#include <TGMenu.h>
#include <TGCanvas.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TGSlider.h>
#include <TGFileDialog.h>
#include "ControlFrame.h"
#include <TChain.h>
class AMSNtupleV;

#include <TApplication.h>
#include <TGeometry.h>

class TCanvas;
class TPad;
class TRadioButton;
class TTUBE;
class TNode;
class TPolyLine3D;
class TList;
class AMSR_GeometrySetter;
enum EAMSR_View { kNoView=-1,kFrontView=0, kSideView=1, kTopView=2, kAllView=3, kTwoView=4 };
enum EAMSType { kall=-1,kusedonly=0,ktrdtracks=1, ktrdclusters=2,kanticlusters=3,ktofclusters=4,ktrtracks=5,ktrclusters=6,krichrings=7,krichhits=8,kecalshowers=9,kecalclusters=10,kparticles=11,kmcinfo=12,kgeometry=13,ktrig=14};


class AMSDisplay : public TObject {

private:
   Double_t               fCooCur[2][3];
   Double_t               fCooDef[2][3];
   AMSNtupleV *          m_ntuple;   
   TChain *          m_chain;   
   EAMSR_View            m_PrevView;		     //prevview
   EAMSR_View            m_View;		     //view
   bool                  m_zoom;
   bool m_idle;                              // idleing
   bool m_trclpr;                           //  show tr cluster profile
   bool m_drawrichringfromplex;
    bool m_drawsolid;                       //  box style
   TApplication *m_theapp;                   //  application
   TGeometry          *m_Geometry;	     //Pointer to the geometry
   AMSR_GeometrySetter          *m_geosetter;	     //Pointer to the geometry
   int m_selected;                         //  Selected focus
   Bool_t              m_DrawParticles;        //Flag True if particles to be drawn
   Bool_t              m_DrawGeometry;         //Draw geometry?
   Bool_t              m_DrawMoreGeometry;         //Draw geometry?
   Double_t             m_Theta;                //Viewing angle theta
   Double_t             m_Phi;                  //Viewing angle phi
 TCanvas              *m_Canvas;               //Pointer to the display canvas
   TPad               *m_Pad;                  //Pointer to the event display main pad
   TPad                *m_AxisPadP[4];           
   TPad                *m_AxisPad[4];           
   TPad               *m_TitlePad;             //Pointer to title pad 
   TPad               *m_ObjInfoPad;           //Pointer to object info pad 


  double           m_scale;
   AMSControlFrame    *m_ControlFrame;        
   

     bool d_usedonly;
     bool d_trdtracks;
     bool d_trdclusters;
     bool d_anticlusters;
     bool d_tofclusters;
     bool d_trtracks;
     bool d_trclusters;
     bool d_richrings;
     bool d_richhits;
     bool d_ecalshowers;
     bool d_ecalclusters;
     bool d_particles;
     bool d_mcinfo;
     bool d_geometry;


      
public:
               int Focus() const {return m_selected;}
               bool & DrawRichRingsFromPlex(){return m_drawrichringfromplex;}
               bool DrawGeometry()const {return d_geometry;}
               bool DrawUsedOnly()const {return d_usedonly;}
               bool DrawObject(EAMSType type)const;
               TGeometry * GetGeometry()const {return m_Geometry;}
               void SetFocus(int selected);
                       double & CooDefMin(int i){return fCooDef[0][i];}
                       double & CooDefMax(int i){return fCooDef[1][i];}
                       double & CooCurMin(int i){return fCooCur[0][i];}
                       double & CooCurMax(int i){return fCooCur[1][i];}
                       void ResetCoo(){
                        for(int i=0;i<3;i++){
                          fCooCur[0][i]=fCooDef[0][i];
                          fCooCur[1][i]=fCooDef[1][i];
                        }
                       }
            
      AMSDisplay(const char *title, TGeometry * geo, AMSNtupleV *ntuple);
     virtual                  ~AMSDisplay(){};
            Bool_t      AllViews() {return (m_View == kAllView);}
           EAMSR_View    GetView() {return m_View;}
           EAMSR_View    GetPrevView() {return m_PrevView;}
    virtual Int_t       DistancetoPrimitive(Int_t px, Int_t py);
    void        Draw(Option_t *option="");
    void        DrawAllViews();
    void ShowHelp();
	   void        DrawFrontAndSideViews();
   Bool_t              DrawParticles() {return m_DrawParticles;}
           void        DrawTitle(Option_t *option="");
           void        DrawCaption(Option_t *option="");
           void        DrawAxis(Int_t index=0, Option_t *option="");
           void        DrawTrigger();
    bool & ShowTrClProfile(){return m_trclpr;}
    bool & UseSolidStyle(){return m_drawsolid;} 
    void        DrawView(Double_t theta=9999, Double_t phi=9999, Int_t index=0);
    void        DrawViewGL();
    void        DrawViewX3D();
           void        AddParticleInfo(char *obj=0);
    void        ExecuteEvent(Int_t event, Int_t px, Int_t py);
    Bool_t      GotoRunEvent(); //*MENU*
   Bool_t    &          Zoom(){return m_zoom;}            
   TPad               *Pad() {return m_Pad;}
   TCanvas  *GetCanvas() { return m_Canvas; }
   TPad               *GetObjInfoPad() { return m_ObjInfoPad; }
   TPad               *GetTitlePad() { return m_TitlePad; }
     char       *GetObjectInfo(Int_t px, Int_t py) const;
    void        SetDrawParticles(Bool_t draw=kTRUE) {m_DrawParticles=draw;} // *MENU*
    void        SetGeometry(TGeometry * geo); // *--MENU*
    void ReSizeCanvas(Long_t zoom, bool draw);
   void                OpenFileCB();
   void                PrintCB();
   void                SaveParticleCB();
   void                SaveParticleGIF();
           void        SetView(EAMSR_View newView=kFrontView);
           void        ResetView(){m_View=kNoView;}
           void        SetNextView(EAMSR_View nextView) {m_PrevView=m_View;m_View=nextView;}
    void        ShowNextEvent(Int_t delta=1);
    void        DrawEvent();
    int         ReLoad();    // Load UF
    void StartStop(bool idle);
    void SetApplication(TApplication *app){m_theapp=app;};
    void SetVisible(EAMSType type, bool visible);
    void Init();
    AMSNtupleV *GetNtuple()const{return m_ntuple;}
};

extern AMSDisplay *gAMSDisplay;

#endif
