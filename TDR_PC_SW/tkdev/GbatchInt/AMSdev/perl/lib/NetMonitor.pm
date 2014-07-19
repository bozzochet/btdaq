# $Id: NetMonitor.pm,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
# May 2006  V. Choutko 
package NetMonitor;
use Net::Ping;
use strict;
use Carp;
use DBI;
use POSIX  qw(strtod);
@NetMonitor::EXPORT=qw(new Run); 

sub new{
    my $type=shift;

my %fields=(
  sendmail=>[],
  hosts=>[],
  hostsstat=>[],
  bad=>[],
  badsave=>[],
  sleep=>60,
  hostfile=>"/afs/ams.cern.ch/Offline/AMSDataDir/DataManagement/prod/NominalHost",
  ping=>undef,
  sqlserver=>undef,
            );
my $self={
  %fields,
};
my %sfields=(
     start=>undef,
     dbhandler=>undef,
     dbdriver=>'Oracle:',
     dbfile=>'amsdb',
     dbinit=>0,
     lastupdate=>0,
     repet=>3600,
     keep=>3600*24,
             );
$self->{sqlserver}={%sfields,};
push @{$self->{sendmail}},{first=>1,repet=>21600,address=>'Alexandre.Eline@cern.ch 41764874733@mail2sms.cern.ch',sent=>0,timesent=>0};
push @{$self->{sendmail}},{first=>0,repet=>21600,address=>'vitali.choutko@cern.ch  41764870923@mail2sms.cern.ch',sent=>0,timesent=>0};

    my $mybless=bless $self,$type;
    if(ref($NetMonitor::Singleton)){
        croak "Only Single NetMonitor Allowed\n";
    }
    $NetMonitor::Singleton=$mybless;
    return $mybless;
}


sub InitOracle{
    my $self=shift;

   set_oracle_env();
    my $pwd="";
    my $user="amsdes";
    
    my $oracle="/afs/cern.ch/user/a/ams/.oracle/.oracle.oracle";
  aga2:
    if(not open(FILE,"<".$oracle)){
      $self->sendmailpolicy("NetMonitor-S-UnableToOpenFile $oracle \n",0,1);
      sleep $self->{sleep};
      goto aga2;
                        }
    while  (<FILE>){
        $pwd=$_;
    }
    close FILE;
      if(not $self->{sqlserver}->{dbhandler}=DBI->connect('DBI:'.$self->{sqlserver}->{dbdriver}.$self->{sqlserver}->{dbfile},$user,$pwd,{PrintError => 1, AutoCommit => 1})){
      $self->sendmailpolicy("NetMonitor-S-CannotConnectToOracle $DBI::errstr \n",0,1);
      sleep $self->{sleep};
      goto aga2;
     
  }


}

