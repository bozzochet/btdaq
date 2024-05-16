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

#pragma link C++ class Cluster + ;
#pragma link C++ class RHClass < 1, 24> + ;
#pragma link C++ class RHClass < 2, 9> + ;
#pragma link C++ class DataVersion + ;
// AMS
#pragma link C++ class GenericEvent < 1, 24, 64, 3, 10, 6> + ;
#pragma link C++ class calib < 1024> + ;
#pragma link C++ class Calibrations < 1, 24, 1024> + ;
// #pragma link C++ class calibelem < 1024> + ;
//  AMS L0
#pragma link C++ class GenericEvent < 2, 9, 64, 8, 16, 0> + ;
#pragma link C++ class calib < 1024> + ;
#pragma link C++ class Calibrations < 2, 9, 1024> + ;
// #pragma link C++ class calibelem < 1024> + ;
//  OCA
#pragma link C++ class GenericEvent < 1, 24, 64, 5, 10, 0> + ;
#pragma link C++ class calib < 640> + ;
#pragma link C++ class Calibrations < 1, 24, 640> + ;
// #pragma link C++ class calibelem < 640> + ;
//  FOOT
#pragma link C++ class GenericEvent < 1, 24, 64, 5, 10, 0> + ;
#pragma link C++ class calib < 640> + ;
#pragma link C++ class Calibrations < 1, 24, 640> + ;
// #pragma link C++ class calibelem < 640> + ;

#endif
