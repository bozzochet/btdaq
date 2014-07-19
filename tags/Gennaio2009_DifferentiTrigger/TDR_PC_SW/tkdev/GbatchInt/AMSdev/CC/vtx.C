// Author J. Alcaraz June-July 2003
//
//

#include "vtx.h"
#include "point.h"
#include "event.h"
#include "trrec.h"
#include "ntuple.h"
#include "tkdbc.h"

#include "extC.h"
#include "mceventg.h"

#include "beta.h"
//

///////////////////////////////////////////////////////////
integer AMSVtx::build(integer refit){

   if (refit) {
// Clean up containers
     for(int i=0;;i++){
       AMSContainer *pctr=AMSEvent::gethead()->getC("AMSVtx",i); 
       if (pctr) pctr->eraseC(); 
       else break; 
     } 
   }

// Go ahead...
   int nfound = 0;
   AMSTrTrack* ptrack[Vtxconst::maxtr];

// Add up tracks
   int maxtracks=Vtxconst::maxtr;
   if (TRFITFFKEY.OnlyGammaVtx) maxtracks=2;

// First pass (only tracks with beta)
   AMSTrTrack *ptr = (AMSTrTrack*)AMSEvent::gethead()->getheadC("AMSTrTrack",0);
   for (;ptr!=NULL && nfound<maxtracks;ptr = ptr->next()) {
      if (ptr->checkstatus(AMSDBc::GOLDEN)) {
            ptrack[nfound] = ptr;
            nfound++;
      }
   }

// Create a vertex
   if (nfound>1) {
     AMSVtx *p= new AMSVtx(nfound, ptrack);
     if (p->set_all()) { 
       AMSEvent::gethead()->addnext(AMSID("AMSVtx",0),p); 
       if (AMSEvent::debug) {
	   p->print();
         for (int i=0; i<p->getntracks(); i++) {
            AMSTrTrack* ptr = p->gettrack(i);
            if (!ptr) continue;
            cout << "AMSVtx: itrack " << i;
//Beta check
            bool track_has_beta = false;
            for(int patb=0; patb<npatb; patb++){
                  AMSBeta *pbeta = (AMSBeta*)AMSEvent::gethead()->getheadC("AMSBeta",patb);
                  for (;pbeta!=NULL;pbeta = pbeta->next()) {
                        if (pbeta->getptrack()==ptr) {
                              track_has_beta = true;
                              goto exit_betaprint;
                        }
                  }
            }
exit_betaprint:
            cout << ", beta " << track_has_beta;
//
            cout << ", PI Chi2 " << ptr->getpichi2();
            cout << ", PI Rigidity " << ptr->getpirid();
            cout << ", WEAK bit " << ptr->checkstatus(AMSDBc::WEAK);
            cout << ", FalseX bit " << ptr->checkstatus(AMSDBc::FalseX);
            cout << ", FalseTOFX bit " << ptr->checkstatus(AMSDBc::FalseTOFX);
            cout << endl;
            ptr->_printEl(cout);
            for (int i=0;i<ptr->getnhits();i++){
                  cout << "        " << ptr->getphit(i)->getHit()[0];
                  cout << ", " << ptr->getphit(i)->getHit()[1];
                  cout << ", " << ptr->getphit(i)->getHit()[2];
                  cout << endl;
            }
         }
       }
     }

   }

   return nfound;

}

