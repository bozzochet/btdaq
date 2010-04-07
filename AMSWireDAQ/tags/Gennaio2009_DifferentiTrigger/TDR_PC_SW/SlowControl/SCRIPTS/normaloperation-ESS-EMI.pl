#!/usr/bin/perl 
# warnings on

#$filelog="/var/www/html/data/testjinf.log";

my @date=localtime; my $year=$date[5]+1900; my $mon=$date[4]+1;
my $day=$date[3]; my $hour=$date[2]; my $min=$date[1];


# List of UDRs
@tdr=('003f','043f','083f','0C3f','103f','143f');
@tdrshort=('00','04','08','0C','10','14');
@tdr2=('013f','053f','093f','0D3f','113f','153f');
@tdrshort2=('01','05','09','0D','11','15');

$filelog="./data/normaloperation-EMI-ESS" . sprintf("_%04d%02d%02d", $year,$mon,$day) . sprintf("_%02d%02d", $hour, $min) . ".log";
$filedef="./def/normaloperation-EMI-ESS-def.log";

my $filedummy="./data/testdummy.log";
open(FILEDUMMY,"> $filedummy") or die "cannot open file\n"; 

open(FILELOG,"> $filelog") or die "cannot open file\n"; 

# force UDR mask in the JINF
#system("./pcamswport21 1 ff10 2e57 00ff ffff; sleep 3");
#system("./pcamswport21 2 ff10 2e57 00ff ffff; sleep 3");

##################################################################
# In the beginning: Read out SC registers  of S9011AUv2
#################################################################


system("/bin/echo -e  Reading out S9011AU via JINF A >> $filelog");
# read status words (do this on both actels)
#print FILELOG "Bus A\n";
#system("/bin/echo -e  Testing Bus A");
system("/bin/echo -e  *******Testing Bus A******* >> $filelog");
system("(/bin/echo -e *******Read Actel/UHVG status...; ./pcamswport21 1 ff10 2e1d 0030 e180 0000) >> $filelog");
system("(/bin/echo -e *******Read UPSFE S9048 status...; ./pcamswport21 1 ff10 2e1d 0030 a181 0000) >> $filelog");
system("(/bin/echo -e *******Read 3.3V - UPSFE - UHVG Global Control last cmd...; ./pcamswport21 1 ff10 2e1d 0030 a182 0000) >> $filelog");
system("(/bin/echo -e *******Read Trip Counter tripped...; ./pcamswport21 1 ff10 2e1d 0030 e183 0000) >> $filelog");


#print FILELOG "Bus B\n";
#system("/bin/echo -e  Testing Bus B");
#system("/bin/echo -e  *******Testing Bus B****** >> $filelog");
#system("(/bin/echo -e *******Read Actel/UHVG status...; ./pcamswport21 1 ff10 2e1d 0040 e180 0000) >> $filelog");
#system("(/bin/echo -e *******Read UPSFE S9048 status...; ./pcamswport21 1 ff10 2e1d 0040 a181 0000) >> $filelog");
#system("(/bin/echo -e *******Read Read 3.3V - UPSFE - UHVG Global Control last cmd...; ./pcamswport21 1 ff10 2e1d 0040 a182 0000) >> $filelog");
#system("(/bin/echo -e *******Read Trip Counter tripped...; ./pcamswport21 1 ff10 2e1d 0040 e183 0000) >> $filelog");



######################################################################
# Read out SC registers of UPSFE
######################################################################

