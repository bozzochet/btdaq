#!/usr/bin/perl 
# warnings on

$filelog="testtpsfe.log";
$filedef="testdeftpsfe.log";
my $filedummy="testdummy.log";

open(FILELOG,"> $filelog") or die "cannot open file\n"; 
open(FILEDUMMY,"> $filedummy") or die "cannot open file\n"; 

# List of TDRs
@tdr=('003f','013f','023f','033f','043f','053f','063f','073f','083f','093f','0a3f','0b3f','0c3f','0d3f','0e3f','0f3f','103f','113f','123f','133f','143f','153f','163f','173f');


######################################################################
# Test of TPSFE 4 via JINF A, Half A
######################################################################
print  "Testing TPSFE 4 via JINF A, Half A\n";
system("/bin/echo -e  Testing TPSFE 4 via JINF A, Half A  >> $filelog");
# read status words
system("/bin/echo -e  Reading Status");
system("/bin/echo -e  Reading Status >> $filelog");
system("(echo Read SEL status...; ./pcamswport 0 ff10 2e1d 0000 a200 0000) >> $filelog"); 
system("(echo Read ACTEL status...; ./pcamswport 0 ff10 2e1d 0000 e201 0000) >> $filelog"); 
system("(echo Read LRS last cmd...; ./pcamswport 0 ff10 2e1d 0000 e202 0000) >> $filelog"); 
system("(echo Read LRK last cmd...; ./pcamswport 0 ff10 2e1d 0000 a203 0000) >> $filelog"); 
system("(echo Read LR fb status...; ./pcamswport 0 ff10 2e1d 0000 e204 0000) >> $filelog"); 
system("(echo Read TDR2 last cmd...; ./pcamswport 0 ff10 2e1d 0000 a205 0000) >> $filelog"); 
system("(echo Read Auto status...; ./pcamswport 0 ff10 2e1d 0000 a206 0000) >> $filelog"); 
system("(echo Read Actel trip cntr...; ./pcamswport 0 ff10 2e1d 0000 e207 0000) >> $filelog"); 
system("(echo Read LRS0-3 trip cntr...; ./pcamswport 0 ff10 2e1d 0000 e210 0000) >> $filelog"); 
system("(echo Read LRS4-5 trip cntr...; ./pcamswport 0 ff10 2e1d 0000 a211 0000) >> $filelog"); 
system("(echo Read LRK0-3 trip cntr...; ./pcamswport 0 ff10 2e1d 0000 a212 0000) >> $filelog"); 
system("(echo Read LRK4-5 trip cntr...; ./pcamswport 0 ff10 2e1d 0000 e213 0000) >> $filelog"); 
system("(echo Read TDR0-3 trip cntr...; ./pcamswport 0 ff10 2e1d 0000 e215 0000) >> $filelog"); 
system("(echo Read TDR4-5 trip cntr...; ./pcamswport 0 ff10 2e1d 0000 e216 0000) >> $filelog"); 
######################################################################
# switch on/off Actels (default is both on!)
system("/bin/echo -e  Testing Actels");
system("/bin/echo -e  Testing Actels >> $filelog");
system("(/bin/echo -e  Turn brother off via Half A...; ./pcamswport 0 ff10 2e1d 0000 e209 0001) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half B...; ./pcamswport 0 ff10 2e1d 0020 e280 0000) >> $filelog");
system("(/bin/echo -e  Turn brother on via Half A...; ./pcamswport 0 ff10 2e1d 0000 e209 0010) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half B...; ./pcamswport 0 ff10 2e1d 0020 e280 0000) >> $filelog");
system("(/bin/echo -e  Turn brother off via Half B...; ./pcamswport 0 ff10 2e1d 0020 e209 0001) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half A...; ./pcamswport 0 ff10 2e1d 0000 e280 0000) >> $filelog");
system("(/bin/echo -e  Turn brother on via Half B...; ./pcamswport 0 ff10 2e1d 0020 e209 0010) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half A...; ./pcamswport 0 ff10 2e1d 0000 e280 0000) >> $filelog");
system("(echo Read Actel trip cntr on A...; ./pcamswport 0 ff10 2e1d 0000 e207 0000) >> $filelog"); 
system("(echo Read Actel trip cntr on B...; ./pcamswport 0 ff10 2e1d 0020 e207 0000) >> $filelog"); 
system("(echo Read Actel trip again on A, autoclear...; ./pcamswport 0 ff10 2e1d 0000 e207 0000) >> $filelog"); 
system("(echo Read Actel trip again on B, autoclear...; ./pcamswport 0 ff10 2e1d 0020 e207 0000) >> $filelog"); 
# trip Actels more than once to check counters
system("/bin/echo -e  Tripping Actels");
system("/bin/echo -e  Tripping Actels >> $filelog");
for ($i=0;$i<15;$i++) {
    system("(/bin/echo -e  Turn brother off via Half A...; ./pcamswport 0 ff10 2e1d 0000 e209 0001) >> $filelog; sleep 1");
    system("(/bin/echo -e  Turn brother on via Half A...; ./pcamswport 0 ff10 2e1d 0000 e209 0010) >> $filelog; sleep 1");
}
system("(echo Read Actel trip cntr on A...; ./pcamswport 0 ff10 2e1d 0000 e207 0000) >> $filelog"); for ($i=0;$i<15;$i++) {
    system("(/bin/echo -e  Turn brother off via Half B...; ./pcamswport 0 ff10 2e1d 0020 e209 0001) >> $filelog; usleep 200000");
    system("(/bin/echo -e  Turn brother on via Half B...; ./pcamswport 0 ff10 2e1d 0020 e209 0010) >> $filelog; usleep 200000");
}
system("(echo Read Actel trip cntr on B...; ./pcamswport 0 ff10 2e1d 0020 e207 0000) >> $filelog");
# clear trip cntrs
system("(echo Read Actel trip again on A, autoclear...; ./pcamswport 0 ff10 2e1d 0000 e207 0000) >> $filelog"); 
system("(echo Read Actel trip again on B, autoclear...; ./pcamswport 0 ff10 2e1d 0020 e207 0000) >> $filelog"); 

