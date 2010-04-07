#!/usr/bin/perl -w       
use Gtk;
use lib::monitorUI;
use lib::Monitor;

package main;

my $window=new monitorUI;

$window->{window}->show_all;
monitorUI::Warning();
monitorUI::MonDialog("qqqqqqqqqqqqqqqrfdsfdsfdsfsdfdsfdsfdsfds\njjjoggdg\nfffffs","Warning",10);
Gtk->main;
