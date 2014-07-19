#!/usr/bin/perl 
# warnings on

$filelog="testtbs.log";
$filedef="testdeftbs.log";
my $filedummy="testdummy.log";

open(FILELOG,"> $filelog") or die "cannot open file\n"; 
open(FILEDUMMY,"> $filedummy") or die "cannot open file\n"; 

######################################################################
# Test of TBS 5 via JINF A, Half A
######################################################################
print  "Testing TBS 5 via JINF A, Half A\n";
system("/bin/echo -e  Testing TBS 5 via JINF A, Half A  >> $filelog");
# read status words
system("/bin/echo -e  Reading Status");
system("/bin/echo -e  Reading Status >> $filelog");
system("(echo Read SEL status...; ./pcamswport 0 ff10 2e1d 0000 e280 0000) >> $filelog"); 
system("(echo Read Actel status...; ./pcamswport 0 ff10 2e1d 0000 a281 0000) >> $filelog"); 
system("(echo Read Last Cmd...; ./pcamswport 0 ff10 2e1d 0000 a282 0000) >> $filelog"); 
system("(echo Read Feedback...; ./pcamswport 0 ff10 2e1d 0000 e283 0000) >> $filelog"); 
system("(echo Read JINF...; ./pcamswport 0 ff10 2e1d 0000 e285 0000) >> $filelog"); 
system("(echo Read ADCmod...; ./pcamswport 0 ff10 2e1d 0000 e286 0000) >> $filelog"); 
#system("(echo Read ADCdata...; ./pcamswport 0 ff10 2e1d 0000 a287 0000) >> $filelog"); 
# ADC values
system("(echo Read ADC 0 0x10...; ./pcamswport 0 ff10 2e1d 0000 a2a0 0000) > $filedummy");
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x10 too low, $adc! \n"; } 
system("(echo Read ADC 0 0x11...; ./pcamswport 0 ff10 2e1d 0000 e2a1 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x11 too low, $adc! \n"; } 
system("(echo Read ADC 0 0x12...; ./pcamswport 0 ff10 2e1d 0000 e2a2 0000) > $filedummy"); 
$adc=readadc();
if ($adc ne "0000") { print "ADC 0x12 not equal 0000, $adc! \n"; } 
system("(echo Read ADC 0 0x13...; ./pcamswport 0 ff10 2e1d 0000 a2a3 0000) > $filedummy"); 
$adc=readadc();
if ($adc ne "0000") { print "ADC 0x13 not equal 0000, $adc! \n"; } 
system("(echo Read ADC 0 0x14...; ./pcamswport 0 ff10 2e1d 0000 e2a4 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x14 too low, $adc! \n"; } 
system("(echo Read ADC 0 0x15...; ./pcamswport 0 ff10 2e1d 0000 a2a5 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x15 too low, $adc! \n"; } 
system("(echo Read ADC 0 0x16...; ./pcamswport 0 ff10 2e1d 0000 a2a6 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x16 too low, $adc! \n"; } 
system("(echo Read ADC 0 0x17...; ./pcamswport 0 ff10 2e1d 0000 e2a7 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x17 too low, $adc! \n"; } 
system("(echo Read ADC 1 0x18...; ./pcamswport 0 ff10 2e1d 0000 e2b0 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x18 too low, $adc! \n"; } 
system("(echo Read ADC 1 0x19...; ./pcamswport 0 ff10 2e1d 0000 a2b1 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x19 too low, $adc! \n"; } 
system("(echo Read ADC 1 0x1a...; ./pcamswport 0 ff10 2e1d 0000 a2b2 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x1a too low, $adc! \n"; } 
system("(echo Read ADC 1 0x1b...; ./pcamswport 0 ff10 2e1d 0000 e2b3 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x1b too low, $adc! \n"; } 
system("(echo Read ADC 1 0x1c...; ./pcamswport 0 ff10 2e1d 0000 a2b4 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x1c too low, $adc! \n"; } 
system("(echo Read ADC 1 0x1d...; ./pcamswport 0 ff10 2e1d 0000 e2b5 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x1d too low, $adc! \n"; } 
system("(echo Read ADC 1 0x1e...; ./pcamswport 0 ff10 2e1d 0000 e2b6 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x1e too low, $adc! \n"; } 
system("(echo Read ADC 1 0x1f...; ./pcamswport 0 ff10 2e1d 0000 a2b7 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x1f too low, $adc! \n"; } 
######################################################################
# switch on/off Actels (default is both on!)
system("/bin/echo -e  Testing Actels");
system("/bin/echo -e  Testing Actels >> $filelog");
system("(/bin/echo -e  Turn brother off via Half A...; ./pcamswport 0 ff10 2e1d 0000 a289 0001) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half B...; ./pcamswport 0 ff10 2e1d 0020 e280 0000) >> $filelog");
system("(/bin/echo -e  Turn brother on via Half A...; ./pcamswport 0 ff10 2e1d 0000 a289 0010) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half B...; ./pcamswport 0 ff10 2e1d 0020 e280 0000) >> $filelog");
system("(/bin/echo -e  Turn brother off via Half B...; ./pcamswport 0 ff10 2e1d 0020 a289 0001) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half A...; ./pcamswport 0 ff10 2e1d 0000 e280 0000) >> $filelog");
system("(/bin/echo -e  Turn brother on via Half B...; ./pcamswport 0 ff10 2e1d 0020 a289 0010) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half A...; ./pcamswport 0 ff10 2e1d 0000 e280 0000) >> $filelog");
######################################################################
# switch off 80 Volts
system("/bin/echo -e  Testing Voltages");
system("/bin/echo -e  Testing Voltages >> $filelog");
system("(/bin/echo -e  Switch off 80 Volts; ./pcamswport 0 ff10 2e1d 0000 e28a 000f) >> $filelog; sleep 2");
# ...and read Actels
system("(echo Read ADC 0 0x10...; ./pcamswport 0 ff10 2e1d 0000 a2a0 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x10 not off, $adc! \n"; } 
system("(echo Read ADC 0 0x11...; ./pcamswport 0 ff10 2e1d 0000 e2a1 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x11 not off, $adc! \n"; } 
system("(echo Read ADC 0 0x12...; ./pcamswport 0 ff10 2e1d 0000 e2a2 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x12 not off, $adc! \n"; } 
system("(echo Read ADC 0 0x13...; ./pcamswport 0 ff10 2e1d 0000 a2a3 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x13 not off, $adc! \n"; } 
# switch on again
system("(/bin/echo -e  Switch on 80 Volts; ./pcamswport 0 ff10 2e1d 0000 e28a 0000) >> $filelog; sleep 2");
# ...and read Actels
system("(echo Read ADC 0 0x10...; ./pcamswport 0 ff10 2e1d 0000 a2a0 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x10 not on, $adc! \n"; } 
system("(echo Read ADC 0 0x11...; ./pcamswport 0 ff10 2e1d 0000 e2a1 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x11 not on, $adc! \n"; } 
system("(echo Read ADC 0 0x12...; ./pcamswport 0 ff10 2e1d 0000 e2a2 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x12 not off, $adc! \n"; } 
system("(echo Read ADC 0 0x13...; ./pcamswport 0 ff10 2e1d 0000 a2a3 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x13 not off, $adc! \n"; } 
# switch to 60 Volts
system("(/bin/echo -e  Switch to 60 Volts; ./pcamswport 0 ff10 2e1d 0000 e28a 00f0) >> $filelog; sleep 2");
# ...and read Actels
system("(echo Read ADC 0 0x10...; ./pcamswport 0 ff10 2e1d 0000 a2a0 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0a70" || $adc gt "0a8f") { print "ADC 0x10 not at 60V, $adc! \n"; } 
system("(echo Read ADC 0 0x11...; ./pcamswport 0 ff10 2e1d 0000 e2a1 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0a70" || $adc gt "0a8f") { print "ADC 0x11 not at 60V, $adc! \n"; } 
system("(echo Read ADC 0 0x12...; ./pcamswport 0 ff10 2e1d 0000 e2a2 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x12 not off, $adc! \n"; } 
system("(echo Read ADC 0 0x13...; ./pcamswport 0 ff10 2e1d 0000 a2a3 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x13 not off, $adc! \n"; } 
# switch on again
system("(/bin/echo -e  Switch to 80 Volts; ./pcamswport 0 ff10 2e1d 0000 e28a 0000) >> $filelog; sleep 2");
# ...and read Actels
system("(echo Read ADC 0 0x10...; ./pcamswport 0 ff10 2e1d 0000 a2a0 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x10 not on, $adc! \n"; } 
system("(echo Read ADC 0 0x11...; ./pcamswport 0 ff10 2e1d 0000 e2a1 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x11 not on, $adc! \n"; } 
system("(echo Read ADC 0 0x12...; ./pcamswport 0 ff10 2e1d 0000 e2a2 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x12 not off, $adc! \n"; } 
system("(echo Read ADC 0 0x13...; ./pcamswport 0 ff10 2e1d 0000 a2a3 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x13 not off, $adc! \n"; } 
# switch to cold part
system("(/bin/echo -e  Switch to Cold 80 Volts; ./pcamswport 0 ff10 2e1d 0000 e28a 0003) >> $filelog; sleep 2");
# ...and read Actels
system("(echo Read ADC 0 0x10...; ./pcamswport 0 ff10 2e1d 0000 a2a0 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x10 still on, $adc! \n"; } 
system("(echo Read ADC 0 0x11...; ./pcamswport 0 ff10 2e1d 0000 e2a1 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x11 still on, $adc! \n"; } 
system("(echo Read ADC 0 0x12...; ./pcamswport 0 ff10 2e1d 0000 e2a2 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d90") { print "ADC 0x12 not on, $adc! \n"; } 
system("(echo Read ADC 0 0x13...; ./pcamswport 0 ff10 2e1d 0000 a2a3 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d90") { print "ADC 0x13 not on, $adc! \n"; } 
# switch back to hot again
system("(/bin/echo -e  Switch to Hot 80 Volts; ./pcamswport 0 ff10 2e1d 0000 e28a 0000) >> $filelog; sleep 2");
# ...and read Actels
system("(echo Read ADC 0 0x10...; ./pcamswport 0 ff10 2e1d 0000 a2a0 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x10 not on, $adc! \n"; } 
system("(echo Read ADC 0 0x11...; ./pcamswport 0 ff10 2e1d 0000 e2a1 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x11 not on, $adc! \n"; } 
system("(echo Read ADC 0 0x12...; ./pcamswport 0 ff10 2e1d 0000 e2a2 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x12 not off, $adc! \n"; } 
system("(echo Read ADC 0 0x13...; ./pcamswport 0 ff10 2e1d 0000 a2a3 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x13 not off, $adc! \n"; } 

