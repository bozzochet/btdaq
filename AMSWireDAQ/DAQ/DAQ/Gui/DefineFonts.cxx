#include "DefineFonts.h"

////////////////////////////////////////////////////////////////////////////
// DefineFonts
// Define the different fonts used in the class
////////////////////////////////////////////////////////////////////////////

//FontStruct_t DefineFonts::ffont[10];
//GCValues_t   DefineFonts::ffontval[10];  
//GContext_t   DefineFonts::ffontcon[10];
TGFont*      DefineFonts::fFont[10];

DefineFonts::DefineFonts(){
//   ffont[0]=gClient->GetFontByName("-*-times-bold-r-*-*-24-*-*-*-*-*-*-*");
//   //  ffont[0]=gClient->GetFontByName("-*-arial-bold-r-*-*-24-*-*-*-*-*-*-*");


//   ffontval[0].fMask = kGCForeground | kGCFont;
//   ffontval[0].fFont = gVirtualX->GetFontHandle(ffont[0]);
//   gClient->GetColorByName("red", ffontval[0].fForeground);
//   ffontcon[0] = gVirtualX->CreateGC(gClient->GetRoot()->GetId(),&ffontval[0]);

//   ffont[1]=gClient->GetFontByName("-*-times-bold-r-*-*-18-*-*-*-*-*-*-*");
//   //ffont[1]=gClient->GetFontByName("-*-arial-bold-r-*-*-18-*-*-*-*-*-*-*");


//   ffontval[1].fMask = kGCForeground | kGCFont;
//   ffontval[1].fFont = gVirtualX->GetFontHandle(ffont[1]);
//   gClient->GetColorByName("blue", ffontval[1].fForeground);
//   ffontcon[1] = gVirtualX->CreateGC(gClient->GetRoot()->GetId(),&ffontval[1]);

//   ffont[2]=gClient->GetFontByName("-*-times-bold-r-*-*-14-*-*-*-*-*-*-*");
//   //ffont[2]=gClient->GetFontByName("-*-arial-bold-r-*-*-14-*-*-*-*-*-*-*");


//   ffontval[2].fMask = kGCForeground | kGCFont;
//   ffontval[2].fFont = gVirtualX->GetFontHandle(ffont[2]);
//   gClient->GetColorByName("black", ffontval[2].fForeground);
//   ffontcon[2] = gVirtualX->CreateGC(gClient->GetRoot()->GetId(),&ffontval[2]);

//   ffont[3]=gClient->GetFontByName("-*-times-bold-r-*-*-14-*-*-*-*-*-*-*");
//   //ffont[3]=gClient->GetFontByName("-*-arial-bold-r-*-*-14-*-*-*-*-*-*-*");


//   ffontval[3].fMask = kGCForeground | kGCFont;
//   ffontval[3].fFont = gVirtualX->GetFontHandle(ffont[3]);
//   gClient->GetColorByName("red", ffontval[3].fForeground);
//   ffontcon[3] = gVirtualX->CreateGC(gClient->GetRoot()->GetId(),&ffontval[3]);

  fFont[0]=gClient->GetFont("-*-times-bold-r-*-*-24-*-*-*-*-*-*-*");
  fFont[1]=gClient->GetFont("-*-times-bold-r-*-*-18-*-*-*-*-*-*-*");
  fFont[2]=gClient->GetFont("-*-times-bold-r-*-*-14-*-*-*-*-*-*-*");
  fFont[3]=gClient->GetFont("-*-times-bold-r-*-*-14-*-*-*-*-*-*-*");

  //cout << "DefineFonts: " << fFont[0] << " " << fFont[1] << " " << fFont[2] << " " << fFont[3] <<endl;
  
}


DefineFonts::~DefineFonts(){
//   for (int i=0;i<10;i++)
//     gVirtualX->DeleteFont(ffont[i]);
  
//   for (int i=0;i<10;i++)
//     gVirtualX->DeleteGC(ffontcon[i]);
}

