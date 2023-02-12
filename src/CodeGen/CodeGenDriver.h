#pragma once

#include "Parser/AstTree.h"
#include "Context.h"
#include <ostream>

namespace mcool::codegen {
class CodeGenDriver {
  public:
  CodeGenDriver(mcool::Context& context, misc::Config& config)
      : context(context), config(config) {}
  bool run(mcool::AstTree& classes);

  private:
  mcool::Context& context;
  misc::Config& config;
};
} // namespace mcool::codegen