drop table m_hostsid;
create table m_hostsid
    ( 
      name       varchar(40),
      id         number(10,0)
    );

INSERT INTO m_hostsid VALUES
                            ('defaulthost',
                              7);

INSERT INTO m_hostsid VALUES
                            ('somehost',
                              6);

INSERT INTO m_hostsid VALUES
                            ('pcamsfx',
                              5);

INSERT INTO m_hostsid VALUES
                            ('pcamsa0',
                              4);

INSERT INTO m_hostsid VALUES
                            ('pcamsp0',
                              3);

INSERT INTO m_hostsid VALUES
                            ('pcamsp1',
                              1);

INSERT INTO m_hostsid VALUES
                            ('pcamsf0',
                              2);

commit;
