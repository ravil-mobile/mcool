#include "TypeDriver.h"
#include "Semant/TypeChecker/InheritanceGraphBuilder.h"
#include "Semant/TypeChecker/EnvironmentsBuilder.h"
#include "Semant/TypeChecker/TypeChecker.h"
#include <iostream>

bool mcool::TypeDriver::run(mcool::AstTree& classes) {
  semant::InheritanceGraphBuilder graphBuilder{};
  auto graph = graphBuilder.build(classes.get());
  if (graphBuilder.hasErrors()) {
    errorLogger.retrieveErrors(&graphBuilder);
    return false;
  }

  if (config.printInheritance) {
    graph->printGraph(std::cout);
  }

  context.setInheritanceGraph(std::move(graph));
  semant::EnvironmentsBuilder envBuilder(context);
  auto env = envBuilder.build(classes.get());
  if (envBuilder.hasErrors()) {
    errorLogger.retrieveErrors(&envBuilder);
    return false;
  }

  semant::TypeChecker typeChecker(context, env);
  typeChecker.run(classes.get());
  if (typeChecker.hasErrors()) {
    errorLogger.retrieveErrors(&typeChecker);
    return false;
  }

  return true;
}

void mcool::TypeDriver::printErrors(std::ostream& os) {
  for (auto& error : errorLogger.getErrors()) {
    os << error << '\n';
  }
}