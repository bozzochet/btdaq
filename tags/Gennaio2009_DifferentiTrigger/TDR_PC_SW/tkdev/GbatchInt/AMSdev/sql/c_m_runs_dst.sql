drop table m_runs_dst;
create table m_runs_dst
    ( 
      run           NUMBER(24,0) not null,
      eventf        NUMBER(12,0),
      eventl        NUMBER(12,0),
      timestamp     DATE,
      processed     NUMBER(12,0),
      version       NUMBER(6,2),
      platform      VARCHAR2(80),
      compiler      VARCHAR2(80),
      rawfilepath   VARCHAR2(256)
)
;
