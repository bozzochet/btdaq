//  $Id: amsstl.h,v 1.2 2008/11/14 10:03:54 haino Exp $
// Author V. Choutko 24-may-1996
// 11.07.96 modified by E.Choumilov.(AMSbins added) 
#ifndef __AMSSTL__
#define __AMSSTL__
#include <math.h>
#include <stdlib.h>
#include "typedefs.h"  
#include "upool.h"
#include <algorithm>
//template <class T> int    cmp( const T* keyval,  const T* datum){
//// for bsearch; isnot really a template;
//typedef int D;
////cout << *(D*)keyval << "   " << *(D*)datum << endl;
//if (*(D*)keyval < *(D*)datum)return -1;
//else if(*(D*)keyval==*(D*)datum)return 0;
//else return 1;
//}
template <class T> int    cmpT( const T& keyval,  const T& datum){
  // IS really a template;
  //cout << keyval << "   " << datum << endl;
  if (keyval < datum)return -1;
  else if(datum < keyval)return 1;
  else return 0;
}
template <class T> integer AMSbi(T *a[], const T& obj, integer i){
  //
  // binary search 
  // input T *a[]   // adresses pf array to search
  // T& obj object
  // i array size
  // output :
  //  -1              obj found
  //  any other       min {*a[]} with obj<*a[output]
  //
  integer ia=0;
  integer il=1;
  integer ir=1;
  integer ib=i-1;
  int j=0;
  while(ia<ib-1 || il*ir){
    integer k=(ia+ib)/2;
    int i=cmpT(obj,*a[k]);
    if(!i)return -1;
    else if(i>0){
      ia=k;
      il=0;
    }
    else  {
      ib=k;
      ir=0;
    }
  }
  //cout << il << ir <<endl;
  if(ir){
    j=cmpT(obj,*a[ib]);
    if(!j)return -1;
    else if(j>0)return ib+1;
  }
  else if(il){
    j=cmpT(obj,*a[ia]);
    if(!j)return -1;
    else if(j<0)return ia;
  }
  return ib;
}
template <class T> integer AMSbiel(const T a[],  T obj, integer i){
  //
  // binary search 
  // input T a[]   //  array to search
  // T obj object
  // i array size
  // output :
  //       min {a[]} obj<=a[output]
  //
  if(i<=0)return -1;
  integer ia=0;
  integer il=1;
  integer ir=1;
  integer ib=i-1;
  int j=0;
  while(ia<ib-1 || il*ir){
    integer k=(ia+ib)/2;
    int i=cmpT(obj,a[k]);
    if(i>0){
      ia=k;
      il=0;
    }
    else  {
      ib=k;
      ir=0;
    }
  }
  if(ir){
    j=cmpT(obj,a[ib]);
    if(j>0)return ib+1;
  }
  else if(il){
    j=cmpT(obj,a[ia]);
    if(j<=0)return ia;
  }
  return ib;
}
template <class T> integer AMSbia(T *a[],  const T & obj, integer i){
  //
  // binary search 
  // input T * a[]   // adresses of  array to search
  // T & obj object
  // i array size
  // output :
  //  any other      min {*a[]} obj<=*a[output]
  //
  integer ia=0;
  integer il=1;
  integer ir=1;
  integer ib=i-1;
  int j=0;
  while(ia<ib-1 || il*ir){
    integer k=(ia+ib)/2;
    int i=cmpT(obj,*a[k]);
    if(i>0){
      ia=k;
      il=0;
    }
    else  {
      ib=k;
      ir=0;
    }
  }
  if(ir){
    j=cmpT(obj,*a[ib]);
    if(j>0)return ib+1;
  }
  else if(il){
    j=cmpT(obj,*a[ia]);
    if(j<=0)return ia;
  }
  return ib;
}
template <class T> T*  AMSbs(T *a[], const T& obj, integer i, char hint){
  //
  // binary search 
  // input T *a[]   // adresses of array to search
  // T& obj object
  // i array size
  // hint if + first try next elem if - first try prev elem
  // output :
  //  0              obj not found
  //                 a[k] ; *a[k]=obj
  //
  integer ia=0;
  integer il=1;
  integer ir=1;
  integer ib=i-1;
  static integer k=0;
  while(ia<ib-1 || il*ir){
    if(hint=='+') {k<ib?k++:ib;hint='\0';}
    else if (hint=='-'){k>ia?k--:ia;hint='\0';}
    else  k=(ia+ib)/2;
    //cout << "search " <<ia<<ib<<k<<endl;
    int i=cmpT(obj,*a[k]);
    if(!i)return a[k];
    else if(i>0){
      ia=k;
      il=0;
      if(ia==ib-1&& ir)if((obj==*a[ib])){k=ib;return a[ib];}
    }
    else  {
      ib=k;
      ir=0;
      if(ia==ib-1 && il)if((obj==*a[ia])){k=ia;return a[ia];}
    }
  }
  return 0;
}
template <class T> integer  AMSbsi(T *a[], const T& obj, integer i, char hint){
  //
  // binary search 
  // input T *a[]   // adresses pf array to search
  // T& obj object
  // i array size
  // output :
  //  -1              obj not found
  //  any other       obj=*a[output]
  //
  integer ia=0;
  integer il=1;
  integer ir=1;
  integer ib=i-1;
  static integer k=0;
  while(ia<ib-1 || il*ir){
    if(hint=='+') {k<ib?k++:ib;hint='\0';}
    else if (hint=='-'){k>ia?k--:ia;hint='\0';}
    else  k=(ia+ib)/2;
    //cout << "search " <<ia<<ib<<k<<endl;
    int i=cmpT(obj,*a[k]);
    if(!i)return k;
    else if(i>0){
      ia=k;
      il=0;
      if(ia==ib-1&& ir)if((obj==*a[ib])){k=ib;return ib;}
    }
    else  {
      ib=k;
      ir=0;
      if(ia==ib-1 && il)if((obj==*a[ia])){k=ia;return ia;}
    }
  }
  return -1;
}

