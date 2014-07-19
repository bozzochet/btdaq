column name format a8
 select name, active, allowed, processed, failed, killed 
   from m_active_hosts, m_nominal_hosts
       where type = 3 AND 
             m_active_hosts.id = m_nominal_hosts.id;
