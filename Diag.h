#ifndef DIAG_H
#define DIAG_H

#include <iostream>
#include "Token.h"

namespace tr
{

class Diagnostics
{
public:
  virtual std::wstring Describe() const
	{
		return L"<Unkown error>";
	}
};

namespace Diag
{
  class InvalidToken : public Diagnostics
  {
    Token exp, got;
  public:
    InvalidToken(Token e, Token g)
      : exp(e), got(g)
    { }

		std::wstring Describe() const;
  };

  class UnknownVariable : public Diagnostics
  {
    std::wstring name;
  public:
    UnknownVariable(const std::wstring& n)
      : name(n)
    { }

		std::wstring Describe() const;
  };

  class UnknownFunction : public Diagnostics
  {
    std::wstring name;
  public:
    UnknownFunction(const std::wstring& n)
      : name(n)
    { }

		std::wstring Describe() const;
  };
};

};

inline std::wostream& operator<<(std::wostream& os, const tr::Diagnostics& di)
{
  return os << di.Describe();
}

#endif //DIAG_H
