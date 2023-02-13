#include "CodeGen/CodeGenDriver.h"
#include <iostream>

namespace mcool::codegen {
bool CodeGenDriver::run(mcool::AstTree& classes) {
  llvmContext = std::make_unique<llvm::LLVMContext>();
  llvmModule = std::make_unique<llvm::Module>("mcool-module", *llvmContext);
  llvmBuilder = std::make_unique<llvm::IRBuilder<>>(*llvmContext);

  std::error_code error{};
  llvm::raw_fd_ostream fstream(llvm::StringRef(config.outputFile), error);
  if (not error) {
    llvmModule->print(fstream, nullptr);
  } else {
    std::cerr << "cannot open file: `" << config.outputFile << "`\n";
    return false;
  }
  fstream.close();
  return true;
}
} // namespace mcool::codegen