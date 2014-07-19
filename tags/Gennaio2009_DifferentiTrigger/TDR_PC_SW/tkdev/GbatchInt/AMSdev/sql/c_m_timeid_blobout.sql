drop table &1;
create table &1
    ( iid       number(4),
      tinsert   date    not null,
      tbegin    date    not null,
      tend      date    not null,
      tdv       blob
)
nologging
tablespace scandata
STORAGE (INITIAL 512K next 256K pctincrease 0) pctfree 0
lob (tdv) store as n_tdv_lob
(tablespace scandata pctversion 0
storage (initial 512K next 512K pctincrease 0)
nocache nologging
);


