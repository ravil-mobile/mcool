#pragma once

#include "visitor.h"
#include "CodeGen/BaseBuilder.h"
#include <deque>

namespace mcool::codegen {
class CodeBuilder : public BaseBuilder, public ast::Visitor {
  public:
  explicit CodeBuilder(Environment& env) : BaseBuilder(env) {}

  void genConstructors(mcool::AstTree& classes);
  void genMethods(mcool::AstTree& classes);
  void generatedMainEntryPoint();

  private:
  void visitCoolClass(ast::CoolClass* coolClass) override;
  void visitSingleMember(ast::SingleMember* member) override;
  void visitSingleMethod(ast::SingleMethod* method) override;
  void visitFormalList(ast::FormalList* formalList) override;
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

  enum class IntegralBinaryOp { Plus, Minus, Mult, Div, Eq, Less, Leq };
  void visitBinaryNode(ast::BinaryExpression* node, IntegralBinaryOp op);
  void compareStringObjects(ast::BinaryExpression* node);
  void compareGeneralCoolObjects(ast::BinaryExpression* node);
  void getLObjValue(ast::ObjectId* id);

  void callParentsConstructors(llvm::Value* objPtr,
                               std::vector<type::Graph::Node*>& inheritanceChain);

  llvm::Value* popStack() {
    auto* value = stack.back();
    stack.pop_back();
    return value;
  }

  std::deque<llvm::Value*> stack;
  std::string currClassName{};
  SymbolTable currSymbolTable{};
  llvm::Function* currLLVMFunction{};
  llvm::PointerType* currFuncReturnType{};
};
} // namespace mcool::codegen