drop table m_tags;
create table m_tags
    ( run        NUMBER(16,0)      not null,
      event      number(8,0)       not null,
      recostatus number(10,0),
      level3     number(2,0),
      prescaled  char(1),
      charge     char(1),
      momentum_sign   char(1),
      velocity_sign   char(1),
      tracker_pattern number(2,0),
      beta_pattern    char(1),
      ctc_layer       char(1),
      anti            char(1),
      particle        char(1),
      rigidity        char(1),
      track           char(1),
      latt            char(1),
      localdb         char(1),
      errbit          char(1),
      nostatus        char(1)
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





