#include "CodeGen/Initializer.h"
#include "CodeGen/Misc.h"
#include "SymbolTable.h"
#include "llvm/IR/Verifier.h"
#include <vector>

namespace mcool::codegen {
std::vector<llvm::Type*> getCompulsoryTypes(Environment& env) {
  auto& context = *(env.llvmContext);

  std::vector<llvm::Type*> types;
  // Garbage Collector Tag
  types.push_back(llvm::Type::getInt32Ty(context));

  // Class tag
  types.push_back(llvm::Type::getInt32Ty(context));

  // Object size
  auto* sizeType = env.getSystemType(Environment::SystemType::SizeType);
  types.push_back(sizeType);

  return types;
}

void Initializer::genCoolClassTypes() {
  for (auto* coolClass : classes.get()->getData()) {
    auto memberTypes = getCompulsoryTypes(env);

    auto& coolClassName = coolClass->getCoolType()->getNameAsStr();
    auto dispatchTableName = getDispatchTableName(coolClassName);
    auto* dispatchTableType = llvm::StructType::getTypeByName(*context, dispatchTableName + "_type");
    assert(dispatchTableType != nullptr);
    memberTypes.push_back(llvm::PointerType::get(dispatchTableType, 0));

    if (coolClassName == "Int" || coolClassName == "Bool") {
      memberTypes.push_back(llvm::Type::getInt32Ty(*context));
    }
    else if (coolClassName == "String") {
      auto* coolIntType = llvm::StructType::getTypeByName(*context, "Int");
      assert(coolIntType != nullptr);
      auto* coolIntPtrType = llvm::PointerType::get(coolIntType, 0);
      memberTypes.push_back(coolIntPtrType);
      memberTypes.push_back(llvm::Type::getInt8PtrTy(*context));
    }
    else {
      auto& classMembersTable = env.globalMembersTable[coolClassName];
      for (auto& scope : classMembersTable) {
        auto classMembers = scope.values();
        for (auto [member, _] : classMembers) {
          auto memberTypeName = member->getIdType()->getNameAsStr();
          auto* memberLLVMType = llvm::StructType::getTypeByName(*context, memberTypeName);
          auto* memberLLVMTypePtr = llvm::PointerType::get(memberLLVMType, 0);
          memberTypes.push_back(memberLLVMTypePtr);
        }
      }
    }
    auto* coolClassType = llvm::StructType::getTypeByName(*context, coolClassName);
    coolClassType->setBody(memberTypes);
  }
}

void Initializer::genClassDeclarations() {
  for (auto* coolClass : classes.get()->getData()) {
    const auto& coolClassName = coolClass->getCoolType()->getNameAsStr();
    llvm::StructType::create(*context, coolClassName);
  }
}

MembersTable createMembersTable(std::vector<type::Graph::Node*>& inheritanceChain) {
  MembersTable membersTable;
  constexpr unsigned coolClassHeaderSize{4};
  int offsetCounter{coolClassHeaderSize};

  bool addScope{false};
  for (auto it = inheritanceChain.rbegin(); it != inheritanceChain.rend(); ++it) {
    auto childCoolClass = (*it)->getCoolClass();

    if (addScope) {
      membersTable.pushScope();
      addScope = false;
    }

    for (auto* attr : childCoolClass->getAttributes()->getData()) {
      if (auto* member = dynamic_cast<ast::SingleMember*>(attr)) {
        auto data = MembersTableData{member, offsetCounter};
        membersTable.add(member->getId()->getNameAsStr(), data);
        addScope = true;
        ++offsetCounter;
      }
    }
  }
  return membersTable;
}

void Initializer::initGlobalMembersTable() {
  auto& coolContext = env.coolContext;
  auto& graph = coolContext.getInheritanceGraph();

  for (auto* coolClass : classes.get()->getData()) {
    auto& coolClassName = coolClass->getCoolType()->getNameAsStr();
    auto& coolClassNode = graph->getInheritanceNode(coolClassName);

    std::vector<type::Graph::Node*> inheritanceChain{};
    type::findInheritanceNodes(coolClassNode, inheritanceChain);

    auto membersTable = createMembersTable(inheritanceChain);
    env.globalMembersTable.insert({coolClassName, membersTable});
  }
}

auto getLeadingConstants(Environment& env, int classTag, unsigned classSize) {
  auto& module = env.llvmModule;
  auto& context = module->getContext();

  std::vector<llvm::Constant*> constants{};
  auto* intType = llvm::Type::getInt32Ty(context);

  constants.push_back(llvm::Constant::getIntegerValue(intType, llvm::APInt(32, 0)));
  constants.push_back(llvm::Constant::getIntegerValue(intType, llvm::APInt(32, classTag)));

  auto* sizeType = env.getSystemType(Environment::SystemType::SizeType);
  auto apInt = llvm::APInt(sizeType->getIntegerBitWidth(), classSize);
  constants.push_back(llvm::Constant::getIntegerValue(sizeType, apInt));

  return constants;
}

void Initializer::createGlobalVariable(ast::CoolClass* coolClass, const std::string& variableName) {
  auto& dl = module->getDataLayout();

  auto& coolClassName = coolClass->getCoolType()->getNameAsStr();
  auto* coolClassType = llvm::StructType::getTypeByName(*context, coolClassName);

  module->getOrInsertGlobal(variableName, coolClassType);
  auto* proto = module->getNamedGlobal(variableName);
  auto* layout = dl.getStructLayout(coolClassType);
  auto constants = getLeadingConstants(env, coolClass->getTag(), layout->getSizeInBytes());

  auto dispatchTableName = getDispatchTableName(coolClassName);
  auto* dispTablePtr = module->getNamedGlobal(dispatchTableName);
  assert(dispTablePtr != nullptr);
  constants.push_back(dispTablePtr);

  if ((coolClassName == "Int") || (coolClassName == "Bool")) {
    auto* intType = llvm::Type::getInt32Ty(*context);
    constants.push_back(llvm::Constant::getIntegerValue(intType, llvm::APInt(32, 0)));
  }
  else if (coolClassName == "String") {
    createGlobalVariable(lookup["Int"], "zero_int");
    auto* zero = module->getNamedGlobal("zero_int");
    assert(zero != nullptr);
    constants.push_back(zero);

    auto* emptyString = builder->CreateGlobalStringPtr("", "empty_str", 0, module.get());
    constants.push_back(emptyString);
  }
  else {
    auto& classMembersTable = env.globalMembersTable[coolClassName];
    for (auto& scope : classMembersTable) {
      auto classMembers = scope.values();
      for (auto [member, _] : classMembers) {
        auto& memberName = member->getIdType()->getNameAsStr();
        auto* memberType = llvm::StructType::getTypeByName(*context, memberName);
        assert(memberType != nullptr);
        auto* memberTypePtr = llvm::PointerType::get(memberType, 0);
        constants.push_back(llvm::ConstantPointerNull::get(memberTypePtr));
      }
    }
  }
  auto* structConstant = llvm::ConstantStruct::get(coolClassType, constants);
  proto->setInitializer(structConstant);

  proto->setLinkage(llvm::GlobalValue::PrivateLinkage);
  auto align = dl.getPrefTypeAlign(coolClassType);
  proto->setAlignment(align);
}

void Initializer::genCoolClassPrototypes() {
  for (auto* coolClass : classes.get()->getData()) {
    auto& coolClassName = coolClass->getCoolType()->getNameAsStr();
    lookup.insert({coolClassName, coolClass});
  }

  for (auto* coolClass : classes.get()->getData()) {
    auto& coolClassName = coolClass->getCoolType()->getNameAsStr();
    auto protoClassName = getProtoName(coolClassName);
    createGlobalVariable(coolClass, protoClassName);
  }
}

void Initializer::genConstructorDeclarations() {
  for (auto *coolClass: classes.get()->getData()) {
    auto &coolClassName = coolClass->getCoolType()->getNameAsStr();
    auto constructorName = getConstructorName(coolClassName);

    auto *coolClassPtrType = getPtrType(coolClassName);

    auto *funcType = llvm::FunctionType::get(coolClassPtrType, coolClassPtrType, false);
    auto *func = llvm::Function::Create(funcType,
                                        llvm::Function::InternalLinkage,
                                        constructorName,
                                        module.get());
    func->setCallingConv(llvm::CallingConv::C);
  }
}

void Initializer::genMethodDeclarations() {
  for (auto* coolClass : classes.get()->getData()) {
    auto *returnOpaqueType = getPtrType("Object");
    auto coolClassName = coolClass->getCoolType()->getNameAsStr();
    for (auto* attr : coolClass->getAttributes()->getData()) {
      if (auto* method = dynamic_cast<ast::SingleMethod*>(attr)) {

        auto& returnTypeName = method->getReturnType()->getNameAsStr();
        auto* returnType = (returnTypeName == "SELF_TYPE") ? returnOpaqueType : getPtrType(returnTypeName);
        assert(returnType != nullptr);

        auto* coolClassPtrType = getPtrType(coolClassName);
        std::vector<llvm::Type*> argsTypes{coolClassPtrType};

        for (auto* formal : method->getParameters()->getFormals()) {
          auto& argTypeName = formal->getIdType()->getNameAsStr();
          argsTypes.push_back(getPtrType(argTypeName));
        }

        auto methodName = getMethodName(coolClassName, method->getId()->getNameAsStr());
        auto* funcType = llvm::FunctionType::get(returnType, argsTypes, false);
        auto* func = llvm::Function::Create(funcType,
                                            llvm::Function::InternalLinkage,
                                            methodName,
                                            module.get());
        func->setCallingConv(llvm::CallingConv::C);
      }
    }
  }
}


MethodsTable createMethodsTable(std::vector<type::Graph::Node*>& inheritanceChain) {
  MethodsTable methodsTable;
  int offsetCounter{0};

  bool addScope{false};
  for (auto it = inheritanceChain.rbegin(); it != inheritanceChain.rend(); ++it) {
    auto childCoolClass = (*it)->getCoolClass();

    if (addScope) {
      methodsTable.pushScope();
      addScope = false;
    }

    for (auto* attr : childCoolClass->getAttributes()->getData()) {
      if (auto* method = dynamic_cast<ast::SingleMethod*>(attr)) {
        auto data = MethodsTableData{childCoolClass, method, offsetCounter};
        methodsTable.add(method->getId()->getNameAsStr(), data);
        addScope = true;
        ++offsetCounter;
      }
    }
  }
  return methodsTable;
}

void Initializer::initGlobalMethodsTable() {
  auto& coolContext = env.coolContext;
  auto& graph = coolContext.getInheritanceGraph();

  for (auto* coolClass : classes.get()->getData()) {
    auto& coolClassName = coolClass->getCoolType()->getNameAsStr();
    auto& coolClassNode = graph->getInheritanceNode(coolClassName);

    std::vector<type::Graph::Node*> inheritanceChain{};
    type::findInheritanceNodes(coolClassNode, inheritanceChain);
    auto methodsTable = createMethodsTable(inheritanceChain);
    env.globalMethodsTable.insert({coolClassName, methodsTable});
  }
}

void Initializer::genDispatchTables() {
  for (auto* coolClass : classes.get()->getData()) {
    auto& coolClassName = coolClass->getCoolType()->getNameAsStr();

    auto dispTableName = getDispatchTableName(coolClassName);
    auto dispTableTypeName = getDispatchTableTypeName(coolClassName);
    auto* dispTableType = llvm::StructType::create(*context, dispTableTypeName);

    std::vector<llvm::Type*> functionTypes{};
    std::vector<llvm::Constant*> functionsPointers{};
    auto& methodsTable = env.globalMethodsTable[coolClassName];
    for (auto& scope : methodsTable) {
      for (auto& item : scope) {
        auto& ownerName = item->owner->getCoolType()->getNameAsStr();
        auto& methodName = item->method->getId()->getNameAsStr();
        auto functionName = getMethodName(ownerName, methodName);
        auto* function = module->getFunction(functionName);

        if (function == nullptr) continue; // TODO

        auto* wrappedFunctionalPointer = llvm::PointerType::get(function->getFunctionType(), 0);
        functionTypes.push_back(wrappedFunctionalPointer);
        functionsPointers.push_back(function);
      }
    }
    dispTableType->setBody(functionTypes);
    module->getOrInsertGlobal(dispTableName, dispTableType);

    auto* dispTable = module->getNamedGlobal(dispTableName);
    auto* structConstant = llvm::ConstantStruct::get(dispTableType, functionsPointers);
    dispTable->setInitializer(structConstant);
    dispTable->setLinkage(llvm::GlobalValue::PrivateLinkage);
  }
}

void Initializer::initClassTagTable() {
  for (auto *coolClass: classes.get()->getData()) {
    auto& coolClassName = coolClass->getCoolType()->getNameAsStr();
    auto coolClassTag = coolClass->getTag();
    env.classTagTable.insert({coolClassName, coolClassTag});
  }
}
} // namespace mcool::codegen