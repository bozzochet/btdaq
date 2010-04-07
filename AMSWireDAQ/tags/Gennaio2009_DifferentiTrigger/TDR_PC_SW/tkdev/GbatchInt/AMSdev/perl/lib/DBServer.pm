# $Id: DBServer.pm,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $

package DBServer;
 use CORBA::ORBit idl => [ '/var/www/cgi-bin/mon/include/server.idl'];
use Error qw(:try);
use Carp;
use strict;
use lib::CID;
use lib::ActiveClient;
use lib::POADBServer;
use Carp;
use Fcntl ":flock";
use Fcntl;
use POSIX qw(tmpnam);
use MLDBM qw(DB_File Storable);


@DBServer::EXPORT= qw(new Update Init InitDBFile InitDBFileNew);
my %lock=(
 dblock=>0,
 uid=>0,
 time=>0,
 timeout=>60,
          );
my %fields=(
     cid=>undef,
     start=>undef,
     ac=>undef,
     orb=>undef,
     root_poa=>undef,
     mypoadbserver=>undef,
     myref=>undef,
     myior=>undef,
     arsref=>[],
     arpref=>[],
     nhl=>undef,
     ahls=>undef,
     ahlp=>undef,
     acl=>undef,
     aml=>undef,
     asl=>undef,
     adbsl=>undef,
     acl_maxc=>0,
     aml_maxc=>0,
     asl_maxc=>0,
     adbsl_maxc=>0,
     nsl=>undef,
     ncl=>undef,
     nkl=>undef,
     rtb=>undef,
     rtb_maxr=>undef,
     dsti=>undef,
     dsts=>undef,
     db=>undef,
     env=>undef,
     rn=>undef,
     dbfile=>undef,
     ok=>0,
             );

sub new{
    if($ENV{MOD_PERL} and ref($DBServer::Singleton)){
        return $DBServer::Singleton;
    }
    my $type=shift;
    my $self={
        %fields,
    };
$self->{start}=time();
$self->{cid}=new CID("DBServer");    
$self->{orb} = CORBA::ORB_init("orbit-local-orb");
$self->{root_poa} = $self->{orb}->resolve_initial_references("RootPOA");
$self->{root_poa}->_get_the_POAManager->activate;
$self->{mypoadbserver}=new POADBServer;
my $id = $self->{root_poa}->activate_object($self->{mypoadbserver});
$self->{myref}=$self->{root_poa}->id_to_reference ($id);
$self->{myior} = $self->{orb}->object_to_string ($self->{myref});
$self->{ac}= new ActiveClient($self->{myior},$self->{start},$self->{cid});

    my $mybless=bless $self,$type;
    if(ref($DBServer::Singleton)){
        croak "Only Single DBServer Allowed\n";
    }
$DBServer::Singleton=$mybless;
return $mybless;
}


