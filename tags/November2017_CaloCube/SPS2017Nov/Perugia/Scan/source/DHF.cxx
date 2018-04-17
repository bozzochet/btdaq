// Authors: P.Zuccon, D.Caraffini - INFN di Perugia
#define DHF_cxx


#include "DHF.hh"

#include <iostream>
using namespace std;

ClassImp(DHF)
///////////////////////////////////////////////////////////////////////
// Class DHF
//
// The class DHF implements the histogram manager. In the method BookHistos()
// the user can define a set histograms he want to fill at each cut level.
//
// The method Define(  ) provides a way to book automatically a three of histograms 
// versions of the same histograms corresponding to the different event classes
// defined in the Class Categories  these histograms are putted in a subdirectory
//
// In the method FillAll(), the user is expected to define the rules to fill 
// the histograms. The methods Fill1a() and Fill2a() are provided to fill automagically
// the histos booked with the method Define(). 


//---------------------------------------------------------------
DHF::DHF(TDirectory * Dir,char * name,char * title,int ntdr,int * tdrnum, int run){ 
  // creer sous directoires pour histos sous Dir
  printf("name %s title %s\n",name,title);

  runno = run;
  memset(TDRNum,0,24*sizeof(TDRNum[0]));
  fHlist=new TObjArray();
  TDirectory *fDir = Dir;
  //  printf("Dir %d gDirectory %d\n",Dir,gDirectory);
  NTdr=ntdr;
  fDir->ls();
  for (int ii=0;ii<NTdr;ii++)
    TDRNum[ii]=tdrnum[ii];

  Ev = 0;
  char dname[255];
  char dtitle[255];
  //create the dir struct
  printf("ntdr %d\n",ntdr);
  for (int ii=0;ii<ntdr;ii++){
    fDir->cd();
    sprintf(dname,"%04d",tdrnum[ii]);
    sprintf(dtitle,"JINF %02d Lad %02d",tdrnum[ii]/100,tdrnum[ii]-(tdrnum[ii]/100)*100 );
    //    printf("ii %d dname %s dtitle %s\n",ii,dname,dtitle);
    fDir->mkdir(dname,dtitle);
  }

  // Set the name and the title of the class  
  this->SetNameTitle(name,title);

}

//---------------------------------------------------------------
DHF::DHF(char * name,char * title,int ntdr,int * tdrnum,int run){ 
  // creer sous directoires pour histos sous Rint

  runno = run;
  memset(TDRNum,0,24*sizeof(TDRNum[0]));
  fHlist=new TObjArray();
  fDHlist0=new TObjArray();
  fDHlist1=new TObjArray();
  fDHlist2=new TObjArray();
  fDHlist3=new TObjArray();
  fDHlist4=new TObjArray();
  fDHlist5=new TObjArray();
  fDHlistTop=new TObjArray();
  NTdr=ntdr;
  printf("ntdr %d\n",ntdr);
  for (int ii=0;ii<NTdr;ii++)
    TDRNum[ii]=tdrnum[ii];

  Ev = 0;
  char dname[255];
  char dtitle[255];
  //create the dir struct
  for (int ii=0;ii<ntdr;ii++){
    gROOT->cd();
    sprintf(dname,"%04d",tdrnum[ii]);
    sprintf(dtitle,"JINF %02d Lad %02d",tdrnum[ii]/100,tdrnum[ii]-(tdrnum[ii]/100)*100 );
    gDirectory->mkdir(dname,dtitle);
  }
  
  // Set the name and the title of the class  
  this->SetNameTitle(name,title);

}

//---------------------------------------------------------------

DHF::DHF(const DHF &orig): TNamed(orig)
{
  fDir=orig.fDir;
  if(orig.fHlist){
    fHlist= new TObjArray();
    for (int ii=0;ii<orig.fHlist->GetEntries();ii++){
      TObject* tmp=orig.fHlist->At(ii);
      fHlist->Add( tmp->Clone());
    }
  }
}


//---------------------------------------------------------------
DHF::~DHF(){
  // Standard destructor
  if (fHlist) {
    fHlist->Clear("C");
    fHlist->Delete();
    fDHlist0->Clear("C");
    fDHlist0->Delete();
    fDHlist1->Clear("C");
    fDHlist1->Delete();
    fDHlist2->Clear("C");
    fDHlist2->Delete();
    fDHlist3->Clear("C");
    fDHlist3->Delete();
    fDHlist4->Clear("C");
    fDHlist4->Delete();
    fDHlist5->Clear("C");
    fDHlist5->Delete();
    fDHlistTop->Clear("C");
    fDHlistTop->Delete();
  }
}

void DHF::Sumw2(){
  //Sets all histograms to have  statistic error calculation
  TIter next( fHlist);
  TObject*isto;
  while ((isto =  next())){
    if (strcmp(isto->ClassName(),"TH1D")!=0) 
      ((TH1D *)isto)->Sumw2();
    
    else if (strcmp(isto->ClassName(),"TH2D")!=0 )  
      ((TH2D *)isto)->Sumw2();
  }
}


//------------------------------------------------------------------------
void DHF::Init(Align* alg){

  gDirectory->pwd();
  BookHistos(alg);
  //require error calculation for all histos
  Sumw2();
  //come back to the previous directory

}

void DHF::AddHisto(TObject* kk){
  fHlist->Add(kk);
  fDHlistTop->Add(kk);
  return;
}

//---------------------------------------------------------------
TObject *DHF::GetHist(char* name){
  // Return a pointer the the histogram with the reqired name. 
  //The output need a cast for the correct type.
  return fHlist->FindObject(name); 
}
	 
//---------------------------------------------------------------
void DHF::Define (char *name, char *title, int binx, Axis_t lowx, Axis_t upx,int sk,
		    char *dirname){
  char dname[20];
  char name2[100];
  TH1D *isto, *istos, *istok;
  for (int ii=0;ii<NTdr;ii++){
    sprintf(dname,"%04d",TDRNum[ii]);
    //    printf("dname %s\n",dname);
    gDirectory->cd(dname);
    if(!sk){
      sprintf(name2,"%s_%04d",name,TDRNum[ii]);
      isto = new TH1D (name2, title, binx, lowx, upx);
      fHlist->Add (isto);
      switch (ii) {
        case 0:
          fDHlist0->Add(isto);
          break;
        case 1:
          fDHlist1->Add(isto);
          break;
        case 2:
          fDHlist2->Add(isto);
          break;
        case 3:
          fDHlist3->Add(isto);
          break;
        case 4:
          fDHlist4->Add(isto);
          break;
        case 5:
          fDHlist5->Add(isto);
          break;
      }
    }else{
      sprintf(name2,"%s_%04d_S",name,TDRNum[ii]);
      istos = new TH1D (name2, title, binx, lowx, upx);
      fHlist->Add (istos);
      sprintf(name2,"%s_%04d_K",name,TDRNum[ii]);
      istok = new TH1D (name2, title, binx, lowx, upx);
      fHlist->Add (istok);
      switch (ii) {
        case 0:
          fDHlist0->Add(istos);
          fDHlist0->Add(istok);
          break;
        case 1:
          fDHlist1->Add(istos);
          fDHlist1->Add(istok);
          break;
        case 2:
          fDHlist2->Add(istos);
          fDHlist2->Add(istok);
          break;
        case 3:
          fDHlist3->Add(istos);
          fDHlist3->Add(istok);
          break;
        case 4:
          fDHlist4->Add(istos);
          fDHlist4->Add(istok);
          break;
        case 5:
          fDHlist5->Add(istos);
          fDHlist5->Add(istok);
          break;
      }
    }
    gDirectory->cd("..");
  }
  
  return;
}
//---------------------------------------------------------------
void DHF::Define(char*name,char*title,int binx,Axis_t lowx,Axis_t upx,int biny,Axis_t lowy,Axis_t upy,int sk,char*dirname){
  //Book a 2d histogram set for the three Categories: Good Bad Neg
  
  char dname[20];
  char name2[100];  
  TH2D *isto;

  for (int ii=0;ii<NTdr;ii++){
    sprintf(dname,"%04d",TDRNum[ii]);
    //    printf("dname %s\n",dname);
    gDirectory->cd(dname);
    if(!sk){
      sprintf(name2,"%s_%04d",name,TDRNum[ii]);
      isto = new TH2D (name2, title, binx, lowx, upx, biny, lowy, upy);
      fHlist->Add (isto);
    }else{
      sprintf(name2,"%s_%04d_K",name,TDRNum[ii]);
      isto = new TH2D (name2, title, binx, lowx, upx, biny, lowy, upy);
      fHlist->Add (isto);
      sprintf(name2,"%s_%04d_S",name,TDRNum[ii]);
      isto = new TH2D (name2, title, binx, lowx, upx, biny, lowy, upy);
      fHlist->Add (isto);
    }
    switch (ii) {
      case 0:
        fDHlist0->Add(isto);
        break;
      case 1:
        fDHlist1->Add(isto);
        break;
      case 2:
        fDHlist2->Add(isto);
        break;
      case 3:
        fDHlist3->Add(isto);
        break;
      case 4:
        fDHlist4->Add(isto);
        break;
      case 5:
        fDHlist5->Add(isto);
        break;
    }
    gDirectory->cd("..");
  }
  
  return;

}

//---------------------------------------------------------------

void DHF::Fill1D(char *histo, Axis_t X, Stat_t w){

  TObject *temp;
  //  char name[80];
  //  char name2[10];

  temp = (TH1D *) fHlist->FindObject(histo);
  TH1D* mhs=(TH1D*) temp;
  mhs->Fill(X,w);
   
  return;

}

//---------------------------------------------------------------

void DHF::Fill2D(char *histo, Axis_t X1,  Axis_t X2, Stat_t w){

  TObject *temp;
  //  char name[80];
  //  char name2[10];

  temp = (TH2D *) fHlist->FindObject(histo);
  TH2D* mhs=(TH2D*) temp;
  mhs->Fill(X1,X2,w);
   
  return;

}


//---------------------------------------------------------------
void DHF::Fill( int sk,Cluster* lad,char *name, Axis_t X1,  Axis_t X2, Stat_t w){
  TObject *temp;
  char histo[255];
  sprintf(histo,"%s_%04d",name,lad->ladder);

  if(sk){
    if(lad->GetSeedAdd()>640)
      sprintf(histo,"%s_%04d_K",name,lad->ladder);
    else
      sprintf(histo,"%s_%04d_S",name,lad->ladder);
  }
  temp = (TH1D *) fHlist->FindObject(histo);
  //cout<<temp->GetName()<<endl;
  if (!temp) {
    cout << "Fill1::Error: " << histo << " does not exist\n";
    cout <<" Ignoring for the moment ..."<<endl;
    return;
  }
   if(strcmp("TH1I",temp->ClassName())==0){ // is a TH1I
      TH1I* hh=(TH1I*) temp;
      hh->Fill(X1, X2);
   }
   else if(strcmp("TH1D",temp->ClassName())==0){ // is a TH1D
      TH1D* hh=(TH1D*) temp;
      hh->Fill(X1, X2);
   }
   
   else if(strcmp("TH2D",temp->ClassName())==0){ // is a TH2D
      TH2D* hh=(TH2D*) temp;
      hh->Fill(X1,X2,w);
   }
   else if(strcmp("TH2I",temp->ClassName())==0){ // is a TH2I
      TH2I* hh=(TH2I*) temp;
      hh->Fill(X1,X2,w);
   }
   
   else if(strcmp("TProfile",temp->ClassName())==0){ // is a TProfile
      TProfile* hh=(TProfile*) temp;
      hh->Fill(X1,X2,w);
   }
   else{
      cout <<"DHF:Fill   errror"<<endl;
      cout <<"        The object named "<<histo<<" is of type "<<temp->ClassName()<<"which is uknown"<<endl;
   
   }
   
  return;
}

