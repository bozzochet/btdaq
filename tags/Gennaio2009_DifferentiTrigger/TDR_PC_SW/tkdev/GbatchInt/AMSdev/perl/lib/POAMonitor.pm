#  $Id: POAMonitor.pm,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
package POAMonitor;
use Error qw(:try);
use lib::monitorUI;
use Gtk;
use strict;
@POAMonitor::ISA = qw(POA_DPS::Monitor);
@POAMonitor::EXPORT =qw(new);
sub new{
    my $type=shift;
    my $self={
    };
    return bless $self, $type;
}


sub MonInfo{
    my ($class,$cid,$message,$error,$timeout)=@_;
    if(ref($monitorUI::Singleton)){
     monitorUI::MonInfo($message,$error,$timeout);     
        while (Gtk->events_pending()){
             Gtk->main_iteration();
         }
    }
    else{
    }    
}
sub  MonDialog{
    my ($class,$cid,$message,$error,$timeout)=@_;
    if(ref($monitorUI::Singleton)){
        $monitorUI::Singleton->{queryanswer}=undef;
       monitorUI::MonDialog($message,$error,$timeout);     
        while (not defined $monitorUI::Singleton->{queryanswer}){
        while (Gtk->events_pending()){
             Gtk->main_iteration();
         }
#            sleep 1;
    }
        return $monitorUI::Singleton->{queryanswer};
    }
    else{
    }    
    return 1;
}

sub ping{
}

package NominalHost;
package NominalClient;
package ActiveHost;

