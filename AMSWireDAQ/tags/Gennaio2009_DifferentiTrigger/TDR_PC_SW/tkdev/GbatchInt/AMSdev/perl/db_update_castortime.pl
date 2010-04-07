#!/usr/bin/perl -w

# Update Castor time according to path
# A.Klimentov, O.Kounina
# 
# Apr 4, 2005 : minor bug in cmpFiles
#
# Last Edit : Apr 4, 2005.
#

my $helpCastor_update = "
# arguments :
# -f       - force mode update time for ALL files (even already copied)
# -dir    -  directory's path
# -pwd     - ORACLE user password
# -time    - unixtime
# -v       - verbose mode
# -update  - update mode
# -uid     - ORACLE user id
#
# ./castor_update.pl -dir:/f8dah2/MC/AMS02/2005A {-time:unixtime} {-f} {-v}
";

my $path    = undef;
my $dir     = undef;
my $pwd     = undef;
my $uid     = undef;

my $utime   = 0;
my $force   = 0;
my $verbose = 0;
my $update  = 0;

my @alldirs = ();           # CASTOR directories
my @castorfiles =();        #        files from subdir
my @castorsizes =();        #        file size as it is returned by nsls
my $nfiles      = 0;        # total files in subdir
my $filenotfound     = 0;   # total files not found on castor, exist in DB 
my $filesizemismatch = 0;   #             with file size discrepancy in DB and CASTOR
my @dbfiles = ();           # DB files paths matched to -path
my @dbsizes = ();           #          size in MB

my $subdir;
my $firstdir         = 0;   # first directory flag

my $Jan2005          = 1104540000; # utime < Jan2005 is invalid

# check login id
    my $whoami = getlogin();
    if ($whoami =~ 'ams') {
    } else {
      print "castor_update -ERROR- script cannot be run from account : $whoami \n";
      die "bye";
    }

# check sintaksis                                 
foreach my $chop (@ARGV) {
    if ($chop =~ /^-dir/) {
        $dir=unpack("x5 A*",$chop);
	$dir = trimblanks($dir);
        my @junk = split "MC/",$dir; 
        if (defined $junk[1]) {
         $path = "/castor/cern.ch/ams/MC/".$junk[1];
        }
    } 
    if ($chop =~ /^-pwd/) {
        $pwd=unpack("x5 A*",$chop);
	$pwd = trimblanks($pwd);
    } 
    if ($chop =~ /^-uid/) {
        $uid=unpack("x5 A*",$chop);
	$uid = trimblanks($uid);
    } 

    if($chop =~ /^-time/){
        $utime=unpack("x6 A*",$chop);
    }

    if ($chop =~ /^-f/){
	$force =1;
    }

    if ($chop =~ /^-h/){
	print "$helpCastor_update /n";
        exit(0);
    }

    if ($chop =~ /^-v/){
	$verbose = 1;
    }

    if ($chop =~ /^-update/){
	$update = 1;
    }
}

if (not defined $path ) {
    print "$helpCastor_update \n";
    die "castor_update -Fatal- path is not defined. \n";
} 

if (not defined $pwd ) {
    print "$helpCastor_update \n";
    die "castor_update -Fatal- ORACLE password is not defined. \n";
}

if (not defined $uid ) {
    print "$helpCastor_update \n";
    die "castor_update -Fatal- ORACLE user id is not defined. \n";
}

my $ctime = time();
if ($utime != 0) {
    if ($utime < $Jan2005) {
        my $ltime = localtime($utime);
        print "castor_update -Fatal- Invalid Time : $ltime \n";
        die;
    }
} else {
    $utime = $ctime;
}


if ($verbose) {
print "Castor path : $path\n";
print "DB search path : $dir\n";
my $ltime = localtime($utime);
print "Castor Copy Time : $ltime \n";
if ($update == 0) {print "NO DB UPDATE \n";}
}

# dump directory list of ../castor to $logfile

my $logfile = "/tmp/castor.dir.$ctime.log";
$cmd = "nsls -R -l $path > $logfile";
$rc = system $cmd;
unless ( $rc == 0 ) {
    die "Can't read $path: $!\n";
}


