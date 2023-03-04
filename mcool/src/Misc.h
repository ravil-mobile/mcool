#pragma once

#include "Parser/Loc.h"
#include <string>
#include <list>
#include <ostream>

namespace mcool {
class AstTree;
}

namespace mcool::misc {
struct Config {
  std::list<std::string> inputFiles{};
  std::string outputFile{"./a"};
  bool dotOutput{false};
  bool printInheritance{false};
  bool emitLLVMIr{false};
  bool writeAsmOutput{false};
  bool verbose{false};
};

Config readCmd(int argc, char* argv[]);
void analyseUntypedAst(mcool::AstTree& astTree, mcool::misc::Config& config);
} // namespace mcool::misc

std::ostream& operator<<(std::ostream& stream, const mcool::Loc& loc);
