//  $Id: TMarker3DCl.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

#include "Riostream.h"
#include "TROOT.h"
#include "TView.h"
#include "TMarker3DCl.h"
#include "TVirtualPad.h"
#include "TVirtualGL.h"
#include "TPadView3D.h"
#include "TH1.h"
#include "TH3.h"
#include "TFile.h"

//remove me when PaintFillArea3D will be in TVirtualPad
//
#include "TPad.h"

ClassImp(TMarker3DCl)

//______________________________________________________________________________
// Marker3DCl is a special 3-D marker designed for event display.
// It has the following parameters:
//    fX;               X coordinate of the center of the box
//    fY;               Y coordinate of the center of the box
//    fZ;               Z coordinate of the center of the box
//    fDx;              half length in X
//    fDy;              half length in Y
//    fDz;              half length in Z
//    fProfileX         array containing cluster profile in X     
//    fProfileY         array containg   cluster profile in Y     
//    fTheta;           Angle of box z axis with respect to main Z axis
//    fPhi;             Angle of box x axis with respect to main Xaxis
//    fRefObject;       A reference to an object
// Begin_Html <P ALIGN=CENTER> <IMG SRC="gif/Marker3DCl.gif"> </P> End_Html




//______________________________________________________________________________
TMarker3DCl::TMarker3DCl()
{
//*-*-*-*-*-*-*-*-*-*-*-*-*Marker3DCl  default constructor*-*-*-*-*-*-*-*-*-*
//*-*                      ================================

   fRefObject = 0;
   //just in case of a stupid call to this constructor
   fDx = 1;
   fDy = 1;
   fDz = 1;

   fX  = 0;
   fY  = 0;
   fZ  = 0;
   fTheta = 0;
   fPhi   = 0;
   fProfileX.clear();
   fProfileY.clear();
   fProfileX.push_back(1);
   fProfileY.push_back(1);
   fNx=1;
   fNy=1; 
   fShowProfileX=true;   
   fShowProfileY=true;   
}


//______________________________________________________________________________
TMarker3DCl::TMarker3DCl( Float_t x, Float_t y, Float_t z,
                            Float_t dx, Float_t dy, Float_t dz,
                            Float_t theta, Float_t phi)
              :TAttLine(1,1,1), TAttFill(1,0)
{
//*-*-*-*-*-*-*-*-*-*-*-*-*Marker3DCl normal constructor*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                      =============================

    fDx = dx;
    fDy = dy;
    fDz = dz;

    fX  = x;
    fY  = y;
    fZ  = z;

    fTheta = theta;
    fPhi   = phi;
    fRefObject = 0;
   fProfileX.clear();
   fProfileY.clear();
   fProfileX.push_back(1);
   fProfileY.push_back(1);
   fNx=1;
   fNy=1; 
   fShowProfileX=true;   
   fShowProfileY=true;   
}


//______________________________________________________________________________
TMarker3DCl::~TMarker3DCl()
{
//*-*-*-*-*-*-*-*-*-*-*-*-*Marker3DCl shape default destructor*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                      =============================

}

void TMarker3DCl::SetShowProfileX(Bool_t set){
  fShowProfileX=set;
 if(set)fNx=fProfileX.size();
 else fNx=1;
}
void TMarker3DCl::SetShowProfileY(Bool_t set){
  fShowProfileY=set;
 if(set)fNy=fProfileY.size();
 else fNy=1;
}


