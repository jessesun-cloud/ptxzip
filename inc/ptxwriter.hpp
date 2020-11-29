#pragma once
#include <vector>
#include <string>
#include <stdio.h>

using namespace std;
#define BUFFERLENGTH 200

class PtxWriter
{
public:
  PtxWriter(const char* pFilename);
  ~PtxWriter();
  void Init(int posPrecision, int intensityPrecision, int subsample);
  void WriteSize(int col, int rows);
  bool WriteLine(const char* pLine);
  bool IsOpen();
  bool WriteHeader(vector<string>& rHeaders);
  bool ProcessLine(const string& rInput);
  bool AnalysisFormat(const string& rLine);
private:
  char mFormatBuffer[BUFFERLENGTH];
  FILE* mFile;
  string mFilename;
  int mPosPrecision, mIntensityPrecision, mSubsample;
  int mFormat;
};