#pragma once

#include "CodeGen/BaseBuilder.h"
#include "SymbolTable.h"
#include <unordered_map>
#include <string>

namespace mcool::codegen {
class Initializer : public BaseBuilder {
  public:
  explicit Initializer(Environment& env, mcool::AstTree& classes)
      : BaseBuilder(env), classes(classes) {}
  void run() {
    initGlobalMembersTable();
    initClassTagTable();
    genClassDeclarations();
    genConstructorDeclarations();
    genMethodDeclarations();
    initGlobalMethodsTable();
    genDispatchTables();
    genCoolClassTypes();
    genCoolClassPrototypes();
    initClassTagTable();
  }

  private:
  void initGlobalMembersTable();
  void initClassTagTable();
  void genClassDeclarations();
  void genMethodDeclarations();
  void genConstructorDeclarations();
  void initGlobalMethodsTable();
  void genDispatchTables();
  void genCoolClassTypes();
  void genCoolClassPrototypes();
  void createGlobalVariable(ast::CoolClass* coolClass, const std::string& variableName);

  mcool::AstTree& classes;
  std::unordered_map<std::string, ast::CoolClass*> lookup{};
};
} // namespace mcool::codegen