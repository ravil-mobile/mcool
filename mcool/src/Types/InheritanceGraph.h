#pragma once

#include "ast.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <ostream>
#include <cassert>

namespace mcool::type {
class Graph {
  public:
  class Node {
public:
    Node() = delete;
    explicit Node(mcool::ast::CoolClass* astNode);
    explicit Node(std::string name);
    Node(const Node& other) = default;
    bool isSame(Node* other) const;
    bool isChildOf(const Node* other) const;

    void setParent(Node* node) { parent = node; }
    void addChild(Node* node) { children.push_back(node); }

    auto getId() { return id; }
    const auto& getNodeName() const { return nodeName; }
    const auto* getParent() const { return parent; }
    auto* getParent() { return parent; }
    const auto& getChildren() const { return children; }
    bool isNodeName(const std::string& name) const { return nodeName == name; }
    auto* getCoolClass() {
      assert(classAstNode != nullptr && "a graph node doesn't point to an ast node");
      return classAstNode;
    }

private:
    size_t id{};
    std::string nodeName{};
    Node* parent{nullptr};
    std::vector<Node*> children{};
    mcool::ast::CoolClass* classAstNode{nullptr};
  };

  Graph() = default;
  Graph(const Graph& other) = default;
  std::unordered_map<std::string, Node>& getNodes() { return nodes; }
  const type::Graph::Node& getInheritanceNode(const std::string& name);
  bool containsNode(const std::string& name) { return nodes.find(name) != nodes.end(); }
  size_t size() { return nodes.size(); }

  auto begin() { return nodes.begin(); }
  auto cbegin() { return nodes.cbegin(); }
  auto end() { return nodes.end(); }
  auto cend() { return nodes.cend(); }
  auto findNode(const std::string& name) { return nodes.find(name); }

  void printGraph(std::ostream& os) const;

  private:
  std::unordered_map<std::string, Node> nodes;
  size_t nodeCounter{0};
};

const Graph::Node* findCommonParentType(const Graph::Node* node1, const Graph::Node* node2);
void findInheritanceNodes(Graph::Node* node, std::vector<Graph::Node*>& nodes);
void findInheritanceNodes(const type::Graph::Node& node, std::vector<Graph::Node*>& nodes);
} // namespace mcool::type
