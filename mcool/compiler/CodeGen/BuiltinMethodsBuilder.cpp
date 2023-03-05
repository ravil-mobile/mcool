#include "CodeGen/BuiltinMethodsBuilder.h"
#include "CodeGen/Misc.h"
#include "llvm/IR/Verifier.h"

namespace mcool::codegen {
void BuiltinMethodsBuilder::build() {
  genCStdFunctions();
  genClearStdinBuffer();
  genObjectCopy();
  genObjectAbort();
  genObjectTypeName();
  genIOOutString();
  genIOOutInt();
  genIOInInt();
  genIOInString();
  genStringLength();
}

void BuiltinMethodsBuilder::genCStdFunctions() {
  auto* intType = llvm::Type::getInt32Ty(*context);
  auto* charPtrType = env.getSystemType(Environment::SystemType::CharPtrType);
  auto* bytePtrType = env.getSystemType(Environment::SystemType::BytePtrType);
  auto* sizeType = env.getSystemType(Environment::SystemType::SizeType);
  auto* voidType = llvm::Type::getVoidTy(*context);

  {
    auto* funcType = llvm::FunctionType::get(intType, {charPtrType}, true);
    auto* func =
        llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "printf", *module);
    func->setCallingConv(llvm::CallingConv::C);
  }
  {
    auto* funcType = llvm::FunctionType::get(bytePtrType, {sizeType}, false);
    auto* func =
        llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "malloc", *module);
    func->setCallingConv(llvm::CallingConv::C);
  }
  {
    auto* funcType = llvm::FunctionType::get(voidType, {bytePtrType}, false);
    auto* func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "free", *module);
    func->setCallingConv(llvm::CallingConv::C);
  }
  {
    auto* funcType = llvm::FunctionType::get(voidType, intType, false);
    auto* func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "exit", *module);
    func->setCallingConv(llvm::CallingConv::C);
  }
  {
    auto* funcType = llvm::FunctionType::get(intType, {charPtrType}, true);
    auto* func = llvm::Function::Create(
        funcType, llvm::Function::ExternalLinkage, "__isoc99_scanf", *module);
    func->setCallingConv(llvm::CallingConv::C);
  }
  {
    auto* funcType = llvm::FunctionType::get(sizeType, {charPtrType}, true);
    auto* func =
        llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "strlen", *module);
    func->setCallingConv(llvm::CallingConv::C);
  }
  {
    auto* funcType = llvm::FunctionType::get(intType, {}, false);
    auto* func =
        llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "getchar", *module);
    func->setCallingConv(llvm::CallingConv::C);
  }
}

void BuiltinMethodsBuilder::genClearStdinBuffer() {
  auto* voidType = llvm::Type::getVoidTy(*context);
  auto* funcType = llvm::FunctionType::get(voidType, {}, false);
  auto* function = llvm::Function::Create(
      funcType, llvm::Function::ExternalLinkage, "clearStdinBuffer", *module);
  function->setCallingConv(llvm::CallingConv::C);

  auto* entryBB = llvm::BasicBlock::Create(*context, "entry", function);
  auto* bodyBB = llvm::BasicBlock::Create(*context);
  auto* exitBB = llvm::BasicBlock::Create(*context);
  auto* repeatBB = llvm::BasicBlock::Create(*context);

  auto* getchar = module->getFunction("getchar");
  assert(getchar != nullptr);

  builder->SetInsertPoint(entryBB);
  builder->CreateBr(bodyBB);

  function->getBasicBlockList().push_back(bodyBB);
  builder->SetInsertPoint(bodyBB);
  auto* receivedChar = builder->CreateCall(getchar);
  auto* result = builder->CreateICmpEQ(receivedChar, builder->getInt32(static_cast<int>('\n')));
  builder->CreateCondBr(result, exitBB, repeatBB);

  function->getBasicBlockList().push_back(repeatBB);
  builder->SetInsertPoint(repeatBB);
  builder->CreateBr(bodyBB);

  function->getBasicBlockList().push_back(exitBB);
  builder->SetInsertPoint(exitBB);
  builder->CreateRet(nullptr);
  llvm::verifyFunction(*function, &(llvm::errs()));
}

