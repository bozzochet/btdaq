#  $Id: monitorHTML.pm,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
package monitorHTML;
use Error qw(:try);
use CGI qw(-unique_headers);;
use Carp;
use strict;
use lib::Warning;
use lib::Monitor;
@monitorHTML::EXPORT =qw(new print_table);

sub new{
my %fields=(
            q=>undef,
            read=>0,
            PAH=>undef,
            PAC=>undef,
            Runs=>undef,
            Ntuples=>undef,
            CAC=>undef,
            DB=>undef,
            rc=>0,
            Control=>undef,
            Name=>"/cgi-bin/mon/monitor.cgi",
            );
   if($ENV{MOD_PERL} and ref($monitorHTML::Singleton)){
         return $monitorHTML::Singleton;
    }

    my $type=shift;
    my $self={
        %fields,
    };

foreach my $chop  (@ARGV){
    if($chop =~/^-N/){
       $self->{Name}= unpack("x2 A*",$chop);
    }
    if($chop =~/^-m/){
#        $self->{Name}="/cgi-bin/mon/monmc.cgi";
     if ($Monitor::Singleton->{updatesviadb}){
#        $self->{Name}="/cgi-bin/mon/monmcdb.cgi";
        $self->{rc}=1;
     }
    }
}
#warn $self->{Name};
    $self->{q}=new CGI;

#open FILE,">/tmp/query" or die $!;
#$self->{q}->save(\*FILE);
#print $self->{q}->header("text/plain"),"File Saved\n";

    if ($self->{q}->param("Monitor")){
        $self->{read}=1;
        foreach my $value ($self->{q}->param("Objects2Monitor")){
            $self->{$value}=1;
        }
        
    }elsif($self->{q}->param("Control")){
        $self->{read}=2;
        my $pass=$self->{q}->param("password");
        my $crypt=crypt($pass,"ams");
        if($crypt ne "amGzkSRlnSMUU"){
                  Warning::error($self->{q},
                  "User authorization failed");
                   return;

                }
        $self->{Control} =$self->{q}->param("Objects2Control");
    }
    else{
            if(defined $self->{q}->param("TOTAL")){
                my $total=$self->{q}->param("TOTAL");
                my $name=$self->{q}->param("NAME");
                for my $i (0...$total){
                 my $string="MyControl$i";
                 my $action=$self->{q}->param($string);
                 if(defined $action){
                     my @data;
                     foreach my $value ($self->{q}->param("noname")){
                         push @data,$value;
                     }
                     my ($monitor, $ok)=Monitor::Update();
                     if(not $ok){
                  Warning::error($self->{q},
                  "Unable to Connect to Servers");
                         return;
                     }
                     Monitor::sendback($name,$action,$i,@data);
                     goto NEXT;
                 }
             }
                if(not defined $name){
                    $name="Undefined";
                }
                  Warning::error($self->{q},
                  "Unable To Send Data for $name");
                   return;
NEXT:
            }
        $self->{read}=0;
    }
    my $mybless=bless $self,$type;
    if(ref($monitorHTML::Singleton)){
        croak "Only Single monitorHTML Allowed\n";
    }
    $monitorHTML::Singleton=$mybless;
return $mybless;
}
sub Warning{

    Warning::error($monitorHTML::Singleton->{q},"Unable To Connect The Servers");

}

sub UpdateForce{
    my $ref=$monitorHTML::Singleton;
    my ($monitor, $ok)=Monitor::UpdateForce();
}

