//  $Id: AMSR_3DCluster.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_3DCluster                                                         //
//                                                                      //
// Cluster class  							//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <math.h>

#include <TROOT.h>
#include <TMath.h>
#include <TView.h>
#include <TPad.h>

#include "AMSR_Display.h"
#include "AMSR_3DCluster.h"
#include "AMSR_Root.h"
#include "Debugger.h"

ClassImp(AMSR_3DCluster)


//_____________________________________________________________________________
AMSR_3DCluster::AMSR_3DCluster()
	: TMarker3DBox(0.0,0.0,0.0,  0.0,0.0,0.0,  0.0,0.0), fRange(5)
{
}


//_____________________________________________________________________________
AMSR_3DCluster::AMSR_3DCluster(Float_t x, Float_t y, Float_t z,
                           Float_t dx, Float_t dy, Float_t dz,
                           Float_t theta, Float_t phi, Int_t range)
        : TMarker3DBox(x,y,z,dx,dy,dz,theta,phi), fRange(range)
{

   //SetBit(kCanDelete);

// const Int_t color[7] = {0,6,3,2,4,7,0};
// const Int_t width[7] = {8,8,8,8,8,8,8};
// const Int_t width[7] = {3,3,3,3,3,3,3};
// Int_t lwidth = width[type];
   Int_t lwidth = 3;
// AMSR_Display *display = (AMSR_Display*)gAMSR_Root->Display();
// if (display->AllViews()) lwidth /= 2;


   SetLineColor(6);
   SetFillColor(6);
   SetFillStyle(1001);			// 1001 is solid filling
   SetLineWidth(lwidth);
}

//_____________________________________________________________________________
AMSR_3DCluster::AMSR_3DCluster(Float_t * xyz, Float_t * dxyz, Float_t * cos, 
			   Int_t range) 
  : TMarker3DBox(xyz[0],xyz[1],xyz[2],dxyz[0],dxyz[1],dxyz[2],0.0,0.0), 
	  fRange(range)
{
   //SetBit(kCanDelete);

// const Int_t color[7] = {0,6,3,2,4,7,0};
// const Int_t width[7] = {8,8,8,8,8,8,8};
// const Int_t width[7] = {3,3,3,3,3,3,3};
// Int_t lwidth = width[type];
   Int_t lwidth = 3;
// AMSR_Display *display = (AMSR_Display*)gAMSR_Root->Display();
// if (display->AllViews()) lwidth /= 2;


   SetLineColor(6);
   SetFillColor(6);
   SetFillStyle(1001);			// 1001 is solid filling
   SetLineWidth(lwidth);
   
   if ( cos == 0 ) return;

   Float_t len = sqrt(cos[0]*cos[0] + cos[1]*cos[1] + cos[2]*cos[2]);


   if (len < 0) return;

   Float_t theta = TMath::ACos(cos[2]/len);
   Float_t phi   = TMath::ATan2(cos[1], cos[0]);
   this->SetDirection(theta,phi);

   debugger.Print("AMSR_3DCluster at (%lf,%lf,%lf)+-(%lf,%lf,%lf) in (%lf,%lf) direction\n", 
	fX, fY, fZ, fDx, fDy, fDz, fTheta, fPhi);
}

//______________________________________________________________________________
 const Float_t * AMSR_3DCluster::GetPosition()
{
////-*-*-*-*-*-*-*-*-*-*set position (x, y, z)*-*-*-*-*-*-*-*-*-*-*
////-*                  ======================
   static Float_t xyz[3];

   xyz[0]  = fX;
   xyz[1]  = fY;
   xyz[2]  = fZ;

   return xyz;
}

//______________________________________________________________________________
 const Float_t * AMSR_3DCluster::GetSize()
{
////-*-*-*-*-*-*-*-*-*-*set position (x, y, z)*-*-*-*-*-*-*-*-*-*-*
////-*                  ======================
   static Float_t xyz[3];

   xyz[0]  = fDx;
   xyz[1]  = fDy;
   xyz[2]  = fDz;

   return xyz;
}

//______________________________________________________________________________
void AMSR_3DCluster::SetPosition(Float_t x, Float_t y, Float_t z)
{
////-*-*-*-*-*-*-*-*-*-*set position (x, y, z)*-*-*-*-*-*-*-*-*-*-*
////-*                  ======================
   TMarker3DBox::SetPosition(x,y,z);
   fX = x;	fY = y;		fZ = z;		// synchronize
}

//______________________________________________________________________________
void AMSR_3DCluster::SetDirection(Float_t theta, Float_t phi)
{
////-*-*-*-*-*-*-*-*-*-*set direction (theta, phi)*-*-*-*-*-*-*-*-*-*-*
////-*                  ==========================

   TMarker3DBox::SetDirection(theta,phi);

}

