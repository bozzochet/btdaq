void Ladder::SetSource(int aSource){
  fSource=aSource;
  Simulation(fSource);  
}


void Ladder::Read(){
  if (fSource==1){GenerateEvent();} 
  else           {ReadEvent();}

  fhScope->Reset();
  for (int va=0;va<max_number_va;va++){          
    for (int ch=0;ch<channel_per_va;ch++){
      fhScope->Fill(va*channel_per_va+ch+1,fch[va][ch]->GetReadOut());      
      fhPedestal[va][ch]->Fill(fch[va][ch]->GetReadOut());
    }
  }
}



void Ladder::GenerateEvent(){
  float cn[max_number_va];
  

  // Generate Common Noise + Pedestal
  for (int va=0; va<max_number_va; va++){
    cn[va]=Gaussian(0.0,fSimCN[va]);    // Common Noise
    for (int ch=0; ch<channel_per_va; ch++)
      fch[va][ch]->SetReadOut((floor) (fSimPedestal[va][ch] +
  	    Gaussian(0.0,fSimSigma[va][ch])+cn[va]));        // Pedestal
  }

  // Generate Signal

  int maxch,len,lmin,lmax,ldif;
  float hit,tot,sum[100],signal;

  maxch = max_number_va*channel_per_va;
  hit=(float)rand()/RAND_MAX*maxch; 
  len=Poisson(0.8)+1;
 
  lmin = (int)(hit-(float)len/2.0+0.5);
  lmax = (int)(hit+(float)len/2.0+0.5);
  ldif=lmax-lmin+1;
  tot=0;
  for (int i=0;i<ldif+1;i++){
    sum[i]=exp(-pow((float)(lmin+i-hit)/len/3.0,2.0));
    tot+=sum[i];  
  }
  for (int i=0;i<ldif+1;i++) sum[i]/=tot;

  signal=Landau(37.0,5.0);
  for (int i=lmin;i<lmax+1;i++){
    int va=i/channel_per_va;
    int ch=i%channel_per_va;
    fch[va][ch]->SetAddReadOut((floor) (sum[i-lmin]*signal));
//      cout << setw(10) << fSimPedestal[va][ch] << setw(6) 
//      	 << fch[va][ch]->GetReadOut() << endl;
  }
}



void Ladder::Simulation(int aSimulate){
  fSource=aSimulate;
  if (fSource == 1){
    srand(time(NULL));
    SimulationLadder();
  }
}

