#include <inc/ptxreader.hpp>
#include <vector>
#include <string>
#include <stdio.h>
#include <streambuf>
#include <algorithm>

#ifdef _WIN32
#include <filesystem>
#else
#include <experimental/filesystem>
#endif
namespace fs = std::experimental::filesystem;


struct PtxReader::Impl
{
  char mBuffer[MAXLINELENTH];
  FILE* mFile;
  string mFilename;
  int mSubsample;
  __int64 mPointCount;
  int mNumScan;
  int mCurrentScan;
  vector<float> mX, mY, mZ;
  vector<float> mIntensity;
  vector<int> mColors;
  int mColumns, mRows;

  bool ProcessScan(int subample);
  int ReadPoints(int subample, ScanPointCallback cb);
  int ReadPoints(vector<float>& x, vector<float>& y, vector<float>& z,
                 vector<float>& rIntensity, vector<int>& rgbColor);
  bool ReadLine(std::string& rLine);
  bool ReadSize(int& columns, int& rows);
};

PtxReader::PtxReader(const char* pFilename)
{
  mpImpl = new Impl;
  mpImpl->mSubsample = 1;
  mpImpl->mFilename = pFilename;
  fopen_s(&mpImpl->mFile, pFilename, "r+b");
  mpImpl->mPointCount = 0;
  mpImpl->mNumScan = 0;
}

PtxReader::~PtxReader()
{
  if (mpImpl->mFile)
  { fclose(mpImpl->mFile); }
  mpImpl->mFile = NULL;
  delete mpImpl;
  mpImpl = nullptr;
}

bool
PtxReader::ReadSize(int& column, int& row)
{
  return mpImpl->ReadSize(column, row);
}

bool
PtxReader::Impl::ReadSize(int& column, int& row)
{
  column = row = 0;
  mBuffer[0] = 0;
  fgets(mBuffer, MAXLINELENTH, mFile);
  column = atoi(mBuffer);
  mBuffer[0] = 0;
  bool success = nullptr != fgets(mBuffer, MAXLINELENTH, mFile);
  row = atoi(mBuffer);
  if (success)
  {
    mNumScan++;
    mRows = row;
    mColumns = column;
  }
  return success;
}

bool PtxReader::ReadHeader(vector<string>& rHeader)
{
  rHeader.clear();
  if (mpImpl->mFile == nullptr)
  { return false; }
  for (int i = 0; i < 8; i++)
  {
    mpImpl->mBuffer[0] = 0;
    if (fgets(mpImpl->mBuffer, MAXLINELENTH, mpImpl->mFile) != NULL)
    {
      string str(mpImpl->mBuffer);
      rHeader.push_back(str);
    }
  }
  return rHeader.size() == 8;
}

bool PtxReader::Impl::ReadLine(string& rLine)
{
  bool ret = fgets(mBuffer, MAXLINELENTH, mFile) != NULL;
  rLine = mBuffer;
  return ret;
}

bool PtxReader::HasMoredata()
{
  if (mpImpl->mFile == NULL || feof(mpImpl->mFile))
  { return false; }
  return true;
}

bool PtxReader::ReadHeader(double scannerMatrix3x4[12], double ucs[16])
{
  int pos = 0;
  vector<std::string> header;
  if (false == ReadHeader(header))
  { return false; }
  for (int i = 0; i < 4; i++)
  {
    sscanf_s(header[i].c_str(), "%lg %lg %lg",
             &scannerMatrix3x4[pos], &scannerMatrix3x4[pos + 1],
             &scannerMatrix3x4[pos + 2]);
    pos += 3;
  }
  pos = 0;
  for (int i = 0; i < 4; i++)
  {
    sscanf_s(header[i + 4].c_str(), "%lg %lg %lg %lg",
             &ucs[pos], &ucs[pos + 1], &ucs[pos + 2], &ucs[pos + 3]);
    pos += 4;
  }
  return true;
}

