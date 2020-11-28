#include <vector>
#include <string>
#include <stdio.h>
#include <inc/ptxwriter.hpp>

#define MAXLINELENTH 200
using namespace std;

class PtxReader
{
public:
  PtxReader(const char* pFilename);
  ~PtxReader();
  void Init(int subsample);
  bool ReadSize(int& column, int& width);
  void ReadHeader(vector<string>& rHeader);
  bool ReadLine(std::string& rLine);
  bool HasMoredata();
  static void RemoveEndCLn(std::string& str);
  bool ProcessConvert(PtxWriter& ptxwriter);
private:
  char mBuffer[MAXLINELENTH];
  char mFormatBuffer[MAXLINELENTH];
  FILE* mFile;
  string mFilename;
  int mSubsample;

};