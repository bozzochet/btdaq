#!/usr/bin/perl -w
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";
#unshift @ARGV, $debug;

unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";


#activate CGI

my $html=new RemoteClient();


#get all info from the db

 my $ok=$html->ConnectOnlyDB();
    $ok=$html->updateDataSetsDescription();

 
#work
 
if (not $ok){
  $html->Warning();
}