///////////////////////////////////////////////////////////
integer AMSVtx::build_old(integer refit){

   if (refit) {
// Clean up containers
     for(int i=0;;i++){
       AMSContainer *pctr=AMSEvent::gethead()->getC("AMSVtx",i); 
       if (pctr) pctr->eraseC(); 
       else break; 
     } 
   }

// Go ahead...
   int nfound = 0;
   AMSTrTrack* ptrack[Vtxconst::maxtr];

// Add up tracks
   int maxtracks=Vtxconst::maxtr;
   if (TRFITFFKEY.OnlyGammaVtx) maxtracks=2;

// First use tracks already reconstructed
   AMSTrTrack *ptr = (AMSTrTrack*)AMSEvent::gethead()->getheadC("AMSTrTrack",0);
   for (;ptr!=NULL && nfound<maxtracks;ptr = ptr->next()) {
      //if (ptr->checkstatus(AMSDBc::FalseX)) continue;
      if (ptr->checkstatus(AMSDBc::WEAK)) continue;
      if (ptr->checkstatus(AMSDBc::FalseTOFX)) continue;
      ptrack[nfound] = ptr;
      nfound++;
   }

// Create a vertex
   if (nfound>1) {
     AMSVtx *p= new AMSVtx(nfound, ptrack);
     if (p->set_all()) { 
         AMSEvent::gethead()->addnext(AMSID("AMSVtx",0),p); 
         if (AMSEvent::debug) p->print();
     }

   }

   return nfound;

}

///////////////////////////////////////////////////////////
integer AMSVtx::set_all(){


     // Initial values
     _Momentum = 0.;
     _Charge = 0;
     _ErrMomentum = 0.;
     _Chi2 = 0.;
     _Ndof = 0;
     number mom[3] = {0.0, 0.0, 0.0};
     _Vertex = AMSPoint(0.0, 0.0, 0.0);
     
     if (_Ntracks <= 0) return 0;

     // Most of the information
     number u[3];
     for (int i=0; i<_Ntracks; i++) { 
       _Ptrack[i]->setstatus(AMSDBc::USED);
	 number rig =  _Ptrack[i]->getpirid();
	 number en =  fabs(rig);
	 number erig =  _Ptrack[i]->getepirid()*en*en;
	 _Momentum += en;
	 _Charge += (rig>=0.0)? 1:-1;
	 _ErrMomentum += erig*erig;
	 _Chi2 += _Ptrack[i]->getpichi2(); 
	 _Ndof += 2*_Ptrack[i]->getnhits() - 5; 
       AMSDir dir = AMSDir(_Ptrack[i]->gettheta(2),_Ptrack[i]->getphi(2));
       dir.getp(u);
	 mom[0] += en*u[0]; 
       mom[1] += en*u[1]; 
       mom[2] += en*u[2];
     }
     _ErrMomentum = sqrt(_ErrMomentum)/_Momentum/_Momentum;
     _Mass = _Momentum*_Momentum - mom[0]*mom[0] - mom[1]*mom[1] - mom[2]*mom[2];
     if (_Mass>0.0) _Mass = sqrt(_Mass); else _Mass = 0.0; 
     AMSDir U = AMSDir(mom);
     _Theta = U.gettheta();
     _Phi = U.getphi();

     set_vertex();

     return 1;

}

