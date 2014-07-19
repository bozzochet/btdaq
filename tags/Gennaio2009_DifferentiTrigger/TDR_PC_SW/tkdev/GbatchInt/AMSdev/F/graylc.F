*  $Id: graylc.F,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $





      SUBROUTINE GRAYLC                                                                 
C.                                                                                      
C.    ******************************************************************                
C.    *                                                                *                
C.    *  Generates COHERENT SCATTERING mechanism                                        
C.    *  for Cerenkov photons                                                           
C.    *                                                                *                
C.    *    ==>Called by : GTCKOV                                       *                
C.    * Authors:  Isabelle Debonis & Jean Favier                       *                
C.    * debonis@lapp.in2p3.fr                                                           
C.    *                                                                *                
C.    ******************************************************************     
                                                                                      
#include "geant321/gcbank.inc"
#include "geant321/gctrak.inc"
#include "geant321/gcphys.inc"
#include "geant321/gconsp.inc"
#include "geant321/gcking.inc"
#include "geant321/gcjloc.inc"
#include "geant321/gcmulo.inc"
*
	DIMENSION VROT(3) 
	DIMENSION RNDM(3)

        CALL GRNDM(RNDM,3)
*
* Sample (1+u^2)
*
        R1=RNDM(1)
        R2=2.*RNDM(2)-1.
        POW=1.
        IF (R1.LT.0.25) POW=1./3
        CT=SIGN(1.,R2)*ABS(R2)**POW
        COSTH = CT
        SINTH = SQRT(1.-COSTH**2)
*                                                                                       
*  ** Extract phi  (PHI unif. between 0 and 2PI)                                        
*                                                                                       
	PH=RNDM(3)*TWOPI                                                                 
	COSPH=COS(PH)                                                                    
	SINPH=SIN(PH)                                                                    
*                                                                                       
* ** Set new VECT(I), i=4,5,6                                                           
*                                                                                       
	VROT(1)=GEKIN*SINTH*COSPH                                                        
	VROT(2)=GEKIN*SINTH*SINPH                                                        
	VROT(3)=GEKIN*COSTH                                                              
*                                                                                       
* ** Rotate scattered photon into GEANT system                                          
*                                                                                       
	CALL GVROT(VECT(4),VROT)                                                         
	VMROT=1./SQRT(VROT(1)*VROT(1)+VROT(2)*VROT(2)+VROT(3)*VROT(3))                   
        VECT(4)=VROT(1)*VMROT                                                            
	VECT(5)=VROT(2)*VMROT                                                            
	VECT(6)=VROT(3)*VMROT                                                            

        call hf1(1000,vect(6),1.)
*                                                                                       
      END                                                                               
