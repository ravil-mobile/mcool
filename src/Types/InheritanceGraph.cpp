#include "Types/InheritanceGraph.h"
#include <cassert>
#include <utility>
#include <functional>

namespace mcool::type {
Graph::Node::Node(mcool::ast::CoolClass* astNode) : classAstNode(astNode) {
  assert(astNode != nullptr);
  this->nodeName = std::string(astNode->getCoolType()->getNameAsStr());
  this->id = std::hash<std::string>{}(this->nodeName);
}

Graph::Node::Node(std::string name) : nodeName(std::move(name)) {
  this->id = std::hash<std::string>{}(this->nodeName);
}

bool Graph::Node::isSame(Node* other) const { return this->id == other->id; }

bool Graph::Node::isChildOf(const Node* other) const {
  if (this->isSame(const_cast<Node*>(other))) {
    return true;
  } else {
    return (this->parent != nullptr) && parent->isChildOf(other);
  }
}

const type::Graph::Node& Graph::getInheritanceNode(const std::string& name) {
  if (containsNode(name)) {
    return nodes.find(name)->second;
  } else {
    throw std::runtime_error("no graph node found with `" + name + "` name");
  }
}

const Graph::Node* findCommonParentType(const Graph::Node* node1, const Graph::Node* node2) {
  if (node1->isNodeName("Object")) {
    return node1;
  } else if (node2->isNodeName("Object")) {
    return node2;
  } else if (node1->isSame(const_cast<Graph::Node*>(node2))) {
    return node1;
  } else {
    const Graph::Node* parent1 = node1->getParent();
    if (node2->isChildOf(parent1)) {
      return parent1;
    } else {
      return findCommonParentType(parent1, node2);
    }
  }
}

void findInheritanceNodes(Graph::Node* node, std::vector<Graph::Node*>& nodes) {
  if (node == nullptr) {
    return;
  } else {
    nodes.push_back(node);
    findInheritanceNodes(node->getParent(), nodes);
  }
}

void Graph::printGraph(std::ostream& os) const {
  for (auto& node : nodes) {
    os << node.second.getNodeName() << ": ";
    for (auto& child : node.second.getChildren()) {
      os << child->getNodeName() << ", ";
    }
    os << '\n';
  }
  os << std::endl;
}

} // namespace mcool::type