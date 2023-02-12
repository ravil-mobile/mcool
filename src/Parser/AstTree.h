#pragma once

#include "visitor.h"
#include "ast.h"
#include <cassert>

namespace mcool {
class Context;

class AstTree {
  public:
  void set(mcool::ast::CoolClassList* program);
  mcool::ast::CoolClassList* get() { return this->tree; }
  void accept(mcool::ast::Visitor* visitor);
  void setError() { isOk = false; }
  bool isAstOk() { return isOk; }

  static void addBuildinClasses(ast::CoolClassList* classes);

  private:
  mcool::ast::CoolClassList* tree{nullptr};
  bool isOk{true};
};
} // namespace mcool
