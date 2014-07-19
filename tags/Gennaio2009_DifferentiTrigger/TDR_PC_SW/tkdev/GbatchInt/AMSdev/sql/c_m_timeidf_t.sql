drop table &1;
create table &1
    ( iid       number(4),
      tinsert   date    not null,
      tbegin    date    not null,
      tend      date    not null,
      filepath  char(256) not null
)
STORAGE (INITIAL 80000000 NEXT 800000 MAXEXTENTS 3000);


