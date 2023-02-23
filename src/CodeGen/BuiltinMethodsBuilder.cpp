#include "CodeGen/BuiltinMethodsBuilder.h"
#include "CodeGen/Misc.h"
#include "llvm/IR/Verifier.h"

namespace mcool::codegen {
void BuiltinMethodsBuilder::build() {
  genPrintfDecl();
  genMallocDecl();
  genFreeDecl();
  genExitDecl();
  genObjectCopy();
  genIOOutString();
  genIOOutInt();
}

void BuiltinMethodsBuilder::genPrintfDecl() {
  auto* intType = llvm::Type::getInt32Ty(*context);
  auto* charPtrType = env.getSystemType(Environment::SystemType::CharPtrType);
  auto* funcType = llvm::FunctionType::get(intType, {charPtrType}, true);
  auto* func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "printf", *module);
  func->setCallingConv(llvm::CallingConv::C);
}

void BuiltinMethodsBuilder::genMallocDecl() {
  auto* bytePtrType = env.getSystemType(Environment::SystemType::BytePtrType);
  auto* sizeType = env.getSystemType(Environment::SystemType::SizeType);
  auto* funcType = llvm::FunctionType::get(bytePtrType, {sizeType}, false);
  auto* func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "malloc", *module);
  func->setCallingConv(llvm::CallingConv::C);
}

void BuiltinMethodsBuilder::genFreeDecl() {
  auto* voidType = llvm::Type::getVoidTy(*context);
  auto* bytePtrType = env.getSystemType(Environment::SystemType::BytePtrType);
  auto* funcType = llvm::FunctionType::get(voidType, {bytePtrType}, false);
  auto* func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "free", *module);
  func->setCallingConv(llvm::CallingConv::C);
}

void BuiltinMethodsBuilder::genExitDecl() {
  auto* voidType = llvm::Type::getVoidTy(*context);
  auto* intType = llvm::Type::getInt32Ty(*context);
  auto* funcType = llvm::FunctionType::get(voidType, intType, false);
  auto* func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "exit", *module);
  func->setCallingConv(llvm::CallingConv::C);
}

void BuiltinMethodsBuilder::genObjectCopy() {
  auto* coolObjectType = getType("Object");
  auto* coolObjectPtrType = getPtrType("Object");

  auto methodName = getMethodName("Object", "copy");
  auto* function = module->getFunction(methodName);
  assert(function != nullptr);

  llvm::BasicBlock *BB = llvm::BasicBlock::Create(*context, "entry", function);
  builder->SetInsertPoint(BB);

  auto* objPtr = function->getArg(0);
  auto gepIndices = getGepIndices({0, 2});
  auto* idx = builder->CreateGEP(coolObjectType, objPtr, gepIndices);

  auto* sizeType = env.getSystemType(Environment::SystemType::SizeType);
  auto* objSize = builder->CreateLoad(sizeType, idx);

  auto* mallocType = module->getFunction("malloc");
  assert(mallocType != nullptr);
  auto* memory = builder->CreateCall(mallocType, objSize);

  builder->CreateMemCpy(memory,
                        memory->getParamAlign(0),
                        objPtr,
                        objPtr->getParamAlign(),
                        objSize);

  auto* returnObj = builder->CreateBitCast(memory, coolObjectPtrType);
  builder->CreateRet(returnObj);

  llvm::verifyFunction(*function, &(llvm::outs()));
}

void BuiltinMethodsBuilder::genIOOutString() {
  auto* coolIOPtrType = getPtrType("IO");
  auto* coolStringPtrType = getPtrType("String");
  auto* coolObjectPtrType = getPtrType("Object");

  auto methodName = getMethodName("IO", "out_string");
  auto* function = module->getFunction(methodName);
  assert(function != nullptr);

  llvm::BasicBlock *BB = llvm::BasicBlock::Create(*context, "entry", function);
  builder->SetInsertPoint(BB);

  auto* stringPtr = function->getArg(1);
  auto* coolStringType = getType("String");
  auto gepIndices = getGepIndices({0, 5});
  auto* idx = builder->CreateGEP(coolStringType, stringPtr, gepIndices);
  auto* address = builder->CreateLoad(idx);

  auto* printf = module->getFunction("printf");
  assert(printf != nullptr);
  builder->CreateCall(printf, {address});

  auto* result = builder->CreateBitCast(function->getArg(0), coolObjectPtrType);
  builder->CreateRet(result);
  llvm::verifyFunction(*function, &(llvm::outs()));
}

void BuiltinMethodsBuilder::genIOOutInt() {
  auto* coolIOPtrType = getPtrType("IO");
  auto* coolIntPtrType = getPtrType("Int");
  auto* coolObjPtrType = getPtrType("Object");

  auto methodName = getMethodName("IO", "out_int");
  auto* function = module->getFunction(methodName);
  assert(function != nullptr);

  llvm::BasicBlock *BB = llvm::BasicBlock::Create(*context, "entry", function);
  builder->SetInsertPoint(BB);

  auto* intPtr = function->getArg(1);
  auto* coolIntType = getType("Int");

  auto gepIndices = getGepIndices({0, 4});
  auto* idx = builder->CreateGEP(coolIntType, intPtr, gepIndices);
  auto* intValue = builder->CreateLoad(idx);

  auto* str = builder->CreateGlobalStringPtr("%d", "", 0, module.get());

  auto* printf = module->getFunction("printf");
  assert(printf != nullptr);
  builder->CreateCall(printf, {str, intValue});

  auto* result = builder->CreateBitCast(function->getArg(0), coolObjPtrType);
  builder->CreateRet(result);
  llvm::verifyFunction(*function, &(llvm::outs()));
}
} // namespace mcool::codegen