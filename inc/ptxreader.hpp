#include <vector>
#include <string>
#include <stdio.h>
#include <inc/ptxwriter.hpp>
#include <inc/IScanInterface.hpp>

#define MAXLINELENTH 200
using namespace std;

class PtxReader
{
public:
  PtxReader(const char* pFilename);
  ~PtxReader();

  bool ReadSize(int& columns, int& rows);
  bool ReadHeader(double scannerMatrix3x4[12], double ucs[16]);
  bool HasMoredata();
  std::string GetScanName();
  bool ProcessConvert(int subample, PtxWriter& ptxwriter);
  __int64 GetPointCount() { return mPointCount; }
  int GetNumScan() { return mNumScan; }
  int ReadPoints(vector<float>& x, vector<float>& y, vector<float>& z,
                 vector<float>& rIntensity, vector<int>& rgbColor);
  int ReadPoints(ScanPointCallback cb);
private:
  bool ReadLine(std::string& rLine);
  bool ReadHeader(vector<string>& rHeader);
  char mBuffer[MAXLINELENTH];
  FILE* mFile;
  string mFilename;
  int mSubsample;
  __int64 mPointCount;
  int mNumScan;
  int mRows;
};