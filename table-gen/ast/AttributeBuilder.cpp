#include "AttributeBuilder.h"
#include "InheritanceTree.h"
#include "Misc.h"


namespace ast {
void AttributeBuilder::visitRootNode(inheritance::tree::RootNode* root) {
  auto attrs = ast::misc::getAttributes(root->getRecord()->getValues());
  for (size_t i{0}; i < attrs.size(); ++i) {
    root->addAttribute(attrs[i].name, std::move(attrs[i]));
  }

  for (auto& child : root->getChildren()) {
    child->accept(this);
  }
}

void AttributeBuilder::visitInnerNode(inheritance::tree::InnerNode* node) {
  auto* parent = node->getParent();
  auto attrs = ast::misc::getAttributes(node->getRecord()->getValues());
  for (size_t i{0}; i < attrs.size(); ++i) {
    if (not (parent->containsAttribute(attrs[i].name))) {
      node->addAttribute(attrs[i].name, std::move(attrs[i]));
    }
  }

  for (auto& child : node->getChildren()) {
    child->accept(this);
  }
}

void AttributeBuilder::visitLeaf(inheritance::tree::LeafNode* node) {
  auto* parent = node->getParent();
  auto attrs = ast::misc::getAttributes(node->getRecord()->getValues());
  for (size_t i{0}; i < attrs.size(); ++i) {
    if (not (parent->containsAttribute(attrs[i].name))) {
      node->addAttribute(attrs[i].name, std::move(attrs[i]));
    }
  }
}
} // namespace ast
