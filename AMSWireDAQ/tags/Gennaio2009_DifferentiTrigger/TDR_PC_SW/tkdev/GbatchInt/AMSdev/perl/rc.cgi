#!/usr/bin/perl -w
#  $Id: rc.cgi,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-D";
#unshift @ARGV, $debug;


#activate CGI

my $html=new RemoteClient();


#get all info from the db

 my $ok=$html->Connect();

 
#work
 
if (not $ok){
  $html->Warning();
}