//______________________________________________________________________________
char * AMSR_3DCluster::GetObjectInfo(Int_t px, Int_t py) const
{
////-*-*-*-*-*-*-*-*-*-*store object info in a string*-*-*-*-*-*-*-*-*-*-*
////-*                  ============================
  static char info[80];

  sprintf(info, 
     "AMSR_3DCluster at (%f,%f,%f)+-(%f,%f,%f) in (%f,%f) direction", 
	fX, fY, fZ, fDx, fDy, fDz, fTheta, fPhi);
  return info;
}

//______________________________________________________________________________
void AMSR_3DCluster::SetPoints(Float_t *buff)
{
////-*-*-*-*-*-*-*-*-*-*save 8 vertex points in buff*-*-*-*-*-*-*-*-*-*-*
////-*                  ============================

    if (buff) {
        buff[ 0] = -fDx;    buff[ 1] = -fDy;    buff[ 2] = -fDz;
        buff[ 3] =  fDx;    buff[ 4] = -fDy;    buff[ 5] = -fDz;
        buff[ 6] =  fDx;    buff[ 7] =  fDy;    buff[ 8] = -fDz;
        buff[ 9] = -fDx;    buff[10] =  fDy;    buff[11] = -fDz;
        buff[12] = -fDx;    buff[13] = -fDy;    buff[14] =  fDz;
        buff[15] =  fDx;    buff[16] = -fDy;    buff[17] =  fDz;
        buff[18] =  fDx;    buff[19] =  fDy;    buff[20] =  fDz;
        buff[21] = -fDx;    buff[22] =  fDy;    buff[23] =  fDz;
    }

   Double_t raddeg = 180.0 / TMath::Pi();

   Double_t theta = fTheta;
   Double_t phi   = fPhi;

   Double_t sinth = TMath::Sin(theta);
   Double_t costh = TMath::Cos(theta);
   Double_t sinfi = TMath::Sin(phi);
   Double_t cosfi = TMath::Cos(phi);

   //
   // Matrix to convert from fruit frame to master frame 
   //
   Float_t M[9];
   M[0] =  costh * cosfi;	M[1] = -sinfi;		M[2] = sinth*cosfi;
   M[3] =  costh * sinfi;	M[4] =  cosfi;		M[5] = sinth*sinfi;
   M[6] = -sinth;		M[7] =  0;		M[8] = costh;

   Float_t x, y, z;
   for (Int_t i = 0; i < 8; i++) {
      x = buff[3*i];
      y = buff[3*i+1];
      z = buff[3*i+2];
//    theta0 = TMath::ACos(z/r);  if (y < 0) theta0 = -theta0;
//    phi0   = TMath::ATan2(y,x);
//    theta  = theta0 + theta1;
//    phi    = phi0   + phi1;
//    sintet = TMath::Sin(theta);
//    sinphi = TMath::Sin(phi);
//      rxy    = r*TMath::Abs(sintet);
//    rxy    = r*sintet;

      buff[3*i]   = fX + M[0] * x + M[1] * y + M[2] * z;
      buff[3*i+1] = fY + M[3] * x + M[4] * y + M[5] * z;
      buff[3*i+2] = fZ + M[6] * x + M[7] * y + M[8] * z;

// printf("i=%d, x=%f, y=%f, z=%f, xp=%f, yp=%f, zp=%f\n",
//       i,x,y,z,buff[3*i],buff[3*i+1],buff[3*i+2]);
   }
}


