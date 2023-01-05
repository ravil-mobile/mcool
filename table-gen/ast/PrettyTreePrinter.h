#pragma once

#include <InheritanceTreeVisitor.h>
#include <ostream>


namespace ast {
class PrettyPrinter : public Visitor {
public:
  explicit PrettyPrinter(std::ostream& stream,
                         char delim)
                         : stream(stream),
                           delim(delim) {}

  void visitRootNode(inheritance::tree::RootNode* root) override;
  void visitInnerNode(inheritance::tree::InnerNode* node) override;
  void visitLeaf(inheritance::tree::LeafNode* leaf) override;

private:
  std::ostream& stream;
  size_t indent{0};
  char delim{' '};
};
}