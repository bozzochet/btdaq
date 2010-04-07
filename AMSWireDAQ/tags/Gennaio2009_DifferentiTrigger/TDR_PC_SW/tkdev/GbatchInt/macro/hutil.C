/* $Id: hutil.C,v 1.1 2008/05/09 10:29:11 haino Exp $ */
enum EAxis{ kXax = 1, kYax = 2, kZax = 3 };

// create histgram with Logarithmic bins

TH1D *loghist( const char *hname, const char *htitle,
	       Int_t nbin, Double_t xmin, Double_t xmax )
{
  Double_t *bin = getlogbin( nbin, xmin, xmax );
  TH1D *hist = new TH1D( hname, htitle, nbin, bin );
  delete [] bin;

  return hist;
}

TH2D *loghist( const char *hname, const char *htitle,
	       Int_t nbx, Double_t xmin, Double_t xmax,
	       Int_t nby, Double_t ymin, Double_t ymax, Int_t mode = 1 )
{
  Double_t *xbin = 0, *ybin = 0;
  TH2D *hist = 0;

  if ( mode == 1 ) {        // logx liny
    xbin = getlogbin( nbx, xmin, xmax );
    hist = new TH2D( hname, htitle, nbx, xbin, nby, ymin, ymax );
  }else if ( mode == 2 ) {  // linx logy
    ybin = getlogbin( nby, ymin, ymax );
    hist = new TH2D( hname, htitle, nbx, xmin, xmax, nby, ybin );
  }else if ( mode == 3 ) {  // logx logy
    xbin = getlogbin( nbx, xmin, xmax );
    ybin = getlogbin( nby, ymin, ymax );
    hist = new TH2D( hname, htitle, nbx, xbin, nby, ybin );
  }

  if ( xbin ) delete [] xbin;
  if ( ybin ) delete [] ybin;

  return hist;
}

TH3D *loghist( const char *hname, const char *htitle,
	       Int_t nbx, Double_t xmin, Double_t xmax,
	       Int_t nby, Double_t ymin, Double_t ymax,
	       Int_t nbz, Double_t zmin, Double_t zmax,
	       Int_t bit = 1 )
{
  Double_t *xbin, *ybin, *zbin;

  if ( bit&1 ) xbin = getlogbin( nbx, xmin, xmax );
  else         xbin = getlinbin( nbx, xmin, xmax );
  if ( bit&2 ) ybin = getlogbin( nby, ymin, ymax );
  else         ybin = getlinbin( nby, ymin, ymax );
  if ( bit&4 ) zbin = getlogbin( nbz, zmin, zmax );
  else         zbin = getlinbin( nbz, zmin, zmax );

  TH3D *hist = new TH3D( hname, htitle, nbx, xbin, nby, ybin, nbz, zbin );

  delete [] xbin;
  delete [] ybin;
  delete [] zbin;

  return hist;
}

Double_t *getlogbin( Int_t nbin, Double_t xmin, Double_t xmax )
{
  Double_t lx1 = TMath::Log10( xmin );
  Double_t lx2 = TMath::Log10( xmax );

  Double_t *bin = new Double_t[nbin+1];
  for ( Int_t i = 0; i <= nbin; i++ )
    bin[i] = TMath::Power( 10, lx1+(lx2-lx1)/nbin*i );

  return bin;
}

Double_t *getlinbin( Int_t nbin, Double_t xmin, Double_t xmax )
{
  Double_t *bin = new Double_t[nbin+1];
  for ( Int_t i = 0; i <= nbin; i++ )
    bin[i] = xmin+(xmax-xmin)/nbin*i;

  return bin;
}

Double_t getlogcen( TAxis *ax, Int_t i )
{
  Double_t x1 = ax->GetBinLowEdge( i+1 );
  Double_t x2 = ax->GetBinLowEdge( i+2 );
  return TMath::Power( 10, (TMath::Log10( x1 )+TMath::Log10( x2 ))/2 );
}


// projection

