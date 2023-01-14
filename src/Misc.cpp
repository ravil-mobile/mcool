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
  cmd.add_option("-o,--output", config.outputFile, "output file name");
  auto* dotOption = cmd.add_flag("--dot-output", "print ast in dot format (graphviz)");
  auto* verboseOption = cmd.add_flag("-v,--verbose", "verbose mode");

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

  std::filesystem::path outputPath{config.outputFile};
  auto parentPath = outputPath.parent_path();
  if (not std::filesystem::exists(parentPath)) {
    throw std::runtime_error("parent output directory does not exist");
  }

  if (*dotOption) {
    config.dotOutput = true;
  }

  if (*verboseOption) {
    config.verbose = true;
  }

  return config;
}
}