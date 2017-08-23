#include "TFile.h"
#include "TTree.h"
#include "TNtuple.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH2I.h"
#include <vector>
#include <string>
#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h> 
#include <string>
#include "TMath.h"
#include "TDirectory.h"
#include "Tools.h"
#include "Settings.h"
#include "Branches.h"

void setTrees(TFile * f, TTree ** trk, TTree ** pf, TTree ** gen, TTree ** jet){
  *trk = (TTree*) f->Get("ppTrack/trackTree");
  *pf  = (TTree*) f->Get("pfcandAnalyzer/pfTree");
  *gen = (TTree*) f->Get("HiGenParticleAna/hi");
  *jet = (TTree*) f->Get("ak4PFJetAnalyzer/t");
  (*trk)->AddFriend(*pf);
  (*trk)->AddFriend(*gen);
  (*trk)->AddFriend(*jet);
}

void ppToy(int job, int nJobs, std::vector< std::string > inputList){
  TH1::SetDefaultSumw2();
  Settings s = Settings();
  s.nEventsToMake = s.nEventsToMake/nJobs;

  Branches b = Branches();
  
  if(s.doTimeSeed) srand(time(NULL));
  else srand(job);

  TFile * outF = TFile::Open(Form("output_%d.root",job),"recreate"); 
  std::string varList = "nColl:recoHF:recoTrk:weight:job:evtID"; 
  TNtuple * ntuple = new TNtuple("ntuple","ntuple",varList.c_str());
  TH1F * MBPtPlot = new TH1F("MBRecoPtSpectrum","MBRecoPtSpectrum",s.ptBins,s.ptBinsArr);
  TH2F * HFvsPt = new TH2F("HFvsPt","HFvsPt",s.ptBins,s.ptBinsArr,s.nHFBins,0,s.nHFBins/10);
  TH2I * HFvsNColl = new TH2I("HFvsNColl","HFvsNColl",250,0,250,s.nHFBins,0,s.nHFBins/10);
  TH1F * HFvsnEvt = new TH1F("HFvsnEvt","HFvsnEvt",s.nHFBins,0,s.nHFBins/10);
  TH1F * evtSpectrum = new TH1F("evtSpectrum","evtSpectrum",s.ptBins,s.ptBinsArr);

  //calculate MB spectrum first by iteself
  float nEvt = 0;
  for(unsigned int file = 0; file < inputList.size(); file++){
    std::cout << inputList.at(file).c_str() << std::endl;
    TFile * fin = TFile::Open(inputList.at(file).c_str(),"READ");
    std::cout << "file: " << file << std::endl;
    TTree *trk, *pf, *gen, *jet;
    setTrees(fin, &trk, &pf, &gen, &jet);
    b.SetBranches(trk, pf, gen, jet);
   
    for(int i = 0; i<trk->GetEntries(); i++){
      trk->GetEntry(i);
      if(i%1000==0) std::cout << i << " " << trk->GetEntries() << std::endl;
      nEvt++;
      for(int t = 0; t<b.nTrk; t++)  MBPtPlot->Fill((b.trkPt)[t]);
    }
    fin->Close();
  }
  outF->cd();
  MBPtPlot->Scale(1./nEvt);
  MBPtPlot->Write();
  
  int nEvtsMade = 0;
  int nFile = 0;
  int evtIndex = 0;
  TFile * fin = TFile::Open(inputList.at(nFile).c_str(),"READ");
  TTree *trk, *pf, *gen, *jet;
  setTrees(fin, &trk, &pf, &gen, &jet);
  b.SetBranches(trk, pf, gen, jet);
  TDirectory * d = outF->mkdir("spectraPlots");
  d->cd();

  while(true){
    float totalTrk = 0, totalHF = 0;

    //nColl generation
    float nCollExact = findNcoll(rand()%200);
    int nColl = (int)nCollExact;
    float weight = -1;
    if((rand()%100) < (nCollExact-nColl)*100) nColl++;//randomly add 1 to nColl to match fractional average nColl values
    if(rand()%nColl != 0) continue;//generate events w/ frequency 1/nColl but weight them by nColl (should prefer generating peripheral evts) 
    else weight = nColl;  
      

    evtSpectrum->Reset();
    evtSpectrum->SetTitle(Form("evtSpectrum_%d_%d",job,nEvtsMade));
    evtSpectrum->SetName(Form("evtSpectrum_%d_%d",job,nEvtsMade));
    for(int i = 0; i<nColl; i++){
      trk->GetEntry(evtIndex);

      totalTrk += b.nTrk;
      float HF = getHFSum(b,3,5);
      totalHF += HF;
      for(int t = 0; t<b.nTrk; t++){
        evtSpectrum->Fill((b.trkPt)[t],weight);
      }
 
      int skip = rand()%(s.maxSkip)+1; 
      if((evtIndex + skip)< trk->GetEntries()){
        evtIndex += skip;
      } else {
        evtIndex = (evtIndex+skip-trk->GetEntries());
       
        if((unsigned int)(nFile+1) == inputList.size()) nFile = 0;
        else nFile++;
        fin->Close();
        fin = TFile::Open(inputList.at(nFile).c_str(),"READ");
        std::cout << "File: " << nFile << std::endl;
        setTrees(fin, &trk, &pf, &gen, &jet);
        b.SetBranches(trk, pf, gen, jet);
        d->cd(); 
      }
    }
    ntuple->Fill((float)nColl,(float)totalHF,(float)totalTrk,(float)weight,(float) job,(float)nEvtsMade);
    
    for(int i = 1; i<evtSpectrum->GetSize()-1; i++){
      HFvsPt->Fill(evtSpectrum->GetBinCenter(i),totalHF,weight*evtSpectrum->GetBinContent(i));    
    }
    HFvsNColl->Fill(nColl,totalHF,weight);
    HFvsnEvt->Fill(totalHF,weight);
 
    //evtSpectrum->Write();
 
    if(nEvtsMade%100 == 0) std::cout << nEvtsMade << "/" << s.nEventsToMake << std::endl;
    nEvtsMade++;
    if(nEvtsMade == s.nEventsToMake) break;
  }
  fin->Close();
  outF->cd();
  std::cout << "Outputting..." << std::endl;
  HFvsPt->Write();
  HFvsNColl->Write();
  HFvsnEvt->Write();
  //ntuple->Write();
  outF->Close();
}

