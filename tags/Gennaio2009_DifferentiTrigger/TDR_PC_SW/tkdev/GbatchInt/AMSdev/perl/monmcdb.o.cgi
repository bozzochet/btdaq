#!/usr/bin/perl -w
#  $Id: monmcdb.o.cgi,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
use Gtk;
use strict;


use lib::Monitor;
use lib::monitorHTML;
if($ENV{MOD_PERL}){
  $#ARGV=-1;
}
my $mc="-m ";
unshift @ARGV, $mc;
my $db="-d ";
unshift @ARGV, $db;
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";
unshift @ARGV, "-N/cgi-bin/mon/monmcdb.o.cgi";



# activate poa and get self ior

my $monitor=new Monitor();



#activate CGI

my $html=new monitorHTML;

 my $ok=$monitor->Connect();

#get ior from server and connect to



 if($ok){
   $html->Update();
 }
 else{
  $html->Warning();
 }


