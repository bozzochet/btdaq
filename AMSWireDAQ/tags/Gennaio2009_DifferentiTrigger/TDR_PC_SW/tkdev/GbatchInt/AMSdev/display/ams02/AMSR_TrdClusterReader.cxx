//  $Id: AMSR_TrdClusterReader.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS TRD Cluster Reader class.                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <math.h>
#include "Debugger.h"
#include <iostream.h>
#include <TRandom.h>

#include <TClonesArray.h>
//#include <TMCParticle.h>

#include "AMSR_Types.h"
#include "AMSR_Root.h"
#include "AMSR_TrdCluster.h"
#include "AMSR_TrdClusterReader.h"
#include "AMSR_Ntuple.h"
#include "TRDClusterRoot.h"

/*
#include "ATLFMCMaker.h"

const Float_t kPI  = TMath::Pi();
const Float_t k2PI = 2*kPI;
const Int_t kMAXCELLS = 2000;
const Int_t kMAXCLU   = 1000;
*/

const Int_t maxTrd=200;
ClassImp(AMSR_TrdClusterReader)


//_____________________________________________________________________________
AMSR_TrdClusterReader::AMSR_TrdClusterReader(const char *name, const char *title)
                 : AMSR_Maker(name,title)
{
   // Construct an AMSR_TrdClusterReader
   //

   m_Fruits     = new TClonesArray("AMSR_TrdCluster",maxTrd,kFALSE);
   m_BranchName = "TrdClusters";
   m_Nclusters  = 0;
//   m_Ncells     = 0;

   //
   // Mark that we want to save things
   //
   Save();

}


//_____________________________________________________________________________
void AMSR_TrdClusterReader::Finish()
{
// Function called when maker for all events have been called
// close histograms... etc.

}

//_____________________________________________________________________________
void AMSR_TrdClusterReader::Make()
{
// Make the branch in result tree
//

   EDataFileType m_DataFileType=gAMSR_Root->GetDataFileType();    
   Int_t k;
   if (m_DataFileType == kNtupleFile){
     TRDCL_DEF *_ntuple = (gAMSR_Root->GetNtuple())->m_BlkTrdcl;

//........................................................
//....Store clusters in Root ClonesArray
//........................................................
//   m_Ncells    = ncells;
     m_Nclusters = 0;		// it will be accumulated by AddCluster()
     debugger.Print("AMSR_TrdClusterReader::Make(): making %d clusters.\n",
	  _ntuple->ntrdcl);
     for (k=0; k<_ntuple->ntrdcl; k++) {
        AddCluster(_ntuple->status[k],
                &_ntuple->coo[k][0],
                 _ntuple->layer[k],
                &_ntuple->coodir[k][0],
                 _ntuple->multip[k],
                 _ntuple->hmultip[k],
                 _ntuple->edep[k],
                 _ntuple->prawhit[k]);
       }

     }
     else if (m_DataFileType == kRootFile){
        m_Nclusters = 0;		// it will be accumulated by AddCluster()              
        TClonesArray *_roottree = (gAMSR_Root->GetNtuple())->m_trdcl;
        if (_roottree->GetSize()>0) {	
	  TObject *element=_roottree->At(0);
          TRDClusterRoot* p_ok=dynamic_cast<TRDClusterRoot*>(element);
          if (p_ok) {

	   for (int k=0;k<_roottree->GetEntries();k++){

	     element=_roottree->At(k);            
             TRDClusterRoot* p_obj=dynamic_cast<TRDClusterRoot*>(element);

             AddCluster(p_obj->Status, 
                &p_obj->Coo[0],
                 p_obj->Layer,
                &p_obj->CooDir[0],
                 p_obj->Multip,
                 p_obj->HMultip,
                 p_obj->EDep,0);
           }
         }

      }

   }
 
      debugger.Print("AMSR_TrdClusterReader::Make(): %d clusters made.\n", m_Nclusters);
}

