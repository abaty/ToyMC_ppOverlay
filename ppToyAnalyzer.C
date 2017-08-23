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
  while(centV.size()<101) centV.push_back(999999);

  for(unsigned int i = 0; i<centV.size(); i++) std::cout << centV.at(i) << std::endl;

  TH1F * spectraNum[nCentBins];
  TH1F * spectraDen = (TH1F*)f->Get("MBRecoPtSpectrum");
  TH1F * RAA[nCentBins];
  for(int i = 0; i<nCentBins; i++){
    spectraNum[i] = (TH1F*)spectraDen->Clone(Form("spectraNum_%d_%d",s.centBins[i],s.centBins[i+1]));
    spectraNum[i]->Reset(); 
 
    for(int x = 0; x<HFvsPt->GetXaxis()->GetNbins(); x++){
      for(int y = 0; y<HFvsPt->GetYaxis()->GetNbins(); y++){
        if(HFvsPt->GetYaxis()->GetBinCenter(y) < centV.at(s.centBins[i])) continue;
        if(HFvsPt->GetYaxis()->GetBinCenter(y) >= centV.at(s.centBins[i+1])) continue;
        spectraNum[i]->SetBinContent(x,spectraNum[i]->GetBinContent(x)+HFvsPt->GetBinContent(x,y));
        spectraNum[i]->SetBinError(x,TMath::Power(TMath::Power(spectraNum[i]->GetBinError(x),2)+TMath::Power(HFvsPt->GetBinContent(x,y),2),0.5);
      }
    } 
    
  }
  TH1F * avgNColl = new TH1F("avgNColl","avgNColl",100,0,100);
}
