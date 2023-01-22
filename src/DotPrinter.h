#pragma once

#include "visitor.h"
#include "ast.h"
#include "Misc.h"
#include <vector>
#include <stack>
#include <sstream>
#include <cassert>


namespace mcool {
class DotPrinter : public ast::Visitor {
public:
  DotPrinter(std::ostream& stream) : OS(stream) {}

  void visitCoolClassList(ast::CoolClassList* classList) override {
    auto& list = classList->getData();

    OS << "digraph " << "Program" << " {\n";
    for (auto* coolClass : list) {
      coolClass->accept(this);
      ++classCounter;
    }
    OS << "}\n";
  }

  void visitCoolClass(ast::CoolClass* coolClass) override {
    auto& className = coolClass->getCoolType()->getNameAsStr();
    OS << "subgraph " << className << " {\n";

    nodeCounter = 0;
    nodeLabelsStream = std::stringstream();
    edgeStream = std::stringstream();
    nodeStack = std::stack<std::string>();

    auto classNodeName = buildNodeName();
    addNodeLabel(classNodeName, std::string("class:") + className);

    nodeStack.push(classNodeName);
    coolClass->getAttributes()->accept(this);

    pop(nodeStack);
    assert(nodeStack.empty() && "some node left on the stack");

    OS << nodeLabelsStream.str();
    OS << edgeStream.str();
    OS << "}\n";
  }

  void visitAttributeList(ast::AttributeList* attributeList) override {
    const auto& classNode = nodeStack.top();
    auto& attributes = attributeList->getData();
    for (auto* attr : attributes) {
      attr->accept(this);
      auto attrNode = pop(nodeStack);
      edgeStream << "  " << classNode << " -> " << attrNode << "\n";
    }
  }

  void visitSingleMemeber(ast::SingleMemeber* member) override {
    std::string memberName = buildNodeName();
    addNodeLabel(memberName, "member");
    nodeStack.push(memberName);

    member->getId()->accept(this);
    auto idNode = pop(nodeStack);
    edgeStream << "  " << memberName << " -> " << idNode << "\n";

    member->getIdType()->accept(this);
    auto typeNode = pop(nodeStack);
    edgeStream << "  " << memberName << " -> " << typeNode << "\n";

    member->getInitExpr()->accept(this);
    auto initExprNode = pop(nodeStack);
    edgeStream << "  " << memberName << " -> " << initExprNode << "\n";
  }

  void visitSingleMethod(ast::SingleMethod* method) override {
    auto& methodName = method->getId()->getNameAsStr();
    std::string methodNodeName = buildNodeName();
    addNodeLabel(methodNodeName, std::string("method:") + methodName);
    nodeStack.push(methodNodeName);

    method->getReturnType()->accept(this);
    auto returnNode = pop(nodeStack);
    edgeStream << "  " << methodNodeName << " -> " << returnNode << "\n";

    method->getParameters()->accept(this);
    auto paramsNode = pop(nodeStack);
    edgeStream << "  " << methodNodeName << " -> " << paramsNode << "\n";

    method->getBody()->accept(this);
    auto bodyNode = pop(nodeStack);
    edgeStream << "  " << methodNodeName << " -> " << bodyNode << "\n";
  }


  void visitFormalList(ast::FormalList* formalList) override {
    std::string formalListNode = buildNodeName();
    addNodeLabel(formalListNode, "formal-list");
    nodeStack.push(formalListNode);

    auto& formals = formalList->getFormals();
    for (auto* formal : formals) {
      formal->accept(this);
      auto formalNode = pop(nodeStack);
      edgeStream << "  " << formalListNode << " -> " << formalNode << "\n";
    }
  }

  void visitSingleFormal(ast::SingleFormal* formal) override {
    std::string formalNode = buildNodeName();
    addNodeLabel(formalNode, "formal");
    nodeStack.push(formalNode);

    formal->getId()->accept(this);
    auto idNode = pop(nodeStack);
    edgeStream << "  " << formalNode << " -> " << idNode << "\n";

    formal->getIdType()->accept(this);
    auto typeNode = pop(nodeStack);
    edgeStream << "  " << formalNode << " -> " << typeNode << "\n";
  }