sub Run{
    my $self=shift;

    $self->InitOracle();




    $self->sendmailpolicy("NetMonitor-I-Started \n",1);
#     $self->{ping} = Net::Ping->new();
    $self->{ping} = Net::Ping->new("tcp");
    $self->{ping}->{port_num} =20001;
#    $self->{ping}->{port_num} = getservbyname("http", "tcp");







again:
if(not open(FILE,"<".$self->{hostfile})){
    $self->sendmailpolicy("NetMonitor-S-UnableToOpen File $self->{hostfile} \n",0);
    sleep $self->{sleep};
    goto again;
}
       my $buf;
    $self->sendmailpolicy("NetMonitor-I-Opened File $self->{hostfile} \n",0);
    if(not read(FILE,$buf,1638400)){
      close FILE;
      $self->sendmailpolicy("NetMonitor-S-ProblemsToReadFile $self->{hostfile} \n",0);
      sleep $self->{sleep};
      goto again;
    }     
      $self->sendmailpolicy("NetMonitor-I-ReadFile $self->{hostfile} \n",0);
    close FILE;
    my @sbuf=split "\n",$buf;
    $#{$self->{hosts}}=-1;
    foreach my $line (@sbuf){
     if($line=~/^\#/){
        next;
     }
     my @sline=split ' ',$line;
     if($#sline>2){
         my $host=$sline[0].".".$sline[1];
         push @{$self->{hosts}},$host;
            }
  }
#         push @{$self->{hosts}},"pcamsf6.cern.ch";
#         push @{$self->{hosts}},"pcamsec.cern.ch";

    while(1){
    $#{$self->{bad}}=-1; 
    my $mes="NetMonitor-W-SomeHostsAreDown";
    foreach my $host (@{$self->{hosts}}) {
    #    print "$host \n";
        if($self->{ping}->ping($host)){
        }
        else{
            push @{$self->{bad}}, $host." ".$mes;
         print "bad $host \n";

        }
    }
#    sleep(6);
#    while (my ($host,$rtt,$ip) = $self->{ping}->ack) {
#      print "HOST: $host [$ip] ACKed in $rtt seconds.\n";
#    }
#    sleep(6);
#@{$self->{bad}}=keys %{$self->{ping}->{bad}};

#
# Now timing
#

    $mes="NetMonitor-W-SomeHostsHaveWrongTime";
    my $command="ssh -2 -x -o \'StrictHostKeyChecking no \' ";
#    my ($sec,$min,$hr,$mday,$mon,$y,$w,$yd,$isdst)=localtime(time());
    foreach my $host (@{$self->{hosts}}) {
        my $gonext=0;
        foreach my $bad (@{$self->{bad}}){
            my @sbad=split ' ',$bad;
            if($sbad[0] eq $host){
                $gonext=1;
                last;
            }
        }
        if($gonext){
            next;
        }
        unlink "/tmp/xtime";
        my $i=system($command.$host.' date +%s > /tmp/xtime ');
        my $curtime=time();
        if(not $i){
          if(not open(FILE,"<"."/tmp/xtime")){
                 push @{$self->{bad}}, $host." NetMonitor-W-ssh1Failed";
            next;
          }           
             my $buf;
             if(not read(FILE,$buf,16384)){
                 push @{$self->{bad}}, $host." NetMonitor-W-ssh2Failed";
                 close FILE;
               next;
             }
             close FILE;
             unlink "/tmp/xtime";
#             my @sbuf= split ' ',$buf;
#             if($#sbuf>3){
#               my $xtl=($mday-1)*24*3600+$hr*3600+$min*60+$sec;
#               my @ssbuf=split ':',$sbuf[3];
#               my $xt=($sbuf[2]-1)*3600*24+$ssbuf[0]*3600+$ssbuf[1]*60+$ssbuf[2];
               if(abs($curtime-$buf)>360){
                  push @{$self->{bad}}, $host." NetMonitor-W-ClockProblems";
               }
               next;
#             }  
                push @{$self->{bad}}, $host." NetMonitor-W-ClockReadProblems";
            }
        else{
            push @{$self->{bad}}, $host." NetMonitor-W-sshFailed";
            next;
        }
    }
#
# df
#
    $mes="NetMonitor-W-SomeHostsHaveDiskSpaceProblems";
     $command="ssh -2 -x -o \'StrictHostKeyChecking no \' ";
    foreach my $host (@{$self->{hosts}}) {
        my $gonext=0;
        foreach my $bad (@{$self->{bad}}){
            my @sbad=split ' ',$bad;
            if($sbad[0] ){
                $gonext=1;
                last;
            }
        }
        if($gonext){
            next;
        }
        unlink "/tmp/xspace";
         #print "$command.$host. \n";
         my $i=system($command.$host." df -x nfs > /tmp/xspace ");
        if(not $i){
            if(not open(FILE,"<"."/tmp/xspace")){
                push @{$self->{bad}}, $host." NetMonitor-W-ssh1Failed";
                next;
            }
            my @words=<FILE>;
            close FILE;
            unlink "/tmp/xspace";
              foreach my $word (@words) {
                if($word =~/\/$/){
                    my @sword= split ' ',$word;
                    if($#sword>1){
                        my $pc=$sword[$#sword-1];
                        my $ava=$sword[$#sword-2];
                        $pc=~ s/\%//;
                        if($ava < 100000 or $pc<2){
                            push @{$self->{bad}}, $host." NetMonitor-W-DiskSpaceProblems";
                       }
                        last;
                    }
                }
            }
        }
         else{
                push @{$self->{bad}}, $host." NetMonitor-W-sshFailed";
                next;
            }
        }
###
    if($#{$self->{bad}}>-1){
        my $found=0;
        foreach my $bad (@{$self->{bad}}){
          foreach my $badsave (@{$self->{badsave}}){
              if($badsave eq $bad){
                  $found++;
                  last;
              }
          }
        }
              $self->sendmailpolicy("NetMonitor-W-SomeHostsAreDown",$#{$self->{bad}}+1-$found);
              $#{$self->{badsave}}=-1;
               foreach my $bada (@{$self->{bad}}){
                 push @{$self->{badsave}}, $bada;
               }                
    }
    else{
#reset mailcounts
      $self->sendmailpolicy("NetMonitor-I-AllHostsAreOK",0);
    }
     
    sleep($self->{sleep});

}
}

sub sendmailpolicy{
   my $self=shift; 
   my $subj=shift;
   my $force=shift;
   my $nooracle=shift;
   my $curtim=time();
   if(not defined $nooracle){
     $nooracle=0;
   } 
   if(not defined $force){
       $force=0;
   }
   my $f2=$force;
   if($subj=~/-I-/){



       for my $i (0..$#{$self->{sendmail}}){
           my $hash=${$self->{sendmail}}[$i];
           my $ok=0;
           my @sadd=split ' ',$hash->{address};
           if($hash->{sent}>0  or $force){
               foreach my $add (@sadd){
                $ok+=$self->sendmailmessage($add,$subj," ");
                if(!$ok){
                  $f2=1;
                }
               }
           } 
           if($ok<$#sadd+1){
            $hash->{sent}=0;
            $hash->{timesent}=0;
           }
   }


#
#   Oracle
#

       if($f2 or $curtim-$self->{sqlserver}->{lastupdate}>$self->{sqlserver}->{repet}){
           if($nooracle or $self->updateoracle($subj," ")){
           }   
           else{
            $self->InitOracle();
            if(not $self->updateoracle($subj," ")){
             $self->sendmailpolicy("NetMonitor-E-UnableToConnectOracle",0,1);
           }            
          }    
       }





   }
   else{
#  error
       my $firstsent=0;
       for my $i (0..$#{$self->{sendmail}}){
           my $hash=${$self->{sendmail}}[$i];
           if($hash->{first} ==1 and $hash->{sent}>1){
            $firstsent=1;
            last;
           }
       }
#
#  group bad hosts by subject
#
               my %badh=();
               my $mes="";
               foreach my $bad (@{$self->{bad}}){
                   my @smes = split ' ',$bad;
                   $subj=$smes[1];
                   $mes="$smes[0] \n";
                   $badh{$subj}="";
               }             
               foreach my $bad (@{$self->{bad}}){
                   my @smes = split ' ',$bad;
                   $subj=$smes[1];
                   $mes="$smes[0] \n";
                   $badh{$subj}=$badh{$subj}.$mes;
               }             

       for my $i (0..$#{$self->{sendmail}}){
           my $hash=${$self->{sendmail}}[$i];
           my $curtim=time();           
           if(($hash->{sent}==0 or $curtim-$hash->{timesent}>$hash->{repet} or $force!=0)
                and ($hash->{first}==1 or $firstsent)){
               my @sadd=split ' ',$hash->{address};
               my $mes="";
               my $ok=0;
               if($#{$self->{bad}} eq -1){
                foreach my $add(@sadd){
                  $ok+=$self->sendmailmessage($add,$subj," ");
                }
            }

               foreach my $subj (keys %badh){
               foreach my $add (@sadd){
                $ok+=$self->sendmailmessage($add,$subj,$badh{$subj});
                if(!$ok){
                 $f2=1;
                }
               }
               }
               my $min=$#sadd+1;
               if($min<($#sadd+1)*scalar(keys(%badh))){
                  $min=($#sadd+1)*scalar(keys(%badh));
              }
              if($ok<$min){
               $hash->{sent}++;
               $hash->{timesent}=$curtim;
              }
           } 
#
#   Oracle
#
       if($f2 or $curtim-$self->{sqlserver}->{lastupdate}>$self->{sqlserver}->{repet}){
    if(scalar(keys(%badh)) ==0){
          if($nooracle or $self->updateoracle($subj,$badh{$subj})){
           }              
           else{
            $self->InitOracle();
            if(not $self->updateoracle($subj,$badh{$subj})){
             $self->sendmailpolicy("NetMonitor-E-UnableToConnectOracle",0,1);
           }            
        }
      }
         foreach my $subj (keys %badh){
           if($nooracle or $self->updateoracle($subj,$badh{$subj})){
           }              
           else{
            $self->InitOracle();
            if(not $self->updateoracle($subj,$badh{$subj})){
             $self->sendmailpolicy("NetMonitor-E-UnableToConnectOracle",0,1);
           }            
       }
        }    

}}

}
}


sub sendmailmessage{
    my $self=shift;
    my $add=shift;
    my $sub=shift;
    my $mes=shift;
    open MAIL, "| /usr/lib/sendmail -t -i"
#        or die "could not open sendmail: $!";
        or return 1;
     print MAIL <<END_OF_MESSAGE2;
To:  $add
Subject: $sub

$mes
END_OF_MESSAGE2
    close MAIL or return 1;
    return 0;
}


sub set_oracle_env {
    $ENV{"ORACLE_HOME"}='/afs/cern.ch/project/oracle/@sys/prod';
#    $ENV{"TNS_ADMIN"}='/afs/cern.ch/project/oracle/admin';
 $ENV{"TNS_ADMIN"}='/afs/cern.ch/exp/ams/Offline/oracle/admin';
#$ENV{"TNS_ADMIN"}='/opt/oracle/10g/network/admin';
    $ENV{"LD_LIBRARY_PATH"}=$ENV{"ORACLE_HOME"}."/lib";
    1;
}

sub updateoracle{
    my $self=shift;
    my $subj=shift;
    my $hosts=shift;
    my $curtim=time();
    my $category="U";
   
    if($subj=~/-I-/){
        $category="I";
    }
    elsif($subj=~/-W-/){
        $category="W";
    }
    elsif($subj=~/-E-/){
        $category="E";
    }
    elsif($subj=~/-S-/){
        $category="S";
    }
#
# delete all the messages less than retention period
#
    my $deltim=$curtim-$self->{sqlserver}->{keep};
    my $sql="delete from netmon where timestamp<$deltim";
    if(not $self->{sqlserver}->{dbhandler}->do($sql)){
        return 0;
    }
    $sql="insert into netmon values('$subj','$hosts',$curtim,'$category')";
    if(not $self->{sqlserver}->{dbhandler}->do($sql)){
        return 0;
    }
    $self->{sqlserver}->{lastupdate}=$curtim;
    return 1;
}
