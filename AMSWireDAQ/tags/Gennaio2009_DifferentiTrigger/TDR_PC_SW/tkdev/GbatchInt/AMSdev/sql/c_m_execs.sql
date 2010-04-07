drop table m_execs;
create table m_execs
    ( 
      name       varchar(255),
      id         number(20,0),
      version    number(10,2),
      platform   number(5,0),
      compiler   number(5,0)
    );

commit;
                           
