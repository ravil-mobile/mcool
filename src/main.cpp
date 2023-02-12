#include "Parser/ParserDriver.h"
#include "Semant/TypeDriver.h"
#include "Misc.h"
#include "Context.h"
#include "CLI/Error.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
  mcool::misc::Config config{};

  try {
    config = mcool::misc::readCmd(argc, argv);
  } catch (const CLI::ParseError& err) {
    std::cerr << err.what() << std::endl;
    return -1;
  } catch (std::runtime_error& err) {
    std::cerr << err.what() << std::endl;
    return -1;
  }

  mcool::ParserDriver driver(config);
  driver.parse();
  auto ast = driver.getAst();
  auto context = driver.getContext();

  if (not ast.has_value()) {
    std::cerr << "parsing failed. cannot continue" << std::endl;
    return -1;
  }

  auto& astTree = ast.value();
  if (not astTree.isAstOk()) {
    std::cerr << "ast tree is not complete due to parsing errors" << std::endl;
    return -1;
  }

  mcool::misc::analyseUntypedAst(astTree, config);

  mcool::AstTree::addBuildinClasses(astTree.get(), &context);

  mcool::TypeDriver typeDriver(context, config);
  auto isTypeCheclingOk = typeDriver.run(astTree);
  if (not isTypeCheclingOk) {
    typeDriver.printErrors(std::cerr);
    return -1;
  }

  return 0;
}
