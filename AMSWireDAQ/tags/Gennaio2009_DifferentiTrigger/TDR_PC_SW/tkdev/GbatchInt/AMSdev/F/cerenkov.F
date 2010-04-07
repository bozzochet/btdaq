*  $Id: cerenkov.F,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
*      subroutine solve(ma3,ma2,ma1,ma0,sols,mt)
      subroutine solve(a3,a2,a1,a0,sols,mt)
      complex*16 csols(4)
      real*8 sols(4)   !,ma3,ma2,ma1,ma0
      integer mt
      real*8 a3,a2,a1,a0,dc

      sols(1)=0.
      sols(2)=0.
      sols(3)=0.
      sols(4)=0.

*      a3=ma3
*      a2=ma2
*      a1=ma1
*      a0=ma0

      call drteq4(a3,a2,a1,a0,csols,dc,mt)

      if(mt.eq.1) then
        sols(1)=real(real(csols(1)))
        sols(2)=real(real(csols(2)))
        sols(3)=real(real(csols(3)))
        sols(4)=real(real(csols(4)))
        mt=4
      else if(mt.eq.3) then
        sols(1)=real(real(csols(1)))
        sols(2)=real(real(csols(2)))
        mt=2
      else
        mt=0
      endif
      end

