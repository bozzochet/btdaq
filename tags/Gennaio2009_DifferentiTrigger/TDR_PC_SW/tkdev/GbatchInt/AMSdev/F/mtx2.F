*  $Id: mtx2.F,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
             subroutine mtx2(nrmout,nrmin)
               real *4 nrmin(3,3)
               real *8 nrmout(3,3)
               real *8 x(3),y(3)
               real *8 sdot,ksi
               do i=1,3
                x(i)=nrmin(i,1)
               enddo
               sdot=x(1)*x(1)+x(2)**2+x(3)**2
               sdot=sqrt(sdot)
               imax=0
               xmax=0
               do i=1,3
                x(i)=x(i)/sdot
                nrmout(i,1)=x(i)
                if(abs(x(i)).gt.xmax)then
                  xmax=abs(x(i))
                  imax=i
                endif
               enddo
               do i=1,3
                x(i)=nrmin(i,2)
               enddo
               if(imax.eq.0)stop 'mtx2-F-imax ==0'
               ksi=-(nrmin(1,1)*x(1)+nrmin(2,1)*x(2)+nrmin(3,1)*x(3))
     +         /nrmin(imax,1)
               x(imax)=x(imax)+ksi
               sdot=x(1)*x(1)+x(2)**2+x(3)**2
               sdot=sqrt(sdot)
               do i=1,3
                x(i)=x(i)/sdot
                nrmout(i,2)=x(i)
               enddo
               
               x(1)=nrmout(2,1)*nrmout(3,2)-nrmout(3,1)*nrmout(2,2)
               x(2)=nrmout(3,1)*nrmout(1,2)-nrmout(1,1)*nrmout(3,2)
               x(3)=nrmout(1,1)*nrmout(2,2)-nrmout(2,1)*nrmout(1,2)
               sdot=x(1)*x(1)+x(2)**2+x(3)**2
               sdot=sqrt(sdot)
               do i=1,3
                x(i)=x(i)/sdot
                nrmout(i,3)=x(i)
               enddo
               
               end
 
