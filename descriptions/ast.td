class Node {}

class Terminal : Node {}
class NonTerminal : Node {}

def Bool : Terminal {
  bit value = false;
}

def Int : Terminal {
  int value = 0;
}

def String : Terminal {
  string value = "unknown";
}

def ObjectId : Terminal {
  string str = "unknown";
}

class Expression : NonTerminal {}

def SingeltonExpression : Expression {
  Node term;
}

class BinaryNode : Expression {
    Node left;
    Node right;
}

def PlusNode : BinaryNode {}
def MinusNode : BinaryNode {}
def MultiplyNode : BinaryNode {}
def DivideNode : BinaryNode {}

def Expressions : Expression {
  list<Node> expressions;
}