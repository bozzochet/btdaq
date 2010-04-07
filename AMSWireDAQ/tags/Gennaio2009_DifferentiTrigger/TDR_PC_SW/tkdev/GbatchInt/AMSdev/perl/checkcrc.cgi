#!/usr/bin/perl -w
#  $Id: checkcrc.cgi,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
use strict;
#use lib qw(/var/www/cgi-bin/mon); 
use lib qw(lib); 
use lib::RemoteClient;
my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";
unshift @ARGV,"-zero";

my $force=0;
my $nocgi=1;
my $html=new RemoteClient($nocgi);
my $ok=$html->ConnectDB();
my $v=1;
my $i=0;
my $upd=0;
my $run2p=0;
my $dir=undef;
 my $HelpTxt = "
     RemoteClient::CheckCRC
#
#
#  check crc of files on disk
#  if wrong upload from castor
#  if still wrong delete  from disk and update corr db entry  
#  put status as 'bad'
#  input paramaters
#  -d :   path  to files like MC/AMS02/2005A/dir or /s0dah1 
#           if missed all the files will be checked
#                                   
#  -v   verbose if 1 
#  -u     do sql  if 1
#  -r   only process run $run2p if not 0
#  -i    will ask confirmation for rm command
#  -f    to recheck bad ntuples
#  output par:
#   1 if ok  0 otherwise
#
# 
#     example
     ./checkcrc.cgi -v -u -d/s0dah1  
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
      if ($chop =~/^-i/) {
        $i=1;
      }
      if ($chop =~/^-f/) {
        $force=1;
      }
      if ($chop =~/^-u/) {
        $upd=1;
      }

    if ($chop =~/^-h/) {
      print "$HelpTxt \n";
      exit();
    }
 }

$html->CheckCRC($v,$i,$upd,$run2p,$force,$dir);







