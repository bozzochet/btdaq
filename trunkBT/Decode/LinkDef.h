#ifdef __CINT__

#pragma link off all  globals;
#pragma link off all  classes;
#pragma link off all  functions;

#pragma link C++ class std::pair<int, std::pair<double, double> >+;

#pragma link C++ class Cluster+;
#pragma link C++ class RHClass+;
#pragma link C++ class Event+;  
#pragma link C++ class GenericEvent<1, 24, 64, 3, 16>+;
#pragma link C++ class GenericEvent<1, 24, 64, 5, 10>+;
#pragma link C++ class GenericEvent<1, 24, 64, 5, 10>+;

#endif 
