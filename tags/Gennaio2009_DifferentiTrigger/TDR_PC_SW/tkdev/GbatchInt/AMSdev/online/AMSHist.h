//  $Id: AMSHist.h,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#ifndef __AMSHIST__
#define __AMSHIST__
#include <TTree.h>
#include <TNamed.h>
#include <TH2.h>
#include <TH1.h>
#include <TFile.h>
#include <TPad.h>
#include <TPaveLabel.h>
#include <TString.h>
#include <vector>
using namespace std;
class AMSNtupleR;
class AMSHist : public TNamed {
protected:
 Int_t _cSet;
 Int_t _cSetl;
 bool _active; 
 vector <TH1*> _filled;
 vector <TString> SetName;
public:
 const char * GetSetName(unsigned int i){return i<SetName.size()?SetName[i].Data():0;}
 Int_t & getCSet() {return _cSet;}
 Int_t & getCSetl() {return _cSetl;}
 Int_t  getMSet() {return SetName.size();}
 void   AddSet(const TString & name){SetName.push_back(name);}  
 void   AddSet(const char* name){SetName.push_back(TString(name));}  
 void Reset(); 
 AMSHist(Text_t * name, Text_t * title,  Int_t active=1):
 TNamed(name,title),_cSet(0),_cSetl(0),_active(active){}
 Int_t DispatchHist(Int_t cset=-1);
 virtual void Book(){};    // User Routine to Book the histos & set up sets
 virtual void Fill(AMSNtupleR * ntuple);  //< User routine fill the histos
 virtual void ShowSet(Int_t cset)=0;
 virtual ~AMSHist();
 void  SetActive(bool active){_active=active;}
 bool  IsActive(){return (SetName.size()>0) && _active;}
 ClassDef(AMSHist, 1)   //Abstract class to browse hist
     };
#endif

