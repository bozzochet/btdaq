#!/usr/bin/perl -w
use Gtk;
use strict;
use lib qw(/var/www/cgi-bin/mon); 
use lib::RemoteClient;
my $debug="-d";
#unshift @ARGV, $debug;

unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";
unshift @ARGV, "-N/cgi-bin/mon/rc.mon.cgi";


#activate CGI

my $html=new RemoteClient();


#get all info from the db

 my $ok=$html->ConnectDB();

    $ok=$html->checkJobsTimeout();

 
 



