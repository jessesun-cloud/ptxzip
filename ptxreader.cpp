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
  mFile = fopen(pFilename, "r+b");
}

PtxReader::~PtxReader()
{
  if (mFile)
  { fclose(mFile); }
  mFile = NULL;
}

void
PtxReader::Init(int subsample)
{
  mSubsample = subsample;
}

bool
PtxReader::ReadSize(int& column, int& width)
{
  column = width = 0;
  mBuffer[0] = 0;
  fgets(mBuffer, MAXLINELENTH, mFile);
  sscanf(mBuffer, "%d", &column);
  mBuffer[0] = 0;
  fgets(mBuffer, MAXLINELENTH, mFile);
  sscanf(mBuffer, "%d", &width);
  mWidth = width;
  mColumn = column;
  return mWidth * mColumn != 0;
}

void PtxReader::RemoveEndCLn(std::string& str)
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
      RemoveEndCLn(str);
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
