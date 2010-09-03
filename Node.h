#ifndef NODE_H
#define NODE_H

#include <string>

namespace tr
{

class Env;

class Node
{
public:
  ~Node()
  { }

  virtual double Eval(Env& e) = 0;
	virtual std::wstring Dump() const = 0;
};

};

#endif //NODE_H
