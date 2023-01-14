#pragma once

#include "visitor.h"
#include "ast.h"
#include <ostream>
#include <vector>
#include <stack>
#include <sstream>

namespace mcool {
class DotPrinter : public ast::Visitor {
public:
  DotPrinter(std::ostream& stream) : OS(stream) {}

  void visitCoolClassList(ast::CoolClassList* classList) {
    auto& list = classList->getData();

    OS << "digraph " << "Program" << " {\n";
    for (auto* coolClass : list) {
      coolClass->accept(this);
      ++classCounter;
    }
    OS << "}\n";
  }

  void visitCoolClass(ast::CoolClass* coolClass) {
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

  void visitAttributeList(ast::AttributeList* attributeList) {
    const auto& classNode = nodeStack.top();
    auto& attributes = attributeList->getData();
    for (auto* attr : attributes) {
      attr->accept(this);
      auto attrNode = pop(nodeStack);
      edgeStream << "  " << classNode << " -> " << attrNode << "\n";
    }
  }

  void visitSingleMethod(ast::SingleMethod* method) {
    auto& methodName = method->getId()->getNameAsStr();
    std::string methodNodeName = buildNodeName();
    addNodeLabel(methodNodeName, std::string("method:") + methodName);

    nodeStack.push(methodNodeName);
    method->getBody()->accept(this);

    auto bodyNode = pop(nodeStack);
    edgeStream << "  " << methodNodeName << " -> " << bodyNode << "\n";
  }

  void visitSingleMemeber(ast::SingleMemeber* member) {
    auto& memberName = member->getId()->getNameAsStr();
    std::string memberNodeName = buildNodeName();
    addNodeLabel(memberNodeName, std::string("member:") + memberName);

    nodeStack.push(memberNodeName);
  }

  void visitBlockExpr(ast::BlockExpr* block) {
    std::string blockName = buildNodeName();
    addNodeLabel(blockName, std::string("block"));
    nodeStack.push(blockName);

    block->getExprs()->accept(this);
  }

  void visitExpressions(ast::Expressions* exprs) {
    const auto& parentNode = nodeStack.top();
    auto& list = exprs->getData();
    for (auto* expr : list) {
      expr->accept(this);
      auto childNode = pop(nodeStack);
      edgeStream << "  " << parentNode << " -> " << childNode << "\n";
    }
  }

  void visitSingleCase(ast::SingleCase*) {

  }

  void visitWhileLoop(ast::WhileLoop*) {

  }

  void visitStaticDispatch(ast::StaticDispatch*) {

  }

  void visitNegationNode(ast::NegationNode* node) {
    std::string negName = buildNodeName();
    addNodeLabel(negName, std::string("neg"));
    nodeStack.push(negName);

    node->getTerm()->accept(this);
    auto termNode = pop(nodeStack);
    edgeStream << "  " << negName << " -> " << termNode << '\n';
  }

  void visitPrimaryExpression(ast::PrimaryExpression* node) {
    node->getTerm()->accept(this);
  }

  void visitIsVoidNode(ast::IsVoidNode* node) {
    std::string isvoidName = buildNodeName();
    addNodeLabel(isvoidName, std::string("isvoid"));
    nodeStack.push(isvoidName);

    node->getTerm()->accept(this);
    auto termNode = pop(nodeStack);
    edgeStream << "  " << isvoidName << " -> " << termNode << '\n';
  }

  void visitNotExpr(ast::NotExpr*) {

  }

  void visitDispatch(ast::Dispatch*) {

  }

  void visitIfThenExpr(ast::IfThenExpr*) {

  }

  void visitNewExpr(ast::NewExpr*) {

  }

  void visitCaseExpr(ast::CaseExpr*) {

  }

  void visitPlusNode(ast::PlusNode* node) {
    std::string plusNodeName = buildNodeName();
    addNodeLabel(plusNodeName, std::string("+"));
    visitBinaryNode(plusNodeName, node);
  }

  void visitMinusNode(ast::MinusNode* node) {
    std::string minusNodeName = buildNodeName();
    addNodeLabel(minusNodeName, std::string("-"));
    visitBinaryNode(minusNodeName, node);
  }

  void visitMultiplyNode(ast::MultiplyNode* node) {
    std::string multNodeName = buildNodeName();
    addNodeLabel(multNodeName, std::string("*"));
    visitBinaryNode(multNodeName, node);
  }

  void visitDivideNode(ast::DivideNode* node) {
    std::string divNodeName = buildNodeName();
    addNodeLabel(divNodeName, std::string("/"));
    visitBinaryNode(divNodeName, node);
  }

  void visitLessNode(ast::LessNode* node) {
    std::string lessNodeName = buildNodeName();
    addNodeLabel(lessNodeName, std::string("<"));
    visitBinaryNode(lessNodeName, node);
  }

  void visitLessEqualNode(ast::LessEqualNode* node) {
    std::string leqNodeName = buildNodeName();
    addNodeLabel(leqNodeName, std::string("<="));
    visitBinaryNode(leqNodeName, node);
  }

  void visitEqualNode(ast::EqualNode* node) {
    std::string equalNodeName = buildNodeName();
    addNodeLabel(equalNodeName, std::string("="));
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

  void visitAssignExpr(ast::AssignExpr* node) {
    auto assignName = buildNodeName();
    addNodeLabel(assignName, std::string("assign"));
    nodeStack.push(assignName);

    node->getId()->accept(this);
    auto lhs = pop(nodeStack);
    edgeStream << "  " << assignName << " -> " << lhs << '\n';

    node->getInitExpr()->accept(this);
    auto rhs = pop(nodeStack);
    edgeStream << "  " << assignName << " -> " << rhs << '\n';
  }

  void visitIfThenElseExpr(ast::IfThenElseExpr*) {

  }

  void visitNoExpr(ast::NoExpr*) {

  }

  void visitLetExpr(ast::LetExpr*) {

  }

  void visitCaseList(ast::CaseList*) {

  }

  void visitFormalList(ast::FormalList*) {

  }

  void visitSingleFormal(ast::SingleFormal*) {

  }

  void visitTypeId(ast::TypeId* type) {
    auto nodeName = buildNodeName();
    addNodeLabel(nodeName, std::string("type:") + type->getNameAsStr());
    nodeStack.push(nodeName);
  }

  void visitObjectId(ast::ObjectId* id) {
    auto nodeName = buildNodeName();
    addNodeLabel(nodeName, std::string("id:") + id->getNameAsStr());
    nodeStack.push(nodeName);
  }

  void visitString(ast::String*) {
    auto nodeName = buildNodeName();
    addNodeLabel(nodeName, std::string("str"));
    nodeStack.push(nodeName);
  }

  void visitBool(ast::Bool* node) {
    auto nodeName = buildNodeName();
    addNodeLabel(nodeName, std::to_string(node->getValue()));
    nodeStack.push(nodeName);
  }

  void visitInt(ast::Int* node) {
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