//template <class T> void AMSsort(T *a[], integer n){
//integer * index;
//T** b;
//index= (integer*)UPool.insert(sizeof(integer)*n);
//b=(T**)UPool.insert(sizeof(T*)*n);
//integer n1=n;
//int i1=0,i2=0,i3=0,i22=0,i33=0,i222=0;
//for (int i=0;i < n;i++)index[i]=i+1;
//
//for ( i1=2;i1 <=n;i1++){
//   i3=i1;
//   i33=index[i3-1];
//  T *ai=a[i33-1];
//  for (  i2=i3/2;i2 > 0;i2=i2/2){
//    i22=index[i2-1];
//   if( *ai < *a[i22-1])break;
//   if( *ai == *a[i22-1])break;
//   index[i3-1]=i22;
//   i3=i2;
//  }
//  index [i3-1]=i33;
//}
//m4: i3=index[n-1];
//    index[n-1]=index[0];
//    T *ai=a[i3-1];
//    n--;
//    if(n < 2)goto m12;
//    i1=1;
//m6: i2=i1+i1;
//    if(i2<=n)i22=index[i2-1];
//    if(i2>n)goto m11;
//    else if (i2==n)goto m9;
//    i222=index[i2];
//    if(*a[i22-1]<*a[i222-1]){
//    i2++;
//    i22=i222;}
//m9: if(*ai<*a[i22-1]){
//    index[i1-1]=i22;
//    i1=i2;
//    goto m6;}
//m11:    index[i1-1]=i3;
//    goto m4;
//m12: index[0]=i3;
//for (i=0;i<n1;i++){
//     b[i]=a[index[i]-1];
//}
//    UCOPY(b,a,n1*sizeof(a[0])/4);
//    if (n1 > sz){
//    UPool.udelete(index);
//    UPool.udelete(b);
//}      
//}


