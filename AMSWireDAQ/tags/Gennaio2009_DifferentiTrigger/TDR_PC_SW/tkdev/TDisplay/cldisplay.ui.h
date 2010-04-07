/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/
#include "Event.h"
#include "TrRecon.h"
#include "RTDisplay.h"

void ClDisplay::init()
{
    Ssig    = 0;
    Snoise = 0;
    Sstat   = 0;
    cls=0;
}


void ClDisplay::UpdateS()
{
    if (!RTrackerDisplay::fSelfPtr || !RTrackerDisplay::fSelfPtr->fEvent) return;

    teSOUT->clear();
    CanvasS->GetCanvas()->Clear();
    
    if(Ssig)   { delete Ssig;   Ssig   = 0; }
    if(Snoise) { delete Snoise; Snoise = 0; }
    if(Sstat)  { delete Sstat;  Sstat  = 0; }

    Event* ev=RTrackerDisplay::fSelfPtr->fEvent;

    int idx = sbClS1->text().toInt();
    if (idx < 0 || ev->NTrClusters <= idx) {
      CanvasS->GetCanvas()->Update();
      return;
    }
    AMSTrCluster* cls=ev->GetTrCluster(idx);
    if (!cls) return;
    
    cls->Info();
     teSOUT->append(QString(cls->sout));
    int infs   =cls->GetAddress(0)-10;
    int sups =cls->GetAddress(0)+10+cls->GetNelem();
    Ssig    = new TH1F("SignalS","SignalS;Channel;ADC count",sups-infs,infs,sups);
    Snoise = new TH1F("NoiseS","NoiseS;Channel;ADC count",sups-infs,infs,sups);
    Sstat   = new TH1F("StatusS","StatusS;Channel;ADC count",sups-infs,infs,sups);
 
    for(int ii=0;ii<cls->GetNelem();ii++){
         Ssig->SetBinContent(ii+11,cls->GetSignal(ii));
         Snoise->SetBinContent(ii+11,cls->GetSigma(ii));
        if( cls->GetStatus(ii)!=0)Sstat->SetBinContent(ii+11,-2);
    }
    CanvasS->GetCanvas()->cd();
    Ssig->Draw();
    Snoise->SetLineColor(4);
    Snoise->SetLineStyle(2);
    Snoise->Draw("same");
    Sstat->SetLineColor(2);
    Sstat->Draw("same");    
    CanvasS->GetCanvas()->Update();
}

static bool chain  = true;
static int  player = 0;
static int  ptkid  = 0;

void ClDisplay::UpdateEvent()
{
    if (!RTrackerDisplay::fSelfPtr || !RTrackerDisplay::fSelfPtr->fEvent) return;

    if (RTrackerDisplay::fSelfPtr->fEvent->NTrClusters == 0) {
      sbClS1->setMinValue(0); sbClS1->setMaxValue(0); 
      sbClS2->setMinValue(0); sbClS2->setMaxValue(0);
      sbClS3->setMinValue(0); sbClS3->setMaxValue(0);
      sbClS4->setMinValue(0); sbClS4->setMaxValue(0);
      rbNside->setChecked(true);
      rbPside->setEnabled(false);
      UpdateS();
      return;
    }
    sbClS3->setMinValue(-815); sbClS3->setMaxValue(815);

    sbClS1->blockSignals(true);
    sbClS1->setMinValue(0);
    sbClS1->setValue(0);
    sbClS1->setMaxValue(RTrackerDisplay::fSelfPtr->fEvent->NTrClusters-1);
    sbClS1->blockSignals(false);

    int layer0 = 8, layer1 = 1;
    for (int ly = 0; ly <= 8; ly++)
      if (TrRecon::gethead()->GetNladder(ly) > 0) { 
	if (ly < layer0) layer0 = ly;
	if (ly > layer1) layer1 = ly;
      }
    sbClS2->blockSignals(true);
    sbClS2->setMinValue(layer0);
    sbClS2->setValue(layer0);
    sbClS2->setMaxValue(layer1);
    player = layer0;
    sbClS2->blockSignals(false);

    UpdateDigits();
    UpdateS();
}


void ClDisplay::UpdateDigits()
{
    if (!RTrackerDisplay::fSelfPtr || !RTrackerDisplay::fSelfPtr->fEvent ||
	!RTrackerDisplay::fSelfPtr->fEvent->NTrClusters || !TrRecon::gethead()) return;

    Event* ev = RTrackerDisplay::fSelfPtr->fEvent;
    int   idx = sbClS1->text().toInt();
    if (idx < 0 || ev->NTrClusters <= idx) return;
    AMSTrCluster* cls=ev->GetTrCluster(idx);
    if (!cls) return;

    int layer = cls->GetLayer();
    int tkid  = cls->GetTkId();
    int side  = cls->GetSide();

    int icls = 0, ncls = TrRecon::gethead()->GetnTrClusters(tkid, side);
    for (int i = 0; i < ncls; i++)
      if (TrRecon::gethead()->GetTrCluster(tkid, side, i) == cls) {
	icls = i;
	break;
      }

//  cout << "UpdateDigits1 " << layer << " " << tkid << " " << side << endl;

    chain = false;
    sbClS2->setValue(layer);
    sbClS3->setValue(tkid);
    if (side == 0) rbNside->setChecked(true);
    else rbPside->setChecked(true);
    sbClS4->setValue(icls);
    chain = true;
}



