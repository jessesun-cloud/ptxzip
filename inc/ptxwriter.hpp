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
  bool IsOpen();
  void WriteHeader(double scannerPos[12],  double ucs[16]);
  int WritePoints(vector<float>& x, vector<float>& y, vector<float>& z,
                  vector<float>& rIntensity, vector<int>& rgbColor);
  bool WritePoint(float x, float y, float z, float i, int r, int g, int b);
  void NextScan() { mFormat = -1; }
private:
  bool WriteLine(const char* pLine);
  bool InitExportFormat();
  char mFormatBuffer[BUFFERLENGTH];
  char mFormatBufferNoIntensity[BUFFERLENGTH];
  FILE* mFile;
  string mFilename;
  int mPosPrecision, mIntensityPrecision, mSubsample;
  int mFormat;
  string mZeroString;
};