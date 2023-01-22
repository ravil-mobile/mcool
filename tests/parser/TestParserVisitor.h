#pragma once

#include "TestAstTokens.h"
#include "ast.h"
#include "visitor.h"
#include <vector>


namespace mcool::tests::parser {
class TestVisitor : public ast::Visitor {
public:
  void visitCoolClassList(ast::CoolClassList* classList) override {
    auto& list = classList->getData();
    for (auto* coolClass : list) {
      coolClass->accept(this);
    }
  }

  void visitCoolClass(ast::CoolClass* coolClass) override {
    testTokens.push_back(TestAstTokens::CoolClass);
    coolClass->getAttributes()->accept(this);
}

  void visitAttributeList(ast::AttributeList* attributeList) override {
    auto& attributes = attributeList->getData();
    for (auto* attr : attributes) {
      attr->accept(this);
    }
  }

  void visitSingleMember(ast::SingleMember* member) override {
    testTokens.push_back(TestAstTokens::Member);
    member->getId()->accept(this);
    member->getIdType()->accept(this);
    member->getInitExpr()->accept(this);
  }

  void visitSingleMethod(ast::SingleMethod* method) override {
    testTokens.push_back(TestAstTokens::Method);
    method->getId()->accept(this);
    method->getParameters()->accept(this);
    method->getReturnType()->accept(this);
    method->getBody()->accept(this);
  }


  void visitFormalList(ast::FormalList* formalList) override {
    auto& formals = formalList->getFormals();
    for (auto* formal : formals) {
      formal->accept(this);
    }
  }

  void visitSingleFormal(ast::SingleFormal* formal) override {
    testTokens.push_back(TestAstTokens::Formal);
    formal->getId()->accept(this);
    formal->getIdType()->accept(this);
  }

  void visitBlockExpr(ast::BlockExpr* block) override {
    testTokens.push_back(TestAstTokens::Block);
    block->getExprs()->accept(this);
  }

  void visitExpressions(ast::Expressions* exprs) override {
    auto& list = exprs->getData();
    for (auto* expr : list) {
      expr->accept(this);
    }
  }

  void visitWhileLoop(ast::WhileLoop* loop) override {
    testTokens.push_back(TestAstTokens::LoopCond);
    loop->getPredicate()->accept(this);

    testTokens.push_back(TestAstTokens::LoopBody);
    loop->getBody()->accept(this);
  }

  void visitNegationNode(ast::NegationNode* node) override {
    testTokens.push_back(TestAstTokens::Neg);
    node->getTerm()->accept(this);
  }

  void visitPrimaryExpr(ast::PrimaryExpr* node) override {
    node->getTerm()->accept(this);
  }

  void visitIsVoidNode(ast::IsVoidNode* node) override {
    testTokens.push_back(TestAstTokens::Isvoid);
    node->getTerm()->accept(this);
  }

  void visitNotExpr(ast::NotExpr* expr) override {
    testTokens.push_back(TestAstTokens::Not);
    expr->getExpr()->accept(this);
  }

  void visitDispatch(ast::Dispatch* dispatch) override {
    testTokens.push_back(TestAstTokens::Dispatch);
    dispatch->getObjectId()->accept(this);
    dispatch->getMethodId()->accept(this);
    dispatch->getArguments()->accept(this);
  }


  void visitStaticDispatch(ast::StaticDispatch* dispatch) override {
    testTokens.push_back(TestAstTokens::StaticDispatch);
    dispatch->getObjectId()->accept(this);
    dispatch->getCastType()->accept(this);
    dispatch->getMethodId()->accept(this);
    dispatch->getArguments()->accept(this);
  }

  void visitNewExpr(ast::NewExpr* expr) override {
    testTokens.push_back(TestAstTokens::New);
    expr->getNewType()->accept(this);
  }