//______________________________________________________________________________
Int_t AMSR_3DCluster::DistancetoPrimitive(Int_t px, Int_t py)
{
////-*-*-*-*-*-*-*Compute distance from point px,py to a AMSR_3DCluster*-*-*-*-*
////-*            ===================================================
////-*
////-*  Compute if (px,py) is inside the drawn 3D box.  If not, compute the 
////-*  closest distance of approach from point px,py to each corner point of 
////-*  the AMSR_3DCluster.
////-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
   Int_t dist=9999;			// return value

   const Int_t numPoints = 8;
   Float_t points[3*numPoints];

   TView *view = gPad->GetView();
   if (!view) return dist;
   //
   //            4 ____________________ 7
   //             /|                  /|
   //            / |_________________/_|
   //           / / 0               / / 3
   //       5  /_/________________ / /
   //         | /               6 | /
   //         |/__________________|/
   //         1                    2
   //
   // points[] is the world coordinates of 8 vertices.
   // xn[]     is the normalized coordinates of 8 vertices.
   // vertex #0 : (xn[0], xn[1], xn[2])
   // vertex #1 : (xn[3], xn[4], xn[5])
   // .....
   //
   // Each line can be expressed as    \cos\theta x + \sin\theta y = d
   // where theta is the direction perpendicular to the line.
   // Each set of parallel lines can be specified by (theta, d1, d2, d3, d4)
   // Define  dmin = min(d1,d2,d3,d4)  and dmax = max(d1,d2,d3,d4)
   // The region bounded by these 3 sets of parallel lines can be specified by
   //  (theta1, dmin1, dmax1), (theta2, dmin2, dmax2), (theta3, dmin3, dmax3)
   //
   // A point (x,y) is bounded by these lines if and only if
   //     dmin1 <=  cos(theta1) x + sin(theta1) y <= dmax1    &&
   //     dmin2 <=  cos(theta2) x + sin(theta2) y <= dmax2    &&
   //     dmin3 <=  cos(theta3) x + sin(theta3) y <= dmax3
   //
   //
   // end1[i] , end2[i]    are two ends of segment # i
   // such that the coordinates of two ends of segment # i is
   // ( xn[3*end1[i]+0], xn[3*end1[i]+1], xn[3*end1[i]+2] )    and
   // ( xn[3*end2[i]+0], xn[3*end2[i]+1], xn[3*end2[i]+2] )
   //
   //
   const Int_t end1[12] = {0,1,2,3,4,5,6,7,0,1,2,3};
   const Int_t end2[12] = {1,2,3,0,5,6,7,4,4,5,6,7};

   SetPoints(points);

   Int_t i, j, i1, i2, dsegment;
   Float_t x1,y1,x2,y2;

   Float_t xn[3*numPoints];	// normalized device coordinate of 8 vertices
   Int_t   xd[numPoints],	// device coordinate
           yd[numPoints];

   for (i = 0; i < numPoints; i++) {
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
   dist = -1;
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
     dmax += fRange;
     dmin -= fRange;
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
       dn = TMath::Min(Int_t(TMath::Abs(d-dmin)), Int_t(TMath::Abs(d-dmax)));
       dist = TMath::Max(dist, dn);
       //
       // right now the way of calculating dist is not right because when
       // (px,py) is on the extended line of one line segment the distance
       // is set to zero, which it shouldn't.  But it is always non-negative.
       // Before I come up with better algorithms to do it I'll settle with
       // this one.  The returned distance should not be used.
       //
       // break;		// don't break because I want to calculate dist
     }
   }
   if (inside) dist = 0;
   // printf("%s dist = %d\n", (inside)? "inside": "outside", dist);

   if ( inside ) {
      gPad->SetCursor(kCross);
      if (fRefObject) { gPad->SetSelected(fRefObject); }
      //else { debugger.Print("object selected by no fRefObject!\n"); }
      return 0;
   }
   else  return (dist+1);	// dist+1 to make it always positive.

   /*
     Float_t xndc[3];		// normalized coordinate of 8 vertices
     for (i = 0; i < 12; i++) {
        i1 = 3*end1[i];
        view->WCtoNDC(&points[i1], xndc);
        x1 = xndc[0];
        y1 = xndc[1];
  
        i2 = 3*end2[i];
        view->WCtoNDC(&points[i2], xndc);
        x2 = xndc[0];
        y2 = xndc[1];
        dsegment = DistancetoLine(px,py,x1,y1,x2,y2);
        if (dsegment < dist) dist = dsegment;
     }
   if (dist < fRange) {
      gPad->SetCursor(kCross);
      if (fRefObject) {gPad->SetSelected(fRefObject); return 0;}
   }
   return dist;
   */

}


/*
//______________________________________________________________________________
Int_t AMSR_3DCluster::DistancetoPrimitive(Int_t px, Int_t py)
{
////-*-*-*-*-*-*-*Compute distance from point px,py to a AMSR_3DCluster*-*-*-*-*
////-*            ===================================================
////-*
////-*  Compute the closest distance of approach from point px,py to each corner
////-*  point of the AMSR_3DCluster.
////-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   const Int_t numPoints = 8;
   Int_t dist = 9999;
   Float_t points[3*numPoints];

   TView *view = gPad->GetView();
   if (!view) return dist;
   const Int_t seg1[12] = {0,1,2,3,4,5,6,7,0,1,2,3};
   const Int_t seg2[12] = {1,2,3,0,5,6,7,4,4,5,6,7};

   SetPoints(points);

   Int_t i, i1, i2, dsegment;
   Float_t x1,y1,x2,y2, xndc[3];
   for (i = 0; i < 12; i++) {
      i1 = 3*seg1[i];
      view->WCtoNDC(&points[i1], xndc);
      x1 = xndc[0];
      y1 = xndc[1];

      i2 = 3*seg2[i];
      view->WCtoNDC(&points[i2], xndc);
      x2 = xndc[0];
      y2 = xndc[1];
      dsegment = DistancetoLine(px,py,x1,y1,x2,y2);
      if (dsegment < dist) dist = dsegment;
   }
   if (dist < 5) {
      gPad->SetCursor(kCross);
      if (fRefObject) {gPad->SetSelected(fRefObject); return 0;}
   }
   return dist;
}
*/

