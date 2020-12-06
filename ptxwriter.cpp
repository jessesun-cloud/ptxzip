#include <inc/ptxwriter.hpp>
#include <vector>
#include <string>
#include <Eigen/Dense>
#define BUFFERLENGTH 200

PtxWriter::PtxWriter()
{
  mPosPrecision = 3;
  mIntensityPrecision = 3;
  mFormat = -1;
}

PtxWriter::~PtxWriter()
{
  if (mFile)
  { fclose(mFile); }
}

void
PtxWriter::Init(int posPrecision, int intensityPrecision)
{
  mPosPrecision = posPrecision;
  mIntensityPrecision = intensityPrecision;
  mScale = 1;
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

std::string FormatFloat(float x, const char* pFormat)
{
  if ((int)x == x)
  { return std::to_string((int)x); }
  char buffer[200];
  char* pBuffer = buffer;
  sprintf_s(buffer, 200, pFormat, x);
  if (strncmp(buffer, "0.", 2) == 0)
  { pBuffer++; } //remove 0 before .
  else if (strncmp(buffer, "-0.", 3) == 0)
  {
    //-0.  change to -.
    pBuffer[1] = pBuffer[0];
    pBuffer++;
  }
  //remove ending 0
  int pos = (int)strlen(pBuffer) - 1;
  while (pos > 0 && pBuffer[pos] == '0')
  {
    pBuffer[pos--] = 0;
  }
  return pBuffer;
}

bool PtxWriter::WritePoint(float x, float y, float z,
                           float i, int r, int g, int b)
{
  string line = FormatFloat(x, mCoordFormat.c_str()) + " " +
                FormatFloat(y, mCoordFormat.c_str()) + " " +
                FormatFloat(z, mCoordFormat.c_str()) + " " +
                FormatFloat(i, mIntensityFormat.c_str());
  if (mScale != 1)
  {
    line = std::to_string((int)(x * mScale)) + " " +
           std::to_string((int)(y * mScale)) + " " +
           std::to_string((int)(z * mScale)) + " " +
           FormatFloat(i, mIntensityFormat.c_str());
  }
  if (mFormat == 7)
  {
    line += " " + std::to_string(r) +
            " " + std::to_string(g) +
            " " + std::to_string(b);
  }
  WriteLine(line.c_str());
  return true;
}

void PtxWriter::InitExportFormat()
{
  mCoordFormat = "%." + std::to_string(mPosPrecision) + "f";
  mIntensityFormat = "%." + std::to_string(mIntensityPrecision) + "f";
}

int
PtxWriter::WritePoints(int numPoint, float* pnt,
                       float* rIntensity, int* rgbColor)
{
  if (mFormat == -1)
  {
    mFormat = rgbColor == nullptr ? 4 : 7;
    InitExportFormat();
  }
  if (rgbColor == nullptr && mFormat == 7)
  { return 0; }
  float* pPoint = pnt;
  for (int i = 0; i < numPoint; i++, pPoint += 3)
  {
    int c = mFormat == 7 ? rgbColor[i] : 0;
    WritePoint(pPoint[0], pPoint[1], pPoint[2], rIntensity ? rIntensity[i] : 0.5f,
               c & 0xff, (c >> 8) & 0xff, (c >> 16) & 0xff);
  }
  return (int)numPoint;
}
