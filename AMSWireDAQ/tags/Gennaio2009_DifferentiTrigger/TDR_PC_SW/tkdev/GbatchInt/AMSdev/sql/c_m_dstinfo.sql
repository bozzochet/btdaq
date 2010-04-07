drop table m_dstinfo;
create table m_dstinfo
    ( 
      type       number(6,0),
      id         number(24,0),
      hostid     number(5,0),
      dirpath    varchar(1024),
      runmode    number(5,0),
      updfreq    number(20,0),
      diehard    number(20,0),
      freespace  number,
      totalspace number,
      timestamp  number(24,0)
    );
commit;
