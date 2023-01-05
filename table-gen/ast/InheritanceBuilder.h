#pragma once

#include "InheritanceTree.h"
#include <vector>
#include <list>
#include <unordered_map>
#include <string>


namespace ast::inheritance {
struct Graph {
  struct Node {
    explicit Node(llvm::Record* record) : record(record) {}

    llvm::Record* record{nullptr};
    std::list<Graph::Node*> children;
  };

  std::unordered_map<std::string, Graph::Node> nodes{};
};

class InheritanceBuilder {
public:
  std::unique_ptr<tree::Node> build(std::vector<llvm::Record*>& records);

private:
  void createNodes(std::vector<llvm::Record*>& records);
  void assignChildren();
  void buildTreeRoot(tree::Node* node);
  static std::unique_ptr<tree::Node> createTreeNode(Graph::Node* graphNode);

  Graph graph{};
};
}