sub UpdateEverything{
 my $ref=shift;
 my $arsref;
 my $sendback=shift;
 
 foreach $arsref (@{$ref->{arsref}}){
     my $length;
     my $ahl;
     try{
         my %cid=%{$ref->{cid}};
         $cid{Type}="Server";
       ($length,$ahl)=$arsref->getAHS(\%cid);
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
#         if(defined $sendback){
#           foreach my $acl (@{$ref->{asl}}){
#           $ref->{myref}->sendACPerl(\%cid,$acl,"Create");
#         }
#       }
         if(defined $sendback and defined $ref->{nhl} and $#{$ref->{nhl}}>=0){
           my @acl=@{$ref->{nhl}};
          $arsref->sendNHS(\%cid,\@acl);
         }
         else{
      ($length,$ahl)=$arsref->getNHS(\%cid);
         if($length==0){
             $ref->{nhl}=undef;
         }
         else {
             $ref->{nhl}=$ahl;
         }
  }
         if(defined $sendback and defined $ref->{nsl} and $#{$ref->{nsl}}>=0){
           my @acl=@{$ref->{nsl}};
          $arsref->sendNCS(\%cid,\@acl);
         }
         else{
        ($length,$ahl)=$arsref->getNC(\%cid);
         if($length==0){
             $ref->{nsl}=undef;
         }
         else {
             $ref->{nsl}=$ahl;
         }
    }
         if(defined $sendback and defined $ref->{nkl} and $#{$ref->{nkl}}>=0){
           my @acl=@{$ref->{nkl}};
          $arsref->sendNKS(\%cid,\@acl);
         }
         else{
        ($length,$ahl)=$arsref->getNK(\%cid);
         if($length==0){
             $ref->{nkl}=undef;
         }
         else {
             $ref->{nkl}=$ahl;
         }
       }
         if(defined $sendback and defined $ref->{env} and $#{$ref->{env}}>=0){
             for my $i (0...$#{$ref->{env}}){
               my $env=${$ref->{env}}[$i];
               my @dum=split '=', $env;
               $arsref->setEnv(\%cid, $dum[0],$dum[1]);               
             }
         }
         else{
        ($length,$ahl)=$arsref->getEnv(\%cid);
         if($length==0){
             $ref->{env}=undef;
         }
         else {
             $ref->{env}=$ahl;
         }
        }
            
         my $path="AMSDataDir";
         my $addpath="/DataBase/";
         my ($ok, $dhl)=$arsref->getDBSpace(\%cid,$path,$addpath);
         $ref->{db}=$dhl;
         $path="AMSProdOutputDir";
         $addpath="/";
          ($ok, $dhl)=$arsref->getDBSpace(\%cid,$path,$addpath);
         $ref->{rn}=$dhl;
         $cid{Type}="Producer";


         if(defined $sendback and defined $ref->{ahlp} and $#{$ref->{ahlp}}>=0){

                 my @acl=@{$ref->{ahlp}};
          $arsref->sendAHS(\%cid,\@acl);
         }
         else{
       ($length,$ahl)=$arsref->getAHS(\%cid);
         if($length==0){
             $ref->{ahlp}=undef;
         }
         else {
             $ref->{ahlp}=$ahl;
         }
   }


         if(defined $sendback and defined $ref->{acl} and $#{$ref->{acl}}>=0){
           foreach my $acl (@{$ref->{acl}}){
               my %ac=%{$acl};
                my $hash_ac=\%ac;
                $arsref->sendAC(\%cid,\$hash_ac,"Create");
           }   
         }
         else{
      ($length,$ahl,$maxc)=$arsref->getACS(\%cid);
         if($length==0){
             $ref->{acl}=undef;
         }
         else {
             $ref->{acl}=$ahl;
             $ref->{acl_maxc}=$maxc;
         }
  }
     warn "in nominal here \n";
         if(defined $sendback and defined $ref->{ncl} and $#{$ref->{ncl}}>=0){
                 my @acl=@{$ref->{ncl}};
          $arsref->sendNCS(\%cid,\@acl);
         }
         else{
        ($length,$ahl)=$arsref->getNC(\%cid);
         if($length==0){
             $ref->{ncl}=undef;
         }
         else {
             $ref->{ncl}=$ahl;
         }
    }
         $cid{Type}="DBServer";
     $length=0;
      ($length,$ahl,$maxc)=$arsref->getACS(\%cid);
         if($length==0){
             $ref->{adbsl}=undef;
         }
         else {
             $ref->{adbsl}=$ahl;
             $ref->{adbsl_maxc}=$maxc;
         }
         goto NEXT;
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
     warn "in rtb here \n";
 foreach $arsref (@{$ref->{arpref}}){
     my $length;
     my $ahl;
     try{
         my %cid=%{$ref->{cid}};
         $cid{Type}="Producer";
       
         my $maxr=0;
         if(defined $sendback and defined $ref->{rtb} and $#{$ref->{rtb}}>=0){
             my @rtb=@{$ref->{rtb}};
       warn " rtb start here \n";
          $arsref->sendRunEvInfoS(\%cid,\@rtb,$ref->{rtb_maxr});
       warn " rtb ok here \n";
         }
         else{
        ($length,$ahl)=$arsref->getRunEvInfoS(\%cid, \$maxr);
         if($length==0){
             $ref->{rtb}=undef;
         }
         else {
             $ref->{rtb}=$ahl;
             $ref->{rtb_maxr}=$maxr;
         }
    }
         if(defined $sendback and defined $ref->{dsti} and $#{$ref->{dsti}}>=0){
             my @dst=@{$ref->{dsti}};
       warn " in dsti here \n";
          $arsref->sendDSTInfoS(\%cid,\@dst);
       warn "  dsti ok here \n";
         }
         else{

        ($length,$ahl)=$arsref->getDSTInfoS(\%cid);
         if($length==0){
             $ref->{dsti}=undef;
         }
         else {
             $ref->{dsti}=$ahl;
         }
    }
         if(defined $sendback and defined $ref->{dsts} and $#{$ref->{dsts}}>=0){
             my @dst=@{$ref->{dsts}};
       warn " in dsts here \n";
            $arsref->sendDSTS(\%cid,\@dst);
       warn "  dst ok here \n";
         }
         else{
        ($length,$ahl)=$arsref->getDSTS(\%cid);
         if($length==0){
             $ref->{dsts}=undef;
         }
         else {
             $ref->{dsts}=$ahl;
         }
    }
          goto LAST;
     }
     catch CORBA::SystemException with{

     };
 }

  return 0; 
LAST:
 return 1;
}
sub DESTROY{
    my $self=shift;
    warn "DESTROYING $self";
}

sub UpdateARS{
 my $ref=shift;
 my $arsref;
 if(not $ref->{ok}){
     return 0;
 }
 foreach $arsref (@{$ref->{arsref}}){
     try{
         my %cid=%{$ref->{cid}};
         $cid{Type}="Server";
      my ($length,$pars)=$arsref->getARS(\%cid,"Any",0,1);
         if($length==0 ){
            carp "updars returns zero \n";
            return 0;
        }
        for(;;){
             my $old=shift @{$ref->{arsref}};
          if(ref($old)){
            undef $old;
          }
          else{
            last;
          }
         }
         my $ior;
         foreach $ior (@$pars){
             try{
              my $newref=$ref->{orb}->string_to_object($ior->{IOR});
              if(rand >0.5){
                  unshift @{$ref->{arsref}}, $newref;
              }
              else{
                  push @{$ref->{arsref}}, $newref;
              }
             }
             catch CORBA::SystemException with{
               carp "getars 2 oops SystemException Error "."\n";
             };

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
         last;
     }
    catch CORBA::SystemException with{
     carp "getars oops SystemException Error "."\n";
 };
 }
 return 1;
}

sub Update{
}




sub Exiting{
    my $ref=shift;
    my $message=shift;
    my $status=shift;
    warn $message;
        my $arsref;
                $ref->{cid}->{Status}=$status;
        foreach $arsref (@{$ref->{arsref}}){
            try{
                my %cid=%{$ref->{cid}};
                 
                $arsref->Exiting(\%cid,$message,$status);
            }
            catch CORBA::SystemException with{
                warn "Exiting corba exc";
            };
        }
}


sub SendId{
    my $ref=shift;
        my $arsref;
        foreach $arsref (@{$ref->{arsref}}){
            try{
                my %cid=%{$ref->{cid}};
                my $hash=\%cid;
                my ($ok,$ok2)=$arsref->sendId(\$hash,2000,30);
                if (not $ok){
                    sleep 10;
                    ($ok,$ok2)=$arsref->sendId(\$hash,2000,30);
                if (not $ok){
                  $ref->Exiting(" Server Requested To Exit ","SInExit");
                   return 0;
                }
                }
                $ref->{cid}=$hash;
                my %ac=%{$ref->{ac}};
                my $hash_ac=\%ac;
                $arsref->sendAC($hash,\$hash_ac,"Update");
                $ref->{ac}=$hash_ac;
            }
            catch CORBA::SystemException with{
                warn "Exiting corba exc";
            };
        }
      $ref->{myref}->sendACPerl($ref->{cid},$ref->{ac},"ClearCreate");
#     catch DPS::DBProblem with{
#         warn " DBProblem : Trying Create  \n";
#         $ref->{myref}->sendACPerl($ref->{cid},$ref->{ac},"Create");
#      }
    return 1;
}






sub Init{
my $ior;
my $amsprodlogdir;
my $amsprodlogfile;
my $uid;
foreach my $chop  (@ARGV){
    if($chop =~/^-U/){
        $uid=unpack("x2 A*",$chop);
    }
    elsif($chop =~/^-i/){
     $ior=unpack("x2 A*",$chop);
    }
    elsif($chop=~/^-A/){
     $amsprodlogdir=unpack("x2 A*",$chop);
    }
    elsif($chop=~/^-F/){
     $amsprodlogfile=unpack("x2 A*",$chop);
    }
}


 my $ref=shift;
 my $standalone=shift;
 $ref->{cid}->{uid}=$uid;
 $ref->{ok}=0;
 if((not defined $standalone) and (not defined $ior)){ 
     warn "NoIor, Exiting";
     return 0;
 }
if( defined $ior){
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
 if(not $ref->{ok}){
   return 0;
 }
 if((not defined $standalone) and (not $ref->UpdateARS())){
     $ref->Exiting(" Unable to get ARS From Server ","CInAbort");
     return 0;
 }
}
   $ref->{dbfile}=$amsprodlogfile;
  if(not $ref->InitDBFile($amsprodlogdir)){
      my $dbfile=$ref->{dbfile};
     $ref->Exiting(" Unable to open  DB File $dbfile","CInAbort");
     return 0;
  }
if(defined $ior){
  return $ref->SendId();
}
else{
    return 1;
}
}
sub InitDBFile{
    my %hash;
    local *DBM;
    my $db;
    my $init=0;
    my $ref=shift;
    my $amsprodlogdir=undef;
    if(ref($ref)){
     $amsprodlogdir=shift;
    }
     else{
         $ref=shift;
#        $amsprodlogdir=shift;
      }
    if (defined $ref->{dbfile}){
#
#        print "InitDBFile : ProdLogDir : $amsprodlogdir \n";
# check if dbfile already includes full path
        if($ref->{dbfile} =~/\//){
        }
        else{
         $ref->{dbfile}=$amsprodlogdir."/db/".$ref->{dbfile};
     }
#        die "trying to open dbfile $ref->{dbfile}\n";

        my $lock="$ref->{dbfile}.lock";
        sysopen(LOCK,$lock,O_RDONLY|O_CREAT) or die " Unable to open lock file $lock\n";
        my $ntry=0;
     until (flock LOCK, LOCK_EX|LOCK_NB){
         sleep 2;
         $ntry=$ntry+1;
         if($ntry>10){
           $ref->Exiting("Unable to get lock for $lock","CInAbort");
           close(LOCK);
          return 0;
         }
     }
        

         if(defined $amsprodlogdir){
            $db=tie %hash, "MLDBM",$ref->{dbfile},O_RDWR;
        }
        else{
            $db=tie %hash, "MLDBM",$ref->{dbfile},O_RDONLY;
        }
    }
    else{
#        die " tried to init db file sobaka";
        my $tmpname=tmpnam();
        $ref->{dbfile}=$amsprodlogdir."/db/".unpack("x5 A*",$tmpname);
        $db=tie %hash, "MLDBM",$ref->{dbfile},O_CREAT | O_RDWR, 0644 ;
        system "chmod o+w $ref->{dbfile}";
        $init=1;
    }
     if(not $db){
      $ref->Exiting("Unable to tie db_file $ref->{dbfile}","CInAbort");
      close(LOCK);
      return 0;
      }
#      my $fd=$db->fd;
#      my $ok=open DBM, "<&=$fd";
#      if( not $ok){
#       $ref->Exiting("Unable to dup DBM for lock: $!","CInAbort");
#       close(LOCK);
#       return 0;
#      }
#     my $ntry=0;
#     until (flock DBM, LOCK_EX|LOCK_NB){
#         sleep 1;
#         $ntry=$ntry+1;
#         if($ntry>10){
#           $ref->Exiting("Unable to get lock for $ref->{dbfile}","CInAbort");
#          close(LOCK);
#          return 0;
#         }
#     }
      undef $db;
    if($init){
        if(not $ref->UpdateEverything()){
          $ref->Exiting("Unable to get Tables From Server","CInAbort");
          close(LOCK);
          return 0;
        }       
        $hash{nhl}=$ref->{nhl};
        $hash{ahls}=$ref->{ahls};
        $hash{ahlp}=$ref->{ahlp};
        $hash{acl}=$ref->{acl};
        $hash{aml}=$ref->{aml};
        $hash{asl}=$ref->{asl};
        $hash{asl_maxc}=$ref->{asl_maxc};
        $hash{adbsl}=$ref->{adbsl};
        $hash{adbsl_maxc}=$ref->{adbsl_maxc};
        $hash{acl_maxc}=$ref->{acl_maxc};
        $hash{aml_maxc}=$ref->{aml_maxc};
        $hash{nsl}=$ref->{nsl};
        $hash{nkl}=$ref->{nkl};
        $hash{ncl}=$ref->{ncl};
        $hash{dsti}=$ref->{dsti};
        $hash{dsts}=$ref->{dsts};
        $hash{rtb}=$ref->{rtb};
        $hash{env}=$ref->{env};
        $hash{db}=$ref->{db};
        $hash{rn}=$ref->{rn};
        $hash{rtb_maxr}=$ref->{rtb_maxr};
    }
    else{
        $ref->{nhl}=$hash{nhl};
        $ref->{ahls}=$hash{ahls};
        $ref->{ahlp}=$hash{ahlp};
        $ref->{acl}=$hash{acl};
        $ref->{aml}=$hash{aml};
        $ref->{asl}=$hash{asl};
        $ref->{asl_maxc}=$hash{asl_maxc};
        $ref->{adbsl}=$hash{adbsl};
        $ref->{adbsl_maxc}=$hash{adbsl_maxc};
        $ref->{acl_maxc}=$hash{acl_maxc};
        $ref->{aml_maxc}=$hash{aml_maxc};
        $ref->{nsl}=$hash{nsl};
        $ref->{nkl}=$hash{nkl};
        $ref->{ncl}=$hash{ncl};
        $ref->{dsts}=$hash{dsts};
        $ref->{dsti}=$hash{dsti};
        $ref->{env}=$hash{env};
        $ref->{db}=$hash{db};
        $ref->{rn}=$hash{rn};
        $ref->{rtb}=$hash{rtb};
        $ref->{rtb_maxr}=$hash{rtb_maxr};
        warn "db file read\n";
        if(defined $amsprodlogdir){
          if( not $ref->UpdateEverything(1)){
          $ref->Exiting("Unable to send Tables To Server","CInAbort");
          close(LOCK);
          return 0;
          }
          else{
           $hash{ahls}=$ref->{ahls};
           $hash{asl}=$ref->{asl};
           $hash{asl_maxc}=$#{$ref->{asl}}+2;
          }
        }       
    }
    untie %hash;
    close(LOCK);
    return 1;

}


sub OpenDBFile{
    my %hash;
    local *DBM;
    my $db;
    my ($ref,$amsprodlogdir)=@_;
    if (defined $ref->{dbfile}){
        my $lock="$ref->{dbfile}.lock";
        sysopen(LOCK,$lock,O_RDONLY|O_CREAT) or die " Unable to open lock file $lock\n";
        my $ntry=0;
     until (flock LOCK, LOCK_EX|LOCK_NB){
         sleep 2;
         $ntry=$ntry+1;
         if($ntry>10){
           $ref->Exiting("Unable to get lock for $lock","CInAbort");
           close(LOCK);
          return 0;
         }
     }
        

      $db=tie %hash, "MLDBM",$ref->{dbfile},O_RDWR;
    }
    else{
        return 0;
    }
     if(not $db){
      close(LOCK);
      return 0;
      }
#      my $fd=$db->fd;
#      my $ok=open DBM, "<&=$fd";
#      if( not $ok){
#       untie %hash;
#       return 0;
#      }
#     my $ntry=0;
#     until (flock DBM, LOCK_EX|LOCK_NB){
#         sleep 2;
#         $ntry=$ntry+1;
#         if($ntry>10){
#          untie %hash;
#          return 0;
#         }
#     }
#      undef $db;
    close(LOCK);
    return 1,$db,%hash;

}



sub sendRunEvInfo{
    my $ref=shift;
    if(not ref($ref)){
         $ref=shift;
    }
    my ($ri,$rc)=@_;
#        my ($ok,%hash)=$ref->OpenDBFile();
# need to explicitely open db file in every sub 
    my $ok=0;
    my %hash;
    local *DBM;
    my $db;
    if (defined $ref->{dbfile}){
        my $lock="$ref->{dbfile}.lock";
        sysopen(LOCK,$lock,O_RDONLY|O_CREAT) or die " Unable to open lock file $lock\n";
        my $ntry=0;
     until (flock LOCK, LOCK_EX|LOCK_NB){
         sleep 2;
         $ntry=$ntry+1;
         if($ntry>10){
           $ref->Exiting("Unable to get lock for $lock","CInAbort");
           close(LOCK);
           goto OUT;
         }
     }
        
      $db=tie %hash, "MLDBM",$ref->{dbfile},O_RDWR;
    }
    else{
        goto OUT;
    }
     if(not $db){
        goto OUT;
      }
#      my $fd=$db->fd;
#      $ok=open DBM, "<&=$fd";
#      if( not $ok){
#        untie %hash;
#        goto OUT;
#      }
#     my $ntry=0;
#     $ok=0;
#     until (flock DBM, LOCK_EX|LOCK_NB){
#         sleep 2;
#         $ntry=$ntry+1;
#         if($ntry>10){
#             untie %hash;
#             goto OUT;
#         }
#     }
    $ok=1;
OUT:
      undef $db;
          if($ok){
              $ref->{rtb}=$hash{rtb};           
              $ref->{rtb_maxr}=$hash{rtb_maxr}; 
             if($rc eq "Update"){
                 for my $i (0 ... $#{$ref->{rtb}} ){
                     my $arel=$ref->{rtb}[$i];
                     if($ri->{uid} ==$arel->{uid}){
                         $ref->{rtb}[$i]=$ri;
                         $hash{rtb}=$ref->{rtb};
                         untie %hash;
                        close(LOCK);
                        return;
                     } 
                 }
      
             }
             elsif($rc eq "Delete"){
                 for my $i (0 ... $#{$ref->{rtb}}){
                     my $arel=$ref->{rtb}[$i];
                     if($ri->{uid} ==$arel->{uid}){
                         $ref->{rtb}[$i]=$ref->{rtb}[$#{$ref->{rtb}}];
                         $#{$ref->{rtb}}=$#{$ref->{rtb}}-1;
                         $hash{rtb}=$ref->{rtb};
                         untie %hash;
                         close(LOCK);
                         return;
                     } 
                 }
      
             }
             elsif($rc eq "Create"){
                 if($hash{rtb_maxr}>$ri->{uid}){
                    $ri->{uid}=$hash{rtb_maxr};
                 }
                         $hash{rtb_maxr}=$ri->{uid}+1;
                         $#{$ref->{rtb}}=$#{$ref->{rtb}}+1;
                         $ref->{rtb}[$#{$ref->{rtb}}]=$ri;
                         $hash{rtb}=$ref->{rtb};
                         untie %hash;
                         close(LOCK);
                         return;
      
             }
             elsif($rc eq "ClearCreate"){
                    $hash{rtb_maxr}=$ri->{uid}+1;
                    $#{$ref->{rtb}}=0;
                         $ref->{rtb}[$#{$ref->{rtb}}]=$ri;
                         $hash{rtb}=$ref->{rtb};
                         untie %hash;
                         close(LOCK);
                         return;
      
             }
                         untie %hash;
              $ref->Exiting ("DPS::DBProblem message=>Unable to $rc the rtb $ri->{uid}");
          }
          else{
             $ref->Exiting ("DPS::DBProblem Unable to Open DB File $ref->{dbfile}");
          } 
}
sub InitDBFileNew{
    my %hash;
    local *DBM;
    my $db;
    my $init=0;
    my $ref=shift;
    my $amsprodlogdir=undef;
    if(ref($ref)){
     $amsprodlogdir=shift;

    }
     else{
         $ref=shift;
     }
    $amsprodlogdir="/afs/ams.cern.ch/AMSDataDir/prod.log";
        my $tmpname=tmpnam();
        $ref->{dbfile}=$amsprodlogdir."/".unpack("x5 A*",$tmpname);
        my $lock="$ref->{dbfile}.lock";
        sysopen(LOCK,$lock,O_RDONLY|O_CREAT) or die " Unable to open lock file $lock\n";
        my $ntry=0;
     until (flock LOCK, LOCK_EX|LOCK_NB){
         sleep 2;
         $ntry=$ntry+1;
         if($ntry>10){
           $ref->Exiting("Unable to get lock for $lock","CInAbort");
           close(LOCK);
          return 0;
         }
     }
        
        $db=tie %hash, "MLDBM",$ref->{dbfile},O_CREAT | O_RDWR, 0644 ;
        system "chmod o+w $ref->{dbfile}";
        $init=1;
     if(not $db){
      $ref->Exiting("Unable to tie db_file $ref->{dbfile}","CInAbort");
           close(LOCK);
    
    return 0;
      }
#      my $fd=$db->fd;
#      my $ok=open DBM, "<&=$fd";
#      if( not $ok){
#       $ref->Exiting("Unable to dup DBM for lock: $!","CInAbort");
#       return 0;
#      }
#     my $ntry=0;
#     until (flock DBM, LOCK_EX|LOCK_NB){
#         sleep 1;
#         $ntry=$ntry+1;
#         if($ntry>10){
#           $ref->Exiting("Unable to get lock for $ref->{dbfile}","CInAbort");
#          return 0;
#         }
#     }
      undef $db;
    if($init){
        if(not $ref->UpdateEverything()){
          $ref->Exiting("Unable to get Tables From Server","CInAbort");
           close(LOCK);
          return 0;
        }
       $hash{nhl}=$ref->{nhl};
        $hash{ahls}=$ref->{ahls};
        $hash{ahlp}=$ref->{ahlp};
        $hash{acl}=$ref->{acl};
        $hash{aml}=$ref->{aml};
        $hash{asl}=$ref->{asl};
        $hash{asl_maxc}=$ref->{asl_maxc};
        $hash{adbsl}=$ref->{adbsl};
        $hash{adbsl_maxc}=$ref->{adbsl_maxc};
        $hash{acl_maxc}=$ref->{acl_maxc};
        $hash{aml_maxc}=$ref->{aml_maxc};
        $hash{nsl}=$ref->{nsl};
        $hash{nkl}=$ref->{nkl};
        $hash{ncl}=$ref->{ncl};
        $hash{dsti}=$ref->{dsti};
        $hash{dsts}=$ref->{dsts};
        $hash{rtb}=$ref->{rtb};
        $hash{env}=$ref->{env};
        $hash{db}=$ref->{db};
        $hash{rn}=$ref->{rn};
        $hash{rtb_maxr}=$ref->{rtb_maxr};
    }
    untie %hash;
           close(LOCK);
    return 1;

}


