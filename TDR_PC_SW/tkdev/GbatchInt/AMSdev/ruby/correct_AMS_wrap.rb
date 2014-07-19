#!/usr/bin/env ruby

file = File.new("AMS_wrap.cxx")
filenew = File.new("AMS_wrapnew.cxx","w")

for line in file
   if line =~ /^(.+)T_DATA && SWIG_ConvertPtr\((.+), &ptr, SWIGTYPE_p_Float_t, 0\) != -1/
      line = "#{$1}T_ARRAY)) ? 1 : 0;\n"
      line += "if (TYPE(*RARRAY(#{$2})->ptr) != T_FLOAT) _v = 0;\n"
   elsif line =~ /^(.+)T_DATA && SWIG_ConvertPtr\((.+), &ptr, SWIGTYPE_p_Int_t, 0\) != -1/
      line = "#{$1}T_ARRAY)) ? 1 : 0;\n"
      line += "if (TYPE(*RARRAY(#{$2})->ptr) != T_FIXNUM) _v = 0;\n"
   end

   filenew.puts line
end
