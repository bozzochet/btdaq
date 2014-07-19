
#ifndef _TrkElosPDF_h
#define _TrkElosPDF_h
#include "typedefs.h"
#include "tkdbc.h"
//------------------------------------------------------------
// class to store Eloss PDF's (vs Z) made from Eloss-distr. files 
class TrkElosPDF{
  //TRK-eloss prob.density function for particular charge 
  private:
    int ichar;//0,1,...index of particle(e,p,he,..)
    int charge;//charge(1,1,2,...)
    int nbins[trconst::TrkTypes];//distribution_length
    geant stpx[trconst::TrkTypes];//bin width(MeV)
    geant xmin[trconst::TrkTypes];//1st bin low edge(MeV)
    geant slope[trconst::TrkTypes];//exp.slope to calc. pdf in ovfl-region
    geant norm[trconst::TrkTypes];//norm.factor=dx*Ntot/Npeak 
    geant elpdf[trconst::TrkTypes][trconst::TrkPdfBins];//PDF-array 
  public:
    static TrkElosPDF TrkEPDFs[AMSChargConst::MaxZTypes];
    TrkElosPDF(){};
    TrkElosPDF(int ich, int charge, int nb[], geant stp[],
	geant norm[], geant slop[], geant distr[trconst::TrkTypes][trconst::TrkPdfBins]);
    int getnbins(int itp){return nbins[itp];}
    int getcharge(){return charge;}
    geant getstep(int itp){return stpx[itp];}
    number getlkhd(int nhits, int htype[], number ehit[], number beta);
    static void build();  
}; 

#endif
