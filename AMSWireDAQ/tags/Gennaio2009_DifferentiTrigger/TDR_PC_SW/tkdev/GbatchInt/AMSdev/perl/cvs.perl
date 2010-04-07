#!/usr/local/bin/perl -w       
use strict;
use Carp;
my $cvsroot=$ENV{CVSROOT};
my @splitroot=split ':',$cvsroot;
my $acvsroot= $splitroot[$#splitroot];
my $initdir=shift @ARGV;
if (not defined $initdir){
    $initdir=".";
}
rd($initdir);
sub rd{
my $dir=shift;
opendir THISDIR ,$dir or die "unable to open $dir";
my @allfiles= readdir THISDIR;
closedir THISDIR;

foreach my $file (@allfiles){
    if ($file =~/^\./){
       next;
    }
    my $newfile=$dir."/".$file;
    if(readlink $newfile){
        next;
    }
    my @sta = stat $newfile;
    if($sta[2]>32000){
    }else{

       if($file eq "CVS"){
        my $root= $dir."/$file/Root";
        open(FILE,"<".$root) or die "Unable to open file $root\n";
        my $buf;
        read(FILE,$buf,16384000) or die "Unable to read file $root\n";;
        my @sbuf=split "\n",$buf;
        my $oldroot=$sbuf[0];
        my @splitoldroot=split ':',$oldroot;
        my $aoldroot= $splitoldroot[$#splitoldroot];
        $buf=~ s/$oldroot/$cvsroot/;         
        close(FILE);        
        open(FILE,">".$root) or die "Unable to open file $root\n";
        print FILE $buf;
        close FILE;
        my $rep= $dir."/$file/Repository";   
        open(FILE,"<".$rep) or die "Unable to open file $rep\n";
        read(FILE,$buf,16384000) or die "Unable to read file $rep\n";;
        $buf=~ s/$aoldroot/$acvsroot/;         
        close(FILE);        
        open(FILE,">".$rep) or die "Unable to open file $rep\n";
        print FILE $buf;
        close FILE;
       }


        rd($newfile);
    }
}
}
