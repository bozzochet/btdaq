# $Id: DBSQLServer.pm,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $

#
#
# Input parameters :
#                   -Iforce    - dbinit 
#                                    2 - recreate all tables
#                   -Ddbdriver - database driver 
#                                                oracle
#                                                mysql
#                   -Fdbfile   - database name or databasefile
#                                (can also include host name)
#                                for mysql : AMSMC02:pcamsf0
#                   -zero      - optional parameter to not autocommit
# example :
#          mySQL
#           ./dbsqlserver.perl -Iforce -Dmysql -F:AMSMC02:pcamsf0
#          Oracle (from pcamsf0)
#           ./dbsqlserver.perl -Iforce -DOracle -F:amsdb
#
# Aug  7, 2002 a.k. Tables names are case sensitive in MySQL
# Mar 30, 2003 a.k. '/vicepa,b,c' directory paths
# Apr  2, 2003 a.k. create indecies for RNDM table (it takes a while)
#                   insert CITES and MAILS info from ../doc/mc.cites.mails
#                          FILESYSTEMS          from ../doc/mc.filesystems
# Apr 21, 2003 a.k.        JournalFiles         from ../doc/mc.journals
# to add  - standalone/client type,
#          CPU, elapsed time, cite and host info into Job table
#
#  Apr 24, 2003. ak. Tables : Jobs, Runs, RNDM - add primary key
#  Oct  2, 2003. ak. Table  : LocalHosts
#  Jan 31, 2004. ak. Table  : Ntuples add CRCtime, CRCflag, CASTORtime
#  Feb 23, 2004. ak. Table  : FilesProcessing
#  May 19, 2004. ak. Table  : MC_RemoteDSTPath,  MC_DSTCopy
#  Nov 23, 2004. ak. Table  : DatasetDesc, filled from $AMSSoftwareDir/Datasets
#  Feb 15, 2005. ak. Table  : productionset add db, gbatch, OS version
#                             datasets      add version (db version)
#  Mar 18, 2005. ak. Table  : Jobs add pid - production period ID
#
package DBSQLServer;
use Error qw(:try);
use lib::CID;
use Carp;
use strict;
use Carp;
use Fcntl ":flock";
use Fcntl;
use POSIX qw(tmpnam);
use POSIX qw(strtod);             
use MLDBM qw(DB_File Storable);
use DBI;
use Time::localtime;

my $MCCitesMailsFile = "../doc/mc.cites.mails"; # file with cites and mails definitions
my $MCFilesystems="../doc/mc.filesystems";      # file with filesystems definitions

@DBSQLServer::EXPORT= qw(new Connect QueryAll Query Update Commit set_oracle_env);
my %fields=(
     start=>undef,
     cid=>undef,
     dbhandler=>undef,
     dbdriver=>undef,
     dbfile=>undef,
     dbdefaultfile=>"DataManagement/CSV",
     dbdefaultdriver=>"CSV:f_dir=",
     dbinit=>0,
             );

my $MAX_RUN_POWER = 26;

sub new{
    if($ENV{MOD_PERL} and ref($DBSQLServer::Singleton)){
        return $DBSQLServer::Singleton;
    }


    my $type=shift;
    my $self={
        %fields,
    };
$self->{start}=time();
$self->{cid}=new CID("DBSQLServer");    
$self->{one}=1;
foreach my $chop  (@ARGV){
    if($chop =~/^-F/){
     $self->{dbfile}=unpack("x2 A*",$chop);
    }
    if($chop =~/^-D/){
     $self->{dbdriver}=unpack("x2 A*",$chop);
    }
    if($chop=~/^-zero/){
     $self->{one}=0;
    }
    if($chop =~/^-I/){
        $self->{dbinit}=unpack("x2 A*",$chop);
        if($self->{dbinit} eq "force"){
            $self->{dbinit}=2;
        }
        else{
            $self->{dbinit}=1;
        }
    }
}

    if(not defined $self->{dbdriver}){
        $self->{dbdriver}=$self->{dbdefaultdriver};
    }

    if(not defined $self->{dbfile}){
        $self->{dbfile}=$self->{dbdefaultfile};
    }
    my $dir=$ENV{AMSDataDir};
    if(defined $dir and $self->{dbdriver} =~ m/CSV/){
        $self->{dbfile}=$dir.'/'.$self->{dbfile};
    }        
    elsif($self->{dbdriver} =~ m/CSV/){
        die "DBSQL Server AMSDataDir Not Defined";
    }

    my $mybless=bless $self,$type;
    if(ref($DBSQLServer::Singleton)){
        croak "Only Single DBSQLServer Allowed\n";
    }
$DBSQLServer::Singleton=$mybless;
return $mybless;
}

