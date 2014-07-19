drop table lob_test;
create table lob_test
    ( 
      tinsert   date    not null,
      exefile   blob
)
STORAGE (INITIAL 1M next 512K pctincrease 0) pctfree 0;