//_____________________________________________________________________________
void AMSR_TrdClusterReader::PrintInfo()
{
   AMSR_Maker::PrintInfo();
}

/*
//_____________________________________________________________________________
Bool_t AMSR_TrdClusterReader::Enabled(TObject * obj)
{
   return kTRUE;		// no discrimation among Trd clusters
}
*/

// Temporary... database of lengths
const float numlad[20] = {14,14,14,14,16,16,16,16,16,16,16,16,
18,18,18,18,18,18,18,18};
const float length[20][18] = 
{
46.8906,56.1412,67.0906,78.3506,78.9513,78.3506,78.9513,78.3506,78.9513,
78.3506,78.9513,66.2412,56.9906,46.0412,0.,0.,0.,0.,
48.5894,57.84,68.7894,79.5519,80.1525,79.5519,80.1525,79.5519,80.1525,
79.5519,80.1525,67.94,58.6894,47.74,0.,0.,0.,0.,
50.2882,59.5388,70.4882,80.7531,81.3537,80.7531,81.3537,80.7531,
81.3537,80.7531,81.3537,69.6388,60.3882,49.4388,0.,0.,0.,0.,
51.987,61.2376,72.187,81.9543,82.5549,81.9543,82.5549,81.9543,
82.5549,81.9543,82.5549,71.3376,62.087,51.1376,0.,0.,0.,0.,
43.5163,54.4657,63.7164,74.6657,83.1555,83.7561,83.1555,83.7561,83.1555,
83.7561,83.1555,83.7561,73.8164,64.5657,53.6163,44.3657,0.,0.,
45.2151,56.1645,65.4151,76.3645,84.3567,84.9574,84.3567,84.9574,
84.3567,84.9574,84.3567,84.9574,75.5151,66.2645,55.3151,46.0645,0.,0.,
46.9139,57.8633,67.1139,78.0633,85.558,86.1586,85.558,86.1586,85.558,
86.1586,85.558,86.1586,77.2139,67.9633,57.0139,47.7633,0.,0.,
48.6127,59.5621,68.8127,79.7621,86.7592,87.3598,86.7592,87.3598,
86.7592,87.3598,86.7592,87.3598,78.9127,69.6621,58.7127,49.4621,0.,0.,
50.3115,61.2609,70.5115,81.4609,87.9604,88.561,87.9604,88.561,87.9604,
88.561,87.9604,88.561,80.6115,71.3609,60.4115,51.1609,0.,0.,
52.0103,62.9596,72.2103,83.1596,89.1616,89.7622,89.1616,89.7622,
89.1616,89.7622,89.1616,89.7622,82.3102,73.0596,62.1103,52.8596,0.,0.,
53.709,64.6584,73.909,84.8584,90.3628,90.9634,90.3628,90.9634,90.3628,
90.9634,90.3628,90.9634,84.009,74.7584,63.809,54.5584,0.,0.,
55.4078,66.3572,75.6078,86.5572,91.5641,92.1647,91.5641,92.1647,
91.5641,92.1647,91.5641,92.1647,85.7078,76.4572,65.5078,56.2572,0.,0.,
47.856,57.1066,68.056,77.3066,88.256,92.7653,93.3659,92.7653,93.3659,
92.7653,93.3659,92.7653,93.3659,87.4066,78.156,67.2066,57.956,47.0066,
49.5548,58.8054,69.7548,79.0054,89.9548,93.9665,94.5671,93.9665,94.5671,
93.9665,94.5671,93.9665,94.5671,89.1054,79.8548,68.9054,59.6548,48.7054,
51.2535,60.5042,71.4535,80.7042,91.6535,95.1677,95.7683,95.1677,95.7683,
95.1677,95.7683,95.1677,95.7683,90.8042,81.5535,70.6042,61.3535,50.4042,
52.9523,62.2029,73.1523,82.4029,93.3523,96.3689,96.9695,96.3689,
96.9695,96.3689, 96.9695,96.3689,96.9695,92.5029,83.2523,
72.3029,63.0523,52.1029,
53.8711,63.1217,74.0711,83.3217,94.2711,97.5702,98.1708,97.5702,
98.1708,97.5702,98.1708,97.5702,98.1708,93.4217,84.1711,
73.2217,63.9711,53.0217,
55.5699,64.8205,75.7699,85.0205,95.9699,98.7714,99.372,
98.7714,99.372,98.7714,99.372,98.7714,99.372,95.1205,
85.8699,74.9205,65.6699,54.7205,
57.2687,66.5193,77.4687,86.7193,97.6687,99.9726,100.573,
99.9726,100.573,99.9726, 100.573,99.9726,100.573,96.8193,
87.5687,76.6193,67.3687,56.4193,
58.9674,68.2181,79.1674,88.4181,99.3674,101.174,101.774,101.174,
101.774,101.174,101.774,101.174,101.774,98.5181,89.2674,
78.3181,69.0675,58.1181};

