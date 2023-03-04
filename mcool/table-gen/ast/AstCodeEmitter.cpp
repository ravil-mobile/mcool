#include "AstCodeEmitter.h"
#include "InheritanceTree.h"
#include "GeneratorAux.h"


namespace ast {
void AstCodeEmitter::visitRootNode(inheritance::tree::RootNode* node) {
  bodyStream << "class StringPtr {\n";
  bodyStream << "public:\n";
  bodyStream << "  explicit StringPtr(std::string& str) : str(str) {}\n";
  bodyStream << "  std::string& get() { return str; }\n";
  bodyStream << "  const std::string& get() const { return str; }\n";
  bodyStream << "private:\n";
  bodyStream << "  std::string str{};\n";
  bodyStream << "};\n\n";

  auto className = node->getName();
  bodyStream << "class " << className << " {\n";
  bodyStream << "public:\n";

  auto inheritanceChain = getInheritanceChain(node);
  auto constructor = genConstructor(className, inheritanceChain);
  bodyStream << constructor;

  bodyStream << "  virtual ~Node() = default;\n";
  bodyStream << "  virtual void accept(Visitor*) = 0;\n";
  bodyStream << "  virtual const std::string& getClassName() = 0;\n\n";

  bodyStream << "  void setLocation(mcool::Loc& loc) { location = loc; }\n";
  bodyStream << "  mcool::Loc& getLocation() { return location; }\n\n";

  bodyStream << "  void setSemantType(mcool::type::Type* type) { semantType = type; }\n";
  bodyStream << "  mcool::type::Type* getSemantType() { return semantType; }\n";

  ast::genGetters(bodyStream, node->getAttributes());
  ast::genSetters(bodyStream, node->getAttributes());

  genAttributes(node);
  bodyStream << "  mcool::Loc location;\n";
  bodyStream << "  mcool::type::Type* semantType{nullptr};\n";

  bodyStream << "};\n\n";

  for (auto& child : node->getChildren()) {
    child->accept(this);
  }

  OS << headerStream.str() << '\n';
  OS << bodyStream.str();
}

void AstCodeEmitter::visitInnerNode(inheritance::tree::InnerNode* node) {
  auto className = node->getName();
  headerStream << "class " << className << ";\n";
  bodyStream << "class " << className << " : public " << node->getParent()->getName() << " {\n";
  bodyStream << "public:\n";

  auto inheritanceChain = getInheritanceChain(node);
  auto constructor = genConstructor(className, inheritanceChain);
  bodyStream << constructor;

  ast::genGetters(bodyStream, node->getAttributes());
  ast::genSetters(bodyStream, node->getAttributes());

  genAttributes(node);

  bodyStream << "};\n\n";

  for (auto& child : node->getChildren()) {
    child->accept(this);
  }
}

void AstCodeEmitter::visitLeaf(inheritance::tree::LeafNode* node) {
  auto className = node->getName();
  headerStream << "class " << className << ";\n";
  bodyStream << "class " << className << " : public " << node->getParent()->getName() << " {\n";
  bodyStream << "public:\n";

  auto inheritanceChain = getInheritanceChain(node);
  auto constructor = genConstructor(className, inheritanceChain);
  bodyStream << constructor;

  bodyStream << "  void accept(Visitor* visitor) override { visitor->visit" << node->getName() << "(this); }\n";
  bodyStream << "  const std::string& getClassName() override { return className; }\n";

  ast::genGetters(bodyStream, node->getAttributes());
  ast::genSetters(bodyStream, node->getAttributes());

  genAttributes(node);

  bodyStream << "};\n\n";
}

void AstCodeEmitter::genAttributes(inheritance::tree::Node* node) {
  auto attrs = node->getAttributes();

  bodyStream << "\nprotected: \n";

  for (auto *attr: attrs) {
    bodyStream << "  " << attr->type->getName() << " " << attr->name << ";\n";
  }
  bodyStream << "  " << "static const inline std::string className{\"" << node->getName() << "\"};\n";
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