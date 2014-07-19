#!/usr/local/bin/perl
use Tk;
use Tk::Event;
use Time::gmtime;
use Time::localtime;
use IO::Handle;
use Tk::ProgressBar;
use Time::Local;
# use Term::ReadKey;
use File::Copy;
#use Audio::Beep;
#use Audio::Data;
#use Audio::Play;
$mwnd=MainWindow->new();
#---> imp.glob.const, vars and arrays:
$prog2run="TofAccPedDBWriter";
$outlogf="out.log";
$binw_tev=120;#binwidth(sec) for time-evolution graphs(2 min)
$RunNum=0;
@barsppl=(8,8,10,8);
$antsec=8;
$last_pedscanned_file_num=0;
$plot1_actf=0;#plot1 "active" flag
$plot2_actf=0;#plot2 "active" flag
$plot3_actf=0;#plot3 "active" flag
@plot_pntids=();# current plot points ids list
$plot_pntmem=0;# members in pntids
@plot_accids=();# ............ accessoiries ids (labels,text, grids,...) 
@plot_fileids=();#current plot point-assosiated files ids
$plot_accmem=0;# members in accids
$plot1_xcmax=0;#max x-coo when leaving plot1
$plot2_xcmax=0;#max x-coo when leaving plot2
$plot3_xcmax=0;#max x-coo when leaving plot3
$htextagi1=="";#last used help_text look_at tag_index_1
$htextagi2=="";#last used help_text look_at tag_index_2
#----------- define mw-size :
open(DISPLS,"xrandr -q |") or die "Couldn't xrandr $!\n";
@dsets=();
while(defined ($line=<DISPLS>)){
  chomp $line;
  if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
  $line =~s/^\s+//s;# remove " "'s at the beg.of line
  if($line =~/^\*/){last;}# break "*" at the beg.of line
}
@dsets=split(/\s+/,$line);#split by any number of continious " "'s
close(DISPLS);
#
$displx=$dsets[1];
$disply=$dsets[3];
$mnwdszx=0.9;#X-size of main window (portion of display x-size)
$mnwdszy=0.6;#Y-size of main window (portion of display y-size)
$mwposx="+0";
$mwposy="+0";
my $szx=int($mnwdszx*$displx);#in pixels
my $szy=int($mnwdszy*$disply);
$mwndgeom=$szx."x".$szy.$mwposx.$mwposy;
print "Mwindow geom=",$mwndgeom,"\n";
$mwnd->geometry($mwndgeom);
$mwnd->title("Tof/Anti-Peds scaner (DB-writer)");
$mwnd->bell;
#--------------
# fonrs:
$font1="Helvetica 18 normal italic";#font for entry-widgets
$font2="times 14 bold";
$font3="Helvetica 12 bold italic";#smaller font for entry-widgets
$font4="Helvetica 12 bold italic";# Attention
$font5="Helvetica 10 bold";#logfile window
$font6="Arial 12 bold";#help window
#--------------
#frame_geom:
my $shf1=0.07;#down shift for dir-widgets
my $drh1=0.155;#height of dir-dir-widgets
#---
$dir_fram=$mwnd->Frame(-label => "Working Directories :",-background => "Grey",
                                                      -relief=>'groove', -borderwidth=>5)->place(
                                                      -relwidth=>0.3, -relheight=>0.4,
                                                      -relx=>0, -rely=>0);
#---
$scd_lab=$dir_fram->Label(-text=>"ScanD:",-font=>$font2)
                                                ->place(-relwidth=>0.2, -relheight=>$drh1,
                                                             -relx=>0, -rely=>$shf1);
$scandir="/f2users/tofaccuser/pedcalib/pedfiles";
$scnd_ent=$dir_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$font1,
                                              -textvariable=>\$scandir)->place(
                                              -relwidth=>0.8, -relheight=>$drh1,  
                                              -relx=>0.2, -rely=>$shf1);
#---
$arcd_lab=$dir_fram->Label(-text=>"ArchD:",-font=>$font2)
                                                    ->place(-relwidth=>0.2, -relheight=>$drh1,
                                                             -relx=>0, -rely=>($shf1+$drh1));
$archdir="/archive";
$arcd_ent=$dir_fram->Entry(-relief=>'sunken', -background=>"white",-background=>yellow,
                                                                   -font=>$font1,
                                                                   -textvariable=>\$archdir)->place(
                                                                   -relwidth=>0.8, -relheight=>$drh1,  
                                                                   -relx=>0.2, -rely=>($shf1+$drh1));
#---
$amsd_lab=$dir_fram->Label(-text=>"TopD:",-font=>$font2)->place(-relwidth=>0.2, -relheight=>$drh1,
                                                             -relx=>0, -rely=>($shf1+2*$drh1));
$calibdir="/f2users/tofaccuser/pedcalib";
$amsd_ent=$dir_fram->Entry(-relief=>'sunken', -background=>"white",-background=>yellow,
                                                                   -font=>$font1,
                                                                   -textvariable=>\$calibdir)->place(
                                                                   -relwidth=>0.8, -relheight=>$drh1,  
                                                                   -relx=>0.2, -rely=>($shf1+2*$drh1));
#---
$amsg_lab=$dir_fram->Label(-text=>"AMS:",-font=>$font2)->place(-relwidth=>0.2, -relheight=>$drh1,
                                                             -relx=>0, -rely=>($shf1+3*$drh1));
$AMSDD=$ENV{AMSDataDir};# current AMSDataDir path 
$amsg_ent=$dir_fram->Entry(-relief=>'sunken', -background=>"white",-background=>yellow,
                                                                   -font=>$font1,
                                                                   -textvariable=>\$AMSDD)->place(
                                                                   -relwidth=>0.8, -relheight=>$drh1,  
                                                                   -relx=>0.2, -rely=>($shf1+3*$drh1));
#---
$file_lab=$dir_fram->Label(-text=>"File:",-font=>$font2)->place(-relwidth=>0.2, -relheight=>$drh1,
                                                             -relx=>0, -rely=>($shf1+4*$drh1));
$files="All";#to process particular file (default->ALL) 
$amsg_ent=$dir_fram->Entry(-relief=>'sunken', -background=>"white",-background=>yellow,
                                                                   -font=>$font1,
                                                                   -textvariable=>\$files)->place(
                                                                   -relwidth=>0.8, -relheight=>$drh1,  
                                                                   -relx=>0.2, -rely=>($shf1+4*$drh1));
#---
$dir_fram->Button(-text=>"Help", -font=>$font2, 
                                 -activebackground=>"yellow",
			         -activeforeground=>"red",
			         -foreground=>"red",
			         -background=>"green",
                                 -borderwidth=>3,-relief=>'raised',
			         -cursor=>hand2,
                                 -command => \&open_help_window)
			         ->place(
                                         -relwidth=>1, -relheight=>$drh1,  
                                         -relx=>0, -rely=>($shf1+5*$drh1));
#-------------
#frame_set:
my $shf2=0.07;#down shift for runcond-widgets
my $drh2=0.12;#height of runcond-widgets
#---
$set_fram=$mwnd->Frame(-label=>"Job Conditions :", -background => "Grey",
                                                      -relief=>'groove', -borderwidth=>5)->place(
                                                      -relwidth=>0.3, -relheight=>0.45,
                                                      -relx=>0, -rely=>0.4);
#---
$tofantsel="useTOF";
$tof_rbt=$set_fram->Radiobutton(-text=>"TofScan",-font=>$font2, -indicator=>0,
                                                      -borderwidth=>5,-relief=>'raised',
						      -selectcolor=>red,-activeforeground=>red,
						      -activebackground=>yellow, 
                                                      -background=>green,
                                                      -disabledforeground=>blue,
                                                      -value=>"useTOF", -variable=>\$tofantsel)
                                              ->place(
						      -relwidth=>0.5, -relheight=>$drh2,
						      -relx=>0, -rely=>$shf2);
#---
$ant_rbt=$set_fram->Radiobutton(-text=>"AccScan",-font=>$font2, -indicator=>0, 
                                                       -borderwidth=>5,-relief=>'raised',
						       -selectcolor=>red,-activeforeground=>red, 
						       -activebackground=>yellow, 
                                                       -background=>green, 
                                                       -disabledforeground=>blue,
                                                       -value=>"useANT", -variable=>\$tofantsel)
                                               ->place(
						       -relwidth=>0.5, -relheight=>$drh2,
						        -relx=>0.5, -rely=>$shf2);
#------
$set_fram->Label(-text=>"Files Dates (YY MM DD hh:mm) :",-font=>$font2)->place(-relwidth=>1, -relheight=>$drh2,
                                                             -relx=>0, -rely=>($shf2+$drh2));
#
$dat1_lab=$set_fram->Label(-text=>"from",-font=>$font2)->place(-relwidth=>0.16, -relheight=>$drh2,
                                                             -relx=>0, -rely=>($shf2+2*$drh2));
$fdat1="04 01 01 00:01";#def. file-date-from 
$fdat1_ent=$set_fram->Entry(-relief=>'sunken', -background=>"white",-background=>yellow,
                                                                   -font=>$font3,
                                                                   -textvariable=>\$fdat1)->place(
                                                                   -relwidth=>0.38, -relheight=>$drh2,  
                                                                   -relx=>0.16, -rely=>($shf2+2*$drh2));
#
$dat2_lab=$set_fram->Label(-text=>"to",-font=>$font2)->place(-relwidth=>0.06, -relheight=>$drh2,
                                                             -relx=>0.54, -rely=>($shf2+2*$drh2));
$fdat2="20 01 01 00:00";#def. file-date-till 
$fdat2_ent=$set_fram->Entry(-relief=>'sunken', -background=>"white",-background=>yellow,
                                                                   -font=>$font3,
                                                                   -textvariable=>\$fdat2)->place(
                                                                   -relwidth=>0.38, -relheight=>$drh2,  
                                                                   -relx=>0.6, -rely=>($shf2+2*$drh2));
#------
$boardpeds=1;#on
$offlpeds=0;#off
$tof_cbt=$set_fram->Checkbutton(-text=>"OnBoardPeds", -font=>$font2, -indicator=>0,
                                                 -borderwidth=>5,-relief=>'raised',
						 -selectcolor=>red,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
                                                 -variable=>\$boardpeds)
					 ->place(
                                                 -relwidth=>0.5, -relheight=>$drh2,
						 -relx=>0, -rely=>($shf2+3*$drh2));
