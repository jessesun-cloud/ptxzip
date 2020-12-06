#pragma once

#include <functional>
//#include <E57Simple.h>
#include <Eigen/Dense>
#include <vector>
#include <map>
using namespace std;
using namespace Eigen;

typedef std::function<bool(int numPoint,
                           float* pos, float* rIntensity,
                           int* rgbColor)> ScanPointCallback;

class RenderableScan
{
public:
  Eigen::Matrix4d mTransformation;
  vector< std::vector <Eigen::Vector3d>> mPos;
  vector< std::vector <float>> mIntensity;
  vector< std::vector <int>> mColor;
  vector<int> mVBOId;
  int mLevels;
  int mCurrentLevel;
  Eigen::Vector3d mMinCoord, mMaxCoord;
};

class ScanNode
{
public:
  std::string mScanName;
  Eigen::Matrix4d mTransformation;
  RenderableScan mData;
  Eigen::Vector3d mMinCoord, mMaxCoord;

  void SetMatrix(Eigen::Matrix4d& rTransformation)
  {
    mTransformation = rTransformation;
  }
  void Add(int np, float* x, float* pIntensity, int* rgbColor) {}
};

typedef std::function<bool(ScanNode*)> ScanNodeCallback;