  void visitBlockExpr(ast::BlockExpr* block) override {
    block->getExprs()->accept(this);
  }

  void visitExpressions(ast::Expressions* exprs) override {
    std::string exprsName = buildNodeName();
    addNodeLabel(exprsName, "expr-list");
    nodeStack.push(exprsName);

    auto& list = exprs->getData();
    for (auto* expr : list) {
      expr->accept(this);
      auto childNode = pop(nodeStack);
      edgeStream << "  " << exprsName << " -> " << childNode << "\n";
    }
  }

  void visitWhileLoop(ast::WhileLoop* loop) override {
    std::string loopName = buildNodeName();
    addNodeLabel(loopName, "loop");
    nodeStack.push(loopName);

    loop->getPredicate()->accept(this);
    auto predicateNode = pop(nodeStack);
    edgeStream << "  " << loopName << " -> " << predicateNode << "\n";

    loop->getBody()->accept(this);
    auto bodyNode = pop(nodeStack);
    edgeStream << "  " << loopName << " -> " << bodyNode << "\n";
  }

  void visitNegationNode(ast::NegationNode* node) override {
    std::string negName = buildNodeName();
    addNodeLabel(negName, "neg");
    nodeStack.push(negName);

    node->getTerm()->accept(this);
    auto termNode = pop(nodeStack);
    edgeStream << "  " << negName << " -> " << termNode << '\n';
  }

  void visitPrimaryExpression(ast::PrimaryExpression* node) override {
    node->getTerm()->accept(this);
  }

  void visitIsVoidNode(ast::IsVoidNode* node) override {
    std::string isvoidName = buildNodeName();
    addNodeLabel(isvoidName, "isvoid");
    nodeStack.push(isvoidName);

    node->getTerm()->accept(this);
    auto termNode = pop(nodeStack);
    edgeStream << "  " << isvoidName << " -> " << termNode << '\n';
  }

  void visitNotExpr(ast::NotExpr* expr) override {
    std::string notEpxrName = buildNodeName();
    addNodeLabel(notEpxrName, "not");
    nodeStack.push(notEpxrName);

    expr->getExpr()->accept(this);
    auto exprNode = pop(nodeStack);
    edgeStream << "  " << notEpxrName << " -> " << exprNode << '\n';
  }

  void visitDispatch(ast::Dispatch* dispatch) override {
    std::string dispatchName = buildNodeName();
    addNodeLabel(dispatchName, "call");
    nodeStack.push(dispatchName);

    dispatch->getObjectId()->accept(this);
    auto objectName = pop(nodeStack);
    edgeStream << "  " << dispatchName << " -> " << objectName << '\n';

    dispatch->getMethodId()->accept(this);
    auto methodName = pop(nodeStack);
    edgeStream << "  " << dispatchName << " -> " << methodName << '\n';

    dispatch->getArguments()->accept(this);
    auto argsName = pop(nodeStack);
    edgeStream << "  " << dispatchName << " -> " << argsName << '\n';
  }


  void visitStaticDispatch(ast::StaticDispatch* dispatch) override {
    std::string dispatchName = buildNodeName();
    addNodeLabel(dispatchName, "call");
    nodeStack.push(dispatchName);

    dispatch->getObjectId()->accept(this);
    auto objectName = pop(nodeStack);
    edgeStream << "  " << dispatchName << " -> " << objectName << '\n';

    dispatch->getCastType()->accept(this);
    auto castTypeName = pop(nodeStack);
    edgeStream << "  " << dispatchName << " -> " << castTypeName << '\n';

    dispatch->getMethodId()->accept(this);
    auto methodName = pop(nodeStack);
    edgeStream << "  " << dispatchName << " -> " << methodName << '\n';

    dispatch->getArguments()->accept(this);
    auto argsName = pop(nodeStack);
    edgeStream << "  " << dispatchName << " -> " << argsName << '\n';
  }

  void visitNewExpr(ast::NewExpr* expr) override {
    std::string newExprName = buildNodeName();
    addNodeLabel(newExprName, "new");
    nodeStack.push(newExprName);

    expr->getNewType()->accept(this);
    auto typeNode = pop(nodeStack);
    edgeStream << "  " << newExprName << " -> " << typeNode << '\n';
  }

