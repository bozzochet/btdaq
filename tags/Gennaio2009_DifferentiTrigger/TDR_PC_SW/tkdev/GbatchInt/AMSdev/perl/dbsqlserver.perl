#!/usr/bin/perl -w

use strict;
#use Gtk;
use lib::DBSQLServer;
$SIG{INT}=sub{exit()};
$SIG{QUIT}=sub{exit()};

# activate poa and get self ior

my $server=new DBSQLServer();

#open db file and connect to server

my $ok=$server->Connect();

if($ok){
    print "  Hurra\n";
    my $query="select * from Cites";
    my ($res,$fields)=$server->QueryAll($query);
      my $cite="test";
     $query="select maxrun from Cites where name='$cite'";
    my $ret=$server->Query($query);
    print "qq $ret->[0][0] \n";
#     $query="update Cites set maxrun=2 where name='test'";
#    $server->Update($query);    
     $query="select maxrun from Cites where name='$cite' and state=1";
    $ret=$server->Query($query);
    if(defined $ret->[0][0]){
              print "qq $ret->[0][0] \n";
          }
}


