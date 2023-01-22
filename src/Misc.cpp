#include "Misc.h"
#include "ast.h"
#include "AstTree.h"
#include "Printer.h"
#include "DotPrinter.h"

#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"

#include <filesystem>
#include <cassert>


namespace mcool::misc {
Config readCmd(int argc, char* argv[]) {
  Config config{};

  CLI::App cmd{"MCool compiler"};

  cmd.add_option("-i,--input", config.inputFiles, "path to the input file");
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

  for (auto& inputFile : config.inputFiles) {
    std::filesystem::path inputPath{inputFile};
    if (not std::filesystem::exists(inputPath)) {
      throw std::runtime_error("input file does not exist. See: " + inputFile);
    }

    if (inputPath.extension() != std::string(".cl")) {
      throw std::runtime_error("unknown input file extension. Expected `.cl`. See: " + inputFile);
    }
  }
  assert((not config.inputFiles.empty()) && "no input files");

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


void analyseUntypedAst(mcool::AstTree& astTree, mcool::misc::Config& config) {
  if (config.verbose) {
    mcool::AstPinter astPinter(std::cout);
    astTree.accept(&astPinter);
  } else if (config.dotOutput) {
    std::string dotFileName = config.outputFile + ".dot";
    std::fstream dotOutputFile(dotFileName.c_str(), std::ios::out);
    if (dotOutputFile.is_open()) {
      mcool::DotPrinter dotPrinter(dotOutputFile);
      astTree.accept(&dotPrinter);
      dotOutputFile.close();

      std::fstream dotInputFile(dotFileName.c_str(), std::ios::in);
      std::string line;
      while (std::getline(dotInputFile, line)) {
        std::cout << line << std::endl;
      }
      dotInputFile.close();
    } else {
      std::cerr << "cannot open dot output file: " << dotFileName << std::endl;
    }
  }
}
} // namespace mcool::misc


std::ostream& operator<<(std::ostream& stream, const mcool::Loc& loc) {
  if (loc.filename) {
    stream << loc.filename->get() << " ";
  }
  stream << loc.begin.line << ':' << loc.begin.column << '-'
         << loc.end.line << ':' << loc.end.column << '\n';
  return stream;
}