  void visitCaseExpr(ast::CaseExpr* caseExpr) override {
    std::string caseExprName = buildNodeName();
    addNodeLabel(caseExprName, "case");
    nodeStack.push(caseExprName);

    caseExpr->getExpr()->accept(this);
    auto conditionNode = pop(nodeStack);
    edgeStream << "  " << caseExprName << " -> " << conditionNode << '\n';

    caseExpr->getCasses()->accept(this);
    auto casesNode = pop(nodeStack);
    edgeStream << "  " << caseExprName << " -> " << casesNode << '\n';
  }

  void visitCaseList(ast::CaseList* caseList) override {
    std::string caseListName = buildNodeName();
    addNodeLabel(caseListName, "case-list");
    nodeStack.push(caseListName);

    auto& cases = caseList->getData();
    for (auto aCase : cases) {
      aCase->accept(this);
      auto caseNode = pop(nodeStack);
      edgeStream << "  " << caseListName << " -> " << caseNode << '\n';
    }
  }

  void visitSingleCase(ast::SingleCase* aCase) override {
    std::string caseName = buildNodeName();
    addNodeLabel(caseName, "case");
    nodeStack.push(caseName);

    aCase->getId()->accept(this);
    auto idNode = pop(nodeStack);
    edgeStream << "  " << caseName << " -> " << idNode << '\n';

    aCase->getIdType()->accept(this);
    auto typeNode = pop(nodeStack);
    edgeStream << "  " << caseName << " -> " << typeNode << '\n';

    aCase->getBody()->accept(this);
    auto bodyNode = pop(nodeStack);
    edgeStream << "  " << caseName << " -> " << bodyNode << '\n';
  }

  void visitPlusNode(ast::PlusNode* node) override {
    std::string plusNodeName = buildNodeName();
    addNodeLabel(plusNodeName, "+");
    visitBinaryNode(plusNodeName, node);
  }

  void visitMinusNode(ast::MinusNode* node) override {
    std::string minusNodeName = buildNodeName();
    addNodeLabel(minusNodeName, "-");
    visitBinaryNode(minusNodeName, node);
  }

  void visitMultiplyNode(ast::MultiplyNode* node) override {
    std::string multNodeName = buildNodeName();
    addNodeLabel(multNodeName, "*");
    visitBinaryNode(multNodeName, node);
  }

  void visitDivideNode(ast::DivideNode* node) override {
    std::string divNodeName = buildNodeName();
    addNodeLabel(divNodeName, "/");
    visitBinaryNode(divNodeName, node);
  }

  void visitLessNode(ast::LessNode* node) override {
    std::string lessNodeName = buildNodeName();
    addNodeLabel(lessNodeName, "<");
    visitBinaryNode(lessNodeName, node);
  }

  void visitLessEqualNode(ast::LessEqualNode* node) override {
    std::string leqNodeName = buildNodeName();
    addNodeLabel(leqNodeName, "<=");
    visitBinaryNode(leqNodeName, node);
  }

  void visitEqualNode(ast::EqualNode* node) override {
    std::string equalNodeName = buildNodeName();
    addNodeLabel(equalNodeName, "=");
    visitBinaryNode(equalNodeName, node);
  }

  void visitBinaryNode(const std::string& parentNode, ast::BinaryExpression* node) {
    nodeStack.push(parentNode);

    node->getLeft()->accept(this);
    node->getRight()->accept(this);

    auto rightNode = pop(nodeStack);
    edgeStream << "  " << parentNode << " -> " << rightNode << "\n";

    auto leftNode = pop(nodeStack);
    edgeStream << "  " << parentNode << " -> " << leftNode << "\n";
  }

  void visitAssignExpr(ast::AssignExpr* node) override {
    auto assignName = buildNodeName();
    addNodeLabel(assignName, "assign");
    nodeStack.push(assignName);

    node->getId()->accept(this);
    auto lhs = pop(nodeStack);
    edgeStream << "  " << assignName << " -> " << lhs << '\n';

    node->getInitExpr()->accept(this);
    auto rhs = pop(nodeStack);
    edgeStream << "  " << assignName << " -> " << rhs << '\n';
  }

