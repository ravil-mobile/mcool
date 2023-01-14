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
  StringPtr name;
}

class Expression : NonTerminal {}

class UnaryExpression : Expression {
  Node term;
}

class BinaryExpression : Expression {
    Expression left;
    Expression right;
}

class PrimaryExpression : UnaryExpression {}
class NegationNode : UnaryExpression {}
class IsVoidNode : UnaryExpression {}


class PlusNode : BinaryExpression {}
class MinusNode : BinaryExpression {}
class MultiplyNode : BinaryExpression {}
class DivideNode : BinaryExpression {}

class LessEqualNode : BinaryExpression {}
class LessNode : BinaryExpression {}
class EqualNode : BinaryExpression {}

class Expressions : Expression {
  list<Node> data;
}

class Dispatch : Expression {
  Node objectId;
  ObjectId methodId;
  Expressions arguments;
}

class StaticDispatch : Expression {
  Node objectId;
  TypeId castType;
  ObjectId methodId;
  Expressions arguments;
}

class WhileLoop : Expression {
  Expression predicate;
  Expression body;
}

class NewExpr : Expression {
  TypeId newType;
}

class AssignExpr : Expression {
  ObjectId id;
  Expression initExpr;
}

class IfThenExpr : Expression {
  Expression condition;
  Expression thenBody;
}

class IfThenElseExpr : Expression {
  Expression condition;
  Expression thenBody;
  Expression elseBody;
}

class NotExpr : Expression {
  Expression expr;
}

class BlockExpr : Expression {
  Expressions exprs;
}

class NoExpr : Expression {}

class LetExpr : Expression {
  ObjectId id;
  TypeId idType;
  Expression initExpr;
  Expression body;
}

class SingleCase : NonTerminal {
  ObjectId id;
  TypeId idType;
  Expression body;
}

class CaseList : NonTerminal {
  list<SingleCase> data;
}

class CaseExpr : Expression {
  Expression expr;
  CaseList casses;
}

class SingleFormal : NonTerminal {
  ObjectId id;
  TypeId idType;
}

class FormalList : NonTerminal {
  list<SingleFormal> formals;
}

class ClassAttribute : NonTerminal {}

class SingleMemeber : ClassAttribute {
  ObjectId id;
  TypeId idType;
  Expression initExpr;
}

class SingleMethod : ClassAttribute {
  ObjectId id;
  FormalList parameters;
  TypeId returnType;
  Expression body;
}

class AttributeList : NonTerminal {
  list<ClassAttribute> data;
}

class CoolClass : NonTerminal {
  TypeId coolType;
  TypeId parentType;
  AttributeList attributes;
  String fileName;
}

class CoolClassList : NonTerminal {
  list<CoolClass> data;
}
