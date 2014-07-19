#!/usr/bin/perl -w
#  $Id: restorefiles.cgi,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
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
my $dir="MC/AMS02/2004A";
 my $HelpTxt = "
     RemoteClient::UploadToDisks
#
#  Copy castor files to disks
#  Updates catalogs
# -dpath   path to castor files like AMS02/2005A/dir
#                                     /dir are optional ones
# -v   verbose if 1 
# -u   do sql  if 1 
# -rRun    only process Run if not 0
#
#     example
     ./restorefiles.cgi -v -dAMS02/2004A -u 
";
   foreach my $chop  (@ARGV){
    if($chop =~/^-d/){
      $dir=unpack("x2 A*",$chop);
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

    if ($chop =~/^-h/) {
      print "$HelpTxt \n";
      return 1;
    }
 }

$html->UploadToDisks($dir,$v,$upd,$run2p);






