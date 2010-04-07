//  $Id: AMSR_ColorManager.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_ColorManager                                                      //
//                                                                      //
// class to manage colors used by AMSR_Root event display program.        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TVirtualX.h>
#ifndef ROOT_TCanvas_H
#include <TCanvas.h>
#endif
#ifndef ROOT_TPad_H
#include <TButton.h>
#endif
//#include "AMSR_Root.h"
//#include "AMSR_Maker.h"
#include "AMSR_ColorManager.h"


ClassImp(AMSR_ColorManager)



//_____________________________________________________________________________
AMSR_ColorManager::AMSR_ColorManager(char * name, char * title) 
                : TNamed(name, title)
{
  //
  // keep my personal backup of the color table
  //
  for (Int_t i=0; i<MaxColor; i++) {
//    gVirtualX->GetRGB(i, colorTable[i].r, colorTable[i].g, colorTable[i].b);
//    printf("color #%3d, r = %f, g = %f, b = %f\n", i, colorTable[i].r, colorTable[i].g, colorTable[i].b);
    gVirtualX->GetRGB(i, red[i], green[i], blue[i]);
    //printf("color #%3d, r = %f, g = %f, b = %f\n", i, red[i], green[i], blue[i]);
  }

}

//_____________________________________________________________________________
Int_t AMSR_ColorManager::GetColorAt(Int_t i, Float_t *r, Float_t *g, Float_t *b)
{
  //
  // returns the components of color # i
  //
  if ( i<0 || i>= MaxColor) return -1;
  *r = red[i];
  *g = green[i];
  *b = blue[i];
  return 0;
}


//_____________________________________________________________________________
Bool_t AMSR_ColorManager::IsValid(Int_t i)
{
  //
  // check if color #i is still valid
  //

  Float_t r, g, b;
  gVirtualX->GetRGB(i, r, g, b);
  if ( r != red[i]  ||
       g != green[i]  ||
       b != blue[i] )    return kFALSE;
  //if ( r != colorTable[i].r  ||
  //     g != colorTable[i].g  ||
  //     b != colorTable[i].b )    return kFALSE;

  return kTRUE;
  
}


//_____________________________________________________________________________
Int_t AMSR_ColorManager::Migrate(Int_t i, Int_t j)
{
  //
  // Migrate color # i to slot # j in the system color table
  //
  if ( i>=0 && i < MaxColor && j >= 0  && j < MaxColor ) {
    //gVirtualX->SetRGB(j, colorTable[i].r, colorTable[i].g, colorTable[i].b);
    gVirtualX->SetRGB(j, red[i], green[i], blue[i]);
    return 0;
  }
  else {
    return -1;
  }
}



//_____________________________________________________________________________
void AMSR_ColorManager::ColorTest(Int_t begin, Int_t end)
{
  TCanvas * c1 = new TCanvas("ColorTest", "Color Test");
  c1->cd();

  //printf("will do color test from color # %d to color # %d\n", begin, end);

  Int_t nCol = 16;
  Int_t nRow = 16;
  Float_t width  = 0.90/nCol;
  Float_t height = 0.90/nRow;
//  printf("%d columns %d rows, width = %f, height = %f\n", nCol, nRow, width, height);
  TButton * pad[MaxColor];
  Int_t c=0;
  char name[5];  name[4] = 0;
  for (Int_t i=0; i<nRow; i++) {
    //printf("======= %f - %f\n", float(i)*height, float(i+1)*height);
    for (Int_t j=0; j<nCol; j++) {
      sprintf(name, "c%03d", c);
      pad[c] = new TButton(name, "", float(j)*width,   float(i)*height, 
				     float(j+1)*width, float(i+1)*height);
      if (pad[c]) {
        pad[c]->SetFillColor(c);
        pad[c]->Draw();
      } else {
	printf("can't have a pad at color #%d.\n", c);
      }
      c++;
    }
  }

  c1->Paint();
//  char ch = fgetc(stdin);
//  for (c=0; c<MaxColor; c++) delete pad[c];
//  delete c1;
}

