drop table m_nominal_ntuples;
create table m_nominal_ntuples
    ( 
       id                  number(10,0),
       mode                number(10,0),
       updfreq             number(10,0),
       host                number(10,0),
       output              char(256)
);

INSERT INTO m_nominal_ntuples VALUES(
                                     1,
                                     0,
                                    200,
                                      7,
           '$AMSProdNtupleDir');

INSERT INTO m_nominal_ntuples VALUES(
                                     2,
                                     0,
                                    400,
                                      2,
           '$AMSProdNtupleDir');

INSERT INTO m_nominal_ntuples VALUES(
                                     3,
                                     0,
                                    200,
                                      1,
           '$AMSProdNtupleDir');

INSERT INTO m_nominal_ntuples VALUES(
                                     4,
                                     0,
                                    200,
                                      3,
           '$AMSProdNtupleDir');

INSERT INTO m_nominal_ntuples VALUES(
                                     5,
                                     0,
                                    200,
                                      4,
           '$AMSProdNtupleDir');
