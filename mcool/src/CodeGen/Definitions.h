#pragma once

#include "llvm/IR/Value.h"

#include "ast.h"
#include "Types/SymbolTable.h"
#include <unordered_map>

namespace mcool::codegen {
struct MembersTableData {
  ast::SingleMember* member{};
  int offset{};
};

struct MethodsTableData {
  ast::CoolClass* owner{};
  ast::SingleMethod* method{};
  int offset{};
};

using MembersTable = mcool::type::SymbolTable<std::string, MembersTableData>;
using GlobalMembersTable = std::unordered_map<std::string, MembersTable>;

using MethodsTable = mcool::type::SymbolTable<std::string, MethodsTableData>;
using GlobalMethodsTable = std::unordered_map<std::string, MethodsTable>;

using SymbolTable = mcool::type::SymbolTable<std::string, llvm::Value*>;
using GlobalSymbolTable = std::unordered_map<std::string, SymbolTable>;
} // namespace mcool::codegen