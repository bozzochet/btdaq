--
-- Update tables with Failed runs
--
-- 
-- Last Edit : Oct. 25, 2000 a.k.
--
-- check via query form
-- rm *.hbk and EventStatus for Failed run

-- check m_runtable
SELECT m_runtable.run 
       FROM m_runtable, m_runstatus
       WHERE 
       m_runtable.status = m_runstatus.n AND 
       m_runstatus.status = 'Failed';

-- check m_tags
SELECT m_tags.run 
       FROM m_tags, m_runtable, m_runstatus
       WHERE 
       m_tags.run = m_runtable.run       AND
       m_runtable.status = m_runstatus.n AND 
       m_runstatus.status = 'Failed';
-- DELETE m_tags where run = FailedRun;


-- check m_dst
SELECT m_dst.run, m_dst.name  
       FROM m_dst, m_runstatus
       WHERE 
       m_dst.status = m_runstatus.n AND 
       m_runstatus.status = 'Failed';
-- DELETE m_dst where run = FailedRun;

-- update m_runtable
-- UPDATE m_runtable SET status = (SELECT m_runstatus.n FROM m_runstatus 
--                                 WHERE m_runstatus.status = 'Failed') 
--                   WHERE run = FailedRun;

-- modify 'updated run' priority
-- UPDATE m_runtable SET priority = 90 WHERE run = FailedRun;
   

-- commit;