$port=1;
print  "Testing UPSFEs via port $port  \n";
for ($slot=2;$slot<9;$slot=$slot+3) {
   $half="A";system("(echo Reading out  UPSFE Boards $slot Half A)");

# read status words
system("/bin/echo -e  Reading Status UPSFE Slot $slot Half $half");
system("/bin/echo -e  *******Reading Status UPSFE Slot $slot Half $half******* >> $filelog"); 


system("(/bin/echo -e  *******READ SEL/UDR2 status UPSFE Slot $slot, Half $half... ;".createcom($port,$slot,$half,"0 0 0000").")  >> $filelog");
system("(/bin/echo -e  *******READ UHVG status UPSFE Slot $slot, Half $half... ;".createcom($port,$slot,$half,"0 1 0000").")  >> $filelog");
system("(/bin/echo -e  *******READ LRA status UPSFE Slot $slot, Half $half... ;".createcom($port,$slot,$half,"0 2 0000").")  >> $filelog");
system("(/bin/echo -e  *******READ LRB status UPSFE Slot $slot, Half $half... ;".createcom($port,$slot,$half,"0 3 0000").")  >> $filelog");
system("(/bin/echo -e  *******READ Global Ctrl. status UPSFE Slot $slot, Half $half... ;".createcom($port,$slot,$half,"0 4 0000").")  >> $filelog");
system("(/bin/echo -e  *******READ Counter UDR2 UPSFE Slot $slot, Half $half... ;".createcom($port,$slot,$half,"0 5 0000").")  >> $filelog");
system("(/bin/echo -e  *******READ Counter UHVG UPSFE Slot $slot, Half $half... ;".createcom($port,$slot,$half,"0 6 0000").")  >> $filelog");
system("(/bin/echo -e  *******READ Counter LRA 0..3 UPSFE Slot $slot, Half $half... ;".createcom($port,$slot,$half,"0 7 0000").")  >> $filelog");
system("(/bin/echo -e  *******READ Counter LRA 4..6 UPSFE Slot $slot, Half $half... ;".createcom($port,$slot,$half,"0 8 0000").")  >> $filelog");
system("(/bin/echo -e  *******READ Counter LRB 0..3 UPSFE Slot $slot, Half $half... ;".createcom($port,$slot,$half,"0 9 0000").")  >> $filelog");
system("(/bin/echo -e  *******READ Counter LRB 4..6 UPSFE Slot $slot, Half $half... ;".createcom($port,$slot,$half,"0 A 0000").")  >> $filelog");
 }

###############################################################################
# UHVG set voltage to 1600;
################################################################################


&UHVGtest("A",1);

###############################################################################
#Get some status commands of JINFv2
################################################################################

system("/bin/echo -e Ping... ");
system("(/bin/echo -e Ping test...; ./pcamswport21 1 d) >> $filelog");
system("/bin/echo -e Status... ");
system("(/bin/echo -e Status test...; ./pcamswport21 1 c 3) >> $filelog");
system("/bin/echo -e Flash... ");
system("(/bin/echo -e Flash listing...; ./pcamswport21 1 7) >> $filelog");
system("(/bin/echo -e Slave mask...; ./pcamswport21 1 ff10 2e17) >> $filelog");

###############################################################################
# Now deal with UDR2s load program 2093
################################################################################

#for ($i=0; $i<$#tdrshort; $i++) {
#    $tmp=$tdr[$i]; $tmpshort=$tdrshort[$i];
#print  "Testing UDR $tmpshort Side 0 via port $port\n";
#    system("(/bin/echo -e ******* Testing UDR $tmpshort Side 0 via port $port) >> $filelog");
#    print "\t\t Ping, Status and Flash tests\n";
#    system("(/bin/echo -e *******Testing UDR $tmp) >> $filelog");
#    system("(/bin/echo -e *******Enter power down state for UDR $tmp...; ./pcamswport21 $port ff10 $tmp 2e4e) >> $filelog");
# #takes too long system("(/bin/echo -e *******Is it really sleeping? Ping!; ./pcamswport21 $port ff10 $tmp 2e0d; sleep 1) >> $filelog");
#    system("(/bin/echo -e *******Reboot to ROM Monitor...; ./pcamswport21 $port ff10 $tmp 2e40; sleep 1) >> $filelog");
#    system("(/bin/echo -e *******Ping test...; ./pcamswport21 $port ff10 $tmp 2e0d) >> $filelog"); 
#    system("(/bin/echo -e *******Status test...; ./pcamswport21 $port c 3 $tmpshort ) >> $filelog");
#    system("(/bin/echo -e *******Flash listing...; ./pcamswport21 $port 7 $tmpshort) >> $filelog");
#    #system("(/bin/echo -e *******Flash listing...; ./pcamswport21 $port 7 $tmpshort) >> $filelog");
#    system("(/bin/echo -e *******Change File status...; ./pcamswport21 $port ff10 $tmp 2e48 2093 0 ) >> $filelog; sleep 1");
#    system("(/bin/echo -e *******Flash listing...; ./pcamswport21 $port 7 $tmpshort) >> $filelog");
#    system("(/bin/echo -e *******Delete File ...; ./pcamswport21 $port ff10 $tmp 2e47 200 ) >> $filelog; sleep 1");
#    system("cp ./Udr3093.dat ./file.dat");
#    system("(/bin/echo -e *******Flash listing...; ./pcamswport21 $port 7 $tmpshort) >> $filelog");
#    system("(/bin/echo -e *******Load File to flash...; ./pcamswport21 $port 45 $tmpshort ) >> $filelog; sleep 1");
#    system("(/bin/echo -e ********Flash listing...; ./pcamswport21 $port 7 $tmpshort) >> $filelog");
#    system("(/bin/echo -e *******Change File status...; ./pcamswport21 $port ff10 $tmp 2e48 3093 1 ) >> $filelog; sleep 1");
#    system("(/bin/echo -e *******Flash listing...; ./pcamswport21 $port 7 $tmpshort) >> $filelog");
#    system("(/bin/echo -e *******Loading file 2093...; ./pcamswport21 $port ff10 $tmp 2e46 2093) >> $filelog; sleep 1");
#    system("(/bin/echo -e *******Status test...; ./pcamswport21 $port c 3 $tmpshort ) >> $filelog");
#  }}


