#pragma once

#include "AstType.h"
#include "llvm/TableGen/Record.h"
#include <optional>
#include <vector>


namespace ast {
enum class NodeKind {
  Terminal,
  NonTerminal
};

struct Attribute {
  std::string name{};
  std::unique_ptr<ast::Type> type;
  std::optional<std::string> initValue{};
};
}

namespace ast::misc {
std::optional<ast::NodeKind> getKind(llvm::RecordRecTy*);
std::optional<std::string> getParentName(llvm::RecordRecTy*);
std::optional<std::string> getInitValue(const llvm::RecordVal*);

std::unique_ptr<ast::Type> buildType(llvm::RecTy*);
std::vector<ast::Attribute> getAttributes(llvm::ArrayRef<llvm::RecordVal>);

std::string capitalize(std::string);
}