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
  if (argc <= 4)
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

int ProcessConvert()
{
  PtxReader ptxReader(input.c_str());
  PtxWriter ptxwriter(output.c_str());
  ptxReader.Init(subsample);
  ptxwriter.Init(posPrecision, intensityPrecision, subsample);
  if (ptxwriter.IsOpen() == false)
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
    if (false == ptxReader.ProcessConvert(ptxwriter))
    {
      printf("failed to convert ptx file");
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