system("/bin/echo -e Reboot all UDR... ");
system("(/bin/echo -e Reboot all UDR... ; ./pcamswport21 1 ff10 2a00 2e40) >> $filelog");
system("/bin/echo -e Ping all UDR... ");
system("(/bin/echo -e Ping all UDR...) >> $filelog");
for ($i=0; $i<$#tdr; $i++) {
    $tmp=$tdr[$i];
    system("(/bin/echo -e UDR $i;./pcamswport21 1 ff10 $tmp 2e0d) >> $filelog");
}
system("/bin/echo -e Status of all UDR... ");
system("(/bin/echo -e Status of all UDR... ) >> $filelog");
for ($i=0; $i<$#tdrshort; $i++) {
    $tmp=$tdrshort[$i];
    system("(/bin/echo -e UDR $i;./pcamswport21 1 c 3 $tmp) >> $filelog");
    system("./pcamswport21 1 c 7 $tmp > $filedummy");
    if ($time=readtime()>5) { print "Boot time of UDR $tmp too long, time: $time\n"; } 
}
system("/bin/echo -e Load all UDR... \n");
system("(/bin/echo -e Load all UDR with file 2093...; ./pcamswport21 1 ff10 2a00 2e46 2093; sleep 1) >> $filelog");
system("/bin/echo -e Status of all UDR... ");
system("(/bin/echo -e Status of all UDR... ) >> $filelog");
for ($i=0; $i<$#tdrshort; $i++) {
    system("(/bin/echo -e UDR $i;./pcamswport21 1 c 3 $tdrshort[$i]) >> $filelog");
}

###############################################################################
# Now read data
###############################################################################

    print "\t\t DAC/ADC tests\n";

    system("(/bin/echo -e *******Test DAQ $tmp; ./readdata_normaloperation-EMI-ESS.pl )");

###############################################################################
# UHVG set voltage to 0; Lets cheat a bit and cycle 120/5V
################################################################################

 system("(/bin/echo -e *******Get HV DC/DC converter status...; ./pcamswport21 1 ff10 2E1D 0030 E180 0000) >> $filelog");

 system("(/bin/echo -e *******Switch off HV DC/DC converter...; ./pcamswport21 1 ff10 2e1D 0030 A188 0AAA) >> $filelog; sleep 1");

 system("(/bin/echo -e *******Get HV DC/DC converter status...; ./pcamswport21 1 ff10 2E1D 0030 E180 0000) >> $filelog");

 system("(/bin/echo -e *******Switch on 1/2 HV DC/DC converter...; ./pcamswport21 1 ff10 2e1D 0030 A188 0999) >> $filelog; sleep 1");

##################################################################
# In the end: Read out SC registers  of S9011AUv2
#################################################################