int PtxReader::Impl::ReadPoints(vector<float>& ax, vector<float>& ay, vector<float>& az,
                                vector<float>& rIntensity, vector<int>& rgbColor)
{
  std::string line;
  bool ok = true;
  ax.clear();
  ay.clear();
  az.clear();
  rIntensity.clear();
  rgbColor.clear();
  for (int row = 0; row < mRows && ok; row++)
  {
    if (ReadLine(line))
    {
      //skip row
      if ((row % mSubsample) != 0)
      {
        continue;
      }
      float x, y, z, i = 0;
      int r, g, b;
      int num = sscanf_s(line.c_str(), "%f %f %f %f %d %d %d",
                         &x, &y, &z, &i, &r, &g, &b);
      if (num >= 3)
      {
        ax.push_back(x);
        ay.push_back(y);
        az.push_back(z);
        rIntensity.push_back(i);
        if (num >= 7)
        {
          rgbColor.push_back(r + (g << 8) + (b << 16));
        }
      }
    }
    else
    {
      ok = false;
      break;
    }
  }
  return (int)ax.size();
}

std::string PtxReader::GetScanName()
{
  fs::path filename(mpImpl->mFilename.c_str());
  //string fn = filename.stem();
  return "";
}

int PtxReader::ReadPoints(int subsample, ScanPointCallback cb)
{
  return mpImpl->ReadPoints(subsample, cb);
}

int PtxReader::Impl::ReadPoints(int subsample, ScanPointCallback cb)
{
  mSubsample = subsample;
  const int ReportCount = 100;
  bool ok = true;
  for (int col = 0; col < mColumns && ok; col++)
  {
    string line;
    if (col % ReportCount == 0)
    {
      printf("%.2f%%\r", (float)col / mColumns);
    }
    const int ReportCount = 100;
    bool bSkipColumn = (col % mSubsample) != 0;
    vector<float>x, y, z, intensity;
    vector<int> color;
    int np = ReadPoints(x, y, z, intensity, color);
    if (np && !bSkipColumn)
    {
      cb(np, x.data(), y.data(), z.data(),
         intensity.data(), color.data());
    }
    if (np == 0)
    {
      break;
    }
    mPointCount += np;
    ok = !feof(mFile);
  }
  return ok;
}

bool PtxReader::Impl::ProcessScan(int subample)
{

  auto ExportLambda = [&](int np, float * x,
                          float * y, float * z,
                          float * pIntensity,
                          int* rgbColor)->bool
  {
    size_t size = mX.size();
    mX.resize(size + np);
    mY.resize(size + np);
    mZ.resize(size + np);
    mIntensity.resize(size + np);
    //todo apply transformation
    memcpy(mX.data() + size, x, sizeof(float)* np);
    memcpy(mY.data() + size, y, sizeof(float)* np);
    memcpy(mZ.data() + size, z, sizeof(float)* np);
    memcpy(mIntensity.data() + size, pIntensity, sizeof(float)* np);
    if (rgbColor)
    {
      mColors.resize(size + np);
      memcpy(mColors.data() + size, rgbColor, sizeof(int)* np);
    }
    return true;
  };
  return ReadPoints(subample, ExportLambda);
}

size_t
PtxReader::Load(int subample, float*& x, float*& y, float*& z,
                float*& intensity, int*& color)
{
  while (HasMoredata())
  {
    int columns, rows;
    if (false == ReadSize(columns, rows))
    {
      break;
    }
    double scannerMatrix3x4[12];
    double ucs[16];
    ReadHeader(scannerMatrix3x4, ucs);
    if (false == mpImpl->ProcessScan(subample))
    {
      break;
    }
  }
  x = mpImpl->mX.data();
  y = mpImpl->mY.data();
  z = mpImpl->mZ.data();
  intensity = mpImpl->mIntensity.data();
  color = mpImpl->mColors.data();
  return mpImpl->mX.size();
}

__int64 PtxReader::GetPointCount() { return mpImpl-> mPointCount; }
int PtxReader::GetNumScan() { return mpImpl->mNumScan; }