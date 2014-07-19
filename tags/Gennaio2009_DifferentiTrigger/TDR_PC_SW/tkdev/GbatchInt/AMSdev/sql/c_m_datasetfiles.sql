rem Data set files table 
rem  c_m_datasetfiles.sql
rem
rem   Sep 2000, A.Klimentov
rem
rem Last edit     Sep 2000, A.Klimentov
rem
 
drop    table m_datasetfiles;

create  table m_datasetfiles
(
dataset          varchar(255),           /* constraint dataset primary key */
rawfiles         number(2,0),             /* raw file exist/not exist */
taginfo          number(2,0),             /* tag info exist/not exist */
primnt           number(2,0),             /* primary ntuple exist/not exist */
mcgen            number(2,0),             /* MC gen file exist/not exist */
mc4v             number(2,0),             /* MC 4 vectors file exist/not exist */
mchits           number(2,0),             /* MC hits file exist/not exist */
rootraw          number(2,0),             /* ROOT raw file exist/not exist */
rootdst          number(2,0)             /* ROOT dst exist/not exist */
);

commit;



