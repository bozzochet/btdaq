#include "Time.h"

#include "iostream"
#include <stdio.h>

Time::Time(){
  //  Reset();
}

Time::~Time(){}

void Time::Reset(){
  fStart.time    = 0;
  fStart.millitm = 0;
  fNow.time      = 0;
  fNow.millitm   = 0;
  fPrevious.time = 0;
  fPrevious.millitm = 0;

  fRefLive.time     = 0;
  fRefLive.millitm  = 0;
  fRefDead.time     = 0;
  fRefDead.millitm  = 0;
  fRefPause.time    = 0;
  fRefPause.millitm = 0;

  fElapsedTime  = 0;
  fLiveTime     = 0;
  fPauseTime    = 0;
  fDeadTime     = 0;
  fLiveTimeOld  = 0;
  fPauseTimeOld = 0;
  fDeadTimeOld  = 0;

  fSincePreviousTime=0;
}

void Time::Start(){
  Reset();
  ftime( &fStart );
  
}


void Time::RefLive(){
  ftime( &fRefLive );
}

void Time::RefPause(){
  ftime( &fRefPause );
}

void Time::RefDead(){
  ftime( &fRefDead );
}

void Time::UpdatePrevious(){
  ftime( &fNow );
  fSincePreviousTime = (int) (difftime(fNow.time,fPrevious.time)) * 1000 +
                 (int) (fNow.millitm - fPrevious.millitm);

  fPrevious.time=fNow.time;
  fPrevious.millitm=fNow.millitm;
}

void Time::UpdateElapsed(){
  ftime( &fNow );
  fElapsedTime = (int) (difftime(fNow.time,fStart.time)) * 1000 +
                 (int) (fNow.millitm - fStart.millitm);
}

void Time::UpdateLive(){
  ftime( &fNow );
  fLiveTime   += (int) (difftime(fNow.time,fRefLive.time)) * 1000 +
                 (int) (fNow.millitm - fRefLive.millitm);
  ftime(&fRefLive);
}

void Time::UpdateDead(){
  ftime( &fNow );
  fDeadTime   += (int) (difftime(fNow.time,fRefDead.time)) * 1000 +
                 (int) (fNow.millitm - fRefDead.millitm);
  ftime(&fRefDead);
}

void Time::UpdatePause(){
  ftime( &fNow );
  fPauseTime  += (int) (difftime(fNow.time,fRefPause.time)) * 1000 +
                 (int) (fNow.millitm - fRefPause.millitm);
  ftime(&fRefPause);
}


int Time::GetElapsed(){
  ftime( &fNow );
  fElapsedTime = (int) (difftime(fNow.time,fStart.time)) * 1000 +
                 (int) (fNow.millitm - fStart.millitm);
  return fElapsedTime;
}

float Time::GetEventRate(int NEvents){
  float Rate = 0.0;
  UpdateElapsed();  
  if (fElapsedTime != 0) Rate=(float)NEvents/(float)fElapsedTime*1000.0;
  return Rate;
}

float Time::GetEventInstantRate(int NEvents){
  float Rate = 0.0;
  UpdatePrevious();  
  if (fSincePreviousTime != 0) Rate=(float)NEvents/(float)fSincePreviousTime*1000.0;
  return Rate;
}

float Time::GetLiveTime(){
  float LiveTime = 0.0;
  UpdateElapsed();
  if (fElapsedTime != 0) LiveTime=(float)fLiveTime/(float)fElapsedTime*100.0;
  //  cerr << "Live Time: " << fLiveTime << " " << fElapsedTime << " " << LiveTime << endl;
  //printf("\nElapsedTime: %12d\n",fElapsedTime);
  //printf("  aLiveTime: %12d\n",fLiveTime);
  //printf("   DeadTime: %12d\n",fDeadTime);
  //printf("  PauseTime: %12d\n",fPauseTime);
  //printf("   LiveTime: %12.3f\n",LiveTime);
  return LiveTime;

}

float Time::GetPauseTime(){
  float PauseTime = 0.0;
  UpdateElapsed();
  if (fElapsedTime != 0) PauseTime=(float)fPauseTime/(float)fElapsedTime;
  return PauseTime;
}

float Time::GetDeadTime(){
  float DeadTime = 0.0;
  UpdateElapsed();
  if (fElapsedTime != 0) DeadTime=(float)fDeadTime/(float)fElapsedTime;
  return DeadTime;
}


float Time::GetInsLiveTime(){
  float InsLiveTime = 0;
  float LiveTime  = (float) (fLiveTime  - fLiveTimeOld);
  float DeadTime  = (float) (fDeadTime  - fDeadTimeOld);
  float PauseTime = (float) (fPauseTime - fPauseTimeOld);
  float TotalTime = LiveTime+DeadTime+PauseTime;
  if (TotalTime > 0) InsLiveTime = LiveTime / TotalTime;
  ToOld();
//    printf("\n   LiveTime: %12.3f\n",LiveTime);
//    printf("   DeadTime: %12.3f\n",DeadTime);
//    printf("  PauseTime: %12.3f\n",PauseTime);
//    printf("InsLiveTime: %12.3f\n",InsLiveTime);
  return InsLiveTime;
}

void Time::ToOld(){
  fLiveTimeOld  = fLiveTime;
  fDeadTimeOld  = fDeadTime;
  fPauseTimeOld = fPauseTime;
}
