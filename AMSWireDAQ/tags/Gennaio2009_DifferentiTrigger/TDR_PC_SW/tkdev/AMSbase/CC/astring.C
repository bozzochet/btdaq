//  $Id: astring.C,v 1.3 2008/11/05 17:39:05 zuccon Exp $
#include "astring.h"



AString::Data::Data(){
pString = new char[1];
pString[0]='\0';
RefC=1;
}


AString::Data::Data(const char * a){
  RefC=0;
  if(a){
   pString=new char[strlen(a)+1];
   if(pString){
    strcpy(pString,a);
    RefC=1;
   }
  }
   else{
    cerr <<" AString::Data::Data-E-ZeroInput " <<endl;
    pString = new char[1];
    pString[0]='\0';
    RefC=1;
   }
}

AString::Data::Data( char  a, int n){
  RefC=0;
   if(n<0)n=0;
   pString=new char[n+1];
   if(pString){
   pString[0]=a;
   pString[1]='\0';
   RefC=1;
  }
}

AString::Data::Data(const AString::Data & a){
  RefC=0;
  if(a.RefC){
    pString=new char[strlen(a.pString)+1];
    if(pString){
     strcpy(pString,a.pString);
     RefC=1;
    }
  }
}

void AString::_mutator(){
  if (pData->RefC > 1 ){
    Data * d = new Data(*pData);
    --pData->RefC;
    pData=d;
  }
}



AString::Data * AString::defaultData(){
  static Data * p=NULL;
    if(p==NULL){
     p=new Data();
    }
  ++p->RefC;
  return p;
}


AString::Data::~Data(){
delete[] pString;
pString=0;
}


AString::~AString(){
if(-- pData->RefC ==0)delete pData;
}


AString & AString::operator = (const AString & s){

++ s.pData->RefC;
if( -- pData->RefC ==0)delete pData;
pData=s.pData;
return *this;
} 

AString & AString::operator = (const char * s){

if( -- pData->RefC ==0)delete pData;
pData=new Data(s);
return *this;
} 


AString AString::operator+(const char * s){
Data *d;
if(s){
    d=new Data('\0',strlen(pData->pString)+strlen(s));
    strcpy(d->pString,pData->pString);
    strcat(d->pString,s);
}
else{
    d=new Data('\0',strlen(pData->pString));
    strcpy(d->pString,pData->pString);
}
    return AString(d);
}



AString AString::operator+(const AString & a){
const char * s= (const char *)a;
return operator+(s);


}




AString  operator +(const char * a, const AString & s ){
return  s+a;
}




AString & AString::operator +=(const char  * s){
  if(s){
    Data * d=new Data('\0',strlen(pData->pString)+strlen(s));
    strcpy(d->pString,pData->pString);
    strcat(d->pString,s);
    if( -- pData->RefC ==0)delete pData;
    pData=d;    
  }
  return *this;
}                 
 




AString & AString::operator +=(const AString & a){
  const char * s= (const char *)a;
  return  (*this)+=s;           
 
}

AString AString::operator ()(int i) {
 return AString((pData->pString)+i);
}