TH1D *proj( TH2 *histi, Int_t axis, Double_t min = 0, Double_t max = 0 )
{
  TString hname = histi->GetName();
  TAxis *ax = histi->GetXaxis();
  TAxis *ay = histi->GetYaxis();
  Int_t  nx = histi->GetNbinsX();
  Int_t  ny = histi->GetNbinsY();

  TAxis *a1, *a2;
  Int_t n1, n2;
  if      ( axis == 1 ) { a1 = ay; a2 = ax; n1 = ny; n2 = nx; }
  else if ( axis == 2 ) { a1 = ax; a2 = ay; n1 = nx; n2 = ny; }
  else return 0;

  if ( min == 0 && max == 0 ) { min = a2->GetXmin(); max = a2->GetXmax(); }

  Double_t *bin = new Double_t[n1+1];
  for ( Int_t i = 0; i <= n1; i++ ) bin[i] = a1->GetBinLowEdge( i+1 );

  Double_t entries = 0;

  Int_t j1 = a2->FindBin( min )-1;
  Int_t j2 = a2->FindBin( max )-1;

  hname += "prj"; hname += axis;
  TH1D *histo = new TH1D( hname, histi->GetTitle(), n1, bin );
  for ( Int_t i = 0; i < n1; i++ ) {
    Double_t cont = 0;
    for ( Int_t j = j1; j < j2; j++ ) {
      if ( axis == 1 ) cont += histi->GetBinContent( j+1, i+1 );
      if ( axis == 2 ) cont += histi->GetBinContent( i+1, j+1 );
    }
    if ( cont > 0 ) histo->SetBinContent( i+1, cont );
    entries += cont;
  }

  histo->SetEntries( entries );

  delete [] bin;

  return histo;
}

TH2D *proj( TH3 *histi, Int_t axis, Double_t min = 0, Double_t max = 0 )
{
  TString hname = histi->GetName();
  TAxis *ax = histi->GetXaxis();
  TAxis *ay = histi->GetYaxis();
  TAxis *az = histi->GetZaxis();
  Int_t  nx = histi->GetNbinsX();
  Int_t  ny = histi->GetNbinsY();
  Int_t  nz = histi->GetNbinsZ();

  TAxis *a1, *a2, *a3;
  Int_t n1, n2, n3;
  if       ( axis == 1 ) { a1 = ay; a2 = az; a3 = ax; 
                           n1 = ny; n2 = nz; n3 = nx; 
  }else if ( axis == 2 ) { a1 = ax; a2 = az; a3 = ay; 
                           n1 = nx; n2 = nz; n3 = ny;
  }else if ( axis == 3 ) { a1 = ax; a2 = ay; a3 = az;
                           n1 = nx; n2 = ny; n3 = nz;
  }else return 0;

  if ( min == 0 && max == 0 ) { min = a3->GetXmin(); max = a3->GetXmax(); }

  Double_t *bin1 = new Double_t[n1+1];
  Double_t *bin2 = new Double_t[n2+1];
  for ( Int_t i = 0; i <= n1; i++ ) bin1[i] = a1->GetBinLowEdge( i+1 );
  for ( Int_t i = 0; i <= n2; i++ ) bin2[i] = a2->GetBinLowEdge( i+1 );

  Double_t entries = 0;

  Int_t k1 = a3->FindBin( min )-1;
  Int_t k2 = a3->FindBin( max )-1;

  hname += "prj"; hname += axis;
  TH2D *histo = new TH2D( hname, histi->GetTitle(), n1, bin1, n2, bin2 );
  for ( Int_t i = 0; i < n1; i++ ) {
    for ( Int_t j = 0; j < n2; j++ ) {
      Double_t cont = 0;

      for ( Int_t k = k1; k < k2; k++ ) {
	if ( axis == 1 ) cont += histi->GetBinContent( k+1, i+1, j+1 );
	if ( axis == 2 ) cont += histi->GetBinContent( i+1, k+1, j+1 );
	if ( axis == 3 ) cont += histi->GetBinContent( i+1, j+1, k+1 );
      }
      if ( cont > 0 ) histo->SetBinContent( i+1, j+1, cont );
      entries += cont;
    }
  }

  histo->SetEntries( entries );

  delete [] bin1;
  delete [] bin2;

  return histo;
}


