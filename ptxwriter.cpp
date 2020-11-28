#include <inc/ptxwriter.hpp>
#include <inc/ptxreader.hpp>
PtxWriter::PtxWriter(const char* pFilename)
{
  mPosPrecision = 3;
  mIntensityPrecision = 3;
  mSubsample = 1;
  mFile = fopen(pFilename, "w+b");
  mFormat = -1;
}

PtxWriter::~PtxWriter()
{
  fclose(mFile);
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
  sprintf(buffer, "%d", cols);
  WriteLine((buffer));
  sprintf(buffer, "%d", rows);
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
    string str(rInput);
    PtxReader::RemoveEndCLn(str);
    WriteLine(str.c_str());
    return false;
  }
  float x, y, z, i;
  char r[20], g[20], b[20];
  int num = sscanf(rInput.c_str(), "%f %f %f %f %s %s %s",
                   &x, &y, &z, &i, r, g, b);
  char buffer[MAXLINELENTH];
  if (mFormat == 4)
  {
    if (!(x == 0 && y == 0 && z == 0))
    {
      sprintf(buffer, mFormatBuffer, x, y, z, i);
    }
    else
    { strcpy_s(buffer, MAXLINELENTH, mFormatBufferZero); }
  }
  else if (mFormat == 7)
  {
    if (!(x == 0 && y == 0 && z == 0))
    { sprintf(buffer, mFormatBuffer, x, y, z, i, r, g, b); }
    else
    { strcpy_s(buffer, MAXLINELENTH, mFormatBufferZero); }
  }
  WriteLine(buffer);
  return true;
}

bool PtxWriter::AnalysisFormat(const string& rLine)
{
  if (mFormat >= 0)
  {
    return mFormat;
  }
  float x, y, z, i;
  //  int r, g, b;
  int num = sscanf(rLine.c_str(), "%f %f %f %f %f %f %f", &x, &y, &z, &i, &x, &y, &z);
  mFormat = num;
  if (mFormat >= 7)
  {
    mFormat = 7;
  }
  if (mFormat == 4)
  {
    sprintf(mFormatBuffer, "%%.%df %%.%df %%.%df %%.%df",
            mPosPrecision, mPosPrecision, mPosPrecision, mIntensityPrecision);
    strcpy_s(mFormatBufferZero, 200, "0 0 0 0.5");
  }
  else if (mFormat == 7)
  {
    sprintf(mFormatBuffer, "%%.%df %%.%df %%.%df %%.%df %%s %%s %%s",
            mPosPrecision, mPosPrecision, mPosPrecision, mIntensityPrecision);
    strcpy_s(mFormatBufferZero, 200, "0 0 0 0.5 0 0 0");
  }
  else
  { mFormat = 0; }//invalid format
  return mFormat;
}