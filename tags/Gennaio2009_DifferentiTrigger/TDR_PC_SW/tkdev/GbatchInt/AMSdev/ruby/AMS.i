%module AMS
%{
#include "AMS.h"
%}

%include "typemaps.i"
%apply int *INPUT { int *argc };
%apply int *OUTPUT { int *error, int *iret };
%apply unsigned int *OUTPUT { unsigned int *found };
%apply int &INOUT { int & };
%apply float &INOUT { float & };
%apply double &INOUT { double & };

%typemap(in) Float_t * {
  /* Get the length of the array */
  int size = RARRAY($input)->len;     
  int i;
  $1 = (Float_t *) malloc(size*sizeof(Float_t));
  /* Get the first element in memory */
  VALUE *ptr = RARRAY($input)->ptr;   
  for (i=0; i < size; i++, ptr++) {
      $1[i]= NUM2DBL(*ptr); 
  };
}
%typemap(freearg) Float_t * {
  free($1);
}

%typemap(in) Int_t * {
  /* Get the length of the array */
  int size = RARRAY($input)->len;     
  int i;
  $1 = (Int_t *) malloc(size*sizeof(Int_t));
  /* Get the first element in memory */
  VALUE *ptr = RARRAY($input)->ptr;   
  for (i=0; i < size; i++, ptr++) {
      $1[i]= NUM2INT(*ptr); 
  };
}
%typemap(freearg) Int_t * {
  free($1);
}

%typemap(in) char ** {
  /* Get the length of the array */
  int size = RARRAY($input)->len;     
  int i;
  $1 = (char **) malloc((size+1)*sizeof(char *));
  /* Get the first element in memory */
  VALUE *ptr = RARRAY($input)->ptr;   
  for (i=0; i < size; i++, ptr++)
    /* Convert Ruby Object String to char* */
    $1[i]= STR2CSTR(*ptr); 
  $1[i]=NULL;  /* End of list */
}
// This cleans up the char ** array created before 
// the function call

%typemap(freearg) char ** {
  free((char *) $1);
}

%typemap(out) float [ANY] {
  int i;
  $result = rb_ary_new2($1_size);
  for (i = 0; i < $1_size; i++) {
    VALUE o = rb_float_new((double) $1[i]);
    rb_ary_store($result,i,o);
  }
}
%typemap(in) float [] {
}
%typemap(freearg) float [] {
}
%typemap(in) float [ANY] {
  /* Get the length of the array */
  int size = RARRAY($input)->len;     
  int i;
  $1 = (float *) malloc(size*sizeof(Float_t));
  /* Get the first element in memory */
  VALUE *ptr = RARRAY($input)->ptr;   
  for (i=0; i < size; i++, ptr++) {
      $1[i]= (float) NUM2DBL(*ptr); 
  };
}
%typemap(freearg) float [ANY] {
   if ($1) free($1);
}

%include "AMS.h"
