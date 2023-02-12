#include "Semant/TypeChecker/EnvironmentsBuilder.h"
#include <sstream>

namespace mcool::semant {
type::TypeEnvironments EnvironmentsBuilder::build(ast::CoolClassList* coolClassList) {
  typeEnvironment = type::TypeEnvironments();
  collectMethodAndAttrs(coolClassList->getData());
  inflateClassEnv(coolClassList->getData());
  return std::move(typeEnvironment);
}

void EnvironmentsBuilder::collectMethodAndAttrs(std::list<ast::CoolClass*>& coolClasses) {
  for (auto* coolClass : coolClasses) {
    auto& className = coolClass->getCoolType()->getNameAsStr();

    currClassAttributes = ClassAttributes();
    coolClass->getAttributes()->accept(this);
    classAttributesTable.insert({className, currClassAttributes});
  }
}

void EnvironmentsBuilder::visitAttributeList(ast::AttributeList* attributeList) {
  auto& attrs = attributeList->getData();
  for (auto* attr : attrs) {
    attr->accept(this);
  }
}

void EnvironmentsBuilder::visitSingleMember(ast::SingleMember* member) {
  auto& memberName = member->getId()->getNameAsStr();
  auto& typeName = member->getIdType()->getNameAsStr();
  auto* memberType = typeBuilder->getType(typeName);
  currClassAttributes.members.insert({memberName, memberType});
}

void EnvironmentsBuilder::visitSingleMethod(ast::SingleMethod* method) {
  auto& returnTypeName = method->getReturnType()->getNameAsStr();
  auto* returnType = typeBuilder->getType(returnTypeName);

  currFormalParameters.clear();
  method->getParameters()->accept(this);

  auto& methodName = method->getId()->getNameAsStr();
  auto* methodType = typeBuilder->getMethodType(methodName, returnType, currFormalParameters);
  currClassAttributes.methods.insert({methodName, methodType});
}

void EnvironmentsBuilder::visitFormalList(ast::FormalList* formalList) {
  auto& formals = formalList->getFormals();
  for (auto* formal : formals) {
    auto& typeName = formal->getIdType()->getNameAsStr();
    currFormalParameters.push_back(typeBuilder->getType(typeName));
  }
}

void EnvironmentsBuilder::inflateClassEnv(std::list<ast::CoolClass*>& coolClasses) {
  const auto& graph = context.getInheritanceGraph();
  for (auto* coolClass : coolClasses) {
    auto& className = coolClass->getCoolType()->getNameAsStr();
    auto& node = graph->findNode(className)->second;

    std::vector<type::Graph::Node*> nodes{};
    type::findInheritanceNodes(&node, nodes);

    auto* env = typeEnvironment.createClassEnvironment(className);
    env->setInheritanceChain(nodes);

    auto& memberSymTable = env->getMembers();
    for (auto it = nodes.rbegin(); it != nodes.rend(); ++it) {
      auto& nodeName = (*it)->getNodeName();
      auto& classAttributes = classAttributesTable[nodeName];

      for (auto& [memberName, memberType] : classAttributes.members) {
        if (memberSymTable.lookup(memberName)) {
          std::stringstream errStream;
          errStream << "redefinition of member `" << memberName << "` in class `" << className
                    << " at ` (see parent classes as well)";
          errList.push_back(errStream.str());
        } else {
          memberSymTable.add(memberName, memberType);
        }
      }
    }

    auto& methodSymTable = env->getMethods();
    auto& classAttributes = classAttributesTable[className];
    for (auto& [methodName, methodType] : classAttributes.methods) {
      bool hasAlreadyBeenDefined{false};
      if (auto currType = methodSymTable.lookup(methodName)) {
        if (currType.value()->isSame(methodType)) {
          std::stringstream errStream;
          errStream << "redefinition of method `" << methodName << "` in class `" << className
                    << "` (see parent classes as well)";
          errList.push_back(errStream.str());
          hasAlreadyBeenDefined = true;
        }
      }
      if (!hasAlreadyBeenDefined) {
        methodSymTable.add(methodName, methodType);
      }
    }

    auto selfName = std::string("self");
    auto* selfType = typeBuilder->getType(className);
    memberSymTable.add(selfName, selfType);
  }
}
} // namespace mcool::semant