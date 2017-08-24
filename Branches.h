#ifndef BRANCHES
#define BRANCHES

#include <vector>
#include <iostream>
#include "Settings.h"

class Branches {
  public:

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

  void  SetBranches(Settings s, TTree * trk, TTree * pf, TTree * gen, TTree * jet);
  Branches();
  
  private:
};

void Branches::SetBranches(Settings s, TTree * trk, TTree * pf, TTree * gen, TTree * jet){
  trk->SetBranchAddress("nTrk",&nTrk);
  trk->SetBranchAddress("trkPt",&trkPt);
  //trk->SetBranchAddress("trkEta",&trkEta);
  pf->SetBranchAddress("pfId",&pfId);
  pf->SetBranchAddress("pfPt",&pfPt);
  pf->SetBranchAddress("pfEta",&pfEta);
  if(s.isMC){
    gen->SetBranchAddress("chg",&genChg);
    gen->SetBranchAddress("eta",&genEta);
    gen->SetBranchAddress("pt",&genPt);
  }
  jet->SetBranchAddress("pthat",&pthat);
}

Branches::Branches(){
}

#endif
