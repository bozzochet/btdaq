#!/usr/bin/perl -w
#  $Id: gr.cgi,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
use strict;


use lib::RemoteClient;


my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";
unshift @ARGV, "-N/cgi-bin/mon/validateRuns.o.cgi";

my $nocgi=1;
my $html=new RemoteClient($nocgi);
my $ok=$html->ConnectDB();
my $v=1;
my $upd=0;
my $run2p=0;
 my $HelpTxt = "
     RemoteClient::GroupRuns
#
#
#
# Ensures all ntuples from single run reside in the same directory
#
#  input parameters
#  -v   verbose  
#  -u   do update 
#  -rRun  only process Run if not 0 
# 
#     example
     ./gr.cgi -v -u 
";
   foreach my $chop  (@ARGV){
    if ($chop =~/^-r/) {
        $run2p=unpack("x2 A*",$chop);
    }
      if ($chop =~/^-v/) {
        $v=1;
      }
      if ($chop =~/^-u/) {
        $upd=1;
      }

    if ($chop =~/^-h/) {
      print "$HelpTxt \n";
      exit();
    }
 }

$html->GroupRuns($v,$upd,$run2p);






