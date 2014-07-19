void fit() {

  float bchamp = 0.522;
  float long_aimant = 23.84;
  float x_filtre = 0.0;
  float z_filtre = 12.92;

  //  float xsi[4] = { 0.0000, 0.010, -1.000, -1.500 };
  //  float ysi[4] = { 0.0, 0.0, 0.0, 0.0 };
  //  float zsi[4] = { 23.0, 18.0, -18.0, -23.0};
  float xsi[4];
  float ysi[4];
  float zsi[4];
  float x[2];
  float y[2];

  int size = 80;
  char ligne[80];
  ifstream fich("si.data");
  int ev;
  float rig;
  float pt[3];
  if (!fich.good()) {
     cerr << "fichier " << fichier << " mauvais !!!" << '\n' << flush;
     return;
  }

  for (int i=0; i<10; i++) {

    fich >> ev >> ws >> rig;
    printf("ev %d rig %f\n",ev,rig);
    fich >> ysi[0] >> ws >> xsi[0] >> ws >> zsi[0];
    //    printf("x %f y %f z %f\n",xsi[0],ysi[0],zsi[0]);
    fich >> ysi[1] >> ws >> xsi[1] >> ws >> zsi[1];
    //    printf("x %f y %f z %f\n",xsi[1],ysi[1],zsi[1]);
    fich >> ysi[2] >> ws >> xsi[2] >> ws >> zsi[2];
    //    printf("x %f y %f z %f\n",xsi[2],ysi[2],zsi[2]);
    fich >> ysi[3] >> ws >> xsi[3] >> ws >> zsi[3];
    //    printf("x %f y %f z %f\n",xsi[3],ysi[3],zsi[3]);

    x[0] = zsi[0];
    y[0] = ysi[0];
    x[1] = zsi[1];
    y[1] = ysi[1];

    float pentey1 = pente(x,y);
    float yinty1 = yint(x,y);
    //    printf(" pentey1 %f yinty1 %f\n",pentey1,yinty1);
    float entree_y = (pentey1*long_aimant/2.) + yinty1;
    //    printf("entree y %f\n",entree_y);

    x[0] = zsi[1];
    y[0] = xsi[1];
    x[1] = z_filtre;
    y[1] = x_filtre;

    float pentex1 = pente(x,y);
    float yintx1 = yint(x,y);
    //    printf(" pentex1 %f yintx1 %f\n",pentex1,yintx1);
    float entree_x = (pentex1*long_aimant/2.) + yintx1;
    //    printf("entree x %f\n",entree_x);

    float zpt_aval = zptpred(entree_y,entree_x,pentey1,yinty1,pentex1,yintx1);
    if (TMath::Abs(zpt_aval) > (long_aimant/2) ) zpt_aval = 0.;
    float xpt_aval = pentex1*zpt_aval + yintx1;
    //    printf("zpt aval %f xpt aval %f\n",zpt_aval,xpt_aval); 

    x[0] = zsi[2];
    y[0] = ysi[2];
    x[1] = zsi[3];
    y[1] = ysi[3];

    float sortie_y = (-pente(x,y)*long_aimant/2.) + yint(x,y);
    //    printf("sortie y %f\n",sortie_y);

    x[0] = zpt_aval;
    y[0] = xpt_aval;
    x[1] = zsi[2];
    y[1] = xsi[2];

    float pentex2 = pente(x,y);
    float yintx2 = yint(x,y);

    float sortie_x = (-pentex2*long_aimant/2.) + yintx2;
    //    printf("sortie x %f\n",sortie_x);
    pentex2 = TMath::ATan(pentex2);
    //    printf("pentex2 %f %f\n",pentex2,(pentex2*180./TMath::Pi()));

    float d = (entree_x-sortie_x)*(entree_x-sortie_x);
    d+= (long_aimant*long_aimant);
    d = TMath::Sqrt(d)/2;
    float angle = (TMath::Pi()+pentex1-pentex2)/2;
    //    printf("angle %f %f\n",angle,(angle*180./TMath::Pi()));
    float cur = TMath::Cos(angle)/d;
    float ptmesu = -bchamp*(2.997E-4)/cur;
    printf("ptmesu %f\n",ptmesu);

  }

  return;
}

float pente( float *x, float *y) {
  if (x[0] != x[1]) 
    return((y[1]-y[0])/(x[1]-x[0]));
  else
    return(0.);

}

float yint( float *x, float *y) {
  if (x[0] != x[1]) 
    return(((x[0]*y[1])-(x[1]*y[0]))/(x[0]-x[1]));
  else
    return(0.);

}

float zptpred(float x, float y, float dxdz, float xz0, float dydz, float yz0) {

  float rayon_aimont = 20.0/2.;
  float long_aimant = 23.84;

  float a = (dxdz*dxdz) + (dydz*dydz);
  float b = 2*((dxdz*x) + (dxdz*xz0) + (dydz*y) + (dydz*yz0));
  float c = (x*x) + (y*y) + (xz0*xz0) + (yz0*yz0) + (2*xz0*x) + (2*yz0*y) - (rayon_aimont*rayon_aimont);
  //  printf("a %f b %f c %f\n",a,b,c);
  float r = (b*b) - (4*a*c);
  //  printf("r %f\n",r);
  int izero = 0;
  if (r > 0) {
    r = TMath::Sqrt(r);
    float zpt1 = (-b+r)/(2*a);
    float zpt2 = (-b-r)/(2*a);
    //    printf("zpt1 %f zpt2 %f\n",zpt1,zpt2);
    if ( zpt1 >= (-long_aimant/2) && zpt1 <= (long_aimant/2) ) return(zpt1);
    if ( zpt2 >= (-long_aimant/2) && zpt2 <= (long_aimant/2) ) return(zpt2);
    if ( zpt1 < (-long_aimant/2) ) return(zpt1);
    if ( zpt2 < (-long_aimant/2) ) return(zpt2);
  } 
  else return(-b/(2+a));

}

void lire_si_data() {
  int size = 80;
  char ligne[80];
  float xsi[4];
  float ysi[4];
  float zsi[4];
  ifstream fich("si.data");
  int ev;
  float rig;
  float pt[3];
  if (!fich.good()) {
     cerr << "fichier " << fichier << " mauvais !!!" << '\n' << flush;
     return;
  }
  for (int i=0; i<2; i++) {
    fich >> ev >> ws >> rig;
    printf("ev %d rig %f\n",ev,rig);
    fich >> ysi[0] >> ws >> xsi[0] >> ws >> zsi[0];
    printf("x %f y %f z %f\n",xsi[0],ysi[0],zsi[0]);
    fich >> ysi[1] >> ws >> xsi[1] >> ws >> zsi[1];
    printf("x %f y %f z %f\n",xsi[1],ysi[1],zsi[1]);
    fich >> ysi[2] >> ws >> xsi[2] >> ws >> zsi[2];
    printf("x %f y %f z %f\n",xsi[2],ysi[2],zsi[2]);
    fich >> ysi[3] >> ws >> xsi[3] >> ws >> zsi[3];
    printf("x %f y %f z %f\n",xsi[3],ysi[3],zsi[3]);
    //     fich.getline(ligne,size,'\n'); 
    //     cout << "ligne " << ligne << '\n' << flush;
  }   

  return;
}
