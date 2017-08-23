#include "TFile.h"
#include "TH1F.h"
#include "TH2I.h"
#include "TH2F.h"
#include <vector>
#include <iostream>

void ppToyAnalyzer(){
  TH1D *centBoundaries = new TH1F("centBoundaries","centBoundaries",100,0,100);
  int n = 1;
  std::vector< float > centV(1,0);
  std::cout << 100 << "% " << 0 << std::endl;
  int sum = 0;
  float netSum = cent->Integral(0,100000);
  for(int i = 1; i<nCentBins; i++){
    sum += cent->GetBinContent(i);
    if(sum/netSum>0.01*n){
      std::cout << 100-n << "% " << cent->GetBinCenter(i) << std::endl;
      centBoundaries->Fill(100-n,cent->GetBinCenter(i));
      centV.push_back(cent->GetBinCenter(i));
      n++;
    }
    if(n==100) break;
  }
  centBoundaries->SetBinContent(1,99999);
  centBoundaries->Write();
  centV.push_back(999999);
}
