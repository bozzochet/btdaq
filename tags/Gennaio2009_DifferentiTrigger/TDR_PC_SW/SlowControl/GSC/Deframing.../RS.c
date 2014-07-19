/***********************************************************************/
/*                                                                     */
/*        REED - SOLOMON encoding/decoding        A.Lebedev, May'97    */
/*                                                                     */
/***********************************************************************/

#include <stdio.h>      /* this is needed for debug printing only */

#define j0 1            /* alpha**j0 is the first root of generator polynomial
                           in fact, one can choose any j0 for RS code */

int M = 8;                  /* RS code over GF(2**M) */
int K;                      /* K = number of information symbols */
int N = 255;                /* N = number of symbols in code word (=block) */

unsigned char Alpha_to[256];   /* index->polynomial form conversion table
                                         i.e., anti-logarithmic table */
unsigned char Index_of[256];   /* polynomial->index form conversion table 
                                         i.e., logarithmic table */

unsigned char Index_of_0;      /* special value for zero in index form */

unsigned char G[256];          /* G[] stores coefficients of the RS code
                                  generator polynomial g(x), 
                                  deg g(x) = 2*t = N-K */

/*
 *
 * This code supports a symbol size of 8 bits only,
 * and a block size of 256 8-bit symbols (2048 bits). 
 * The code parameter K is set by initialize_RS function.
 *
 */


#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MOD(a)   ((a) < N  ? (a) : (a)-N)
/** #define MOD(a)   ((a) % N) **/


void initialize_RS( const int k) {
/*-------------------------------------------------------------------------
 * Generate all elements of the Galois field GF(2**M) using the irreducible
 * (a.k.a. primitive) polynomial p(x) whose (binary) coefficients are 
 * stuffed into the bits of the variable P.
 *
 * Essentially, produce two lookup tables:
 *
 *       1. index form to polynomial form:  Alpha_to[i] contains j=alpha**i
 *       2. polynomial form to index form:  Index_of[j=alpha**i] contains i
 *
 *            where alpha=2 is the primitive element of GF(2**M)
 *
 * How to use these lookup tables?
 *
 * Let "alpha" be the primitive element that is the root of the primitive 
 * polynomial p(x). 
 * Then in GF(2**M), for any 0 <= i <= 2**M-2,
 *
 * alpha**i = a(0) + a(1)*alpha + a(2)*alpha**2 + ... + a(M-1)*alpha**(M-1)
 *
 * where the binary vector (a(0),a(1),a(2),...,a(M-1)) is the representation
 * of the integer "alpha_to[i]" with a(0) being the LSB and a(M-1) the MSB. 
 * For example, the polynomial representation of alpha**5 would be given by 
 * the binary representation of the integer "alpha_to[5]".
 *
 * Let again "alpha" represent the primitive element of GF(2**M) that is the 
 * root of the primitive polynomial p(x). In order to find the power of 
 * "alpha" that has the polynomial representation
 *
 *       a(0) + a(1)*alpha + a(2)*alpha**2 + ... + a(M-1)*alpha**(M-1)
 *
 * we consider the integer "i" whose binary representation with a(0) being 
 * LSB and a(M-1) MSB is (a(0),a(1),...,a(M-1)) and locate the entry 
 * "index_of[i]". 
 * Now, alpha**index_of[i] is that element whose polynomial representation 
 * is (a(0),a(1),a(2),...,a(M-1)).
 *
 * The element alpha_to[2**M-1] = 0 always signifying that the representation 
 * of "alpha**infinity" = 0 is (0,0,0,...,0).
 *
 * The element Index_of[0] = N always signifying that the power of "alpha" 
 * which has the polynomial representation (0,0,...,0) is "infinity".
 * 
 */

#define DEBUG 0

  int i, j, P;

  K = k;
  Index_of_0 = N;

  if (DEBUG) {
    printf("\nInitialize_RS: N,M,K = %d,%d,%d\n", N, M, K);
  }

/*
 * Primitive (=irreducible) polynomials P are taken from:
 * W.W.Peterson, Error-Correcting Codes, Second Edition,
 * The MIT Press, 1972, Appendix C, Table C.2, pp 476-495.
 *
 */
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

  if (DEBUG) {
    printf("\n Alpha_to[] = %d %d %d %d ... %d %d \n",
    Alpha_to[0], Alpha_to[1], Alpha_to[2], Alpha_to[3],
    Alpha_to[N-1], Alpha_to[N]);
  }

/*
 * Generator polynomial g(x) for (N,K) Reed-Solomon code is calculated
 * as it is described in 
 * R.E.Blahut, Theory and Practice of Error Control Codes, 
 * Addison-Wesley, 1983, p.174. 
 * 
 * The generator polynomial is a product:
 *
 * g(x)=(x+alpha**j0)*(x+alpha**(j0+1))*...*(x+alpha**(j0+(N-K-1)))
 *
 * where: alpha is a primitive element of GF(2**M);
 *        j0 is arbitrary number.
 *
 * The polynomial coefficients are stored in G[], so that
 *
 * g(x)=G[0]+G[1]*x + ... +G[N-K]x**(N-K)
 *
 * Of course, all multiplications and additions should be performed
 * in the Galois field GF(2**M).
 * That's why logarithmic and antilogarithmic tables Alpha_to[] and 
 * Index_of[] are used. 
 * At the end, the g(x) coefficients G[] are transormed into the index 
 * form to speed up the RS encoding.
 *
 */

 /*
  * set g(x) = (alpha**j0+x)
  */
  G[0] = Alpha_to[j0];
  G[1] = 1;
  for (i=1; i<=N-K-1; i++) {
  /*
   * multiply:
   * (G[0]+G[1]*x + ... +G[i]x**i) by (alpha**(j0+i) + x) in GF(2**M)
   */
    G[i+1] = G[i];
    for (j=i; j>0; j--) {
     /*
      * calculate G[j] = G[j]<*>alpha**(j0+i) <+> G[j-1]
      * where operations <*> and <+> are in Galois field GF(2**M)
      */
      if (G[j] != 0) {
        G[j] = Alpha_to[MOD((Index_of[G[j]]) + j0 + i)];
      }
      G[j] = G[j] ^ G[j - 1];
    }
    G[0] = Alpha_to[MOD((Index_of[G[0]]) + j0 + i)];
  }
 /*
  * go to index form to facilitate further usage
  */
  for (i=0; i<=N-K; i++) G[i] = Index_of[G[i]];
  if (DEBUG) {
    printf("\n G[] = %d %d %d %d ... %d %d \n", 
    G[0], G[1], G[2], G[3], G[N-K-1], G[N-K]);
  }
}
#undef DEBUG


