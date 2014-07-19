package CastorCopy;
#
# November 25, 2002. A.Klimentov
#
# Last Edit : December 4, 2002. A.Klimentov
#
# doCopy   - subr. to copy from local to castor
# doCmp    - subr. to compare directories content
# doCRCcmp - subr. to compare CRC values stored in two files
#
my $helpCastorCopy = "
# CastorCopy
# Program copies files from 'local' directory to 'CASTOR'.
# - checks sintaksis
# - checks directory content
# - does copy
# - check directories 
# - the output is /tmp/castor.cp.'copy start time'.log
# 
# arguments :
# -from    - source files path
# -to      - target directory path
# -h       - help
# -v       - verbose                             
# -u       - copy file if it doesn't exist or file size is changed (default)
# -force   - copy ALL files (even if it exist in output directory)
#
# ./castor.cp -v -u -from  /f2dah1/MC/AMS02/2004A -to /castor/cern.ch/ams/MC/AMS02/2004A/
";
my $helpCmp = "
#
# CastorCmp
# Program compare files in local and remote directories
# arguments - local and remote directories paths
# castor.cmp /castor/cern.ch/ams/mydir /f2users/alexei/mydir
#
# November 25, 2002. A.Klimentov
";

my $helpCRCcmp = "
#
# CastorCRCcmp
# Program compare CRC (calculated elsethere) of files in local and 
# remote directories
#
# arguments - dir prefix, inpfile1 inpfile2
# castor.CRCcmp -subdir inpfile1 inpfile2
#
# December 4, 2002. A.Klimentov
";

use File::Find;
use Time::Local;
use Sys::Hostname;

@CastorCopy::EXPORT= qw(newCopy newCmp newCRCcmp doCopy doCmp doCRCcmp);

my $nfiles = 0;     # number of files in (input/output) dir 
my @inputFiles;     # list of file names in input dir
my @outputFiles;    # list of file names in output dir
my @inputSizes;     # size in bytes of files from inputFiles
my @outputSizes;    # size in bytes of files from outputFiles
my $verbose=>undef; # verbose copy mode
my $update = 1;     # update mode
my $force  = 0;     # force mode

my $dirnum = -77;   # castor directory

my %fields=(
    start=>undef,
    hostname=>undef,
    source=>undef,
    target=>undef,
    subdir=>undef,
    ok=>undef
            );

sub newCopy {
#
my $input;           # input directory path
my $output;          # output directory path
#
    my $type=shift;
    my $self={
              %fields,
          };
    $self->{start}=time();
    $verbose = 0;


# check sintaksis
my $inputflag = 0;
my $outputflag = 0;

foreach my $chop  (@ARGV){
    if ($inputflag == 1) {
        $input=$chop;
    }
    if ($outputflag == 1) {
        $output=$chop;
    }
    $inputflag=0;
    $outputflag=0;
    if($chop =~/^-from/){
        $inputflag=1;
    }
    if($chop =~/^-to/){
        $outputflag=1;
    }

    if($chop =~/^-v/){
      $verbose = 1;
    }

    if($chop =~/^-u/) {
        $update = 1;
    }

    if ($chop=~/^-force/) {
        $force = 1;
    }

    if($chop =~/^-h/){
        print $helpCastorCopy;
        die "...";
    }

}


if (not defined $input or not defined $output) {
    die "CastorCopy -E- Invalid sintaksis.   castor.cp -from /inputDir/files -to /outputDir/";
 }

if ($force && $update) {
    die "CastorCopy -E- check options, (-f AND -u)";
}

# get hostname
  my $hostname = hostname();
  my @host = split '.',$hostname;
  if (defined $host[0]) {
   $self->{hostname}=$host[0];
  } else {
   $self->{hostname}=$hostname;
  } 
 if ($verbose) {
     print "Hostname : $self->{hostname} \n";
     print "Input  Directory : $input \n";
     print "Output Directory : $output \n";
 }
# check dirpath

if (substr($input, 0, 1) ne '/' or substr($output, 0, 1) ne '/' ) {
    print "castor.cmp $input $output\n";
    die "castor.cp -E- Invalid sintaksis.  Specify directory explicitly";
}
    $self->{source} = $input;
    $self->{target} = $output;
#
my $mybless=bless $self,$type;
$CastorCopy::Singleton=$mybless;
return $mybless;

}