######################################################################
# switch S, K, TDRs
system("/bin/echo -e  Testing S,K,TDRs");
system("/bin/echo -e  Testing S,K,TDRs >> $filelog");
system("/bin/echo -e Switch off all LRS...");
system("(echo Switch off all LRS...; ./pcamswport 0 ff10 2e1d 0000 a20a 003f) >> $filelog; sleep 1"); 
# to check: calibrate TDRs and read pedestals, if off should be at 4095
system("(echo Calibrate all TDR...; ./pcamswport 0 ff10 4000 2e53) >> $filelog; sleep 1"); 
$out="";
for ($i=0;$i<24;$i++) {
    system("(echo Read Pedestals TDR $tdr...; ./pcamswport 0 ff10 $tdr[$i] 2e10 0004 1300) > $filedummy"); 
    $tmp=readtdr(); $out= $out . $tmp;
}
if ($out ne "010100000101000001010000000000000000000000000000") { print "Wrong answer switching off LRS: $out\n"; } 
system("/bin/echo -e Switch on all LRS...");
system("(echo Switch on all LRS...; ./pcamswport 0 ff10 2e1d 0000 a20a 0000) >> $filelog; sleep 1"); 
system("/bin/echo -e Switch off all LRK...");
system("(echo Switch off all LRK...; ./pcamswport 0 ff10 2e1d 0000 e20b 003f) >> $filelog; sleep 1"); 
# to check: calibrate TDRs and read pedestals, if off should be at 4095
system("(echo Calibrate all TDR...; ./pcamswport 0 ff10 4000 2e53) >> $filelog; sleep 1"); 
$out="";
for ($i=0;$i<24;$i++) {
    system("(echo Read Pedestals TDR $tdr...; ./pcamswport 0 ff10 $tdr[$i] 2e10 0004 1300) > $filedummy"); 
    $tmp=readtdr(); $out= $out . $tmp;
}
if ($out ne "101000001010000010100000000000000000000000000000") { print "Wrong answer switching off LRK: $out\n"; } 

system("/bin/echo -e Switch on all LRK...");
system("(echo Switch on all LRK...; ./pcamswport 0 ff10 2e1d 0000 e20b 0000) >> $filelog; sleep 1"); 
system("/bin/echo -e Switch off all TDR...");
system("(echo Switch off all TDR...; ./pcamswport 0 ff10 2e1d 0000 e20d 003f) >> $filelog; sleep 1"); 
# ping TDRs to check
system("(echo Ping TDR  0...; ./pcamswport 0 ff10 003f 2e0d) >> $filelog"); 
system("(echo Ping TDR  1...; ./pcamswport 0 ff10 013f 2e0d) >> $filelog"); 
system("(echo Ping TDR  4...; ./pcamswport 0 ff10 043f 2e0d) >> $filelog"); 
system("(echo Ping TDR  5...; ./pcamswport 0 ff10 053f 2e0d) >> $filelog"); 
system("(echo Ping TDR  8...; ./pcamswport 0 ff10 083f 2e0d) >> $filelog"); 
system("(echo Ping TDR  9...; ./pcamswport 0 ff10 093f 2e0d) >> $filelog"); 
system("/bin/echo -e Switch on all TDR...");
system("(echo Switch on all TDR...; ./pcamswport 0 ff10 2e1d 0000 e20d 0000) >> $filelog; sleep 1"); 
system("(echo Ping TDR  0...; ./pcamswport 0 ff10 003f 2e0d) >> $filelog"); 
system("(echo Ping TDR  1...; ./pcamswport 0 ff10 013f 2e0d) >> $filelog"); 
system("(echo Ping TDR  4...; ./pcamswport 0 ff10 043f 2e0d) >> $filelog"); 
system("(echo Ping TDR  5...; ./pcamswport 0 ff10 053f 2e0d) >> $filelog"); 
system("(echo Ping TDR  8...; ./pcamswport 0 ff10 083f 2e0d) >> $filelog"); 
system("(echo Ping TDR  9...; ./pcamswport 0 ff10 093f 2e0d) >> $filelog"); 

######################################################################
# check autoclear and reset
system("/bin/echo -e  Testing autoclear and reset");
system("/bin/echo -e  Testing autoclear and reset >> $filelog");
system("(echo Disable LR  autoclear...; ./pcamswport 0 ff10 2e1d 0000 a20f 0000) >> $filelog"); 
system("(echo Read Auto status...; ./pcamswport 0 ff10 2e1d 0000 a206 0000) >> $filelog"); 
system("(echo Disable TDR autoclear...; ./pcamswport 0 ff10 2e1d 0000 a218 0000) >> $filelog"); 
system("(echo Read Auto status...; ./pcamswport 0 ff10 2e1d 0000 a206 0000) >> $filelog"); 
system("(echo Reset LR  counter...; ./pcamswport 0 ff10 2e1d 0000 e20f 0200) >> $filelog"); 
system("(echo Read LRS0-3 trip cntr...; ./pcamswport 0 ff10 2e1d 0000 e210 0000) >> $filelog"); 
system("(echo Read LRS4-5 trip cntr...; ./pcamswport 0 ff10 2e1d 0000 a211 0000) >> $filelog"); 
system("(echo Read LRK0-3 trip cntr...; ./pcamswport 0 ff10 2e1d 0000 a212 0000) >> $filelog"); 
system("(echo Read LRK4-5 trip cntr...; ./pcamswport 0 ff10 2e1d 0000 e213 0000) >> $filelog"); 
system("(echo Reset TDR counter...; ./pcamswport 0 ff10 2e1d 0000 e218 0200) >> $filelog"); 
system("(echo Read TDR0-3 trip cntr...; ./pcamswport 0 ff10 2e1d 0000 e215 0000) >> $filelog"); 
system("(echo Read TDR4-5 trip cntr...; ./pcamswport 0 ff10 2e1d 0000 e216 0000) >> $filelog"); 
system("(echo Enable LR  autoclear...; ./pcamswport 0 ff10 2e1d 0000 e20f 0400) >> $filelog"); 
system("(echo Enable TDR counter...; ./pcamswport 0 ff10 2e1d 0000 e218 0400) >> $filelog"); 
system("(echo Read Auto status...; ./pcamswport 0 ff10 2e1d 0000 a206 0000) >> $filelog"); 

