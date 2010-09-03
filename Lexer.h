#ifndef LEXER_H
#define LEXER_H

#include <cstddef>
#include <string>
#include <vector>
#include "Token.h"

namespace tr
{
class Lexer
{
  std::wstring input;
  std::size_t inPos, tokPos;
  wchar_t cur;
  
  std::vector<Token> tokens;
public:
  Lexer(const std::wstring& inp);

  Token NextToken(bool peek = false);

private:
  Token ReadNextToken();
  void ReadTokens();

  Token ReadNumber();
  Token ReadIdent();

  void SkipSpaces();
  void NextChar();
};
};

#endif //LEXER_H
