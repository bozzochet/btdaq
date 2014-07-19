package Mover;
#
# Last Edit : Apr 24, 2003. ak.
#
my $help = "
# Program copies files from 'source' directory to 'target'.
# Program requests list of NTuples from server (or finds it in database), 
# finds NTuples with status Success or Validated and directory 
# path matches to 'source' directory, validates NTuples if 
# its status is Success, copies NTuples to Target directory 
# validates them, sends new NTuple path to server and 
# remove copied NTuples from 'source' directory
#
# mover.cgi -S/f2users/scratch/MC/ciemat/nt -T/f2dah1/MC/AMS02/2003Trial/ -db -pwdPASSWD -v
#
# arguments :
# -S    - source files path
# -T    - target directory path
# -h    - help
# -db   - database mode (no connection to server)
# -ior  - path to file with IOR                (optional)
# -pwd  - pass to access Oracle
# -v    - verbose                              (optional)
#
# March 22, 2002. A.Klimentov
";

 use CORBA::ORBit idl => [ '../include/server.idl'];
use Error qw(:try);
use Carp;
use strict;
use lib::CID;
use lib::ActiveClient;
use lib::POAMonitor;
use Sys::Hostname;

#use lib::DBServer;
use Time::Local;

use DBI;

@Mover::EXPORT= qw(new Connect doCopy);


my %fields=(
    cid=>undef,
    orb=>undef,
    root_poa=>undef,
    mypoamonitor=>undef,
    myref=>undef,
    myior=>undef,
    arsref=>[],
    arpref=>[],
    AMSDataDir=>undef,
    AMSSoftwareDir=>undef,
    verbose=>undef,
    database=>undef,
    ac=>undef,
    start=>undef,
    hostname=>undef,
    source=>undef,
    target=>undef,
    sourcedir=>undef,
    targetdir=>undef,
    sourcefile=>undef,
    sourcestr1=>undef,
    sourcestr2=>undef,
    sourcestr3=>undef,
    ok=>undef,
    registered=>0,
    DataMC=>1,
    IOR=>undef,
    dbhandler=>undef,
    dbpwd=>undef
            );


