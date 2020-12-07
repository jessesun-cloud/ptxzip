#include <inc/IScanInterface.hpp>
#include <vector>
#include <algorithm>
#include <math.h>

void ScanNode::SetMatrix(double matrix3x4[12], double ucs[16])
{
  Eigen::Vector3d translate = Eigen::Vector3d(matrix3x4[0], matrix3x4[1], matrix3x4[2]);
  Eigen::Vector3d x = *(Eigen::Vector3d*)(matrix3x4 + 3);
  Eigen::Vector3d y = *(Eigen::Vector3d*)(matrix3x4 + 6);
  Eigen::Vector3d z = *(Eigen::Vector3d*)(matrix3x4 + 9);
  Eigen::Matrix3d rot;
  rot.col(0) = x;
  rot.col(1) = y;
  rot.col(2) = z;
  //Eigen::Quaterniond d(rot);
  mTransformation.setIdentity();   // Set to Identity to make bottom row of Matrix 0,0,0,1
  mTransformation.block<3, 3>(0, 0) = rot;
  mTransformation.block<3, 1>(0, 3) = translate;
  //if scanner transformation is identity, use UCS
  if (mTransformation.isIdentity())
  {
    memcpy(mTransformation.data(), ucs, 16);
    mTransformation = mTransformation.transpose();
  }
}

RenderableScan::RenderableScan()
{
  mLevels = 0;
  mCurrentLevel = 0;
}

ScanNode::ScanNode()
{
  mMinCoord = Vector3f(FLT_MAX, FLT_MAX, FLT_MAX);
  mMaxCoord = Vector3f(FLT_MIN, FLT_MIN, FLT_MIN);
}

void ScanNode::SetName(const char* pScanName)
{
  mScanName = pScanName;
}

void ScanNode::Add(int np, float* xyz, float* pIntensity, int* rgbColor)
{
  size_t size = mXyz.size();
  mXyz.resize(size + np * 3);
  memcpy(mXyz.data() + size, xyz, sizeof(float) * np * 3);
  mIntensity.resize(size + np);
  memcpy(mIntensity.data() + size, pIntensity, sizeof(float) * np);

  if (rgbColor)
  {
    mColor.resize(size + np);
    memcpy(mColor.data() + size, rgbColor, sizeof(int) * np);
  }
  ComputeBBox(np, xyz);
}

void ScanNode::ComputeBBox(int np, float* xyz)
{
  //compute
  float* pos = xyz;
  for (int i = 0; i < np; i++, pos += 3)
  {
    for (int index = 0; index < 3; index++)
    {
      mMinCoord[index] = std::min(mMinCoord[index], pos[index]);
      mMaxCoord[index] = std::max(mMaxCoord[index], pos[index]);
    }
  }
}

void ScanNode::Finish() {}

void ScanNode::GetBox(double minCorner[3], double maxCorner[3])
{
  memcpy(minCorner, mMinCoord.data(), 12);
  memcpy(maxCorner, mMaxCoord.data(), 12);
}