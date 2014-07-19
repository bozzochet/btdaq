drop table m_active_hosts;
create table m_active_hosts
    ( 
      type       number(6,0),
      id         number(10,0),
      interface  number(6,0),
      status     number(6,0),
      active     number(6,0),
      allowed    number(6,0),
      processed  number(20,0),
      failed     number(20,0),
      killed     number(20,0),
      lastupdate number(24,0),
      clock      number(6,0),
      timestamp  number(24,0) 
    );
