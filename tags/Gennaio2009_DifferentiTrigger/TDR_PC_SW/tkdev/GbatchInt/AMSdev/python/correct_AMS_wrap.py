#!/usr/bin/env python2.2

import re

file = open("AMS_wrap.cxx")
filenew = open("AMS_wrapnew.cxx","w")

for line in file:
   match1 = re.search(r'^(\s*)if \(SWIG_ConvertPtr\((.+),.+, SWIGTYPE_p_Float_t, 0\) == -1\) \{',line)
   match2 = re.search(r'^(\s*)if \(SWIG_ConvertPtr\((.+),.+, SWIGTYPE_p_Int_t, 0\) == -1\) \{',line)
   if match1:
      line = "%sif (!(PyList_Check(%s)) || !(PyFloat_Check(PyList_GetItem(%s,0)))) {\n" % \
                  (match1.group(1),match1.group(2),match1.group(2))
   elif match2:
      line = "%sif (!(PyList_Check(%s)) || !(PyInt_Check(PyList_GetItem(%s,0)))) {\n" % \
                  (match2.group(1),match2.group(2),match2.group(2))

   filenew.write(line)
