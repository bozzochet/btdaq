*  $Id: fortrmc.f,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
                subroutine fortrmc
      parameter(nchx=384)
      parameter(nchy=640)
      vector ax(nchx)
      vector ax2(nchx)
      vector ay2(nchy)
      vector ay(nchy)
      vector cx(nchx)
      vector cy(nchy)
      vector d1(100)
      call hbook1(10001,'av x',100,-20.,20.,0.)
      call hbook1(10002,'av y',100,-10.,10.,0.)
      call hbook1(20001,'s x',100,0.,30.,0.)
      call hbook1(20002,'s y',100,0.,20.,0.)
      call hbook1(30001,'av x',nchx,0.5,float(nchx)+0.5,0.)
      call hbook1(30002,'av x',nchy,0.5,float(nchy)+0.5,0.)
      do i=1,nchx
        if(cx(i).ne.0)then
           ax(i)=ax(i)/cx(i)
           ax2(i)=ax2(i)/cx(i)
           ax2(i)=sqrt(abs(ax2(i)-ax(i)**2))
           call hf1(10001,ax(i),1.)              
           call hf1(20001,ax2(i),1.)              
           call hf1(30001,float(i),cx(i))
        endif
      enddo
      do i=1,nchy
        if(cy(i).ne.0)then
           ay(i)=ay(i)/cy(i)
           ay2(i)=ay2(i)/cy(i)
           ay2(i)=sqrt(abs(ay2(i)-ay(i)**2))
           call hf1(10002,ay(i),1.)              
           call hf1(20002,ay2(i),1.)              
           call hf1(30002,float(i),cy(i))
        endif
      enddo
      end
