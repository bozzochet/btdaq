drop table m_nominal_clients;
create table m_nominal_clients
    ( 
       id                  number(20,0),
       type                number(6,0),
       maxclients          number(6,0),
       cpu                 number(6,2),
       memory              number(6,0),
       scriptpath          varchar2(1024),
       logpath             varchar2(1024),
       submitcmd           varchar2(1024),
       hostid              number(6,0),
       logintheend         number(5,0),
       timestamp           number(24,0)
);


INSERT INTO m_nominal_clients VALUES(
                                     2,
                                     1,
                                     1,
                                    1.9,
                                     50,
           '$AMSProdDir/$AMSProdServer',
           '-o /dat0/local/logs/',
           '/usr/local/lsf/bin/bsub -q linux_server -m ',
           7,
           0,
           980000000);

INSERT INTO m_nominal_clients VALUES(
                                     3,
                                     1,
                                     4,
                                    1.9,
                                     50,
           '$AMSProdDir/$AMSProdServer',
           '"1>&"/dat0/local/logs/',
           'rsh ',
           5,
           1,
           980000000);

INSERT INTO m_nominal_clients VALUES(
                                     4,
                                     3,
                                     1,
                                    0.9,
                                    100,
           '$AMSDatacardsDir/$AMSProdJob',
           ' -o /dat0/local/logs/',
           '/usr/local/lsf/bin/bsub -q linux_client -m ',
           7,
           0,
           980000000);

INSERT INTO m_nominal_clients VALUES(
                                     5,
                                     3,
                                     12,
                                    0.9,
                                    100,
           '$AMSDatacardsDir/$AMSProdJob',
           '"1>&"/dat0/local/logs/',
           '/usr/local/lsf/bin/bsub -q linux_client -m ',
           5,
           1,
           980000000);



INSERT INTO m_nominal_clients VALUES(
                                     6,
                                     5,
                                     1,
                                    0.9,
                                     50,
           'kill',
           '-o /dat0/local/logs/',
           '/usr/local/lsf/bin/bsub -K -q linux_control -m ',
           7,
           0,
           980000000);


commit;
