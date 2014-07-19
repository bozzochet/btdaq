//  $Id: TMarker3DCl.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

#ifndef ROOT_TMarker3DCl
#define ROOT_TMarker3DCl


////////////////////////////////////////////////////////////////////////////
//                                                                        //
// TMarker3DCl                                                           //
//                                                                        //
// TMarker3DCl is a special 3-D marker designed for event display.        //
// It has the following parameters:                                       //
//    fDx;              half length in X                                  //
//    fDy;              half length in Y                                  //
//    fDz;              half length in Z                                  //
//    fProfileX         array containing cluster profile in X     
//    fProfileY         array containg   cluster profile in Y     
//    fTranslation[3];  the coordinates of the center of the box          //
//    fDirCos[3];       the direction cosinus defining the orientation    //
//    fRefObject;       A reference to an object                          //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include "TObject.h"
#endif
#ifndef ROOT_TAttLine
#include "TAttLine.h"
#endif
#ifndef ROOT_TAttFill
#include "TAttFill.h"
#endif
#ifndef ROOT_TAtt3D
#include "TAtt3D.h"
#endif
#ifndef ROOT_X3DBuffer
#include "X3DBuffer.h"
#endif
#include <vector>
class TH1;

class TMarker3DCl : public TObject, public TAttLine, public TAttFill,
                     public TAtt3D {

protected:

    Float_t  fX;               // X coordinate of center of cluster
    Float_t  fY;               // Y coordinate of center of cluster
    Float_t  fZ;               // Z coordinate of center of cluster
    Float_t  fDx;              // half length in x
    Float_t  fDy;              // half length in y
    Float_t  fDz;              // half length in z
    vector<float> fProfileX;
    vector<float> fProfileY;
    Int_t fNx;                //  number of hits along X
    Int_t fNy;                //  number of hits along Y
    Bool_t fShowProfileX;     //  Draw cluster (true) or simple box (false) in X                            
    Bool_t fShowProfileY;     //  Draw cluster (true) or simple box (false) in Y
    Float_t  fTheta;           // Angle of box z axis with respect to main Z axis
    Float_t  fPhi;             // Angle of box x axis with respect to main Xaxis
    TObject *fRefObject;       // Pointer to an object

protected:
    virtual void    PaintGLPoints(Float_t *buff);

public:
    TMarker3DCl();
    TMarker3DCl(Float_t x, Float_t y, Float_t z,
                 Float_t dx, Float_t dy, Float_t dz,
                 Float_t theta, Float_t phi);
    virtual        ~TMarker3DCl();

    virtual Int_t   DistancetoPrimitive(Int_t px, Int_t py);
    virtual void    ExecuteEvent(Int_t event, Int_t px, Int_t py);
    TObject        *GetRefObject() const {return fRefObject;}
    virtual void    GetDirection(Float_t &theta, Float_t &phi) const {theta = fTheta; phi = fPhi;}
    virtual void    GetPosition(Float_t &x, Float_t &y, Float_t &z) const {x=fX; y=fY, z=fZ;}
    virtual void    GetSize(Float_t &dx, Float_t &dy, Float_t &dz) const {dx=fDx; dy=fDy; dz=fDz;}

    virtual void    Paint(Option_t *option);
    virtual void    PaintShape(X3DBuffer *buff, Bool_t rangeView=kFALSE);
    void            PaintShape3D(X3DBuffer *buff, Bool_t rangeView=kFALSE);

    virtual void    SavePrimitive(ofstream &out, Option_t *option);
    virtual void    SetPoints(Float_t *buff);
    virtual void    SetDirection(Float_t theta, Float_t phi);
    virtual void    SetPosition(Float_t x, Float_t y, Float_t z);
    void SetProfileX(Int_t n,Float_t *x){fProfileX.clear();for(Int_t k=0;k<n;k++)fProfileX.push_back(x[k]);}
    void SetProfileY(Int_t n,Float_t *x){fProfileY.clear();for(Int_t k=0;k<n;k++)fProfileY.push_back(x[k]);}
    void SetShowProfileX(Bool_t set);
    void SetShowProfileY(Bool_t set);
    virtual void    SetSize(Float_t dx, Float_t dy, Float_t dz);
    virtual void    SetRefObject(TObject *obj=0) {fRefObject = obj;}

    virtual void    Sizeof3D() const;

    ClassDef(TMarker3DCl,1)  //A special 3-D marker designed for event display
};

#endif
