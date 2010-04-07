#include "CalSlider.h"
#include "CalPars.h"
#include "TROOT.h"

#include "TH2F.h"

#define MAXB1 -1
#define MAXB2 -2

ClassImp(CalSlider);

char * GetOctName(int oct);
char* CalSlider::filename=0;
int CalSlider::Opened=0;

CalSlider::CalSlider(char *name,char *title,int xsize,int ysize)
		:SliderI(name,title,xsize,ysize){

	Ped=0; Sig=0; SigR=0; SigU=0; SigRU=0; SigL=0; SigRL=0; SigR2=0; 
	SigU2=0; SigRU2=0; SigL2=0; SigRL2=0; ST0=0; ST1=0; ST2=0; ST3=0; 
	ST4=0; NG=0;
	rootfile = NULL;
	histfile = NULL;
	label = NULL;
	graphtype = 1;

//	if(!Opened) OpenFile();
//	Draw(0);
//	Update();
//	Print(filename,"ps");
  
	/* DB init */
	TkDBc::CreateTkDBc();
	TkDBc::Head->init();
	gROOT->SetStyle("Plain");
}

int CalSlider::GetTkId(){
	if(!lad) return 0;
	return lad->GetTkId();
}

int CalSlider::GetHwId(){
	if(!lad) return 0;
	return lad->GetHwId();
}

void CalSlider::selectGraph(int id){
	graphtype = id;
}

int CalSlider::try2Draw(TrLadCal *cc){
	if(cc && cc->IsFilled()){
		cal = cc;
		lad = TkDBc::Head->FindHwId(cal->HwId);
		Draw();
		/* update my pointer now...*/
		for(calit_cur=calit_begin;calit_cur->second!=cc;calit_cur++);
		return 0;
	}else{
		TCanvas::Clear();
		if(!label)
			label = new TPaveLabel(0.3,0.35,0.7,0.65,"Not found");
		label->Draw();
    		TCanvas::Update();
	}
	return 1;
}

void CalSlider::setRootFile(char *filename){
	rootfile = new TFile(filename);
	caldb = (TrCalDB*) rootfile->Get("TrCalDB");
	calit_begin = caldb->trcal_hwidmap.begin();
	calit_end = caldb->trcal_hwidmap.end();
	calit_cur = calit_begin;
	if(calit_cur->second->IsFilled())
		try2Draw(calit_cur->second);
	else
		showNext();
}

void CalSlider::setHistFile(char *filename){
	histfile = new TFile(filename);
}

void CalSlider::showNext(){
	if(rootfile == NULL) return;
	trcalIT calit_save = calit_cur;
	do{
		calit_cur++;
		if(calit_cur == calit_end) calit_cur = calit_begin;
	}while(!(calit_cur->second->IsFilled()) || (calit_cur == calit_save));
	try2Draw(calit_cur->second);
}

void CalSlider::showPrev(){
	if(rootfile == NULL) return;
	trcalIT calit_save = calit_cur;
	do{
		if(calit_cur == calit_begin) calit_cur = calit_end;
		calit_cur--;
	}while(!(calit_cur->second->IsFilled()) || (calit_cur == calit_save));
	try2Draw(calit_cur->second);
}

int CalSlider::showTkId(int tkid){
	if(rootfile == NULL) return 2;
	return try2Draw(caldb->FindCal_TkId(tkid));
}

int CalSlider::showHwId(int hwid){
	if(rootfile == NULL) return 2;
	return try2Draw(caldb->FindCal_HwId(hwid));
}

void CalSlider::PrintThis(){
	char filename[20];
	/* Cal_TkId+210.ps */
	sprintf(filename,"Cal_TkId%+d.ps",lad->GetTkId());
	TCanvas::Print(filename,"ps");
}

void CalSlider::PrintAll(){
	TCanvas::Print("Cal_All.ps[","ps");
	for(calit_cur=calit_begin;calit_cur!=calit_end;calit_cur++){
		if(!calit_cur->second->IsFilled()) continue;
		try2Draw(calit_cur->second);
		TCanvas::Print("Cal_All.ps","ps");
	}
	TCanvas::Print("Cal_All.ps]","ps");
}



