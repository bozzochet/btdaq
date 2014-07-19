CPAT,BLKDAT   .                                                         
      block data rqmdat                                                 
        include 'scatter7'                                              
        include  'sigmasx'                                              
      data cvalu1,cvalu2,cvalu3,cvalu0,cvalu4,cvalu5,cvalu6,cvalu7/     
     a      1.0,  1.e-35, 0.1,   0.0, 1.e-20, 0.0 , 0.0001, 0.000001/   
      data  emnuc,emdelt,empion,emrho,emeta,widdlt,emstrm,emctst,       
     a      emkap,emka0,emlamb,emsigp,emsig0,emsigm,                    
     b      emkdec,emydec,emrhdc,dltmas,tprd,tpar1,tpar2,tpar3,ekelmi,  
     c      widn1,widn2,widrho,emmnn2,emmnd2,gmprop,rho0,               
     d      cst,cst1,cst2,emnres,wdnres , ekinmi, r0cor,                
     e      hc,pi,pi2, pscal1,pscal2,p0ref/                             
     f      0.938,1.232,.138,0.770,0.550,0.120, 2., 2.5,                
     g      0.494,0.498,1.116,1.189,1.192,1.197,                        
     h      1.15,1.5,1.0,0.3, 1.0,0.650,0.000,0.000,0.05,               
     i      0.1,0.1,0.154,1.3,1.5,0.1,0.16,                             
     j      -2.26,-1.154215,2.725011,1.44,0.2,0.001, 0.88,              
     k      1.2398,  3.1415927, 6.2831854,                              
     l      .238,.318,.227/                                             
      data  enhcnd,snnlim,snpilm/1.5,4.1,1.9/                           
      data kspin /                                                      
     > -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 0, 0, 0, 0,     
     >  0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,-1,-1,     
     >  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3,     
     >  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,     
     > -1,-1, 0, 0, 0, 0, 1, 1, 1, 1 /                                  
