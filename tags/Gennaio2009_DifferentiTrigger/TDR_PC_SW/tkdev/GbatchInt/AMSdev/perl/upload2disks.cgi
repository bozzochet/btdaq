#!/usr/bin/perl -w
#  $Id: upload2disks.cgi,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
use strict;
use lib qw(/var/www/cgi-bin/mon); 
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
my $upd=1;
my $run2p=0;
my $dir="MC/AMS02/2005A";
 my $HelpTxt = "
     RemoteClient::UploadToCastor
#
#
#  uploads file to disk
#  input paramaters
#  -d :   path  to castor files like MC/AMS02/2005A/dir
#                                   /dir are optional ones
#  -v   verbose if 1 
#  -u     do sql  if 1
#  -r   only process run $run2p if not 0
#  output par:
#   1 if ok  0 otherwise
#
# 
#     example
     ./upload2disks.cgi -v -u -dMC/AMS02/2004A 
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
      exit();
    }
 }

$html->UploadToDisks($dir,$v,$upd,$run2p);







