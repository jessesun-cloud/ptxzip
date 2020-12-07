#pragma once

#include <functional>
//#include <E57Simple.h>
#include <Eigen/Dense>
#include <vector>
#include <map>

using namespace std;
using namespace Eigen;

typedef std::function<void(int numPoint,
                           float* pos, float* rIntensity,
                           int* rgbColor)> ScanPointCallback;

class RenderableScan
{
public:
  RenderableScan();
  vector<int> mVBOId;
  int mLevels;
  int mCurrentLevel;
  Eigen::Matrix4d mTransformation;
};

class ScanNode
{
  std::vector <float> mXyz;
  std::vector <float> mIntensity;
  std::vector <int> mColor;
  Eigen::Vector3f mMinCoord, mMaxCoord;
  RenderableScan mData;
  Eigen::Matrix4d mTransformation;
  std::string mScanName;
public:

  ScanNode();
  void SetMatrix(double scannerMatrix3x4[12], double ucs[16]);
  void SetName(const char* pScanName);
  void Add(int np, float* xyz, float* pIntensity, int* rgbColor);
  void ComputeBBox(int np, float* xyz);
  void Finish();
  std::vector <float>& Position()
  {
    return mXyz;
  }
  std::vector <float>& Intensity() { return mIntensity; }
  std::vector <int>& Color()
  {
    return mColor;
  }
  void GetBox(double* minCorner, double* maxCorner);
  Eigen::Matrix4d Transformation() { return mTransformation; }
  std::string ScanName() { return mScanName; }
};

typedef std::function<bool(ScanNode*)> ScanNodeCallback;