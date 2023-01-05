#include "AstCodeEmitter.h"
#include "InheritanceTree.h"
#include "GeneratorAux.h"
#include <sstream>
#include <iostream>


namespace ast {
void AstCodeEmitter::visitRootNode(inheritance::tree::RootNode* node) {
  auto className = node->getName();
  OS << "class " << className << " {\n";
  OS << "public:\n";

  auto inheritanceChain = getInheritanceChain(node);
  auto constructor = genConstructor(className, inheritanceChain);
  OS << constructor;

  OS << "  virtual ~Node() = default;\n";
  OS << "  virtual void accept(Visitor*) = 0;\n";
  OS << "  virtual std::string getName() = 0;\n\n";

  ast::genGetters(OS, node->getAttributes());
  ast::genSetters(OS, node->getAttributes());

  genAttributes(node);

  OS << "};\n\n";

  for (auto& child : node->getChildren()) {
    child->accept(this);
  }
}

void AstCodeEmitter::visitInnerNode(inheritance::tree::InnerNode* node) {
  auto className = node->getName();
  OS << "class " << className << " : public " << node->getParent()->getName() << " {\n";
  OS << "public:\n";

  auto inheritanceChain = getInheritanceChain(node);
  auto constructor = genConstructor(className, inheritanceChain);
  OS << constructor;

  ast::genGetters(OS, node->getAttributes());
  ast::genSetters(OS, node->getAttributes());

  genAttributes(node);

  OS << "};\n\n";

  for (auto& child : node->getChildren()) {
    child->accept(this);
  }
}

void AstCodeEmitter::visitLeaf(inheritance::tree::LeafNode* node) {
  auto className = node->getName();
  OS << "class " << className << " : public " << node->getParent()->getName() << " {\n";
  OS << "public:\n";

  auto inheritanceChain = getInheritanceChain(node);
  auto constructor = genConstructor(className, inheritanceChain);
  OS << constructor;

  OS << "  void accept(Visitor* visitor) override { visitor->visit" << node->getName() << "(this); }\n";
  OS << "  std::string getName() override { return \"" << node->getName() << "\"; }\n";

  ast::genGetters(OS, node->getAttributes());
  ast::genSetters(OS, node->getAttributes());

  genAttributes(node);

  OS << "};\n\n";
}

void AstCodeEmitter::genAttributes(inheritance::tree::Node* node) {
  auto attrs = node->getAttributes();

  if (not attrs.empty()) {
    OS << "\nprotected: \n";

    for (auto *attr: attrs) {
      OS << "  " << attr->type->getName() << " " << attr->name << ";\n";
    }
  }
}

std::vector<inheritance::tree::Node*>
AstCodeEmitter::getInheritanceChain(inheritance::tree::Node* node) {
  std::vector<inheritance::tree::Node*> nodes{};
  auto* currNode = node;
  while (currNode->getParent() != nullptr) {
    nodes.push_back(currNode);
    currNode = currNode->getParent();
  }
  nodes.push_back(currNode);
  return nodes;
}
} // namespace ast