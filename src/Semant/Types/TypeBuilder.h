#pragma once

#include "Semant/Types/Types.h"
#include "Context.h"
#include <unordered_map>

namespace mcool::semant {
class TypeBuilder {
  public:
  ~TypeBuilder();

  Type* getType(mcool::Context& context, const std::string& typeName);
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
};
} // namespace mcool::semant
