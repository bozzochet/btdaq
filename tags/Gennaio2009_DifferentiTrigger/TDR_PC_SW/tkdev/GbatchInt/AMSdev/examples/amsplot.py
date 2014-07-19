#!/usr/bin/env python2.2
# If the previous commans does not work, look for something like: 
#  /afs/cern.ch/asis/packages/GNU.LANG/python-2.2.2/i386_redhat73/usr.local/bin/python2.2
#
#   Program to plot any AMS variable
#   Juan Alcaraz, 18-Oct-2003
import sys
import os
import re

def process_cut(cut):
      try:
            float(cut)
      except ValueError:
            print 'Wrong value of cut "' + cut + '"; EXIT'
            sys.exit()
      else:
            return cut

def process_operator(op):
      if op=="<":
            op
      elif op==">":
            op
      elif op=="<=":
            op
      elif op==">=":
            op
      elif op=="==":
            op
      elif op=="!=":
            op
      elif op=="<>":
            op = "!="
      else:
            print 'Wrong value of operand "' + op + '"; EXIT'
            sys.exit()
      return op

def process_variable(variable,root_h_file):
      splitvar = variable.split(".")
      if len(splitvar)==1:
            if splitvar[0]=="nHeader":
                  print 'Variable "nHeader" not allowed; EXIT'
                  sys.exit()
            elif splitvar[0][0:1]=="n":
                  splitvar[0]= splitvar[0][1:]
                  variable = splitvar[0]
            else:
                  variable = "Header." + splitvar[0]
                  splitvar = variable.split(".")


      match = re.search(r'^([\w\d]+)[\W\D]*',splitvar[0])
      if match:
            splittest0 = match.group(1)
      else:
            print 'Wrong variable "' + variable + '"; EXIT'
            sys.exit()
      
      if len(splitvar)>1:
            match2 = re.search(r'^([\w\d]+)[\W\D]*',splitvar[1])
            if match2:
                  splittest1 = match2.group(1)
            else:
                  print 'Wrong variable "' + variable + '"; EXIT'
                  sys.exit()

      line = root_h_file.readline()
      found = 0
      while line:
            match3 = re.search(r'class\s+' + splittest0 + 'R\s*\{',line)
            if not match3:
                  line = root_h_file.readline()
                  continue
            if len(splitvar)==1: 
                  found = 1
                  break

            found = 0
            while not re.search(r'ClassDef\s*\(' + splittest0 + 'R',line):     
                  line = root_h_file.readline()
                  if re.search(r'\s*public:',line):
                        found = 1
                  elif re.search(r'\s*private:',line):
                        found = 0
                  elif re.search(r'\s*protected:',line):
                        found = 0
                  if re.search(r'\W+' + splittest1 + '\W+',line):
                        found += 1
                        if found==2: break

      if found<len(splitvar):
            print 'Wrong variable "' + variable + '"; EXIT'
            sys.exit()

      return variable

def make_amsplot_file(filearray, options, full_variable):
      root_h = os.environ["AMSDir"] + "/include/root.h"
      if os.path.exists(root_h):
            root_h_file = file(root_h,"r")
      else:
            print "Header file \"" + root_h + "\" does not exist; EXIT"
            sys.exit()

      amsplot_file = file("amsplot.C","w")

      amsplot_file.write("int amsplot() {\n")
      amsplot_file.write("\tgROOT->Reset();\n")
      amsplot_file.write("\tgSystem->Load(\"$AMSDir/lib/linux/ntuple.so\");\n")
      amsplot_file.write("\n\tTChain chain(\"AMSRoot\");\n")

      for filename in filearray:
            if "--list" in options:
                  filelist = file(filename,"r")
                  for rootfile in filelist.readlines():
                        rootfile = rootfile.rstrip()
                        amsplot_file.write("\tchain.Add(\"" + rootfile + "\");\n")
                  filelist.close()
            else:
                  amsplot_file.write("\tchain.Add(\"" + filename +"\");\n")

      amsplot_file.write("\tchain.SetMakeClass(1);\n")
      amsplot_file.write("\n\tAMSEventR Event;\n")
      amsplot_file.write("\tEvent.SetBranchA(&chain);\n")

      match1 = re.search(r'^([^<>!=]+)([<>=!]+)(\w+[^<>!=]+)([<>=!]+)([^<>=!]+)$',full_variable)
      match2 = re.search(r'^(\w+[^<>!=]+)([<>=!]+)([^<>=!]+)$',full_variable)
      match3 = re.search(r'^([^<>!=]+)([<>=!]+)(\w+[^<>!=]+)$',full_variable)
      if match1:
            # print "min<var<max: " + match.group(1:)
            mincut = process_cut(match1.group(1))
            minop = process_operator(match1.group(2))
            variable = process_variable(match1.group(3),root_h_file)
            maxop = process_operator(match1.group(4))
            maxcut = process_cut(match1.group(5))
      elif match2:
            # print "var<max: " + match.group(1:)
            mincut = False
            minop = False
            variable = process_variable(match2.group(1),root_h_file)
            maxop = process_operator(match2.group(2))
            maxcut = process_cut(match2.group(3))
      elif match3:
            # print "min<var: " + match.group(1:)
            mincut = process_cut(match3.group(1))
            minop = process_operator(match3.group(2))
            variable = process_variable(match3.group(3),root_h_file)
            maxop = False
            maxcut = False
      else:
            # print "Only var: " + full_variable
            mincut = False
            minop = False
            variable = process_variable(full_variable,root_h_file)
            maxop = False
            maxcut = False

      splitvar = variable.split(".")

      amsplot_file.write("\n\th_test = new TH1F(\"h_test\",\"" + full_variable + "\",100,0.,0.);\n\n")

      amsplot_file.write("\tfor (int entry=0; entry<chain.GetEntries(); entry++) {\n")
      amsplot_file.write("\t\tchain.GetEntry(entry);\n")
      if len(splitvar)>1:
            if splitvar[0]!="Header":
                  amsplot_file.write("\t\tfor (int i=0; i<Event.n" + splitvar[0] + "(); i++){\n" )
                  if  mincut:
                        amsplot_file.write("\t\t\tif (!(" + mincut + minop + "Event." + splitvar[0] + "(i)." + splitvar[1] + ")) continue;\n")
                  if  maxcut:
                        amsplot_file.write("\t\t\tif (!(Event." + splitvar[0] + "(i)." + splitvar[1] + maxop + maxcut + ")) continue;\n")
                  amsplot_file.write("\t\t\th_test->Fill(Event." + splitvar[0] + "(i)." + splitvar[1] + ");\n")
                  amsplot_file.write("\t\t}\n")
            else:
                  if  mincut:
                        amsplot_file.write("\t\tif (!(" + mincut + minop + "Event.fHeader." + splitvar[1] + ")) continue;\n")
                  if  maxcut:
                        amsplot_file.write("\t\tif (!(Event.fHeader." + splitvar[1] + maxop + maxcut + ")) continue;\n")
                  amsplot_file.write("\t\th_test->Fill(Event.fHeader." + splitvar[1] + ");\n")

      else:
            if  mincut:
                  amsplot_file.write("\t\tif (!(" + mincut + minop + "Event.n" + splitvar[0] + "())) continue;\n")
            if  maxcut:
                  amsplot_file.write("\t\tif (!(Event.n" + splitvar[0] + "()" + maxop + maxcut + ")) continue;\n")
            amsplot_file.write("\t\th_test->Fill(Event.n" + splitvar[0] + "());\n")

      amsplot_file.write("\t}\n\n" )

      amsplot_file.write("\th_test->Draw();\n")

      amsplot_file.write("}\n\n")

      amsplot_file.close()

