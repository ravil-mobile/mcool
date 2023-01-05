#pragma once

namespace ast::inheritance::tree {
  class RootNode;
  class InnerNode;
  class LeafNode;
  class Node;
} // ast::inheritance::tree

namespace ast {
  class Visitor {
  public:
    virtual void visitRootNode(inheritance::tree::RootNode* root) = 0;
    virtual void visitInnerNode(inheritance::tree::InnerNode* node) = 0;
    virtual void visitLeaf(inheritance::tree::LeafNode* leaf) = 0;
  };
} // namespace ast::inheritance