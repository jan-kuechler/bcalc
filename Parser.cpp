#include "stdafx.h"
#include <cassert>
#include "Token.h"
#include "Parser.h"

using namespace tr;

void Parser::Accept(Token::Type t)
{
  if (token != t)
    throw Diag::InvalidToken(t, token, origInput);
  NextToken();
}

Node *Parser::Parse()
{
  Node *nd = Line();
  Accept(Token::NONE);
  return nd;
}

Node *Parser::Line()
{
  if (token == Token::IDENT) {
    if (lexer.NextToken(true) == Token::EQUAL)
      return Definition();
    else if (lexer.NextToken(true) == Token::COLON)
      return FuncDef();
  }
  return Expression();
}

Node *Parser::Definition()
{
  std::unique_ptr<NodeDefVar> nd(new NodeDefVar(token.GetStringValue()));
  Accept(Token::IDENT);
  Accept(Token::EQUAL);
  nd->SetValue(std::unique_ptr<Node>(Expression()));
  return nd.release();
}

Node *Parser::FuncDef()
{
  std::unique_ptr<NodeDefFunc> nd(new NodeDefFunc(token.GetStringValue()));
  Accept(Token::IDENT);
  Accept(Token::COLON);
  Accept(Token::LPAREN);
  if (token != Token::RPAREN) {
    if (token != Token::IDENT)
      throw Diag::InvalidToken(Token::IDENT, token);
    nd->PushArg(token.GetStringValue());
    Accept(Token::IDENT);
    while (token == Token::COMMA) {
      Accept(Token::COMMA);
      if (token != Token::IDENT)
				throw Diag::InvalidToken(Token::IDENT, token);
      nd->PushArg(token.GetStringValue());
      Accept(Token::IDENT);
    }
  }
  Accept(Token::RPAREN);
  Accept(Token::EQUAL);
  nd->SetDef(std::shared_ptr<Node>(Expression()));
  return nd.release();
}

Node *Parser::Expression()
{
  Node *nd = Mult();

  while (token == Token::ADD || token == Token::SUB) {
    switch (token.GetType()) {
    case Token::ADD:
      NextToken();
      nd = new NodeAdd(nd, Mult());
      break;
    case Token::SUB:
      NextToken();
      nd = new NodeSub(nd, Mult());
      break;
    default:
      assert(false && "Impossible token type");
    }
  }
  return nd;
}

Node *Parser::Mult()
{
  Node *nd = Power();
  
  while (token == Token::MUL || token == Token::DIV) {
    switch (token.GetType()) {
    case Token::MUL:
      NextToken();
      nd = new NodeMul(nd, Power());
      break;
    case Token::DIV:
      NextToken();
      nd = new NodeDiv(nd, Power());
      break;
    default:
      assert(false && "Impossible token type");
    }
  }
  return nd;
}

Node *Parser::Power()
{
  Node *nd = Paren();
  
  if (token == Token::POW) {
    NextToken();
    nd = new NodePow(nd, Paren());
  }
  return nd;
}

Node *Parser::Paren()
{
  double sign = 1;

  if (token == Token::ADD || token == Token::SUB) {
    if (token == Token::SUB)
      sign = -1;
    NextToken();
  }

  if (token == Token::IDENT) {
    if (lexer.NextToken(true) == Token::LPAREN) {
      return Function();
    }
    else {
      Node *nd = new NodeIdent(token.GetStringValue());
      Accept(Token::IDENT);
      return new NodeMul(new NodeNumber(sign), nd);
    }
  }
  else if (token == Token::LPAREN) {
    NextToken();
    Node *nd = Expression();
    Accept(Token::RPAREN);
    return new NodeMul(new NodeNumber(sign), nd);
  }
  
  return new NodeMul(new NodeNumber(sign), Number());
}

Node *Parser::Function()
{
  std::wstring name = token.GetStringValue();
  Accept(Token::IDENT);

  std::unique_ptr<NodeFunc> nd(new NodeFunc(name));

  Accept(Token::LPAREN);
  if (token != Token::RPAREN) {
    nd->PushArg(std::unique_ptr<Node>(Expression()));

    while (token == Token::COMMA) {
      Accept(Token::COMMA);
      nd->PushArg(std::unique_ptr<Node>(Expression()));
    }
  }
  Accept(Token::RPAREN);

  return nd.release();
}

Node *Parser::Number()
{

  Token t = token;
  Accept(Token::NUMBER);
  return new NodeNumber(t.GetNumberValue());
}