//---------------------------------------------------------------
void DHF::FillCalib(int ntdr,char *name, Axis_t X1,  Axis_t X2, Stat_t w){
  TObject *temp;
  char histo[255];
  sprintf(histo,"%s_%04d",name,ntdr);

  temp = (TH1D *) fHlist->FindObject(histo);
  //cout<<temp->GetName()<<endl;
  if (!temp) {
    cout << "Fill1::Error: " << histo << " does not exist\n";
    cout <<" Ignoring for the moment ..."<<endl;
    return;
  }
   if(strcmp("TH1I",temp->ClassName())==0){ // is a TH1I
      TH1I* hh=(TH1I*) temp;
      hh->Fill(X1, X2);
   }
   else if(strcmp("TH1D",temp->ClassName())==0){ // is a TH1D
      TH1D* hh=(TH1D*) temp;
      hh->Fill(X1, X2);
   }
   
   else if(strcmp("TH2D",temp->ClassName())==0){ // is a TH2D
      TH2D* hh=(TH2D*) temp;
      hh->Fill(X1,X2,w);
   }
   else if(strcmp("TH2I",temp->ClassName())==0){ // is a TH2I
      TH2I* hh=(TH2I*) temp;
      hh->Fill(X1,X2,w);
   }
   
   else if(strcmp("TProfile",temp->ClassName())==0){ // is a TProfile
      TProfile* hh=(TProfile*) temp;
      hh->Fill(X1,X2,w);
   }
   else{
      cout <<"DHF:Fill   errror"<<endl;
      cout <<"        The object named "<<histo<<" is of type "<<temp->ClassName()<<"which is uknown"<<endl;
   
   }
   
  return;
}


TDirectory * DHF::Define(char*prof,char *name, char *title, int binx, Axis_t lowx, Axis_t upx, int sk,char *dirname)
{
  TDirectory*pp=gDirectory;
  char dname[20];
  char name2[100];
  for (int ii=0;ii<NTdr;ii++){
    fDir->cd();
    sprintf(dname,"%04d",TDRNum[ii]);
    fDir->cd(dname);
    sprintf(name2,"%s_%04d",name,TDRNum[ii]);
    TProfile*isto = new TProfile (name2, title, binx, lowx, upx, prof);
    fHlist->Add (isto);
  }
  //come back to the previous directory
  pp->cd ();
  
  return pp;
}


//---------------------------------------------------------------

void DHF::EcrireHistos(char* ffnom){
 
  char dname[20];
  TIter nexttop(fDHlistTop);
  TIter next0(fDHlist0);
  TIter next1(fDHlist1);
  TIter next2(fDHlist2);
  TIter next3(fDHlist3);
  TIter next4(fDHlist4);
  TIter next5(fDHlist5);
  TObject*isto;

  fhist= new TFile(ffnom,"RECREATE");
  if (fhist->IsOpen()) printf("ouvert\n");
  while ((isto =  nexttop())){
     if (strcmp(isto->ClassName(),"TH1D")!=0)
       ((TH1D *)isto)->Write();   
     else if (strcmp(isto->ClassName(),"TH2D")!=0 )  
       ((TH2D *)isto)->Write();
  }
  for (int ii=0;ii<NTdr;ii++){
    sprintf(dname,"%04d",TDRNum[ii]);
    //    printf("dname %s\n",dname);
    gROOT->cd();
    gDirectory->cd(dname);
    //    gDirectory->ls();
    fhist->mkdir(dname);
    fhist->cd(dname);
    if (ii == 0) {
     while ((isto =  next0())){
       if (strcmp(isto->ClassName(),"TH1D")!=0)
         ((TH1D *)isto)->Write();   
       else if (strcmp(isto->ClassName(),"TH2D")!=0 )  
         ((TH2D *)isto)->Write();
     }
    }
    if (ii == 1) {
     while ((isto =  next1())){
       if (strcmp(isto->ClassName(),"TH1D")!=0)
         ((TH1D *)isto)->Write();   
       else if (strcmp(isto->ClassName(),"TH2D")!=0 )  
         ((TH2D *)isto)->Write();
     }
    }
    if (ii == 2) {
     while ((isto =  next2())){
       if (strcmp(isto->ClassName(),"TH1D")!=0)
         ((TH1D *)isto)->Write();   
       else if (strcmp(isto->ClassName(),"TH2D")!=0 )  
         ((TH2D *)isto)->Write();
     }
    }
    if (ii == 3) {
     while ((isto =  next3())){
       if (strcmp(isto->ClassName(),"TH1D")!=0)
         ((TH1D *)isto)->Write();   
       else if (strcmp(isto->ClassName(),"TH2D")!=0 )  
         ((TH2D *)isto)->Write();
     }
    }
    if (ii == 4) {
     while ((isto =  next4())){
       if (strcmp(isto->ClassName(),"TH1D")!=0)
         ((TH1D *)isto)->Write();   
       else if (strcmp(isto->ClassName(),"TH2D")!=0 )  
         ((TH2D *)isto)->Write();
     }
    }
    if (ii == 5) {
     while ((isto =  next5())){
       if (strcmp(isto->ClassName(),"TH1D")!=0)
         ((TH1D *)isto)->Write();   
       else if (strcmp(isto->ClassName(),"TH2D")!=0 )  
         ((TH2D *)isto)->Write();
     }
    } 
    //    fhist->ls();
  }

  /*  
  printf("ffnom %s\n",ffnom);
  gROOT->cd();
  gDirectory->pwd();
  fhist= new TFile(ffnom,"RECREATE");
  fhist->Write();
  fhist->ls(); */
  fhist->Close();

  return;
}



