rem  c_m_compilerdesc.sql
rem   Jan 2001, A.Klimentov
rem
rem Last edit     Jan 2001, A.Klimentov
rem
 
drop    table m_compilers;
drop    table m_platforms;

create  table m_compilers
(
name           varchar(255),
id                  number(10,0)
);
insert into m_compilers  VALUES (
              'Dummy', 0);
insert into m_compilers  VALUES (
              'gcc version 2.95.2 19991024', 1);
insert into m_compilers  VALUES (
              'Compaq C++ V6.2-024 ', 2);
insert into m_compilers  VALUES (
              'gcc version egcs-2.91.66 19990314', 3);

create  table m_platforms
(
type           varchar(80),
id                  number(10,0)
);
insert into m_platforms  VALUES
                  ('Dummy',0);
insert into m_platforms  VALUES
                  ('Digital UNIX V4.0D  (Rev. 878)',1);
insert into m_platforms  VALUES
   ('Red Hat Linux release 6.2 (Zoot) Kernel 2.2.16-3smp on a 2-processor i686',2);
insert into m_platforms  VALUES
('Red Hat Linux release 6.1 (Cartman) Kernel 2.2.12-20smp on a 2-processor i686',3);
insert into m_platforms  VALUES
('Linux-g++',4);
commit;



