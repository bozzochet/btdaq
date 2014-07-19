#!/usr/bin/perl 
# warnings on

#$filelog="/var/www/html/data/testjinf.log";
$filelog="test9011at.log";
$filedef="testdef9011at.log";

my $filedummy="testdummy.log";

open(FILELOG,"> $filelog") or die "cannot open file\n"; 
open(FILEDUMMY,"> $filedummy") or die "cannot open file\n"; 


print  "Testing 9011AT via JINF A\n";
system("/bin/echo -e  Testing 9011AT via JINF A  >> $filelog");
# read status words (do this on both actels)
#print FILELOG "Bus A, Half A\n";
system("/bin/echo -e  Testing Bus A, Half A");
system("/bin/echo -e  Testing Bus A, Half A >> $filelog");
system("(/bin/echo -e  Read SEL status...; ./pcamswport 0 ff10 2e1d 0040 e180 0000) >> $filelog"); 
system("(/bin/echo -e  Read Actel status...; ./pcamswport 0 ff10 2e1d 0040 a181 0000) >> $filelog");
system("(/bin/echo -e  Read DCL last cmd...; ./pcamswport 0 ff10 2e1d 0040 a182 0000) >> $filelog");
system("(/bin/echo -e  Read DCL tripped...; ./pcamswport 0 ff10 2e1d 0040 e183 0000) >> $filelog");
system("(/bin/echo -e  Read DCH last cmd...; ./pcamswport 0 ff10 2e1d 0040 a184 0000) >> $filelog");
system("(/bin/echo -e  Read DCH tripped...; ./pcamswport 0 ff10 2e1d 0040 e185 0000) >> $filelog");
system("(/bin/echo -e  Read DCL feedback...; ./pcamswport 0 ff10 2e1d 0040 e186 0000) >> $filelog");
system("(/bin/echo -e  Read DCH feedback...; ./pcamswport 0 ff10 2e1d 0040 a187 0000) >> $filelog");
system("(/bin/echo -e  Read DCL status...; ./pcamswport 0 ff10 2e1d 0040 a190 0000) >> $filelog");
system("(/bin/echo -e  Read DCH status...; ./pcamswport 0 ff10 2e1d 0040 e191 0000) >> $filelog");
#print FILELOG "Bus A, Half B\n";
system("/bin/echo -e  Testing Bus A, Half B");
system("/bin/echo -e  Testing Bus A, Half B >> $filelog");
system("(/bin/echo -e  Read SEL status...; ./pcamswport 0 ff10 2e1d 0060 e180 0000) >> $filelog"); 
system("(/bin/echo -e  Read Actel status...; ./pcamswport 0 ff10 2e1d 0060 a181 0000) >> $filelog");
system("(/bin/echo -e  Read DCL last cmd...; ./pcamswport 0 ff10 2e1d 0060 a182 0000) >> $filelog");
system("(/bin/echo -e  Read DCL tripped...; ./pcamswport 0 ff10 2e1d 0060 e183 0000) >> $filelog");
system("(/bin/echo -e  Read DCH last cmd...; ./pcamswport 0 ff10 2e1d 0060 a184 0000) >> $filelog");
system("(/bin/echo -e  Read DCH tripped...; ./pcamswport 0 ff10 2e1d 0060 e185 0000) >> $filelog");
system("(/bin/echo -e  Read DCL feedback...; ./pcamswport 0 ff10 2e1d 0060 e186 0000) >> $filelog");
system("(/bin/echo -e  Read DCH feedback...; ./pcamswport 0 ff10 2e1d 0060 a187 0000) >> $filelog");
system("(/bin/echo -e  Read DCL status...; ./pcamswport 0 ff10 2e1d 0060 a190 0000) >> $filelog");
system("(/bin/echo -e  Read DCH status...; ./pcamswport 0 ff10 2e1d 0060 e191 0000) >> $filelog");
#print FILELOG "Bus B, Half A\n";
system("/bin/echo -e  Testing Bus B, Half A");
system("/bin/echo -e  Testing Bus B, Half A >> $filelog");
system("(/bin/echo -e  Read SEL status...; ./pcamswport 0 ff10 2e1d 0050 e180 0000) >> $filelog"); 
system("(/bin/echo -e  Read Actel status...; ./pcamswport 0 ff10 2e1d 0050 a181 0000) >> $filelog");
system("(/bin/echo -e  Read DCL last cmd...; ./pcamswport 0 ff10 2e1d 0050 a182 0000) >> $filelog");
system("(/bin/echo -e  Read DCL tripped...; ./pcamswport 0 ff10 2e1d 0050 e183 0000) >> $filelog");
system("(/bin/echo -e  Read DCH last cmd...; ./pcamswport 0 ff10 2e1d 0050 a184 0000) >> $filelog");
system("(/bin/echo -e  Read DCH tripped...; ./pcamswport 0 ff10 2e1d 0050 e185 0000) >> $filelog");
system("(/bin/echo -e  Read DCL feedback...; ./pcamswport 0 ff10 2e1d 0050 e186 0000) >> $filelog");
system("(/bin/echo -e  Read DCH feedback...; ./pcamswport 0 ff10 2e1d 0050 a187 0000) >> $filelog");
system("(/bin/echo -e  Read DCL status...; ./pcamswport 0 ff10 2e1d 0050 a190 0000) >> $filelog");
system("(/bin/echo -e  Read DCH status...; ./pcamswport 0 ff10 2e1d 0050 e191 0000) >> $filelog");
#print FILELOG "Bus B, Half B\n";
system("/bin/echo -e  Testing Bus B, Half B");
system("/bin/echo -e  Testing Bus B, Half B >> $filelog");
system("(/bin/echo -e  Read SEL status...; ./pcamswport 0 ff10 2e1d 0070 e180 0000) >> $filelog"); 
system("(/bin/echo -e  Read Actel status...; ./pcamswport 0 ff10 2e1d 0070 a181 0000) >> $filelog");
system("(/bin/echo -e  Read DCL last cmd...; ./pcamswport 0 ff10 2e1d 0070 a182 0000) >> $filelog");
system("(/bin/echo -e  Read DCL tripped...; ./pcamswport 0 ff10 2e1d 0070 e183 0000) >> $filelog");
system("(/bin/echo -e  Read DCH last cmd...; ./pcamswport 0 ff10 2e1d 0070 a184 0000) >> $filelog");
system("(/bin/echo -e  Read DCH tripped...; ./pcamswport 0 ff10 2e1d 0070 e185 0000) >> $filelog");
system("(/bin/echo -e  Read DCL feedback...; ./pcamswport 0 ff10 2e1d 0070 e186 0000) >> $filelog");
system("(/bin/echo -e  Read DCH feedback...; ./pcamswport 0 ff10 2e1d 0070 a187 0000) >> $filelog");
system("(/bin/echo -e  Read DCL status...; ./pcamswport 0 ff10 2e1d 0070 a190 0000) >> $filelog");
system("(/bin/echo -e  Read DCH status...; ./pcamswport 0 ff10 2e1d 0070 e191 0000) >> $filelog"); 
# switch on/off Actels (default is both on!)
system("/bin/echo -e  Testing Actels");
system("/bin/echo -e  Testing Actels >> $filelog");
system("(/bin/echo -e  Turn brother off via Bus A...; ./pcamswport 0 ff10 2e1d 0040 a189 0001) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Bus B...; ./pcamswport 0 ff10 2e1d 0050 a181 0000) >> $filelog");
system("(/bin/echo -e  Turn brother on via Bus A...; ./pcamswport 0 ff10 2e1d 0040 a189 0010) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Bus B...; ./pcamswport 0 ff10 2e1d 0050 a181 0000) >> $filelog");
system("(/bin/echo -e  Turn brother off via Bus B...; ./pcamswport 0 ff10 2e1d 0050 a189 0001) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Bus A...; ./pcamswport 0 ff10 2e1d 0040 a181 0000) >> $filelog");
system("(/bin/echo -e  Turn brother on via Bus B...; ./pcamswport 0 ff10 2e1d 0050 a189 0010) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Bus A...; ./pcamswport 0 ff10 2e1d 0040 a181 0000) >> $filelog");
# turn on/off S/K voltages (more needed)
system("/bin/echo -e  Testing S/K Voltages");
system("/bin/echo -e  Testing S/K Voltages >> $filelog");
# check trips here
system("(/bin/echo -e  Check Trips...; ./pcamswport 0 ff10 2e1d 0040 e183 0000) >> $filelog");
system("(/bin/echo -e  Read DCL status...; ./pcamswport 0 ff10 2e1d 0040 a182 0000) >> $filelog");
system("(/bin/echo -e  Turn off S0...; ./pcamswport 0 ff10 2e1d 0040 a18a 0001) >> $filelog");
system("(/bin/echo -e  Turn off K0...; ./pcamswport 0 ff10 2e1d 0040 a18a 0002) >> $filelog");
system("(/bin/echo -e  Turn off S1...; ./pcamswport 0 ff10 2e1d 0040 a18a 0004) >> $filelog");
system("(/bin/echo -e  Turn off K1...; ./pcamswport 0 ff10 2e1d 0040 a18a 0008) >> $filelog");
system("(/bin/echo -e  Turn off S2...; ./pcamswport 0 ff10 2e1d 0040 a18a 0010) >> $filelog");
system("(/bin/echo -e  Turn off K2...; ./pcamswport 0 ff10 2e1d 0040 a18a 0020) >> $filelog");
system("(/bin/echo -e  Turn off S3...; ./pcamswport 0 ff10 2e1d 0040 a18a 0040) >> $filelog");
system("(/bin/echo -e  Turn off K3...; ./pcamswport 0 ff10 2e1d 0040 a18a 0080) >> $filelog; sleep 1");
system("(/bin/echo -e  Read DCL status...; ./pcamswport 0 ff10 2e1d 0040 a182 0000) >> $filelog");
# read data from TDR
system("(/bin/echo -e  Turn on all S/K...; ./pcamswport 0 ff10 2e1d 0040 e18a 0000) >> $filelog; sleep 2");
# check trips here
system("(/bin/echo -e  Check Trips...; ./pcamswport 0 ff10 2e1d 0040 e183 0000) >> $filelog");
# read data from TDR
system("(/bin/echo -e  Read DCL status...; ./pcamswport 0 ff10 2e1d 0040 a182 0000) >> $filelog");
system("(/bin/echo -e  Turn off all S...; ./pcamswport 0 ff10 2e1d 0040 e18a 0055) >> $filelog; sleep 1");
system("(/bin/echo -e  Read DCL status...; ./pcamswport 0 ff10 2e1d 0040 a182 0000) >> $filelog");
system("(/bin/echo -e  Turn on S/off K...; ./pcamswport 0 ff10 2e1d 0040 e18a 00aa) >> $filelog; sleep 2");
system("(/bin/echo -e  Check Trips...; ./pcamswport 0 ff10 2e1d 0040 e183 0000) >> $filelog");
system("(/bin/echo -e  Read DCL status...; ./pcamswport 0 ff10 2e1d 0040 a182 0000) >> $filelog");
system("(/bin/echo -e  Turn on all S/K...; ./pcamswport 0 ff10 2e1d 0040 e18a 0000) >> $filelog; sleep 2");
system("(/bin/echo -e  Check Trips...; ./pcamswport 0 ff10 2e1d 0040 e183 0000) >> $filelog");
system("(/bin/echo -e  Read DCL status...; ./pcamswport 0 ff10 2e1d 0040 a182 0000) >> $filelog");
# turn on/off 120 volts (more needed)
# read ADC on TBS to check
system("/bin/echo -e  Testing 120 Volts");
system("/bin/echo -e  Testing 120 Volts >> $filelog");
system("(/bin/echo -e  Read DCH status...; ./pcamswport 0 ff10 2e1d 0040 a184 0000) >> $filelog");
system("(/bin/echo -e  Read TBS ADC 0 0x10...; ./pcamswport 0 ff10 2e1d 0000 a2a0 0000) > $filedummy");
$adc=readadc();
if ($adc lt "0db0") { print "ADC 0x10 not on, $adc! \n"; } 
system("(/bin/echo -e  Read TBS ADC 0 0x11...; ./pcamswport 0 ff10 2e1d 0000 e2a1 0000) > $filedummy");
$adc=readadc();
if ($adc lt "0db0") { print "ADC 0x11 not on, $adc! \n"; } 
system("(/bin/echo -e  Turn off 120 Volts...; ./pcamswport 0 ff10 2e1d 0040 e18c 00f0) >> $filelog; sleep 2");
system("(/bin/echo -e  Read DCH status...; ./pcamswport 0 ff10 2e1d 0040 a184 0000) >> $filelog");
system("(/bin/echo -e  Read TBS ADC 0 0x10...; ./pcamswport 0 ff10 2e1d 0000 a2a0 0000) > $filedummy");
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x10 not off, $adc! \n"; } 
system("(/bin/echo -e  Read TBS ADC 0 0x11...; ./pcamswport 0 ff10 2e1d 0000 e2a1 0000) > $filedummy");
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x11 not off, $adc! \n"; } 
system("(/bin/echo -e  Switch 120 Volts to cold...; ./pcamswport 0 ff10 2e1d 0040 e18c 0050) >> $filelog; sleep 2");
system("(/bin/echo -e  Read DCH status...; ./pcamswport 0 ff10 2e1d 0040 a184 0000) >> $filelog");
system("(/bin/echo -e  Read TBS ADC 0 0x10...; ./pcamswport 0 ff10 2e1d 0000 a2a0 0000) > $filedummy");
$adc=readadc();
if ($adc lt "0db0") { print "ADC 0x10 not on, $adc! \n"; } 
system("(/bin/echo -e  Read TBS ADC 0 0x11...; ./pcamswport 0 ff10 2e1d 0000 e2a1 0000) > $filedummy");
$adc=readadc();
if ($adc lt "0db0") { print "ADC 0x11 not on, $adc! \n"; } 
system("(/bin/echo -e  Switch 120 Volts to hot...; ./pcamswport 0 ff10 2e1d 0040 e18c 00a0) >> $filelog; sleep 2");
system("(/bin/echo -e  Read DCH status...; ./pcamswport 0 ff10 2e1d 0040 a184 0000) >> $filelog");
system("(/bin/echo -e  Read TBS ADC 0 0x10...; ./pcamswport 0 ff10 2e1d 0000 a2a0 0000) > $filedummy");
$adc=readadc();
if ($adc lt "0db0") { print "ADC 0x10 not on, $adc! \n"; } 
system("(/bin/echo -e  Read TBS ADC 0 0x11...; ./pcamswport 0 ff10 2e1d 0000 e2a1 0000) > $filedummy");
$adc=readadc();
if ($adc lt "0db0") { print "ADC 0x11 not on, $adc! \n"; } 
system("(/bin/echo -e  Switch 120 Volts both on default...; ./pcamswport 0 ff10 2e1d 0040 e18c 0000) >> $filelog; sleep 2");
system("(/bin/echo -e  Read DCH status...; ./pcamswport 0 ff10 2e1d 0040 a184 0000) >> $filelog");
system("(/bin/echo -e  Read TBS ADC 0 0x10...; ./pcamswport 0 ff10 2e1d 0000 a2a0 0000) > $filedummy");
$adc=readadc();
if ($adc lt "0db0") { print "ADC 0x10 not on, $adc! \n"; } 
system("(/bin/echo -e  Read TBS ADC 0 0x11...; ./pcamswport 0 ff10 2e1d 0000 e2a1 0000) > $filedummy");
$adc=readadc();
if ($adc lt "0db0") { print "ADC 0x11 not on, $adc! \n"; } 
######################################################################
# turn on/off 3.3 volts and verify 
######################################################################

