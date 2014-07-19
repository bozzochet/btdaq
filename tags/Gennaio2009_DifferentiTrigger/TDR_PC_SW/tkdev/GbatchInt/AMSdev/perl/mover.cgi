#!/usr/bin/perl -w

use strict;
use lib::Mover;

 my $mover=new Mover();
 my $ok = $mover -> Connect();
 if ($ok) {
    $ok = $mover-> doCopy;
 } 
