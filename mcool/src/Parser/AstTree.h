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
  [[nodiscard]] bool isAstOk() const { return isOk; }

  static void addBuildinClasses(ast::CoolClassList* classes, mcool::Context* context);

  private:
  mcool::ast::CoolClassList* tree{nullptr};
  bool isOk{true};
};
} // namespace mcool
