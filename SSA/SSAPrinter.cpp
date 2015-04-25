#include "SSAPrinter.h"
#include "SyntaxTree.h"
#include "Table.h"
#include <cstdio>
#include <string>

using SSA_Printer::Printer;

void Printer::PrintHandler(stn* curr,bool parenth)
{
  if (!curr)
    return;

  switch (curr->meaning)
    {
    case SemTable::ASSIGN:
    case SemTable::NUM:
    case SemTable::NAME:
    case SemTable::UN_PLUS:
    case SemTable::UN_MINUS:
    case SemTable::NOT:
    case SemTable::INC:
    case SemTable::DEC:
    case SemTable::INC_P:
    case SemTable::DEC_P:
      PrintSimpleNode(curr,parenth);
      return;
    default:
      PrintComplexNode(curr);
      return;
    }
}

void Printer::PrintSimpleNode(stn* curr,bool parenth)
{
  switch (curr->meaning)
    {
    case SemTable::ASSIGN:
      if (parenth)
	fprintf(out,"(");
      PrintHandler(curr->GetOp(1),true);
      fprintf(out," = ");
      PrintHandler(curr->GetOp(2),true);
      if (parenth)
	fprintf(out,")");
      return;

    case SemTable::NUM:
      fprintf(out,"%lg",curr->GetVal());
      return;
    case SemTable::NAME:
      fprintf(out,"%s%u",curr->GetName()->c_str(),curr->GetVersion());
      return;

    case SemTable::UN_PLUS:
      fprintf(out," +");
      break;
    case SemTable::UN_MINUS:
      fprintf(out," -");
      break;
    case SemTable::NOT:
      fprintf(out," !");
      break;
    case SemTable::INC:
      fprintf(out," ++");
      break;
    case SemTable::DEC:
      fprintf(out," --");
      break;

    case SemTable::INC_P:
      PrintHandler(curr->GetOp(1),true);
      fprintf(out," ++");
      return;
    case SemTable::DEC_P:
      PrintHandler(curr->GetOp(1),true);
      fprintf(out," --");
      return;

    default:
      printf("Error in PrintSimpleNode with %d\n",curr->meaning);
      exit(1);
    }
  PrintHandler(curr->GetOp(1),true);
}

void Printer::PrintComplexNode(stn* curr)
{
  if (curr->meaning==SemTable::ST_SEQ)
    {
      for (unsigned i=0;i<curr->Size();++i)
	{
	  PrintHandler(curr->GetOp(i),false);
	  fprintf(out,";\n");
	}
      return;
    }

  fprintf(out,"(");
  PrintHandler(curr->GetOp(0),true);
  for (unsigned i=1;i<curr->Size();++i)
    {
      std::string c;
      switch (curr->meaning)
	{
	case SemTable::OR:
	  c=" || ";break;
	case SemTable::AND:
	  c=" && ";break;
	default:
	  switch (curr->GetType(i-1))
	    {
	    case SemTable::OR:
	      c=" || ";break;
	    case SemTable::AND: 
	      c=" && ";break;
	    case SemTable::LESS:
	      c=" < ";break;
	    case SemTable::LESS_EQ:
	      c=" <= ";break;
	    case SemTable::GR:
	      c=" > ";break;
	    case SemTable::GR_EQ:
	      c=" >= ";break;
	    case SemTable::EQUAL:
	      c=" == ";break;
	    case SemTable::NOT_EQUAL:
	      c=" != ";break;
	    case SemTable::PLUS:
	      c=" + ";break;
	    case SemTable::MINUS:
	      c=" - ";break;
	    case SemTable::MUL:
	      c=" * ";break;
	    case SemTable::DIV:
	      c=" / ";break;
	    default:
	      printf("Error in PrintComplexNode with %d\n",curr->meaning);
	      exit(1);
	    }
	}
      fprintf(out,"%s",c.c_str());
      PrintHandler(curr->GetOp(i),true);
    }
  fprintf(out,")");
}
