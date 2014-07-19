CPAT,CROSS3   .  PDS = UF09.RQMD9.FORT  DATE = 951006  TIME = 215431562 
      subroutine getbb(srt,sigma,ktot,ityp1,ityp2,npid1,npid2,em1,em2,  
     &                 itp1,itp2,itp3,npd1,npd2,npd3,emb,emy,emk,       
     &                 iflag,ikinp,                                     
     &                 vkkb,arat)                                       
      include 'scatter7'                                                
      dimension itb(2)                                                  
      dimension ity(4)                                                  
      dimension itk(2)                                                  
      dimension sigma(99)                                               
      real*4 cgks,cgks2                                                 
      real*4 empar(99,4),itpar(99,4),nppar(99,4)                        
      common/sigsum/vkp,vlam,vtot,vsgp,sgt,vk0,vksp,vknul,vkm,          
     &           vk0kq,vkpkq,vk0km,vkpkm,empar,itpar,nppar              
      data itb /1,3/                                                    
      data ity /0,2,0,2/                                                
      data itk /1,1/                                                    
      vkkb=0                                                            
      call  retcgk(ityp1,npid1,it1,im1,-1,cgks2)                        
      call  retcgk(ityp2,npid2,it2,im2,-1,cgks2)                        
      do 50 i=ktot+1,99                                                 
      sigma(i)=0.                                                       
 50   continue                                                          
      vkp=0                                                             
      vk0=0                                                             
      vlam=0                                                            
      vtot=0                                                            
      vsgp=0                                                            
      vksp=0                                                            
      kcount=ktot                                                       
      do 100 ib=1,2                                                     
             it=itb(ib)                                                 
      do 100 iy=1,4                                                     
      do 100 imy=-ity(iy),ity(iy),2                                     
      do 100 ik=1,2                                                     
      do 100 nu=-itk(ik),itk(ik),2                                      
      val1=0                                                            
       if (iy.gt.2.and.ik.gt.1) goto 100                                
      ich=0                                                             
      icoin=0                                                           
 200  continue                                                          
       m=imy+nu                                                         
       mu=m-im2                                                         
       im=im1-mu                                                        
       if (iabs(im).gt.it.and.ich.eq.0) then                            
          icoin=1                                                       
          goto 300                                                      
       else                                                             
     & if (iabs(im).gt.it.and.ich.eq.1) then                            
          if (icoin.eq.1) then                                          
               goto 100                                                 
          else                                                          
               goto 300                                                 
          end if                                                        
       else                                                             
     & if (ich.eq.1.and.icoin.eq.1) then                                
          kcount=kcount+1                                               
       end if                                                           
       if (iabs(mu).gt.2.and.ich.eq.0) then                             
          icoin=1                                                       
          goto 300                                                      
       else                                                             
     & if (iabs(mu).gt.it.and.ich.eq.1) then                            
          if (icoin.eq.1) then                                          
               goto 100                                                 
          else                                                          
               goto 300                                                 
          end if                                                        
       else                                                             
     & if (ich.eq.1.and.icoin.eq.1) then                                
          kcount=kcount+1                                               
       end if                                                           
       call getifo(1,ib,it,im,itp1,npd1,emb)                            
       call getifo(2,iy,ity(iy),imy,itp2,npd2,emy)                      
       call getifo(3,ik,itk(ik),nu,itp3,npd3,emk)                       
      if (iflag.eq.1) then                                              
          if(ich.eq.0.and.ikinp.eq.kcount+1) then                       
          empar(ikinp,1)=emb                                            
          empar(ikinp,2)=emy                                            
          empar(ikinp,3)=emk                                            
          empar(ikinp,4)=0                                              
          itpar(ikinp,1)=itp1                                           
          itpar(ikinp,2)=itp2                                           
          itpar(ikinp,3)=itp3                                           
          itpar(ikinp,4)=0                                              
          nppar(ikinp,1)=npd1                                           
          nppar(ikinp,2)=npd2                                           
          nppar(ikinp,3)=npd3                                           
          nppar(ikinp,4)=0                                              
               return                                                   
          else                                                          
     &    if(ich.eq.1.and.ikinp.eq.kcount) then                         
          empar(ikinp,1)=emb                                            
          empar(ikinp,2)=emy                                            
          empar(ikinp,3)=emk                                            
          empar(ikinp,4)=0                                              
          itpar(ikinp,1)=itp1                                           
          itpar(ikinp,2)=itp2                                           
          itpar(ikinp,3)=itp3                                           
          itpar(ikinp,4)=0                                              
          nppar(ikinp,1)=npd1                                           
          nppar(ikinp,2)=npd2                                           
          nppar(ikinp,3)=npd3                                           
          nppar(ikinp,4)=0                                              
          return                                                        
          end if                                                        
      else                                                              
      end if                                                            
       if (srt.lt.emb+emy+emk) then                                     
               kcount=kcount+1                                          
               goto 100                                                 
       else                                                             
              call getpar(srt,p1,p2,p3,psf,vkkb,arat,iy,ik,emb,emy,emk) 
       end if                                                           
           arat=arat*1.3                                                
           vkkb=vkkb/1.3                                                
       vals=cgks(it,2,it1,im,mu)                                        
       valy=0.                                                          
       do 510 i=1,3,2                                                   
        if (ity(iy).eq.0) then                                          
           if (i.eq.1) then                                             
                 g=p1                                                   
           else                                                         
                 g=0.                                                   
           end if                                                       
        else if(ity(iy).eq.2) then                                      
           if (i.eq.1) then                                             
                g=p2                                                    
           else if(i.eq.3) then                                         
                g=p3                                                    
           else                                                         
                call errex( 'wrong i in getbb')                         
           end if                                                       
        else                                                            
           call errex( 'wrong ity in getbb')                            
        end if                                                          
      valy=valy+cgks(it2,2,i,im2,mu)*cgks(ity(iy),itk(ik),i,imy,nu)*g   
                                                                        
 510  continue                                                          
      vals=vals*valy                                                    
      if (iy.le.2.and.ik.eq.1) then                                     
        vals=vals*psf                                                   
      end if                                                            
      if (srt-emb-emk-emy.lt.0.25) then                                 
        if ((iy.gt.2.or.ik.gt.1).and.ib.gt.1)                           
     @    vals=vals*(srt-emb-emy-emk)*4                                 
      end if                                                            
      if (ich.eq.0)                                                     
     &             kcount=kcount+1                                      
      sigma(kcount)=sigma(kcount)+vals                                  
      val1=sigma(kcount)                                                
 300  continue                                                          
      if (ich.eq.1) then                                                
           sigma(kcount)=sigma(kcount)/2                                
           val1=val1/2                                                  
      end if                                                            
      if ((im1.ne.im2.or.it1.ne.it2).and.ich.eq.0) then                 
            ich=1                                                       
            mm=im1                                                      
            tt=it1                                                      
            im1=im2                                                     
            it1=it2                                                     
            im2=mm                                                      
            it2=tt                                                      
            goto 200                                                    
      else                                                              
     &if (ich.eq.1) then                                                
            mm=im1                                                      
            tt=it1                                                      
            im1=im2                                                     
            it1=it2                                                     
            im2=mm                                                      
            it2=tt                                                      
      end if                                                            
          if (ik.eq.2) then                                             
                  mu=nu-1                                               
                  c1=cgks(1,2,1,1,mu)                                   
                  mu=nu+1                                               
                  c2=cgks(1,2,1,-1,mu)                                  
                  vkp=vkp+val1*c1/(c1+c2)                               
                  vk0=vk0+val1*c2/(c1+c2)                               
                  vknul=vknul+val1*c2/(c1+c2)                           
                  if (nu.eq.1) vksp=vksp+val1                           
          else                                                          
                  if (nu.eq.1) then                                     
                  vkp=vkp+val1                                          
                  else                                                  
                  vk0=vk0+val1                                          
                  vknul=vknul+val1                                      
                  end if                                                
          end if                                                        
          if (iy.gt.2) then                                             
                if (ity(iy).eq.0) then                                  
                  mu=imy-0                                              
                  c1=cgks(2,2,0,0,mu)                                   
                  mu=imy-2                                              
                  c2=cgks(2,2,0,2,mu)                                   
                  mu=imy+2                                              
                  c3=cgks(2,2,0,-2,mu)                                  
                  vlam=vlam+val1*c1/(c1+c2+c3)                          
                  vsgp=vsgp+val1*c2/(c1+c2+c3)                          
                else                                                    
                  fac=0.9                                               
                  vlam=vlam+val1*fac                                    
                  mu=imy-0                                              
                  c1=cgks(2,2,ity(iy),0,mu)                             
                  mu=imy-2                                              
                  c2=cgks(2,2,ity(iy),2,mu)                             
                  mu=imy+2                                              
                  c3=cgks(2,2,ity(iy),-2,mu)                            
                  vlam=vlam+val1*(1-fac)*c1/(c1+c2+c3)                  
                  vsgp=vsgp+val1*(1-fac)*c2/(c1+c2+c3)                  
                end if                                                  
          else                                                          
               if (ity(iy).eq.0) then                                   
                  vlam=vlam+val1                                        
               else                                                     
                   if (imy.eq.0) then                                   
                       vlam=vlam+val1                                   
                   else                                                 
     &             if (imy.eq.2) then                                   
                       vsgp=vsgp+val1                                   
                   end if                                               
               end if                                                   
          end if                                                        
          vtot=vtot+val1                                                
 100  continue                                                          
       ktot=kcount                                                      
      return                                                            
      end                                                               
      subroutine getpar(srt,p1,p2,p3,psf,vkkq,arat,iy,ik,em1,em2,em3)   
      include 'scatter7'                                                
      common/fpars/ vpar1,vpar2,vpar3,vpar4,vpar5,vpar6,vpar7,          
     &              vpar8,vpar9,vpkkb,vprat,vpmax                       
      dimension pararr(1200)                                            
       data (pararr(i),i=1,100) /                                       
     @ 0.0000,0.0010,0.0020,0.0030,0.0040,0.0065,0.0099,0.0133,         
     @ 0.0167,0.0201,0.0250,0.0298,0.0346,0.0395,0.0445,0.0510,         
     @ 0.0575,0.0639,0.0704,0.0770,0.0836,0.0901,0.0941,0.0973,         
     @ 0.1005,0.1037,0.1069,0.1101,0.1128,0.1152,0.1175,0.1198,         
     @ 0.1222,0.1245,0.1268,0.1292,0.1312,0.1330,0.1348,0.1366,         
     @ 0.1384,0.1402,0.1420,0.1439,0.1457,0.1477,0.1500,0.1522,         
     @ 0.1545,0.1567,0.1590,0.1613,0.1635,0.1658,0.1681,0.1703,         
     @ 0.1726,0.1743,0.1756,0.1768,0.1781,0.1794,0.1807,0.1820,         
     @ 0.1832,0.1845,0.1858,0.1881,0.1907,0.1934,0.1961,0.1988,         
     @ 0.2015,0.2042,0.2068,0.2095,0.2122,0.2149,0.2176,0.2202,         
     @ 0.2229,0.2258,0.2293,0.2328,0.2364,0.2399,0.2434,0.2469,         
     @ 0.2504,0.2539,0.2574,0.2610,0.2645,0.2679,0.2713,0.2747,         
     @ 0.2780,0.2814,0.2848,0.2882                                      
     @/                                                                 
       data (pararr(i),i=101,200) /                                     
     @ 0.3882,0.3882,0.4165,0.4448,0.4730,0.5013,0.5305,0.5635,         
     @ 0.5966,0.6297,0.6628,0.6958,0.7289,0.7620,0.7951,0.8247,         
     @ 0.8527,0.8808,0.9089,0.9370,0.9651,0.9932,1.0212,1.0450,         
     @ 1.0664,1.0878,1.1092,1.1306,1.1520,1.1735,1.1949,1.2163,         
     @ 1.2377,1.2591,1.2805,1.3020,1.3237,1.3460,1.3682,1.3905,         
     @ 1.4127,1.4350,1.4572,1.4795,1.5017,1.5240,1.5462,1.5697,         
     @ 1.5936,1.6176,1.6416,1.6655,1.6895,1.7134,1.7374,1.7614,         
     @ 1.7853,1.8093,1.8346,1.8638,1.8929,1.9221,1.9513,1.9804,         
     @ 2.0096,2.0388,2.0679,2.0971,2.1262,2.1574,2.1918,2.2261,         
     @ 2.2604,2.2947,2.3291,2.3634,2.3977,2.4320,2.4664,2.5007,         
     @ 2.5350,2.5754,2.6171,2.6588,2.7004,2.7421,2.7838,2.8255,         
     @ 2.8671,2.9088,2.9506,2.9926,3.0346,3.0765,3.1185,3.1605,         
     @ 3.2025,3.2445,3.2864,3.3284                                      
     @/                                                                 
       data (pararr(i),i=201,300) /                                     
     @ 0.1388,0.1463,0.1538,0.1612,0.1687,0.1762,0.1837,0.1911,         
     @ 0.1986,0.2069,0.2151,0.2234,0.2317,0.2400,0.2483,0.2566,         
     @ 0.2649,0.2732,0.2812,0.2892,0.2971,0.3050,0.3130,0.3209,         
     @ 0.3288,0.3368,0.3447,0.3526,0.3606,0.3685,0.3794,0.3929,         
     @ 0.4065,0.4201,0.4336,0.4472,0.4608,0.4743,0.4879,0.5015,         
     @ 0.5150,0.5284,0.5385,0.5486,0.5587,0.5687,0.5788,0.5889,         
     @ 0.5990,0.6090,0.6191,0.6292,0.6392,0.6579,0.6783,0.6988,         
     @ 0.7192,0.7397,0.7625,0.7898,0.8172,0.8445,0.8719,0.8992,         
     @ 0.9265,0.9539,0.9888,1.0263,1.0639,1.1015,1.1390,1.1766,         
     @ 1.2142,1.2517,1.2951,1.3391,1.3830,1.4270,1.4709,1.5149,         
     @ 1.5588,1.5930,1.6211,1.6491,1.6771,1.7051,1.7331,1.7611,         
     @ 1.7798,1.7964,1.8131,1.8297,1.8464,1.8630,1.8797,1.8963,         
     @ 1.9107,1.9228,1.9348,1.9468                                      
     @/                                                                 
      dimension g3(100)                                                 
       data (pararr(i),i=301,400) /                                     
     @ 1.0130,1.0129,1.0128,1.0128,1.0127,1.0126,1.0125,1.0125,         
     @ 1.0124,1.0123,1.0122,1.0122,1.0121,1.0120,1.0119,1.0119,         
     @ 1.0118,1.0117,1.0116,1.0116,1.0115,1.0114,1.0113,1.0113,         
     @ 1.0112,1.0111,1.0110,1.0110,1.0109,1.0108,1.0107,1.0107,         
     @ 1.0106,1.0105,1.0104,1.0104,1.0103,1.0102,1.0101,1.0101,         
     @ 1.0100,1.0109,1.0118,1.0127,1.0135,1.0144,1.0153,1.0161,         
     @ 1.0170,1.0179,1.0188,1.0196,1.0242,1.0317,1.0391,1.0466,         
     @ 1.0540,1.0615,1.0690,1.0764,1.0870,1.0994,1.1117,1.1240,         
     @ 1.1364,1.1487,1.1610,1.1733,1.1864,1.2022,1.2180,1.2337,         
     @ 1.2495,1.2653,1.2811,1.2968,1.3166,1.3391,1.3615,1.3839,         
     @ 1.4063,1.4287,1.4511,1.4736,1.4964,1.5293,1.5621,1.5950,         
     @ 1.6279,1.6607,1.6936,1.7264,1.7593,1.7947,1.8325,1.8703,         
     @ 1.9080,1.9458,1.9836,2.0214                                      
     @/                                                                 
      data (pararr(i),i=401,500) /                                      
     @ 0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,         
     @ 0.0000,0.0000,0.0000,0.0000,0.0011,0.0028,0.0097,0.0178,         
     @ 0.0259,0.0340,0.0420,0.0501,0.0582,0.0663,0.0744,0.0826,         
     @ 0.0908,0.0991,0.1073,0.1155,0.1237,0.1319,0.1465,0.1614,         
     @ 0.1764,0.1913,0.2062,0.2211,0.2428,0.2654,0.2880,0.3107,         
     @ 0.3329,0.3446,0.3564,0.3681,0.3806,0.3965,0.4125,0.4284,         
     @ 0.4444,0.4603,0.4763,0.4922,0.5082,0.5241,0.5401,0.5560,         
     @ 0.5720,0.5880,0.6027,0.6147,0.6267,0.6387,0.6507,0.6628,         
     @ 0.6748,0.6868,0.6988,0.7108,0.7228,0.7348,0.7468,0.7562,         
     @ 0.7641,0.7721,0.7800,0.7879,0.7959,0.8038,0.8117,0.8197,         
     @ 0.8276,0.8357,0.8446,0.8535,0.8624,0.8713,0.8802,0.8891,         
     @ 0.8981,0.9070,0.9159,0.9238,0.9304,0.9371,0.9437,0.9503,         
     @ 0.9570,0.9636,0.9702,0.9769                                      
     @/                                                                 
      data (pararr(i),i=501,600) /100*0.0/                              
      data (pararr(i),i=601,700) /                                      
     @ 0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,         
     @ 0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,         
     @ 0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,         
     @ 0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,         
     @ 0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,         
     @ 0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,         
     @ 0.0000,0.0000,0.0000,0.0006,0.0026,0.0045,0.0065,0.0115,         
     @ 0.0169,0.0223,0.0278,0.0333,0.0387,0.0420,0.0453,0.0486,         
     @ 0.0519,0.0552,0.0584,0.0615,0.0647,0.0678,0.0710,0.0737,         
     @ 0.0761,0.0785,0.0808,0.0832,0.0856,0.0880,0.0903,0.0922,         
     @ 0.0941,0.0960,0.0979,0.0998,0.1017,0.1035,0.1047,0.1059,         
     @ 0.1072,0.1084,0.1096,0.1109,0.1120,0.1132,0.1143,0.1155,         
     @ 0.1166,0.1178,0.1189,0.1193                                      
     @/                                                                 
       data (pararr(i),i=701,800) /100*0.0/                             
       data (pararr(i),i=801,900) /100*0.0/                             
       data (pararr(i),i=901,1000) /100*0.0/                            
       data (pararr(i),i=1001,1100) /                                   
     @ 0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,         
     @ 0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,         
     @ 0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,         
     @ 0.0000,0.0000,0.0000,0.0000,0.0021,0.0055,0.0089,0.0294,         
     @ 0.0730,0.1308,0.1938,0.2499,0.2952,0.3405,0.3857,0.4204,         
     @ 0.4472,0.4741,0.5009,0.5278,0.5546,0.5814,0.6061,0.6265,         
     @ 0.6469,0.6674,0.6878,0.7063,0.7228,0.7394,0.7560,0.7725,         
     @ 0.7891,0.8056,0.8212,0.8328,0.8444,0.8559,0.8675,0.8791,         
     @ 0.8906,0.9022,0.9138,0.9247,0.9349,0.9451,0.9552,0.9654,         
     @ 0.9756,0.9858,0.9959,1.0061,1.0165,1.0273,1.0380,1.0488,         
     @ 1.0595,1.0702,1.0810,1.0917,1.1024,1.1132,1.1261,1.1392,         
     @ 1.1523,1.1654,1.1785,1.1916,1.2047,1.2179,1.2310,1.2408,         
     @ 1.2491,1.2574,1.2658,1.2741                                      
     @/                                                                 
       data (pararr(i),i=1101,1200) /                                   
     @ 0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,         
     @ 0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,         
     @ 0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,         
     @ 0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,         
     @ 0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,         
     @ 0.0000,0.0000,0.0000,0.0000,0.0000,0.0034,0.0108,0.0182,         
     @ 0.0333,0.0484,0.0635,0.0838,0.1139,0.1440,0.1740,0.2041,         
     @ 0.2342,0.2595,0.2813,0.3031,0.3249,0.3467,0.3728,0.3989,         
     @ 0.4249,0.4504,0.4747,0.4990,0.5233,0.5476,0.5719,0.5963,         
     @ 0.6156,0.6341,0.6526,0.6710,0.6895,0.7080,0.7260,0.7386,         
     @ 0.7512,0.7638,0.7765,0.7891,0.8017,0.8143,0.8270,0.8396,         
     @ 0.8463,0.8515,0.8568,0.8621,0.8673,0.8726,0.8794,0.8867,         
     @ 0.8941,0.9014,0.9088,0.9161                                      
     @/                                                                 
      data ds/0.026/,smin/2.555/                                        
      data dp/0.023/,pmin/0.001/                                        
      fpms(x)=                                                          
     & sqrt((x**2-(em1+em2+em3)**2)*(x**2-(em1+em2-em3)**2))/2/x        
      if (ik.eq.1.and.iy.le.2) then                                     
                 iofset=100                                             
      else                                                              
     &if (ik.eq.2.and.iy.le.2) then                                     
                 iofset=400                                             
      else                                                              
     &if (ik.eq.1.and.iy.gt.2) then                                     
                 iofset=700                                             
      else                                                              
           p1=0                                                         
           p2=0                                                         
           p3=0                                                         
           psf=0                                                        
           return                                                       
      end if                                                            
      if (srt.lt.em1+em2+em3) then                                      
        call errex( 'wrong srt in getpar')                              
      else                                                              
        pmax=fpms(srt)                                                  
      end if                                                            
       if(iofset.lt.400) then                                           
          vmax=pmax                                                     
          vmin=pmin                                                     
          dv=dp                                                         
       else                                                             
          vmax=srt                                                      
          vmin=smin                                                     
          dv=ds                                                         
       end if                                                           
       index=int((vmax-vmin)/dv+1)                                      
       if (index.lt.1.or.index.gt.100) then                             
              p1=0                                                      
              p2=0                                                      
              p3=0                                                      
              psf=0                                                     
              vkkq=0                                                    
              return                                                    
       else if (index.eq.100) then                                      
              p1=pararr(iofset+100)                                     
              p2=pararr(iofset+200)                                     
              p3=pararr(iofset+300)                                     
              psf=pararr(100)                                           
              vkkq=pararr(1100)                                         
              return                                                    
       end if                                                           
      if (iofset.lt.400) then                                           
         psf=(vmax-vmin-(index-1)*dv)*(pararr(index+1)-pararr(index))/dv
         psf=psf+pararr(index)                                          
      else                                                              
         psf=0                                                          
      end if                                                            
      p1=(vmax-vmin-(index-1)*dv)*                                      
     &            (pararr(iofset+index+1)-pararr(iofset+index))/dv      
      p1=p1+pararr(iofset+index)                                        
      p2=(vmax-vmin-(index-1)*dv)*                                      
     &        (pararr(iofset+100+index+1)-pararr(iofset+100+index))/dv  
      p2=p2+pararr(iofset+100+index)                                    
      p3=(vmax-vmin-(index-1)*dv)*                                      
     &        (pararr(iofset+200+index+1)-pararr(iofset+200+index))/dv  
      p3=p3+pararr(iofset+200+index)                                    
         index=int((vmax-vmin)/dv+1)                                    
      vkkq=(vmax-vmin-(index-1)*dv)*                                    
     &        (pararr(1000+index+1)-pararr(1000+index))/dv              
      vkkq=vkkq+pararr(1000+index)                                      
         index=int((vmax-vmin)/dv+1)                                    
      arat=(vmax-vmin-(index-1)*dv)*                                    
     &        (pararr(1100+index+1)-pararr(1100+index))/dv              
      arat=arat+pararr(1100+index)                                      
      if (ik.eq.1.and.iy.le.2..and.em1+em2+em3.lt.2.56) then            
                 vpmax=pmax                                             
                 vpar1=p1*psf                                           
                 vpar2=p2*psf                                           
                 vpar3=p3*psf                                           
                 vprat=arat                                             
                 vpkkb=vkkq                                             
      else                                                              
     &if (ik.eq.2.and.iy.le.2) then                                     
                 vpar4=p1                                               
                 vpar5=p2                                               
                 vpar6=p3                                               
      else                                                              
     &if (ik.eq.1.and.iy.gt.2) then                                     
                 vpar7=p1                                               
                 vpar8=p2                                               
                 vpar9=p3                                               
      end if                                                            
      return                                                            
      end                                                               
      subroutine getifo(ikat,i,it,im,itp,npd,em)                        
      include 'scatter7'                                                
      if (ikat.eq.1) then                                               
            if (i.eq.1) then                                            
              npd=0                                                     
              em=emnuc                                                  
              itp=(im+3)/2                                              
              if (itp.lt.1.or.itp.gt.2) then                            
                 call errex('wrong im in getifo:ikat,i,it,im')          
              end if                                                    
              return                                                    
            else                                                        
     &      if (i.eq.2) then                                            
              npd=1                                                     
              em=emdelt                                                 
              itp=(im+9)/2                                              
              if(itp.lt.3.or.itp.gt.6) then                             
                 call errex('wrong im in getifo:ikat,i,it,im')          
              end if                                                    
            else                                                        
              call errex(                                               
     a              'wrong i in getifo:ikat,i,it,im')                   
            end if                                                      
              return                                                    
      else                                                              
     &if (ikat.eq.2) then                                               
            if (i.eq.1) then                                            
              npd=0                                                     
              em=emlamb                                                 
              itp=13                                                    
              return                                                    
            else                                                        
     &      if (i.eq.2) then                                            
              itp=15                                                    
              npd=im/2                                                  
              if (im.eq.-2) em=emsigm                                   
              if (im.eq. 0) em=emsig0                                   
              if (im.eq. 2) em=emsigp                                   
              if (im.lt.-2.or.im.gt.2.or.mod(im,2).ne.0) then           
              call errex(                                               
     a               'wrong im in getifo:ikat,i,it,im')                 
              end if                                                    
              return                                                    
            else                                                        
     &      if (i.eq.3) then                                            
              npd=1                                                     
              em=1.405                                                  
              itp=13                                                    
              return                                                    
            else                                                        
     &      if (i.eq.4) then                                            
              itp=99                                                    
              npd=(132-im)/2                                            
              if (im.eq.-2) em=1.389                                    
              if (im.eq. 0) em=1.384                                    
              if (im.eq. 2) em=1.383                                    
              if (im.lt.-2.or.im.gt.2.or.mod(im,2).ne.0) then           
                call errex('wrong im in getifo')                        
              end if                                                    
              return                                                    
            else                                                        
              call errex(                                               
     a                'wrong i in getifo:ikat,i,it,im')                 
            end if                                                      
      else                                                              
     &if (ikat.eq.3) then                                               
            if (i.eq.1) then                                            
              itp=14                                                    
              npd=(im+3)/2                                              
              if (im.eq.-1) em=0.498                                    
              if (im.eq. 1) em=0.494                                    
              if (npd.lt.1.or.npd.gt.2) then                            
                call errex(                                             
     a               'wrong im in getifo:ikat,i,it,im')                 
              end if                                                    
              return                                                    
            else                                                        
     &      if (i.eq.2) then                                            
              itp=99                                                    
              npd=(57-im)/2                                             
              em=0.892                                                  
              if (npd.lt.28.or.npd.gt.29) then                          
                call errex(                                             
     a               'wrong im in getifo:ikat,i,it,im')                 
              end if                                                    
              return                                                    
            else                                                        
                call errex(                                             
     a               'wrong i in getifo:ikat,i,it,im')                  
            end if                                                      
      else                                                              
     &if (ikat.eq.4) then                                               
            if(i.eq.1) then                                             
              itp=14                                                    
              npd=(im-3)/2                                              
              if (npd.eq.-1) then                                       
                 em=0.498                                               
                 return                                                 
              else                                                      
     &        if (npd.eq.-2) then                                       
                 em=0.494                                               
                 return                                                 
              else                                                      
                call errex(                                             
     a          'wrong im in ikat4, getifo,im=')                        
              end if                                                    
            else                                                        
              call errex(                                               
     a            'wrong i in ikat4, getifo,i=')                        
            end if                                                      
      else                                                              
            call errex(                                                 
     a         'wrong ikat in getifo')                                  
      end if                                                            
      end                                                               
      subroutine gpair(icltyp,ityp1,npid1,em1,ityp2,npid2,em2,          
     &               itp1,npd1,e1,                                      
     &               itp2,npd2,e2,                                      
     &               itp3,npd3,e3,                                      
     &               itp4,npd4,e4,                                      
     &               ikinp,iflg,                                        
     &               sigin,k,nscop,srt,par,arat)                        
      include 'scatter7'                                                
      include 'sigmasx'                                                 
      dimension sigin(mxchan)                                           
      real*4 cgks,cgks2                                                 
      real*4  empar(99,4),itpar(99,4),nppar(99,4)                       
        common/sigsum/vkp,vlam,vtot,vsgp,sgt,vk0,vksp,vknul,vkm,        
     &                vk0kq,vkpkq,vk0km,vkpkm,empar,itpar,nppar         
      call  retcgk(ityp1,npid1,it1,im1,-1,cgks2)                        
      call  retcgk(ityp2,npid2,it2,im2,-1,cgks2)                        
      ic=k                                                              
      nscop=k                                                           
      do 10 i=nscop+1,mxchan                                            
      sigin(i)=0                                                        
  10  continue                                                          
      sgt=0                                                             
      ich=0                                                             
      do 100 imn1=-1,1,2                                                
      do 100 imk=-1,1,2                                                 
