#include "TrRecon.h"
#include "AMSEvent.h"
#include "amsgobj.h"
#include "point.h"
#include "TrRecon.h"
#include "TrMCCluster.h"

#include "AMSJob.h"

AMSEvent* AMSEvent::_Head=0;
AMSNodeMap AMSEvent::EventMap;


void AMSEvent::_init(){
  const char dbname[100]="/amssw/AMSDataDir/DataBase/";

  sprintf(AMSDBc::amsdatabase,"%s",dbname);
  time_t now;
  
  now = time(NULL);
  AMSID id("TDV:",0);    
  AMSTimeID *p=(AMSTimeID*)AMSJob::JobMap.getp(id);
  if(!p){
    cerr << "AMSJob::gettimestructure-F-no time structure found"<<endl;
    exit(1);
    return;
  }
 
  AMSTimeID * offspring=dynamic_cast<AMSTimeID*>(p->down());
  while(offspring){
    if(!offspring->validate(now)){

      cerr<<"AMSEvent::_validate-F-"<<offspring->getname()<<" not validated."<<endl;
      time_t b,e,i;
      offspring->gettime(i,b,e);
      cerr<<" Time: "<<ctime(&_time)<<endl;
      cerr<<" Begin : " <<ctime(&b)<<endl; 
      cerr<<" End : " <<ctime(&e)<<endl; 
      cerr<<" Insert : " <<ctime(&i)<<endl; 
      exit(2);
 
    }
    offspring=(AMSTimeID*)offspring->next();
  }

  
  _siamsinitrun();
  _reamsinitrun();


  _siamsinitevent();
  _reamsinitevent();
  
}



void AMSEvent::_retkinitevent(){

  gethead()->add(
		 new AMSContainer(AMSID("AMSContainer:AMSTrRawCluster",0),0)
		 );

  gethead()->add(
		 new AMSContainer(AMSID("AMSContainer:AMSTrCluster",0),0)
		 );
  gethead()->add(
		 new AMSContainer(AMSID("AMSContainer:AMSTrRecHit",0),0)
		 );
  gethead()->add(
		 new AMSContainer(AMSID("AMSContainer:AMSTrTrack",0),0)
		 );
  
    
}



//=============================================================

void AMSEvent::_findC(AMSID & id){

  static char names[1024]="AMSContainer:";
  static char * name=names;
#ifdef __AMSDEBUG__
  int nc=0; 
  if(13+strlen(id.getname())+1  > 1024){
    name=new char[13+strlen(id.getname())+1];
    nc=1;
    name[0]='\0';
    strcat(name,"AMSContainer:");
  }
#endif
  name[13]='\0';
  if(id.getname())strcat(name,id.getname());
  id.setname(name); 
#ifdef __AMSDEBUG__
  if(nc)delete[] name;
#endif


}


integer AMSEvent::setbuilderC(char name[], pBuilder pb){
  AMSID id(name,0);
  _findC(id);
  AMSContainer *p = (AMSContainer*)AMSEvent::gethead()->getp(id);
  if(p){
    p->setbuilder(pb);
    return 1;
  }
  else return 0;

}

integer AMSEvent::buildC(char name[], integer par){
  AMSID id(name,0);
  _findC(id);
  AMSContainer *p = (AMSContainer*)AMSEvent::gethead()->getp(id);
  if(p){
    p->runbuilder(par);
    return p->buildOK();
  }
  else return 0; 

}

integer AMSEvent::rebuildC(char name[], integer par){
  AMSID id(name,0);
  _findC(id);
  for(int i=0;;i++){
    id.setid(i);
    AMSContainer *p = (AMSContainer*)AMSEvent::gethead()->getp(id);
    if(p){
      p->eraseC();
    }
    else break;
  }
  id.setid(0);
  AMSContainer *p = (AMSContainer*)AMSEvent::gethead()->getp(id);
  if(p){
    p->runbuilder(par);
    return p->buildOK();
  }
  else return 0; 


}


AMSlink * AMSEvent::_getheadC( AMSID id, integer sorted){
  _findC(id);
  AMSContainer *p = (AMSContainer*)AMSEvent::gethead()->getp(id);
  if(p){
    if(sorted)p->sort(sorted-1);
    // Reset global ref if any
    if(p->gethead())(p->gethead())->resethash(id.getid(),p->gethead());
    return p->gethead();
  }
  else return 0;
} 
AMSlink * AMSEvent::_getlastC( AMSID id){
  _findC(id);
  AMSContainer *p = (AMSContainer*)AMSEvent::gethead()->getp(id);
  if(p){
    return p->getlast();
  }
  else return 0;
}

integer AMSEvent::_setheadC( AMSID id, AMSlink *head){
  _findC(id);
  AMSContainer *p = (AMSContainer*)AMSEvent::gethead()->getp(id);
  if(p && head){
    p->sethead(head);
    return 1;
  }
  else return 0;
}

AMSContainer * AMSEvent::_getC( AMSID  id){
  _findC(id);  
  AMSContainer *p = dynamic_cast<AMSContainer*>(AMSEvent::gethead()->getp(id));
  return p;
}


integer AMSEvent::getnC(char n[]){
  AMSID id;
  _findC(id);
  AMSContainer *p;
  for(int i=0;;i++){
    id.setid(i);
    p = (AMSContainer*)AMSEvent::gethead()->getp(id);
    if(p==0) return i;
  }
}



