#include "CodeGen/CodeBuilder.h"
#include "CodeGen/Misc.h"
#include "llvm/IR/Verifier.h"
#include <string>
#include <unordered_set>

namespace mcool::codegen {
void CodeBuilder::genMethods(mcool::AstTree& classes) {
  for (auto* coolClass : classes.get()->getData()) {
    coolClass->accept(this);
  }
}

void CodeBuilder::visitCoolClass(ast::CoolClass* coolClass) {
  // TODO: rework (you should check it using type)
  const static std::unordered_set<std::string> defaultClasses{
      "Object", "IO", "Int", "String", "Bool"};
  currClassName = coolClass->getCoolType()->getNameAsStr();

  bool isDefaultClass = defaultClasses.find(currClassName) != defaultClasses.end();
  if (not isDefaultClass) {
    for (auto* attr : coolClass->getAttributes()->getData()) {
      if (auto* coolMethod = dynamic_cast<ast::SingleMethod*>(attr)) {
        coolMethod->accept(this);
      }
    }
  }
}

void CodeBuilder::visitSingleMember(ast::SingleMember* member) {
  getLObjValue(member->getId());
  auto* idAddress = popStack();
  assert(idAddress != nullptr);

  member->getInitExpr()->accept(this);
  auto* initValue = popStack();
  bool hasInitValue = initValue != nullptr;

  auto* idSemantType = member->getId()->getSemantType();
  auto idSemantTypeName = idSemantType->getAsString();
  llvm::Value* newInstance{nullptr};
  if ((idSemantType->hasImplicitConstructor()) && (not hasInitValue)) {
    newInstance = createNewClassInstanceOnHeap(idSemantTypeName);
    builder->CreateStore(newInstance, idAddress);
  }

  if (hasInitValue) {
    newInstance = copyObject(initValue);
    auto idTypeName = member->getId()->getSemantType()->getAsString();
    auto* castedNewInstance = builder->CreateBitCast(newInstance, getPtrType(idTypeName));
    builder->CreateStore(castedNewInstance, idAddress);
  }

  stack.push_back(newInstance);
}

void CodeBuilder::visitSingleMethod(ast::SingleMethod* coolMethod) {
  auto& idName = coolMethod->getId()->getNameAsStr();
  auto methodName = getMethodName(currClassName, idName);
  currLLVMFunction = module->getFunction(methodName);

  llvm::BasicBlock* BB = llvm::BasicBlock::Create(*context, "entry", currLLVMFunction);
  builder->SetInsertPoint(BB);

  currSymbolTable = codegen::SymbolTable{};
  llvm::Value* selfPtr = currLLVMFunction->getArg(0);
  llvm::Value* selfPtrAddress = genAlloca(selfPtr->getType());
  builder->CreateStore(selfPtr, selfPtrAddress);
  currSymbolTable.add("self", selfPtrAddress);

  coolMethod->getParameters()->accept(this);

  auto& returnTypeName = coolMethod->getReturnType()->getNameAsStr();
  currFuncReturnType = llvm::cast<llvm::PointerType>(getPtrType(returnTypeName));

  coolMethod->getBody()->accept(this);

  auto* returnValue = popStack();
  auto* copiedReturnValue = copyObject(returnValue);

  auto* castedReturnValue = builder->CreateBitCast(copiedReturnValue, currFuncReturnType);
  builder->CreateRet(castedReturnValue);
  llvm::verifyFunction(*currLLVMFunction, &(llvm::errs()));
}

void CodeBuilder::visitFormalList(ast::FormalList* formalList) {
  size_t paramCounter{0};
  for (auto* formal : formalList->getFormals()) {
    auto& paramName = formal->getId()->getNameAsStr();
    llvm::Value* paramValue = currLLVMFunction->getArg(paramCounter + 1);
    llvm::Value* paramValueAddress = genAlloca(paramValue->getType());
    builder->CreateStore(paramValue, paramValueAddress);
    assert(paramValue != nullptr);

    currSymbolTable.add(paramName, paramValueAddress);
    ++paramCounter;
  }
}

void CodeBuilder::visitBlockExpr(ast::BlockExpr* block) { block->getExprs()->accept(this); }

void CodeBuilder::visitExpressions(ast::Expressions* exprs) {
  llvm::Value* result{nullptr};
  for (auto* expr : exprs->getData()) {
    expr->accept(this);
    result = popStack();
  }
  stack.push_back(result);
}

void CodeBuilder::visitDispatch(ast::Dispatch* dispatch) {
  dispatch->getObjectId()->accept(this);
  auto* objectPtr = popStack();
  assertNotNullptr(objectPtr);

  auto* dispatchObjType = dispatch->getObjectId()->getSemantType();
  auto dispatchObjTypeName = dispatchObjType->getAsString();
  auto& methodsTable = env.globalMethodsTable[dispatchObjTypeName];
  auto& methodName = dispatch->getMethodId()->getNameAsStr();
  auto data = methodsTable.lookup(methodName);
  assert(data.has_value());

  auto* castedDispatchObj = builder->CreateBitCast(objectPtr, getPtrType(dispatchObjTypeName));
  auto* dispatchTableAddress = builder->CreateGEP(castedDispatchObj, getGepIndices({0, 3}));
  auto* dispatchTable = builder->CreateLoad(dispatchTableAddress);

  auto* calleeAddress = builder->CreateGEP(dispatchTable, getGepIndices({0, data.value().offset}));
  auto* callee = builder->CreateLoad(calleeAddress);

  llvm::SmallVector<llvm::Value*> args{};

  auto* calleeWrappedPtrType = llvm::cast<llvm::PointerType>(callee->getType());
  auto* calleeFunctionPtrType =
      llvm::cast<llvm::FunctionType>(calleeWrappedPtrType->getElementType());
  auto* calleeSelfType = calleeFunctionPtrType->getFunctionParamType(0);
  auto* castedCalleeSelfType = builder->CreateBitCast(objectPtr, calleeSelfType);
  args.push_back(castedCalleeSelfType);

  for (auto* arg : dispatch->getArguments()->getData()) {
    arg->accept(this);
    args.push_back(popStack());
  }

  auto result = builder->CreateCall(calleeFunctionPtrType, callee, args);
  stack.push_back(result);
}

void CodeBuilder::visitStaticDispatch(ast::StaticDispatch* dispatch) {
  dispatch->getObjectId()->accept(this);
  auto* objectPtr = popStack();
  assertNotNullptr(objectPtr);

  auto* dispatchObjType = dispatch->getObjectId()->getSemantType();
  auto dispatchObjTypeName = dispatchObjType->getAsString();
  auto& methodsTable = env.globalMethodsTable[dispatchObjTypeName];
  auto& methodName = dispatch->getMethodId()->getNameAsStr();
  auto data = methodsTable.lookup(methodName);
  assert(data.has_value());

  auto& staticCastTypeName = dispatch->getCastType()->getNameAsStr();
  auto dispatchTableName = getDispatchTableName(staticCastTypeName);
  auto* dispatchTable = module->getGlobalVariable(dispatchTableName, true);

  auto* calleeAddress = builder->CreateGEP(dispatchTable, getGepIndices({0, data.value().offset}));
  auto* callee = builder->CreateLoad(calleeAddress);

  llvm::SmallVector<llvm::Value*> args{};

  auto* calleeWrappedPtrType = llvm::cast<llvm::PointerType>(callee->getType());
  auto* calleeFunctionPtrType =
      llvm::cast<llvm::FunctionType>(calleeWrappedPtrType->getElementType());
  auto* calleeSelfType = calleeFunctionPtrType->getFunctionParamType(0);
  auto* castedCalleeSelfType = builder->CreateBitCast(objectPtr, calleeSelfType);
  args.push_back(castedCalleeSelfType);

  for (auto* arg : dispatch->getArguments()->getData()) {
    arg->accept(this);
    args.push_back(popStack());
  }

  auto result = builder->CreateCall(calleeFunctionPtrType, callee, args);
  stack.push_back(result);
}

void CodeBuilder::visitWhileLoop(ast::WhileLoop* loop) {
  auto* loopHeaderBB = llvm::BasicBlock::Create(*context);
  auto* loopBodyBB = llvm::BasicBlock::Create(*context);
  auto* endLoopBB = llvm::BasicBlock::Create(*context);

  builder->CreateBr(loopHeaderBB);
  {
    currLLVMFunction->getBasicBlockList().push_back(loopHeaderBB);
    builder->SetInsertPoint(loopHeaderBB);
    loop->getPredicate()->accept(this);
    auto* predResult = popStack();
    assert(predResult != nullptr);

    auto* boolAddress = builder->CreateGEP(predResult, getGepIndices({0, 4}));
    auto* boolValue = builder->CreateLoad(boolAddress);
    auto* condValue = builder->CreateTrunc(boolValue, builder->getInt1Ty());

    builder->CreateCondBr(condValue, loopBodyBB, endLoopBB);
    loopHeaderBB = builder->GetInsertBlock();
  }
  {
    currLLVMFunction->getBasicBlockList().push_back(loopBodyBB);
    builder->SetInsertPoint(loopBodyBB);
    loop->getBody()->accept(this);
    auto* bodyResult = popStack();
    assert(bodyResult != nullptr);

    builder->CreateBr(loopHeaderBB);
    loopBodyBB = builder->GetInsertBlock();
  }

  currLLVMFunction->getBasicBlockList().push_back(endLoopBB);
  builder->SetInsertPoint(endLoopBB);

  auto* targetCoolType = loop->getSemantType();
  assert(targetCoolType != nullptr);

  auto* loopResult = createNewClassInstanceOnHeap(targetCoolType->getAsString());
  stack.push_back(loopResult);
}

void CodeBuilder::visitIsVoidNode(ast::IsVoidNode* node) {
  node->getTerm()->accept(this);
  auto* resultValue = popStack();

  auto objTypeName = node->getTerm()->getSemantType()->getAsString();
  auto* objTypePtr = getPtrType(objTypeName);
  auto* nullPtr = llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(objTypePtr));

