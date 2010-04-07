# $Id: root.perl,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#!/usr/bin/perl -w       
use strict;
use Carp;
my $input="../include/root.h";
my $output="../include/root_methods.h";
open(FILEI,"<".$input) or die "Unable to open file $input\n";
open(FILEO,">".$output) or die "Unable to open file $output\n";
print FILEO "// \n";
print FILEO "// File Is Auto Generated \n";
print FILEO "// All Manual Changes Will Be Lost \n";
print FILEO "// \n";

# scan file
while ( my $line = <FILEI>){
    if($line=~/^class/ and $line=~/\{/ and !($line=~/\:/) and !($line=~/\;/) and !($line=~/Header/) and !($line=~/Service/) ){
#    print "found $line \n";
    my @pat=split / /,$line;
    $pat[1]=~s/\{//;
    if($pat[1]=~/R$/){
        chomp $pat[1];
        chop $pat[1];
        my $Class="$pat[1]";
        my $class="\L$Class\E";
    #scan up to public
    while ($line=<FILEI>){
        if($line=~/public/){
            last;
        }
    }
    while ($line=<FILEI>){
        if($line=~/ClassDef/ or $line=~/^\}\;/){
            last;
        }
        my $count=0;
        if($line=~/\{/ and !($line=~/\}/)){
            $count++;
            while ($line=<FILEI>){
                if($line=~/\{/){
                    $count++;
                }
                if($line=~/\}/){
                    $count--;
                    if($count<=0){
                       last;
                   }
                }
            }
        } 
        if(!($line=~/\;/) ){
            next;
        }
        my @pat=split /\;/, $line;
        my $subpat=$pat[0];
        if($subpat=~/\(/ or $subpat=~/\{/ or $subpat=~/\,/    or $subpat=~/Info/ or $subpat=~/friend/){
            next;
        }
        @pat=split / /, $subpat;
        if($#pat<1){
            next;
        }
        my $Var=$pat[$#pat];
        my $var="\L$Var\E";
        if($subpat=~/\</ and $subpat=~/vector/){
# get type 
        my @sp1=split /\</,$subpat;
        my @sp2=split /\>/,$sp1[$#sp1];
        my $type=$sp2[0];
        my $output="$type $class\_$var(unsigned int j, unsigned int i){return ( p$Class\(i) && j<p$Class\(i)->$Var.size() )?p$Class\(i)->$Var\[j]:0;} \n";  
        print FILEO $output;
        my $output="$type $class\_$var\_size(unsigned int i){return ( p$Class\(i)  )?p$Class\(i)->$Var.size\():0;} \n";  
        print FILEO $output;
    }
        elsif(!($subpat=~/\</)){
        my $type="";
        for my $i (0 ... $#pat-1){
            if($pat[$i]=~/ /){
                next;
            }
            $type="$type $pat[$i]";
        }
#        print " $type $var \n"; 
        if($var=~/\[/){
          # array
           my @sp1=split /\[/,$var;
           my @Sp1=split /\[/,$Var;
           $var=$sp1[0];
           $Var=$Sp1[0];
           if($#sp1==1){
            my @sp2=split /\]/,$sp1[$#sp1];
            my $max=$sp2[0];
            my $output="$type $class\_$var(unsigned int j, unsigned int i){return (j<$max && p$Class\(i))?p$Class\(i)->$Var\[j]:0;} \n";
            print FILEO $output;
           }
           elsif ($#sp1==2){
            my @sp2=split /\]/,$sp1[$#sp1];
            my $max2=$sp2[0];
            @sp2=split /\]/,$sp1[$#sp1-1];
            my $max1=$sp2[0];
            my $output="$type $class\_$var(unsigned int j, unsigned int k, unsigned int i){return (j<$max1 && k<$max2 && p$Class\(i))?p$Class\(i)->$Var\[j]\[k]:0;} \n";
           print FILEO $output;
           }
           else{
               warn "***triple array found *** $var ****  do nothing \n";
           }
        }
        else{
         #scalar
           my $output="$type $class\_$var(unsigned int i){return p$Class\(i)?p$Class\(i)->$Var:0;} \n";
           print FILEO $output;
#           print $output;
        }   
    }    
}
}
}
}