######################################################################
# Test of TPSFE 6 via JINF A, Half A
######################################################################
print  "Testing TPSFE 6 via JINF A, Half A\n";
system("/bin/echo -e  Testing TPSFE 6 via JINF A, Half A  >> $filelog");
# read status words
system("/bin/echo -e  Reading Status");
system("/bin/echo -e  Reading Status >> $filelog");
system("(echo Read SEL status...; ./pcamswport 0 ff10 2e1d 0000 e300 0000) >> $filelog"); 
system("(echo Read ACTEL status...; ./pcamswport 0 ff10 2e1d 0000 a301 0000) >> $filelog"); 
system("(echo Read LRS last cmd...; ./pcamswport 0 ff10 2e1d 0000 a302 0000) >> $filelog"); 
system("(echo Read LRK last cmd...; ./pcamswport 0 ff10 2e1d 0000 e303 0000) >> $filelog"); 
system("(echo Read LR fb status...; ./pcamswport 0 ff10 2e1d 0000 a304 0000) >> $filelog"); 
system("(echo Read TDR2 last cmd...; ./pcamswport 0 ff10 2e1d 0000 e305 0000) >> $filelog"); 
system("(echo Read Auto status...; ./pcamswport 0 ff10 2e1d 0000 e306 0000) >> $filelog"); 
system("(echo Read Actel trip cntr...; ./pcamswport 0 ff10 2e1d 0000 a307 0000) >> $filelog"); 
system("(echo Read LRS0-3 trip cntr...; ./pcamswport 0 ff10 2e1d 0000 a310 0000) >> $filelog"); 
system("(echo Read LRS4-5 trip cntr...; ./pcamswport 0 ff10 2e1d 0000 e311 0000) >> $filelog"); 
system("(echo Read LRK0-3 trip cntr...; ./pcamswport 0 ff10 2e1d 0000 e312 0000) >> $filelog"); 
system("(echo Read LRK4-5 trip cntr...; ./pcamswport 0 ff10 2e1d 0000 a313 0000) >> $filelog"); 
system("(echo Read TDR0-3 trip cntr...; ./pcamswport 0 ff10 2e1d 0000 a315 0000) >> $filelog"); 
system("(echo Read TDR4-5 trip cntr...; ./pcamswport 0 ff10 2e1d 0000 a316 0000) >> $filelog"); 
######################################################################
# switch on/off Actels (default is both on!)
system("/bin/echo -e  Testing Actels");
system("/bin/echo -e  Testing Actels >> $filelog");
system("(/bin/echo -e  Turn brother off via Half A...; ./pcamswport 0 ff10 2e1d 0000 a309 0001) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half B...; ./pcamswport 0 ff10 2e1d 0020 a380 0000) >> $filelog");
system("(/bin/echo -e  Turn brother on via Half A...; ./pcamswport 0 ff10 2e1d 0000 a309 0010) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half B...; ./pcamswport 0 ff10 2e1d 0020 a380 0000) >> $filelog");
system("(/bin/echo -e  Turn brother off via Half B...; ./pcamswport 0 ff10 2e1d 0020 a309 0001) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half A...; ./pcamswport 0 ff10 2e1d 0000 a380 0000) >> $filelog");
system("(/bin/echo -e  Turn brother on via Half B...; ./pcamswport 0 ff10 2e1d 0020 a309 0010) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half A...; ./pcamswport 0 ff10 2e1d 0000 a380 0000) >> $filelog");
system("(echo Read Actel trip cntr on A...; ./pcamswport 0 ff10 2e1d 0000 a307 0000) >> $filelog"); 
system("(echo Read Actel trip cntr on B...; ./pcamswport 0 ff10 2e1d 0020 a307 0000) >> $filelog"); 
system("(echo Read Actel trip again on A, autoclear...; ./pcamswport 0 ff10 2e1d 0000 a307 0000) >> $filelog"); 
system("(echo Read Actel trip again on B, autoclear...; ./pcamswport 0 ff10 2e1d 0020 a307 0000) >> $filelog"); 
# trip Actels more than once to check counters
system("/bin/echo -e  Tripping Actels");
system("/bin/echo -e  Tripping Actels >> $filelog");
for ($i=0;$i<15;$i++) {
    system("(/bin/echo -e  Turn brother off via Half A...; ./pcamswport 0 ff10 2e1d 0000 a309 0001) >> $filelog; sleep 1");
    system("(/bin/echo -e  Turn brother on via Half A...; ./pcamswport 0 ff10 2e1d 0000 a309 0010) >> $filelog; sleep 1");
}
system("(echo Read Actel trip cntr on A...; ./pcamswport 0 ff10 2e1d 0000 a307 0000) >> $filelog"); for ($i=0;$i<15;$i++) {
    system("(/bin/echo -e  Turn brother off via Half B...; ./pcamswport 0 ff10 2e1d 0020 a309 0001) >> $filelog; usleep 200000");
    system("(/bin/echo -e  Turn brother on via Half B...; ./pcamswport 0 ff10 2e1d 0020 a309 0010) >> $filelog; usleep 200000");
}
system("(echo Read Actel trip cntr on B...; ./pcamswport 0 ff10 2e1d 0020 a307 0000) >> $filelog");
# clear trip cntrs
system("(echo Read Actel trip again on A, autoclear...; ./pcamswport 0 ff10 2e1d 0000 a307 0000) >> $filelog"); 
system("(echo Read Actel trip again on B, autoclear...; ./pcamswport 0 ff10 2e1d 0020 a307 0000) >> $filelog"); 

