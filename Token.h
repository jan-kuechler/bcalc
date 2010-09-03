#ifndef TOKEN_H
#define TOKEN_H

#include <cassert>
#include <string>

namespace tr
{

class Token
{
public:
  enum Type
  {
    ADD, SUB, MUL, DIV, POW,

    LPAREN, RPAREN,

    COMMA, EQUAL, COLON,

    IDENT, NUMBER,

    NONE, 

    NUM_TOKEN_TYPES
  };

private:
  Type type;
  
  double num;
  std::wstring str;

  size_t col;

  static std::wstring typeNames[NUM_TOKEN_TYPES];
  static std::wstring typeDescs[NUM_TOKEN_TYPES];

public:
 Token(Type t = NONE, size_t c = 0)
   : type(t), col(c)
  { 
    assert(t < NUM_TOKEN_TYPES);
  }

 Token(double n, size_t c = 0)
   : type(NUMBER), num(n), col(c)
  { }

 Token(const std::wstring& s, size_t c = 0)
   : type(IDENT), str(s), col(c)
  { }

  Type GetType() const
  {
    return type;
  }

  std::wstring GetStringValue() const
  {
    assert(type == IDENT);
    return str;
  }

  double GetNumberValue() const
  {
    assert(type == NUMBER);
    return num;
  }

  std::wstring GetName() const
  {
    return typeNames[type];
  }

  std::wstring GetDesc() const
  {
    return typeDescs[type];
  }

  size_t GetPos() const
  {
    return col;
  }
};

inline bool operator==(const Token& tok, const Token::Type& type)
{
  return tok.GetType() == type;
}

inline bool operator!=(const Token& tok, const Token::Type& type)
{
  return !(tok == type);
}

};

#endif //TOKEN_H
