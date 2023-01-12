class StringPtr {
  string str;
}

class Node {}

class Terminal : Node {}
class NonTerminal : Node {}

def Bool : Terminal {
  bit value = false;
}

class Int : Terminal {
  int value = 0;
}

class String : Terminal {
  StringPtr value;
}

class ObjectId : Terminal {
  StringPtr name;
}

class TypeId : Terminal {
  StringPtr typeName;
}

class Expression : NonTerminal {}

class SingeltonExpression : Expression {
  Node term;
}

class BinaryNode : Expression {
    Node left;
    Node right;
}

class PlusNode : BinaryNode {}
class MinusNode : BinaryNode {}
class MultiplyNode : BinaryNode {}
class DivideNode : BinaryNode {}

class Expressions : Expression {
  list<Node> expressions;
}

class Dispatch : Expression {
  Node dispatchObject;
  ObjectId dispatchMethod;
  Expressions arguments;
}

class StaticDispatch : Expression {
  Node dispatchObject;
  TypeId castType;
  ObjectId dispatchMethod;
  Expressions arguments;
}