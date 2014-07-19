create bitmap index trk_qual_idx on m_tags 
(floor(floor(mod(recostatus,1024*1024*128)/(1024*1024))/32)) 
local
nologging
storage (initial 10M pctincrease 0) pctfree 0 
tablespace scanidx
parallel 8
/