  void visitIfThenExpr(ast::IfThenExpr* condExpr) override {
    auto ifExprName = buildNodeName();
    addNodeLabel(ifExprName, "if-then");
    nodeStack.push(ifExprName);

    condExpr->getCondition()->accept(this);
    auto condition = pop(nodeStack);
    edgeStream << "  " << ifExprName << " -> " << condition << '\n';

    condExpr->getThenBody()->accept(this);
    auto thenBody = pop(nodeStack);
    edgeStream << "  " << ifExprName << " -> " << thenBody << '\n';
  }

  void visitIfThenElseExpr(ast::IfThenElseExpr* condExpr) override {
    auto ifExprName = buildNodeName();
    addNodeLabel(ifExprName, "if-then-else");
    nodeStack.push(ifExprName);

    condExpr->getCondition()->accept(this);
    auto condition = pop(nodeStack);
    edgeStream << "  " << ifExprName << " -> " << condition << '\n';

    condExpr->getThenBody()->accept(this);
    auto thenBody = pop(nodeStack);
    edgeStream << "  " << ifExprName << " -> " << thenBody << '\n';

    condExpr->getElseBody()->accept(this);
    auto elseBody = pop(nodeStack);
    edgeStream << "  " << ifExprName << " -> " << elseBody << '\n';
  }

  void visitNoExpr(ast::NoExpr*) override {
    auto noExprName = buildNodeName();
    addNodeLabel(noExprName, "no expr");
    nodeStack.push(noExprName);
  }

  void visitLetExpr(ast::LetExpr* letExpr) override {
    auto letExprName = buildNodeName();
    addNodeLabel(letExprName, "let-expr");
    nodeStack.push(letExprName);

    letExpr->getId()->accept(this);
    auto idName = pop(nodeStack);
    edgeStream << "  " << letExprName << " -> " << idName << '\n';

    letExpr->getIdType()->accept(this);
    auto typeName = pop(nodeStack);
    edgeStream << "  " << letExprName << " -> " << typeName << '\n';

    letExpr->getInitExpr()->accept(this);
    auto initName = pop(nodeStack);
    edgeStream << "  " << letExprName << " -> " << initName << '\n';

    letExpr->getBody()->accept(this);
    auto bodyName = pop(nodeStack);
    edgeStream << "  " << letExprName << " -> " << bodyName << '\n';
  }

  void visitTypeId(ast::TypeId* type) override {
    auto nodeName = buildNodeName();
    addNodeLabel(nodeName, std::string("type:") + type->getNameAsStr());
    nodeStack.push(nodeName);
  }

  void visitObjectId(ast::ObjectId* id) override {
    auto nodeName = buildNodeName();
    addNodeLabel(nodeName, std::string("id:") + id->getNameAsStr());
    nodeStack.push(nodeName);
  }

  void visitString(ast::String*) override {
    auto nodeName = buildNodeName();
    addNodeLabel(nodeName, "str");
    nodeStack.push(nodeName);
  }

  void visitBool(ast::Bool* node) override {
    auto nodeName = buildNodeName();
    auto value = node->getValue();
    addNodeLabel(nodeName, value ? "true" : "false");
    nodeStack.push(nodeName);
  }

  void visitInt(ast::Int* node) override {
    auto nodeName = buildNodeName();
    addNodeLabel(nodeName, std::to_string(node->getValue()));
    nodeStack.push(nodeName);
  }

private:
  std::string buildNodeName() {
    std::string prefix = "c" + std::to_string(classCounter);
    std::string nodeName = prefix + "node" + std::to_string(nodeCounter);
    ++nodeCounter;
    return nodeName;
  }

  void addNodeLabel(std::string& nodeName, std::string&& name) {
    nodeLabelsStream << "  " << nodeName << " [label=\"" << name << "\"];\n";
  }

  std::string pop(std::stack<std::string>& stack) {
    auto& top = stack.top();
    stack.pop();
    return top;
  }

  std::ostream& OS;
  std::stack<std::string> nodeStack{};
  std::stringstream nodeLabelsStream;
  std::stringstream edgeStream;
  size_t classCounter{0};
  size_t nodeCounter{0};
};
}
