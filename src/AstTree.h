#pragma once

#include "visitor.h"
#include "ast.h"
#include <cassert>

namespace mcool {
class AstTree {
public:
  void set(mcool::ast::Expressions* program) {
    this->tree = program;
  }

  mcool::ast::Expressions* get() {
    return this->tree;
  }

  void accept(mcool::ast::Visitor* visitor) {
    assert(this->tree && "tree is not set");
    this->tree->accept(visitor);
  }

private:
  mcool::ast::Expressions* tree{nullptr};
};
} // namespace mcool