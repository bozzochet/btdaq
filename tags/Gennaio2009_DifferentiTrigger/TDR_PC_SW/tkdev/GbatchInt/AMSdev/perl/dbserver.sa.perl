#!/usr/bin/perl -w
use strict;
#use Gtk;
use lib::DBServer;
$SIG{INT}=sub{exit()};
$SIG{QUIT}=sub{exit()};

# activate poa and get self ior

my $server=new DBServer();

#open db file and connect to server

my $ok=$server->Init(1);

if($ok){

    print "$server->{dbfile} $server->{rtb_maxr} \n "
}