//______________________________________________________________________________
Int_t TMarker3DCl::DistancetoPrimitive(Int_t px, Int_t py){


   const Int_t numPoints = 8;
   Int_t dist = 9999;
   Float_t points[3*numPoints];

   TView *view = gPad->GetView();
   if (!view) return dist;
   Bool_t shprx=fShowProfileX;
   Bool_t shpry=fShowProfileY;
   fNx=1;
   fNy=1;
   fShowProfileX=false;
   fShowProfileY=false;
   SetPoints(points);
   fShowProfileX=shprx;
   fShowProfileY=shpry;
   if(fShowProfileX)fNx=fProfileX.size();
   if(fShowProfileY)fNy=fProfileY.size();
   double  sum[3]={0,0,0};
   Float_t xndc[3];
   for (int i = 0; i < 8; i++) {
      view->WCtoNDC(points+i*3, xndc);
      for(int k=0;k<3;k++)sum[k]+=xndc[k];
   }
   for(int k=0;k<3;k++)sum[k]/=8;
   double x=gPad->XtoAbsPixel(sum[0]);        
   double y=gPad->YtoAbsPixel(sum[1]);        
   dist=sqrt((x-px)*(x-px)+(y-py)*(y-py));
//check if inside return -dist in this case
//

   const Int_t end1[12] = {0,1,2,3,4,5,6,7,0,1,2,3};
   const Int_t end2[12] = {1,2,3,0,5,6,7,4,4,5,6,7};


   Int_t i, j, i1, i2, dsegment;
   Float_t x1,y1,x2,y2;

   Float_t xn[3*numPoints];	// normalized device coordinate of 8 vertices
   Int_t   xd[numPoints],	// device coordinate
           yd[numPoints];

   for (int i = 0; i < numPoints; i++) {
      view->WCtoNDC(&points[3*i], &xn[3*i]);
      xd[i] = gPad->XtoAbsPixel(xn[3*i]);
      yd[i] = gPad->YtoAbsPixel(xn[3*i+1]);
   }

   //
   // each segment can be described as a 2D line y = y0 + slope * ( x - x0 )
   //
   // segment 0, 2, 4, 6 are parallel to each other, i.e., have the same slope
   //         1, 3, 5, 7
   //         8, 9, 10, 11
   //
   Int_t   segs[4*3] = { 0,2,4,6,   1,3,5,7,  8,9,10,11 };
   Float_t slope, intercept[4];
   Float_t ymax, ymin, xmax, xmin;
   Bool_t  inside = kTRUE;
   //
   // loop over 3 sets of parallel segments
   //
   Double_t theta, dmin, dmax, d;
   Int_t dn;
   for ( i = 0; i < 3;  i++ ) {
     Int_t segno = segs[4*i];
     //
     // Calculate theta
     // 
     theta = atan2( Double_t(xd[end2[segno]] - xd[end1[segno]]),
                  - Double_t(yd[end2[segno]] - yd[end1[segno]]) );
     //
     // Calculate dmin and dmax
     //
     dmin = 9999;	dmax = -9999;
     for ( j = 0; j < 4; j++ ) {
       segno = segs[4*i+j];
       d = TMath::Cos(theta) * xd[end1[segno]] +
	   TMath::Sin(theta) * yd[end1[segno]];
       if ( d > dmax )  dmax = d;
       if ( d < dmin )  dmin = d;
     }
     //
     // Include the range
     //
     //
     // Calculate d value of (px,py)
     //
     d = TMath::Cos(theta) * px + TMath::Sin(theta) * py;
     //
     // Compare with dmin and dmax: being outside of one set means outside 
     // of the whole box.
     //
     if ( d > dmax  ||  d < dmin ) {
       inside = kFALSE;
     }
   }
   if(inside)dist=-dist;
   if (dist < 0) {
      gPad->SetCursor(kCross);
   }
   return dist;
}


//______________________________________________________________________________
void TMarker3DCl::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
//*-*-*-*-*-*-*-*-*-*Execute action corresponding to one event*-*-*-*-*-*-*-*-*-*
//*-*                =========================================
//*-*
//*-*  This member function must be implemented to realize the action
//*-*  corresponding to the mouse click on the object in the window
//*-*
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

        if (gPad->GetView()){
                //gPad->GetView()->ExecuteRotateView(event, px, py);
        }

}