use strict;
use DBI;


my $db = DBI->connect("DBI:Oracle:amsdb",
		      $uid,
		      $pwd,
		      ) || die "Database connection not made: $DBI::errstr";


my $query = "SELECT path, sizemb FROM amsdes.ntuples WHERE path LIKE '%$dir%'";
my $castorquery = " ";
if ($force == 0){
    $castorquery = " AND castortime = 0";
}
$query = $query.$castorquery." ORDER BY run";
my $ret = Query($db,$query);

if (defined $ret->[0][0]) {
    foreach my $f (@{$ret}) {
      push (@dbfiles, $f->[0]);
      push (@dbsizes,  $f->[1]);
  }
}


open(FILE,"<".$logfile) or die "Unable to open file $logfile \n";
my @lines = <FILE>;
close FILE;


$filenotfound     = 0;
$filesizemismatch = 0;
$nfiles           = 0;
foreach my $line (@lines) {
    if ($line =~ /^\//) {
         $subdir  = trimblanks($line);
         $subdir  =~ s/:/\//;
         $firstdir         = 1;
    } else {
      if ($firstdir != 0) {
         $nfiles++;
         my ($prot,$unk,$owner,$grp,$size,$mon,$day,$time,$file) =  split " ",$line;
         if (defined $size && defined $file) {
          $file = trimblanks($file);
          if ($file =~ m/./) {
           push (@castorfiles,$file);
           push (@castorsizes, $size);
          }
        }
     }
   }
}
cmpFiles();
$query = "UPDATE amsdes.ntuples SET castortime = $utime WHERE path LIKE '%";
foreach my $q (@alldirs) {
   my $upd = $query.$q."%'".$castorquery;
   if ($verbose == 1) {print "$upd \n";}
   if ($update == 1) {
    my $sth=$db->prepare($upd)  or die "Cannot prepare $upd ".$db->errstr();
    $sth->execute or die "Cannot execute $upd ".$db->errstr();
   }
 } 
$db->disconnect();

sub trimblanks {
    my @inp_string = @_;
    for (@inp_string) {
        s/^\s+//;        
        s/\s+$//;
    }
    return wantarray ? @inp_string : $inp_string[0];
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

sub cmpFiles {
         my $i        = 0;
         if ($verbose == 1) {print "cmpFiles -I- files in DB matched query : $#dbfiles \n";}
         foreach my $dbf (@dbfiles) {
           my $notfound = 1;
           my $j = 0;
           my @junk = split '/',$dbf;
           if ($#junk > 1) {
            my $filename = trimblanks($junk[$#junk]);
            foreach my $casf (@castorfiles) {
               if ($filename =~ $casf) {
                if ($j <= $#castorsizes) { 
                 my $csize = $castorsizes[$j];
                 my $sizemb = sprintf("%.1f",$csize/1000000.);
                 if ($dbsizes[$i] - $sizemb > 0.8 || $sizemb - $dbsizes[$i] > 0.8) {
                  if ($verbose) {
                      print "Size mismatch : file $dbf $sizemb, $dbsizes[$i] \n";
                  }
                  $filesizemismatch++;
                 }
                 $notfound = 0;
                 last;
                } else {
                 print "Error in program j=$j, array size = $#castorsizes \n";
                }
             }
             $j++;
           }
          if ($notfound == 1) {
             $filenotfound++;
             if ($verbose) {
              print "File $dbf not copied to CASTOR, yet \n";
             }
         }
       } else {
           print "Error - invalid DB file $dbf \n";
       }
        $i++;
      }
      if ($filenotfound == 0 && $filesizemismatch == 0) {
             if ($nfiles > 0) { 
              my @junk = split '/castor/cern.ch/ams/',$subdir;
              if (defined $junk[1]) {
                my $s    = $junk[1]; 
                my @jj   = split '/',$dir;
                my $d    = $jj[1];
                my $ss   = "/".$d."/".$s;
                push (@alldirs, $ss);
            } else {
                print "Error - skip $subdir - cannot parse\n";
            }
          }
         } else {
            print "skip $subdir :  $nfiles: $filenotfound : $filesizemismatch  \n";
         }
     }

