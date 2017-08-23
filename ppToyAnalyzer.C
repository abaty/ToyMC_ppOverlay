#include "TFile.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2I.h"
#include "TH2F.h"
#include "Settings.h"
#include <vector>
#include <iostream>

void ppToyAnalyzer(){
  Settings s = Settings();

  TFile * f = TFile::Open("Aug22_10M_sample.root","read");
  TH2F * HFvsPt = (TH2F*)f->Get("HFvsPt");
  TH2I * HFvsNColl = (TH2I*)f->Get("HFvsNColl");
  TH1F * HFvsnEvt = (TH1F*)f->Get("HFvsnEvt");

  //centrality calculation
  TH1D *centBoundaries = new TH1D("centBoundaries","centBoundaries",100,0,100);
  int n = 1;
  std::vector< float > centV(1,0);
  std::cout << 100 << "% " << 0 << std::endl;
  int sum = 0;
  float netSum = HFvsnEvt->Integral(0,s.nHFBins+1);
  

  for(int i = 1; i<HFvsnEvt->GetSize(); i++){
    sum += HFvsnEvt->GetBinContent(i);
    if(sum/netSum>0.01*n){
      std::cout << 100-n << "% " << HFvsnEvt->GetBinCenter(i) << std::endl;
      centBoundaries->Fill(100-n,HFvsnEvt->GetBinCenter(i));
      centV.push_back(HFvsnEvt->GetBinCenter(i));
      n++;
    }
    if(n==100) break;
  }
  centBoundaries->SetBinContent(1,99999);
  centV.push_back(999999);


}
