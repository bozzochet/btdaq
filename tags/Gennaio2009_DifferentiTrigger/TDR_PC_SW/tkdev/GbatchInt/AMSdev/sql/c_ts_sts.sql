create tablespace sts
       datafile 'sts_01.dbf' size 50M 
       default storage (
         initial 10M
         next    10M
         minextents 1
         maxextents 2041
         pctincrease 0);