######################################################################
# Test of TBS 15 via JINF A, Half A
######################################################################
print  "Testing TBS 15 via JINF A, Half A\n";
system("/bin/echo -e  Testing TBS 15 via JINF A, Half A  >> $filelog");
# read status words
system("/bin/echo -e  Reading Status");
system("/bin/echo -e  Reading Status >> $filelog");
system("(echo Read SEL status...; ./pcamswport 0 ff10 2e1d 0010 e780 0000) >> $filelog"); 
system("(echo Read Actel status...; ./pcamswport 0 ff10 2e1d 0010 a781 0000) >> $filelog"); 
system("(echo Read Last Cmd...; ./pcamswport 0 ff10 2e1d 0010 a782 0000) >> $filelog"); 
system("(echo Read Feedback...; ./pcamswport 0 ff10 2e1d 0010 e783 0000) >> $filelog"); 
system("(echo Read JINF...; ./pcamswport 0 ff10 2e1d 0010 e785 0000) >> $filelog"); 
system("(echo Read ADCmod...; ./pcamswport 0 ff10 2e1d 0010 e786 0000) >> $filelog"); 
#system("(echo Read ADCdata...; ./pcamswport 0 ff10 2e1d 0010 a787 0000) >> $filelog"); 
# ADC values
system("(echo Read ADC 0 0x10...; ./pcamswport 0 ff10 2e1d 0010 a7a0 0000) > $filedummy");
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x10 too low, $adc! \n"; } 
system("(echo Read ADC 0 0x11...; ./pcamswport 0 ff10 2e1d 0010 e7a1 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x11 too low, $adc! \n"; } 
system("(echo Read ADC 0 0x12...; ./pcamswport 0 ff10 2e1d 0010 e7a2 0000) > $filedummy"); 
$adc=readadc();
if ($adc ne "0000") { print "ADC 0x12 not equal 0000, $adc! \n"; } 
system("(echo Read ADC 0 0x13...; ./pcamswport 0 ff10 2e1d 0010 a7a3 0000) > $filedummy"); 
$adc=readadc();
if ($adc ne "0000") { print "ADC 0x13 not equal 0000, $adc! \n"; } 
system("(echo Read ADC 0 0x14...; ./pcamswport 0 ff10 2e1d 0010 e7a4 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x14 too low, $adc! \n"; } 
system("(echo Read ADC 0 0x15...; ./pcamswport 0 ff10 2e1d 0010 a7a5 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x15 too low, $adc! \n"; } 
system("(echo Read ADC 0 0x16...; ./pcamswport 0 ff10 2e1d 0010 a7a6 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x16 too low, $adc! \n"; } 
system("(echo Read ADC 0 0x17...; ./pcamswport 0 ff10 2e1d 0010 e7a7 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x17 too low, $adc! \n"; } 
system("(echo Read ADC 1 0x18...; ./pcamswport 0 ff10 2e1d 0010 e7b0 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x18 too low, $adc! \n"; } 
system("(echo Read ADC 1 0x19...; ./pcamswport 0 ff10 2e1d 0010 a7b1 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x19 too low, $adc! \n"; } 
system("(echo Read ADC 1 0x1a...; ./pcamswport 0 ff10 2e1d 0010 a7b2 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x1a too low, $adc! \n"; } 
system("(echo Read ADC 1 0x1b...; ./pcamswport 0 ff10 2e1d 0010 e7b3 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x1b too low, $adc! \n"; } 
system("(echo Read ADC 1 0x1c...; ./pcamswport 0 ff10 2e1d 0010 a7b4 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x1c too low, $adc! \n"; } 
system("(echo Read ADC 1 0x1d...; ./pcamswport 0 ff10 2e1d 0010 e7b5 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x1d too low, $adc! \n"; } 
system("(echo Read ADC 1 0x1e...; ./pcamswport 0 ff10 2e1d 0010 e7b6 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x1e too low, $adc! \n"; } 
system("(echo Read ADC 1 0x1f...; ./pcamswport 0 ff10 2e1d 0010 a7b7 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x1f too low, $adc! \n"; } 
######################################################################
# switch on/off Actels (default is both on!)
system("/bin/echo -e  Testing Actels");
system("/bin/echo -e  Testing Actels >> $filelog");
system("(/bin/echo -e  Turn brother off via Half A...; ./pcamswport 0 ff10 2e1d 0010 a789 0001) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half B...; ./pcamswport 0 ff10 2e1d 0030 e780 0000) >> $filelog");
system("(/bin/echo -e  Turn brother on via Half A...; ./pcamswport 0 ff10 2e1d 0010 a789 0010) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half B...; ./pcamswport 0 ff10 2e1d 0030 e780 0000) >> $filelog");
system("(/bin/echo -e  Turn brother off via Half B...; ./pcamswport 0 ff10 2e1d 0030 a789 0001) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half A...; ./pcamswport 0 ff10 2e1d 0010 e780 0000) >> $filelog");
system("(/bin/echo -e  Turn brother on via Half B...; ./pcamswport 0 ff10 2e1d 0030 a789 0010) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half A...; ./pcamswport 0 ff10 2e1d 0010 e780 0000) >> $filelog");
######################################################################
# switch off 80 Volts
system("/bin/echo -e  Testing Voltages");
system("/bin/echo -e  Testing Voltages >> $filelog");
system("(/bin/echo -e  Switch off 80 Volts; ./pcamswport 0 ff10 2e1d 0010 e78a 000f) >> $filelog; sleep 2");
# ...and read Actels
system("(echo Read ADC 0 0x10...; ./pcamswport 0 ff10 2e1d 0010 a7a0 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x10 not off, $adc! \n"; } 
system("(echo Read ADC 0 0x11...; ./pcamswport 0 ff10 2e1d 0010 e7a1 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x11 not off, $adc! \n"; } 
system("(echo Read ADC 0 0x12...; ./pcamswport 0 ff10 2e1d 0010 e7a2 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x12 not off, $adc! \n"; } 
system("(echo Read ADC 0 0x13...; ./pcamswport 0 ff10 2e1d 0010 a7a3 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x13 not off, $adc! \n"; } 
# switch on again
system("(/bin/echo -e  Switch on 80 Volts; ./pcamswport 0 ff10 2e1d 0010 e78a 0000) >> $filelog; sleep 2");
# ...and read Actels
system("(echo Read ADC 0 0x10...; ./pcamswport 0 ff10 2e1d 0010 a7a0 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x10 not on, $adc! \n"; } 
system("(echo Read ADC 0 0x11...; ./pcamswport 0 ff10 2e1d 0010 e7a1 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x11 not on, $adc! \n"; } 
system("(echo Read ADC 0 0x12...; ./pcamswport 0 ff10 2e1d 0010 e7a2 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x12 not off, $adc! \n"; } 
system("(echo Read ADC 0 0x13...; ./pcamswport 0 ff10 2e1d 0010 a7a3 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x13 not off, $adc! \n"; } 
# switch to 60 Volts
system("(/bin/echo -e  Switch to 60 Volts; ./pcamswport 0 ff10 2e1d 0010 e78a 00f0) >> $filelog; sleep 2");
# ...and read Actels
system("(echo Read ADC 0 0x10...; ./pcamswport 0 ff10 2e1d 0010 a7a0 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0a70" || $adc gt "0a8f") { print "ADC 0x10 not at 60V, $adc! \n"; } 
system("(echo Read ADC 0 0x11...; ./pcamswport 0 ff10 2e1d 0010 e7a1 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0a70" || $adc gt "0a8f") { print "ADC 0x11 not at 60V, $adc! \n"; } 
system("(echo Read ADC 0 0x12...; ./pcamswport 0 ff10 2e1d 0010 e7a2 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x12 not off, $adc! \n"; } 
system("(echo Read ADC 0 0x13...; ./pcamswport 0 ff10 2e1d 0010 a7a3 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x13 not off, $adc! \n"; } 
# switch on again
system("(/bin/echo -e  Switch to 80 Volts; ./pcamswport 0 ff10 2e1d 0010 e78a 0000) >> $filelog; sleep 2");
# ...and read Actels
system("(echo Read ADC 0 0x10...; ./pcamswport 0 ff10 2e1d 0010 a7a0 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x10 not on, $adc! \n"; } 
system("(echo Read ADC 0 0x11...; ./pcamswport 0 ff10 2e1d 0010 e7a1 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x11 not on, $adc! \n"; } 
system("(echo Read ADC 0 0x12...; ./pcamswport 0 ff10 2e1d 0010 e7a2 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x12 not off, $adc! \n"; } 
system("(echo Read ADC 0 0x13...; ./pcamswport 0 ff10 2e1d 0010 a7a3 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x13 not off, $adc! \n"; } 
# switch to cold part
system("(/bin/echo -e  Switch to Cold 80 Volts; ./pcamswport 0 ff10 2e1d 0010 e78a 0003) >> $filelog; sleep 2");
# ...and read Actels
system("(echo Read ADC 0 0x10...; ./pcamswport 0 ff10 2e1d 0010 a7a0 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x10 still on, $adc! \n"; } 
system("(echo Read ADC 0 0x11...; ./pcamswport 0 ff10 2e1d 0010 e7a1 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x11 still on, $adc! \n"; } 
system("(echo Read ADC 0 0x12...; ./pcamswport 0 ff10 2e1d 0010 e7a2 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d90") { print "ADC 0x12 not on, $adc! \n"; } 
system("(echo Read ADC 0 0x13...; ./pcamswport 0 ff10 2e1d 0010 a7a3 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d90") { print "ADC 0x13 not on, $adc! \n"; } 
# switch back to hot again
system("(/bin/echo -e  Switch to Hot 80 Volts; ./pcamswport 0 ff10 2e1d 0010 e78a 0000) >> $filelog; sleep 2");
# ...and read Actels
system("(echo Read ADC 0 0x10...; ./pcamswport 0 ff10 2e1d 0010 a7a0 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x10 not on, $adc! \n"; } 
system("(echo Read ADC 0 0x11...; ./pcamswport 0 ff10 2e1d 0010 e7a1 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x11 not on, $adc! \n"; } 
system("(echo Read ADC 0 0x12...; ./pcamswport 0 ff10 2e1d 0010 e7a2 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x12 not off, $adc! \n"; } 
system("(echo Read ADC 0 0x13...; ./pcamswport 0 ff10 2e1d 0010 a7a3 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x13 not off, $adc! \n"; } 