void BuiltinMethodsBuilder::genObjectCopy() {
  auto* coolObjectType = getType("Object");
  auto* coolObjectPtrType = getPtrType("Object");

  auto methodName = getMethodName("Object", "copy");
  auto* function = module->getFunction(methodName);
  assert(function != nullptr);

  llvm::BasicBlock* BB = llvm::BasicBlock::Create(*context, "entry", function);
  builder->SetInsertPoint(BB);

  auto* objPtr = function->getArg(0);
  auto gepIndices = getGepIndices({0, 2});
  auto* idx = builder->CreateGEP(coolObjectType, objPtr, gepIndices);

  auto* sizeType = env.getSystemType(Environment::SystemType::SizeType);
  auto* objSize = builder->CreateLoad(sizeType, idx);

  auto* mallocType = module->getFunction("malloc");
  assert(mallocType != nullptr);
  auto* memory = builder->CreateCall(mallocType, objSize);

  builder->CreateMemCpy(memory, memory->getParamAlign(0), objPtr, objPtr->getParamAlign(), objSize);

  auto* returnObj = builder->CreateBitCast(memory, coolObjectPtrType);
  builder->CreateRet(returnObj);

  llvm::verifyFunction(*function, &(llvm::errs()));
}

void BuiltinMethodsBuilder::genObjectAbort() {
  auto methodName = getMethodName("Object", "abort");
  auto* function = module->getFunction(methodName);
  assert(function != nullptr);

  llvm::BasicBlock* BB = llvm::BasicBlock::Create(*context, "entry", function);
  builder->SetInsertPoint(BB);

  auto* className = extractClassName(function->getArg(0));

  auto* printfFunc = module->getFunction("printf");
  assert(printfFunc != nullptr);
  auto* msg = builder->CreateGlobalStringPtr("calling abort from class: %s\n", "", 0, module.get());
  builder->CreateCall(printfFunc, {msg, className});

  auto* abortFunc = module->getFunction("exit");
  assert(abortFunc != nullptr);
  builder->CreateCall(abortFunc, builder->getInt32(-1));

  builder->CreateRet(function->getArg(0));
  llvm::verifyFunction(*function, &(llvm::errs()));
}

void BuiltinMethodsBuilder::genObjectTypeName() {
  auto methodName = getMethodName("Object", "type_name");
  auto* function = module->getFunction(methodName);
  assert(function != nullptr);

  llvm::BasicBlock* BB = llvm::BasicBlock::Create(*context, "entry", function);
  builder->SetInsertPoint(BB);

  auto* className = extractClassName(function->getArg(0));
  auto* strlenFunc = module->getFunction("strlen");
  assert(strlenFunc != nullptr);
  auto* classNameLength = builder->CreateCall(strlenFunc, className);

  auto* mallocFunc = module->getFunction("malloc");
  assert(mallocFunc != nullptr);
  auto* newStrMemory = builder->CreateCall(mallocFunc, classNameLength);
  builder->CreateMemCpy(newStrMemory, stdAlign, className, stdAlign, classNameLength);

  auto* newIntObject = createNewClassInstanceOnHeap("Int");
  auto* intValueAddress = builder->CreateGEP(newIntObject, getGepIndices({0, 4}));
  auto* castedClassNameLength = builder->CreateTrunc(classNameLength, builder->getInt32Ty());
  builder->CreateStore(castedClassNameLength, intValueAddress);

  auto* newStringObject = createNewClassInstanceOnHeap("String");
  auto* strSizeAddress = builder->CreateGEP(newStringObject, getGepIndices({0, 4}));
  builder->CreateStore(newIntObject, strSizeAddress);
  auto* strPtr = builder->CreateGEP(newStringObject, getGepIndices({0, 5}));
  builder->CreateStore(newStrMemory, strPtr);

  builder->CreateRet(newStringObject);
  llvm::verifyFunction(*function, &(llvm::errs()));
}

