drop view stat_2005A_Jobs;
create view stat_2005A_Jobs (FirstJobTime,LastJobTime,TotalJobs,TotalTriggers)   
       AS   
       SELECT  
        MIN(Jobs.time), MAX(Jobs.timestamp), Count(jobs.jid), SUM(Jobs.triggers)  
         FROM Jobs WHERE Jobs.pid=7 AND Jobs.cid != 2;
drop view stat_2005B_Jobs;
create view stat_2005B_Jobs (FirstJobTime,LastJobTime,TotalJobs,TotalTriggers)   
       AS   
       SELECT  
        MIN(Jobs.time), MAX(Jobs.timestamp), Count(jobs.jid), SUM(Jobs.triggers)  
         FROM Jobs WHERE Jobs.pid=8 AND Jobs.cid != 2;


drop view stat_2005A_EndRuns;
create view stat_2005A_EndRuns (TotalRuns,SumFEvent,SumLEvent)   
       AS 
       SELECT 
        COUNT(Runs.jid), SUM(Runs.fevent), SUM(Runs.levent)  FROM Jobs, Runs 
          WHERE Runs.jid=Jobs.jid AND (Runs.status='Finished' OR Runs.status='Completed') 
           AND Jobs.pid=7 AND Jobs.cid != 2;
drop view stat_2005B_EndRuns;
create view stat_2005B_EndRuns (TotalRuns,SumFEvent,SumLEvent)   
       AS 
       SELECT 
        COUNT(Runs.jid), SUM(Runs.fevent), SUM(Runs.levent)  FROM Jobs, Runs 
          WHERE Runs.jid=Jobs.jid AND (Runs.status='Finished' OR Runs.status='Completed') 
           AND Jobs.pid=8 AND Jobs.cid != 2;



drop view stat_2005A_FailedRuns;
create view stat_2005A_FailedRuns (TotalRuns)   
        AS
        SELECT
          COUNT(Runs.jid) FROM Runs, Jobs         
           WHERE (Runs.status='Failed' OR Runs.status='Unchecked') AND Runs.jid=Jobs.jid 
            AND Jobs.pid=7 AND Jobs.cid != 2;
drop view stat_2005B_FailedRuns;
create view stat_2005B_FailedRuns (TotalRuns)   
        AS
        SELECT
          COUNT(Runs.jid) FROM Runs, Jobs         
           WHERE (Runs.status='Failed' OR Runs.status='Unchecked') AND Runs.jid=Jobs.jid 
             AND Jobs.pid=8 AND Jobs.cid != 2;

drop view stat_2005A_TimeOutRuns;
create view stat_2005A_TimeOutRuns (TotalRuns)   
        AS
        SELECT
          COUNT(Runs.jid) FROM Runs WHERE  Runs.status='TimeOut' AND Runs.submit > 1107510000;
drop view stat_2005B_TimeOutRuns;
create view stat_2005B_TimeOutRuns (TotalRuns)   
        AS
        SELECT
          COUNT(Runs.jid) FROM Runs WHERE  Runs.status='TimeOut' AND Runs.submit > 1111137000;



drop view stat_2005A_NTuples;
create view stat_2005A_NTuples (TotalFiles,SIZEMB)   
        AS 
        SELECT 
          COUNT(ntuples.run), SUM(ntuples.SIZEMB) FROM ntuples  WHERE ntuples.path LIKE '%2005A%';
drop view stat_2005B_NTuples;
create view stat_2005B_NTuples (TotalFiles,SIZEMB)   
        AS 
        SELECT 
          COUNT(ntuples.run), SUM(ntuples.SIZEMB) FROM ntuples  WHERE ntuples.path LIKE '%2005B%';


 
drop view stat_2005A_CastorNTuples;
create view stat_2005A_CastorNTuples (TotalFiles,SIZEMB)   
       AS
        SELECT 
          COUNT(ntuples.run), SUM(ntuples.SIZEMB) FROM ntuples  WHERE 
             ntuples.path LIKE '%2005A%'  AND ntuples.castortime != 0;
drop view stat_2005B_CastorNTuples;
create view stat_2005B_CastorNTuples (TotalFiles,SIZEMB)   
       AS
        SELECT 
          COUNT(ntuples.run), SUM(ntuples.SIZEMB) FROM ntuples  WHERE 
             ntuples.path LIKE '%2005B%'  AND ntuples.castortime != 0;

SELECT 
        stat_2005A_Jobs.firstjobtime,stat_2005A_Jobs.lastjobtime,
        stat_2005A_Jobs.totaljobs,stat_2005A_Jobs.totaltriggers,
        stat_2005A_EndRuns.TotalRuns,stat_2005A_EndRuns.SumFEvent,stat_2005A_EndRuns.SumLEvent,
        stat_2005A_FailedRuns.TotalRuns,
        stat_2005A_TimeoutRuns.TotalRuns,
        stat_2005A_NTuples.TotalFiles, stat_2005A_NTuples.SizeMB,    
        stat_2005A_CastorNTuples.TotalFiles, stat_2005A_CastorNTuples.SizeMB
     FROM 
        stat_2005A_Jobs, stat_2005A_EndRuns, stat_2005A_FailedRuns, stat_2005A_TimeoutRuns, 
        stat_2005A_NTuples,stat_2005A_CastorNTuples;


SELECT 
        stat_2005B_Jobs.firstjobtime,stat_2005B_Jobs.lastjobtime,
        stat_2005B_Jobs.totaljobs,stat_2005B_Jobs.totaltriggers,
        stat_2005B_EndRuns.TotalRuns,stat_2005B_EndRuns.SumFEvent,stat_2005B_EndRuns.SumLEvent,
        stat_2005B_FailedRuns.TotalRuns,
        stat_2005B_TimeoutRuns.TotalRuns,
        stat_2005B_NTuples.TotalFiles, stat_2005B_NTuples.SizeMB,    
        stat_2005B_CastorNTuples.TotalFiles, stat_2005B_CastorNTuples.SizeMB
     FROM 
        stat_2005B_Jobs, stat_2005B_EndRuns, stat_2005B_FailedRuns, stat_2005B_TimeoutRuns, 
        stat_2005B_NTuples,stat_2005B_CastorNTuples;
 
        





