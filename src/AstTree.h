#pragma once

#include "visitor.h"
#include "ast.h"
#include <cassert>

namespace mcool {
class AstTree {
public:
  void set(mcool::ast::CoolClassList* program) {
    if (tree == nullptr) {
      tree = program;
    }
    else {
      for (auto* coolClass : program->getData()) {
        tree->add(coolClass);
      }
    }
  }

  mcool::ast::CoolClassList* get() {
    return this->tree;
  }

  void accept(mcool::ast::Visitor* visitor) {
    assert(this->tree && "tree is not set");
    this->tree->accept(visitor);
  }

  void setError() {
    isOk = false;
  }

  bool isAstOk() {
    return isOk;
  }

private:
  mcool::ast::CoolClassList* tree{nullptr};
  bool isOk{true};
};
} // namespace mcool