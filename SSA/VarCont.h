#ifndef __CALC_VAR_CONTAINER_H__
#define __CALC_VAR_CONTAINER_H__

#include <string>
#include <map>
#include <cassert>

namespace VarCont
{

enum VarType {DOUBLE,REF,ARRAY,ARR_REF};

class Var
{
  VarType type;
  unsigned size;
  unsigned version;
public:
  Var(VarType t,unsigned sz):
    type(t),size(sz),version(0){};

  VarType GetType() const {return type;}
  unsigned GetSize() const {return size;}
  unsigned GetVersion() const {return version;}
  
  void SetNextVersion() {++version;}

  ~Var(){}
};


typedef std::pair<bool,Var*> apair;

class VarContainer
{
  std::map<std::string,Var> rep;
public:
  VarContainer(){}
  ~VarContainer(){}
  apair Insert(const std::string&,VarType,unsigned sz);
  Var* Find(const std::string&);
};

class VarContStack
{
  struct VNode
  {
    VNode* prev;
    VarContainer vc;
    VNode(VNode* pr):
      prev(pr),vc(){}
  };

  VNode* top;
  Var* result;

public:

  VarContStack():
    top(nullptr),result(nullptr){}

  VarContStack(const VarContStack&) = delete;
  VarContStack& operator=(const VarContStack&) = delete;
  ~VarContStack();
  
  void PushVC(){top=new VNode(top);}
  void PopVC()
  {
    VNode* tmp=top;
    assert(top);
    top=top->prev;
    delete tmp;
  }

  apair AddVar(const std::string& name,VarType,unsigned size);
  apair GetVar(const std::string&) const;

  bool IsResultExist() const {return result;}

  void Reset();
private:
  Var* Find(const std::string&) const;
};

} // namespace VarCont

#endif
