#include "TreeAnalyzer.h"
#include "Table.h"
#include "VarCont.h"
#include "Error.h"
#include "SyntaxTree.h"

#include <string>
#include <vector>
#include <algorithm>

using TreeAnalysis::TreeAnalyzer;

typedef ParseTree::ISyntaxTreeNode stn;

bool TreeAnalyzer::MakeSSA()
{
  vc_stack.PushVC();
  NodeHandler(mn);
  vc_stack.PopVC();
  if (!vc_stack.IsResultExist())
    err.NoRetValue("",1);
  return !err.Count();
}

TreeAnalyzer::iv TreeAnalyzer::NodeHandler(stn* curr)
{
  if (!curr) return iv(IntVar::NOTHING);
  switch (curr->meaning)
    {
    case SemTable::ST_SEQ:
      StatementSeq(curr);
      return iv(IntVar::NOTHING);

    case SemTable::ASSIGN:
      return Assign(curr);

    case SemTable::OR:
    case SemTable::AND:
      return Logical(curr);
      
    case SemTable::CMP:
      return Cmp(curr);
      
    case SemTable::PLUS:    
    case SemTable::MUL:
      return MathBasic(curr);
      
    case SemTable::UN_PLUS:
    case SemTable::UN_MINUS:
    case SemTable::NOT:
      return UnarySimple(curr);

    case SemTable::INC:
    case SemTable::DEC:
    case SemTable::INC_P:
    case SemTable::DEC_P:
      return IncDec(curr);

    case SemTable::NAME:
      return Name(curr);
    case SemTable::NUM:
      return Num(curr);

    default:
      err.Error("Is it possible?"); exit(1);
    }
}

void TreeAnalyzer::StatementSeq(stn* curr)
{
  vc_stack.PushVC();
  
  for (unsigned i=0;i<curr->Size();++i)
    {
      if (stn* tmp=curr->GetOp(i))
	NodeHandler(tmp);
    }
  
  vc_stack.PopVC();
}

TreeAnalyzer::iv TreeAnalyzer::Assign(stn* curr)
{
  iv rhs=NodeHandler(curr->GetOp(2));
  DoubleConv(rhs,curr->Line());

  iv lhs=NodeHandler(curr->GetOp(1));
  if (lhs.type==IntVar::BOOL || lhs.type==IntVar::TMP)
    {
      err.LineError("Lvalue required as left operand of assignment",
		    curr->Line());
      return iv(IntVar::NOTHING);
    }
  if (lhs.type==IntVar::NOTHING)
    return iv(IntVar::NOTHING);

  if (!IsVal(lhs,curr->Line()))
      return iv(IntVar::NOTHING);

  if (lhs.name)
    {
      auto res=vc_stack.AddVar(lhs.name->name,VarCont::DOUBLE,1);
      if (!res.first) 
	res.second->SetNextVersion();

      curr->GetOp(1)->SetVersion(res.second->GetVersion());
    }

  
  if (IsVarFailed(rhs,curr->Line()))
    return iv(IntVar::NOTHING);

  
  return lhs;
}

TreeAnalyzer::iv TreeAnalyzer::Logical(stn* curr)
{
  iv tmp=NodeHandler(curr->GetOp(0));
  if (IsVarFailed(tmp,curr->Line()) || 
      !IsVal(tmp,curr->Line()))
    return iv(IntVar::NOTHING);
  BoolConv(tmp,curr->Line());

  for (unsigned i=1; i<curr->Size(); ++i)
    {
      iv rhs=NodeHandler(curr->GetOp(i));
      if (!IsVarFailed(rhs,curr->Line()) ||
	  !IsVal(rhs,curr->Line()))
	BoolConv(rhs,curr->Line());
    }

  return iv(IntVar::BOOL);
}

TreeAnalyzer::iv TreeAnalyzer::Cmp(stn* curr)
{
  iv tmp=NodeHandler(curr->GetOp(0));
  if (IsVarFailed(tmp,curr->Line()) || 
      !IsVal(tmp,curr->Line()))
    return iv(IntVar::NOTHING);
  DoubleConv(tmp,curr->Line());

  for (unsigned i=1;i<curr->Size();++i)
    {	  
      iv rhs=NodeHandler(curr->GetOp(i));
      if (IsVarFailed(rhs,curr->Line()) ||
	  !IsVal(rhs,curr->Line()))
	return iv(IntVar::NOTHING);
      else 
	DoubleConv(rhs,curr->Line());
    }
  
  return iv(IntVar::BOOL);
}

