#pragma once
#include <vector>
#include <string>
#include <stdio.h>

using namespace std;
#define BUFFERLENGTH 200

class PtxWriter
{
public:
  PtxWriter();
  ~PtxWriter();
  void Init(int posPrecision, int intensityPrecision, int subsample);
  void WriteSize(int col, int rows);
  bool Open(const char* pFilename);
  void WriteHeader(double scannerPos[12],  double ucs[16]);
  int WritePoints(int numPoint, float* x, float* y, float* z,
                  float* rIntensity, int* pRgbColor);
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