#!/usr/bin/perl -w
#  $Id: parseJournalFiles.o.cgi,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#use Gtk;
use strict;


use lib::RemoteClient;

$SIG{INT}=sub{exit()};
$SIG{QUIT}=sub{exit()}; 
my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV,"-m ";
unshift @ARGV, "-Famsdb";
unshift @ARGV, "-N/cgi-bin/mon/parseJournalRuns.o.cgi";




my $nocgi=1;
my $html=new RemoteClient($nocgi);

my $ok = $html->parseJournalFiles();






