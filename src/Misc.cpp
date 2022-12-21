#include "Misc.h"

#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"

#include <filesystem>

namespace mcool::misc {
Config readCmd(int argc, char* argv[]) {
  Config config{};

  CLI::App cmd{"MCool compiler"};

  cmd.add_option("-i,--input", config.inputFile, "path to the input file");
  cmd.add_option("-v,--verbose", config.verbose, "verbose mode");
  
  try {
    cmd.parse(argc, argv);
  }
  catch (const CLI::ParseError &err) {
    cmd.exit(err);
    throw std::runtime_error("");
  }

  if (cmd.count("--input") == 0) {
    throw std::runtime_error("no input file was provided");
  }

  std::filesystem::path inputPath{config.inputFile};
  if (not std::filesystem::exists(inputPath)) {
    throw std::runtime_error("input file does not exist");
  }

  if (inputPath.extension() != std::string(".cl")) {
    throw std::runtime_error("unknown input file extension. Expected `.cl`");
  }

  return config;
}
}