#include "scanner.h"
#include "parser.h"

#include "Misc.h"
#include "MemoryManagement.h"
#include "Printer.h"
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

  mcool::MemoryManagement& mm = mcool::MemoryManagement::getInstance();
  auto* plusNode = mcool::make<mcool::ast::PlusNode>(
    mcool::make<mcool::ast::Int>(5),
    mcool::make<mcool::ast::Int>(10));

  std::cout << std::endl;
  mcool::AstPinter astPinter(std::cout);
  plusNode->accept(&astPinter);

  fileStream.close();
  std::cout << std::endl << "end" << std::endl;
  return 0;
}
