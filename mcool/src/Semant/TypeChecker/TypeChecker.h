#pragma once

#include "visitor.h"
#include "Context.h"
#include "Types/Types.h"
#include "Types/Environment.h"
#include "Semant/TypeChecker/Error.h"
#include <deque>

namespace mcool::semant {
class TypeChecker : public ast::Visitor, public Error {
  public:
  explicit TypeChecker(Context& context, type::TypeEnvironments& environments)
      : context(context), typeBuilder(context.getTypeBulder()), environments(environments),
        graph(context.getInheritanceGraph()) {
    errorType = typeBuilder->getType("Object");
  }

  void run(ast::CoolClassList* coolClassList);

  private:
  void visitCoolClass(ast::CoolClass* coolClass) override;
  void visitAttributeList(ast::AttributeList* attributeList) override;
  void visitSingleMember(ast::SingleMember* member) override;
  void visitSingleMethod(ast::SingleMethod* method) override;
  void visitFormalList(ast::FormalList* formalList) override;
  void visitSingleFormal(ast::SingleFormal* formal) override;
  void visitBlockExpr(ast::BlockExpr* block) override;
  void visitExpressions(ast::Expressions* node) override;
  void visitWhileLoop(ast::WhileLoop* loop) override;
  void visitNegationNode(ast::NegationNode* node) override;
  void visitPrimaryExpr(ast::PrimaryExpr* node) override;
  void visitIsVoidNode(ast::IsVoidNode* node) override;
  void visitNotExpr(ast::NotExpr* noExpr) override;
  void visitDispatch(ast::Dispatch* dispatch) override;
  void visitStaticDispatch(ast::StaticDispatch* dispatch) override;
  void visitNewExpr(ast::NewExpr* newExpr) override;
  void visitCaseExpr(ast::CaseExpr* caseExpr) override;
  void visitCaseList(ast::CaseList* caseList) override;
  void visitSingleCase(ast::SingleCase* aCase) override;
  void visitPlusNode(ast::PlusNode* node) override;
  void visitMinusNode(ast::MinusNode* node) override;
  void visitMultiplyNode(ast::MultiplyNode* node) override;
  void visitDivideNode(ast::DivideNode* node) override;
  void visitLessNode(ast::LessNode* node) override;
  void visitLessEqualNode(ast::LessEqualNode* node) override;
  void visitEqualNode(ast::EqualNode* node) override;
  void visitAssignExpr(ast::AssignExpr* node) override;
  void visitIfThenExpr(ast::IfThenExpr* condExpr) override;
  void visitIfThenElseExpr(ast::IfThenElseExpr* condExpr) override;
  void visitNoExpr(ast::NoExpr*) override;
  void visitLetExpr(ast::LetExpr* letExpr) override;
  void visitObjectId(ast::ObjectId* id) override;
  void visitBool(ast::Bool* item) override;
  void visitInt(ast::Int* item) override;
  void visitString(ast::String* str) override;

  type::Type* getBinaryExprType(ast::BinaryExpression* binaryExpr);
  type::Type* getLogicalExprType(ast::BinaryExpression* binaryExpr);
  bool doesTypeExist(const std::string& typeName, Loc& loc);
  bool isAllMethodArgsOk(type::MethodType* methodType, std::list<ast::Node*>& argsTypes);

  Context& context;
  std::unique_ptr<type::TypeBuilder>& typeBuilder;
  type::TypeEnvironments& environments;
  const std::unique_ptr<type::Graph>& graph;

  type::ClassEnvironment* currClassEnv{nullptr};
  std::string currClassName{};

  type::Type* selfType{nullptr};
  type::Type* errorType{nullptr};
};
} // namespace mcool::semant