///////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------
//   STD  USERS DO NOT NEED TO EDIT ABOVE THIS POINT
//------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////
 void DHF::BookHistos(Align* alg){

   // histos au niveau des evenements
   AddHisto(new TH1D("cluster_ev","mult en temps",5000,0,50000));
   AddHisto(new TH1F("tra_1px","amas 1p par trace",5000,0,4999));
   AddHisto(new TH1F("tra_1py","amas 1p par trace",5000,0,4999));
   AddHisto(new TH1F("tra_m1px","amas 1p par trace",5000,0,39999));
   AddHisto(new TH1F("tra_m1py","amas 1p par trace",5000,0,39999));
   AddHisto(new TH1D("no_1px","no amas 1p",6,0.5,6.5));
   AddHisto(new TH1D("no_1py","no amas 1p",6,0.5,6.5));
   AddHisto(new TH1D("Accept","bon evenements",16,-0.5,15.5));
   AddHisto(new TH2D("Amult","cote k vs cote s",7,-0.5,6.5,7,-0.5,6.5));
   AddHisto(new TH2D("dxdz","dxdz et comb",15,0,15,50,-0.0003,0.0003));
   AddHisto(new TH2D("dydz","dydz et comb",15,0,15,50,-0.001,0.001));
   AddHisto(new TH2D("dydz_y1","dydz et y comb 5",50,-0.75,0.75,50,-0.0003,0.0003));
   AddHisto(new TH2D("dydz_y3","dydz et y comb 5",50,-0.75,0.75,50,-0.0003,0.0003));
   AddHisto(new TH2D("y1_y3","y1 et y3 comb 5",100,-0.25,0.25,100,-0.25,0.25));
   AddHisto(new TH1D("dxdz_fais","dxdz faisceau 6pt",50,-0.0003,0.0003));   
   AddHisto(new TH1D("dydz_fais","dydz faisceau 6pt",50,-0.0003,0.0003));   

   printf("book align %d\n",alg->align);
   float limxb = -3.6;
   float limxh = 3.6;
   float limyb = -2.2;
   float limyh = 2.2;
   float limbp = -1.5;
   float limhp = 1.5;

   if (alg->align < 2) { 
     limxb = 0.;
     limxh = 7.2;
     limyb = 0.;
     limyh = 4.2;
     limbp = 0.;
     limhp = 3.0;
   }
   
   AddHisto(new TH1D("x0","xpos AMS02 0",500,limxb,limxh));
   AddHisto(new TH1D("y0","ypos AMS02 0",500,limyb,limyh));
   AddHisto(new TH1D("x1","xpos AMS02 1",500,limxb,limxh));
   AddHisto(new TH1D("y1","ypos AMS02 1",500,limyb,limyh));
   AddHisto(new TH1D("x2","xpos AMS02 2",500,limxb,limxh));
   AddHisto(new TH1D("y2","ypos AMS02 2",500,limyb,limyh));
   AddHisto(new TH1D("x3","xpos AMS02 3",500,limxb,limxh));
   if (runno == 1237 && alg->align == 1)
      AddHisto(new TH1D("y3","ypos AMS02 3",500,limyb+2.,limyh+2.));
   else
      AddHisto(new TH1D("y3","ypos AMS02 3",500,limyb,limyh));
   AddHisto(new TH1D("x4","xpos petit 1",500,limbp,limhp));
   AddHisto(new TH1D("y4","ypos petit 1",500,limbp,limhp));
   AddHisto(new TH1D("x5","xpos petit 2",500,limbp,limhp));
   AddHisto(new TH1D("y5","ypos petit 2",500,limbp,limhp));

   AddHisto(new TH2D("y3c","ypos canal AMS02 3",384,640,1024,200,-8.,8.));

   AddHisto(new TH1D("depz","AMS02 3 dz",500,-5.,5.));   

   if (alg->align >= 5) {

    AddHisto(new TH1D("x0hr","xpos AMS02 0",2000,-10,10));
    AddHisto(new TH1D("y0hr","ypos AMS02 0",2000,-10,10));
    AddHisto(new TH1D("x1hr","xpos AMS02 1",2000,-10,10));
    AddHisto(new TH1D("y1hr","ypos AMS02 1",2000,-10,10));
    AddHisto(new TH1D("x2hr","xpos AMS02 2",2000,-10,10));
    AddHisto(new TH1D("y2hr","ypos AMS02 2",2000,-10,10));
    AddHisto(new TH1D("x3hr","xpos AMS02 3",2000,-10,10));
    AddHisto(new TH1D("y3hr","ypos AMS02 3",2000,-10,10));
    AddHisto(new TH1D("x4hr","xpos petit 1",2000,-5,5));
    AddHisto(new TH1D("y4hr","ypos petit 1",2000,-5,5));
    AddHisto(new TH1D("x5hr","xpos petit 2",2000,-5,5));
    AddHisto(new TH1D("y5hr","ypos petit 2",2000,-5,5));

    AddHisto(new TH1D("x01p","xpos AMS02 0",2000,-10,10));
    AddHisto(new TH1D("y01p","ypos AMS02 0",2000,-10,10));
    AddHisto(new TH1D("x11p","xpos AMS02 1",2000,-10,10));
    AddHisto(new TH1D("y11p","ypos AMS02 1",2000,-10,10));
    AddHisto(new TH1D("x21p","xpos AMS02 2",2000,-10,10));
    AddHisto(new TH1D("y21p","ypos AMS02 2",2000,-10,10));
    AddHisto(new TH1D("x31p","xpos AMS02 3",2000,-10,10));
    AddHisto(new TH1D("y31p","ypos AMS02 3",2000,-10,10));
    AddHisto(new TH1D("x41p","xpos petit 1",2000,-5,5));
    AddHisto(new TH1D("y41p","ypos petit 1",2000,-5,5));
    AddHisto(new TH1D("x51p","xpos petit 2",2000,-5,5));
    AddHisto(new TH1D("y51p","ypos petit 2",2000,-5,5));

   }

   if (alg->align == 2) {
     AddHisto(new TH2D("dyx_ref","dyx_ref",100,limxb/2,limxh/2,50,-0.15,0.15));
     AddHisto(new TH2D("dxy_ref","dxy_ref",100,limyb,limyh,50,-0.15,0.15));     
   }

   if (alg->align == 3) {
     printf("align hist\n");
     AddHisto(new TH2D("dxx_pos3","dxx_pos3",100,limxb/2,limxh/2,50,-0.15,0.15));
     AddHisto(new TH2D("dyy_pos3","dyy_pos3",100,limyb,limyh,50,-0.15,0.15));     
     AddHisto(new TH2D("dxx_pos4","dxx_pos4",100,limxb/2,limxh/2,50,-0.15,0.15));
     AddHisto(new TH2D("dyy_pos4","dyy_pos4",100,limyb,limyh,50,-0.15,0.15));     
     AddHisto(new TH2D("dxx_pos1p","dxx_pos1p",100,limxb/2,limxh/2,50,-0.15,0.15));
     AddHisto(new TH2D("dyy_pos1p","dyy_pos1p",100,limyb,limyh,50,-0.15,0.15));     
     AddHisto(new TH2D("dxx_pos2p","dxx_pos2p",100,limxb/2,limxh/2,50,-0.15,0.15));
     AddHisto(new TH2D("dyy_pos2p","dyy_pos2p",100,limyb,limyh,50,-0.15,0.15));     
   }

   if (alg->align == 4) {
     printf("align hist\n");
     AddHisto(new TH2D("dxy_pos3","dxy_pos3",100,limyb,limyh,50,-0.15,0.15));
     AddHisto(new TH2D("dyx_pos3","dyx_pos3",100,limxb/2,limxh/2,50,-0.15,0.15));     
     AddHisto(new TH2D("dxy_pos4","dxy_pos4",100,limyb,limyh,50,-0.15,0.15));
     AddHisto(new TH2D("dyx_pos4","dyx_pos4",100,limxb/2,limxh/2,50,-0.15,0.15));     
     AddHisto(new TH2D("dxy_pos1p","dxy_pos1p",100,limyb,limyh,50,-0.15,0.15));
     AddHisto(new TH2D("dyx_pos1p","dyx_pos1p",100,limxb/2,limxh/2,50,-0.15,0.15));     
     AddHisto(new TH2D("dxy_pos2p","dxy_pos2p",100,limyb,limyh,50,-0.15,0.15));
     AddHisto(new TH2D("dyx_pos2p","dyx_pos2p",100,limxb/2,limxh/2,50,-0.15,0.15));     
   }

   // en mm
   if (alg->align == 5 || alg->align == 6) {
     float prem=1.;
     float dern=10000.;
     printf("prem %f dern %f\n",prem,dern);
     AddHisto(new TH1D("resx0","xres AMS02 0",100,-0.25,0.25));
     AddHisto(new TH1D("resex0","xrese AMS02 0",100,-0.25,0.25));
     AddHisto(new TH1D("resx01p","xres AMS02 0",100,-0.25,0.25));
     AddHisto(new TH1D("resex01p","xrese AMS02 0",100,-0.25,0.25));
     AddHisto(new TH2D("reseposx0","xrese et position AMS02 0",100,0,1,100,-0.25,0.25));

     AddHisto(new TH1D("resy0","yres AMS02 0",100,-0.5,0.5));
     AddHisto(new TH1D("resey0","yrese AMS02 0",100,-0.5,0.5));
     AddHisto(new TH1D("resy01p","yres AMS02 0",100,-0.5,0.5));
     AddHisto(new TH1D("resey01p","yrese AMS02 0",100,-0.5,0.5));
     AddHisto(new TH2D("resetray0","yres et ntrace AMS02 0",500,prem,dern,100,-0.3,0.3));
     AddHisto(new TH2D("reseposy0","yrese et position AMS02 0",100,0,1,100,-0.5,0.5));

     AddHisto(new TH1D("resx1","xres AMS02 1",100,-0.25,0.25));
     AddHisto(new TH1D("resex1","xrese AMS02 1",100,-0.25,0.25));
     AddHisto(new TH1D("resx11p","xres AMS02 1",100,-0.25,0.25));
     AddHisto(new TH1D("resex11p","xrese AMS02 1",100,-0.25,0.25));
     AddHisto(new TH2D("reseposx1","xrese et position AMS02 1",100,0,1,100,-0.25,0.25));

     if (runno == 0) {
      AddHisto(new TH1D("resy1","yres AMS02 1",120,-0.7,0.5));
      AddHisto(new TH1D("resey1","yrese AMS02 1",120,-0.7,0.5));
      AddHisto(new TH1D("resy11p","yres AMS02 1",120,-0.7,0.5));
      AddHisto(new TH1D("resey11p","yrese AMS02 1",120,-0.7,0.5));
      AddHisto(new TH2D("resetray1","yres et ntrace AMS02 1",500,prem,dern,100,-0.3,0.3));
      AddHisto(new TH2D("reseposy1","yrese et position AMS02 1",100,0,1,120,-0.7,0.5));
     }
     else {
      AddHisto(new TH1D("resy1","yres AMS02 1",100,-0.5,0.5));
      AddHisto(new TH1D("resey1","yrese AMS02 1",100,-0.5,0.5));
      AddHisto(new TH1D("resy11p","yres AMS02 1",100,-0.5,0.5));
      AddHisto(new TH1D("resey11p","yrese AMS02 1",100,-0.5,0.5));
      AddHisto(new TH2D("resetray1","yres et ntrace AMS02 1",500,prem,dern,100,-0.3,0.3));
      AddHisto(new TH2D("reseposy1","yrese et position AMS02 1",100,0,1,100,-0.5,0.5));
     }

     AddHisto(new TH1D("resx2","xres AMS02 2",100,-0.25,0.25));
     AddHisto(new TH1D("resex2","xrese AMS02 2",100,-0.25,0.25));
     AddHisto(new TH1D("resx21p","xres AMS02 2",100,-0.25,0.25));
     AddHisto(new TH1D("resex21p","xrese AMS02 2",100,-0.25,0.25));
     AddHisto(new TH2D("reseposx2","xrese et position AMS02 2",100,0,1,100,-0.25,0.25));

     AddHisto(new TH1D("resy2","yres AMS02 2",100,-0.5,0.5));
     AddHisto(new TH1D("resey2","yrese AMS02 2",100,-0.5,0.5));
     AddHisto(new TH1D("resy21p","yres AMS02 2",100,-0.5,0.5));
     AddHisto(new TH1D("resey21p","yrese AMS02 2",100,-0.5,0.5));
     AddHisto(new TH2D("resetray2","yres et ntrace AMS02 2",500,prem,dern,100,-0.3,0.3));
     AddHisto(new TH2D("reseposy2","yrese et position AMS02 2",100,0,1,100,-0.5,0.5));

     AddHisto(new TH1D("resx3","xres AMS02 3",100,-0.25,0.25));
     AddHisto(new TH1D("resex3","xrese AMS02 3",100,-0.25,0.25));
     AddHisto(new TH1D("resx31p","xres AMS02 3",100,-0.25,0.25));
     AddHisto(new TH1D("resex31p","xrese AMS02 3",100,-0.25,0.25));
     AddHisto(new TH2D("reseposx3","xrese et position AMS02 3",100,0,1,100,-0.25,0.25));

     if (runno == 0) {
      AddHisto(new TH1D("resy3","yres AMS02 3",100,-0.5,0.5));
      AddHisto(new TH1D("resey3","yrese AMS02 3",100,-0.5,0.5));
      AddHisto(new TH1D("resy31p","yres AMS02 3",100,-0.5,0.5));
      AddHisto(new TH1D("resey31p","yrese AMS02 3",100,-0.5,0.5));
      AddHisto(new TH2D("resetray3","yres et ntrace AMS02 3",500,prem,dern,100,-0.3,0.3));
      AddHisto(new TH2D("reseposy3","yrese et position AMS02 3",100,0,1,100,-0.5,0.5));
     }
     else {
      AddHisto(new TH1D("resy3","yres AMS02 3",100,-0.5,0.5));
      AddHisto(new TH1D("resey3","yrese AMS02 3",100,-0.5,0.5));
      AddHisto(new TH1D("resy31p","yres AMS02 3",100,-0.5,0.5));
      AddHisto(new TH1D("resey31p","yrese AMS02 3",100,-0.5,0.5));
      AddHisto(new TH2D("resetray3","yres et ntrace AMS02 3",500,prem,dern,100,-0.3,0.3));
      AddHisto(new TH2D("reseposy3","yrese et position AMS02 3",100,0,1,100,-0.5,0.5));
     }

     AddHisto(new TH1D("resx4","xres petit 1",100,-0.25,0.25));
     AddHisto(new TH1D("resex4","xrese petit 1",100,-0.25,0.25));
     AddHisto(new TH1D("resx41p","xres petit 1",100,-0.25,0.25));
     AddHisto(new TH1D("resex41p","xrese petit 1",100,-0.25,0.25));
     AddHisto(new TH2D("reseposx4","xrese et position petit 1",100,0,1,100,-0.25,0.25));

     AddHisto(new TH1D("resy4","yres petit 1",100,-0.5,0.5));
     AddHisto(new TH1D("resey4","yrese petit 1",100,-0.5,0.5));
     AddHisto(new TH1D("resy41p","yres petit 1",100,-0.5,0.5));
     AddHisto(new TH1D("resey41p","yrese petit 1",100,-0.5,0.5));
     AddHisto(new TH2D("resetray4","yres et ntrace petit 1",500,prem,dern,100,-0.3,0.3));
     AddHisto(new TH2D("reseposy4","yrese et position petit 1",100,0,1,100,-0.5,0.5));

     AddHisto(new TH1D("resx5","xres petit 2",100,-0.25,0.25));
     AddHisto(new TH1D("resex5","xrese petit 2",100,-0.25,0.25));
     AddHisto(new TH1D("resx51p","xres petit 2",100,-0.25,0.25));
     AddHisto(new TH1D("resex51p","xrese petit 2",100,-0.25,0.25));
     AddHisto(new TH2D("reseposx5","xrese et position petit 2",100,0,1,100,-0.25,0.25));

     if (runno == 0) {
      AddHisto(new TH1D("resy5","yres petit 2",120,-0.7,0.5));
      AddHisto(new TH1D("resey5","yrese petit 2",120,-0.7,0.5));
      AddHisto(new TH1D("resy51p","yres petit 2",120,-0.7,0.5));
      AddHisto(new TH1D("resey51p","yrese petit 2",120,-0.7,0.5));
      AddHisto(new TH2D("resetray5","yres et ntrace petit 2",500,prem,dern,100,-0.3,0.3));
      AddHisto(new TH2D("reseposy5","yrese et position petit 2",100,0,1,120,-0.7,0.5));
     }
     else {
      AddHisto(new TH1D("resy5","yres petit 2",100,-0.5,0.5));
      AddHisto(new TH1D("resey5","yrese petit 2",100,-0.5,0.5));
      AddHisto(new TH1D("resy51p","yres petit 2",100,-0.5,0.5));
      AddHisto(new TH1D("resey51p","yrese petit 2",100,-0.5,0.5));
      AddHisto(new TH2D("resetray5","yres et ntrace petit 2",500,prem,dern,100,-0.3,0.3));
      AddHisto(new TH2D("reseposy5","yrese et position petit 2",100,0,1,100,-0.5,0.5));
     }

   }

   if (alg->align == 6) {
 
     AddHisto(new TH1D("resx4b","xres petit 1",100,-0.25,0.25));
     AddHisto(new TH1D("resex4b","xrese petit 1",100,-0.25,0.25));
     AddHisto(new TH2D("reseposx4b","xrese et position petit 1",100,0,1,100,-0.25,0.25));

     AddHisto(new TH1D("resy4b","yres petit 1",100,-0.5,0.5));
     AddHisto(new TH1D("resey4b","yrese petit 1",100,-0.5,0.5));
     AddHisto(new TH2D("reseposy4b","yrese et position petit 1",100,0,1,100,-0.5,0.5));
 
     AddHisto(new TH1D("resx5b","xres petit 1",100,-0.25,0.25));
     AddHisto(new TH1D("resex5b","xrese petit 1",100,-0.25,0.25));
     AddHisto(new TH2D("reseposx5b","xrese et position petit 1",100,0,1,100,-0.25,0.25));

     AddHisto(new TH1D("resy5b","yres petit 1",100,-0.5,0.5));
     AddHisto(new TH1D("resey5b","yrese petit 1",100,-0.5,0.5));
     AddHisto(new TH2D("reseposy5b","yrese et position petit 1",100,0,1,100,-0.5,0.5));
     AddHisto(new TH2D("dyy_pos4","dyy_pos4",100,limyb,limyh,50,-0.15,0.15));     
     AddHisto(new TH2D("dyx_pos4","dyx_pos4",100,limxb/2,limxh/2,50,-0.15,0.15));     

     AddHisto(new TH1D("res33x1","xres AMS02 1",100,-0.25,0.25));
     AddHisto(new TH1D("res23x1","xrese AMS02 1",100,-0.25,0.25));

     AddHisto(new TH1D("res33y1","yres AMS02 1",100,-0.5,0.5));
     AddHisto(new TH1D("res23y1","yrese AMS02 1",100,-0.5,0.5));

     AddHisto(new TH1D("res33x2","xres AMS02 2",100,-0.25,0.25));
     AddHisto(new TH1D("res23x2","xrese AMS02 2",100,-0.25,0.25));

     AddHisto(new TH1D("res33y2","yres AMS02 2",100,-0.5,0.5));
     AddHisto(new TH1D("res23y2","yrese AMS02 2",100,-0.5,0.5));

     AddHisto(new TH1D("res33x3","xres AMS02 3",100,-0.25,0.25));
     AddHisto(new TH1D("res23x3","xrese AMS02 3",100,-0.25,0.25));

     AddHisto(new TH1D("res33y3","yres AMS02 3",100,-0.5,0.5));
     AddHisto(new TH1D("res23y3","yrese AMS02 3",100,-0.5,0.5));

     AddHisto(new TH1D("xeff","efficiences cote p",6,0,6));
     AddHisto(new TH1D("yeff","efficiences cote n",6,0,6));
     AddHisto(new TH1D("xineff","inefficiences cote p",6,0,6));
     AddHisto(new TH1D("yineff","inefficiences cote n",6,0,6));
     AddHisto(new TH1D("xbruit","amas faux cote p",6,0,6));
     AddHisto(new TH1D("ybruit","amas faux cote n",6,0,6));
     AddHisto(new TH1D("reseffx0","xrese AMS02 0",100,-0.25,0.25));
     AddHisto(new TH1D("reseffy0","yrese AMS02 0",100,-0.5,0.5));
     AddHisto(new TH1D("reseffx1","xrese AMS02 1",100,-0.25,0.25));
     AddHisto(new TH1D("reseffy1","yrese AMS02 1",100,-0.5,0.5));
     AddHisto(new TH1D("reseffx2","xrese AMS02 2",100,-0.25,0.25));
     AddHisto(new TH1D("reseffy2","yrese AMS02 2",100,-0.5,0.5));
     AddHisto(new TH1D("reseffx3","xrese AMS02 3",100,-0.25,0.25));
     AddHisto(new TH1D("reseffy3","yrese AMS02 3",100,-0.5,0.5));
     AddHisto(new TH1D("reseffx4","xrese petit 1",100,-0.25,0.25));
     AddHisto(new TH1D("reseffy4","yrese petit 1",100,-0.5,0.5));
     AddHisto(new TH1D("reseffx5","xrese petit 2",100,-0.25,0.25));
     AddHisto(new TH1D("reseffy5","yrese petit 2",100,-0.5,0.5));

     AddHisto(new TH1D("ressx0","xress AMS02 0",100,-0.25,0.25));
     AddHisto(new TH1D("ressmx0","xress AMS02 0",100,-0.25,0.25));
     AddHisto(new TH1D("ressy0","yress AMS02 0",100,-0.5,0.5));
     AddHisto(new TH1D("ressmy0","yress AMS02 0",100,-0.5,0.5));
     AddHisto(new TH1D("ressy01p","yress AMS02 0",100,-0.5,0.5));
     AddHisto(new TH1D("ressx1","xress AMS02 1",100,-0.25,0.25));
     AddHisto(new TH1D("ressmx1","xress AMS02 1",100,-0.25,0.25));
     AddHisto(new TH1D("ressy1","yress AMS02 1",100,-0.5,0.5));
     AddHisto(new TH1D("ressmy1","yress AMS02 1",100,-0.5,0.5));
     AddHisto(new TH1D("ressy11p","yress AMS02 1",100,-0.5,0.5));
     AddHisto(new TH1D("ressx2","xress AMS02 2",100,-0.25,0.25));
     AddHisto(new TH1D("ressmx2","xress AMS02 2",100,-0.25,0.25));
     AddHisto(new TH1D("ressy2","yress AMS02 2",100,-0.5,0.5));
     AddHisto(new TH1D("ressmy2","yress AMS02 2",100,-0.5,0.5));
     AddHisto(new TH1D("ressy21p","yress AMS02 2",100,-0.5,0.5));
     AddHisto(new TH1D("ressx3","xress AMS02 3",100,-0.25,0.25));
     AddHisto(new TH1D("ressmx3","xress AMS02 3",100,-0.25,0.25));
     if (runno == 1237) {
      AddHisto(new TH1D("ressy3","yress AMS02 3",100,-0.5,0.5));
      AddHisto(new TH1D("ressmy3","yress AMS02 3",100,-0.5,0.5));
      AddHisto(new TH1D("ressy31p","yress AMS02 3",100,-0.5,0.5));
      AddHisto(new TH1D("ressyb3","yress bord AMS02 3",100,-0.5,0.5));
     }
     else {
      AddHisto(new TH1D("ressy3","yress AMS02 3",100,-0.5,0.5));
      AddHisto(new TH1D("ressmy3","yress AMS02 3",100,-0.5,0.5));
      AddHisto(new TH1D("ressy31p","yress AMS02 3",100,-0.5,0.5));
     }

     AddHisto(new TH1D("resesx0","xress AMS02 0",100,-0.25,0.25));
     AddHisto(new TH1D("resesmx0","xress AMS02 0",100,-0.25,0.25));
     AddHisto(new TH1D("resesy0","yress AMS02 0",100,-0.5,0.5));
     AddHisto(new TH1D("resesmy0","xress AMS02 0",100,-0.5,0.5));
     AddHisto(new TH1D("resesy01p","yress AMS02 0",100,-0.5,0.5));
     AddHisto(new TH2D("resesposy0","yrese et position AMS02 0",100,0,1,100,-0.5,0.5));
     AddHisto(new TH1D("resesx1","xress AMS02 1",100,-0.25,0.25));
     AddHisto(new TH1D("resesmx1","xress AMS02 0",100,-0.25,0.25));
     AddHisto(new TH1D("resesy1","yress AMS02 1",100,-0.5,0.5));
     AddHisto(new TH1D("resesmy1","xress AMS02 0",100,-0.5,0.5));
     AddHisto(new TH1D("resesy11p","yress AMS02 1",100,-0.5,0.5));
     AddHisto(new TH2D("resesposy1","yrese et position AMS02 1",100,0,1,100,-0.5,0.5));
     AddHisto(new TH1D("resesx2","xress AMS02 2",100,-0.25,0.25));
     AddHisto(new TH1D("resesmx2","xress AMS02 0",100,-0.25,0.25));
     AddHisto(new TH1D("resesy2","yress AMS02 2",100,-0.5,0.5));
     AddHisto(new TH1D("resesmy2","xress AMS02 0",100,-0.5,0.5));
     AddHisto(new TH1D("resesy21p","yress AMS02 2",100,-0.5,0.5));
     AddHisto(new TH2D("resesposy2","yrese et position AMS02 2",100,0,1,100,-0.5,0.5));
     AddHisto(new TH1D("resesx3","xress AMS02 3",100,-0.25,0.25));
     AddHisto(new TH1D("resesmx3","xress AMS02 0",100,-0.25,0.25));
     if (runno == 1237) {
      AddHisto(new TH1D("resesy3","yress AMS02 3",130,-0.5,0.5));
      AddHisto(new TH1D("resesmy3","xress AMS02 0",100,-0.5,0.5));
      AddHisto(new TH1D("resesy31p","yress AMS02 3",130,-0.5,0.5));
      AddHisto(new TH1D("resesyb3","yress bord AMS02 3",130,-0.5,0.5));
      AddHisto(new TH2D("resesposy3","yrese et position AMS02 3",100,0,1,130,-0.5,0.5));
     }
     else {
      AddHisto(new TH1D("resesy3","yress AMS02 3",100,-0.5,0.5));
      AddHisto(new TH1D("resesmy3","xress AMS02 0",100,-0.5,0.5));
      AddHisto(new TH1D("resesy31p","yress AMS02 3",100,-0.5,0.5));
      AddHisto(new TH2D("resesposy3","yrese et position AMS02 3",100,0,1,100,-0.5,0.5));
      AddHisto(new TH2D("resesy3pi","yres AMS02 3",384,641.,1024.,100,-0.5,0.5));
     }
   }
 
   // histos au niveau des TDRs
   Define("Occupancy","Occupancy",1024,0,1024,0,"dir");
   Define("Width","Width",50,0.5,50.5,1,"dir");
   Define("Width2","Width",50,0.5,50.5,1,"dir");
   Define("Larg","Larg",50,0.5,50.5,1,"dir");
   Define("NPiste","Npistes",10,0.5,10.5,1,"dir");
   Define("Signal","Signal",750,0,750,1,"dir");
   Define("SN","SN",500,0,250,1,"dir");
   Define("SeedSN","SeedSN",500,0,250,1,"dir");
   Define("OccupancyCOG","OccupancyCOG",1024,0,1024,0,"dir");
   Define("Eta","Eta",120,0.,1.0,1,"dir");
   Define("Eta3","Eta3",120,0.,2.0,1,"dir");
   Define("STot_VS_SeedAdd","STot_VS_SeedAdd",1024,0,1024,500,0,500,0);
   Define("EtaRaw","EtaRaw",120,-0.6,0.6,1,"dir");
   Define("EtaRaw2","EtaRaw2",120,-0.6,0.6,1,"dir");
   Define("OccupancyGap","OccupancyGap",100,0,1,1,"dir");
   Define("Multiplicity","Multiplicity",100,0,100,1,"dir");
   Define("Status_Seed_Strip","Status_Seed_Strip",1024,0,1024,0,"dir");
   Define("Piedestaux","Piedestaux",1024,0,1024,0,"dir");
   Define("Sigma","Sigma",1024,0,1024,0,"dir");
   Define("Statut","Statut",1024,0,1024,0,"dir");
   Define("SNpiste","SN",11,-0.5,10.5,1,"dir");
   Define("NSNpiste","entrees",11,-0.5,10.5,1,"dir");
   Define("SNpiste2","SN",11,-0.5,10.5,1,"dir");
   Define("NSNpiste2","entrees",11,-0.5,10.5,1,"dir");
   Define("SNp","SN",11,-0.5,10.5,1,"dir");
   Define("NSNp","entrees",11,-0.5,10.5,1,"dir");
   Define("SNp2","SN",11,-0.5,10.5,1,"dir");
   Define("NSNp2","entrees",11,-0.5,10.5,1,"dir");
   Define("EVpge8","evenements grand amas",500,0,1000000,1,"dir");
   Define("Pge8","pistes des grand amas",1024,0,1024,0,"dir");

   return;

 }

 void DHF::FillAll(Event* ev, RHClass* rh, Align* alg) {

   TH1D *h1d;
   TH2D *h2d;
   char hnom[255];

   int ech;
   //   printf("nclus %d\n",ev->Cls->GetEntries());
   Fill1D("cluster_ev",((float) ev->Evtnum),((float) ev->Cls->GetEntries()));
   for (int ii=0;ii<ev->Cls->GetEntries();ii++){
     Cluster* clu=(Cluster*)ev->Cls->At(ii);
     if (clu->ladder != -1) {
       ech = rh->FindPos(clu->ladder);
       if (ech < 4) {
        Fill(0,clu,"Occupancy",clu->GetSeedAdd()*1.);
        Fill(1,clu,"Width2",((float) clu->GetLength()));
        int off = 5 - clu->GetSeed();
        for (int ip=0; ip<clu->GetLength(); ip++) {
	 Fill(1,clu,"SNpiste2",((float)(ip+off)),clu->Signal[ip]/clu->Noise[ip]);
	 Fill(1,clu,"NSNpiste2",((float)(ip+off)),1.);
         if (clu->GetLength() >= 8) {
	  Fill(1,clu,"SNp2",((float) (ip+off)),clu->Signal[ip]/clu->Noise[ip]);
	  Fill(1,clu,"NSNp2",((float)(ip+off)),1.);
          Fill(0,clu,"Pge8",((float) (ip+clu->GetSeed()))+clu->GetSeedAdd()*1.);
         }
        }
       }
       else if (ech >= 4 && alg->mauvais[ech][clu->GetSeedAdd()-1] == 0) {
        Fill(0,clu,"Occupancy",clu->GetSeedAdd()*1.);
        Fill(1,clu,"Width2",((float) clu->GetLength()));
        int off = 5 - clu->GetSeed();
        for (int ip=0; ip<clu->GetLength(); ip++) {
	 Fill(1,clu,"SNpiste2",((float)(ip+off)),clu->Signal[ip]/clu->Noise[ip]);
	 Fill(1,clu,"NSNpiste2",((float)(ip+off)),1.);
         if (clu->GetLength() >= 8) {
	  Fill(1,clu,"SNp2",((float)(ip+off)),clu->Signal[ip]/clu->Noise[ip]);
	  Fill(1,clu,"NSNp2",((float)(ip+off)),1.);
          Fill(0,clu,"Pge8",((float) (ip+clu->GetSeed()))+clu->GetSeedAdd()*1.);
         }
        }
       }
     }
   }

   float nas = 0.;
   float nak = 0.;
   for (int ii=0; ii<4; ii++) {
     //     printf("tdr %d mult s %d mult k %d\n",TDRNum[ii],ev->NClus[ii][0],ev->NClus[ii][1]);
     sprintf(hnom,"Multiplicity_%04d_S",TDRNum[ii]);
     h1d = (TH1D *) fHlist->FindObject(hnom);
     //     printf("%s Pointer: %hx\n", hnom, h1d);
     h1d->Fill(((float)ev->NClus[ii][0]),1.);
     sprintf(hnom,"Multiplicity_%04d_K",TDRNum[ii]);
     h1d = (TH1D *) fHlist->FindObject(hnom);
     h1d->Fill(((float)ev->NClus[ii][1]),1.);
     if (ev->NClus[ii][0]==1) nas+=1.;
     if (ev->NClus[ii][1]==1) nak+=1.;
   }
   h2d = (TH2D *) fHlist->FindObject("Amult"); 
   Fill2D("Amult",nas,nak,1.);  
                     
   return;
 }

 void DHF::FillSelect(Event* ev, RHClass* rh, Align* alg) {

   //   printf("evtnom %d\n",ev->Evtnum);
   Trace* tra=(Trace*)ev->Tra->At(0);
   /*   for (int ie=3; ie<4; ie++) {
       printf("ie %d x %lf y %lf zpos %lf\n",ie,tra->xx[0][ie],tra->yy[0][ie],alg->z_fixe[ie]);
       printf("     xpos %lf ypos %lf zpos %lf\n",tra->x[ie],tra->y[ie],alg->z_fixe[ie]);
       } */ 
   Fill1D("x0",tra->x[0],1.);
   Fill1D("y0",tra->y[0],1.);
   Fill1D("x1",tra->x[1],1.);
   Fill1D("y1",tra->y[1],1.);
   Fill1D("x2",tra->x[2],1.);
   Fill1D("y2",tra->y[2],1.);
   Fill1D("x3",tra->x[3],1.);
   Fill1D("y3",tra->y[3],1.);
   Cluster* clu = ev->GetCluster(tra->amas_entree[1][3]);
   Fill2D("y3c",clu->GetCoG()*1.,tra->y[3],1.);   
   Fill1D("x4",tra->x[4],1.);
   Fill1D("y4",tra->y[4],1.);
   Fill1D("x5",tra->x[5],1.);
   Fill1D("y5",tra->y[5],1.);
   Fill1D("depz",alg->depz,1.);

   if (alg->align >= 5) {

    Fill1D("dxdz_fais",tra->dxdz,1.);
    Fill1D("dydz_fais",tra->dydz,1.);

    char chist[10];
    for (int i=0; i<6; i++) {
      Cluster* clu = ev->GetCluster(tra->amas_entree[0][i]);
      if (clu->npiste == 2) {
         sprintf(chist,"x%dhr",i);
         Fill1D(chist,10.*(tra->x[i]),1.);
      }
      else if (clu->npiste == 1) {
         sprintf(chist,"x%d1p",i);
         Fill1D(chist,10.*(tra->x[i]),1.);
      }
      clu = ev->GetCluster(tra->amas_entree[1][i]);
      if (clu->npiste >= 2) {
         sprintf(chist,"y%dhr",i);
         Fill1D(chist,10.*(tra->y[i]),1.);
      }
      else if (clu->npiste == 1) {
         sprintf(chist,"y%d1p",i);
         Fill1D(chist,10.*(tra->y[i]),1.);
      }
    }
   }
   
   int ech;
   for (int ii=0;ii<ev->Cls->GetEntries();ii++){
     Cluster* clu=(Cluster*)ev->Cls->At(ii);
     if (clu->ladder != -1) {
       ech = rh->FindPos(clu->ladder);
       if (ech < 4) {
       // printf("ii %d lad %d\n",ii,clu->ladder);
       // printf("tdr %d cote %d\n",rh->FindPos(clu->ladder),clu->side);
	 //        Fill(0,clu,"Occupancy",clu->GetSeedAdd()*1.);
        Fill(1,clu,"Width",((float) clu->GetLength()));
        Fill(1,clu,"Larg",((float) clu->GetLargeur(1.)));
	//        if (ech == 3 && clu->side == 1) printf("select npiste %d\n",clu->npiste); 
        Fill(1,clu,"NPiste",((float) clu->npiste));
        Fill(0,clu,"OccupancyCOG",clu->GetCoG()*1.);
        Fill(1,clu,"OccupancyGap",fmod(clu->GetCoG(),1.));
        Fill(1,clu,"Signal",clu->GetTotSig(1.)*1.);
        Fill(1,clu,"SN",clu->GetTotSN(1.)*1.);
        Fill(1,clu,"SeedSN",clu->GetSeedSN()*1.);
        Fill(1,clu,"Eta",clu->GetEtaTrue()*1.);
        Fill(1,clu,"Eta3",clu->GetEta3()*1.);
        Fill(1,clu,"EtaRaw",clu->GetEtaRaw()*1.);
        Fill(0,clu,"STot_VS_SeedAdd",clu->GetSeedAdd()*1.,clu->GetTotSig(1.));
        Fill(0,clu,"Status_Seed_Strip",clu->GetSeedAdd()*1.,((float)clu->Status[clu->GetSeedAdd()]));
        int off = 5 - clu->GetSeed();
        for (int ip=0; ip<clu->GetLength(); ip++) {
	  Fill(1,clu,"SNpiste",((float)(ip+off)),clu->Signal[ip]/clu->Noise[ip]);
	  Fill(1,clu,"NSNpiste",((float)(ip+off)),1.);
          if (clu->GetLength() >= 8) {
 	   Fill(1,clu,"SNp",((float)(ip+off)),clu->Signal[ip]/clu->Noise[ip]);
	   Fill(1,clu,"NSNp",((float)(ip+off)),1.);
           if (ip == 0) Fill(1,clu,"EVpge8",((float) ev->Evtnum),1.);
	   //           if (clu->GetSeedAdd() > 640 && ech == 0) printf("add %d seed %d ip %d sn %f\n",
	   //		clu->GetSeedAdd(),clu->GetSeed(),ip,clu->Signal[ip]/clu->Noise[ip]);
          }
        }
       }
       else if (ech >= 4 && alg->mauvais[ech][clu->GetSeedAdd()-1] == 0) {
       // printf("ii %d lad %d\n",ii,clu->ladder);
       // printf("tdr %d cote %d\n",rh->FindPos(clu->ladder),clu->side);
	 //        Fill(0,clu,"Occupancy",clu->GetSeedAdd()*1.);
	if ( ii == tra->amas_entree[0][ech] || ii == tra->amas_entree[1][ech] ) {
         Fill(1,clu,"Width",((float) clu->GetLength()));
         Fill(1,clu,"Larg",((float) clu->GetLargeur(1.)));
         Fill(1,clu,"NPiste",((float) clu->npiste));
         Fill(0,clu,"OccupancyCOG",clu->GetCoG()*1.);
         Fill(1,clu,"OccupancyGap",fmod(clu->GetCoG(),1.));
         Fill(1,clu,"Signal",clu->GetTotSig(1.)*1.);
         Fill(1,clu,"SN",clu->GetTotSN(1.)*1.);
         Fill(1,clu,"SeedSN",clu->GetSeedSN()*1.);
         Fill(1,clu,"Eta",clu->GetEta()*1.);
         Fill(1,clu,"EtaRaw",clu->GetEtaRaw()*1.);
         Fill(0,clu,"STot_VS_SeedAdd",clu->GetSeedAdd()*1.,clu->GetTotSig(1.));
         Fill(0,clu,"Status_Seed_Strip",clu->GetSeedAdd()*1.,((float)clu->Status[clu->GetSeedAdd()]));
         int off = 5 - clu->GetSeed();
         for (int ip=0; ip<clu->GetLength(); ip++) {
	   Fill(1,clu,"SNpiste",((float)(ip+off)),clu->Signal[ip]/clu->Noise[ip]);
	   Fill(1,clu,"NSNpiste",((float)(ip+off)),1.);
           if (clu->GetLength() >= 8) {
 	    Fill(1,clu,"SNp",((float)(ip+off)),clu->Signal[ip]/clu->Noise[ip]);
	    Fill(1,clu,"NSNp",((float)(ip+off)),1.);
            if (ip == 0) Fill(1,clu,"EVpge8",((float) ev->Evtnum),1.);
            }
         }
	}
       }
     }
     //       else clu->Print();
   }
                     
   return;
 }

