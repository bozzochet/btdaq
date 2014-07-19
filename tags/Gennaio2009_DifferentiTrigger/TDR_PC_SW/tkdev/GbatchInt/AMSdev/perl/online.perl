# $Id: online.perl,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#!/usr/bin/perl -w       
use strict;
use Carp;
my $output="../online/AMSEverything.cxx";
my $input="../include/root_methods.h";
open(FILEI,"<".$input) or die "Unable to open file $input\n";
open(FILEO,">".$output) or die "Unable to open file $output\n";
print FILEO "// \n";
print FILEO "// File Is Auto Generated \n";
print FILEO "// All Manual Changes Will Be Lost \n";
print FILEO "// \n";
print FILEO '#include <iostream>'."\n";
print FILEO '#include "AMSNtuple.h"'."\n";
print FILEO '#include "AMSDisplay.h"'."\n";
print FILEO '#include <math.h>'."\n";
print FILEO '#include <TPaveText.h>'."\n";
print FILEO '#include <TProfile.h>'."\n";
print FILEO '#include <TF1.h>'."\n";
print FILEO '#include "AMSEverything.h"'."\n";
print FILEO 'void AMSEverything::Book(){'."\n";
print FILEO "int color=18;\n";
my $i=0;
# scan file
my $funo="";
while ( my $line = <FILEI>){
    if($line=~/^\// ){
      next;
    }
    if($line =~/\(/){
     my @junk=split /\(/, $line;
     my @junk2 = split / /, $junk[0];
     my $fun=$junk2[$#junk2];
     my @junk3=split /\)/, $junk[1];
     if($junk3[0]=~/\,/){
#    two arguments
#      next;
         if(not ($line=~/\j</)){
             next;
         }
         my @a3=split /\,/,$junk3[0];
         if($#a3>1){
          next;
         }
     }
     elsif($fun=~/size/){
#        next;
    }
     @junk=split /\_/,$fun;
     if($funo ne $junk[0]){
         $funo=$junk[0];
          print FILEO 'AddSet("_'.$funo.'");'."\n"; 
          my @junk4=split /[ (]/,$junk3[1]; 
          $junk4[1]=~s/p/N/;
          print FILEO '_filled.push_back(new TH1F("'.$junk4[1].'","'.$junk4[1].'",200,0,0));'."\n";
     } 
     if($junk3[0]=~/\,/){
         if($line =~/size/){
#             print "$line \n";
      print FILEO 'AddSet("'.$fun.'");'."\n"; 
      print FILEO '_filled.push_back(new TH1F("'.$fun.'","'.$fun.'",200,0,0));'."\n";
             
         }
         else{
         my @junk5=split /[\<&]/,$line;
         for my $i (0...$junk5[1]-1){
          print FILEO 'AddSet("'.$fun."\_$i".'");'."\n"; 
          print FILEO '_filled.push_back(new TH1F("'.$fun."\_$i".'","'.$fun."\_$i".'",200,0,0));'."\n";
          }
     }
     }
     else{
      print FILEO 'AddSet("'.$fun.'");'."\n"; 
      print FILEO '_filled.push_back(new TH1F("'.$fun.'","'.$fun.'",200,0,0));'."\n";
 }
    }
}
    $funo="";
    print FILEO ' cout <<"  TotalOf " <<_filled.size()<<" Histos Booked for AMSEveryThing "<<endl;'."\n";
close (FILEI);
open(FILEI,"<".$input) or die "Unable to open file $input\n";
print FILEO "}\n";
print FILEO 'void AMSEverything::ShowSet(Int_t Set){'."\n";
print FILEO "gPad->Clear();
TVirtualPad * gPadSave = gPad;
_filled[Set]->Draw();
gPadSave->cd();
}
void AMSEverything::Fill(AMSNtupleR * ntuple){
int i=0;\nfloat t;\n";

while (my $line=<FILEI>){
    if($line=~/^\// ){
      next;
    }
    if($line =~/\(/){
     my @junk=split /\(u/, $line;
     my @junk2 = split / /, $junk[0];
     my $fun=$junk2[$#junk2];
     # two arguments?
     my @junk3=split /\)/, $junk[1];
     my $tag=1;
     if($junk3[0]=~/\,/){
         $tag=4;
         if(not ($line=~/\j</)){
             next;
         }
         my @a3=split /\,/,$junk3[0];
         if($#a3>1){
          next;
      }


     }
     elsif($fun=~/size/){
#        next;
    }

     
     my @junk4=split /[ (]/,$junk3[1]; 
     $junk4[$tag]=~s/p/N/;
     if($fun=~/size/){
#       next;
     }
     @junk=split /\_/,$fun;
     if($funo ne $junk[0]){
         $funo=$junk[0];
         print FILEO "_filled[i]->Fill(ntuple->".$junk4[1].'(),1.);'."\ni++;\n";
     }
     if($junk3[0]=~/\,/){
          if($line =~/size/){
              my @j1=split /</,$line; 
              my @j2=split / \)/,$j1[1];
              $j2[0]=~s/\(i\)/\(loc\)/;
              my @j3=split /\(/,$j2[0];
              $j3[0]=~s/p/N/; 
              print FILEO " for(int loc=0;loc<ntuple->$j3[0]();loc++){
  for( int j=0;j\<ntuple->$fun\_size(loc);j++){
   t=ntuple->$fun\(j,loc\);
   if(fabs(t)<1e20)_filled[i]->Fill(t,1.);
  }
 }\n";
 print FILEO "i++;\n";   
          }
          else{
       my @junk5=split /[\<&]/,$line;
          print FILEO "for( int j=0;j\<$junk5[1];j++){\n";
          print FILEO " for(int loc=0;loc<ntuple->$junk4[$tag]();loc++){
          t=ntuple->$fun\(j,loc\);
          if(fabs(t)<1e20)_filled[i]->Fill(t,1.);\n";
          print FILEO "}\ni++;\n";   
          print FILEO "}\n";   
   }
     }
     else{
          if($line =~/size/){
              my @j1=split /\?/,$line;
              my @j2=split/\(/,$j1[1];
              $j2[0]=~s/p/N/;
          print FILEO "{
         for(int loc=0;loc<ntuple->$j2[0]();loc++){
          t=ntuple->$fun\(loc\);
          if(fabs(t)<1e20)_filled[i]->Fill(t,1.);\n";
          print FILEO "}\n}\ni++;\n";   
          }
          else{
          print FILEO "{
          for(int loc=0;loc<ntuple->$junk4[$tag]();loc++){
           t=ntuple->$fun\(loc\);
           if(fabs(t)<1e20)_filled[i]->Fill(t,1.);\n";
          print FILEO "}\n}\ni++;\n";   
          }
     }
    }
}
print FILEO "}\n";