c                                                                       
c nucleus surface parameter a: rho(r)=rho0/(exp((r-r0)/a)+1)            
c     da(2) = 0.52                                                      
c                                                                       
c                                                                       
c da(3)                                                                 
c radius parameter, corresponds to 0.17 part/fm**3                      
c (only  needed for  fermi momentum at  rho0).                          
c     da(3) = 1.12                                                      
c                                                                       
c                                                                       
c da(4)                                                                 
c parameter multiplying the life time of all resonances                 
c and the formation time of all new hadrons.                            
c    (input:sepfp)                                                      
c    default:da(4) = 1.                                                 
c                                                                       
c                                                                       
c da(5)                                                                 
c parameter multiplying the total reaction time for switch(23)=t        
c     default:da(5)=1.                                                  
c                                                                       
c                                                                       
c                                                                       
c da(7)                                                                 
c parameter giving the decrease of m-b annihilation x section           
c  sig-annihilation=sig0*(4.0/s)**da(7)                                 
c     default:da(7)=0.5                                                 
c                                                                       
c da(8)                                                                 
c use as a scale factor for squared distances which enter               
c calculation of cross sections                                         
c                                                                       
c da(9)                                                                 
c  nonideal  mixing angle for  phi/omega.                               
c                                                                       
c da(10)                                                                
c  effective nonideal  mixing angle for  phi/omega to  be used in       
c  diffractive  excitation in bb  collisions.                           
c                                                                       
c da(11)                                                                
c input impact parameter is stored in  da(11)                           
c                                                                       
c                                                                       
c da(12)                                                                
c parameter multiplying/suppressing  d n ==> d d cross section          
c (changing the ddpi-coupling constant**2.)                             
c     default:da(12)= 0.25                                              
c                                                                       
c                                                                       
c da(13)                                                                
c     for ida(12)=1: parameter giving the relation between              
c                v12/vnn in the term linear in rho (12=dd,nb,bb).       
c                 (above pr2max)                                        
c                                                                       
c                                                                       
c da(14)                                                                
c     for ida(12)=1: parameter giving the relation between              
c                    vnd/vnn (in the term linear in rho).               
c                 (above pr2max)                                        
c                                                                       
c da(15)                                                                
c     parameter multiplying the r.h.s of 'uncertainty relation'         
c     delta (p +) * delta (x +) = da(15)                                
c     which defines minimum delta(x +) between 2 subsequent collisions. 
c                                                                       
c                                                                       
c da(16)                                                                
c     parameter multiplying rho_ij in the repulsive piece of            
c     quasi-potential for B_1-B_2 combinations for which                
c     attractive piece is modified (see above for da(13) explanation).  
c                 (above pr2max)                                        
c                                                                       
c     da(17)= 0.8                                                       
c...transverse radius of rope                                           
c                                                                       
c                                                                       
c     da(18)= 0.8                                                       
c...splitting probability of spin1 diquarks in string fragmentation.    
c                                                                       
c     da(19)= 0.2                                                       
c...splitting probability of spin0 diquarks in string fragmentation.    
c                                                                       
c     da(20)= 1.0                                                       
c... giving the  probability that in diffractive interaction            
c    the interacting quark ends up in leading hadron                    
c    (=sea  string  excitation).                                        
c                                                                       
csuny011195                                                             
c     da(21)= 1.0                                                       
c     width parameter multiplying n nbar molecule decay width (=50mev)  
c     (for ida(49)=3 option)                                            
c                                                                       
c     da(22)                                                            
c             factor to increase the fireball zone in the switch(17)=f  
c             option                                                    
c             fireball radius= da(22)*1.5 + radius(proj.)               
c                                                                       
c                                                                       
c                                                                       
c                                                                       
c              1  no particle decay, but decay of jet systems           
c                 and resonances (except delta and rho)                 
c    ida(1)=                                                            
c              0  no restriction                                        
c                                                                       
c                                                                       
c                                                                       
c                                                                       
c              1  neglect all collisions with cms energy below 4gev.    
c    ida(2)=                                                            
c              0  no restriction                                        
c                                                                       
c                                                                       
c                                                                       
c              1  nucleons get free masses after elastic scattering.    
c    ida(3)=                                                            
c              0  nucleons get effctv. masses after elastic scattering. 
c                                                                       
c                                                                       
c                                                                       
c                                                                       
c              2  pion absorption according to delta hole               
c                 local density approximation.                          
c    ida(4)=                                                            
c              1  pi n ==> delta, n n ==> n delta: matter modification; 
c                 delta n ==> n n (dlt.-massdependent ope x section)    
c                                                                       
c              0  delta n ==> n n (detailed balance, pp x section)      
c                                                                       
c                                                                       
c              1  print of vector meson decays on unit 10               
c    ida(5)=                                                            
c              0  no                                                    
c                                                                       
c                                                                       
c                                                                       
c              0  global fermi-momentum                                 
c    ida(6)=                                                            
c              1    local density-dependent fermi-momentum              
c              2    dito (with the help of ws distribution)             
c                                                                       
c                                                                       
c              1  retardation in gain of pt for hadron daughter         
c                 carrying cquark is taken into account.                
c    ida(7)=                                                            
c              0    no                                                  
c                                                                       
c                                                                       
c            0  mixed computation of the hamiltonian (4/5 of all time   
c               steps simplified(itrig=1) and 1/5 of all time steps full
c               computation)                                            
c     ida(8)=1  simplified computation of the hamiltonian               
c               (no chi(pion)-components therein).itrig is chosen like  
c               in the case before.                                     
c            2  fully correct computation of the hamiltonian            
c            3  simplified computation of the hamiltonian (itrig=1)     
c                                                                       
c                                                                       
c              1                                                        
c    ida(9) =  0  no                                                    
c                                                                       
c                                                                       
c              1  leading hadron interacts without time delay           
c                 (full x section)                                      
c    ida(10)=  0  no                                                    
c                                                                       
c                                                                       
c              1  only b b collisions                                   
c    ida(11)=  0  no restriction                                        
c                                                                       
c                                                                       
c031096                                                                 
c              1   relative momentum dependence                         
c    ida(12)=                                                           
c              2  no relative momentum dependence                       
c                                                                       
c                                                                       
c                                                                       
c             1 only nucleons and delta feel potential forces           
c    ida(13)=                                                           
c             2 all non-strange baryons  feel potential forces          
c             3 all baryons  feel potential forces                      
c                                                                       
c                                                                       
c                                                                       
c             1  chi(i)= sum(j,j.ne.i) exp(qij***2/2l)*(-)2l/qij**2 *   
c                             em(i)**2*em(j)**2 *qij*pij/sqrt(pij**2)   
c                                                        i=1, ....n-1   
c    ida(14)=                                                           
c             0  chi(i)= sum(j,j.ne.i) exp(qij***2/2l)*(-)2l/qij**2 *   
c                                            qij *pij    i=1, ....nt-1  
c                                                                       
c                                                                       
c                                                                       
c             1 only hadrons containing the original constituent quarks 
c               can interact.                                           
c    ida(15)=                                                           
c             0 default                                                 
c                                                                       
c                                                                       
c                                                                       
c             2/3 string in position space corrected for cnst. quark    
c                 propagation.                                          
c                 (difference between 2 and 3 the same as 0 and 1).     
c    ida(16)=                                                           
c             1 earliest hadron formation point = the first (fictive)   
c               formation point of the yo-yo                            
c             0 arithmetic mean of the two production points            
c                                                                       
c                                                                       
c             1 nucleons are woods-saxon distributed initially          
c    ida(17)=                                                           
c             0 nucleons are uniformly   distributed initially          
c                                                                       
c                                                                       
c             2 finite life time for jet systems with exponentially     
c               decreasing probability.                                 
c             1 finite life time for jet systems below cutoff mass      
c    ida(18)=                                                           
c             0 else                                                    
c                                                                       
c                                                                       
c                                                                       
c            0 b-antib annihilation x sct.= sigma(nnbar)at equal s.     
c    ida(19)=                                                           
c            1 b-antib annihilation x sct.= sigma(nnbar)at equal pr.    
c            2 a la m^* formation,  with vector coefficient coupling.   
c                                                                       
c                                                                       
c              1 leading baryon always contains constituent diquark     
c    ida(20)=  0 constituent diquark can be split up.                   
c                                                                       
c                                                                       
c             1 no finite lifetime (with exceptions given by ida(18)    
c    ida(21)=                                                           
c             0 finite interaction- and lifetime of strings             
c               which are treated like resonances                       
c                                                                       
c                                                                       
c             0 none                                                    
c    ida(22)=                                                           
c             3 triple pomeron diffraction                              
c                                                                       
c                                                                       
c             1 equal time propagation in the ess or ts or ls           
c    ida(23)=                                                           
c             0 switched off                                            
c                                                                       
c                                                                       
c             1 check on available cpu time                             
c    ida(24)=                                                           
c             0 no check on available cpu time                          
c                                                                       
c                                                                       
c             1 collisions/decay information is written on unit 15      
c    ida(25)=                                                           
c             0 no collisions/decay information is written on unit 15   
c                                                                       
c                                                                       
c             2 the same as for 1, but no exception.                    
c             1 h h low and medium energy strangeness production        
c               with general resonance formation and production         
c               scheme (exception: nn). (default)                       
c    ida(26)=                                                           
c             0 h h low and medium energy strangeness production        
c               according to parametrized pp x sections.                
c                                                                       
c                                                                       
c             1 fast initialization (initfa) is switched on             
c    ida(27)=                                                           
c             0 fast initialization (initfa) is switched off            
c                                                                       
c                                                                       
c             0 mass shell correction in relham                         
c    ida(28)=                                                           
c             1 none                                                    
c                                                                       
c                                                                       
c             3 full comp. of hadron positions according to  yoyo model 
c             2 mass dependent formation time based on yoyo model       
c    ida(29)= 1 mass dependent formation time                           
c             0 mass independent formation time                         
c                                                                       
c                                                                       
c             1 variable tau step width                                 
c    ida(30)=                                                           
c             0 old version                                             
c                                                                       
c                                                                       
c             1 minimum (hard core) distance between nclns. in nucleus  
c    ida(31)=                                                           
c             0 none                                                    
c                                                                       
c                                                                       
c             2 gaussian nucleon width= 1.100 fm  (for quasipotential)  
c               + interaction strength takes difference between         
c                 correlated interaction and true baryon density into   
c                 account.                                              
c    ida(32)=                                                           
c             1 gaussian nucleon width= 1.100 fm  (for quasipotential)  
c             0 gaussian nucleon width= 2.165 fm                        
c                                                                       
c                                                                       
c                                                                       
c             1 switched off                                            
c    ida(33)=                                                           
c             0 particles are removed from the interaction region       
c               if no collisions are expected any more (relham).        
c                                                                       
c                                                                       
c                                                                       
c             2 potential forces act only on projectile, secondaries    
c               or nucleons which have already collided in a            
c               woods saxon type mean field of a nucleus.               
c               collision frequency= sigma*v*rho(woods-saxon)           
c             1 potential forces act only on projectile, secondaries    
c               or nucleons which have already collided.                
c               (only in connection with ida(23).ne.0).                 
c    ida(34)=                                                           
c             0 switched off (default)                                  
c                                                                       
c                                                                       
c                                                                       
c            1  resonance potential the same as for nucleons.           
c    ida(35)=                                                           
c            2  for pr2>pr2max: allow for different values              
c               (with ratios given by da(13) and da(14))                
c                                                                       
c                                                                       
c            1  density is computed and printed after every collision   
c               and decay.                                              
c    ida(36)=                                                           
c            0  no                                                      
c                                                                       
c                                                                       
c            1  isotropic scattering in collisions                      
c    ida(37)=                                                           
c            0  no                                                      
c                                                                       
c                                                                       
c            7  output of sources of  cascade on unit 13                
c            5  output of sources of phi on unit 13                     
c    ida(38)=                                                           
c            4  the same as 3,                                          
c               but with excitation point of 'mother particle'.         
c            3  freeze-out point together with source written on unit 9 
c               (code:1 coll, 2 string, 3 b*, 4 rope, 0 nothing,        
c                .ne.0 lund code for mother particle).                  
c            2  output of sources of  lambda on unit 13                 
c            1  output of sources of kaons on unit 13                   
c            0  none                                                    
c                                                                       
c                                                                       
c             1  medium correction for delta propagator in pi n/nn==>nd 
c    ida(39)=                                                           
c             0 none                                                    
c                                                                       
c                                                                       
c                                                                       
c             3  1+2                                                    
c             2  estimate cross section in hit.                         
c    ida(40)=                                                           
c             1  calculate collision possibility with help of z-y       
c                correlation (in hit).                                  
c             0 no                                                      
c                                                                       
c                                                                       
c             1  strange hadrons (k,phi,lambda,sigma,xi,omega) stable.  
c    ida(41)=                                                           
c             0  no                                                     
c                                                                       
c                                                                       
c             1  print phase space info on unit 7(unformatted)          
c    ida(42)=                                                           
c             0  no                                                     
c                                                                       
c                                                                       
c             1  for errex-call: print on unit 14 and stop              
c    ida(43)=                                                           
c             0  no                                                     
c                                                                       
c                                                                       
c             1  sea chain excitation in elementary collisions          
c    ida(44)=                                                           
c             0  no                                                     
c                                                                       
c                                                                       
c             1  modified  lusysj                                       
c    ida(45)=                                                           
c             0  old lund-lusysj                                        
c                                                                       
c                                                                       
c             1  no single diffractive events in hh                     
c    ida(46)=                                                           
c             0  default                                                
c                                                                       
c                                                                       
c             1  string fusion to ropes                                 
c    ida(47)=                                                           
c             0  switched off.                                          
c                                                                       
c                                                                       
c             1  meson-meson interaction via ps-ps-v coupling           
c    ida(48)=                                                           
c             0  switched off.                                          
c                                                                       
c                                                                       
csuny011195                                                             
c             3  molecule formation in l.e. anti-b b annihilation.      
c             2  no anti-b b annihilation.                              
c    ida(49)=                                                           
c             1  diquark formation time = yoyo time  (for ida(16)=0).   
c             0  no                                                     
c                                                                       
c                                                                       
c                high  mass baryon resonances decay  generating         
c             1  an  average pt**2.le.(400mev)**2,                      
c                the standard value of string decay.                    
c    ida(50)=                                                           
c             0  isotropic  decay  of high  mass baryon resonances      
c                                                                       
c                                                                       
c             2  the  same  as 1 and additional effective angle         
c                 for  bb collisions simulating the                     
c                 strange nucleon sea (da(10)).                         
c    ida(51)=                                                           
c             1  nonideal mixing for phi/omega meson given              
c                 by parameter da(9).                                   
c             0  ideal mixing for phi/omega meson.                      
c                                                                       
c                                                                       
c             0  no restriction                                         
c    ida(52)=                                                           
c             1  target nucleons without collisions are not written     
c                for ha collisions on unit 9.                           
c             2  ta/pr nucleons without collisions,pi0's, neutrons      
c                are not written                                        
c                on unit 9.                                             
c                                                                       
c                                                                       
c             0  hh coupling, ssbar suppression,phase space given       
c                by maximum pt.                                         
c    ida(53)=                                                           
c             1  Vandermeulen model of hh coupling.                     
c             2  Munigl model of hh coupling. (not implemented yet)     
c                                                                       
c                                                                       
c             1 xs correction for shortliving resonances                
c               in mb==> n*,delta*                                      
c    ida(54)=                                                           
c             0  no correction                                          
c                                                                       
c                                                                       
c             2 s quark motion in mb==>y,xi,om ==>mb                    
c               is reggeized.                                           
c    ida(55)=                                                           
c             1                                                         
c             0                                                         
c                                                                       
c                                                                       
c                                                                       
c300198                                                                 
c             1                                                         
c    ida(56)=      disabled                                             
c             0                                                         
c                                                                       
c                                                                       
c             1 check whether time according to uncertainty             
c               relation is available before next 'hard'                
c               collision, else forbidden.                              
c    ida(57)=                                                           
c             0 none                                                    
c                                                                       
c                                                                       
c    ida(58)  different parametrizations for kbar lambda                
c                                                                       
      data da/ 0.0,0.52,1.12,1.0,1.0,1.0,0.5,1.0,41.0,35.3,             
     a               0.0,0.25,-0.1,1.0,1.0,1.00,0.8,0.8,0.2,1.0,        
     b                  1.0,1.0/                                        
      data ida/  0,0,1,1,3,2,1,3,1,0,                                   
     a             0,1,3,0,0,2,1,2,2,0,                                 
     b               1,3,1,2,1,2,1,0,3,1,                               
     c                 1,2,0,0,2,2,0,3,1,1,                             
     d                   1,0,1,1,1,0,0,1,3,1,                           
     e                     1,1,0,1,2,0,1,0,2*0 /                        
      data spntot/                                                      
     @248.20, 93.38, 55.26, 44.50, 41.33, 38.48, 37.20, 35.98,          
     @ 35.02, 34.47, 34.37, 34.67, 35.23, 35.97, 36.75, 37.37,          
     @ 37.77, 38.03, 38.40, 38.83, 39.26, 39.67, 40.06, 40.45,          
     @ 40.79, 41.06, 41.31, 41.52, 41.70, 41.81, 41.87, 41.98,          
     @ 42.12, 42.29, 42.55, 42.82, 43.01, 43.12, 43.16, 43.14,          
     @ 43.06, 42.95, 42.81, 42.67, 42.54, 42.45, 42.38, 42.33,          
     @ 42.30, 42.29, 42.28, 42.26, 42.24, 42.21, 42.17, 42.14,          
     @ 42.10, 42.07, 42.06, 42.05, 42.04, 42.03, 42.02, 42.00,          
     @ 41.97, 41.94, 41.89, 41.84, 41.79, 41.73, 41.67, 41.61,          
     @ 41.55, 41.49, 41.44, 41.38, 41.34, 41.31, 41.29, 41.28,          
     @ 41.27, 41.28, 41.30, 41.33, 41.36, 41.40, 41.44, 41.49,          
     @ 41.50, 41.51, 41.51, 41.51, 41.52, 41.51, 41.51, 41.50,          
     @ 41.50, 41.49, 41.47, 41.46                                       
     @/                                                                 
      data spnel /                                                      
     @248.20, 93.38, 55.26, 44.50, 41.33, 38.48, 37.20, 35.80,          
     @ 34.71, 33.43, 32.52, 31.54, 30.65, 30.34, 29.14, 27.22,          
     @ 26.93, 26.18, 25.64, 25.16, 24.89, 24.49, 24.08, 23.86,          
     @ 23.17, 22.70, 21.88, 21.48, 20.22, 19.75, 18.97, 18.39,          
     @ 17.98, 17.63, 17.21, 16.72, 16.68, 16.58, 16.42, 16.22,          
     @ 15.98, 15.71, 15.42, 15.14, 14.87, 14.65, 14.44, 14.26,          
     @ 14.10, 13.95, 13.80, 13.64, 13.47, 13.29, 13.09, 12.89,          
     @ 12.68, 12.47, 12.27, 12.06, 11.84, 11.76, 11.69, 11.60,          
     @ 11.50, 11.41, 11.29, 11.17, 11.06, 10.93, 10.81, 10.68,          
     @ 10.56, 10.44, 10.33, 10.21, 10.12, 10.03,  9.96,  9.89,          
     @  9.83,  9.80,  9.77,  9.75,  9.74,  9.74,  9.74,  9.76,          
     @  9.73,  9.70,  9.68,  9.65,  9.63,  9.60,  9.57,  9.55,          
     @  9.52,  9.49,  9.46,  9.43                                       
     @/                                                                 
      data spptot/                                                      
     @ 39.48, 31.76, 26.26, 24.05, 23.94, 23.77, 23.74, 24.13,          
     @ 25.09, 26.23, 28.04, 32.30, 34.93, 37.85, 41.47, 44.73,          
     @ 46.73, 47.33, 47.29, 47.49, 47.50, 47.37, 47.54, 47.39,          
     @ 47.38, 47.38, 47.22, 47.14, 46.91, 46.60, 46.39, 45.80,          
     @ 45.47, 45.54, 45.28, 45.00, 44.95, 44.74, 44.32, 44.21,          
     @ 43.86, 43.63, 43.50, 43.04, 43.01, 42.79, 42.65, 42.44,          
     @ 42.20, 42.02, 41.87, 41.75, 41.64, 41.53, 41.43, 41.33,          
     @ 41.24, 41.16, 41.09, 41.03, 40.97, 40.90, 40.83, 40.75,          
     @ 40.67, 40.59, 40.51, 40.45, 40.39, 40.34, 40.29, 40.25,          
     @ 40.22, 40.19, 40.17, 40.15, 40.13, 40.11, 40.10, 40.08,          
     @ 40.06, 40.04, 40.02, 40.00, 39.98, 39.95, 39.93, 39.91,          
     @ 39.92, 39.92, 39.92, 39.92, 39.92, 39.92, 39.91, 39.90,          
     @ 39.89, 39.88, 39.86, 39.84                                       
     @/                                                                 
      data sppel /                                                      
     @ 39.48, 31.76, 26.26, 24.05, 23.94, 23.77, 23.72, 23.98,          
     @ 24.48, 24.52, 24.48, 26.24, 26.01, 26.01, 26.90, 27.78,          
     @ 27.80, 26.82, 25.50, 24.64, 23.66, 22.46, 22.04, 21.42,          
     @ 21.05, 20.76, 20.39, 20.14, 19.79, 19.39, 19.12, 18.49,          
     @ 18.13, 18.19, 17.92, 17.63, 17.58, 17.37, 16.95, 16.83,          
     @ 16.48, 16.24, 16.09, 15.62, 15.57, 15.32, 15.15, 14.91,          
     @ 14.64, 14.42, 14.22, 14.06, 13.90, 13.73, 13.58, 13.42,          
     @ 13.27, 13.13, 13.00, 12.88, 12.75, 12.62, 12.48, 12.34,          
     @ 12.19, 12.04, 11.90, 11.77, 11.64, 11.53, 11.41, 11.31,          
     @ 11.22, 11.13, 11.05, 10.97, 10.89, 10.82, 10.75, 10.68,          
     @ 10.61, 10.54, 10.48, 10.41, 10.35, 10.28, 10.22, 10.16,          
     @ 10.13, 10.10, 10.08, 10.05, 10.02,  9.99,  9.96,  9.93,          
     @  9.90,  9.87,  9.84,  9.80                                       
     @/                                                                 
       data spbrpt/                                                     
     @ 253.118,223.598,195.010,174.600,158.832,147.941,139.813,137.000, 
     @ 134.904,133.381,131.843,130.142,128.223,126.285,124.389,122.569, 
     @ 120.731,119.171,117.910,116.637,115.350,113.752,112.018,110.266, 
     @ 109.063,107.875,106.675,105.461,104.232,102.991,101.737,100.189, 
     @  98.600, 96.996, 95.562, 94.249, 92.922, 91.630, 90.413, 89.184, 
     @  87.942, 86.383, 84.423, 82.444, 81.044, 80.322, 79.593, 77.903, 
     @  75.395, 74.128, 72.849, 70.948, 69.467, 68.426, 67.832, 67.233, 
     @  66.513, 65.691, 64.860, 64.147, 63.722, 63.293, 62.860, 62.422, 
     @  62.000, 61.627, 61.250, 60.869, 60.485, 60.097, 59.705, 59.371, 
     @  59.034, 58.703, 58.378, 58.059, 57.746, 57.439, 57.138, 56.842, 
     @  56.552, 56.266, 55.987, 55.712, 55.442, 55.177, 54.916, 54.660, 
     @  54.409, 54.162, 53.919, 53.681, 53.447, 53.217, 52.991, 52.768, 
     @  52.550, 52.335, 52.124, 51.917                                  
     @  /                                                               
       data spbrpe/                                                     
     @  72.937, 64.944, 58.213, 55.509, 52.919, 51.639, 50.346, 49.040, 
     @  47.820, 46.613, 45.394, 44.314, 43.285, 42.245, 41.293, 40.391, 
     @  39.480, 38.629, 37.803, 36.970, 36.128, 35.192, 34.225, 33.252, 
     @  32.370, 31.479, 30.579, 29.879, 29.422, 28.960, 28.493, 27.870, 
     @  27.236, 26.594, 25.947, 25.288, 24.621, 23.947, 23.270, 22.589, 
     @  21.901, 21.206, 20.769, 20.479, 20.186, 19.889, 19.636, 19.386, 
     @  19.133, 18.879, 18.511, 18.044, 17.572, 17.656, 17.371, 17.095, 
     @  16.828, 16.570, 16.319, 16.077, 15.843, 15.615, 15.395, 15.181, 
     @  14.974, 14.773, 14.578, 14.388, 14.205, 14.026, 13.852, 13.684, 
     @  13.520, 13.361, 13.206, 13.055, 12.909, 12.766, 12.627, 12.492, 
     @  12.361, 12.233, 12.108, 11.987, 11.869, 11.754, 11.641, 11.532, 
     @  11.425, 11.321, 11.220, 11.121, 11.024, 10.930, 10.839, 10.749, 
     @  10.662, 10.576, 10.493, 10.412                                  
     @  /                                                               
      data sdnel1/                                                      
     @   0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000, 
     @   0.000,  0.000,  0.000,  0.000,  0.000, 70.000, 55.067, 42.486, 
     @  35.025, 29.261, 26.552, 23.592, 21.345, 19.601, 19.213, 18.832, 
     @  18.460, 18.094, 17.736, 17.384, 17.040, 16.703, 16.372, 16.048, 
     @  15.730, 15.419, 15.113, 14.814, 14.521, 14.233, 13.951, 13.675, 
     @  13.404, 13.139, 12.879, 12.624, 12.374, 12.129, 11.889, 11.653, 
     @  11.422, 11.196, 10.975, 10.757, 10.544, 10.335, 10.131,  9.930, 
     @   9.734,  9.541,  9.352,  9.167,  8.985,  8.807,  8.633,  8.462, 
     @   8.294,  8.130,  7.969,  7.811,  7.657,  7.505,  7.356,  7.211, 
     @   7.068,  6.928,  6.791,  6.656,  6.525,  6.395,  6.269,  6.145, 
     @   6.023,  5.904,  5.787,  5.672,  5.560,  5.450,  5.342,  5.236, 
     @   5.132,  5.031,  4.931,  4.834,  4.738,  4.644,  4.552,  4.462, 
     @   4.374,  4.287,  4.202,  4.119                                  
     @ /                                                                
      data sdnel2/                                                      
     @   0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000, 
     @   0.000,  0.000,  0.000,  0.000,  0.000,230.000,180.070,141.345, 
     @ 110.116, 88.757, 62.962, 54.483, 52.379, 52.134, 51.103, 50.090, 
     @  49.098, 48.126, 47.173, 46.240, 45.323, 44.426, 43.546, 42.683, 
     @  41.839, 41.009, 40.198, 39.403, 38.622, 37.857, 37.108, 36.373, 
     @  35.654, 34.947, 34.255, 33.576, 32.912, 32.260, 31.620, 30.995, 
     @  30.382, 29.780, 29.189, 28.611, 28.045, 27.490, 26.945, 26.412, 
     @  25.889, 25.376, 24.874, 24.381, 23.899, 23.426, 22.961, 22.507, 
     @  22.062, 21.624, 21.196, 20.777, 20.364, 19.963, 19.567, 19.179, 
     @  18.799, 18.427, 18.061, 17.706, 17.354, 17.011, 16.674, 16.343, 
     @  16.020, 15.702, 15.392, 15.086, 14.789, 14.495, 14.208, 13.927, 
     @  13.652, 13.381, 13.117, 12.856, 12.602, 12.353, 12.107, 11.868, 
     @  11.633, 11.402, 11.176, 10.956                                  
     @  /                                                               
      data sdnav/                                                       
     @   0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000, 
     @   0.000,  0.000,  0.000,  0.000,  0.000,177.900,133.194,104.273, 
     @  81.957, 66.446, 49.308, 42.899, 40.741, 39.934, 39.144, 38.368, 
     @  37.609, 36.864, 36.134, 35.419, 34.717, 34.030, 33.356, 32.695, 
     @  32.048, 31.413, 30.791, 30.182, 29.584, 28.998, 28.424, 27.861, 
     @  27.310, 26.769, 26.239, 25.719, 25.210, 24.711, 24.221, 23.742, 
     @  23.272, 22.811, 22.359, 21.916, 21.482, 21.057, 20.640, 20.231, 
     @  19.831, 19.438, 19.053, 18.676, 18.306, 17.944, 17.588, 17.240, 
     @  16.899, 16.564, 16.236, 15.915, 15.599, 15.291, 14.988, 14.691, 
     @  14.400, 14.115, 13.835, 13.562, 13.293, 13.030, 12.772, 12.519, 
     @  12.271, 12.028, 11.790, 11.556, 11.328, 11.103, 10.883, 10.668, 
     @  10.457, 10.250, 10.047,  9.848,  9.653,  9.462,  9.274,  9.091, 
     @   8.911,  8.734,  8.561,  8.392                                  
     @   /                                                              
      data snnnd /                                                      
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.15,          
     @  0.61,  1.71,  3.56,  6.06,  8.92, 11.01, 12.76, 14.40,          
     @ 16.06, 17.74, 19.35, 20.82, 22.10, 23.14, 23.95, 24.53,          
     @ 24.89, 25.06, 25.06, 24.92, 24.66, 24.31, 23.87, 23.37,          
     @ 22.82, 22.23, 21.92, 21.00, 19.53, 18.99, 18.40, 17.87,          
     @ 17.27, 16.98, 17.02, 16.29, 16.54, 17.25, 16.96, 16.97,          
     @ 15.10, 14.66, 13.70, 11.74, 10.94, 10.56,  9.90,  9.41,          
     @  8.93,  8.46,  7.99,  7.53,  7.08,  6.64,  6.20,  5.77,          
     @  5.36,  4.95,  4.55,  4.16,  3.79,  3.43,  3.08,  2.74,          
     @  8.10,  7.83,  6.96,  5.70,  4.59,  3.53,  2.60,  1.75,          
     @  1.19,  0.64,  0.19,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.00                                       
     @/                                                                 
      data sdndd /                                                      
     @   0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000, 
     @   0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000, 
     @   0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000, 
     @   0.000,  0.000,  0.000, 33.013, 45.444, 51.728, 54.743, 55.804, 
     @  55.647, 54.719, 53.305, 51.587, 49.691, 47.699, 45.670, 43.643, 
     @  41.644, 39.692, 37.799, 35.973, 34.217, 32.535, 30.928, 29.394, 
     @  27.933, 26.542, 25.218, 23.963, 22.770, 21.639, 20.564, 19.543, 
     @  18.576, 17.657, 16.786, 15.960, 15.176, 14.432, 13.728, 13.059, 
     @  12.425, 11.823, 11.251, 10.709, 10.194,  9.704,  9.240,  8.799, 
     @   8.379,  7.981,  7.603,  7.243,  6.901,  6.576,  6.266,  5.972, 
     @   5.692,  5.426,  5.173,  4.931,  4.702,  4.484,  4.276,  4.078, 
     @   3.890,  3.710,  3.539,  3.376,  3.221,  3.072,  2.932,  2.797, 
     @   2.669,  2.547,  2.431,  2.320                                  
     @/                                                                 
      data snpid /                                                      
     @   0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  2.647, 
     @  10.881, 21.678, 52.692, 97.428,163.619,196.966,165.886,118.453, 
     @  82.930, 59.727, 53.996, 44.465, 38.289, 31.868, 28.052, 24.537, 
     @  21.569, 19.602, 18.549, 18.329, 19.177, 20.592, 22.994, 25.367, 
     @  26.443, 26.995, 28.052, 29.497, 31.558, 33.698, 36.171, 39.101, 
     @  40.857, 41.363, 39.794, 37.616, 34.792, 31.917, 30.886, 30.223, 
     @  29.791, 29.522, 29.501, 29.480, 29.459, 29.656, 29.725, 30.517, 
     @  30.036, 29.633, 29.232, 31.200, 30.571, 29.948, 29.553, 29.450, 
     @  29.347, 29.243, 29.138, 29.031, 28.924, 28.815, 28.706, 28.595, 
     @  28.483, 28.370, 28.256, 28.141, 28.025, 27.907, 27.788, 27.669, 
     @  27.547, 27.425, 27.302, 27.177, 27.051, 26.923, 26.795, 26.665, 
     @  26.534, 26.401, 26.267, 26.132, 25.995, 25.858, 25.718, 25.578, 
     @  25.435, 25.292, 25.147, 25.000                                  
     @/                                                                 
      data sppipe /                                                     
     @   0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  2.647, 
     @  10.881, 21.678, 52.692, 97.428,163.619,196.966,165.886,118.453, 
     @  82.930, 59.727, 53.996, 44.465, 38.289, 31.868, 28.052, 24.537, 
     @  21.569, 19.602, 16.549, 13.763, 11.959,  9.773,  8.932,  8.928, 
     @   9.178, 10.576, 10.799, 11.626, 12.359, 13.100, 13.876, 14.811, 
     @  15.756, 16.922, 18.900, 17.130, 15.074, 13.172, 11.573, 10.787, 
     @  10.059,  9.594,  9.230,  8.862,  8.511,  8.311,  8.126,  7.939, 
     @   7.750,  7.470,  7.231,  7.062,  6.890,  6.717,  6.542,  6.332, 
     @   6.209,  6.120,  6.030,  5.940,  5.817,  5.678,  5.537,  5.394, 
     @   5.250,  5.105,  5.081,  5.081,  5.081,  5.081,  5.081,  5.017, 
     @   4.916,  4.815,  4.712,  4.624,  4.563,  4.501,  4.439,  4.377, 
     @   4.328,  4.279,  4.244,  4.244,  4.244,  4.216,  4.170,  4.123, 
     @   4.076,  4.028,  3.979,  3.931                                  
     @/                                                                 
      data spimp /                                                      
     @   0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.741, 
     @   3.658,  7.251, 17.543, 32.820, 54.558, 65.232, 55.765, 44.780, 
     @  35.167, 31.693, 30.300, 29.634, 29.891, 30.327, 31.131, 34.146, 
     @  38.955, 46.808, 49.792, 44.286, 38.373, 37.016, 39.269, 51.126, 
     @  61.069, 57.527, 48.820, 41.953, 37.497, 35.609, 35.141, 35.207, 
     @  35.054, 35.031, 34.753, 34.841, 35.126, 35.405, 35.619, 35.906, 
     @  36.275, 36.311, 36.250, 36.173, 35.921, 35.741, 35.654, 35.317, 
     @  34.904, 34.665, 34.176, 33.631, 33.273, 32.843, 32.355, 32.235, 
     @  32.114, 31.991, 31.867, 31.742, 31.616, 31.488, 31.359, 31.229, 
     @  31.097, 30.965, 30.830, 30.695, 30.558, 30.420, 30.280, 30.139, 
     @  29.997, 29.853, 29.707, 29.560, 29.412, 29.262, 29.111, 28.958, 
     @  28.804, 28.648, 28.491, 28.332, 28.171, 28.009, 27.845, 27.679, 
     @  27.512, 27.343, 27.173, 27.000                                  
     @/                                                                 
      data sppime /                                                     
     @   0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.247, 
     @   1.219,  2.417,  5.848, 10.940, 18.186, 21.744, 21.862, 16.804, 
     @  12.325, 10.551, 10.024, 10.371, 11.443, 12.667, 13.828, 15.032, 
     @  16.914, 18.080, 19.002, 17.334, 15.318, 14.823, 17.115, 23.610, 
     @  27.137, 26.694, 21.230, 17.699, 15.131, 13.094, 11.221, 10.181, 
     @  10.162,  9.477,  9.604,  9.589,  9.449,  9.391,  9.363,  9.207, 
     @   8.822,  8.245,  8.106,  7.985,  7.880,  7.774,  7.667,  7.456, 
     @   7.248,  7.171,  7.093,  7.015,  6.936,  6.902,  6.736,  6.526, 
     @   6.314,  6.140,  5.987,  5.832,  5.676,  5.519,  5.630,  5.544, 
     @   5.458,  5.370,  5.254,  5.130,  5.004,  4.877,  4.834,  4.834, 
     @   4.834,  4.834,  4.834,  4.799,  4.776,  4.752,  4.728,  4.688, 
     @   4.631,  4.573,  4.515,  4.456,  4.397,  4.336,  4.275,  4.222, 
     @   4.171,  4.120,  4.068,  4.016                                  
     @/                                                                 
      data sppnn2/                                                      
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.39,  0.40,  0.44,          
     @  0.51,  0.61,  0.74,  0.89,  1.06,  1.26,  1.47,  1.70,          
     @  1.94,  2.20,  2.47,  2.75,  3.04,  3.34,  3.64,  3.95,          
     @  4.26,  4.58,  4.89,  5.20,  5.51,  5.82,  6.12,  6.41,          
     @  6.69,  6.96,  7.23,  7.47,  7.71,  7.93,  8.13,  8.31,          
     @  8.48,  8.63,  8.76,  8.86,  8.95,  9.02,  9.06,  9.09,          
     @  9.09,  9.07,  9.03,  8.97,  8.89,  8.80,  8.68,  8.55,          
     @  8.40,  8.23,  8.05,  7.86,  7.66,  7.45,  7.23,  7.00,          
     @  6.77,  6.53,  6.29,  6.05                                       
     @/                                                                 
      data snpnn2/                                                      
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.04,  0.07,  0.11,  0.16,  0.24,  0.86,          
     @  1.46,  2.19,  2.95,  3.29,  3.60,  3.91,  4.22,  4.53,          
     @  4.84,  5.14,  5.43,  5.71,  5.98,  6.23,  6.46,  6.66,          
     @  6.85,  7.01,  7.15,  7.27,  7.65,  7.70,  7.73,  7.74,          
     @  7.73,  7.71,  7.68,  7.64,  7.59,  7.53,  7.46,  7.39,          
     @  7.31,  7.23,  7.15,  7.06,  6.97,  6.89,  6.80,  6.71,          
     @  6.63,  6.55,  6.46,  6.38,  6.30,  6.22,  6.15,  6.07,          
     @  6.00,  5.93,  5.86,  5.79,  5.73,  5.66,  5.59,  5.53,          
     @  5.46,  5.40,  5.33,  5.27,  5.20,  5.14,  5.07,  5.00,          
     @  4.93,  4.86,  4.79,  4.71,  4.64,  4.56,  4.48,  4.41,          
     @  4.33,  4.25,  4.16,  4.08,  4.00,  3.92,  3.83,  3.75,          
     @  3.67,  3.59,  3.51,  3.43                                       
     @/                                                                 
      data sppdd/                                                       
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.10,  0.24,  0.45,  0.73,  0.97,  1.24,          
     @  1.45,  1.71,  1.97,  2.31,  2.72,  0.38,  0.82,  1.43,          
     @  2.10,  2.91,  3.39,  4.40,  5.25,  5.32,  5.76,  6.06,          
     @  6.37,  6.34,  5.94,  6.25,  5.57,  4.70,  4.66,  4.10,          
     @  5.30,  5.30,  5.70,  6.90,  7.23,  6.98,  6.98,  6.83,          
     @  6.64,  6.47,  6.29,  6.11,  5.94,  5.79,  5.64,  5.49,          
     @  5.37,  5.26,  5.17,  5.08,  5.00,  4.96,  4.92,  4.90,          
     @  0.00,  0.00,  0.54,  2.80,  4.74,  6.56,  7.99,  8.90,          
     @ 10.10, 10.50, 11.90, 12.90, 15.10, 15.70, 17.06, 18.30,          
     @ 18.86, 20.70, 21.10, 21.10, 21.50, 21.50, 21.90, 21.10,          
     @ 21.10, 21.50, 21.50, 20.70                                       
     @/                                                                 
      data s0dd/                                                        
     @   0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000, 
     @   0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000, 
     @   0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000, 
     @   0.000,  0.000,  1.043,  1.836,  3.270,  5.313,  6.694,  7.662, 
     @   8.281,  8.773,  9.416, 10.229,  9.659, 10.130, 10.387, 10.775, 
     @  11.016, 10.773, 10.788, 10.363, 10.620, 10.450, 10.035, 10.155, 
     @   9.181,  8.754,  8.164,  7.263,  6.640,  6.596,  6.401,  6.306, 
     @   6.300,  6.185,  6.080,  5.945,  5.771,  5.627,  5.464,  5.282, 
     @   5.080,  4.860,  4.610,  4.402,  4.104,  3.818,  3.503,  3.139, 
     @  14.406, 16.224, 17.343, 14.854, 12.785,  9.898,  7.032,  4.007, 
     @   2.514,  0.781,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000, 
     @   0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000, 
     @   0.000,  0.000,  0.000,  0.000                                  
     @/                                                                 
      data sppdn2/                                                      
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.00,  1.00,  2.57,  2.67,  2.65,          
     @  2.58,  2.42,  2.29,  2.21,  2.77,  2.89,  3.04,  3.24,          
     @  3.47,  3.72,  3.98,  4.27,  4.56,  4.54,  4.71,  5.06,          
     @  5.48,  5.70,  6.02,  6.51,  6.75,  6.99,  7.15,  7.27,          
     @  7.39,  7.49,  7.57,  7.63,  7.67,  7.69,  7.69,  7.68,          
     @  7.64,  7.57,  7.48,  7.38,  7.26,  7.12,  6.96,  6.78,          
     @  4.01,  3.80,  3.61,  3.34,  3.08,  2.85,  2.68,  2.59,          
     @  2.33,  2.20,  1.91,  1.67,  1.17,  0.94,  0.54,  0.38,          
     @  0.05,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.00                                       
     @/                                                                 
      data spp2n2/                                                      
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.14,  0.39,  0.66,          
     @  0.95,  1.24,  1.55,  1.87,  2.19,  2.53,  2.88,  3.24,          
     @  3.58,  3.96,  4.33,  4.72,  5.11,  5.50,  5.90,  6.31,          
     @  8.66,  9.05,  9.46,  8.68,  8.07,  7.49,  7.17,  7.22,          
     @  6.87,  7.18,  6.60,  6.14,  4.55,  4.30,  3.50,  2.56,          
     @  2.50,  0.87,  0.64,  0.81,  0.59,  0.78,  0.58,  1.56,          
     @  1.77,  1.58,  1.78,  2.77                                       
     @/                                                                 
      data snp2n2/                                                      
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.14,  0.56,  0.89,  1.45,  1.67,  2.08,  2.60,  2.84,          
     @  3.64,  4.14,  4.73,  5.51,  6.14,  6.56,  7.14,  7.68,          
     @  8.19,  8.75,  9.32,  9.91, 10.53, 10.99, 11.45, 11.95,          
     @ 12.45, 12.96, 13.51, 14.04, 14.59, 15.15, 15.72, 16.31,          
     @ 11.97, 11.43, 11.26, 12.27, 13.13, 14.41, 15.77, 17.39,          
     @ 18.03, 19.09, 19.65, 19.67, 19.01, 18.96, 18.63, 18.24,          
     @ 18.25, 17.52, 17.44, 17.55, 17.47, 17.58, 17.49, 17.98,          
     @ 18.08, 17.99, 18.09, 18.57                                       
     @/                                                                 
      data sks10/                                                       
     @   7.80,  7.19,  6.49,  6.02,  5.55,  5.08,  4.64,  4.38,         
     @ 3.654, 3.741, 3.905, 4.198, 5.010, 5.745, 6.429, 6.685,          
     @ 6.733, 5.920, 4.935, 3.869, 3.589, 3.137, 2.660, 2.259,          
     @ 1.955, 1.760, 1.733, 1.846, 1.748, 1.650, 1.642, 1.633,          
     @   1.44,  1.10,  0.85,  0.68,  0.59,  0.53,  0.48,  0.44,         
     @   0.41,  0.38,  0.36,  0.34,  0.33,  0.31,  0.29,  0.28,         
     @   0.27,  0.26,  0.24,  0.23,  0.21,  0.20,  0.20,  0.19,         
     @   0.18,  0.17,  0.16,  0.15,  0.14,  0.13,  0.12,  0.12,         
     @   0.11,  0.11,  0.10,  0.10,  0.09,  0.09,  0.08,  0.08,         
     @   0.08,  0.07,  0.07,  0.07,  0.07,  0.06,  0.06,  0.06,         
     @   0.05,  0.05,  0.05,  0.04,  0.04,  0.04,  0.03,  0.03,         
     @   0.03,  0.03,  0.02,  0.02,  0.02,  0.01,  0.01,  0.01,         
     @   0.01,  0.01,  0.01,  0.008                                     
     @/                                                                 
      data sks01/                                                       
     @40.719,36.695,30.366,27.314,50.764,27.079,19.306,13.094,          
     @  13.50, 11.81, 10.48,  9.34,  8.36,  7.46,  6.79,  6.16,         
     @   5.65,  5.24,  4.84,  4.46,  4.15,  3.87,  3.63,  3.40,         
     @   3.18,  2.96,  2.77,  2.58,  2.39,  2.26,  2.13,  2.02,         
     @   1.92,  1.82,  1.70,  1.58,  1.48,  1.39,  1.31,  1.22,         
     @   1.15,  1.07,  1.00,  0.94,  0.89,  0.85,  0.81,  0.77,         
     @   0.73,  0.70,  0.67,  0.64,  0.61,  0.57,  0.54,  0.50,         
     @   0.47,  0.45,  0.43,  0.41,  0.39,  0.37,  0.36,  0.34,         
     @   0.33,  0.31,  0.30,  0.28,  0.26,  0.25,  0.24,  0.23,         
     @   0.22,  0.21,  0.20,  0.19,  0.18,  0.18,  0.17,  0.16,         
     @   0.15,  0.14,  0.13,  0.12,  0.11,  0.10,  0.09,  0.08,         
     @   0.08,  0.07,  0.06,  0.06,  0.05,  0.05,  0.04,  0.04,         
     @   0.04,  0.03,  0.03,  0.03                                      
     @/                                                                 
      data sks11/                                                       
     @29.238,13.759, 9.522, 6.904, 5.373, 4.491, 3.759, 3.349,          
     @ 3.245, 3.517, 5.862,  8.158, 9.292, 5.622,  3.84,  3.20,         
     @   2.91,  2.62,  2.38,  2.19,  2.00,  1.83,  1.68,  1.52,         
     @   1.41,  1.31,  1.22,  1.12,  1.05,  0.98,  0.91,  0.85,         
     @   0.80,  0.75,  0.70,  0.65,  0.61,  0.57,  0.53,  0.50,         
     @   0.46,  0.43,  0.40,  0.37,  0.34,  0.32,  0.30,  0.29,         
     @   0.27,  0.25,  0.24,  0.23,  0.21,  0.20,  0.19,  0.18,         
     @   0.17,  0.16,  0.15,  0.13,  0.12,  0.11,  0.10,  0.09,         
     @   0.08,  0.07,  0.06,  0.06,  0.05,  0.05,  0.04,  0.04,         
     @   0.04,  0.03,  0.03,  0.03,  0.03,  0.03,  0.02,  0.02,         
     @   0.02,  0.02,  0.01,  0.01,  0.01,  0.01,  0.01,  0.01,         
     @   12*0.0                                                         
     @/                                                                 
      data skncs/                                                       
     @-500.00,274.34, 92.01, 40.21, 26.16, 20.26, 17.24, 15.43,         
     @  15.40, 15.85, 16.44, 17.28, 20.91, 24.08, 30.82, 32.02,         
     @  33.09, 33.95, 34.71, 34.05, 33.38, 32.71, 32.07, 31.57,         
     @  31.07, 30.55, 30.02, 29.49, 28.94, 28.39, 27.82, 27.25,         
     @  26.66, 26.06, 25.45, 24.82, 24.18, 23.70, 23.66, 23.61,         
     @  23.56, 23.51, 23.46, 23.41, 23.36, 23.31, 23.16, 22.99,         
     @  22.82, 22.65, 22.47, 22.36, 22.25, 22.14, 22.03, 21.91,         
     @  21.79, 21.68, 21.60, 21.56, 21.52, 21.47, 21.43, 21.39,         
     @  21.34, 21.30, 21.25, 21.27, 21.33, 21.38, 21.44, 21.49,         
     @  21.55, 21.61, 21.67, 21.79, 21.93, 22.08, 22.23, 22.39,         
     @  22.55, 22.37, 22.11, 21.85, 21.58, 21.31, 21.25, 21.19,         
     @  21.13, 21.07, 21.01, 20.94, 20.99, 21.05, 21.12, 21.19,         
     @  21.26, 21.33, 21.32, 21.28                                      
     @/                                                                 
      data skmne/                                                       
     @   0.00,  0.00,  0.00,  0.34,  1.31,  2.43,  3.33,  4.19,         
     @   4.85,  5.60,  6.44,  7.38,  8.42,  9.62, 10.93, 13.87,         
     @  16.84, 19.40, 19.37, 15.89, 14.38, 13.51, 12.92, 12.32,         
     @  11.70, 11.08, 10.44,  9.79,  9.13,  8.46,  7.77,  7.13,         
     @   6.97,  6.80,  6.64,  6.47,  6.29,  6.11,  5.93,  5.75,         
     @   5.56,  5.37,  5.17,  4.97,  4.76,  4.55,  4.34,  4.18,         
     @   4.12,  4.07,  4.01,  3.95,  3.89,  3.83,  3.77,  3.71,         
     @   3.64,  3.58,  3.51,  3.44,  3.37,  3.33,  3.30,  3.28,         
     @   3.27,  3.26,  3.25,  3.25,  3.24,  3.23,  3.22,  3.21,         
     @   3.21,  3.20,  3.19,  3.18,  3.17,  3.16,  3.15,  3.14,         
     @   3.13,  3.12,  3.11,  3.10,  3.09,  3.08,  3.07,  3.05,         
     @   3.04,  3.03,  3.02,  3.01,  2.99,  2.98,  2.97,  2.95,         
     @   2.94,  2.92,  2.91,  2.89                                      
     @/                                                                 
      data skmpce/                                                      
     @  17.98, 11.37,  8.01,  7.63, 11.01,  5.88,  4.63,  4.15,         
     @   3.88,  3.70,  3.35,  2.35,  3.20,  4.20,  4.56,  4.90,         
     @   5.69,  6.93,  7.71,  6.33,  4.76,  3.92,  3.00,  2.50,         
     @  2.027, 1.892, 1.768, 1.655, 1.551, 1.455, 1.367, 1.285,         
     @  1.209, 1.139, 1.074, 1.013, 0.956, 0.903, 0.854, 0.808,         
     @  0.765, 0.724, 0.687, 0.651, 0.618, 0.586, 0.557, 0.529,         
     @  0.502, 0.478, 0.454, 0.432, 0.411, 0.392, 0.373, 0.355,         
     @  0.339, 0.323, 0.308, 0.294, 0.280, 0.268, 0.255, 0.244,         
     @  0.233, 0.223, 0.213, 0.203, 0.194, 0.186, 0.178, 0.170,         
     @  0.163, 0.156, 0.149, 0.142, 0.136, 0.131, 0.125, 0.120,         
     @  0.115, 0.110, 0.105, 0.101, 0.097, 0.093, 0.089, 0.085,         
     @  0.082, 0.078, 0.075, 0.072, 0.069, 0.066, 0.064, 0.061,         
     @  0.059, 0.056, 0.054, 0.052/                                     
      data skpcs/                                                       
     @ -500.0,344.87,163.85, 99.63, 76.39, 83.06, 63.93, 46.09,         
     @  41.51, 38.84, 36.44, 35.36, 36.05, 37.62, 40.17, 42.55,         
     @  44.50, 46.47, 45.22, 43.51, 41.72, 39.61, 37.43, 35.04,         
     @  33.00, 33.19, 33.37, 33.56, 33.76, 33.95, 34.15, 33.93,         
     @  33.04, 32.13, 31.20, 30.91, 30.78, 30.43, 30.08, 29.72,         
     @  29.35, 28.97, 28.59, 28.20, 27.80, 27.39, 26.97, 26.54,         
     @  26.11, 26.15, 26.25, 26.36, 26.47, 26.58, 26.70, 26.81,         
     @  26.93, 27.05, 27.02, 26.93, 26.84, 26.75, 26.65, 26.55,         
     @  26.45, 26.35, 26.25, 26.14, 26.04, 25.93, 25.81, 25.70,         
     @  25.58, 25.46, 25.35, 25.29, 25.22, 25.15, 25.08, 25.01,         
     @  24.94, 24.86, 24.78, 24.71, 24.63, 24.55, 24.46, 24.38,         
     @  24.29, 24.20, 24.11, 24.02, 23.92, 23.83, 23.73, 23.63,         
     @  23.53, 23.42, 23.31, 23.20                                      
     @/                                                                 
      data skmpe/                                                       
     @   0.00, 54.46, 47.13, 41.79, 36.16, 34.52, 29.21, 24.14,         
     @  21.91, 20.17, 18.45, 16.75, 15.04, 17.01, 19.09, 20.30,         
     @  21.02, 21.74, 22.33, 21.01, 19.66, 18.29, 16.90, 15.49,         
     @  14.05, 12.59, 11.11,  9.59,  8.50,  8.82,  9.14,  9.47,         
     @   9.61,  9.27,  8.92,  8.56,  8.20,  7.83,  7.45,  7.07,         
     @   6.67,  6.32,  6.23,  6.14,  6.05,  5.96,  5.87,  5.77,         
     @   5.67,  5.57,  5.47,  5.36,  5.26,  5.15,  5.03,  4.92,         
     @   4.80,  4.69,  4.56,  4.44,  4.39,  4.36,  4.34,  4.32,         
     @   4.29,  4.27,  4.25,  4.22,  4.20,  4.17,  4.14,  4.12,         
     @   4.09,  4.06,  4.03,  4.00,  3.97,  3.94,  3.91,  3.88,         
     @   3.84,  3.81,  3.77,  3.74,  3.70,  3.67,  3.65,  3.63,         
     @   3.61,  3.59,  3.57,  3.55,  3.53,  3.51,  3.49,  3.47,         
     @   3.45,  3.43,  3.40,  3.38                                      
     @/                                                                 
      data skppt/                                                       
     @  10.00, 10.00, 10.35, 10.80, 11.17, 11.33, 11.46, 11.59,         
     @  11.70, 11.82, 11.94, 12.12, 12.41, 12.69, 13.15, 13.71,         
     @  14.30, 15.08, 15.86, 16.65, 17.46, 18.26, 18.20, 18.14,         
     @  18.08, 18.01, 17.95, 17.88, 17.82, 17.75, 17.68, 17.61,         
     @  17.54, 17.46, 17.39, 17.31, 17.24, 17.16, 17.08, 16.99,         
     @  16.91, 16.92, 16.92, 16.93, 16.93, 16.93, 16.94, 16.94,         
     @  16.95, 16.95, 16.95, 16.96, 16.96, 16.97, 16.97, 16.98,         
     @  16.98, 16.99, 16.99, 17.00, 17.00, 17.01, 17.01, 17.01,         
     @  17.01, 17.02, 17.02, 17.02, 17.03, 17.03, 17.03, 17.04,         
     @  17.04, 17.04, 17.05, 17.05, 17.05, 17.06, 17.06, 17.06,         
     @  17.07, 17.07, 17.08, 17.08, 17.09, 17.09, 17.09, 17.10,         
     @  17.10, 17.11, 17.11, 17.12, 17.12, 17.13, 17.14, 17.15,         
     @  17.16, 17.17, 17.18, 17.19                                      
     @/                                                                 
      data skppe/                                                       
     @  10.00, 10.00, 10.35, 10.11, 10.39, 10.57, 10.73, 10.88,         
     @  11.03, 11.18, 11.28, 11.28, 11.29, 11.29, 11.30, 11.31,         
     @  11.31, 11.32, 11.22, 11.03, 10.84, 10.65, 10.46, 10.26,         
     @  10.06,  9.85,  9.64,  9.43,  9.22,  8.98,  8.70,  8.41,         
     @   8.11,  7.81,  7.50,  7.19,  6.87,  6.54,  6.32,  6.19,         
     @   6.07,  5.94,  5.81,  5.67,  5.54,  5.40,  5.25,  5.11,         
     @   4.96,  4.81,  4.68,  4.62,  4.57,  4.52,  4.46,  4.41,         
     @   4.35,  4.29,  4.23,  4.17,  4.11,  4.04,  3.98,  3.91,         
     @   3.84,  3.77,  3.70,  3.63,  3.56,  3.50,  3.43,  3.37,         
     @   3.32,  3.34,  3.36,  3.39,  3.41,  3.44,  3.46,  3.49,         
     @   3.51,  3.54,  3.57,  3.60,  3.60,  3.59,  3.57,  3.55,         
     @   3.54,  3.52,  3.50,  3.49,  3.47,  3.45,  3.43,  3.41,         
     @   3.40,  3.41,  3.42,  3.43                                      
     @/                                                                 
      data skpnt/                                                       
     @  10.00, 11.06, 12.22, 13.10, 13.80, 14.59, 15.20, 15.75,         
     @  16.17, 16.58, 16.82, 16.88, 16.85, 16.83, 16.81, 17.28,         
     @  18.10, 18.93, 19.77, 20.54, 21.00, 21.06, 20.50, 19.92,         
     @  19.59, 19.45, 19.30, 19.16, 19.01, 18.85, 18.75, 18.70,         
     @  18.65, 18.59, 18.54, 18.48, 18.42, 18.36, 18.30, 18.24,         
     @  18.18, 18.12, 18.05, 17.98, 17.94, 17.91, 17.88, 17.85,         
     @  17.83, 17.80, 17.77, 17.74, 17.71, 17.68, 17.65, 17.61,         
     @  17.58, 17.56, 17.56, 17.55, 17.55, 17.55, 17.55, 17.54,         
     @  17.54, 17.54, 17.54, 17.53, 17.53, 17.53, 17.52, 17.52,         
     @  17.52, 17.51, 17.51, 17.51, 17.50, 17.50, 17.50, 17.49,         
     @  17.49, 17.49, 17.48, 17.48, 17.48, 17.48, 17.48, 17.48,         
     @  17.48, 17.48, 17.49, 17.49, 17.49, 17.49, 17.49, 17.49,         
     @  17.50, 17.50, 17.50, 17.50                                      
     @/                                                                 
      data skpnel/                                                      
     @  10.00, 10.00, 10.00, 10.00, 10.00, 10.00, 10.00, 10.00,         
     @  10.00, 10.00, 10.00, 10.00, 10.00, 10.00, 10.00, 10.00,         
     @  11.31, 11.32, 11.22, 11.03, 10.84, 10.65, 10.46, 10.26,         
     @  10.06,  9.85,  9.64,  9.43,  9.22,  8.98,  8.70,  8.41,         
     @   8.11,  7.81,  7.50,  7.19,  6.87,  6.54,  6.32,  6.19,         
     @   6.07,  5.94,  5.81,  5.67,  5.54,  5.40,  5.25,  5.11,         
     @   4.96,  4.81,  4.68,  4.62,  4.57,  4.52,  4.46,  4.41,         
     @   4.35,  4.29,  4.23,  4.17,  4.11,  4.04,  3.98,  3.91,         
     @   3.84,  3.77,  3.70,  3.63,  3.56,  3.50,  3.43,  3.37,         
     @   3.32,  3.34,  3.36,  3.39,  3.41,  3.44,  3.46,  3.49,         
     @   3.51,  3.54,  3.57,  3.60,  3.60,  3.59,  3.57,  3.55,         
     @   3.54,  3.52,  3.50,  3.49,  3.47,  3.45,  3.43,  3.41,         
     @   3.40,  3.41,  3.42,  3.43                                      
     @/                                                                 
      data skpnce/                                                      
     @   0.00,  1.06,  2.22,  3.10,  3.80,  4.59,  5.20,  5.75,         
     @   6.17,  6.58,  6.82,  6.88,  6.85,  6.83,  6.81,  6.64,         
     @   6.45,  6.24,  6.03,  5.71,  5.40,  5.08,  4.58,  4.11,         
     @   3.83,  3.58,  3.34,  3.03,  2.70,  2.37,  2.04,  1.70,         
     @   1.48,  1.34,  1.19,  1.05,  0.91,  0.79,  0.67,  0.54,         
     @   0.43,  0.32,  0.21,  0.15,  0.10,  0.05,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00/                                     
      data skpd1/                                                       
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00,  0.20,  0.43,  0.98,  1.81,         
     @   2.69,  3.44,  4.09,  4.40,  4.55,  4.52,  4.41,  4.28,         
     @   4.12,  3.87,  3.62,  3.37,  3.12,  2.86,  2.65,  2.45,         
     @   2.26,  2.08,  1.92,  1.80,  1.67,  1.57,  1.47,  1.43,         
     @   1.40,  1.37,  1.33,  1.27,  1.22,  1.18,  1.13,  1.08,         
     @   1.04,  0.99,  0.94,  0.89,  0.90,  0.50,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00/                                     
      data skpd2/                                                       
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00,  0.00,  1.57,  1.91,  2.23,         
     @   2.52,  2.82,  3.13,  3.23,  3.27,  3.26,  3.15,  2.92,         
     @   2.64,  2.45,  2.30,  2.12,  2.00,  1.91,  1.83,  1.77,         
     @   1.73,  1.67,  1.61,  1.56,  1.51,  1.48,  1.46,  1.41,         
     @   1.36,  1.32,  1.30,  1.25,  1.21,  1.19,  1.18,  1.00,         
     @   0.80,  0.50,  0.30,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00/                                     
      data skpd3/                                                       
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.31,  0.62,  0.89,         
     @   1.14,  1.40,  1.66,  1.88,  2.00,  2.17,  2.24,  2.29,         
     @   2.35,  2.41,  2.45,  2.48,  2.49,  2.46,  2.44,  2.39,         
     @   2.33,  2.28,  2.22,  2.17,  2.13,  2.10,  1.50,  1.00,         
     @   0.50,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @   0.00,  0.00,  0.00,  0.00/                                     
      data splmt/                                                       
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @ 209.26, 34.00, 22.84, 21.23, 20.72, 21.46, 22.40, 24.24,         
     @  26.01, 27.75, 29.65, 31.54, 33.41, 35.28, 36.53, 36.76,         
     @  36.98, 37.20, 37.42, 37.65, 37.74, 37.58, 37.42, 37.26,         
     @  37.09, 36.93, 36.76, 36.59, 36.42, 36.24, 36.06, 35.88,         
     @  35.70, 35.57, 35.47, 35.38, 35.28, 35.18, 35.07, 34.97,         
     @  34.86, 34.76, 34.65, 34.54, 34.42, 34.31, 34.19, 34.07,         
     @  33.95, 33.82, 33.69, 33.58, 33.53, 33.49, 33.45, 33.41,         
     @  33.36, 33.32, 33.27, 33.22, 33.17, 33.13, 33.08, 33.02,         
     @  32.97, 32.92, 32.87, 32.81, 32.75, 32.70, 32.64, 32.58,         
     @  32.52, 32.46, 32.39, 32.33, 32.26, 32.20, 32.13, 32.06,         
     @  31.99, 31.91, 31.84, 31.77, 31.69, 31.61, 31.53, 31.45,         
     @  31.37, 31.28, 31.19, 31.11                                      
     @/                                                                 
      data splmel/                                                      
     @   0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,         
     @ 209.26, 34.00, 22.84, 21.23, 18.87, 17.29, 15.81, 15.18,         
     @  15.04, 14.91, 14.78, 14.65, 14.52, 14.38, 14.16, 13.94,         
     @  13.72, 13.49, 13.27, 13.04, 12.81, 12.58, 12.34, 12.15,         
     @  11.98, 11.81, 11.63, 11.45, 11.27, 11.09, 10.90, 10.71,         
     @  10.52, 10.32, 10.13,  9.92,  9.72,  9.51,  9.36,  9.22,         
     @   9.08,  8.94,  8.80,  8.66,  8.51,  8.36,  8.21,  8.05,         
     @   7.89,  7.73,  7.59,  7.44,  7.28,  7.13,  6.97,  6.81,         
     @   6.65,  6.48,  6.31,  6.13,  5.97,  5.94,  5.91,  5.88,         
     @   5.85,  5.82,  5.79,  5.76,  5.72,  5.69,  5.66,  5.62,         
     @   5.59,  5.55,  5.52,  5.48,  5.44,  5.40,  5.37,  5.33,         
     @   5.29,  5.25,  5.21,  5.17,  5.13,  5.09,  5.05,  5.00,         
     @   4.96,  4.91,  4.86,  4.82                                      
     @/                                                                 
      data wqnnnd/                                                      
     @   0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.021, 
     @   0.168,  0.570,  1.423,  3.027,  5.836,  9.931, 14.440, 18.294, 
     @  21.150, 23.073, 24.287, 24.965, 25.251, 25.251, 25.043, 24.684, 
     @  24.217, 23.672, 23.074, 22.441, 21.787, 21.121, 20.452, 19.786, 
     @  19.127, 18.479, 17.845, 17.226, 16.623, 16.038, 15.471, 14.921, 
     @  14.573, 14.221, 13.866, 13.507, 13.144, 12.778, 12.408, 12.034, 
     @  11.657, 11.276, 10.891, 10.502, 10.109,  9.712,  9.312,  8.907, 
     @   8.498,  8.085,  7.668,  7.247,  6.821,  6.563,  6.314,  6.074, 
     @   5.842,  5.618,  5.402,  5.194,  4.993,  4.799,  4.612,  4.431, 
     @   4.258,  4.090,  3.928,  3.773,  3.623,  3.478,  3.339,  3.204, 
     @   3.075,  2.951,  2.831,  2.715,  2.604,  2.497,  2.394,  2.295, 
     @   2.200,  2.108,  2.020,  1.935,  1.853,  1.774,  1.699,  1.626, 
     @   1.557,  1.490,  1.425,  1.364/                                 
      data (dsdm( 1,j),j=1,20) /                                        
     @  1.41, 1.38, 1.35, 1.32, 1.29, 1.26, 1.22, 1.19, 1.15, 1.11,     
     @  1.07, 1.02, 0.94, 0.86, 0.80, 0.73, 0.65, 0.55, 0.44, 0.27/     
      data (dsdm( 2,j),j=1,20) /                                        
     @  1.36, 1.34, 1.32, 1.30, 1.28, 1.24, 1.21, 1.18, 1.15, 1.11,     
     @  1.06, 1.01, 0.96, 0.90, 0.85, 0.76, 0.67, 0.57, 0.45, 0.28/     
      data (dsdm( 3,j),j=1,20) /                                        
     @  1.33, 1.31, 1.30, 1.28, 1.26, 1.23, 1.21, 1.18, 1.15, 1.11,     
     @  1.07, 1.03, 0.98, 0.92, 0.86, 0.79, 0.70, 0.59, 0.48, 0.25/     
      data (dsdm( 4,j),j=1,20) /                                        
     @  1.29, 1.28, 1.27, 1.26, 1.24, 1.22, 1.20, 1.18, 1.15, 1.12,     
     @  1.08, 1.04, 0.99, 0.93, 0.87, 0.80, 0.72, 0.61, 0.48, 0.27/     
      data (dsdm( 5,j),j=1,20) /                                        
     @  1.27, 1.26, 1.25, 1.24, 1.23, 1.22, 1.20, 1.18, 1.15, 1.12,     
     @  1.09, 1.05, 1.00, 0.94, 0.88, 0.81, 0.72, 0.62, 0.49, 0.28/     
      data (dsdm( 6,j),j=1,20) /                                        
     @  1.24, 1.24, 1.24, 1.23, 1.23, 1.21, 1.20, 1.18, 1.16, 1.13,     
     @  1.10, 1.06, 1.01, 0.95, 0.89, 0.82, 0.73, 0.62, 0.49, 0.28/     
      data (dsdm( 7,j),j=1,20) /                                        
     @  1.22, 1.22, 1.22, 1.22, 1.22, 1.21, 1.20, 1.18, 1.16, 1.14,     
     @  1.10, 1.06, 1.02, 0.96, 0.90, 0.82, 0.73, 0.63, 0.50, 0.28/     
      data (dsdm( 8,j),j=1,20) /                                        
     @  1.19, 1.20, 1.21, 1.21, 1.21, 1.21, 1.20, 1.19, 1.17, 1.15,     
     @  1.11, 1.07, 1.03, 0.98, 0.91, 0.84, 0.74, 0.63, 0.49, 0.26/     
      data (dsdm( 9,j),j=1,20) /                                        
     @  1.17, 1.19, 1.20, 1.20, 1.21, 1.21, 1.20, 1.19, 1.17, 1.15,     
     @  1.12, 1.08, 1.03, 0.98, 0.91, 0.84, 0.74, 0.63, 0.49, 0.28/     
      data (dsdm(10,j),j=1,20) /                                        
     @  1.15, 1.17, 1.19, 1.20, 1.20, 1.21, 1.21, 1.20, 1.18, 1.16,     
     @  1.13, 1.09, 1.04, 0.99, 0.92, 0.84, 0.74, 0.63, 0.48, 0.27/     
      data (dsdm(11,j),j=1,20) /                                        
     @  1.14, 1.16, 1.18, 1.19, 1.20, 1.21, 1.21, 1.20, 1.19, 1.17,     
     @  1.14, 1.10, 1.05, 0.99, 0.92, 0.84, 0.74, 0.63, 0.49, 0.26/     
      data (dsdm(12,j),j=1,20) /                                        
     @  1.12, 1.14, 1.16, 1.18, 1.20, 1.21, 1.21, 1.21, 1.20, 1.18,     
     @  1.15, 1.11, 1.06, 1.00, 0.93, 0.84, 0.74, 0.63, 0.48, 0.26/     
      data (dsdm(13,j),j=1,20) /                                        
     @  1.10, 1.13, 1.15, 1.18, 1.20, 1.21, 1.22, 1.22, 1.21, 1.19,     
     @  1.16, 1.12, 1.07, 1.01, 0.93, 0.84, 0.74, 0.62, 0.47, 0.25/     
      data (dsdm(14,j),j=1,20) /                                        
     @  1.08, 1.11, 1.14, 1.17, 1.19, 1.21, 1.22, 1.22, 1.22, 1.20,     
     @  1.17, 1.13, 1.08, 1.01, 0.93, 0.84, 0.74, 0.61, 0.47, 0.25/     
      data (dsdm(15,j),j=1,20) /                                        
     @  1.06, 1.10, 1.13, 1.16, 1.19, 1.21, 1.23, 1.23, 1.23, 1.21,     
     @  1.18, 1.14, 1.08, 1.02, 0.94, 0.84, 0.74, 0.61, 0.46, 0.25/     
      data (dsdm(16,j),j=1,20) /                                        
     @  1.05, 1.08, 1.12, 1.16, 1.19, 1.22, 1.23, 1.24, 1.23, 1.22,     
     @  1.19, 1.15, 1.09, 1.02, 0.94, 0.84, 0.73, 0.60, 0.45, 0.25/     
      data (dsdm(17,j),j=1,20) /                                        
     @  1.03, 1.07, 1.11, 1.15, 1.19, 1.22, 1.24, 1.25, 1.24, 1.23,     
     @  1.20, 1.16, 1.10, 1.03, 0.94, 0.84, 0.73, 0.59, 0.44, 0.24/     
      data (dsdm(18,j),j=1,20) /                                        
     @  1.01, 1.06, 1.10, 1.15, 1.19, 1.22, 1.24, 1.26, 1.26, 1.24,     
     @  1.21, 1.17, 1.11, 1.03, 0.94, 0.84, 0.72, 0.59, 0.44, 0.23/     
      data (dsdm(19,j),j=1,20) /                                        
     @  1.00, 1.04, 1.09, 1.14, 1.18, 1.22, 1.25, 1.26, 1.27, 1.25,     
     @  1.22, 1.18, 1.11, 1.04, 0.94, 0.84, 0.72, 0.58, 0.43, 0.23/     
      data (dsdm(20,j),j=1,20) /                                        
     @  0.98, 1.03, 1.08, 1.13, 1.18, 1.22, 1.26, 1.27, 1.28, 1.27,     
     @  1.23, 1.19, 1.12, 1.04, 0.95, 0.84, 0.71, 0.58, 0.42, 0.21/     
      data (dsdm(21,j),j=1,20) /                                        
     @  0.96, 1.02, 1.07, 1.13, 1.18, 1.23, 1.26, 1.28, 1.29, 1.28,     
     @  1.25, 1.20, 1.13, 1.05, 0.95, 0.83, 0.71, 0.57, 0.41, 0.21/     
      data (dsdm(22,j),j=1,20) /                                        
     @  0.94, 1.00, 1.06, 1.12, 1.18, 1.23, 1.27, 1.29, 1.30, 1.29,     
     @  1.26, 1.21, 1.14, 1.05, 0.95, 0.83, 0.70, 0.56, 0.40, 0.20/     
      data (dsdm(23,j),j=1,20) /                                        
     @  0.93, 0.99, 1.05, 1.11, 1.18, 1.23, 1.27, 1.30, 1.31, 1.30,     
     @  1.27, 1.22, 1.15, 1.06, 0.95, 0.83, 0.70, 0.56, 0.40, 0.20/     
      data (dsdm(24,j),j=1,20) /                                        
     @  0.91, 0.97, 1.04, 1.11, 1.18, 1.23, 1.28, 1.31, 1.32, 1.31,     
     @  1.28, 1.23, 1.15, 1.06, 0.95, 0.83, 0.69, 0.55, 0.39, 0.20/     
      data (dsdm(25,j),j=1,20) /                                        
     @  0.89, 0.96, 1.03, 1.10, 1.17, 1.24, 1.29, 1.32, 1.34, 1.33,     
     @  1.30, 1.24, 1.16, 1.06, 0.95, 0.82, 0.69, 0.54, 0.38, 0.19/     
      data (dsdm(26,j),j=1,20) /                                        
     @  0.88, 0.94, 1.02, 1.10, 1.17, 1.24, 1.30, 1.33, 1.35, 1.34,     
     @  1.31, 1.25, 1.17, 1.07, 0.95, 0.82, 0.68, 0.53, 0.37, 0.19/     
      data (dsdm(27,j),j=1,20) /                                        
     @  0.86, 0.93, 1.01, 1.09, 1.17, 1.24, 1.30, 1.34, 1.36, 1.35,     
     @  1.32, 1.26, 1.18, 1.08, 0.95, 0.82, 0.67, 0.52, 0.36, 0.18/     
      data (dsdm(28,j),j=1,20) /                                        
     @  0.84, 0.92, 1.00, 1.08, 1.17, 1.24, 1.31, 1.35, 1.37, 1.37,     
     @  1.33, 1.27, 1.19, 1.08, 0.95, 0.81, 0.67, 0.51, 0.36, 0.18/     
      data (dsdm(29,j),j=1,20) /                                        
     @  0.83, 0.90, 0.99, 1.07, 1.17, 1.25, 1.31, 1.36, 1.39, 1.38,     
     @  1.35, 1.28, 1.19, 1.08, 0.95, 0.81, 0.66, 0.51, 0.35, 0.17/     
      data (dsdm(30,j),j=1,20) /                                        
     @  0.81, 0.89, 0.98, 1.07, 1.16, 1.25, 1.32, 1.37, 1.40, 1.39,     
     @  1.36, 1.29, 1.20, 1.09, 0.95, 0.81, 0.65, 0.50, 0.34, 0.16/     
      data (dsdm(31,j),j=1,20) /                                        
     @  0.79, 0.87, 0.96, 1.06, 1.16, 1.25, 1.33, 1.38, 1.41, 1.41,     
     @  1.37, 1.31, 1.21, 1.09, 0.95, 0.81, 0.65, 0.49, 0.33, 0.16/     
      data (dsdm(32,j),j=1,20) /                                        
     @  0.78, 0.86, 0.95, 1.06, 1.16, 1.26, 1.34, 1.40, 1.42, 1.42,     
     @  1.39, 1.32, 1.22, 1.09, 0.95, 0.80, 0.64, 0.48, 0.32, 0.15/     
      data (dsdm(33,j),j=1,20) /                                        
     @  0.76, 0.84, 0.94, 1.05, 1.16, 1.26, 1.35, 1.41, 1.44, 1.43,     
     @  1.40, 1.33, 1.23, 1.10, 0.95, 0.80, 0.63, 0.47, 0.32, 0.15/     
      data (dsdm(34,j),j=1,20) /                                        
     @  0.74, 0.83, 0.93, 1.04, 1.16, 1.26, 1.35, 1.42, 1.45, 1.45,     
     @  1.41, 1.34, 1.23, 1.10, 0.95, 0.79, 0.63, 0.46, 0.31, 0.14/     
      data (dsdm(35,j),j=1,20) /                                        
     @  0.73, 0.81, 0.92, 1.03, 1.15, 1.27, 1.36, 1.43, 1.46, 1.46,     
     @  1.42, 1.35, 1.24, 1.11, 0.95, 0.79, 0.62, 0.46, 0.30, 0.14/     
      data (dsdm(36,j),j=1,20) /                                        
     @  0.71, 0.80, 0.91, 1.03, 1.15, 1.27, 1.37, 1.44, 1.48, 1.48,     
     @  1.44, 1.36, 1.25, 1.11, 0.95, 0.78, 0.61, 0.45, 0.29, 0.13/     
      data (dsdm(37,j),j=1,20) /                                        
     @  0.69, 0.79, 0.90, 1.02, 1.15, 1.27, 1.37, 1.45, 1.49, 1.49,     
     @  1.45, 1.37, 1.25, 1.11, 0.95, 0.78, 0.61, 0.44, 0.29, 0.13/     
      data (dsdm(38,j),j=1,20) /                                        
     @  0.68, 0.77, 0.88, 1.01, 1.15, 1.27, 1.38, 1.46, 1.50, 1.50,     
     @  1.46, 1.38, 1.26, 1.11, 0.95, 0.78, 0.60, 0.43, 0.28, 0.14/     
      data (dsdm(39,j),j=1,20) /                                        
     @  0.64, 0.73, 0.83, 0.96, 1.09, 1.21, 1.32, 1.40, 1.45, 1.47,     
     @  1.44, 1.38, 1.28, 1.16, 1.01, 0.85, 0.68, 0.52, 0.36, 0.22/     
      data (dsdm(40,j),j=1,20) /                                        
     @  0.61, 0.69, 0.79, 0.91, 1.04, 1.16, 1.27, 1.36, 1.42, 1.45,     
     @  1.43, 1.39, 1.30, 1.19, 1.05, 0.90, 0.74, 0.58, 0.43, 0.29/     
      data (dsdm(41,j),j=1,20) /                                        
     @  0.58, 0.66, 0.76, 0.87, 0.99, 1.11, 1.23, 1.32, 1.39, 1.42,     
     @  1.42, 1.39, 1.32, 1.22, 1.09, 0.95, 0.80, 0.64, 0.49, 0.36/     
      data (dsdm(42,j),j=1,20) /                                        
     @  0.55, 0.63, 0.72, 0.83, 0.95, 1.07, 1.19, 1.28, 1.36, 1.40,     
     @  1.41, 1.39, 1.33, 1.24, 1.13, 1.00, 0.85, 0.70, 0.56, 0.42/     
      data (dsdm(43,j),j=1,20) /                                        
     @  0.53, 0.60, 0.69, 0.80, 0.92, 1.03, 1.15, 1.25, 1.33, 1.38,     
     @  1.40, 1.38, 1.34, 1.26, 1.16, 1.04, 0.90, 0.76, 0.62, 0.48/     
      data (dsdm(44,j),j=1,20) /                                        
     @  0.51, 0.58, 0.66, 0.77, 0.88, 1.00, 1.11, 1.21, 1.30, 1.35,     
     @  1.38, 1.38, 1.35, 1.28, 1.19, 1.08, 0.95, 0.81, 0.67, 0.54/     
      data (dsdm(45,j),j=1,20) /                                        
     @  0.49, 0.55, 0.64, 0.74, 0.85, 0.96, 1.08, 1.18, 1.27, 1.33,     
     @  1.37, 1.38, 1.35, 1.30, 1.22, 1.12, 1.00, 0.87, 0.73, 0.60/     
      data (dsdm(46,j),j=1,20) /                                        
     @  0.47, 0.53, 0.61, 0.71, 0.82, 0.93, 1.04, 1.15, 1.24, 1.31,     
     @  1.35, 1.37, 1.36, 1.31, 1.25, 1.15, 1.04, 0.92, 0.79, 0.65/     
      data (dsdm(47,j),j=1,20) /                                        
     @  0.45, 0.51, 0.59, 0.68, 0.79, 0.90, 1.01, 1.12, 1.21, 1.29,     
     @  1.34, 1.36, 1.36, 1.33, 1.27, 1.19, 1.09, 0.97, 0.84, 0.71/     
      data (dsdm(48,j),j=1,20) /                                        
     @  0.43, 0.49, 0.57, 0.66, 0.76, 0.87, 0.98, 1.09, 1.19, 1.27,     
     @  1.32, 1.36, 1.36, 1.34, 1.29, 1.22, 1.13, 1.02, 0.90, 0.77/     
      data (dsdm(49,j),j=1,20) /                                        
     @  0.42, 0.47, 0.55, 0.63, 0.73, 0.84, 0.95, 1.06, 1.16, 1.24,     
     @  1.31, 1.35, 1.36, 1.35, 1.31, 1.25, 1.16, 1.06, 0.95, 0.82/     
      data (dsdm(50,j),j=1,20) /                                        
     @  0.40, 0.46, 0.53, 0.61, 0.71, 0.82, 0.93, 1.04, 1.14, 1.23,     
     @  1.29, 1.34, 1.36, 1.36, 1.33, 1.28, 1.20, 1.11, 1.00, 0.88/     
      data (dsdm(51,j),j=1,20) /                                        
     @  0.39, 0.44, 0.51, 0.59, 0.69, 0.79, 0.90, 1.01, 1.11, 1.20,     
     @  1.28, 1.33, 1.36, 1.37, 1.35, 1.30, 1.23, 1.15, 1.05, 0.93/     
      data (dsdm(52,j),j=1,20) /                                        
     @  0.38, 0.43, 0.49, 0.57, 0.67, 0.77, 0.88, 0.99, 1.09, 1.19,     
     @  1.27, 1.32, 1.36, 1.37, 1.36, 1.33, 1.27, 1.19, 1.09, 0.99/     
      data (dsdm(53,j),j=1,20) /                                        
     @  0.36, 0.41, 0.48, 0.56, 0.65, 0.75, 0.86, 0.96, 1.07, 1.17,     
     @  1.25, 1.31, 1.36, 1.38, 1.38, 1.35, 1.30, 1.23, 1.14, 1.04/     
      data dnnnfc/                                                      
     @ 2357.86, 350.16, 104.86,                                         
     @ 41.17,18.44, 9.09, 5.05, 3.26, 2.43, 1.97, 1.70, 1.53, 1.40,     
     @  1.31, 1.24, 1.19, 1.14, 1.11, 1.08, 1.06, 1.04, 1.02, 1.01,     
     @  1.00, 0.99, 0.98, 0.98, 0.98, 0.98, 0.98, 0.98, 0.98, 0.98,     
     @  0.99, 0.99, 1.00, 1.00, 1.01, 1.05, 1.09, 1.13, 1.16, 1.20,     
     @  1.24, 1.27, 1.31, 1.34, 1.38, 1.41, 1.44, 1.48, 1.51, 1.54/     
      real*4 cgksq                                                      
      common/cgkblk/ cgksq(227)                                         
      data (cgksq(i),i=1,120)/  0., 0.5, 0.5, 0., 1., 0.5,              
     @ 0.50000, 1.00000, 0.00000, 0.33333, 0.66667, 0.66667,            
     @ 0.33333, 0.00000, 1.00000, 0.66667, 0.33333, 0.33333,            
     @ 0.66667, 1.00000, 0.00000, 0.25000, 0.50000, 0.75000,            
     @ 0.75000, 0.50000, 0.25000, 0.00000, 1.00000, 0.75000,            
     @ 0.50000, 0.25000, 0.25000, 0.50000, 0.75000, 1.00000,            
     @ 0.00000, 0.66667, 0.33333, 0.33333, 0.66667, 0.00000,            
     @ 1.00000, 0.33333, 0.66667, 0.66667, 0.33333, 1.00000,            
     @ 0.00000, 0.00000, 0.33333, 0.00000, 0.33333, 0.00000,            
     @ 0.33333, 0.00000, 0.00000, 0.00000, 0.50000, 0.50000,            
     @ 0.50000, 0.00000, 0.50000, 0.50000, 0.50000, 0.00000,            
     @ 1.00000, 0.50000, 0.16667, 0.50000, 0.66667, 0.50000,            
     @ 0.16667, 0.50000, 1.00000, 0.00000, 0.00000, 0.16667,            
     @ 0.50000, 0.00000, 0.33333, 0.33333, 0.00000, 0.50000,            
     @ 0.16667, 0.00000, 0.00000, 0.00000, 0.40000, 0.53333,            
     @ 0.40000, 0.60000, 0.06667, 0.06667, 0.60000, 0.40000,            
     @ 0.53333, 0.40000, 0.00000, 1.00000, 0.60000, 0.30000,            
     @ 0.10000, 0.40000, 0.60000, 0.60000, 0.40000, 0.10000,            
     @ 0.30000, 0.60000, 1.00000, 0.00000, 0.75000, 0.25000,            
     @ 0.50000, 0.50000, 0.25000, 0.75000, 0.00000, 1.00000/            
       data (cgksq(i),i=121,227) /                                      
     @ 0.25000, 0.75000, 0.50000, 0.50000, 0.75000, 0.25000,            
     @ 1.00000, 0.00000, 0.00000, 0.50000, 0.00000, 0.33333,            
     @ 0.16667, 0.16667, 0.33333, 0.00000, 0.50000, 0.00000,            
     @ 0.00000, 0.00000, 0.60000, 0.40000, 0.40000, 0.06667,            
     @ 0.53333, 0.53333, 0.06667, 0.40000, 0.40000, 0.60000,            
     @ 0.00000, 1.00000, 0.40000, 0.10000, 0.60000, 0.60000,            
     @ 0.30000, 0.30000, 0.60000, 0.60000, 0.10000, 0.40000,            
     @ 1.00000, 0.00000, 0.00000, 0.00000, 0.25000, 0.00000,            
     @ 0.00000, 0.25000, 0.00000, 0.00000, 0.25000, 0.00000,            
     @ 0.00000, 0.25000, 0.00000, 0.00000, 0.00000, 0.00000,            
     @ 0.00000, 0.30000, 0.45000, 0.00000, 0.40000, 0.05000,            
     @ 0.30000, 0.30000, 0.05000, 0.40000, 0.00000, 0.45000,            
     @ 0.30000, 0.00000, 0.00000, 0.00000, 0.50000, 0.50000,            
     @ 0.25000, 0.50000, 0.00000, 0.25000, 0.50000, 0.50000,            
     @ 0.25000, 0.00000, 0.50000, 0.25000, 0.50000, 0.50000,            
     @ 0.00000, 1.00000, 0.50000, 0.20000, 0.05000, 0.50000,            
     @ 0.60000, 0.45000, 0.20000, 0.20000, 0.45000, 0.60000,            
     @ 0.50000, 0.05000, 0.20000, 0.50000, 1.00000                      
     @ /                                                                
      end                                                               
      subroutine retcgk(it1,np1,it2,np2,j,cgk2)                         
      include 'amsrqmd.h'
      common /aludat/ prtgam(120),isosp(120),isosp3(120),mgpar(120)     
       np1a=iabs(np1)                                                   
       if(it1.gt.15) then                                               
         if(np1a.le.0.or.np1a.gt.70) then                               
           call errex('retcgk:wrong input')                             
         end if                                                         
         j1=isosp(np1a)                                                 
         m1=isign(1,np1)*isosp3(np1a)                                   
       else if(it1.eq.15) then                                          
         j1=2                                                           
         m1=2*np1                                                       
       else if(it1.eq.14) then                                          
         j1=1                                                           
         m1=isign(1,np1)*(2*np1a-3)                                     
       else if(it1.eq.12.or.it1.eq.13) then                             
         j1=0                                                           
         m1=0                                                           
       else if(it1.ge.7.and.it1.le.9) then                              
         j1=2                                                           
         m1=2*it1-16                                                    
       else if(it1.ge.3.and.it1.le.6) then                              
         j1=3                                                           
         m1=2*it1-9                                                     
       else if(mod(it1,9).eq.1.or.mod(it1,9).eq.2) then                 
         j1=1                                                           
         m1=2*mod(it1,9)-3                                              
       else                                                             
         call errex('retcgk:wrong input')                               
       end if                                                           
       if(j.lt.0) then                                                  
         it2=j1                                                         
         np2=m1                                                         
         return                                                         
       end if                                                           
       np2a=iabs(np2)                                                   
       if(it2.gt.15) then                                               
         if(np2a.le.0.or.np2a.gt.70) then                               
           call errex('retcgk:wrong input')                             
         end if                                                         
         j2=isosp(np2a)                                                 
         m2=isign(1,np2)*isosp3(np2a)                                   
       else if(it2.eq.15) then                                          
         j2=2                                                           
         m2=2*np2                                                       
       else if(it2.eq.14) then                                          
         j2=1                                                           
         m2=isign(1,np2)*(2*np2a-3)                                     
       else if(it2.eq.12.or.it2.eq.13) then                             
         j2=0                                                           
         m2=0                                                           
       else if(it2.ge.7.and.it2.le.9) then                              
         j2=2                                                           
         m2=2*it2-16                                                    
       else if(it2.ge.3.and.it2.le.6) then                              
         j2=3                                                           
         m2=2*it2-9                                                     
       else if(mod(it2,9).eq.1.or.mod(it2,9).eq.2) then                 
         j2=1                                                           
         m2=2*mod(it2,9)-3                                              
       else                                                             
         call errex('retcgk:wrong input')                               
       end if                                                           
       cgk2=cgks(j1,j2,j,m1,m2)                                         
      return                                                            
      end                                                               
      function cgks(j1,j2,j,m1,m2)                                      
      dimension ivek(9)                                                 
      data ivek /0,8,20,36,48,75,111,127,163/                           
      common/cgkblk/ cgksq(227)                                         
      if(j.gt.j1+j2.or.j.lt.iabs(j1-j2) .or.                            
     &       iabs(m1).gt.j1.or.iabs(m2).gt.j2) then                     
        cgks=0.                                                         
        return                                                          
      else if(mod(j1,2).ne.mod(iabs(m1),2).or.                          
     &             mod(j2,2).ne.mod(iabs(m2),2)) then                   
        cgks=0.                                                         
        return                                                          
      else if(mod(j1+j2+j,2).ne.0) then                                 
        cgks=0.                                                         
        return                                                          
      end if                                                            
       iofs1=((j-iabs(j1-j2))*(j1+1)*(j2+1))/2                          
       iofs2=((m1+j1)*(j2+1))/2                                         
       iofs3=(m2+j2)/2                                                  
       if(j1.gt.0.and.j2.gt.0) then                                     
         iarg=(j1-1)*3+j2                                               
         if(iarg.le.0.or.iarg.gt.9) then                                
            call errex('wrong input in cgks')                           
            cgks=0.                                                     
            return                                                      
         end if                                                         
         ifofs=iofs1+iofs2+iofs3+1+ivek((j1-1)*3+j2)                    
         if(1.le.ifofs.and.227.ge.ifofs) then                           
            cgks=cgksq(ifofs)                                           
         else                                                           
            call errex('wrong index in cgks')                           
            cgks=0.                                                     
         end if                                                         
       else if(j1.eq.j.or.j2.eq.j) then                                 
         cgks=1.                                                        
       else                                                             
         call errex('wrong input in cgks')                              
         cgks=0.                                                        
       end if                                                           
       return                                                           
       end                                                              
      subroutine isocgk(m1,j1,m2,j2,j1new,j2new,m1new,m2new,itag)       
      include 'amsrqmd.h'
      real*4 cgksq                                                      
      common/cgkblk/ cgksq(227)                                         
      dimension pjin(4),prbout(7),m1out(7)                              
      common /pjbk/ pjin                                                
      data nj,jmin / 0,0 /                                              
      if(itag.ne.-100) then                                             
        itag=0                                                          
      end if                                                            
      m=m1+m2                                                           
      jminol=max0(iabs(j1-j2),iabs(m))                                  
      jmaxol=j1+j2                                                      
      jminnw=max0(iabs(j1new-j2new),iabs(m))                            
      jmaxnw=j1new+j2new                                                
      jmin=max0(jminol,jminnw)                                          
      jmax=min0(jmaxol,jmaxnw)                                          
      nj=(jmax-jmin)/2 +1                                               
      if(j1.eq.0.or.j2.eq.0) then                                       
         if(jmin.ne.jmax) call errex('j1(2)=0,jmin.ne.jmax in isocgk')  
         pjin(1)=1.                                                     
         go to 310                                                      
      end if                                                            
      if(nj.le.0) then                                                  
         itag=-1                                                        
         return                                                         
      end if                                                            
       ifind=0                                                          
       ifg=0                                                            
       do 20 j1pr=1,3                                                   
       do 19 j2pr=1,3                                                   
       do 18 jpr=iabs(j1pr-j2pr),j1pr+j2pr,2                            
       do 17 m1pr=-j1pr,j1pr,2                                          
       do 16 m2pr=-j2pr,j2pr,2                                          
       ifg=ifg+1                                                        
       if((m1.eq.m1pr).and.                                             
     a    (m2.eq.m2pr).and.                                             
     a    (j1.eq.j1pr).and.                                             
     a    (j2.eq.j2pr) )then                                            
          if((jpr.ge.jmin).and.(jpr.le.jmax)) then                      
            ifind=ifind+1                                               
            pjin(ifind)=cgksq(ifg)                                      
          end if                                                        
       end if                                                           
   16 continue                                                          
   17 continue                                                          
   18 continue                                                          
   19 continue                                                          
   20 continue                                                          
      if(ifind.ne.nj) then                                              
           write(lun8,*)'j1,m1,j2,m2,j1new,j2new'                          
           write(lun8,*) j1,m1,j2,m2,j1new,j2new                           
           write(lun8,*) 'jmin,jmax,ifind,nj'                              
           write(lun8,*)  jmin,jmax,ifind,nj                               
           call errex('error in isocgk ifind.ne.nj')                    
      end if                                                            
      prbsum=0.                                                         
      do 30 il=1,nj                                                     
   30 prbsum=prbsum+pjin(il)                                            
      if(prbsum.le.0.) then                                             
           write(lun8,*)'j1,m1,j2,m2,j1new,j2new'                          
           write(lun8,*) j1,m1,j2,m2,j1new,j2new                           
           write(lun8,*) 'jmin,jmax,ifind,nj'                              
           write(lun8,*)  jmin,jmax,ifind,nj                               
           write(lun8,*) (pjin(il),il=1,nj),'pjin'                         
           call errex('error in isocgk 30:prbsum.le.0.')                
      end if                                                            
      do 31 il=1,nj                                                     
   31 pjin(il)=pjin(il)/prbsum                                          
      if(itag.eq.-100) then                                             
          itag=0                                                        
          return                                                        
      else                                                              
          go to 310                                                     
      end if                                                            
      entry isoent(m1,j1,m2,j2,j1new,j2new,m1new,m2new,itag)            
      m=m1+m2                                                           
 310  kjfnd=0                                                           
      zrand=ranf(0)                                                     
      do 32 il=1,nj                                                     
      if(kjfnd.lt.0) go to 33                                           
      if(zrand.le.pjin(il)) then                                        
         kjfnd=-1                                                       
         jtot=jmin +2*(il-1)                                            
      else                                                              
         zrand=zrand-pjin(il)                                           
      end if                                                            
   32 continue                                                          
   33 continue                                                          
      go to 34                                                          
      entry isonew(jin,min,j1new,j2new,m1new,m2new,itag)                
      if(itag.eq.-50) then                                              
         jtot=jin                                                       
         m=min                                                          
         itag=0                                                         
      end if                                                            
   34 continue                                                          
      if(j1new.eq.0.or.j2new.eq.0) then                                 
        if(jtot.ne.j1new+j2new)                                         
     a       call errex('isocgk:j1/2new=0,jtot different')              
        if(j1new.eq.0) then                                             
          m1new=0                                                       
          m2new=m                                                       
        else                                                            
          m2new=0                                                       
          m1new=m                                                       
        end if                                                          
        return                                                          
      end if                                                            
      m1pos=0                                                           
      ifg=0                                                             
      do 40 j1pr=1,3                                                    
      do 39 j2pr=1,3                                                    
      do 38 jpr=iabs(j1pr-j2pr),j1pr+j2pr,2                             
      do 37 m1pr=-j1pr,j1pr,2                                           
      do 36 m2pr=-j2pr,j2pr,2                                           
       ifg=ifg+1                                                        
       mpr=m1pr+m2pr                                                    
       if((j1new.eq.j1pr).and.                                          
     a    (j2new.eq.j2pr).and.                                          
     a    (jtot.eq.jpr).and.                                            
     a    (m .eq. mpr) )then                                            
            m1pos=m1pos+1                                               
            m1out(m1pos)=m1pr                                           
            prbout(m1pos)=cgksq(ifg)                                    
       end if                                                           
   36 continue                                                          
   37 continue                                                          
   38 continue                                                          
   39 continue                                                          
   40 continue                                                          
      if(m1pos.eq.0) call errex('in isocgk: m1pos=0 error')             
      prbsum=0.                                                         
      do 50 m1p=1,m1pos                                                 
   50 prbsum=prbsum+prbout(m1p)                                         
      if(prbsum.le.0.) call errex('error in isocgk:prbsum.le.0.')       
      do 51 m1p=1,m1pos                                                 
   51 prbout(m1p)=prbout(m1p)/prbsum                                    
      km1fnd=0                                                          
      zrand=ranf(0)                                                     
      m1new=-1000                                                       
      do 52 m1p=1,m1pos                                                 
      if(km1fnd.lt.0) go to 52                                          
      if(zrand.le.prbout(m1p)) then                                     
         km1fnd=-1                                                      
         m1new=m1out(m1p)                                               
      else                                                              
         zrand=zrand-prbout(m1p)                                        
      end if                                                            
   52 continue                                                          
      m2new=m-m1new                                                     
      return                                                            
      end                                                               