### Now the main program...
options = []
parameters = []
for arg in sys.argv[1:]:
  if arg[0:2]=="--":
      options.append(arg)
  else:
      parameters.append(arg)

if "--help" in options:
      print "SCRIPT amsplot: "
      print "\t\tamsplot [options] [variable] [files]"
      print "\t to plot one AMS variable from a set of root files"
      print ""
      print "\ta) It checks the validity of [variable] and the existence of [files]"
      print "\tb) It creates the file \"amsplot.C\""
      print "\tc) It executes \"root amsplot.C\" at the end (use \"--test\" to avoid this)"
      print "USAGES: "
      print "\tamsplot [variable] [root_file1] [root_file2] ..."
      print "\tamsplot --list [variable] [file_with_list_of_root_files]"
      print "\tamsplot --test [variable] [root_file1] [root_file2] ..."
      print "\tamsplot --test --list [variable] [file_with_list_of_root_files]"
      print "EXAMPLES: "
      print "\tamsplot nTrTrack proton_1.root               --> #tracks in event" 
      print "\tamsplot nTrTrack proton_1.root proton_2.root --> #tracks in event" 
      print "\tamsplot nTrTrack proton*.root                --> #tracks in event" 
      print "\tamsplot '1<=nTrTrack<=5' gammas_*_*.root     --> #tracks in a range" 
      print "\tamsplot Header.Run helium.root.*             --> Run number"
      print "\tamsplot Run helium.root.*                    --> Run number"
      print "\tamsplot TrTrack.Rigidity ntp/*.root.*        --> Track rigidities"
      print "\tamsplot 'TrTrack.Rigidity>0' my_file.root    --> Track rigidities>0"
      print "\tamsplot 'TrTrack.NTrRecHit()' my_file.root   --> #hits on tracks"
      print "\tamsplot 'TrTrack.HPhi[1]' my_file.root       --> Phi of first half-track"
      print "\tamsplot 'TrTrack.HPhi[1]!=0' my_file.root    --> (HPhi==0 => not filled)"
      print "\tamsplot 'TrTrack.iTrRecHit(2)' my_file.root  --> link to 2nd hit on track"
      print "\tamsplot --test nParticle my_file.root        --> #particles; test mode"
      print "\tamsplot --list nParticle list_of_root_files  --> #particles; list mode"
      print "IMPORTANT: "
      print "\tPlease set the $AMSDir environment variable before using the script"
      sys.exit()

elif len(sys.argv)<3 or len(options)>2 or len(parameters)<2:
      print "SCRIPT amsplot: "
      print "\t\tamsplot [options] [variable] [files]"
      print "\t to plot one AMS variable from a set of root files"
      print ""
      print "Use \"amsplot --help\" to get a longer explanation"
      sys.exit()

if not os.environ["AMSDir"]:
      print "\tPlease set the $AMSDir environment variable before using the script."
      print "\tExample on tcsh:\tsetenv AMSDir /afs/ams.cern.ch/Offline/vdev"
      print "\tExample on bash:\texport AMSDir=/afs/ams.cern.ch/Offline/vdev"
      sys.exit()

for opt in options:
      if opt != "--list" and opt != "--test":
            print "Wrong option \"" + opt + "\"; EXIT"
            sys.exit()

filearray = parameters[1:]
for filename in filearray:
      if not os.path.exists(filename):
            print 'Root file "' + filename + '" does not exist; EXIT'
            sys.exit()
full_variable = parameters[0]
make_amsplot_file(filearray, options, full_variable)

if "--test" in options:
  print "TEST mode> OK. Anyway:"
  print "               a) Check the file \"amsplot.C\""
  print "               b) Run \"root amsplot.C\" after user modifications"
else:
  os.system("root amsplot.C")

