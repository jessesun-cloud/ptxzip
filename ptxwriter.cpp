#include <inc/ptxwriter.hpp>

PtxWriter::PtxWriter()
{
  mPosPrecision = 3;
  mIntensityPrecision = 3;
  mSubsample = 1;
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

bool PtxWriter::Open(const char* pFilename)
{
  mFile = nullptr;
  fopen_s(&mFile, pFilename, "w+b");
  return mFile != nullptr;
}

void PtxWriter::WriteHeader(double scannerMatrix3x4[12], double ucs[16])
{
  int pos = 0;
  char buffer[BUFFERLENGTH];
  for (int i = 0; i < 4; i++)
  {
    sprintf_s(buffer, BUFFERLENGTH, "%g %g %g",
              scannerMatrix3x4[pos], scannerMatrix3x4[pos + 1], scannerMatrix3x4[pos + 2]);
    WriteLine(buffer);
    pos += 3;
  }
  pos = 0;
  for (int i = 0; i < 4; i++)
  {
    sprintf_s(buffer, BUFFERLENGTH, "%g %g %g %g",
              ucs[pos], ucs[pos + 1], ucs[pos + 2], ucs[pos + 3]);
    WriteLine(buffer);
    pos += 4;
  }
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

static bool IsEmptyLine(float x, float y, float z)
{
  return x == 0 && y == 0 && z == 0;
}

bool PtxWriter::WritePoint(float x, float y, float z,
                           float i, int r, int g, int b)
{
  char buffer[BUFFERLENGTH];
  const char* pOutput = buffer;
  const char* pFormat = (i == 0.5) ?
                        mFormatBufferNoIntensity : mFormatBuffer;

  if (mFormat == 4)
  {
    if (IsEmptyLine(x, y, z))
    { return "0 0 0 0"; }
    sprintf_s(buffer, BUFFERLENGTH, pFormat, x, y, z, i);
  }
  else
  {
    if (IsEmptyLine(x, y, z))
    { return "0 0 0 0 0 0 0"; }
    sprintf_s(buffer, BUFFERLENGTH, pFormat, x, y, z, i, r, g, b);
  }
  string line = pOutput;
  eraseAllSubStr(line, mZeroString.c_str());
  WriteLine(line.c_str());
  return true;
}

bool PtxWriter::InitExportFormat()
{
  mZeroString = ".0000000000";
  mZeroString[mPosPrecision + 1] = 0;
  if (mFormat == 4)
  {
    sprintf_s(mFormatBufferNoIntensity, BUFFERLENGTH,
              "%%.%dg %%.%dg %%.%dg %1g",
              mPosPrecision, mPosPrecision, mPosPrecision);
    sprintf_s(mFormatBuffer, BUFFERLENGTH,
              "%%.%dg %%.%dg %%.%dg %%.%dg",
              mPosPrecision, mPosPrecision, mPosPrecision, mIntensityPrecision);
  }
  else if (mFormat == 7)
  {
    sprintf_s(mFormatBufferNoIntensity, BUFFERLENGTH,
              "%%.%dg %%.%dg %%.%dg %%.%dg %%d %%d %%d",
              mPosPrecision, mPosPrecision, mPosPrecision, 1);
    sprintf_s(mFormatBuffer, BUFFERLENGTH,
              "%%.%dg %%.%dg %%.%dg %%.%dg %%d %%d %%d",
              mPosPrecision, mPosPrecision, mPosPrecision, mIntensityPrecision);
  }
  else
  { mFormat = 0; }//invalid format
  return mFormat;
}

int
PtxWriter::WritePoints(int numPoint, float* x, float* y, float* z,
                       float* rIntensity, int* rgbColor)
{
  if (mFormat == -1)
  {
    mFormat = rgbColor == nullptr ? 4 : 7;
    InitExportFormat();
  }
  if (rgbColor == nullptr && mFormat == 7)
  { return 0; }

  for (int i = 0; i < numPoint; i++)
  {
    if (i % mSubsample != 0)
    { continue; }
    int c = mFormat == 7 ? rgbColor[i] : 0;
    WritePoint(x[i], y[i], z[i], rIntensity ? rIntensity[i] : 0.5f,
               c & 0xff, (c >> 8) & 0xff, (c >> 16) & 0xff);
  }
  return (int)numPoint;
}