print  "Test";
system("/bin/echo -e  Reading out S9011AU via JINF A >> $filelog");
# read status words (do this on both actels)
#print FILELOG "Bus A\n";
#system("/bin/echo -e  Testing Bus A");
system("/bin/echo -e  *******Testing Bus A******* >> $filelog");
system("(/bin/echo -e *******Read Actel/UHVG status...; ./pcamswport21 1 ff10 2e1d 0030 e180 0000) >> $filelog");
system("(/bin/echo -e *******Read UPSFE S9048 status...; ./pcamswport21 1 ff10 2e1d 0030 a181 0000) >> $filelog");
system("(/bin/echo -e *******Read 3.3V - UPSFE - UHVG Global Control last cmd...; ./pcamswport21 1 ff10 2e1d 0030 a182 0000) >> $filelog");
system("(/bin/echo -e *******Read Trip Counter tripped...; ./pcamswport21 1 ff10 2e1d 0030 e183 0000) >> $filelog");


#print FILELOG "Bus B\n";
#system("/bin/echo -e  Testing Bus B");
#system("/bin/echo -e  *******Testing Bus B****** >> $filelog");
#system("(/bin/echo -e *******Read Actel/UHVG status...; ./pcamswport21 1 ff10 2e1d 0040 e180 0000) >> $filelog");
#system("(/bin/echo -e *******Read UPSFE S9048 status...; ./pcamswport21 1 ff10 2e1d 0040 a181 0000) >> $filelog");
#system("(/bin/echo -e *******Read Read 3.3V - UPSFE - UHVG Global Control last cmd...; ./pcamswport21 1 ff10 2e1d 0040 a182 0000) >> $filelog");
#system("(/bin/echo -e *******Read Trip Counter tripped...; ./pcamswport21 1 ff10 2e1d 0040 e183 0000) >> $filelog");



######################################################################
# Read out SC registers of UPSFE
######################################################################

$port=1;
print  "Testing UPSFEs via port $port  \n";
for ($slot=2;$slot<9;$slot=$slot+3) {
   $half="A";system("(echo Reading out  UPSFE Boards $slot Half A)");

# read status words
system("/bin/echo -e  Reading Status UPSFE Slot $slot Half $half");
system("/bin/echo -e  *******Reading Status UPSFE Slot $slot Half $half******* >> $filelog"); 


system("(/bin/echo -e  *******READ SEL/UDR2 status UPSFE Slot $slot, Half $half... ;".createcom($port,$slot,$half,"0 0 0000").")  >> $filelog");
system("(/bin/echo -e  *******READ UHVG status UPSFE Slot $slot, Half $half... ;".createcom($port,$slot,$half,"0 1 0000").")  >> $filelog");
system("(/bin/echo -e  *******READ LRA status UPSFE Slot $slot, Half $half... ;".createcom($port,$slot,$half,"0 2 0000").")  >> $filelog");
system("(/bin/echo -e  *******READ LRB status UPSFE Slot $slot, Half $half... ;".createcom($port,$slot,$half,"0 3 0000").")  >> $filelog");
system("(/bin/echo -e  *******READ Global Ctrl. status UPSFE Slot $slot, Half $half... ;".createcom($port,$slot,$half,"0 4 0000").")  >> $filelog");
system("(/bin/echo -e  *******READ Counter UDR2 UPSFE Slot $slot, Half $half... ;".createcom($port,$slot,$half,"0 5 0000").")  >> $filelog");
system("(/bin/echo -e  *******READ Counter UHVG UPSFE Slot $slot, Half $half... ;".createcom($port,$slot,$half,"0 6 0000").")  >> $filelog");
system("(/bin/echo -e  *******READ Counter LRA 0..3 UPSFE Slot $slot, Half $half... ;".createcom($port,$slot,$half,"0 7 0000").")  >> $filelog");
system("(/bin/echo -e  *******READ Counter LRA 4..6 UPSFE Slot $slot, Half $half... ;".createcom($port,$slot,$half,"0 8 0000").")  >> $filelog");
system("(/bin/echo -e  *******READ Counter LRB 0..3 UPSFE Slot $slot, Half $half... ;".createcom($port,$slot,$half,"0 9 0000").")  >> $filelog");
system("(/bin/echo -e  *******READ Counter LRB 4..6 UPSFE Slot $slot, Half $half... ;".createcom($port,$slot,$half,"0 A 0000").")  >> $filelog");
 }