######################################################################
# switch S, K, TDRs
system("/bin/echo -e  Testing S,K,TDRs");
system("/bin/echo -e  Testing S,K,TDRs >> $filelog");
system("/bin/echo -e Switch off all LRS...");
system("(echo Switch off all LRS...; ./pcamswport 0 ff10 2e1d 0000 e30a 003f) >> $filelog; sleep 1"); 
# to check: calibrate TDRs and read pedestals, if off should be at 4095
system("(echo Calibrate all TDR...; ./pcamswport 0 ff10 4000 2e53) >> $filelog; sleep 1"); 
$out="";
for ($i=0;$i<24;$i++) {
    system("(echo Read Pedestals TDR $tdr...; ./pcamswport 0 ff10 $tdr[$i] 2e10 0004 1300) > $filedummy"); 
    $tmp=readtdr(); $out= $out . $tmp;
}
if ($out ne "010100000101000001010000000000000000000000000000") { print "Wrong answer switching off LRS: $out\n"; } 
system("/bin/echo -e Switch on all LRS...");
system("(echo Switch on all LRS...; ./pcamswport 0 ff10 2e1d 0000 e30a 0000) >> $filelog; sleep 1"); 
system("/bin/echo -e Switch off all LRK...");
system("(echo Switch off all LRK...; ./pcamswport 0 ff10 2e1d 0000 a30b 003f) >> $filelog; sleep 1"); 
# to check: calibrate TDRs and read pedestals, if off should be at 4095
system("(echo Calibrate all TDR...; ./pcamswport 0 ff10 4000 2e53) >> $filelog; sleep 1"); 
$out="";
for ($i=0;$i<24;$i++) {
    system("(echo Read Pedestals TDR $tdr...; ./pcamswport 0 ff10 $tdr[$i] 2e10 0004 1300) > $filedummy"); 
    $tmp=readtdr(); $out= $out . $tmp;
}
if ($out ne "000000000000000000000000010100000101000001010000") { print "Wrong answer switching off LRK: $out\n"; } 

system("/bin/echo -e Switch on all LRK...");
system("(echo Switch on all LRK...; ./pcamswport 0 ff10 2e1d 0000 a30b 0000) >> $filelog; sleep 1"); 
system("/bin/echo -e Switch off all TDR...");
system("(echo Switch off all TDR...; ./pcamswport 0 ff10 2e1d 0000 a30d 003f) >> $filelog; sleep 1"); 
# ping TDRs to check
system("(echo Ping TDR  C...; ./pcamswport 0 ff10 0c3f 2e0d) >> $filelog"); 
system("(echo Ping TDR  D...; ./pcamswport 0 ff10 0d3f 2e0d) >> $filelog"); 
system("(echo Ping TDR 10...; ./pcamswport 0 ff10 103f 2e0d) >> $filelog"); 
system("(echo Ping TDR 11...; ./pcamswport 0 ff10 113f 2e0d) >> $filelog"); 
system("(echo Ping TDR 14...; ./pcamswport 0 ff10 143f 2e0d) >> $filelog"); 
system("(echo Ping TDR 15...; ./pcamswport 0 ff10 153f 2e0d) >> $filelog"); 
system("/bin/echo -e Switch on all TDR...");
system("(echo Switch on all TDR...; ./pcamswport 0 ff10 2e1d 0000 a30d 0000) >> $filelog; sleep 1"); 
system("(echo Ping TDR  C...; ./pcamswport 0 ff10 0c3f 2e0d) >> $filelog"); 
system("(echo Ping TDR  D...; ./pcamswport 0 ff10 0d3f 2e0d) >> $filelog"); 
system("(echo Ping TDR 10...; ./pcamswport 0 ff10 103f 2e0d) >> $filelog"); 
system("(echo Ping TDR 11...; ./pcamswport 0 ff10 113f 2e0d) >> $filelog"); 
system("(echo Ping TDR 14...; ./pcamswport 0 ff10 143f 2e0d) >> $filelog"); 
system("(echo Ping TDR 15...; ./pcamswport 0 ff10 153f 2e0d) >> $filelog"); 

######################################################################
# check autoclear and reset
system("/bin/echo -e  Testing autoclear and reset");
system("/bin/echo -e  Testing autoclear and reset >> $filelog");
system("(echo Disable LR  autoclear...; ./pcamswport 0 ff10 2e1d 0000 e30f 0000) >> $filelog"); 
system("(echo Read Auto status...; ./pcamswport 0 ff10 2e1d 0000 e306 0000) >> $filelog"); 
system("(echo Disable TDR autoclear...; ./pcamswport 0 ff10 2e1d 0000 e318 0000) >> $filelog"); 
system("(echo Read Auto status...; ./pcamswport 0 ff10 2e1d 0000 e306 0000) >> $filelog"); 
system("(echo Reset LR  counter...; ./pcamswport 0 ff10 2e1d 0000 a30f 0200) >> $filelog"); 
system("(echo Read LRS0-3 trip cntr...; ./pcamswport 0 ff10 2e1d 0000 a310 0000) >> $filelog"); 
system("(echo Read LRS4-5 trip cntr...; ./pcamswport 0 ff10 2e1d 0000 e311 0000) >> $filelog"); 
system("(echo Read LRK0-3 trip cntr...; ./pcamswport 0 ff10 2e1d 0000 e312 0000) >> $filelog"); 
system("(echo Read LRK4-5 trip cntr...; ./pcamswport 0 ff10 2e1d 0000 a313 0000) >> $filelog"); 
system("(echo Reset TDR counter...; ./pcamswport 0 ff10 2e1d 0000 a318 0200) >> $filelog"); 
system("(echo Read TDR0-3 trip cntr...; ./pcamswport 0 ff10 2e1d 0000 a315 0000) >> $filelog"); 
system("(echo Read TDR4-5 trip cntr...; ./pcamswport 0 ff10 2e1d 0000 a316 0000) >> $filelog"); 
system("(echo Enable LR  autoclear...; ./pcamswport 0 ff10 2e1d 0000 a30f 0400) >> $filelog"); 
system("(echo Enable TDR counter...; ./pcamswport 0 ff10 2e1d 0000 a318 0400) >> $filelog"); 
system("(echo Read Auto status...; ./pcamswport 0 ff10 2e1d 0000 e306 0000) >> $filelog"); 

