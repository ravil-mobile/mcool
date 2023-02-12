#pragma once

#include "Types/Types.h"
#include "InheritanceGraph.h"
#include <unordered_map>
#include <memory>

namespace mcool::type {
class TypeBuilder {
  public:
  TypeBuilder(std::unique_ptr<type::Graph>& graph) : inheritanceGraph(graph) {}
  ~TypeBuilder();

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

  std::unique_ptr<type::Graph>& inheritanceGraph;
};
} // namespace mcool::type
