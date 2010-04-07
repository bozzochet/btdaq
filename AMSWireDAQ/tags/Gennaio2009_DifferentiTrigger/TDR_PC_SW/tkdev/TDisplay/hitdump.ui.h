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
#include "RTDisplay.h"

void HitDump::init(){
    
cbLayers->insertItem(QString("Layer 1"),0);    
   cbLayers->insertItem(QString("Layer 2"),1); 
   cbLayers->insertItem(QString("Layer 3"),2); 
   cbLayers->insertItem(QString("Layer 4"),3); 
   cbLayers->insertItem(QString("Layer 5"),4); 
   cbLayers->insertItem(QString("Layer 6"),5); 
cbLayers->insertItem(QString("Layer 7"),6); 
cbLayers->insertItem(QString("Layer 8"),7); 
cbLayers->insertItem(QString("All Layers"),8); 

cbType->insertItem(QString("TrRawCluster"),0);
cbType->insertItem(QString("TrCluster"),1);
cbType->insertItem(QString("TrRecHit"),2);
Update();
}



void HitDump::Update()
{
    char msg[1000];
    Event* ev=RTrackerDisplay::fSelfPtr->fEvent;
    teDump->clear();
    int type=cbType->currentItem();
    int layer=cbLayers->currentItem()+1;
    int count=0;    
//    sprintf(msg,"Layer %d \n",layer);
//    teDump->setText(QString(msg));
    if(type==2){
        for (int ii=0;ii<ev->GetnTrRecHits();ii++){
              AMSTrRecHit* hit=ev->GetTrRecHit(ii);
              if( hit->GetLayer()==layer|| layer==9){
               count++;
            sprintf(msg,"Hit %d ",count);
           teDump->append(QString(msg));
           hit->Info();
               teDump->append(QString(hit->sout));
              }
         }
    }
 if(type==1){
        for (int ii=0;ii<ev->GetnTrClusters();ii++){
              AMSTrCluster* hit=ev->GetTrCluster(ii);
              if( hit->GetLayer()==layer|| layer==9){
               count++;
            sprintf(msg,"Cluster %d ",count);
           teDump->append(QString(msg));
           hit->Info();
               teDump->append(QString(hit->sout));
              }
         }
    }
 if(type==0){
        for (int ii=0;ii<ev->GetnTrRawClusters();ii++){
              AMSTrRawCluster* hit=ev->GetTrRawCluster(ii);
              if( hit->GetLayer()==layer|| layer==9){
               count++;
            sprintf(msg,"Cluster %d ",count);
           teDump->append(QString(msg));
           hit->Info();
               teDump->append(QString(hit->sout));
              }
         }
    }
 
}



