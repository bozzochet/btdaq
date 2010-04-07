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
  /* Check if is a list */
  if (PyList_Check($input)) {
    int size = PyList_Size($input);
    int i = 0;
    $1 = (Float_t *) malloc(size*sizeof(float));
    for (i = 0; i < size; i++) {
      PyObject *o = PyList_GetItem($input,i);
      if (PyFloat_Check(o))
      $1[i] = PyFloat_AsDouble(PyList_GetItem($input,i));
      else {
      PyErr_SetString(PyExc_TypeError,"list must contain floats");
      free($1);
      return NULL;
      }
    }
  } else {
    PyErr_SetString(PyExc_TypeError,"not a list");
    return NULL;
  }
}
%typemap(freearg) Float_t * {
  free($1);
}

%typemap(in) Int_t * {
  /* Check if is a list */
  if (PyList_Check($input)) {
    int size = PyList_Size($input);
    int i = 0;
    $1 = (Int_t *) malloc(size*sizeof(int));
    for (i = 0; i < size; i++) {
      PyObject *o = PyList_GetItem($input,i);
      if (PyInt_Check(o))
      $1[i] = PyInt_AsLong(PyList_GetItem($input,i));
      else {
      PyErr_SetString(PyExc_TypeError,"list must contain ints");
      free($1);
      return NULL;
      }
    }
  } else {
    PyErr_SetString(PyExc_TypeError,"not a list");
    return NULL;
  }
}
%typemap(freearg) Int_t * {
  free($1);
}

%typemap(in) char ** {
  /* Check if is a list */
  if (PyList_Check($input)) {
    int size = PyList_Size($input);
    int i = 0;
    $1 = (char **) malloc((size+1)*sizeof(char *));
    for (i = 0; i < size; i++) {
      PyObject *o = PyList_GetItem($input,i);
      if (PyString_Check(o))
      $1[i] = PyString_AsString(PyList_GetItem($input,i));
      else {
      PyErr_SetString(PyExc_TypeError,"list must contain strings");
      free($1);
      return NULL;
      }
    }
    $1[i] = 0;
  } else {
    PyErr_SetString(PyExc_TypeError,"not a list");
    return NULL;
  }
}

%typemap(freearg) char ** {
  free((char *) $1);
}
%typemap(out) float [ANY] {
  int i;
  $result = PyList_New($1_size);
  for (i = 0; i < $1_size; i++) {
    PyObject *o = PyFloat_FromDouble((double) $1[i]);
    PyList_SetItem($result,i,o);
  }
}
%typemap(in) float [] {
}
%typemap(freearg) float [] {
}
%typemap(in) float [ANY] {
  int i;
  if (!PySequence_Check($input)) {
    PyErr_SetString(PyExc_ValueError,"Expected a sequence");
    return NULL;
  }
  if (PySequence_Length($input) != $1_size) {
    PyErr_SetString(PyExc_ValueError,"Size mismatch. Expected $1_size elements");
    return NULL;
  }
  $1 = (float *) malloc($1_size*sizeof(float));
  for (i = 0; i < $1_size; i++) {
    PyObject *o = PySequence_GetItem($input,i);
    if (PyNumber_Check(o)) {
      $1[i] = (float) PyFloat_AsDouble(o);
    } else {
      PyErr_SetString(PyExc_ValueError,"Sequence elements must be numbers");      
      return NULL;
    }
  }
}
%typemap(freearg) float [ANY] {
   if ($1) free($1);
}

%include "AMS.h"
