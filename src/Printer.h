#pragma once

#include "visitor.h"
#include "ast.h"
#include <ostream>

namespace mcool {
class AstPinter : public ast::Visitor {
public:
  explicit AstPinter(std::ostream& stream) : stream(stream) {}

  void visitPlusNode(ast::PlusNode* node) override {
    stream << std::string(indent, ' ') << " + \n";
    indent += 2;

    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    indent -= 2;
  }

  void visitMinusNode(ast::MinusNode* node) override {
    stream << std::string(indent, ' ') << " - \n";
    indent += 2;

    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    indent -= 2;
  }

  void visitMultiplyNode(ast::MultiplyNode* node) override {
    stream << std::string(indent, ' ') << " * \n";
    indent += 2;

    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    indent -= 2;
  }

  void visitDivideNode(ast::DivideNode* node) override {
    stream << std::string(indent, ' ') << " / \n";
    indent += 2;

    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    indent -= 2;

  }

  void visitExpressions(ast::Expressions* node) override {
    auto& exprs = node->getExpressions();
    for (auto* expr : exprs) {
      expr->accept(this);
    }
    stream << "\n";
  }

  void visitSingeltonExpression(ast::SingeltonExpression* node) override {
    auto* term = node->getTerm();
    term->accept(this);
  }

  void visitObjectId(ast::ObjectId* id) override {
    stream << std::string(indent, ' ') << "id: " << id->getStr() << "\n";
  }

  void visitInt(ast::Int* item) override {
    stream << std::string(indent, ' ') << item->getValue() << "\n";
  }

  void visitBool(ast::Bool* item) override {
    stream << std::string(indent, ' ') << std::boolalpha << item->getValue() << "\n";
  }

private:
  std::ostream& stream;
  size_t indent{0};
};
}