#!/usr/bin/perl -w
#  $Id: validateRuns.o.cgi,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
use Gtk;
use strict;

#use lib qw(/var/www/cgi-bin/mon);
use lib::RemoteClient;


my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";
unshift @ARGV, "-N/cgi-bin/mon/validateRuns.o.cgi";
unshift @ARGV, "-r0";



my $nocgi=1;
my $html=new RemoteClient($nocgi);

$html->set_root_env();
$html->ValidateRuns();






