#pragma once


namespace mcool::semant {
enum class TypeKind {
  Object,
  SelfType,
  Bool,
  Int,
  String,
  IO,
  DerivedType,
  MethodType
};

class Type {
public:
  virtual ~Type() = default;
  virtual bool isBuiltintType() = 0;
  virtual std::string getAsString() = 0;
  virtual TypeKind getTypeKind() = 0;
};
} // namespace mcool::semant