void ClDisplay::UpdateLayer()
{
    if (!RTrackerDisplay::fSelfPtr || !RTrackerDisplay::fSelfPtr->fEvent ||
	!RTrackerDisplay::fSelfPtr->fEvent->NTrClusters || !TrRecon::gethead()) return;

    int layer = sbClS2->text().toInt();

//  cout << "UpdateLayer0 " << player << " " << layer << endl;

    if (layer < sbClS2->minValue() || sbClS2->maxValue() < layer) {
	cerr << "Consistency check failed at UpdateLayer1 " 
	     << sbClS2->minValue() << " " << layer << " " 
	     << sbClS2->maxValue() << " " << player << endl;
	return;
    }

    while (TrRecon::gethead()->GetNladder(layer) <= 0) {
      if (layer == player) {
	cerr << "Consistency check failed at UpdateLayer2 " << player << endl;
	return;
      }
      if      (player < layer && layer < sbClS2->maxValue()) layer++;
      else if (player > layer && layer > sbClS2->minValue()) layer--;
      else layer = player;
    }
    player = layer;
    sbClS2->setValue(layer);

    if (chain) {
      int tkid = TrRecon::gethead()->GetLadderHit(layer, 0);

  //  cout << "UpdateLayer1 " << layer << " " << tkid << endl;

      sbClS3->blockSignals(true);
      sbClS3->setValue(tkid);
      ptkid = tkid;
      sbClS3->blockSignals(false);
      UpdateNP();
    }
}


void ClDisplay::UpdateTkIdRange()
{
  return;

    if (!RTrackerDisplay::fSelfPtr || !RTrackerDisplay::fSelfPtr->fEvent ||
	!RTrackerDisplay::fSelfPtr->fEvent->NTrClusters ||
	!TrRecon::gethead()) return;

    int layer = sbClS2->text().toInt();
    int nlad  = TrRecon::gethead()->GetNladder(layer);
    if (nlad <= 0) {
  //  cout << "UpdateTkIdRange-1 layer to be updated" << endl;
      return;
    }

    int tkid0 = TrRecon::gethead()->GetLadderHit(layer, 0);
    int tkid1 = TrRecon::gethead()->GetLadderHit(layer, nlad-1);

//  cout << "UpdateTkIdRange1 " << tkid0 << " " << tkid1 << endl;

    sbClS3->blockSignals(true);
    sbClS3->setMinValue(tkid0);
    sbClS3->setValue(tkid0);
    ptkid = tkid0;
    sbClS3->setMaxValue(tkid1);
    sbClS3->blockSignals(false);

    if (chain) UpdateNP();
}


void ClDisplay::UpdateTkId()
{
    if (!RTrackerDisplay::fSelfPtr || !RTrackerDisplay::fSelfPtr->fEvent ||
	!RTrackerDisplay::fSelfPtr->fEvent->NTrClusters || !TrRecon::gethead()) return;

    int tkid = sbClS3->text().toInt();
    if (TMath::Abs(tkid)/100 < 1 || 8 < TMath::Abs(tkid)/100 ||	TMath::Abs(tkid%100) > 15) {
      sbClS3->setValue(ptkid);
      return;
    }

    int layer = sbClS2->text().toInt();
    int nlad  = TrRecon::gethead()->GetNladder(layer);
    if (nlad <= 0) {
      cerr << "Consistency check failed at UpdateTkId1 " 
	   << layer << " " << tkid << " " << ptkid << endl;
      return;
    }

    int nclsn = TrRecon::gethead()->GetnTrClusters(tkid, 0);
    int nclsp = TrRecon::gethead()->GetnTrClusters(tkid, 1);

//  cout << "UpdateTkId1 " << layer << " " << nlad << " | " 
//	 << ptkid << " " << tkid << " | " << nclsn << " " << nclsp << endl;

    if (nclsn > 0 || nclsp > 0) {
  //  cout << "UpdateTkId check OK" << endl;
      ptkid = tkid;
      return;
    }
    else if (tkid == ptkid) {
      cerr << "Consistency check failed at UpdateTkId2 " 
	   << layer << " " << tkid << " " << endl;
      return;
    }
    
    int tkid0 = TrRecon::gethead()->GetLadderHit(layer, 0);
    int tkid1 = TrRecon::gethead()->GetLadderHit(layer, nlad-1);
    if (TMath::Abs(tkid)/100 != layer || tkid < tkid0 || tkid1 < tkid1) {
      sbClS3->setValue(ptkid);
      return;
    }

    int idx = -1;
    for (int i = 0; i < nlad; i++)
      if (TrRecon::gethead()->GetLadderHit(layer, i) == ptkid) {
	idx = i;
	break;
      }
    if (idx < 0) {
      cerr << "Consistency check failed at UpdateTkId3 "
	   << layer << " " << nlad << " " << ptkid << " " << tkid << endl;
      return;
    }
    int i0 = (idx == 0)      ? 0      : idx-1;
    int i1 = (idx == nlad-1) ? nlad-1 : idx+1;

    tkid0 = TrRecon::gethead()->GetLadderHit(layer, i0);
    tkid1 = TrRecon::gethead()->GetLadderHit(layer, i1);
    tkid = (tkid < ptkid) ? tkid0 : tkid1;

//  cout << "UpdateTkId2 " << idx << " | " << i0 << " " << i1 << " | "
//       << tkid0 << " " << tkid1 << " | " << ptkid << " " << tkid << endl;

    ptkid = tkid;
    sbClS3->setValue(tkid);
}


