#pragma once

#include "visitor.h"
#include "ast.h"
#include <ostream>

namespace mcool {
class AstPinter : public ast::Visitor {
public:
  explicit AstPinter(std::ostream& stream) : stream(stream) {}

  void visitPlusNode(ast::PlusNode* plusNode) override {
    plusNode->getLeft()->accept(this);
    stream << " + ";
    plusNode->getRight()->accept(this);
  }

  void visitInt(ast::Int* item) override {
    stream << item->getValue();
  }

private:
  std::ostream& stream;
};
}