//______________________________________________________________________________
void TMarker3DCl::Paint(Option_t *option)
{
//*-*-*-*-*-*-*-*Paint this 3-D box marker with its current attributes*-*-*-*
//*-*            =====================================================


    

    Int_t numpoints = 8*fNx*fNy;
 
//*-* Allocate memory for points *-*

   Float_t *points = new Float_t[3*numpoints];
   if (!points) return;

   SetPoints(points);

   TPadView3D *view3D = (TPadView3D*)gPad->GetView3D();
   Bool_t rangeView = option && *option && strcmp(option,"range")==0 ? kTRUE : kFALSE;
   if (view3D && !rangeView) {
     if(gVirtualGL){
       gVirtualGL->SetLineAttr(GetLineColor(),GetLineWidth());
       PaintGLPoints(points);
     }
   }

   Int_t c = ((GetLineColor() % 8) - 1) * 4;  // Basic colors: 0, 1, ... 7
   if (c < 0) c = 0;

//*-* Allocate memory for segments *-*

    X3DBuffer *buff = new X3DBuffer;
    if (buff) {
        buff->numPoints = numpoints;
        buff->numSegs   = 12;
        buff->numPolys  = 6;
    }

//*-* Allocate memory for points *-*

    buff->points = points;
    buff->segs = new Int_t[buff->numSegs*3];
    if (buff->segs) {
        buff->segs[ 0] = c;    buff->segs[ 1] = 0;    buff->segs[ 2] = 1;
        buff->segs[ 3] = c+1;  buff->segs[ 4] = 1;    buff->segs[ 5] = 2;
        buff->segs[ 6] = c+1;  buff->segs[ 7] = 2;    buff->segs[ 8] = 3;
        buff->segs[ 9] = c;    buff->segs[10] = 3;    buff->segs[11] = 0;
        buff->segs[12] = c+2;  buff->segs[13] = 4;    buff->segs[14] = 5;
        buff->segs[15] = c+2;  buff->segs[16] = 5;    buff->segs[17] = 6;
        buff->segs[18] = c+3;  buff->segs[19] = 6;    buff->segs[20] = 7;
        buff->segs[21] = c+3;  buff->segs[22] = 7;    buff->segs[23] = 4;
        buff->segs[24] = c;    buff->segs[25] = 0;    buff->segs[26] = 4;
        buff->segs[27] = c+2;  buff->segs[28] = 1;    buff->segs[29] = 5;
        buff->segs[30] = c+1;  buff->segs[31] = 2;    buff->segs[32] = 6;
        buff->segs[33] = c+3;  buff->segs[34] = 3;    buff->segs[35] = 7;
    }

//*-* Allocate memory for polygons *-*

    buff->polys = new Int_t[buff->numPolys*6];
    if (buff->polys) {
      int step=0;
        buff->polys[step+ 0] = c;   buff->polys[step+ 1] = 0;  buff->polys[step+ 2] = 1;
        buff->polys[step+ 3] = 5;   buff->polys[step+ 4] = 4;  buff->polys[step+ 5] = 8;
        buff->polys[step+ 6] = c+1; buff->polys[step+ 7] = 1;  buff->polys[step+ 8] = 2;
        buff->polys[step+ 9] = 6;  buff->polys[step+10] = 5;  buff->polys[step+11] = 9;
        buff->polys[step+12] = c;   buff->polys[step+13] = 0;  buff->polys[step+14] = 1;
        buff->polys[step+15] = 2;  buff->polys[step+16] = 3;  buff->polys[step+17] = 10;
        buff->polys[step+18] = c+1; buff->polys[step+19] = 0;  buff->polys[step+20] = 3;
        buff->polys[step+21] = 7;   buff->polys[step+22] = 4;  buff->polys[step+23] = 11;
        buff->polys[step+24] = c+2; buff->polys[step+25] = 3;  buff->polys[step+26] = 2;
        buff->polys[step+27] = 6;   buff->polys[step+28] = 7;  buff->polys[step+29] = 1;
        buff->polys[step+30] = c+3; buff->polys[step+31] = 5;  buff->polys[step+32] = 6;
        buff->polys[step+33] = 7;   buff->polys[step+34] = 4;  buff->polys[step+35] = 7;
    }

    //*-* Paint in the pad
    if(GetFillStyle()==0)PaintShape(buff,rangeView);
    else PaintShape3D(buff,rangeView);

    if (strstr(option, "x3d")) {
        if(buff && buff->points && buff->segs)
            FillX3DBuffer(buff);
        else {
            gSize3D.numPoints -= buff->numPoints;
            gSize3D.numSegs   -= buff->numSegs;
            gSize3D.numPolys  -= buff->numPolys;
        }
    }

    delete [] points;
    if (buff->segs)     delete [] buff->segs;
    if (buff->polys)    delete [] buff->polys;
    if (buff)           delete    buff;


}

