#pragma once

#include "InheritanceTreeVisitor.h"
#include "llvm/Support/raw_ostream.h"
#include <sstream>

namespace ast {
class VisitorCodeEmitter : public Visitor {
public:
  explicit VisitorCodeEmitter(llvm::raw_ostream& stream) : OS(stream) {}

  void visitRootNode(inheritance::tree::RootNode* root) override;
  void visitInnerNode(inheritance::tree::InnerNode* node) override;
  void visitLeaf(inheritance::tree::LeafNode* leaf) override;

private:
  llvm::raw_ostream& OS;
  std::stringstream DS;
};
}