######################################################################
# Test of TBS 5 via JINF B, Half A
######################################################################
print  "Testing TBS 5 via JINF B, Half A\n";
system("/bin/echo -e  Testing TBS 5 via JINF B, Half A  >> $filelog");
# read status words
system("/bin/echo -e  Reading Status");
system("/bin/echo -e  Reading Status >> $filelog");
system("(echo Read SEL status...; ./pcamswport 2 ff10 2e1d 0000 e280 0000) >> $filelog"); 
system("(echo Read Actel status...; ./pcamswport 2 ff10 2e1d 0000 a281 0000) >> $filelog"); 
system("(echo Read Last Cmd...; ./pcamswport 2 ff10 2e1d 0000 a282 0000) >> $filelog"); 
system("(echo Read Feedback...; ./pcamswport 2 ff10 2e1d 0000 e283 0000) >> $filelog"); 
system("(echo Read JINF...; ./pcamswport 2 ff10 2e1d 0000 e285 0000) >> $filelog"); 
system("(echo Read ADCmod...; ./pcamswport 2 ff10 2e1d 0000 e286 0000) >> $filelog"); 
#system("(echo Read ADCdata...; ./pcamswport 2 ff10 2e1d 0000 a287 0000) >> $filelog"); 
# ADC values
system("(echo Read ADC 0 0x10...; ./pcamswport 2 ff10 2e1d 0000 a2a0 0000) > $filedummy");
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x10 too low, $adc! \n"; } 
system("(echo Read ADC 0 0x11...; ./pcamswport 2 ff10 2e1d 0000 e2a1 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x11 too low, $adc! \n"; } 
system("(echo Read ADC 0 0x12...; ./pcamswport 2 ff10 2e1d 0000 e2a2 0000) > $filedummy"); 
$adc=readadc();
if ($adc ne "0000") { print "ADC 0x12 not equal 0000, $adc! \n"; } 
system("(echo Read ADC 0 0x13...; ./pcamswport 2 ff10 2e1d 0000 a2a3 0000) > $filedummy"); 
$adc=readadc();
if ($adc ne "0000") { print "ADC 0x13 not equal 0000, $adc! \n"; } 
system("(echo Read ADC 0 0x14...; ./pcamswport 2 ff10 2e1d 0000 e2a4 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x14 too low, $adc! \n"; } 
system("(echo Read ADC 0 0x15...; ./pcamswport 2 ff10 2e1d 0000 a2a5 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x15 too low, $adc! \n"; } 
system("(echo Read ADC 0 0x16...; ./pcamswport 2 ff10 2e1d 0000 a2a6 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x16 too low, $adc! \n"; } 
system("(echo Read ADC 0 0x17...; ./pcamswport 2 ff10 2e1d 0000 e2a7 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x17 too low, $adc! \n"; } 
system("(echo Read ADC 1 0x18...; ./pcamswport 2 ff10 2e1d 0000 e2b0 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x18 too low, $adc! \n"; } 
system("(echo Read ADC 1 0x19...; ./pcamswport 2 ff10 2e1d 0000 a2b1 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x19 too low, $adc! \n"; } 
system("(echo Read ADC 1 0x1a...; ./pcamswport 2 ff10 2e1d 0000 a2b2 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x1a too low, $adc! \n"; } 
system("(echo Read ADC 1 0x1b...; ./pcamswport 2 ff10 2e1d 0000 e2b3 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x1b too low, $adc! \n"; } 
system("(echo Read ADC 1 0x1c...; ./pcamswport 2 ff10 2e1d 0000 a2b4 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x1c too low, $adc! \n"; } 
system("(echo Read ADC 1 0x1d...; ./pcamswport 2 ff10 2e1d 0000 e2b5 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x1d too low, $adc! \n"; } 
system("(echo Read ADC 1 0x1e...; ./pcamswport 2 ff10 2e1d 0000 e2b6 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x1e too low, $adc! \n"; } 
system("(echo Read ADC 1 0x1f...; ./pcamswport 2 ff10 2e1d 0000 a2b7 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x1f too low, $adc! \n"; } 
######################################################################
# switch on/off Actels (default is both on!)
system("/bin/echo -e  Testing Actels");
system("/bin/echo -e  Testing Actels >> $filelog");
system("(/bin/echo -e  Turn brother off via Half A...; ./pcamswport 2 ff10 2e1d 0000 a289 0001) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half B...; ./pcamswport 2 ff10 2e1d 0020 e280 0000) >> $filelog");
system("(/bin/echo -e  Turn brother on via Half A...; ./pcamswport 2 ff10 2e1d 0000 a289 0010) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half B...; ./pcamswport 2 ff10 2e1d 0020 e280 0000) >> $filelog");
system("(/bin/echo -e  Turn brother off via Half B...; ./pcamswport 2 ff10 2e1d 0020 a289 0001) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half A...; ./pcamswport 2 ff10 2e1d 0000 e280 0000) >> $filelog");
system("(/bin/echo -e  Turn brother on via Half B...; ./pcamswport 2 ff10 2e1d 0020 a289 0010) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half A...; ./pcamswport 2 ff10 2e1d 0000 e280 0000) >> $filelog");
######################################################################
# switch off 80 Volts
system("/bin/echo -e  Testing Voltages");
system("/bin/echo -e  Testing Voltages >> $filelog");
system("(/bin/echo -e  Switch off 80 Volts; ./pcamswport 2 ff10 2e1d 0000 e28a 000f) >> $filelog; sleep 2");
# ...and read Actels
system("(echo Read ADC 0 0x10...; ./pcamswport 2 ff10 2e1d 0000 a2a0 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x10 not off, $adc! \n"; } 
system("(echo Read ADC 0 0x11...; ./pcamswport 2 ff10 2e1d 0000 e2a1 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x11 not off, $adc! \n"; } 
# switch on again
system("(/bin/echo -e  Switch on 80 Volts; ./pcamswport 2 ff10 2e1d 0000 e28a 0000) >> $filelog; sleep 2");
# ...and read Actels
system("(echo Read ADC 0 0x10...; ./pcamswport 2 ff10 2e1d 0000 a2a0 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x10 not on, $adc! \n"; } 
system("(echo Read ADC 0 0x11...; ./pcamswport 2 ff10 2e1d 0000 e2a1 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x11 not on, $adc! \n"; } 
# switch to 60 Volts
system("(/bin/echo -e  Switch to 60 Volts; ./pcamswport 2 ff10 2e1d 0000 e28a 00f0) >> $filelog; sleep 2");
# ...and read Actels
system("(echo Read ADC 0 0x10...; ./pcamswport 2 ff10 2e1d 0000 a2a0 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0a70" || $adc gt "0a8f") { print "ADC 0x10 not at 60V, $adc! \n"; } 
system("(echo Read ADC 0 0x11...; ./pcamswport 2 ff10 2e1d 0000 e2a1 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0a70" || $adc gt "0a8f") { print "ADC 0x11 not at 60V, $adc! \n"; } 
# switch on again
system("(/bin/echo -e  Switch to 80 Volts; ./pcamswport 2 ff10 2e1d 0000 e28a 0000) >> $filelog; sleep 2");
# ...and read Actels
system("(echo Read ADC 0 0x10...; ./pcamswport 2 ff10 2e1d 0000 a2a0 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x10 not on, $adc! \n"; } 
system("(echo Read ADC 0 0x11...; ./pcamswport 2 ff10 2e1d 0000 e2a1 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x11 not on, $adc! \n"; } 

