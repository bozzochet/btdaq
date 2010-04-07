//  $Id: AMSR_Display.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef AMSR_Display_H
#define AMSR_Display_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Display                                                           //
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




#include <TApplication.h>
#ifndef AMSR_VirtualDisplay_H
#include "AMSR_VirtualDisplay.h"
#endif
//#ifndef AMSR_GeometrySetter_H
//#include "AMSR_GeometrySetter.h"
//#endif
#ifndef AMSR_Canvas_H
#include "AMSR_Canvas.h"
#endif
//#include "AMSR_ColorManager.h"
#ifndef ROOT_TGeometry_H
#include <TGeometry.h>
#endif

class TCanvas;
class TPad;
class TRadioButton;
// class TArc;
class TTUBE;
class TNode;
class TPolyLine3D;
class TList;
class AMSR_GeometrySetter;


class AMSR_Display : public AMSR_VirtualDisplay {

private:
   Double_t               fCooCur[2][3];
   Double_t               fCooDef[2][3];
   
   EAMSR_View            m_View;		     //view
   Long_t              m_IdleTime;           //idle time in seconds
   Text_t             *m_IdleCommand;        //command after each idle time
   Bool_t              m_IdleOn;             //IdleTimer is on?
//   Bool_t            m_DrawAllViews;         //Flag True if AllViews selected
//   Bool_t            m_DrawTwoViews;         //Flag True if TwoViews selected
   Bool_t              m_DrawParticles;        //Flag True if particles to be drawn
   Bool_t              m_DrawGeometry;         //Draw geometry?
   Bool_t              m_DrawMoreGeometry;         //Draw geometry?
   TGeometry          *m_Geometry;	     //Pointer to the geometry
   AMSR_GeometrySetter  *m_GeoSetter;	     //Pointer to the geometry setter
// AMSR_KeyNode         *m_KeyNode;		     //Pointer to selected geom nodes

//   AMSR_ColorManager  *m_ColorManager;         //Pointer to the color manager

   Float_t             m_PTcut;                //PT cut to display objects
   Float_t             m_PTcutEGMUNU;          //PT cut for Electrons, Gammas, MUons, Neutrinos
   Double_t             m_Theta;                //Viewing angle theta
   Double_t             m_Phi;                  //Viewing angle phi
// TCanvas            *m_Canvas;               //Pointer to the display canvas
   AMSR_Canvas          *m_Canvas;               //Pointer to the display canvas
   TPad               *m_UCPad;                //Pointer to the UC pad 
   TPad               *m_LogoPad;              //Pointer to the logo pad 
   TPad               *m_TrigPad;              //Pointer to the trigger pad 
   TPad               *m_ButtonPad;            //Pointer to the buttons pad
   TPad               *m_Pad;                  //Pointer to the event display main pad
   TPad               *m_AxisPad[4];           //Pointer to the axis pad
   TPad               *m_TitlePad;             //Pointer to title pad 
   TPad               *m_EventInfoPad;         //Pointer to event info pad 
   TPad               *m_ObjInfoPad;           //Pointer to object info pad 
   TPad               *m_PartInfoPad;           //Pointer to object info pad
   TRadioButton       *m_IdleSwitch;           //Pointer to idle TRadioButton
//   TPad             *m_AxisPad[4];           //Pointers to axis pad 

// ATLFParticle       *m_Particle;             //Pointer to Particle graphics manager
   TList	      *m_ToBeDrawn;	     //List of objects to be drawn

      
public:
                       double & CooDefMin(int i){return fCooDef[0][i];}
                       double & CooDefMax(int i){return fCooDef[0][i];}
                       double & CooCurMin(int i){return fCooCur[0][i];}
                       double & CooCurMax(int i){return fCooCur[0][i];}
                       void ResetCoo(){
                        for(int i=0;i<3;i++){
                          fCooCur[0][i]=fCooDef[0][i];
                          fCooCur[1][i]=fCooDef[1][i];
                        }
                       }
            