void DHF::align_xy_positions(Align* alg) {
  double x0,x1,x2,x3,x4,x5,y0,y1,y2,y3,y4,y5;
  TH1D* temp;
  if (alg->align == 1) {
    temp = (TH1D *) fHlist->FindObject("x0");
    x0 = temp->GetMean();
    temp = (TH1D *) fHlist->FindObject("x1");
    x1 = temp->GetMean();
    temp = (TH1D *) fHlist->FindObject("x2");
    x2 = temp->GetMean();
    temp = (TH1D *) fHlist->FindObject("x3");
    x3 = temp->GetMean();
    temp = (TH1D *) fHlist->FindObject("x4");
    x4 = temp->GetMean();
    temp = (TH1D *) fHlist->FindObject("x5");
    x5 = temp->GetMean();
    temp = (TH1D *) fHlist->FindObject("y0");
    y0 = temp->GetMean();
    temp = (TH1D *) fHlist->FindObject("y1");
    y1 = temp->GetMean();
    temp = (TH1D *) fHlist->FindObject("y2");
    y2 = temp->GetMean();
    temp = (TH1D *) fHlist->FindObject("y3");
    if (runno != 1237)
     y3 = temp->GetMean();
    else {
     float ymaxp1 = alg->plaq_dim_active[1];
     float yminp2 = ymaxp1 + (alg->plaq_dim[1]-alg->plaq_dim_active[1]) + alg->ecart;
     float ycen = (ymaxp1 + yminp2)/2.;
     int nbin = temp->GetNbinsX();
     float ybord1 = -999.;
     float ybord2 = 999.;
     float bbb, bbh, blarg;
     for (int ib=1; ib<=nbin; ib++) {
       bbb = temp->GetBinLowEdge(ib);
       blarg = temp->GetBinWidth(ib);
       bbh = bbb + blarg;
       if (temp->GetBinContent(ib) > 5.) {
         if (bbb > ycen && bbb < ybord2) ybord2=bbb;
         if (bbh < ycen && bbb > ybord1) ybord1=bbh;
       }
     }
     y3 = (ybord1+ybord2)/2.;
     printf("y3 ybord1 %f ybord2 %f cen12 %f\n",ybord1,ybord2,y3); 
    }
    temp = (TH1D *) fHlist->FindObject("y4");
    y4 = temp->GetMean();
    temp = (TH1D *) fHlist->FindObject("y5");
    y5 = temp->GetMean();
    alg->MaJXYpos(0,x0,y0,x1,y1,x2,y2,x3,y3,x4,y4,x5,y5);
  }
  else if (alg->align == 5) {
    // histos en mm, parametres d'alignement en cm
    temp = (TH1D *) fHlist->FindObject("resx0");
    x0 = temp->GetMean()/10.;
    temp = (TH1D *) fHlist->FindObject("resx1");
    x1 = temp->GetMean()/10.;
    temp = (TH1D *) fHlist->FindObject("resx2");
    x2 = temp->GetMean()/10.;
    temp = (TH1D *) fHlist->FindObject("resx3");
    x3 = temp->GetMean()/10.;
    temp = (TH1D *) fHlist->FindObject("resx4");
    x4 = temp->GetMean()/10.;
    temp = (TH1D *) fHlist->FindObject("resx5");
    x5 = temp->GetMean()/10.;
    temp = (TH1D *) fHlist->FindObject("resy0");
    y0 = temp->GetMean()/10.;
    temp = (TH1D *) fHlist->FindObject("resy1");
    y1 = temp->GetMean()/10.;
    temp = (TH1D *) fHlist->FindObject("resy2");
    y2 = temp->GetMean()/10.;
    temp = (TH1D *) fHlist->FindObject("resy3");
    y3 = temp->GetMean()/10.;
    temp = (TH1D *) fHlist->FindObject("resy4");
    y4 = temp->GetMean()/10.;
    temp = (TH1D *) fHlist->FindObject("resy5");
    y5 = temp->GetMean()/10.;
    alg->MaJXYpos(1,x0,y0,x1,y1,x2,y2,x3,y3,x4,y4,x5,y5);
  }
  alg->EcrireAlgPar();
  printf("ecrire parametre dans algpar.dat\n");
  return;
}

