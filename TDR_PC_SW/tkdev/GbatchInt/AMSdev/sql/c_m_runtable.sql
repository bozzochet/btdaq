drop table m_runtable;
create table m_runtable
    ( run         number(24,0)     NOT NULL,
      id          number(24,0)     NOT NULL, 
      fevent      number(20,0),
      levent      number(20,0),
      utimef      number(24,0),
      utimel      number(24,0),
      priority    number(24,0),
      rawfileapth varchar(1024),
      status      number(10,0),
      history     number(10,0),
      submittime  number(24,0),
      cuid        number(12,0),
      timestamp   number(24,0)
   );
