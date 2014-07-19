drop table &1;
create table &1
    ( iid       number(4),
      tinsert   date    not null,
      tbegin    date    not null,
      tend      date    not null,
      tdv       blob
)
STORAGE (INITIAL 1M next 512K pctincrease 0) pctfree 0;


