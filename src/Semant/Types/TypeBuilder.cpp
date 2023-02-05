#include "Semant/Types/TypeBuilder.h"
#include <cassert>
#include <sstream>

namespace mcool::semant {
TypeBuilder::~TypeBuilder() {
  for (auto& type : types) {
    delete type.second;
  }
  types.clear();
}

Type* TypeBuilder::getType(mcool::Context& context, const std::string& typeName) {
  Type* type{};
  if (typeName == "Object") {
    type = &objectType;
  } else if (typeName == "SELF_TYPE") {
    type = &selfType;
  } else if (typeName == "Bool") {
    type = &boolType;
  } else if (typeName == "Int") {
    type = &intType;
  } else if (typeName == "String") {
    type = &stringType;
  } else if (typeName == "IO") {
    type = &io;
  } else {
    const auto& graph = context.getInheritanceGraph();

    const auto it = graph->findNode(typeName);
    bool found = it != graph->end();
    if (found) {
      assert(it->second.getParent() && "parent node is nullptr");
      auto typeIt = types.find(typeName);
      if (typeIt != types.end()) {
        type = typeIt->second;
      } else {
        type = new DerivedType(typeName, it->second.getParent()->getNodeName());
        types[typeName] = type;
      }
    } else {
      type = &objectType;
    }
  }

  return type;
}

bool MethodType::isSame(MethodType* type) {
  assert(type != nullptr);

  if (this->methodName != type->methodName) {
    return false;
  }

  const size_t size{parameters.size()};
  if (size != type->parameters.size()) {
    return false;
  }

  for (size_t i = 0; i < size; ++i) {
    if (parameters[i]->getAsString() != type->parameters[i]->getAsString()) {
      return false;
    }
  }

  return true;
}

MethodType* TypeBuilder::getMethodType(const std::string& methodName,
                                       Type* methodReturnType,
                                       const std::vector<Type*>& parameterList) {
  std::stringstream typeNameStream;
  constexpr char delim{'_'};
  typeNameStream << "!method" << delim << methodName;
  typeNameStream << delim << "!params";
  for (auto* param : parameterList) {
    typeNameStream << delim << param->getAsString();
  }
  typeNameStream << delim << "!return";
  typeNameStream << delim << methodReturnType->getAsString();

  std::string typeName = typeNameStream.str();
  auto typeIt = types.find(typeName);

  MethodType* methodType{nullptr};
  if (typeIt != types.end()) {
    methodType = dynamic_cast<MethodType*>(typeIt->second);
    assert(methodType != nullptr);
  } else {
    methodType = new MethodType(methodName, methodReturnType, parameterList);
    types[typeName] = methodType;
  }

  return methodType;
}
} // namespace mcool::semant