  void visitCaseExpr(ast::CaseExpr* caseExpr) override {
    testTokens.push_back(TestAstTokens::CaseCond);
    caseExpr->getExpr()->accept(this);
    caseExpr->getCasses()->accept(this);
  }

  void visitCaseList(ast::CaseList* caseList) override {
    auto& cases = caseList->getData();
    for (auto aCase : cases) {
      aCase->accept(this);
    }
  }

  void visitSingleCase(ast::SingleCase* aCase) override {
    testTokens.push_back(TestAstTokens::Case);
    aCase->getId()->accept(this);
    aCase->getIdType()->accept(this);
    aCase->getBody()->accept(this);
  }

  void visitPlusNode(ast::PlusNode* node) override {
    testTokens.push_back(TestAstTokens::Plus);
    visitBinaryNode(node);
  }

  void visitMinusNode(ast::MinusNode* node) override {
    testTokens.push_back(TestAstTokens::Minus);
    visitBinaryNode(node);
  }

  void visitMultiplyNode(ast::MultiplyNode* node) override {
    testTokens.push_back(TestAstTokens::Mult);
    visitBinaryNode(node);
  }

  void visitDivideNode(ast::DivideNode* node) override {
    testTokens.push_back(TestAstTokens::Div);
    visitBinaryNode(node);
  }

  void visitLessNode(ast::LessNode* node) override {
    testTokens.push_back(TestAstTokens::Less);
    visitBinaryNode(node);
  }

  void visitLessEqualNode(ast::LessEqualNode* node) override {
    testTokens.push_back(TestAstTokens::Leq);
    visitBinaryNode(node);
  }

  void visitEqualNode(ast::EqualNode* node) override {
    testTokens.push_back(TestAstTokens::Eq);
    visitBinaryNode(node);
  }

  void visitBinaryNode(ast::BinaryExpression* node) {
    node->getLeft()->accept(this);
    node->getRight()->accept(this);
  }

  void visitAssignExpr(ast::AssignExpr* node) override {
    testTokens.push_back(TestAstTokens::Assign);
    node->getId()->accept(this);
    node->getInitExpr()->accept(this);
  }

  void visitIfThenExpr(ast::IfThenExpr* condExpr) override {
    testTokens.push_back(TestAstTokens::IfCond);
    condExpr->getCondition()->accept(this);

    testTokens.push_back(TestAstTokens::IfThen);
    condExpr->getThenBody()->accept(this);
  }

  void visitIfThenElseExpr(ast::IfThenElseExpr* condExpr) override {
    testTokens.push_back(TestAstTokens::IfCond);
    condExpr->getCondition()->accept(this);

    testTokens.push_back(TestAstTokens::IfThen);
    condExpr->getThenBody()->accept(this);

    testTokens.push_back(TestAstTokens::IfElse);
    condExpr->getElseBody()->accept(this);
  }

  void visitNoExpr(ast::NoExpr*) override {
    testTokens.push_back(TestAstTokens::NoExpr);
  }

  void visitLetExpr(ast::LetExpr* letExpr) override {
    testTokens.push_back(TestAstTokens::Let);
    letExpr->getId()->accept(this);
    letExpr->getIdType()->accept(this);
    letExpr->getInitExpr()->accept(this);
    letExpr->getBody()->accept(this);
  }

  void visitTypeId(ast::TypeId* type) override {
    testTokens.push_back(TestAstTokens::Type);
  }

  void visitObjectId(ast::ObjectId* id) override {
    testTokens.push_back(TestAstTokens::Object);
  }

  void visitString(ast::String*) override {
    testTokens.push_back(TestAstTokens::String);
  }

  void visitBool(ast::Bool* node) override {
    testTokens.push_back(TestAstTokens::Bool);
  }

  void visitInt(ast::Int* node) override {
    testTokens.push_back(TestAstTokens::Int);
  }

  std::vector<int>& getTestAstTokens() { return testTokens; }

private:
  std::vector<int> testTokens{};
};
} // namespace mcool::tests::parser