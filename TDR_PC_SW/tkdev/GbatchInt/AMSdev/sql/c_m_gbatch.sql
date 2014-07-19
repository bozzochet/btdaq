drop table m_gbatch;
create table m_gbatch
    ( 
      tinsert   date    not null,
      tcreate   date,
      version   number(6,2),
      platform  varchar2(20),
      compiler  varchar2(40),
      idx       number(6),
      exefile   blob
)
STORAGE (INITIAL 1M next 512K pctincrease 0) pctfree 0;


