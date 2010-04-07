*  $Id: loc2gl.f,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
      subroutine loc2gl(thetas,phis,thetav,phiv,roll,pitch,yaw, 
     + thetalocgl,philocgl,cosmag)
*
*     input   
*     thetas   station lattitude GTOD(rad) (z gl axis)
*     phis     station longitude GTOD(rad) 
*     thetav   station velocity vector lattitude GTOD(rad)
*     phiv     station velocity vector longitude GTOD(rad)
*     roll     roll (rad)
*     pitch    pitch (rad)
*     yaw      yaw (rad)
*     input/output
*     thetalocgl (rad)   on entry :loc on exit gl   
*     philocglgl   (rad)  -----------------------
*
*     local === ams cooo sys
*     global == GTOD
*
*     output
*     cosmag cos(particle dir, mag meridian plane)
*

           double precision amsxg(3),amsyg(3),amszg(3)
           double precision amsx(3),amsy(3),amsz(3),axis(3)
           double precision c1,c2,c3,s1,s2,s3
           double precision n1,n2,n3,l1,l2,l3,m1,m2,m3
           double precision dir(3)
           double precision t,p,sth,cl
           pi=3.1415926
*
* AMS global coo
*


                 amszg(1)=sin(pi/2-thetas)*cos(phis)
                 amszg(2)=sin(pi/2-thetas)*sin(phis)
                 amszg(3)=cos(pi/2-thetas)
                 amsxg(1)=sin(pi/2-thetav)*cos(phiv)
                 amsxg(2)=sin(pi/2-thetav)*sin(phiv)
                 amsxg(3)=cos(pi/2-thetav)
                 amsyg(1)=amszg(2)*amsxg(3)-amszg(3)*amsxg(2)
                 amsyg(2)=amszg(3)*amsxg(1)-amszg(1)*amsxg(3) 
                 amsyg(3)=amszg(1)*amsxg(2)-amszg(2)*amsxg(1) 
                 cp=cos(pitch)  
                 sp=sin(pitch) 
                 cy=cos(yaw) 
                 sy=sin(yaw) 
                 cr=cos(roll) 
                 sr=sin(roll) 
*
* have to use U^T matrix here
*
                 l1=cy*cp
                 m1=-sy
                 n1=cy*sp
                 l2=cr*sy*cp-sr*sp
                 m2=cr*cy
                 n2=cr*sy*sp+sr*cp
                 l3=-sr*sy*cp-cr*sp
                 m3=-sr*cy
                 n3=-sr*sy*sp+cr*cp
*
*               ams local coo system
*
                 amsx(1)=-(l1*amsxg(1)+l2*amsyg(1)+l3*amszg(1)) 
                 amsx(2)=-(l1*amsxg(2)+l2*amsyg(2)+l3*amszg(2)) 
                 amsx(3)=-(l1*amsxg(3)+l2*amsyg(3)+l3*amszg(3)) 
                 amsy(1)=-(m1*amsxg(1)+m2*amsyg(1)+m3*amszg(1)) 
                 amsy(2)=-(m1*amsxg(2)+m2*amsyg(2)+m3*amszg(2)) 
                 amsy(3)=-(m1*amsxg(3)+m2*amsyg(3)+m3*amszg(3)) 
                 amsz(1)=n1*amsxg(1)+n2*amsyg(1)+n3*amszg(1) 
                 amsz(2)=n1*amsxg(2)+n2*amsyg(2)+n3*amszg(2) 
                 amsz(3)=n1*amsxg(3)+n2*amsyg(3)+n3*amszg(3) 


                 dir(1)=sin(thetalocgl)*cos(philocgl)
                 dir(2)=sin(thetalocgl)*sin(philocgl)
                 dir(3)=cos(thetalocgl)
*
* Goto global system from AMS one
*

                 ue=dir(1)*amsx(1)+dir(2)*amsy(1)+dir(3)*amsz(1) 
                 ve=dir(1)*amsx(2)+dir(2)*amsy(2)+dir(3)*amsz(2) 
                 we=dir(1)*amsx(3)+dir(2)*amsy(3)+dir(3)*amsz(3) 
                 thetalocgl=acos(we)
                 philocgl=atan2(ve,ue)
*
* Magnetic meridian
*

               thetam=78.6/180.*3.1415926
               phim=290.6/180.*3.1415926
               um=sin(pi/2-thetam)*cos(phim)
               vm=sin(pi/2-thetam)*sin(phim)
               wm=cos(pi/2-thetam)
               uv=vm*amszg(3)-wm*amszg(2)
               vv=wm*amszg(1)-um*amszg(3)
               wv=um*amszg(2)-vm*amszg(1)
               cosmag=ue*uv+ve*vv+we*wv


             end
