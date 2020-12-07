#pragma once

#include <IScanInterface.hpp>
#include <string>

#define MAXLINELENTH 200
using namespace std;

class PtxReader
{
public:
  PtxReader(const char* pFilename);
  ~PtxReader();

  bool MoveNextScan();
  bool GetSize(int& columns, int& rows);
  bool GetHeader(double scannerMatrix3x4[12], double ucs[16]);
  std::string GetScanName();
  __int64 GetPointCount();
  int GetNumScan();
  int ReadPoints(int subample, ScanPointCallback cb);
  bool LoadScan(int sub, vector< shared_ptr<ScanNode>>& rNodes);
private:
  bool GetHeader(vector<string>& rHeader);

  struct Impl;
  Impl* mpImpl;
};