template <class T> void AMSsortNAG(T *rv[], integer m2){
  /* Initialized data */
  const int minq=15;
//const double c_b5 = 1.;
  integer ir1 = 15223;
  integer ir2 = 17795;
  integer ir3 = 28707;

  /* System generated locals */
  integer i__1;
  double d__1;

  /* Builtin functions */
  double d_mod();

  /* Local variables */
  static double rand;
  static integer leng, ierr, istk, ilow[100];
  T *a;
  static integer i, j, k;
  T *x;
  static integer ihigh[100];
  static char order[1];
  static integer i1, i2, j1, j2, m1;


  ierr = 0;
  m1 = 0;
  m2=m2-1;
  *order = 'a';
  if (m1 < m2) {
      
    
    leng = m2 - m1 + 1;
    if (leng <= minq) {
      goto L100;
    }

    /*           INITIALISE AND START QUICKSORT ON THE WHOLE VECTOR. */

    istk = 0;
    i = m1;
    j = m2;

    /*           IF THE PREVIOUS PASS WAS BAD, CHANGE THE END VALUES AT */
    /*           RANDOM. */

  L20:
    if (i < 0) {
      i = -i;
      ir1 = ir1 % 177 * 171 - (ir1 / 177 << 1);
      ir2 = ir2 % 176 * 172 - ir2 / 176 * 35;
      ir3 = ir3 % 178 * 170 - ir3 / 178 * 63;
      if (ir1 < 0) {
	ir1 += 30269;
      }
      if (ir2 < 0) {
	ir2 += 30307;
      }
      if (ir3 < 0) {
	ir3 += 30323;
      }
      d__1 = (double) ir1 / 30269. + (double) ir2 / 30307. + (
							      double) ir3 / 30323.;
      rand = d__1-floor(d__1);
      k = (integer) (i + rand * (j - i));
      x = rv[i];
      rv[i] = rv[k];
      rv[k] = x;
      k = i + j - k;
      x = rv[k];
      rv[k] = rv[j];
      rv[j] = x;
    }

    /*           CALCULATE A MEDIAN BY SINGLETONS METHOD. */

    k = (i + j) / 2;
    if (*rv[j] < *rv[i]) {
      x = rv[i];
      rv[i] = rv[j];
      rv[j] = x;
    }
    a = rv[k];
    if (*a < *rv[i]) {
      rv[k] = rv[i];
      rv[i] = a;
      a = rv[k];
    } else if ( *rv[j]< *a) {
      rv[k] = rv[j];
      rv[j] = a;
      a = rv[k];
    }

    /*           SPLIT THE VECTOR INTO TWO ASCENDING PARTS.  THIS IS WHERE */
    /*           THE TIME IS SPENT. */

    i1 = i;
    j1 = j;
  L40:
    ++i1;
    if (*rv[i1] < *a) {
      goto L40;
    }
  L60:
    --j1;
    if (*a <*rv[j1] ) {
      goto L60;
    }
    if (i1 >= j1) {
      goto L80;
    }
    x = rv[i1];
    rv[i1] = rv[j1];
    rv[j1] = x;
    goto L40;

    /*           STACK ONE SUBFILE, IF APPROPRIATE, AND CARRY ON. */

  L80:
    i2 = i1 - i;
    j2 = j - j1;
    if (j2 <= i2) {
      if (i2 <= minq) {
	if (istk <= 0) {
	  goto L100;
	}
	i = ilow[istk - 1];
	j = ihigh[istk - 1];
	--istk;
      } else {

	/*                 TEST FOR VERY UNBALANCED SUBFILES */
	/*                 ( THE DETAILS OF THE TEST ARE FAIRLY ARBITRARY.
			   ) */

	if ((j2 + 5) * 5 < i2) {
	  i = -i;
	}
	if (j2 <= minq) {
	  j = i1 - 1;
	} else {
	  ++istk;
	  ilow[istk - 1] = i;
	  ihigh[istk - 1] = i1 - 1;
	  i = j1 + 1;
	}
      }
    } else {

      /*              DEAL WITH THE CASE WHEN THE SECOND PART IS LARGER. */

      if (j2 <= minq) {
	if (istk <= 0) {
	  goto L100;
	}
	i = ilow[istk - 1];
	j = ihigh[istk - 1];
	--istk;
      } else {

	/*                 TEST FOR VERY UNBALANCED SUBFILES */
	/*                 ( THE DETAILS OF THE TEST ARE FAIRLY ARBITRARY.
			   ) */

	if ((i2 + 5) * 5 < j2) {
	  j1 = -(j1 + 2);
	}
	if (i2 <= minq) {
	  i = j1 + 1;
	} else {
	  ++istk;
	  ilow[istk - 1] = j1 + 1;
	  ihigh[istk - 1] = j;
	  j = i1 - 1;
	}
      }
    }
    goto L20;

    /*           TIDY UP AND DO AN ASCENDING INSERTION SORT. */

  L100:
    i__1 = m2;
    for (i = m1 + 1; i <= i__1; ++i) {
      a = rv[i];
      j = i - 1;
      if (*a < *rv[j]) {
      L120:
	rv[j + 1] = rv[j];
	--j;
	if (j >= m1) {
	  if (*a < *rv[j]) {
	    goto L120;
	  }
	}
	rv[j + 1] = a;
      }
      /* L140: */
    }

    /*           REVERSE THE ORDER IF NECESSARY AND RETURN. */

    /*         IF ((ORDER.EQ.'D') .OR. (ORDER.EQ.'d')) THEN */
    /*            DO 160 I = M1, (M1+M2-1)/2 */
    /*               I1 = M1 + M2 - I */
    /*               X = RV(I) */
    /*               RV(I) = RV(I1) */
    /*               RV(I1) = X */
    /*  160       CONTINUE */
    /*         END IF */

  }
} /* m01caf_ */
template <class T> void AMSsortNAGa(T rv[], integer m2){
  /* Initialized data */
  const int minq=15;
//const double c_b5 = 1.;
  integer ir1 = 15223;
  integer ir2 = 17795;
  integer ir3 = 28707;

  /* System generated locals */
  integer i__1;
  double d__1;

  /* Builtin functions */
  double d_mod();

  /* Local variables */
  static double rand;
  static integer leng, ierr, istk, ilow[100];
  T a;
  static integer i, j, k;
  T x;
  static integer ihigh[100];
  static char order[1];
  static integer i1, i2, j1, j2, m1;


  ierr = 0;
  m1 = 0;
  m2=m2-1;
  *order = 'a';
  if (m1 < m2) {
      
    
    leng = m2 - m1 + 1;
    if (leng <= minq) {
      goto L100;
    }

    /*           INITIALISE AND START QUICKSORT ON THE WHOLE VECTOR. */

    istk = 0;
    i = m1;
    j = m2;

    /*           IF THE PREVIOUS PASS WAS BAD, CHANGE THE END VALUES AT */
    /*           RANDOM. */

  L20:
    if (i < 0) {
      i = -i;
      ir1 = ir1 % 177 * 171 - (ir1 / 177 << 1);
      ir2 = ir2 % 176 * 172 - ir2 / 176 * 35;
      ir3 = ir3 % 178 * 170 - ir3 / 178 * 63;
      if (ir1 < 0) {
	ir1 += 30269;
      }
      if (ir2 < 0) {
	ir2 += 30307;
      }
      if (ir3 < 0) {
	ir3 += 30323;
      }
      d__1 = (double) ir1 / 30269. + (double) ir2 / 30307. + (
							      double) ir3 / 30323.;
      rand = d__1-floor(d__1);
      k = (integer) (i + rand * (j - i));
      x = rv[i];
      rv[i] = rv[k];
      rv[k] = x;
      k = i + j - k;
      x = rv[k];
      rv[k] = rv[j];
      rv[j] = x;
    }

    /*           CALCULATE A MEDIAN BY SINGLETONS METHOD. */

    k = (i + j) / 2;
    if (rv[j] < rv[i]) {
      x = rv[i];
      rv[i] = rv[j];
      rv[j] = x;
    }
    a = rv[k];
    if (a < rv[i]) {
      rv[k] = rv[i];
      rv[i] = a;
      a = rv[k];
    } else if ( rv[j]< a) {
      rv[k] = rv[j];
      rv[j] = a;
      a = rv[k];
    }

    /*           SPLIT THE VECTOR INTO TWO ASCENDING PARTS.  THIS IS WHERE */
    /*           THE TIME IS SPENT. */

    i1 = i;
    j1 = j;
  L40:
    ++i1;
    if (rv[i1] < a) {
      goto L40;
    }
  L60:
    --j1;
    if (a <rv[j1] ) {
      goto L60;
    }
    if (i1 >= j1) {
      goto L80;
    }
    x = rv[i1];
    rv[i1] = rv[j1];
    rv[j1] = x;
    goto L40;

    /*           STACK ONE SUBFILE, IF APPROPRIATE, AND CARRY ON. */

  L80:
    i2 = i1 - i;
    j2 = j - j1;
    if (j2 <= i2) {
      if (i2 <= minq) {
	if (istk <= 0) {
	  goto L100;
	}
	i = ilow[istk - 1];
	j = ihigh[istk - 1];
	--istk;
      } else {

	/*                 TEST FOR VERY UNBALANCED SUBFILES */
	/*                 ( THE DETAILS OF THE TEST ARE FAIRLY ARBITRARY.
			   ) */

	if ((j2 + 5) * 5 < i2) {
	  i = -i;
	}
	if (j2 <= minq) {
	  j = i1 - 1;
	} else {
	  ++istk;
	  ilow[istk - 1] = i;
	  ihigh[istk - 1] = i1 - 1;
	  i = j1 + 1;
	}
      }
    } else {

      /*              DEAL WITH THE CASE WHEN THE SECOND PART IS LARGER. */

      if (j2 <= minq) {
	if (istk <= 0) {
	  goto L100;
	}
	i = ilow[istk - 1];
	j = ihigh[istk - 1];
	--istk;
      } else {

	/*                 TEST FOR VERY UNBALANCED SUBFILES */
	/*                 ( THE DETAILS OF THE TEST ARE FAIRLY ARBITRARY.
			   ) */

	if ((i2 + 5) * 5 < j2) {
	  j1 = -(j1 + 2);
	}
	if (i2 <= minq) {
	  i = j1 + 1;
	} else {
	  ++istk;
	  ilow[istk - 1] = j1 + 1;
	  ihigh[istk - 1] = j;
	  j = i1 - 1;
	}
      }
    }
    goto L20;

    /*           TIDY UP AND DO AN ASCENDING INSERTION SORT. */

  L100:
    i__1 = m2;
    for (i = m1 + 1; i <= i__1; ++i) {
      a = rv[i];
      j = i - 1;
      if (a < rv[j]) {
      L120:
	rv[j + 1] = rv[j];
	--j;
	if (j >= m1) {
	  if (a < rv[j]) {
	    goto L120;
	  }
	}
	rv[j + 1] = a;
      }
      /* L140: */
    }

    /*           REVERSE THE ORDER IF NECESSARY AND RETURN. */

    /*         IF ((ORDER.EQ.'D') .OR. (ORDER.EQ.'d')) THEN */
    /*            DO 160 I = M1, (M1+M2-1)/2 */
    /*               I1 = M1 + M2 - I */
    /*               X = RV(I) */
    /*               RV(I) = RV(I1) */
    /*               RV(I1) = X */
    /*  160       CONTINUE */
    /*         END IF */

  }
} 
#ifdef OBSOLETSOFTWARE
template <class T>
inline  T min(const T& a, const T& b) {
  return b < a ? b : a;
}

template <class T>
inline  T max(const T& a, const T& b) {
  return  a < b ? b : a;
}
#endif
//
template <class T> integer AMSbins(T a[],  T obj, integer i){
  // Modified analog of FORTRAN's LOCATF/LOKATI binary search functions.
  // Array a[] is implied sorted in increasing order (equal values are allowed). 
  //return value: =   0, if obj < a[0];
  //              = - m, where m points to nearest a[m-1] smaller than obj,
  //                     (m is NOT array C++ index, it is position (1,2,...,i);
  //              = + m in case of exact match, m points to first a[] member
  //                    in group of equal elements, if any.
  integer il=0;
  integer ih=i;
  integer im;
  i=cmpT(obj,a[0]);
  if(i<0)return 0;
  else if(i==0)return 1;
  while(1)
    {
      if((ih-il)<=1)return(-il-1);
      im=(il+ih)/2;
      i=cmpT(obj,a[im]);
      if(i<0){ih=im;}
      else if(i>0){il=im;}
      else { if(im==0)return(im+1); // equal elements case :
	while(a[im-1]==a[im])im-=1;
	return(im+1);
      }
    }
}
#endif