/*

//______________________________________________________________________________
 void AMSR_3DCluster::Paint(Option_t *option)
{
////-*-*-*-*-*-*-*Paint this 3-D box marker with its current attributes*-*-*-*
////-*            =====================================================

   char *matname = "NONE";
   const Int_t numpoints = 8;

////-* Allocate memory for points *-*

   Float_t *points = new Float_t[3*numpoints];
   if (!points) return;

   SetPoints(points);


#ifdef NEVER
   Double_t rotate[16] = {fDirCos[0],      0,        0,     fTranslation[0],
                               0    ,  fDirCos[1],  0,      fTranslation[1],
                               0    ,      0,   fDirCos[2], fTranslation[2],
                               0    ,      0,       0,           1
                          };
   if (gPad->GetView3D()) {
     gGLKernel->UpdateMatrix(0,rotate);
     gGLKernel->SetLineAttr(GetLineColor(),GetLineWidth());
     PaintGLPoints(points);
   }
#endif

   Int_t c = ((GetLineColor() % 8) - 1) * 4;  // Basic colors: 0, 1, ... 7
   if (c < 0) c = 0;

////-* Allocate memory for segments *-*

    X3DBuffer *buff = new X3DBuffer;
    if (buff) {
        buff->numPoints = 8;
        buff->numSegs   = 12;
        buff->numPolys  = 6;
    }

////-* Allocate memory for points *-*

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

////-* Allocate memory for polygons *-*

    buff->polys = new Int_t[buff->numPolys*6];
    if (buff->polys) {
        buff->polys[ 0] = c;   buff->polys[ 1] = 4;  buff->polys[ 2] = 0;
        buff->polys[ 3] = 9;   buff->polys[ 4] = 4;  buff->polys[ 5] = 8;
        buff->polys[ 6] = c+1; buff->polys[ 7] = 4;  buff->polys[ 8] = 1;
        buff->polys[ 9] = 10;  buff->polys[10] = 5;  buff->polys[11] = 9;
        buff->polys[12] = c;   buff->polys[13] = 4;  buff->polys[14] = 2;
        buff->polys[15] = 11;  buff->polys[16] = 6;  buff->polys[17] = 10;
        buff->polys[18] = c+1; buff->polys[19] = 4;  buff->polys[20] = 3;
        buff->polys[21] = 8;   buff->polys[22] = 7;  buff->polys[23] = 11;
        buff->polys[24] = c+2; buff->polys[25] = 4;  buff->polys[26] = 0;
        buff->polys[27] = 3;   buff->polys[28] = 2;  buff->polys[29] = 1;
        buff->polys[30] = c+3; buff->polys[31] = 4;  buff->polys[32] = 4;
        buff->polys[33] = 5;   buff->polys[34] = 6;  buff->polys[35] = 7;
    }

    ////-* Paint in the pad
    PaintShape(buff);

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
 void AMSR_3DCluster::PaintShape(X3DBuffer *buff)
{
////-*-*-*-*Paint 3-D marker in current pad with its current attributes*-*-*-*-*
////-*      ==========================================================

    ////-* Paint in the pad

    Float_t points[6];
    int i0;

    TAttLine::Modify();  //Change line attributes only if necessary
    TAttFill::Modify();  //Change fill area attributes only if necessary

    for (Int_t i = 0; i < buff->numSegs; i++) {
        i0 = 3*buff->segs[3*i+1];
        points[0] = buff->points[i0++];
        points[1] = buff->points[i0++];
        points[2] = buff->points[i0];

        i0 = 3*buff->segs[3*i+2];
        points[3] = buff->points[i0++];
        points[4] = buff->points[i0++];
        points[5] = buff->points[i0];

        gPad->PaintLine3D(&points[0], &points[3]);
    }
}
*/

//______________________________________________________________________________
void AMSR_3DCluster::Sizeof3D() const
{
   const Int_t npoints = 2;
   gSize3D.numPoints += npoints;
   gSize3D.numSegs   += (npoints-1);
   gSize3D.numPolys  += 0;
}