// profile

enum EProfMethods{ kProf, kRMS, kGmean, kGsigma, kGaus, kLandau };

TH1D *prof( TH2 *histi, Int_t axis, Double_t min = 0, Double_t max = 0,
	    Double_t cut = 0, Int_t method = kProf )
{
  TString hname = histi->GetName();
  TAxis *ax = histi->GetXaxis();
  TAxis *ay = histi->GetYaxis();
  Int_t  nx = histi->GetNbinsX();
  Int_t  ny = histi->GetNbinsY();

  TAxis *a1, *a2;
  Int_t n1, n2;
  if      ( axis == 1 ) { a1 = ay; a2 = ax; n1 = ny; n2 = nx; }
  else if ( axis == 2 ) { a1 = ax; a2 = ay; n1 = nx; n2 = ny; }
  else return 0;

  if ( min == 0 && max == 0 ) { min = a2->GetXmin(); max = a2->GetXmax(); }

  Double_t *bin1 = new Double_t[n1+1];
  Double_t *bin2 = new Double_t[n2+1];
  for ( Int_t i = 0; i <= n1; i++ ) bin1[i] = a1->GetBinLowEdge( i+1 );
  for ( Int_t i = 0; i <= n2; i++ ) bin2[i] = a2->GetBinLowEdge( i+1 );

  Double_t totsum = histi->GetSumOfWeights();

  Int_t j1 = a2->FindBin( min )-1;
  Int_t j2 = a2->FindBin( max )-1;

  hname += "prf"; hname += axis;
  TH1D *histo = new TH1D( hname, histi->GetTitle(), n1, bin1 );

  TH1D *htmp = new TH1D( hname+"tmp", histi->GetTitle(), n2, bin2 );

  for ( Int_t i = 0; i < n1; i++ ) {
    htmp->Reset();
    for ( Int_t j = j1; j < j2; j++ ) {
      Double_t cont;
      if ( axis == 1 ) cont = histi->GetBinContent( j+1, i+1 );
      if ( axis == 2 ) cont = histi->GetBinContent( i+1, j+1 );
      htmp->SetBinContent( j+1, cont );
    }
    if ( htmp->GetSumOfWeights() < totsum/n1*cut ) continue;

    if ( method == kProf ) {
      Double_t sqsum = TMath::Sqrt( htmp->GetSumOfWeights() );
      if ( sqsum > 0 ) {
	histo->SetBinContent( i+1, htmp->GetMean() );
	histo->SetBinError  ( i+1, htmp->GetRMS ()/sqsum );
      }
    }
    if ( method == kRMS ) {
      histo->SetBinContent( i+1, htmp->GetMean() );
      histo->SetBinError  ( i+1, htmp->GetRMS () );
    }
    if ( method == kGmean || method == kGsigma || method == kGaus ) {
      htmp->Fit( "gaus", "q0", "", min, max );
      TF1 *func = htmp->GetFunction( "gaus" );
      if ( method == kGmean ) {
	histo->SetBinContent( i+1, func->GetParameter( 1 ) );
	histo->SetBinError  ( i+1, func->GetParError ( 1 ) );
      }
      if ( method == kGsigma ) {
	histo->SetBinContent( i+1, func->GetParameter( 2 ) );
	histo->SetBinError  ( i+1, func->GetParError ( 2 ) );
      }
      if ( method == kGaus ) {
	histo->SetBinContent( i+1, func->GetParameter( 1 ) );
	histo->SetBinError  ( i+1, func->GetParameter( 2 ) );
      }
    }
    if ( method == kLandau ) {
      htmp->Fit( "landau", "q0", "", min, max );
      TF1 *func = htmp->GetFunction( "landau" );
      histo->SetBinContent( i+1, func->GetParameter( 1 ) );
      histo->SetBinError  ( i+1, func->GetParError ( 1 ) );
    }
  }

  delete htmp;

  histo->SetEntries( histo->GetSumOfWeights() );

  delete [] bin1;
  delete [] bin2;

  return histo;
}

