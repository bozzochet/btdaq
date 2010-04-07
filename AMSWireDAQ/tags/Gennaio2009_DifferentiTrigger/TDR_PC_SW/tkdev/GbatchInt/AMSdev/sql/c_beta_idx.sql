create bitmap index beta_idx on m_tags 
(floor(floor(mod(recostatus,131072)/1024)/16)) 
local
nologging
storage (initial 10M pctincrease 0) pctfree 0 
tablespace scanidx
parallel 8
/
