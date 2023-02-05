#pragma once

#include "Semant/Types/InheritanceGraph.h"
#include <memory>
#include <cassert>

namespace mcool {
class Context {
  public:
  void setInheritanceGraph(std::unique_ptr<semant::Graph> userGraph) {
    this->graph = std::move(userGraph);
  }
  const std::unique_ptr<semant::Graph>& getInheritanceGraph() { return graph; }

  const semant::Graph::Node& getInheritanceNode(semant::Type* type) {
    assert(type != nullptr);
    auto name = type->getAsString();
    auto it = graph->findNode(name);
    if (it == graph->end()) {
      throw std::runtime_error("no graph node found with `" + name + "` name");
    }
    return it->second;
  }

  const semant::Graph::Node& getInheritanceNode(const std::string& name) {
    auto it = graph->findNode(name);
    if (it == graph->end()) {
      throw std::runtime_error("no graph node found with `" + name + "` name");
    }

    return it->second;
  }

  private:
  std::unique_ptr<semant::Graph> graph{nullptr};
};
} // namespace mcool