######################################################################
# Test of TBS 15 via JINF B, Half A
######################################################################
print  "Testing TBS 15 via JINF B, Half A\n";
system("/bin/echo -e  Testing TBS 15 via JINF B, Half A  >> $filelog");
# read status words
system("/bin/echo -e  Reading Status");
system("/bin/echo -e  Reading Status >> $filelog");
system("(echo Read SEL status...; ./pcamswport 2 ff10 2e1d 0010 e780 0000) >> $filelog"); 
system("(echo Read Actel status...; ./pcamswport 2 ff10 2e1d 0010 a781 0000) >> $filelog"); 
system("(echo Read Last Cmd...; ./pcamswport 2 ff10 2e1d 0010 a782 0000) >> $filelog"); 
system("(echo Read Feedback...; ./pcamswport 2 ff10 2e1d 0010 e783 0000) >> $filelog"); 
system("(echo Read JINF...; ./pcamswport 2 ff10 2e1d 0010 e785 0000) >> $filelog"); 
system("(echo Read ADCmod...; ./pcamswport 2 ff10 2e1d 0010 e786 0000) >> $filelog"); 
#system("(echo Read ADCdata...; ./pcamswport 2 ff10 2e1d 0010 a787 0000) >> $filelog"); 
# ADC values
system("(echo Read ADC 0 0x10...; ./pcamswport 2 ff10 2e1d 0010 a7a0 0000) > $filedummy");
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x10 too low, $adc! \n"; } 
system("(echo Read ADC 0 0x11...; ./pcamswport 2 ff10 2e1d 0010 e7a1 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x11 too low, $adc! \n"; } 
system("(echo Read ADC 0 0x12...; ./pcamswport 2 ff10 2e1d 0010 e7a2 0000) > $filedummy"); 
$adc=readadc();
if ($adc ne "0000") { print "ADC 0x12 not equal 0000, $adc! \n"; } 
system("(echo Read ADC 0 0x13...; ./pcamswport 2 ff10 2e1d 0010 a7a3 0000) > $filedummy"); 
$adc=readadc();
if ($adc ne "0000") { print "ADC 0x13 not equal 0000, $adc! \n"; } 
system("(echo Read ADC 0 0x14...; ./pcamswport 2 ff10 2e1d 0010 e7a4 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x14 too low, $adc! \n"; } 
system("(echo Read ADC 0 0x15...; ./pcamswport 2 ff10 2e1d 0010 a7a5 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x15 too low, $adc! \n"; } 
system("(echo Read ADC 0 0x16...; ./pcamswport 2 ff10 2e1d 0010 a7a6 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x16 too low, $adc! \n"; } 
system("(echo Read ADC 0 0x17...; ./pcamswport 2 ff10 2e1d 0010 e7a7 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x17 too low, $adc! \n"; } 
system("(echo Read ADC 1 0x18...; ./pcamswport 2 ff10 2e1d 0010 e7b0 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x18 too low, $adc! \n"; } 
system("(echo Read ADC 1 0x19...; ./pcamswport 2 ff10 2e1d 0010 a7b1 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x19 too low, $adc! \n"; } 
system("(echo Read ADC 1 0x1a...; ./pcamswport 2 ff10 2e1d 0010 a7b2 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x1a too low, $adc! \n"; } 
system("(echo Read ADC 1 0x1b...; ./pcamswport 2 ff10 2e1d 0010 e7b3 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x1b too low, $adc! \n"; } 
system("(echo Read ADC 1 0x1c...; ./pcamswport 2 ff10 2e1d 0010 a7b4 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x1c too low, $adc! \n"; } 
system("(echo Read ADC 1 0x1d...; ./pcamswport 2 ff10 2e1d 0010 e7b5 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x1d too low, $adc! \n"; } 
system("(echo Read ADC 1 0x1e...; ./pcamswport 2 ff10 2e1d 0010 e7b6 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x1e too low, $adc! \n"; } 
system("(echo Read ADC 1 0x1f...; ./pcamswport 2 ff10 2e1d 0010 a7b7 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0d10") { print "ADC 0x1f too low, $adc! \n"; } 
######################################################################
# switch on/off Actels (default is both on!)
system("/bin/echo -e  Testing Actels");
system("/bin/echo -e  Testing Actels >> $filelog");
system("(/bin/echo -e  Turn brother off via Half A...; ./pcamswport 2 ff10 2e1d 0010 a789 0001) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half B...; ./pcamswport 2 ff10 2e1d 0030 e780 0000) >> $filelog");
system("(/bin/echo -e  Turn brother on via Half A...; ./pcamswport 2 ff10 2e1d 0010 a789 0010) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half B...; ./pcamswport 2 ff10 2e1d 0030 e780 0000) >> $filelog");
system("(/bin/echo -e  Turn brother off via Half B...; ./pcamswport 2 ff10 2e1d 0030 a789 0001) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half A...; ./pcamswport 2 ff10 2e1d 0010 e780 0000) >> $filelog");
system("(/bin/echo -e  Turn brother on via Half B...; ./pcamswport 2 ff10 2e1d 0030 a789 0010) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half A...; ./pcamswport 2 ff10 2e1d 0010 e780 0000) >> $filelog");
######################################################################
# switch off 80 Volts
system("/bin/echo -e  Testing Voltages");
system("/bin/echo -e  Testing Voltages >> $filelog");
system("(/bin/echo -e  Switch off 80 Volts; ./pcamswport 2 ff10 2e1d 0010 e78a 000f) >> $filelog; sleep 2");
# ...and read Actels
system("(echo Read ADC 0 0x10...; ./pcamswport 2 ff10 2e1d 0010 a7a0 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x10 not off, $adc! \n"; } 
system("(echo Read ADC 0 0x11...; ./pcamswport 2 ff10 2e1d 0010 e7a1 0000) > $filedummy"); 
$adc=readadc();
if ($adc gt "0005") { print "ADC 0x11 not off, $adc! \n"; } 
# switch on again
system("(/bin/echo -e  Switch on 80 Volts; ./pcamswport 2 ff10 2e1d 0010 e78a 0000) >> $filelog; sleep 2");
# ...and read Actels
system("(echo Read ADC 0 0x10...; ./pcamswport 2 ff10 2e1d 0010 a7a0 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x10 not on, $adc! \n"; } 
system("(echo Read ADC 0 0x11...; ./pcamswport 2 ff10 2e1d 0010 e7a1 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x11 not on, $adc! \n"; } 
# switch to 60 Volts
system("(/bin/echo -e  Switch to 60 Volts; ./pcamswport 2 ff10 2e1d 0010 e78a 00f0) >> $filelog; sleep 2");
# ...and read Actels
system("(echo Read ADC 0 0x10...; ./pcamswport 2 ff10 2e1d 0010 a7a0 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0a70" || $adc gt "0a8f") { print "ADC 0x10 not at 60V, $adc! \n"; } 
system("(echo Read ADC 0 0x11...; ./pcamswport 2 ff10 2e1d 0010 e7a1 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0a70" || $adc gt "0a8f") { print "ADC 0x11 not at 60V, $adc! \n"; } 
# switch on again
system("(/bin/echo -e  Switch to 80 Volts; ./pcamswport 2 ff10 2e1d 0010 e78a 0000) >> $filelog; sleep 2");
# ...and read Actels
system("(echo Read ADC 0 0x10...; ./pcamswport 2 ff10 2e1d 0010 a7a0 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x10 not on, $adc! \n"; } 
system("(echo Read ADC 0 0x11...; ./pcamswport 2 ff10 2e1d 0010 e7a1 0000) > $filedummy"); 
$adc=readadc();
if ($adc lt "0da0") { print "ADC 0x11 not on, $adc! \n"; } 

# read back the file and verify for consistency
# do it simple: compare with a default correct output and just show differences, using 'diff'!!
print system("diff $filelog $filedef");

close FILELOG;

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
