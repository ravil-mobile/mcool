#pragma once

#include "CodeGen/Environment.h"
#include "llvm/Target/TargetMachine.h"
#include <ostream>


namespace mcool::codegen {
class CodeGenDriver {
  public:
  CodeGenDriver(mcool::Context& context, misc::Config& coolConfig)
      : env(context, coolConfig) {
  }
  bool run(mcool::AstTree& classes);

  private:
  bool initDataLayout();
  bool writeOutputFile(llvm::CodeGenFileType fileType);
  bool writeLLVMIr();
  bool readLLVMIr();

  Environment env;
  llvm::TargetMachine* targetMachine{};
  std::string targetTriple{};
};
} // namespace mcool::codegen