  auto* compareValue = builder->CreateICmpEQ(resultValue, nullPtr);
  compareValue = builder->CreateZExt(compareValue, builder->getInt32Ty());

  auto* boolObj = createNewClassInstanceOnStack("Bool");
  auto* boolValueAddress = builder->CreateGEP(boolObj, getGepIndices({0, 4}));
  builder->CreateStore(compareValue, boolValueAddress);
  stack.push_back(boolObj);
}

void CodeBuilder::visitNegationNode(ast::NegationNode* node) {
  node->getTerm()->accept(this);
  auto* result = popStack();
  auto* address = builder->CreateGEP(result, getGepIndices({0, 4}));
  result = builder->CreateLoad(address);
  result = builder->CreateNeg(result);

  auto* newIntObj = createNewClassInstanceOnStack("Int");
  address = builder->CreateGEP(newIntObj, getGepIndices({0, 4}));
  builder->CreateStore(result, address);
  stack.push_back(newIntObj);
}

void CodeBuilder::visitNotExpr(ast::NotExpr* noExpr) {
  noExpr->getExpr()->accept(this);
  auto* result = popStack();
  auto* address = builder->CreateGEP(result, getGepIndices({0, 4}));
  result = builder->CreateLoad(address);
  result = builder->CreateNot(result);

  auto* boolIntObj = createNewClassInstanceOnStack("Bool");
  address = builder->CreateGEP(boolIntObj, getGepIndices({0, 4}));
  builder->CreateStore(result, address);
  stack.push_back(boolIntObj);
}

