#include "GonPos.hh"


void GonPos::Print(){
  printf("Alpha: %11.2f  Craddle: %11.2f   Lin1: %8.4f  Lin2: %8.4f\n",
	 alpha,craddle,lin1,lin2);
  return;
}
void GonPos::PrintGrad(){
  printf("Alpha: %f (mgrad) Craddle: %f (mgrad)   Lin1: %8.4f  Lin2: %8.4f \n",
	 1E3*alpha*180./PI,1E3*craddle*180./PI,lin1,lin2);
  return;

}

