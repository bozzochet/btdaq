// file test_RS.c
//
// Example program to use Reed-Solomon encoding (for Prof. Yuan-Hann Chang)
//
// A.Lebedev - May 2006...
//

/******************************************************************************

this is log showing how it should be compiled and run...

1. on ams.cern.ch (digital unix):

A.Lebedev @ ams <- gcc -o test_RS test_RS.c
A.Lebedev @ ams <- ./test_RS
No errors found
 1 errors found and corrected...checked: no differences found
 2 errors found and corrected...checked: no differences found
 3 errors found and corrected...checked: no differences found
 4 errors found and corrected...checked: no differences found
 5 errors found and corrected...checked: no differences found
 6 errors found and corrected...checked: no differences found
 7 errors found and corrected...checked: no differences found
 8 errors found and corrected...checked: no differences found
 9 errors found and corrected...checked: no differences found
10 errors found and corrected...checked: no differences found
11 errors found and corrected...checked: no differences found
12 errors found and corrected...checked: no differences found
13 errors found and corrected...checked: no differences found
14 errors found and corrected...checked: no differences found
15 errors found and corrected...checked: no differences found
16 errors found and corrected...checked: no differences found
Too many errors, correction is not possible.
A.Lebedev @ ams <-

2. on linux Fedora Core 1 computer:

[lebedev@pcaal05 RS]$ gcc -o test_RS test_RS.c
[lebedev@pcaal05 RS]$ ./test_RS
No errors found
 1 errors found and corrected...checked: no differences found
 2 errors found and corrected...checked: no differences found
 3 errors found and corrected...checked: no differences found
 4 errors found and corrected...checked: no differences found
 5 errors found and corrected...checked: no differences found
 6 errors found and corrected...checked: no differences found
 7 errors found and corrected...checked: no differences found
 8 errors found and corrected...checked: no differences found
 9 errors found and corrected...checked: no differences found
10 errors found and corrected...checked: no differences found
11 errors found and corrected...checked: no differences found
12 errors found and corrected...checked: no differences found
13 errors found and corrected...checked: no differences found
14 errors found and corrected...checked: no differences found
15 errors found and corrected...checked: no differences found
16 errors found and corrected...checked: no differences found
Too many errors, correction is not possible.
[lebedev@pcaal05 RS]$

******************************************************************************/

//~----------------------------------------------------------------------------

#include "stdio.h"
#include "stdlib.h"

#define j0 1
int M = 8;
int K;
int N = 255;
unsigned char Alpha_to[256];
unsigned char Index_of[256];
unsigned char Index_of_0;
unsigned char G[256];
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MOD(a)   ((a) < N  ? (a) : (a)-N)

//~----------------------------------------------------------------------------

void initialize_RS( const int k) {

  int i, j, P = 0;

  K = k;
  Index_of_0 = N;

  if (M == 2) P = 0007;
  if (M == 3) P = 0013;
  if (M == 4) P = 0023;
  if (M == 5) P = 0045;
  if (M == 6) P = 0103;
  if (M == 7) P = 0211;
  if (M == 8) P = 0435;

  Alpha_to[0] = 1;
  for (i=1; i<N+1; i++) {
    Alpha_to[i] = Alpha_to[i-1] << 1;
    if ((Alpha_to[i-1] & (1 << (M - 1))) != 0) {
      Alpha_to[i] ^= P;
    }
    Index_of[Alpha_to[i]] = i;
  }
  Alpha_to[N] = 0;

  Index_of[0] = Index_of_0;
  Index_of[Alpha_to[0]] = 0;

  G[0] = Alpha_to[j0];
  G[1] = 1;
  for (i=1; i<=N-K-1; i++) {
    G[i+1] = G[i];
    for (j=i; j>0; j--) {
      if (G[j] != 0) {
        G[j] = Alpha_to[MOD((Index_of[G[j]]) + j0 + i)];
      }
      G[j] = G[j] ^ G[j - 1];
    }
    G[0] = Alpha_to[MOD((Index_of[G[0]]) + j0 + i)];
  }
  for (i=0; i<=N-K; i++) G[i] = Index_of[G[i]];
}

//~----------------------------------------------------------------------------

void encode_RS(unsigned char data[], unsigned char T[]) {

  int i, j;
  unsigned char carry;

  for (i=0; i<N-K; i++) {
    T[i] = 0;
  }
  for (i=K-1; i>=0; i--) {
    carry = Index_of[data[i] ^ T[N-K-1]];
    if (carry != Index_of_0) {
      for (j=N-K-1; j>0; j--) {
        T[j] = T[j-1];
        if (G[j] != Index_of_0) {
          T[j] ^= Alpha_to[MOD(G[j] + carry)];
        }
      }
      T[0] = Alpha_to[MOD(G[0] + carry)];
    }
    else {
      for (j=N-K-1; j>0; j--) {
        T[j] = T[j-1];
      }
      T[0] = 0;
    }
  }
}

//~----------------------------------------------------------------------------

