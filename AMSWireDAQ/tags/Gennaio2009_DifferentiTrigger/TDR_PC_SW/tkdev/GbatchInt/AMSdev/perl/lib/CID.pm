#  $Id: CID.pm,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
package CID;
@CID::EXPORT = qw(new);
sub new{
    my $class=shift;
    my $type=shift;
    my $self={
      HostName=>undef,
      uid=>0,
      pid=>$$,
      ppid=>0,
      Mips=>1000,
      Type=>"Monitor",
      Status=>"NOP",
      StatusType=>"Permanent",
      Interface=>"default",
    };
    if (defined $type){
        $self->{Type}=$type;
    }
    use Sys::Hostname;
    $self->{HostName}=hostname();
    return bless $self,$class;
}


