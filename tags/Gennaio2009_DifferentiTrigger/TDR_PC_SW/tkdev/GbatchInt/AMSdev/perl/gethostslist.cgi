#!/usr/bin/perl -w
#  $Id: gethostslist.cgi,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";
#unshift @ARGV, $debug;

unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";
unshift @ARGV, "-N/cgi-bin/mon/rc.mon.cgi";


#activate CGI

my $html=new RemoteClient();


#get number of used hosts for each site

 my $ok=$html->ConnectOnlyDB();
    $ok=$html->getHostsList();

 
#work
 
if (not $ok){
  $html->Warning();
}



