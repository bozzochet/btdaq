drop table m_active_clients;
create table m_active_clients
    ( 
     id          number,
     type        number(6,0),
     lastupdate  number(24,0),
     starttime   number(24,0),
     status      number(6,0),
     timestamp   number(24,0)
    );

drop table m_active_clients_ref;
create table m_active_clients_ref
    ( 
     id          number,
     ior         varchar2(1024),
     type        number(6,0),
     reftype     number(6,0),
     interface   number(6,0),
     uiid        number
    );

drop table m_active_clients_id;
create table m_active_clients_id
    ( 
     id          number,
     pid         number,
     ppid        number,
     hostid      number(6,0),
     type        number(6,0),
     exitingstatus      number(6,0),
     interface   number(6,0),
     timestamp   number(24,0)
    );

commit;
