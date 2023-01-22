#pragma once

#include "visitor.h"
#include "ast.h"
#include <ostream>

namespace mcool {
class AstPinter : public ast::Visitor {
public:
  explicit AstPinter(std::ostream& stream) : stream(stream) {}

  void visitCoolClassList(ast::CoolClassList* classList) override {
    auto& classes = classList->getData();
    for (auto* coolClass : classes) {
      coolClass->accept(this);
    }
  }

  void visitCoolClass(ast::CoolClass* coolClass) override {
    auto& className = coolClass->getCoolType()->getNameAsStr();
    stream << std::string(indent, ' ') << "class: " << className << "\n";
    indent += 2;
    coolClass->getAttributes()->accept(this);
    indent -= 2;
  }

  void visitAttributeList(ast::AttributeList* attributeList) override {
    auto& attrs = attributeList->getData();
    for (auto* attr : attrs) {
      attr->accept(this);
    }
  }

  void visitSingleMethod(ast::SingleMethod* method) override {
    auto methodName = method->getId()->getNameAsStr();
    stream << std::string(indent, ' ') << "method: " << methodName << "\n";

    indent += 2;
    method->getBody()->accept(this);
    indent -= 2;
  };

  void visitBlockExpr(ast::BlockExpr* block) override {
    stream << std::string(indent, ' ') << "block: \n";
    auto* exprs = block->getExprs();
    exprs->accept(this);
  }

  void visitPlusNode(ast::PlusNode* node) override {
    stream << std::string(indent, ' ') << "+\n";
    indent += 2;
    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    indent -= 2;
  }

  void visitMinusNode(ast::MinusNode* node) override {
    stream << std::string(indent, ' ') << "-\n";
    indent += 2;
    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    indent -= 2;
  }

  void visitMultiplyNode(ast::MultiplyNode* node) override {
    stream << std::string(indent, ' ') << "*\n";
    indent += 2;
    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    indent -= 2;
  }

  void visitDivideNode(ast::DivideNode* node) override {
    stream << std::string(indent, ' ') << "/\n";
    indent += 2;
    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    indent -= 2;
  }

  void visitExpressions(ast::Expressions* node) override {
    auto& exprs = node->getData();
    indent += 2;
    for (auto* expr : exprs) {
      expr->accept(this);
    }
    indent -= 2;
  }

  void visitIsVoidNode(ast::IsVoidNode* node) override {
    stream << std::string(indent, ' ') << "is void: " << "\n";
    indent += 2;
    auto* term = node->getTerm();
    term->accept(this);
    indent -= 2;
  }

  void visitPrimaryExpression(ast::PrimaryExpression* node) override {
    auto* term = node->getTerm();
    term->accept(this);
  }

  void visitNegationNode(ast::NegationNode* node) override {
    stream << std::string(indent, ' ') << "negate: " << "\n";
    indent += 2;
    auto* term = node->getTerm();
    term->accept(this);
    indent -= 2;
  }

  void visitObjectId(ast::ObjectId* id) override {
    stream << std::string(indent, ' ') << "id: " << id->getNameAsStr() << "\n";
  }

  void visitBool(ast::Bool* item) override {
    stream << std::string(indent, ' ') << std::boolalpha << item->getValue() << "\n";
  }

  void visitInt(ast::Int* item) override {
    stream << std::string(indent, ' ') << item->getValue() << "\n";
  }

  void visitString(ast::String* str) override {
    stream << std::string(indent, ' ') << "str: " << str->getValueAsStr() << "\n";
  }

  void visitDispatch(ast::Dispatch* dispatch) override {
    stream << std::string(indent, ' ') << "call: \n";
    indent += 2;
    dispatch->getObjectId()->accept(this);
    dispatch->getMethodId()->accept(this);
    dispatch->getArguments()->accept(this);
    indent -= 2;
  }

  void visitEqualNode(ast::EqualNode* node) override {
    stream << std::string(indent, ' ') << "==\n";
    indent += 2;
    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    indent -= 2;
  }

  void visitLessEqualNode(ast::LessEqualNode* node) override {
    stream << std::string(indent, ' ') << "<=\n";
    indent += 2;
    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    indent -= 2;
  }

  void visitLessNode(ast::LessNode* node) override {
    stream << std::string(indent, ' ') << "<\n";
    indent += 2;
    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    indent -= 2;
  }

private:
  std::ostream& stream;
  size_t indent{0};
};
}