void CalSlider::CloseFile(){
//  char ntemp[90];
//  TCanvas* temp=new TCanvas("temp","temp");
//  sprintf(ntemp,"%s]",filename);
//  temp->Print(ntemp,"ps");
//  Opened=0;
//  delete temp;
}

void CalSlider::OpenFile(){
//  char ntemp[90];
//  sprintf(ntemp,"%s[",filename);
//  Print(ntemp,"ps");
//  Opened=1;
}

// CalSlider::CalSlider(CalSlider& orig):TCanvas(orig){
  
//   Ped=orig.Ped  ;
//   Sig=orig.Sig  ;
//   SigR=orig.SigR  ;
//   SigU=orig.SigU  ;
//   SigRU=orig.SigRU  ;
//   SigL=orig.SigL  ;
//   SigRL=orig.SigRL  ;


//   SigR2=orig.SigR2  ;
//   SigU2=orig.SigU2  ;
//   SigRU2=orig.SigRU2  ;
//   SigL2=orig.SigL2  ;
//   SigRL2=orig.SigRL2  ;

//   cal=orig.cal  ;
//   ST0=orig.ST0;
//   ST1=orig.ST1;
//   ST2=orig.ST2;
//   ST3=orig.ST3;
//   ST4=orig.ST4;
//   NG=orig.NG;
//   cal=orig.cal;
// }

CalSlider::~CalSlider(){
 if( Ped) delete  Ped;
 if( Sig) delete  Sig;
 if( SigR) delete  SigR;
 if( SigU) delete  SigU;
 if( SigRU) delete SigRU ;
 if( SigL) delete  SigL;
 if( SigRL) delete SigRL  ;


 if( SigR2) delete  SigR2;
 if( SigU2) delete  SigU2;
 if( SigRU2) delete SigRU2 ;
 if( SigL2) delete  SigL2;
 if( SigRL2) delete SigRL2  ;


 if( ST0) delete ST0 ;
 if( ST1) delete ST1 ;
 if( ST2) delete ST2 ;
 if( ST3) delete ST3 ;
 if( ST4) delete ST4 ;
 if(NG)   delete NG;
}

void CalSlider::Clear(const Option_t* aa){
  Clear(aa);
  if( Ped) delete  Ped;
  if( Sig) delete  Sig;
  if( SigR) delete  SigR;
  if( SigU) delete  SigU;
  if( SigRU) delete SigRU ;
  if( SigL) delete  SigL;
  if( SigRL) delete SigRL  ;

  if( SigR2) delete  SigR2;
  if( SigU2) delete  SigU2;
  if( SigRU2) delete SigRU2 ;
  if( SigL2) delete  SigL2;
  if( SigRL2) delete SigRL2  ;


 if( ST0) delete ST0 ;
 if( ST1) delete ST1 ;
 if( ST2) delete ST2 ;
 if( ST3) delete ST3 ;
 if( ST4) delete ST4 ;
 if( NG) delete NG ;
}

void CalSlider::Draw(const Option_t *aa,int flag){
	switch(graphtype){
		case 1:
			Draw1(aa,flag);
			break;
		case 2:
			Draw2(aa,flag);
			break;
		case 3:
			Draw3(aa,flag);
			break;
		case 4:
			Draw4(aa,flag);
			break;
		case 5:
			Draw5(aa,flag);
			break;
	}
}

void CalSlider::Draw5(const Option_t* aa,int flag){
	printf("Draw5() called\n");
}

void CalSlider::Draw4(const Option_t* aa,int flag){
	printf("Draw4() called\n");
}

void CalSlider::Draw3(const Option_t* aa,int flag){
	printf("Draw3() called\n");
}

