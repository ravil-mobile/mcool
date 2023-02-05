#pragma once

#include "visitor.h"
#include "ast.h"
#include <cassert>

namespace mcool {
class AstTree {
  public:
  void set(mcool::ast::CoolClassList* program);
  mcool::ast::CoolClassList* get() { return this->tree; }
  void accept(mcool::ast::Visitor* visitor);
  static void addBuildinClasses(mcool::ast::CoolClassList* classes);
  void setError() { isOk = false; }
  bool isAstOk() { return isOk; }

private:

  mcool::ast::CoolClassList* tree{nullptr};
  bool isOk{true};
};
} // namespace mcool
