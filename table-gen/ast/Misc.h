#pragma once

#include "AstType.h"
#include "llvm/TableGen/Record.h"
#include <optional>
#include <vector>
#include <list>


namespace ast {
enum class NodeKind {
  Terminal,
  NonTerminal
};

struct Attribute {
  bool operator==(const Attribute& other) const {
    return name == other.name;
  }

  std::string name{};
  std::unique_ptr<ast::Type> type;
  std::optional<std::string> initValue{};
};
}

namespace ast::misc {
std::vector<llvm::Record*> collectDefsAndClasses(llvm::RecordKeeper& records);
std::optional<ast::NodeKind> getKind(llvm::Record*);
std::optional<std::string> getInitValue(const llvm::RecordVal*);

std::unique_ptr<ast::Type> buildType(llvm::RecTy*);
std::vector<ast::Attribute> getAttributes(llvm::ArrayRef<llvm::RecordVal>);

std::string capitalize(std::string);
}