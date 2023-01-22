#pragma once

#include "Misc.h"
#include "AstTree.h"
#include <optional>


namespace mcool {
class ParserDriver {
public:
  explicit ParserDriver(misc::Config& config) : config(config) {}
  void parse();
  std::optional<mcool::AstTree> getAst();

private:
  misc::Config& config;
  mcool::AstTree astTree{};
  bool isParserOk{false};
};
}