sub new {
    if($ENV{MOD_PERL} and ref($Mover::Singleton)){
        return $Mover::Singleton;
    }
#
my $input;
my $output;
my $idir;
my $ifile;
my $cmd;
my $logfile;
my $status;
my $time;
#
    my $type=shift;
    my $self={
              %fields,
          };
    $self->{start}=time();
    $self->{verbose} = 0;


# check sintaksis

foreach my $chop  (@ARGV){
    if($chop =~/^-S/){
        $input=unpack("x2 A*",$chop);
    }
    if($chop =~/^-T/){
        $output=unpack("x2 A*",$chop);
    }

    if($chop =~/^-v/){
        $self->{verbose} = 1;
    }

    if ($chop =~/^-db/) {
        $self->{database} = 1;
    }

    if ($chop =~/^-pwd/) {
        $self->{dbpwd} = unpack("x4 A*",$chop);
    }

    if($chop =~/^-h/){
        print $help;
        die "...";
    }
}


if (not defined $input or not defined $output) {
    die "move -E- Invalid sintaksis.   move -S/inputDir/files -T/outputDir/";
 }

# get hostname
  my $hostname = hostname();
  my @host = split '.',$hostname;
  if (defined $host[0]) {
   $self->{hostname}=$host[0];
  } else {
   $self->{hostname}=$hostname;
  } 
 if ($self->{verbose}) {
     print "Hostname : $self->{hostname} \n";
 }
# check dirpath

if (substr($input, 0, 1) ne '/' or substr($output, 0, 1) ne '/' ) {
    print "move $input $output\n";
    die "move -E- Invalid sintaksis.  Specify directory explicitly";
}
    $self->{source} = $input;
    $self->{target} = $output;
#
my ($f1,$f2,$f3) = split(/\*/,$input);
if (defined $f1) {
    $self->{sourcestr1} = $f1;
}
if (defined $f2) {
    if (length($input) < length($f1)+1) {
     $self->{sourcestr2} = $f2;
    } 
}
# input directory, file
  ($idir,$ifile) = parseArgs($input); 
  if (not defined $idir or not defined $ifile) {
    die "move -E- cannot parse input.  Specify directory and file ";
  }
  $self->{sourcedir}  = $idir;
  $self->{sourcefile} = $ifile;
# output directory
   $self->{targetdir} = addslash($output);
  
# check disk space 
  my $isize  = getTotalSize($input);
  if ($isize < 1) {
      die "empty directory :  $input";
  }
  my $osize  = getDiskSize($output);
  if ($osize < $isize) {
      print "$cmd\n";
      print "$input  - total $isize KBytes\n";
      print "$output - space left $osize\n";
      die "not enough space on $output";
  }

if (defined $self->{database}) {
   set_oracle_env();
   $self->{dbhandler}=connect_to_oracle($self->{dbpwd});
} 
# Corba part a la new in Monitor.pm
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
#
#
#
#amsdatadir

if (defined $self->{database}) {
    my $key = 'AMSDataDir';
    my $sql = "select myvalue from Environment where mykey='".$key."'";
    my $dbh = $self->{dbhandler};
    my $ret =  Query($dbh,$sql);
    if (defined $ret->[0][0]) {
        $self->{AMSDataDir} = $ret->[0][0];
        $ENV{AMSDataDir}=$self->{AMSDataDir};
    }
    $key='AMSSoftwareDir';
    $sql = "select myvalue from Environment where mykey='".$key."'";
    $ret = Query($dbh,$sql);
    if (defined $ret->[0][0]) {
        $self->{AMSSoftwareDir} = $ret->[0][0];
        $ENV{AMSSoftwareDir}=$self->{AMSSoftwareDir};
    }
} else {
 my $dir=$ENV{AMSDataDir};
 if (defined $dir){
    $self->{AMSDataDir}=$dir;
 }
 else{
    $self->{AMSDataDir}="/f0dat1/AMSDataDir";
    $ENV{AMSDataDir}=$self->{AMSDataDir};
 }
   $dir=$ENV{AMSSoftwareDir};
 if (defined $dir){
    $self->{AMSSoftwareDir}=$dir;
 }
 else{
    $self->{AMSSoftwareDir}="/offline/vdev";
    $ENV{AMSSoftwareDir}=$self->{AMSSoftwareDir};
 }
}

if ($self->{verbose}) {
    print "Environment settings AMSDataDir : $self->{AMSDataDir}\n";
    print "Environment settings AMSSoftwareDir : $self->{AMSSoftwareDir}\n";
}
#

 my $mybless=bless $self,$type;
 if(ref($Mover::Singleton)){
    croak "Only Single Mover Allowed\n";
 }
  $Mover::Singleton=$mybless;
  return $mybless;
}




