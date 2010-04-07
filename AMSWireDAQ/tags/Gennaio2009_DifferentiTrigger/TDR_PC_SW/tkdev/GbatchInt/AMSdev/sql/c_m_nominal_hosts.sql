drop table m_nominal_hosts;
create table m_nominal_hosts
    ( 
      name       varchar(40),
      id         number(10,0),
      interface  number(5,0),
      platform   number(10,0),
      compiler   number(10,0),
      ncpu       number(5,0),
      ram        number(6,0),
      clock      number(6,0),
      timestamp  number(24,0)
    );

INSERT INTO m_nominal_hosts VALUES
                            ('defaulthost',
                              7,
                              1,
                              3,
                              0,
                              0,
                              0,
                              0,
                             980999988);             

INSERT INTO m_nominal_hosts VALUES
                            ('somehost',
                              6,
                              1,
                              3,
                              0,
                              0,
                              0,
                              0,
                             980999988);             

INSERT INTO m_nominal_hosts VALUES
                            ('pcamsfx',
                              5,
                              1,
                              2,
                              0,
                              0,
                              0,
                              0,
                               980999988);             

INSERT INTO m_nominal_hosts VALUES
                            ('pcamsa0',
                              4,
                              1,
                              3,
                              1,
                              2,
                             384,
                             450,
                          980999988);             
INSERT INTO m_nominal_hosts VALUES
                            ('pcamsp0',
                              3,
                              1,
                              3,
                              1,
                              2,
                             384,
                             450,
                            980999988);             
INSERT INTO m_nominal_hosts VALUES
                            ('pcamsp1',
                              1,
                              1,
                              3,
                              1,
                              2,
                             384,
                             450,
                             980999988);             
INSERT INTO m_nominal_hosts VALUES
                            ('pcamsf0',
                              2,
                              1,
                              2,
                              1,
                              2,
                            1024,
                             933,
                            980999988);             

commit;
