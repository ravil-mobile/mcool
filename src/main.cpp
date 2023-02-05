#include "ParserDriver.h"
#include "Misc.h"
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

  if (ast.has_value()) {
    auto& astTree = ast.value();
    if (not astTree.isAstOk()) {
      std::cerr << "ast tree is not complete due to parsing errors" << std::endl;
    }

    mcool::misc::analyseUntypedAst(astTree, config);

  } else {
    std::cerr << "parsing failed. cannot continue" << std::endl;
  }
}
