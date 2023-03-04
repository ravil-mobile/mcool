#include "Semant/TypeChecker/InheritanceGraphBuilder.h"
#include "Misc.h"
#include <algorithm>
#include <sstream>

mcool::semant::InheritanceGraphBuilder::InheritanceGraphBuilder() {
  graph = std::make_unique<type::Graph>();
}

void mcool::semant::InheritanceGraphBuilder::initGraph(const std::list<ast::CoolClass*>& classes) {
  for (auto& coolClass : classes) {
    auto& className{coolClass->getCoolType()->getNameAsStr()};

    auto& nodes = graph->getNodes();
    if (not graph->containsNode(className)) {
      nodes.insert(std::make_pair(className, coolClass));
    } else {
      std::stringstream errStream;
      errStream << "found a second class definition `" << className
                << "` at : " << coolClass->getLocation();
      errList.push_back(errStream.str());
    }
  }
}

void mcool::semant::InheritanceGraphBuilder::checkParentClassDefinitions() {
  std::vector<std::string> removeCandidates{};
  removeCandidates.reserve(graph->size());

  auto& nodes = graph->getNodes();
  for (auto& node : nodes) {
    if (auto* classPtr = node.second.getCoolClass()) {
      if (node.second.getNodeName() == "Object")
        continue;

      auto& parentName = classPtr->getParentType()->getNameAsStr();
      if (not graph->containsNode(parentName)) {
        auto className = classPtr->getCoolType()->getNameAsStr();
        std::stringstream errStream;
        errStream << "class `" << className << "` inherits from class `" << parentName << "` but `"
                  << parentName << "` has not been defined";
        errList.push_back(errStream.str());
        removeCandidates.push_back(className);
      }
    } else {
      assert(classPtr && "nullptr found in checkParentClassDefinitions");
    }
  }

  for (auto& className : removeCandidates) {
    nodes.erase(className);
  }
}

void mcool::semant::InheritanceGraphBuilder::assignParentNodes() {
  auto& nodes = graph->getNodes();
  for (auto& node : nodes) {
    if (auto* classPtr = node.second.getCoolClass()) {
      if (node.second.isNodeName("Object"))
        continue;

      auto& parentName = classPtr->getParentType()->getNameAsStr();
      auto parentIt = nodes.find(parentName);

      if (parentIt != nodes.end()) {
        node.second.setParent(&(parentIt->second));
      } else {
        node.second.setParent(nullptr);
      }
    }
  }
}

void mcool::semant::InheritanceGraphBuilder::assignChildNodes() {
  auto& nodes = graph->getNodes();
  for (auto& node : nodes) {
    if (node.second.isNodeName("Object"))
      continue;
    auto& className = node.second.getNodeName();
    auto& parentName = node.second.getParent()->getNodeName();

    auto& parentNode = nodes.find(parentName)->second;
    auto& selfNode = nodes.find(className)->second;
    parentNode.addChild(&selfNode);
  }
}

void mcool::semant::InheritanceGraphBuilder::visitGraphNode(type::Graph::Node* node,
                                                            std::deque<type::Graph::Node*>& stack,
                                                            std::set<size_t>& discovered) {

  auto isSameNode = [node](type::Graph::Node* otherNode) { return node->isSame(otherNode); };
  bool hasBeenVisisted = std::find_if(stack.begin(), stack.end(), isSameNode) != stack.end();
  if (hasBeenVisisted) {
    // we have already seen this node before
    // it means that we found a cycle
    std::vector<decltype(stack.begin())> toRemove{};
    toRemove.reserve(stack.size());

    std::stringstream errStream;
    errStream << "found a cycle in inheritance: `";
    for (auto it = stack.begin(); it != stack.end(); ++it) {
      errStream << (*it)->getNodeName() << " <- ";
      toRemove.push_back(it);
    }
    errStream << node->getNodeName() << "`";

    for (auto& it : toRemove) {
      stack.erase(it);
    }

    errList.push_back(errStream.str());
    return;
  } else {
    stack.push_back(node);
  }

  for (auto child : node->getChildren()) {
    visitGraphNode(child, stack, discovered);
  }
  stack.pop_back();
  discovered.insert(node->getId());
}

void mcool::semant::InheritanceGraphBuilder::checkInheritanceGraphCycles() {
  std::set<size_t> discovered{};
  for (auto& nodeDescr : graph->getNodes()) {
    auto& node = nodeDescr.second;
    bool hasBeenDiscovered = discovered.find(node.getId()) != discovered.end();
    if (not hasBeenDiscovered) {
      std::deque<type::Graph::Node*> stack{};
      visitGraphNode(&node, stack, discovered);
    }
  }
}
