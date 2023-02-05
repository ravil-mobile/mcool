#include "ParserDriver.h"
#include "scanner.h"
#include "parser.h"
#include <fstream>
#include <iostream>

void mcool::ParserDriver::parse() {
  isParserOk = true;
  astTree = mcool::AstTree{};

  mcool::Scanner scanner(true);
  mcool::Parser parser(scanner, astTree);

  for (auto& inputFile : config.inputFiles) {
    std::ifstream fileStream(inputFile);
    if (fileStream.fail()) {
      std::cerr << "cannot open file: " << inputFile << std::endl;
      isParserOk = false;
      continue;
    }

    scanner.set(&fileStream, &inputFile);
    int currParserStatus = parser.parse();
    isParserOk &= (currParserStatus == 0);
  }
}

std::optional<mcool::AstTree> mcool::ParserDriver::getAst() {
  if (isParserOk) {
    isParserOk = false;
    return std::optional<mcool::AstTree>(astTree);
  } else {
    return std::optional<mcool::AstTree>{};
  }
}
