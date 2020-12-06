#include <vector>
#include <string>
#include <stdio.h>
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
  __int64 GetPointCount();
  int GetNumScan();
  int ReadPoints(int subample, ScanPointCallback cb);
  size_t Load(int subample, float*& x, float*& y,
              float*& z, float*& intensity, int*& color);
private:
  bool ReadHeader(vector<string>& rHeader);

  struct Impl;
  Impl* mpImpl;
};