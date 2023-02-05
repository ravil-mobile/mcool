#pragma once

#include "visitor.h"
#include "Semant/TypeChecker/Error.h"
#include "Semant/Types/InheritanceGraph.h"
#include "Context.h"
#include <vector>
#include <memory>
#include <deque>
#include <set>

namespace mcool::semant {

class InheritanceGraphBuilder : public ast::Visitor, public semant::Error {
  public:
  InheritanceGraphBuilder();

  std::unique_ptr<Graph> build(ast::CoolClassList* classes) {
    initGraph(classes->getData());
    checkParentClassDefinitions();
    assignParentNodes();
    assignChildNodes();
    checkInheritanceGraphCycles();

    return std::move(graph);
  }

  private:
  void initGraph(const std::list<ast::CoolClass*>& classes);
  void checkParentClassDefinitions();
  void assignParentNodes();
  void assignChildNodes();
  void visitGraphNode(Graph::Node* node,
                      std::deque<Graph::Node*>& stack,
                      std::set<size_t>& discovered);
  void checkInheritanceGraphCycles();

  std::unique_ptr<Graph> graph{nullptr};
};

} // namespace mcool::semant