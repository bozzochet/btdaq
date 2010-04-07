drop table m_datacards;
create table m_datacards
    ( 
      dname       varchar2(80),
      idx         number(10),
      timestamp   number(24)
)
;

drop table m_datacards_cards;
create table m_datacards_cards
    ( 
      idx       number(10),
      datacards varchar2(4000)
)
;

