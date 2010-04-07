// $Id: hdiff.C,v 1.1 2008/01/21 21:58:35 haino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  hdiff.C
///\brief Compare all histgrams in two files named fname1 and fname2.
///
/// Comparison accuracy can be set as the third argument, acy
///
///\date  2007/11/26 SH  Imported from SH personal library
///$Date: 2008/01/21 21:58:35 $
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////


void hdiff(const char *fname1, const char *fname2, Double_t acy = 1e-10)
{
///\brief Main function
///
/// Compare between two files

  TFile f1(fname1); if (!f1.IsOpen()) return;
  TFile f2(fname2); if (!f2.IsOpen()) return;

  TList *ls = f1.GetListOfKeys();
  TKey *key;

  for (TIter next(ls); (key = (TKey *)next());) {
    TObject *o1 = f1.Get(key->GetName());
    TString cls = o1->ClassName();

    if (cls.Contains("TH")) {
      TObject *o2 = f2.Get(key->GetName());
      if (!o2)
	cout << "Only in \"" << fname1 << "\" : " << key->GetName() << endl;
      else if (cls.Contains("TH1")) hdiff((TH1*)o1, (TH1*)o2, acy);
      else if (cls.Contains("TH2")) hdiff((TH2*)o1, (TH2*)o2, acy);
    }
  }
}

void hdiff(TDirectory *dir1, TDirectory *dir2, Double_t acy = 1e-10)
{
///\brief Compare between two directories

  TList *ls = dir1->GetListOfKeys();
  TKey *key;

  for (TIter next(ls); (key = (TKey *)next());) {
    TObject *o1 = dir1->Get(key->GetName());
    TString cls = o1->ClassName();

    if (cls.Contains("TH")) {
      TObject *o2 = dir2->Get(key->GetName());
      if (!o2)
	cout << "Only in 1 : " << key->GetName() << endl;
      else if (cls.Contains("TH1")) hdiff((TH1*)o1, (TH1*)o2, acy);
      else if (cls.Contains("TH2")) hdiff((TH2*)o1, (TH2*)o2, acy);
    }
  }
}

Int_t hdiff(TH1 *hist1, TH1 *hist2, Double_t accuracy = 1e-10)
{
///\brief Compare between two histograms

  Int_t n1 = hist1->GetNbinsX();
  Int_t n2 = hist2->GetNbinsX();

  if (n1 != n2) {
    cout << "Bin differ : " << hist1->GetName() << endl;
    return -1;
  }

  for (Int_t i = 0; i < n1; i++) {
    Double_t c1 = hist1->GetBinContent(i+1);
    Double_t c2 = hist2->GetBinContent(i+1);

    if (c1+c2 != 0 && TMath::Abs((c1-c2)/(c1+c2)) > accuracy ) {
      cout << "Cont differ : " << hist1->GetName() << endl;
      return 1;
    }
  }

  cout << "Same : " << hist1->GetName() << endl;
  return 0;
}

Int_t hdiff(TH2 *hist1, TH2 *hist2)
{
///\brief Compare between two histograms (2D)

  Int_t nx1 = hist1->GetNbinsX();
  Int_t ny1 = hist1->GetNbinsY();
  Int_t nx2 = hist2->GetNbinsX();
  Int_t ny2 = hist2->GetNbinsY();

  if (nx1 != nx2 || ny1 != ny2) {
    cout << "Bin differ : " << hist1->GetName() << endl;
    return -1;
  }

  for (Int_t i = 0; i < nx1; i++)
    for (Int_t j = 0; j < ny1; j++)
      if (hist1->GetBinContent(i+1, j+1) != 
	   hist2->GetBinContent(i+1, j+1)) {
	cout << "Cont differ : " << hist1->GetName() << endl;
	return 1;
      }

  cout << "Same : " << hist1->GetName() << endl;
  return 0;
}
