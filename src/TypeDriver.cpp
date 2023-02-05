#include "TypeDriver.h"
#include "Semant/TypeChecker/InheritanceGraphBuilder.h"
#include "Semant/Types/Environment.h"
#include <iostream>

bool mcool::TypeDriver::run(mcool::AstTree& classes) {
  semant::InheritanceGraphBuilder graphBuilder{};
  auto graph = graphBuilder.build(classes.get());
  if (graphBuilder.hasErrors()) {
    graphBuilder.getErrors();
    errorLogger.retrieveErrors(&graphBuilder);
    return false;
  }

  std::cout << std::boolalpha << config.printInheritance << std::endl;
  if (config.printInheritance) {
    graph->printGraph(std::cout);
  }

  context.setInheritanceGraph(std::move(graph));
  semant::TypeEnvironments typeEnvironments{};

  return false;
}

void mcool::TypeDriver::printErrors(std::ostream& os) {
  for (auto& error : errorLogger.getErrors()) {
    os << error << '\n';
  }
}