// ratio in the bin

TH1D *ratio( TH2 *histi, Int_t axis, Double_t min1, Double_t max1,
	     Double_t min2 = 0, Double_t max2 = 0 )
{
  TString hname = histi->GetName();
  TAxis *ax = histi->GetXaxis();
  TAxis *ay = histi->GetYaxis();
  Int_t  nx = histi->GetNbinsX();
  Int_t  ny = histi->GetNbinsY();

  TAxis *a1, *a2;
  Int_t n1, n2;
  if      ( axis == 1 ) { a1 = ay; a2 = ax; n1 = ny; n2 = nx; }
  else if ( axis == 2 ) { a1 = ax; a2 = ay; n1 = nx; n2 = ny; }
  else return 0;

  if ( min2 == 0 && max2 == 0 ) { 
    min2 = a2->GetXmin(); 
    max2 = a2->GetXmax();
  }

  Double_t *bin = new Double_t[n1+1];
  for ( Int_t i = 0; i <= n1; i++ ) bin[i] = a1->GetBinLowEdge( i+1 );

  Int_t j11 = a2->FindBin( min1 );
  Int_t j12 = a2->FindBin( max1 );
  Int_t j21 = a2->FindBin( min2 );
  Int_t j22 = a2->FindBin( max2 );

  hname += "rt"; hname += axis;
  TH1D *histo = new TH1D( hname, histi->GetTitle(), n1, bin );
  for ( Int_t i = 0; i < n1; i++ ) {
    Double_t cont = 0, sum = 0;
    for ( Int_t j = j11; j < j12; j++ ) {
      if ( axis == 1 ) cont += histi->GetBinContent( j, i+1 );
      if ( axis == 2 ) cont += histi->GetBinContent( i+1, j );
    }
    for ( Int_t j = j21; j < j22; j++ ) {
      if ( axis == 1 ) sum += histi->GetBinContent( j, i+1 );
      if ( axis == 2 ) sum += histi->GetBinContent( i+1, j );
    }
    if ( cont > 0 && sum > 0 ) {
      histo->SetBinError  ( i+1, TMath::Sqrt( 1/cont+1/sum )*cont/sum );
      histo->SetBinContent( i+1, cont/sum );
    }
  }

  delete [] bin;

  return histo;
}


// rebin

TH1D *rebin( TH1 *histi, Int_t rebin, Int_t del = 0 )
{
  TAxis *ax = histi->GetXaxis();
  Int_t nbo;
  Double_t *bin = getrebin( ax, rebin, nbo );
    
  TString hname = histi->GetName();
  if ( del ) histi->SetName( hname+"del" );
  else hname += "rb";

  TH1D *histo = new TH1D( hname, histi->GetTitle(), nbo, bin );
  for ( Int_t i = 0; i < nbo; i++ ) {
    Double_t cont = 0;
    for ( Int_t j = 0; j < rebin; j++ ) {
      Int_t ib = i*rebin+j+1;
      if ( ib > histi->GetNbinsX() ) ib = histi->GetNbinsX();
      cont += histi->GetBinContent( ib );
    }
    histo->SetBinContent( i+1, cont );
  }
  delete [] bin;

  histo->SetEntries( histi->GetEntries() );

  if ( del ) delete histi;

  return histo;
}

