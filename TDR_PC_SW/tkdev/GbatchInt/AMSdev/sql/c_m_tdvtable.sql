drop table m_tdv_names;
create table m_tdv_names
    (name       char(255)    not null, 
     datamc     number(3,0),
     idx        number(5,0)  not null
    );

drop table m_tdv;
create table m_tdv
    ( idx        number(5,0)  not null,
      tinsert       date      not null,
      tbegin        date,
      tend          date,
      uinsert       number(24)   not null,
      ubegin        number(24),
      uend          number(24),
      filepath      varchar(255) not null
     )
     PARTITION by RANGE(ubegin)
    (PARTITION pre_STS91  VALUES LESS THAN (896824000),
     PARTITION STS91      VALUES LESS THAN (897688800),
     PARTITION year_1998  VALUES LESS THAN (915145201),
     PARTITION year_1999  VALUES LESS THAN (978303601),
     PARTITION year_2000  VALUES LESS THAN (1009839601),
     PARTITION year_2001  VALUES LESS THAN (1041462001),
     PARTITION year_2002  VALUES LESS THAN (1072911601),
     PARTITION year_2003  VALUES LESS THAN (1104534001),
     PARTITION year_2004  VALUES LESS THAN (1136070001),
     PARTITION year_2005  VALUES LESS THAN (1167606001),
     PARTITION year_2006  VALUES LESS THAN (1199142001),
     PARTITION year_2007  VALUES LESS THAN (1230764401)
);

drop table m_tdvdef;
create table m_tdvdef
    ( idx           number(5,0)  not null,
      tinsert       date         not null,
      tbegin        date,
      tend          date,
      uinsert       number(24)   not null,
      ubegin        number(24),
      uend          number(24),
      filepath      varchar(255) not null
     );

commit;

