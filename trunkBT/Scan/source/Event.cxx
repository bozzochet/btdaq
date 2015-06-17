#include "Event.hh"

ClassImp(Event);

Event::Event(){

  //  rawtdrnum=irawtdrnum;
  Evtnum=0;
  JINJStatus=0;
  for (int ii=0;ii<NJINF;ii++)
    JINFStatus[ii]=0;

  for(int ii=0;ii<NTDRS;ii++){ 
    ReadTDR[ii]=0;
    TDRStatus[ii]=31;
    for(int jj=0;jj<16;jj++)    
      CNoise[ii][jj]=0;
    NClus[ii][0]=0;
    NClus[ii][1]=0;
  }
  for(int ii=0;ii<1024;ii++)
    for(int kk=0;kk<8;kk++)
    Signal[kk][ii]=0;
  
  //  RawLadder = new TClonesArray("RawData",rawtdrnum);

  NClusTot=0;
  notgood=0;
  Cls = new TClonesArray("Cluster",5000);
  Cls->SetOwner();

  NTraceTot=0;
  TraType=0;
  Tra = new TClonesArray("Trace",50);
  Tra->SetOwner();

}

Event::~Event(){

  if(Cls) {Cls->Delete(); delete Cls;}
  if(Tra) {Tra->Delete(); delete Tra;}   
}

void Event::Clear(){
  JINJStatus=0;
   for (int ii=0;ii<NJINF;ii++)
    JINFStatus[ii]=0;

  NClusTot=0;
  NTraceTot=0;
  for(int ii=0;ii<NTDRS;ii++){ 
    ReadTDR[ii]=0;
    TDRStatus[ii]=31;
    for(int jj=0;jj<16;jj++)    
      CNoise[ii][jj]=0;
    NClus[ii][0]=0;
    NClus[ii][1]=0;

  }
  for(int ii=0;ii<8;ii++){ 
    for(int kk=0;kk<1024;kk++)
      Signal[ii][kk]=0;
  }
  if(Cls)  Cls->Delete();
    
  
  //   for (int ii=Cls->GetEntries();ii>-1;ii--){
//     Cluster* ff=(Cluster*) Cls->RemoveAt(ii);
//     if(ff) delete ff;
//   }
  
  return;
}

Cluster* Event::AddCluster(int lad,int side){
  Cluster* pp=(Cluster*)Cls->New(NClusTot);
    NClus[lad][side]++;
  NClusTot++;
  return pp;

}

Cluster* Event::GetCluster(int ii){
 return (Cluster*)Cls->At(ii);

}

int Event::NGoldenClus(int lad, int side){
  int num=0;
  for (int ii=0;ii<NClusTot;ii++){
    Cluster* cl=GetCluster(ii);
    if(cl->ladder==lad&&cl->side==side&&cl->golden==1) num++;
  }
  return num;
}


