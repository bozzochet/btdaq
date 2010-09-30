#include "DefineLadder.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>


////////////////////////////////////////////////////////////////////////////
// DefineLadder
// Define a Ladder. Intermediate layer between the graphical and the control
// parts of the TDR program
////////////////////////////////////////////////////////////////////////////

char DefineLadder::fName[9];    
char DefineLadder::fVoltage;   
char DefineLadder::fWaffers[3]; 
char DefineLadder::fAssembly;  
char DefineLadder::fLayer;     
char DefineLadder::fSerial[4];  

DefineLadder::DefineLadder(){
  fVoltage='H';
  strcpy(fWaffers,"12");
  fAssembly='G';
  fLayer='I';
  strcpy(fSerial,"001");
  sprintf(fName,"%c%s%c%c%s",fVoltage,fWaffers,fAssembly,fLayer,fSerial);
}

DefineLadder::~DefineLadder(){}

char *DefineLadder::GetLadderName(){
  sprintf(fName,"%c%s%c%c%s",fVoltage,fWaffers,fAssembly,fLayer,fSerial);
  return fName;
}