/*
void ppToy2(int nEvts = 10, int maxSkip = 10, double pthatThresh = 30, double pthatEnhancement = 5, bool doShort = false, int shortSize = 100000){
  srand (time(NULL));
  //srand (5);

  const int ptBins = 25; 
  const float ptBinsArr[ptBins+1] = {0.5,0.7,0.9,1.1,1.3,1.5,1.7,2,2.5,3,4,5,6,7,8,10,12,15,20,25,30,35,40,50,60,80};
  
  int nCentBins = 10000000;
  TH1F * cent = new TH1F("cent","cent",nCentBins,0,100000);

  TH1::SetDefaultSumw2();
  TFile * fin = TFile::Open("/export/d00/scratch/abaty/merging/0.root","READ");
  //TFile * fin = TFile::Open("/mnt/hadoop/cms/store/user/abaty/Pythia_MB_pp5TeV/MinBias_TuneCUETP8M1_5p02TeV-pythia8/crab_20170405_184915/170405_164944/0000/HiForestAOD_1.root","READ");
  TTree * trk = (TTree*) fin->Get("ppTrack/trackTree");
  TTree * pf  = (TTree*) fin->Get("pfcandAnalyzer/pfTree");
  TTree * gen = (TTree*) fin->Get("HiGenParticleAna/hi");
  TTree * jet = (TTree*) fin->Get("ak4PFJetAnalyzer/t");
  trk->AddFriend(pf);
  trk->AddFriend(gen);
  trk->AddFriend(jet);

  int nTrk;
  float trkPt[20000];
  float trkEta[20000];
  
  float weight = 0;
  float weightSum = 0;

  float pthat = 0;

  std::vector< int > * pfId = 0;
  std::vector< float > * pfPt = 0;
  std::vector< float > * pfEta = 0;

  std::vector< float > * genChg = 0;
  std::vector< float > * genEta = 0;
  std::vector< float > * genPt = 0;

  trk->SetBranchAddress("nTrk",&nTrk);
  trk->SetBranchAddress("trkPt",&trkPt);
  //trk->SetBranchAddress("trkEta",&trkEta);
  pf->SetBranchAddress("pfId",&pfId);
  pf->SetBranchAddress("pfPt",&pfPt);
  pf->SetBranchAddress("pfEta",&pfEta);
  gen->SetBranchAddress("chg",&genChg);
  gen->SetBranchAddress("eta",&genEta);
  gen->SetBranchAddress("pt",&genPt);
  jet->SetBranchAddress("pthat",&pthat);

  trk->GetEntry(0);
  std::cout << nTrk << " " << trkPt[0] << " " << trkEta[0] << " " << pfId->at(0) << " " << pfPt->at(0) << " " << pfEta->at(0) << " " << genChg->at(0) << " " << genEta->at(0) << " " << genPt->at(0) << std::endl;


  TFile * outF = TFile::Open("ppToys.root","recreate"); 
  std::string varList = "nColl:genHF:recoHF:genTrk:recoTrk:weight:pthat"; 
  TNtuple * ntuple = new TNtuple("ntuple","ntuple",varList.c_str());
  TH1F * MBPtPlot = new TH1F("MBRecoPtSpectrum","MBRecoPtSpectrum",ptBins,ptBinsArr);
  double pthatFrac = 0;
  for(int i = 0; i<(doShort?shortSize:trk->GetEntries()); i++){
    trk->GetEntry(i);
    if(pthat > pthatThresh) pthatFrac += 1.0/(float)(doShort?shortSize:trk->GetEntries());
    for(int t = 0; t<nTrk; t++)  MBPtPlot->Fill(trkPt[t]);
  }
  MBPtPlot->Scale(1./(float)(doShort?shortSize:trk->GetEntries()));
  MBPtPlot->Write();
  
  TDirectory * d = outF->mkdir("spectraPlots");
  d->cd();

  //figure out how to generate the extra events with large pt had (should be weighted by weight/enhancement
  int startEvt = 0;
  int evtN = -1;
  bool onlyDoHighPthat = false;
  while(true){
  //for(int evtN = 0; evtN<nEvts; evtN++){
    evtN++;
    if(evtN >= nEvts){
      onlyDoHighPthat = true;
    }
    if(evtN >= nEvts+pthatFrac*nEvts*(pthatEnhancement-1)) break;

    if(evtN%1000==0) std::cout << evtN << "/" << nEvts << std::endl;
    float nCollExact = (findNcoll(rand()%200));
    int nColl = (int)nCollExact;
    if((rand()%100) < (nCollExact-nColl)*100) nColl++;//randomly add 1 to nColl to match fractional average nColl values

    if(rand()%nColl != 0){//generate events w/ frequency 1/nColl but weight them by nColl (should prefer generating peripheral evts)
      evtN--;
      continue;
    } 
    else{
      weight = nColl;  
    }
    //int nColl = rand()%maxNColl+1;
  
    int skipSize = rand()%maxSkip+1;
    int nCollCounter = 0;
    float pthatFirst = -1;
    float genTrkSum = 0, recoTrkSum = 0, genHFP = 0, recoHFP = 0, genHFM = 0, recoHFM = 0;
    TH1F * genPtPlot = new TH1F(Form("GenPtSpectrum%d",evtN),Form("GenPtSpectrum%d",evtN),ptBins,ptBinsArr);
    TH1F * recoPtPlot = new TH1F(Form("RecoPtSpectrum%d",evtN),Form("RecoPtSpectrum%d",evtN),ptBins,ptBinsArr);

    bool hasFoundFirstCol = false;
    for(int i = startEvt; i<(doShort?shortSize:trk->GetEntries()); i+=skipSize){
      trk->GetEntry(i);
      if(i==startEvt && onlyDoHighPthat && pthat<=pthatThresh){
        if(i>=(doShort?shortSize:trk->GetEntries())-1){
          startEvt = 0;
          i = -skipSize;  //keep using same file
        }
        else{
          startEvt++;
          i = i+1-skipSize;
        }
        continue;
      }

      if(!hasFoundFirstCol && (pthat > pthatThresh)){
        weight = weight/pthatEnhancement;
      }

      if(!hasFoundFirstCol) pthatFirst = pthat;
      
      hasFoundFirstCol = true;
      nCollCounter++;
    
    //  std::cout << i << "/" << trk->GetEntries() << " " << nCollCounter << std::endl; 
      recoTrkSum += nTrk;
      for(int t = 0; t<nTrk; t++){
        recoPtPlot->Fill(trkPt[t],weight);
      }
      for(int pf = 0; pf<pfId->size(); pf++){
        if(pfId->at(pf)!=6 && pfId->at(pf)!=7) continue;
        if(TMath::Abs(pfEta->at(pf))<3 || TMath::Abs(pfEta->at(pf))>5) continue;
        if(pfEta->at(pf)>0) recoHFP += pfPt->at(pf);
        else                recoHFM += pfPt->at(pf);
      }
      for(int g = 0; g<genChg->size(); g++){
        if(TMath::Abs(genEta->at(g))<2.4 && genChg->at(g)!=0){
          genTrkSum++;
          genPtPlot->Fill(genPt->at(g),weight);
        }
        if(TMath::Abs(genEta->at(g))<3 || TMath::Abs(genEta->at(g))>5) continue;
        if(genEta->at(g)>0) genHFP += genPt->at(g);
        else                genHFM += genPt->at(g);
      }

      if(i>=(doShort?shortSize:trk->GetEntries())-skipSize){
        startEvt = skipSize-((doShort?shortSize:trk->GetEntries())-i);
        i=-1;  //keep using same file
        if(nCollCounter == nColl) break;
      }
      else{
        startEvt+=skipSize;
        if(nCollCounter == nColl) break;
      }
    }

    weightSum += weight;
    genPtPlot->Write();
    recoPtPlot->Write();
    cent->Fill(recoHFP+recoHFM,weight);
    ntuple->Fill(nColl,genHFP+genHFM, recoHFP+recoHFM, genTrkSum, recoTrkSum,weight,pthatFirst);
    delete genPtPlot, recoPtPlot;
  }
  outF->cd();
  ntuple->Write();

  //centrality calculation
  std::cout << "Doing reco centrality calculation..." << std::endl;
  TH1F * centBoundaries = new TH1F("centBoundaries","centBoundaries",100,0,100);
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
  //std::cout << 0 << "% " << 999999 << std::endl;

  std::cout << "writing centrality ntuples" << std::endl;
  TNtuple * centralities = new TNtuple("centralities","centralities","cent");
  float HFSum = 0;
  for(int i = 0; i<ntuple->GetEntries(); i++){
    ntuple->SetBranchAddress("recoHF",&HFSum);
    ntuple->GetEntry(i);
    for(int c = 100; c>-1; c--){
      if(HFSum>=centV.at(100-c) && HFSum<centV.at(101-c))
      centralities->Fill(c-1);
    } 
  }
  centralities->Write();
  outF->Close();
}

*/
