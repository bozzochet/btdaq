create bitmap index charge_idx on m_tags 
(floor(mod(recostatus,256)/32)) 
local
nologging
storage (initial 10M pctincrease 0) pctfree 0 
tablespace scanidx
parallel 8
/
