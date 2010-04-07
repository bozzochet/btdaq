#!/usr/bin/perl -w
#  $Id: chfs.cgi,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
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
my $upd=0;
my $run2p=0;
my $dir=undef;
my $new=undef;
 my $HelpTxt = "
     RemoteClient::ChangeFS
#
# update ntuple table in case one of FS had been renamed
# 
# 
# input par:
#  -dOld:   old fs name eg /f6dah1
#  -nNew:   new fs name   /disk 
#  -u    do update
#  -U    do update + commit
#  -rRun   only process run Run if not 0
#     example
     ./chfs.cgi  -d/f0dah1 -n/s0dah1 -u2
";
   foreach my $chop  (@ARGV){
    if($chop =~/^-d/){
      $dir=unpack("x2 A*",$chop);
    }

    if ($chop =~/^-r/) {
        $run2p=unpack("x2 A*",$chop);
    }
      if ($chop =~/^-u/) {
        $upd=1;
      }
      if ($chop =~/^-U/) {
        $upd=2;
      }
      if($chop =~/^-n/){
      $new=unpack("x2 A*",$chop);
      } 

    if ($chop =~/^-h/) {
      print "$HelpTxt \n";
      return 1;
    }
 }

$html->ChangeFS($dir,$new,$upd,$run2p);






