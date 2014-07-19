*  $Id: timel.F,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
      subroutine timel(ss)
      data ss0/2.e6/
      call timex(ss1)
      ss=ss0-ss1
      end
