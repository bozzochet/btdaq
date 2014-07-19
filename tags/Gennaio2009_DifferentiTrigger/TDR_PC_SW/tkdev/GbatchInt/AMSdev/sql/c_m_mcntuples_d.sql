drop table m_mcntuples_d;
create table m_mcntuples_d
    ( 
      ndataset    NUMBER(5,0),
      run         NUMBER(12,0)  NOT NULL,
      ntupletype  VARCHAR(255)  NOT NULL,
      filepath    VARCHAR(255)  NOT NULL,
      timestamp   DATE,
      version     NUMBER(6,0),
      mask        NUMBER(6,0),
      comments    VARCHAR(255)
     )
      STORAGE (INITIAL 40K NEXT 10K PCTINCREASE 0)
      nologging
      PARTITION BY RANGE (timestamp)
      (PARTITION  Y1998  VALUES LESS THAN 
                  ('01-JAN-1999'),
       PARTITION  Y1999  VALUES LESS THAN 
                  ('01-JAN-2000'),
       PARTITION  Y2000  VALUES LESS THAN 
                  ('01-JAN-2001')
     )
;
