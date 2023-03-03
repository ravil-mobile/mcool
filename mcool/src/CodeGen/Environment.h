#pragma once

#include "Parser/AstTree.h"
#include "Context.h"
#include "CodeGen/Definitions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include <string>
#include <memory>

namespace mcool::codegen {
struct Environment {
  Environment(mcool::Context& context, misc::Config& config)
      : coolContext(context), coolConfig(config) {
    llvmContext = std::make_unique<llvm::LLVMContext>();
    llvmModule = std::make_unique<llvm::Module>("mcool-module", *llvmContext);
    llvmBuilder = std::make_unique<llvm::IRBuilder<>>(*llvmContext);

    systemTypes.insert({SystemType::CharPtrType, llvm::Type::getInt8PtrTy(*llvmContext)});
    systemTypes.insert({SystemType::BytePtrType, llvm::Type::getInt8PtrTy(*llvmContext)});
    systemTypes.insert({SystemType::SizeType, llvm::Type::getInt64Ty(*llvmContext)});
  }

  mcool::Context& coolContext;
  misc::Config& coolConfig;

  std::unique_ptr<llvm::LLVMContext> llvmContext;
  std::unique_ptr<llvm::Module> llvmModule;
  std::unique_ptr<llvm::IRBuilder<>> llvmBuilder;

  GlobalMembersTable globalMembersTable{};
  GlobalMethodsTable globalMethodsTable{};
  GlobalSymbolTable globalSymbolTable{};

  std::unordered_map<std::string, int> classTagTable{};

  enum class SystemType { CharPtrType, BytePtrType, SizeType };
  llvm::Type* getSystemType(SystemType index) { return systemTypes.at(index); }

  std::unordered_map<SystemType, llvm::Type*> systemTypes{};
};
} // namespace mcool::codegen