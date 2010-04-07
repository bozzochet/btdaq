drop table m_prodinfo;
create table m_prodinfo
    ( run             number(24,0)     NOT NULL,
      idx             number(24,0),
      events          number(24,0),
      lastevent       number(24,0),
      criticalerrors  number(10,0),
      errors          number(10,0),
      cputime         number(20,4),
      elapsedtime     number(20,4),
      status          number(5,0),
      hostid          number(5,0),
      timestamp       number(24,0)
    )
      PCTFREE 1 PCTUSED 99
      STORAGE (INITIAL 100M NEXT 100M PCTINCREASE 0 )
      nologging
      PARTITION BY RANGE (RUN) 
       (PARTITION day_0   VALUES LESS THAN (896824800),
        PARTITION day_1   VALUES LESS THAN (896911200),
        PARTITION day_2   VALUES LESS THAN (896997600),
        PARTITION day_3   VALUES LESS THAN (897084000),
        PARTITION day_4   VALUES LESS THAN (897170400),
        PARTITION day_5   VALUES LESS THAN (897256800),
        PARTITION day_6   VALUES LESS THAN (897343200),
        PARTITION day_7   VALUES LESS THAN (897429600),
        PARTITION day_8   VALUES LESS THAN (897516000),
        PARTITION day_9   VALUES LESS THAN (897602400),
        PARTITION day_10  VALUES LESS THAN (897688800)
     );
