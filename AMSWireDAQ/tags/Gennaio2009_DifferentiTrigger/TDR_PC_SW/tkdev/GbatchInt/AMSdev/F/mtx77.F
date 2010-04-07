*  $Id: mtx77.F,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
             subroutine mtx(nrm,y)
               real *4 nrm(3,3)
               real *4 x(3),y(3)
               real *4 sdot
               do i=1,3
                x(i)=y(i)
               enddo
               sdot=x(1)*x(1)+x(2)**2+x(3)**2
               sdot=sqrt(sdot)
               do i=1,3
                x(i)=x(i)/sdot
                nrm(i,1)=x(i)
               enddo
               x(2)=1/((nrm(2,1)/nrm(1,1))**2+1)**0.5
               x(1)=-nrm(2,1)/nrm(1,1)*x(2)
               x(3)=0
               sdot=x(1)*x(1)+x(2)**2+x(3)**2
               sdot=sqrt(sdot)
               do i=1,3
                x(i)=x(i)/sdot
                nrm(i,2)=x(i)
               enddo
               
                x(1)=nrm(2,1)*nrm(3,2)-nrm(3,1)*nrm(2,2)
                x(2)=nrm(3,1)*nrm(1,2)-nrm(1,1)*nrm(3,2)
                x(3)=nrm(1,1)*nrm(2,2)-nrm(2,1)*nrm(1,2)
               sdot=x(1)*x(1)+x(2)**2+x(3)**2
               sdot=sqrt(sdot)
               do i=1,3
                x(i)=x(i)/sdot
                nrm(i,3)=x(i)
               enddo
               
               end
 
