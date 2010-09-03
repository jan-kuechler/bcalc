#include "stdafx.h"
#include "Token.h"

using namespace tr;

std::wstring Token::typeNames[Token::NUM_TOKEN_TYPES] = {
  L"ADD", L"SUB", L"MUL", L"DIV", L"POW",
  L"LPAREN", L"RPAREN", L"COMMA", L"EQUAL", L"COLON",
  L"IDENT", L"NUMBER", L"NONE",
};

std::wstring Token::typeDescs[Token::NUM_TOKEN_TYPES] = {
  L"'+'", L"'-'", L"'*'", L"'/'", L"'^'",
  L"'('", L"')'", L"','", L"'='", L"':'",
  L"identifier", L"number", L"nothing",
};