# read back the file and verify for consistency
# do it simple: compare with a default correct output and just show differences, using 'diff'!!
print system("diff $filelog $filedef");

# repeat for JINF B via Port2/Port3
# read back the file and verify for consistency

close FILE;

exit;

######################################################
#
# Createcom subroutines
#
######################################################


sub createcom{
  local($port,$s,$h,$com)=@_;
  local($rescom,$bcom,$p,$i);

    if ($h=~/A/){$rescom="2e1d 0020 ";}
    else {$rescom="2e1d 0070 ";}

  $bcom="10100000000000001111000011110000";
  if ($s==2) {substr($bcom,4,5)="00010";}
  if ($s==5) {substr($bcom,4,5)="00101";}
  if ($s==8) {substr($bcom,4,5)="01000";}

#command
#special treatment due to irregular UPSFE register setting
if (hex(substr($com,2,1))<8){
  substr($bcom,12,4)=dec2bin(hex(substr($com,2,1)),"4");}
else { substr($bcom,11,1)=1;
substr($bcom,12,4)=dec2bin(substr($com,2,1)-8,"4");
}
#get/set bit
substr($bcom,12,1)=substr($com,0,1);

for ($i=0;$i<4;$i++) {substr($bcom,16+$i*4,4)=dec2bin(hex(substr($com,4+$i,1)),"4");}
  $p=0;
#print($bcom);
#Parity
  for ($i=0;$i<32;$i++) {if(substr($bcom,$i,1)) {$p++;}}
  if ($p%2==0) {substr($bcom,1,1)="1";}
#change in hex
for ($i=0;$i<8;$i++) {if ($i==4) {$rescom=$rescom." ";}
  $rescom=$rescom.sprintf("%x",&bin2dec(substr($bcom,$i*4,4)))};
##   print $bcom." => ";
##   print "./pcamswport21 ".$port."  ff10 ".$rescom."\n";
return("./pcamswport21 ".$port." ff10 ".$rescom);}



sub dec2bin {
  local($int,$l)=@_;
  my $bin=unpack("B*",pack("N",$int));
return(substr($bin,32-$l,$l));}

sub bin2dec {
  local($bin)=@_;
return unpack("N",pack("B*","0000000000000000000000000000".$bin));}





sub readadc() {
    open(FILE1,"$filedummy") or die "cannot open file $filedummy\n";
    while (<FILE1>) {
	chop; # throw away newline
	if (/^00/)  { # search for adc info
	    my @foo = split;
	    $adc=$foo[4]; 
	    # print "adc val: $adc\n";
	}
    }
    close FILE1;
    return $adc;

}
sub readtime() {
    open(FILE1,"$filedummy") or die "cannot open file $filedummy\n";
    while (<FILE1>) {
	chop; # throw away newline
	if (/^Elapsed/)  { # search for adc info
	    my @foo = split;
	    $time=$foo[6]; 
	    print "time val: $time\n";
	}
    }
    close FILE1;
    return $time;

}


##########################################################
# UHVG SubRoutines
##########################################################

################################
################################
################################