######################################################################
# Test of TPSFE 14 via JINF A, Half A
######################################################################
print  "Testing TPSFE 14 via JINF A, Half A\n";
system("/bin/echo -e  Testing TPSFE 14 via JINF A, Half A  >> $filelog");
# read status words
system("/bin/echo -e  Reading Status");
system("/bin/echo -e  Reading Status >> $filelog");
system("(echo Read SEL status...; ./pcamswport 0 ff10 2e1d 0010 a700 0000) >> $filelog"); 
system("(echo Read ACTEL status...; ./pcamswport 0 ff10 2e1d 0010 e701 0000) >> $filelog"); 
system("(echo Read LRS last cmd...; ./pcamswport 0 ff10 2e1d 0010 e702 0000) >> $filelog"); 
system("(echo Read LRK last cmd...; ./pcamswport 0 ff10 2e1d 0010 a703 0000) >> $filelog"); 
system("(echo Read LR fb status...; ./pcamswport 0 ff10 2e1d 0010 e704 0000) >> $filelog"); 
system("(echo Read TDR2 last cmd...; ./pcamswport 0 ff10 2e1d 0010 a705 0000) >> $filelog"); 
system("(echo Read Auto status...; ./pcamswport 0 ff10 2e1d 0010 a706 0000) >> $filelog"); 
system("(echo Read Actel trip cntr...; ./pcamswport 0 ff10 2e1d 0010 e707 0000) >> $filelog"); 
system("(echo Read LRS0-3 trip cntr...; ./pcamswport 0 ff10 2e1d 0010 e710 0000) >> $filelog"); 
system("(echo Read LRS4-5 trip cntr...; ./pcamswport 0 ff10 2e1d 0010 a711 0000) >> $filelog"); 
system("(echo Read LRK0-3 trip cntr...; ./pcamswport 0 ff10 2e1d 0010 a712 0000) >> $filelog"); 
system("(echo Read LRK4-5 trip cntr...; ./pcamswport 0 ff10 2e1d 0010 e713 0000) >> $filelog"); 
system("(echo Read TDR0-3 trip cntr...; ./pcamswport 0 ff10 2e1d 0010 e715 0000) >> $filelog"); 
system("(echo Read TDR4-5 trip cntr...; ./pcamswport 0 ff10 2e1d 0010 e716 0000) >> $filelog"); 
######################################################################
# switch on/off Actels (default is both on!)
system("/bin/echo -e  Testing Actels");
system("/bin/echo -e  Testing Actels >> $filelog");
system("(/bin/echo -e  Turn brother off via Half A...; ./pcamswport 0 ff10 2e1d 0010 e709 0001) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half B...; ./pcamswport 0 ff10 2e1d 0020 e780 0000) >> $filelog");
system("(/bin/echo -e  Turn brother on via Half A...; ./pcamswport 0 ff10 2e1d 0010 e709 0010) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half B...; ./pcamswport 0 ff10 2e1d 0020 e780 0000) >> $filelog");
system("(/bin/echo -e  Turn brother off via Half B...; ./pcamswport 0 ff10 2e1d 0020 e709 0001) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half A...; ./pcamswport 0 ff10 2e1d 0010 e780 0000) >> $filelog");
system("(/bin/echo -e  Turn brother on via Half B...; ./pcamswport 0 ff10 2e1d 0020 e709 0010) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half A...; ./pcamswport 0 ff10 2e1d 0010 e780 0000) >> $filelog");
system("(echo Read Actel trip cntr on A...; ./pcamswport 0 ff10 2e1d 0010 e707 0000) >> $filelog"); 
system("(echo Read Actel trip cntr on B...; ./pcamswport 0 ff10 2e1d 0020 e707 0000) >> $filelog"); 
system("(echo Read Actel trip again on A, autoclear...; ./pcamswport 0 ff10 2e1d 0010 e707 0000) >> $filelog"); 
system("(echo Read Actel trip again on B, autoclear...; ./pcamswport 0 ff10 2e1d 0020 e707 0000) >> $filelog"); 
# trip Actels more than once to check counters
system("/bin/echo -e  Tripping Actels");
system("/bin/echo -e  Tripping Actels >> $filelog");
for ($i=0;$i<15;$i++) {
    system("(/bin/echo -e  Turn brother off via Half A...; ./pcamswport 0 ff10 2e1d 0010 e709 0001) >> $filelog; sleep 1");
    system("(/bin/echo -e  Turn brother on via Half A...; ./pcamswport 0 ff10 2e1d 0010 e709 0010) >> $filelog; sleep 1");
}
system("(echo Read Actel trip cntr on A...; ./pcamswport 0 ff10 2e1d 0010 e707 0000) >> $filelog"); for ($i=0;$i<15;$i++) {
    system("(/bin/echo -e  Turn brother off via Half B...; ./pcamswport 0 ff10 2e1d 0020 e709 0001) >> $filelog; usleep 200000");
    system("(/bin/echo -e  Turn brother on via Half B...; ./pcamswport 0 ff10 2e1d 0020 e709 0010) >> $filelog; usleep 200000");
}
system("(echo Read Actel trip cntr on B...; ./pcamswport 0 ff10 2e1d 0020 e707 0000) >> $filelog");
# clear trip cntrs
system("(echo Read Actel trip again on A, autoclear...; ./pcamswport 0 ff10 2e1d 0010 e707 0000) >> $filelog"); 
system("(echo Read Actel trip again on B, autoclear...; ./pcamswport 0 ff10 2e1d 0020 e707 0000) >> $filelog"); 