sub Update{
    my $ref=$monitorHTML::Singleton;
    if($ref->{read}){
     my ($monitor, $ok)=Monitor::Update();
     if(not $ok){
         Warning();
         return;
     }
 }
    my $q=$monitorHTML::Singleton->{q};
    if($ref->{rc}){

    print $q->header( "text/html" ),
    $q->start_html( "Connected To Servers Succesfully" );

    print $q->h1( "Connected To Servers Succesfully" );
    if($ref->{read}==0){
        print $q->start_form(-method=>"GET", 
          -action=>$monitorHTML::Singleton->{Name});
        print $q->p ("Monitor:");
   print qq`
<INPUT TYPE="checkbox" NAME="Objects2Monitor" VALUE="PAH" CHECKED>Local Producer Active Hosts<BR>
<INPUT TYPE="checkbox" NAME="Objects2Monitor" VALUE="PAC" CHECKED>Producer Active Clients<BR>
<INPUT TYPE="checkbox" NAME="Objects2Monitor" VALUE="Runs" CHECKED>Runs Table<BR>
<INPUT TYPE="checkbox" NAME="Objects2Monitor" VALUE="Ntuples" >Ntuples Table<BR>
<INPUT TYPE="checkbox" NAME="Objects2Monitor" VALUE="CAC" CHECKED>Server Active Clients<BR>
<INPUT TYPE="checkbox" NAME="Objects2Monitor" VALUE="DBAC" CHECKED>DBServer Active Clients<BR>
<INPUT TYPE="checkbox" NAME="Objects2Monitor" VALUE="DB" CHECKED>Disk Space<BR>
`;
        print $q->submit(-value=>"SubmitRequest", -name=>"Monitor");
        print $q->end_form();
        print $q->start_form(-method=>"GET", 
          -action=>$monitorHTML::Singleton->{Name});
        print $q->end_form();
          
    }elsif($ref->{read} == 1){

    my @titles;
    $#titles=-1;
            @titles = (
	    "HostName",
	    "Running Pr",
	    "Allowed Pr",
	    "Failed Pr",
	    " Ntuples",
            "EventTags ",
	    " Events ",
	    "  % of Total",
	    " Warnings  ",
	    " Errors  ",
	    " CPU/Event ",
	    " Efficiency",
	    "Status ",
	);

    if($ref->{PAH}){
     my @output=Monitor::getactivehosts();
     my $buffer="Producer_ActiveHosts";

     print_table($q,$buffer,$#titles,@titles,@output);
    
    }


     $#titles=-1;
                        @titles = (
            "ID",
            "HostName",
            "Mips",
            "Process ID",
            "Start Time",
            "LastUpdate Time",
            "TimeOut",
            "Run",
            "Name",
            "Status",
                                   );

    if($ref->{PAC}){
     my $buffer="Producer_ActiveClients";
     my @output=Monitor::getactiveclients("Producer");
     print_table($q,$buffer,$#titles,@titles,@output);
    }

      




     $#titles=-1;
                        @titles = (
            "Run",
            "Time",
            "First Event",
            "Last Event",
	    "Name",
            "CRC",
            "Size(MB)",
            "Status ",
                                   );

    if($ref->{Ntuples}){
     my $buffer="Producer_Ntuples";
     my @output=Monitor::getntuples();
     print_table($q,$buffer,$#titles,@titles,@output);
 }

     $#titles=-1;
                        @titles = (
            "ID",
            "HostName",
             "Mips",
            "Process ID",
            "Start Time",
            "LastUpdate Time",
            "TimeOut",
            "Status",
                                   );
 
    if($ref->{CAC}){

     my $buffer="Server_ActiveClients";

    my @output=Monitor::getactiveclients("Server");
     print_table($q,$buffer,$#titles,@titles,@output);
 }
     $#titles=-1;
                        @titles = (
            "ID",
            "HostName",
            "Mips",
            "Process ID",
            "Start Time",
            "LastUpdate Time",
            "TimeOut",
            "Status",
                                   );
 
    if($ref->{DBAC}){

     my $buffer="DBServer_ActiveClients";

    my @output=Monitor::getactiveclients("DBServer");
     print_table($q,$buffer,$#titles,@titles,@output);
 }
        @titles = (
            "FileSystem",
            "Total (Mbytes)",
            "Free (Mbytes)",
            "% Free",
#            "OK",
                   );
    if($ref->{DB}){
     my @output=Monitor::getdbok();
     my $buffer="DiskUsage";
     print_table($q,$buffer,$#titles,@titles,@output);
 }


     $#titles=-1;
                        @titles = (
            "Run",
            "SubmitTime",
            "First Event",
            "Last Event",
            "Priority ",
            "History ",
            "Fails ",
            "Status ",
                                   );

    if($ref->{Runs}){
     my $buffer="Producer_Runs";
     my @output=Monitor::getruns();
     print_table($q,$buffer,$#titles,@titles,@output);
   }

}elsif($ref->{read}==2){

    my $name=$ref->{Control};
    my @output=Monitor::getcontrolthings($name);
    my @titles;    
    if( $name eq "ServerClient"){
        $#titles=-1;
        @titles=(
          "Uid",
          "Type",
          "MaxClients",
          "CPU",
           "Memory",
             "ScriptPath",
            "LogPath",
            "Submit",
            "HostName",
           "LogInTheEnd",
                 );
    }elsif( $name eq "ProducerClient"){
        $#titles=-1;
        @titles=(
          "Uid",
          "Type",
          "MaxClients",
          "CPU",
           "Memory",
             "ScriptPath",
            "LogPath",
            "Submit",
            "HostName",
           "LogInTheEnd",
                 );
    }elsif( $name eq "ProducerActiveClient"){
        $#titles=-1;
        @titles=(
          "Uid",
          "HostName",
          "pid",
          "Status",
          "Type",
          "TimeOut",
                 );
    }elsif( $name eq "ServerHost"){
        $#titles=-1;
        @titles=(
"HostName",
"Interface",
"OS",
"CPUNumber",
"Memory",
"Clock",
"Clients Allowed",
"Status",
                 );
    }elsif( $name eq "ProducerHost"){
        $#titles=-1;
        @titles=(
"HostName",
"Interface",
"OS",
"CPUNumber",
"Memory",
"Clock",
"Clients Allowed",
"Status",
                 );
}elsif( $name eq "PNtuple"){
    $#titles=-1;
        @titles=(
         "Run",
        "Time",
        "FirstEvent",
        "LastEvent",
        "Name",
        "Size",
        "Status",
        "Type",
                 );
}elsif( $name eq "Ntuple"){
    $#titles=-1;
        @titles=(
         "Uid",
        "HostName",
        "OutputDirPath",
        "RunMode",
        "UpdateFreq",
        "DSTType",
                 );
}elsif( $name eq "Run"){
    $#titles=-1;
        @titles=(
"Uid",
"Run",
"FirstEvent",
"LastEvent",
"Priority",
"FilePath",
"Status",
"History",
"Fails",
                 );
}elsif( $name eq "Killer"){
    $#titles=-1;
        @titles=(
          "Uid",
          "Type",
          "MaxClients",
          "CPU",
           "Memory",
             "ScriptPath",
            "LogPath",
            "Submit",
            "HostName",
           "LogInTheEnd",
                 );
}elsif( $name eq "setEnv"){
    $#titles=-1;
        @titles=(
          "Environment Var",
          "Path",
                 );
}
    for my $i (0 ... $#output){
        print $q->start_form(-method=>"GET", 
          -action=>$monitorHTML::Singleton->{Name});
        my @text=@{$output[$i]};
        my $string="";
     for my $j (0...$#titles){

         print $titles[$j];
         print $q->textfield(-name=>"noname",-default=>"$text[$j]");
         print "<BR>";
         
     }
        print $q->submit(-name=>"MyControl$i", -value=>"Create");
        print $q->submit(-name=>"MyControl$i", -value=>"Update");
        print $q->submit(-name=>"MyControl$i", -value=>"Delete");
        print qq`
        <INPUT TYPE="hidden" NAME="TOTAL" VALUE=$#output>
        <INPUT TYPE="hidden" NAME="NAME" VALUE=$name>
        `;        
         print $q->end_form;
    }

}

      print $q->end_html;



    }
    else{
    print $q->header( "text/html" ),
    $q->start_html( "Connected To Servers Succesfully" );

    print $q->h1( "Connected To Servers Succesfully" );
    if($ref->{read}==0){
        print $q->start_form(-method=>"GET", 
          -action=>$monitorHTML::Singleton->{Name});
        print $q->p ("Monitor:");
   print qq`
<INPUT TYPE="checkbox" NAME="Objects2Monitor" VALUE="PAH" CHECKED>Producer Active Hosts<BR>
<INPUT TYPE="checkbox" NAME="Objects2Monitor" VALUE="PAC" CHECKED>Producer Active Clients<BR>
<INPUT TYPE="checkbox" NAME="Objects2Monitor" VALUE="Runs">Runs Table<BR>
<INPUT TYPE="checkbox" NAME="Objects2Monitor" VALUE="Ntuples" >Ntuples Table<BR>
<INPUT TYPE="checkbox" NAME="Objects2Monitor" VALUE="CAC" CHECKED>Server Active Clients<BR>
<INPUT TYPE="checkbox" NAME="Objects2Monitor" VALUE="DBAC" CHECKED>DBServer Active Clients<BR>
<INPUT TYPE="checkbox" NAME="Objects2Monitor" VALUE="DB" CHECKED>Disk Space<BR>
`;
        print $q->submit(-value=>"SubmitRequest", -name=>"Monitor");
        print $q->end_form();
        print $q->start_form(-method=>"GET", 
          -action=>$monitorHTML::Singleton->{Name});
        print $q->p ("Control:");
   print qq`
<INPUT TYPE="radio" NAME="Objects2Control" VALUE="ProducerActiveClient" CHECKED>Producer Active Client<BR>
<INPUT TYPE="radio" NAME="Objects2Control" VALUE="ProducerClient" >Producer Nominal Client<BR>
<INPUT TYPE="radio" NAME="Objects2Control" VALUE="ProducerHost" >Producer Nominal Host<BR>
<INPUT TYPE="radio" NAME="Objects2Control" VALUE="Ntuple" >Producer Nominal Ntuple<BR>
<INPUT TYPE="radio" NAME="Objects2Control" VALUE="PNtuple" >Producer Produced Ntuple<BR>
<INPUT TYPE="radio" NAME="Objects2Control" VALUE="Run" >Producer Run Table<BR>
<INPUT TYPE="radio" NAME="Objects2Control" VALUE="ServerClient" >Server Nominal Client<BR>
<INPUT TYPE="radio" NAME="Objects2Control" VALUE="ServerHost" >Server Nominal Host<BR>
<INPUT TYPE="radio" NAME="Objects2Control" VALUE="Killer" >Server Nominal Killer<BR>
<INPUT TYPE="radio" NAME="Objects2Control" VALUE="setEnv" >Server Environment Vars<BR>
Password: <INPUT TYPE="password" NAME="password" VALUE="" ><BR>
`;
        print $q->submit(-name=>"Control", -value=>"SubmitRequest");
        print $q->end_form();
          
    }elsif($ref->{read} == 1){

    my @titles;
    $#titles=-1;
            @titles = (
	    "HostName",
	    "Running Pr",
	    "Allowed Pr",
	    "Failed Pr",
	    " Ntuples",
            "EventTags ",
	    " Events ",
	    "  % of Total",
	    " Warnings  ",
	    " Errors  ",
	    " CPU/Event ",
	    " Efficiency",
	    "Status ",
	);

    if($ref->{PAH}){
     my @output=Monitor::getactivehosts();
     my $buffer="Producer_ActiveHosts";

     print_table($q,$buffer,$#titles,@titles,@output);
    
    }


     $#titles=-1;
                        @titles = (
            "ID",
            "HostName",
            "Mips",
            "Process ID",
            "Start Time",
            "LastUpdate Time",
            "TimeOut",
            "Run",
            "Name",
            "Status",
                                   );

    if($ref->{PAC}){
     my $buffer="Producer_ActiveClients";
     my @output=Monitor::getactiveclients("Producer");
     print_table($q,$buffer,$#titles,@titles,@output);
    }

      




     $#titles=-1;
                        @titles = (
            "Run",
            "Time",
            "First Event",
            "Last Event",
	    "Name",
            "Size(MB)",
            "Status ",
                                   );

    if($ref->{Ntuples}){
     my $buffer="Producer_Ntuples";
     my @output=Monitor::getntuples();
     print_table($q,$buffer,$#titles,@titles,@output);
 }

     $#titles=-1;
                        @titles = (
            "ID",
            "HostName",
            "Mips",
            "Process ID",
            "Start Time",
            "LastUpdate Time",
            "TimeOut",
            "Status",
                                   );
 
    if($ref->{CAC}){

     my $buffer="Server_ActiveClients";

    my @output=Monitor::getactiveclients("Server");
     print_table($q,$buffer,$#titles,@titles,@output);
 }
     $#titles=-1;
                        @titles = (
            "ID",
            "HostName",
            "Mips",
            "Process ID",
            "Start Time",
            "LastUpdate Time",
            "TimeOut",
            "Status",
                                   );
 
    if($ref->{DBAC}){

     my $buffer="DBServer_ActiveClients";

    my @output=Monitor::getactiveclients("DBServer");
     print_table($q,$buffer,$#titles,@titles,@output);
 }
        @titles = (
            "FileSystem",
            "Total (Mbytes)",
            "Free (Mbytes)",
            "% Free",
#            "OK",
                   );
    if($ref->{DB}){
     my @output=Monitor::getdbok();
     my $buffer="DiskUsage";
     print_table($q,$buffer,$#titles,@titles,@output);
 }


     $#titles=-1;
                        @titles = (
            "Run",
            "Time",
            "First Event",
            "Last Event",
            "Priority ",
            "History ",
            "Status ",
                                   );

    if($ref->{Runs}){
     my $buffer="Producer_Runs";
     my @output=Monitor::getruns();
     print_table($q,$buffer,$#titles,@titles,@output);
   }

}elsif($ref->{read}==2){

    my $name=$ref->{Control};
    my @output=Monitor::getcontrolthings($name);
    my @titles;    
    if( $name eq "ServerClient"){
        $#titles=-1;
        @titles=(
          "Uid",
          "Type",
          "MaxClients",
          "CPU",
           "Memory",
             "ScriptPath",
            "LogPath",
            "Submit",
            "HostName",
           "LogInTheEnd",
                 );
    }elsif( $name eq "ProducerClient"){
        $#titles=-1;
        @titles=(
          "Uid",
          "Type",
          "MaxClients",
          "CPU",
           "Memory",
             "ScriptPath",
            "LogPath",
            "Submit",
            "HostName",
           "LogInTheEnd",
                 );
    }elsif( $name eq "ProducerActiveClient"){
        $#titles=-1;
        @titles=(
          "Uid",
          "HostName",
          "pid",
          "Status",
          "Type",
          "TimeOut",
                 );
    }elsif( $name eq "ServerHost"){
        $#titles=-1;
        @titles=(
"HostName",
"Interface",
"OS",
"CPUNumber",
"Memory",
"Clock",
"Clients Allowed",
"Status",
                 );
    }elsif( $name eq "ProducerHost"){
        $#titles=-1;
        @titles=(
"HostName",
"Interface",
"OS",
"CPUNumber",
"Memory",
"Clock",
"Clients Allowed",
"Status",
                 );
}elsif( $name eq "PNtuple"){
    $#titles=-1;
        @titles=(
         "Run",
        "Time",
        "FirstEvent",
        "LastEvent",
        "Name",
        "Size",
        "Status",
        "Type",
                 );
}elsif( $name eq "Ntuple"){
    $#titles=-1;
        @titles=(
         "Uid",
        "HostName",
        "OutputDirPath",
        "RunMode",
        "UpdateFreq",
        "DSTType",
                 );
}elsif( $name eq "Run"){
    $#titles=-1;
        @titles=(
"Uid",
"Run",
"FirstEvent",
"LastEvent",
"Priority",
"FilePath",
"Status",
"History",
                 );
}elsif( $name eq "Killer"){
    $#titles=-1;
        @titles=(
          "Uid",
          "Type",
          "MaxClients",
          "CPU",
           "Memory",
             "ScriptPath",
            "LogPath",
            "Submit",
            "HostName",
           "LogInTheEnd",
                 );
}elsif( $name eq "setEnv"){
    $#titles=-1;
        @titles=(
          "Environment Var",
          "Path",
                 );
}
    for my $i (0 ... $#output){
        print $q->start_form(-method=>"GET", 
          -action=>$monitorHTML::Singleton->{Name});
        my @text=@{$output[$i]};
        my $string="";
     for my $j (0...$#titles){

         print $titles[$j];
         print $q->textfield(-name=>"noname",-default=>"$text[$j]");
         print "<BR>";
         
     }
        print $q->submit(-name=>"MyControl$i", -value=>"Create");
        print $q->submit(-name=>"MyControl$i", -value=>"Update");
        print $q->submit(-name=>"MyControl$i", -value=>"Delete");
        print qq`
        <INPUT TYPE="hidden" NAME="TOTAL" VALUE=$#output>
        <INPUT TYPE="hidden" NAME="NAME" VALUE=$name>
        `;        
         print $q->end_form;
    }

}

      print $q->end_html;
}
}

sub print_table{
    my $red="<font color=red>";
    my $green="<font color=green>";
    my $yellow="<font color=yellow>";
    my @color=($green,$yellow,$red);
    my ($q,$name,$mt,@output) = @_;
    print $q->h2($name);
     print "<TABLE BORDER=1>\n<TR>";
     for my $i (0 ... $mt){
         print "<TH>$output[$i]</TH>\n";
     }
     print  "</TR>\n";

     for my $i ($mt+1 ... $#output){
         my @text=@{$output[$i]};
         my $string="<TR>";
         for my $j (0 ...$#text-1){
          my $alig="";
          if($j>0){
            $alig="align=right";
          }
          $string=$string."<TD $alig>$color[$text[$#text]]$text[$j]</TD>";
         }         
         $string=$string."</TR>\n";
         print $string;
     }
     print $q->end_table;
}



