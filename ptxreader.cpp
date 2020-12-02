#include <inc/ptxreader.hpp>
#include <vector>
#include <string>
#include <stdio.h>
#include <streambuf>
#include <algorithm>

PtxReader::PtxReader(const char* pFilename)
{
  mSubsample = 1;
  mFilename = pFilename;
  fopen_s(&mFile, pFilename, "r+b");
  mPointCount = 0;
  mNumScan = 0;
}

PtxReader::~PtxReader()
{
  if (mFile)
  { fclose(mFile); }
  mFile = NULL;
}

bool
PtxReader::ReadSize(int& column, int& row)
{
  column = row = 0;
  mBuffer[0] = 0;
  fgets(mBuffer, MAXLINELENTH, mFile);
  column = atoi(mBuffer);
  mBuffer[0] = 0;
  bool success = nullptr != fgets(mBuffer, MAXLINELENTH, mFile);
  row = atoi(mBuffer);
  mPointCount += column * row;
  if (column * row != 0)
  { mNumScan++; }
  mRows = row;
  return success;
}

void PtxReader::RemoveEndCrLn(std::string& str)
{
  std::string::iterator end_pos = std::remove(str.begin(), str.end(), '\r');
  str.erase(end_pos, str.end());
  end_pos = std::remove(str.begin(), str.end(), '\n');
  str.erase(end_pos, str.end());
}

void PtxReader::ReadHeader(vector<string>& rHeader)
{
  rHeader.clear();
  if (mFile == nullptr)
  { return ; }
  for (int i = 0; i < 8; i++)
  {
    mBuffer[0] = 0;
    if (fgets(mBuffer, MAXLINELENTH, mFile) != NULL)
    {
      string str(mBuffer);
      RemoveEndCrLn(str);
      rHeader.push_back(str);
    }
  }
}

bool PtxReader::ReadLine(string& rLine)
{
  bool ret = fgets(mBuffer, MAXLINELENTH, mFile) != NULL;
  rLine = mBuffer;
  return ret;
}

bool PtxReader::HasMoredata()
{
  if (mFile == NULL || feof(mFile))
  { return false; }
  return true;
}

bool PtxReader::ProcessConvert(int subample, PtxWriter& ptxwriter)
{
  mSubsample = subample;
  int columns, rows;
  if (false == ReadSize(columns, rows))
  {
    return true;
  }
  if (columns / mSubsample == 0 || rows / mSubsample == 0)
  {
    return false;
  }
  ptxwriter.WriteSize(columns / mSubsample, rows / mSubsample);
  double scannerMatrix3x4[12];
  double ucs[16];
  ReadHeader(scannerMatrix3x4, ucs);
  ptxwriter.WriteHeader(scannerMatrix3x4, ucs);

  const int ReportCount = 100;
  bool ok = true;
  for (int col = 0; col < columns && ok; col++)
  {
    string line;
    if (col % ReportCount == 0)
    {
      printf("%.2f%%\r", (float)col / columns);
    }
    const int ReportCount = 100;
    bool bSkipColumn = (col % mSubsample) != 0;
    vector<float>x, y, z, intensity;
    vector<int> color;
    int np = ReadPoints(x, y, z, intensity, color);
    if (np && !bSkipColumn)
    {
      ptxwriter.WritePoints(x, y, z, intensity, color);
    }
    if (np == 0)
    { break; }
  }
  return ok;
}

void PtxReader::ReadHeader(double scannerMatrix3x4[12], double ucs[16])
{
  int pos = 0;
  vector<std::string > header;
  ReadHeader(header);

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
}

int PtxReader::ReadPoints(vector<float>& ax, vector<float>& ay, vector<float>& az,
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
      float x, y, z, i;
      int r, g, b;
      int num = sscanf_s(line.c_str(), "%f %f %f %f %d %d %d",
                         &x, &y, &z, &i, &r, &g, &b);
      ax.push_back(x);
      ay.push_back(y);
      az.push_back(z);
      rIntensity.push_back(i);
      if (num >= 7)
      {
        rgbColor.push_back(r + (g << 8) + (b << 16));
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