#include "ParserDriver.h"
#include "scanner.h"
#include "parser.h"
#include <fstream>
#include <iostream>


bool mcool::ParserDriver::parse(misc::Config& config) {
  bool isOk{true};

  mcool::Scanner scanner;
  mcool::Parser parser(scanner, astTree);

  for (auto& inputFile : config.inputFiles) {
    std::ifstream fileStream(inputFile);
    if (fileStream.fail()) {
      std::cerr << "cannot open file: " << inputFile << std::endl;
      isOk = false;
      continue;
    }
    scanner.reset(&fileStream, &inputFile);
    isOk &= (parser.parse() == 0);
  }
  return isOk;
}