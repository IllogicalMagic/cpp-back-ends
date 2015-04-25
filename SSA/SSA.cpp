#include <cstdio>
#include <cassert>
#include "Parser/ParsingDriver.h"
#include "TreeAnalyzer.h"
#include "Error.h"
#include "SSAPrinter.h"

int main()
{
  FILE* in=fopen("expression.txt","r");
  assert(in);
  IntrError::ErrHandler err;
  yy::ParsingDriver ps(in,err);
  if (ps.Parse())
    {
      TreeAnalysis::TreeAnalyzer ta(ps.GetMain(),err);
      if (ta.MakeSSA())
	{
	  SSA_Printer::Printer pr;
	  FILE* out=fopen("output.txt","w");
	  pr.Print(out,ta.GetMain());
	  fclose(out);
	}
    }
  fclose(in);
  return 0;
}
