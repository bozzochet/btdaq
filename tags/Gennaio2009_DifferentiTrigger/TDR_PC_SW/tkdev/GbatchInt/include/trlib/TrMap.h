#include <vector>
#include <map>
typedef unsigned int uint;

//! Template used to implement a map based ordering for Cluster classes
template <class T > class TrMap{

public: 
  typedef typename map< int, vector<T*> >::const_iterator TrMapIT;
  
  map< int, vector<T*> > LadMap;
  
  TrMap() {LadMap.clear();}
  ~TrMap(){LadMap.clear();}
  int Size(){return LadMap.size();}
  void Add(T* pp);
  int GetNelem(int tkid);
  T* GetElem(int tkid,uint pos);
  void info();
};



template <class T> int TrMap<T>::GetNelem(int tkid){
    TrMapIT iter=LadMap.find(tkid);
    if(iter==LadMap.end()) return -1;
    return (*iter).second.size();
}



template <class T> T*  TrMap<T>::GetElem(int tkid,uint pos){
    TrMapIT iter=LadMap.find(tkid);
    if(iter==LadMap.end()) return 0;
    if(pos>=(*iter).second.size()) return 0;
    else return (*iter).second.at(pos);
}

template <class T> void  TrMap<T>::Add(T* pp){
  int ii=pp->GetTkId();
  LadMap[ii].push_back(pp);
}


template <class T> void  TrMap<T>::info(){
  TrMapIT iter=LadMap.begin();
  while (iter!=LadMap.end()){
    printf("LADMAP tkid %d \n",(*iter).first);
    iter++;
  }
}