sub doCopy {
    my $cmdstatus=>undef;
    my @cpntuples=(); # full path to input directory files
    my @mvntuples=(); # full path to output directory files

    my @cmplruns =(); # runs 

    my $ntvalid  = 0; # number of validated ntuples
    my $ntproblem= 0; # number of ntuples not found/not validated
    my $self = shift;
    my $input  = $self->{source};
    my $output = $self->{targetdir};
    my $time = time();
    my $logfile = "/tmp/cp.$time.log";
    my $cmd = "touch $logfile";
    $cmdstatus = system($cmd);

    if (defined $self->{database}) {
       my $dbh = $self->{dbhandler};
       my $dir=$input;
       opendir THISDIR ,$dir or die "unable to open $dir";
       my @allfiles= readdir THISDIR;
       closedir THISDIR;
       foreach my $file (@allfiles){
         if ($file =~/^\./){
          next;
         }
         my $newfile=$dir."/".$file;
         my $sql = "SELECT jid, status, crc, nevents FROM ntuples WHERE path='$newfile'";
         my $ret = Query($dbh,$sql);
         if (defined $ret->[0][0]) {
             foreach my $nt (@{$ret}) {
                 my $jid    = $nt->[0];
                 my $status = $nt->[1];
                 my $crc    = $nt->[2];
                 my $events = $nt->[3];
                  $sql  = "SELECT cites.name,jobname,jobs.jid  FROM jobs,cites 
                           WHERE jid=$jid AND cites.cid=jobs.cid";
                  my $r1 = Query($dbh,$sql);
                  if (defined $r1->[0][0]) {
                   foreach my $job (@{$r1}) {
                     my $cite    = $job->[0];
                     my $jobname = $job->[1];
                     $jobname =~ s/$cite.//;
                     $jobname =~ s/$jid.//;
                     $jobname =~ s/.job//;
                     my $outputpath = $output.$jobname."/".$file;
                     push @cpntuples,$outputpath;
                     push @mvntuples,$newfile;
                     my $cmd = "cp -pi -d -v -r $newfile  $outputpath >> $logfile";
                     $cmdstatus = system($cmd);
                     if ($cmdstatus == 0) {
                      my $vstatus = "BAD";
                      if ($newfile =~ m/hbk/) {
                       $vstatus=$self->doValidate($outputpath,$events);
                      } else {
                          print "doCopy - no validation for ROOT files : $outputpath\n";
                          $vstatus = "OK";
                      }
                       if ($vstatus ne "OK") {
                        print "doCopy -W- validation failed for $outputpath \n";
                        print "doCopy -I- delete all previuosly copied NTuples and exit\n";
                        $cmdstatus = -1;
                       } else {
                          if ($self->{verbose}) {
                              print "doCopy -I- validated : $outputpath \n";
                          }
                        $sql = "UPDATE ntuples SET path='$outputpath', status='Validated', timestamp=$time 
                                WHERE path='$newfile'";
                        push @cmplruns, $jid;

                        my $dbh=$self->{dbhandler};
                        my $sth=$dbh->prepare($sql)  or die "Cannot prepare $sql ".$dbh->errstr();
                        $sth->execute or die "Cannot execute $sql ".$dbh->errstr();
                        $ntvalid++;
                      }     
                     } 
                     if ($cmdstatus != 0) {
                      print "doCopy -E- Copy or validation failed :  $cmd \n";
                      $self->doRemove(@cpntuples);
                      goto LAST;
                     }
                   }
                  } else {
                   print "Warning - Job : $jid, cannot find  description in Jobs and Cites tables\n";
                   $ntproblem++;
                  }
             }
          } else {
              print "Warning - NTuple : $newfile not found in database table ntuples\n";
              $ntproblem++;
          }
         }
         if ($ntvalid > 0) {
             my $sql;
             my $sth;
             my $dbh;
             print "doCopy - ntuples validated :  $ntvalid  \n";
             print "doCopy - not validated and not copied : $ntproblem  \n";

             foreach my $run (@cmplruns) {
                 $sql = "UPDATE runs SET status='Completed' WHERE run=$run";
                 $dbh=$self->{dbhandler};
                 $sth=$dbh->prepare($sql)  or die "Cannot prepare $sql ".$dbh->errstr();
                 $sth->execute or die "Cannot execute $sql ".$dbh->errstr();
             }
             $sql = "commit";
             $dbh=$self->{dbhandler};
             $sth=$dbh->prepare($sql)  or die "Cannot prepare $sql ".$dbh->errstr();
             $sth->execute or die "Cannot execute $sql ".$dbh->errstr();

             doRemove(@mvntuples);

             $cmdstatus = 1;
         }
    } else {
     my $ref = $Mover::Singleton;
     my $stat= $ref->readServer();
     if ( not $stat) { 
      die "Quit due to previous error";
     }
     my @data = $ref->getNTuples($input);
     my $nnt = $#data/15;
     if ($nnt > 0) {
      if ($self->{verbose}) {
         print "doCopy -I- $nnt files will be copied\n";
      }
      my $i = 0;
      while ($i < $#data) {
       my ($host,$ntuple) = split(/:/,$data[$i+1]);
       my $events         = $data[$i+2];
       $ntuple=~s/\/\//\//;
       my ($dir,$file) = parseArgs($ntuple);
       my $tpath = $output.$file;
       push @cpntuples,$tpath;
       push @mvntuples,$ntuple;
       $cmd = "cp -pi -d -v -r $ntuple  $tpath >> $logfile";
       $cmdstatus = system($cmd);
       if ($cmdstatus == 0) {
        my $vstatus=$self->doValidate($tpath,$events);
        if ($vstatus ne "OK") {
            print "doCopy -W- validation failed for $tpath \n";
            print "doCopy -I- delete all previuosly copied NTuples and exit\n";
            $cmdstatus = -1;
        }
      }
      if ($cmdstatus != 0) {
       print "doCopy -E- Copy failed :  $cmd \n";
       $self->doRemove(@cpntuples);
       goto LAST;
      }
      $i=$i+16;
   }
   my $cpnt = $#cpntuples+1;
   if ($self->{verbose}) {
    print "doCopy -I- $cpnt files are copied to $output\n";
   }
  } else {
    print "doCopy -I- Nothing to copy\n";
    print "doCopy -I- No Validated NTuples or/and NT's paths don't match to $input\n";
  }  
  if ($nnt > 0) {
#
# Update NTuples path
# sendDSTEnd($action="Create","Delete")
#
     my $host = $self->{hostname};
     updateNTuples("Create",$host,$output,@data);
     updateNTuples("Delete",$host,$output,@data);
     doRemove(@mvntuples);
     $cmdstatus = 1;
   }
  }
LAST : 
    return $cmdstatus;
}