void DHF::align_ref_dyx_dxy(Align* alg) {

  printf("nfit1 %d\n",alg->nfit1);
  for (int i=0; i<alg->nfit1; i++)
    Fill2D("dxy_ref",alg->xval1[i],alg->yval1[i],1.);    
  for (int i=0; i<alg->nfit2; i++)
    Fill2D("dyx_ref",alg->xval2[i],alg->yval2[i],1.);    
  TGraph* gfit1 = new TGraph(alg->nfit1,alg->xval1,alg->yval1);
  TF1* f1 = new TF1("f1","pol1",-5,5);
  gfit1->Fit("f1");
  float pente1 = f1->GetParameter(1);
  float inter1 = f1->GetParameter(0);
  double ang1 = 1000.*atan(pente1);
  printf("ang1 %f mrad\n",ang1);
  TGraph* gfit2 = new TGraph(alg->nfit2,alg->xval2,alg->yval2);
  gfit2->Fit("f1");
  float pente2 = f1->GetParameter(1);
  float inter2 = f1->GetParameter(0);
  double ang2 = 1000.*atan(pente2);
  printf("ang2 %f mrad\n",ang2);
  alg->MaJRotRef1(pente1,inter1,pente2,inter2);
  alg->EcrireAlgPar();
  printf("ecrire parametre dans algpar.dat\n");
  return;
}