///////////////////////////////////////////////////////////
void AMSVtx::set_vertex(){

   _Vertex = AMSPoint(0.0, 0.0, 0.0);
   if (_Ntracks<2) return;

// Find minimum distance in the case of 2 tracks
//  if (_Ntracks==2) {

//  VC 12-18-2003
//  Correct vertex coordinates
//
  int number_of_pairs = 0;
  float maxz=200;
  for (int i0=0; i0<_Ntracks-1; i0++){ 
   for (int i1=i0+1; i1<_Ntracks; i1++){ 

    AMSDir dir[2];
    dir[0] = AMSDir(_Ptrack[i0]->gettheta(2),_Ptrack[i0]->getphi(2));
    dir[1] = AMSDir(_Ptrack[i1]->gettheta(2),_Ptrack[i1]->getphi(2));

    number dirprod = dir[0].prod(dir[1]);
    AMSPoint deltax =  _Ptrack[i0]->getpiP0() - _Ptrack[i1]->getpiP0();

    number lambda[2];
    if (fabs(dirprod)<1.) {
       AMSPoint aux = dir[0] - dir[1]*dirprod;
       lambda[0] = - deltax.prod(aux) / (1.-dirprod*dirprod);
       aux = dir[1] - dir[0]*dirprod;
       lambda[1] = deltax.prod(aux) / (1.-dirprod*dirprod);
       AMSPoint poi[2];
       poi[0] = AMSPoint(_Ptrack[i0]->getpiP0()+dir[0]*lambda[0]);
       poi[1] = AMSPoint(_Ptrack[i1]->getpiP0()+dir[1]*lambda[1]);
       number mom=fabs(_Ptrack[i0]->getpirid())+fabs(_Ptrack[i1]->getpirid());
//       cout <<" poi0 "<<poi[0]<<endl;
//       cout <<" poi1 "<<poi[1]<<endl;
       if(fabs(poi[0][2])<maxz && fabs(poi[1][2])<maxz){
         number_of_pairs++;
        for (int j=0; j<3; j++) {
          _Vertex[j] += poi[0][j] * fabs(_Ptrack[i0]->getpirid())/mom;
          _Vertex[j] += poi[1][j] * fabs(_Ptrack[i1]->getpirid())/mom;
        }
       }
       else{
         cerr<<"AMSVTx::set_vertex-W-VertexZTooBig "<<poi[0]<<" "<<poi[1]<<endl; 
         setstatus(AMSDBc::BAD);
       }
    }
   
  }
  }
  for (int j=0; j<3; j++) {
      _Vertex[j] = _Vertex[j]/number_of_pairs;
  }
  if(checkstatus(AMSDBc::BAD))cout <<" vertex "<<_Vertex<<endl;
  

}

///////////////////////////////////////////////////////////
void AMSVtx::_copyEl(){
#ifdef __WRITEROOT__
   if (PointerNotSet()) return;
   VertexR & ptr = AMSJob::gethead()->getntuple()->Get_evroot02()->Vertex(_vpos);
   for (int i=0;i<_Ntracks;i++) {
      if (_Ptrack[i]) {
              ptr.fTrTrack.push_back(_Ptrack[i]->GetClonePointer());
      } else {
              ptr.fTrTrack.push_back(-1);
      }
   }
#endif
}

///////////////////////////////////////////////////////////
void AMSVtx::_writeEl(){

   if(!AMSVtx::Out(1)) return;

#ifdef __WRITEROOT__
   AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif

   VtxNtuple* ntp = AMSJob::gethead()->getntuple()->Get_vtx02();

   int nvtx = ntp->Nvtx;
   if (nvtx>=MAXVTX02) return; 

// Fill the ntuple in
   //
   ntp->Momvtx[nvtx]= (float)_Momentum;
   ntp->ErrMomvtx[nvtx]= (float)_ErrMomentum;
   ntp->Massvtx[nvtx]= (float)_Mass;
   ntp->Thetavtx[nvtx]= (float)_Theta;
   ntp->Phivtx[nvtx]= (float)_Phi;
   ntp->Chargevtx[nvtx]= _Charge;
   ntp->Statusvtx[nvtx]= getstatus();
   for (int i=0;i<3;i++) {ntp->Vertex[nvtx][i]= (float)_Vertex[i];}
   ntp->Chi2vtx[nvtx]= (float)_Chi2;
   ntp->Ndofvtx[nvtx]= _Ndof;
   ntp->Ntrkvtx[nvtx]= _Ntracks;
   for (int i=0; i<_Ntracks; i++) { 
           ntp->PtrTrack[nvtx][i] = _Ptrack[i]->getpos();
   }
   
   ntp->Nvtx++;
     
}
 
///////////////////////////////////////////////////////////
integer AMSVtx::Out(integer status){
  static integer init=0;
  static integer WriteAll=0;
  if(init == 0){
    init=1;
    integer ntrig=AMSJob::gethead()->gettriggerN();
    for(int n=0;n<ntrig;n++){
      if(strcmp("AMSVtx",AMSJob::gethead()->gettriggerC(n))==0){
        WriteAll=1;
        break;
      }
    }
  }
  return (WriteAll || status);
}
 
