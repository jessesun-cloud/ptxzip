#include <inc/ptxreader.hpp>
#include <vector>
#include <string>
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
  int mColumns, mRows;

  int ReadPoints(int subample, ScanPointCallback cb);
  int ReadPoints(vector<float>& pos,
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
  if (feof(mFile))
  { return false; }
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

bool PtxReader::GetHeader(vector<string>& rHeader)
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

bool PtxReader::GetHeader(double scannerMatrix3x4[12], double ucs[16])
{
  int pos = 0;
  vector<std::string> header;
  if (false == GetHeader(header))
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

int PtxReader::Impl::ReadPoints(vector<float>& rPos,
                                vector<float>& rIntensity, vector<int>& rgbColor)
{
  std::string line;
  bool ok = true;
  rPos.clear();
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
        rPos.push_back(x);
        rPos.push_back(y);
        rPos.push_back(z);
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
  return (int)rPos.size() / 3;
}

std::string PtxReader::GetScanName()
{
  fs::path filename(mpImpl->mFilename.c_str());
  string fn = filename.stem().u8string();
  fn += "_" + std::to_string(mpImpl->mNumScan);
  fn += "_" + std::to_string(mpImpl->mColumns);
  fn += "x" + std::to_string(mpImpl->mRows);
  return fn;
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
  vector<float> pos, intensity;
  vector<int> color;
  for (int col = 0; col < mColumns && ok; col++)
  {
    string line;
    if (col % ReportCount == 0)
    {
      printf("%.2f%%\r", (float)col / mColumns);
    }
    const int ReportCount = 100;
    bool bSkipColumn = (col % mSubsample) != 0;
    int np = ReadPoints(pos, intensity, color);
    if (np && !bSkipColumn)
    {
      cb(np, pos.data(), intensity.data(), color.data());
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

__int64 PtxReader::GetPointCount() { return mpImpl->mPointCount; }
int PtxReader::GetNumScan() { return mpImpl->mNumScan; }

bool PtxReader::LoadScan(int subsample, vector< shared_ptr<ScanNode>>& rNodes)
{
  while (true)
  {
    int columns, rows;
    if (false == ReadSize(columns, rows))
    {
      return false;
    }
    ScanNode* pNode = new ScanNode;
    double scannerMatrix3x4[12];
    double ucs[16];
    GetHeader(scannerMatrix3x4, ucs);
    pNode->SetName(GetScanName().c_str());
    pNode->SetMatrix(scannerMatrix3x4, ucs);
    auto ExportLambda = [&](int np, float * x,
                            float * pIntensity,
                            int* rgbColor)->void
    {
      pNode->Add(np, x, pIntensity, rgbColor);
    };
    ReadPoints(subsample, ExportLambda);
    shared_ptr<ScanNode> scan(pNode);
    rNodes.push_back(scan);
  }
  return true;
}
