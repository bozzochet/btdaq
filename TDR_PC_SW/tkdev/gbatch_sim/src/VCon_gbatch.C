#include "VCon_gbatch.h"

#include "AMSEvent.h"

VCon_gb::VCon_gb(AMSContainer * cc):VCon(){
  con=cc;
}

VCon* VCon_gb::GetCont(char * name){
  AMSContainer *cc=AMSEvent::gethead()->getC(AMSID(name));
  if (cc)
    return (VCon*)(new VCon_gb(cc));
  else return 0;

}

int VCon_gb::getnelem(){
  if(!con) return -1;
  else return con->getnelem();
}

void VCon_gb::eraseC(){
  if(!con) return ;
  else return con->eraseC();
}

AMSlink* VCon_gb::getelem(int ii){
  if(!con) return 0;
  AMSlink* primo=con->gethead();
  for (int jj=0;jj<ii;jj++)
    primo=primo->next();
  return primo;
}

void  VCon_gb::addnext(AMSlink* aa){
  
  if(!con) return;
  con->addnext(aa);
}

int  VCon_gb::getindex(AMSlink* aa){
  if(!con) return 0;
  AMSlink* primo=con->gethead();
  for (int jj=0;jj<con->getnelem();jj++){
    if(primo==aa) return jj;
    primo=primo->next();
  }
  return -1;
}
