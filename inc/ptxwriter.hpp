#pragma once
#include <string>
#include <stdio.h>

using namespace std;

class PtxWriter
{
public:
  PtxWriter();
  ~PtxWriter();
  void Init(int posPrecision, int intensityPrecision);
  void WriteSize(int col, int rows);
  bool Open(const char* pFilename);
  void WriteHeader(double scannerPos[12],  double ucs[16]);
  int WritePoints(int numPoint, float* pos,
                  float* rIntensity, int* pRgbColor);
  bool WritePoint(float x, float y, float z, float i, int r, int g, int b);
  void NextScan() { mFormat = -1; }
private:
  bool WriteLine(const char* pLine);
  void InitExportFormat();
  FILE* mFile;
  string mFilename;
  int mPosPrecision, mIntensityPrecision;
  int mFormat;
  string mCoordFormat;
  string mIntensityFormat;
};