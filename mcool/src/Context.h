#pragma once

#include "MemoryManager.h"
#include "Types/InheritanceGraph.h"
#include "Types/TypeBuilder.h"
#include <memory>
#include <cassert>

namespace mcool {
class Context {
  public:
  explicit Context() : typeBuilder(std::make_unique<type::TypeBuilder>()) {}
  Context(Context&& other) = default;

  void setInheritanceGraph(std::unique_ptr<type::Graph> userGraph) {
    graph = std::move(userGraph);
    typeBuilder->setInheritanceGraph(graph.get());
  }
  const std::unique_ptr<type::Graph>& getInheritanceGraph() { return graph; }
  std::unique_ptr<type::TypeBuilder>& getTypeBulder() { return typeBuilder; }
  mcool::MemoryManager& getMemoryManager() { return memoryManager; }

  private:
  std::unique_ptr<type::Graph> graph{nullptr};
  std::unique_ptr<type::TypeBuilder> typeBuilder{nullptr};
  mcool::MemoryManager memoryManager{};
};
} // namespace mcool