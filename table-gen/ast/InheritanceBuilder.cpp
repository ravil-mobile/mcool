#include "InheritanceBuilder.h"
#include <cassert>
#include <iostream>


namespace ast::inheritance {
std::unique_ptr<tree::Node> InheritanceBuilder::build(std::vector<llvm::Record*>& records) {
  this->createNodes(records);
  this->assignChildren();

  auto& node = graph.nodes.at("Node");
  auto treeRoot = InheritanceBuilder::createTreeNode(&node);
  this->buildTreeRoot(treeRoot.get());

  return treeRoot;
}

void InheritanceBuilder::createNodes(std::vector<llvm::Record*>& records) {
  for (auto& record: records) {
    auto recordName = record->getName().str();
    graph.nodes.insert(std::make_pair(record->getName().str(), Graph::Node(record)));
  }
}

void InheritanceBuilder::assignChildren() {
  for(auto& item : graph.nodes) {
    auto& currNode = item.second;
    auto type = currNode.record->getType();

    if (type->getClasses().empty()) {
      assert(currNode.record->getName() == "Node" || currNode.record->getName() == "StringPtr");
    }
    else {
      assert(type->getClasses().size() == 1);
      for (auto* parentRecord : type->getClasses()) {
        auto& parentNode = graph.nodes.at(parentRecord->getName().str());
        parentNode.children.push_back(&currNode);
      }
    }
  }
}

void InheritanceBuilder::buildTreeRoot(tree::Node* node) {
  assert(node != nullptr);
  if (node->isLeaf()) return;

  auto nodeName = node->getName();
  auto &graphNode = graph.nodes.at(nodeName);

  for (auto *graphChild: graphNode.children) {
    auto childTreeNode = InheritanceBuilder::createTreeNode(graphChild);
    childTreeNode->setParent(node);
    buildTreeRoot(childTreeNode.get());
    node->addChild(std::move(childTreeNode));
  }
}

std::unique_ptr<tree::Node> InheritanceBuilder::createTreeNode(Graph::Node* graphNode) {
  auto* record = graphNode->record;
  auto* recordType = record->getType();
  bool isRoot = recordType->getClasses().empty();
  bool isLeaf = graphNode->children.empty();

  if (isRoot) {
    return std::make_unique<tree::RootNode>(record, isLeaf);
  }
  else {
    if (isLeaf) {
      return std::make_unique<tree::LeafNode>(record);
    }
    else {
      return std::make_unique<tree::InnerNode>(record);
    }
  }
}
}