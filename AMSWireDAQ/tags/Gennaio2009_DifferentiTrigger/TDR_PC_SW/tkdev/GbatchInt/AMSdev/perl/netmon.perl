#!/usr/bin/perl -w
#  $Id: netmon.perl,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $

use Carp;
use strict;
$SIG{INT}=sub{exit()};


package main;
use lib qw(/afs/cern.ch/user/a/ams/vc/perl);

#use Error qw(:try);
use lib::NetMonitor;
use Sys::Hostname;
    my $host = hostname;

# activate poa and get self ior


print " ip: $host \n";
my $monitor=new NetMonitor();
$monitor->Run();