void CalSlider::Draw2(const Option_t* aa,int flag){
  printf("Draw2() called  %d \n", GetHwId());
  Color_t col[5]={kGreen,kRed,kGreen,kBlue,kBlack};
  
  char namein[90];
  
  
  sprintf(namein,"Ladder %s",lad->name);
  TCanvas::Clear();
  TCanvas::Divide(2,2,0.001,0.001);
  
  char dname[13][20]={"Pedestal","SigmaRaw","Sigma","SigmaRawU","SigmaU","SigmaRawL","SigmaL",
		      "ST0","ST1","ST2","ST3","ST4","Not Gaussian"};
  sprintf(namein,"Ladder %s  Oct: %s Crate: %d TDR: %d Layer: %d Slot: %d Side: %d ",
	  lad->name,GetOctName(lad->GetOctant()),lad->GetCrate(),lad->GetTdr(),lad->GetLayer(),lad->GetSlot(),lad->GetSide());

  if(Ped==0)  Ped  = new TH1F(dname[0],namein,1024,0,1024); else Ped->SetTitle(namein);
  if(SigR==0) SigR = new TH1F(dname[1],dname[1],1024,0,1024);
  if(Sig==0 ) Sig  = new TH1F(dname[2],dname[2],1024,0,1024);
  
  if(SigRU==0)   SigRU    = new TH1F(dname[3],dname[3],1024,0,1024);
  if(SigU==0 ) SigU   = new TH1F(dname[4],dname[4],1024,0,1024);
  
  if(SigRL==0)   SigRL    = new TH1F(dname[5],dname[5],1024,0,1024);
  if(SigL==0 ) SigL   = new TH1F(dname[6],dname[6],1024,0,1024);
  


  if(ST0==0 ) ST0   = new TH1F(dname[7],dname[7],1024,0,1024);
  if(ST1==0 ) ST1   = new TH1F(dname[8],dname[8],1024,0,1024);
  if(ST2==0 ) ST2   = new TH1F(dname[9],dname[9],1024,0,1024);
  if(ST3==0 ) ST3   = new TH1F(dname[10],dname[10],1024,0,1024);
  if(ST4==0 ) ST4   = new TH1F(dname[11],dname[11],1024,0,1024);
  if(NG==0 ) NG     = new TH1F(dname[12],dname[12],1024,0,1024);

  if(SigRU2==0)   SigRU2    = new TH1F(dname[13],dname[13],1024,0,1024);
  if(SigU2==0 )   SigU2   = new TH1F(dname[14],dname[14],1024,0,1024);

  if(SigRL2==0)   SigRL2    = new TH1F(dname[15],dname[15],1024,0,1024);
  if(SigL2==0 )   SigL2   = new TH1F(dname[16],dname[16],1024,0,1024);


  
  for (int ii=0;ii<1024;ii++){
    Ped->SetBinContent(ii,cal->Pedestal(ii));
    Sig->SetBinContent(ii,cal->Sigma(ii));
    SigR->SetBinContent(ii,cal->SigmaRaw(ii));
    //    NG->SetBinContent(ii,cal->NG[ii]/cal->NumNG*1.);
    if(TESTBIT(cal->Status(ii),0)==0) ST0->SetBinContent(ii,0); else ST0->SetBinContent(ii,MAXB2);
    if(TESTBIT(cal->Status(ii),1)==0) ST1->SetBinContent(ii,0); else ST1->SetBinContent(ii,MAXB1);
    if(TESTBIT(cal->Status(ii),2)==0) ST2->SetBinContent(ii,0); else ST2->SetBinContent(ii,MAXB2);
    if(TESTBIT(cal->Status(ii),3)==0) ST3->SetBinContent(ii,0); else ST3->SetBinContent(ii,MAXB1);
    if(TESTBIT(cal->Status(ii),4)==0) ST4->SetBinContent(ii,0); else ST4->SetBinContent(ii,MAXB2);
  }
  
  TVirtualPad*cc0=TCanvas::cd(1);  
  SetHistStyle(Ped);
  cc0->SetGrid(1,0);
  Ped->SetTitleSize(0.1);
  Ped->Draw();

  cc0=TCanvas::cd(4);  
  SetHistStyle(Sig);
  cc0->SetGrid(1,0);
  Sig->SetFillColor(col[4]);
  Sig->SetFillStyle(3004);
  Sig->GetYaxis()->SetRangeUser(-2,10);
  Sig->Draw();
  SigU->SetLineColor(col[1]);
  SigL->SetLineColor(col[1]);
  SigU->Draw("same");
  SigL->Draw("same");
  ST2->SetFillColor(col[2]);
  ST2->SetFillStyle(3004);
  ST2->SetLineColor(col[2]);
  ST2->Draw("same");
  ST3->SetFillColor(col[3]);
  ST3->SetFillStyle(3005);
  ST3->SetLineColor(col[3]);
  ST3->Draw("same");
  if(flag>1){
    SigU2->SetLineColor(col[0]);
    SigL2->SetLineColor(col[0]);
    SigU2->Draw("same");
    SigL2->Draw("same");
  }


  cc0=TCanvas::cd(2);  
  SetHistStyle(SigR);
  cc0->SetGrid(1,0);
  SigR->SetFillColor(col[4]);
  SigR->SetFillStyle(3004);
  if(flag!=4)SigR->GetYaxis()->SetRangeUser(-2,18);
  SigR->Draw();
  SigRU->SetLineColor(col[1]);
  SigRL->SetLineColor(col[1]);
  SigRU->Draw("same");
  SigRL->Draw("same");
  ST0->SetFillColor(col[2]);
  ST0->SetFillStyle(3004);
  ST0->SetLineColor(col[2]);
  ST0->Draw("same");
  ST1->SetFillColor(col[3]);
  ST1->SetFillStyle(3005);
  ST1->SetLineColor(col[3]);
  ST1->Draw("same");
  if(flag>1){
    SigRU2->SetLineColor(col[0]);
    SigRL2->SetLineColor(col[0]);
    SigRU2->Draw("same");
    SigRL2->Draw("same");
  }


    cc0=TCanvas::cd(3);  
    //  sprintf(ntemp,"%s.ps",cal->LadName);

    cc0->SetLogy();

  int crate=GetHwId()/100;
  int lad= GetHwId()%100;
  int nn=crate*24+lad;
  char name[200];
  sprintf(name,"Sizes/Siz_lad%03dVsDeltaT",nn+1);
  if(histfile) printf("Rootfile %d\n",rootfile);
  TH2F* hh =(TH2F*)histfile->Get(name);
  if(hh)  hh->Draw();
  else printf("hh is zero %d %s\n",hh,name);
  
    TCanvas::Update();

}

