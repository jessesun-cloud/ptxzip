#include <inc/ptxwriter.hpp>
#include <inc/ptxreader.hpp>
PtxWriter::PtxWriter(const char* pFilename)
{
  mPosPrecision = 3;
  mIntensityPrecision = 3;
  mSubsample = 1;
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
  char buffer[200];
  sprintf_s(buffer, 200, "%d", cols);
  WriteLine((buffer));
  sprintf_s(buffer, 200, "%d", rows);
  WriteLine((buffer));
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
  char buffer[MAXLINELENTH];
  const char* pOutput = buffer;
  const char* pFormat = i == 0.5 ? mFormatBufferNoIntensity : mFormatBuffer;
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
  WriteLine(pOutput);
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
  int num = sscanf_s(rLine.c_str(), "%f %f %f %f %f %f %f", &x, &y, &z, &i, &r, &g, &b);
  mFormat = num;
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