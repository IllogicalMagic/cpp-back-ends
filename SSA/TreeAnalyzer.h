#ifndef __TREE_ANALYZER_H__
#define __TREE_ANALYZER_H__

#include "Table.h"
#include "SyntaxTree.h"
#include "VarCont.h"
#include "Error.h"
#include <string>

namespace TreeAnalysis
{

class TreeAnalyzer
{
  // Internal representation of function result
  struct IntVar
  {
    enum IntVarType {TMP,VAR,REF,ARR,BOOL,NOTHING};
    struct Buf
    {
      std::string name;
      unsigned users;
      Buf(const std::string nm):
	name(nm),users(1){}
    };
    IntVarType type;
    Buf* name;
    double value;
    unsigned size;
    
    IntVar(IntVarType tp):
      type(tp),name(nullptr),value(1.0),size(1){}

    IntVar(const IntVar& rhs):
      type(rhs.type),name(rhs.name),
      value(rhs.value),size(rhs.size)
    {
      if (name)
	++name->users;;
    }
    
    ~IntVar()
    {
      if (name && --name->users==0)
	delete name;
    }
    IntVar& operator=(const IntVar&) = delete;
  }; // struct IntVar
  
  typedef ParseTree::ISyntaxTreeNode stn;
  typedef IntVar iv;
  
  stn* mn;
  VarCont::VarContStack vc_stack;
  IntrError::ErrHandler& err;

public:
  TreeAnalyzer(stn* mn_body,IntrError::ErrHandler& r_err):
    mn(mn_body),vc_stack(),err(r_err){}
  ~TreeAnalyzer()
  {
    delete mn;
  }

  bool MakeSSA();
  stn* GetMain() const {return mn;}

private:

  iv NodeHandler(stn*);

  void StatementSeq(stn*);

  iv Assign(stn*);
  iv Logical(stn*);
  iv Cmp(stn*);			
  iv MathBasic(stn*);		// +,-,*,/		
  iv UnarySimple(stn*);		// -,+,!
  iv IncDec(stn*);
  iv Name(stn*);
  iv Num(stn*);

  bool CheckVar(const iv&,unsigned);
  bool IsVarFailed(const iv&,unsigned);
  bool IsVal(const iv&,unsigned);

  void BoolConv(const iv&,unsigned);
  void DoubleConv(const iv&,unsigned);

}; // class TreeAnalyzer

} // namespace TreeAnalysis

#endif
