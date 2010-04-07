#!/usr/bin/env ruby
#   Ruby/Tk GUI to plot AMS variables
#   Juan Alcaraz, Jan-2004

require "tk"

if FileTest.exists?("/afs/ams.cern.ch/Offline/vdev")
      ENV["AMSDir"] = "/afs/ams.cern.ch/Offline/vdev"
end

if !ENV["AMSDir"]
      puts "\tPlease set the $AMSDir environment variable before using the script."
      puts "\tExample on tcsh:\tsetenv AMSDir /afs/ams.cern.ch/Offline/vdev"
      puts "\tExample on bash:\texport AMSDir=/afs/ams.cern.ch/Offline/vdev"
      exit
end

root_h = ENV["AMSDir"] + "/include/root.h"
if FileTest.exists?(root_h)
      $root_h_file = File.new(root_h,"r")
else
      puts "Header file \"" + root_h + "\" does not exist; EXIT"
      exit
end

if ARGV.size==0
      $myfiles = TkVariable.new("/f2users/choutko/g3v1g3.root")
else
      $myfiles = TkVariable.new(ARGV.join(" "))
end
$cutmin = TkVariable.new("-1.e10<")
$cutmax = TkVariable.new("!=0")
$root_processes = []

def process_cut(cut)
    begin
        cut_test = Float(cut)
    rescue ArgumentError 
        #puts "Wrong value of cut \"#{cut}\"; ignored"
        nil
    end
    cut
end

def process_operator(op)
    case op
      when "<"
            op
      when ">"
            op
      when "<="
            op
      when ">="
            op
      when "=="
            op
      when "!="
            op
      when "<>"
            op = "!="
      else
            #puts "Wrong value of operand \"#{op}\"; ignored"
            nil
    end
end

