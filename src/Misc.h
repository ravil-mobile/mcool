#pragma once

#include <string>

namespace mcool::misc {
struct Config {
  std::string inputFile{};
  bool verbose{false};
};

Config readCmd(int argc, char* argv[]);

}