TH2D *rebin( TH2 *histi, Int_t rebin, Int_t axis = 1, Int_t del = 0 )
{
  TAxis *ax;
  Int_t nbx, nby, rbx = 1, rby = 1;
  Double_t *xbin = 0, *ybin = 0;

  if ( axis & 1 ) {
    ax = histi->GetXaxis();
    xbin = getrebin( ax, rebin, nbx );
    rbx = rebin;
  }
  if ( axis & 2 ) {
    ax = histi->GetYaxis();
    ybin = getrebin( ax, rebin, nby );
    rby = rebin;
  }

  if ( !xbin ) {
    nbx  = histi->GetNbinsX();
    xbin = new Double_t[nbx+1];
    for ( Int_t i = 0; i <= nbx; i++ )
      xbin[i] = histi->GetXaxis()->GetBinLowEdge( i+1 );
  }
  if ( !ybin ) {
    nby  = histi->GetNbinsY();
    ybin = new Double_t[nby+1];
    for ( Int_t i = 0; i <= nby; i++ )
      ybin[i] = histi->GetYaxis()->GetBinLowEdge( i+1 );
  }

  TString hname = histi->GetName();
  if ( del ) histi->SetName( hname+"del" );
  else hname += "rb";

  TH2D *histo = new TH2D( hname, histi->GetTitle(),
			  nbx, xbin, nby, ybin );

  for ( Int_t i = 0; i < nbx; i++ )
    for ( Int_t j = 0; j < nby; j++ ) {
      Double_t cont = 0;
      for ( Int_t k = 0; k < rbx; k++ )
	for ( Int_t l = 0; l < rby; l++ ) {
	  Int_t ibx = i*rbx+k+1;
	  Int_t iby = j*rby+l+1;
	  if ( ibx > histi->GetNbinsX() ) ibx = histi->GetNbinsX();
	  if ( iby > histi->GetNbinsY() ) iby = histi->GetNbinsY();
	  cont += histi->GetBinContent( ibx, iby );
	}
      histo->SetBinContent( i+1, j+1, cont );
    }

  histo->SetEntries( histi->GetEntries() );

  delete [] xbin;
  delete [] ybin;

  if ( del ) delete histi;

  return histo;
}

Double_t *getrebin( TAxis *ax, Int_t rebin, Int_t &nbo )
{
  Int_t nbi = ax->GetNbins();
  nbo = nbi/rebin;
  if ( nbi%rebin != 0 ) nbo++;

  Double_t *bin = new Double_t[nbo+1];
  for ( Int_t i = 0; i <= nbo; i++ ) {
    if ( i*rebin <= nbi ) bin[i] = ax->GetBinLowEdge( i*rebin+1 );
    else bin[i] = ax->GetBinLowEdge( nbi+1 );
  }

  return bin;
}


// conversion

void conv( TH1 *hist, TF1 *func )
{
  for ( Int_t i = 0; i < hist->GetNbinsX(); i++ ) {
    Double_t x = hist->GetBinCenter ( i+1 );
    Double_t c = hist->GetBinContent( i+1 );
    Double_t e = hist->GetBinError  ( i+1 );
    Double_t f = func->Eval( x );
    hist->SetBinContent( i+1, c*f );
    hist->SetBinError  ( i+1, e*f );
  }
}

// scale

void scale( TH1 *hist, Double_t scl = 0 )
{
  if ( scl == 0 ) scl = hist->GetSumOfWeights();
  if ( scl == 0 ) return;

  for ( Int_t i = 0; i < hist->GetNbinsX(); i++ ) {
    Double_t cnt = hist->GetBinContent( i+1 );
    hist->SetBinContent( i+1, cnt/scl );
    hist->SetBinError  ( i+1, TMath::Sqrt( cnt )/scl );
  }
}

void scale( TH2 *hist, Double_t scale, Double_t max = 0 )
{
  Int_t nx = hist->GetNbinsX();
  Int_t ny = hist->GetNbinsY();

  Double_t mx = 0;

  for ( Int_t i = 0; i < nx; i++ )
    for ( Int_t j = 0; j < ny; j++ ) {
      Double_t ct = hist->GetBinContent( i+1, j+1 )*scale;
      if ( ct < 1 && gRandom->Rndm() < ct ) ct = 1;
      if ( max > 0 && ct > max ) ct = max;
      ct = TMath::Floor( ct );
      if ( ct > mx ) mx = ct;
      hist->SetBinContent( i+1, j+1, ct );
    }
}


// integral

