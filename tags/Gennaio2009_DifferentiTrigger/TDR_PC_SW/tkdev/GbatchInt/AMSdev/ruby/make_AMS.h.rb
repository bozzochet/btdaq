#!/usr/bin/env ruby
$VERBOSE = nil

file = File.new("USE_THESE_ROOT_CLASSES")
filenew = File.new("AMS.h","w")
troot_h = File.new("TROOT.h","w")

root_config = `which root-config 2> /dev/null`.chomp
root_inc_dir = `#{root_config} --incdir`.chomp

file.seek(0)
before_object_class = ""
after_object_class = "public:\n"
for line in file
  if line =~ /^(T\w+).h\s*$/
    class_type = $1
    if class_type != "TObject"
      chfile = line.chomp
      inc_file = File.new("#{root_inc_dir}/#{chfile}")
      for inc_line in inc_file
        if inc_line =~ /^\s*class\s+(T\w+)\s*[:\{]/
          before_object_class << "class #{$1};\n"
          after_object_class << "      #{$1}* to_#{$1}(){ return (#{$1}*)this;};\n"
        end
      end
    end
  end 
end

file.seek(0)
for line in file
      chfile = line.chomp
      inc_file = File.new("#{root_inc_dir}/#{chfile}")
      for inc_line in inc_file
            if chfile == "TROOT.h"
                  next if inc_line =~ /operator new/
                  filenew.puts inc_line
                  troot_h.puts inc_line
            elsif chfile == "TObject.h"
                  if inc_line =~ /^\s*class\s+TObject\s*\{/
                        filenew.puts before_object_class
                        filenew.puts inc_line
                        filenew.puts after_object_class
                  else
                        filenew.puts inc_line
                  end 
            else
                  filenew.puts inc_line
            end
      end
      inc_file.close
end

file.close
filenew.close
troot_h.close