def process_variable(variable)
      splitvar = variable.split(".")
      if splitvar.size==1
            if splitvar[0]=="nHeader"
                  puts 'Variable "nHeader" not allowed; ignored'
                  return nil
            elsif splitvar[0][0..0]=="n"
                  splitvar[0].slice!(0)
                  variable = splitvar[0]
            else
                  variable = "Header." + splitvar[0]
                  splitvar = variable.split(".")
            end
      end

      if /^([\w\d]+)[\W\D]*/.match(splitvar[0])
            splittest0 = $1
      else
            puts "Wrong variable '#{variable}'; ignored"
            return nil
      end

      $root_h_file.seek(0)
      match = 0
      for line in $root_h_file
            if line =~ /class\s+#{splittest0}R\s*\{/
                  match = 1
                  break
            end
      end

      if splitvar.size==2 && match==1
            if /^([\w\d]+)[\W\D]*/.match(splitvar[1])
                  splittest1 = $1
            else
                  puts "Wrong variable '#{variable}'"
                  return nil
            end
            match = 0
            for line in $root_h_file
                  break if line =~ /ClassDef\s*\(#{splittest0}R/ 
                  if line =~ /\s*public:/ 
                        match = 1
                  elsif line =~ /\s*private:/ 
                        match = 0
                  elsif line =~ /\s*protected:/ 
                        match = 0
                  end
                  if line =~ /\W+#{splittest1}\W+/ 
                        match += 1
                        break if match==2
                  end
            end
      end

      if match<splitvar.size
            puts "Wrong variable '#{variable}'; ignored"
            return nil
      end

      variable

end

def make_amsplot_file(leftcut, var, rightcut)
      all_files = []
      for fil in $myfiles.to_s.split(" ")
        all_files.concat(Dir[fil])
      end
      if all_files.size==0
            return nil
      end   

      amsplot_file = File.new("amsplot.C","w")

      amsplot_file.write("int amsplot() {\n")
      amsplot_file.write("\tgROOT->Reset();\n")
      amsplot_file.write("\tgSystem->Load(\"$AMSDir/lib/linux/ntuple.so\");\n")
      amsplot_file.write("\n\tTChain chain(\"AMSRoot\");\n")

      for filename in all_files
            if FileTest.exists?(filename)
                  amsplot_file.write("\tchain.Add(\"" + filename +"\");\n")
            else
                  puts "File #{filename} does not exists; ignoring it"
            end
      end

      amsplot_file.write("\tchain.SetMakeClass(1);\n")
      amsplot_file.write("\n\tAMSEventR Event;\n")
      amsplot_file.write("\tEvent.SetBranchA(&chain);\n")

      variable = process_variable(var)
      if variable==nil 
            return variable
      end

      mincut = nil
      minop = nil
      if /^([^<>!=]+)([<>=!]+)$/.match(leftcut)
            mincut = process_cut($1)
            minop = process_operator($2)
            if minop==nil or mincut==nil
                  minop = nil
                  mincut = nil 
                  $cutmin.value = ""
                  topinfo_update
            end
      end

      maxcut = nil
      maxop = nil
      if /^([<>=!]+)([^<>=!]+)$/.match(rightcut)
            maxop = process_operator($1)
            maxcut = process_cut($2)
            if maxop==nil or maxcut==nil
                  maxop = nil
                  maxcut = nil 
                  $cutmax.value = ""
                  topinfo_update
            end
      end

      splitvar = variable.split(".")
      if mincut and maxcut
            full_variable = mincut + minop + var + maxop + maxcut
      elsif mincut
            full_variable = mincut + minop + var
      elsif maxcut
            full_variable = var + maxop + maxcut
      else
            full_variable = var
      end

      amsplot_file.write("\n\th_test = new TH1F(\"h_test\",\"" + full_variable + "\",100,0.,0.);\n\n")

      amsplot_file.write("\tfor (int entry=0; entry<chain.GetEntries(); entry++) {\n")
      amsplot_file.write("\t\tchain.GetEntry(entry);\n")
      if splitvar.size>1
            if splitvar[0]!="Header"
                  amsplot_file.write("\t\tfor (int i=0; i<Event.n" + splitvar[0] + "(); i++){\n" )
                  if mincut
                        amsplot_file.write("\t\t\tif (!(" + mincut + minop + "Event." + splitvar[0] + "(i)." + splitvar[1] + ")) continue;\n")
                  end
                  if maxcut
                        amsplot_file.write("\t\t\tif (!(Event." + splitvar[0] + "(i)." + splitvar[1] + maxop + maxcut + ")) continue;\n")
                  end
                  amsplot_file.write("\t\t\th_test->Fill(Event." + splitvar[0] + "(i)." + splitvar[1] + ");\n")
                  amsplot_file.write("\t\t}\n")
            else
                  if mincut
                        amsplot_file.write("\t\tif (!(" + mincut + minop + "Event.fHeader." + splitvar[1] + ")) continue;\n")
                  end
                  if maxcut
                        amsplot_file.write("\t\tif (!(Event.fHeader." + splitvar[1] + maxop + maxcut + ")) continue;\n")
                  end
                  amsplot_file.write("\t\th_test->Fill(Event.fHeader." + splitvar[1] + ");\n")
            end

      else
            if  mincut
                  amsplot_file.write("\t\tif (!(" + mincut + minop + "Event.n" + splitvar[0] + "())) continue;\n")
            end
            if  maxcut
                  amsplot_file.write("\t\tif (!(Event.n" + splitvar[0] + "()" + maxop + maxcut + ")) continue;\n")
            end
            amsplot_file.write("\t\th_test->Fill(Event.n" + splitvar[0] + "());\n")
      end

      amsplot_file.write("\t}\n\n" )

      amsplot_file.write("\th_test->Draw();\n")

      amsplot_file.write("}\n\n")

      amsplot_file.close

      variable
end

TkRoot.new {
  title "AMS INSPECTOR"
  background "black"
}

$showfiles = TkLabel.new {
   background "black"
   foreground "white"
   pack
}

framefile = TkFrame.new {
      background "black"
      pack(:pady=>10)
}

frame0 = TkFrame.new {
      background "black"
      pack(:pady=>10)
}

mainframe = TkFrame.new {
      background "darkblue"
      borderwidth 20
}

def finalize_ams_inspect
   for pro in $root_processes
      #puts "Killing process [" + pro + "]"
      `kill -9 #{pro} 2> /dev/null`
   end
   exit
end

def amsplot(v)
   topinfo_update
   leftcut = $cutmin.to_s
   rightcut = $cutmax.to_s
   var = v
   istat = make_amsplot_file(leftcut, var, rightcut)
   #puts " istat= " + istat.to_s
   if istat
      pro = fork{`root amsplot.C 2> /dev/null`}
      $root_processes << (pro+3).to_s
   end
end

def topinfo_update
   colortext = "cyan"
   used_files = 0
   all_files = []
   for fil in $myfiles.to_s.split(" ")
      all_files.concat(Dir[fil])
   end
   files_in_many_lines = ""
   files_in_one_line = ""
   files_one_per_line = ""
   maxlen = 80
   nextlen = maxlen
   len = 0
   for filename in all_files
      if FileTest.exists?(filename)
            used_files += 1
            len += filename.size
            if len>nextlen
                  files_in_many_lines << "\n"
                  nextlen += maxlen
            end
            if used_files==1
                  files_in_many_lines << filename
                  files_in_one_line << filename
            else
                  files_in_many_lines << ", " + filename
                  files_in_one_line << " " + filename
            end
            files_one_per_line << filename + "\n"
      end
   end
   if used_files==0
      $myfiles.value = ""
      files_in_many_lines = "Please provide file(s)..."
   end

   if /^([^<>!=]+)([<>=!]+)$/.match($cutmin.to_s)
     if process_cut($1) and process_operator($2)
       mintext = $cutmin.to_s + " variable_in_plot"
     else
       $cutmin.value = ""
       colortext = "red"
       mintext = "INVALID CUT"
     end
   elsif $cutmin.to_s!=""
     $cutmin.value = ""
     colortext = "red"
     mintext = "INVALID CUT"
   else
     mintext = ""
   end
   if /^([<>=!]+)([^<>=!]+)$/.match($cutmax.to_s)
     if process_operator($1) and process_cut($2)
       maxtext = "variable_in_plot " + $cutmax.to_s
     else
       $cutmax.value = ""
       colortext = "red"
       maxtext = "INVALID CUT"
     end
   elsif $cutmax.to_s!=""
     $cutmax.value = ""
     colortext = "red"
     maxtext = "INVALID CUT"
   else
     maxtext = ""
   end
   $showfiles.configure("foreground"=>colortext,
   "text"=>"AMS INSPECTOR\n\n =====CURRENT FILE(S)=====\n" +\
        "#{files_in_many_lines}\n" + \
        "\n ======CURRENT CUTS=======\n" + \
   "#{mintext} \t\t\t #{maxtext}\n"
   )
end

topinfo_update

labelfile = TkLabel.new(framefile) {
  text "Use file(s): "
  background "black"
  foreground "white"
}
labelfile.pack(:side=>:left,:padx=>5)

selfile = TkEntry.new(framefile,:textvariable=>$myfiles) {
      background "white"
      foreground "black"
      relief "sunken"
      width 70
      bind('Return',proc{topinfo_update})
}
selfile.pack(:side=>:left, :fill=>:both, :expand=>true)

label0 = TkLabel.new(frame0) {
  text "Apply the following cuts:"
  background "black"
  foreground "white"
}
label0.pack(:side=>:left,:padx=>5)

cut0 = TkEntry.new(frame0,:textvariable=>$cutmin) {
      justify "right"
      background "white"
      foreground "black"
      relief "sunken"
      width 10
      bind('Return',proc{topinfo_update})
}
cut0.pack(:side=>:left,:fill=>:both, :expand=>true)

label1 = TkLabel.new(frame0) {
  text "variable_in_plot"
  background "black"
  foreground "white"
}
label1.pack(:side=>:left,:padx=>5)

cut1 = TkEntry.new(frame0,:textvariable=>$cutmax) {
      background "white"
      foreground "black"
      relief "sunken"
      width 10
      bind('Return',proc{topinfo_update})
}
cut1.pack(:side=>:left,:fill=>:both, :expand=>true)

TkButton.new { 
      background "red"
      foreground "black"
      text "EXIT"
      command proc{finalize_ams_inspect}
      pack(:pady=>15)
}

other = TkFrame.new(mainframe) {
      background "darkblue"
      pack(:pady=>10)
}
trd = TkFrame.new(mainframe) {
      background "darkblue"
      pack(:pady=>10)
}
tof = TkFrame.new(mainframe) {
      background "darkblue"
      pack(:pady=>10)
}
tr = TkFrame.new(mainframe) {
      background "darkblue"
      pack(:pady=>10)
}
anti = TkFrame.new(mainframe) {
      background "darkblue"
      pack(:pady=>10)
}
rich = TkFrame.new(mainframe) {
      background "darkblue"
      pack(:pady=>10)
}
ecal = TkFrame.new(mainframe) {
      background "darkblue"
      pack(:pady=>10)
}

list_of_classes = []
current_class = nil
public_flag = false
if_comments = false
barstar_comments = false
variables = []
for line in $root_h_file
      if line =~ /^#if/
            if_comments = true
            next
      elsif line =~ /^#endif/
            if_comments = false
            next
      end
      if if_comments
            next
      end
      if line =~ /^\s*\/\*/
            barstar_comments = true
            next
      elsif line =~ /^\s*\*\//
            barstar_comments = false
            next
      end
      if barstar_comments
            next
      end
      if line =~ /^(.+)\/\/\//
            line = $1
      elsif line =~ /^\/\/\//
            next
      end
      if line =~ /^class\s+(.+)R\s*[:\{]/
            if current_class
              if current_class[0..3]=="Ecal"
                mycol1 = "lightgrey"
                myframe = ecal
              elsif current_class[0..3]=="Rich"
                mycol1 = "cyan"
                myframe = rich
              elsif current_class[0..3]=="Anti"
                mycol1 = "orange"
                myframe = anti
              elsif current_class[0..2]=="Tof"
                mycol1 = "lightgreen"
                myframe = tof
              elsif current_class[0..2]=="Trd"
                mycol1 = "pink"
                myframe = trd
              elsif current_class[0..1]=="Tr"
                mycol1 = "lightblue"
                myframe = tr
              else 
                mycol1 = "white"
                myframe = other
              end
              TkMenubutton.new(myframe,:relief=>:raised) { |menumother|
                  foreground "black"
                  background mycol1
                  text current_class
                  i = 0
                  mycol2 = "yellow"
                  menu TkMenu.new(menumother) {
                      if current_class != "Header"
                        var = "# of " + current_class + "'s"
                        var0 = "n" + current_class 
                        add(:command,:background=>mycol2,:label=>var,:command=>proc{amsplot(var0)})
                      end
                      variables.each { |var|
                        var2 = current_class + "." + var 
                        if var[-2..-1]=="()"
                              lab = var[0..-3]
                        else
                              lab = var
                        end
                        add(:command,:background=>mycol2,:label=>lab,:command=>proc{amsplot(var2)})
                      }
                  }
                  pack(:side=>:left,:padx=>10)
              }
            end

            variables.clear
            current_class = $1
            if current_class=="AMSEvent"
                  current_class=nil
                  next
            end
            list_of_classes << current_class

      end
      if not current_class
            next
      end

      if line =~ /^\s*#{current_class}R\(/
            next
      end
      if line =~ /^\s*public:/ 
            public_flag = true
            next
      elsif line =~ /^\s*private:/ 
            public_flag = false
            next
      elsif line =~ /^\s*protected:/ 
            public_flag = false
            next
      end

      if !public_flag
            next
      end
      if line =~ /^\s*ClassDef/
            next
      elsif line =~ /^\s*friend/
            next
      end

      if line =~ /^(\s*int\s+)i(\w+)\s*\(unsigned\s+int.*\)/
             variables.unshift("N#{$2}()")
      elsif line =~ /^(\s*int\s+)(\w+);/
             variables << "#{$2}"
      elsif line =~ /^(\s*unsigned\s+int\s+)(\w+);/
             variables << "#{$2}"
      elsif line =~ /^(\s*float\s+)(\w+);/
             variables << "#{$2}"
      elsif line =~ /^(\s*int\s+)(\w+)\[(\d+)\]\s*;/
            for i in 0...$3.to_i
                  variables << "#{$2}[#{i}]"
            end
      elsif line =~ /^(\s*unsigned int\s+)(\w+)\[(\d+)\]\s*;/
            for i in 0...$3.to_i
                  variables << "#{$2}[#{i}]"
            end
      elsif line =~ /^(\s*float\s+)(\w+)\[(\d+)\]\s*;/
            for i in 0...$3.to_i
                  variables << "#{$2}[#{i}]"
            end
      elsif line =~ /^(\s*int\s+)(\w+)\[(\d+)\]\[(\d+)\]\s*;/
            for i in 0...$3.to_i
            for j in 0...$4.to_i
                  variables << "#{$2}[#{i}][#{j}]"
            end
            end
      elsif line =~ /^(\s*unsigned int\s+)(\w+)\[(\d+)\]\[(\d+)\]\s*;/
            for i in 0...$3.to_i
            for j in 0...$4.to_i
                  variables << "#{$2}[#{i}][#{j}]"
            end
            end
      elsif line =~ /^(\s*float\s+)(\w+)\[(\d+)\]\[(\d+)\]\s*;/
            for i in 0...$3.to_i
            for j in 0...$4.to_i
                  variables << "#{$2}[#{i}][#{j}]"
            end
            end
      end

end

mainframe.pack

selfile.focus

Tk.mainloop