void BuiltinMethodsBuilder::genIOOutString() {
  auto* coolObjectPtrType = getPtrType("Object");

  auto methodName = getMethodName("IO", "out_string");
  auto* function = module->getFunction(methodName);
  assert(function != nullptr);

  llvm::BasicBlock* BB = llvm::BasicBlock::Create(*context, "entry", function);
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
  llvm::verifyFunction(*function, &(llvm::errs()));
}

void BuiltinMethodsBuilder::genIOOutInt() {
  auto* coolObjPtrType = getPtrType("Object");

  auto methodName = getMethodName("IO", "out_int");
  auto* function = module->getFunction(methodName);
  assert(function != nullptr);

  llvm::BasicBlock* BB = llvm::BasicBlock::Create(*context, "entry", function);
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
  llvm::verifyFunction(*function, &(llvm::errs()));
}

void BuiltinMethodsBuilder::genIOInInt() {
  auto methodName = getMethodName("IO", "in_int");
  auto* function = module->getFunction(methodName);
  assert(function != nullptr);

  auto* entryBB = llvm::BasicBlock::Create(*context, "entry", function);
  auto* scanfCallBB = llvm::BasicBlock::Create(*context);
  auto* errorMsgDisplayBB = llvm::BasicBlock::Create(*context);
  auto* processingBB = llvm::BasicBlock::Create(*context);
  builder->SetInsertPoint(entryBB);

  auto* str = builder->CreateGlobalStringPtr("%d", "", 0, module.get());
  auto* intValueAddress = builder->CreateAlloca(builder->getInt32Ty());
  builder->CreateBr(scanfCallBB);

  function->getBasicBlockList().push_back(scanfCallBB);
  builder->SetInsertPoint(scanfCallBB);
  auto* scanfFunc = module->getFunction("__isoc99_scanf");
  assert(scanfFunc != nullptr);

  auto* status = builder->CreateCall(scanfFunc, {str, intValueAddress});
  auto* result = builder->CreateICmpSGT(status, builder->getInt32(0));
  builder->CreateCondBr(result, processingBB, errorMsgDisplayBB);

  function->getBasicBlockList().push_back(errorMsgDisplayBB);
  builder->SetInsertPoint(errorMsgDisplayBB);
  auto* printfFunc = module->getFunction("printf");
  assert(printfFunc != nullptr);
  auto* errorMsg = builder->CreateGlobalStringPtr(
      "Failed to read an integer value. Try again\n", "", 0, module.get());
  builder->CreateCall(printfFunc, errorMsg);
  auto* clearStdinBufferFunc = module->getFunction("clearStdinBuffer");
  assert(clearStdinBufferFunc != nullptr);
  builder->CreateCall(clearStdinBufferFunc);
  builder->CreateBr(scanfCallBB);

  function->getBasicBlockList().push_back(processingBB);
  builder->SetInsertPoint(processingBB);

  auto* newIntObj = createNewClassInstanceOnHeap("Int");
  auto* intValue = builder->CreateLoad(intValueAddress);
  auto address = builder->CreateGEP(newIntObj, getGepIndices({0, 4}));
  builder->CreateStore(intValue, address);

  builder->CreateRet(newIntObj);
  llvm::verifyFunction(*function, &(llvm::errs()));
}

