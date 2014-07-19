#!/usr/local/bin/perl -w
$FILEO=">/f0dat1/AMSDataDir/DataManagement/prod/RunTable";
    open FILEO or die "ups\n";
print FILEO "# Run Ev_first Ev_last Time_Ev_first Time_Ev_last Prio  Path\n";
while ($line=<@ARGV>){
    @runs=split '-',$line;
    $FILEI="</f2users/alexei/STS91/offruns/logs/sts91.runs";
    open FILEI or die "ups\n";
LINE: while($line=<FILEI>){
    next LINE if $line =~ /^#/;
    next LINE if $line =~/laser/;
    @args=split ' ',$line;
    if ($args[0] >= $runs[0] && $args[0] <= $runs[1]){
        $count=$args[2];
        $xmax=26214400;
        while($args[1]<$count){
        if($count-$args[1]>$xmax+$xmax/2){
            $args[2]=$args[1]+$xmax;
        }
        else{
            $args[2]=$count;
        }
         foreach $args (@args){
             print FILEO "$args ";
         } 
         print FILEO "\n";
            $args[1]=$args[2]+1;
    }
    }
}
    close FILEI;
}


