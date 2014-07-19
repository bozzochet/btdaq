#!/usr/bin/perl -w       
use strict;
use Carp;
rd(".");
sub rd{
my $dir=shift;
opendir THISDIR ,$dir or die "unable to open $dir";
my @allfiles= readdir THISDIR;
closedir THISDIR;
foreach my $file (@allfiles){
    if ($file =~/^\./){
       next;
    }
    if ($file =~/^CVS/){
       next;
    }
    my $newfile=$dir."/".$file;
    if(readlink $newfile){
        next;
    }
    my @sta = stat $newfile;
    if($sta[2]>32000){
        my $id="  \$Id: fupd.perl,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $\n";
        my $pr;
        if(($file =~/.C/) or ($file =~/\.h/) or ($file =~/\.c/)){
            $pr="\/\/";
        }elsif($file =~/\.F/ or $file =~/\.f/ or $file =~/\.inc/){
            $pr="\*";
        }elsif($file =~/\.doc/ or $file =~/^M/){
            $pr="\#";
        }elsif($file =~/\.txt/ or $file =~/\.pm/){
            $pr="\#";
        }elsif($dir =~/install/ ){
            $pr="\#";
        }
        else{
            print "\n******** Unknown file type found $newfile \n";
            next;
        }
        $id=$pr.$id;
        open(FILE,"<".$newfile) or die "Unable to open file $file\n";
        my $buf;
        my $buf2;
        read(FILE,$buf,16384000) or next;
        read(FILE,$buf2,1) and die "Buffer too small $file\n";
        close(FILE);        
        open(FILE,">".$newfile) or die "Unable to open file $file\n";
        print FILE $id;
        print FILE $buf;
        close FILE;
    }else{
        rd($newfile);
    }
}
}
