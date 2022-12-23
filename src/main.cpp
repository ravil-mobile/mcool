#include "scanner.h"
#include "parser.h"

#include "Misc.h"
#include "CLI/Error.hpp"
#include <string>
#include <fstream>
#include <iostream>

#include "ast.h"


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

  mcool::Scanner scanner(&fileStream);
  mcool::Parser parser(scanner);
  parser.parse();

  mcool::ast::MyDefinition def;

  fileStream.close();
  return 0;
}
