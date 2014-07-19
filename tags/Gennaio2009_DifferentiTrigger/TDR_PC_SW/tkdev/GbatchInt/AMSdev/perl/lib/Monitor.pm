# $Id: Monitor.pm,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $

package Monitor;
use CORBA::ORBit idl => [ '../include/server.idl'];
use Error qw(:try);
use Carp;
use strict;
use lib::CID;
use lib::ActiveClient;
use lib::POAMonitor;
use lib::DBServer;
use lib::DBSQLServer;
use Time::Local;
use Fcntl ":flock";
use Fcntl;
use MLDBM qw(DB_File Storable);
@Monitor::EXPORT= qw(new Update Connect);

my %fields=(
     cid=>undef,
     start=>undef,
     ac=>undef,
     orb=>undef,
     root_poa=>undef,
     mypoamonitor=>undef,
     myref=>undef,
     myior=>undef,
     arsref=>[],
     arpref=>[],
     ardref=>[],
     nhl=>undef,
     ahls=>undef,
     ahlp=>undef,
     acl=>undef,
     aml=>undef,
     asl=>undef,
     adbsl=>undef,
     acl_maxc=>undef,
     aml_maxc=>-1,
     asl_maxc=>undef,
     adbsl_maxc=>undef,
     nsl=>undef,
     ncl=>undef,
     nkl=>undef,
     rtb=>undef,
     rtb_maxr=>undef,
     dsti=>undef,
     dsts=>undef,
     dbfile=>undef,
     db=>undef,
     env=>undef,
     rn=>undef,
     ok=>0,
     registered=>0,
     updatesviadb=>0,
     sqlserver=>undef,
     DataMC=>1,
        IOR=>undef,
            );

sub new{
    my $type=shift;
    my $self={
        %fields,
    };
    if($ENV{MOD_PERL} and ref($Monitor::Singleton)){
        return $Monitor::Singleton;     }

$self->{start}=time();
$self->{cid}=new CID;    
$self->{orb} = CORBA::ORB_init("orbit-local-orb");
$self->{root_poa} = $self->{orb}->resolve_initial_references("RootPOA");
$self->{root_poa}->_get_the_POAManager->activate;
$self->{mypoamonitor}=new POAMonitor;
my $id = $self->{root_poa}->activate_object($self->{mypoamonitor});
$self->{myref}=$self->{root_poa}->id_to_reference ($id);
$self->{myior} = $self->{orb}->object_to_string ($self->{myref});
$self->{ac}= new ActiveClient($self->{myior},$self->{start},$self->{cid});

foreach my $chop  (@ARGV){
    if($chop =~/^-m/){
        $self->{DataMC}=0;
    }
    if($chop =~/^-d/){
        $self->{updatesviadb}=1;
#amsdatadir

        my $dir=$ENV{AMSataDir};
        if (defined $dir){
            $self->{AMSDataDir}=$dir;
        }
        else{
            $self->{AMSDataDir}="/f0dat1/AMSDataDir";
            $ENV{AMSDataDir}=$self->{AMSDataDir};
        }
        $self->{sqlserver}=new DBSQLServer();
        $self->{sqlserver}->Connect();
#try to get ior
    my $sql="select dbfilename,lastupdate,IORS from Servers where status='Active'";
        my $ret=$self->{sqlserver}->Query($sql);
        my $updlast=0;
        foreach my $upd (@{$ret}){
            if($upd->[1]> $updlast){
                $updlast=$upd->[1];
                $self->{dbfile}=$upd->[0];
                $self->{IOR}=$upd->[2];
            }
        }
    }
}
    my $mc=shift;
    if(defined $mc){
    if($mc =~/^-m/){
        $self->{DataMC}=0;
    }
}




    my $mybless=bless $self,$type;
    if(ref($Monitor::Singleton)){
        croak "Only Single Monitor Allowed\n";
    }
$Monitor::Singleton=$mybless;
return $mybless;
}

sub Connect{
 
 my $ref=shift;
     $ref->{ok}=0;
 if(defined $ref->{IOR}){
     return 1;
 }
 my $ior=>undef;
 if(not defined $ior){ 
  foreach my $chop  (@ARGV){
    if($chop =~/^-I/){
        $ior=unpack("x1 A*",$chop);
    }
    if($chop =~/^-ior/){
            my $iorfile = unpack("x4 A*",$chop);
            my $buf;
            open(FILE,"<".$iorfile) or die "Unable to open file [$iorfile] \n";
            read(FILE,$buf,1638400);
            close FILE;
            $ior=$buf;
#            print "Monitor::Connect -I- IOR read from $iorfile \n";
    }
 }
}
 if(not defined $ior) { 
  $ior=getior($ref);
}
 if( not defined $ior){
  return $ref->{ok};
 } 
 chomp $ior;
 my $tm={};
 try{
  $tm = $ref->{orb}->string_to_object($ior);
  $ref->{arsref}=[$tm];
     $ref->{ok}=1;
}
 catch CORBA::MARSHAL with{
     carp "MARSHAL Error "."\n";
 }
 catch CORBA::COMM_FAILURE with{
     carp "Commfailure Error "."\n";
 }
 catch CORBA::SystemException with{
     carp "SystemException Error "."\n";
 };
 return $ref->{ok};

}