void BuiltinMethodsBuilder::genIOInString() {
  auto methodName = getMethodName("IO", "in_string");
  auto* function = module->getFunction(methodName);
  assert(function != nullptr);

  auto* entryBB = llvm::BasicBlock::Create(*context, "entry", function);
  auto* scanfCallBB = llvm::BasicBlock::Create(*context);
  auto* errorMsgDisplayBB = llvm::BasicBlock::Create(*context);
  auto* processingBB = llvm::BasicBlock::Create(*context);

  builder->SetInsertPoint(entryBB);
  auto* mallocFunc = module->getFunction("malloc");
  assert(mallocFunc != nullptr);

  auto* bufferSize = builder->getInt64(1024);
  auto* buffer = builder->CreateCall(mallocFunc, bufferSize);
  builder->CreateBr(scanfCallBB);

  function->getBasicBlockList().push_back(scanfCallBB);
  builder->SetInsertPoint(scanfCallBB);
  auto* str = builder->CreateGlobalStringPtr("%[^\n]", "", 0, module.get());
  auto* scanfFunc = module->getFunction("__isoc99_scanf");
  assert(scanfFunc != nullptr);
  auto* result = builder->CreateCall(scanfFunc, {str, buffer});
  auto* status = builder->CreateICmpSGT(result, builder->getInt32(0));
  builder->CreateCondBr(status, processingBB, errorMsgDisplayBB);

  function->getBasicBlockList().push_back(errorMsgDisplayBB);
  builder->SetInsertPoint(errorMsgDisplayBB);
  auto* printfFunc = module->getFunction("printf");
  assert(printfFunc != nullptr);
  auto* errorMsg =
      builder->CreateGlobalStringPtr("Failed to read a string. Try again\n", "", 0, module.get());
  builder->CreateCall(printfFunc, errorMsg);
  auto* clearStdinBufferFunc = module->getFunction("clearStdinBuffer");
  assert(clearStdinBufferFunc != nullptr);
  builder->CreateCall(clearStdinBufferFunc);
  builder->CreateBr(scanfCallBB);

  function->getBasicBlockList().push_back(processingBB);
  builder->SetInsertPoint(processingBB);

  auto* strlenFunc = module->getFunction("strlen");
  assert(strlenFunc != nullptr);
  auto* stringLength = builder->CreateCall(strlenFunc, buffer);
  auto* newIntObj = createNewClassInstanceOnHeap("Int");
  auto* intValueAddress = builder->CreateGEP(newIntObj, getGepIndices({0, 4}));
  auto* castedStringLength = builder->CreateTrunc(stringLength, builder->getInt32Ty());
  builder->CreateStore(castedStringLength, intValueAddress);

  auto* stringMemory = builder->CreateCall(mallocFunc, stringLength);
  builder->CreateMemCpy(stringMemory, stdAlign, buffer, stdAlign, stringLength);
  auto* freeFunc = module->getFunction("free");
  assert(freeFunc != nullptr);
  builder->CreateCall(freeFunc, buffer);

  auto* newStringObj = createNewClassInstanceOnHeap("String");
  auto* stringSizeAddress = builder->CreateGEP(newStringObj, getGepIndices({0, 4}));
  builder->CreateStore(newIntObj, stringSizeAddress);
  auto* stringMemoryAddress = builder->CreateGEP(newStringObj, getGepIndices({0, 5}));
  builder->CreateStore(stringMemory, stringMemoryAddress);

  builder->CreateRet(newStringObj);
  llvm::verifyFunction(*function, &(llvm::errs()));
}

void BuiltinMethodsBuilder::genStringLength() {
  auto methodName = getMethodName("String", "length");
  auto* function = module->getFunction(methodName);
  assert(function != nullptr);

  auto* entryBB = llvm::BasicBlock::Create(*context, "entry", function);
  builder->SetInsertPoint(entryBB);

  llvm::Value* stringObjPtr = function->getArg(0);
  auto* stringSizeAddress = builder->CreateGEP(stringObjPtr, getGepIndices({0, 4}));
  auto* stringSizeObj = builder->CreateLoad(stringSizeAddress);

  auto* objPtrType = getPtrType("Object");
  auto copyObjFuncName = getMethodName("Object", "copy");
  auto* copyObjFunc = module->getFunction(copyObjFuncName);
  auto* newIntObj =
      builder->CreateCall(copyObjFunc, builder->CreateBitCast(stringSizeObj, objPtrType));
  builder->CreateRet(builder->CreateBitCast(newIntObj, getPtrType("Int")));
  llvm::verifyFunction(*function, &(llvm::errs()));
}
} // namespace mcool::codegen