#pragma once

#include <string>
#include <list>
#include <ostream>
#include <Loc.h>

namespace mcool {
  class AstTree;
}

namespace mcool::misc {
struct Config {
  std::list<std::string> inputFiles{};
  std::string outputFile{"./a.out"};
  bool verbose{false};
  bool dotOutput{false};
};

Config readCmd(int argc, char* argv[]);
void analyseUntypedAst(mcool::AstTree& astTree, mcool::misc::Config& config);
} // namespace mcool::misc


std::ostream& operator<<(std::ostream& stream, const mcool::Loc& loc);