300   continue                                                          
      imy=im1-imk                                                       
      if (iabs(imy).gt.2) goto 210                                      
      itn1=1                                                            
      imkq=imy-imn1                                                     
      if (iabs(imkq).gt.1) goto 210                                     
      itk=1                                                             
      itkq=1                                                            
      ic=ic+1                                                           
      if(ich.eq.0) then                                                 
           itp2=ityp2                                                   
           npd2=npid2                                                   
           e2=em2                                                       
           call getifo(1,1,itn1,imn1,itp1,npd1,e1)                      
      else                                                              
           itp2=ityp1                                                   
           npd2=npid1                                                   
           e2=em1                                                       
           call getifo(1,1,itn1,imn1,itp1,npd1,e1)                      
      end if                                                            
      call getifo(3,1,itk,imk,itp3,npd3,e3)                             
      call getifo(4,1,itkq,imkq,itp4,npd4,e4)                           
      if (ikinp.eq.ic.and.iflg.eq.1) then                               
         return                                                         
      end if                                                            
      sigin(ic)=0.                                                      
      if (e1+e2+e3+e4.gt.srt) goto 210                                  
      do 200 ity=0,2,2                                                  
      if (iabs(imy).gt.ity) goto 200                                    
      if(ity.eq.0) then                                                 
          a=arat*.35                                                    
      else                                                              
          a=0.25                                                        
      end if                                                            
      sigin(ic)=sigin(ic)+par*a*cgks(ity,itk,it1,imy,imk)               
     &                         *cgks(itn1,itkq,ity,imn1,imkq)           
 200  continue                                                          
      if(ityp1.ne.ityp2.or.npid1.ne.npid2) sigin(ic)=sigin(ic)/2.       
      sgt=sgt+sigin(ic)                                                 
      val1=sigin(ic)                                                    
                  if (imk.eq.1) then                                    
                     vkp=vkp+val1                                       
                     if (imkq.eq.1) vkpkq=vkpkq+val1                    
                     if (imkq.eq.-1) vkpkm=vkpkm+val1                   
                  else                                                  
                     vk0=vk0+val1                                       
                     vknul=vknul+val1                                   
                     if (imkq.eq.1) vk0kq=vk0kq+val1                    
                     if (imkq.eq.-1) vk0km=vk0km+val1                   
                  end if                                                
                  if (imkq.eq.1) then                                   
                     vk0=vk0+val1                                       
                  else                                                  
                     vkm=vkm+val1                                       
                  end if                                                
 210  continue                                                          
      if((ityp1.ne.ityp2.or.npid1.ne.npid2).and.ich.eq.0)then           
           it=it1                                                       
           im=im1                                                       
           it1=it2                                                      
           im1=im2                                                      
           it2=it                                                       
           im2=im                                                       
           ich=1                                                        
           goto 300                                                     
      else                                                              
     &if((ityp1.ne.ityp2.or.npid1.ne.npid2).and.ich.eq.1)then           
           it=it1                                                       
           im=im1                                                       
           it1=it2                                                      
           im1=im2                                                      
           it2=it                                                       
           im2=im                                                       
           ich=0                                                        
      end if                                                            
 100  continue                                                          
      if (iflg.eq.1) then                                               
          call errex(                                                   
     a             'wrong ikinp in gpair')                              
      end if                                                            
      nscop=k                                                           
      k=ic                                                              
      return                                                            
      end                                                               
