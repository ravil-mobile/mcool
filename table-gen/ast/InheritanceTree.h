#pragma once

#include "InheritanceTreeVisitor.h"
#include "OrderedDict.h"
#include "Misc.h"
#include "llvm/TableGen/Record.h"
#include <list>

namespace ast::inheritance::tree {
  class Node {
  public:
    explicit Node(llvm::Record* record) : record(record) {}
    virtual void accept(ast::Visitor* visitor) = 0;
    virtual bool isLeaf() = 0;

    std::string getName() {
      assert(record != nullptr);
      return record->getName().str();
    }

    llvm::Record* getRecord() {
      assert(record != nullptr);
      return record;
    }

    void setParent(Node* node) {
      parent = node;
    }

    Node* getParent() {
      return parent;
    }

    void addChild(std::unique_ptr<Node>&& node) {
      children.push_back(std::move(node));
    }

    std::list<std::unique_ptr<Node>>& getChildren() {
      return children;
    }

    void addAttribute(const std::string& name, ast::Attribute&& attr) {
      attributes.insert(name, std::move(attr));
    }

    bool containsAttribute(const std::string& name) {
      if (parent == nullptr) {
        return attributes.contains(name);
      }

      bool contains = parent->containsAttribute(name);
      if (contains) {
        return true;
      }
      else {
        return attributes.contains(name);
      }
    }

    auto getAttributes() {
      auto keys = attributes.keys();

      std::vector<Attribute*> values{};
      values.reserve(keys.size());

      for(auto& key : keys) {
        values.push_back(&attributes[key]);
      }
      return values;
    }

  private:
    llvm::Record* record{nullptr};
    Node* parent{nullptr};
    std::list<std::unique_ptr<Node>> children{};
    mcool::OrderedDict<std::string, ast::Attribute> attributes{};
  };

  class RootNode : public Node {
  public:
    explicit RootNode(llvm::Record* record, bool isLeafNode) : Node(record), isLeafNode(isLeafNode) {}
    void accept(ast::Visitor* visitor) override {
      visitor->visitRootNode(this);
    }
    bool isLeaf() override { return isLeafNode; }

  private:
    bool isLeafNode{false};
  };

  class InnerNode : public Node {
  public:
    explicit InnerNode(llvm::Record* record) : Node(record) {}
    void accept(ast::Visitor* visitor) override {
      visitor->visitInnerNode(this);
    }
    bool isLeaf() override { return false; }
  };

  class LeafNode : public Node {
  public:
    explicit LeafNode(llvm::Record* record) : Node(record) {}
    void accept(ast::Visitor* visitor) override {
      visitor->visitLeaf(this);
    }
    bool isLeaf() override { return true; }
  };
}