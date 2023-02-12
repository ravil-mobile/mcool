#pragma once

#include "Parser/Scanner.h"
#include "Parser.h"
#include "Context.h"
#include "ast.h"
#include "Semant/TypeDriver.h"
#include "Semant/TypeChecker/InheritanceGraphBuilder.h"
#include "Semant/TypeChecker/EnvironmentsBuilder.h"
#include "Semant/TypeChecker/TypeChecker.h"
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <cassert>
#include "gtest/gtest.h"
#include "gmock/gmock-matchers.h"

namespace mcool::tests::semant {
class TestDriver {
  public:
  explicit TestDriver(std::istream& stream) : stream(stream) {}

  auto run() {
    mcool::AstTree astTree{};

    mcool::Scanner scanner(true);
    mcool::Parser parser(scanner, astTree, context.getMemoryManagement());

    scanner.set(&stream, &inputFileName);
    bool parserStatus = parser.parse() == 0;

    mcool::AstTree::addBuildinClasses(astTree.get(), &context);

    return std::make_tuple(parserStatus, astTree);
  }

  mcool::Context& getContext() { return context; }

  private:
  std::istream& stream;
  std::string inputFileName{"test-stream"};
  mcool::Context context{};
};

inline void applyTypeChecking(mcool::AstTree& ast, Context& context) {
  mcool::semant::InheritanceGraphBuilder graphBuilder;
  auto graph = graphBuilder.build(ast.get());
  context.setInheritanceGraph(std::move(graph));

  mcool::semant::EnvironmentsBuilder envBuilder(context);
  auto env = envBuilder.build(ast.get());
  if (envBuilder.hasErrors()) {
    for (auto& errorMsg: envBuilder.getErrors()) {
      std::cout << errorMsg << std::endl;
    }
  }
  ASSERT_FALSE(envBuilder.hasErrors());

  mcool::semant::TypeChecker typeChecker(context, env);
  typeChecker.run(ast.get());
  if (typeChecker.hasErrors()) {
    for (auto& errorMsg: typeChecker.getErrors()) {
      std::cout << errorMsg << std::endl;
    }
  }
  ASSERT_FALSE(typeChecker.hasErrors());
}

} // namespace mcool::tests::parser
