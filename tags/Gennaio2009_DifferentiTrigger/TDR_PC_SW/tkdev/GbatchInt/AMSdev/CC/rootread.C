#include <iostream.h>
#include "TBranch.h"
#include "root.h"
#include <stdio.h>
#include "TObjString.h"
#include <fstream.h>
using namespace std;
int rootread(char * fname, int nev, int iver,  int & lastevent,bool jou){
 int LastEvent=lastevent;
 int diff=0;
 lastevent=0;
int firstevent=-1;
 if(!iver){
  fclose(stderr);
  cout.setstate(ios_base::failbit);
  cerr.setstate(ios_base::failbit);
 }
 TFile * rfile= new TFile(fname,"READ");
 if(!rfile){
        if(iver>0)cout <<"problem to open file "<<fname<<" "<<endl;
	return -1;
 }
 TTree * tree=dynamic_cast<TTree*>(rfile->Get("AMSRoot"));
 if(!tree){
        if(iver>0)cout <<"problem to find tree AMSRoot "<<endl;
	return -1;
 }
 if(jou){
   TObjString s("");
   s.Read("DataCards");
   if(s.String().Length()>1){
     TString fnam(fname);
     fnam+=".jou";
     ofstream fout;
     fout.open((const char*)fnam);
     if(fout){
       fout <<s.String();
       fout.close();
     }
     else if(iver){
      cerr<<" Unable to open jou file "<<(const char*)fnam<<endl; 
     }  
   }
   else if(iver){
    cerr<<" Unable to find datacards in "<<fname<<endl;
   }
   
 } 
 int nevents=tree->GetEntries();
 if(iver)cout <<"  entries "<<nevents<<" "<<nev<<endl;
 if(nevents<=0)return -2;
 else if(nev>nevents)return -3;
 else if(nev<nevents)return -4;
  AMSEventR *pev = new AMSEventR();
//  tree->SetMakeClass(1);
  pev->Init(tree);
  pev->GetBranch(tree);
  int nbadev=0;
  int nevread=0;
  for (int i=0;i<nevents;i++){
   if(!pev->ReadHeader(i))break;
   if((pev->fHeader.Status[0]/1073741824)%2)nbadev++;
   if(firstevent<0)firstevent=pev->fHeader.Event;
   if(lastevent && abs(lastevent-int(pev->fHeader.Event))>diff){
     diff=std::abs(lastevent-int(pev->fHeader.Event));
   }
   lastevent=pev->fHeader.Event;
   nevread++;
  }
  rfile->Close();
  if(nevread!=nev)return -2;
  if(nev>2 && LastEvent>0){
   int rate=(lastevent+1-firstevent)/nevread;
   if(iver)cout <<"last event "<<LastEvent <<" " <<lastevent<<endl; 
   if(abs(LastEvent-lastevent)>5*rate && abs(LastEvent-lastevent)>2*diff){
     return -5;
   } 
  }
  float rrr=100*float(nbadev)/float(nevread);
  return int(rrr); 
}