######################################################################
# JINF A via Bus A can switch 3.3 B
#print FILELOG "Testing 3.3 Volts\n";
system("/bin/echo -e  Testing 3.3 Volts");
system("/bin/echo -e  Testing 3.3 Volts >> $filelog");
system("(/bin/echo -e  Turn off 3.3 Volts for B...; ./pcamswport 0 ff10 2e1d 0040 a18c 0004) >> $filelog; sleep 1");
# ping B to verify 
system("(/bin/echo -e  Ping B...; ./pcamswport 2 d) >> $filelog"); 
system("(/bin/echo -e  Turn on 3.3 Volts for B again...; ./pcamswport 0 ff10 2e1d 0040 e18c 0000) >> $filelog; sleep 1");
# ping B to verify 
system("(/bin/echo -e  Ping B...; ./pcamswport 2 d) >> $filelog"); 
######################################################################
# JINF B via Bus B can switch 3.3 A
system("(/bin/echo -e  Turn off 3.3 Volts for A...; ./pcamswport 2 ff10 2e1d 0050 a18c 0001) >> $filelog; sleep 1");
# ping A to verify 
system("(/bin/echo -e  Ping A...; ./pcamswport 0 d) >> $filelog"); 
system("(/bin/echo -e  Turn on 3.3 Volts for A again...; ./pcamswport 2 ff10 2e1d 0050 e18c 0000) >> $filelog; sleep 1");
# ping A to verify 
system("(/bin/echo -e  Ping A...; ./pcamswport 0 d) >> $filelog"); 
######################################################################
# JINF A/B via Bus B will glitch 3.3 B
system("(/bin/echo -e  Glitch 3.3 Volts for B...; ./pcamswport 0 ff10 2e1d 0050 a18c 0004) >> $filelog; sleep 1");
# here we still need to check the boot time!!!
system("(/bin/echo -e  Status B...; ./pcamswport 2 c 3) >> $filelog"); 
system("(/bin/echo -e  Status B...; ./pcamswport 2 c 7) > $filedummy"); 
system("/bin/echo -e Status B after 1st glitch");
$time=readtime();
if ($time > 3) { print "error: Glitchtime too big, time: $time\n";}
######################################################################
# JINF B/A via Bus A will glitch 3.3 A
system("(/bin/echo -e  Glitch 3.3 Volts for A...; ./pcamswport 2 ff10 2e1d 0040 a18c 0001) >> $filelog; sleep 1");
# here we still need to check the boot time!!!
system("(/bin/echo -e  Status A...; ./pcamswport 0 c 3) >> $filelog; sleep 1"); 
system("(/bin/echo -e  Status A...; ./pcamswport 0 c 7) > $filedummy; sleep 1"); 
system("/bin/echo -e Status A after 1st glitch");
$time=readtime(); 
if ($time > 3) { print "error: Glitchtime too big, time: $time\n";}
######################################################################
# glitch both sides by switching from Hot to cold
system("(/bin/echo -e  Switch 3.3 Volts from Hot to Cold...; ./pcamswport 0 ff10 2e1d 0040 e18c 000f) >> $filelog; sleep 1");
# check glitch 
system("(/bin/echo -e  Status A...; ./pcamswport 0 c 3) >> $filelog"); 
system("(/bin/echo -e  Status A...; ./pcamswport 0 c 7) > $filedummy"); 
system("/bin/echo -e Status A after Hot-Cold");
$time=readtime();
if ($time > 3){  print "error: Glitchtime too big, time: $time\n";}
system("(/bin/echo -e  Status B...; ./pcamswport 2 c 3) >> $filelog"); 
system("(/bin/echo -e  Status B...; ./pcamswport 2 c 7) > $filedummy"); 
system("/bin/echo -e Status B after Hot-Cold");
$time=readtime();
if ($time > 3) { print "error: Glitchtime too big, time: $time\n";}
######################################################################
# back to default
system("(/bin/echo -e  Switch 3.3 Volts from Cold to Hot...; ./pcamswport 0 ff10 2e1d 0040 e18c 0000) >> $filelog; sleep 1");
# check glitch (to do)
system("(/bin/echo -e  Status A...; ./pcamswport 0 c 3) >> $filelog"); 
system("(/bin/echo -e  Status A...; ./pcamswport 0 c 7) > $filedummy"); 
system("/bin/echo -e Status A after Cold-Hot");
$time=readtime();
if ($time > 3) { print "error: Glitchtime too big, time: $time\n";}
system("(/bin/echo -e  Status B...; ./pcamswport 2 c 3) >> $filelog"); 
system("(/bin/echo -e  Status B...; ./pcamswport 2 c 7) > $filedummy"); 
system("/bin/echo -e Status B after Cold-Hot");
$time=readtime();
if ($time > 3) { print "error: Glitchtime too big, time: $time\n";}