void ClDisplay::UpdateNP()
{
    if (!RTrackerDisplay::fSelfPtr || !RTrackerDisplay::fSelfPtr->fEvent ||
	!RTrackerDisplay::fSelfPtr->fEvent->NTrClusters || !TrRecon::gethead()) return;

    int tkid  = sbClS3->text().toInt();
    int nclsn = TrRecon::gethead()->GetnTrClusters(tkid, 0);
    int nclsp = TrRecon::gethead()->GetnTrClusters(tkid, 1);

    if (nclsn == 0 && nclsp == 0) {
  //  cout << "UpdateNP tkid to be updated" << endl;
      return;
    }

    rbNside->setEnabled((nclsn > 0 && nclsp > 0));
    rbPside->setEnabled((nclsn > 0 && nclsp > 0));

//  cout << "UpdateNP " << tkid << " " << nclsn << " " << nclsp << endl;

    
    rbNside->blockSignals(true);
    rbPside->blockSignals(true);
    if (nclsn > 0) { 
      rbNside->setChecked(true); rbPside->setChecked(false); }
    else if (nclsp > 0) { 
      rbPside->setChecked(true); rbNside->setChecked(false); }
    else cerr << "Consistency check failed at UpdatNP " 
	      << tkid << " " << nclsn << " " << nclsp << endl;
    rbNside->blockSignals(false);
    rbPside->blockSignals(false);
    
    if (chain) UpdateIclsRange();
}


void ClDisplay::UpdateIclsRange()
{
    if (!RTrackerDisplay::fSelfPtr || !RTrackerDisplay::fSelfPtr->fEvent ||
	!RTrackerDisplay::fSelfPtr->fEvent->NTrClusters || !TrRecon::gethead()) return;

    int tkid = sbClS3->text().toInt();
    int side = (rbNside->isChecked()) ? 0 : 1;
    int ncls = (tkid != 0) ? TrRecon::gethead()->GetnTrClusters(tkid, side) : 0;
//  cout << "UpdateIclsRange " << tkid << " " << side << " " << ncls << endl;

    sbClS4->blockSignals(true);
    sbClS4->setMinValue(0);
    sbClS4->setValue(0);
    sbClS4->setMaxValue(TMath::Max(ncls-1, 0));
    sbClS4->blockSignals(false);

    if (chain) UpdateIdx();
}


void ClDisplay::UpdateIdx()
{
    if (!RTrackerDisplay::fSelfPtr || !RTrackerDisplay::fSelfPtr->fEvent ||
	!RTrackerDisplay::fSelfPtr->fEvent->NTrClusters || !TrRecon::gethead()) return;

    int tkid  = sbClS3->text().toInt();
    int icls  = sbClS4->text().toInt();
    int side  = (rbNside->isChecked()) ? 0 : 1;
    if (tkid == 0) return;

    int ncls = TrRecon::gethead()->GetnTrClusters(tkid, side);
    if (ncls <= 0 || icls < 0 || icls >= ncls) return;

    TrClusterR *cls = TrRecon::gethead()->GetTrCluster(tkid, side, icls);

    Event* ev = RTrackerDisplay::fSelfPtr->fEvent;
    int idx = -1;
    for (int i = 0; i < ev->NTrClusters; i++)
      if (ev->GetTrCluster(i) == cls) {
	idx = i;
	break;
      }

//  cout << "UpdateIdx " << layer << " " << tkid << " " << icls << " "
//       << side << " " << ncls << " " << idx << endl;

    if (!chain) sbClS1->blockSignals(true);
    if (0 <= idx && idx < ev->NTrClusters) sbClS1->setValue(idx);
    if (!chain) sbClS1->blockSignals(false);
}


void ClDisplay::Exit()
{
    this->close();
}


void ClDisplay::rbPside_toggled(bool sw)
{
  if (!(sw^rbNside->isChecked())) rbNside->setChecked(!sw);
  if (sw) UpdateIclsRange();
}


void ClDisplay::rbNside_toggled(bool sw)
{
  if (!(sw^rbPside->isChecked())) rbPside->setChecked(!sw);
  if (sw) UpdateIclsRange();
}
