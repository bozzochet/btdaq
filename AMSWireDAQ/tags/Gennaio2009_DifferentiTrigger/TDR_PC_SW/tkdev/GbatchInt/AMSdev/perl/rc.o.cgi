#!/usr/bin/perl -w
#  $Id: rc.o.cgi,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
use Gtk;
use strict;


use lib::RemoteClient;
if($ENV{MOD_PERL}){
  $#ARGV=-1;
}


my $debug="-d";
#unshift @ARGV, $debug;

unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";
unshift @ARGV, "-N/cgi-bin/mon/rc.o.cgi";


#activate CGI

my $html=new RemoteClient();


#get all info from the db

 my $ok=$html->Connect();

 
#work
 
if (not $ok){
  $html->Warning();
}



