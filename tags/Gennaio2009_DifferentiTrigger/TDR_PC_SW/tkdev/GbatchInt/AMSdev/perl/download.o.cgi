#!/usr/bin/perl -w
#  $Id: download.o.cgi,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";
unshift @ARGV, "-N/cgi-bin/mon/download.o.cgi";




my $nocgi=1;
my $html=new RemoteClient($nocgi);


$html->DownloadSA();






