#include "AstCodeEmitter.h"
#include "InheritanceTree.h"
#include "GeneratorAux.h"
#include <sstream>
#include <iostream>


namespace ast {
void AstCodeEmitter::visitRootNode(inheritance::tree::RootNode* node) {
  OS << "class StringPtr {\n";
  OS << "public:\n";
  OS << "  explicit StringPtr(std::string& str) : str(str) {}\n";
  OS << "  std::string& get() { return str; }";
  OS << "  const std::string& get() const { return str; }";
  OS << "private:\n";
  OS << "  std::string str{};\n";
  OS << "};\n\n";

  auto className = node->getName();
  OS << "class " << className << " {\n";
  OS << "public:\n";

  auto inheritanceChain = getInheritanceChain(node);
  auto constructor = genConstructor(className, inheritanceChain);
  OS << constructor;

  OS << "  virtual ~Node() = default;\n";
  OS << "  virtual void accept(Visitor*) = 0;\n";
  OS << "  virtual const std::string& getClassName() = 0;\n\n";

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
  OS << "  const std::string& getClassName() override { return className; }\n";

  ast::genGetters(OS, node->getAttributes());
  ast::genSetters(OS, node->getAttributes());

  genAttributes(node);

  OS << "};\n\n";
}

void AstCodeEmitter::genAttributes(inheritance::tree::Node* node) {
  auto attrs = node->getAttributes();

  OS << "\nprotected: \n";

  for (auto *attr: attrs) {
    OS << "  " << attr->type->getName() << " " << attr->name << ";\n";
  }
  OS << "  " << "static const inline std::string className{\"" << node->getName() << "\"};\n";
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