TH1D *integral( TH1D *histi, Double_t scale = 0, 
		Double_t xmin = 0, Double_t xmax = 0 )
{
  if ( xmin == 0 && xmax == 0 ) {
    xmin = histi->GetXaxis()->GetXmin();
    xmax = histi->GetXaxis()->GetXmax();
  }
  Int_t ib1 = histi->FindBin( xmin );
  Int_t ib2 = histi->FindBin( xmax )-1;

  if ( scale == 0 ) scale = histi->GetSumOfWeights();
  if ( scale == 0 ) scale = 1;

  TString hname = histi->GetName();
  TH1D *histo = (TH1D *)histi->Clone( hname+"int" );
  for ( Int_t i = 1; i <= ib2; i++ ) 
    histo->SetBinContent( i, histi->Integral( ib1, i )/scale );

  return histo;
}

// division

TH1D *divide( TH1 *hist1, TH1 *hist2, Int_t err = 1 )
{
  TString hname = hist1->GetName();
  TH1D *histr = (TH1D *)hist1->Clone( hname+"div" );

  for ( Int_t i = 0; i < histr->GetNbinsX(); i++ ) {
    Double_t bc1 = hist1->GetBinContent( i+1 );
    Double_t bc2 = hist2->GetBinContent( i+1 );
    Double_t be1 = hist1->GetBinError  ( i+1 );
    Double_t be2 = hist2->GetBinError  ( i+1 );
    if ( bc1 > 0 && bc2 > 0 ) {
      Double_t bc3 = bc1/bc2;
      Double_t be3 = bc3*TMath::Sqrt( be1*be1/bc1/bc1+be2*be2/bc2/bc2 );
      histr->SetBinContent( i+1, bc3 );
      if ( err ) histr->SetBinError  ( i+1, be3 );
    }
  }

  return histr;
}


// cut contents with funcs of axis value

void cutcont( TH2 *hist, TF1 *funcl, TF1 *funch, Int_t axis = 1 )
{
  TAxis *a1, *a2;
  Int_t nb1, nb2;
  if ( axis == 1 ) {
    a1 = hist->GetXaxis(); nb1 = hist->GetNbinsX();
    a2 = hist->GetYaxis(); nb2 = hist->GetNbinsY();
  }else if ( axis == 2 ) {
    a1 = hist->GetYaxis(); nb1 = hist->GetNbinsY();
    a2 = hist->GetXaxis(); nb2 = hist->GetNbinsX();
  }else return;
	  
  for ( Int_t i = 0; i < nb1; i++ ) {
    Double_t x1 = a1->GetBinCenter( i+1 );
    for ( Int_t j = 0; j < nb2; j++ ) {
      Double_t x2 = a2->GetBinCenter( j+1 );
      Int_t cut = 0;
      if ( funcl && funcl->GetXmin() <= x1 && x1 <= funcl->GetXmax()
	   && x2 < funcl->Eval( x1 ) ) cut = 1; 
      if ( funch && funch->GetXmin() <= x1 && x1 <= funch->GetXmax()
	   && x2 > funch->Eval( x1 ) ) cut = 1;
      if ( cut ) {
	if ( axis == 1 ) hist->SetBinContent( i+1, j+1, 0 );
	if ( axis == 2 ) hist->SetBinContent( j+1, i+1, 0 );
      }
    }
  }
}


// auto fill

TH1F *fill( TTree *ttree, const char *expr, const char *cut,
	    const char *hid, Int_t nbin )
{
  if ( !gDirectory ) return 0;
  TDirectory *dir = gDirectory;

  delete dir->Get( "htmp" );
  ttree->Draw( Form( "%s>>htmp", expr ), cut, "goff" );

  TH1F *htmp = (TH1F *)dir->Get( "htmp" );
  if ( !htmp ) return 0;

  Double_t xmin = htmp->GetXaxis()->GetXmin();
  Double_t xmax = htmp->GetXaxis()->GetXmax();
  delete htmp;

  TH1F *hist = new TH1F( hid, expr, nbin, xmin, xmax );
  ttree->Draw( Form( "%s>>%s", expr, hid ), cut, "goff" );

  return hist;
}


