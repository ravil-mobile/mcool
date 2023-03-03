#include "PrettyTreePrinter.h"
#include "InheritanceTree.h"
#include <string>


namespace ast {
void PrettyPrinter::visitRootNode(inheritance::tree::RootNode* root) {
  stream << std::string(indent, delim) << root->getName() << std::endl;
  indent += 4;
  for (auto& child : root->getChildren()) {
    child->accept(this);
  }
  indent -= 4;
}


void PrettyPrinter::visitInnerNode(inheritance::tree::InnerNode* node) {
  stream << std::string(indent, delim) << node->getName() << std::endl;
  indent += 4;
  for (auto& child : node->getChildren()) {
    child->accept(this);
  }
  indent -= 4;
}

void PrettyPrinter::visitLeaf(inheritance::tree::LeafNode* leaf) {
  stream << std::string(indent, delim) << leaf->getName() << std::endl;
}
}