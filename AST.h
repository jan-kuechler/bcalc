#ifndef AST_H
#define AST_H

#include <cmath>
#include <list>

#include "Node.h"
#include "Env.h"

#include <boost/lexical_cast.hpp>

namespace tr
{

class NodeNumber : public Node
{
  double num;
public:
  explicit NodeNumber(double val)
    : num (val)
  { }

  double Eval(Env&) 
	{
    return num;
  }

	std::wstring Dump() const
	{
		return boost::lexical_cast<std::wstring>(num);
	}
};

#define CONCAT_(A_, B_) A_##B_
#define CONCAT(A_, B_) CONCAT_(A_, B_)

#define DECLARE_BINARY_NODE(Name_, Op_)						\
	class Name_ : public Node												\
		{																							\
			std::unique_ptr<Node> left, right;					\
		public:																				\
		Name_(Node *l, Node *r)												\
			: left(l), right(r)													\
			{ }																					\
																									\
			double Eval(Env& e)													\
			{																						\
				return left->Eval(e) Op_ right->Eval(e);	\
			}																						\
																									\
			std::wstring Dump() const										\
			{																		  			\
				std::wstring res = left->Dump();					\
				res += L" " CONCAT(L, #Op_) L" ";					\
				res += right->Dump();											\
				return res;																\
			}																						\
		}

DECLARE_BINARY_NODE(NodeAdd, +);
DECLARE_BINARY_NODE(NodeSub, -);
DECLARE_BINARY_NODE(NodeMul, *);
DECLARE_BINARY_NODE(NodeDiv, /);

#undef DECLARE_BINARY_NODE
#undef CONCAT
#undef CONCAT_

class NodePow : public Node
{
  std::unique_ptr<Node> base, exp;
public:
  NodePow(Node *b, Node *e)
    : base(b), exp(e)
  { }

  double Eval(Env& e) 
  {
    return std::pow(base->Eval(e), exp->Eval(e));
  }

	std::wstring Dump() const
	{
		return base->Dump() + L"^" + exp->Dump();
	}
};

class NodeIdent : public Node
{
  std::wstring name;
public:
  NodeIdent(const std::wstring& n)
    : name(n)
  { }

  double Eval(Env& e)
  {
    return e.GetVar(name);
  }

	std::wstring Dump() const
	{
		return name;
	}
};

class NodeFunc : public Node
{
  std::wstring name;
  std::list<std::unique_ptr<Node>> args;

public:
  NodeFunc(const std::wstring& n)
    : name(n)
  { }

  void PushArg(std::unique_ptr<Node> arg)
  {
    args.push_back(std::move(arg));
  }

  double Eval(Env& e)
  {
    Function& func = e.GetFunc(name);
    if (func.GetNumArgs() != args.size())
      throw std::runtime_error("Incorrect number of arguments");

    size_t n = 0;
    for (auto i = args.begin(); i != args.end(); ++i, ++n) {
      func.SetArg(n, (*i)->Eval(e));
    }

    return func.Execute(e);
  }

	std::wstring Dump() const
	{
		std::wstring res = name + L"(";

		auto i = args.begin();
		while (i != args.end()) {
			res += (*i)->Dump();
			i++;
			if (i != args.end())
				res += L", ";
		}
		res += L")";
		return res;
	}
};

class NodeDefVar : public Node
{
  std::wstring name;
  std::unique_ptr<Node> val;
public:
  NodeDefVar(const std::wstring& n)
    : name(n)
  { }

  void SetValue(std::unique_ptr<Node> v)
  {
    val = std::move(v);
  }
  
  double Eval(Env& e)
  {
    double v = val->Eval(e);
    e.SetVar(name, v);
    return v;
  }

	std::wstring Dump() const
	{
		return name + L" = " + val->Dump();
	}
};

class NodeDefFunc : public Node
{
  std::wstring name;
  std::list<std::wstring> args;
  std::shared_ptr<Node> body;
public:
  NodeDefFunc(const std::wstring& n)
    : name(n)
  { }

  void PushArg(const std::wstring& arg)
  {
    args.push_back(arg);
  }

  void SetDef(std::shared_ptr<Node> def)
  {
    body = def;
  }

  double Eval(Env& e)
  {
    e.SetFunc(name, args, body);
    return 0.0;
  }

	std::wstring Dump() const
	{
		std::wstring res = name;
		res += L": (";

		auto i = args.begin();
		while (i != args.end()) {
			res += *i;
			i++;
			if (i != args.end())
				res += L", ";
		}
		res += L") = ";
		res += body->Dump();
		return res;
 	}
};

};

#endif //AST_H
