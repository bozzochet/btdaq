*  $Id: write.f,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
           real x(30),y(40),z(41)
           real bx(40,10,15),by(100,20,10),bz(30,30,30)
           integer iq
           open(33,file='test.dat',
     +     form='unformatted',status='new',iostat=iostat)
           if(iostat.ne.0)then
            write(*,*)'yok'
            stop
           endif
           bz(30,30,30)=135.
           iq=123456789
           write(33)x,y,z,iq,bx,by,bz
           end
            
                
