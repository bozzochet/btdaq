#!/usr/local/bin/perl -w
#  $Id: monitor.perl,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $

use Error qw(:try);
use Gtk;
use Carp;
use strict;
$SIG{INT}=sub{exit()};


package main;
use Error qw(:try);
use lib::Monitor;
use lib::monitorUI;


# activate poa and get self ior


#my $mc="-m";
#unshift @ARGV, $mc;
   $#ARGV=-1;
     unshift @ARGV, "-DOracle:";
     unshift @ARGV, "-Famsdb";
                                                                                            

my $monitor=new Monitor();

#get ior from server and connect to

my $ok=$monitor->Connect();

#Activate GTK

my $window=new monitorUI;



#show window and run orbit
 if($ok){
  $ok=$window->Update();
 }
 $window->{window}->show_all;
if(not $ok){
  monitorUI::Warning();
}
else{
 $monitor->{orb}->run();
}
# Gtk->main();
