#include "Semant/TypeChecker/TypeChecker.h"
#include <sstream>
#include <unordered_set>

namespace mcool::semant {
void TypeChecker::run(ast::CoolClassList* coolClassList) {
  for (auto* coolClass : coolClassList->getData()) {
    coolClass->accept(this);
  }
}

void TypeChecker::visitCoolClass(ast::CoolClass* coolClass) {
  const static std::unordered_set<std::string> defaultClasses{
      "Object", "IO", "Int", "String", "Bool"};
  currClassName = coolClass->getCoolType()->getNameAsStr();
  bool isDefaultClass = defaultClasses.find(currClassName) != defaultClasses.end();

  if (not isDefaultClass) {
    if (auto classEnv = environments.getClass(currClassName)) {
      selfType = typeBuilder->getType(currClassName);
      currClassEnv = classEnv.value();

      coolClass->getAttributes()->accept(this);
      coolClass->setSemantType(selfType);
    } else {
      std::stringstream errStream;
      errStream << "could not find class env. for `" << currClassName << "`. See at "
                << coolClass->getCoolType()->getLocation();
      errList.push_back(errStream.str());
      coolClass->setSemantType(errorType);
    }
  }
}

void TypeChecker::visitAttributeList(ast::AttributeList* attributeList) {
  auto& attrs = attributeList->getData();
  if (not attrs.empty()) {
    for (auto* attr : attrs) {
      attr->accept(this);
    }
    auto* lastAttr = attributeList->getData().back();
    assert(lastAttr != nullptr);
    attributeList->setSemantType(lastAttr->getSemantType());
  }
}

void TypeChecker::visitSingleMember(ast::SingleMember* member) {
  auto& memberName = member->getId()->getNameAsStr();
  auto& members = currClassEnv->getMembers();

  auto declType = members.lookup(memberName);
  assert(declType.has_value());

  if (declType.value()->getAsString() == "SELF_TYPE") {
    declType = selfType;
  }
  member->setSemantType(declType.value());

  auto* initExpr = member->getInitExpr();
  if (dynamic_cast<ast::NoExpr*>(initExpr) == nullptr) {
    initExpr->accept(this);
    auto* derivedType = initExpr->getSemantType();

    auto declTypeNode = graph->getInheritanceNode(declType.value()->getAsString());
    auto derivedTypeNode = graph->getInheritanceNode(derivedType->getAsString());

    if (not derivedTypeNode.isChildOf(&declTypeNode)) {
      std::stringstream errStream;
      errStream << "type `" << derivedType->getAsString() << "` does not conform to type `"
                << declType.value()->getAsString() << "` of a member `" << memberName
                << "`. See at " << member->getLocation();
      errList.push_back(errStream.str());
    }
  }
}

void TypeChecker::visitSingleMethod(ast::SingleMethod* method) {
  auto methodName = method->getId()->getNameAsStr();
  auto& members = currClassEnv->getMembers();

  members.pushScope();
  method->getParameters()->accept(this);
  method->getBody()->accept(this);

  {
    type::Type* returnDeclType{nullptr};
    auto& declReturnName = method->getReturnType()->getNameAsStr();
    if (declReturnName == "SELF_TYPE") {
      returnDeclType = selfType;
    } else {
      returnDeclType = typeBuilder->getType(declReturnName);
    }
    auto* returnBodyType = method->getBody()->getSemantType();
    assert(returnBodyType != nullptr);

    auto returnDeclNode = graph->getInheritanceNode(returnDeclType->getAsString());
    auto returnBodyNode = graph->getInheritanceNode(returnBodyType->getAsString());

    if (returnBodyNode.isChildOf(&returnDeclNode)) {
      std::vector<type::Type*> parameterList{};
      auto& formals = method->getParameters()->getFormals();
      std::for_each(formals.begin(), formals.end(), [&parameterList](ast::SingleFormal* formal) {
        parameterList.push_back(formal->getSemantType());
      });
      auto* methodType = typeBuilder->getMethodType(methodName, returnDeclType, parameterList);
      method->setSemantType(methodType);
    } else {
      std::stringstream errStream;
      errStream << "method `" << methodName << "` returns type `" << returnBodyType->getAsString()
                << "` but expected `" << returnDeclType->getAsString() << "`. See at "
                << method->getReturnType()->getLocation();
      errList.push_back(errStream.str());
      method->setSemantType(errorType);
    }
  }
  members.popScope();
}

void TypeChecker::visitFormalList(ast::FormalList* formalList) {
  auto formals = formalList->getFormals();
  for (auto* formal : formals) {
    formal->accept(this);
  }
}

void TypeChecker::visitSingleFormal(ast::SingleFormal* formal) {
  auto& formalName = formal->getId()->getNameAsStr();

  auto& members = currClassEnv->getMembers();
  auto& formalTypeName = formal->getIdType()->getNameAsStr();
  auto* fortmalType = typeBuilder->getType(formalTypeName);
  formal->setSemantType(fortmalType);
  members.add(formalName, fortmalType);

  formal->setSemantType(typeBuilder->getType(formalTypeName));
}

void TypeChecker::visitBlockExpr(ast::BlockExpr* block) {
  block->getExprs()->accept(this);
  auto& exprs = block->getExprs()->getData();
  if (exprs.empty()) {
    block->setSemantType(typeBuilder->getType("Object"));
  } else {
    auto* lastExpr = exprs.back();
    assert(lastExpr != nullptr);
    block->setSemantType(lastExpr->getSemantType());
  }
}

void TypeChecker::visitExpressions(ast::Expressions* node) {
  auto& exprs = node->getData();
  if (not exprs.empty()) {
    for (auto* expr : exprs) {
      expr->accept(this);
    }
    auto* lastExprType = exprs.back()->getSemantType();
    assert(lastExprType != nullptr);
    node->setSemantType(lastExprType);
  }
}

void TypeChecker::visitWhileLoop(ast::WhileLoop* loop) {
  loop->getPredicate()->accept(this);
  auto* predicatedType = loop->getPredicate()->getSemantType();
  if (predicatedType->getTypeKind() != type::TypeKind::Bool) {
    std::stringstream errStream;
    errStream << "expected predicate type `Bool`, obtained `" << predicatedType->getAsString()
              << "`"
              << ". See at " << loop->getPredicate()->getLocation();
    errList.push_back(errStream.str());
  }

  loop->getBody()->accept(this);
  auto* loopType = typeBuilder->getType("Object");
  loop->setSemantType(loopType);
}

void TypeChecker::visitPrimaryExpr(ast::PrimaryExpr* node) {
  node->getTerm()->accept(this);
  auto* resultType = node->getTerm()->getSemantType();
  assert(resultType != nullptr);
  node->setSemantType(resultType);
}

void TypeChecker::visitDispatch(ast::Dispatch* dispatch) {
  auto& methodName = dispatch->getMethodId()->getNameAsStr();

  dispatch->getObjectId()->accept(this);
  auto* objectIdType = dispatch->getObjectId()->getSemantType();
  if (objectIdType->getAsString() == "SELF_TYPE") {
    objectIdType = selfType;
  }

  std::vector<type::MethodType*> foundMethodTypes{};
  if (auto exprEnv = environments.getClass(objectIdType->getAsString())) {
    auto& inheritanceChain = exprEnv.value()->getInheritanceChain();
    for (auto* classNode : inheritanceChain) {
      auto env = environments.getClass(classNode->getNodeName());
      assert(env.has_value());
      auto& currMethodTable = env.value()->getMethods();
      if (auto methodType = currMethodTable.lookup(methodName)) {
        if (methodType.has_value()) {
          foundMethodTypes.push_back(methodType.value());
        }
      }
    }
  } else {
    std::stringstream errStream;
    errStream << "could not find class env. for type `" << objectIdType->getAsString()
              << "`. See at " << dispatch->getObjectId()->getLocation();
    errList.push_back(errStream.str());
    dispatch->setSemantType(errorType);
    return;
  }

  if (foundMethodTypes.empty()) {
    std::stringstream errStream;
    errStream << "[dynamic disp.] could not find method `" << methodName << "` of type `"
              << objectIdType->getAsString() << "`. See at " << dispatch->getLocation();
    errList.push_back(errStream.str());
    dispatch->setSemantType(errorType);
    return;
  }

  dispatch->getArguments()->accept(this);
  auto& args = dispatch->getArguments()->getData();

  type::MethodType* methodType{nullptr};
  for (auto* candidate : foundMethodTypes) {
    if (isAllMethodArgsOk(candidate, args)) {
      methodType = candidate;
      break;
    }
  }

  if (methodType == nullptr) {
    std::stringstream errStream;
    errStream << "method `" << methodName << "` of class `" << objectIdType->getAsString()
              << "` not found. See at " << dispatch->getObjectId()->getLocation();
    errList.push_back(errStream.str());
    dispatch->setSemantType(errorType);
    return;
  }

  auto* methodReturnType = methodType->getReturnType();
  if (methodReturnType->getAsString() == std::string("SELF_TYPE")) {
    methodReturnType = objectIdType;
  }

  dispatch->setSemantType(methodReturnType);
}

void TypeChecker::visitStaticDispatch(ast::StaticDispatch* dispatch) {
  auto& methodName = dispatch->getMethodId()->getNameAsStr();

  dispatch->getObjectId()->accept(this);
  auto& castTypeName = dispatch->getCastType()->getNameAsStr();
  auto* objectIdType = dispatch->getObjectId()->getSemantType();

  const auto& castNode = graph->getInheritanceNode(castTypeName);
  const auto& objectIdNode = graph->getInheritanceNode(objectIdType->getAsString());
  if (!objectIdNode.isChildOf(&castNode)) {
    std::stringstream errStream;
    errStream << "could not downcast `" << objectIdType->getAsString() << "` to type `"
              << castNode.getNodeName() << "`. See at " << dispatch->getObjectId()->getLocation();
    errList.push_back(errStream.str());
    dispatch->setSemantType(errorType);
    return;
  }

  std::vector<type::MethodType*> foundMethodTypes{};
  if (auto exprEnv = environments.getClass(castTypeName)) {
    auto& inheritanceChain = exprEnv.value()->getInheritanceChain();
    for (auto* classNode : inheritanceChain) {
      auto env = environments.getClass(classNode->getNodeName());
      assert(env.has_value());
      auto& currMethodTable = env.value()->getMethods();
      if (auto methodType = currMethodTable.lookup(methodName)) {
        if (methodType.has_value()) {
          foundMethodTypes.push_back(methodType.value());
        }
      }
    }
  } else {
    std::stringstream errStream;
    errStream << "could not find class env. for type `" << objectIdType->getAsString()
              << "`. See at " << dispatch->getObjectId()->getLocation();
    errList.push_back(errStream.str());
    dispatch->setSemantType(errorType);
    return;
  }

  if (foundMethodTypes.empty()) {
    std::stringstream errStream;
    errStream << "[static disp.] could not find method `" << methodName << "` of type `"
              << objectIdType->getAsString() << "`. See at " << dispatch->getLocation();
    errList.push_back(errStream.str());
    dispatch->setSemantType(errorType);
    return;
  }

  dispatch->getArguments()->accept(this);
  auto& args = dispatch->getArguments()->getData();

  type::MethodType* methodType{nullptr};
  for (auto* candidate : foundMethodTypes) {
    if (isAllMethodArgsOk(candidate, args)) {
      methodType = candidate;
      break;
    }
  }

  if (methodType == nullptr) {
    std::stringstream errStream;
    errStream << "method `" << methodName << "` of class `" << castTypeName
              << "` not found. See at " << dispatch->getCastType()->getLocation();
    errList.push_back(errStream.str());
    dispatch->setSemantType(errorType);
    return;
  }

  auto* methodReturnType = methodType->getReturnType();
  if (methodReturnType->getAsString() == std::string("SELF_TYPE")) {
    methodReturnType = objectIdType;
  }

  dispatch->setSemantType(methodReturnType);
}

void TypeChecker::visitCaseExpr(ast::CaseExpr* caseExpr) {
  caseExpr->getExpr()->accept(this);
  auto* exprType = caseExpr->getExpr()->getSemantType();
  assert(exprType != nullptr);

  caseExpr->getCasses()->accept(this);
  auto& casses = caseExpr->getCasses()->getData();

  std::vector<type::Type*> casesTypes{};
  for (auto* aCase : casses) {
    casesTypes.push_back(aCase->getSemantType());
    assert(casesTypes.back() != nullptr);
  }
  assert(not casesTypes.empty());

  auto* commonType = casesTypes[0];
  for (size_t i = 1; i < casses.size(); ++i) {
    const auto& node1 = graph->getInheritanceNode(commonType->getAsString());
    const auto& node2 = graph->getInheritanceNode(casesTypes[i]->getAsString());

    auto* commonNode = type::findCommonParentType(&node1, &node2);
    commonType = typeBuilder->getType(commonNode->getNodeName());
  }
  caseExpr->setSemantType(commonType);
}

void TypeChecker::visitCaseList(ast::CaseList* caseList) {
  auto& cases = caseList->getData();
  for (auto* aCase : cases) {
    aCase->accept(this);
  }
}

void TypeChecker::visitSingleCase(ast::SingleCase* aCase) {
  auto& bindVarName = aCase->getId()->getNameAsStr();
  auto& bindVarTypeName = aCase->getIdType()->getNameAsStr();
  auto* bindType = typeBuilder->getType(bindVarTypeName);

  auto& members = currClassEnv->getMembers();
  members.pushScope();
  members.add(bindVarName, bindType);

  aCase->getBody()->accept(this);
  auto* bodyType = aCase->getBody()->getSemantType();
  assert(bodyType != nullptr);
  aCase->setSemantType(bodyType);

  members.popScope();
}

void TypeChecker::visitIfThenExpr(ast::IfThenExpr* condExpr) {
  condExpr->getCondition()->accept(this);

  auto* condType = condExpr->getCondition()->getSemantType();
  assert(condType != nullptr);
  if (condType->getTypeKind() != type::TypeKind::Bool) {
    std::stringstream errStream;
    errStream << "expected predicate type `Bool`, obtained `" << condType->getAsString()
              << "`. See at " << condExpr->getCondition()->getLocation();
    errList.push_back(errStream.str());
  }

  condExpr->getThenBody()->accept(this);
  auto* thenBodyType = condExpr->getThenBody()->getSemantType();
  assert(thenBodyType != nullptr);
  condExpr->setSemantType(thenBodyType);
}

void TypeChecker::visitIfThenElseExpr(ast::IfThenElseExpr* condExpr) {
  condExpr->getCondition()->accept(this);

  auto* condType = condExpr->getCondition()->getSemantType();
  assert(condType != nullptr);
  if (condType->getTypeKind() != type::TypeKind::Bool) {
    std::stringstream errStream;
    errStream << "expected predicate type `Bool`, obtained `" << condType->getAsString()
              << "`. See at " << condExpr->getCondition()->getLocation();
    errList.push_back(errStream.str());
  }

  condExpr->getThenBody()->accept(this);
  auto* thenBodyType = condExpr->getThenBody()->getSemantType();
  assert(thenBodyType != nullptr);

  condExpr->getElseBody()->accept(this);
  auto* elseBodyType = condExpr->getElseBody()->getSemantType();
  assert(elseBodyType != nullptr);

  const auto& thenNode = graph->getInheritanceNode(thenBodyType->getAsString());
  const auto& elseNode = graph->getInheritanceNode(elseBodyType->getAsString());

  auto* commonNode = type::findCommonParentType(&thenNode, &elseNode);
  auto* commonType = typeBuilder->getType(commonNode->getNodeName());
  assert(commonType != nullptr);
  condExpr->setSemantType(commonType);
}

void TypeChecker::visitLetExpr(ast::LetExpr* letExpr) {
  auto& idName = letExpr->getId()->getNameAsStr();
  auto& declTypeName = letExpr->getIdType()->getNameAsStr();
  auto& declTypeLoc = letExpr->getIdType()->getLocation();

  type::Type* idType{errorType};
  if (declTypeName == "SELF_TYPE") {
    idType = selfType;
  } else if (doesTypeExist(declTypeName, declTypeLoc)) {
    idType = typeBuilder->getType(declTypeName);
  } else {
    idType = errorType;
  }

  auto* initExpr = letExpr->getInitExpr();
  initExpr->accept(this);
  auto* initExprType = initExpr->getSemantType();
  assert(initExprType != nullptr);

  if (dynamic_cast<ast::NoExpr*>(initExpr) != nullptr) {
    initExprType = idType;
  }

  const auto& idNode = graph->getInheritanceNode(idType->getAsString());
  const auto& initExprNode = graph->getInheritanceNode(initExprType->getAsString());

  if (not initExprNode.isChildOf(&idNode)) {
    std::stringstream errStream;
    errStream << "type `" << initExprType->getAsString() << "` does not conform to type `"
              << idType->getAsString() << "` of identifier `" << idName << "`. See at "
              << letExpr->getLocation();
    errList.push_back(errStream.str());
    idType = errorType;
  }

  auto& members = currClassEnv->getMembers();
  members.pushScope();
  members.add(idName, idType);

  letExpr->getBody()->accept(this);
  auto* bodyExprType = letExpr->getBody()->getSemantType();
  assert(bodyExprType != nullptr);
  letExpr->setSemantType(bodyExprType);

  members.popScope();
}

void TypeChecker::visitAssignExpr(ast::AssignExpr* node) {
  auto idName = node->getId()->getNameAsStr();

  auto members = currClassEnv->getMembers();
  if (auto idType = members.lookup(idName)) {
    node->getInitExpr()->accept(this);
    auto* initExprType = node->getInitExpr()->getSemantType();

    auto idGraphNode = graph->getInheritanceNode(idType.value()->getAsString());
    auto initGraphNode = graph->getInheritanceNode(initExprType->getAsString());

    if (initGraphNode.isChildOf(&idGraphNode)) {
      node->setSemantType(idType.value());
    } else {
      std::stringstream errStream;
      errStream << "type `" << initExprType->getAsString() << "` does not conform to `"
                << idType.value()->getAsString() << "` of `" << idName << "` identifier."
                << " See at " << node->getLocation();
      node->setSemantType(errorType);
    }
  } else {
    std::stringstream errStream;
    errStream << "could not find `" << idName << "` in the current scope. See at "
              << node->getLocation();
    errList.push_back(errStream.str());
    node->setSemantType(errorType);
  }
}

void TypeChecker::visitNewExpr(ast::NewExpr* newExpr) {
  auto& typeName = newExpr->getNewType()->getNameAsStr();

  if (typeName == "SELF_TYPE") {
    newExpr->setSemantType(selfType);
  } else {
    auto it = graph->findNode(typeName);
    if (it != graph->end()) {
      newExpr->setSemantType(typeBuilder->getType(typeName));
    } else {
      newExpr->setSemantType(errorType);
    }
  }
}

void TypeChecker::visitNegationNode(ast::NegationNode* negExpr) {
  negExpr->getTerm()->accept(this);
  auto resultType = negExpr->getTerm()->getSemantType();

  if (resultType->getTypeKind() == type::TypeKind::Int) {
    negExpr->setSemantType(typeBuilder->getType("Int"));
  } else {
    std::stringstream errStream;
    errStream << "expr has type `" << resultType->getAsString() << "`, expected `Int`. See at "
              << negExpr->getLocation();
    negExpr->setSemantType(errorType);
  }
}

void TypeChecker::visitNotExpr(ast::NotExpr* notExpr) {
  notExpr->getExpr()->accept(this);
  auto resultType = notExpr->getExpr()->getSemantType();

  if (resultType->getTypeKind() == type::TypeKind::Bool) {
    notExpr->setSemantType(typeBuilder->getType("Bool"));
  } else {
    std::stringstream errStream;
    errStream << "expr has type `" << resultType->getAsString() << "`, expected `Bool`"
              << ". See at " << notExpr->getLocation();
    errList.push_back(errStream.str());

    notExpr->setSemantType(errorType);
  }
}

void TypeChecker::visitIsVoidNode(ast::IsVoidNode* node) {
  node->getTerm()->accept(this);

  auto* resultType = node->getTerm()->getSemantType();
  assert(resultType != nullptr);
  node->setSemantType(typeBuilder->getType("Bool"));
}

void TypeChecker::visitPlusNode(ast::PlusNode* node) {
  auto* result = getBinaryExprType(node);
  assert(result != nullptr);
  node->setSemantType(result);
}

void TypeChecker::visitMinusNode(ast::MinusNode* node) {
  auto* result = getBinaryExprType(node);
  assert(result != nullptr);
  node->setSemantType(result);
}

void TypeChecker::visitMultiplyNode(ast::MultiplyNode* node) {
  auto* result = getBinaryExprType(node);
  assert(result != nullptr);
  node->setSemantType(result);
}

void TypeChecker::visitDivideNode(ast::DivideNode* node) {
  auto* result = getBinaryExprType(node);
  assert(result != nullptr);
  node->setSemantType(result);
}

void TypeChecker::visitLessNode(ast::LessNode* node) {
  auto* result = getLogicalExprType(node);
  assert(result != nullptr);
  node->setSemantType(result);
}

void TypeChecker::visitLessEqualNode(ast::LessEqualNode* node) {
  auto* result = getLogicalExprType(node);
  assert(result != nullptr);
  node->setSemantType(result);
}

void TypeChecker::visitEqualNode(ast::EqualNode* node) {
  auto* result = getLogicalExprType(node);
  assert(result != nullptr);
  node->setSemantType(result);
}

type::Type* TypeChecker::getBinaryExprType(ast::BinaryExpression* binaryExpr) {
  binaryExpr->getLeft()->accept(this);
  auto* leftOperandType = binaryExpr->getLeft()->getSemantType();
  assert(leftOperandType != nullptr);

  binaryExpr->getRight()->accept(this);
  auto* rightOperandType = binaryExpr->getRight()->getSemantType();
  assert(rightOperandType != nullptr);

  if (leftOperandType->isSame(rightOperandType)) {
    return typeBuilder->getType(leftOperandType->getAsString());
  } else {
    mcool::Loc loc(binaryExpr->getLeft()->getLocation(), binaryExpr->getRight()->getLocation());
    std::stringstream errStream;
    errStream << "Types do not match in binary expression. "
              << "Left operand type: `" << leftOperandType->getAsString()
              << "`. Right operand type: `" << rightOperandType->getAsString() << "`. See at "
              << loc;
    errList.push_back(errStream.str());
    return errorType;
  }
}

type::Type* TypeChecker::getLogicalExprType(ast::BinaryExpression* binaryExpr) {
  auto* result = getBinaryExprType(binaryExpr);
  assert(result != nullptr);
  if (result->isSame(errorType)) {
    return errorType;
  } else {
    return typeBuilder->getType("Bool");
  }
}

void TypeChecker::visitNoExpr(ast::NoExpr* noExpr) {
  auto* noExprType = typeBuilder->getType("_no_type");
  noExpr->setSemantType(noExprType);
}

void TypeChecker::visitObjectId(ast::ObjectId* id) {
  auto& idName = id->getNameAsStr();
  auto& members = currClassEnv->getMembers();
  if (auto idType = members.lookup(idName)) {
    id->setSemantType(idType.value());
  } else {
    std::stringstream errStream;
    errStream << "could not find `" << idName << "` in the current scope. See at "
              << id->getLocation();
    errList.push_back(errStream.str());
    id->setSemantType(errorType);
  }
}

void TypeChecker::visitBool(ast::Bool* item) {
  auto* type = typeBuilder->getType("Bool");
  item->setSemantType(type);
}

void TypeChecker::visitInt(ast::Int* item) {
  auto* type = typeBuilder->getType("Int");
  item->setSemantType(type);
}

void TypeChecker::visitString(ast::String* str) {
  auto* type = typeBuilder->getType("String");
  str->setSemantType(type);
}

bool TypeChecker::doesTypeExist(const std::string& typeName, Loc& loc) {
  const auto& it = graph->findNode(typeName);
  if (it == graph->end()) {
    std::stringstream errStream;
    errStream << "type `" << typeName << "` does not exist. See at " << loc;
    errList.push_back(errStream.str());
    return false;
  } else {
    return true;
  }
}

bool TypeChecker::isAllMethodArgsOk(type::MethodType* methodType, std::list<ast::Node*>& args) {
  auto& paramTypes = methodType->getParameters();
  if (paramTypes.size() != args.size()) {
    return false;
  }

  auto argsIt = args.begin();
  for (size_t i = 0; i < paramTypes.size(); ++i, ++argsIt) {
    const auto& paramNode = graph->getInheritanceNode(paramTypes[i]->getAsString());
    auto* argType = (*argsIt)->getSemantType();
    const auto& argNode = graph->getInheritanceNode(argType->getAsString());

    if (!argNode.isChildOf(&paramNode)) {
      return false;
    }
  }

  return true;
}

} // namespace mcool::semant