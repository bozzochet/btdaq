#include "TrackerLadders.h"


TkLadder::TkLadder() {
  fX=0;
  fLayer=0;
  fSlot=0;
  sprintf(fName,"empty");
  sprintf(fNameLow,"empty");
  fCrate=0;
  fPwg=0;
  fPwgpos=0;
  fTdr=0;
  fLc80=0;
  fLc90=0;
}


TkLadder::TkLadder(int x,int layer,int slot, char *name,int crate,int pwg,int pwgpos,int tdr,double lc80,double lc90) {

  fX=x;
  fLayer=layer;
  fSlot=slot;
  sprintf(fName,"%s",name);
  fCrate=crate;
  fPwg=pwg;
  fPwgpos=pwgpos;
  fTdr=tdr;
  fLc80=lc80;
  fLc90=lc90;

  char lowercase[20];

  sprintf(lowercase,"%s",fName);
  for (int c=0; c<strlen(lowercase); c++) lowercase[c]=tolower(lowercase[c]);
  sprintf(fNameLow,"%s",lowercase);


}


TkLadder::TkLadder(const TkLadder& ladder) {

  fX=ladder.fX;
  fLayer=ladder.fLayer;
  fSlot=ladder.fSlot;
  sprintf(fName,"%s",ladder.fName);
  sprintf(fNameLow,"%s",ladder.fNameLow);
  fCrate=ladder.fCrate;
  fPwg=ladder.fPwg;
  fPwgpos=ladder.fPwgpos;
  fTdr=ladder.fTdr;
  fLc80=ladder.fLc80;
  fLc90=ladder.fLc90;

}


// void Ladder::SetLadder(trackerlad ladder) {

//   fX=ladder.x;
//   fLayer=ladder.layer;
//   fSlot=ladder.slot;
//   sprintf(fName,"%s",ladder.name);
//   fXtant=ladder.xtant;
//   fPwg=ladder.pwg;
//   fPwgpos=ladder.pwgpos;
//   fTdr=ladder.tdr;
//   fLc80=ladder.lc80;
//   fLc90=ladder.lc90;

// }



void TkLadder::SetLadder(int x,int layer,int slot, char *name,int crate,int pwg,int pwgpos,int tdr,double lc80,double lc90) {

  fX=x;
  fLayer=layer;
  fSlot=slot;
  sprintf(fName,"%s",name);
  fCrate=crate;
  fPwg=pwg;
  fPwgpos=pwgpos;
  fTdr=tdr;
  fLc80=lc80;
  fLc90=lc90;

  //printf("welcome to setladder: name=%s, crate=%d, tdr=%d, pwg=%d, pwgpos=%d\n", fName, fCrate, fTdr, fPwg, fPwgpos);

  char lowercase[20];
 
  sprintf(lowercase,"%s",fName);
  for (int c=0; c<strlen(lowercase); c++) lowercase[c]=tolower(lowercase[c]);
  sprintf(fNameLow,"%s",lowercase);

}


void TkLadder::SetLadder(TkLadder ladder) {

  fX=ladder.GetX();
  fLayer=ladder.GetLayer();
  fSlot=ladder.GetSlot();
  sprintf(fName,"%s",ladder.GetName());
  sprintf(fNameLow,"%s",ladder.GetLowercaseName());
  fCrate=ladder.GetCrate();
  fPwg=ladder.GetPwg();
  fPwgpos=ladder.GetPwgpos();
  fTdr=ladder.GetTdr();
  fLc80=ladder.GetLc80();
  fLc90=ladder.GetLc90();

  //printf("welcome do the 2nd setladder: name=%s, lc80=%lf\n", fName, fLc80);

}


TrackerLadders::TrackerLadders() {

  fNlads=0;

  for (int lad=0; lad<MAXLAD+1; lad++) fLadder[lad]=new TkLadder();
}


TrackerLadders::~TrackerLadders() {
  for (int lad=0; lad<MAXLAD+1; lad++) {
    //printf("%d, %s\n",lad, fLadder[lad]->GetName());
    delete fLadder[lad];
  }
  //printf("delete finished\n");
}