void DHF::align_dxx_dyy(Align* alg) {

  //  for (int i=0; i<10; i++)
  //    printf("i %d xval %f yval %f\n",i,alg->xval1[i],alg->yval1[i]);
  for (int i=0; i<alg->nfit1; i++)
    Fill2D("dxx_pos3",alg->xval1[i],alg->yval1[i],1.);    
  for (int i=0; i<alg->nfit2; i++)
    Fill2D("dyy_pos3",alg->xval2[i],alg->yval2[i],1.);    
  if (alg->deux_plaq_n[3] == 1) {
   for (int i=0; i<alg->nfit2_2; i++)
    Fill2D("dyy_pos3",alg->xval2_2[i],alg->yval2_2[i],1.);    
  }
  for (int i=0; i<alg->nfit3; i++)
    Fill2D("dxx_pos4",alg->xval3[i],alg->yval3[i],1.);    
  for (int i=0; i<alg->nfit4; i++)
    Fill2D("dyy_pos4",alg->xval4[i],alg->yval4[i],1.);    
  for (int i=0; i<alg->nfit5; i++)
    Fill2D("dxx_pos1p",alg->xval5[i],alg->yval5[i],1.);    
  for (int i=0; i<alg->nfit6; i++)
    Fill2D("dyy_pos1p",alg->xval6[i],alg->yval6[i],1.);    
  for (int i=0; i<alg->nfit7; i++)
    Fill2D("dxx_pos2p",alg->xval7[i],alg->yval7[i],1.);    
  for (int i=0; i<alg->nfit8; i++)
    Fill2D("dyy_pos2p",alg->xval8[i],alg->yval8[i],1.);    
  TGraph* gfit1 = new TGraph(alg->nfit1,alg->xval1,alg->yval1);
  TF1* f1 = new TF1("f1","pol1",-2,2);
  gfit1->Fit("f1");
  float pente1 = f1->GetParameter(1);
  float inter1 = f1->GetParameter(0);
  double ang1 = 1000.*atan(pente1);
  printf("ang1 %f mrad\n",ang1);
  TGraph* gfit2 = new TGraph(alg->nfit2,alg->xval2,alg->yval2);
  gfit2->Fit("f1");
  float pente2 = f1->GetParameter(1);
  float inter2 = f1->GetParameter(0);
  double ang2 = 1000.*atan(pente2);
  printf("ang2 %f mrad\n",ang2);
  float pente2_2 = 0.;
  float inter2_2 = 0.;
  if (alg->deux_plaq_n[3] == 1 && alg->nfit2_2 > 0) {
    printf("nfit2_2 %d\n",alg->nfit2_2);
    for (int i=0; i<alg->nfit2_2; i++) printf("i %d x %f y %f\n",i,alg->xval2_2[i],alg->yval2_2[i]);
    TGraph* gfit2_2 = new TGraph(alg->nfit2_2,alg->xval2_2,alg->yval2_2);
    gfit2_2->Fit("f1");
    pente2_2 = f1->GetParameter(1);
    inter2_2 = f1->GetParameter(0);
    double ang2_2 = 1000.*atan(pente2_2);
    printf("ang2_2 %f mrad\n",ang2_2);
  }
  TGraph* gfit3 = new TGraph(alg->nfit3,alg->xval3,alg->yval3);
  gfit3->Fit("f1");
  float pente3 = f1->GetParameter(1);
  float inter3 = f1->GetParameter(0);
  double ang3 = 1000.*atan(pente3);
  printf("ang3 %f mrad\n",ang3);
  TGraph* gfit4 = new TGraph(alg->nfit4,alg->xval4,alg->yval4);
  gfit4->Fit("f1");
  float pente4 = f1->GetParameter(1);
  float inter4 = f1->GetParameter(0);
  double ang4 = 1000.*atan(pente4);
  printf("ang4 %f mrad\n",ang4);
  TGraph* gfit5 = new TGraph(alg->nfit5,alg->xval5,alg->yval5);
  gfit5->Fit("f1");
  float pente5 = f1->GetParameter(1);
  float inter5 = f1->GetParameter(0);
  double ang5 = 1000.*atan(pente5);
  printf("ang5 %f mrad\n",ang5);
  TGraph* gfit6 = new TGraph(alg->nfit6,alg->xval6,alg->yval6);
  gfit6->Fit("f1");
  float pente6 = f1->GetParameter(1);
  float inter6 = f1->GetParameter(0);
  double ang6 = 1000.*atan(pente6);
  printf("ang6 %f mrad\n",ang6);
  TGraph* gfit7 = new TGraph(alg->nfit7,alg->xval7,alg->yval7);
  gfit7->Fit("f1");
  float pente7 = f1->GetParameter(1);
  float inter7 = f1->GetParameter(0);
  double ang7 = 1000.*atan(pente7);
  printf("ang7 %f mrad\n",ang7);
  TGraph* gfit8 = new TGraph(alg->nfit8,alg->xval8,alg->yval8);
  gfit8->Fit("f1");
  float pente8 = f1->GetParameter(1);
  float inter8 = f1->GetParameter(0);
  double ang8 = 1000.*atan(pente8);
  printf("ang8 %f mrad\n",ang8);
  alg->MaJRotxxyy(pente1,inter1,pente2,inter2,pente3,inter3,pente4,inter4,
      pente5,inter5,pente6,inter6,pente7,inter7,pente8,inter8,pente2_2,inter2_2);  
  alg->EcrireAlgPar();
  printf("ecrire parametre dans algpar.dat\n");
  return;
}

