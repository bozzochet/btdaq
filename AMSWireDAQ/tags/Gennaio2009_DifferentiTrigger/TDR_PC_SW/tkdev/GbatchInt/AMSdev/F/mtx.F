!  $Id: mtx.F,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
subroutine mtx(nrm,y)
real, dimension (3,3) :: nrm
real, dimension(3) :: y,x
real :: sdot
x=y
sdot=dot_product(x,x)
sdot=sqrt(sdot)
x=x/sdot
nrm(:,1)=x
x=0
x(2)=1/((nrm(2,1)/nrm(1,1))**2+1)**0.5
x(1)=-nrm(2,1)/nrm(1,1)*x(2)
x(3)=0
x=x/sqrt(dot_product(x,x))
nrm(:,2)=x
x(1)=nrm(2,1)*nrm(3,2)-nrm(3,1)*nrm(2,2)
x(2)=nrm(3,1)*nrm(1,2)-nrm(1,1)*nrm(3,2)
x(3)=nrm(1,1)*nrm(2,2)-nrm(2,1)*nrm(1,2)
x=x/sqrt(dot_product(x,x))
nrm(:,3)=x
! write(*,*)nrm
end subroutine mtx
