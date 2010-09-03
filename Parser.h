#ifndef PARSER_H
#define PARSER_H

#include <stdexcept>
#include "AST.h"
#include "Diag.h"
#include "Lexer.h"
#include "Token.h"

namespace tr
{

class Parser
{
  Lexer lexer;
  Token token;
public:
  Parser(const std::wstring& input)
    : lexer(input)
  {
    NextToken();
  }

  Node* Parse();

private:
  void Accept(Token::Type type);

  void NextToken()
  {
    token = lexer.NextToken();
  }

  Node* Line();
  Node* Definition();
  Node* FuncDef();
  Node* Expression();
  Node* Mult();
  Node* Power();
  Node* Paren();
  Node* Function();
  Node* Identifier();
  Node* Number();
};

};

#endif //PARSER_H
