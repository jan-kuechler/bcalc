#include "stdafx.h"
#include <cmath>
#include <functional>
#include <list>
#include <stdexcept>
#include <vector>
#include "Env.h"

using namespace tr;

class BasicFunc : public Function
{
  double arg;
  std::function<double (double)> func;
public:
  BasicFunc(double (*f)(double))
    : func(f)
  { }

  size_t GetNumArgs() const
  {
    return 1;
  }
 
  void SetArg(size_t index, double value)
  {
    if (index != 0)
      throw std::runtime_error("Tried to set invalid argument!");
    arg = value;
  }
  
  double Execute(const Env& )
  {
    return func(arg);
  }
};

class DynamicFunction : public Function
{
  const std::list<std::wstring> args;
  std::vector<double> argValues;
  std::shared_ptr<Node> body;
public:
  DynamicFunction(const std::list<std::wstring>& a, std::shared_ptr<Node> b)
    : args(a), body(b)
  { 
    argValues.reserve(args.size());
  }

  size_t GetNumArgs() const
  {
    return args.size();
  }

  void SetArg(size_t index, double value)
  {
    argValues[index] = value;
  }

  double Execute(const Env& env)
  {
    Env funcEnv = env;
    size_t n = 0;
    for (auto i = args.begin(); i != args.end(); ++i, ++n) {
      funcEnv.SetVar(*i, argValues[n]);
    }
    return funcEnv.Eval(body);
  }
};

void Env::InitStdVars()
{
  vars[L"pi"] = std::acos(-1.0); 
  vars[L"e"]  = std::exp(1.0); 

  funcs[L"sin"].reset(new BasicFunc(std::sin));
  funcs[L"cos"].reset(new BasicFunc(std::cos));
  funcs[L"tan"].reset(new BasicFunc(std::tan));
  funcs[L"exp"].reset(new BasicFunc(std::exp));
  funcs[L"ln"].reset(new BasicFunc(std::log));
  funcs[L"sqrt"].reset(new BasicFunc(std::sqrt));
}

double Env::GetVar(const std::wstring& sym) const
{
  auto i = vars.find(sym);
  if (i == vars.end())
		throw Diag::UnknownVariable(sym);
  return i->second;
}

void Env::SetVar(const std::wstring& sym, double val)
{
  vars[sym] = val;
}

Function& Env::GetFunc(const std::wstring& sym) const
{
  auto i = funcs.find(sym);
  if (i == funcs.end())
    throw Diag::UnknownFunction(sym);
  return *i->second;
}

void Env::SetFunc(const std::wstring& sym, const std::list<std::wstring>& args, std::shared_ptr<Node> body)
{
  funcs[sym].reset(new DynamicFunction(args, body));
}

