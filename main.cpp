#include <inc/ptxreader.hpp>
#include <inc/ptxwriter.hpp>

std::string input, output;
int posPrecision = 3, intensityPrecision = 3, subsample = 1;

void printusage()
{
  printf("usage: ptxzip input output -p P -i I -s S"\
         "input -input filename"\
         "output -output filename"\
         "P -position precision, range 1-6"\
         "I -intensity precision, range 1-6"\
         "S -position precision, range 1-10"
        );
}

bool parseInput(int argc, char** argv)
{
  if (argc <= 3)
  {
    printusage();
    return false;
  }

  input = argv[1];
  output = argv[2];
  for (int i = 3; i < argc; i++)
  {
    std::string key = argv[i];
    if (key[0] != '-')
    { continue; }
    if (key[1] == 'p')
    { posPrecision = atoi(argv[++i]); }
    else if (key[1] == 'i')
    { intensityPrecision = atoi(argv[++i]); }
    else if (key[1] == 's')
    { subsample = atoi(argv[++i]); }
  }
  return true;
}

bool ProcessScan(int subample, PtxReader ptxreader, PtxWriter& ptxwriter)
{
  int columns, rows;
  if (false == ptxreader.ReadSize(columns, rows))
  {
    return false;
  }
  ptxwriter.NextScan();
  ptxwriter.WriteSize(columns / subsample, rows / subsample);
  double scannerMatrix3x4[12];
  double ucs[16];
  if (ptxreader.ReadHeader(scannerMatrix3x4, ucs))
  {
    ptxwriter.WriteHeader(scannerMatrix3x4, ucs);
  }
  auto ExportLambda = [&](int np, float * x,
                          float * y, float * z,
                          float * pIntensity,
                          int* rgbColor)->bool
  {
    ptxwriter.WritePoints(np, x, y, z, pIntensity, rgbColor);
    return true;
  };
  return ptxreader.ReadPoints(subample, columns, rows, ExportLambda);
}

int ProcessConvert()
{
  PtxReader ptxReader(input.c_str());
  PtxWriter ptxwriter;

  ptxwriter.Init(posPrecision, intensityPrecision, subsample);
  if (ptxwriter.Open(output.c_str()) == false)
  {
    printf("can not create file %s", output.c_str());
    return -3;
  }
  if (ptxReader.HasMoredata() == false)
  {
    printf("can not open file %s", input.c_str());
    return -2;
  }
  while (ptxReader.HasMoredata())
  {
    if (false == ProcessScan(subsample, ptxReader, ptxwriter))
    {
      break;
    }
  }
  printf("successfully convert %I64d points %d scan",
         ptxReader.GetPointCount(), ptxReader.GetNumScan());
  return ptxReader.GetNumScan() > 0;
}

int main(int argc, char** argv)
{
  if (parseInput(argc, argv))
  {
    return ProcessConvert();
  }
  return -1;
}