######################################################################
# afterwards repeat for JINF B (switch 3.3 accordingly)
######################################################################
print  "Testing 9011AT via JINF B\n";
system("/bin/echo -e  Testing 9011AT via JINF B  >> $filelog");
# read status words (do this on both actels)
#print FILELOG "Bus A, Half A\n";
system("/bin/echo -e  Testing Bus A, Half A");
system("/bin/echo -e  Testing Bus A, Half A >> $filelog");
system("(/bin/echo -e  Read SEL status...; ./pcamswport 2 ff10 2e1d 0040 e180 0000) >> $filelog"); 
system("(/bin/echo -e  Read Actel status...; ./pcamswport 2 ff10 2e1d 0040 a181 0000) >> $filelog");
system("(/bin/echo -e  Read DCL last cmd...; ./pcamswport 2 ff10 2e1d 0040 a182 0000) >> $filelog");
system("(/bin/echo -e  Read DCL tripped...; ./pcamswport 2 ff10 2e1d 0040 e183 0000) >> $filelog");
system("(/bin/echo -e  Read DCH last cmd...; ./pcamswport 2 ff10 2e1d 0040 a184 0000) >> $filelog");
system("(/bin/echo -e  Read DCH tripped...; ./pcamswport 2 ff10 2e1d 0040 e185 0000) >> $filelog");
system("(/bin/echo -e  Read DCL feedback...; ./pcamswport 2 ff10 2e1d 0040 e186 0000) >> $filelog");
system("(/bin/echo -e  Read DCH feedback...; ./pcamswport 2 ff10 2e1d 0040 a187 0000) >> $filelog");
system("(/bin/echo -e  Read DCL status...; ./pcamswport 2 ff10 2e1d 0040 a190 0000) >> $filelog");
system("(/bin/echo -e  Read DCH status...; ./pcamswport 2 ff10 2e1d 0040 e191 0000) >> $filelog");
#print FILELOG "Bus A, Half B\n";
system("/bin/echo -e  Testing Bus A, Half B");
system("/bin/echo -e  Testing Bus A, Half B >> $filelog");
system("(/bin/echo -e  Read SEL status...; ./pcamswport 2 ff10 2e1d 0060 e180 0000) >> $filelog"); 
system("(/bin/echo -e  Read Actel status...; ./pcamswport 2 ff10 2e1d 0060 a181 0000) >> $filelog");
system("(/bin/echo -e  Read DCL last cmd...; ./pcamswport 2 ff10 2e1d 0060 a182 0000) >> $filelog");
system("(/bin/echo -e  Read DCL tripped...; ./pcamswport 2 ff10 2e1d 0060 e183 0000) >> $filelog");
system("(/bin/echo -e  Read DCH last cmd...; ./pcamswport 2 ff10 2e1d 0060 a184 0000) >> $filelog");
system("(/bin/echo -e  Read DCH tripped...; ./pcamswport 2 ff10 2e1d 0060 e185 0000) >> $filelog");
system("(/bin/echo -e  Read DCL feedback...; ./pcamswport 2 ff10 2e1d 0060 e186 0000) >> $filelog");
system("(/bin/echo -e  Read DCH feedback...; ./pcamswport 2 ff10 2e1d 0060 a187 0000) >> $filelog");
system("(/bin/echo -e  Read DCL status...; ./pcamswport 2 ff10 2e1d 0060 a190 0000) >> $filelog");
system("(/bin/echo -e  Read DCH status...; ./pcamswport 2 ff10 2e1d 0060 e191 0000) >> $filelog");
#print FILELOG "Bus B, Half A\n";
system("/bin/echo -e  Testing Bus B, Half A");
system("/bin/echo -e  Testing Bus B, Half A >> $filelog");
system("(/bin/echo -e  Read SEL status...; ./pcamswport 2 ff10 2e1d 0050 e180 0000) >> $filelog"); 
system("(/bin/echo -e  Read Actel status...; ./pcamswport 2 ff10 2e1d 0050 a181 0000) >> $filelog");
system("(/bin/echo -e  Read DCL last cmd...; ./pcamswport 2 ff10 2e1d 0050 a182 0000) >> $filelog");
system("(/bin/echo -e  Read DCL tripped...; ./pcamswport 2 ff10 2e1d 0050 e183 0000) >> $filelog");
system("(/bin/echo -e  Read DCH last cmd...; ./pcamswport 2 ff10 2e1d 0050 a184 0000) >> $filelog");
system("(/bin/echo -e  Read DCH tripped...; ./pcamswport 2 ff10 2e1d 0050 e185 0000) >> $filelog");
system("(/bin/echo -e  Read DCL feedback...; ./pcamswport 2 ff10 2e1d 0050 e186 0000) >> $filelog");
system("(/bin/echo -e  Read DCH feedback...; ./pcamswport 2 ff10 2e1d 0050 a187 0000) >> $filelog");
system("(/bin/echo -e  Read DCL status...; ./pcamswport 2 ff10 2e1d 0050 a190 0000) >> $filelog");
system("(/bin/echo -e  Read DCH status...; ./pcamswport 2 ff10 2e1d 0050 e191 0000) >> $filelog");
#print FILELOG "Bus B, Half B\n";
system("/bin/echo -e  Testing Bus B, Half B");
system("/bin/echo -e  Testing Bus B, Half B >> $filelog");
system("(/bin/echo -e  Read SEL status...; ./pcamswport 2 ff10 2e1d 0070 e180 0000) >> $filelog"); 
system("(/bin/echo -e  Read Actel status...; ./pcamswport 2 ff10 2e1d 0070 a181 0000) >> $filelog");
system("(/bin/echo -e  Read DCL last cmd...; ./pcamswport 2 ff10 2e1d 0070 a182 0000) >> $filelog");
system("(/bin/echo -e  Read DCL tripped...; ./pcamswport 2 ff10 2e1d 0070 e183 0000) >> $filelog");
system("(/bin/echo -e  Read DCH last cmd...; ./pcamswport 2 ff10 2e1d 0070 a184 0000) >> $filelog");
system("(/bin/echo -e  Read DCH tripped...; ./pcamswport 2 ff10 2e1d 0070 e185 0000) >> $filelog");
system("(/bin/echo -e  Read DCL feedback...; ./pcamswport 2 ff10 2e1d 0070 e186 0000) >> $filelog");
system("(/bin/echo -e  Read DCH feedback...; ./pcamswport 2 ff10 2e1d 0070 a187 0000) >> $filelog");
system("(/bin/echo -e  Read DCL status...; ./pcamswport 2 ff10 2e1d 0070 a190 0000) >> $filelog");
system("(/bin/echo -e  Read DCH status...; ./pcamswport 2 ff10 2e1d 0070 e191 0000) >> $filelog"); 
# switch on/off Actels (default is both on!)
system("/bin/echo -e  Testing Actels");
system("/bin/echo -e  Testing Actels >> $filelog");
system("(/bin/echo -e  Turn brother off via Bus A...; ./pcamswport 2 ff10 2e1d 0040 a189 0001) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Bus B...; ./pcamswport 2 ff10 2e1d 0050 a181 0000) >> $filelog");
system("(/bin/echo -e  Turn brother on via Bus A...; ./pcamswport 2 ff10 2e1d 0040 a189 0010) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Bus B...; ./pcamswport 2 ff10 2e1d 0050 a181 0000) >> $filelog");
system("(/bin/echo -e  Turn brother off via Bus B...; ./pcamswport 2 ff10 2e1d 0050 a189 0001) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Bus A...; ./pcamswport 2 ff10 2e1d 0040 a181 0000) >> $filelog");
system("(/bin/echo -e  Turn brother on via Bus B...; ./pcamswport 2 ff10 2e1d 0050 a189 0010) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Bus A...; ./pcamswport 2 ff10 2e1d 0040 a181 0000) >> $filelog");
# turn on/off S/K voltages (more needed)
system("/bin/echo -e  Testing S/K Voltages");
system("/bin/echo -e  Testing S/K Voltages >> $filelog");
# check trips here
system("(/bin/echo -e  Check Trips...; ./pcamswport 0 ff10 2e1d 0040 e183 0000) >> $filelog");
system("(/bin/echo -e  Read DCL status...; ./pcamswport 2 ff10 2e1d 0040 a182 0000) >> $filelog");
system("(/bin/echo -e  Turn off S0...; ./pcamswport 2 ff10 2e1d 0040 a18a 0001) >> $filelog");
system("(/bin/echo -e  Turn off K0...; ./pcamswport 2 ff10 2e1d 0040 a18a 0002) >> $filelog");
system("(/bin/echo -e  Turn off S1...; ./pcamswport 2 ff10 2e1d 0040 a18a 0004) >> $filelog");
system("(/bin/echo -e  Turn off K1...; ./pcamswport 2 ff10 2e1d 0040 a18a 0008) >> $filelog");
system("(/bin/echo -e  Turn off S2...; ./pcamswport 2 ff10 2e1d 0040 a18a 0010) >> $filelog");
system("(/bin/echo -e  Turn off K2...; ./pcamswport 2 ff10 2e1d 0040 a18a 0020) >> $filelog");
system("(/bin/echo -e  Turn off S3...; ./pcamswport 2 ff10 2e1d 0040 a18a 0040) >> $filelog");
system("(/bin/echo -e  Turn off K3...; ./pcamswport 2 ff10 2e1d 0040 a18a 0080) >> $filelog; sleep 1");
system("(/bin/echo -e  Read DCL status...; ./pcamswport 2 ff10 2e1d 0040 a182 0000) >> $filelog");
# read data from TDR
system("(/bin/echo -e  Turn on all S/K...; ./pcamswport 2 ff10 2e1d 0040 e18a 0000) >> $filelog; sleep 2");
# check trips here
system("(/bin/echo -e  Check Trips...; ./pcamswport 2 ff10 2e1d 0040 e183 0000) >> $filelog");
# read data from TDR
system("(/bin/echo -e  Read DCL status...; ./pcamswport 2 ff10 2e1d 0040 a182 0000) >> $filelog");
system("(/bin/echo -e  Turn off all S...; ./pcamswport 2 ff10 2e1d 0040 e18a 0055) >> $filelog; sleep 1");
system("(/bin/echo -e  Read DCL status...; ./pcamswport 2 ff10 2e1d 0040 a182 0000) >> $filelog");
system("(/bin/echo -e  Turn on S/off K...; ./pcamswport 2 ff10 2e1d 0040 e18a 00aa) >> $filelog; sleep 2");
system("(/bin/echo -e  Check Trips...; ./pcamswport 2 ff10 2e1d 0040 e183 0000) >> $filelog");
system("(/bin/echo -e  Read DCL status...; ./pcamswport 2 ff10 2e1d 0040 a182 0000) >> $filelog");
system("(/bin/echo -e  Turn on all S/K...; ./pcamswport 2 ff10 2e1d 0040 e18a 0000) >> $filelog; sleep 2");
system("(/bin/echo -e  Check Trips...; ./pcamswport 2 ff10 2e1d 0040 e183 0000) >> $filelog");
system("(/bin/echo -e  Read DCL status...; ./pcamswport 2 ff10 2e1d 0040 a182 0000) >> $filelog");
# turn on/off 120 volts (more needed)
# read ADC on TBS to check
system("/bin/echo -e  Testing 120 Volts");
system("/bin/echo -e  Testing 120 Volts >> $filelog");
system("(/bin/echo -e  Read DCH status...; ./pcamswport 2 ff10 2e1d 0040 a184 0000) >> $filelog");
system("(/bin/echo -e  Read TBS ADC 0 0x10...; ./pcamswport 2 ff10 2e1d 0000 a2a0 0000) > $filedummy");
$adc=readadc();
if ($adc lt "0db0") { print "ADC 0x10 not on, $adc! \n"; } 
system("(/bin/echo -e  Read TBS ADC 0 0x11...; ./pcamswport 2 ff10 2e1d 0000 e2a1 0000) > $filedummy");
$adc=readadc();
if ($adc lt "0db0") { print "ADC 0x11 not on, $adc! \n"; } 
system("(/bin/echo -e  Turn off 120 Volts...; ./pcamswport 2 ff10 2e1d 0040 e18c 00f0) >> $filelog; sleep 2");
system("(/bin/echo -e  Read DCH status...; ./pcamswport 2 ff10 2e1d 0040 a184 0000) >> $filelog");
system("(/bin/echo -e  Read TBS ADC 0 0x10...; ./pcamswport 2 ff10 2e1d 0000 a2a0 0000) > $filedummy");
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x10 not off, $adc! \n"; } 
system("(/bin/echo -e  Read TBS ADC 0 0x11...; ./pcamswport 2 ff10 2e1d 0000 e2a1 0000) > $filedummy");
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x11 not off, $adc! \n"; } 
system("(/bin/echo -e  Switch 120 Volts to cold...; ./pcamswport 2 ff10 2e1d 0040 e18c 0050) >> $filelog; sleep 2");
system("(/bin/echo -e  Read DCH status...; ./pcamswport 2 ff10 2e1d 0040 a184 0000) >> $filelog");
system("(/bin/echo -e  Read TBS ADC 0 0x10...; ./pcamswport 2 ff10 2e1d 0000 a2a0 0000) > $filedummy");
$adc=readadc();
if ($adc lt "0db0") { print "ADC 0x10 not on, $adc! \n"; } 
system("(/bin/echo -e  Read TBS ADC 0 0x11...; ./pcamswport 2 ff10 2e1d 0000 e2a1 0000) > $filedummy");
$adc=readadc();
if ($adc lt "0db0") { print "ADC 0x11 not on, $adc! \n"; } 
system("(/bin/echo -e  Switch 120 Volts to hot...; ./pcamswport 2 ff10 2e1d 0040 e18c 00a0) >> $filelog; sleep 2");
system("(/bin/echo -e  Read DCH status...; ./pcamswport 2 ff10 2e1d 0040 a184 0000) >> $filelog");
system("(/bin/echo -e  Read TBS ADC 0 0x10...; ./pcamswport 2 ff10 2e1d 0000 a2a0 0000) > $filedummy");
$adc=readadc();
if ($adc lt "0db0") { print "ADC 0x10 not on, $adc! \n"; } 
system("(/bin/echo -e  Read TBS ADC 0 0x11...; ./pcamswport 2 ff10 2e1d 0000 e2a1 0000) > $filedummy");
$adc=readadc();
if ($adc lt "0db0") { print "ADC 0x11 not on, $adc! \n"; } 
system("(/bin/echo -e  Switch 120 Volts both on default...; ./pcamswport 2 ff10 2e1d 0040 e18c 0000) >> $filelog; sleep 2");
system("(/bin/echo -e  Read DCH status...; ./pcamswport 2 ff10 2e1d 0040 a184 0000) >> $filelog");
system("(/bin/echo -e  Read TBS ADC 0 0x10...; ./pcamswport 2 ff10 2e1d 0000 a2a0 0000) > $filedummy");
$adc=readadc();
if ($adc lt "0db0") { print "ADC 0x10 not on, $adc! \n"; } 
system("(/bin/echo -e  Read TBS ADC 0 0x11...; ./pcamswport 2 ff10 2e1d 0000 e2a1 0000) > $filedummy");
$adc=readadc();
if ($adc lt "0db0") { print "ADC 0x11 not on, $adc! \n"; } 
######################################################################
# turn on/off 3.3 volts and verify 
######################################################################

