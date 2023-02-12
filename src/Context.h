#pragma once

#include "Types/InheritanceGraph.h"
#include "Types/TypeBuilder.h"
#include <memory>
#include <cassert>

namespace mcool {
class Context {
  public:
  void setInheritanceGraph(std::unique_ptr<type::Graph> userGraph) {
    graph = std::move(userGraph);
    typeBuilder = std::make_unique<type::TypeBuilder>(graph);
  }
  const std::unique_ptr<type::Graph>& getInheritanceGraph() { return graph; }
  std::unique_ptr<type::TypeBuilder>& getTypeBulder() { return typeBuilder; }

  private:
  std::unique_ptr<type::Graph> graph{nullptr};
  std::unique_ptr<type::TypeBuilder> typeBuilder{nullptr};
};
} // namespace mcool