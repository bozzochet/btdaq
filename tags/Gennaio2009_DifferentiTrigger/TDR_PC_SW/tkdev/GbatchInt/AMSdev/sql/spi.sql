column Status FORMAT A10
column Host FORMAT A8
select  m_prodinfo.run "Run",  
        m_prodinfo.events "Events",  
        m_prodinfo.criticalerrors "CErr",  
        m_prodinfo.errors "Errors",  
        m_prodinfo.elapsedtime "Elapsed",
        m_prodinfo.cputime "CPUtime",  
        m_nominal_hosts.name "Host",  
        m_runstatus.status     "Status",  
        m_prodinfo.timestamp "Timestamp"  
from
m_prodinfo, 
m_nominal_hosts, m_runstatus  
where
m_nominal_hosts.id = m_prodinfo.hostid AND
m_runstatus.n      = m_prodinfo.status
ORDER by m_prodinfo.timestamp;