sub ResetHosts{
 my $ref=shift;
      for my $j (0 ... $#{$ref->{ahlp}}){
        my %host=%{${$ref->{ahlp}}[$j]};
        $host{Status}="OK";
        $host{ClientsFailed}=0;
        my $arsref;
        foreach $arsref (@{$ref->{arsref}}){
            try{
                my %cid=%{$ref->{cid}};
                $cid{Type}="Producer";
                $arsref->sendAH(\%cid,\%host,"Update");
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
                my %cid=%{$ref->{cid}};
                $cid{Type}="Producer";
                $arsref->sendAH(\%cid,\%host,"Update");
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }


}
}

sub ForceCheck{
 my $ref=shift;
 my $arsref;
 foreach $arsref (@{$ref->{arsref}}){
     my $length;
     my $ahl;
     try{
         $arsref->SystemCheck();
     }
     catch CORBA::SystemException with{
         print "SystemCheckFailed.\n";
     };
 }
}

sub UpdateServers{
}

sub UpdateEverything{
 my $ref=shift;
 my $arsref;
 foreach $arsref (@{$ref->{arsref}}){
     my $length;
     my $ahl;
     try{
         my %cid=%{$ref->{cid}};
         $cid{Type}="Server";
#       ($length,$ahl)=$arsref->getAHS(\%cid);
         my @qq;
       $length=$arsref->getAHS(\%cid,$ahl);
         if($length==0){
             $ref->{ahls}=undef;
         }
         else {
             $ref->{ahls}=$ahl;
         }
         my $maxc=0;
      ($length,$ahl,$maxc)=$arsref->getACS(\%cid);
         if($length==0){
             $ref->{asl}=undef;
         }
         else {
             $ref->{asl}=$ahl;
             $ref->{asl_maxc}=$maxc;
         }
      ($length,$ahl)=$arsref->getNHS(\%cid);
         if($length==0){
             $ref->{nhl}=undef;
         }
         else {
             $ref->{nhl}=$ahl;
         }
        ($length,$ahl)=$arsref->getNC(\%cid);
         if($length==0){
             $ref->{nsl}=undef;
         }
         else {
             $ref->{nsl}=$ahl;
         }
        ($length,$ahl)=$arsref->getNK(\%cid);
         if($length==0){
             $ref->{nkl}=undef;
         }
         else {
             $ref->{nkl}=$ahl;
         }
        ($length,$ahl)=$arsref->getEnv(\%cid);
         if($length==0){
             $ref->{env}=undef;
         }
         else {
             $ref->{env}=$ahl;
         }
         my $path="AMSDataDir";
         my $addpath="/DataBase/";
         my ($ok, $dhl)=$arsref->getDBSpace(\%cid,$path,$addpath);
         $ref->{db}=$dhl;
         $path="AMSProdOutDir";
         $addpath="/";
          ($ok, $dhl)=$arsref->getDBSpace(\%cid,$path,$addpath);
         $ref->{rn}=$dhl;
         $cid{Type}="Producer";
      ($length,$ahl,$maxc)=$arsref->getACS(\%cid);
         if($length==0){
             $ref->{acl}=undef;
         }
         else {
             $ref->{acl}=$ahl;
             $ref->{asl_maxc}=$maxc;
         }
       ($length,$ahl)=$arsref->getAHS(\%cid);
         if($length==0){
             $ref->{ahlp}=undef;
         }
         else {
             $ref->{ahlp}=$ahl;
         }
        ($length,$ahl)=$arsref->getNC(\%cid);
         if($length==0){
             $ref->{ncl}=undef;
         }
         else {
             $ref->{ncl}=$ahl;
         }
         $cid{Type}="DBServer";
      ($length,$ahl,$maxc)=$arsref->getACS(\%cid);
         if($length==0){
             $ref->{adbsl}=undef;
         }
         else {
             $ref->{adbsl}=$ahl;
             $ref->{adbsl_maxc}=$maxc;
         }
         $cid{Type}="Monitor";
      ($length,$ahl,$maxc)=$arsref->getACS(\%cid);
         if($length==0){
             $ref->{aml}=undef;
         }
         else {
             $ref->{aml}=$ahl;
             $ref->{aml_maxc}=$maxc;
         }
         goto NEXT;
     }
     catch CORBA::SystemException with{
     };
 }
       return 0;
NEXT:
 foreach $arsref (@{$ref->{arpref}}){
     my $length;
     my $ahl;
     try{
         my %cid=%{$ref->{cid}};
         $cid{Type}="Producer";

         my $maxr=0;
        ($length,$ahl)=$arsref->getRunEvInfoS(\%cid, \$maxr);
         if($length==0){
             $ref->{rtb}=undef;
         }
         else {
             $ref->{rtb}=$ahl;
             $ref->{rtb_maxr}=$maxr;
         }


        ($length,$ahl)=$arsref->getDSTInfoS(\%cid);
         if($length==0){
             $ref->{dsti}=undef;
         }
         else {
             $ref->{dsti}=$ahl;
         }
         
        ($length,$ahl)=$arsref->getDSTS(\%cid);
         if($length==0){
             $ref->{dsts}=undef;
         }
         else {
             $ref->{dsts}=$ahl;
         }

         goto LAST;
     }
     catch CORBA::SystemException with{
     };
 }

  return 0; 
LAST:
 foreach $arsref (@{$ref->{ardref}}){
     try{
         my %cid=%{$ref->{cid}};
         $ref->{dbfile}=$arsref->getDBFilePath(\%cid);
         return 1;
     }
     catch CORBA::SystemException with{
     };
 }
 return 1;
}
sub DESTROY{
    my $self=shift;
    warn "DESTROYING $self";
}

sub UpdateARS{ my $ref=shift; my $arsref; if(not $ref->{ok}){ return
 0; } foreach $arsref (@{$ref->{arsref}}){ try{ my
 %cid=%{$ref->{cid}}; $cid{Type}="Server"; my
 ($length,$pars)=$arsref->getARS(\%cid,"Any",0,1); if($length==0 ){
 carp "updars returns zero \n"; return 0; } for(;;){ my $old=shift
 @{$ref->{arsref}}; if(ref($old)){ undef $old; } else{ last; } } my
 $ior; foreach $ior (@$pars){ try{ my
 $newref=$ref->{orb}->string_to_object($ior->{IOR}); if(rand >0.5){
 unshift @{$ref->{arsref}}, $newref; } else{ push @{$ref->{arsref}},
 $newref; } } catch CORBA::SystemException with{ carp "getars 2 oops
 SystemException Error "."\n"; };

         }   
         $cid{Type}="Producer";
         ($length,$pars)=$arsref->getARS(\%cid,"Any",0,1);

         if($length==0 ){
            carp "updars returns zero \n";
            return 0;
        }
        for(;;){
             my $old=shift @{$ref->{arpref}};
          if(ref($old)){
            undef $old;
          }
          else{
            last;
          }
         }
         foreach $ior (@$pars){
             try{
              my $newref=$ref->{orb}->string_to_object($ior->{IOR});
              if(rand >0.5){
#                  warn "unshift"; 
                  unshift @{$ref->{arpref}}, $newref;
              }
              else{
#                  warn "push"; 
                  push @{$ref->{arpref}}, $newref;
              }
             }
             catch CORBA::SystemException with{
               carp "getars 3 oops SystemException Error "."\n";
             };

         }   

         $cid{Type}="DBServer";
         ($length,$pars)=$arsref->getARS(\%cid,"Any",0,1);

         if($length==0 ){
#            carp "updars returns zero \n";
            return 1;
        }
        for(;;){
             my $old=shift @{$ref->{ardref}};
          if(ref($old)){
            undef $old;
          }
          else{
            last;
          }
         }
         foreach $ior (@$pars){
             try{
              my $newref=$ref->{orb}->string_to_object($ior->{IOR});
              if(rand >0.5){
#                  warn "unshift"; 
                  unshift @{$ref->{ardref}}, $newref;
              }
              else{
#                  warn "push"; 
                  push @{$ref->{ardref}}, $newref;
              }
          }
             catch CORBA::SystemException with{
               carp "getars 3 oops SystemException Error "."\n";
             };

         }   

         last;
     }
    catch CORBA::SystemException with{
     carp "getars oops SystemException Error "."\n";
 };
 }
 return 1;
}

sub Update{
    my  $ref=$Monitor::Singleton;
    my $ok=$ref->UpdateARS();
    if($ref->{updatesviadb}){
      $ok=DBServer::InitDBFile(undef,$ref);
      
    }
    else{
     if (not $ok){
        $ref->Connect() or return ($ref, 0);
        $ref->UpdateARS() or return ($ref, 0);
     }
     $ok=$ref->UpdateEverything();
    }
    return ($ref,$ok);
}

sub UpdateForce{
    my  $ref=$Monitor::Singleton;
    my $ok=$ref->UpdateARS();
    $ok=$ref->UpdateEverything();
    return ($ref,$ok);
}

sub Update2{
    warn "Update2Running";
    my $ref=shift;
    if(not ref($ref)){
     $ref=$Monitor::Singleton;
    }
}


sub getior{
#
# new method based on oracle
#
    my $sqls=new DBSQLServer();
    if($sqls->Connect()){
     my $sql="select IORS,IORP,dbfilename from Servers where status='Active' order by lastupdate desc";
     my $ret=$sqls->Query($sql);
     if(defined $ret->[0][0]){
         my $ref=shift;
         $ref->{dbfile}=$ret->[0][2];
         return $ret->[0][0];
     } 
    }    
    warn "  no sql server connect \n";
    my $pid=$Monitor::Singleton->{cid}->{pid};
    my $file ="/tmp/o."."$pid";
    my $fileo ="/tmp/oo."."$pid";
#    my $i=system "/usr/local/lsf/bin/bjobs -q linux_server -u all>$file" ;
    my $i=1;
    if($i){
        unlink $file;
        return getior2();
    }
    open(FILE,"<".$file) or return undef;
    my $ii=0;
    while (<FILE>){
        if (  $_ =~/^\d/){
            if($ii>0 || 1){
            my @args=split ' ';
            $i=system "/usr/local/lsf/bin/bpeek $args[0] >$fileo";
            if($i){
                next;
            }
            open(FILEO,"<".$fileo) or next;
            my $datamc=0;
            while (<FILEO>){
                if (/^DumpIOR/){
                    my $tag=substr($_,8,1);
                    if($tag eq $Monitor::Singleton->{DataMC}){
                        $datamc=1;
                    }
                }  
                if ($datamc and /^IOR:/){
                    close (FILEO);
                    unlink $file,$fileo;
                    return $_;
                }
            }
        }
        $ii++;
        }
    }
    close (FILEO);
    unlink $file,$fileo;
    return getior2();      
}
sub getior2{
    my $file ="/tmp/DumpIOR".".$Monitor::Singleton->{DataMC}";
    open(FILE,"<".$file) or return undef;
            while (<FILE>){
                if (/^IOR:/){
                    close(FILE);
                    return $_;
                }
            }
            close(FILE);
    return undef;
}
sub getdbok{
    my @output=();
    my @text=();
    push @text, $Monitor::Singleton->{db}->{fs};
    push @text, int $Monitor::Singleton->{db}->{dbtotal};
    push @text, int $Monitor::Singleton->{db}->{dbfree};
    push @text, int $Monitor::Singleton->{db}->{dbfree}/($Monitor::Singleton->{db}->{dbtotal}+1.1)*100;
    if($text[1]<0 or $text[2]<0 or $text[2]<200){
      push @text ,1;
  }elsif($text[2]<100){
      push @text ,2;
  }else{
      push @text ,0;
  }
    push @output, [@text];
#     $#text=-1;
#    push @text, $Monitor::Singleton->{rn}->{fs};
#    push @text, int $Monitor::Singleton->{rn}->{dbtotal};
#    push @text, int $Monitor::Singleton->{rn}->{dbfree};
#    push @text, int $Monitor::Singleton->{rn}->{dbfree}/$Monitor::Singleton->{rn}->{dbtotal}*100;
#    if($text[1]<0 or $text[2]<0 or $text[2]<2000){
#      push @text ,1;
#  }elsif($text[2]<1000){
#      push @text ,2;
#  }else{
#      push @text ,0;
#  }
#    push @output, [@text];
    my $i;
    my %fst;
    my %fsf;
     sub prioi{ $a->{Insert} <=> $b->{Insert};}
     my @sortedrtb=sort prioi @{$Monitor::Singleton->{dsts}};
    for $i (0 ... $#sortedrtb){
     my $hash=$sortedrtb[$i];
     my $tring=$hash->{Name};
     my @spar= split /\//,$tring;
     my $string;
     if($#spar<1){
      warn "  problem with  ntuple $tring \n" ;
      next;
     }
     elsif($spar[1]=~/^f/){
      $string="/".$spar[1];
      warn "  $tring $hash->{TotalSpace} \n";
  }
     else{
      $string=$spar[0]."/".$spar[1];
 }
     $fsf{$string}=$hash->{FreeSpace};
     $fst{$string}=$hash->{TotalSpace};
     warn  "$tring $string $hash->{TotalSpace}\n";
    }
    foreach my $string (sort keys %fst){
     $#text=-1;
     
#     warn "  string in rd $string \n";
    push @text, $string;
    push @text, int  $fst{$string}/1024;           
    push @text, int $fsf{$string}/1024;           
    push @text, int $fsf{$string}/($fst{$string}+2)*100; 
    if($text[1]<0){
      $text[1]=4194304+$text[1];
    }
    if($text[2]<0){
      $text[2]=4194304+$text[2];
    }
     
    if($text[1]<0 or $text[2]<0 or $text[2]<2000){
      push @text ,1;
  }elsif($text[2]<1000){
      push @text ,2;
  }else{
      push @text ,0;
  }
    push @output, [@text];
 }

    return @output;
}

sub getactivehosts{
    my @output=();
    my @text=();
    my @final_text=();
     my $total_time=0;
     my $total_cpu=0;
     my $total_ev=0;
     my $totcpu=0;
     my $totproc=0;
    my $i;
    for $i (0 ... $#{$Monitor::Singleton->{ahlp}}){
     $#text=-1;
     my $hash=$Monitor::Singleton->{ahlp}[$i];
  
     my $string=$hash->{HostName};
    push @text, $string;
    push @text, int $hash->{ClientsRunning};           
    push @text, int $hash->{ClientsAllowed};           
    push @text, int $hash->{ClientsFailed};           
     my $ntp=0;
     my $evtag=0;
     my @dummy=split '\.', $string;
     my $host=$dummy[0];
     #print "blia  $host \n";
     if($host =~/pcamsf2/){
      $host="ams";
     }
     for my $j (0 ... $#{$Monitor::Singleton->{dsts}}){
     my $hdst=$Monitor::Singleton->{dsts}[$j];
      @dummy=split '\:', $hdst->{Name};
     my $chop=$dummy[0];
                     $chop=~/^(.*?)(\.|$)/;
                         if($1 eq $host ){
#      if ( $chop eq $host){
          if( $hdst->{Type} eq "Ntuple" or $hdst->{Type} eq "RootFile"){
              $ntp++;
          }elsif( $hdst->{Type} eq "EventTag"){
              $evtag++;
          }
      }       
    }   
     push @text, $ntp;
     push @text, $evtag;
     my $evt=0;
     my $lastevt=0;
     my $err=0;
     my $cerr=0;
     my $cpu=0;
     my $cpup=0;
     my $time=0;
     my $timep=0;
     my $total=0;
     my $tevt=0;
     for my $j (0 ... $#{$Monitor::Singleton->{rtb}}){
       my $rdst=$Monitor::Singleton->{rtb}[$j];
       $total+=$rdst->{LastEvent}+1-$rdst->{FirstEvent};
       my $rdstc=$rdst->{cinfo};
       if($rdst->{Status} eq "Failed" and (not ($rdst->{FilePath} =~/laser/))){
          warn "  Run $rdst->{Run}  $rdst->{FilePath} \n";
       }
       if( $rdst->{Status} eq "Finished" or $rdst->{Status} eq "Processing"){
           $lastevt+=$rdstc->{LastEventProcessed}+1-$rdst->{FirstEvent};
           $tevt+=$rdstc->{EventsProcessed};
                     $rdstc->{HostName}=~/^(.*?)(\.|$)/;
                         if(($1 eq $host) ){
#           if( $rdstc->{HostName} eq $host){
             print " pidarasy $rdst->{Run}   Host $rdstc->{HostName}  $rdstc->{EventsProcessed} $rdstc->{CPUTimeSpent} $rdstc->{CPUMipsTimeSpent} $rdstc->{Mips} \n";
               $evt+=$rdstc->{EventsProcessed};
               $cerr+=$rdstc->{CriticalErrorsFound};
               $err+=$rdstc->{ErrorsFound};
               $cpu+=$rdstc->{CPUTimeSpent};
               $time+=$rdstc->{TimeSpent};
               if($rdst->{Status} eq "Processing"){
                 $cpup+=$rdstc->{CPUTimeSpent};
                 $timep+=$rdstc->{TimeSpent};
               }
           }
       }
   }
#     print "$total $host $lastevt \n";
   push @text, $evt; 
   push @text, int(1000*$lastevt/($total+1)*$evt/($tevt+1))/10.; 
   push @text, $err,$cerr; 
     $total_cpu+=$cpu;
     $total_time+=$time;
     $total_ev+=$evt;
     my $proc=$hash->{ClientsRunning}==0?1:$hash->{ClientsRunning};
   my $cpuper=int ($cpu*1000/($evt+1)/$proc);
   
     my $cpunumber=1;
    for my $nhlcycle (0 ...$#{$Monitor::Singleton->{nhl}}){
      my $hashnhl=$Monitor::Singleton->{nhl}[$nhlcycle];
      if($hash->{HostName} eq $hashnhl->{HostName}){
          $cpunumber=$hashnhl->{CPUNumber};
          last;
      }
  }
    my $effic=$time==0?0:int ($cpu*100*$proc/$cpunumber/($time)); 
     $totproc+=$proc;
     $totcpu+=$cpunumber;
  push @text, $cpuper/1000., $effic/100.;
     push @text, $hash->{Status};
    if( $hash->{Status} eq "LastClientFailed" or $hash->{Status} eq "InActive"){
      push @text ,1;
  }elsif( $hash->{Status} eq "NoResponse"){
      push @text ,2;
  }else{
      push @text ,0;
  }
     if($i==0){
         @final_text=@text;
         $final_text[0]="Total";
         $final_text[11]="  ";
         $final_text[12]=0;
     }
     else{
        for my $i (1 ...9){ 
         $final_text[$i]+=$text[$i];
        }
     }
    push @output, [@text];
 }
    my $total_pr=$final_text[2]==0?1:$final_text[2];
   my $cpuper=int ($total_cpu*1000/($total_ev+1)/$total_pr);
   $final_text[10]= $cpuper/1000.;
   $final_text[11]= int($total_cpu/($totcpu+0.001)*$totproc/($total_time+0.001)*100)/100.;
    
    push @output, [@final_text];


    return @output;
}
sub getactiveclients{
    my $producer=shift;
    if(ref($producer)){
       $producer=shift;
    }
    my @output=();
    my @text=();
    my $hash;
    my $xmax;
    if($producer eq "Producer"){
        $xmax=$#{$Monitor::Singleton->{acl}};
 }elsif($producer eq "Server"){
        $xmax=$#{$Monitor::Singleton->{asl}};
 }elsif($producer eq "DBServer"){
        $xmax=$#{$Monitor::Singleton->{adbsl}};
 }else{
        $xmax=$#{$Monitor::Singleton->{aml}};
 }
    for my $i (0 ... $xmax){
        $#text=-1;
    if($producer eq "Producer"){
        $hash=$Monitor::Singleton->{acl}[$i];
 }elsif($producer eq "Server"){
        $hash=$Monitor::Singleton->{asl}[$i];
 }elsif($producer eq "DBServer"){
        $hash=$Monitor::Singleton->{adbsl}[$i];
 }else{
        $hash=$Monitor::Singleton->{aml}[$i];
 }
        push @text, $hash->{id}->{uid};
        push @text, $hash->{id}->{HostName};
        push @text, int($hash->{id}->{Mips});
        push @text, $hash->{id}->{pid};
        my $time=localtime($hash->{Start});
        push @text, $time;
        my $dt=time()-$hash->{LastUpdate};
        my $dts=$dt." sec ago";
#        $time=localtime($hash->{LastUpdate});
        push @text, $dts;
        my $timeout=$hash->{TimeOut};
        push @text, $timeout;
if ($producer eq "Producer"){
        my $run=-1;
     for my $j (0 ... $#{$Monitor::Singleton->{rtb}}){
       my $rdst=$Monitor::Singleton->{rtb}[$j];
       my $rdstc=$rdst->{cinfo};
       if( $rdst->{Status} eq "Processing"){
           if ($rdst->{cuid}==$hash->{id}->{uid}){
               $run=$rdst->{Run};
#               warn "run ... $run \n";
               last;
           }
       }
   }
        my $ntuple=" // ";
        my $lastev=0;
        my $ltime=0;
     for my $j (0 ... $#{$Monitor::Singleton->{dsts}}){
       my $rdst=$Monitor::Singleton->{dsts}[$j];
       if( $rdst->{Type} eq "Ntuple" or $rdst->{Type} eq "RootFile"){
#           warn "wasrun $run $rdst->{Run} \n";
           if ($rdst->{Run}==$run and $rdst->{Status} eq "InProgress"){
               my @host=split ":",$rdst->{Name};
               my @fh=split '\.',$host[0];
               if($host[0] eq $hash->{id}->{HostName} or
                  ($fh[1] eq 'om' and ($hash->{id}->{HostName} =~ /$fh[0]/)) or                   ($host[0] =~/ams/ and $hash->{id}->{HostName} =~ /pcamsf2/)){
#               warn "runfound $rdst->{FirstEvent} $lastev $rdst->{Name} \n";
               if($rdst->{Insert}>$ltime){
                   $ltime=$rdst->{Insert};
                   $ntuple=$rdst->{Name};
               }
             }
           }
       }
   }
        push @text, $run;
        my @dummy=split "/",$ntuple;
        push @text, $dummy[$#dummy];
        push @text, $hash->{Status};
        if ($hash->{Status} eq "Registered" or $hash->{Status} eq "Active"){
            if($ltime>0){
             push @text, 0;
            }
            else{
             push @text, 1;
            }   
     }elsif( $hash->{Status} eq "TimeOut" or $hash->{Status} eq "Submitted"){
         push @text, 1;
     }else{
         push @text, 2;
     }


    }
    else{
        push @text, $hash->{Status};
        if ($hash->{Status} eq "Registered" or $hash->{Status} eq "Active"){
         push @text, 0;
     }elsif( $hash->{Status} eq "TimeOut" or $hash->{Status} eq "Submitted"){
         push @text, 1;
     }else{
         push @text, 2;
     }
    }
    push @output, [@text];
}
    sub prio1 {$a->[0] <=> $b->[0]};
    my @sortedoutput= sort prio1 @output;
    return @sortedoutput;
 #   return @output;

}


sub getruns{
     sub prio{ $a->{Run} <=> $b->{Run};}
     my @sortedrtb=sort prio @{$Monitor::Singleton->{rtb}};
    my @output=();
    my @text=();
    my @final_text=();
     my $total_cpu=0;
     my $total_ev=0;
    my @sort=( "Failed","Processing", "Finished","Allocated", "Foreign","ToBeRerun");
    for my $j (0 ... $#sort){
    for my $i (0 ... $#sortedrtb){
     $#text=-1;
     my $hash=$sortedrtb[$i];
     my $order;
     if($hash->{History} eq "Failed"){
         $order=$hash->{History};
     }
     else{
      $order=$hash->{Status} ;
     }
     if ($order eq $sort[$j]){
     my $ctime=localtime($hash->{SubmitTime});
     push @text, $hash->{Run},$ctime,$hash->{FirstEvent},$hash->{LastEvent},$hash->{Priority},$hash->{History},$hash->{CounterFail},$hash->{Status};
     if ($hash->{Status} eq "Failed" and $hash->{History} eq "Failed"){
         push @text, 2;
     }elsif($hash->{History} eq "Failed" and $hash->{Status} ne "Finished"){
         push @text, 1;
     }else{
         push @text, 0;
     }
     push @output, [@text];
 }
 }
}
    return @output;

}

sub getntuples{
     sub prion{ $a->{Run} <=> $b->{Run};}
     my @sorteddst=sort prion @{$Monitor::Singleton->{dsts}};
    my @output=();
    my @text=();
    my @final_text=();
     my $total_cpu=0;
     my $total_ev=0;
    my @sort=("Failure","InProgress","Success","Validated");
    for my $j (0 ... $#sort){
    for my $i (0 ... $#sorteddst){
     $#text=-1;
     my $hash=$sorteddst[$i];
     if($hash->{Type} eq "Ntuple" or $hash->{Type} eq "RootFile"){
         if($hash->{Status} eq $sort[$j]){
     my $ctime=localtime($hash->{Insert});
     my $smartsize=$hash->{size};
     if($smartsize>2000){
        $smartsize=int($smartsize/1024/1024+0.5);
    }
     push @text, $hash->{Run},$ctime,$hash->{FirstEvent},$hash->{LastEvent},$hash->{Name},$hash->{crc},$smartsize,$hash->{Status};
         my $dt=time()-$hash->{Insert};
     if ($hash->{Status} eq "InProgress"){
         if($dt>3600*12 && $dt<3600*24){
             push @text,2;
         }elsif($dt>3600*6 && $dt<3600*24){
             push @text,1;
         }
         else{
             push @text,0;
         }
     }elsif($hash->{Status} eq "Failure" && $dt<3600*24){
         push @text, 1;
     }else{
         push @text, 0;
     }

     push @output, [@text];
 }
 }
 }
}
    return @output;

}


sub getcontrolthings{
    my $name=shift;
    my @output=();
    my @text=();
    if( $name eq "ServerClient"){    
     for my $i (0 ... $#{$Monitor::Singleton->{nsl}}){
         $#text=-1;
         my $hash=$Monitor::Singleton->{nsl}[$i];
         
         push @text,  $hash->{uid},$hash->{Type},$hash->{MaxClients}, 
int($hash->{CPUNeeded}*10)/10,
        $hash->{MemoryNeeded}, $hash->{WholeScriptPath}, $hash->{LogPath}, 
         $hash->{SubmitCommand}, $hash->{HostName}, $hash->{LogInTheEnd},$hash;
         push @output, [@text];   
     }
    }elsif( $name eq "ProducerClient"){           
     for my $i (0 ... $#{$Monitor::Singleton->{ncl}}){
         $#text=-1;
         my $hash=$Monitor::Singleton->{ncl}[$i];
         
         push @text,  $hash->{uid},$hash->{Type},$hash->{MaxClients}, 
int($hash->{CPUNeeded}*10)/10,
        $hash->{MemoryNeeded}, $hash->{WholeScriptPath}, $hash->{LogPath}, 
         $hash->{SubmitCommand}, $hash->{HostName}, $hash->{LogInTheEnd},$hash;
         push @output, [@text];   
     }
    }elsif( $name eq "ProducerActiveClient"){           
     for my $i (0 ... $#{$Monitor::Singleton->{acl}}){
         $#text=-1;
         my $hash=$Monitor::Singleton->{acl}[$i];
         push @text, $hash->{id}->{uid};
         push @text, $hash->{id}->{HostName};
         push @text, int($hash->{id}->{Mips});
         push @text, $hash->{id}->{pid};
         push @text, $hash->{Status};
         push @text, $hash->{StatusType};
         push @text, $hash->{TimeOut};
         push @output, [@text];   
     }
    }elsif( $name eq "ServerHost"){       
     for my $i (0 ... $#{$Monitor::Singleton->{nhl}}){
         $#text=-1;
         my $hash=$Monitor::Singleton->{nhl}[$i];
         
         push @text,  $hash->{HostName},$hash->{Interface},$hash->{OS}, 
         $hash->{CPUNumber}, $hash->{Memory}, $hash->{Clock};
#        find status;
                
     for my $j (0 ... $#{$Monitor::Singleton->{ahls}}){
         my $ahl=$Monitor::Singleton->{ahls}[$j];
         if( $ahl->{HostName} eq $hash->{HostName}){
            push @text,$ahl->{ClientsAllowed}; 
            push @text,$ahl->{Status}; 
            goto FOUND1;
         }
     }
         push @text,0;
         push @text,"NotFound";
     FOUND1:    
         push @output, [@text];   
     }
    }elsif( $name eq "ProducerHost"){        
     for my $i (0 ... $#{$Monitor::Singleton->{nhl}}){
         $#text=-1;
         my $hash=$Monitor::Singleton->{nhl}[$i];
         
         push @text,  $hash->{HostName},$hash->{Interface},$hash->{OS}, 
         $hash->{CPUNumber}, $hash->{Memory};
#        find status;
                
     for my $j (0 ... $#{$Monitor::Singleton->{ahlp}}){
         my $ahl=$Monitor::Singleton->{ahlp}[$j];
         if( $ahl->{HostName} eq $hash->{HostName}){
            push @text,$ahl->{Clock}; 
            push @text,$ahl->{ClientsAllowed}; 
            push @text,$ahl->{ClientsFailed}; 
            push @text,$ahl->{Status}; 
            goto FOUND;
         }
     }
         push @text, $hash->{Clock};
         push @text,0;
         push @text,0;
         push @text,"NotFound";
     FOUND:    
         push @output, [@text];   
     }
    }elsif( $name eq "Ntuple"){        
     for my $i (0 ... $#{$Monitor::Singleton->{dsti}}){
         $#text=-1;
         my $hash=$Monitor::Singleton->{dsti}[$i];
         
         push @text, $hash->{uid}, $hash->{HostName},$hash->{OutputDirPath},$hash->{Mode},$hash->{UpdateFreq},$hash->{type};
         push @output, [@text];   
     }
    }elsif( $name eq "PNtuple"){        
     my @houtput=();
     for my $i (0 ... $#{$Monitor::Singleton->{dsts}}){
         my $hash=$Monitor::Singleton->{dsts}[$i];
         if($hash->{Type} eq "Ntuple" or $hash->{Type} eq "RootFile"){
          push @houtput, $hash;
         }
     }
sub PNtupleSort{
    if( $a->{Status} ne $b->{Status}){
        return $a->{Status} cmp $b->{Status};
    }
    else{
        return $a->{Run}<=>$b->{Run};
    }
}

         my @sortedoutput=sort PNtupleSort @houtput;
         for my $i (0 ... $#sortedoutput){
          $#text=-1;
          my $hash=$sortedoutput[$i];
     my $smartsize=$hash->{size};
     if($smartsize>2000){
        $smartsize=int($smartsize/1024/1024+0.5);
    }
          push @text, $hash->{Run}, $hash->{Insert},$hash->{FirstEvent},$hash->{LastEvent},$hash->{Name},$smartsize,$hash->{Status},$hash->{Type};
          push @output, [@text];   
      }
    }elsif( $name eq "Run"){        
     for my $i (0 ... $#{$Monitor::Singleton->{rtb}}){
         $#text=-1;
         my $hash=$Monitor::Singleton->{rtb}[$i];
         my $st="Undefined";
         if($hash->{cuid} ne 0){
         for my $j (0 ... $#{$Monitor::Singleton->{acl}}){
          my $hasha=$Monitor::Singleton->{acl}[$j];
           
          if( $hasha->{id}->{uid} eq $hash->{cuid}){
              $st=$hasha->{StatusType};
              last;
          }
          }
         }
         push @text,  $hash->{uid},$hash->{cuid},$hash->{Run},$hash->{FirstEvent}, 
        $hash->{LastEvent}, $hash->{Priority}, $hash->{FilePath}, 
         $hash->{Status}, $hash->{History},$st,$hash->{CounterFail},;
         push @output, [@text];   
     }
    }elsif( $name eq "Killer"){        
     for my $i (0 ... $#{$Monitor::Singleton->{nkl}}){
         $#text=-1;
         my $hash=$Monitor::Singleton->{nkl}[$i];
         
         push @text,  $hash->{uid},$hash->{Type},$hash->{MaxClients}, 
int($hash->{CPUNeeded}*10)/10,
        $hash->{MemoryNeeded}, $hash->{WholeScriptPath}, $hash->{LogPath}, 
         $hash->{SubmitCommand}, $hash->{HostName}, $hash->{LogInTheEnd},$hash;
         push @output, [@text];   
     }
 }elsif( $name eq "setEnv"){
     for my $i (0 ... $#{$Monitor::Singleton->{env}}){
         $#text=-1;
         my $hash=$Monitor::Singleton->{env}[$i];
         my @text = split '\=',$hash;
         push @output, [@text];   
     }
 }
    return @output;

}

sub sendback{
    my ($name, $action, $row,@data) =@_;
                my $safeact="Update";
                if($action eq "Delete"){
                  $safeact=$action;
              }
    if($name eq "ServerClient"){
        my $ref=$Monitor::Singleton;
        my %nc=%{${$ref->{nsl}}[$row]};
        $nc{uid}=shift @data;
        $nc{Type}=shift @data;
        $nc{MaxClients}=shift @data;
        $nc{CPUNeeded}=shift @data;
        $nc{MemoryNeeded}=shift @data;
        $nc{WholeScriptPath}=shift @data;
        $nc{LogPath}=shift @data;
        $nc{SubmitCommand}=shift @data;
        $nc{HostName}=shift @data;
        $nc{LogInTheEnd}=shift @data;
        my $arsref;
        foreach $arsref (@{$ref->{arsref}}){
            try{
                my %cid=%{$ref->{cid}};
                $cid{Type}=$nc{Type};
                $arsref->sendNC(\%cid,\%nc,$action);
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
                my %cid=%{$ref->{cid}};
                $cid{Type}=$nc{Type};
                $arsref->sendNC(\%cid,\%nc,$action);
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
   }elsif($name eq "ProducerClient"){
        my $ref=$Monitor::Singleton;
        my %nc=%{${$ref->{ncl}}[$row]};
        $nc{uid}=shift @data;
        $nc{Type}=shift @data;
        $nc{MaxClients}=shift @data;
        $nc{CPUNeeded}=shift @data;
        $nc{MemoryNeeded}=shift @data;
        $nc{WholeScriptPath}=shift @data;
        $nc{LogPath}=shift @data;
        $nc{SubmitCommand}=shift @data;
        $nc{HostName}=shift @data;
        $nc{LogInTheEnd}=shift @data;
        my $arsref;
        foreach $arsref (@{$ref->{arsref}}){
            try{
                my %cid=%{$ref->{cid}};
                $cid{Type}=$nc{Type};
                $arsref->sendNC(\%cid,\%nc,$action);
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
                my %cid=%{$ref->{cid}};
                $cid{Type}=$nc{Type};
                $arsref->sendNC(\%cid,\%nc,$action);
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
   }elsif($name eq "ProducerActiveClient"){
        my $ref=$Monitor::Singleton;
        my %ac=%{${$ref->{acl}}[$row]};
        shift @data;
        shift @data;
        shift @data;
        shift @data;
        shift @data;
        $ac{StatusType}=shift @data;
        $ac{TimeOut}=shift @data;
        my $arsref;
        foreach $arsref (@{$ref->{arsref}}){
            try{
                my $hash_ac=\%ac;
                my %cid=%{$ac{id}};
                $arsref->sendAC(\%cid,\$hash_ac,$safeact);
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
                my %cid=%{$ac{id}};
                my $hash=\%cid;
                my $hash_ac=\%ac;
            try{
                $arsref->sendACPerl($hash,$hash_ac,$safeact);
#                $arsref->sendAC($hash,\$hash_ac,$safeact);
            }  catch DPS::DBProblem   with{
                my $e=shift;
                warn "DBProblem: $e->{message}\n";
            }
            catch CORBA::SystemException with{
                 $arsref->sendAC($hash,\$hash_ac,$safeact);
#                 $arsref->sendACPerl($hash,$hash_ac,$safeact);
            };
                last;
            }
            catch DPS::DBProblem   with{
                my $e=shift;
                warn "DBProblem: $e->{message}\n";
            }
            catch CORBA::SystemException with{
                warn "Exiting corba exc";
            };
}

    }elsif($name eq "Killer"){
        my $ref=$Monitor::Singleton;
        my %nc=%{${$ref->{nkl}}[$row]};
        $nc{uid}=shift @data;
        $nc{Type}=shift @data;
        $nc{MaxClients}=shift @data;
        $nc{CPUNeeded}=shift @data;
        $nc{MemoryNeeded}=shift @data;
        $nc{WholeScriptPath}=shift @data;
        $nc{LogPath}=shift @data;
        $nc{SubmitCommand}=shift @data;
        $nc{HostName}=shift @data;
        $nc{LogInTheEnd}=shift @data;
        my $arsref;
        foreach $arsref (@{$ref->{arsref}}){
            try{
                my %cid=%{$ref->{cid}};
                $cid{Type}=$nc{Type};
                $arsref->sendNK(\%cid,\%nc,$action);
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
                my %cid=%{$ref->{cid}};
                $cid{Type}=$nc{Type};
                $arsref->sendNK(\%cid,\%nc,$action);
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
    }elsif($name eq "Ntuple"){
        my $ref=$Monitor::Singleton;
        my %nc=%{${$ref->{dsti}}[$row]};
        $nc{uid}=shift @data;
        $nc{HostName}=shift @data;
        $nc{OutputDirPath}=shift @data;
        $nc{Mode}=shift @data;
        $nc{UpdateFreq}=shift @data;
        $nc{type}=shift @data;
        my $arsref;
        foreach $arsref (@{$ref->{arpref}}){
            try{
                $arsref->sendDSTInfo(\%nc,$action);
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
#        foreach $arsref (@{$ref->{ardref}}){
#            try{
#                $arsref->sendDSTInfo(\%nc,$action);
#                last;
#            }
#            catch CORBA::SystemException with{
#                warn "sendback corba exc";
#            };
#        }
    }elsif($name eq "PNtuple"){
        my $ref=$Monitor::Singleton;
        my %nc=%{${$ref->{dsts}}[$row]};
        $nc{Run}=shift @data;
        $nc{Insert}=shift @data;
        $nc{FirstEvent}=shift @data;
        $nc{LastEvent}=shift @data;
        $nc{Name}=shift @data;
        $nc{size}=shift @data;
        $nc{Status}=shift @data;
        $nc{Type}=shift  @data;
        my $arsref;
                my %cid=%{$ref->{cid}};
        foreach $arsref (@{$ref->{arpref}}){
            try{
                $arsref->sendDSTEnd(\%cid,\%nc,$action);
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
warn " sent via arpref\n";
#        foreach $arsref (@{$ref->{ardref}}){
#            try{
#                $arsref->sendDSTEnd(\%cid,\%nc,$action);
#                last;
#            }
#            catch CORBA::SystemException with{
#                warn "sendback corba exc";
#            };
#        }
    }elsif($name eq "Run"){
        my $ref=$Monitor::Singleton;
        my %nc=%{${$ref->{rtb}}[$row]};
        $nc{uid}=shift @data;
        $nc{cuid}=shift @data;
        $nc{Run}=shift @data;
        $nc{FirstEvent}=shift @data;
        $nc{LastEvent}=shift @data;
        $nc{Priority}=shift @data;
        $nc{FilePath}=shift @data;
        $nc{Status}=shift @data;
        $nc{History}=shift @data;
        shift @data;
        $nc{CounterFail}=shift @data;
        if($nc{History} eq "ToBeRerun"){
         my $rdstc=$nc{cinfo};
         $rdstc->{HostName}="      ";
        }
        my $arsref;
        foreach $arsref (@{$ref->{arpref}}){
            try{
                $arsref->sendRunEvInfo(\%nc,$action);
            }
            catch CORBA::SystemException with{
                warn "sendback p corba exc";
            };
}
        foreach $arsref (@{$ref->{ardref}}){
            try{
                $arsref->sendRunEvInfo(\%nc,$action);
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
    }elsif($name eq "ServerHost"){
        my $ref=$Monitor::Singleton;
        my %nc=%{${$ref->{nhl}}[$row]};
        $nc{HostName}=shift @data;
        $nc{Interface}=shift @data;
        $nc{OS}=shift @data;
        $nc{CPUNumber}=shift @data;
        $nc{Memory}=shift @data;
        $nc{Clock}=shift @data;
#find ahl
my %ac;
      for my $j (0 ... $#{$Monitor::Singleton->{ahls}}){
         my $ahl=$Monitor::Singleton->{ahls}[$j];
         if( $ahl->{HostName} eq $nc{HostName}){
          %ac=%{${$ref->{ahls}}[$j]};
          $ac{ClientsAllowed}=shift @data;
          $ac{Status}=shift @data;
          $ac{Clock}=$nc{Clock};
          $ac{Interface}=$nc{Interface};
          goto FOUND2;
         }
      }
           %ac=%{${$ref->{ahls}}[0]};
          $ac{ClientsAllowed}=shift @data;
          $ac{Status}=shift @data;
          $ac{HostName}=$nc{HostName};
          $ac{ClientsRunning}=0;
          $ac{ClientsProcessed}=0;
          $ac{ClientsFailed}=0;
          $ac{ClientsKilled}=0;
          $ac{Clock}=$nc{Clock};
          $ac{Interface}=$nc{Interface};
FOUND2:
        my $arsref;
        foreach $arsref (@{$ref->{arsref}}){
            try{
                my %cid=%{$ref->{cid}};
                $cid{Type}="Server";
                $arsref->sendNH(\%cid,\%nc,$action);
                $arsref->sendAH(\%cid,\%ac,$action);
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
                my %cid=%{$ref->{cid}};
                $cid{Type}="Server";
                $arsref->sendNH(\%cid,\%nc,$action);
                $arsref->sendAH(\%cid,\%ac,$action);
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
    }elsif($name eq "ProducerHost"){
        my $ref=$Monitor::Singleton;
        my %nc=%{${$ref->{nhl}}[$row]};
        $nc{HostName}=shift @data;
        $nc{Interface}=shift @data;
        $nc{OS}=shift @data;
        $nc{CPUNumber}=shift @data;
        $nc{Memory}=shift @data;
        $nc{Clock}=shift @data;
#find ahl
my %ac;
      for my $j (0 ... $#{$Monitor::Singleton->{ahlp}}){
         my $ahl=$Monitor::Singleton->{ahlp}[$j];
         if( $ahl->{HostName} eq $nc{HostName}){
          %ac=%{${$ref->{ahlp}}[$j]};
          $ac{ClientsAllowed}=shift @data;
          $ac{ClientsFailed}=shift @data;
          $ac{Status}=shift @data;
          $ac{Clock}=$nc{Clock};
          $ac{Interface}=$nc{Interface};
          goto FOUND3;
         }
        }
           %ac=%{${$ref->{ahlp}}[0]};
          $ac{ClientsAllowed}=shift @data;
          $ac{ClientsFailed}=shift @data;
          $ac{Status}=shift @data;
          $ac{HostName}=$nc{HostName};
          $ac{ClientsRunning}=0;
          $ac{ClientsProcessed}=0;
          $ac{ClientsFailed}=0;
          $ac{ClientsKilled}=0;
          $ac{Clock}=$nc{Clock};
          $ac{Interface}=$nc{Interface};
          $ac{LastFailed}=0;
          $ac{LastUpdate}=time();
FOUND3:
        my $arsref;
        foreach $arsref (@{$ref->{arsref}}){
            try{
                my %cid=%{$ref->{cid}};
                $cid{Type}="Server";
                $arsref->sendNH(\%cid,\%nc,$action);
                $cid{Type}="Producer";
                $arsref->sendAH(\%cid,\%ac,$action);
            }
            catch CORBA::SystemException with{
                warn "sendback p corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
                my %cid=%{$ref->{cid}};
                $cid{Type}="Server";
                $arsref->sendNH(\%cid,\%nc,$action);
                $cid{Type}="Producer";
                $arsref->sendAH(\%cid,\%ac,$action);
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback d corba exc";
            };
        }
      }elsif($name eq "setEnv"){
        my $ref=$Monitor::Singleton;
                my $env=shift @data;
                my $path=shift @data;
                foreach my $arsref (@{$ref->{arsref}}){
                try{
                 my %cid=%{$ref->{cid}};
                 $cid{Type}="Server";
                 $arsref->setEnv(\%cid,$env,$path);
                }
                catch CORBA::SystemException with{
                 warn "sendback corba exc";
                };
            }
      }
}


sub Exiting{
    my $ref=shift;
    if(defined $ref->{IOR}){
        $ref->ErrorPlus(shift);        
    }
    else{
        my $arsref;
        foreach $arsref (@{$ref->{arsref}}){
            try{
                my %cid=%{$ref->{cid}};
                $arsref->Exiting(\%cid,"Exiting ","CInExit");
                return;
            }
            catch CORBA::SystemException with{
                warn "Exiting corba exc";
            };
        }
    } 
}


sub SendId{

    use Error qw(:try);
    my $ref=$Monitor::Singleton;
        my $arsref;
        foreach $arsref (@{$ref->{arsref}}){
            try{
                my %cid=%{$ref->{cid}};
                my $hash=\%cid;
                my ($ok,$ok2)=$arsref->sendId(\$hash,2000,600);
                $ref->{cid}=$hash;
                $ref->{ac}->{id}=$hash;
                my %ac=%{$ref->{ac}};
                my $hash_ac=\%ac;
                $arsref->sendAC($hash,\$hash_ac,"Update");
                $ref->{ac}=$hash_ac;
            }
            catch CORBA::SystemException with{
                warn "Exiting corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
                my %cid=%{$ref->{cid}};
                my $hash=\%cid;
                my %ac=%{$ref->{ac}};
                my $hash_ac=\%ac;
            try{
                 $arsref->sendACPerl($hash,$hash_ac,"Create");
#                $arsref->sendAC($hash,\$hash_ac,"Create");
            }            
            catch DPS::DBProblem   with{
                my $e=shift;
                warn "DBProblem: $e->{message}\n";
            }
            catch CORBA::SystemException with{
                 $arsref->sendACPerl($hash,$hash_ac,"Create");
            };
                $ref->{ac}=$hash_ac;
                last;
            }
            catch CORBA::SystemException with{
                warn "Exiting corba exc";
            };
        }
}


sub ErrorPlus{
   my $ref=shift;
   die shift;
}

sub RemoveRuns{
 my $ref=shift;


      for my $j (0 ... $#{$ref->{rtb}}){
        my %rdst=%{${$ref->{rtb}}[$j]};
     if($rdst{Status} eq "Canceled"){
        my $arsref;
        foreach $arsref (@{$ref->{arpref}}){
            try{
                $arsref->sendRunEvInfo(\%rdst,"Delete");
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
                $arsref->sendRunEvInfo(\%rdst,"Delete");
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }


    }
}
}





sub ResetFailedRuns{
 my $ref=shift;


      for my $j (0 ... $#{$ref->{rtb}}){
        my %rdst=%{${$ref->{rtb}}[$j]};

 if($rdst{Status} eq "Processing"){
     my $found=0;
     foreach my $ac (@{$ref->{acl}}){
         if($ac->{id}->{uid} eq $rdst{cuid}){
             $found=1;
             last;
         }
     }
     if($found eq 0){
         $rdst{Status} ="Failed";
         $rdst{History}="Failed";
     }
 }
       if($rdst{Status} eq "Failed"){
         $rdst{Status}="ToBeRerun";
         if($rdst{Run}>(1<<26)){
          $rdst{Status}="Foreign";
          }
     }
       if($rdst{Status} eq "Allocated"){
      
         $rdst{Status}="Finished";
     }

        my $arsref;
        foreach $arsref (@{$ref->{arpref}}){
            try{
                $arsref->sendRunEvInfo(\%rdst,"Update");
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
                $arsref->sendRunEvInfo(\%rdst,"Update");
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        
        }



}

    for my $i (0 ... $#{$Monitor::Singleton->{dsts}}){
     my %nc=%{$Monitor::Singleton->{dsts}[$i]};
     if($nc{Type} eq "Ntuple" or $nc{Type} eq "RootFile"){
         if($nc{Status} eq "InProgress"){
             my $Run=$nc{Run};
             for my $j (0 ... $#{$ref->{rtb}}){
              my %rdst=%{${$ref->{rtb}}[$j]};
              if($Run eq $rdst{Run} and $rdst{Status} ne "Processing"){
                  $nc{Status}="Failure";
                 my $arsref;
                my %cid=%{$ref->{cid}};
                foreach $arsref (@{$ref->{arpref}}){
                 try{
                  $arsref->sendDSTEnd(\%cid,\%nc,"Update");
                 }
                catch CORBA::SystemException with{
                 warn "sendback corba exc";
                };
                }
                foreach $arsref (@{$ref->{ardref}}){
                 try{
                   $arsref->sendDSTEnd(\%cid,\%nc,"Update");
                   last;
                 }
                catch CORBA::SystemException with{
                 warn "sendback corba exc";
                };
              }
              }
             }
         }
     }
}




}


sub RestoreRuns{

 my $ref=shift;


#
# case than runs disappered from run table on server
#

#
# kill double runs
#
      my %rdst; 
      for my $j (0 ... $#{$ref->{rtb}}){
        %rdst=%{${$ref->{rtb}}[$j]};
#         warn " run $rdst{Run} \n";
        my %rdst2; 
       for my $k ($j+1...$#{$ref->{rtb}}){
        %rdst2=%{${$ref->{rtb}}[$k]};
      if( $rdst{Run} eq $rdst2{Run}){
          warn "  Deleting $rdst{Run} \n";
        my $arsref;
        foreach $arsref (@{$ref->{arpref}}){
            try{
                $arsref->sendRunEvInfo(\%rdst2,"Delete");
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
                $arsref->sendRunEvInfo(\%rdst2,"Delete");
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }

       
    }
  }
}
#return;





 my $dir;
# get amsdatadir
     for my $i (0 ... $#{$Monitor::Singleton->{env}}){
         my $hash=$Monitor::Singleton->{env}[$i];
         my @text = split '\=',$hash;
         if($text[0]=~/^AMSDataDir/){
           warn "  AMSDataDir $text[1] \n";
           $dir=$text[1]."/prod.log/scripts/";
           last;
         }
     }

# get client list
 my $maxrun=0;
 for my $i (0... $#{$ref->{acl}}){
     my %client=%{${$ref->{acl}}[$i]};
     my %cid=%{$client{id}};
     my $id=$cid{uid};
     if($id<1000000){
      my %rdst; 
      for my $j (0 ... $#{$ref->{rtb}}){
        %rdst=%{${$ref->{rtb}}[$j]};
      if ($rdst{uid}>$maxrun){
          $maxrun=$rdst{uid};
      }
     if($rdst{Run} eq $id){
         warn "  Found $id \n";
         #goto FoundRun;
         next;


    }
}
warn " Run $id NotFound !!!!!!\n";

opendir THISDIR ,$dir or die "unable to open $dir";
my @allfiles= readdir THISDIR;
closedir THISDIR;
foreach my $file (@allfiles){
    if ($file =~/^cern\.$id/){
        warn " found file for $id \n";
        my $full=$dir.$file;
        open(FILE,"<".$full) or die "Unable to open file $full\n";
        my $buf;
        read(FILE,$buf,16384000) or next;
        close FILE;
        $rdst{Priority}=0;
        $rdst{CounterFail}=0;
        $rdst{Run}=$id;
        $maxrun=$maxrun+1;
        $rdst{uid}=$maxrun;
        $rdst{FirstEvent}=1;
        $rdst{FilePath}=$file;
        $rdst{History}="ToBeRerun";
        $rdst{Status}="ToBeRerun";
        $rdst{cuid}=$id;
        $rdst{SubmitTime}=$client{Start};
           my @sbuf=split "\n",$buf;
             foreach my $line (@sbuf){
                 if($line=~/^TRIG=/){ 
                     my @text= split '=',$line;
                     $rdst{LastEvent}=$text[1];
                 }
                 elsif($line=~/^TIMBEG=/){
                      my @text= split '=',$line;
                      my $timbeg=$text[1];
                     my $year=$timbeg%10000-1900;
                     my $month=int($timbeg/10000)%100-1;
                     my $date=int($timbeg/1000000)%100;
                     $rdst{TFEvent}=timelocal(1,0,8,$date,$month,$year);
                 }
                 elsif($line=~/^TIMEND=/){
                      my @text= split '=',$line;
                      my $timbeg=$text[1];
                     my $year=$timbeg%10000-1900;
                     my $month=int($timbeg/10000)%100-1;
                     my $date=int($timbeg/1000000)%100;
                     $rdst{TLEvent}=timelocal(1,0,8,$date,$month,$year);
                     warn " Run $id completed \n";

        my $arsref;
        foreach $arsref (@{$ref->{arpref}}){
            try{
                $arsref->sendRunEvInfo(\%rdst,"Create");
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
                $arsref->sendRunEvInfo(\%rdst,"Create");
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }


                     last;
                 }
             } 
    }
}
}
FoundRun:
}


# now rootfiles ( for runs which already finished)

     sub prioa{ $a->{Run} <=> $b->{Run};}
     my @sortedrtb=sort prioa @{$Monitor::Singleton->{dsts}};
     my $run=-1;
     for my $j (0 ... $#sortedrtb){
          my $hdst=$sortedrtb[$j];
          if( $hdst->{Type} eq "Ntuple"){
           if($hdst->{Status} eq "Validated"){
               my @text=split '\/',$hdst->{Name};
               my @name=split '\.',$text[$#text];
               my $id=$hdst->{Run};
              if($id<100000 && $id!=$run && not ($hdst->{Name}=~/dat0\/local\/logs/)){
               $run=$id;
               my %rdst; 
               for my $j (0 ... $#{$ref->{rtb}}){
                %rdst=%{${$ref->{rtb}}[$j]};
                if ($rdst{uid}>$maxrun){
                 $maxrun=$rdst{uid};
                }
                if($rdst{Run} eq $id){
                 warn "  Found $id \n";
                 goto FoundRun2;
                } 
               }
               warn "   Run $id not found !!!!!!! \n";
opendir THISDIR ,$dir or die "unable to open $dir";
my @allfiles= readdir THISDIR;
closedir THISDIR;
foreach my $file (@allfiles){
    if ($file =~/^cern\.$id/){
        warn " found file for $id \n";
        my $full=$dir.$file;
        open(FILE,"<".$full) or die "Unable to open file $full\n";
        my $buf;
        read(FILE,$buf,16384000) or next;
        close FILE;
        $rdst{Run}=$id;
        $maxrun=$maxrun+1;
        $rdst{uid}=$maxrun;
        $rdst{Priority}=0;
        $rdst{CounterFail}=0;
        $rdst{FirstEvent}=1;
        $rdst{FilePath}=$file;
        $rdst{History}="ToBeRerun";
        $rdst{Status}="Finished";
        $rdst{SubmitTime}=$hdst->{Begin};
        $rdst{cuid}=$id;
           my @sbuf=split "\n",$buf;
             foreach my $line (@sbuf){
                 if($line=~/^TRIG=/){ 
                     my @text= split '=',$line;
                     $rdst{LastEvent}=$text[1];
                 }
                 elsif($line=~/^TIMBEG=/){
                      my @text= split '=',$line;
                      my $timbeg=$text[1];
                     my $year=$timbeg%10000-1900;
                     my $month=int($timbeg/10000)%100-1;
                     my $date=int($timbeg/1000000)%100;
                     $rdst{TFEvent}=timelocal(1,0,8,$date,$month,$year);
                 }
                 elsif($line=~/^TIMEND=/){
                      my @text= split '=',$line;
                      my $timbeg=$text[1];
                     my $year=$timbeg%10000-1900;
                     my $month=int($timbeg/10000)%100-1;
                     my $date=int($timbeg/1000000)%100;
                     $rdst{TLEvent}=timelocal(1,0,8,$date,$month,$year);
                     warn " Run $id completed \n";

        my $arsref;
        foreach $arsref (@{$ref->{arpref}}){
            try{
                $arsref->sendRunEvInfo(\%rdst,"Create");
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
                $arsref->sendRunEvInfo(\%rdst,"Create");
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }


                     last;
                 }
             } 
    }
}
              }

            }
          }
FoundRun2:
      
       }


}





sub RestoreRuns1{
#  in case if runs are in scripts, but not in server
#
 my $ref=shift;
 my $dir;
 my $maxrun=0;

# get amsdatadir
     for my $i (0 ... $#{$Monitor::Singleton->{env}}){
         my $hash=$Monitor::Singleton->{env}[$i];
         my @text = split '\=',$hash;
         if($text[0]=~/^AMSDataDir/){
           warn "  AMSDataDir $text[1] \n";
           $dir=$text[1]."/prod.log/scripts/";
           last;
         }
     }
{



 my $joudir="/f2users/scratch/MC/cern/jou/MCProducer/";
opendir THISDIR ,$joudir or die "unable to open $joudir";
my @allfiles= readdir THISDIR;
closedir THISDIR;
foreach my $file (@allfiles){
    if($file=~/\.journal$/){
        my @junk=split '\.', $file;
        my $run=int($junk[0]);
        my $ctime=time();
            my $full=$joudir.$file;
        my $mtime=(stat $full)[9];
        if($ctime-$mtime<48*3600){
            open(FILE,"<".$full) or die "Unable to open journal file $full \n";
            my $buf;
            read(FILE,$buf,1638400) or next;
            close FILE;
            my $mips=0;
            my $hname=" ";
            my $tspent=0;
            my $cpu=0;
            my $events=0;
            my $errors=0;
            @junk=split 'Mips ',$buf;
            if($#junk>0){
                my @jmips=split "\n",$junk[1];
                $mips=$jmips[0];    
            }
            if($buf=~/RunFinished/ && $buf=~/, Status Finished/){
              @junk=split ' RunFinished CInfo  \, ', $buf;
              if($#junk<1){
                  die " no  run finished ??  $buf";
              }
              my @jcinfo=split '\, ',$junk[1];
              $hname=(split ' ',$jcinfo[0])[1];  
              $events=(split ' ',$jcinfo[2])[1];  
              $errors=(split ' ',$jcinfo[4])[1];  
              $tspent=(split ' ',$jcinfo[6])[1];  
              $cpu=(split ' ',$jcinfo[7])[1];  
          }
             elsif($buf=~/RunIncomplete/ && $buf=~/, Status Failed/){
              @junk=split ' RunIncomplete CInfo  \, ', $buf;
              if($#junk<1){
                  die " no  run incomplete ??  $buf";
              }
              my @jcinfo=split '\, ',$junk[1];
              $hname=(split ' ',$jcinfo[0])[1];  
              $events=(split ' ',$jcinfo[2])[1];  
              $errors=(split ' ',$jcinfo[4])[1];  
              $tspent=(split ' ',$jcinfo[6])[1];  
              $cpu=(split ' ',$jcinfo[7])[1];  
          }
            if($buf=~/RunFinished1/ && $buf=~/, Status Finished/){
#              warn "  $run finished "; 
              @junk=split 'CloseDST  \, Status Validated \, Type RootFile \, Name ', $buf;
              if($#junk<1){
                  die " no  dst found for run $run";
              }
              else{ 
               my @j2=split '[:,]', $junk[1];
#               warn "  file $j2[1] found \n";
               if( open(FILE,"<".$j2[1])){
                 close FILE;
                 die "  $j2[1] exists !!! \n";
               }        
            }              
          }
           else{
      my %rdst; 
      for my $j (0 ... $#{$ref->{rtb}}){
        %rdst=%{${$ref->{rtb}}[$j]};
        if ($rdst{uid}>$maxrun){
          $maxrun=$rdst{uid};
         }
        if($rdst{Run} eq $run){
         goto FoundRun;
        }
       }
               warn "  run $ run not finished \n";
            my @rec=split '-I-TimeStamp', $buf;
            my $dstf=0;
            my $lste=0;
            my $stime=time();
            foreach my $sbuf (@rec){
                if($sbuf=~/StartingRun/ && $sbuf=~/SubmitTimeU/){
                    @junk=split /SubmitTimeU/,$sbuf;
                    my @sjunk=split '\,', $junk[1];
                    $stime=$sjunk[0];
                    warn "  submit time $stime run $run \n";
                }
            }
            foreach my $sbuf (@rec){
              @junk=split 'CloseDST  \, Status Validated \, Type RootFile \, ', $sbuf;
              if($#junk<1){
                warn " no clsed dst found for run $run \n";
                next;
              }               
              $dstf=1;
              my %hdst=%{$Monitor::Singleton->{dsts}[0]};
            $hdst{Status}='Validated';
            my @src=split ' \, ',$junk[1];
            foreach my $entry (@src){
                my @value=split ' ',$entry;
                $hdst{$value[0]}=$value[1];
                warn " $value[0] $value[1] \n";
                if($value[0]=~/LastEvent/){
                 $lste=$value[1];
                }                
            }
              if($maxrun<10000){
                my %cid=%{$ref->{cid}};
        my $arsref;
        foreach $arsref (@{$ref->{arpref}}){
            try{
                $arsref->sendDSTEnd(\%cid,\%hdst,"Create");
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
#                $arsref->sendDSTEnd(\%cid,\%hdst,"ClearCreate");
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
    }
          }
opendir THISDIR ,$dir or die "unable to open $dir";
my @allfiles= readdir THISDIR;
closedir THISDIR;
foreach my $file (@allfiles){
    if ($file =~/^cern\.$run/){
        warn " found file for $run \n";
        my $full=$dir.$file;
        open(FILE,"<".$full) or die "Unable to open file $full\n";
        my $buf;
        read(FILE,$buf,16384000) or next;
        close FILE;
        $maxrun=$maxrun+1;
        $rdst{Run}=$run;
        $rdst{uid}=$maxrun;
        $rdst{FirstEvent}=1;
        $rdst{FilePath}=$file;
        $rdst{History}="ToBeRerun";
        $rdst{cuid}=$run;
        $rdst{SubmitTime}=$stime;
        $rdst{Priority}=0;
        $rdst{CounterFail}=0;
           my @sbuf=split "\n",$buf;
             foreach my $line (@sbuf){
                 if($line=~/^TRIG=/){ 
                     my @text= split '=',$line;
                    if($dstf eq 0){
                     $rdst{LastEvent}=$text[1];
                     $rdst{Status}="ToBeRerun";
                    }
                    else{
                     $rdst{Status}="Finished";
                     $rdst{LastEvent}=$lste;
#                     my $cinfo={};
#                     $rdst{cinfo}=$cinfo;
                     $rdst{cinfo}->{Mips}=$mips;
                     $rdst{cinfo}->{HostName}=$hname;
                     $rdst{cinfo}->{TimeSpent}=$tspent;
                     $rdst{cinfo}->{EventsProcessed}=$events;
                     $rdst{cinfo}->{CriticalErrorsFound}=$errors;
                     $rdst{cinfo}->{CPUMipsTimeSpent}=$cpu;
                    }

                 }
                 elsif($line=~/^TIMBEG=/){
                      my @text= split '=',$line;
                      my $timbeg=$text[1];
                     my $year=$timbeg%10000-1900;
                     my $month=int($timbeg/10000)%100-1;
                     my $date=int($timbeg/1000000)%100;
                     $rdst{TFEvent}=timelocal(1,0,8,$date,$month,$year);
                 }
                 elsif($line=~/^TIMEND=/){
                      my @text= split '=',$line;
                      my $timbeg=$text[1];
                     my $year=$timbeg%10000-1900;
                     my $month=int($timbeg/10000)%100-1;
                     my $date=int($timbeg/1000000)%100;
                     $rdst{TLEvent}=timelocal(1,0,8,$date,$month,$year);
                     warn " Run $run completed $rdst{Status} $rdst{uid} \n";
                      if($maxrun<=10000){
        my $arsref;
        foreach $arsref (@{$ref->{arpref}}){
            try{
                $arsref->sendRunEvInfo(\%rdst,"Create");
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
                $arsref->sendRunEvInfo(\%rdst,"Create");
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
    }

                     last;
                  }
             }
    }
}
 





FoundRun:
           }             
    }
}
}



     for my $i (0 ... $#{$Monitor::Singleton->{env}}){
         my $hash=$Monitor::Singleton->{env}[$i];
         my @text = split '\=',$hash;
         if($text[0]=~/^AMSDataDir/){
           warn "  AMSDataDir $text[1] \n";
           $dir=$text[1]."/prod.log/scripts/";
           last;
         }
     }
opendir THISDIR ,$dir or die "unable to open $dir";
@allfiles= readdir THISDIR;
closedir THISDIR;

for my $run (3552...3551){
foreach my $file (@allfiles){
    if ($file =~/^cern\.$run/){
               my %rdst; 
               for my $j (0 ... $#{$ref->{rtb}}){
                %rdst=%{${$ref->{rtb}}[$j]};
                if ($rdst{uid}>$maxrun){
                 $maxrun=$rdst{uid};
                }
                if($rdst{Run} eq $run){
                 warn "  Found $run \n !!!!!";
                 goto fr3;
                } 
               }
               warn "   Run $run not found !!!!!!! \n";
        warn " found file for $run \n";
        my $full=$dir.$file;
        open(FILE,"<".$full) or die "Unable to open file $full\n";
        my $buf;
        read(FILE,$buf,16384000) or next;
        close FILE;
        $rdst{Run}=$run;
        $maxrun=$maxrun+1;
        $rdst{uid}=$maxrun;
        $rdst{FirstEvent}=1;
        $rdst{FilePath}=$file;
        $rdst{History}="ToBeRerun";
        $rdst{cuid}=$run;
        $rdst{SubmitTime}=time();
        $rdst{Priority}=0;
        $rdst{CounterFail}=0;
           my @sbuf=split "\n",$buf;
             foreach my $line (@sbuf){
                 if($line=~/^TRIG=/){ 
                     my @text= split '=',$line;
                     $rdst{LastEvent}=$text[1];
                     $rdst{Status}="ToBeRerun";

                 }
                 elsif($line=~/^TIMBEG=/){
                      my @text= split '=',$line;
                      my $timbeg=$text[1];
                     my $year=$timbeg%10000-1900;
                     my $month=int($timbeg/10000)%100-1;
                     my $date=int($timbeg/1000000)%100;
                     $rdst{TFEvent}=timelocal(1,0,8,$date,$month,$year);
                 }
                 elsif($line=~/^TIMEND=/){
                      my @text= split '=',$line;
                      my $timbeg=$text[1];
                     my $year=$timbeg%10000-1900;
                     my $month=int($timbeg/10000)%100-1;
                     my $date=int($timbeg/1000000)%100;
                     $rdst{TLEvent}=timelocal(1,0,8,$date,$month,$year);
                     warn " Run $run completed $rdst{Status} $rdst{uid} \n";
                      if($maxrun<=10000){
        my $arsref;
        foreach $arsref (@{$ref->{arpref}}){
            try{
                $arsref->sendRunEvInfo(\%rdst,"Create");
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
                $arsref->sendRunEvInfo(\%rdst,"Create");
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
    }

                     last;
                  }
             }
fr3:
}
}
 
}
}




}

sub RestoreRuns_no{
 my $ref=shift;
 my $dir;
 my $maxrun=0;



      for my $j (0 ... $#{$ref->{rtb}}){
        my %rdst=%{${$ref->{rtb}}[$j]};
        foreach my $arsref (@{$ref->{ardref}}){
            try{
                $arsref->sendRunEvInfo(\%rdst,"Create");
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };


        }
            warn " sent $rdst{Run} \n";
}
return;
# get amsdatadir
     for my $i (0 ... $#{$Monitor::Singleton->{env}}){
         my $hash=$Monitor::Singleton->{env}[$i];
         my @text = split '\=',$hash;
         if($text[0]=~/^AMSDataDir/){
           warn "  AMSDataDir $text[1] \n";
           $dir=$text[1]."/prod.log/scripts/";
           last;
         }
     }

 my $joudir="/f2users/scratch/MC/cern/jou/MCProducer/";
opendir THISDIR ,$joudir or die "unable to open $joudir";
my @allfiles= readdir THISDIR;
closedir THISDIR;
 my @runs=(533);
   foreach my $file (@allfiles){
    if($file=~/\.journal$/){
        my @junk=split '\.', $file;
        my $run=int($junk[0]);
        foreach my $en (@runs){
           if($run eq $en){
            my $full=$joudir.$file;
            open(FILE,"<".$full) or die "Unable to open journal file $full \n";
            my $buf;
            read(FILE,$buf,1638400) or next;
            close FILE;
            my @rec=split '-I-TimeStamp', $buf;
            my $dstf=0;
            my $lste=0;
            my $stime=time();
            foreach my $sbuf (@rec){
                if($sbuf=~/StartingRun/ && $sbuf=~/SubmitTimeU/){
                    @junk=split /SubmitTimeU/,$sbuf;
                    my @sjunk=split '\,', $junk[1];
                    $stime=$sjunk[0];
                    warn "  submit time $stime run $run \n";
                }
            }
            foreach my $sbuf (@rec){
              @junk=split 'CloseDST  \, Status Validated \, Type RootFile \, ', $sbuf;
              if($#junk<1){
                warn " no clsed dst found for run $run \n";
                next;
              }               
              $dstf=1;
              my %hdst=%{$Monitor::Singleton->{dsts}[0]};
            $hdst{Status}='Validated';
            my @src=split ' \, ',$junk[1];
            foreach my $entry (@src){
                my @value=split ' ',$entry;
                if($value[0]=~/CRC/){
                 $hdst{crc}=$value[1];
                 warn " crc updated!!! \n";
                }
                else{
                 $hdst{$value[0]}=$value[1];
                }
                warn " $value[0] $value[1] \n";
                if($value[0]=~/LastEvent/){
                 $lste=$value[1];
                }                
            }
                my %cid=%{$ref->{cid}};
              my $arsref;
            foreach $arsref (@{$ref->{arpref}}){
            try{
#                $arsref->sendDSTEnd(\%cid,\%hdst,"Delete");
#                $arsref->sendDSTEnd(\%cid,\%hdst,"Create");
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
            }
     for my $i (0...$#{$Monitor::Singleton->{dsts}}){
                 my %hdsts=%{$Monitor::Singleton->{dsts}[$i]};
        foreach $arsref (@{$ref->{ardref}}){
            try{
                 $arsref->sendDSTEnd(\%cid,\%hdsts,"Delete");
                 $arsref->sendDSTEnd(\%cid,\%hdsts,"Create");
                 warn " $i $hdsts{Name} \n ";
                last;
             }
            catch DPS::DBProblem   with{
                my $e=shift;
                warn "DBProblem: $e->{message}\n";
                 $arsref->sendDSTEnd(\%cid,\%hdsts,"Create");
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
          }
            last;
        }
        }
       }
    }
}
return; 
}








sub DeleteFailedDST{
 my $ref=shift;

     my @ncrun=();
     my @nctime=();
     my @ncname=();
     my @nchost=();
    for my $i (0 ... $#{$Monitor::Singleton->{dsts}}){
     my %nc=%{$Monitor::Singleton->{dsts}[$i]};
     if($nc{Type} eq "Ntuple" or $nc{Type} eq "RootFile"){
         if($nc{Status} eq "Failure"){
#            warn " $nc{Name} \n";           
                my @parser=split ':',$nc{Name};
            if($#parser>0){
                my $host=$parser[0];
                my $hostok=0;
                if($host =~ /cern.ch/ ){
#                   ok
                    $hostok=1;
                }
                else{
                 my @hparser=split '\.',$host;
                 if($#hparser>0 and $hparser[1] eq 'om'){
                     $host=$hparser[0].'.cern.ch';
                     $hostok=1;
                 }
                 else {
#                   warn "Host Bizarre $host \n";           
                 }
               }
                if($hostok){
                    my $ifg=0;
                    my $rm = "rm ";
                    my $cmd="ssh -2 -x $host $rm $parser[1]";
                    foreach my $ntuple (@{$Monitor::Singleton->{dsts}}){
                        if($ntuple->{Name} eq $nc{Name} and 
                           $ntuple->{Status} ne $nc{Status}){
                            $ifg=1;
                            last;
                        }
                    }
                        
                    my $i=0;
                    if(!$ifg){
                          $i=system($cmd);
                      }
                    if($i){
                        warn "$cmd failed \n";
                      my $force=1;
                     if($force){
                     my $arsref;
                     my %cid=%{$ref->{cid}};
                     foreach $arsref (@{$ref->{arpref}}){
                      try{
                       $arsref->sendDSTEnd(\%cid,\%nc,"Delete");
                      }
                      catch CORBA::SystemException with{
                       warn "sendback corba exc";
                      };
                     }
                    }
                    }
                    else{
                        warn " $cmd ok \n";
                     my $arsref;
                     my %cid=%{$ref->{cid}};
                     foreach $arsref (@{$ref->{arpref}}){
                      try{
                       $arsref->sendDSTEnd(\%cid,\%nc,"Delete");
                      }
                      catch CORBA::SystemException with{
                       warn "sendback corba exc";
                      };
                     }
                     foreach $arsref (@{$ref->{ardref}}){
                     try{
                      $arsref->sendDSTEnd(\%cid,\%nc,"Delete");
                      last;
                     }
            catch DPS::DBProblem   with{
                my $e=shift;
                warn "DBProblem: $e->{message}\n";
            }
                    catch CORBA::SystemException with{
                    warn "sendback corba exc";
                   };
                 }
                 }
                }
            }
            }
         elsif($nc{Status} eq "InProgress"){
             my $rfound=0;
           foreach my $hash (@{$Monitor::Singleton->{acl}}){
               if($hash->{id}->{uid} eq $nc{Run}){
               my @host=split ":",$nc{Name};
               my @fh=split '\.',$host[0];
               if($host[0] eq $hash->{id}->{HostName} or
                  ($fh[1] eq 'om' and ($hash->{id}->{HostName} =~ /$fh[0]/))or
                  ($host[0] =~/ams/ and $hash->{id}->{HostName} =~ /pcamsf2/)){
                   push @nchost , $hash->{id}->{HostName} ;
                   push @ncname , $nc{Name};
                   push @ncrun, $nc{Run};
                   push @nctime, $nc{Insert};
               }
              } 
           }
       }
     }
 
}
    my $good=0;
     my @ncgood=();
     my @ncgh=();
     my @ncgr=();
    for my $i (0 ... $#ncrun){
       if($ncrun[$i] == 0){
         next;
       }
       my $ltime=$nctime[$i];
       my $idef=$i;
      for my $j ($i+1 ... $#ncrun){ 
        if($ncrun[$i] == $ncrun[$j]){
           $ncrun[$j]=0;
         if($ltime<$nctime[$j]){
           $ltime=$nctime[$j];
           $idef=$j;
         }
        }
      }
      warn " $ncname[$idef] ok \n";
       push @ncgood,  $ncname[$idef];
       push @ncgh,  $nchost[$idef];
       push @ncgr,  $ncrun[$i];
      $good++; 
     } 
  warn "  in progress good $good \n";
         for my $i (0 ... $#{$Monitor::Singleton->{dsts}}){
     my %nc=%{$Monitor::Singleton->{dsts}[$i]};
     if($nc{Type} eq "Ntuple" or $nc{Type} eq "RootFile"){
         if($nc{Status} eq "InProgress"){
          my $ifg=0;
          foreach my $ncg (@ncgood){
           if($nc{Name} eq $ncg){
             $ifg=1;
#             last;
           }
          }
          if(!$ifg ){
           $nc{Status}="Failure";
                     my $arsref;
                     my %cid=%{$ref->{cid}};
                     foreach $arsref (@{$ref->{arpref}}){
                      try{
                       $arsref->sendDSTEnd(\%cid,\%nc,"Update");
                      }
                      catch CORBA::SystemException with{
                       warn "sendback corba exc";
                      };
                     }

          }
         }
      }
 }









 my $ok=1;
   for my $i (0 ... $#ncgr){
    my $move="ssh -2 -x $ncgh[$i] mv /dat0/local/logs/MCProducer.".$ncgr[$i].".log /dat0/local/logs/aMCProducer.".$ncgr[$i].".log";
    if(system($move)){
        $ok=0;
        last;
    }
   }
 if($ok){
   for my $i (0 ... $#ncgr){
    my $rm="ssh -2 -x $ncgh[$i] rm  -rf '/dat0/local/logs/MCProducer*.log'";
    system($rm);
     $rm="ssh -2 -x $ncgh[$i] rm  -rf '/dat0/local/logs/Killer*.log'";
    system($rm);
   }
 }
   for my $i (0 ... $#ncgr){
    my $move="ssh -2 -x $ncgh[$i] mv /dat0/local/logs/aMCProducer.".$ncgr[$i].".log /dat0/local/logs/MCProducer.".$ncgr[$i].".log";
    system($move);
   }
warn "   ***evrthing finished***\n";
}

sub DeleteValidatedDst{
 my $ref=shift;

#
# Find ntuples without runs and delete them
#
    for my $i (0 ... $#{$Monitor::Singleton->{dsts}}){
     my %nc=%{$Monitor::Singleton->{dsts}[$i]};
     if($nc{Type} eq "Ntuple" or $nc{Type} eq "RootFile"){
         my $Run=$nc{Run};
         my $rfound=0;  
    foreach my $run (@{$Monitor::Singleton->{rtb}}){
          if($run->{Run} eq $Run){
              $rfound=1;
              last;
          }

      }

         if(!$rfound){
                     print "deleting $Run $nc{Name}\n";
                     my $arsref;
                     my %cid=%{$ref->{cid}};
                     foreach $arsref (@{$ref->{arpref}}){
                      try{
                       $arsref->sendDSTEnd(\%cid,\%nc,"Delete");
                      }
                      catch CORBA::SystemException with{
                       warn "sendback corba exc";
                      };
                  }
                     foreach $arsref (@{$ref->{ardref}}){
                     try{
                      $arsref->sendDSTEnd(\%cid,\%nc,"Delete");
                      last;
                     }
            catch DPS::DBProblem   with{
                my $e=shift;
                warn "DBProblem: $e->{message}\n";
            }
                    catch CORBA::SystemException with{
                    warn "sendback corba exc";
                   };
                 }


         }
     }
 }

}





sub FinishFailedRuns{
 my $ref=shift;


      for my $j (0 ... $#{$ref->{rtb}}){
        my %rdst=%{${$ref->{rtb}}[$j]};

 if($rdst{Status} eq "Processing"){
     my $found=0;
     foreach my $ac (@{$ref->{acl}}){
         if($ac->{id}->{uid} eq $rdst{cuid}){
             $found=1;
             last;
         }
     }
     if($found eq 0){
         $rdst{Status} ="Failed";
         $rdst{History}="Failed";
     }
 }
       if($rdst{Status} eq "Failed"){

    for my $i (0 ... $#{$Monitor::Singleton->{dsts}}){
     my %nc=%{$Monitor::Singleton->{dsts}[$i]};
     if($nc{Type} eq "Ntuple" or $nc{Type} eq "RootFile"){
         if($nc{Status} eq "Validated" or $nc{Status} eq "Success"){
             $rdst{Status}="Finished";
             last;
         }
     }
    }
         if($rdst{Run}>(1<<26)){
          $rdst{Status}="Foreign";
          }
     }
       if($rdst{Status} eq "Allocated"){
      
         $rdst{Status}="Finished";
     }

        my $arsref;
        foreach $arsref (@{$ref->{arpref}}){
            try{
                $arsref->sendRunEvInfo(\%rdst,"Update");
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        }
        foreach $arsref (@{$ref->{ardref}}){
            try{
                $arsref->sendRunEvInfo(\%rdst,"Update");
                last;
            }
            catch CORBA::SystemException with{
                warn "sendback corba exc";
            };
        
        }

}

}
