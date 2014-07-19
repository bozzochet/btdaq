drop table m_runs;
create table m_runs
    ( dataset       VARCHAR(10) not null,
      run           NUMBER      not null,
      tag           NUMBER(12),
      runtype       NUMBER(3),
      runstype      NUMBER(3),
      target        VARCHAR(10),
      energy        NUMBER(10),
      position      NUMBER(5),
      events        NUMBER(12),
      lastevent     NUMBER(24),
      timef         DATE NOT NULL,
      timel         DATE NOT NULL,
      cdt           VARCHAR(4),
      rawfilepath   VARCHAR(128),
      comments      VARCHAR(80)) 
      STORAGE (INITIAL 40K NEXT 10K PCTINCREASE 0)
      PARTITION BY HASH (RUNTYPE) 
      (PARTITION ISS    tablespace ISS_DATA,
       PARTITION STS    tablespace STS_DATA,
       PARTITION GROUND tablespace GROUND_DATA,
       PARTITION MC     tablespace MC_DATA) ;