//_____________________________________________________________________________
void  AMSR_TrdClusterReader::AddCluster(Int_t status, 
                                Float_t * coo, Int_t layer,
                                Float_t * coodir,
                                Int_t multip, Int_t hmultip, 
                                Float_t edep, Int_t prawhit)
{
//            Add a new cluster to the list of clusters

 //Note the use of the "new with placement" to create a new cluster object.
 //This complex "new" works in the following way:
 //   clusters[i] is the value of the pointer for cluster number i in the TClonesArray
 //   if it is zero, then a new cluster must be generated. This typically
 //   will happen only at the first events
 //   If it is not zero, then the already existing object is overwritten
 //   by the new cluster parameters.
 // This technique should save a huge amount of time otherwise spent
 // in the operators new and delete.

   Float_t cooA[3];
   Float_t ercooA[3];
   cooA[0]=coo[0];
   cooA[1]=coo[1];
   cooA[2]=coo[2];

// Get the counter lengths, dumb way
// Probably possible to get this info from geom somehow...?
 
   float wcoo; 
   int ladder_num;

   if (layer<4||layer>15) 
	{
	   ercooA[1]= 0.6;
// Check if inside bulkhead
	   wcoo = fabs(coo[0])-(fabs(coo[0])<613.8?0:0.3);
	   ladder_num = wcoo/10.1*(coo[0]?-1:1)+numlad[layer]/2;
           ercooA[0] = length[layer][ladder_num];

	}
	else
	{
	   ercooA[0] = 0.6;
	   wcoo = fabs(coo[1]);
	   ladder_num = wcoo/10.1*(coo[1]?-1:1)+numlad[layer]/2;
           ercooA[1] = length[layer][ladder_num];

	}

 
    debugger.Print("x,y,layer,ladder_num,ercooA[0],ercooA[1] %f %f %d %d %f %f\n",cooA[0],cooA[1],layer,ladder_num,ercooA[0],ercooA[1]);

// Probably need a better way to indicate energy than this...
   ercooA[2]=sqrt(edep/2);

   TClonesArray &clusters = *(TClonesArray*)m_Fruits;
   new(clusters[m_Nclusters++]) AMSR_TrdCluster(cooA,ercooA);

}

//_____________________________________________________________________________
void AMSR_TrdClusterReader::RemoveCluster(const Int_t cluster)
{
   TClonesArray &clusters = *(TClonesArray*)m_Fruits;
   clusters.RemoveAt(cluster);
   clusters.Compress();
   m_Nclusters--;
}

//_____________________________________________________________________________
void AMSR_TrdClusterReader::Clear(Option_t *option)
{
//    Reset Cluster Maker

   m_Nclusters  = 0;
//   m_Ncells     = 0;
   if (m_Fruits) m_Fruits->Clear();
}