void CodeBuilder::visitNewExpr(ast::NewExpr* newExpr) {
  auto& newTypeName = newExpr->getNewType()->getNameAsStr();
  auto* newObject = createNewClassInstanceOnHeap(newTypeName);

  auto constructorName = getConstructorName(newTypeName);
  auto* constructor = module->getFunction(constructorName);
  assert(constructor != nullptr);
  builder->CreateCall(constructor, newObject);
  stack.push_back(newObject);
}

void CodeBuilder::visitCaseExpr(ast::CaseExpr* caseExpr) {
  caseExpr->getExpr()->accept(this);
  auto* exprValue = popStack();
  assertNotNullptr(exprValue);

  auto* address = builder->CreateGEP(exprValue, getGepIndices({0, 1}));
  auto* exprClassTag = builder->CreateLoad(address);

  auto targetTypeName = caseExpr->getSemantType()->getAsString();
  auto* targetPtrType = llvm::cast<llvm::PointerType>(getPtrType(targetTypeName));

  auto& coolCases = caseExpr->getCasses()->getData();
  const auto numCases = coolCases.size();
  std::vector<llvm::BasicBlock*> selectBlocks(numCases);
  std::vector<llvm::BasicBlock*> computeBlocks(numCases);
  for (size_t i = 0; i < coolCases.size(); ++i) {
    selectBlocks[i] = llvm::BasicBlock::Create(*context);
    computeBlocks[i] = llvm::BasicBlock::Create(*context);
  }

  size_t caseCounter{0};
  for (auto* aCase : coolCases) {
    auto& bindVarTypeName = aCase->getIdType()->getNameAsStr();
    auto caseClassTag = env.classTagTable[bindVarTypeName];
    auto* condValue = builder->CreateICmpEQ(exprClassTag, builder->getInt32(caseClassTag));

    auto* nextSelectBlock = selectBlocks[caseCounter];
    builder->CreateCondBr(condValue, computeBlocks[caseCounter], nextSelectBlock);

    currLLVMFunction->getBasicBlockList().push_back(nextSelectBlock);
    builder->SetInsertPoint(nextSelectBlock);

    ++caseCounter;
  }
  callExit("No match in `case` statement\n", -1);
  auto* nullPtr = llvm::ConstantPointerNull::get(currFuncReturnType);
  builder->CreateRet(nullPtr);

  auto* mergeBlock = llvm::BasicBlock::Create(*context);
  std::vector<llvm::Value*> results(numCases);
  caseCounter = 0;
  for (auto* aCase : coolCases) {
    auto* currBlock = computeBlocks[caseCounter];
    currLLVMFunction->getBasicBlockList().push_back(currBlock);
    builder->SetInsertPoint(currBlock);

    auto& bindVarTypeName = aCase->getIdType()->getNameAsStr();
    auto* bindVarType = getPtrType(bindVarTypeName);
    llvm::Value* bindVarAddress = genAlloca(bindVarType);
    auto* castedCaseExprValue = builder->CreateBitCast(exprValue, bindVarType);
    builder->CreateStore(castedCaseExprValue, bindVarAddress);

    currSymbolTable.pushScope();
    currSymbolTable.add(aCase->getId()->getNameAsStr(), bindVarAddress);

    aCase->getBody()->accept(this);
    auto* result = popStack();
    if (result) {
      result = builder->CreateBitCast(result, targetPtrType);
    } else {
      result = llvm::ConstantPointerNull::get(targetPtrType);
    }
    results[caseCounter] = result;
    currSymbolTable.popScope();

    builder->CreateBr(mergeBlock);
    computeBlocks[caseCounter] = builder->GetInsertBlock();
    ++caseCounter;
  }

  currLLVMFunction->getBasicBlockList().push_back(mergeBlock);
  builder->SetInsertPoint(mergeBlock);
  auto* phi = builder->CreatePHI(targetPtrType, numCases);
  for (size_t i = 0; i < numCases; ++i) {
    phi->addIncoming(results[i], computeBlocks[i]);
  }
  stack.push_back(phi);
}