//______________________________________________________________________________
void TMarker3DCl::PaintGLPoints(Float_t *vertex)
{
//*-*-*-*-*-*-*-*-*-*-*Direct copy of TBRIK::PaintGLPoints*-*-*-*-*-*-*-*-*-*
//*-*                  ===================================
    gVirtualGL->PaintBrik(vertex);
}
//______________________________________________________________________________
void TMarker3DCl::PaintShape3D(X3DBuffer *buff, Bool_t rangeView)
{
//*-*-*-*-*Paint 3-D marker in current pad with its current attributes*-*-*-*-*
//*-*      ==========================================================

    //*-* Paint in the pad

    Float_t points[12];
    Double_t x0, y0, z0, x1, y1, z1;
    const Int_t kExpandView = 2;
    int i0;

    x0 = y0 = z0 = x1 = y1 = z1 = buff->points[0];

    TAttLine::Modify();  //Change line attributes only if necessary
    TAttFill::Modify();  //Change fill area attributes only if necessary

    for (Int_t i = 0; i < buff->numPolys; i++) {
    for(Int_t kx=0;kx<fNx;kx++){
     for(Int_t ky=0;ky<fNy;ky++){
      Int_t step=24*(kx+ky*fNx);
        i0 = 3*buff->polys[6*i+1];
        points[0] = buff->points[step+i0++];
        points[1] = buff->points[step+i0++];
        points[2] = buff->points[step+i0];

        i0 = 3*buff->polys[6*i+2];
        points[3] = buff->points[step+i0++];
        points[4] = buff->points[step+i0++];
        points[5] = buff->points[step+i0];

        i0 = 3*buff->polys[6*i+3];
        points[6] = buff->points[step+i0++];
        points[7] = buff->points[step+i0++];
        points[8] = buff->points[step+i0];

        i0 = 3*buff->polys[6*i+4];
        points[9] = buff->points[step+i0++];
        points[10] = buff->points[step+i0++];
        points[11] = buff->points[step+i0];
        x0 = points[0] < x0 ? points[0] : x0;
        y0 = points[1] < y0 ? points[1] : y0;
        z0 = points[2] < z0 ? points[2] : z0;
        x1 = points[0] > x1 ? points[0] : x1;
        y1 = points[1] > y1 ? points[1] : y1;
        z1 = points[2] > z1 ? points[2] : z1;

       if (!rangeView) ((TPad*)gPad)->PaintFillArea3D(4,points);
//       cout <<i<<" painting  "<<points[0]<<" "<<points[1]<<" "<<points[2];
//       cout <<" "<<points[4]<<" "<<points[5]<<" "<<points[6]<<endl;
    }
    }
    }
    if (rangeView){
      TView *view = gPad->GetView();
      //if (view->GetAutoRange()) view->SetRange(x0,y0,z0,x1,y1,z1,kExpandView);
    }

}

//______________________________________________________________________________
void TMarker3DCl::PaintShape(X3DBuffer *buff, Bool_t rangeView)
{
//*-*-*-*-*Paint 3-D marker in current pad with its current attributes*-*-*-*-*
//*-*      ==========================================================

    //*-* Paint in the pad

    Double_t points[6], x0, y0, z0, x1, y1, z1;
    const Int_t kExpandView = 2;
    Int_t i0;

    x0 = y0 = z0 = x1 = y1 = z1 = buff->points[0];

    TAttLine::Modify();  //Change line attributes only if necessary
    TAttFill::Modify();  //Change fill area attributes only if necessary

    for (Int_t i = 0; i < buff->numSegs; i++) {
    for(Int_t kx=0;kx<fNx;kx++){
     for(Int_t ky=0;ky<fNy;ky++){
      Int_t step=24*(kx+ky*fNx);
        i0 = 3*buff->segs[3*i+1];
        points[0] = buff->points[step+i0++];
        points[1] = buff->points[step+i0++];
        points[2] = buff->points[step+i0];

        i0 = 3*buff->segs[3*i+2];
        points[3] = buff->points[step+i0++];
        points[4] = buff->points[step+i0++];
        points[5] = buff->points[step+i0];

        x0 = points[0] < x0 ? points[0] : x0;
        y0 = points[1] < y0 ? points[1] : y0;
        z0 = points[2] < z0 ? points[2] : z0;
        x1 = points[0] > x1 ? points[0] : x1;
        y1 = points[1] > y1 ? points[1] : y1;
        z1 = points[2] > z1 ? points[2] : z1;

       if (!rangeView) gPad->PaintLine3D(&points[0], &points[3]);
//       cout <<i<<" painting  "<<points[0]<<" "<<points[1]<<" "<<points[2];
//       cout <<" "<<points[4]<<" "<<points[5]<<" "<<points[6]<<endl;
    }
    }
    }
    if (rangeView){
      TView *view = gPad->GetView();
      if (view->GetAutoRange()) view->SetRange(x0,y0,z0,x1,y1,z1,kExpandView);
    }

}