sub UHVGtest {
local($half,$port)=@_;
my $filedummy="./data/test-Uhvgdummy.log";
#my $filelog="./data/normaloperation-UHVG-ESS-EMI" . sprintf("_%04d%02d%02d", $year,$mon,$day) . 
sprintf("_%02d%02d", $hour, $min) . ".log";
my $fileadc="./data/normaloperation-UHVG-ESS-EMI" . sprintf("_%04d%02d%02d", $year,$mon,$day) . 
sprintf("_%02d%02d", $hour, $min) . ".data";

#open(FILELOG,"> $filelog") || die "cannot open file\n"; 
open(FILEDUMMY,"> $filedummy") || die "cannot open file\n"; 
open(FILEADC,"> $fileadc") || die "cannot open file\n"; 

   if ($port<=1) {system("(echo ******Testing via JINF Half A******)>>$filelog");}
     else {system("(echo ******Testing via JINF Half B******)>>$filelog");}
   if ($l==0) {$half="A";system("(echo ******Testing UHVG Boards Half A******)");}
     else {$half="B";system("(echo ******Testing UHVG Boards Half B******)");}

#######################################
#Channel Init
#######################################
system("(echo Init...)");
&HVwritecom("******Set Voltagelim 0x33...","a cc00");
&HVwritecom("******Set Current unlim...","b fc00");
system("(echo Set Voltage...)");
&HVwritecom("******Set Voltage 0x2000...","8 c000");#1600V
&HVwritecom("******Enable...","d 0000");
system("(echo Status...) ;sleep 3");
&HVwritecom("******Readback Status","6 0000");

###################################
# ADC Reading
###################################
system("(echo Read ADC values...)");
for ($ali=0;$ali<2;$ali++){&HVwritecom("******Set ADC 14bit","c 3840");}
&HVreadadc("14bit","2f00");
for ($ali=0;$ali<2;$ali++){&HVwritecom("******Set ADC current","c 3ac0");}
&HVreadadc("Current","0000");
for ($ali=0;$ali<2;$ali++){&HVwritecom("******Set ADC Temp","c 38c0");}
&HVreadadc("Temp","0200");
for ($ali=0;$ali<2;$ali++){&HVwritecom("******Set ADC HV","c 3b40");}
&HVreadadc("HV","2f00");


##############################################
# At the end, read out slow control registers
##############################################


print  "Testing S9011AU via JINF A\n";
system("/bin/echo -e  Testing S9011AU via JINF A  >> $filelog");
# read status words (do this on both actels)
#print FILELOG "Bus A\n";
system("/bin/echo -e  Testing Bus A");
system("/bin/echo -e  *******Testing Bus A******* >> $filelog");
system("(/bin/echo -e *******Read Actel/UHVG status...; ./pcamswport21 1 ff10 2e1d 0030 e180 0000) >> $filelog");
system("(/bin/echo -e *******Read UPSFE S9048 status...; ./pcamswport21 1 ff10 2e1d 0030 a181 0000) >> $filelog");
system("(/bin/echo -e *******Read 3.3V - UPSFE - UHVG Global Control last cmd...; ./pcamswport21 1 ff10 2e1d 0030 a182 0000) >> $filelog");
system("(/bin/echo -e *******Read Trip Counter tripped...; ./pcamswport21 1 ff10 2e1d 0030 e183 0000) >> $filelog");


#print FILELOG "Bus B\n";
#system("/bin/echo -e  Testing Bus B");
#system("/bin/echo -e  *******Testing Bus B****** >> $filelog");
#system("(/bin/echo -e *******Read Actel/UHVG status...; ./pcamswport21 1 ff10 2e1d 0040 e180 0000) >> $filelog");
#system("(/bin/echo -e *******Read UPSFE S9048 status...; ./pcamswport21 1 ff10 2e1d 0040 a181 0000) >> $filelog");
#system("(/bin/echo -e *******Read Read 3.3V - UPSFE - UHVG Global Control last cmd...; ./pcamswport21 1 ff10 2e1d 0040 a182 0000) >> $filelog");
#system("(/bin/echo -e *******Read Trip Counter tripped...; ./pcamswport21 1 ff10 2e1d 0040 e183 0000) >> $filelog");


#################################
# Final Testing & Disable
#################################
system("(echo Disable...)");
&HVwritecom("******Disable...","d 8000"); #commented out because we switch off via DCDC
&HVwritecom("******...and Readback Enable","5 0000");
system("(echo )>>$filelog;(echo ******Testing UHVG finished******)>> $filelog");
print "Testing UHVG finished \n \n";

#close FILELOG;
close FILEDUMMY;
close FILEADC;


# Create BIN & convert to HEX
sub HVcreatecom{
  local($port,$s,$h,$c,$com)=@_;
  local($rescom,$bcom,$p,$i);
  #select bus
  if ($s<14){if ($h=~/A/){$rescom="2e1d 0010 ";}
    else {$rescom="2e1d 0050 ";}}
  else {if ($h=~/A/){$rescom="2e1d 0000 ";}
    else {$rescom="2e1d 0060 ";}}
  #create bin command
  $bcom="10100000000000001111000011110000";
  if (($s==12)||($s==15)) {substr($bcom,8,1)="1";}
  if (($s==13)||($s==16)) {substr($bcom,7,1)="1";}
  substr($bcom,9,3)=dec2bin($c,"3");
  substr($bcom,12,4)=dec2bin(hex(substr($com,0,1)),"4");
  for ($i=0;$i<4;$i++) {substr($bcom,16+$i*4,4)=dec2bin(hex(substr($com,2+$i,1)),"4");}
  ###paritycheck
  for ($i=0;$i<32;$i++) {if(substr($bcom,$i,1)) {$p++;}}
  if ($p%2==0) {substr($bcom,1,1)="1";}
  ###create hex command
  for ($i=0;$i<8;$i++) {if ($i==4) {$rescom=$rescom." ";}
  $rescom=$rescom.sprintf("%x",&bin2dec(substr($bcom,$i*4,4)))};
return("./pcamswport21 ".$port." ff10 ".$rescom);}

sub dec2bin {
  local($int,$l)=@_;
  my $bin=unpack("B*",pack("N",$int));
return(substr($bin,32-$l,$l));}

sub bin2dec {
  local($bin)=@_;
return unpack("N",pack("B*","0000000000000000000000000000".$bin));}

#convert ADC to DEC
sub HVcalcadc {
  local($str)=@_;
  $strback="";
  for ($ai=0;$ai<4;$ai++){
    $binstr=dec2bin(hex(substr($str,$ai,1)),"4");
    $bintmp=substr($binstr,0,1);
    substr($binstr,0,1)=substr($binstr,3,1);
    substr($binstr,3,1)=$bintmp;
    $bintmp=substr($binstr,1,1);
    substr($binstr,1,1)=substr($binstr,2,1);
    substr($binstr,2,1)=$bintmp;
    $strback=sprintf("%x",&bin2dec($binstr)).$strback;}
return($strback);
}

# read ADC from dummy
sub HVreadadcv {
    local($rxcom)=@_;
    $adc="0001";
    open(FILE1,"$filedummy") or die "cannot open file $filedummy\n";
    while (<FILE1>) {
	chop; # throw away newline
	if (/^$rxcom/)  { # search for adc info
	    my @foo = split;
	    $adc=$foo[4];
	    #print "adc val: $adc\n";
    }}
    close FILE1;
return $adc;}

# create command and output
sub HVwritecom {
  local($txt,$com5)=@_;
  local($slot,$chip);
 for ($sin=11;$sin<17;$sin++) {
  for ($cin=0;$cin<7;$cin++) {
   system("(echo )>>$filelog");
   system("(echo )>>$filelog;(echo --- Testing UHVG Slot $sin, Chip $cin, Half $half) >> $filelog");
   system("(echo ".$txt."; ".&HVcreatecom($port,$sin,$half,$cin,$com5).") >> $filelog");
}}}

# create command and output for ADCreading
sub HVreadadc {
  local($txt,$limt)=@_;
  my $slot,$chip;
 for ($slot=11;$slot<17;$slot++) {
   for ($chip=0;$chip<7;$chip++) {
  system("(echo ) >> $filelog;(echo --- Reading ADC $txt  UHVG Slot $slot, Chip $chip, Half $half) >> $filelog");
  system("(echo ) >> $fileadc;(echo --- Testing UHVG Slot $slot, Chip $chip, Half $half) >> $fileadc");
  system("(echo Read ADC value ".$txt."; ".&HVcreatecom($port,$slot,$half,$chip,"7 0000").") >> $fileadc");
  system("(echo Read ADC value ".$txt."; ".&HVcreatecom($port,$slot,$half,$chip,"7 0000").") > $filedummy");
  $adc=&HVcalcadc(&HVreadadcv(substr(&HVcreatecom($port,$slot,$half,$chip,"7 0000"),27,14)));
  if (($adc) lt $limt) { system("(echo ***ADC failed - too low: $adc!)>> $fileadc");
       system("(echo ***ADC failed - too low: $adc!)>> $filelog");}
    else {if (($adc) eq "8000"){system("(echo ***ADC failed - not read!)>> $fileadc");
       system("(echo ***ADC failed - not read!)>> $filelog");}
      else {system("(echo ***ADC OK: $adc)>> $fileadc");
       system("(echo ***ADC OK)>> $filelog");}}
}}}
}