void CodeBuilder::visitCaseList(ast::CaseList* caseList) {}
void CodeBuilder::visitSingleCase(ast::SingleCase* aCase) {}

void CodeBuilder::visitLetExpr(ast::LetExpr* letExpr) {
  auto& varTypeName = letExpr->getIdType()->getNameAsStr();
  auto* varTypePtr = getPtrType(varTypeName);
  llvm::Value* varPtr = genAlloca(varTypePtr);

  letExpr->getInitExpr()->accept(this);
  if (auto* initExprValue = popStack()) {
    auto* copiedInitExpr = copyObject(initExprValue);
    builder->CreateStore(copiedInitExpr, varPtr);
  } else {
    auto* nullPtr = llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(varTypePtr));
    builder->CreateStore(nullPtr, varPtr);
  }
  currSymbolTable.pushScope();
  currSymbolTable.add(letExpr->getId()->getNameAsStr(), varPtr);

  letExpr->getBody()->accept(this);
  currSymbolTable.popScope();
}

void CodeBuilder::visitIfThenExpr(ast::IfThenExpr* condExpr) {
  condExpr->getCondition()->accept(this);
  auto* condResult = popStack();

  auto* boolAddress = builder->CreateGEP(condResult, getGepIndices({0, 4}));
  auto* boolValue = builder->CreateLoad(boolAddress);
  auto* condValue = builder->CreateTrunc(boolValue, builder->getInt1Ty());

  auto* thenBB = llvm::BasicBlock::Create(*context);
  auto* elseBB = llvm::BasicBlock::Create(*context);
  auto* mergeBB = llvm::BasicBlock::Create(*context);

  llvm::Value* thenValue;
  llvm::Value* elseValue;

  auto* targetCoolType = condExpr->getSemantType();
  assert(targetCoolType != nullptr);
  auto* targetPtrType = getPtrType(targetCoolType->getAsString());

  builder->CreateCondBr(condValue, thenBB, elseBB);
  {
    currLLVMFunction->getBasicBlockList().push_back(thenBB);
    builder->SetInsertPoint(thenBB);
    condExpr->getThenBody()->accept(this);
    thenValue = popStack();
    assert(thenValue != nullptr);
    thenValue = builder->CreateBitCast(thenValue, targetPtrType);

    builder->CreateBr(mergeBB);
    thenBB = builder->GetInsertBlock();
  }
  {
    currLLVMFunction->getBasicBlockList().push_back(elseBB);
    builder->SetInsertPoint(elseBB);
    elseValue = createNewClassInstanceOnHeap(targetCoolType->getAsString());

    assert(elseValue != nullptr);
    elseValue = builder->CreateBitCast(elseValue, targetPtrType);

    builder->CreateBr(mergeBB);
    elseBB = builder->GetInsertBlock();
  }

  currLLVMFunction->getBasicBlockList().push_back(mergeBB);
  builder->SetInsertPoint(mergeBB);

  auto* phi = builder->CreatePHI(targetPtrType, 2);
  phi->addIncoming(thenValue, thenBB);
  phi->addIncoming(elseValue, elseBB);
  stack.push_back(phi);
}

