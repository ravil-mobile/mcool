#pragma once

#include "Types/Types.h"
#include "InheritanceGraph.h"
#include <unordered_map>
#include <memory>

namespace mcool::type {
class TypeBuilder {
  public:
  TypeBuilder() = default;
  ~TypeBuilder();

  void setInheritanceGraph(type::Graph* graph) { inheritanceGraph = graph; }
  Type* getType(const std::string& typeName);
  MethodType* getMethodType(const std::string& methodName,
                            Type* methodReturnType,
                            const std::vector<Type*>& parameterList);

  private:
  std::unordered_map<std::string, Type*> types;

  ObjectType objectType{};
  SelfType selfType{};
  BoolType boolType{};
  IntType intType{};
  StringType stringType{};
  IOType io{};

  type::Graph* inheritanceGraph{};
};
} // namespace mcool::type