sub Connect{
    my $self=shift;
    my $user   = "amsdes";
    my $pwd;
    if($self->{dbdriver} =~ m/Oracle/){
     set_oracle_env();
    }
    my $oracle="/var/www/cgi-bin/mon/lib/.oracle.oracle";
    open(FILE,"<".$oracle) or die "Unable to open file $oracle \n";
    while  (<FILE>){
        $pwd=$_;
    }
    close FILE;
#- interactive debug
#    print "dbinfo.......DBI:$self->{dbdriver}$self->{dbfile} \n";
#    print "user.........$user identified by pwd\n";
#    print "init level...$self->{dbinit}\n";
#-
    if($self->{dbinit}){
    $self->{dbhandler}=DBI->connect('DBI:'.$self->{dbdriver}.$self->{dbfile},$user,$pwd,{PrintError => 0, AutoCommit => $self->{one}}) or die "Cannot connect: ".$DBI::errstr;
      $self->Create();
    }
    else{
    $self->{dbhandler}=DBI->connect('DBI:'.$self->{dbdriver}.$self->{dbfile},$user,$pwd,{PrintError => 1, AutoCommit => $self->{one}}) or die "Cannot connect: ".$DBI::errstr;
        return 1;
    }
}

sub Create{
# creates a database
    my $self=shift;
    my $dbh=$self->{dbhandler};


    my @tables=("Filesystems", "Cites","LocalHosts","Journals","Mails" ,"Jobs", "DatasetDesc", "RNDM","Servers", "Runs","Ntuples","DataSets", "DataSetFiles", "Environment","ProductionSet","FilesProcessing","RemoteCopy");
    my @createtables=("    CREATE TABLE Filesystems
     (   
     host    VARCHAR(40),            
     disk    VARCHAR(20),            
     path    VARCHAR(255),           
     totalsize    INT,               
     occupied     INT,               
     available    INT,               
     allowed      INT,               
     status   CHAR(40),              
     priority     INT,               
     timestamp    INT,
     isonline       INT)",             

     "CREATE TABLE Cites
     (cid      INT NOT NULL ,
      name     VARCHAR(64),
      mid      INT,
      status   VARCHAR(255),
      maxrun   INT,
      state    INT,
      descr    VARCHAR(255),
      ghz      INT,
      timestamp INT)",

     "CREATE TABLE LocalHosts
     (hid       INT NOT NULL ,
      name      VARCHAR(64),
      os        VARCHAR(64),
      cpunumber INT,
      memory    INT,
      clock     INT,
      interface VARCHAR(64),
      status   VARCHAR(255),
      timestamp INT)",

     "CREATE TABLE Journals
     (cid       INT NOT NULL ,
      dirpath   VARCHAR(255),
      lastfile  VARCHAR(255),
      timelast  INT,
      timestamp INT)",

      "CREATE TABLE Mails
      (mid INT NOT NULL ,
       address VARCHAR(255),
       alias   VARCHAR(255),
       name    VARCHAR(64),
       rsite   INT,
       rserver INT,
       cid     INT,
       status  VARCHAR(16),
       requests INT,
       timestamp INT,
       timeu1    INT,
       timeu2    INT)",
      "CREATE TABLE Jobs 
      (jid        INT NOT NULL primary key,
       jobname    VARCHAR(255),
       mid        INT,
       cid        INT,
       did        INT,
       time       INT,
       triggers   INT,
       timeout    INT,
       content    TEXT,
       timestamp  INT,
       nickname   VARCHAR(80),
       host       VARCHAR(80),
        events    INT,
        errors    int,
        cputime   int,
        elapsed   int,
        jobtype   VARCHAR(20),
        mips      number,
        pid       number,
        realtriggers int,
        timekill int)",

      "CREATE TABLE  DataSetsDesc 
       (did        int not null,
        dataset    varchar(16),
        dirpath    varchar(128),
        jobname    varchar(32),
        jobdesc    varchar(128),
        jobdata    varchar(4000),
        timeread   int,
        timeupdate int)",


      "CREATE TABLE RNDM
      (rid     INT NOT NULL primary key,
       rndm1   INT,
       rndm2   INT)",
      "CREATE TABLE Servers
       (dbfilename VARCHAR(255) NOT NULL,
        IORS   TEXT NOT NULL,
        IORP   TEXT NOT NULL,
        IORD   TEXT NOT NULL,
        status VARCHAR(64),
        createtime INT,
        lastupdate INT)",
       "CREATE TABLE Runs
        (run   INT NOT NULL primary key,
         jid   INT,
         FEvent INT,
         LEvent INT,
         FETime INT,
         LETime INT,
         submit INT, 
         status VARCHAR(64))",
        "CREATE TABLE Ntuples
        (run        INT NOT NULL,
         version    VARCHAR(64),
         type       VARCHAR(64),
         jid        INT,
         FEvent     INT,
         LEvent     INT,
         NEvents    INT,
         NEventsErr INT,
         timestamp  INT,
         sizemb     INT,
         status     VARCHAR(64),
         path       VARCHAR(255),
         crc        INT,
         crctime    INT,
         crcflag    INT,
         castortime INT,
         buildno    INT)",
        "CREATE TABLE MC_DST_Copy
          (
           run       int not null,
           path      VARCHAR(128),
           prefix    VARCHAR(128),
           copytime  INT,
           cite      VARCHAR(64),
           timestamp INT)",
        "CREATE TABLE DataSets
         (did     INT NOT NULL,
          name    VARCHAR(255),
          version VARCHAR(12),
          timestamp int)",
        "CREATE TABLE ProductionSet 
         (did        INT NOT NULL,
          name       VARCHAR(20),
          begin      int,
          end        int,
          status     char(10),
          version    char(64),
          vos        char(6),
          vgbatch    char(12),
          vdb        char(12),
          description VARCHAR(255))",
        "CREATE TABLE FilesProcessing  
         (
          cites      int,
          active     int,
          jou        int,
          good       int,
          failed     int,
          gooddsts   int,
          baddsts    int,
          flag       int,
          timestamp  int)",
        "CREATE TABLE DataSetFiles 
         (did        INT NOT NULL,
          name       VARCHAR(255),
          filename   VARCHAR(255),
          timestamp int)",
        "CREATE TABLE Environment
         (mykey    VARCHAR(255),
          myvalue   VARCHAR(255))"
         );
# status ntuples:  OK, Unchecked, Errors
#      
#   check if tables exist
    my $i=0;
    foreach my $table (@tables){
#    if($i==0){
#     $i++;
#     next;
#    } 
    if($self->{dbinit}<2){
       print "Create -I- check table $table \n";
       my $ok =$dbh->do("select * from $table");
      if(defined $ok){
        warn "Table $table already exist";
        $i=$i+1;
        next;
      }
    }
    if($self->{dbdriver} =~ m/CSV/){
         system "mkdir -p $self->{dbfile}";
         system "rm -f $self->{dbfile}"."/"."$table";
     }
     elsif($self->{dbdriver}=~ m/Oracle/){
         $createtables[$i] =~ s/TEXT/VARCHAR2(4000)/g;
     }        
    $dbh->do("drop table ".$tables[$i]);
    my $sth=$dbh->prepare($createtables[$i]) or die "cannot prepare: ".$dbh->errstr();
    print " $createtables[$i] \n";
    $sth->execute() or die "cannot execute: ".$dbh->errstr();
    $sth->finish();    
     if ($tables[$i] eq "Servers" and $self->{dbdriver} =~ m/Oracle/) {
         $sth=$dbh->prepare("grant select on servers to amsro") or die "cannot prepare".$dbh->errstr();
         $sth->execute() or die "cannot execute: ".$dbh->errstr();
         $sth->finish();    
     }
    print "$tables[$i]\n";
     if($self->{dbdriver} =~ m/CSV/){
         system "chmod o+w $self->{dbfile}"."/"."$table";
     }
     $i=$i+1;
 }

#

# initialize rndm table;

# my $RNDMTable="/afs/cern.ch/user/b/biland/public/AMS/rndm.seeds";
my $RNDMTable="/scratchA/groupA/biland/seed_1E12";

my $cntr = 0;
my $cnt  = 0;
my $sql;

   if($self->{dbdriver} =~ m/Oracle/){
    print "Check RNDM table \n";
    $sql="SELECT COUNT(rid) FROM RNDM";
    $cntr=$self->Query($sql);
     foreach my $ret (@{$cntr}) {
        $cnt = $ret->[0];
     }
    }
    if ($cnt == 0) {
     print "...and fill it from $RNDMTable \n";
     open(FILEI,"<".$RNDMTable) or die "Unable to open file $RNDMTable\n";
     my $line;
     while ( $line = <FILEI>){
       my @pat=split / /,$line;
       my @arr;
      foreach my $chop (@pat){
        if($chop  =~/^\d/){
            $arr[$#arr+1]=$chop;
        }
     }
     $dbh->do("insert into RNDM values($arr[0],$arr[1],$arr[2])") or die "cannot do: ".$dbh->errstr();     
    }    
    close(FILEI);
# create indecies, it takes a while.
    $dbh->do("create index rid_ind on rndm (rid)") or print "cannot do: ".$dbh->errstr(); 
   }

# initialize 
     if($self->{dbdriver} =~ m/Oracle/){
    $sql="SELECT COUNT(mykey) FROM Environment";
    $cntr=$self->Query($sql);
    foreach my $ret (@{$cntr}) {
        $cnt = $ret->[0];
    }
}
    if ($cnt == 0) {
     $dbh->do("insert into Environment values('AMSDataDir','/afs/ams.cern.ch/AMSDataDir')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('CERN_ROOT','/cern/2001')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('UploadsDir','/var/www/cgi-bin/AMS02MCUploads')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('UploadsHREF','AMS02MCUploads')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('AMSSoftwareDir','DataManagement')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('DataSets','DataSets')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('gbatch','exe/linux/gbatch-orbit.exe')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('ntuplevalidator','exe/linux/fastntrd.exe')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('getior','exe/linux/getior.exe')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('FileBBFTP','amsbbftp.tar.gz')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('FileBookKeeping','amsbookkeeping.tar.gz')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('filedb','ams02mcdb.tar.gz')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('filedb.att','ams02mcdb.att.tar.gz')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('dbversion','v4.00')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('AMSProdDir','prod')") or die "cannot do: ".$dbh->errstr();     
     my $apd='$AMSProdDir/prod/starttagmtb_db_mc';
     $dbh->do("insert into Environment values('amsserver','$apd')") or die "cannot do: ".$dbh->errstr();     
     $dbh->do("insert into Environment values('ValidationDirPath','/f2users/scratch/MC/validate/')") or die "cannot do : ".$dbh->errstr();
     $dbh->do("insert into Environment values('ROOTSYS','/afs/ams.cern.ch/Offline/root/Linux/pro/')") or die "cannot do : ".$dbh->errstr();
     $dbh->do("insert into Environment values('HTTPserver','pcamsf0.cern.ch')") or die "cannot do : ".$dbh->errstr();
      
 } else {
    warn "Table Environment has $cnt entries. Not initialized";
 }
# ProductionSet Table
    $cnt=0;
   if($self->{dbdriver} =~ m/Oracle/){
      $sql = "SELECT COUNT(did) from ProductionSet";
      $cntr=$self->Query($sql);
      foreach my $ret (@{$cntr}) {
        $cnt = $ret->[0];
      }
    } 
    if ($cnt == 0) {
     $dbh->do("insert into ProductionSet values(1,
                                                '2003Trial',
                                                1049891834,
                                                1054891834,
                                                'Finished',
                                                'v4.00/build78/os2',
                                                '1st try of distributed MC production')") 
           or die    "cannot do: ".$dbh->errstr();    
     $dbh->do("insert into ProductionSet values(2,
                                                '2003TrialA',
                                                1055135834,
                                                0,
                                                'Active',
                                                'v4.00/build79/os2',
                                                'MC production stress test')") 
           or die    "cannot do: ".$dbh->errstr();    
 }
#
    $cnt = 0;
   if($self->{dbdriver} =~ m/Oracle/){
    $sql="SELECT COUNT(cid) FROM Cites";
    $cntr=$self->Query($sql);
    foreach my $ret (@{$cntr}) {
        $cnt = $ret->[0];
    }
}
  if ($cnt == 0) {
   $dbh->do("delete journals") or die "cannot do: ".$dbh->errstr();    
   my $time=time();
   my $run=110;
   my $n  =  0;
   open(FILE,"<",$MCCitesMailsFile) or die "Unable to open $MCFilesystems";
   my @lines = <FILE>;
   close FILE;
   my $mails = 0;
   my $cites = 0;
   foreach my $line (@lines) {
   if ($line =~/^#/) {
     if ($line =~/MAILS/) {
         $mails = 1;
         $cites = 0;
     } 
     if ($line =~/CITES/) {
         $cites = 1;
         $mails = 0
     } 
   } else {
     if ($cites == 1) {
      my ($description,$name,$cid,$stat,$status,$journal,$ghz,@junk) = split ":",$line;
       if (defined $description and defined $status) {

         $description = trimblanks($description);    # cite description
         $name        = trimblanks($name);           #      nickname
         $cid         = strtod($cid);                #      id
         $stat        = strtod($stat);               #      state   - Active/Blocked
         $status      = trimblanks($status);         #      status  - Local/Remote
         $journal     = trimblanks($journal);        #      journal file directory
         $ghz         = strtod($ghz);                #      computing facility equiv in GHz

         print "$cid,$name,0,$status,$run,$stat,$description,$ghz,$time \n";
         $dbh->do("INSERT INTO Cites VALUES
                        ($cid,
                         '$name',
                         0,
                         '$status',
                         $run,
                         $stat,
                         '$description',
                         $ghz,
                         $time)")
         or die "cannot do: ".$dbh->errstr();
         my $dirpath=$journal."/".$name;    
         $dbh->do("INSERT INTO journals VALUES($cid,'$dirpath',' ',0,$time)")    
         or die "cannot do: ".$dbh->errstr();    
         $n++;
         $run=($n<<$MAX_RUN_POWER)+1;
     }
    }
   } 
  } # foreach my $line
      my $junkdir = "/f2users/scratch/MC/junk";
      $dbh->do("INSERT INTO journals VALUES(-1,'$junkdir',' ',0,$time)")  
      or die "cannot do: ".$dbh->errstr();    
 } else {
    warn "Table Cites has $cnt entries. Not initialized";
 }

    $cnt = 0;
   if($self->{dbdriver} =~ m/Oracle/){
    $sql="SELECT COUNT(fid) FROM Filesystems";
    $cntr=$self->Query($sql);
    foreach my $ret (@{$cntr}) {
        $cnt = $ret->[0];
    }
}
  if ($cnt == 0) {
   my $time=time();
   open(FILE,"<",$MCFilesystems) or die "Unable to open $MCFilesystems";
   my @lines = <FILE>;
   close FILE;

   my $fs = 0;
   my $fid= 0;
   foreach my $line (@lines) {
    if ($line =~/^#/) {
     if ($line =~/FILESYSTEMS/) {
         $fs = 1;
     } 
    } else {
     if ($fs == 1) {
         my ($host,$disk,$path,$total,$occup,$avail,$allow,$status,$prio,@junk) = split ":",$line;
         if (defined $host and defined $prio) {
          $host    = trimblanks($host);
          $disk    = trimblanks($disk);
          $path    = trimblanks($path);
          $total   = strtod($total);
          $occup   = strtod($occup);
          $avail   = strtod($avail);
          $allow   = strtod($allow);
          $status  = trimblanks($status);
          $prio    = strtod($prio);
          print "$host,$disk,$path,$total,$occup,$avail,$allow,$status\n";
          $dbh->do("INSERT INTO Filesystems VALUES
           ('$host','$disk','$path',$total,$occup,$avail,$allow,'$status',$prio,$time,1)")
           or die "cannot do: ".$dbh->errstr();    
      }
     }
   }
  }
  } else {
    warn "Table Filesystems has $cnt entries. Not initialized";
  }
 
#
# INSERT INTO datasets
#
    $cnt = 0;
    if($self->{dbdriver} =~ m/Oracle/){
     $sql="SELECT COUNT(did) FROM Datasets";
     $cntr=$self->Query($sql);
     foreach my $ret (@{$cntr}) {
        $cnt = $ret->[0];
     }
    }
    if ($cnt == 0) {
     my $did  = 0;
     my $timestamp = time();    
     my $dir  => undef;
     my $key='AMSDataDir';
     $sql="select myvalue from Environment where mykey='".$key."'";
     my $ret = $self->Query($sql);
     if( defined $ret->[0][0]){
      $dir=$ret->[0][0];
     }
     $key='AMSSoftwareDir';
     $sql="select myvalue from Environment where mykey='".$key."'";
     $ret = $self->Query($sql);
     if( defined $ret->[0][0]){
      $dir=$dir."/".$ret->[0][0];
     }
     if (defined $dir) {
      $dir="$dir/DataSets";  # top level directory for datasets
      opendir THISDIR ,$dir or die "unable to open $dir";
      my @allfiles= readdir THISDIR;
      closedir THISDIR;    
       foreach my $file (@allfiles){
        my $newfile="$dir/$file";
        if(readlink $newfile or  $file =~/^\./){
         next;
        }
          my $dataset={};
          my @tmpa;
          opendir THISDIR, $newfile or die "unable to open $newfile";
          my @jobs=readdir THISDIR;
          closedir THISDIR;
          foreach my $job (@jobs){
           if($job =~ /\.job$/){
            if($job =~ /^\./){
             next;
            }
           my $template={};
           my $full="$newfile/$job";
           my $buf;
           open(FILE,"<".$full) or die "Unable to open dataset file $full \n";
           read(FILE,$buf,1638400) or next;
           close FILE;
           my @sbuf=split "\n",$buf;
           my $datasetdesc= "generate";
           foreach my $line (@sbuf){
            if($line =~/$datasetdesc/){
              my ($junk,$dataset) = split "#",$line;                 
              $dbh->do(
                       "INSERT INTO datasets values($did,'$dataset',$timestamp)")
              or die "cannot do: ".$dbh->errstr();    
              $did++;
              last;
          }
         }
        }
       }
    }
      } else {
       die "$key is not defined in Environment table";
    }
 }
#
# end INSERT INTO datasets
#
#
# INSERT INTO DataSetFiles
#
    $cnt = 0;
    if($self->{dbdriver} =~ m/Oracle/){
     $sql="SELECT COUNT(did) FROM datasetfiles";
     $cntr=$self->Query($sql);
     foreach my $ret (@{$cntr}) {
        $cnt = $ret->[0];
     }
    }
    if ($cnt == 0) {
     my $did  = 0;
     my $timestamp = time();    
     my $dir  => undef;
     my $key='AMSDataDir';
     $sql="select myvalue from environment where mykey='".$key."'";
     my $ret = $self->Query($sql);
     if( defined $ret->[0][0]){
      $dir=$ret->[0][0];
     }
     $key='AMSSoftwareDir';
     $sql="select myvalue from environment where mykey='".$key."'";
     $ret = $self->Query($sql);
     if( defined $ret->[0][0]){
      $dir=$dir."/".$ret->[0][0];
     }
     if (defined $dir) {
      $dir="$dir/DataSets";  # top level directory for datasets
      opendir THISDIR ,$dir or die "unable to open $dir";
      my @allfiles= readdir THISDIR;
      closedir THISDIR;    
       foreach my $file (@allfiles){
        my $newfile="$dir/$file";
        if(readlink $newfile or  $file =~/^\./){
         next;
        }
          my $dataset={};
          my @tmpa;
          opendir THISDIR, $newfile or die "unable to open $newfile";
          my @jobs=readdir THISDIR;
          closedir THISDIR;
          foreach my $job (@jobs){
           if($job =~ /\.job$/){
            if($job =~ /^\./){
             next;
            }
           my $template={};
           my $full="$newfile/$job";
           my $buf;
           open(FILE,"<".$full) or die "Unable to open dataset file $full \n";
           read(FILE,$buf,1638400) or next;
           close FILE;
           my @sbuf=split "\n",$buf;
           my $datasetdesc= "generate";
           foreach my $line (@sbuf){
            if($line =~/$datasetdesc/){
              my ($junk,$dataset) = split "#",$line;                 
              $dbh->do(
                       "INSERT INTO datasetfiles values($did,'$dataset','$job',$timestamp)")
              or die "cannot do: ".$dbh->errstr();    
              $did++;
              last;
          }
         }
        }
       }
    }
      } else {
       die "$key is not defined in Environment table";
    }
 }
#
# end INSERT INTO datasets
#
    $cnt = 0;
    if($self->{dbdriver} =~ m/Oracle/){
     $sql="SELECT COUNT(mid) FROM Mails";
     $cntr=$self->Query($sql);
     foreach my $ret (@{$cntr}) {
        $cnt = $ret->[0];
     }
    }
   if ($cnt == 0) {
    my $mid  = 1;
    my $time = time();    
    open(FILE,"<",$MCCitesMailsFile) or die "Unable to open $MCCitesMailsFile";
    my @lines = <FILE>;
    close FILE;
    my $mails = 0;
    my $cites = 0;             
    foreach my $line (@lines) {
    if ($line =~/^#/) {
     if ($line =~/MAILS/) {
         $mails = 1;
         $cites = 0;
     } 

     if ($line =~/CITES/) {
         $cites = 1;
         $mails = 0
     } 
    } else {
     if ($mails == 1) {
         my ($address,$name,$r,$cid,$rs,$alias,@junk) = split ":",$line;
         if (defined $address and defined $alias) {
          $address = trimblanks($address);
          $alias   = trimblanks($alias);
          $name    = trimblanks($name);
          $r       = strtod($r);
          $rs      = strtod($rs);
          $cid     = strtod($cid);
          $dbh->do
           ("insert into Mails values($mid,
                                      '$address',
                                      '$alias',
                                      '$name',
                                      $r,
                                      $rs,
                                      $cid,
                                      'Active',
                                      0,
                                      $time,
                                      0,
                                      0)")
           or die "cannot do: ".$dbh->errstr();    
          $mid++;
      }
     }
    }
   } # foreach my $line 
   $time=time();
   warn $time;
#find responsible
    $sql="select cid from Cites";
    my $ret=$self->Query($sql);
    $sql="select cid,mid from Mails where rsite=1";
    my $rem=$self->Query($sql);
    foreach my $cid (@{$ret}){ 
        foreach my $cmid (@{$rem}){
            if($cid->[0]==$cmid->[0]){
                my $mid=$cmid->[1];
                $sql="update Cites set mid=$mid where cid=".$cid->[0];
                $self->Update($sql);
#                last;
            }
        }
    }
  } else {
    warn "Table Mails has $cnt entries. Not initialized";
 }

    print "************ Check cern_hosts and cpu_coeff tables \n";
    print "             to update run ~AMS/sql/c_m_all_hosts_cern.sql \n";
    print "             and ~AMS/perl/updatemips.cgi -h from pcamsf0 \n";
    print "************ Check localhosts table \n";
    print "             to update run ~AMS/perl/updateHostClock.o.cgi \n";

}


sub Query{
    my $self=shift;
    my $query=shift;
    my $dbh=$self->{dbhandler};
    my $sth=$dbh->prepare($query) or die "Cannot prepare query : $query ".$dbh->errstr();
    $sth->execute or die "Cannot execute ".$dbh->errstr();
    my $ret=$sth->fetchall_arrayref();     
    $sth->finish();
    return $ret;
    
    
} 
sub QueryAll{
    my $self=shift;
    my $query=shift;
    my $dbh=$self->{dbhandler};
    my $sth=$dbh->prepare($query) or die "Cannot prepare ".$dbh->errstr();
    $sth->execute or die "Cannot execute ".$dbh->errstr();
    my @fields;
    my $row={};
    my @ret;
     
    while ($row=$sth->fetchrow_hashref()){
        @fields=keys %{$row};
        my @res=values %{$row};
        push @ret, \@res;
    }
    return (\@ret,\@fields);
    
    
} 
sub Update{
    my $self=shift;
    my $query=shift;
    my $dbh=$self->{dbhandler};
    $dbh->do($query) or die "Cannot do query : $query ".$dbh->errstr();
    return 1;
}
sub Commit{
     my $self=shift;
     my $success=shift;
   if($self->{one}){
     return 1;
   }
   else{
     if(not defined $success){
       $success=1;
      }
        my $dbh=$self->{dbhandler};
        my $result= ($success ? $dbh->commit : $dbh->rollback);
          unless ($result) { 
            die "Couldn't finish transaction: " . $dbh->errstr; 
          }
          return $success;  
    }
} 

# this function sets up the necessary environement variables
# to be able to connect to Oracle
sub set_oracle_env {
    $ENV{"ORACLE_HOME"}='/afs/cern.ch/project/oracle/@sys/prod';
#    $ENV{"TNS_ADMIN"}='/afs/cern.ch/project/oracle/admin';
 $ENV{"TNS_ADMIN"}='/afs/cern.ch/exp/ams/Offline/oracle/admin';
#$ENV{"TNS_ADMIN"}='/opt/oracle/10g/network/admin';
    $ENV{"LD_LIBRARY_PATH"}=$ENV{"ORACLE_HOME"}."/lib";
    1;
}

sub DESTROY{

    my $self=shift;
    if(defined $self->{dbhandler}){
        $self->{dbhandler}->disconnect();
    }
#    warn "DESTROYING $self";
}

sub trimblanks {
    my @inp_string = @_;
    for (@inp_string) {
        s/^\s+//;        
        s/\s+$//;
    }
    return wantarray ? @inp_string : $inp_string[0];
}
