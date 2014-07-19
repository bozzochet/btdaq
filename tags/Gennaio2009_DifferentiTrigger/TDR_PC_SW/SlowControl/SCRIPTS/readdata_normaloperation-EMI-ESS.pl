#!/usr/bin/perl 
# warnings on

$iii=$ARGV[0];

my @date=localtime; my $year=$date[5]+1900; my $mon=$date[4]+1;
my $day=$date[3]; my $hour=$date[2]; my $min=$date[1];

$file="./data/normaloperation-ESS-EMI-events" . sprintf("_%04d%02d%02d", $year,$mon,$day) . sprintf("_%02d%02d", $hour, $min) . ".data";

# List of UDRs
@tdr=('003f','043f','083f','0C3f','103f','143f','013f','053f','093f','0D3f','113f','153f');
@tdrshort=('00','04','08','0C','10','14','01','05','09','0D','11','15');
#@tdr2=('013f','053f','093f','0D3f','113f','153f');
#@tdrshort2=('01','05','09','0D','11','15');

@tdrmask=('0000 0001', '0000 0010','0000 0100', '0000 1000', '0001 0000', '0010 0000','0000 0002','0000 0020', '0000 0200', '0000 2000', '0020 0000', '0020 0000');
#@tdrmask2=('0000 0002','0000 0020', '0000 0200', '0000 2000', '0020 0000', '0020 0000');

open(FILE1,"> $file") or die "cannot open file\n"; 

$tmp=$tdr[$iii];
$tmpmask=$tdrmask[$iii];

#configure JINF
# ... (set mask for one TDR only)
#system("(./pcamswport21 1 ff10 2e57 $tmpmask) >> $file; sleep 1");
#system("(./pcamswport21 1 ff10 2e17) >> $file");
# take raw data
system("(./pcamswport21 1 52 1) >> $file; sleep 1");

#system("(./pcamswport21 1 ff10 $tmp 2e46 2093) > $file; sleep 1");
#system("(./pcamswport21 1 ff10 $tmp 2e52 1; sleep 1) >> $file");
#system("(./pcamswport21 1 ff047 00c3> $file");

print FILE1 "\n...taking events...\n";
# give a first trigger to be ahead of data taking...
system("((echo -e *******Read Slave mask on JINF);(./pcamswport21 1 ff10 2e17; sleep 1)) >> $file");
system("((echo -e *******Clear last event number on JINF);(./pcamswport21 1 42; sleep 1)) >> $file");
system("((echo -e *******Clear last event number on UDR2s);(./pcamswport21 1 ff10 2a00 2e42; sleep 1)) >> $file");
system("((echo -e Set all UDR2s to raw event readout);(./pcamswport21 1 ff10 2a00 2e52 0001; sleep 1)) >> $file");
#system("(echo -e *******show".(sprintf("%x",$j)).");(./pcamswport21 1 ff10 $tmp 2e54 ".sprintf("%x",$j)."; sleep 2)");

# Give first trigger
system("(./pcamswport21 1 ff0a; sleep 1)");

system("(echo -e Start collecting JINF events....)");
system("((echo -e *******Setting DAC....);(./pcamswport21 1 ff10 2a00 2e54 0080; sleep 3)) >> $file");
for ($j=0;$j<255;$j++){
    system("(./pcamswport21 1 ff0a >> /dev/null)"); #Trigger
    system("(./pcamswport21 1 1) >> $file");
}

 
print FILE1 "\n...ok, processing events now...\n";
close FILE1;

#my @s1; my @s2; my @k;
#for (my $i=0;$i<4096;$i++) {
#    $s1[$i]=1;
#    $s2[$i]=1;
#    $k[$i]=1;
#}

#my @ped; my @newped; my @foo;

#open(FILE1,"$file") or die "cannot open file\n"; 
#my $i=0; my $j=0; 
#	sleep(1);
#while (<FILE1>) {
#    chop; # throw away newline
#    if (/^[0-9]/)  { # search for pedestals
#	@foo = split;
#	for ($j=0; $j<14; $j++) {
#	    $ped[$i+$j]="0x" . "$foo[$j]";
#	    $ped[$i+$j]=(oct($ped[$i+$j])>>3);
#	    print "$i+$j $ped[$i+$j]\n";
#	}
#	$i+=14;
#    }
#    if (/GET EVENT: BuildSTAT = /) { # treat your events here (as below)
#	$i=0; $j=0;
#	for(my $ch=0; $ch<320;$ch++) {
#	    $cha=3*$ch;
#	    $newped[$ch]=$ped[$cha];
#	    $newped[$ch+320]=$ped[$cha+1];
#	    $newped[$ch+640]=$ped[$cha+2];
#	}
#	for($ch=0; $ch<64; $ch++) {
#	    $newped[$ch+960]=$ped[$ch+960];
#	}
#	for($ch=0; $ch<320; $ch++) {
#	    $s1[$newped[$ch]]++;
#	    $s2[$newped[$ch+320]]++;
#	}
#	for($ch=0; $ch<384; $ch++) {
#	    $k[$newped[$ch+640]]++;
#	}
#	print "I found it\n@newped\n\ @ped \n";	
# 	for ($iii=0;$iii<1024;$iii++) {
# 	    print "$iii $newped[$iii]\n ";
# 	}
# 	print "\n *************** \n";
# 	for ($iii=0;$iii<1024;$iii++) {
# 	    print "$iii $ped[$iii] \n ";
# 	}
# 	print "\n";
#    }
#}
close FILE1;

#print "**** S1 ****\n @s1 \n\n";
#print "**** S2 ****\n @s2 \n\n";
#print "**** K  ****\n @k \n\n";

#open(FILE2,"> ./adc.dat") or die "cannot open file\n";
#for($i=0;$i<4096;$i++) {
#    print FILE2 "$i $s1[$i]\n";
#}
#close FILE2;
#system("mv adc.dat s1.dat");

#system("gnuplot ./s1.input");


#open(FILE2,"> ./adc.dat") or die "cannot open file\n";
#for($i=0;$i<4096;$i++) {
#    print FILE2 "$i $s2[$i]\n";
#}
#close FILE2;
#system("mv adc.dat s2.dat");

#system("gnuplot ./s2.input");


#open(FILE2,"> ./adc.dat") or die "cannot open file\n";
#for($i=0;$i<4096;$i++) {
#    print FILE2 "$i $k[$i]\n";
#}
#close FILE2;
#system("mv adc.dat k.dat");
#system("gnuplot ./adc.input");

#$fileeps="testadc" . sprintf("_%04d%02d%02d", $year,$mon,$day) . sprintf("_%02d%02d_tdr_%02d", $hour, $min, $iii) . ".eps";
#system("mv adc.eps $fileeps");

#open(FILE1,">> $file") or die "cannot open file\n"; 

#print FILE1 "...done\n";

# set the mask back just to be sure, and set jinf back to compressed
#system("./pcamswport21 1 ff10 2e57 00ff ffff; sleep 3");
#system("./pcamswport21 2 ff10 2e57 00ff ffff; sleep 3");
#system("./pcamswport21 1 52 0; sleep 1");

exit;
