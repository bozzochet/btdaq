#!/usr/bin/perl -w
#  $Id: removefiles.cgi,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";
unshift @ARGV, "-N/cgi-bin/mon/validateRuns.o.cgi";
unshift @ARGV,"-zero";


my $nocgi=1;
my $html=new RemoteClient($nocgi);
my $ok=$html->ConnectDB();
my $v=1;
my $upd=0;
my $run2p=0;
my $tmp="/tmp/castor";
my $irm=0;
my $dir="AMS02/2004A";
my $notverify=0;
 my $HelpTxt = "
     RemoteClient::RemoveFromDisks
#
# check castor files compare them with local ones
# update ntuple table
# remove files from disks disks 
# 
# input par:
#  -dPath:   path to local files like /disk/MC/AMS02/2005A/dir
#                                   /disk /MC  /dir are optional ones
#  -v   verbose mode 
#  -u    do sql/file rm  
#  -i        rm -i  
#  -tTmp  path to temporarily storage castorfiles
#  -rRun   only process run Run if not 0
#     example
     ./removefiles.cgi -v -d/f0dah1/MC/AMS02/2004A -u -i
";
   foreach my $chop  (@ARGV){
    if($chop =~/^-d/){
      $dir=unpack("x2 A*",$chop);
    }
    if($chop =~/^-t/){
      $tmp=unpack("x2 A*",$chop);
    }

    if ($chop =~/^-r/) {
        $run2p=unpack("x2 A*",$chop);
    }
      if ($chop =~/^-v/) {
        $v=1;
      }
      if ($chop =~/^-u/) {
        $upd=1;
      }
      if ($chop =~/^-i/) {
        $irm=1;
      }
      if($chop =~/^-notverify/){
        $notverify=1;
      } 

    if ($chop =~/^-h/) {
      print "$HelpTxt \n";
      return 1;
    }
 }

$html->RemoveFromDisks($dir,$v,$upd,$irm,$tmp,$run2p,$notverify);






