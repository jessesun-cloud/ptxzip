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
PtxReader::ReadSize(int& column, int& row)
{
  column = row = 0;
  mBuffer[0] = 0;
  fgets(mBuffer, MAXLINELENTH, mFile);
  sscanf(mBuffer, "%d", &column);
  mBuffer[0] = 0;
  fgets(mBuffer, MAXLINELENTH, mFile);
  sscanf(mBuffer, "%d", &row);
  return column * row != 0;
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

bool PtxReader::ProcessConvert(PtxWriter& ptxwriter)
{
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
  vector<string> header;
  ReadHeader(header);
  ptxwriter.WriteHeader(header);

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
    for (int x = 0; x < rows; x++)
      if (ReadLine(line))
      {
        //skip column or row
        if (bSkipColumn || (x % mSubsample) != 0)
        {
          continue;
        }
        if (false == ptxwriter.ProcessLine(line))
        {
          ok = false;
          break;
        }
      }
      else
      {
        ok = false;
        break;
      }
  }
  return ok;
}