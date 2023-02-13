#pragma once

#include "Parser/AstTree.h"
#include "Context.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include <ostream>

namespace mcool::codegen {
class CodeGenDriver {
  public:
  CodeGenDriver(mcool::Context& context, misc::Config& config)
      : coolContext(context), config(config) {}
  bool run(mcool::AstTree& classes);

  private:
  mcool::Context& coolContext;
  misc::Config& config;

  std::unique_ptr<llvm::LLVMContext> llvmContext;
  std::unique_ptr<llvm::Module> llvmModule;
  std::unique_ptr<llvm::IRBuilder<>> llvmBuilder;
};
} // namespace mcool::codegen