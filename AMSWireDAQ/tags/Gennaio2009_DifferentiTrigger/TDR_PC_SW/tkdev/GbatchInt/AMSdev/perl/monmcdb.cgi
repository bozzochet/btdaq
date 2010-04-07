#!/usr/bin/perl -w
#  $Id: monmcdb.cgi,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
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

unshift @ARGV, "-N/cgi-bin/mon/monmcdb.cgi";


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


