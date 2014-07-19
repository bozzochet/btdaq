*  $Id: rndm.F,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
             function rndm(dummy)
             
             data i/0/
             real *4 r(2)
             call grndm(r,1)
             rndm=r(1)
c             call grndmq(i1,i2,0,'G')
c              write(*,*)i1,i2
c             i=i+1
c             write(*,*)r(1)
              end
