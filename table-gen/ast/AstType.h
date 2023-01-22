#pragma once

#include <string>
#include <memory>

namespace ast::type::details {
  struct Bool { static const char* asString() { return "bool"; } };
  struct Int { static const char* asString() { return "int"; } };
  struct String { static const char* asString() { return "std::string"; } };
  struct Location { static const char* asString() { return "mcool::Loc"; } };
}


namespace ast {
class Type {
public:
  Type(bool builtinFlag,
       bool compositeFlag,
       bool ptrFlag,
       bool preferReferenceFlag)
       : builtinFlag(builtinFlag),
         compositeFlag(compositeFlag),
         ptrFlag(ptrFlag),
         preferReferenceFlag(preferReferenceFlag) {}

  virtual std::string getName() = 0;
  bool isBuiltin() { return builtinFlag; }
  bool isComposite() { return compositeFlag; }
  bool isPtr() { return ptrFlag; }
  bool preferReference() { return preferReferenceFlag; }

private:
  bool builtinFlag;
  bool compositeFlag;
  bool ptrFlag;
  bool preferReferenceFlag;
};


template <typename T, bool PreferReferenceFlag>
class BuiltinType : public Type {
public:
  using ConcreteType = T;

  BuiltinType() : Type(true, false, false, PreferReferenceFlag) {}
  std::string getName() override { return std::string(T::asString()); }
};

using BoolType = BuiltinType<type::details::Bool, false>;
using IntType = BuiltinType<type::details::Int, false>;
using StringType = BuiltinType<type::details::String, true>;
using LocationType = BuiltinType<type::details::Location, true>;


class PtrType : public Type {
public:
  PtrType(std::string name) : Type(false, false, true, false), name(name + "*") {}
  std::string getName() override { return name; }

private:
  std::string name;
};


class Composite : public Type {
public:
  Composite(std::unique_ptr<Type> type)
  : Type(false, true, false, true),
    innerType(std::move(type)) {}

  Composite(Composite&& other) : Type(false, true, false, true) {
    innerType = std::move(other.innerType);
  }

  Composite& operator=(Composite&& other) = default;

  std::string getName() override {
    auto innerTypeName = innerType->getName();
    return "std::list<" + innerTypeName + ">";
  }

  Type* getInnerType() {
    return innerType.get();
  }

private:
  std::unique_ptr<Type> innerType{nullptr};
};
}