                       AMSR_Display();
                       AMSR_Display(const char *title, TGeometry * geo=0, int resx=1024, int resy=768);
   virtual            ~AMSR_Display();
//   virtual void      SetApplication(TApplication* papp){m_theapp=papp;}
   virtual Bool_t      AllViews() {return (m_View == kAllView);}
           EAMSR_View    GetView() {return m_View;}
   virtual void        Clear(Option_t *option="");
   virtual void        DisplayButtons();
//   virtual void      DisplayTrigger(ATLFTrigger *trig);
   virtual Int_t       DistancetoPrimitive(Int_t px, Int_t py);
   virtual void        Draw(Option_t *option="");
   virtual void        DrawAllViews();
	   void        DrawFrontAndSideViews();
// Bool_t              DrawClusters() {return m_DrawClusters;}
   Bool_t              DrawParticles() {return m_DrawParticles;}
//           void        Select();
           void        DrawTitle(Option_t *option="");
           void        DrawEventInfo(Option_t *option="");
           void        DrawCaption(Option_t *option="");
           void        DrawAxis(Int_t index=0, Option_t *option="");
//         void        UpdateAxis(Int_t index=0, Option_t *option="");
   virtual void        DrawView(Double_t theta=9999, Double_t phi=9999, Int_t index=0);
   virtual void        DrawViewGL();
   virtual void        DrawViewX3D();
//	   void        DoubleSize();
//	   void        HalfSize();
           void        AddParticleInfo();
   virtual void        ExecuteEvent(Int_t event, Int_t px, Int_t py);
//   virtual Bool_t      GetEvent(Int_t event); //*MENU*
   virtual Bool_t      GotoRunEvent(); //*MENU*
   Bool_t              Zoom();            
// AMSR_KeyNode         *GetKeyNode() const { return m_KeyNode; }
   TPad               *Pad() {return m_Pad;}
   virtual AMSR_Canvas  *GetCanvas() { return m_Canvas; }
   TPad               *GetObjInfoPad() { return m_ObjInfoPad; }
   TPad               *GetPartInfoPad() { return m_PartInfoPad; }
   TPad               *GetEventInfoPad() { return m_EventInfoPad; }
   TPad               *GetTitlePad() { return m_TitlePad; }
//   AMSR_ColorManager  *GetColorManager() { return m_ColorManager; }
   virtual void        Paint(Option_t *option="");
   virtual void        PaintParticles(Option_t *option="");
   Float_t             PTcut() {return m_PTcut;}
   Float_t             PTcutEGMUNU() {return m_PTcutEGMUNU;}
   virtual char       *GetObjectInfo(Int_t px, Int_t py) const;
   virtual void        SetDrawParticles(Bool_t draw=kTRUE) {m_DrawParticles=draw;} // *MENU*
   virtual void        SetPTcut(Float_t ptcut=0.4); // *MENU*
   virtual void        SetPTcutEGMUNU(Float_t ptcut=5); // *MENU*
   virtual void        SetGeometry(TGeometry * geo); // *--MENU*
           void        SetIdleTimer(Long_t seconds, Text_t *command);
           Bool_t      IsIdleOn() {return m_IdleOn;}
   virtual void        IdleSwitch(Int_t flag=0);
           Long_t      IdleTime() {return m_IdleTime;}
           Text_t     *IdleCommand() {return m_IdleCommand;}
           void        IdleTime(Long_t seconds);
           void        IdleCommand(Text_t *command);
   void                Print(){m_Canvas->PrintCB();}
   void                SavePS(){m_Canvas->SaveParticleCB();}
   void                SaveGIF(){m_Canvas->SaveParticleGIF();}
   virtual void        SetView(Double_t theta, Double_t phi);
           void        SetView(EAMSR_View newView=kFrontView);
           void        SetNextView(EAMSR_View nextView) {m_View=nextView;}
   virtual void        ShowNextEvent(Int_t delta=1);
   virtual void        DrawEvent();
   virtual void        SizeParticles() const;
   //
   // Geometry related functions
   //
   AMSR_GeometrySetter  *GetGeometrySetter() { return m_GeoSetter; }
           void       SetGeo();
   //      void       OkGeo();

   ClassDef(AMSR_Display, 0)   //Utility class to display AMSR_Root outline, tracks,...
			     // Note: class versionID = 0 to inhibit Streamer
			     // because  R__b >> m_Canvas   doesn't compile.
};

extern AMSR_Display *gAMSR_Display;

#endif