void TrackerLadders::SetTrackerLadder(int lad, TkLadder ladder) {

  if ((lad<0) || (lad>MAXLAD-1)) return;
  //printf("Setting ladder %d\n", lad);
  fLadder[lad]->SetLadder(ladder);
  //printf("ladder name is now %s\n", fLadder[lad]->GetName());
}


void TrackerLadders::ReadFile(const char *filename) {

  FILE *f=fopen(filename,"r");

  if (f==0) return;

  TkLadder aladder;

  char *dum[200];

  for (int i=0; i<10; i++) fscanf(f,"%s",dum); // skipping the first line with column names
  
  int lad=0;
  do {
    char pos[10], name[20], name2[20];
    int x,layer,slot,crate,pwg,pwgpos,tdr;
    double lc80,lc90;
    fscanf(f,"%s %d %d %s %d %d %d %d %lf %lf",&pos,&layer,&slot,&name,&crate,&pwg,&pwgpos,&tdr,&lc80,&lc90);

    if (feof(f)) break;

    if ((strstr(name,"one"))) continue; // we skip the "None" line

    //printf("%s\n", name);
    char *temp=strrchr(name,'\"');
    *temp='\0';

    x=(strcmp("\"M\"",pos))?1:-1;
    sprintf(name2,"%s",&name[1]);

    aladder.SetLadder(x,layer,slot,name2,crate,pwg-1,pwgpos-1,tdr,lc80,lc90);

    SetTrackerLadder(lad, aladder);

    lad++;

  } while(1);

  fNlads=lad;

  //printf("%d ladders found\n", lad);

//   for (int i=0; i<lad; i++) {
//     Ladder *aladder=GetLadder(i);
   
//     if (aladder==0) printf("sorry, ladder=0\n");
//     printf("%d %d %d %s %d %d %d %d %lf %lf\n", aladder->GetX(), aladder->GetLayer(), aladder->GetSlot(), aladder->GetName(), aladder->GetCrate(), aladder->GetPwg(), aladder->GetPwgpos(), aladder->GetTdr(), aladder->GetLc80(), aladder->GetLc90());


//   }


}


int TrackerLadders::FindLadderIndex(int crate, int pwg, int pwgpos) {

  for (int lad=0; lad<MAXLAD; lad++) {
    if ((fLadder[lad]->GetCrate()==crate) && (fLadder[lad]->GetPwg()==pwg) && (fLadder[lad]->GetPwgpos()==pwgpos)) return lad;
  }

  return MAXLAD;
}


TkLadder* TrackerLadders::FindLadder(int crate, int pwg, int pwgpos) {

  int lad=FindLadderIndex(crate,pwg,pwgpos);

  return fLadder[lad];

}


int TrackerLadders::FindLadderIndex(char *name) {

  char uppercase[20];
  for (int i=0; i<strlen(name); i++) {
    if (i>19) break;
    uppercase[i]=toupper(name[i]);
  }

  for (int lad=0; lad<MAXLAD; lad++) {
    if (!strcmp(fLadder[lad]->GetName(),uppercase)) return lad;
  }

  return MAXLAD;
  
}


TkLadder* TrackerLadders::FindLadder(char *name) {

  int lad=FindLadderIndex(name);
  
  return fLadder[lad];

}


int TrackerLadders::FindLadderIndex(int crate, int tdr) {

  for (int lad=0; lad<MAXLAD; lad++) {
    if ((fLadder[lad]->GetCrate()==crate) && (fLadder[lad]->GetTdr()==tdr)) return lad;
  }

  return MAXLAD;
}


TkLadder* TrackerLadders::FindLadder(int crate, int tdr) {

  int lad=FindLadderIndex(crate,tdr);
  
  return fLadder[lad];

}


 int TrackerLadders::CheckTdrAssignment(int lad) {

   const int conversion[4][6]= {
     { 0, 1, 4, 5, 8, 9}, // 1st power group
     {12,13,16,17,20,21}, // 2nd power group
     {22,23,18,19,14,15}, // 3rd power group
     {10,11, 6, 7, 2, 3}  // 4th power group
   };

   int pwg=fLadder[lad]->GetPwg();
   int pwgpos=fLadder[lad]->GetPwgpos();
   int tdr=fLadder[lad]->GetTdr();

   return (conversion[pwg][pwgpos]==tdr);

 }
