#include "VarCont.h"
#include <string>
#include <map>
#include <utility>
#include <cassert>

using namespace VarCont;

apair VarContainer::Insert(const std::string& name,VarType t,unsigned sz)
{
  auto res=rep.emplace
    (std::make_pair(name,Var(t,sz) ));

  return apair(res.second, &(res.first->second) );
}

Var* VarContainer::Find(const std::string& name)
{
  auto curr=rep.find(name);
  if (curr==rep.end()) return nullptr;
  return &(curr->second);
}

VarContStack::~VarContStack()
{
  VNode* tmp=top;
  while (top!=nullptr)
    {
      top=top->prev;
      delete tmp;
      tmp=top;
    }
  delete result;
}

apair VarContStack::AddVar(const std::string& v_name,VarType t,unsigned size)
{
  if (v_name=="result")
    {
      if (t==DOUBLE)
	{
	  bool is_added=!result;
	  Var* res=result ? result : (result=new Var(DOUBLE,1)) ;
	  return std::make_pair(is_added,res);
	}
      else 
	return std::make_pair(false,nullptr);
    }

  Var* res=Find(v_name);
  if (res)
    return apair(false,res);
  return top->vc.Insert(v_name,t,size);
}

apair 
VarContStack::GetVar(const std::string& v_name) const
{  
  if (v_name=="result")
    // Without explicit cast it looks strange - apair(result,result)
    return apair(static_cast<bool>(result),result);

  Var* res=Find(v_name);
  return apair(static_cast<bool>(res),res);
}

void VarContStack::Reset() 
{
  delete result;
  result=nullptr;
}

Var* VarContStack::Find(const std::string& v_name) const
{
  VNode* curr=top;
  Var* res=nullptr;
  while (curr && !(res=curr->vc.Find(v_name)))
    curr=curr->prev;
  return res;
}
