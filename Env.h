#ifndef ENV_H
#define ENV_H

#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

#include "Diag.h"
#include "Node.h"

namespace tr
{

class Env;

class Function 
{
public:
  virtual size_t GetNumArgs() const = 0;
  virtual void SetArg(size_t index, double value) = 0;
  
  virtual double Execute(const Env& env) = 0;
};

class Env
{
private:
  std::map<std::wstring, double> vars;
  std::map<std::wstring, std::shared_ptr<Function>> funcs;
public:
  struct env_error : std::runtime_error
  {
    env_error(const Diagnostics& d)
      : std::runtime_error("env error"), diag(d)
    { }

    const Diagnostics& Diag() const
    {
      //std::wcerr << "Diag()";
      return diag;
    }
  private:
    const Diagnostics& diag;
  };

  Env()
  {
    InitStdVars();
  }

  double Eval(std::shared_ptr<Node> ast)
  {
    return ast->Eval(*this);
  }

  double GetVar(const std::wstring& sym) const;
  void SetVar(const std::wstring& sym, double val);

  Function& GetFunc(const std::wstring& sym) const;
  void SetFunc(const std::wstring& sym, const std::list<std::wstring>& args, std::shared_ptr<Node> body);

	void DumpFuncs(std::ostream& out);
	void ReadFuncs(std::istream& in);

private:
  void InitStdVars();
};

};

#endif //ENV_H