void CalSlider::Draw1(const Option_t* aa,int flag){
	TCanvas::Draw(aa);
  Color_t col[5]={kGreen,kRed,kGreen,kBlue,kBlack};
  
  char namein[90];

  
  sprintf(namein,"Ladder %s",lad->name);
  TCanvas::Clear();
  TCanvas::Divide(2,2,0.001,0.001);
  
  char dname[13][20]={"Pedestal","SigmaRaw","Sigma","SigmaRawU","SigmaU","SigmaRawL","SigmaL",
		      "ST0","ST1","ST2","ST3","ST4","Not Gaussian"};
  sprintf(namein,"Ladder %s  Oct: %s Crate: %d TDR: %d Layer: %d Slot: %d Side: %d ",
	  lad->name,GetOctName(lad->GetOctant()),lad->GetCrate(),lad->GetTdr(),lad->GetLayer(),lad->GetSlot(),lad->GetSide());

  if(Ped==0)  Ped  = new TH1F(dname[0],namein,1024,0,1024); else Ped->SetTitle(namein);
  if(SigR==0) SigR = new TH1F(dname[1],dname[1],1024,0,1024);
  if(Sig==0 ) Sig  = new TH1F(dname[2],dname[2],1024,0,1024);

  if(SigRU==0)   SigRU    = new TH1F(dname[3],dname[3],1024,0,1024);
  if(SigU==0 ) SigU   = new TH1F(dname[4],dname[4],1024,0,1024);

  if(SigRL==0)   SigRL    = new TH1F(dname[5],dname[5],1024,0,1024);
  if(SigL==0 ) SigL   = new TH1F(dname[6],dname[6],1024,0,1024);



  if(ST0==0 ) ST0   = new TH1F(dname[7],dname[7],1024,0,1024);
  if(ST1==0 ) ST1   = new TH1F(dname[8],dname[8],1024,0,1024);
  if(ST2==0 ) ST2   = new TH1F(dname[9],dname[9],1024,0,1024);
  if(ST3==0 ) ST3   = new TH1F(dname[10],dname[10],1024,0,1024);
  if(ST4==0 ) ST4   = new TH1F(dname[11],dname[11],1024,0,1024);
  if(NG==0 ) NG     = new TH1F(dname[12],dname[12],1024,0,1024);

  if(SigRU2==0)   SigRU2    = new TH1F(dname[13],dname[13],1024,0,1024);
  if(SigU2==0 )   SigU2   = new TH1F(dname[14],dname[14],1024,0,1024);

  if(SigRL2==0)   SigRL2    = new TH1F(dname[15],dname[15],1024,0,1024);
  if(SigL2==0 )   SigL2   = new TH1F(dname[16],dname[16],1024,0,1024);


  
  for (int ii=0;ii<1024;ii++){
    Ped->SetBinContent(ii,cal->Pedestal(ii));
    Sig->SetBinContent(ii,cal->Sigma(ii));
    SigR->SetBinContent(ii,cal->SigmaRaw(ii));
    //    NG->SetBinContent(ii,cal->NG[ii]/cal->NumNG*1.);
    if(TESTBIT(cal->Status(ii),0)==0) ST0->SetBinContent(ii,0); else ST0->SetBinContent(ii,MAXB2);
    if(TESTBIT(cal->Status(ii),1)==0) ST1->SetBinContent(ii,0); else ST1->SetBinContent(ii,MAXB1);
    if(TESTBIT(cal->Status(ii),2)==0) ST2->SetBinContent(ii,0); else ST2->SetBinContent(ii,MAXB2);
    if(TESTBIT(cal->Status(ii),3)==0) ST3->SetBinContent(ii,0); else ST3->SetBinContent(ii,MAXB1);
    if(TESTBIT(cal->Status(ii),4)==0) ST4->SetBinContent(ii,0); else ST4->SetBinContent(ii,MAXB2);
  }
  
  // for (int jj=0;jj<16;jj++)
//     for (int ii=0;ii<64;ii++){
//       SigRU->SetBinContent(ii+jj*64,cal->SRMean[jj][0]+CalPars::high_raw_noise*cal->SRMean[jj][1]);
//       SigRL->SetBinContent(ii+jj*64,cal->SRMean[jj][0]-CalPars::low_raw_noise*cal->SRMean[jj][1]);
//     }

  

//   for (int jj=0;jj<2;jj++)
//     for (int ii=0;ii<320;ii++){
//       SigU->SetBinContent(ii+jj*320,cal->SMean[jj][0]+CalPars::high_noise*cal->SMean[jj][1]);
//       SigL->SetBinContent(ii+jj*320,cal->SMean[jj][0]-CalPars::low_noise*cal->SMean[jj][1]);
//     }

//   for (int ii=640;ii<1024;ii++){
//     SigU->SetBinContent(ii,cal->SMean[2][0]+CalPars::high_noise*cal->SMean[2][1]);
//     SigL->SetBinContent(ii,cal->SMean[2][0]-CalPars::low_noise*cal->SMean[2][1]);
//   }

  if(flag>1){
    for (int jj=0;jj<16;jj++){
      float mean=0;
      for (int ii=0;ii<64;ii++)
	mean+=cal->SigmaRaw(ii+64*jj)/64.;
      for (int ii=0;ii<64;ii++){
	SigRU2->SetBinContent(ii+jj*64,mean*1.5);
	SigRL2->SetBinContent(ii+jj*64,mean/4.);
	// 	SigRU2->SetBinContent(ii+jj*64,cal->SRMean[jj][0]*1.5);
	// 	SigRL2->SetBinContent(ii+jj*64,cal->SRMean[jj][0]/4.);
      }
    

      mean=0;
      int count=0;
      for (int ii=0;ii<64;ii++){
	if(TESTBIT(cal->Status(ii+64*jj),0)==0&&TESTBIT(cal->Status(ii+64*jj),1)==0){
	  mean+=cal->Sigma(ii+64*jj); count++;
	}
      }
      mean/=count;
      for (int ii=0;ii<64;ii++){
	SigU2->SetBinContent(ii+jj*64,mean*1.5);
	SigL2->SetBinContent(ii+jj*64,mean/4.);
// 	SigU2->SetBinContent(ii+jj*64,cal->SMean[jj][0]*1.5);
// 	SigL2->SetBinContent(ii+jj*64,cal->SMean[jj][0]/4.);
      }
    
    }
  }

  TVirtualPad*cc0=TCanvas::cd(1);  
  SetHistStyle(Ped);
  cc0->SetGrid(1,0);
  Ped->SetTitleSize(0.1);
  Ped->Draw();

  cc0=TCanvas::cd(4);  
  SetHistStyle(Sig);
  cc0->SetGrid(1,0);
  Sig->SetFillColor(col[4]);
  Sig->SetFillStyle(3004);
  Sig->GetYaxis()->SetRangeUser(-2,10);
  Sig->Draw();
  SigU->SetLineColor(col[1]);
  SigL->SetLineColor(col[1]);
  SigU->Draw("same");
  SigL->Draw("same");
  ST2->SetFillColor(col[2]);
  ST2->SetFillStyle(3004);
  ST2->SetLineColor(col[2]);
  ST2->Draw("same");
  ST3->SetFillColor(col[3]);
  ST3->SetFillStyle(3005);
  ST3->SetLineColor(col[3]);
  ST3->Draw("same");
  if(flag>1){
    SigU2->SetLineColor(col[0]);
    SigL2->SetLineColor(col[0]);
    SigU2->Draw("same");
    SigL2->Draw("same");
  }


  cc0=TCanvas::cd(2);  
  SetHistStyle(SigR);
  cc0->SetGrid(1,0);
  SigR->SetFillColor(col[4]);
  SigR->SetFillStyle(3004);
  if(flag!=4)SigR->GetYaxis()->SetRangeUser(-2,18);
  SigR->Draw();
  SigRU->SetLineColor(col[1]);
  SigRL->SetLineColor(col[1]);
  SigRU->Draw("same");
  SigRL->Draw("same");
  ST0->SetFillColor(col[2]);
  ST0->SetFillStyle(3004);
  ST0->SetLineColor(col[2]);
  ST0->Draw("same");
  ST1->SetFillColor(col[3]);
  ST1->SetFillStyle(3005);
  ST1->SetLineColor(col[3]);
  ST1->Draw("same");
  if(flag>1){
    SigRU2->SetLineColor(col[0]);
    SigRL2->SetLineColor(col[0]);
    SigRU2->Draw("same");
    SigRL2->Draw("same");
  }


    cc0=TCanvas::cd(3);  
    SetHistStyle(Sig);
    cc0->SetGrid(1,0);
    Sig->SetFillColor(col[4]);
    Sig->SetFillStyle(3004);
    Sig->GetYaxis()->SetRangeUser(-2,10);
    Sig->Draw();
    ST4->SetFillColor(col[1]);
    ST4->SetFillStyle(3005);
    ST4->SetLineColor(col[1]);
    //    ST4->Scale(1./cal->NumNG);
    ST4->Draw("same");
    ST3->Draw("same");
    TCanvas::Update();
    //  sprintf(ntemp,"%s.ps",cal->LadName);
}


void CalSlider::SetHistStyle(TH1* hh){
  
  hh->GetXaxis()->SetLabelSize(.05);
  hh->GetYaxis()->SetLabelSize(.05);
  hh->GetXaxis()->SetNdivisions(516,0);
  hh->GetYaxis()->SetTitleOffset(0.5);
  hh->GetYaxis()->SetTitleSize(0.06);
  hh->GetXaxis()->SetTitleSize(0.05);
  hh->SetLineStyle(1); 
  hh->SetFillStyle(0);
}

char * GetOctName(int oct){
  static char name[10];
  sprintf(name," ");
  if(oct==1) sprintf(name,"WAKE++");
  if(oct==1) sprintf(name,"WAKE-+");
  if(oct==3) sprintf(name,"RAM ++");
  if(oct==4) sprintf(name,"RAM -+");
  if(oct==5) sprintf(name,"WAKE+-");
  if(oct==6) sprintf(name,"WAKE--");
  if(oct==7) sprintf(name,"RAM +-");
  if(oct==8) sprintf(name,"RAM --");
  return name;
}

