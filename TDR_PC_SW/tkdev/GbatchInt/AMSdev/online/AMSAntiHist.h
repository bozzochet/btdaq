//  $Id: AMSAntiHist.h,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#ifndef __AMSANTIHIST__
#define __AMSANTIHIST__
#include "AMSHist.h"

#include "AMSHist.h"

const Int_t ANTISECMX=8;

class AntiPars{
  private:
    static char dat1[30];
    static char dat2[30];
    static char dat3[30];
    static Int_t evcount[20];
  public:
    inline static void setdat1(char *d){strcpy(dat1,d);}
    inline static void setdat2(char *d){strcpy(dat2,d);}
    inline static void setdat3(char *d){strcpy(dat3,d);}
    inline static char* getdat1(){return dat1;}
    inline static char* getdat2(){return dat2;}
    inline static char* getdat3(){return dat3;}
    inline static void addstat(Int_t k){evcount[k]+=1;}
    inline static Int_t getstat(Int_t k){return evcount[k];}
    inline static void clearstat(){for(int i=0;i<20;i++)evcount[i]=0;} 
};


class AMSAntiHist : public AMSHist{
public:
AMSAntiHist(Text_t * name, Text_t * title, Int_t active=1):AMSHist(name,title,active){};
void Book();
void ShowSet(int );
void Fill(AMSNtupleR *ntuple);
};


#endif