#---
$ant_cbt=$set_fram->Checkbutton(-text=>"OfflinePeds", -font=>$font2, -indicator=>0,
                                                 -borderwidth=>5,-relief=>'raised',
                                                 -selectcolor=>red,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green, 
                                                 -variable=>\$offlpeds)
					 ->place(
                                                 -relwidth=>0.5, -relheight=>$drh2,
						 -relx=>0.5, -rely=>($shf2+3*$drh2));
#---
$debug=0;#off
$deb_cbt=$set_fram->Checkbutton(-text=>"DebugPrint", -font=>$font2, -indicator=>0,
                                                 -borderwidth=>5,-relief=>'raised',
						 -selectcolor=>red,-activeforeground=>red,
						 -activebackground=>yellow, 
                                                 -background=>green,
                                                 -variable=>\$debug)
					 ->place(
                                                 -relwidth=>0.5, -relheight=>$drh2,
						 -relx=>0, -rely=>($shf2+4*$drh2));
#----
$archval=0;#0/1->not/archive processed ped-files
$archtxt="ArchiveOnExit"; 
$archbt=$set_fram->Checkbutton(-text=>$archtxt,  -font=>$font2, -indicator=>0, 
                                          -borderwidth=>5,-relief=>'raised',
                                          -selectcolor=>red,-activeforeground=>red,
				          -activebackground=>yellow, 
                                          -background=>green,
                                          -variable=>\$archval)
			          ->place(
				          -relwidth=>0.5,-relheight=>$drh2,      
                                          -relx=>0.5,-rely=>($shf2+4*$drh2));
$archbt->bind("<Button-3>", \&archbt_help);
#---
$dbwrsel="InsDB";
$dbre_rbt=$set_fram->Radiobutton(-text=>"DBrewriteMode",-font=>$font2, -indicator=>0,
                                                      -borderwidth=>5,-relief=>'raised',
						      -selectcolor=>red,-activeforeground=>red,
						      -activebackground=>yellow, 
                                                      -background=>green,
                                                      -disabledforeground=>blue,
                                                      -value=>"RewrDB", -variable=>\$dbwrsel)
                                              ->place(
						      -relwidth=>0.5, -relheight=>$drh2,
						      -relx=>0.5, -rely=>($shf2+5*$drh2));
#---
$dbin_rbt=$set_fram->Radiobutton(-text=>"DBinsertMode",-font=>$font2, -indicator=>0, 
                                                       -borderwidth=>5,-relief=>'raised',
						       -selectcolor=>red,-activeforeground=>red, 
						       -activebackground=>yellow, 
                                                       -background=>green, 
                                                       -disabledforeground=>blue,
                                                       -value=>"InsDB", -variable=>\$dbwrsel)
                                               ->place(
						       -relwidth=>0.5, -relheight=>$drh2,
						        -relx=>0, -rely=>($shf2+5*$drh2));
#---
$soundtext="Sound-ON";
$set_fram->Button(-text=>"Sound-ON", -font=>$font2, 
                              -activebackground=>"yellow",
			      -activeforeground=>"red",
			      -foreground=>"red",
			      -background=>"green",
                              -borderwidth=>5,-relief=>'raised',
			      -cursor=>hand2,
			      -textvariable=>\$soundtext,
                              -command => sub{if($soundtext eq "Sound-ON"){$soundtext="Sound-OFF";}
			                      else {$soundtext="Sound-ON";}})
			         ->place(
                                                 -relwidth=>0.5, -relheight=>$drh2,
						 -relx=>0, -rely=>($shf2+6*$drh2));
#---
$dryrunval=1;#0/1->real/dry DBupdate run
$dryruntext="UpdDB_DryRun"; 
$dryrbt=$set_fram->Checkbutton(-text=>$dryruntext,  -font=>$font2, -indicator=>0, 
                                          -borderwidth=>5,-relief=>'raised',
                                          -selectcolor=>red,-activeforeground=>red,
				          -activebackground=>yellow, 
                                          -background=>green,
                                          -variable=>\$dryrunval)
			          ->place(
				          -relwidth=>0.5,-relheight=>$drh2,      
                                          -relx=>0.5,-rely=>($shf2+6*$drh2));
$dryrbt->bind("<Button-3>", \&dryrbt_help);
#---
# $bellb=$set_fram->Button(-text => 'bell', -command =>[\&mybeep, 3, 3])
#			          ->place(
#				          -relwidth=>0.5,-relheight=>$drh2,      
#                                          -relx=>0.5,-rely=>($shf2+6*$drh2));
#--------------
#frame_progr:
$prg_fram=$mwnd->Frame(-label=>"DB-update progress :", -relief=>'groove', -borderwidth=>5)->place(
                                                      -relwidth=>0.3, -relheight=>0.1,
                                                      -relx=>0, -rely=>0.85);
$perc_done=0.;
$prg_but=$prg_fram->ProgressBar( -width=>10, -from=>0, -to=>100, -blocks=>100,
                                 -colors=>[0,'green'], -gap=> 0,
                                -variable=>\$perc_done)->place(-relwidth=>0.99, -relheight=>0.55,
                                                             -relx=>0, -rely=>0.4);
#--------------
$exitbt=$mwnd->Button(-text => "Exit", -font=>$font2,
                               -activebackground=>"yellow",
			       -activeforeground=>"red",
			       -background=>"green",
			       -cursor=>hand2,
                               -command => \&exit_actions)->place(
                                         -relx=>0.925,-rely=>0.95,
                                         -relwidth=>0.075,-relheight=>0.05);
$exitbt->bind("<ButtonRelease-3>", \&exitbt_help);
#---
$updbtext="UpdDB";
$upddbbt=$mwnd->Button(-text=>"UpdDB", -font=>$font2, 
                              -activebackground=>"yellow",
			      -activeforeground=>"red",
			      -background=>"green",
			      -cursor=>hand2,
			      -textvariable=>\$updbtext,
                              -command => \&upd_db)
			         ->place(
                                     -relx=>0.85,-rely=>0.95,
                                     -relwidth=>0.075,-relheight=>0.05);
$upddbbt->bind("<Button-3>", \&upddbbt_help);
#---
$scanbt=$mwnd->Button(-text => "Scan", -font=>$font2, 
                              -activebackground=>"yellow",
			      -activeforeground=>"red",
			      -background=>"green",
			      -cursor=>hand2,
                              -command => \&scand)->place(
                                         -relx=>0,-rely=>0.95,
                                         -relwidth=>0.075,-relheight=>0.05);
$scanbt->bind("<Button-3>", \&scanbt_help);
#---
$checkbt=$mwnd->Button(-text => "Check", -font=>$font2, 
                              -activebackground=>"yellow",
			      -activeforeground=>"red",
			      -background=>"green",
			      -cursor=>hand2,
                              -command => \&checkq)->place(
                                         -relx=>0.075,-rely=>0.95,
                                         -relwidth=>0.075,-relheight=>0.05);
$checkbt->bind("<Button-3>", \&checkbt_help);
#---
$mwnd->Label(-textvariable=>\$message,-background=>"yellow",-relief=>'ridge')->place( 
                                         -relx=>0.15,-rely=>0.95,
                                         -relwidth=>0.7,-relheight=>0.05);
#
#------- create log-file screen:
#
$log_fram=$mwnd->Frame(-label=>"LogFileFrame", -relief=>'groove', -borderwidth=>5)->place(
                                                      -relwidth=>0.7, -relheight=>0.95,
                                                      -relx=>0.3, -rely=>0);
$logtext=$log_fram->Scrolled("Text",-width=>80, -height=>200,
                                                    -font=>$font5,
#						    -foreground=>blue,
                                                    -background=>LightBlue)
                                                         ->pack(-fill=>'both', -expand=>1);
