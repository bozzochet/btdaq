drop table m_interfaces;
create table m_interfaces
    ( 
      name       varchar(40),
      id         number(6,0)
    );
insert into m_interfaces values('default',0);
insert into m_interfaces values('hrdl',1);
insert into m_interfaces values('srdl',2);
insert into m_interfaces values('Dummy',3);
                           