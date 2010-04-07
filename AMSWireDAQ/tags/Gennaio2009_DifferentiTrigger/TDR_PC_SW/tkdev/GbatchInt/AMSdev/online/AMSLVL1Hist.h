//  $Id: AMSLVL1Hist.h,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#ifndef __AMSLVL1HIST__
#define __AMSLVL1HIST__
#include "AMSHist.h"

const float lev1trange[3]={120,60,360};//mins,hours,days

class Lvl1Pars{
  private:
    static char dat1[30];
    static char dat2[30];
    static char dat3[30];
  public:
    inline static void setdat1(char *d){strcpy(dat1,d);}
    inline static void setdat2(char *d){strcpy(dat2,d);}
    inline static void setdat3(char *d){strcpy(dat3,d);}
    inline static char* getdat1(){return dat1;}
    inline static char* getdat2(){return dat2;}
    inline static char* getdat3(){return dat3;}
};

class AMSLVL1Hist : public AMSHist{
public:
AMSLVL1Hist(Text_t * name, Text_t * title, Int_t active=1):AMSHist(name,title,active){};
void Book();
void ShowSet(int );
void Fill(AMSNtupleR *ntuple);
};




#endif
