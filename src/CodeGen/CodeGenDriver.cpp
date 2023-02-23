#include "CodeGen/CodeGenDriver.h"
#include "CodeGen/Initializer.h"
#include "CodeGen/BuiltinMethodsBuilder.h"
#include "CodeGen/CodeBuilder.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/Host.h"
#include <fstream>
#include <iostream>

namespace mcool::codegen {
bool CodeGenDriver::run(mcool::AstTree& classes) {
  auto isOk = initDataLayout();
  if (not isOk) {
    return false;
  }

  Initializer initializer(env, classes);
  initializer.run();

  BuiltinMethodsBuilder builtinMethodsBuilder(env);
  builtinMethodsBuilder.build();

  CodeBuilder codeBuilder(env);
  codeBuilder.genConstructors(classes);
  codeBuilder.genMethods(classes);
  codeBuilder.generatedMainEntryPoint();

  bool outputStatus = writeOutput();
  if (outputStatus) {
    readOutput();
  }

  return true;
}

bool CodeGenDriver::initDataLayout() {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  std::string error;
  targetTriple = llvm::sys::getDefaultTargetTriple();
  auto* target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
  if (!target) {
    std::cerr << "codegen error: " << error << '\n';
    return false;
  }

  auto cpu = "generic";
  auto features = "";
  llvm::TargetOptions opt;
  auto relocationModel = llvm::Optional<llvm::Reloc::Model>();
  targetMachine = target->createTargetMachine(targetTriple, cpu, features, opt, relocationModel);
  if (!targetMachine) {
    std::cerr << "codegen error: could not create a target machine\n";
    return false;
  }

  env.llvmModule->setDataLayout(targetMachine->createDataLayout());
  env.llvmModule->setTargetTriple(targetTriple);
  return true;
}


bool CodeGenDriver::writeOutput() {
  auto& outputFile = env.coolConfig.outputFile;
  std::error_code error{};
  llvm::raw_fd_ostream fstream(llvm::StringRef(outputFile), error);
  if (not error) {
    env.llvmModule->print(fstream, nullptr);
  } else {
    std::cerr << "cannot open file: `" << outputFile << "`\n";
    return false;
  }
  fstream.close();
  return true;
}

bool CodeGenDriver::readOutput() {
  auto& outputFile = env.coolConfig.outputFile;
  std::fstream stream(outputFile, std::ios::in);
  if (not stream.fail()) {
    std::string out;
    while (std::getline(stream, out)) {
      std::cout << out << '\n';
    }
  }
  else {
    std::cerr << "cannot open file\n";
    return false;
  }
  return true;
}

} // namespace mcool::codegen