#!/usr/bin/perl -w
#  $Id: calculateMips.o.cgi,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";
unshift @ARGV, "-N/cgi-bin/mon/calculateMips.o.cgi";




my $nocgi=1;
my $html=new RemoteClient($nocgi);
my $ok=$html->ConnectOnlyDB();
my $fast=0;
my $verbose=1;
   my @output=$html->calculateMipsVC($fast,$verbose);
    my $tcpu=0;
    my $rcpu=0;
   foreach my $arr (@output){
     foreach my $comp (@{$arr}){
       print " $comp ";
     }
     $tcpu+=$arr->[4];
     $rcpu+=$arr->[5];
     print "\n";
   }
   print " $tcpu $rcpu \n";




