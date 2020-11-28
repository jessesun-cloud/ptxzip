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

bool ProcessConvert(PtxReader& reader, PtxWriter& ptxwriter)
{
  int column, width;
  if (false == reader.ReadSize(column, width))
  { return true; }
  if (column / subsample == 0 || width / subsample == 0)
  { return false; }
  ptxwriter.WriteSize(column / subsample, width / subsample);
  vector<string> header;
  reader.ReadHeader(header);
  ptxwriter.WriteHeader(header);

  const int ReportCount = 100;
  bool ok = true;
  for (int i = 0; i < column && ok; i++)
  {
    string line;
    if (i % ReportCount == 0)
    {
      printf("%.2f%%\r", (float)i / column);
    }
    const int ReportCount = 100;
    bool bSkip = (i % subsample) != 0;
    for (int x = 0; x < width; x++)
      if (reader.ReadLine(line))
      {
        if (bSkip)
        { continue; }
        if ((x % subsample) == 0)
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

void ProcessConvert()
{
  PtxReader ptxReader(input.c_str());
  PtxWriter ptxwriter(output.c_str());
  ptxReader.Init(subsample);
  ptxwriter.Init(posPrecision, intensityPrecision, subsample);
  if (ptxwriter.IsOpen() == false)
  {
    printf("can not create file %s", output.c_str());
    return;
  }
  if (ptxReader.HasMoredata() == false)
  {
    printf("can not open file %s", input.c_str());
    return;
  }
  while (ptxReader.HasMoredata())
  {
    if (false == ProcessConvert(ptxReader, ptxwriter))
    {
      printf("failed to convert ptx file");
      break;
    }
  }
}

void main(int argc, char** argv)
{
  if (parseInput(argc, argv))
  {
    ProcessConvert();
  }
}