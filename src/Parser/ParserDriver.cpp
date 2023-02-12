#include "ParserDriver.h"
#include "Scanner.h"
#include "Parser.h"
#include <fstream>
#include <iostream>

void mcool::ParserDriver::parse() {
  isParserOk = true;
  astTree = mcool::AstTree{};

  mcool::Scanner scanner(true);
  mcool::Parser parser(scanner, astTree, context.getMemoryManagement());

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
    return astTree;
  } else {
    return {};
  }
}
