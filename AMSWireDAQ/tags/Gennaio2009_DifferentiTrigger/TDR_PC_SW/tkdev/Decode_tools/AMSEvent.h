#ifndef _AMSEVENT_
#define _AMSEVENT_
#include "Event.h"
#include "node.h"
#include "cont.h"

//! Temporary class for compatibility 
class AMSEvent :public AMSNode{

  uinteger _run;
  uinteger _runtype;
  time_t _time;
  uinteger _usec;
  uinteger _Error;
  uinteger _status[2];
  Event* ev;
public:
  
  static AMSEvent* _Head;
  static AMSEvent* gethead()  {return _Head;}
  static AMSNodeMap EventMap;

  //!tiggers the reconstruction
  void event();
  //!writes out the event (dummy)
  void write();
  
  AMSEvent(AMSID type):AMSNode(type){
    _Head=this;
   _status[0]=0;
    _status[1]=0;
  }
 AMSEvent(AMSID id, integer run, integer runtype, time_t time, uinteger usec):
   AMSNode(id),_run(run),
   _runtype(runtype), _time(time), _usec(usec),_Error(0){
    _Head=this;
    _status[0]=0;
    _status[1]=0;
  }
  static void  sethead(AMSEvent* head) 
  { _Head=head;if(_Head)AMSEvent::EventMap.map(*_Head);}
  time_t gettime(){return _time;}
private:
  void _init();

  void _siamsinitrun(){_signinitevent();_sitkinitrun() ;}
  void _reamsinitrun(){_retkinitrun() ;}
  void _siamsinitevent(){_sitkinitevent() ;}
  void _reamsinitevent(){_retkinitevent() ;}


  void _signinitevent();

  void _sitkinitrun(){}
  void _retkinitrun(){}
  void _sitkinitevent();
  void _retkinitevent();

  
  void _siamsevent();
  void _reamsevent();

  void _retkevent();
  void _sitkevent();




  //! despite the name it only prepends the string "AMSContainer:" to id._name
  void            _findC    (AMSID & id);

  //! if id._name is "xxx", it search for the container named "AMSContainer:xxx", 
  //! then it sort it (if required),
  //! it calls resethash 
  //! returns a pointer to the first object within the conatiner
  AMSlink      *  _getheadC ( AMSID id, integer sorted);

  //! if id._name is "xxx", it search for the container named "AMSContainer:xxx",
  //!returns a pointer to the last  object within the conatiner
  AMSlink      *  _getlastC ( AMSID id);
  
  //! if id._name is "xxx", it search for the container named "AMSContainer:xxx", 
  //! and returns a pointer to it 
  AMSContainer *  _getC     ( AMSID  id);

  //! if id._name is "xxx", it search for the container named "AMSContainer:xxx", 
  //! and set the object pointed by head as the first object within the container
  integer         _setheadC ( AMSID id, AMSlink *head);

public:
  //! Stupid reimplementation of a private function
  AMSContainer * getC( AMSID id){return _getC(id);}

  //! Stupid reimplementation of a private function
  AMSlink * getheadC(char name[], integer id, integer sorted=0)
  {return _getheadC(AMSID(name,id),sorted);}

  //! Stupid reimplementation of a private function
  AMSlink * getlastC(char name[], integer id)
  {return _getlastC(AMSID(name,id));}

  //! Stupid reimplementation of a private function
  integer setheadC(char name[], integer id, AMSlink *p)
  {return _setheadC(AMSID(name,id),p);}


  //! Use the element pointed by p  to replace the element after prev 
  //! in the contained identified by id (beware of "AMSContainer:")
  integer replace(AMSID id, AMSlink *p, AMSlink *prev);

  //Add the  element pointed by p  at the end of the container identified by id (beware of "AMSContainer:")
  AMSlink * addnext(AMSID id, AMSlink *p);

  integer getnC(char n[]);

  //! it sets the builder for the conatainer identified by name (beware of "AMSContainer:")
  integer setbuilderC(char name[], pBuilder pb);
  integer buildC(char name[], integer par);
  integer rebuildC(char name[], integer par);
  
  inline AMSNode * getp(const AMSID & id, char hint=0) const{
    return AMSEvent::EventMap.getp(id,hint);
  }

};

#endif
