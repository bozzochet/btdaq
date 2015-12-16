// Authors: M.Duranti - INFN di Perugia

#include <cstdio>
#include <iostream>
#include <stdlib.h>

#include <TFile.h>
#include <TKey.h>
#include <TChain.h>
#include <TTree.h>
#include <TBranch.h>
#include <TBranchElement.h>
#include <TObjArray.h>

#include <time.h>
#include <ctime>

#include <Event.hh>

using namespace std;

int main(int argc, char* argv[]);
void AddAdditionalStuff(TFile* infiles[2], const char* t3names[2], TFile* fout);
void GetStartANDStopTime(string t3names, time_t &start_time, time_t &stop_time);
//--------------------------------------------------------------------

static bool nosave = false;

int main(int argc, char* argv[]){

  const int n_trees=2;
  if (argc<=(n_trees+1)) {
    printf("\n\n");
    printf("Usage:\n");
    printf("%s output_file AMS_root_file HERD_root_file ...\n", argv[0]);
    printf("\n\n");
    exit(1);
  }
  
  const char* t3names[n_trees] = {"t4", "HERD_CALO"};
  TFile* fin[n_trees];

  TChain* ams = new TChain(t3names[0]);
  ams->Add(argv[2]);
  fin[0] = new TFile(argv[2]);
  Long64_t nentriesams = ams->GetEntries();
  printf("AMS: TChain (%s) with %llu entries\n", ams->ClassName(), nentriesams);
  
  TChain* herd = new TChain(t3names[1]);
  
  time_t start_time =0;
  time_t stop_time =0;
  //  GetStartANDStopTime(t3names[1], start_time, stop_time);


    

  herd->Add(argv[3]);
  fin[1] = new TFile(argv[3]);
  Long64_t nentriesherd = herd->GetEntries();
  printf("HERD: TChain (%s) with %llu entries\n", herd->ClassName(), nentriesherd);

  Long64_t nentries = std::min(nentriesams, nentriesherd);
  
  if (nentriesams != nentriesherd) {
    printf("WARNING: The number of entries is different. I will process only %lld entries...\n", nentries);
  }

  TFile* f = new TFile(argv[1], "RECREATE");
  f->cd();
  AddAdditionalStuff(fin, t3names, f);

  TTree* t3out = new TTree("mergedtree", "mergedtree");

  
  
  if( ams->GetTree()->GetUserInfo()->GetSize() > 0) {
    cout<<" ams->GetUserInfo()->GetSize() "<<ams->GetTree()->GetUserInfo()->GetSize()<<endl;  
    RHClass *rh = (RHClass*) ams->GetTree()->GetUserInfo()->At(0);
    if(rh){
      cout<<" rh->Run "<<rh->Run<<endl;
      RHClass rh2= *rh;
      cout<<" t3out->GetUserInfo()->GetSize() "<<t3out->GetTree()->GetUserInfo()->GetSize()<<endl; 
      t3out->GetUserInfo()->Add(&rh2);
      cout<<" t3out->GetUserInfo()->GetSize() "<<t3out->GetTree()->GetUserInfo()->GetSize()<<endl;
    }
  } 


  TChain* t3in[n_trees] = {ams, herd};
  
  int intvar[99999];
  int indexintvar=0;
  double doublevar[99999];
  int indexdoublevar=0;  


  for (int tt=0; tt<n_trees; tt++) {
    t3in[tt]->GetEntry(0);
    TObjArray* oa = t3in[tt]->GetListOfBranches();
    //    oa->Print();
    for (int ii=0; ii<oa->GetEntries(); ii++) {
      TObject* obj = oa->At(ii);
      //      printf("%s, %s\n", obj->GetName(), obj->ClassName());
      TBranchElement* tbe = dynamic_cast<TBranchElement*>(obj);
      if (tbe) {
	TString bn = tbe->GetName();
	//    printf("%s\n", bn.Data());
	TString classname = tbe->GetClassName();
	//      printf("%s\n", classname.Data());
	t3out->Branch(bn.Data(), classname.Data(), (void**)(tbe->GetAddress()));
      }
      else {
	TBranch* tb = dynamic_cast<TBranch*>(obj);
	if (tb) {
	  tb->SetAutoDelete(kFALSE);
	  TString bn = tb->GetName();
	  TString bt = tb->GetTitle();
	  //	  printf("%s %s\n", bn.Data(), bt.Data());
	  TString classname = tb->GetClassName();
	  //	printf("%s\n", classname.Data());
	  if (classname!=""){
	    t3out->Branch(bn.Data(), tb->GetClassName(), (void**)(tb->GetAddress()));
	  }
	  else {//leaflist mode
	    TClass* expectedClass;
	    EDataType expectedType;
	    tb->GetExpectedType(expectedClass, expectedType);
	    //	    printf("%d\n", expectedType);
	    TObjArray* tslash = bt.Tokenize("/");
	    // for (int ss=0; ss<tslash->GetEntries(); ss++) {
	    //   printf("%d) %s\n", ss, ((TObjString*)(tslash->At(ss)))->GetString().Data());
	    // }
	    TString btnoslash = ((TObjString*)(tslash->At(0)))->GetString();
	    //	    printf("%s\n", btnoslash.Data());
	    TObjArray* tbracket = btnoslash.Tokenize("[]");
	    int size=1;
	    for (int ss=0; ss<tbracket->GetEntries(); ss++) {
	      //	      printf("%d) %s\n", ss, ((TObjString*)(tbracket->At(ss)))->GetString().Data());
	      if (ss!=0) size*=((TObjString*)(tbracket->At(ss)))->GetString().Atoi();
	    }
	    //	    printf("size: %d\n", size);
	    if (expectedType==kInt_t) {
	      t3in[tt]->SetBranchAddress(bn.Data(), &intvar[indexintvar]);
	      t3out->Branch(bn.Data(), &intvar[indexintvar], bt.Data());
	      indexintvar+=size;
	    }
	    else if (expectedType==kDouble_t) {
	      t3in[tt]->SetBranchAddress(bn.Data(), &doublevar[indexdoublevar]);
	      t3out->Branch(bn.Data(), &doublevar[indexdoublevar], bt.Data());
	      indexdoublevar+=size;
	    }
	    else {
	      printf("WARNING: Branch type %d not yet impemented...\n", expectedType);
	    }
	  }
	}
	else {
	  printf("Not TBranchElement, not TBranch... ???\n");
	}
      }
    }
  }

  //-----------------------
  
  Long64_t start=0;
  Long64_t stop=nentries;

  double perc=0;
  
  Long64_t ii=0;
  for (ii=start; ii<stop; ii++) {
    ams->GetEvent(ii);
    herd->GetEvent(ii);
    //printf("Got Event %9d\n",ii);
    
    Double_t pperc=(100.0*((ii-start)+1))/(stop-start);

    t3out->Fill();
    
    if (pperc>=perc) {
      printf("Processed %lld out of %lld: %d%%\n", (ii+1-start), stop-start, (int)perc);
      perc++;
    }

  }
  printf("Processed %lld out of %lld: %d%%\n", (ii+1-start), stop-start, (int)perc);
  f->cd();
 
  f->Write(0, TObject::kOverwrite);
  f->Close();
  
  return 0;
}

