//  $Id: AMSR_Canvas.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef AMSR_Canvas_H
#define AMSR_Canvas_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Canvas                                                            //
//                                                                      //
// Main class to control the canvas.                                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TCanvas.h>
#include <TRootCanvas.h>
#include <TGCanvas.h>
#include <TGFileDialog.h>


class AMSR_Canvas : public TCanvas {

private:
            void      DrawEventStatus(Int_t event, Int_t px, Int_t py, TObject *selected);

protected:
   static TGFileInfo *m_FileInfo;  //File info container for OpenFileCB

public:
   static TRootCanvas * fTheCanvas;
   static   void      AddParticleInfo();
                      AMSR_Canvas() : TCanvas()  {}
                      AMSR_Canvas(Text_t *name, Text_t *title="The AMS Display with Root", Int_t ww=700, Int_t wh=550);
   virtual           ~AMSR_Canvas() {if(m_FileInfo) delete m_FileInfo;}

            void      Update();
            void      EditorBar();
	    void      HandleInput(Int_t event, Int_t px, Int_t py);

   //
   // Call Back functions
   //
   static   void      OpenFileCB();
   static   void      SaveParticleCB();
   static   void      SaveParticleGIF();
   static   void      PrintCB();


   ClassDef(AMSR_Canvas, 0)   //AMS canvas control class
};

#endif
