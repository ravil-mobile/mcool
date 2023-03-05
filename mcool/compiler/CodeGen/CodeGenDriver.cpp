#include "CodeGen/CodeGenDriver.h"
#include "CodeGen/Initializer.h"
#include "CodeGen/BuiltinMethodsBuilder.h"
#include "CodeGen/CodeBuilder.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/Host.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/FileSystem.h"
#include <fstream>
#include <iostream>

namespace mcool::codegen {
bool CodeGenDriver::run(mcool::AstTree& classes) {
  bool isOk = initDataLayout();
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

  if (env.coolConfig.emitLLVMIr) {
    isOk = writeLLVMIr();
    if (isOk) {
      readLLVMIr();
    }
  }

  auto fileType = llvm::CGFT_ObjectFile;
  if (env.coolConfig.writeAsmOutput) {
    fileType = llvm::CGFT_AssemblyFile;
  }

  isOk = writeOutputFile(fileType);
  return isOk;
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

bool CodeGenDriver::writeOutputFile(llvm::CodeGenFileType fileType) {
  const std::string fileSuffix = (fileType == llvm::CGFT_AssemblyFile) ? ".s" : ".o";
  auto outputFile = env.coolConfig.outputFile + fileSuffix;
  std::error_code errorCode;
  llvm::raw_fd_ostream dest(outputFile, errorCode, llvm::sys::fs::OF_None);

  if (errorCode) {
    llvm::errs() << "Could not open file: " << errorCode.message() << '\n';
    return false;
  }

  llvm::legacy::PassManager pass;
  if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
    llvm::errs() << "TargetMachine can't emit a file of this type" << '\n';
    return false;
  }

  pass.run(*env.llvmModule);
  dest.flush();
  return true;
}

bool CodeGenDriver::writeLLVMIr() {
  auto outputFile = env.coolConfig.outputFile + ".ll";
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

bool CodeGenDriver::readLLVMIr() {
  auto outputFile = env.coolConfig.outputFile + ".ll";
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