void AddAdditionalStuff(TFile* fin[2], const char* t3names[2], TFile* fout){

  for (int ff=0; ff<2; ff++) {
    TList *klst = fin[ff]->GetListOfKeys();
    for (int ii=0; ii<klst->GetSize(); ii++) {
      TKey* key=(TKey*)(klst->At(ii));
      if (key) {
	TString kname = key->GetName();
	TObject* obj=fin[ff]->Get(kname.Data());
	if (obj) {
	  TString oname = obj->GetName();
	  // printf("-----------> %s (%s)\n", oname.Data(), obj->ClassName());//only for debug
	  bool dontwrite=false;
	  for (int ff2=0; ff2<2; ff2++) {
	    if (oname==t3names[ff2]) {
	      dontwrite=true;
	      break;
	    }
	  }
	  if (!dontwrite) {
	    //	    printf("writing on output file\n");
	    fout->cd();
	    TTree* T = dynamic_cast<TTree*>(obj);
	    if (T) {
	      //	      printf("it's a tree...\n");
	      TTree *T2 = T->CopyTree("");
	      T2->Write();
	    }
	    else {
	      //	    fout->WriteTObject(obj, obj->GetName(), "Overwrite");
	      obj->Write(obj->GetName(), TObject::kOverwrite);
	    }
	  }
	}
      }
    }
    fout->Write(0, TObject::kOverwrite);//otherwise the keys copied in the loop above are not present in the final file...
  }

  return;
}



void GetStartANDStopTime(string t3names, time_t &start_time, time_t &stop_time){
  
  const int n_pos=6;
  int  pos_start[n_pos];
  int index_pos=0;
  pos_start[index_pos]= t3names.find("eV_");index_pos++;//0
  pos_start[index_pos]= t3names.find("Nov");index_pos++;//1
  pos_start[index_pos]= t3names.find("2015_");index_pos++;//2
  pos_start[index_pos]= t3names.find_first_of("\\:");index_pos++;//3
  pos_start[index_pos]= t3names.find_last_of("~");index_pos++;//4
  pos_start[index_pos]= t3names.find_last_of("\\:");index_pos++;//5

  for (int i_pos=1; i_pos<n_pos; i_pos++){
    if (pos_start[i_pos]==std::string::npos){
      cout<<" File name with different standard time indication "<<endl;
      exit(-i_pos);
    }
  }
  
  if(pos_start[1]-(pos_start[0]+3)<1){
    cout<<" File name with different standard time indication "<<endl;
    exit(-n_pos);
  }
  

  string _day = t3names.substr(pos_start[0]+3,pos_start[1]-(pos_start[0]+3));
  string _month="12";
  string _year="2015";

  if(pos_start[3]-(pos_start[2]+5)<1){
    cout<<" File name with different standard time indication "<<endl;
    exit(-(n_pos+1));
  }  
  string _h_start=t3names.substr(pos_start[2]+5,pos_start[3]-(pos_start[2]+5));

  if(pos_start[4]-(pos_start[3]+2)<1){
    cout<<" File name with different standard time indication "<<endl;
    exit(-(n_pos+2));
  }
  string _m_start=t3names.substr(pos_start[3]+2,pos_start[4]-(pos_start[3]+2));
  string _s_start="00";
  string _start_time=_year+"-"+_month+"-"+_day+"-"
    +_h_start+":"+_m_start+":"+_s_start;
  
  struct tm tm;
  strptime(_start_time.c_str(), "%Y-%m-%d-%H:%M:%S", &tm);
  time_t t = mktime(&tm);  // t is now your desired time_t

}