//______________________________________________________________________________
void TMarker3DCl::SavePrimitive(ofstream &out, Option_t *)
{
    // Save primitive as a C++ statement(s) on output stream out

   out<<"   "<<endl;
   if (gROOT->ClassSaved(TMarker3DCl::Class())) {
       out<<"   ";
   } else {
       out<<"   TMarker3DCl *";
   }
   out<<"marker3DCl = new TMarker3DCl("<<fX<<","
                                         <<fY<<","
                                         <<fZ<<","
                                         <<fDx<<","
                                         <<fDy<<","
                                         <<fDz<<","
                                         <<fTheta<<","
                                         <<fPhi<<");"<<endl;

   SaveLineAttributes(out,"marker3DCl",1,1,1);
   SaveFillAttributes(out,"marker3DCl",1,0);

   out<<"   marker3DCl->Draw();"<<endl;
}

//______________________________________________________________________________
void TMarker3DCl::SetDirection(Float_t theta, Float_t phi)
{
  fTheta = theta;
  fPhi   = phi;
}

//______________________________________________________________________________
void TMarker3DCl::SetSize(Float_t dx, Float_t dy, Float_t dz)
{
  fDx = dx;
  fDy = dy;
  fDz = dz;
}

//______________________________________________________________________________
void TMarker3DCl::SetPosition(Float_t x, Float_t y, Float_t z)
{
  fX  = x;
  fY  = y;
  fZ  = z;
}

