// Define fonts
#ifndef FILE_DEFINEFONTS_SEEN
#define FILE_DEFINEFONTS_SEEN

#include <TGFrame.h>
#include <TGFont.h>
#include <iostream>
using namespace std;

class DefineFonts{

 protected:
  //  static FontStruct_t ffont[10];
  //  static GCValues_t   ffontval[10];  
  //  static GContext_t   ffontcon[10];

 

 private:

 public:
static TGFont *fFont[10]; 
 DefineFonts();
  ~DefineFonts();
};

#endif

