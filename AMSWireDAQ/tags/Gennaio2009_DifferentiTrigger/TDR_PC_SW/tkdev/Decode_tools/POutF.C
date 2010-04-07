#include "POutF.h"
POutF* POutF::pp=0;

POutF::POutF(){
  ss.clear();
  ss.reserve(1000);
  pp=this;
}

void POutF:: IOutput ( char * fmt, ... )
{
  char msg[1024];
  va_list ap;
  va_start( ap, fmt );
  vsnprintf( msg, sizeof(msg), fmt, ap );
  va_end( ap );
  ss.append(msg);
  return;
}