void DHF::align_dxy_dyx(Align* alg) {

  //  for (int i=0; i<10; i++)
  //    printf("i %d xval %f yval %f\n",i,alg->xval1[i],alg->yval1[i]);
  for (int i=0; i<alg->nfit1; i++)
    Fill2D("dxy_pos3",alg->xval1[i],alg->yval1[i],1.);    
  for (int i=0; i<alg->nfit2; i++)
    Fill2D("dyx_pos3",alg->xval2[i],alg->yval2[i],1.);    
  for (int i=0; i<alg->nfit3; i++)
    Fill2D("dxy_pos4",alg->xval3[i],alg->yval3[i],1.);    
  for (int i=0; i<alg->nfit4; i++)
    Fill2D("dyx_pos4",alg->xval4[i],alg->yval4[i],1.);    
  for (int i=0; i<alg->nfit5; i++)
    Fill2D("dxy_pos1p",alg->xval5[i],alg->yval5[i],1.);    
  for (int i=0; i<alg->nfit6; i++)
    Fill2D("dyx_pos1p",alg->xval6[i],alg->yval6[i],1.);    
  for (int i=0; i<alg->nfit7; i++)
    Fill2D("dxy_pos2p",alg->xval7[i],alg->yval7[i],1.);    
  for (int i=0; i<alg->nfit8; i++)
    Fill2D("dyx_pos2p",alg->xval8[i],alg->yval8[i],1.);    
  TGraph* gfit1 = new TGraph(alg->nfit1,alg->xval1,alg->yval1);
  TF1* f1 = new TF1("f1","pol1",-2,2);
  gfit1->Fit("f1");
  float pente1 = f1->GetParameter(1);
  float inter1 = f1->GetParameter(0);
  double ang1 = 1000.*atan(pente1);
  printf("ang1 %f mrad\n",ang1);
  TGraph* gfit2 = new TGraph(alg->nfit2,alg->xval2,alg->yval2);
  gfit2->Fit("f1");
  float pente2 = f1->GetParameter(1);
  float inter2 = f1->GetParameter(0);
  double ang2 = 1000.*atan(pente2);
  printf("ang2 %f mrad\n",ang2);
  TGraph* gfit3 = new TGraph(alg->nfit3,alg->xval3,alg->yval3);
  gfit3->Fit("f1");
  float pente3 = f1->GetParameter(1);
  float inter3 = f1->GetParameter(0);
  double ang3 = 1000.*atan(pente3);
  printf("ang3 %f mrad\n",ang3);
  TGraph* gfit4 = new TGraph(alg->nfit4,alg->xval4,alg->yval4);
  gfit4->Fit("f1");
  float pente4 = f1->GetParameter(1);
  float inter4 = f1->GetParameter(0);
  double ang4 = 1000.*atan(pente4);
  printf("ang4 %f mrad\n",ang4);
  TGraph* gfit5 = new TGraph(alg->nfit5,alg->xval5,alg->yval5);
  gfit5->Fit("f1");
  float pente5 = f1->GetParameter(1);
  float inter5 = f1->GetParameter(0);
  double ang5 = 1000.*atan(pente5);
  printf("ang5 %f mrad\n",ang5);
  TGraph* gfit6 = new TGraph(alg->nfit6,alg->xval6,alg->yval6);
  gfit6->Fit("f1");
  float pente6 = f1->GetParameter(1);
  float inter6 = f1->GetParameter(0);
  double ang6 = 1000.*atan(pente6);
  printf("ang6 %f mrad\n",ang6);
  TGraph* gfit7 = new TGraph(alg->nfit7,alg->xval7,alg->yval7);
  gfit7->Fit("f1");
  float pente7 = f1->GetParameter(1);
  float inter7 = f1->GetParameter(0);
  double ang7 = 1000.*atan(pente7);
  printf("ang7 %f mrad\n",ang7);
  TGraph* gfit8 = new TGraph(alg->nfit8,alg->xval8,alg->yval8);
  gfit8->Fit("f1");
  float pente8 = f1->GetParameter(1);
  float inter8 = f1->GetParameter(0);
  double ang8 = 1000.*atan(pente8);
  printf("ang8 %f mrad\n",ang8);
  alg->MaJRotxyyx(pente1,inter1,pente2,inter2,pente3,inter3,pente4,inter4,
        pente5,inter5,pente6,inter6,pente7,inter7,pente8,inter8);
  alg->EcrireAlgPar();
  printf("ecrire parametre dans algpar.dat\n");
  return;
}

void DHF::residus(Event* ev, Align* alg) {
   
  Trace* tra=(Trace*)ev->Tra->At(0);

  float amas_1p_x = 0.;
  float amas_1p_y = 0.;
  // en mm
  Fill1D("resx0",10.*alg->resx[0],1.);    
  Fill1D("resex0",10.*alg->resex[0],1.);    
  float pos = fmod(alg->ipred(0,0,tra),1.);
  Fill2D("reseposx0",pos,10.*alg->resex[0],1.);  
  Cluster* clu=(Cluster*)ev->Cls->At(tra->amas_entree[0][0]);
  if (clu->npiste == 1) {
    Fill1D("resx01p",10.*alg->resx[0],1.);    
    Fill1D("resex01p",10.*alg->resex[0],1.);    
    amas_1p_x++;
  }

  Fill1D("resx1",10.*alg->resx[1],1.);    
  Fill1D("resex1",10.*alg->resex[1],1.);    
  pos = fmod(alg->ipred(1,0,tra),1.);
  Fill2D("reseposx1",pos,10.*alg->resex[1],1.);  
  clu=(Cluster*)ev->Cls->At(tra->amas_entree[0][1]);
  if (clu->npiste == 1) {
    Fill1D("resx11p",10.*alg->resx[1],1.);    
    Fill1D("resex11p",10.*alg->resex[1],1.);    
    amas_1p_x++;
  }

  Fill1D("resx2",10.*alg->resx[2],1.);    
  Fill1D("resex2",10.*alg->resex[2],1.);    
  pos = fmod(alg->ipred(2,0,tra),1.);
  Fill2D("reseposx2",pos,10.*alg->resex[2],1.);  
  clu=(Cluster*)ev->Cls->At(tra->amas_entree[0][2]);
  if (clu->npiste == 1) {
    Fill1D("resx21p",10.*alg->resx[2],1.);    
    Fill1D("resex21p",10.*alg->resex[2],1.);    
    amas_1p_x++;
  }

  Fill1D("resx3",10.*alg->resx[3],1.);    
  Fill1D("resex3",10.*alg->resex[3],1.);    
  pos = fmod(alg->ipred(3,0,tra),1.);
  Fill2D("reseposx3",pos,10.*alg->resex[3],1.);  
  clu=(Cluster*)ev->Cls->At(tra->amas_entree[0][3]);
  if (clu->npiste == 1) {
    Fill1D("resx31p",10.*alg->resx[3],1.);    
    Fill1D("resex31p",10.*alg->resex[3],1.);    
    amas_1p_x++;
  }

  Fill1D("resx4",10.*alg->resx[4],1.);    
  Fill1D("resex4",10.*alg->resex[4],1.);    
  pos = fmod(alg->ipred(4,0,tra),1.);
  Fill2D("reseposx4",pos,10.*alg->resex[4],1.);  
  clu=(Cluster*)ev->Cls->At(tra->amas_entree[0][4]);
  if (clu->npiste == 1) {
    Fill1D("resx41p",10.*alg->resx[4],1.);    
    Fill1D("resex41p",10.*alg->resex[4],1.);    
    amas_1p_x++;
  }

  Fill1D("resx5",10.*alg->resx[5],1.);    
  Fill1D("resex5",10.*alg->resex[5],1.);    
  pos = fmod(alg->ipred(5,0,tra),1.);
  Fill2D("reseposx5",pos,10.*alg->resex[5],1.);  
  clu=(Cluster*)ev->Cls->At(tra->amas_entree[0][5]);
  if (clu->npiste == 1) {
    Fill1D("resx51p",10.*alg->resx[5],1.);    
    Fill1D("resex51p",10.*alg->resex[5],1.);    
    amas_1p_x++;
  }
  //  printf("residus y ntrace %d\n",alg->ntrace);
  Fill1D("resy0",10.*alg->resy[0],1.);    
  Fill1D("resey0",10.*alg->resey[0],1.);    
  /*  float piste = clu->GetSeedAdd();
  float cog = clu->GetCoG();
  float ppiste = alg->ipred(0,1,tra);
  printf("y0 piste %f cog %f ppiste %f\n",piste,cog,ppiste);*/
  Fill2D("resetray0",((float)alg->ntrace),10.*alg->resey[0],1.);
  pos = fmod(alg->ipred(0,1,tra),1.);
  Fill2D("reseposy0",pos,10.*alg->resey[0],1.);  
  clu=(Cluster*)ev->Cls->At(tra->amas_entree[1][0]);
  if (clu->npiste == 1) {
    Fill1D("resy01p",10.*alg->resy[0],1.);    
    Fill1D("resey01p",10.*alg->resey[0],1.);    
    amas_1p_y++;
  }

  Fill1D("resy1",10.*alg->resy[1],1.);     
  Fill1D("resey1",10.*alg->resey[1],1.);     
  /*  piste = clu->GetSeedAdd();
  cog = clu->GetCoG();
  ppiste = alg->ipred(1,1,tra);
  printf("y1 piste %f cog %f ppiste %f\n",piste,cog,ppiste); */
  Fill2D("resetray1",((float)alg->ntrace),10.*alg->resey[1],1.);    
  pos = fmod(alg->ipred(1,1,tra),1.);
  Fill2D("reseposy1",pos,10.*alg->resey[1],1.);  
  clu=(Cluster*)ev->Cls->At(tra->amas_entree[1][1]);
  if (clu->npiste == 1) {
    Fill1D("resy11p",10.*alg->resy[1],1.);    
    Fill1D("resey11p",10.*alg->resey[1],1.);    
    amas_1p_y++;
  }

  Fill1D("resy2",10.*alg->resy[2],1.);    
  Fill1D("resey2",10.*alg->resey[2],1.);    
  /*  piste = clu->GetSeedAdd();
  cog = clu->GetCoG();
  ppiste = alg->ipred(2,1,tra);
  printf("y2 piste %f cog %f ppiste %f\n",piste,cog,ppiste); */
  Fill2D("resetray2",((float)alg->ntrace),10.*alg->resey[2],1.);    
  pos = fmod(alg->ipred(2,1,tra),1.);
  Fill2D("reseposy2",pos,10.*alg->resey[2],1.);  
  clu=(Cluster*)ev->Cls->At(tra->amas_entree[1][2]);
  if (clu->npiste == 1) {
    Fill1D("resy21p",10.*alg->resy[2],1.);    
    Fill1D("resey21p",10.*alg->resey[2],1.);    
    amas_1p_y++;
  }

  Fill1D("resy3",10.*alg->resy[3],1.);    
  Fill1D("resey3",10.*alg->resey[3],1.);    
  /*  piste = clu->GetSeedAdd();
  cog = clu->GetCoG();
  ppiste = alg->ipred(3,1,tra);
  printf("y3 piste %f cog %f ppiste %f\n\n",piste,cog,ppiste); */
  Fill2D("resetray3",((float)alg->ntrace),10.*alg->resey[3],1.);    
  pos = fmod(alg->ipred(3,1,tra),1.);
  Fill2D("reseposy3",pos,10.*alg->resey[3],1.);  
  clu=(Cluster*)ev->Cls->At(tra->amas_entree[1][3]);
  if (clu->npiste == 1) {
    Fill1D("resy31p",10.*alg->resy[3],1.);    
    Fill1D("resey31p",10.*alg->resey[3],1.);    
    amas_1p_y++;
  }

  Fill1D("resy4",10.*alg->resy[4],1.);    
  Fill1D("resey4",10.*alg->resey[4],1.);    
  /*  piste = clu->GetSeedAdd();
  cog = clu->GetCoG();
  ppiste = alg->ipred(4,1,tra);
  printf("y4 piste %f cog %f ppiste %f\n\n",piste,cog,ppiste); */
  Fill2D("resetray4",((float)alg->ntrace),10.*alg->resey[4],1.);    
  pos = fmod(alg->ipred(4,1,tra),1.);
  Fill2D("reseposy4",pos,10.*alg->resey[4],1.);  
  clu=(Cluster*)ev->Cls->At(tra->amas_entree[1][4]);
  if (clu->npiste == 1) {
    Fill1D("resy41p",10.*alg->resy[4],1.);    
    Fill1D("resey41p",10.*alg->resey[4],1.);    
    amas_1p_y++;
  }

  Fill1D("resy5",10.*alg->resy[5],1.);    
  Fill1D("resey5",10.*alg->resey[5],1.);    
  /*  piste = clu->GetSeedAdd();
  cog = clu->GetCoG();
  ppiste = alg->ipred(5,1,tra);
  printf("y5 piste %f cog %f ppiste %f\n\n",piste,cog,ppiste); */
  Fill2D("resetray5",((float)alg->ntrace),10.*alg->resey[3],1.);    
  pos = fmod(alg->ipred(5,1,tra),1.);
  Fill2D("reseposy5",pos,10.*alg->resey[5],1.);  
  clu=(Cluster*)ev->Cls->At(tra->amas_entree[1][5]);
  if (clu->npiste == 1) {
    Fill1D("resy51p",10.*alg->resy[5],1.);    
    Fill1D("resey51p",10.*alg->resey[5],1.);    
    amas_1p_y++;
  }

  Fill1D("tra_1px",((float) alg->ntrace),amas_1p_x);
  Fill1D("tra_1py",((float) alg->ntrace),amas_1p_y);
  Fill1D("tra_m1px",((float) alg->ntrace),amas_1p_x);
  Fill1D("tra_m1py",((float) alg->ntrace),amas_1p_y);
  Fill1D("no_1px",amas_1p_x,1.);
  Fill1D("no_1py",amas_1p_y,1.);

  //  printf("retour residus\n");
  return;
}

