drop view sts91_chronology;
create view sts91_chronology
       as   select
       to_char(timestamp,'DD-MON-YY HH24:MI:SS') UnivTime,
       metdd,methh,metmm,metss,
       shuttle_mode  smode
       FROM m_sts_chronology;
column univtime        HEADING    UniversalTime;
column smode           HEADING    Event;

column metdd           HEADING    Day;
column methh           HEADING    HH:;
column metmm           HEADING    MM:;
column metss           HEADING    SS;

column metdd FORMAT 999;
column methh FORMAT 99;
column metmm FORMAT 99;
column metss FORMAT 99;
column smode FORMAT A40;

set pagesize 120;

select all * from sts91_chronology;







