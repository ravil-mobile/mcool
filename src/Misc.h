#pragma once

#include <string>
#include <ostream>
#include <Loc.h>

namespace mcool::misc {
struct Config {
  std::string inputFile{};
  std::string outputFile{"./a.out"};
  bool verbose{false};
  bool dotOutput{false};
};

Config readCmd(int argc, char* argv[]);
} // namespace mcool::misc


std::ostream& operator<<(std::ostream& stream, const mcool::Loc& loc);