######################################################################
# switch S, K, TDRs
system("/bin/echo -e  Testing S,K,TDRs");
system("/bin/echo -e  Testing S,K,TDRs >> $filelog");
system("/bin/echo -e Switch off all LRS...");
system("(echo Switch off all LRS...; ./pcamswport 0 ff10 2e1d 0010 a70a 003f) >> $filelog; sleep 1"); 
# to check: calibrate TDRs and read pedestals, if off should be at 4095
system("(echo Calibrate all TDR...; ./pcamswport 0 ff10 4000 2e53) >> $filelog; sleep 1"); 
$out="";
for ($i=0;$i<24;$i++) {
    system("(echo Read Pedestals TDR $tdr...; ./pcamswport 0 ff10 $tdr[$i] 2e10 0004 1300) > $filedummy"); 
    $tmp=readtdr(); $out= $out . $tmp;
}
if ($out ne "000000000000000000000000000010100000101000001010") { print "Wrong answer switching off LRS: $out\n"; } 
system("/bin/echo -e Switch on all LRS...");
system("(echo Switch on all LRS...; ./pcamswport 0 ff10 2e1d 0010 a70a 0000) >> $filelog; sleep 1"); 
system("/bin/echo -e Switch off all LRK...");
system("(echo Switch off all LRK...; ./pcamswport 0 ff10 2e1d 0010 e70b 003f) >> $filelog; sleep 1"); 
# to check: calibrate TDRs and read pedestals, if off should be at 4095
system("(echo Calibrate all TDR...; ./pcamswport 0 ff10 4000 2e53) >> $filelog; sleep 1"); 
$out="";
for ($i=0;$i<24;$i++) {
    system("(echo Read Pedestals TDR $tdr...; ./pcamswport 0 ff10 $tdr[$i] 2e10 0004 1300) > $filedummy"); 
    $tmp=readtdr(); $out= $out . $tmp;
}
if ($out ne "000000000000000000000000000001010000010100000101") { print "Wrong answer switching off LRK: $out\n"; } 

system("/bin/echo -e Switch on all LRK...");
system("(echo Switch on all LRK...; ./pcamswport 0 ff10 2e1d 0010 e70b 0000) >> $filelog; sleep 1"); 
system("/bin/echo -e Switch off all TDR...");
system("(echo Switch off all TDR...; ./pcamswport 0 ff10 2e1d 0010 e70d 003f) >> $filelog; sleep 1"); 
# ping TDRs to check
system("(echo Ping TDR 16...; ./pcamswport 0 ff10 163f 2e0d) >> $filelog"); 
system("(echo Ping TDR 17...; ./pcamswport 0 ff10 173f 2e0d) >> $filelog"); 
system("(echo Ping TDR 12...; ./pcamswport 0 ff10 123f 2e0d) >> $filelog"); 
system("(echo Ping TDR 13...; ./pcamswport 0 ff10 133f 2e0d) >> $filelog"); 
system("(echo Ping TDR  F...; ./pcamswport 0 ff10 0f3f 2e0d) >> $filelog"); 
system("(echo Ping TDR  E...; ./pcamswport 0 ff10 0e3f 2e0d) >> $filelog"); 
system("/bin/echo -e Switch on all TDR...");
system("(echo Switch on all TDR...; ./pcamswport 0 ff10 2e1d 0010 e70d 0000) >> $filelog; sleep 1"); 
system("(echo Ping TDR 16...; ./pcamswport 0 ff10 163f 2e0d) >> $filelog"); 
system("(echo Ping TDR 17...; ./pcamswport 0 ff10 173f 2e0d) >> $filelog"); 
system("(echo Ping TDR 12...; ./pcamswport 0 ff10 123f 2e0d) >> $filelog"); 
system("(echo Ping TDR 13...; ./pcamswport 0 ff10 133f 2e0d) >> $filelog"); 
system("(echo Ping TDR  F...; ./pcamswport 0 ff10 0f3f 2e0d) >> $filelog"); 
system("(echo Ping TDR  E...; ./pcamswport 0 ff10 0e3f 2e0d) >> $filelog"); 

######################################################################
# check autoclear and reset
system("/bin/echo -e  Testing autoclear and reset");
system("/bin/echo -e  Testing autoclear and reset >> $filelog");
system("(echo Disable LR  autoclear...; ./pcamswport 0 ff10 2e1d 0010 a70f 0000) >> $filelog"); 
system("(echo Read Auto status...; ./pcamswport 0 ff10 2e1d 0010 a706 0000) >> $filelog"); 
system("(echo Disable TDR autoclear...; ./pcamswport 0 ff10 2e1d 0010 a718 0000) >> $filelog"); 
system("(echo Read Auto status...; ./pcamswport 0 ff10 2e1d 0010 a706 0000) >> $filelog"); 
system("(echo Reset LR  counter...; ./pcamswport 0 ff10 2e1d 0010 e70f 0200) >> $filelog"); 
system("(echo Read LRS0-3 trip cntr...; ./pcamswport 0 ff10 2e1d 0010 e710 0000) >> $filelog"); 
system("(echo Read LRS4-5 trip cntr...; ./pcamswport 0 ff10 2e1d 0010 a711 0000) >> $filelog"); 
system("(echo Read LRK0-3 trip cntr...; ./pcamswport 0 ff10 2e1d 0010 a712 0000) >> $filelog"); 
system("(echo Read LRK4-5 trip cntr...; ./pcamswport 0 ff10 2e1d 0010 e713 0000) >> $filelog"); 
system("(echo Reset TDR counter...; ./pcamswport 0 ff10 2e1d 0010 e718 0200) >> $filelog"); 
system("(echo Read TDR0-3 trip cntr...; ./pcamswport 0 ff10 2e1d 0010 e715 0000) >> $filelog"); 
system("(echo Read TDR4-5 trip cntr...; ./pcamswport 0 ff10 2e1d 0010 e716 0000) >> $filelog"); 
system("(echo Enable LR  autoclear...; ./pcamswport 0 ff10 2e1d 0010 e70f 0400) >> $filelog"); 
system("(echo Enable TDR counter...; ./pcamswport 0 ff10 2e1d 0010 e718 0400) >> $filelog"); 
system("(echo Read Auto status...; ./pcamswport 0 ff10 2e1d 0010 a706 0000) >> $filelog"); 