sub newCmp {
#
my $input;           # input directory path
my $output;          # output directory path
#
    my $type=shift;
    my $self={
              %fields,
          };
    $self->{start}=time();
    $verbose = 0;

foreach my $chop  (@ARGV){
    if($chop =~/^-h/){
        print $helpCmp;
        die "...";
    }
}

# check sintaksis
my $inputflag = 0;
my $outputflag = 0;

$input=$ARGV[0];
$output=$ARGV[1];

if (not defined $input or not defined $output) {
    die "CastorCopy -E- Invalid sintaksis.   castor.cp -from /inputDir/files -to /outputDir/";
 }

# get hostname
  my $hostname = hostname();
  my @host = split '.',$hostname;
  if (defined $host[0]) {
   $self->{hostname}=$host[0];
  } else {
   $self->{hostname}=$hostname;
  } 
 if ($verbose) {
     print "Hostname : $self->{hostname} \n";
     print "Input  Directory : $input \n";
     print "Output Directory : $output \n";
 }
# check dirpath

if (substr($input, 0, 1) ne '/' or substr($output, 0, 1) ne '/' ) {
    print "castor.cp $input $output\n";
    die "castor.cp -E- Invalid sintaksis.  Specify directory explicitly";
}
    $self->{source} = $input;
    $self->{target} = $output;
#
my $mybless=bless $self,$type;
$CastorCopy::Singleton=$mybless;
return $mybless;

}


sub newCRCcmp {
#
my $input;           # file with CRC for input directory 
my $output;          # file with CRC for output directory
#
    my $type=shift;
    my $self={
              %fields,
          };

    $self->{start}=time();
    $verbose = 0;
   
my $subdirflag = 0;
my $subdir => undef;

foreach my $chop  (@ARGV){
    if ($subdirflag == 1) {
        $subdir=$chop;
        $subdirflag = 0;
    }

    if($chop =~/^-h/){
        print $helpCRCcmp;
        die "...";
    }

    if($chop =~/^-subdir/){
        $subdirflag = 1;
    }

}

if (defined $subdir) {$subdirflag = 1;}
# check sintaksis




# check dirpath
    if (defined $subdir) {
     $self->{subdir}=$subdir;
     $input=$ARGV[2];
     $output=$ARGV[3];
    } else {
     $input =$ARGV[0];
     $output=$ARGV[1];
    }
#
    $self->{source} = $input;
    $self->{target} = $output;
    if (not defined $input or not defined $output) {
     die "CastorCopy -E- Invalid sintaksis.   castor.CRCcmp  inputfile outputfile";
    }
#
    print "***** Input  dir list  $input \n";
    print "***** Output dir list  $output \n";
    if (defined $subdir) {print "Subdirectory path $subdir \n";}
    
#
my $mybless=bless $self,$type;
$CastorCopy::Singleton=$mybless;
return $mybless;

}

sub castorDir {
    my $dir     = shift;
    my $mode    = shift;
    my $time    = time();
    my $logfile = "/tmp/castor.dir.$time.log";
    my $cmd     = "touch $logfile";
    $status     = systemCmd($cmd);
    if ($status == 0) {
     $cmd = "nsls -R -l $dir >> $logfile";
     $status     = systemCmd($cmd);
     if ($status == 0) {
       parseLog($mode,$logfile);
     } 
     $cmd = "rm $logfile";
#---     systemCmd($cmd);
    }      
}