int decode_RS(unsigned char data[]) {

  int deg_lambda, el, deg_omega;
  int i, j, r;
  int q, tmp, num1, num2, den, discr_r;
  unsigned char recd[255];
  unsigned char lambda[255];
  unsigned char S[255];
  unsigned char b[255], t[255], omega[255];
  unsigned char root[255], reg[255], loc[255];
  int errors, count;

  for (i=0; i<N; i++) {
    recd[i] = Index_of[data[i]];
  }
  errors = 0;
  for (i=0; i<N-K; i++) {   
    tmp = 0;
    for (j=0; j<N; j++) {
      if (recd[j] != Index_of_0) {
        tmp ^= Alpha_to[(recd[j] + (j0+i)*j) % N];
      }
    }
    errors |= tmp;
    S[i+1] = Index_of[tmp];
  }
  if (!errors) {
    return (0);
  }

  for (i=0; i<N-K+1; i++) {
    lambda[i] = (i == 0) ? 1 : 0;
    b[i] = Index_of[lambda[i]];
  }

  r = 0;
  el = 0;
  while (++r <= N-K) {
    discr_r = 0;
    for (i=0; i<r; i++) {
      if ((lambda[i] != 0) && (S[r - i] != Index_of_0)) {
        discr_r ^= Alpha_to[(Index_of[lambda[i]] + S[r - i]) % N];
      }
    }
    discr_r = Index_of[discr_r];
    if (discr_r == Index_of_0) { 
      for (i=N-K-1; i>=0; i--) {
        b[i+1] = b[i];
      }
      b[0] = Index_of_0;
    } 
    else {                     
      for (i=N-K-1; i>=0; i--) {
        t[i+1] = lambda[i+1];
        if (b[i] != Index_of_0) {
          t[i+1] ^= Alpha_to[MOD(discr_r + b[i])];
        }
      }
      t[0] = lambda[0];

      if (2*el <= r-1) {
        el = r - el;
        for (i=0; i<=N-K; i++) {
          b[i] = (lambda[i] == 0) ? 
                 Index_of_0 : MOD(Index_of[lambda[i]] - discr_r + N);
        }
      } 
      else {
        for (i=N-K-1; i>=0; i--) {
          b[i+1] = b[i];
        }
        b[0] = Index_of_0;
      }
      for (i=0; i<N-K+1; i++) {
        lambda[i] = t[i];
      }
    }
  }

  deg_lambda = 0;
  for (i=0; i<N-K+1; i++) {
    lambda[i] = Index_of[lambda[i]];
    if(lambda[i] != Index_of_0) deg_lambda = i;
  }

  for (i=1; i<N-K+1; i++) {
    reg[i] = lambda[i];
  }

  count = 0;
  for (i=1; i<=N; i++) {
    q = 1;
    for (j=deg_lambda; j>0; j--) {
      if (reg[j] != Index_of_0) {
        reg[j] = MOD(reg[j] + j);
        q ^= Alpha_to[reg[j]];
      }
    }
    if (!q) {
      root[count] = i;
      loc[count] = N - i;
      count++;
    }
  }

  if (deg_lambda != count) {
    return (-1);
 }

  deg_omega = 0;
  for (i=0; i<N-K; i++) {
    tmp = 0;
    for (j=(deg_lambda<i) ? deg_lambda : i; j>=0; j--) {
      if ((S[i+1-j] != Index_of_0) && (lambda[j] != Index_of_0)) {
        tmp ^= Alpha_to[MOD(S[i+1-j] + lambda[j])];
      }
    }
    if (tmp != 0) {
      deg_omega = i;
    }
    omega[i] = Index_of[tmp];
  }
  omega[N-K] = Index_of_0;

  for (j=count-1; j>=0; j--) {
    num1 = 0;
    for (i=deg_omega; i>=0; i--) {
      if (omega[i] != Index_of_0) {
        num1 ^= Alpha_to[(omega[i] + i * root[j]) % N];
      }
    }
    num2 = Alpha_to[MOD(root[j] * (j0-1) + N)];
    den = 0;

    for (i=MIN(deg_lambda,N-K-1) & ~1; i>=0; i-=2) {
      if(lambda[i+1] != Index_of_0) {
        den ^= Alpha_to[(lambda[i+1] + i * root[j]) % N];
      }
    }
    if (den == 0) {
      return (-1);
    }

    if (num1 != 0) {
      data[loc[j]] ^= Alpha_to[(Index_of[num1] + Index_of[num2] + 
                      N - Index_of[den]) % N];
    }
  }
  return (count);
}

//~============================================================================

int main(int argc, char* argv[]) {

  int i, N, nerr, status;
  unsigned char original[255], damaged[255], corrected[255];
  
//~-- Reed-Solomon software package initialization (once)

  initialize_RS(223);

//~----- demonstration loop ------

  for (N=0; N<255; N++) {

//~-- generate some data

    for (i=0; i<255; i++) original[i] = rand();

//~-- encode them

    encode_RS(&original[0], &original[223]);

//~-- introduce some errors

    do {
      for (i=0; i<255; i++) damaged[i] = original[i];
      for (i=0; i<N; i++) {
        int k = rand() & 0xFF;
        damaged[k] = rand();
      }
      nerr = 0;
      for (i=0; i<255; i++) {
        if (original[i] != damaged[i]) nerr = nerr + 1;
      }
    } while (nerr != N);
    
//~-- try to detect and correct errors

    for (i=0; i<255; i++) corrected[i] = damaged[i];
    status = decode_RS(&corrected[0]);

//~-- check and print results

    if (status == 0) {
      printf("No errors found\n");
    }
    else if (status > 0) {
      printf("%2d errors found and corrected...", status);
      nerr = 0;
      for (i=0; i<255; i++) {
        if (original[i] != corrected[i]) nerr = nerr + 1;
      }
      if (nerr) printf("checked: %d differences found\n", nerr);
      else      printf("checked: no differences found\n");
    }
    else {
      printf("Too many errors, correction is not possible.\n");
      exit(1);
    }
  }   // end of demonstration loop
  
  return 0;
}

//~============================================================================
