drop table cern_hosts;
// mhz    : cat /proc/cpuinfo | grep -i -e 'cpu mhz'
// name   : cat /proc/cpuinfo | grep -i -e name
// gcc    : gcc -v
// kernel : uname -a
// mips   : clock*coeff 
//
create table cern_hosts
    ( 
      host        varchar(40),
      kernel      varchar(40),
      gcc         varchar(20),
      ncpu        number(5,0),
      mhz         number(10,4),
      hyperthread number(2,0),
      model       varchar(50),
      mips        number(10,4),
      timestamp   number(24,0)
    );

INSERT INTO cern_hosts VALUES
                            ('pcamsa0',
                              'RH 7.3.3 2.4.20-24.7.cernsmp',
                              'gcc version 3.2.3',
                              2,
                              451.026,
                              0,
                             'Pentium II (Deschutes)',
                              0,
                              1079075025);             

INSERT INTO cern_hosts VALUES
                            ('pcamsp0',
                              'RH 7.3.3 2.4.20-24.7.cernsmp',
                              'gcc version 2.96',
                              2,
                              451.033,
                              0,
                             'Pentium II (Deschutes)',
                              0,
                              1079075025);             


INSERT INTO cern_hosts VALUES
                            ('pcamsp1',
                              'RH 7.3.3 2.4.20-24.7.cernsmp',
                              'gcc version 2.95.3',
                              2,
                              451.028,
                              0,
                             'Pentium II (Deschutes)',
                              0,
                              1079075025);             

INSERT INTO cern_hosts VALUES
                            ('pcamsf0',
                              'RH 7.3.3 2.4.20-24.7.cernsmp',
                              'gcc version 2.95.3',
                              2,
                              932.908,
                              0,
                             'Pentium III (Coppermine)',
                              0,
                              1079075025);             

INSERT INTO cern_hosts VALUES
                            ('pcamsf1',
                              'RH 7.3.3 2.4.20-24.7.cernsmp',
                              'gcc version 3.2.1',
                              2,
                              1526.556,
                              0,
                             'AMD Athlon(tm) MP 1800+',
                              0,
                              1079075025);             

INSERT INTO cern_hosts VALUES
                            ('pcamsf2',
                              'RH 7.3.3 2.4.20-24.7.cernsmp',
                              'gcc version 3.2.3',
                              2,
                              1982.579,
                              0,
                             'Intel(R) Xeon(TM) CPU 2.00GHz',
                              0,
                              1079075025);             


INSERT INTO cern_hosts VALUES
                            ('pcamsf3',
                              'RH 7.3.3 2.4.20-24.7.cernsmp',
                              'gcc version 2.96',
                              1,
                              1200.063,
                              0,
                             'AMD Athlon(tm) Processor',
                              0,
                              1079075025);             


INSERT INTO cern_hosts VALUES
                            ('pcamsf4',
                              'RH 7.3.3 2.4.20-24.7.cernsmp',
                              'gcc version  3.2.2',
                              2,
                              1526.559,
                              0,
                             'AMD Athlon(tm) MP 1800+',
                              0,
                              1079075025);             


INSERT INTO cern_hosts VALUES
                            ('pcamsf5',
                              'RH 9.0  2.4.20-20.9smp',
                              'gcc version 3.2.3 ',
                              2,
                              1526.544,
                              0,
                             'AMD Athlon(tm) MP 1800+',
                              0,
                              1079075025);           

INSERT INTO cern_hosts VALUES
                            ('pcamsf6',
                              'RH 7.3.3 2.4.20-24.7.cernsmp',
                              'gcc version 3.2.3',
                              2,
                              3056.870,
                              1,
                             'Intel(R) Xeon(TM) CPU 3.06GHz',
                              0,
                              1079075025);             


INSERT INTO cern_hosts VALUES
                            ('pcamsf7',
                              'RH 7.3.3 2.4.20-24.7.cernsmp',
                              'gcc version 3.2.3',
                              2,
                              3056.874,
                              1,
                             'Intel(R) Xeon(TM) CPU 3.06GHz',
                              0,
                              1079075025);             

  
INSERT INTO cern_hosts VALUES
                            ('pcamsf9',
                              'RH 9.0 2.4.20-20.9smp',
                              'gcc version 3.2.3',
                              1,
                              3192.273,
                              1,
                             'Intel(R) Pentium(R) 4 CPU 3.20GHz',
                              0,
                              1079075025);             


INSERT INTO cern_hosts VALUES
                            ('pcamst0',
                              'RH 7.3.3 2.4.20-24.7cernsmp',
                              'gcc version 2.96',
                              2,
                              601.370,
                              0,
                             'Pentium III (Coppermine)',
                              0,
                              1079075025);             


commit;

drop table cpu_coeff;
create table cpu_coeff
    ( 
      model     varchar(30),
      minclock  number,
      maxclock  number,
      coeff     number(4,2)
);

INSERT INTO cpu_coeff VALUES ('Pentium II',0,10000,1.07);
INSERT INTO cpu_coeff VALUES ('Pentium III',0,10000,1.0);
INSERT INTO cpu_coeff VALUES ('Pentium(R) 4',0,2400,0.7);
INSERT INTO cpu_coeff VALUES ('Pentium(R) 4',2399,10000,0.85);
INSERT INTO cpu_coeff VALUES ('Pentium(R) M',0,10000,1.07);
INSERT INTO cpu_coeff VALUES ('Xeon',0,10000,0.8);
INSERT INTO cpu_coeff VALUES ('Athlon',0,10000,1.15);
commit;
