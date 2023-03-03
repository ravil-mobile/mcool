#pragma once

#include "Misc.h"
#include "AstTree.h"
#include "Context.h"
#include <optional>

namespace mcool {
class ParserDriver {
  public:
  explicit ParserDriver(misc::Config& config) : config(config) {}
  void parse();
  std::optional<mcool::AstTree> getAst();
  mcool::Context getContext() { return std::move(context); }

  private:
  misc::Config& config;
  mcool::AstTree astTree{};
  mcool::Context context{};
  bool isParserOk{false};
};
} // namespace mcool