######################################################################
# Test of TPSFE 16 via JINF A, Half A
######################################################################
print  "Testing TPSFE 16 via JINF A, Half A\n";
system("/bin/echo -e  Testing TPSFE 16 via JINF A, Half A  >> $filelog");
# read status words
system("/bin/echo -e  Reading Status");
system("/bin/echo -e  Reading Status >> $filelog");
system("(echo Read SEL status...; ./pcamswport 0 ff10 2e1d 0010 a800 0000) >> $filelog"); 
system("(echo Read ACTEL status...; ./pcamswport 0 ff10 2e1d 0010 e801 0000) >> $filelog"); 
system("(echo Read LRS last cmd...; ./pcamswport 0 ff10 2e1d 0010 e802 0000) >> $filelog"); 
system("(echo Read LRK last cmd...; ./pcamswport 0 ff10 2e1d 0010 a803 0000) >> $filelog"); 
system("(echo Read LR fb status...; ./pcamswport 0 ff10 2e1d 0010 e804 0000) >> $filelog"); 
system("(echo Read TDR2 last cmd...; ./pcamswport 0 ff10 2e1d 0010 a805 0000) >> $filelog"); 
system("(echo Read Auto status...; ./pcamswport 0 ff10 2e1d 0010 a806 0000) >> $filelog"); 
system("(echo Read Actel trip cntr...; ./pcamswport 0 ff10 2e1d 0010 e807 0000) >> $filelog"); 
system("(echo Read LRS0-3 trip cntr...; ./pcamswport 0 ff10 2e1d 0010 e810 0000) >> $filelog"); 
system("(echo Read LRS4-5 trip cntr...; ./pcamswport 0 ff10 2e1d 0010 a811 0000) >> $filelog"); 
system("(echo Read LRK0-3 trip cntr...; ./pcamswport 0 ff10 2e1d 0010 a812 0000) >> $filelog"); 
system("(echo Read LRK4-5 trip cntr...; ./pcamswport 0 ff10 2e1d 0010 e813 0000) >> $filelog"); 
system("(echo Read TDR0-3 trip cntr...; ./pcamswport 0 ff10 2e1d 0010 e815 0000) >> $filelog"); 
system("(echo Read TDR4-5 trip cntr...; ./pcamswport 0 ff10 2e1d 0010 e816 0000) >> $filelog"); 
######################################################################
# switch on/off Actels (default is both on!)
system("/bin/echo -e  Testing Actels");
system("/bin/echo -e  Testing Actels >> $filelog");
system("(/bin/echo -e  Turn brother off via Half A...; ./pcamswport 0 ff10 2e1d 0010 e809 0001) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half B...; ./pcamswport 0 ff10 2e1d 0020 e880 0000) >> $filelog");
system("(/bin/echo -e  Turn brother on via Half A...; ./pcamswport 0 ff10 2e1d 0010 e809 0010) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half B...; ./pcamswport 0 ff10 2e1d 0020 e880 0000) >> $filelog");
system("(/bin/echo -e  Turn brother off via Half B...; ./pcamswport 0 ff10 2e1d 0020 e809 0001) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half A...; ./pcamswport 0 ff10 2e1d 0010 e880 0000) >> $filelog");
system("(/bin/echo -e  Turn brother on via Half B...; ./pcamswport 0 ff10 2e1d 0020 e809 0010) >> $filelog; sleep 1");
system("(/bin/echo -e  ...and verify status on Half A...; ./pcamswport 0 ff10 2e1d 0010 e880 0000) >> $filelog");
system("(echo Read Actel trip cntr on A...; ./pcamswport 0 ff10 2e1d 0010 e807 0000) >> $filelog"); 
system("(echo Read Actel trip cntr on B...; ./pcamswport 0 ff10 2e1d 0020 e807 0000) >> $filelog"); 
system("(echo Read Actel trip again on A, autoclear...; ./pcamswport 0 ff10 2e1d 0010 e807 0000) >> $filelog"); 
system("(echo Read Actel trip again on B, autoclear...; ./pcamswport 0 ff10 2e1d 0020 e807 0000) >> $filelog"); 
# trip Actels more than once to check counters
system("/bin/echo -e  Tripping Actels");
system("/bin/echo -e  Tripping Actels >> $filelog");
for ($i=0;$i<15;$i++) {
    system("(/bin/echo -e  Turn brother off via Half A...; ./pcamswport 0 ff10 2e1d 0010 e809 0001) >> $filelog; sleep 1");
    system("(/bin/echo -e  Turn brother on via Half A...; ./pcamswport 0 ff10 2e1d 0010 e809 0010) >> $filelog; sleep 1");
}
system("(echo Read Actel trip cntr on A...; ./pcamswport 0 ff10 2e1d 0010 e807 0000) >> $filelog"); for ($i=0;$i<15;$i++) {
    system("(/bin/echo -e  Turn brother off via Half B...; ./pcamswport 0 ff10 2e1d 0020 e809 0001) >> $filelog; usleep 200000");
    system("(/bin/echo -e  Turn brother on via Half B...; ./pcamswport 0 ff10 2e1d 0020 e809 0010) >> $filelog; usleep 200000");
}
system("(echo Read Actel trip cntr on B...; ./pcamswport 0 ff10 2e1d 0020 e807 0000) >> $filelog");
# clear trip cntrs
system("(echo Read Actel trip again on A, autoclear...; ./pcamswport 0 ff10 2e1d 0010 e807 0000) >> $filelog"); 
system("(echo Read Actel trip again on B, autoclear...; ./pcamswport 0 ff10 2e1d 0020 e807 0000) >> $filelog"); 