sub parseLog {
#
# parse log files of nsls -R -l /castor/cern.ch/whatever
# substitue ':' by '/'
# assumed sintaksis '/castor/cern.ch/ams/MC:'
#
    my $mode    =  shift;
    my $logfile =  shift;
    my $dir    => undef;
    print "parse $logfile as $mode \n";
    open(FILE,"<".$logfile) or die "parseLog -E- Unable to open file $logfile \n"; 
    my @lines= <FILE>;
    close FILE;
    my $i = 0;
    my $n = 0; # file name index in input(output)List
    while ($i <= $#lines){     
     if($lines[$i]  =~/^\//) {      #directory
      $lines[$i] =~ s/:/\//;
      $lines[$i] =~ s/\n//;
      $dir=$lines[$i];
      print "Directory : $dir \n";
     } else {                  # file
#  0           1   2      3     4       5  6  7     8
# mrw-r--r--   1 casadmva va  26400000 Nov 26 12:20 101108000.1.hbk
         my ($prot,$unk,$owner,$grp,$size,$mon,$day,$time,$file) 
         = split " ",$lines[$i];
         if (defined $file && defined $size) {
          $file = $dir.$file;
          if ($mode eq "input") {
              $inputFiles[$n] = $file;
              my @array = split(//,$prot);
              if ($array[0] eq "d") {
                  $inputSizes[$n] = $dirnum;
                  
              } else {
                $inputSizes[$n] = $size;
              }
          }
          if ($mode eq "output") {
              $outputFiles[$n] = $file;
              $outputSizes[$n] = $size;
          }
         $n++;
      } 
     }
     $i++;
 }
}

sub inputList {
    $inputFiles[$nfiles] = $File::Find::name;
#    if (-d $inputFiles[$nfiles]) {
#        print "Directory : $inputFiles[$nfiles] \n";
#    } else {
#        print ".........................$inputFiles[$nfiles] \n";
#    }
    $inputSizes[$nfiles] = (stat($inputFiles[$nfiles])) [7];
    $nfiles++;
}

sub outputList {
    $outputFiles[$nfiles] = $File::Find::name;
#    if (-d $outputFiles[$nfiles]) {
#        print "Directory : $outputFiles[$nfiles] \n";
#    } else {
#        print ".........................$outputFiles[$nfiles] \n";
#    }
    $outputSizes[$nfiles] = (stat($outputFiles[$nfiles])) [7];
    $nfiles++;
}

sub doCRCcmp {

    my $status =>undef;
    my $cmd    =>undef;
    my $self = shift;
    my $input  = $self->{source};
    my $output = $self->{target};
    my $subdir => undef;
    if (defined $self->{subdir}) {
       $subdir = $self->{subdir};
    }

    if (defined $subdir) {
     print "***** Sub directory    $subdir \n";
    }

    open(FILE,"<".$input) or die "-E- Unable to open file $input \n"; 
    my @inplines= <FILE>;
    close FILE;

    open(FILE,"<".$output) or die "-E- Unable to open file $output \n"; 
    my @outlines= <FILE>;
    close FILE;


    my $i = 0;
    my $j = 0;
    my $crcok = 0;
    my $notfound = 0;
    my $badcrc   = 0;
    my $nfiles   = 0;

    while ($i <= $#inplines){     
     my ($filepath,$crc) = split " ",$inplines[$i];
     if (defined $filepath && defined $crc) {
         my @junk   = split '/',$filepath;
         $filepath = $junk[$#junk];
         if (defined $subdir) {$filepath = $subdir.$filepath;}
         $j=0;
         $crcok = 0;
         while ($j <= $#outlines && $crcok ==0) {
          if ($outlines[$j] =~ /$filepath/) {
           my ($ofilepath,$ocrc) = split " ",$outlines[$j];
           if (defined $ofilepath && defined $ocrc) {
               if ($crc == $ocrc) {
                   $crcok = 1;
               } else {
                   $crcok = -1;
               }
           } else {
               print "doCRCcmp -W- check $filepath, $crc \n";
               print "doCRCcmp -W- found $outlines[$j] ";
           }          
         }
         $j++;
        }
         if ($crcok == 0) {
             print "doCRCcmp -E- not found $inplines[$i]";
             $notfound++;
         } 
         if ($crcok == -1) {
             print "doCRCcmp -E- $inplines[$i], $crc \n";
             print "doCRCcmp -E- $outlines[$i], $ocrc \n";
             $badcrc++;
         } 
         $nfiles++;
     }
     $i++;
 }
    print "Files checked in $input : $nfiles \n";
    print "Files not found in $output : $notfound \n";
    print "CRC differs for : $badcrc files \n";
}
sub doCopy {
#
# 4 possibilities 
#
#  input   output
#  ---------------
#   local    local  : cp -pi -r -u /localdirX /localdirY
#   local    castor
#   castor   local
#   castor   castor
#  ----------------
#
    return;
    my $status =>undef;
    my $cmd    =>undef;
    my $self = shift;
    my $input  = $self->{source};
    my $output = $self->{target};

    print "***** Input directory $input \n";
    print "***** Output directory $output \n";
    if ($update) {
      print "***** -update mode -I-  copy only new or modified files \n";
    } else {
      print "***** -force mode -I- copy ALL files \n";
    } 

    my $inputdir  = "local";
    my $outputdir = "local";

    if($input  =~/castor\/cern.ch/){ $inputdir = "castor";}
    if($output =~/castor\/cern.ch/){ $outputdir = "castor";}
 
    my $starttime = time();
    my $logfile = "/tmp/castor.cp.$starttime.log";
    $cmd = "touch $logfile";
    $status = systemCmd($cmd);

    if ($inputdir eq "local" && $outputdir eq "local") {
       if ($update) {
        $cmd="/bin/cp -pi -r -u $inputdir $outputdir";
       } else {
        $cmd="/bin/cp -pi -r $inputdir $outputdir";
       } 
        $status = systemCmd($cmd);
        return 1;
    }

    if ($inputdir eq "local") {
     $nfiles = 0;
     find (\&inputList, $input);
    } else {
     castorDir($input,"input");
    }

    if ($outputdir eq "local") {
     $nfiles = 0;
     find (\&outputList, $output);
    } else {
     castorDir($output,"output");
    }

    if ($#inputFiles < 1) {
        die "Nothing to copy, $input has $#inputFiles files \n";
    }


    my $i = 0;
    my $j = 0;
    my $dir  = $output;
    my $sdir => undef;

    my $nbytes   = 0;  # bytes copied
    my $nnfiles  = 0;  # files copied

    my $founddir = 0;
    my $newdir   = 0;

    my $ifile    => undef; # input file path

    while ($i <= $#inputFiles) {
     if ($verbose) {print "inputFiles[$i]... $inputFiles[$i] \n";}
     $newdir = 0;
     if ($inputdir eq "local") {
      if (-d $inputFiles[$i]) { $newdir = 1;}
     } else {
      if($inputSizes[$i] == $dirnum) { $newdir = 1;
                           print "Directory.... $inputFiles[$i] \n";}
     }
     if ($newdir) {
      $founddir = 0;
      $subdir   = $inputFiles[$i];
      $subdir   =~ s/$input//;
      if (defined $subdir) {
       $dir=$output.$subdir;
       if ($dir ne $output) { # assume that top directory always exists
        $j  = 0;
        while ($j<= $#outputFiles && $founddir == 0) {
        if ($outputFiles[$j] eq $dir) {
             $founddir = 1;
            }
            $j++;
        }
        if (!$founddir) {
         if ($outputdir eq "castor") {
           $cmd = "/usr/local/bin/rfmkdir $dir >> $logfile";
          } else {
            $cmd = "/bin/mkdir $dir >> $logfile";
          }
          systemCmd($cmd);
          $status = 0;
         }  # new directory : mkdir
       }
      }
     } # inputFiles[$i] is a directory
      else {
       # inputFiles[$i] is a file
        $ifile=$inputFiles[$i];
        $ifile =~ s/$input//;
        if (defined $ifile) {
         my $isize = $inputSizes[$i];
         my $docopy = 1;
         if ($update) {
          $j = 0;
          while ($j<= $#outputFiles && $docopy==1) {
           my $ofile = $outputFiles[$j];
              $ofile =~ s/$output//;
           if ($ofile eq $ifile) {
            if ($outputSizes[$j] == $isize) {
              $docopy = 0;
              if ($verbose) {
                  print "not overwrite -I- $outputFiles[$j] is the same size \n";
              }
             } else {
               if ($verbose) {
                   print "overwrite $outputFiles[$j] by $ifile \n";
                   print "input  file size = $isize \n";
                   print "output file size = $outputSizes[$j] \n";
               }
             }
           }
           $j++;
          }
         } 
         if ($docopy == 1) {         
          my $ofile=$output.$ifile;
          $cmd = "/usr/local/bin/rfcp $inputFiles[$i] $ofile >> $logfile";
          $status = systemCmd($cmd);
          if ($inputdir eq "local") {
           $filesize = (stat($inputFiles[$i])) [7];
           $nbytes   = $nbytes + $filesize;
          } else {
           $nbytes   = $nbytes + $inputSizes[$i];
          }
           $nnfiles++;
          }
        }
     }
     $i++;
    } 
   if ($verbose) {
    print "castor.cp -I- $#inputFiles files are copied to $output\n";
   }
    $status = 1;
LAST : 
    my $endtime = time();
    my $secs    = $endtime - $starttime;
    my $start   = localtime($starttime);
    my $end     = localtime($endtime);
    print "Copy started : $start \n";
    print "     ended   : $end \n";
    my $mbytes = sprintf("%.1f",$nbytes/1000000.);
    print "MBytes : $mbytes \n";
    print "Files  : $nnfiles \n";
    if ($nnfiles > 0) {
     my $fsize= sprintf("%.1f",$mbytes/$nnfiles);
     print "Aver. file size : $fsize [MB] \n";
    }
    if ($secs > 0) {
        my $io = sprintf("%.1f",$nbytes/$secs/1000.);
        print "I/O rate : $io [KBytes/sec] \n";
    }
    return $status;
}

sub doCmp {
    my $status=>undef;

    my $self = shift;
    my $input  = $self->{source};
    my $output = $self->{target};

#
# 4 possibilities 
#
#  input   output
#  ---------------
#   local    local
#   local    castor
#   castor   local
#   castor   castor
#  ----------------
#
    my $inputdir  = "local";
    my $outputdir = "local";

    if($input  =~/castor\/cern.ch/){ $inputdir = "castor";}
    if($output =~/castor\/cern.ch/){ $outputdir = "castor";}

    print "***** Input directory $input \n";
    if ($inputdir eq "local") {
     $nfiles = 0;
     find (\&inputList, $input);
    } else {
     castorDir($input,"input");
    }

    print "***** Output directory $output \n";
    if ($outputdir eq "local") {
     $nfiles = 0;
     find (\&outputList, $output);
    } else {
     castorDir($output,"output");
    }

    my $ifile  =>undef;
    my $ofile  =>undef;

    my $i        = 0;
    my $j        = 0;
    my $next     = 0;
    my $notfound = 0;
    my $dir;
    my $inpfileflag = 1;
    my $outfileflag = 1;
      while ($i <= $#inputFiles) {
       $inpfileflag = 1;
       if ($inputdir eq "local") {
        if (-d $inputFiles[$i]) {
          $dir = $inputFiles[$i];
          $inpfileflag = 0;
         }
       }
       if ($inpfileflag) {
           $ifile=$inputFiles[$i];
           $ifile =~ s/$input//;
         if (defined $ifile) {
          $next = 0;
          $j    = 0;
          if ($#outputFiles > 0) {
           while ($j <= $#outputFiles && $next < 1) {
            $outfileflag = 1;
            if (defined $outputFiles[$j]) {
             if ($outputdir eq "local") {
              if (-d $outputFiles[$j]) {
               $outfileflag = 0;
              }
             }
            if ($outfileflag) {
             $ofile = $outputFiles[$j];
             $ofile =~ s/$output//;
             if (defined $ofile) {
              if ($ofile eq $ifile) {
               $next = 1;
             }
            }
         }
        }
        $j++;
       }
       if ($next == 0) {
        print "not found... $inputFiles[$i] \n";
        $notfound++;
       }
      }
     }
    }
   $i++;
   }
    print "$input : $#inputFiles Files \n";
    print "$output : $#outputFiles Files \n";
    print "$notfound files not matched in $output \n";

    $status=1;
    return $status;
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

sub systemCmd {
    my $cmd = shift;
    my $status = 0;
    if (defined $cmd) {
     if ($verbose) {
        print "$cmd \n";
     }
     my $rc     = system($cmd);
     if ($rc != 0) {
         if ($rc == 0xff00) {
             print "command failed : $! \n";
         } elsif (($rc & 0xff) == 0) {
             if ($rc >>=8) {
              print "ran with non-zero exit status $rc \n";
             }
         } else {
             print "ran with ";
             if ($rc & 0x80) {
                 $rc &= ~0x80;
                 print "coredump from ";
             }
             print "signal $rc \n";
         }
       die "cannot execute $cmd , ret code $rc\n";
     }
   } else {
     die "systemCmd is called with undefined argument \n";
   }
  return $status;
}
