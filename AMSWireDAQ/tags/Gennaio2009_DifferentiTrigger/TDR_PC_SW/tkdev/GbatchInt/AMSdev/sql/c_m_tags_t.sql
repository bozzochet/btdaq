drop table m_tags;
create table m_tags
    ( run        NUMBER(16,0)      not null,
      event      number(8,0)       not null,
      offset     number(12,0),
      recostatus number(10,0)
     )
      PCTFREE 1 PCTUSED 99
      STORAGE (INITIAL 100M NEXT 100M PCTINCREASE 0 )
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