void CodeBuilder::visitIfThenElseExpr(ast::IfThenElseExpr* condExpr) {
  condExpr->getCondition()->accept(this);
  auto* condResult = popStack();

  auto* boolAddress = builder->CreateGEP(condResult, getGepIndices({0, 4}));
  auto* boolValue = builder->CreateLoad(boolAddress);
  auto* condValue = builder->CreateTrunc(boolValue, builder->getInt1Ty());

  auto* thenBB = llvm::BasicBlock::Create(*context);
  auto* elseBB = llvm::BasicBlock::Create(*context);
  auto* mergeBB = llvm::BasicBlock::Create(*context);

  llvm::Value* thenValue;
  llvm::Value* elseValue;

  auto* targetCoolType = condExpr->getSemantType();
  assert(targetCoolType != nullptr);
  auto* targetPtrType = getPtrType(targetCoolType->getAsString());

  builder->CreateCondBr(condValue, thenBB, elseBB);
  {
    currLLVMFunction->getBasicBlockList().push_back(thenBB);
    builder->SetInsertPoint(thenBB);
    condExpr->getThenBody()->accept(this);
    thenValue = popStack();
    assert(thenValue != nullptr);
    thenValue = builder->CreateBitCast(thenValue, targetPtrType);

    builder->CreateBr(mergeBB);
    thenBB = builder->GetInsertBlock();
  }
  {
    currLLVMFunction->getBasicBlockList().push_back(elseBB);
    builder->SetInsertPoint(elseBB);
    condExpr->getElseBody()->accept(this);
    elseValue = popStack();
    assert(elseValue != nullptr);
    elseValue = builder->CreateBitCast(elseValue, targetPtrType);

    builder->CreateBr(mergeBB);
    elseBB = builder->GetInsertBlock();
  }

  currLLVMFunction->getBasicBlockList().push_back(mergeBB);
  builder->SetInsertPoint(mergeBB);

  auto* phi = builder->CreatePHI(targetPtrType, 2);
  phi->addIncoming(thenValue, thenBB);
  phi->addIncoming(elseValue, elseBB);
  stack.push_back(phi);
}

