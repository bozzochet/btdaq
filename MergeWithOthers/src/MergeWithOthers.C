// Authors: M.Duranti - INFN di Perugia

#include <cstdio>
#include <iostream>
#include <stdlib.h>

#include <TFile.h>
#include <TKey.h>
#include <TChain.h>
#include <TBranch.h>
#include <TBranchElement.h>
#include <TObjArray.h>

#include <Event.hh>

using namespace std;

int main(int argc, char* argv[]);
void AddAdditionalStuff(TFile* infiles[3], const char* t3names[3], TFile* fout);

//--------------------------------------------------------------------

static bool nosave = false;

int main(int argc, char* argv[]){

  if (argc<=4) {
    printf("\n\n");
    printf("Usage:\n");
    printf("%s output_file AMS_root_file ANC_root_file HERD_root_file ...\n", argv[0]);
    printf("\n\n");
    exit(1);
  }

  const char* t3names[3] = {"t4", "AncillaryEvent", "HERD_CALO"};
  TFile* fin[3];
  
  TChain* ams = new TChain(t3names[0]);
  ams->Add(argv[2]);
  fin[0] = new TFile(argv[2]);
  Long64_t nentriesams = ams->GetEntries();
  printf("AMS: TChain (%s) with %llu entries\n", ams->ClassName(), nentriesams);
  
  TChain* anc = new TChain(t3names[1]);
  anc->Add(argv[3]);
  fin[1] = new TFile(argv[3]);
  Long64_t nentriesanc = anc->GetEntries();
  printf("ANC: TChain (%s) with %llu entries\n", anc->ClassName(), nentriesanc);

  TChain* herd = new TChain(t3names[2]);
  herd->Add(argv[4]);
  fin[2] = new TFile(argv[4]);
  Long64_t nentriesherd = herd->GetEntries();
  printf("HERD: TChain (%s) with %llu entries\n", herd->ClassName(), nentriesherd);

  Long64_t nentries = std::min(nentriesams, nentriesanc);
  nentries = std::min(nentries, nentriesherd);
  
  if (nentriesams != nentriesanc || nentriesams != nentriesherd || nentriesanc != nentriesherd) {
    printf("WARNING: The number of entries is different. I will process only %lld entries...\n", nentries);
  }

  TFile* f = new TFile(argv[1], "RECREATE");
  f->cd();
  AddAdditionalStuff(fin, t3names, f);

  TTree* t3out = new TTree("mergedtree", "mergedtree");

  TChain* t3in[3] = {ams, anc, herd};
  
  int intvar[99999];
  int indexintvar=0;
  double doublevar[99999];
  int indexdoublevar=0;  

  for (int tt=0; tt<3; tt++) {
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
    anc->GetEvent(ii);
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
  
  f->Write(0, TObject::kOverwrite);
  f->Close();
  
  return 0;
}

void AddAdditionalStuff(TFile* fin[3], const char* t3names[3], TFile* fout){

  for (int ff=0; ff<3; ff++) {
    TList *klst = fin[ff]->GetListOfKeys();
    for (int ii=0; ii<klst->GetSize(); ii++) {
      TKey* key=(TKey*)(klst->At(ii));
      if (key) {
	TString kname = key->GetName();
	TObject* obj=fin[ff]->Get(kname.Data());
	if (obj) {
	  TString oname = obj->GetName();
	  //	  printf("-----------> %s (%s)\n", oname.Data(), obj->ClassName());//only for debug
	  bool dontwrite=false;
	  for (int ff2=0; ff2<3; ff2++) {
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
