#pragma once

#include "AstTree.h"
#include "Context.h"
#include "Semant/TypeChecker/Error.h"

namespace mcool {
class TypeDriver {
  public:
  explicit TypeDriver(mcool::Context& context, misc::Config& config)
      : context(context), config(config) {}
  bool run(mcool::AstTree& classes);
  void printErrors(std::ostream& os);

  private:
  mcool::Context& context;
  misc::Config& config;
  semant::Error errorLogger{};
};
} // namespace mcool