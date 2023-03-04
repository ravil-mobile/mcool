#pragma once

#include "Misc.h"
#include "InheritanceTreeVisitor.h"
#include "llvm/Support/raw_ostream.h"
#include "OrderedDict.h"
#include <list>
#include <tuple>
#include <sstream>


namespace ast {
class AstCodeEmitter : public ast::Visitor {
public:
  explicit AstCodeEmitter(llvm::raw_ostream& stream) : OS(stream) {}

  void visitRootNode(inheritance::tree::RootNode* root) override;
  void visitInnerNode(inheritance::tree::InnerNode* node) override;
  void visitLeaf(inheritance::tree::LeafNode* leaf) override;

private:
  void genAttributes(inheritance::tree::Node* node);
  std::vector<inheritance::tree::Node*> getInheritanceChain(inheritance::tree::Node* node);

  llvm::raw_ostream& OS;
  std::stringstream headerStream;
  std::stringstream bodyStream;
};
} // namespace ast::inheritance