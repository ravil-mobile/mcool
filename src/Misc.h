#pragma once

#include <string>

namespace mcool::misc {
struct Config {
  std::string inputFile{};
  std::string outputFile{"./a.out"};
  bool verbose{false};
  bool dotOutput{false};
};

Config readCmd(int argc, char* argv[]);
}