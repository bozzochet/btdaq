drop table m_dst;
create table m_dst
    ( 
      tinsert    number(24,0),
      tbegin     number(24,0),
      tend       number(24,0),
      run        number(24,0),
      firstevent number(24,0),
      lastevent  number(24,0),
      events     number(20,0),
      status     number(6,0),
      type       number(6,0),
      filesize   number(24,0),
      name       varchar(1024),
      timestamp  number(24,0) 
    );
commit;