void CodeBuilder::visitNoExpr(ast::NoExpr*) { stack.push_back(nullptr); }

void CodeBuilder::visitPlusNode(ast::PlusNode* node) { visitBinaryNode(node, BinaryOp::Plus); }

void CodeBuilder::visitMinusNode(ast::MinusNode* node) { visitBinaryNode(node, BinaryOp::Minus); }

void CodeBuilder::visitMultiplyNode(ast::MultiplyNode* node) {
  visitBinaryNode(node, BinaryOp::Mult);
}

void CodeBuilder::visitDivideNode(ast::DivideNode* node) { visitBinaryNode(node, BinaryOp::Div); }

void CodeBuilder::visitLessNode(ast::LessNode* node) { visitBinaryNode(node, BinaryOp::Less); }

void CodeBuilder::visitEqualNode(ast::EqualNode* node) { visitBinaryNode(node, BinaryOp::Eq); }

void CodeBuilder::visitLessEqualNode(ast::LessEqualNode* node) {
  visitBinaryNode(node, BinaryOp::Leq);
}

void CodeBuilder::visitBinaryNode(ast::BinaryExpression* node, BinaryOp op) {
  node->getRight()->accept(this);
  auto* rightIntObj = popStack();
  auto* rightValueAddress = builder->CreateGEP(rightIntObj, getGepIndices({0, 4}));
  auto* rightValue = builder->CreateLoad(rightValueAddress);

  node->getLeft()->accept(this);
  auto* leftIntObj = popStack();
  auto* leftValueAddress = builder->CreateGEP(leftIntObj, getGepIndices({0, 4}));
  auto* leftValue = builder->CreateLoad(leftValueAddress);

  llvm::Value* resultValue{};
  switch (op) {
  case BinaryOp::Plus: {
    resultValue = builder->CreateAdd(leftValue, rightValue);
    break;
  }
  case BinaryOp::Minus: {
    resultValue = builder->CreateSub(leftValue, rightValue);
    break;
  }
  case BinaryOp::Mult: {
    resultValue = builder->CreateMul(leftValue, rightValue);
    break;
  }
  case BinaryOp::Div: {
    resultValue = builder->CreateSDiv(leftValue, rightValue);
    break;
  }
  case BinaryOp::Eq: {
    auto* boolValue = builder->CreateICmpEQ(leftValue, rightValue);
    resultValue = builder->CreateZExt(boolValue, builder->getInt32Ty());
  }
  case BinaryOp::Less: {
    auto* boolValue = builder->CreateICmpSLT(leftValue, rightValue);
    resultValue = builder->CreateZExt(boolValue, builder->getInt32Ty());
  }
  case BinaryOp::Leq: {
    auto* boolValue = builder->CreateICmpSLE(leftValue, rightValue);
    resultValue = builder->CreateZExt(boolValue, builder->getInt32Ty());
  }
  }
  auto resultTypeName = node->getSemantType()->getAsString();
  auto* resultObjPtr = createNewClassInstanceOnStack(resultTypeName);
  auto* resultValueAddress = builder->CreateGEP(resultObjPtr, getGepIndices({0, 4}));
  builder->CreateStore(resultValue, resultValueAddress);

  stack.push_back(resultObjPtr);
}

