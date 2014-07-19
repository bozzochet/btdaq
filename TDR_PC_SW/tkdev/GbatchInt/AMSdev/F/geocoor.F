*  $Id: geocoor.F,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
          SUBROUTINE GEOCOOR(glat,glong,galt,blat,blong,distd)
	
c******************************************************************************
c	
c	Author:	Davide Grandi 
c		INFN MIlano
c		Via Celoria 16
c		20133 Milano
c		ITALY
c		
c	FUNCTION: Transformation of geographical spherical coordinates in 
c	geomagnetical spherical coordinates, taking into account the simple
c	shifthed dipole model.	
c	
c	INPUT PARAMETERS: 
c	glat  -  geographical latitude		 (in rad)  
c	glong -  geographical longitude  	 (in rad)
c	galt  -  distance from the earth center  (in km).
c	
c	OUTPUT PARAMETERS:
c	blat  -  geomagnetic latitude 		 (in rad)
c	blong -  geomagnetic longitude 		 (in rad)
c	distd -  distance from the dipole center (in km). 
c	
c
c******************************************************************************
	

	
	real *8 deltac,cc,ss,deltas,rx,ry,rz,c,rlat,rlong,cblat,srx,sry,srz
	real *8	glat,glong,galt
        real *4 blat,blong,distd
	
c	constants used in the calculations

	plambdan=-1.38404609 !magnetic north pole latitude
	pphin=1.89525303     !magnetic north pole longitude
	dlambda=0.37472219   !dipole center latitude
	dphi=2.52671316      !dipole center longitude
	erred=527	     !dipole center shift respect earth's center in km
	
c	Input data are geographic coordinates
c	glat is geographical latitude
c	glong is geographical longitude
c	galt is distance from the center of the earth in km
	
	
c	transformation from geographic coordinates to geomagnetic coordinates


	
c	constants
	
	sp=sin(plambdan)
	cp=cos(plambdan)
	
c	rotating geographical spherical coordinates in the new system

c	first calculate rotated latitude	
	
	 deltac=cos(glong-pphin)
	 cc=cos(glat)*cp
	 ss=sin(glat)*sp
	 rlat=asin(ss+cc*deltac)
		
c	then calculate rotated longitude

	 deltas=sin(glong-pphin)
	 c=cos(glat)
	 cblat=cos(rlat)
	 rlong=asin((c*deltas)/cblat)
		
		
	
c	rotate center of dipole spherical coordinates to the new system

c	first calculate rotated dipole latitude
	
	 a=cos(dphi-pphin)
	 b=cos(dlambda)*cos(plambdan)
	 g=sin(dlambda)*sin(plambdan)
	 rdlambda=asin(g+b*a)
		
c	then calculate rotated dipole longitude	
		
	 d=sin(dphi-pphin)
	 e=cos(dlambda)
	 f=cos(rdlambda)
	 rdphi=asin((e*d)/f)
		
		
		
c	center of dipole coordinates in the new rotated system	
	
	 xx=erred*cos(rdphi)*cos(rdlambda)	
	 yy=erred*sin(rdphi)*cos(rdlambda)
	 zz=erred*sin(rdlambda)
	
	
	
c	cartesian coordinates in the new rotated system
	
	 rx=galt*cos(rlong)*cos(rlat)
	 ry=galt*sin(rlong)*cos(rlat)
	 rz=galt*sin(rlat)
	 
c	final cartesian coordinates 
	 
	 srx=rx-xx
	 sry=ry-yy
	 srz=rz-zz
	 

	 
c	calculate the effective altitude from the dipole center
	
	 distd=sqrt((srx)**2+(sry)**2+(srz)**2)
	 
c	new geomagnetic spherical coordinates in the shifted-rotated system

c	blat is geomagnetic latitude, and blong is geomagnetic longitude

	 blat=asin(srz/distd)	
	 blong=atan2(sry,srx)	 
	 
	
	
	return
        
	
	end
c*****************************************************************************	
c
c	 	REFERENCES
c
c	 Asgeir Brekke. Physics Of The Upper Polar Atmosphere. 
c	Wiley, pp.127-145, 1997.
c
c*****************************************************************************
