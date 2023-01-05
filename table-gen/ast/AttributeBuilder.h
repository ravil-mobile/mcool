#pragma once

#include "Misc.h"
#include "InheritanceTreeVisitor.h"
#include "llvm/Support/raw_ostream.h"
#include "OrderedDict.h"
#include <list>
#include <tuple>

namespace ast {
class AttributeBuilder : public ast::Visitor {
public:
  void visitRootNode(inheritance::tree::RootNode* root) override;
  void visitInnerNode(inheritance::tree::InnerNode* node) override;
  void visitLeaf(inheritance::tree::LeafNode* leaf) override;
};
} // namespace ast