//______________________________________________________________________________
void TMarker3DCl::SetPoints(Float_t *buff)
{
//*-*-*-*-*-*-*-*-*-*-*Direct copy of TBRIK::SetPoints *-*-*-*-*-*-*-*-*-*-*
//*-*                  ===============================
/*
    if (buff)
      {
        buff[ 0] = -fDx;    buff[ 1] = -fDy;    buff[ 2] = -fDz;
        buff[ 3] =  fDx;    buff[ 4] = -fDy;    buff[ 5] = -fDz;
        buff[ 6] =  fDx;    buff[ 7] =  fDy;    buff[ 8] = -fDz;
        buff[ 9] = -fDx;    buff[10] =  fDy;    buff[11] = -fDz;
        buff[12] = -fDx;    buff[13] = -fDy;    buff[14] =  fDz;
        buff[15] =  fDx;    buff[16] = -fDy;    buff[17] =  fDz;
        buff[18] =  fDx;    buff[19] =  fDy;    buff[20] =  fDz;
        buff[21] = -fDx;    buff[22] =  fDy;    buff[23] =  fDz;
    }
*/
   if(buff){
    for(int kx=0;kx<fNx;kx++){
        float fx1=-fDx+2*fDx/fNx*kx;
        float fx2=fx1+2*fDx/fNx;
     for(int ky=0;ky<fNy;ky++){
        float fy1=-fDy+2*fDy/fNy*ky;
        float fy2=fy1+2*fDy/fNy;
       buff[24*(kx+ky*fNx)+ 0] = fx1;    buff[24*(kx+ky*fNx)+ 1] = fy1;    buff[24*(kx+ky*fNx)+ 2] = -fDz;
        buff[24*(kx+ky*fNx)+ 3] =  fx2;    buff[24*(kx+ky*fNx)+ 4] = fy1;    buff[24*(kx+ky*fNx)+ 5] = -fDz;
        buff[24*(kx+ky*fNx)+ 6] =  fx2;    buff[24*(kx+ky*fNx)+ 7] =  fy2;    buff[24*(kx+ky*fNx)+ 8] = -fDz;
        buff[24*(kx+ky*fNx)+ 9] = fx1;    buff[24*(kx+ky*fNx)+10] =  fy2;    buff[24*(kx+ky*fNx)+11] = -fDz;
        buff[24*(kx+ky*fNx)+12] = fx1;    buff[24*(kx+ky*fNx)+13] = fy1;    buff[24*(kx+ky*fNx)+14] =  fDz*(fShowProfileX?fProfileX[kx]:1)*(fShowProfileY?fProfileY[ky]:1);
        buff[24*(kx+ky*fNx)+15] =  fx2;    buff[24*(kx+ky*fNx)+16] = fy1;    buff[24*(kx+ky*fNx)+17] =  fDz*(fShowProfileX?fProfileX[kx]:1)*(fShowProfileY?fProfileY[ky]:1);
        buff[24*(kx+ky*fNx)+18] =  fx2;    buff[24*(kx+ky*fNx)+19] =  fy2;    buff[24*(kx+ky*fNx)+20] =  fDz*(fShowProfileX?fProfileX[kx]:1)*(fShowProfileY?fProfileY[ky]:1);
        buff[24*(kx+ky*fNx)+21] = fx1;    buff[24*(kx+ky*fNx)+22] =  fy2;    buff[24*(kx+ky*fNx)+23] =  fDz*(fShowProfileX?fProfileX[kx]:1)*(fShowProfileY?fProfileY[ky]:1);

}
}
      
   Double_t x, y, z;
   const Double_t kPI = TMath::Pi();
   Double_t theta  = fTheta*kPI/180;
   Double_t phi    = fPhi*kPI/180;
   Double_t sinth = TMath::Sin(theta);
   Double_t costh = TMath::Cos(theta);
   Double_t sinfi = TMath::Sin(phi);
   Double_t cosfi = TMath::Cos(phi);

   //
   // Matrix to convert from fruit frame to master frame
   //

   Double_t M[9];
   M[0] =  costh * cosfi;       M[1] = -sinfi;          M[2] = sinth*cosfi;
   M[3] =  costh * sinfi;       M[4] =  cosfi;          M[5] = sinth*sinfi;
   M[6] = -sinth;               M[7] =  0;              M[8] = costh;
    for(int kx=0;kx<fNx;kx++){
     for(int ky=0;ky<fNy;ky++){
      for (Int_t i = 0; i < 8; i++) {
       x = buff[3*i+24*(kx+ky*fNx)];
       y = buff[3*i+24*(kx+ky*fNx)+1];
       z = buff[3*i+24*(kx+ky*fNx)+2];
       buff[3*i+24*(kx+ky*fNx)]   = fX + M[0] * x + M[1] * y + M[2] * z;
       buff[3*i+24*(kx+ky*fNx)+1] = fY + M[3] * x + M[4] * y + M[5] * z;
       buff[3*i+24*(kx+ky*fNx)+2] = fZ + M[6] * x + M[7] * y + M[8] * z;
      }
    }
   }
}

}
//______________________________________________________________________________
void TMarker3DCl::Sizeof3D() const
{
//*-*-*-*-*-*-*Return total X3D size of this marker*-*-*-*-*-*-*-*
//*-*          ====================================

    gSize3D.numPoints += 8*fNx*fNy;
    gSize3D.numSegs   += 12*fNx*fNy;
    gSize3D.numPolys  += 6*fNx*fNy;
}

/*
void TMarker3DCl::Streamer(TBuffer &R__b)
{
   // Stream an object of class TMarker3DCl.

   if (R__b.IsReading()) {
      UInt_t R__s, R__c;
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
      if (R__v > 1) {
         TMarker3DCl::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
         return;
      }
   } else {
      TMarker3DCl::Class()->WriteBuffer(R__b,this);
   }
}



*/
