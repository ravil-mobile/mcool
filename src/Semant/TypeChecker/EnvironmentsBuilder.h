#pragma once

#include "visitor.h"
#include "Context.h"
#include "Types/Types.h"
#include "Types/Environment.h"
#include "Semant/TypeChecker/Error.h"
#include <unordered_map>
#include <vector>

namespace mcool::semant {
class EnvironmentsBuilder : public ast::Visitor, public Error {
  public:
  explicit EnvironmentsBuilder(Context& context)
      : context(context), typeBuilder(context.getTypeBulder()) {}
  type::TypeEnvironments build(ast::CoolClassList* coolClassList);

  private:
  void collectMethodAndAttrs(std::list<ast::CoolClass*>& coolClasses);
  void visitAttributeList(ast::AttributeList* attributeList) override;
  void visitSingleMember(ast::SingleMember* member) override;
  void visitSingleMethod(ast::SingleMethod* method) override;
  void visitFormalList(ast::FormalList* formalList) override;

  void inflateClassEnv(std::list<ast::CoolClass*>& coolClasses);

  std::vector<type::Type*> currFormalParameters{};
  type::TypeEnvironments typeEnvironment{};

  struct ClassAttributes {
    std::unordered_map<std::string, type::Type*> members{};
    std::unordered_map<std::string, type::MethodType*> methods{};
  };

  ClassAttributes currClassAttributes{};
  std::unordered_map<std::string, ClassAttributes> classAttributesTable{};
  Context& context;
  std::unique_ptr<type::TypeBuilder>& typeBuilder;
};
} // namespace mcool::semant