void DHF::residus_petits(Event* ev, Align* alg) {
   
  Trace* tra=(Trace*)ev->Tra->At(0);

  // en mm

  Fill1D("resx4b",10.*alg->resx_pos4[0],1.);    
  Fill1D("resex4b",10.*alg->resx_pos4[1],1.);    
  float pos = fmod(alg->ipred(4,0,tra),1.);
  Fill2D("reseposx4b",pos,10.*alg->resx_pos4[1],1.);  

  Fill1D("resy4b",10.*alg->resy_pos4[0],1.);    
  Fill1D("resey4b",10.*alg->resy_pos4[1],1.);    
  pos = fmod(alg->ipred(4,1,tra),1.);
  Fill2D("reseposy4b",pos,10.*alg->resy_pos4[1],1.);  

  Fill1D("resx5b",10.*alg->resx_pos0[0],1.);    
  Fill1D("resex5b",10.*alg->resx_pos0[1],1.);    
  pos = fmod(alg->ipred(5,0,tra),1.);
  Fill2D("reseposx5b",pos,10.*alg->resx_pos0[1],1.);  

  Fill1D("resy5b",10.*alg->resy_pos0[0],1.);    
  Fill1D("resey5b",10.*alg->resy_pos0[1],1.);    
  pos = fmod(alg->ipred(5,1,tra),1.);
  Fill2D("reseposy5b",pos,10.*alg->resy_pos0[1],1.);  

  return;
}

void DHF::residus_echelles(Event* ev, Align* alg) {
   
  // en mm

  Fill1D("res33x1",10.*alg->resxech[1],1.);    
  Fill1D("res23x1",10.*alg->resexech[1],1.);    

  Fill1D("res33y1",10.*alg->resyech[1],1.);    
  Fill1D("res23y1",10.*alg->reseyech[1],1.);    

  Fill1D("res33x2",10.*alg->resxech[3],1.);    
  Fill1D("res23x2",10.*alg->resexech[3],1.);    

  Fill1D("res33y2",10.*alg->resyech[3],1.);    
  Fill1D("res23y2",10.*alg->reseyech[3],1.);    

  Fill1D("res33x3",10.*alg->resxech[0],1.);    
  Fill1D("res23x3",10.*alg->resexech[0],1.);    

  Fill1D("res33y3",10.*alg->resyech[0],1.);    
  Fill1D("res23y3",10.*alg->reseyech[0],1.);    

  return;
}

void DHF::residus_eff(Event* ev, Align* alg, int fin) {
   
  char chist[20];

  if (fin) { 
     TH1D* temp[6];
     temp[0] = (TH1D *) fHlist->FindObject("xeff");
     temp[1] = (TH1D *) fHlist->FindObject("yeff");
     temp[2] = (TH1D *) fHlist->FindObject("xineff");
     temp[3] = (TH1D *) fHlist->FindObject("yineff");
     temp[4] = (TH1D *) fHlist->FindObject("xbruit");
     temp[5] = (TH1D *) fHlist->FindObject("ybruit");
     float bcont[6][6];
     for (int ib=0; ib<6; ib++) 
       for(int icote=0; icote<2; icote++) {
         float fscale = alg->ntrace_eff[icote][ib];
         if (fscale != 0.) {
           fscale = 1./fscale;
           bcont[icote][ib] = temp[icote]->GetBinContent(ib+1);
           float bval = fscale*bcont[icote][ib];
           temp[icote]->SetBinContent(ib+1,bval);
           if (bcont[icote][ib] != 0.) {
	     float beval = bval*sqrt(fscale + 1./bcont[icote][ib]);
             temp[icote]->SetBinError(ib+1,beval);
           }
           bcont[icote+2][ib] = temp[icote+2]->GetBinContent(ib+1);
           bval = fscale*bcont[icote+2][ib];
           temp[icote+2]->SetBinContent(ib+1,bval);
           if (bcont[icote][ib+2] != 0.) {
             float beval = bval*sqrt(fscale + 1./bcont[icote+2][ib]);
             temp[icote+2]->SetBinError(ib+1,beval);
           }
           bcont[icote+4][ib] = temp[icote+4]->GetBinContent(ib+1);
           bval = fscale*bcont[icote+4][ib];
           temp[icote+4]->SetBinContent(ib+1,bval);
           if (bcont[icote][ib+4] != 0.) {
             float beval = bval*sqrt(fscale + 1./bcont[icote+4][ib]);
             temp[icote+4]->SetBinError(ib+1,beval);
           }
         }
     }
     for (int ib=0; ib<6; ib++) {
       printf("-> trace x %f y %f\n",alg->ntrace_eff[0][ib],alg->ntrace_eff[1][ib]);
       printf("   ech %d xeff %f xineff %f xbruit %f yeff %f yineff %f ybruit %f\n",ib,
	      bcont[0][ib],bcont[2][ib],bcont[4][ib],bcont[1][ib],bcont[3][ib],bcont[5][ib]);
     }
     return;
  }

  Trace* tra=(Trace*)ev->Tra->At(0);
  // histos pour les residus en mm

  for (int i=0; i<6; i++) {
    if (alg->reseffx[i] != 999.) {
      sprintf(chist,"reseffx%d",i);
      Fill1D(chist,10.*alg->reseffx[i],1.);
      if (fabs(alg->reseffx[i]) <= 0.03000)
        Fill1D("xeff",((float) i),1.);
      else {
      //      printf("i %d reseffx %f\n",i,alg->reseffx[i]);
        if (alg->reseffx[i] != -999.) Fill1D("xbruit",((float) i),1.);
        else Fill1D("xineff",((float) i),1.);
      } 
    }
  }
  for (int i=0; i<6; i++) {
    if (alg->reseffy[i] != 999.) {
      sprintf(chist,"reseffy%d",i);
      Fill1D(chist,10.*alg->reseffy[i],1.);
      if (fabs(alg->reseffy[i]) <= 0.03000)
        Fill1D("yeff",((float) i),1.);
      else {
      //      printf("i %d reseffy %f\n",i,alg->reseffy[i]);
        if (alg->reseffy[i] != -999.) Fill1D("ybruit",((float) i),1.);
        else Fill1D("yineff",((float) i),1.);
      }
    } 
  }

  return;
}

void DHF::residus_scan_va(Event* ev, Align* alg, Calib* cal) {
   
  char chist[20];

  Cluster* clu;
  Trace* tra=(Trace*)ev->Tra->At(0);
  // histos pour les residus en mm

  for (int i=0; i<4; i++) {
    sprintf(chist,"ressx%d",i);
    Fill1D(chist,10.*alg->ressx[i],1.);
    clu =(Cluster*)ev->Cls->At(tra->amas_entree[0][i]);
    //    for (int ic=0; ic<1024; ic++) printf("i %d ic %d statut %d\n",i,ic,cal->ret_statut(i,ic));
    if (cal->ret_statut(i,clu->GetSeedAdd()) != 0) {
     sprintf(chist,"ressmx%d",i);
     printf("%s ic %d statut %d\n",chist,clu->GetSeedAdd(),cal->ret_statut(i,clu->GetSeedAdd()));
     Fill1D(chist,10.*alg->ressx[i],1.);
    }
    double xpred = alg->ligne_projx(5,4,i,tra);
    sprintf(chist,"resesx%d",i);
    Fill1D(chist,10.*(tra->x[i]-xpred),1.);
    if (cal->ret_statut(i,clu->GetSeedAdd()) != 0) {
     sprintf(chist,"resesmx%d",i);
     Fill1D(chist,10.*(tra->x[i]-xpred),1.);
    }
    sprintf(chist,"ressy%d",i);
    Fill1D(chist,10.*alg->ressy[i],1.);
    clu =(Cluster*)ev->Cls->At(tra->amas_entree[1][i]);
    if (cal->ret_statut(i,clu->GetSeedAdd()) != 0) {
     sprintf(chist,"ressmy%d",i);
     Fill1D(chist,10.*alg->ressy[i],1.);
    }
    double ypred = alg->ligne_projy(5,4,i,tra);
    sprintf(chist,"resesy%d",i);
    Fill1D(chist,10.*(tra->y[i]-ypred),1.);
    if (cal->ret_statut(i,clu->GetSeedAdd()) != 0) {
     sprintf(chist,"resesmy%d",i);
     Fill1D(chist,10.*(tra->y[i]-ypred),1.);
    }
    sprintf(chist,"resesposy%d",i);
    float pos = fmod(alg->ipred(i,1,tra),1.);
    Fill2D(chist,pos,10.*(tra->y[i]-ypred),1.);  
    if (i == 3) {
      sprintf(chist,"resesy%dpi",i);
      Fill2D(chist,((float) clu->GetCoG()),10.*(tra->y[i]-ypred),1.);
      Fill2D("dyy_pos4",tra->y[i],(tra->y[i]-ypred),1.);
      Fill2D("dyx_pos4",tra->x[i],(tra->y[i]-ypred),1.);
    } 
    if (clu->npiste == 1) {
      sprintf(chist,"ressy%d1p",i);
      Fill1D(chist,10.*alg->ressy[i],1.);    
      sprintf(chist,"resesy%d1p",i);
      Fill1D(chist,10.*(tra->y[i]-ypred),1.);    
    }
    if (runno == 1237 && i ==3) {
      if (tra->yp[3] < 642. || tra->yp[3] > 1021.) {
       sprintf(chist,"ressyb%d",i);
       Fill1D(chist,10.*alg->ressy[i],1.);    
       sprintf(chist,"resesyb%d",i);
       Fill1D(chist,10.*(tra->y[i]-ypred),1.);    
      }
    }
  }

  return;
}

int DHF::divergences(Event* ev, Align* alg) {
   
  Trace* tra=(Trace*)ev->Tra->At(0);
  int comb = 0;
  int bon = 1;
  int iz1, iz2;
  float dxdz, dydz;
  for (int i1=0; i1<5; i1++)
    for (int i2=i1+1; i2<6; i2++) {
      iz1 = alg->ztdr[i1];
      iz2 = alg->ztdr[i2];
      dxdz = alg->div_dxdz(iz1,iz2,tra);
      if (fabs(dxdz) > 0.0006) bon=0;
      dydz = alg->div_dydz(iz1,iz2,tra);
      if (fabs(dydz) > 0.0008) bon=0;
      //      printf("comb %d i1 %d i2 %d iz1 %d iz2 %d\n",comb,i1,i2,iz1,iz2);
      //      printf("dxdz %f dydz %f\n",dxdz,dydz);
      //      printf("----\n");
      //      printf("diver comb %d iz1 %d iz2 %d\n",comb,iz1,iz2);
      Fill2D("dxdz",((float) comb),dxdz,1.);
      Fill2D("dydz",((float) comb),dydz,1.);
      if (comb == 5) {
        Fill2D("dydz_y1",tra->y[1],dydz,1.);
        Fill2D("dydz_y3",tra->y[3],dydz,1.);
        Fill2D("y1_y3",tra->y[3],tra->y[1],1.);
      }
      comb++;
  };
 
  return(bon);
}

void DHF::hcalib(Calib* cal, RHClass* rh) {

  for (int ii=0; ii<rh->ntdrCmp; ii++) {
    for (int jj=0; jj<1024; jj++) {
         FillCalib(rh->tdrCmpMap[ii],"Piedestaux",((float)jj),cal->ret_piedestal(ii,jj));
         FillCalib(rh->tdrCmpMap[ii],"Sigma",((float)jj),cal->ret_sigma(ii,jj));
         FillCalib(rh->tdrCmpMap[ii],"Statut",((float)jj),((float) cal->ret_statut(ii,jj)));
    }
 }
  return;
}