/*---------------------------------------------------------------------
 * Take the string of symbols in data[i], i=0..(k-1) and encode it
 * systematically to produce n-k parity symbols in T[0]..T[N-K-1]. 
 *
 * Array data[] is input and T[] is output, and both are in polynomial 
 * form. 
 *
 * Encoding is done by using a feedback shift register with appropriate 
 * connections specified by the elements of G[], which was generated 
 * above.
 * 
 * The resulting codeword is c(x) = data(x)*x**(N-K) + t(x)
 * see Blahut pp.136-137
 *
 */

void encode_RS(unsigned char data[/*K*/], unsigned char T[/*N-K*/]) {

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

/*---------------------------------------------------------------------
 * Performs ERRORS decoding of RS codes. If decoding is successful,
 * writes the codeword into data[] itself. Otherwise data[] is unaltered.
 *
 * Return number of symbols corrected, or -1 if codeword is illegal
 * or uncorrectable.
 * 
 * First "no_eras" erasures are declared by the calling program. Then, 
 * the maximum # of errors correctable is 
 * t_after_eras = floor((N-K-no_eras)/2).
 * If the number of channel errors is not greater than "t_after_eras"
 * the transmitted codeword will be recovered. Details of algorithm 
 * can be found in R. Blahut's "Theory ... of Error-Correcting Codes".
 */

int decode_RS(unsigned char data[/*N*/]) {

#define DEBUG 0

        int deg_lambda, el, deg_omega;
        int i, j, r;
        int u, q, tmp, num1, num2, den, discr_r;
        unsigned char recd[255];
        unsigned char lambda[255];      /* Err+Eras Locator polynomial */
        unsigned char S[255];           /* syndromes, 
                                           not syndrome polynomial */
        unsigned char b[255], t[255], omega[255];
        unsigned char root[255], reg[255], loc[255];
        int errors, count;
/*
 * copy data[] to recd[] converting to index form
 */
 for (i=0; i<N; i++) {
   recd[i] = Index_of[data[i]];
 }
/*
 * calculate syndromes as values of recd(x) at roots of 
 * generator polynomial g(x) i.e. at 
 * x=alpha**(j0+i), i = 0,1,2,..,(N-K-1) (see Blahut p.162)
 */
  errors = 0;
  for (i=0; i<N-K; i++) {   
    tmp = 0;
    for (j=0; j<N; j++) {
      if (recd[j] != Index_of_0) {
        tmp ^= Alpha_to[(recd[j] + (j0+i)*j) % N];
      }
    }
    errors |= tmp;           /* accumulate errors */
    S[i+1] = Index_of[tmp];  /* store syndromes in index form  */
  }
  if (!errors) {
    return (0);     /* no errors found, exit here */
  }
/*
 *
 */
  for (i=0; i<N-K+1; i++) {
    lambda[i] = (i == 0) ? 1 : 0;
    b[i] = Index_of[lambda[i]];
  }

/*
 * Begin Berlekamp-Massey algorithm 
 * to determine error+erasure locator polynomial
 */

  r = 0;
  el = 0;
  while (++r <= N-K) {    /* r is the step number */
           /* Compute discrepancy at the r-th step in poly-form */
    discr_r = 0;
    for (i=0; i<r; i++) {
      if ((lambda[i] != 0) && (S[r - i] != Index_of_0)) {
        discr_r ^= Alpha_to[(Index_of[lambda[i]] + S[r - i]) % N];
      }
    }
    discr_r = Index_of[discr_r];     /* go to index form */

    if (discr_r == Index_of_0) { 
      for (i=N-K-1; i>=0; i--) {
        b[i+1] = b[i];      /*   b(x) = x*b(x)  */
      }
      b[0] = Index_of_0;
    } 
    else {                     
      /*   t(x) = lambda(x) - discr_r*x*b(x)  */
      for (i=N-K-1; i>=0; i--) {
        t[i+1] = lambda[i+1];
        if (b[i] != Index_of_0) {
          t[i+1] ^= Alpha_to[MOD(discr_r + b[i])];
        }
      }
      t[0] = lambda[0];

      if (2*el <= r-1) {
        el = r - el;
        /*   b(x) = lambda(x)/discr_r   */
        for (i=0; i<=N-K; i++) {
          b[i] = (lambda[i] == 0) ? 
                 Index_of_0 : MOD(Index_of[lambda[i]] - discr_r + N);
        }
      } 
      else {
        for (i=N-K-1; i>=0; i--) {
          b[i+1] = b[i];              /* b(x) = x*b(x)  */
        }
        b[0] = Index_of_0;
      }
      for (i=0; i<N-K+1; i++) {
        lambda[i] = t[i];             /* lambda(x) = t(x) */
      }
    }
  }  /* end of   while (++r <= n-k)   */

/*
 * Convert lambda into index form and calculate degree of lambda(x)
 */

  deg_lambda = 0;
  for (i=0; i<N-K+1; i++) {
    lambda[i] = Index_of[lambda[i]];
    if(lambda[i] != Index_of_0) deg_lambda = i;
  }

/*
 * Use Chien search algorithm to find roots 
 * of the error locator polynomial
 */

  for (i=1; i<N-K+1; i++) {
    reg[i] = lambda[i];
  }

  count = 0;              /* Number of roots of lambda(x) */
  for (i=1; i<=N; i++) {
    q = 1;
    for (j=deg_lambda; j>0; j--) {
      if (reg[j] != Index_of_0) {
        reg[j] = MOD(reg[j] + j);
        q ^= Alpha_to[reg[j]];
      }
    }
    if (!q) {
      root[count] = i;     /* store root in index-form    */
      loc[count] = N - i;  /* store error location number */
      count++;
    }
  }

  if (DEBUG) {
    printf("\n Final error positions:\t");
    for (i=0; i<count; i++) {
      printf("%d ", loc[i]);
    }
    printf("\n");
  }

/*
 * if deg(lambda) unequal to number of roots => 
 * => uncorrectable error detected
 */

  if (deg_lambda != count) {
    return (-1);
 }

/*
 * Calculate error evaluator polynomial 
 * omega(x) = s(x)*lambda(x) (modulo x**(N-K))
 * (in index form of course). 
 * Calculate deg(omega).
 */

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

/*
 * Calculate error values in polynomial form:
 * num1 = omega(inv(X(l)))
 * num2 = inv(X(l))**(j0-1) 
 * den  = lambda_pr(inv(X(l)))
 */

  for (j=count-1; j>=0; j--) {
    num1 = 0;
    for (i=deg_omega; i>=0; i--) {
      if (omega[i] != Index_of_0) {
        num1 ^= Alpha_to[(omega[i] + i * root[j]) % N];
      }
    }
    num2 = Alpha_to[MOD(root[j] * (j0-1) + N)];
    den = 0;

/* lambda[i+1] for i even is the formal derivative 
 * lambda_pr of lambda[i] 
 */

    for (i=MIN(deg_lambda,N-K-1) & ~1; i>=0; i-=2) {
      if(lambda[i+1] != Index_of_0) {
        den ^= Alpha_to[(lambda[i+1] + i * root[j]) % N];
      }
    }
    if (den == 0) {
      if (DEBUG) {
        printf("\n ERROR: denominator = 0\n");
      }
      return (-1);
    }

/* Apply error to data  (=make correction) */

    if (num1 != 0) {
      data[loc[j]] ^= Alpha_to[(Index_of[num1] + Index_of[num2] + 
                      N - Index_of[den]) % N];
    }
  }
  return (count);
}
#undef DEBUG
