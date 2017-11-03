#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "include/Settings.h"
#include "glauber/runglauber_v3.0.C"

void ppToy(int job, int totalJobs, std::vector< std::string > fileList){

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