sub readServer {
#
    my $ref=shift;
    my $ok=$ref->UpdateARS();
    if(not $ok) {
       return 0;
   }
    my $arsref;
        foreach $arsref (@{$ref->{arpref}}){
            my $length;
            my $ahl;
            try{
                my %cid=%{$ref->{cid}};
                $cid{Type}="Producer";
                ($length,$ahl)=$arsref->getDSTS(\%cid);
#                ($length,$ahl)=$arsref->getDSTInfoS(\%cid);
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
LAST :
    return 1;
}


sub getNTuples {

#
# get from Server NTuples with status "Validated" and "Success" 
# NT's with status "Success" must be validated
#
    my $self = shift;
    my $path = shift;
    my @data = ();
    my $events;
    my $ntuple;
#
    my @sort=("Validated", "Success");
        foreach my $sort (@sort) {    
        for my $i (0 ... $#{$Mover::Singleton->{dsts}}){
            my $hash=$Mover::Singleton->{dsts}[$i];
            if($hash->{Type} eq "Ntuple"){
                if($hash->{Status} eq $sort){
# suppress double //
                    $ntuple    = $hash->{Name};
                    $events    = $hash->{EventNumber};
                    $ntuple=~s/\/\//\//;
                    my @fpatha=split ':', $ntuple;
                    my $fpath=$fpatha[$#fpatha];
                    my ($dir,$file) = parseArgs($fpath);
                    print "$sort $dir $file \n";
                    if ($dir eq $self->{sourcedir}) {
# check filename here
                     my $sstr1;
                     my $sstr2;
                     my $sstr;
                     if (defined $self->{sourcestr1}) {
                         $sstr1 = $self->{sourcestr1};
                         $sstr    = substr($fpath,0,length($sstr1));
                         if ($sstr ne $sstr1) {
                             goto NEXTNT;
                         }
                     }
                     if (defined $self->{sourcestr2}) {
                         $sstr2 = $self->{sourcestr2};
                         $sstr  = substr($fpath,length($sstr1)+1,length($fpath));
                         if ($sstr ne $sstr2) {
                             goto NEXTNT;
                         }
                     }
                     if ($sort ne "Validated") {
                       my $status = $self->doValidate($ntuple,$events);
                       if ($status ne "OK") {
                           print "getNTuples -W- $ntuple not validated\n";
                           print "getNTuples -W- returned status : $status\n";
                           print "getNtuples -W- skip it\n";
                       } else {
                           $sort = "Validated";
                       }
                   }
                   if ($sort eq "Validated") {
                    push @data,$i,$ntuple,$events,
                                    $hash->{Insert},$hash->{Begin},$hash->{End},
                                    $hash->{Run},$hash->{FirstEvent},$hash->{LastEvent},
                                    $hash->{EventNumber},$hash->{ErrorNumber},
                                    $hash->{Status},$hash->{Type},$hash->{size},
                                    $hash->{Name},$hash->{Version};
                  }
                 }
                  
                }
            }
NEXTNT :
        }
    }
    return @data;
}

sub updateNTuples {

#
# get from Server NTuples with status "Validated" and "Success" 
# NT's with status "Success" must be validated
#
   my %ntnc=(
         Insert=>undef,
         Begin=>undef,
         End=>undef,
         Run=>undef,
         FirstEvent=>undef,
         LastEvent=>undef,
         EventNumber=>undef,
         ErrorNumber=>undef,
         Status=>undef,
         Type=>undef,
         size=>undef,
         Name=>undef,
         Version=>undef,
           );

   my ($action,$host,$path,@data) = @_;
   my $ref=$Mover::Singleton;
   my $i = 0;
#   my $j = 0;
#   my %nc=%{${$ref->{dsts}}[0]};  
   while ($i < $#data){
    my $row = $data[$i];
    $ntnc{Insert}      = $data[$i+3];
    $ntnc{Begin}       = $data[$i+4];
    $ntnc{End}         = $data[$i+5];
    $ntnc{Run}         = $data[$i+6];
    $ntnc{FirstEvent}  = $data[$i+7];
    $ntnc{LastEvent}   = $data[$i+8];
    $ntnc{EventNumber} = $data[$i+9];
    $ntnc{ErrorNumber} = $data[$i+10];
    $ntnc{Status}      = "Validated";
    $ntnc{Type}        = $data[$i+12];
    $ntnc{size}        = $data[$i+13];
    if ($action eq 'Create') {
      my ($h,$ntuple)=split(/:/,$data[$i+1]);
      my ($dir,$file) = parseArgs($ntuple);
      $ntnc{Name}        = $host.":".$path.$file;
    }
    if ($action eq 'Delete') {
      $ntnc{Name}        = $data[$i+14];
    }
    $ntnc{Version}     = $data[$i+15];
    $i=$i+16;
                       
    my $arsref;
    my %cid=%{$ref->{cid}};

    foreach $arsref (@{$ref->{arpref}}){
       try{
        $arsref->sendDSTEnd(\%cid,\%ntnc,$action);
       }
       catch CORBA::SystemException with{
           warn "sendback corba exc";
       };
   }
 }
}

sub doValidate {
#
# validate output from RemoteClient.pm  L645-693
#
    my $self   = shift;
    my $ntuple = shift;
    my $events = shift;
    my $vexe   = $self->{AMSDataDir}."/".$self->{AMSSoftwareDir}."/exe/linux/fastntrd.exe";
    my @fpatha=split ':', $ntuple;
    my $fpath=$fpatha[$#fpatha];
    if ($self->{verbose}) {
        print "doValidate -I- by $vexe $fpath $events\n";
    }
    my $suc=open(FILE,"<".$fpath);
    my $status="OK";
    if(not $suc){
      $status="Unchecked";
    }
    else{
     close FILE;
     my $i=system("$vexe $fpath $events");
     if( ($i == 0xff00) or ($i & 0xff)){
                $status="Unchecked";
            }
        else{
            $i=($i>>8);
            if(int($i/128)){
                $status="Bad".($i-128);
            }
            else{
                $status="OK";
            }
        }
    }
    return $status;
}


sub doRemove {
# delete input files
    my $time = time();
    my (@ntuples) = @_;
    print "doRemove -I- Remove all copied NTuples \n";
    my $logfile = "/tmp/aarm.$time.log";
    foreach my $rmntuple (@ntuples) {
     my $cmd = "rm -i  $rmntuple >> $logfile";
     my $status = system($cmd);
    }
}
sub parseArgs {
  
  my $string = shift;
  my $slash = "/";
  my @chars = split(//,$string);
  my $dir;
  my $file;
  my $i = 0;
  my $s = 0;
  my $j = 0;
  foreach my $char (@chars) {
      if ($char eq $slash)  {
          $s = $i;
      }
      $i++;
  }
 $dir= $chars[$j];
 $j++;
 while ($j<$s+1) {
     $dir= $dir.$chars[$j];
     $j++;
 }
  $file= $chars[$j];
  $j++;
  while ($j<$i) {
     $file= $file.$chars[$j];
     $j++;
 }
  return($dir,$file);
}

sub addslash {
  my $string = shift;
  my $slash = "/";
  my @chars = split(//,$string);
  my $i = 0;
  foreach my $char (@chars) {
      $i++;
  }
  if ($chars[$i-1] ne $slash) {
      $string = $string.$slash;
  }
  return $string;
}

sub getTotalSize {
  my $dir = shift;
  my $size = 0;
  my $file;
  my $time = time();
  my $du_file  = "/tmp/aadu".$time;
  my $cmd = "du -k -s -c $dir > $du_file";
  my $status = system($cmd);
  if ($status == 0) {
    open(DUINPUT,"<".$du_file) or die "cannot open $du_file for read";
    while (<DUINPUT>) {
        ($size, $file) = split;
    }
    close (DUINPUT);
 } else {
     die "cannot execute $cmd";
 }
  return $size;
}

sub getDiskSize {
  my $dir = shift;
  my $size = 0;
  my $time = time();
  my $du_file  = "/tmp/aadu".$time;
  my $cmd = "df -k  $dir > $du_file";
  my $status = system($cmd);
  if ($status == 0) {
    open(DUINPUT,"<".$du_file) or die "cannot open $du_file for read";
    while (<DUINPUT>) {
      my ($fs, $tsize, $used, $avail, $perc, $mnt) = split;
      $size = $avail;
    }
    close (DUINPUT);
 } else {
     die "cannot execute $cmd";
 }
  return $size;
}
sub ErrorPlus{
    my $ref=shift;
    die shift;
}


# Corba Relates Subs

sub Connect{
   my $ior;
   my $ref=shift;
   if (not defined $ref->{database}) {
    $ref->{ok}=0;
    if(defined $ref->{IOR}){
        return 1;
    }
# check -IOR first and only then getior
    foreach my $chop  (@ARGV){
        if($chop =~/^-ior/){
             my $iorfile = unpack("x4 A*",$chop);
             my $buf;
             open(FILE,"<".$iorfile) or die "Unable to open file [$iorfile] \n";
             read(FILE,$buf,1638400);
             close FILE;
             $ior=$buf;
             print "Connect -I- IOR read from $iorfile \n";
         }
        
    }
    if (not defined $ior) {
     $ior=getior();
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
  } else {
    $ref->{ok}=1;
  }
    return $ref->{ok};
}

sub getior{
    my $pid=$Mover::Singleton->{cid}->{pid};
    my $file ="/tmp/o."."$pid";
    my $fileo ="/tmp/oo."."$pid";
    my $i=system "/usr/local/lsf/bin/bjobs -q linux_server -u all>$file" ;
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
                        if($tag eq $Mover::Singleton->{DataMC}){
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
    my $file ="/tmp/DumpIOR".".$Mover::Singleton->{DataMC}";
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
               carp "getARS 2 oops SystemException Error "."\n";
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

# this function sets up the necessary environement variables
# to be able to connect to Oracle
sub set_oracle_env {
    $ENV{"ORACLE_HOME"}='/afs/cern.ch/project/oracle/@sys/prod';
    $ENV{"TNS_ADMIN"}='/afs/cern.ch/project/oracle/admin';
    $ENV{"LD_LIBRARY_PATH"}=$ENV{"ORACLE_HOME"}."/lib";
    1;
}

sub connect_to_oracle {

    my $user   = "amsdes";
    my $pwd    =  shift;
    my $dbname = "DBI:Oracle:amsdb";

    my $dbh = DBI->connect( $dbname,$user, $pwd, 
                      ) || die print "Can't connect : $DBI::errstr";

    return $dbh;
}
sub Query {

    my $dbh =shift;
    my $sql  =shift;
    my $sth=$dbh->prepare($sql)  or die "Cannot prepare $sql ".$dbh->errstr();
    $sth->execute or die "Cannot execute $sql ".$dbh->errstr();
    my $ret=$sth->fetchall_arrayref();
    $sth->finish();
    return $ret;
}
sub trimblanks {
    my @inp_string = @_;
    for (@inp_string) {
        s/^\s+//;        
        s/\s+$//;
    }
    return wantarray ? @inp_string : $inp_string[0];
}
