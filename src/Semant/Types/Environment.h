#pragma once

#include "Semant/Types/Types.h"
#include "Semant/Types/InheritanceGraph.h"
#include "Semant/Types/SymbolTable.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <optional>
#include <cassert>

namespace mcool::semant {
class ClassEnvironment {
  public:
  ClassEnvironment(std::string userClassName) : className(userClassName) {}

  using MembersTableType = SymbolTable<std::string, Type*>;
  using MethodsTableType = SymbolTable<std::string, MethodType*>;

  MembersTableType& getMembers() { return members; }
  MethodsTableType& getMethods() { return methods; }

  std::string getClassName() { return className; }
  void setInheritanceChain(const std::vector<Graph::Node*>& chain) { inheritanceChain = chain; }
  std::vector<Graph::Node*>& getInheritanceChain() { return inheritanceChain; }

  private:
  std::string className{};
  MembersTableType members{};
  MethodsTableType methods{};
  std::vector<Graph::Node*> inheritanceChain{};
};

class TypeEnvironments {
  public:
  ClassEnvironment* createClassEnvironment(const std::string& className) {
    assert(classes.find(className) == classes.end());

    auto* env = new ClassEnvironment(className);
    classes.insert(std::make_pair(className, env));
    return env;
  }

  auto getAllClassNames() {
    std::vector<std::string> names{};
    for (auto& classInstance : classes) {
      names.push_back(classInstance.first);
    }
    return names;
  }

  std::optional<ClassEnvironment*> getClass(const std::string& className) {
    std::optional<ClassEnvironment*> result{};
    if (classes.find(className) != classes.end()) {
      result = classes[className].get();
    }
    return result;
  }

  private:
  std::unordered_map<std::string, std::unique_ptr<ClassEnvironment>> classes{};
};
} // namespace mcool::semant