#ifndef SETTINGS
#define SETTINGS

#include <iostream>
#include "TH1F.h"

class Settings {
  public:

  //generator
  static const int ptBins = 25; 
  const double ptBinsArr[ptBins+1] = {0.5,0.7,0.9,1.1,1.3,1.5,1.7,2,2.5,3,4,5,6,7,8,10,12,15,20,25,30,35,40,50,60,80};
  
  static const int nHFBins = 29000;

  int nEventsToMake = 10000000;//
  int maxSkip = 2;

  bool doTimeSeed = false;

  //analyzer
  static const int nCentBins = 14;
  const int centBinsLow[nCentBins] = {0,50,60,70,80,90,95,70,50,30,10,5,0,0};
  const int centBinsHigh[nCentBins] = {50,60,70,80,90,95,100,90,70,50,30,10,5,100};
  bool isMC = true;

  Settings();
  
  private:
};

Settings::Settings(){
  std::cout << "Gettings settings..." << std::endl;
  return;
}

#endif
