rem  c_m_runstatusdec.sql
rem   Jan 2001, A.Klimentov
rem
rem Last edit     Jan 2001, A.Klimentov
rem
 
drop    table m_runstatus;
drop    table m_clientstatus;
drop    table m_clientexitingstatus;
drop    table m_hoststatus;
drop    table m_clienttype;
drop    table m_dststatus;
drop    table m_dsttype;
drop    table m_runmode;

create  table m_runstatus
(
status           varchar(40),
n                number(6,0)
);
insert into m_runstatus VALUES ('ToBeRerun',0);
insert into m_runstatus VALUES ('Failed',1);
insert into m_runstatus VALUES ('Processing',2);
insert into m_runstatus VALUES ('Finished',3);
insert into m_runstatus VALUES ('Unknown',4);


create  table m_hoststatus
(
status           varchar(60),
n                number(6,0)
);
insert into m_hoststatus VALUES ('NoResponse',0);
insert into m_hoststatus VALUES ('InActive',1);
insert into m_hoststatus VALUES ('LastClientFailed',2);
insert into m_hoststatus VALUES ('OK',3);

create  table m_clientstatus
(
status           varchar(40),
n                number(6,0)
);
insert into m_clientstatus VALUES ('Lost',0);
insert into m_clientstatus VALUES ('Unknown',1);
insert into m_clientstatus VALUES ('Submitted',2);
insert into m_clientstatus VALUES ('Registered',3);
insert into m_clientstatus VALUES ('Active',4);
insert into m_clientstatus VALUES ('TimeOut',5);
insert into m_clientstatus VALUES ('Killed',6);

create  table m_clientexitingstatus
(
status           varchar(40),
n                number(6,0)
);
insert into m_clientexitingstatus VALUES ('NOP',0);
insert into m_clientexitingstatus VALUES ('CInExit',1);
insert into m_clientexitingstatus VALUES ('SInExit',2);
insert into m_clientexitingstatus VALUES ('CInAbort',3);
insert into m_clientexitingstatus VALUES ('SInAbort',4);
insert into m_clientexitingstatus VALUES ('SInKill',5);

create  table m_clienttype
(
type             varchar(40),
n                number(6,0)
);
insert into m_clienttype VALUES ('Generic',0);
insert into m_clienttype VALUES ('Server',1);
insert into m_clienttype VALUES ('Consumer',2);
insert into m_clienttype VALUES ('Producer',3);
insert into m_clienttype VALUES ('Monitor',4);
insert into m_clienttype VALUES ('Killer',5);
insert into m_clienttype VALUES ('DBServer',6);
insert into m_clienttype VALUES ('ODBServer',7);


create  table m_dststatus
(
status             varchar(40),
n                  number(6,0)
);
insert into m_dststatus VALUES ('Success',0);
insert into m_dststatus VALUES ('InProgress',1);
insert into m_dststatus VALUES ('Failure',2);

create  table m_dsttype
(
type              varchar(40),
n                  number(6,0)
);
insert into m_dsttype VALUES ('Ntuple',0);
insert into m_dsttype VALUES ('EventTag',1);
insert into m_dsttype VALUES ('RootFile',2);

create  table m_runmode
(
runmode               varchar(40),
n                  number(6,0)
);
insert into m_runmode VALUES ('RILO',0);
insert into m_runmode VALUES ('LILO',1);
insert into m_runmode VALUES ('RIRO',2);
insert into m_runmode VALUES ('LIRO',3);

commit;