AMSlink * AMSEvent::addnext(AMSID id, AMSlink *p){
  AMSContainer * ptr= AMSEvent::gethead()->getC(id);
  if(ptr){
    ptr->addnext(p);
    return p;
  }
  else {
    cerr<<"AMSEvent::addnext-S-NoContainer "<<id<<endl;
#ifdef __AMSDEBUG__
    exit(1);
#endif
    return 0;
  }
}



integer AMSEvent::replace(AMSID id, AMSlink *p, AMSlink *prev){
  AMSContainer * ptr= AMSEvent::gethead()->getC(id);
  if(ptr){
    ptr->replaceEl(p,prev);
    return 1;
  }
  else {
    cerr<<"AMSEvent::replace-S-NoContainer "<<id<<endl;
#ifdef __AMSDEBUG__
    exit(1);
#endif
    return 0;
  }
}


void AMSEvent::_signinitevent(){
  AMSNode *ptr = AMSEvent::gethead()->add (
					   new AMSContainer(AMSID("AMSContainer:AMSmceventg",0),0));
  AMSEvent::gethead()->add (
			    new AMSContainer(AMSID("AMSContainer:AMSmctrack",0),0));
}




void AMSEvent::_sitkinitevent(){
  AMSEvent::gethead()->add (
			    new AMSContainer(AMSID("AMSContainer:AMSTrMCCluster",0),0));
}



void AMSEvent::event(){


  //  addnext(AMSID("WriteAll",0),new Test());


  // First Selected Events
  
  
  _siamsevent();
  _reamsevent();
  //  _collectstatus();
  
  
  
}

//------------------------------------------------------------------
void AMSEvent::_siamsevent(){
  AMSgObj::BookTimer.start("SIAMSEVENT");
  
  _sitkevent(); 

  AMSgObj::BookTimer.stop("SIAMSEVENT");
}
  
void AMSEvent::_reamsevent(){
  AMSgObj::BookTimer.start("REAMSEVENT");  



  _retkevent(); 

  AMSgObj::BookTimer.stop("REAMSEVENT");  
}



//==============================================================================================
void AMSEvent:: _sitkevent(){


  //Simulate the noise clusters (skip for the moment FIXME PZ)
  //  if(TRMCFFKEY.NoiseOn && fastrigger )TrSim::sitknoise();

  // it is dummy ib ghbatch
  //AMSTrMCCluster::sitkcrosstalk();


  TrRecon::sitkdigi();



}


void AMSEvent::_retkevent(){
  int retr=TrRecon::gethead()->BuildTrClusters();
  if(retr>=0){
    int retr2=TrRecon::gethead()->BuildTrRecHits();
    if(retr2>=0)
      int retr3=TrRecon::gethead()->BuildTrTracks();
  }
}


void AMSEvent::write(){
  

  AMSJob::gethead()->ev->Clear();
  AMSContainer* con=0;
 
  Event* ev= AMSJob::gethead()->ev;
  con = AMSEvent::gethead()->getC(AMSID("AMSTrMCCluster"));
  if (con)  {  
    int nelem= con->getnelem();
    AMSTrMCCluster* pp= (AMSTrMCCluster*) con->gethead();
    while (pp){
      ev->AddTrMCCluster(pp);
      pp=(AMSTrMCCluster*)pp->next();
    }
  }
  con = AMSEvent::gethead()->getC(AMSID("AMSTrRawCluster"));
  if(con){
    int nelem= con->getnelem();
    AMSTrRawCluster* pp= (AMSTrRawCluster*) con->gethead();
    while (pp){
      ev->AddTrRawCluster(pp);
      pp=(AMSTrRawCluster*)pp->next();
    }
  }else
    printf("WARNING !!!! cannot find the TrRawCluster Container!!!!!\n");



  con = AMSEvent::gethead()->getC(AMSID("AMSTrCluster"));
  if(con){
    int nelem= con->getnelem();
    AMSTrCluster* pp= (AMSTrCluster*) con->gethead();
    while (pp){
      ev->AddTrCluster(pp);
      pp=(AMSTrCluster*)pp->next();
    }
  }else
    printf("WARNING !!!! cannot find the TrCluster Container!!!!!\n");
  
  con= AMSEvent::gethead()->getC(AMSID("AMSTrRecHit"));
  if(con){
    int nelem= con->getnelem();
    AMSTrRecHit* pp= (AMSTrRecHit*) con->gethead();
    while (pp){
      ev->AddTrRecHit(pp);
      pp=(AMSTrRecHit*)pp->next();
    }
    //con3->printC(cout);
    printf("Hits  #: %d\n",con->getnelem());
  }else
      printf("WARNING !!!! cannot find the TrRecHits Container!!!!!\n");

  con= AMSEvent::gethead()->getC(AMSID("AMSTrTrack"));
  if(con){
    int nelem= con->getnelem();
    AMSTrTrack* pp= (AMSTrTrack*) con->gethead();
    while (pp){
      ev->AddTrTrack(pp);
      pp=(AMSTrTrack*)pp->next();
    }
  }else
    printf("WARNING !!!! cannot find the TrTrack Container!!!!!\n");
  
  AMSJob::gethead()->t4->Fill();

}
