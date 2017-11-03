#ifndef INPUTFILEH
#define INPUTFILEH

#include <string>
#include <vector>
#include "TTree.h"
#include "TFile.h"

class InputFile {
  public:

  InputFile(std::string file);
  ~InputFile();
  void GetEntry(int i);
  int GetEntries();
  
  int n;
  int process;
  float pthat;
  int pID[50000]; 
  char status[50000];
  float pt[50000];
  float eta[50000];
  float phi[50000];
  int charge[50000];

  private:

  TFile * f;
  TTree * t;

};

InputFile::InputFile(std::string file){
  f = TFile::Open(file.c_str(),"read");
  t = (TTree*)f->Get("t");
  
  t->SetBranchAddress("n",&n);
  t->SetBranchAddress("process",&process);
  t->SetBranchAddress("pthat",&pthat);
  t->SetBranchAddress("pID",pID);
  t->SetBranchAddress("status",status);
  t->SetBranchAddress("pt",pt);
  t->SetBranchAddress("eta",eta);
  t->SetBranchAddress("phi",phi);
  t->SetBranchAddress("charge",charge);
}

void InputFile::GetEntry(int i){
  t->GetEntry(i); 
}

int InputFile::GetEntries(){
  return t->GetEntries(); 
}

InputFile::~InputFile(){
  f->Close();
}
#endif