// chain fill

void chfill( const char *fname, const char *tname, const char *hname,
	     const char *expr,  const char *cut )
{
  TChain ch( tname );
  ch.Add( fname );
  
  TObjArray *lf = ch.GetListOfFiles();
  for ( Int_t i = 0; i < lf->GetEntries(); i++ ) {
    TChainElement *che = (TChainElement *)lf->At( i );
    if ( !che ) continue;

    TFile f( che->GetTitle() );
    TTree *ttree = (TTree *)f.Get( tname );

    gROOT->cd();
    cout << che->GetTitle() << " "; cout.flush();
    cout << ttree->Draw( Form( "%s>>+%s", expr, hname ), cut, "goff" )
	 << endl;
  }
}


// draw marker

Long64_t tgdraw( TTree *tree, const char *expr, const char *cut,
		 const char *option = "", Int_t sty = 29, Int_t col = 1 )
{
  Long64_t n = tree->Draw( expr, cut, option );
  TGraph *gr = new TGraph( tree->GetSelectedRows(), 
			   tree->GetV2(), tree->GetV1() );
  gr->SetMarkerStyle( sty );
  gr->SetMarkerColor( col );
  gr->Draw( "p" );

  return n;
}


// conversions

TH2F *d2f( TH2D *hist1 )
{
  TString hname = hist1->GetName();
  Int_t nbx = hist1->GetNbinsX(), nby = hist1->GetNbinsY();
  Double_t *xbin = new Double_t[nbx+1];
  Double_t *ybin = new Double_t[nby+1];

  TAxis *ax = hist1->GetXaxis();
  TAxis *ay = hist1->GetYaxis();

  for ( Int_t i = 0; i <= nbx; i++ ) xbin[i] = ax->GetBinLowEdge( i+1 );
  for ( Int_t i = 0; i <= nby; i++ ) ybin[i] = ay->GetBinLowEdge( i+1 );
  
  TH2F *hist2 = new TH2F( hname+"f", hist1->GetTitle(),
			  nbx, xbin, nby, ybin );
  for ( Int_t i = 0; i < nbx; i++ )
    for ( Int_t j = 0; j < nby; j++ )
      hist2->SetBinContent( i+1, j+1, hist1->GetBinContent( i+1, j+1 ) );

  delete [] xbin;
  delete [] ybin;

  return hist2;
}

// set attributes

void attset( TH1 *hist, Int_t col, Int_t msty = 1, Double_t msiz = 1, 
	     Int_t fsty = 1001, Int_t fcol = 0,
	     Int_t lsty = 1, Double_t lwid = 1 )
{
  hist->SetMarkerStyle( msty );
  hist->SetMarkerColor(  col );
  hist->SetMarkerSize ( msiz );
  hist->SetFillStyle  ( fsty );
  hist->SetFillColor  ( fcol );
  hist->SetLineStyle  ( lsty );
  hist->SetLineColor  (  col );
  hist->SetLineWidth  ( lwid );
}

// attributes copy

void attcopy( TH1 *src, TH1 *dst )
{
  src->SetFillStyle( dst->GetFillStyle() );
  src->SetFillColor( dst->GetFillColor() );
  src->SetLineStyle( dst->GetLineStyle() );
  src->SetLineColor( dst->GetLineColor() );
}

void attcopy( TH2 *src, TH2 *dst )
{
  src->SetFillStyle( dst->GetFillStyle() );
  src->SetFillColor( dst->GetFillColor() );
  src->SetLineStyle( dst->GetLineStyle() );
  src->SetLineColor( dst->GetLineColor() );
}

void attcopy( TH3 *src, TH3 *dst )
{
  src->SetFillStyle( dst->GetFillStyle() );
  src->SetFillColor( dst->GetFillColor() );
  src->SetLineStyle( dst->GetLineStyle() );
  src->SetLineColor( dst->GetLineColor() );
}

