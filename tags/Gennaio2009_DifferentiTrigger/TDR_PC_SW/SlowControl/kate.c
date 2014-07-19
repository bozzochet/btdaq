// file kate.c
//
// to solve some Kate's problems
//
// A.Lebedev Sep 2006 - ...
//

#include "mylib.h"

float result[4][5] = {
  {37.7, 32.9, 33.8, 37.0, 35.4},
  {43.4, 33.1, 42.2, 34.7, 41.8},
  {33.3, 28.5, 38.9, 30.4, 33.6},
  {29.2, 26.4, 29.6, 28.5, 31.1}};

int main (void) {

  int k1, k2, k3, k4;
  float time;
  struct {
    int   runner[4];
    float time;
  } best;

  best.time = 100000.0;

  for (k1=0; k1<5; k1++) {
    for (k2=0; k2<5; k2++) {
      if (k2 == k1) continue;
      for (k3=0; k3<5; k3++) {
        if (k3 == k1 || k3 == k2) continue;
        for (k4=0; k4<5; k4++) {
          if (k4 == k1 || k4 == k2 || k4 == k3) continue;
          time = result[0][k1] +
                 result[1][k2] +
                 result[2][k3] +
                 result[3][k4];
          if (time < best.time) {
            best.time = time;
            best.runner[0] = k1;
            best.runner[1] = k2;
            best.runner[2] = k3;
            best.runner[3] = k4;
          }
        }
      }
    }
  }
  printf("Best time = %4.1f, Runners = %d %d %d %d\n", 
          best.time, best.runner[0], best.runner[1], best.runner[2], best.runner[3]);
}