TreeAnalyzer::iv TreeAnalyzer::MathBasic(stn* curr)
{
  iv tmp=NodeHandler(curr->GetOp(0));
  if (IsVarFailed(tmp,curr->Line()) ||
      !IsVal(tmp,curr->Line()))
    return iv(IntVar::NOTHING);
  DoubleConv(tmp,curr->Line());

  for (unsigned i=1;i<curr->Size();++i)
    {		
      iv rhs=NodeHandler(curr->GetOp(i));
      if (IsVarFailed(rhs,curr->Line()) ||
	  !IsVal(rhs,curr->Line()))
	return iv(IntVar::NOTHING);
      DoubleConv(rhs,curr->Line());
      if (curr->GetType(i-1)==SemTable::DIV && rhs.value==0)
	err.LineError("Division by zero",curr->Line());
    }
  return iv(IntVar::TMP);
}

TreeAnalyzer::iv TreeAnalyzer::UnarySimple(stn* curr)
{
  iv op=NodeHandler(curr->GetOp());
  if (IsVarFailed(op,curr->Line()) ||
      !IsVal(op,curr->Line()))
    return iv(IntVar::NOTHING);

  switch (curr->meaning)
    {
    case SemTable::NOT:
      BoolConv(op,curr->Line());
      return iv(IntVar::BOOL);
    default:
      DoubleConv(op,curr->Line());
      return iv(IntVar::TMP);
    }
}

TreeAnalyzer::iv TreeAnalyzer::IncDec(stn* curr)
{
  iv op=NodeHandler(curr->GetOp());

  if (IsVarFailed(op,curr->Line()) ||
      !IsVal(op,curr->Line()))  
    return iv(IntVar::NOTHING);

  if (op.type==IntVar::BOOL || op.type==IntVar::TMP)
    {
      switch (curr->meaning)
	{
	case SemTable::INC:
	case SemTable::INC_P:
	  err.LineError("Increment requires lvalue as argument",curr->Line());
	  break;
	case SemTable::DEC:
	case SemTable::DEC_P:
	  err.LineError("Decrement requires lvalue as argument",curr->Line());
	  break;
	default:
	  err.Error("Error in IncDec()");
	  exit(1);
	}
      return iv(IntVar::NOTHING);
    }
  
  auto var_to_inc=vc_stack.GetVar(op.name->name);
  var_to_inc.second->SetNextVersion();;

  switch (curr->meaning)
    {
    case SemTable::INC:
    case SemTable::DEC:
      return op;
    case SemTable::INC_P:
    case SemTable::DEC_P:
      return iv(IntVar::TMP);
    default:
      err.Error("Error in IncDec()");
      exit(1);
    }
}

TreeAnalyzer::iv TreeAnalyzer::Name(stn* curr)
{
  VarCont::apair p=vc_stack.GetVar(*curr->GetName());
  IntVar::IntVarType t;
  if (!p.first)
    t=IntVar::VAR;
  else
    {
      curr->SetVersion(p.second->GetVersion());
      switch (p.second->GetType())
	{
	case VarCont::DOUBLE:
	  {
	    t=IntVar::VAR;
	    break;
	  }
	case VarCont::REF:
	  {
	    t=IntVar::REF;
	    break;
	  }
	case VarCont::ARRAY:
	  {
	    t=IntVar::ARR;
	    break;
	  }
	case VarCont::ARR_REF:
	  {
	    t=IntVar::ARR;
	    break;
	  }
	default:
	  err.Error("Unexpected type in Name()");
	  exit(1);
	}
    }

  iv res(t);
  res.size=p.first ? p.second->GetSize() : 1;
  res.name=new iv::Buf(*curr->GetName()); 
  return res;
}

TreeAnalyzer::iv TreeAnalyzer::Num(stn* curr)
{
  iv res(IntVar::TMP);
  res.value=curr->GetVal();
  return res;
}

bool TreeAnalyzer::CheckVar(const iv& var,unsigned line)
{
  if (!var.name)
    return false;
  VarCont::apair p=vc_stack.GetVar(var.name->name);
  if (p.first)
    return true;
  err.VarError("Unknown variable",var.name->name,line);
  return false;
}

bool TreeAnalyzer::IsVarFailed(const iv& var,unsigned line)
{
  return var.type==IntVar::NOTHING || 
    (var.name && !CheckVar(var,line));
}

void TreeAnalyzer::BoolConv(const iv& var,unsigned ln)
{
  if (var.type!=IntVar::BOOL)
    err.BoolWarn(ln);
}

void TreeAnalyzer::DoubleConv(const iv& var,unsigned ln)
{
  if (var.type==IntVar::BOOL)
    err.DoubleWarn(ln);
}

bool TreeAnalyzer::IsVal(const iv& var,unsigned ln)
{
  if (var.type==IntVar::ARR)
    {
      err.LineError("Array name cannot be used as value",ln);
      return false;
    }
  return true;
}
