#pragma once

#include "Misc.h"
#include <vector>
#include <string>

namespace mcool::type {
enum class TypeKind { Object, SelfType, Bool, Int, String, IO, DerivedType, MethodType };

class Type {
  public:
  virtual ~Type() = default;
  virtual bool isBuiltinType() = 0;
  virtual std::string getAsString() = 0;
  virtual TypeKind getTypeKind() = 0;
};

class BuiltinType : public Type {
  public:
  ~BuiltinType() override = default;
  bool isBuiltinType() override { return true; };
};

class ObjectType : public BuiltinType {
  public:
  ~ObjectType() override = default;
  std::string getAsString() override { return "Object"; }
  TypeKind getTypeKind() override { return TypeKind::Object; }
};

class SelfType : public BuiltinType {
  public:
  ~SelfType() override = default;
  std::string getAsString() override { return "SELF_TYPE"; }
  TypeKind getTypeKind() override { return TypeKind::SelfType; }
};

class BoolType : public BuiltinType {
  public:
  ~BoolType() override = default;
  std::string getAsString() override { return "Bool"; }
  TypeKind getTypeKind() override { return TypeKind::Bool; }
};

class IntType : public BuiltinType {
  public:
  ~IntType() override = default;
  std::string getAsString() override { return "Int"; }
  TypeKind getTypeKind() override { return TypeKind::Int; }
};

class StringType : public BuiltinType {
  public:
  ~StringType() override = default;
  std::string getAsString() override { return "String"; }
  TypeKind getTypeKind() override { return TypeKind::String; }
};

class IOType : public BuiltinType {
  public:
  ~IOType() override = default;
  std::string getAsString() override { return "IO"; }
  TypeKind getTypeKind() override { return TypeKind::IO; }
};

class DerivedType : public Type {
  public:
  DerivedType() = default;
  DerivedType(std::string type, std::string parent = "Object")
      : typeName(type), parentTypeName(parent) {}
  ~DerivedType() override = default;
  bool isBuiltinType() override { return false; }
  std::string getAsString() override { return typeName; }
  std::string getParentAsString() { return parentTypeName; }
  TypeKind getTypeKind() override { return TypeKind::DerivedType; }

  protected:
  std::string typeName{};
  std::string parentTypeName{};
};

class MethodType : public Type {
  public:
  MethodType(std::string method, Type* methodReturnType, std::vector<Type*> parameterList)
      : methodName(method), returnType(methodReturnType), parameters(parameterList) {}
  ~MethodType() override = default;

  bool isBuiltinType() override { return false; }
  std::string getAsString() override { return methodName; }
  std::vector<Type*>& getParameters() { return parameters; }
  size_t getNumParameters() { return parameters.size(); }
  Type* getReturnType() { return returnType; }
  TypeKind getTypeKind() override { return TypeKind::MethodType; }
  bool isSame(MethodType* type);

  protected:
  std::string methodName{};
  Type* returnType{nullptr};
  std::vector<Type*> parameters{};
};

} // namespace mcool::type