######################################################################
# JINF A via Bus A can switch 3.3 B
#print FILELOG "Testing 3.3 Volts\n";
system("/bin/echo -e  Testing 3.3 Volts");
system("/bin/echo -e  Testing 3.3 Volts >> $filelog");
system("(/bin/echo -e  Turn off 3.3 Volts for B...; ./pcamswport 0 ff10 2e1d 0040 a18c 0004) >> $filelog; sleep 1");
# ping B to verify 
system("(/bin/echo -e  Ping B...; ./pcamswport 2 d) >> $filelog"); 
system("(/bin/echo -e  Turn on 3.3 Volts for B again...; ./pcamswport 0 ff10 2e1d 0040 e18c 0000) >> $filelog; sleep 1");
# ping B to verify 
system("(/bin/echo -e  Ping B...; ./pcamswport 2 d) >> $filelog"); 
######################################################################
# JINF B via Bus B can switch 3.3 A
system("(/bin/echo -e  Turn off 3.3 Volts for A...; ./pcamswport 2 ff10 2e1d 0050 a18c 0001) >> $filelog; sleep 1");
# ping A to verify 
system("(/bin/echo -e  Ping A...; ./pcamswport 0 d) >> $filelog"); 
system("(/bin/echo -e  Turn on 3.3 Volts for A again...; ./pcamswport 2 ff10 2e1d 0050 e18c 0000) >> $filelog; sleep 1");
# ping A to verify 
system("(/bin/echo -e  Ping A...; ./pcamswport 0 d) >> $filelog"); 
######################################################################
# JINF A/B via Bus B will glitch 3.3 B
system("(/bin/echo -e  Glitch 3.3 Volts for B...; ./pcamswport 0 ff10 2e1d 0050 a18c 0004) >> $filelog; sleep 1");
# here we still need to check the boot time!!!
system("(/bin/echo -e  Status B...; ./pcamswport 2 c 3) >> $filelog"); 
system("(/bin/echo -e  Status B...; ./pcamswport 2 c 7) > $filedummy"); 
system("/bin/echo -e Status B after 1st glitch");
$time=readtime();
if ($time > 3) { print "error: Glitchtime too big, time: $time\n";}
######################################################################
# JINF B/A via Bus A will glitch 3.3 A
system("(/bin/echo -e  Glitch 3.3 Volts for A...; ./pcamswport 2 ff10 2e1d 0040 a18c 0001) >> $filelog");
# here we still need to check the boot time!!!
system("(/bin/echo -e  Status A...; ./pcamswport 0 c 3) >> $filelog; sleep 1"); 
system("/bin/echo -e Status A after 1st glitch");
$time=readtime();
if ($time > 3) { print "error: Glitchtime too big, time: $time\n";}
######################################################################
# glitch both sides by switching from Hot to cold
system("(/bin/echo -e  Switch 3.3 Volts from Hot to Cold...; ./pcamswport 0 ff10 2e1d 0040 e18c 000f) >> $filelog; sleep 1");
# check glitch 
system("(/bin/echo -e  Status A...; ./pcamswport 0 c 3) >> $filelog"); 
system("(/bin/echo -e  Status A...; ./pcamswport 0 c 7) > $filedummy"); 
system("/bin/echo -e Status A after Hot-Cold");
$time=readtime();
if ($time > 3) { print "error: Glitchtime too big, time: $time\n";}
system("(/bin/echo -e  Status B...; ./pcamswport 2 c 3) >> $filelog"); 
system("(/bin/echo -e  Status B...; ./pcamswport 2 c 7) > $filedummy"); 
system("/bin/echo -e Status B after Hot-Cold");
$time=readtime();
if ($time > 3) { print "error: Glitchtime too big, time: $time\n";}
######################################################################
# back to default
system("(/bin/echo -e  Switch 3.3 Volts from Cold to Hot...; ./pcamswport 0 ff10 2e1d 0040 e18c 0000) >> $filelog; sleep 1");
# check glitch (to do)
system("(/bin/echo -e  Status A...; ./pcamswport 0 c 3) >> $filelog"); 
system("(/bin/echo -e  Status A...; ./pcamswport 0 c 7) > $filedummy"); 
system("/bin/echo -e Status A after Cold-Hot");
$time=readtime();
if ($time > 3) { print "error: Glitchtime too big, time: $time\n";}
system("(/bin/echo -e  Status B...; ./pcamswport 2 c 3) >> $filelog"); 
system("(/bin/echo -e  Status B...; ./pcamswport 2 c 7) > $filedummy"); 
system("/bin/echo -e Status B after Cold-Hot");
$time=readtime();
if ($time > 3) { print "error: Glitchtime too big, time: $time\n";}

# read back the file and verify for consistency
# do it simple: compare with a default correct output and just show differences, using 'diff'!!
print system("diff $filelog $filedef");

# repeat for JINF B via Port2/Port3
# read back the file and verify for consistency

close FILE;

exit;

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