$logtext->tagConfigure('Attention',-foreground=> red, -font=>$font4);
#
#if(! Exists($logscreen)){
#  $logscreen=$mwnd->Toplevel();
#  $logscreen->geometry("600x900-0-0");
#  $logscreen->title("Log-file content");
#  $logtext=$logscreen->Scrolled("Text")->pack(-fill=>'both', -expand=>1);
#}
#else{
#  $logscreen->deiconify();
#  $logscreen->raise();
#}
#-------  
MainLoop;
#------------------------------------------------------
sub show_warn {# 1-line message + 3*beep
  my $text=$_[0];
  $message=$text;
  $logtext->insert('end',$text."\n",'Attention');
  $logtext->yview('end');
  $count=3;
  if($soundtext eq "Sound-ON"){
    while($count-- >0){
      $mwnd->bell;
      $mwnd->after(150);
    }
  }
}
#--------------
sub show_messg {# 1-line message + 1*beep
  my $text=$_[0];
  $message=$text;
  $logtext->insert('end',$text."\n");
  $logtext->yview('end');
  if($soundtext eq "Sound-ON"){$mwnd->bell;}
}
#--------------
sub play_swav {
  my $file;
  if ($_[0]){ $file = $_[0]; }
  else{ $file = "explose.wav"; }
  die "$file is not a file\n" if (! -f $file);

  open(BEEP,"/dev/beep") || die "Can't open /dev/beep\n";

  fork && exit;
  while (1)
  {
	if (read(BEEP,$a,1))
	{
		fork &&	exec ("/usr/bin/esdplay", "$file");
	}
  }

  close BEEP;
}
#-------------
sub mybeep { 
  my ($button,$count,$var) = @_;
  print $button,$count,$var,"\n";
  if($soundtext eq "Sound-ON"){ 
    while ($count-- > 0) { 	
      $mwnd->bell;                # ring the bell
      $mwnd->after(150);
    }
  }
  $curline="   <-- Beep is clicked !! \n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
}
#---------------------------------------------------------
sub open_help_window
{
  my $helpfile="TofAccPedDBWriterHelp.txt";
  my $hwdszx=0.4;#X-size of genhelp window (portion of display x-size)
  my $hwdszy=0.45;#Y-size of genhelp window (portion of display y-size)
  my $szx=int($hwdszx*$displx);#in pixels
  my $szy=int($hwdszy*$disply);
  my $hwdpx=int(0.01*$disply);# 2nd wind. pos-x in pixels(0.01->safety)
  my $hwdposx="-".$hwdpx;
#  my $hwdpy=int(0.11*$disply);# 2nd wind. pos-y in pixels(0.11->safety)
  my $hwdpy=int(0.48*$disply);# 2nd wind. pos-y in pixels(0.11->safety)
  my $hwdpozy="-".$hwdpy;
  my $hwdgeom=$szx."x".$szy.$hwdposx.$hwdpozy;
  print "HELP-window geom: ",$hwdgeom,"\n";
#
  if($soundtext eq "Sound-ON"){$mwnd->bell;}
  $helpwindow=$mwnd->Toplevel();
  $helpwindow->geometry($hwdgeom);
  $helpwindow->title("Help Window");
#  
  $helptext=$helpwindow->Scrolled("Text", -font=>$font6
#                          ,-width=>40
                           , -height=>200
			                 )->pack(-fill=>'both', -expand=>1);
  open(HFN,"< $helpfile") or die show_warn("   <-- Cannot open $helpfile for reading !");
  while(<HFN>){
    $helptext->insert('end',$_);
    $helptext->yview('end');
  }
  close(HFN);
  $helptext->see("1.0");
  $helptext->tagConfigure('lookat',-foreground=> red);
}
#-------------
sub scanbt_help{
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
#    $helpwindow->deiconify();
    $helpwindow->after(500);
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "Scan-Button:",'end');
  if($indx eq ""){
    show_warn("   <-- No info yet !");
  }
  else {
    $helptext->see("$indx",);
    $helptext->tagAdd('lookat',"$indx - 1 chars","$indx + 1 lines");
    $htextagi1="$indx - 1 chars";
    $htextagi2="$indx + 1 lines";
  }
}
#-------------
sub dryrbt_help{
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
#    $helpwindow->deiconify();
    $helpwindow->after(500);
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "UpdDB_DryRun-Button:",'end');
  if($indx eq ""){
    show_warn("   <-- No info yet !");
  }
  else {
    $helptext->see("$indx");
    $helptext->tagAdd('lookat',"$indx - 1 chars","$indx + 1 lines");
    $htextagi1="$indx - 1 chars";
    $htextagi2="$indx + 1 lines";
  }
}
#-------------
sub checkbt_help{
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
#    $helpwindow->deiconify();
    $helpwindow->after(500);
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "Check-Button:",'end');
  if($indx eq ""){
    show_warn("   <-- No info yet !");
  }
  else {
    $helptext->see("$indx");
    $helptext->tagAdd('lookat',"$indx - 1 chars","$indx + 1 lines");
    $htextagi1="$indx - 1 chars";
    $htextagi2="$indx + 1 lines";
  }
}
#-------------
sub upddbbt_help{
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
#    $helpwindow->deiconify();
    $helpwindow->after(500);
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "UpdDB-Button:",'end');
  if($indx eq ""){
    show_warn("   <-- No info yet !");
  }
  else {
    $helptext->see("$indx");
    $helptext->tagAdd('lookat',"$indx - 1 chars","$indx + 1 lines");
    $htextagi1="$indx - 1 chars";
    $htextagi2="$indx + 1 lines";
  }
}
#-------------
sub archbt_help{
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
#    $helpwindow->deiconify();
    $helpwindow->after(500);
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "ArchiveOnExit-Button:",'end');
  if($indx eq ""){
    show_warn("   <-- No info yet !");
  }
  else {
    $helptext->see("$indx");
    $helptext->tagAdd('lookat',"$indx - 1 chars","$indx + 1 lines");
    $htextagi1="$indx - 1 chars";
    $htextagi2="$indx + 1 lines";
  }
}
#-------------
sub exitbt_help{
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->after(500);
    $helpwindow->deiconify();
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "Exit-Button:",'end');
  if($indx eq ""){
    show_warn("   <-- No info yet !");
  }
  else {
    $helptext->see("$indx");
    $helptext->tagAdd('lookat',"$indx - 1 chars","$indx + 1 lines");
    $htextagi1="$indx - 1 chars";
    $htextagi2="$indx + 1 lines";
  }
}
#-------------
sub resplbt_help{
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->after(500);
    $helpwindow->deiconify();
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "ResetPlot-Button:",'end');
  if($indx eq ""){
    show_warn("   <-- No info yet !");
  }
  else {
    $helptext->see("$indx");
    $helptext->tagAdd('lookat',"$indx - 1 chars","$indx + 1 lines");
    $htextagi1="$indx - 1 chars";
    $htextagi2="$indx + 1 lines";
  }
}
#-------------
sub dlenplbt_help{
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->after(500);
    $helpwindow->deiconify();
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "DataLengthTimeScanPlot-Button:",'end');
  if($indx eq ""){
    show_warn("   <-- No info yet !");
  }
  else {
    $helptext->see("$indx");
    $helptext->tagAdd('lookat',"$indx - 1 chars","$indx + 1 lines");
    $htextagi1="$indx - 1 chars";
    $htextagi2="$indx + 1 lines";
  }
}
#-------------
sub ptscplbt_help{
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->after(500);
    $helpwindow->deiconify();
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "PedTimeScanPlot-Button:",'end');
  if($indx eq ""){
    show_warn("   <-- No info yet !");
  }
  else {
    $helptext->see("$indx");
    $helptext->tagAdd('lookat',"$indx - 1 chars","$indx + 1 lines");
    $htextagi1="$indx - 1 chars";
    $htextagi2="$indx + 1 lines";
  }
}
#-------------
sub fpedplbt_help{
  if(! Exists($helpwindow)){
    &open_help_window;
  }
  else {
    $helpwindow->after(500);
    $helpwindow->deiconify();
    $helpwindow->raise();
    if($htextagi1 ne ""){$helptext->tagRemove('lookat',$htextagi1,$htextagi2);}
  }
  $helptext->see("end");
  my $indx=$helptext->search(-backwards, "OneFilePedsPlot-Button:",'end');
  if($indx eq ""){
    show_warn("   <-- No info yet !");
  }
  else {
    $helptext->see("$indx");
    $helptext->tagAdd('lookat',"$indx - 1 chars","$indx + 1 lines");
    $htextagi1="$indx - 1 chars";
    $htextagi2="$indx + 1 lines";
  }
}
#------------------------------------------------------------------------------
sub scand{ # scan ped-directory for needed ped-files in required date-window
#
  $RunNum+=1;
  $curline="\n =====> New Scan $RunNum started...\n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
  $updbtext="UpdDB";
#
  $pathpeds=$scandir;#complete path to tof/anti peds_files_store directory
  $patharch=$pathpeds.$archdir;#complete path to tof/anti peds_files_archive-directory
#
  if($tofantsel eq "useTOF"){
    $detpat="tofp_";
  }
  else{
    $detpat="antp_";
  }
#
  $patt_tb=$detpat."tb_rl";
  $patt_ds=$detpat."ds_rl";
#--- scan directory to create ped-file's list :
  @filelist=();
  opendir(DIR,$pathpeds);
  if($boardpeds==1 && $offlpeds==1){
    @filelist=grep{/$patt_tb/ || /$patt_ds/ } readdir(DIR);
  }
  elsif($boardpeds==1){
    @filelist=grep{/$patt_tb/} readdir(DIR);
  }
  elsif($offlpeds==1){
    @filelist=grep{/$patt_ds/} readdir(DIR);
  }
  else{
    $message="Undefined ped-type (onboard|offline) !";
    return;
  }
  closedir(DIR);
#
  $nelem=scalar(@filelist);
  if($nelem == 0){
    $message="Dir $pathpeds is empty !";
    return;
  }
#--- sort list in the increasing order of run-number (it is unix-time of calibration):
  @processed = ();#clear list-pattern of processed files
  @runs = ();
  @detnam = ();
  @pedtyp = ();
  @sfilelist= ();#clear sorted list
#
  @sfilelist = sort{unpack("x11 A*",$a) <=> unpack("x11 A*",$b)} @filelist;# sort by run>
#
#--- make necessary selection of files in the ordered list(make pattern of good ones):
#
  my ($yy1,$mm1,$dd1,$hh1,$mn1);
  my ($yy2,$mm2,$dd2,$hh2,$mn2);
  my ($ptime,$year,$month,$day,$hour,$min,$sec,$time);
  $ss1=0;
  ($yy1,$mm1,$dd1,$hh1,$mn1)=unpack("A2 x1 A2 x1 A2 x1 A2 x1 A2",$fdat1);#unpack dates low limit
  $yy1=$yy1+2000;
  $mm1=$mm1;
  $TIMEL=timelocal($ss1,$mn1,$hh1,$dd1,$mm1-1,$yy1-1900);# UTC(GMT) in seconds (imply local time as input)
#   
  $ss2=0;
  ($yy2,$mm2,$dd2,$hh2,$mn2)=unpack("A2 x1 A2 x1 A2 x1 A2 x1 A2",$fdat2);#unpack dates high limit
  $yy2=$yy2+2000;
  $mm2=$mm2;
  $TIMEH=timelocal($ss2,$mn2,$hh2,$dd2,$mm2-1,$yy2-1900);
#   
  $nselem=0;
  if($debug==1){
    $curline="   <-- FilesList is created :\n\n";
    $logtext->insert('end',$curline);
    $logtext->yview('end');
  }
  for($i=0;$i<$nelem;$i++){#<--- loop over list to select files in dates window
    ($detnam[$i],$pedtyp[$i],$runs[$i])=unpack("A4 x1 A2 x4 A*",$sfilelist[$i]);
#    print $detnam[$i]," ",$pedtyp[$i]," ",$runs[$i]," ",$processed[$i],"\n";
    $ptime=localtime($runs[$i]);#local time (imply run# to be UTC-seconds as input)
    $year=$ptime->year+1900;
    $month=$ptime->mon+1;
    $day=$ptime->mday;
    $hour=$ptime->hour;
    $min=$ptime->min;
    $sec=$ptime->sec;
#
    $time="yyyy/mm/dd=".$year."/".$month."/".$day." hh:mm:ss=".$hour.":".$min.":".$sec;
    if($debug==1){
#      printf("Date: %02d:%02d:%02d-%04d/%02d/%02d\n",$hour,$min,$sec,$year,$month,$day);
      $curline=$detnam[$i]." ".$pedtyp[$i]." ".$runs[$i]." ".$processed[$i]."  PedFile time(local): ".$time."\n\n";
      $logtext->insert('end',$curline);
      $logtext->yview('end');
      $mwnd->update;
    }
#
    if($processed[$i] == 0){
      if($files != "All"){ # select single file
        if($files == $sfilelist[$i]){
          $nselem+=1;
          $processed[$i]=1;
          $curline="      file $sfilelist[$i] is selected, date(local):".$time."\n";
          $logtext->insert('end',$curline);
          $logtext->yview('end');
	}
      }
      else{ #select all files in required dates range
        if(($runs[$i]>=$TIMEL && $runs[$i]<$TIMEH) || ($TIMEL > $TIMEH)){
          $nselem+=1;
          $processed[$i]=1;
          $curline="      file $sfilelist[$i] is selected, date(local):".$time."\n";
          $logtext->insert('end',$curline);
          $logtext->yview('end');
        }
      }
#
    }
  }#---> endof files loop
  if($nselem == 0){
    $message="No files selected for processing ?!\n";
    return;
  }
  else{
    $curline="   \n   <-- Found $nselem files in dates-window\n";
    $logtext->insert('end',$curline);
    $logtext->yview('end');
  }
#--- find min/max run# in the directory:
  $runmx=0;
  for($i=0;$i<$nelem;$i++){
      if($runs[$i] > $runmx) {$runmx=$runs[$i];}
  }
  $runmn=$runmx;
  for($i=0;$i<$nelem;$i++){
    if($runs[$i] <= $runmn) {$runmn=$runs[$i];}
  }
#
#--- create standard list-file with selected file-names:
#
  $sfname="selfile.dat";
  open(OUTFN,"> $sfname") or die show_warn("   <-- Cannot open $sfname $!");
  for($i=0;$i<$nelem;$i++){#<--- selected files --> list-file
    if($processed[$i] == 1){
      print OUTFN $sfilelist[$i],"\n";#write to file 
    }
  }
  close(OUTFN);
#
  $curline="   <-- File-list is saved in $sfname\n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
#
  $ptime=localtime($runmn);#loc.time of earliest run in dir (imply run# to be UTC-seconds as input)
  $year=$ptime->year+1900;
  $month=$ptime->mon+1;
  $day=$ptime->mday;
  $hour=$ptime->hour;
  $min=$ptime->min;
  $sec=$ptime->sec;
  $time="yyyy/mm/dd=".$year."/".$month."/".$day." hh:mm:ss=".$hour.":".$min.":".$sec;
  $curline="  Date(local) of the earliest run in the dir : ".$time."\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
#
  $ptime=localtime($runmx);#local time of the latest run in the dir 
  $year=$ptime->year+1900;
  $month=$ptime->mon+1;
  $day=$ptime->mday;
  $hour=$ptime->hour;
  $min=$ptime->min;
  $sec=$ptime->sec;
  $time="yyyy/mm/dd=".$year."/".$month."/".$day." hh:mm:ss=".$hour.":".$min.":".$sec;
  $curline="  Date(local) of the   latest run in the dir : ".$time."\n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
#
  $curline="  **** NEED TO MODIFY dates window ?? CAN DO THAT NOW and repeate scan !!! ****\n\n";
  $logtext->insert('end',$curline,'Attention');
  $logtext->yview('end');
  $mwnd->update;
}
#----------------------------------------------------------------
sub checkq{# create TopLevel with canvas and control panel
#
if(! Exists($topl1)){# <-- define size and create 2nd window:
  $topl1=$mwnd->Toplevel();
  $swdszx=$mnwdszx;#X-size of 2nd window (portion of display x-size)
  $swdszy=1-$mnwdszy-0.11;#Y-size of 2nd window (portion of display y-size)(0.1->bot.syst.panel)
  my $szx=int($swdszx*$displx);#in pixels
  my $szy=int($swdszy*$disply);
  my $swdposx="+0";
  my $swdpy=int(($mnwdszy+0.03)*$disply);# 2nd wind. pos-y in pixels(0.03->safety)
  my $swdpozy="+".$swdpy;
  my $swdgeom=$szx."x".$szy.$swdposx.$swdpozy;
  print "CANVAS-window geom-",$swdgeom,"\n";
  $topl1->geometry($swdgeom);
  $topl1->title("Peds Analysis");
#
  my $cnvrsz=0.7;
  my $bheight=0.1;
  my $bspac=0.02;
  my $crely;
  $cp_fram=$topl1->Frame(-label => "Control panel",-background => "Cyan",  
                                                    -relief=>'groove', -borderwidth=>5)->place(
                                                    -relwidth=>(1-$cnvrsz), -relheight=>1,
                                                    -relx=>0, -rely=>0);
  $crely=0.1;
  $resplbt=$cp_fram->Button(-text=>"Reset plot",               -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&points_clear)->place(
                                                                 -relwidth=>0.25, -relheight=>$bheight,
                                                                           -relx=>0, -rely=>0.9);
  $resplbt->bind("<Button-3>", \&resplbt_help);
#----
  $cp_fram->Button(-text=>"Save plot",               -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&plot_save)->place(
                                                                 -relwidth=>0.25, -relheight=>$bheight,
                                                                           -relx=>0.25, -rely=>0.9);
#----
  $cp_fram->Button(-text=>"Close",                      -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>sub{$topl1->withdraw})->place(
                                                                 -relwidth=>0.25, -relheight=>$bheight,
                                                                           -relx=>0.75, -rely=>0.9);
#----
  $dlenplbt=$cp_fram->Button(-text=>"File DataLength time-scan",           -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&dlen_tscan)->place(
                                                                 -relwidth=>1, -relheight=>$bheight,
                                                                           -relx=>0, -rely=>$crely);
  $crely+=($bheight+$bspac);
  $dlenplbt->bind("<Button-3>", \&dlenplbt_help);
#----
  $ptscplbt=$cp_fram->Button(-text=>"Ped time-scan for Ch_ID(LBBSP) :",-borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&ped_tscan)->place(
                                                                 -relwidth=>1, -relheight=>$bheight,
                                                                    -relx=>0, -rely=>$crely);
  $crely+=($bheight);
  $ptscplbt->bind("<Button-3>", \&ptscplbt_help);
#----
if($tofantsel eq "useTOF"){
  $chidvar="10410(LBBSP)";
}
else{
  $chidvar="410(BSP)";
}
$cp_fram->Entry(-relief=>'sunken', -background=>yellow,
                                              -font=>$font3,
                                              -textvariable=>\$chidvar)->place(
                                              -relwidth=>1, -relheight=>$bheight,  
                                              -relx=>0, -rely=>$crely);
  $crely+=($bheight+$bspac);
#----
  $fpedplbt=$cp_fram->Button(-text=>"Plot All Channels Peds for file name :",  -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&file_scan)->place(
                                                                 -relwidth=>1, -relheight=>$bheight,
                                                                 -relx=>0, -rely=>$crely);
  $crely+=($bheight);
  $fpedplbt->bind("<Button-3>", \&fpedplbt_help);
#----
$fileidvar="";
$fentrw=$cp_fram->Entry(-relief=>'sunken', -background=>yellow,
#                                              -foreground=>"blue",
                                              -font=>$font3,
                                              -textvariable=>\$fileidvar)->place(
                                              -relwidth=>1, -relheight=>$bheight,  
                                              -relx=>0, -rely=>$crely);
  $crely+=($bheight);
#----
  $file_remove_btext="^^^ Mark above file as bad ^^^";
  $cp_fram->Button(-textvariable=>\$file_remove_btext,  -borderwidth=>3,
                                                        -activebackground=>"yellow",
			                                -activeforeground=>"red",
			                                -background=>"green",
			                                -cursor=>hand2,
                                                        -command=>\&mark_file)->place(
                                                                 -relwidth=>1, -relheight=>$bheight,
                                                                           -relx=>0, -rely=>$crely);
  $crely+=($bheight+$bspac);
#---
  $cv_fram=$topl1->Frame(-label => "Params time-evolution window", 
                                                    -relief=>'groove', -borderwidth=>5)->place(
                                                    -relwidth=>$cnvrsz, -relheight=>1,
                                                    -relx=>(1-$cnvrsz), -rely=>0);
  $canscrx1=-40;# scroll-area (min/max), all in pixels
  $canscry1=-250;
  $canscrx2=10000;
  $canscry2=20;
  $cnv=$cv_fram->Scrolled("Canvas",-background => "yellow",
                                        -scrollregion=>[$canscrx1,$canscry1,
					                $canscrx2,$canscry2])
                                                              ->pack(-fill => 'both', -expand => 1);
  $canvas=$cnv->Subwidget("canvas");
    
  $xaxe=$canvas->createLine($canscrx1,0,$canscrx2,0, -fill=>"black", -arrow=>"last", -width=>2);
  $yaxe=$canvas->createLine(0,$canscry2,0,$canscry1, -fill=>"black", -arrow=>"last", -width=>2);
#---
  }
  else{
    $topl1->deiconify();
    $topl1->raise();
  }
  $plot1_actf=0;#plot1 "active" flag
  $plot2_actf=0;#plot2 "active" flag
  $plot3_actf=0;#plot3 "active" flag
  $topl1->bell;
#  print "\aQUQU\n";
}
#-------------
sub points_clear #<--- delete canvas-items in plot's point-ids list 
{
  if($plot_pntmem > 0){
    for($i=0;$i<$plot_pntmem;$i++){$canvas->delete($plot_pntids[$i]);}
  }
  $plot_pntmem = 0;
}
#-------------
sub access_clear #<--- delete canvas-items in plot's accessories-ids list 
{
  if($plot_accmem > 0){
    for($i=0;$i<$plot_accmem;$i++){$canvas->delete($plot_accids[$i]);}
  }
  $plot_accmem = 0;
}
#-------------
sub plot_save #<--- current plot in ps-file 
{
  my ($fname,$width,$height,$xcmax);
  $height=$canscry2-$canscry1;
#
  if($plot1_actf>0){
    $fname="plot_1_v".$plot1_actf.".ps";
    $width=($canscrx2-$canscrx1);
    $xcmax=$canscrx2;
  }
  elsif($plot2_actf>0){
    $fname="plot_2_v".$plot2_actf.".ps";
    $width=($canscrx2-$canscrx1);
#    $width=($plot2_xcmax-$canscrx1)+10;
    $xcmax=$plot2_xcmax;
  }
  else{
    $fname="plot_3_v".$plot3_actf.".ps";
    $width=($plot3_xcmax-$canscrx1)+10;
    $xcmax=$plot3_xcmax;
  }
  print " want to write ",$fname,"\n";
  print " width/height=",$width," ",$height," xcmax=",$xcmax,"\n";
  $canvas->postscript(-file=>$fname,-width=>$width,-height=>$height,-x=>$canscrx1,-y=>$canscry1);
  $curline="   <-- Plot is saved in $fname\n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
}
#-------------
sub dlen_tscan
{
#---> this is plot-1
#
  my ($xc,$yc)=(0,0);
  my ($xcoarsegrd,$xfinegrd)=(86400,3600);# day/hour in sec
  my ($ycoarsegrd,$yfinegrd)=(500,100);
  my $ybinw=5;
#
  my $goodc="green";
  my $badc="red";
  my $nzlines=0;
  my $tnwords=0;
  my $goodfiles=0;
  my $vcoo;
  my ($width,$ltxt,$dig,$colr,$vpos,$dasht,$titl);
  my ($ptime,$year,$month,$day,$hour,$min,$sec,$time);
#
#---> create scale/grid:
#
  if($plot1_actf==0){# 1st entry for plot2
    &access_clear;
    &points_clear;
    @plot_fileids=();# clear current plot point-assosiated files ids
#-- create Grid vertical lines + X-axes subscripts :
    for($i=0;$i<$canscrx2;$i++){
       if($i>0 && (($i*$binw_tev % $xfinegrd)==0 || ($i*$binw_tev % $xcoarsegrd)==0)){
         $xc=$i;
	 if(($i*$binw_tev % $xcoarsegrd)>0){# hour-line
	   $width=1;
	   $dig=(($i*$binw_tev % $xcoarsegrd)/$xfinegrd);
	   $ltxt="$dig";
	   $colr="blue";
	   $dasht='.';
	 }
	 else{# day-line
	   $width=1;
	   $dig=($i*$binw_tev/$xcoarsegrd);
	   $ltxt="$dig";
	   $colr="red";
	   $dasht='-';
	 }
         $lid=$canvas->createLine($xc,0,$xc,$canscry1, -fill=>'black', -width=>$width,-dash=>$dasht);
         $plot_accids[$plot_accmem]=$lid;#store in glob. plot-accessoiries list
         $plot_accmem+=1;
         $lid=$canvas->createText($xc,+10,-text=>$ltxt,-font=>$font2,-fill=>$colr);  
         $plot_accids[$plot_accmem]=$lid;#store in glob.list
         $plot_accmem+=1;
       }
    }
#-- create Grid horizontal lines + Y-axes subscripts :
    for($i=0;$i<-$canscry1;$i++){
       if($i>0 && (($i*$ybinw % $yfinegrd)==0 || ($i*$ybinw % $ycoarsegrd)==0)){
         $yc=$i;
	 if(($i*$ybinw % $ycoarsegrd)>0){# fine scale line
	   $width=1;
	   $dig=$i*$ybinw;
	   $ltxt="$dig";
	   $colr="blue";
	   $dasht='.';
	 }
	 else{# coarse scale line
	   $width=1;
	   $dig=$i*$ybinw;
	   $ltxt="$dig";
	   $colr="red";
	   $dasht='-';
	 }
         $lid=$canvas->createLine(0,-$yc,$canscrx2,-$yc, -fill=>'black', -width=>$width,-dash=>$dasht);
         $plot_accids[$plot_accmem]=$lid;#store in glob. plot-accessoiries list
         $plot_accmem+=1;
         $lid=$canvas->createText(-20,-$yc,-text=>$ltxt,-font=>$font2,-fill=>$colr);  
         $plot_accids[$plot_accmem]=$lid;#store in glob.list
         $plot_accmem+=1;
       }
    }
#-- add 0 :
    $lid=$canvas->createText(-10,10,-text=>"0",-font=>$font2,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
#-- add title :
    $ptime=localtime($runmn);#loc.time of earliest run in dir (imply run# to be UTC-seconds as input)
    $year=$ptime->year+1900;
    $month=$ptime->mon+1;
    $day=$ptime->mday;
    $hour=$ptime->hour;
    $min=$ptime->min;
    $sec=$ptime->sec;
    $time=$year."/".$month."/".$day."  ".$hour.":".$min;
    $titl="PedTable_Length Time-evolution,  '0'-time is $time";
    $lid=$canvas->createText(400,$canscry1+40,-text=>$titl,-font=>$font2,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
    $titl="Time-scale: red digits ->days, blue ->hours";
    $lid=$canvas->createText(400,-10,-text=>$titl,-font=>$font2,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
#
#    
#    $lid=$canvas->createGrid(0,0,3600/$binw_tev,10,-line=>1,-dash=>'-.',-fill=>'blue');#(1h x 10) grid  
#    $plot_accids[$plot_accmem]=$lid;#store in glob. plot-accessoiries list
#    $plot_accmem+=1;
#    $lid=$canvas->createGrid(0,0,86400/$binw_tev,50,-line=>1,-width=>1,-fill=>'blue');#(24h x 50) grid  
#    $plot_accids[$plot_accmem]=$lid;#store in glob.list
#    $plot_accmem+=1;
#    $lid=$canvas->createText(-10,-7,-text=>"o",-font=>$font2,-fill=>'blue');  
#    $plot_accids[$plot_accmem]=$lid;#store in glob.list
#    $plot_accmem+=1;
#    $lid=$canvas->createText(-20,-100,-text=>"100",-font=>$font2,-fill=>'blue');  
#    $plot_accids[$plot_accmem]=$lid;#store in glob.list
#    $plot_accmem+=1;
#    $lid=$canvas->createText(-20,-200,-text=>"200",-font=>$font2,-fill=>'blue');  
#    $plot_accids[$plot_accmem]=$lid;#store in glob.list
#    $plot_accmem+=1;
#    $lid=$canvas->createText(300,9,-text=>"---- 1h time grid --->",-font=>$font2,-fill=>'blue');  
#    $plot_accids[$plot_accmem]=$lid;#store in glob.list
#    $plot_accmem+=1;
    $plot2_actf=0;
    $plot3_actf=0;
  }
  $plot1_actf+=1;
#
#
  for($i=0;$i<$nelem;$i++){#
    $sfname=$pathpeds."/".$sfilelist[$i];
    $nzlines=0;
    $nwords=0;
    $tnwords=0;
    if($processed[$i] == 1){
#
      open(INPFN,"< $sfname") or die show_warn("   <-- Cannot open $sfname for reading $!");
      while(defined ($line = <INPFN>)){
        chomp $line;
#	print $line,"\n";
        if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
        if($line =~/==========/){last;}# break on my internal EOF
        if($line =~/12345/){last;}# break on my internal EOF
	$nzlines+=1;
	$line =~s/^\s+//s;# remove " "'s at the beg.of line
	@words=split(/\s+/,$line);#split by any number of continious " "'s
	$nwords=scalar(@words);
	$tnwords=$tnwords+$nwords;
#	for($j=0;$j<$nwords;$j++){
#	  print $words[$j]," ";
#	}
#	print "\n";
#	print "nwords=",$nwords,"\n";
      }
      close(INPFN) or die show_warn("   <-- Cannot close $sfname after reading $!");
#      print "Total nz-lines:",$nzlines,"  total words:",$tnwords,"\n";
#
      $xc=($runs[$i]-$runmn)/$binw_tev;#wrt min run# in relat.units(=binwidth)
      $vcoo=$tnwords/$ybinw; 
      if($detnam[$i] eq "tofp"){
        if($tnwords==816){
	  $lid=$canvas->createOval($xc-2,-($vcoo-2),$xc+2,-($vcoo+2), -fill=>$goodc, -width=>3);
	  $goodfiles+=1;
	}
	else{
	  $lid=$canvas->createOval($xc-2,-($vcoo-2),$xc+2,-($vcoo+2), -fill=>$badc, -width=>2);
          $processed[$i] = 0;# mark as bad
	}
      }
      if($detnam[$i] eq "antp"){
        if($tnwords==48){
	  $lid=$canvas->createOval($xc-2,-($vcoo-2),$xc+2,-($vcoo+2), -fill=>$goodc, -width=>2);
	  $goodfiles+=1;
	}
	else{
	  $lid=$canvas->createOval($xc-2,-($vcoo-2),$xc+2,-($vcoo+2), -fill=>$badc, -width=>2);
          $processed[$i] = 0;# mark as bad
	}
      }
      $plot_pntids[$plot_pntmem]=$lid;
      $plot_pntmem+=1;
      if($xc>$plot1_xcmax){$plot1_xcmax=$xc;}
    }
    $topl1->update;
  }
  $curline="   <-- DataLength scan was completed with $goodfiles good files \n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
  if($goodfiles==0){
    $message="No good files after data-length check ?!";
    if($soundtext eq "Sound-ON"){$mwnd->bell;}
  }
}
#-------------
sub ped_tscan
{
#---> time-evolution of particular channel ped/sig, this is plot-2
#
  my $layx,$barx,$sidx,$pmtx,$rest;
  my $maxb;
  my $locvar;
  my @goodcolorlist=qw(green darkred blue orange black );
  $locvar=$chidvar;
#--> check ID :
  if($tofantsel eq "useTOF"){
    if($locvar =~/(LBBSP)/){substr($locvar,-7,7)="";}# remove "(LBBSP)" if present
    ($layx,$barx,$sidx,$pmtx,$rest)=unpack("A1 A2 A1 A1 A*",$locvar);
    if($layx>4 || $layx==0 || $barx==0 || $barx>$barsppl[$layx-1] || $sidx==0 || $sidx>2 || $pmtx>3){
      $message="wrong TOF channel-ID setting: $locvarvar ?!";
      $mwnd->bell;
      return;
    }
  }
  if($tofantsel eq "useANT"){
    if($locvar =~/(BSP)/){substr($locvar,-5,5)="";}# remove "(BSP)" if present
    ($barx,$sidx,$pmtx,$rest)=unpack("A1 A1 A1 A*",$locvar);
    $layx=0;
    if($barx==0 || $barx>8 || $sidx==0 || $sidx>2 || $pmtx>0){
      $message="wrong ACC channel-ID setting: $locvar ?!";
      $mwnd->bell;
      return;
    }
  }
  $curline="   <-- Single channel Time-scan for id=$locvar, l/bb/s/p=$layx/$barx/$sidx/$pmtx \n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
#
  my $ped=0;
  my $sig=0;
  my $sta=0;
  my ($xc,$yc)=(0,0);
  my ($xcoarsegrd,$xfinegrd)=(86400,3600);# day/hour in sec
  my ($ycoarsegrd,$yfinegrd)=(100,25);
  my $ybinw=1;
  my $goodc="darkgreen";
  my $badc="red";
  my $nzlines=0;
  my $curcol;
  my $colindx;
  my ($width,$dig,$ltxt,$colr,$dasht,$titl);
  my ($ptime,$year,$month,$day,$hour,$min,$sec,$time);
#
  @globpeds=();
#
#---> create scale/grid:
#
  if($plot2_actf==0){# 1st entry for plot2
    &access_clear;
    &points_clear;
    @plot_fileids=();# clear current plot point-assosiated files ids
#-- create Grid vertical lines + X-axes subscripts :
    for($i=0;$i<$canscrx2;$i++){
       if($i>0 && (($i*$binw_tev % $xfinegrd)==0 || ($i*$binw_tev % $xcoarsegrd)==0)){
         $xc=$i;
	 if(($i*$binw_tev % $xcoarsegrd)>0){# hour-line
	   $width=1;
	   $dig=(($i*$binw_tev % $xcoarsegrd)/$xfinegrd);
	   $ltxt="$dig";
	   $colr="blue";
	   $dasht='.';
	 }
	 else{# day-line
	   $width=1;
	   $dig=($i*$binw_tev/$xcoarsegrd);
	   $ltxt="$dig";
	   $colr="red";
	   $dasht='-';
	 }
         $lid=$canvas->createLine($xc,0,$xc,$canscry1, -fill=>'black', -width=>$width,-dash=>$dasht);
         $plot_accids[$plot_accmem]=$lid;#store in glob. plot-accessoiries list
         $plot_accmem+=1;
         $lid=$canvas->createText($xc,+10,-text=>$ltxt,-font=>$font2,-fill=>$colr);  
         $plot_accids[$plot_accmem]=$lid;#store in glob.list
         $plot_accmem+=1;
       }
    }
#-- create Grid horizontal lines + Y-axes subscripts :
    for($i=0;$i<-$canscry1;$i++){
       if($i>0 && (($i*$ybinw % $yfinegrd)==0 || ($i*$ybinw % $ycoarsegrd)==0)){
         $yc=$i;
	 if(($i*$ybinw % $ycoarsegrd)>0){# fine scale line
	   $width=1;
	   $dig=$i*$ybinw;
	   $ltxt="$dig";
	   $colr="blue";
	   $dasht='.';
	 }
	 else{# coarse scale line
	   $width=1;
	   $dig=$i*$ybinw;
	   $ltxt="$dig";
	   $colr="red";
	   $dasht='-';
	 }
         $lid=$canvas->createLine(0,-$yc,$canscrx2,-$yc, -fill=>'black', -width=>$width,-dash=>$dasht);
         $plot_accids[$plot_accmem]=$lid;#store in glob. plot-accessoiries list
         $plot_accmem+=1;
         $lid=$canvas->createText(-20,-$yc,-text=>$ltxt,-font=>$font2,-fill=>$colr);  
         $plot_accids[$plot_accmem]=$lid;#store in glob.list
         $plot_accmem+=1;
       }
    }
#-- add 0 :
    $lid=$canvas->createText(-10,10,-text=>"0",-font=>$font2,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
#-- add title :
    $ptime=localtime($runmn);#loc.time of earliest run in dir (imply run# to be UTC-seconds as input)
    $year=$ptime->year+1900;
    $month=$ptime->mon+1;
    $day=$ptime->mday;
    $hour=$ptime->hour;
    $min=$ptime->min;
    $sec=$ptime->sec;
    $time=$year."/".$month."/".$day."  ".$hour.":".$min;
    $titl="Ped/sigma Time-evolution,  '0'-time is $time";
    $lid=$canvas->createText(400,$canscry1+40,-text=>$titl,-font=>$font2,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
    $titl="Time-scale: red digits ->days, blue ->hours";
    $lid=$canvas->createText(400,-10,-text=>$titl,-font=>$font2,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;
    $plot_accmem+=1;
#
    $plot1_actf=0;
    $plot3_actf=0;
  }
  $colindx=($plot2_actf % 5);
  $goodc=$goodcolorlist[$colindx];#change color according fill-entry number
  $plot2_actf+=1;
#
  for($i=0;$i<$nelem;$i++){#<--- selected files loop
    $sfname=$pathpeds."/".$sfilelist[$i];
    $nzlines=0;
    $nwords=0;
    @globpeds=();#store all data of the file
    if($processed[$i] == 1){
#
      open(INPFN,"< $sfname") or die show_warn("   <-- Cannot open $sfname for reading $!");
      while(defined ($line = <INPFN>)){
        chomp $line;
        if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
        if($line =~/==========/){last;}# break on my internal EOF
        if($line =~/12345/){last;}# break on my internal EOF
	$nzlines+=1;
	$line =~s/^\s+//s;# remove " "'s at the beg.of line
	@words=split(/\s+/,$line);#split by any number of continious " "'s
	$nwords=scalar(@words);
	push(@globpeds,@words);
      }
      close(INPFN) or die show_warn("   <-- Cannot open $sfname after reading $!");
#
      $seqnum=0;
      if($pmtx==0){# anode (TOF/ANTI)
        if($layx>0){# tof
          for($il=0;$il<$layx-1;$il++){$seqnum+=$barsppl[$il]*6;}# count in layers befor layx
          for($ib=0;$ib<$barx-1;$ib++){$seqnum+=6;}# count in paddles befor barx
          for($is=0;$is<$sidx-1;$is++){$seqnum+=3;}# count in sides befor sidx
	  $sta=$globpeds[$seqnum];
	  $ped=$globpeds[$seqnum+1];
	  $sig=$globpeds[$seqnum+2];
	}
	else{# anti
          for($ib=0;$ib<$barx-1;$ib++){$seqnum+=6;}# count in paddles befor barx
          for($is=0;$is<$sidx-1;$is++){$seqnum+=1;}# count in sides befor sidx
	  $sta=$globpeds[$seqnum];
	  $ped=$globpeds[$seqnum+2];
	  $sig=$globpeds[$seqnum+4];
	}
      }
      else{# dynodes (only TOF)
        $seqnum=6*($barsppl[0]+$barsppl[1]+$barsppl[2]+$barsppl[3]);# total anode-data
        for($il=0;$il<$layx-1;$il++){$seqnum+=$barsppl[$il]*18;}# count in layers befor layx
        for($ib=0;$ib<$barx-1;$ib++){$seqnum+=18;}# count in paddles befor barx
        for($is=0;$is<$sidx-1;$is++){$seqnum+=9;}# count in sides befor sidx
        for($ip=0;$ip<$pmtx-1;$ip++){$seqnum+=3;}# count in pmts befor pmtx
	$sta=$globpeds[$seqnum];
	$ped=$globpeds[$seqnum+1];
	$sig=$globpeds[$seqnum+2];
      }
#
      $xc=($runs[$i]-$runmn)/$binw_tev;#run-position wrt min run# in relat.units(=binwidth)
#
      if($sta==0){$curcol=$goodc;}
      else {$curcol=$badc;}
      $lid=$canvas->createLine($xc,-($ped-$sig),$xc,-($ped+$sig), -fill=>$curcol, -width=>4);
      $canvas->bind($lid,"<Button-1>",[\&item_mark1,$lid]);
      $plot_fileids[$plot_pntmem]=$i;#store file-index
      $plot_pntids[$plot_pntmem]=$lid;#store in glob.list
      $plot_pntmem+=1;
      if($xc>$plot2_xcmax){$plot2_xcmax=$xc;}
    }#--->endof file-loop
    $topl1->update;
  }
}
#-------------
sub item_mark
{
  my ($canv,$id) = @_;
  my $fnam;
  my $color=$canv->itemcget($id, -fill);
  if($color eq "green"){
    print "set_bad branch\n";
    $canv->itemconfigure($id,-fill=>"red");
    $elem_num=-1;
    for($i=0;$i<$nelem;$i++){# scan IDlist to find item#
      if($plot_fileids[$i]==$id){
        $elem_num=$i;
        last;
      }
    }
    if($elem_num>=0 && $elem_num<$nelem){
      $fnam=$sfilelist[$elem_num];
      $processed[$i] = 0;# bad
      $message="Select $fnam as bad ($elem_num-th in the FileList) !";
      $mwnd->bell;
    }
    else{
      $message="BindError: LineID=$id not found  !";
      $mwnd->bell;
    }
  }
#
  else{
    print "set_good branch\n";
    $canv->itemconfigure($id,-fill=>"green");
    $elem_num=-1;
    for($i=0;$i<$nelem;$i++){# scan IDlist to find item#
      if($idlist[$i]==$id){
        $elem_num=$i;
        last;
      }
    }
    if($elem_num>=0 && $elem_num<$nelem){
      $fnam=$sfilelist[$elem_num];
      $processed[$i] = 1;# good
      $message="Restore $fnam as good ($elem_num-th in the FileList) !";
    }
    else{
      $message="BindError: LineID=$id not found  !";
      $mwnd->bell;
    }
  }
  $topl1->update;
}
#-------------
sub item_mark1
{
  my ($canv,$id) = @_;
  my ($findx,$fnam,$elem_num);
  $elem_num=-1;
  for($i=0;$i<$plot_pntmem;$i++){
    if($id == $plot_pntids[$i]){
      $elem_num=$i;
      last;
    }
  }
#
  if($elem_num>=0 && $elem_num<$plot_pntmem){
    $findx=$plot_fileids[$elem_num];
    $fnam=$sfilelist[$findx];
    $fileidvar=$fnam;# show name
  }
  else{
    $message="Bind1Error: LineID=$id not found  !";
    $mwnd->bell;
  }
#
  $topl1->update;
}
#---------
sub file_scan
{
#--->all-channels  ped/sig-info for one particular ped-file, this is plot-3
#
  $last_pedscanned_file_num=0;
  if($fileidvar ne ""){
    for($i=0;$i<$nelem;$i++){#<--- selected files loop
      if($fileidvar eq $sfilelist[$i]){
        $last_pedscanned_file_num=1+$i;
	last;
      }
    }
    if($last_pedscanned_file_num==0){
      $message="FileScan Error: file name not in the list!";
      $mwnd->bell;
      return
    }
  }
  else{
    $message="FileScan Error: missing file name !";
    $mwnd->bell;
    return;
  }
#
  my($laymx,$xc,$barmx);
  if($tofantsel eq "useTOF"){$laymx=4;}
  if($tofantsel eq "useANT"){$laymx=1;}
  my($bot,$top,$xc1,$xc2);
#
  $sfname=$pathpeds."/".$fileidvar;
  @globpeds=();
  my $pntwid=2;#point width in pixels
  my $barspac=2;#pixels spacing between 2 neigbour paddles
  my $sidspac=4;#pixels spacing between 2 neigbour sides
  my $layspac=8;#pixels spacing between 2 neigbour layers
  my $dynspac=16;#pixels spacing between Anode and Dynode plots
#
  my $layshift=2*10*($pntwid+$barspac)+$sidspac+$layspac;
  my $dynshift=4*$layshift+$dynspac;
  my $sidshift=10*($pntwid+$barspac)+$sidspac;
  my $dynvsh=50;#pix. vert.shift to plot dynodes for the same side/paddle
  my ($width,$ltxt,$stxt,$lmax,$dapl,$vstp);
#
#---> create scales/greeds:
#
  if($plot3_actf==0){# 1st entry for plot3
    &access_clear;
    &points_clear;
    @plot_fileids=();# clear current plot point-assosiated files ids
    $dapl=2;#need dynode/anode plot
    $lmax=$laymx;
    if($laymx==1){$dapl=1;}
#
    for($dyn=0;$dyn<$dapl;$dyn++){
    for($il=0;$il<$lmax;$il++){#layer-loop
      $ltxt="L".($il+1);
      for($is=0;$is<2;$is++){#side-loop
        $stxt="S".($is+1);
        $xc1=$il*$layshift+$is*$sidshift+$dyn*$dynshift;
	$xc2=$xc1+($barspac+$pntwid)*9;
	$width=1;
#	if($is==0){$width=2;}
        $lid=$canvas->createLine($xc1,0,$xc1,$canscry1, -fill=>'black', -width=>$width,-dash=>'-');
        $plot_accids[$plot_accmem]=$lid;#store in glob. plot-accessoiries list
        $plot_accmem+=1;
	$width=1;
#	if($is==1){$width=2;}
        $lid=$canvas->createLine($xc2,0,$xc2,$canscry1, -fill=>'black', -width=>$width,-dash=>'-');
        $plot_accids[$plot_accmem]=$lid;#store in glob. plot-accessoiries list
        $plot_accmem+=1;
	$xc=$xc1+5*($barspac+$pntwid);
        $lid=$canvas->createText($xc,-8,-text=>$stxt,-font=>$font3,-fill=>'blue');  
        $plot_accids[$plot_accmem]=$lid;#store in glob.list
        $plot_accmem+=1;
      }
      $xc=$xc1-0.5*$sidspac;
      $lid=$canvas->createText($xc,-16,-text=>$ltxt,-font=>$font2,-fill=>'red');  
      $plot_accids[$plot_accmem]=$lid;#store in glob.list
      $plot_accmem+=1;
    }#--->endof Layer-loop
#
    if($dyn==0){$stxt="Anode-channels";}
    else {$stxt="Dynode-channels";}
    $xc=2*$layshift+$dyn*$dynshift;
    $lid=$canvas->createText($xc,9,-text=>$stxt,-font=>$font2,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;#store in glob.list
    $plot_accmem+=1;
    }#--->endof An/Dyn loop
#
    $lid=$canvas->createText(-10,-8,-text=>"0",-font=>$font2,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;#store in glob.list
    $plot_accmem+=1;
    $lid=$canvas->createText(-20,-100,-text=>"100",-font=>$font2,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;#store in glob.list
    $plot_accmem+=1;
    $lid=$canvas->createText(-20,-200,-text=>"200",-font=>$font2,-fill=>'blue');  
    $plot_accids[$plot_accmem]=$lid;#store in glob.list
    $plot_accmem+=1;
    $plot_accmem+=1;
#--horiz.grid:
    $vstp=-25;
    my $imx=$canscry1/$vstp;
    for($i=1;$i<$imx;$i++){
      $lid=$canvas->createLine(0,$i*$vstp,2*$dynshift,$i*$vstp, -fill=>'black', -width=>1,-dash=>'.');
      $plot_accids[$plot_accmem]=$lid;#store in glob. plot-accessoiries list
      $plot_accmem+=1;
    }
    $plot1_actf=0;
    $plot2_actf=0;
  }
  $plot3_actf+=1;
  $lid=$canvas->createText(160,$canscry1+40,-text=>"Peds/Rms for file $fileidvar",-font=>$font2,-fill=>'blue');  
  $plot_accids[$plot_accmem]=$lid;#store in glob.list
  $plot_accmem+=1;
#---------
    open(INPFN,"< $sfname") or die show_warn("   <-- Cannot open $sfname for reading $!");
    while(defined ($line = <INPFN>)){
      chomp $line;
      if($line =~/^\s*$/){next;}# skip empty or all " "'s lines
      if($line =~/==========/){last;}# break on my internal EOF
      if($line =~/12345/){last;}# break on my internal EOF
      $line =~s/^\s+//s;# remove " "'s at the beg.of line
      @words=split(/\s+/,$line);#split by any number of continious " "'s
      $nwords=scalar(@words);
      push(@globpeds,@words);
    }
    close(INPFN) or die show_warn("   <-- Cannot open $sfname after reading $!");
#
    $seqnum=0;
#--->first anodes:
    $xc=0;
    for($il=0;$il<$laymx;$il++){
      if($tofantsel eq "useTOF"){$barmx=$barsppl[$il];}
      else {$barmx=8;}
      for($ib=0;$ib<$barmx;$ib++){
        for($is=0;$is<2;$is++){
	  if($tofantsel eq "useTOF"){
	    $sta=$globpeds[$seqnum+3*$is];
	    $ped=$globpeds[$seqnum+3*$is+1];
	    $sig=$globpeds[$seqnum+3*$is+2];
	  }
	  else{
	    $sta=$globpeds[$seqnum+$is];
	    $ped=$globpeds[$seqnum+$is+2];
	    $sig=$globpeds[$seqnum+$is+4];
	  }
          $xc=$il*$layshift+$is*$sidshift+($barspac+$pntwid)*$ib+$pntwid/2;
#
	  if(sta==0){$curcol="green";}
	  else {$curcol="red";}
          $lid=$canvas->createLine($xc,-($ped-$sig),$xc,-($ped+$sig), -fill=>$curcol, -width=>$pntwid);
#          $canvas->bind($lid,"<Button-1>",[\&item_mark1,$lid]);
          $plot_pntids[$plot_pntmem]=$lid;#store in glob.list
          $plot_pntmem+=1;
          if($xc>$plot3_xcmax){$plot3_xcmax=$xc;}
	}#-->endof side-loop
	$seqnum+=6;
#
      }#-->endof bar-loop
#
    }#-->endof lay-loop
#--->next - dynodes:
    if($tofantsel eq "useTOF"){#only for TOF
    for($il=0;$il<$laymx;$il++){
      if($tofantsel eq "useTOF"){$barmx=$barsppl[$il];}
      else {$barmx=8;}
      for($ib=0;$ib<$barmx;$ib++){
        for($is=0;$is<2;$is++){
          for($pm=0;$pm<3;$pm++){
	    $sta=$globpeds[$seqnum];
	    $ped=$globpeds[$seqnum+1];
	    $sig=$globpeds[$seqnum+2];
            $xc=$dynshift+$il*$layshift+$is*$sidshift+($barspac+$pntwid)*$ib+$pntwid/2;
#
            if($ped!=0 && $sig!=0){#dynode exists
	      $bot=-(($ped-$sig)+$pm*$dynvsh); 
	      $top=-(($ped+$sig)+$pm*$dynvsh); 
	      if(sta==0){$curcol="green";}
	      else {$curcol="red";}
              $lid=$canvas->createLine($xc,$bot,$xc,$top, -fill=>$curcol, -width=>$pntwid);
#              $canvas->bind($lid,"<Button-1>",[\&item_mark1,$lid]);
              $plot_pntids[$plot_pntmem]=$lid;#store points in glob.list
              $plot_pntmem+=1;
              if($xc>$plot3_xcmax){$plot3_xcmax=$xc;}
	    }
	    $seqnum+=3;
	  }#-->endof pm-loop
	}#-->endof side-loop
#
      }#-->endof bar-loop
#
    }#-->endof lay-loop
    }
#
}
#---------
sub mark_file
{
#
#--- Mark displayed(after its peds scan) file as bad/good:
#
  if($fileidvar ne ""){
    my $flag=0;
    for($i=0;$i<$nelem;$i++){#<--- selected files loop
      if($fileidvar eq $sfilelist[$i]){
        if($processed[$i]==1){
          $file_remove_btext="^^^ Mark above file as good ^^^";
	  $fentrw->configure(-foreground=>"red");
	  $processed[$i]=0;	
          $curline="    <--File $fileidvar marked as bad !\n\n";
          $logtext->insert('end',$curline);
          $logtext->yview('end');
	  $flag=1;
	  last;
	}
	else{
          $file_remove_btext="^^^ Mark above file as bad ^^^";
	  $fentrw->configure(-foreground=>"blue");
	  $processed[$i]=1;	
          $curline="    <--File $fileidvar marked as good !\n\n";
          $logtext->insert('end',$curline);
          $logtext->yview('end');
	  $flag=2;
	  last;
	}
      }
#      $topl1->update;
    }
    if($flag==0){
      $message="Mark_file Error: file name not in the list !";
      $mwnd->bell;
    }
  }
  else{
    $message="Mark_file Error: missing file name !";
    $mwnd->bell;
    return;
  }
}
#------------------------------------------------------------------------------
sub upd_db
{
#
#--- call DB-writer after ped_table files are selected[and checked] :
#
#
#---> check if "DBrewr"-mode:
#
  if($dryrunval==0){# real DB-update
    if($dbwrsel eq "RewrDB" && $updbtext eq "UpdDB"){
      show_warn("   ===> Your choise is to clean DB befor update, please 'Confirm' that! <===");
      $updbtext="Confirm";
      $mwnd->after(500);
      return;
    }
    elsif ($updbtext eq "Confirm"){
      $updbtext="UpdDB";
    }
  }
#
#---> check presence of selected("good"="processed") ped-files :
#
  my $ngelem=0;
  for($i=0;$i<$nelem;$i++){if($processed[$i] == 1){$ngelem+=1;}}
  if($ngelem==0){
    show_warn("   <-- Missing good ped-files !?");
    return;
  }
#
#---> read map-file and "day"-subdirectories names:
#
  @daysdlist=();#list of "day"-subdirectories
  @mapfcont=();# current map-file content
  my ($sizemap,$nrecmap);
  my ($begtm,$endtm,$tdvname,$instm,$instmm,$updflg);
  my ($pedfn,$pedfile,$mapdir,$mapfile);
  $mapdir=$AMSDD."/DataBase";
  $mapfile="pedmap.dat";
  my ($mapfn,$daysdir,$ndaysd);
  if($tofantsel eq "useTOF"){
    $daysdir=$AMSDD."/DataBase/Tofpeds";
    $mapfn=$AMSDD."/DataBase/.Tofpeds.1.map";
    $tdvname="Tofpeds";
    $len=7;#length of tdvname
  }
  else {
    $daysdir=$AMSDD."/DataBase/Antipeds";
    $mapfn=$AMSDD."/DataBase/.Antipeds.1.map";
    $tdvname="Antipeds";
    $len=8;
  }
#
#--- call of DBWriter just to update official map-file (dry-run):
#
  $dbwlfname="dbwlogf.log";#tempor DBwriter log-file (also copied into $logtext window !)
  $updflg=0;#dry run
  $begtm=1000000000;
  $endtm=1581120000;
#
  $curline="=====> Start DBwriter to update map-file (dry-run)\n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
  $mwnd->update;
# make def.ped-file from the 1st existing one (i need it now pure formally for offline-program)
  $pedfn=$pathpeds."/".$sfilelist[0];
  $pedfile=$detnam[0]."_df_rl.dat";#default input ped-file name required by offline-program
  copy($pedfn,$pedfile) or die show_warn("   <-- Copy failed for $pedfn (dry-run)!");#copy 
#
  open(DBWLOGF,"+> $dbwlfname") or die show_warn("   <-- Cannot open $dbwlfname (dry-run) $!");# clear if exists
  $status = system("$prog2run $tdvname $begtm $endtm $updflg >> $dbwlfname");#<-- call DBWriter 
  if($status != 0) {die  show_warn("   <-- $prog2run exited badly(dry-run) $!");}
  while(<DBWLOGF>){
    $curline=$_;
    $logtext->insert('end',$curline);
    $logtext->yview('end');
  }
  close(DBWLOGF) or die show_warn("   <-- Cannot close DBWLOGF (dry-run) !");
#
  $curline="<===== Finished DBWriter dry-run !\n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
  $mwnd->update;
#
#--- check the presence of map-file:
#
  my $mapfound=0;
  my $patt;
  my @mapflist=();
  opendir(DIR,$mapdir);#<--- get map-dir list
  $patt=".".$tdvname.".1.map";
  @mapflist=grep{/$patt/} readdir(DIR);
  $mapfound=scalar(@mapflist);
  closedir(DIR);
  if($mapfound==1){show_messg("   <-- Map-file found !");}
  elsif($mapfound==0){show_messg("   <-- Map-file was not found - DB is empty !");}
  else{
    show_warn("   <-- Error: multiple Map-files !");
    return;
  }
  $sizemap=0;
  $nrecmap=0;
#
#---> copy current map-file (if present) to local "pedmap.dat"-file:
#
  if($mapfound==1){
    copy($mapfn,$mapfile) or die show_warn("   <-- copy failed for $mapfn !!!");#copy  ped map-file
    open(MAPF,"< $mapfile") or die show_warn("   <-- Cannot open $mapfile for reading !");
    while(defined ($line = <MAPF>)){
      chomp $line;
      push(@mapfcont,$line);
    }
    close(MAPF) or die show_warn("   <-- Cannot close $mapfile after reading $!");
    $sizemap=scalar(@mapfcont);
    $nrecmap=$mapfcont[0];# records in the I(B,Bs) sections of map-file
    show_messg("   <-- Map-file $mapfn has $nrecmap records !");
    if($nrecmap>0){
    show_messg("       BeginTime-ordered Begin/End/Insert-times list is followed:\n");
      for($i=0;$i<$nrecmap;$i++){
        $begtm=$mapfcont[1+4*$nrecmap+$i];
	for($j=0;$j<$nrecmap;$j++){# find corresponding end/ins-times
	  if($begtm==$mapfcont[1+2*$nrecmap+$j]){
	    $endtm=$mapfcont[1+3*$nrecmap+$j];
	    $instm=$mapfcont[1+$j];
	  }
	}
        show_messg("          Beg/End/Ins :  $begtm  $endtm  $instm");
      }
      show_messg("---------------------------------------------\n\n");
      $mwnd->update;
    }
  }
  if($dryrunval==1){return;}
  if($nrecmap==0){goto UPDATE;}# empty(def) map-file is equivalent to "DB clean up" mode
#
#---> get list of "day"-directories:
  opendir(DIR,$daysdir);#<--- get day-dirs list
  @daysdlist=grep{/^\d+$/} readdir(DIR);# read only "number"-dirs
  $ndaysd=scalar(@daysdlist);
  closedir(DIR);
#
#---> delete TDV DB-records in requested dates range and remove original map-file
#
  my ($sdir,$fmt,$rest);
  my @dbrlist=();
  if($dbwrsel eq "RewrDB"){#<--- only for "clean up" mode
    $patt=$tdvname.".1.";
    for($i=0;$i<$ndaysd;$i++){#<-- day-directories loop
      if((($TIMEL-$daysdlist[$i])<=86400 && ($daysdlist[$i]-$TIMEH)<=86400)
                                         || ($TIMEL > $TIMEH)){;#need day-subdirs only in the range
        $sdir=$daysdir."/".$daysdlist[$i];
        opendir(DDIR,$sdir);#open sub-dir of days-dir
	@dbrnlist=grep{/$patt/} readdir(DDIR);# select real_data ped-record names
	foreach $dbrn(@dbrnlist) {#<--- records loop
	  $fmt="A".$len;
	  ($rest,$instm)=unpack("$fmt x3 A*",$dbrn);#get insert-time of db-record from his name
	  for($im=0;$im<$nrecmap;$im++){#<--- map-file content loop
	    $instmm=$mapfcont[1+$im];#instime from map-file
	    if($instmm>0 && $instmm==$instm){# check map<->dir instime match 
	      $mapfcont[1+$im]=0;#clear map entry 
              $begtm=$mapfcont[1+2*$nrecmap+$im];
	      if(($begtm>=$TIMEL && $begtm<=$TIMEH) || ($TIMEL > $TIMEH)){#delete file because it in time-range
		unlink($sdir."/".$dbrn) or die show_warn("   <-- Cannot delete TDV-record $dbrn !");
		last;#skip the rest of map-file
	      }
	    }#--->endof names-match check
	  }#--->endof map-file loop
	}#--->endof records loop
	close(DDIR);
      }#--->endof "day-subdir in the range" check
    }#--->endof day-dirs loop
    unlink($mapfn) or die show_warn("   <-- Cannot delete map-file $mapfn !");#delete original map-file
  }#--->endof DB clean up request
#---------
# 
UPDATE:
#
  $percent=0;
  $perc_done=0;
#
  $curline="===========> Start Run $RunNum for real DB-update...\n\n";
  $logtext->insert('end',$curline);
  $logtext->yview('end');
  $mwnd->update;
#
#---> write selected ped-files into DB:
# 
  open(DBWLOGF,"+> $dbwlfname") or die show_warn("   <-- Cannot open $dbwlfname  !");# clear file if exists
  for($i=0;$i<$nelem;$i++){#<--- selected files loop 
    if($processed[$i] == 1){
      system("date >> $dbwlfname");
      $pedfn=$pathpeds."/".$sfilelist[$i];
      $pedfile=$detnam[$i]."_df_rl.dat";#default input ped-file name required by offline-program
      copy($pedfn,$pedfile) or die show_warn("   <-- Copy failed for $pedfn $!");#copy peds into "***p_df_rl.dat"
      $begtm=$runs[$i];
      $endtm=0;
      if(($dbwrsel eq "RewrDB") || ($nrecmap == 0) ){#<--- "DB clean up" mode
        $endtm=1581120000;# till year 2020
      }
      else{#<-- "insert"-mode
        if($begtm < $mapfcont[1+4*$nrecmap]){
          $endtm = $mapfcont[1+4*$nrecmap];
        }
        elsif($begtm >= $mapfcont[5*$nrecmap]){
          $endtm=1581120000;# till year 2020
        }
        else {
          for($im=0;$im<$nrecmap;$im++){# loop over 5th group(ordered "begins")
            if(($begtm >= $mapfcont[1+4*$nrecmap+$im]) && ($begtm < $mapfcont[1+4*$nrecmap+$im+1])){
	      $endtm=$mapfcont[1+4*$nrecmap+$im+1]-1;
	      last;
	    }
	  }
        }
      }#-->endof "insert"-mode
      $updflg=1; 
      $status = system("$prog2run $tdvname $begtm $endtm $updflg >> $dbwlfname");#<-- call DBWriter 
      if($status != 0) {die  show_warn("   <-- $prog2run exited badly $!");}
      while(<DBWLOGF>){
        $curline=$_;
        $logtext->insert('end',$curline);
        $logtext->yview('end');
      }
#
      $curline="<======================== File $sfilelist[$i] is written to DB !\n\n";
      $logtext->insert('end',$curline);
      $logtext->yview('end');
    }#--->endof "selected-file" check
    $percent+=1;
    $perc_done=100*($percent/$nelem);
#
    $mwnd->update;
  }#--->endof ped-files loop
  close(DBWLOGF) || die show_warn("   <-- Cannot close DBWLOGF !");
}
#------------------------------------------------------------------------------
sub exit_actions
{
#---> archive processed files if requested:
#
  my ($curfn,$newfn);
  if($archval==1){
    show_messg("   <-- Archiving of processed ped-files...");
    for($i=0;$i<$nelem;$i++){#<--- processed files loop 
      if($processed[$i] == 1){
        $curfn=$pathpeds."/".$sfilelist[$i];
        $newfn=$patharch."/".$sfilelist[$i];
        move($curfn,$newfn) or show_warn("   <-- Archiving failed for $curfn $!");# move to archive
      }
    }
    show_messg("   <-- Archiving is completed !");
  }
#
#---> save log-file (use run# of the 1st processed file as time-stamp of log-file):
#
  my $firstr=0;
  my $logfname="logfile";
  my $filen;
  for($i=0;$i<$nelem;$i++){#<--- selected files loop 
    if($processed[$i] == 1){
      $firstr=$runs[$i];
      last;
    }
  }
  if($firstr>0){
    $filen=$logfname.".".$firstr.".txt";
    open(OFN, "> $filen") or die show_warn("   <-- Cannot open $filen for writing !");
    print OFN $logtext->get("1.0","end");
    show_warn("   <-- LogFile $filen is saved, Bye-Bye !");
  }
  $mwnd->update;
  if($soundtext eq "Sound-ON"){$mwnd->bell;}
  $mwnd->after(3000);
  exit;
}
#--------