######################################################################
# switch S, K, TDRs
system("/bin/echo -e  Testing S,K,TDRs");
system("/bin/echo -e  Testing S,K,TDRs >> $filelog");
system("/bin/echo -e Switch off all LRS...");
system("(echo Switch off all LRS...; ./pcamswport 0 ff10 2e1d 0010 a80a 003f) >> $filelog; sleep 1"); 
# to check: calibrate TDRs and read pedestals, if off should be at 4095
system("(echo Calibrate all TDR...; ./pcamswport 0 ff10 4000 2e53) >> $filelog; sleep 1"); 
$out="";
for ($i=0;$i<24;$i++) {
    system("(echo Read Pedestals TDR $tdr...; ./pcamswport 0 ff10 $tdr[$i] 2e10 0004 1300) > $filedummy"); 
    $tmp=readtdr(); $out= $out . $tmp;
}
if ($out ne "000010100000101000001010000000000000000000000000") { print "Wrong answer switching off LRS: $out\n"; } 
system("/bin/echo -e Switch on all LRS...");
system("(echo Switch on all LRS...; ./pcamswport 0 ff10 2e1d 0010 a80a 0000) >> $filelog; sleep 1"); 
system("/bin/echo -e Switch off all LRK...");
system("(echo Switch off all LRK...; ./pcamswport 0 ff10 2e1d 0010 e80b 003f) >> $filelog; sleep 1"); 
# to check: calibrate TDRs and read pedestals, if off should be at 4095
system("(echo Calibrate all TDR...; ./pcamswport 0 ff10 4000 2e53) >> $filelog; sleep 1"); 
$out="";
for ($i=0;$i<24;$i++) {
    system("(echo Read Pedestals TDR $tdr...; ./pcamswport 0 ff10 $tdr[$i] 2e10 0004 1300) > $filedummy"); 
    $tmp=readtdr(); $out= $out . $tmp;
}
if ($out ne "000001010000010100000101000000000000000000000000") { print "Wrong answer switching off LRK: $out\n"; } 

system("/bin/echo -e Switch on all LRK...");
system("(echo Switch on all LRK...; ./pcamswport 0 ff10 2e1d 0010 e80b 0000) >> $filelog; sleep 1"); 
system("/bin/echo -e Switch off all TDR...");
system("(echo Switch off all TDR...; ./pcamswport 0 ff10 2e1d 0010 e80d 003f) >> $filelog; sleep 1"); 
# ping TDRs to check
system("(echo Ping TDR  A...; ./pcamswport 0 ff10 0a3f 2e0d) >> $filelog"); 
system("(echo Ping TDR  B...; ./pcamswport 0 ff10 0b3f 2e0d) >> $filelog"); 
system("(echo Ping TDR  6...; ./pcamswport 0 ff10 063f 2e0d) >> $filelog"); 
system("(echo Ping TDR  7...; ./pcamswport 0 ff10 073f 2e0d) >> $filelog"); 
system("(echo Ping TDR  2...; ./pcamswport 0 ff10 023f 2e0d) >> $filelog"); 
system("(echo Ping TDR  3...; ./pcamswport 0 ff10 033f 2e0d) >> $filelog"); 
system("/bin/echo -e Switch on all TDR...");
system("(echo Switch on all TDR...; ./pcamswport 0 ff10 2e1d 0010 e80d 0000) >> $filelog; sleep 1"); 
system("(echo Ping TDR  A...; ./pcamswport 0 ff10 0a3f 2e0d) >> $filelog"); 
system("(echo Ping TDR  B...; ./pcamswport 0 ff10 0b3f 2e0d) >> $filelog"); 
system("(echo Ping TDR  6...; ./pcamswport 0 ff10 063f 2e0d) >> $filelog"); 
system("(echo Ping TDR  7...; ./pcamswport 0 ff10 073f 2e0d) >> $filelog"); 
system("(echo Ping TDR  2...; ./pcamswport 0 ff10 023f 2e0d) >> $filelog"); 
system("(echo Ping TDR  3...; ./pcamswport 0 ff10 033f 2e0d) >> $filelog"); 

######################################################################
# check autoclear and reset
system("/bin/echo -e  Testing autoclear and reset");
system("/bin/echo -e  Testing autoclear and reset >> $filelog");
system("(echo Disable LR  autoclear...; ./pcamswport 0 ff10 2e1d 0010 a80f 0000) >> $filelog"); 
system("(echo Read Auto status...; ./pcamswport 0 ff10 2e1d 0010 a806 0000) >> $filelog"); 
system("(echo Disable TDR autoclear...; ./pcamswport 0 ff10 2e1d 0010 a818 0000) >> $filelog"); 
system("(echo Read Auto status...; ./pcamswport 0 ff10 2e1d 0010 a806 0000) >> $filelog"); 
system("(echo Reset LR  counter...; ./pcamswport 0 ff10 2e1d 0010 e80f 0200) >> $filelog"); 
system("(echo Read LRS0-3 trip cntr...; ./pcamswport 0 ff10 2e1d 0010 e810 0000) >> $filelog"); 
system("(echo Read LRS4-5 trip cntr...; ./pcamswport 0 ff10 2e1d 0010 a811 0000) >> $filelog"); 
system("(echo Read LRK0-3 trip cntr...; ./pcamswport 0 ff10 2e1d 0010 a812 0000) >> $filelog"); 
system("(echo Read LRK4-5 trip cntr...; ./pcamswport 0 ff10 2e1d 0010 e813 0000) >> $filelog"); 
system("(echo Reset TDR counter...; ./pcamswport 0 ff10 2e1d 0010 e818 0200) >> $filelog"); 
system("(echo Read TDR0-3 trip cntr...; ./pcamswport 0 ff10 2e1d 0010 e815 0000) >> $filelog"); 
system("(echo Read TDR4-5 trip cntr...; ./pcamswport 0 ff10 2e1d 0010 e816 0000) >> $filelog"); 
system("(echo Enable LR  autoclear...; ./pcamswport 0 ff10 2e1d 0010 e80f 0400) >> $filelog"); 
system("(echo Enable TDR counter...; ./pcamswport 0 ff10 2e1d 0010 e818 0400) >> $filelog"); 
system("(echo Read Auto status...; ./pcamswport 0 ff10 2e1d 0010 a806 0000) >> $filelog"); 

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
sub readtdr() {
    open(FILE1,"$filedummy") or die "cannot open file $filedummy\n";
    $s="0"; $k="0";
    while (<FILE1>) {
	chop; # throw away newline
	if (/^\d/)  { # search for pedestals
	    my @foo = split;
	    $tdrs=$foo[0]; 
	    $tdrk=$foo[3]; 
	    # print "TDRS: $tdrs TDRK: $tdrk\n";
	}
    }
    if ($tdrs eq "7ff8") { $s="1"; }
    if ($tdrk eq "7ff8") { $k="1"; }
    close FILE1;
    $onoff= $s . $k;
    return $onoff;

}
