#include "stdafx.h"
#include <cwctype>
#include <sstream>
#include "Lexer.h"

using namespace tr;

Lexer::Lexer(const std::wstring& inp)
  : input(inp), inPos(0), tokPos(0), cur(L'\0')
{
  NextChar();
  ReadTokens();
}

Token Lexer::NextToken(bool peek)
{ 
  if (tokPos >= tokens.size())
    return Token(Token::NONE, inPos + 1);
  return tokens[peek ? tokPos : tokPos++];
}

Token Lexer::ReadNextToken()
{
  SkipSpaces();
  size_t pos = inPos;
  switch (cur) {
  case L'+': NextChar(); return Token(Token::ADD, pos);
  case L'-': NextChar(); return Token(Token::SUB, pos);
  case L'*': NextChar(); return Token(Token::MUL, pos);
  case L'/': NextChar(); return Token(Token::DIV, pos);
  case L'^': NextChar(); return Token(Token::POW, pos);
  case L'(': NextChar(); return Token(Token::LPAREN, pos);
  case L')': NextChar(); return Token(Token::RPAREN, pos);
  case L',': NextChar(); return Token(Token::COMMA, pos);
  case L'=': NextChar(); return Token(Token::EQUAL, pos);
  case L':': NextChar(); return Token(Token::COLON, pos);

  default:
    if (std::iswdigit(cur) || cur == L'.')
      return ReadNumber();
    if (std::iswalpha(cur) || cur == L'_' || cur == L'$')
      return ReadIdent();
  }
  return Token(Token::NONE, pos);
}

Token Lexer::ReadNumber()
{
  std::wstring buf;
  size_t pos = inPos;
  while (std::iswdigit(cur)) {
    buf += cur;
    NextChar();
  }
  if (cur == L'.') {
    buf += cur;
    NextChar();
    
    while (std::iswdigit(cur)) {
      buf += cur;
      NextChar();
    }
  }

  std::wstringstream s;
  s << buf;
  double val;
  s >> val;
  return Token(val, pos);
}

Token Lexer::ReadIdent()
{
  std::wstring buf;
  size_t pos = inPos;
  do {
    buf += cur;
    NextChar();
  } while (std::iswalpha(cur) || std::iswdigit(cur)|| cur == L'_');
  return Token(buf, pos);
}

void Lexer::ReadTokens()
{
  Token t = ReadNextToken();
  while (t != Token::NONE) {
    tokens.push_back(t);
    t = ReadNextToken();
  }
}

void Lexer::SkipSpaces()
{
  while (std::iswspace(cur))
    NextChar();
}

void Lexer::NextChar()
{
  if (inPos >= input.length())
    cur = L'\0';
  else
    cur = input[inPos++];
}

