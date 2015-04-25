#ifndef __SSA__PRINTER_INCLUDED__H__
#define __SSA__PRINTER_INCLUDED__H__

#include "SyntaxTree.h"

namespace SSA_Printer
{

class Printer
{
  typedef ParseTree::ISyntaxTreeNode stn;
  FILE* out;
public:
  Printer():out(nullptr){}
  void Print(FILE* where,stn* prog)
  {
    out=where;
    if (!prog)
      return;
    PrintHandler(prog,false);
  }
private:
  void PrintHandler(stn*,bool);
  void PrintSimpleNode(stn*,bool);
  void PrintComplexNode(stn*);
};

}

#endif
