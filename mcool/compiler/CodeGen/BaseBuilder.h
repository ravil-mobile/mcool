#pragma once

#include "CodeGen/Environment.h"
#include "CodeGen/Misc.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/IRBuilder.h"

namespace mcool::codegen {
class BaseBuilder {
  public:
  explicit BaseBuilder(Environment& env)
      : env(env), context(env.llvmContext), module(env.llvmModule), builder(env.llvmBuilder) {}

  llvm::Type* getType(const std::string& className) {
    auto* classType = llvm::StructType::getTypeByName(*context, className);
    assert(classType != nullptr);
    return classType;
  }

  llvm::Type* getPtrType(const std::string& className) {
    auto* classType = getType(className);
    auto* classPtrType = llvm::PointerType::get(classType, 0);
    assert(classPtrType != nullptr);
    return classPtrType;
  }

  auto getGepIndices(const std::initializer_list<int>& indices) {
    llvm::SmallVector<llvm::Value*> result;
    for (auto item : indices) {
      result.push_back(builder->getInt32(item));
    }
    return result;
  }

  auto* createNewClassInstanceOnHeap(const std::string& className) {
    auto protoName = getProtoName(className);
    auto* proto = module->getGlobalVariable(protoName, true);
    assert(proto != nullptr);

    auto* coolObjectPtrType = getPtrType("Object");
    auto* objPtr = builder->CreateBitCast(proto, coolObjectPtrType);

    auto copyObjectMethodName = getMethodName("Object", "copy");
    auto* copyObjectMethod = module->getFunction(copyObjectMethodName);
    auto* newObject = builder->CreateCall(copyObjectMethod, objPtr);

    return builder->CreateBitCast(newObject, getPtrType(className));
  }

  auto* createNewClassInstanceOnStack(const std::string& className) {
    auto protoName = getProtoName(className);
    auto* proto = module->getGlobalVariable(protoName, true);
    assert(proto != nullptr);

    auto* coolObjectType = getType(className);
    auto* numElements = llvm::Constant::getIntegerValue(builder->getInt32Ty(), llvm::APInt(32, 1));

    auto* instancePtr = genAlloca(coolObjectType, numElements);
    auto* sizePtr = builder->CreateGEP(proto, getGepIndices({0, 2}));
    auto* size = builder->CreateLoad(sizePtr);
    builder->CreateMemCpy(instancePtr, stdAlign, proto, stdAlign, size);

    return instancePtr;
  }

  llvm::Value* copyObject(llvm::Value* objPtr) {
    auto copyObjectMethodName = getMethodName("Object", "copy");
    auto* copyObjectMethod = module->getFunction(copyObjectMethodName);

    auto* castedObjPtr = builder->CreateBitCast(objPtr, getPtrType("Object"));
    auto* newObject = builder->CreateCall(copyObjectMethod, castedObjPtr);
    return builder->CreateBitCast(newObject, objPtr->getType());
  }

  void genMemcpy(llvm::Value* dst, llvm::Value* src) {
    auto* sizePtr = builder->CreateGEP(src, getGepIndices({0, 2}));
    auto* size = builder->CreateLoad(sizePtr);

    builder->CreateMemCpy(dst, stdAlign, src, stdAlign, size, true);
  }

  void callExit(const std::string& msg, int errCode) {
    auto* printfFunc = module->getFunction("printf");
    assert(printfFunc != nullptr);

    auto* abortMsg = builder->CreateGlobalStringPtr(msg, "", 0, module.get());
    builder->CreateCall(printfFunc, abortMsg);

    auto* abortFunc = module->getFunction("exit");
    assert(abortFunc != nullptr);
    builder->CreateCall(abortFunc, builder->getInt32(errCode));
  }

  llvm::AllocaInst* genAlloca(llvm::Type* ty, llvm::Value* arraySize = nullptr) {
    auto* parentFunction = builder->GetInsertBlock()->getParent();
    llvm::IRBuilder<> tmpBuilder(&(parentFunction->getEntryBlock()),
                                 parentFunction->getEntryBlock().begin());
    return tmpBuilder.CreateAlloca(ty, arraySize);
  }

  llvm::Value* extractClassName(llvm::Value* classInstancePtr) {
    auto* objPtrType = getPtrType("Object");
    auto* objPtr = builder->CreateBitCast(classInstancePtr, objPtrType);
    auto* classTagAddress = builder->CreateGEP(objPtr, getGepIndices({0, 1}));
    llvm::Value* classTag = builder->CreateLoad(classTagAddress);

    llvm::Value* classNameTable = module->getGlobalVariable(getClassNameTableName(), true);
    assert(classNameTable != nullptr);

    auto* classNameAddress =
        builder->CreateInBoundsGEP(classNameTable, {builder->getInt32(0), classTag});
    return builder->CreateLoad(classNameAddress);
  }

  protected:
  Environment& env;
  std::unique_ptr<llvm::LLVMContext>& context;
  std::unique_ptr<llvm::Module>& module;
  std::unique_ptr<llvm::IRBuilder<>>& builder;

  llvm::Align stdAlign{8};
};
} // namespace mcool::codegen