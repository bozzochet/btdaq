drop table m_runs_d;
create table m_runs_d
    ( ndataset      NUMBER(8,0)  not null,
      run           NUMBER(24,0) not null,
      runtype       VARCHAR(255),
      daqtag        VARCHAR(7),
      events        NUMBER(12,0),
      particle      VARCHAR(10),
      energymin     NUMBER(8,1),
      energymax     NUMBER(8,1), 
      spectrum      VARCHAR(255),
      trigmask      VARCHAR(255),
      position      VARCHAR(80),
      phi           NUMBER(5,2),
      theta         NUMBER(5,2),
      x             NUMBER(6,2),
      y             NUMBER(6,2),
      z             NUMBER(6,2),
      timef         DATE NOT NULL,
      timel         DATE NOT NULL,
      utimef        NUMBER(10,0),
      utimel        NUMBER(10,0),
      cdt           VARCHAR(4),
      rerunflag     NUMBER(4,0),
      comments      VARCHAR(255),
      errors        VARCHAR(255)) 
      STORAGE (INITIAL 40K NEXT 10K PCTINCREASE 0)
      nologging
      PARTITION BY RANGE (RUN)
      (PARTITION  STS91  VALUES LESS THAN (897688800),
       PARTITION  GSI98  VALUES LESS THAN (908000000),
       PARTITION  CERN98 VALUES LESS THAN (914046516)
     )
;


