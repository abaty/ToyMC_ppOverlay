#include "TFile.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2I.h"
#include "TH2F.h"
#include "Settings.h"
#include "Tools.h"
#include <vector>
#include <iostream>

void ppToyAnalyzer(){
  TH1::SetDefaultSumw2();
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
  std::reverse(centV.begin(),centV.end());
  for(unsigned int i = 0; i<centV.size(); i++) std::cout << i << " " << centV.at(i) << std::endl;

  TFile * out = TFile::Open("out.root","recreate");
  TH1F * spectraNum[s.nCentBins];
  TH1F * spectraDen = (TH1F*)f->Get("MBRecoPtSpectrum");
  TH1F * averageNColl = new TH1F("averageNColl","averageNColl",s.nCentBins,0,s.nCentBins);
  TH1F * averageNColl_fine = new TH1F("averageNColl_fine","averageNColl_fine",100,0,100);
  TH1F * averageNColl_glauber = new TH1F("averageNColl_glauber","averageNColl_glauber",100,0,100);
  TH1F * nMBEvt = (TH1F*)f->Get("nMBEvt");
  spectraDen->Scale(1/(float)nMBEvt->GetBinContent(1));
  spectraDen->SetDirectory(out);
  spectraDen->Write();
  
  TH1F * RAA[s.nCentBins];
  for(int i = 0; i<s.nCentBins; i++){
    spectraNum[i] = (TH1F*)spectraDen->Clone(Form("spectraNum_%d_%d",s.centBinsLow[i],s.centBinsHigh[i]));
    spectraNum[i]->Reset();
    int n_MB = 0; 
    float nColl = 0;
    for(int y = 1; y<HFvsPt->GetYaxis()->GetNbins()+1; y++){
      if(HFvsPt->GetYaxis()->GetBinCenter(y) >= centV.at(s.centBinsLow[i])) continue;
      if(HFvsPt->GetYaxis()->GetBinCenter(y) < centV.at(s.centBinsHigh[i])) continue;
      n_MB += HFvsnEvt->GetBinContent(y);
      for(int x = 1; x<HFvsPt->GetXaxis()->GetNbins()+1; x++){
        spectraNum[i]->SetBinContent(x,spectraNum[i]->GetBinContent(x)+HFvsPt->GetBinContent(x,y));
        spectraNum[i]->SetBinError(x,TMath::Power(TMath::Power(spectraNum[i]->GetBinError(x),2)+TMath::Power(HFvsPt->GetBinContent(x,y),2),0.5));
      }
      //calculate average nColl
      for(int x = 1; x<HFvsNColl->GetXaxis()->GetNbins()+1; x++){
        nColl += HFvsNColl->GetBinContent(x,y)*x;
      }
    }
    spectraNum[i]->Scale(1/(float)n_MB);//divide by 1/MB
    float avgNColl = nColl/(float)n_MB;
    averageNColl->Fill(i,avgNColl);
    std::cout << "N_MB: " << n_MB << " avgNColl " << avgNColl << " Glauber NColl: " << getAvgNCollGlauber(s.centBinsLow[i]*2,s.centBinsHigh[i]*2) << std::endl;
    spectraNum[i]->Write();

    RAA[i] = (TH1F*)spectraNum[i]->Clone(Form("RAA_%d_%d",s.centBinsLow[i],s.centBinsHigh[i]));
    RAA[i]->Divide(spectraDen);
    RAA[i]->Scale(1/avgNColl); 
    RAA[i]->Write();

    RAA[i]->GetYaxis()->SetRangeUser(0,2);
    RAA[i]->GetXaxis()->SetRangeUser(0.5,30);
    RAA[i]->GetXaxis()->SetTitle("P_{T}");
    RAA[i]->GetYaxis()->SetTitle(Form("R_{AA}^{pp Overlay} (%d-%d%%)",s.centBinsLow[i],s.centBinsHigh[i]));

    TCanvas * c1 = new TCanvas("c","c",800,600);
    RAA[i]->Draw();
    c1->SaveAs(Form("img/RAA%d_%d_%d.png",s.centBinsLow[i],s.centBinsHigh[i],(int)s.isMC));
    c1->SaveAs(Form("img/RAA%d_%d_%d.pdf",s.centBinsLow[i],s.centBinsHigh[i],(int)s.isMC));
    delete c1;
  }
  averageNColl->Write();
  
  for(int i = 0; i<100; i++){
    int n_MB = 0; 
    float nColl = 0;
    for(int y = 1; y<HFvsPt->GetYaxis()->GetNbins()+1; y++){
      if(HFvsPt->GetYaxis()->GetBinCenter(y) >= centV.at(i)) continue;
      if(HFvsPt->GetYaxis()->GetBinCenter(y) < centV.at(i+1)) continue;
      n_MB += HFvsnEvt->GetBinContent(y);
      for(int x = 1; x<HFvsNColl->GetXaxis()->GetNbins()+1; x++){
        nColl += HFvsNColl->GetBinContent(x,y)*x;
      }
    }    
    averageNColl_fine->Fill(i,nColl/(float)n_MB);
    averageNColl_glauber->Fill(i,(findNcoll(2*i)+findNcoll(2*i+1))/2.0);
  }
  averageNColl_fine->Write();
  averageNColl_glauber->Write();
  out->Close();
}
