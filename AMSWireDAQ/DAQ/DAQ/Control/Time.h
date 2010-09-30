#ifndef TIME_H
#define TIME_H

#include <time.h>
#include <sys/timeb.h>

class Time {
 public:
  Time();
  ~Time();
  
  void Reset();
  void Start();
  void RefLive();
  void RefPause();
  void RefDead();
  void UpdateElapsed();
  void UpdateLive();
  void UpdatePause();
  void UpdateDead();
  void UpdatePrevious();
  void ToOld();
  int   GetElapsed();
  float GetEventRate(int NEvents);
  float GetEventInstantRate(int NEvents);
  float GetLiveTime();
  float GetPauseTime();
  float GetDeadTime();
  float GetInsLiveTime();

 private:
  struct timeb fStart;
  struct timeb fNow;
  struct timeb fPrevious;
  struct timeb fRefLive;
  struct timeb fRefDead;
  struct timeb fRefPause;
  int fElapsedTime;
  int fLiveTime;
  int fPauseTime;
  int fDeadTime;
  int fLiveTimeOld;
  int fPauseTimeOld;
  int fDeadTimeOld;
  int fSincePreviousTime;
};

#endif /*TIME_H*/




