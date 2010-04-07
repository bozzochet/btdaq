drop table m_ntuples;
create table m_ntuples
    ( 
      dataset     VARCHAR2(255) NOT NULL,
      run         NUMBER(12)  NOT NULL,
      ntupletype  VARCHAR2(255),
      filepath    VARCHAR2(255) NOT NULL,
      timestamp   DATE,
      timef       DATE,
      timel       DATE,
      version     NUMBER(6),
      mask        NUMBER(6),
      comments    VARCHAR2(255)
)
      STORAGE (INITIAL 40K NEXT 10K PCTINCREASE 0)
      nologging
      PARTITION BY RANGE (RUN)
      (PARTITION  STS91  VALUES LESS THAN (897688800),
       PARTITION  GSI98  VALUES LESS THAN (908000000),
       PARTITION  CERN98 VALUES LESS THAN (914046516)
     )
;
