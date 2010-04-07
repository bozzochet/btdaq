#!/usr/bin/perl -w
#  $Id: monitor.cgi,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
use Gtk;
use strict;


use lib::Monitor;
use lib::monitorHTML;



# activate poa and get self ior

my $monitor=new Monitor();


#activate CGI

my $html=new monitorHTML;


 my $ok=$monitor->Connect();

#get ior from server and connect to



 if($ok){
   $html->Update();
 }
 else{
  $html->Warning();
 }


