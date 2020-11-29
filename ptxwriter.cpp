#include <inc/ptxwriter.hpp>

PtxWriter::PtxWriter(const char* pFilename)
{
  mPosPrecision = 3;
  mIntensityPrecision = 3;
  mSubsample = 1;
  mFile = nullptr;
  fopen_s(&mFile, pFilename, "w+b");
  mFormat = -1;
}

PtxWriter::~PtxWriter()
{
  if (mFile)
  { fclose(mFile); }
}

void
PtxWriter::Init(int posPrecision, int intensityPrecision, int subsample)
{
  mPosPrecision = posPrecision;
  mIntensityPrecision = intensityPrecision;
  mSubsample = subsample;
}

void PtxWriter::WriteSize(int cols, int rows)
{
  mFormat = -1;
  WriteLine(std::to_string(cols).c_str());
  WriteLine(std::to_string(rows).c_str());
}

bool PtxWriter::WriteLine(const char* pLine)
{
  return 1 == fprintf(mFile, "%s\r\n", pLine);
}

bool PtxWriter::IsOpen()
{
  return mFile != nullptr;
}

bool PtxWriter::WriteHeader(vector<string>& header)
{
  for (string line : header)
  {
    WriteLine(line.c_str());
  }
  return true;
}

//https://thispointer.com/how-to-remove-substrings-from-a-string-in-c/
/*
 * Erase all Occurrences of given substring from main string.
 */
void eraseAllSubStr(std::string& mainStr, const std::string& toErase)
{
  size_t pos = std::string::npos;
  // Search for the substring in string in a loop untill nothing is found
  while ((pos = mainStr.find(toErase)) != std::string::npos)
  {
    // If found then erase it from string
    mainStr.erase(pos, toErase.length());
  }
}

bool PtxWriter::ProcessLine(const string& rInput)
{
  AnalysisFormat(rInput);
  if (mFormat == 0)
  {
    return false;
  }
  float x, y, z, i;
  char r[20], g[20], b[20];
  int num = sscanf_s(rInput.c_str(), "%f %f %f %f %s %s %s",
                     &x, &y, &z, &i, r, 20, g, 20, b, 20);
  char buffer[BUFFERLENGTH];
  const char* pOutput = buffer;
  const char* pFormat = (i == 0 || i == 0.5) ?
                        mFormatBufferNoIntensity : mFormatBuffer;
  if (num != mFormat)
  { return false; }
  if (mFormat == 4)
  {
    if (!(x == 0 && y == 0 && z == 0))
    {
      sprintf_s(buffer, BUFFERLENGTH, pFormat, x, y, z, i);
    }
    else
    {
      pOutput = "0 0 0 0.5";
    }
  }
  else
  {
    if (!(x == 0 && y == 0 && z == 0))
    {
      sprintf_s(buffer, BUFFERLENGTH, pFormat, x, y, z, i, r, g, b);
    }
    else
    {
      pOutput = "0 0 0 0.5 0 0 0";
    }
  }
  string line = pOutput;
  eraseAllSubStr(line, mZeroString.c_str());
  WriteLine(line.c_str());
  return true;
}

bool PtxWriter::AnalysisFormat(const string& rLine)
{
  if (mFormat >= 0)
  {
    return mFormat;
  }
  float x, y, z, i;
  float r, g, b;
  int num = sscanf_s(rLine.c_str(), "%f %f %f %f %f %f %f",
                     &x, &y, &z, &i, &r, &g, &b);
  mFormat = num;
  mZeroString = ".0000000000";
  mZeroString[mPosPrecision + 1] = 0;
  if (mFormat == 4)
  {
    sprintf_s(mFormatBufferNoIntensity, BUFFERLENGTH, "%%.%df %%.%df %%.%df %%.%df",
              mPosPrecision, mPosPrecision, mPosPrecision, 1);
    sprintf_s(mFormatBuffer, BUFFERLENGTH, "%%.%df %%.%df %%.%df %%.%df",
              mPosPrecision, mPosPrecision, mPosPrecision, mIntensityPrecision);
  }
  else if (mFormat == 7)
  {
    sprintf_s(mFormatBufferNoIntensity, BUFFERLENGTH,
              "%%.%df %%.%df %%.%df %%.%df %%s %%s %%s",
              mPosPrecision, mPosPrecision, mPosPrecision, 1);
    sprintf_s(mFormatBuffer, BUFFERLENGTH,
              "%%.%df %%.%df %%.%df %%.%df %%s %%s %%s",
              mPosPrecision, mPosPrecision, mPosPrecision, mIntensityPrecision);
  }
  else
  { mFormat = 0; }//invalid format
  return mFormat;
}