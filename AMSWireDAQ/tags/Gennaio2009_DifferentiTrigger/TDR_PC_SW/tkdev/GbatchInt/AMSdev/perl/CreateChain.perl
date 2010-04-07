#!/usr/bin/perl -w       
use strict;
use Carp;

#my $RunTable="/f0dat1/AMSDataDir/DataManagement/prod/RunTable";
my $RunTable="../prod/RunTable";
my @dirs=("/f2dat1/STS91/hen");
my $token="hbk";
my $OldChain="sts91.prechain";
my $NewChain="../analysis/sts91.hen.chain";
my @myruns;
my @myevf;
my @myevl;
foreach my $chop  (@ARGV){
}


open(FILEI,"<".$RunTable) or die "Unable to open file $RunTable\n";
my $line;
while ( $line = <FILEI>){
    if (($line =~/^\#/)){
       next;
    }
    if (($line =~/laser/)){
       next;
    }
 my @pat=split / /,$line;
 $myruns[$#myruns+1]=$pat[0];
 $myevf[$#myruns+1]=$pat[1];
 $myevl[$#myruns+1]=$pat[2];
}
close(FILEI);
print "$#myruns found\n";


#open(CHAII,"<".$OldChain) or die "Unable to open file $OldChain\n";

#my $buf;

#read(CHAII,$buf,1638400) or die "Unable to read file $OldChain\n";
#close(CHAII);
#my @sbuf = split /\n/,$buf;

my @sbuf=    rd (\@dirs, $token);

my @sortedbuf= sort prio @sbuf;
open(CHAIO,">".$NewChain) or die "Unable to open file $NewChain\n";
foreach my $line (@sortedbuf){
    print CHAIO "nt/chain sts91 $line \n";
    my @i1=split '\.',$line;
    my @i3=split '\/',$i1[0];
    $i1[0]=$i3[$#i3];
    for my $i (0...$#myruns){
     if($i1[0] eq $myruns[$i]){
         $myruns[$i]=-$myruns[$i];
       last;
     }
    }    
}
close(CHAIO);
    foreach my $run (@myruns){
        if($run>0){
            print " run $run not found\n";
         }
    }
sub prio{
    my @i1=split '\.',$a;
    my @i2=split '\.',$b;
    my @i3=split '\/',$i1[0];
    $i1[0]=$i3[$#i3];
    @i3=split '\/',$i2[0];
    $i2[0]=$i3[$#i3];
    if($i1[0] ne $i2[0]){
        return $i1[0]<=>$i2[0];
    }
    else{
        if($i1[1] eq $i2[1]){
            warn "duplicated entry found $a $b \n";
        }
        return $i1[1]<=>$i2[1];
    }
}

sub rd{
    my $dirs=shift;
    my $token=shift;
    my @files;
    $#files=-1;
foreach my $dir (@{$dirs}){
 opendir THISDIR ,$dir or die "unable to open $dir";
 my @allfiles= readdir THISDIR;
 closedir THISDIR;
 foreach my $file (@allfiles){
    if (($file =~/^\./)){
       next;
    }
    my $newfile=$dir."/".$file;
    if(readlink $newfile){
        next;
    }
    my @sta = stat $newfile;
    if($sta[2]>32000){
        if($file =~ /\.$token$/){
            push @files, $newfile;
    }
    }
    else{
        my @newfiles=($newfile);
        my @addon=rd(\@newfiles,$token);
        foreach my $addonf (@addon){
            push @files, $addonf;
         
        }
    }
}

}
 return @files;
}
