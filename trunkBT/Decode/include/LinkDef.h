#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class std::pair < int, std::pair < double, double>> + ;
#pragma link C++ class std::vector < float> + ;
#pragma link C++ class std::vector < std::vector < float>> + ;
#pragma link C++ class std::vector < std::vector < std::vector < float>>> + ;

#pragma link C++ class LadderParams + ;
#pragma link C++ class std::map < int, LadderParams *> + ;
#pragma link C++ class LadderParamsMap + ;

#pragma link C++ class GenericRHClass + ;
#pragma link C++ class GenericEvent + ;
#pragma link C++ class Cluster + ;
#pragma link C++ class RHClass < 1, 24> + ;
#pragma link C++ class RHClass < 8, 9> + ;
#pragma link C++ class RHClassOCA + ;
#pragma link C++ class RHClassHEF + ;
#pragma link C++ class DataVersion + ;
// AMS
#pragma link C++ class Event < 1, 24, 64, 3, 10, 6> + ;
#pragma link C++ class calib < 1024> + ;
#pragma link C++ class Calibrations < 1, 24, 1024> + ;
// #pragma link C++ class calibelem < 1024> + ;
//  AMS L0
#pragma link C++ class Event < 8, 9, 64, 8, 16, 0> + ;
#pragma link C++ class calib < 1024> + ;
#pragma link C++ class Calibrations < 8, 9, 1024> + ;
// #pragma link C++ class calibelem < 1024> + ;
//  OCA
#pragma link C++ class Event < 1, 24, 64, 5, 10, 0> + ;
#pragma link C++ class calib < 640> + ;
#pragma link C++ class Calibrations < 1, 24, 640> + ;
// #pragma link C++ class calibelem < 640> + ;
//  HEF
#pragma link C++ class Event < 1, 24, 64, 7, 14, 0> + ;
#pragma link C++ class calib < 896> + ;
#pragma link C++ class Calibrations < 1, 24, 896> + ;
// #pragma link C++ class calibelem < 896> + ;
//  FOOT
#pragma link C++ class Event < 1, 24, 64, 5, 10, 0> + ;
#pragma link C++ class calib < 640> + ;
#pragma link C++ class Calibrations < 1, 24, 640> + ;
// #pragma link C++ class calibelem < 640> + ;

#endif