int Event::trace(RHClass* rh, Align* alg) {
  int bon = 0;
  int nas1 = 0;
  int nak1 = 0;
  int nask1 = 0;
  int nas = 0;
  int nak = 0;
  int ech;
  float xdif,ydif;

  Trace* tra=(Trace*)Tra->New(NTraceTot);
  NTraceTot++;
  for (int i=0; i<4; i++) 
    for (int j=0; j<2; j++) tra->amas_entree[j][i]=-1;
  for (int ii=0; ii<Cls->GetEntries(); ii++) {
    Cluster* clu = (Cluster*)Cls->At(ii);
    //    printf("clu %d ladder %d\n",clu,clu->ladder);
    if (clu->ladder != -1) {
      ech = rh->FindPos(clu->ladder);
      //      printf("ech %d\n",ech);
      float asignal = clu->GetTotSN(1.);
      if (ech < 4 && alg->mauvais[ech][clu->GetSeedAdd()-1] == 0) {
         AddCluster(ech,clu->side);
         if ( asignal > tra->amas_signal[clu->side][ech]) {
	   tra->amas_signal[clu->side][ech] = asignal;
           tra->amas_entree[clu->side][ech] = ii;
	 }
      }
      if (ech >= 4 && alg->mauvais[ech][clu->GetSeedAdd()-1] == 0) {
	if (clu->side == 0 && clu->GetSeedAdd() <= 256) {
            AddCluster(ech,clu->side);
	    tra->amas_signal[clu->side][ech] = asignal;
            tra->amas_entree[clu->side][ech] = ii;
	}
	else if (clu->side == 1 && clu->GetSeedAdd() <= 896) {
            AddCluster(ech,clu->side);
	    tra->amas_signal[clu->side][ech] = asignal;
            tra->amas_entree[clu->side][ech] = ii;
        }
      }
    }
  }
  for (int ii=0; ii<6; ii++) {
     if (NClus[ii][0]==1) nas1++;
     if (NClus[ii][1]==1) nak1++;
     if (NClus[ii][0]>=1) nas++;
     if (NClus[ii][1]>=1) nak++;
     if (NClus[ii][0]==1 && NClus[ii][1]==1) nask1++;
  }
  if (nas == 6) bon++;
  if (nak == 6) bon+=2;
  if (nas1 == 6) bon+=4;
  if (nak1 == 6) bon+=8;
  TraType = bon;
  //  printf("nclus %d tratype %d\n",Cls->GetEntries(),TraType);

  if (bon >= 12) {
    for (int ie=0; ie<3; ie++) {
      printf("amas_entree0 %d\n",tra->amas_entree[0][ie]);
      printf("amas_entree1 %d\n",tra->amas_entree[1][ie]);
       Cluster* clu = (Cluster*)Cls->At(tra->amas_entree[0][ie]);
       //       tra->xp[ie] = clu->GetCoG();
       tra->xp[ie] = clu->GetCoGN(alg->amaspar[0][ie]);
       clu = (Cluster*)Cls->At(tra->amas_entree[1][ie]);
       //       tra->yp[ie] = clu->GetPositionEtaN(alg,ie);
       //if (ie != 3) tra->yp[ie] = clu->GetPositionEtaN(alg,ie);
       tra->yp[ie] = clu->GetCoGN(alg->amaspar[1][ie]);
	 /* {
         if (clu->GetLargeur(1.) == 3)
	   tra->yp[ie] = clu->GetPositionEta3N(alg,ie);
         else
           tra->yp[ie] = clu->GetCoGN(2);
	   } */
      //       printf("det %d position y cogn %f feta %f\n",ie,ycogn,tra->yp[ie]);
    }
    //    printf("\n");
    alg->coord(tra);
    if (alg->align == 2) {
      xdif = tra->x[alg->ref1] - tra->x[alg->ref2];
      if (alg->nfit1 < alg->npts && fabs(xdif) <= 0.05) {
         alg->xval1[alg->nfit1] = tra->y[alg->ref1];
         alg->yval1[alg->nfit1] = xdif;
         alg->nfit1++;
      }
      ydif = tra->y[alg->ref1] - tra->y[alg->ref2];
      if (alg->nfit2 < alg->npts && fabs(ydif) <= 0.06) {
   	 alg->xval2[alg->nfit2] = tra->x[alg->ref1];
         alg->yval2[alg->nfit2] = ydif;
         alg->nfit2++;
      }
    }
    if (alg->align == 3) {
      xdif = tra->x[alg->ind3] - alg->ligne_projx(alg->ref1,alg->ref2,alg->ind3,tra);
      //      printf("x3 %f xdif %f\n",tra->x[alg->ztdr[3]],xdif);
      if (alg->nfit1 < alg->npts && fabs(xdif) < 0.15) {
	alg->xval1[alg->nfit1] = tra->x[alg->ind3];
        alg->yval1[alg->nfit1] = xdif;
	//	        printf("nfit1 %d xval %f yval %f\n",alg->nfit1,alg->xval1[alg->nfit1],alg->yval1[alg->nfit1]);
        alg->nfit1++;
      }
      ydif = tra->y[alg->ind3] - alg->ligne_projy(alg->ref1,alg->ref2,alg->ind3,tra);
      if (fabs(ydif) < 0.06) {
        if (alg->deux_plaq_n[alg->ind3] <= 1 && alg->nfit2 < alg->npts) {
	  //        if (alg->deux_plaq_n[alg->ind3] == 0 && alg->nfit2 < alg->npts) {
	 alg->xval2[alg->nfit2] = tra->y[alg->ind3];
         alg->yval2[alg->nfit2] = ydif;
         alg->nfit2++;
        }
        else {
         Cluster* clu = (Cluster*)Cls->At(tra->amas_entree[1][alg->ind3]);
         if (clu->GetSeedAdd() < 833 && alg->nfit2 < alg->npts) {
           alg->xval2[alg->nfit2] = tra->y[alg->ind3];
           alg->yval2[alg->nfit2] = ydif;
           alg->nfit2++;
         }
         if (clu->GetSeedAdd() >= 833 && alg->nfit2_2 < alg->npts) {
           alg->xval2_2[alg->nfit2_2] = tra->y[alg->ind3];
           alg->yval2_2[alg->nfit2_2] = ydif;
           alg->nfit2_2++;
         }
        }
      }
      xdif = tra->x[alg->ind4] - alg->ligne_projx(alg->ref1,alg->ref2,alg->ind4,tra);
      if (alg->nfit3 < alg->npts && fabs(xdif) < 0.06) {
	alg->xval3[alg->nfit3] = tra->x[alg->ind4];
        alg->yval3[alg->nfit3] = xdif;
        alg->nfit3++;
      }
      ydif = tra->y[alg->ind4] - alg->ligne_projy(alg->ref1,alg->ref2,alg->ind4,tra);
      if (alg->nfit4 < alg->npts && fabs(ydif) < 0.06) {
	alg->xval4[alg->nfit4] = tra->y[alg->ind4];
        alg->yval4[alg->nfit4] = ydif;
        alg->nfit4++;
      }
      xdif = tra->x[alg->indp1] - alg->ligne_projx(alg->ref1,alg->ref2,alg->indp1,tra);
      if (alg->nfit5 < alg->npts && fabs(xdif) < 0.06) {
	alg->xval5[alg->nfit5] = tra->x[alg->indp1];
        alg->yval5[alg->nfit5] = xdif;
        alg->nfit5++;
      }
      ydif = tra->y[alg->indp1] - alg->ligne_projy(alg->ref1,alg->ref2,alg->indp1,tra);
      if (alg->nfit6 < alg->npts && fabs(ydif) < 0.06) {
	alg->xval6[alg->nfit6] = tra->y[alg->indp1];
        alg->yval6[alg->nfit6] = ydif;
        alg->nfit6++;
      }
      xdif = tra->x[alg->indp2] - alg->ligne_projx(alg->ref1,alg->ref2,alg->indp2,tra);
      if (alg->nfit7 < alg->npts && fabs(xdif) < 0.06) {
	alg->xval7[alg->nfit7] = tra->x[alg->indp2];
        alg->yval7[alg->nfit7] = xdif;
        alg->nfit7++;
      }
      ydif = tra->y[alg->indp2] - alg->ligne_projy(alg->ref1,alg->ref2,alg->indp2,tra);
      if (alg->nfit8 < alg->npts && fabs(ydif) < 0.06) {
	alg->xval8[alg->nfit8] = tra->y[alg->indp2];
        alg->yval8[alg->nfit8] = ydif;
        alg->nfit8++;
      }
    }
    if (alg->align == 4) {
      xdif = tra->x[alg->ind3] - alg->ligne_projx(alg->ref1,alg->ref2,alg->ind3,tra);
      //      printf("x3 %f xdif %f\n",tra->x[alg->ztdr[3]],xdif);
      if (alg->nfit1 < alg->npts && fabs(xdif) < 0.06) {
	alg->xval1[alg->nfit1] = tra->y[alg->ind3];
        alg->yval1[alg->nfit1] = xdif;
	//	        printf("nfit1 %d xval %f yval %f\n",alg->nfit1,alg->xval1[alg->nfit1],alg->yval1[alg->nfit1]);
        alg->nfit1++;
      }
      ydif = tra->y[alg->ind3] - alg->ligne_projy(alg->ref1,alg->ref2,alg->ind3,tra);
      if (alg->nfit2 < alg->npts && fabs(ydif) < 0.06 && fabs(tra->x[alg->ind3]) < 0.5) {
	alg->xval2[alg->nfit2] = tra->x[alg->ind3];
        alg->yval2[alg->nfit2] = ydif;
        alg->nfit2++;
      }
      xdif = tra->x[alg->ind4] - alg->ligne_projx(alg->ref1,alg->ref2,alg->ind4,tra);
      if (alg->nfit3 < alg->npts && fabs(xdif) < 0.06) {
	alg->xval3[alg->nfit3] = tra->y[alg->ind4];
        alg->yval3[alg->nfit3] = xdif;
        alg->nfit3++;
      }
      ydif = tra->y[alg->ind4] - alg->ligne_projy(alg->ref1,alg->ref2,alg->ind4,tra);
      if (alg->nfit4 < alg->npts && fabs(ydif) < 0.06) {
	alg->xval4[alg->nfit4] = tra->x[alg->ind4];
        alg->yval4[alg->nfit4] = ydif;
        alg->nfit4++;
      }
      xdif = tra->x[alg->indp1] - alg->ligne_projx(alg->ref1,alg->ref2,alg->indp1,tra);
      if (alg->nfit5 < alg->npts && fabs(xdif) < 0.06) {
	alg->xval5[alg->nfit5] = tra->y[alg->indp1];
        alg->yval5[alg->nfit5] = xdif;
        alg->nfit5++;
      }
      ydif = tra->y[alg->indp1] - alg->ligne_projy(alg->ref1,alg->ref2,alg->indp1,tra);
      if (alg->nfit6 < alg->npts && fabs(ydif) < 0.06) {
	alg->xval6[alg->nfit6] = tra->x[alg->indp1];
        alg->yval6[alg->nfit6] = ydif;
        alg->nfit6++;
      }
      xdif = tra->x[alg->indp2] - alg->ligne_projx(alg->ref1,alg->ref2,alg->indp2,tra);
      if (alg->nfit7 < alg->npts && fabs(xdif) < 0.06) {
	alg->xval7[alg->nfit7] = tra->y[alg->indp2];
        alg->yval7[alg->nfit7] = xdif;
        alg->nfit7++;
      }
      ydif = tra->y[alg->indp2] - alg->ligne_projy(alg->ref1,alg->ref2,alg->indp2,tra);
      if (alg->nfit8 < alg->npts && fabs(ydif) < 0.06) {
	alg->xval8[alg->nfit8] = tra->x[alg->indp2];
        alg->yval8[alg->nfit8] = ydif;
        alg->nfit8++;
      }
    }
    if (alg->align == 5 || alg->align == 6) {
           alg->div_dxdz_lin(tra,-1);
           for (int ii=0; ii<6; ii++) alg->div_dxdz_lin(tra,ii);
           alg->div_dydz_lin(tra,-1);
           for (int ii=0; ii<6; ii++) alg->div_dydz_lin(tra,ii);
    }
    if (alg->align == 6) {
      for (int ii=0; ii<6; ii++) {
          int retour = alg->div_dxdz_lin_eff(tra,ii);
          if (retour) alg->ntrace_eff[0][ii]++;
      }
      for (int ii=0; ii<6; ii++) {
          int retour = alg->div_dydz_lin_eff(tra,ii);
          if (retour) alg->ntrace_eff[1][ii]++;
	  } 
      alg->div_dxdz_lin_proj_pos0(tra);
      alg->div_dydz_lin_proj_pos0(tra);
      alg->div_dxdz_lin_proj_pos4(tra);
      alg->div_dydz_lin_proj_pos4(tra);
      alg->div_dxdz_lin_pos123(tra,-1);
      alg->div_dxdz_lin_ref(tra);
      alg->div_dydz_lin_ref(tra);
      for (int ii=0; ii<4; ii++) {
       if (ii != 2) alg->div_dxdz_lin_pos123(tra,ii);
      }
      alg->div_dydz_lin_pos123(tra,-1);
      for (int ii=0; ii<4; ii++) {
       if (ii != 2) alg->div_dydz_lin_pos123(tra,ii);
      }
    }
    alg->ntrace++;
  }
  else {
    if (nask1 == 5) {
      for (int ie=0; ie<6; ie++)
	//      printf("ie %d nas %d %d nak %d %d\n",ie,NClus[ie][0],tra->amas_entree[0][ie],NClus[ie][1],tra->amas_entree[1][ie]);
       for (int ie=0; ie<6; ie++) {
         if (tra->amas_entree[0][ie] != -1) {
           Cluster* clu = (Cluster*)Cls->At(tra->amas_entree[0][ie]);
           tra->xp[ie] = clu->GetCoGN(alg->amaspar[0][ie]);
         }
         else tra->xp[ie] = -999.;
         if (tra->amas_entree[1][ie] != -1) {
           Cluster* clu = (Cluster*)Cls->At(tra->amas_entree[1][ie]);
           tra->yp[ie] = clu->GetCoGN(alg->amaspar[1][ie]);
         }
         else tra->yp[ie] = -999.;
	 //         printf("ie %d x %f y %f\n",ie,tra->xp[ie],tra->yp[ie]);
       }
       alg->coord(tra);
       for (int ii=0; ii<6; ii++) {
          int retour = alg->div_dxdz_lin_eff(tra,ii);
          if (retour) alg->ntrace_eff[0][ii]++;
       }
       for (int ii=0; ii<6; ii++) {
          int retour = alg->div_dydz_lin_eff(tra,ii);
          if (retour) alg->ntrace_eff[1][ii]++;
       }
       bon = -bon;
       //       printf("bon %d\n",bon);
    }
  }

  return(bon);


}