void CodeBuilder::visitAssignExpr(ast::AssignExpr* node) {
  getLObjValue(node->getId());
  auto* idAddress = popStack();

  node->getInitExpr()->accept(this);
  if (auto* initValue = popStack()) {
    auto* copiedInitValue = copyObject(initValue);

    auto idTypeName = node->getSemantType()->getAsString();
    auto* castedCopiedInitValue = builder->CreateBitCast(copiedInitValue, getPtrType(idTypeName));
    builder->CreateStore(castedCopiedInitValue, idAddress);

    auto* idValue = builder->CreateLoad(idAddress);
    stack.push_back(idValue);
  } else {
    stack.push_back(nullptr);
  }
}

void CodeBuilder::visitPrimaryExpr(ast::PrimaryExpr* node) { node->getTerm()->accept(this); }

void CodeBuilder::visitObjectId(ast::ObjectId* id) {
  getLObjValue(id);
  auto* address = popStack();
  assert(address != nullptr);
  stack.push_back(builder->CreateLoad(address));
}

void CodeBuilder::getLObjValue(ast::ObjectId* id) {
  auto& idName = id->getNameAsStr();
  if (auto currScopeResults = currSymbolTable.lookup(idName)) {
    stack.push_back(currScopeResults.value());
  } else {
    auto* selfPtrAddress = currSymbolTable.lookup("self").value();
    auto* selfPtr = builder->CreateLoad(selfPtrAddress);

    auto& membersTable = env.globalMembersTable[currClassName];
    if (auto classScopeResult = membersTable.lookup(idName)) {
      auto [_, offset] = classScopeResult.value();
      auto* address = builder->CreateGEP(selfPtr, getGepIndices({0, offset}));
      stack.push_back(address);
    } else {
      assert(false && "symbol was not found during code generation");
    }
  }
}

void CodeBuilder::visitBool(ast::Bool* item) {
  auto* boolPtr = createNewClassInstanceOnStack("Bool");
  auto* valueAddress = builder->CreateGEP(boolPtr, getGepIndices({0, 4}));
  auto* literalConstant = builder->getInt32(item->getValue());
  builder->CreateStore(literalConstant, valueAddress);
  stack.push_back(boolPtr);
}

void CodeBuilder::visitInt(ast::Int* item) {
  auto* intPtr = createNewClassInstanceOnStack("Int");
  auto* valueAddress = builder->CreateGEP(intPtr, getGepIndices({0, 4}));
  auto* literalConstant = builder->getInt32(item->getValue());
  builder->CreateStore(literalConstant, valueAddress);
  stack.push_back(intPtr);
}

void CodeBuilder::visitString(ast::String* str) {
  llvm::Value* intPtr{nullptr};
  {
    intPtr = createNewClassInstanceOnHeap("Int");
    auto* valueAddress = builder->CreateGEP(intPtr, getGepIndices({0, 4}));
    auto* literalConstant = builder->getInt32(str->getValueAsStr().size());
    builder->CreateStore(literalConstant, valueAddress);
  }

  auto* stringPtr = createNewClassInstanceOnHeap("String");
  auto* address = builder->CreateGEP(stringPtr, getGepIndices({0, 5}));
  auto* strLiteral = builder->CreateGlobalStringPtr(str->getValueAsStr(), "", 0, module.get());
  builder->CreateStore(strLiteral, address);

  address = builder->CreateGEP(stringPtr, getGepIndices({0, 4}));
  builder->CreateStore(intPtr, address);
  stack.push_back(stringPtr);
}
} // namespace mcool::codegen