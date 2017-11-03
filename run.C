#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "TFile.h"
#include "TTree.h"
#include "include/Settings.h"
#include "include/inputFile.h"
#include "glauber/runglauber_v3.0.C"
#include <iostream>


void ppToy(int job, int totalJobs, std::vector< std::string > fileList){

  Settings s;

  std::vector< int > pt;
  for(int i = 0; i<s.ptBins; i++) pt.push_back(i);

  TFile * outFile = TFile::Open(Form("output_%d",job),"recreate");
  TTree * o = new TTree("t","t");  
  o->Branch("pt",&pt);

  InputFile input = InputFile(fileList.at(0));
  for(int i = 0; i<input.GetEntries(); i++){
    input.GetEntry(i);
    for(int j = 0; j<input.n; j++){
       
    }
  }
 
  
  outFile->Write();
  outFile->Close();
}

int main(int argc, const char* argv[])
{
  if(argc != 5)
  {
    std::cout << "Usage: <job> <totalJobs> <fileList> <dummyVar>" << std::endl;
    return 1;
  }  

  int job = std::atoi(argv[1]);
  int totalJobs = std::atoi(argv[2]);
  std::string fList = argv[3];
  int dummy = std::atoi(argv[4]);
  std::string buffer;
  std::vector<std::string> listOfFiles;
  std::ifstream inFile(fList.data());

  if(!inFile.is_open())
  {
    std::cout << "Error opening jet file. Exiting." <<std::endl;
    return 1;
  }
  else
  {
    int line = 0;
    while(true)
    {
      inFile >> buffer;
      if(inFile.eof()) break;
      listOfFiles.push_back(buffer);
      line++;
    }
  }

  dummy = dummy+1;

  ppToy(job,totalJobs,listOfFiles);
  
  return 0; 
}
