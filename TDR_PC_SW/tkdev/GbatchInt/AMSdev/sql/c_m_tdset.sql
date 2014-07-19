OPrem Data set chronology table 
rem  c_m_chronology.sql
rem   July 1999, A.Klimentov
rem
rem Last edit     Sep 2000, A.Klimentov
rem
 
drop    table m_newdsets;

create  table m_newdsets
(
ndataset         number(5),
timef            date not null,          /* first event time or production date */
timel            date,                   /* time of last event */
timei            date not null,          /* update/create time */
nruns            number(12,0),           /* total number of runs in dataset */
cruns            number(12,0),           /* number of calibration runs */
bruns            number(12,0),           /* number of bad runs */
tevents          number(24,0),           /* total number of events */
events           number(24,0),           /* number of events in normal runs */ 
comments         varchar2(3000)          /* dataset description */
);

commit;



