#include "CodeGen/CodeBuilder.h"
#include "CodeGen/Misc.h"
#include "llvm/IR/Verifier.h"

namespace mcool::codegen {
void CodeBuilder::callParentsConstructors(llvm::Value* objPtr,
                                          std::vector<type::Graph::Node*>& inheritanceChain) {
  inheritanceChain.erase(inheritanceChain.begin());
  for (auto it = inheritanceChain.rbegin(); it != inheritanceChain.rend(); ++it) {
    auto* parentCoolClass = (*it)->getCoolClass();
    auto& parentCoolTypeName = parentCoolClass->getCoolType()->getNameAsStr();

    auto* parentCoolType = llvm::StructType::getTypeByName(*context, parentCoolTypeName);
    assert(parentCoolType != nullptr);
    auto* parentCoolPtrType = llvm::PointerType::get(parentCoolType, 0);

    auto* castetObjPtr = builder->CreateBitCast(objPtr, parentCoolPtrType);
    std::string parentConstructorName = getConstructorName(parentCoolTypeName);

    auto* parentConstructor = module->getFunction(parentConstructorName);
    assert(parentConstructor != nullptr);
    builder->CreateCall(parentConstructor, castetObjPtr);
  }
}

void CodeBuilder::genConstructors(mcool::AstTree& classes) {
  for (auto* coolClass : classes.get()->getData()) {
    currClassName = coolClass->getCoolType()->getNameAsStr();
    currSymbolTable = codegen::SymbolTable{};

    auto constructorName = getConstructorName(currClassName);
    auto* constructor = module->getFunction(constructorName);

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(*context, "entry", constructor);
    builder->SetInsertPoint(BB);

    llvm::Value* selfPtr = constructor->getArg(0);
    llvm::Value* selfPtrAddress = genAlloca(selfPtr->getType());
    builder->CreateStore(selfPtr, selfPtrAddress);
    currSymbolTable.add("self", selfPtrAddress);

    {
      auto &graph = env.coolContext.getInheritanceGraph();
      auto &coolClassNode = graph->getInheritanceNode(currClassName);

      std::vector<type::Graph::Node *> inheritanceChain{};
      type::findInheritanceNodes(coolClassNode, inheritanceChain);
      callParentsConstructors(constructor->getArg(0), inheritanceChain);

      bool isIntegralType = (currClassName == "Bool") || (currClassName == "Int");
      if (not isIntegralType) {
        auto &membersTable = env.globalMembersTable[currClassName];
        auto &topScope = membersTable.getTopScope();
        for (auto &item: topScope) {
          assert(item->member != nullptr);
          item->member->accept(this);
          popStack();
        }
      }
    }

    builder->CreateRet(constructor->getArg(0));
    llvm::verifyFunction(*constructor, &(llvm::errs()));
  }
}

void CodeBuilder::generatedMainEntryPoint() {
  auto* intType = llvm::Type::getInt32Ty(*context);
  auto* funcType = llvm::FunctionType::get(intType, {}, false);
  auto* function =
      llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "main", module.get());

  llvm::BasicBlock* BB = llvm::BasicBlock::Create(*context, "entry", function);
  builder->SetInsertPoint(BB);

  auto* coolMainPtrType = getPtrType("Main");
  auto* coolObjectPtrType = getPtrType("Object");

  auto protoName = getProtoName("Main");
  auto* protoMain = module->getGlobalVariable(protoName, true);
  assert(protoMain != nullptr);

  auto copyMethodName= getMethodName("Object", "copy");
  auto* objCopyMethod = module->getFunction(copyMethodName);
  assert(objCopyMethod != nullptr);

  auto* objPtr = builder->CreateBitCast(protoMain, coolObjectPtrType);
  auto* newObjPtr = builder->CreateCall(objCopyMethod, objPtr);
  auto* newMainPtr = builder->CreateBitCast(newObjPtr, coolMainPtrType);

  auto constructorName = getConstructorName("Main");
  auto* mainInitMethod = module->getFunction(constructorName);
  assert(mainInitMethod != nullptr);
  builder->CreateCall(mainInitMethod, {newMainPtr});

  auto mainMethodName = getMethodName("Main", "main");
  auto* mainMainMethod = module->getFunction(mainMethodName);
  assert(mainMainMethod != nullptr);
  builder->CreateCall(mainMainMethod, {newMainPtr});

  auto* zero = llvm::Constant::getIntegerValue(intType, llvm::APInt(32, 0));
  builder->CreateRet(zero);
  llvm::verifyFunction(*function, &(llvm::outs()));
}
} // namespace mcool::codegen