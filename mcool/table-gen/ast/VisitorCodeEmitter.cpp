#include "VisitorCodeEmitter.h"
#include "InheritanceTree.h"
#include "Misc.h"


namespace ast {
  void VisitorCodeEmitter::visitRootNode(inheritance::tree::RootNode* root) {
    for (auto& child : root->getChildren()) {
      child->accept(this);
    }

    OS << '\n' << "class Visitor {\n";
    OS << "public:\n";
    OS << "  virtual ~Visitor() = default;\n";

    OS << DS.str();

    OS << "};\n";
  }

  void VisitorCodeEmitter::visitInnerNode(inheritance::tree::InnerNode* node) {
    for (auto& child : node->getChildren()) {
      child->accept(this);
    }
  }

  void VisitorCodeEmitter::visitLeaf(inheritance::tree::LeafNode* node) {
    auto className = node->getName();
    OS << "class " << className << ";\n";

    DS << "  virtual void visit"
       << ast::misc::capitalize(className)
       << "(" << className << "*) {};\n";
  }
}