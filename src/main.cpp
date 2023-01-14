#include "scanner.h"
#include "parser.h"
#include "AstTree.h"
#include "Misc.h"
#include "Printer.h"
#include "DotPrinter.h"
#include "CLI/Error.hpp"
#include <string>
#include <fstream>
#include <iostream>


int main (int argc, char* argv[]) {
  mcool::misc::Config config{};

  try {
    config = mcool::misc::readCmd(argc, argv);
  }
  catch (const CLI::ParseError &err) {
    std::cerr << err.what() << std::endl;
    return -1;
  }
  catch(std::runtime_error& err) {
    std::cerr << err.what() << std::endl;
    return -1;
  }

  std::ifstream fileStream(config.inputFile.c_str());
  if (fileStream.fail()) {
    std::cerr << "cannot open file: " << config.inputFile << std::endl;
    return -1;
  }

  mcool::Scanner scanner(&fileStream, &config.inputFile);
  mcool::AstTree astTree;
  mcool::Parser parser(scanner, astTree);
  auto status = parser.parse();
  std::cout << "parser status: " << status << std::endl; 

  if (config.verbose) {
    mcool::AstPinter astPinter(std::cout);
    astTree.accept(&astPinter);
  }
  else if (config.dotOutput) {
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
    }
    else {
      std::cerr << "cannot open dot output file: " << dotFileName << std::endl;
    }
  }

  fileStream.close();
  return 0;
}
