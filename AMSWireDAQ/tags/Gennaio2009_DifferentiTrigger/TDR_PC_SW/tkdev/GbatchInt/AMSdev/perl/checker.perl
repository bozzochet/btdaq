#!/usr/bin/perl -w
use strict;
$SIG{INT}=sub{exit()};
$SIG{QUIT}=sub{exit()};
my $amsprodlogdir=$ENV{AMSDataDir};
$amsprodlogdir=$amsprodlogdir."/prod.log/";
foreach my $chop  (@ARGV){
    if($chop=~/^-A/){
     $amsprodlogdir=unpack("x2 A*",$chop);
    }
}
   my $checkfile=$amsprodlogdir."systemC";
    while(1){ 
      sleep 10;
      open(FILE,"<".$checkfile) or next;
      my $line=<FILE>;
      close(FILE);
      my @spbuf=split "; ",$line;
      if($#spbuf>1){
         $spbuf[2]=~s/\'//g;
         $spbuf[2]=~s/\"//g;
         $spbuf[1]=~ s/ +$//;
         my @sb4=split ' ',$spbuf[2];
          $spbuf[2]="";
         foreach my  $chlen  (@sb4){
          $spbuf[2]=$spbuf[2]." ".$chlen;
         }
         #print  " $spbuf[0] $spbuf[1] $spbuf[2]\n"; 
       if(time()-$spbuf[0]>120){
        unlink "$checkfile.$spbuf[1]";
        my $cmd="ps -elf | grep $spbuf[1] >$checkfile.$spbuf[1]";
        system($cmd);
        #print " $cmd \n";
             open(FILE,"<".$checkfile.".".$spbuf[1]) or next;
             my @buf=<FILE>;
             close(FILE);
             unlink "$checkfile.$spbuf[1]";
             foreach $line (@buf){
               $line=~s/\'//g;
               if($line=~$spbuf[1]){
                   print "$line \n";
                }
               if($line=~$spbuf[2] && $line=~$spbuf[1]){
                my @goal=split " ",$line;
                 if($#goal>3){
                  print "checker.perl-W-FoundStickyProcess $line \n";
                  system("kill -9 $goal[3]");
                 }
               }
             }        
       }
      }
     }


