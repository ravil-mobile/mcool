#pragma once

#include "visitor.h"
#include "ast.h"
#include <ostream>


namespace mcool {
class AstPinter : public ast::Visitor {
public:
  explicit AstPinter(std::ostream& stream) : stream(stream) {}

  void visitCoolClassList(ast::CoolClassList* classList) override {
    auto& classes = classList->getData();
    for (auto* coolClass : classes) {
      coolClass->accept(this);
    }
  }

  void visitCoolClass(ast::CoolClass* coolClass) override {
    auto& className = coolClass->getCoolType()->getNameAsStr();
    stream << std::string(indent, ' ') << "class: " << className << "\n";
    indent += 2;
    coolClass->getAttributes()->accept(this);
    indent -= 2;
  }

  void visitAttributeList(ast::AttributeList* attributeList) override {
    auto& attrs = attributeList->getData();
    for (auto* attr : attrs) {
      attr->accept(this);
    }
  }

  void visitSingleMember(ast::SingleMember* member) override {
    auto& id = member->getId()->getNameAsStr();
    auto& type = member->getIdType()->getNameAsStr();
    stream << std::string(indent, ' ')
           << "member: "
           << id << " : "
           << type << "\n";
    indent += 2;
    member->getInitExpr()->accept(this);
    indent -= 2;
  }

  void visitSingleMethod(ast::SingleMethod* method) override {
    auto methodName = method->getId()->getNameAsStr();
    stream << std::string(indent, ' ') << "method: " << methodName << "\n";
    auto returnType = method->getReturnType()->getNameAsStr();

    indent += 2;
    stream << std::string(indent, ' ') << "return type: " << returnType << std::endl;
    stream << std::string(indent, ' ') << "parameters: " << std::endl;
    method->getParameters()->accept(this);

    stream << std::string(indent, ' ') << "body: " << "\n";
    method->getBody()->accept(this);
    indent -= 2;
  };

  void visitFormalList(ast::FormalList* formalList) override {
    auto& formals = formalList->getFormals();
    indent += 2;
    for (auto& formal : formals) {
      formal->accept(this);
    }
    indent -= 2;
  }

  void visitSingleFormal(ast::SingleFormal* formal) override {
    auto& id = formal->getId()->getNameAsStr();
    auto& type = formal->getIdType()->getNameAsStr();
    stream << std::string(indent, ' ') << id << " : " << type << std::endl;
  }

  void visitBlockExpr(ast::BlockExpr* block) override {
    stream << std::string(indent, ' ') << "block: \n";
    indent += 2;
    auto* exprs = block->getExprs();
    exprs->accept(this);
    indent -= 2;
  }

  void visitExpressions(ast::Expressions* node) override {
    auto& exprs = node->getData();
    indent += 2;
    for (auto* expr : exprs) {
      expr->accept(this);
    }
    indent -= 2;
  }

  void visitWhileLoop(ast::WhileLoop* loop) override {
    stream << std::string(indent, ' ') << "while: \n";
    indent += 2;

    stream << std::string(indent, ' ') << "cond: \n";
    indent += 2;
    loop->getPredicate()->accept(this);
    indent -= 2;

    stream << std::string(indent, ' ') << "body: \n";
    indent += 2;
    loop->getBody()->accept(this);
    indent -= 2;

    indent -= 2;
  }

  void visitNegationNode(ast::NegationNode* node) override {
    stream << std::string(indent, ' ') << "negate: " << "\n";
    indent += 2;
    auto* term = node->getTerm();
    term->accept(this);
    indent -= 2;
  }

  void visitPrimaryExpr(ast::PrimaryExpr* node) override {
    auto* term = node->getTerm();
    term->accept(this);
  }

  void visitIsVoidNode(ast::IsVoidNode* node) override {
    stream << std::string(indent, ' ') << "is void: " << "\n";
    indent += 2;
    auto* term = node->getTerm();
    term->accept(this);
    indent -= 2;
  }

  void visitNotExpr(ast::NotExpr* noExpr) override {
    stream << std::string(indent, ' ') << "not: " << "\n";
    indent += 2;
    noExpr->getExpr()->accept(this);
    indent -= 2;
  }

  void visitDispatch(ast::Dispatch* dispatch) override {
    stream << std::string(indent, ' ') << "call: \n";
    indent += 2;
    dispatch->getObjectId()->accept(this);
    dispatch->getMethodId()->accept(this);
    dispatch->getArguments()->accept(this);
    indent -= 2;
  }

  void visitStaticDispatch(ast::StaticDispatch* dispatch) override {
    stream << std::string(indent, ' ') << "call: \n";
    indent += 2;
    dispatch->getObjectId()->accept(this);

    stream << std::string(indent, ' ') << "cast to: \n";
    indent += 2;
    dispatch->getCastType()->accept(this);
    indent -= 2;

    dispatch->getMethodId()->accept(this);
    dispatch->getArguments()->accept(this);
    indent -= 2;  }

  void visitNewExpr(ast::NewExpr* newExpr) override {
    stream << std::string(indent, ' ') << "new:\n";
    indent += 2;
    newExpr->getNewType()->accept(this);
    indent -= 2;
  }

  void visitCaseExpr(ast::CaseExpr* caseExpr) override {
    stream << std::string(indent, ' ') << "cases:\n";
    indent += 2;

    stream << std::string(indent, ' ') << "cond:\n";
    indent += 2;
    caseExpr->getExpr()->accept(this);
    indent -= 2;

    stream << std::string(indent, ' ') << "body:\n";
    indent += 2;
    caseExpr->getCasses()->accept(this);
    indent -= 2;

    indent -= 2;
  }

  void visitCaseList(ast::CaseList* caseList) override {
    auto& data = caseList->getData();
    for (auto item : data) {
      item->accept(this);
    }
  }

  void visitSingleCase(ast::SingleCase* aCase) override {
    stream << std::string(indent, ' ') << "case:\n";
    indent += 2;

    aCase->getId()->accept(this);
    aCase->getIdType()->accept(this);

    stream << std::string(indent, ' ') << "body:\n";
    indent += 2;
    aCase->getBody()->accept(this);
    indent -= 2;

    indent -= 2;
  }

  void visitPlusNode(ast::PlusNode* node) override {
    stream << std::string(indent, ' ') << "+\n";
    indent += 2;
    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    indent -= 2;
  }

  void visitMinusNode(ast::MinusNode* node) override {
    stream << std::string(indent, ' ') << "-\n";
    indent += 2;
    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    indent -= 2;
  }

  void visitMultiplyNode(ast::MultiplyNode* node) override {
    stream << std::string(indent, ' ') << "*\n";
    indent += 2;
    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    indent -= 2;
  }

  void visitDivideNode(ast::DivideNode* node) override {
    stream << std::string(indent, ' ') << "/\n";
    indent += 2;
    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    indent -= 2;
  }

  void visitLessNode(ast::LessNode* node) override {
    stream << std::string(indent, ' ') << "<\n";
    indent += 2;
    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    indent -= 2;
  }

  void visitLessEqualNode(ast::LessEqualNode* node) override {
    stream << std::string(indent, ' ') << "<=\n";
    indent += 2;
    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    indent -= 2;
  }

  void visitEqualNode(ast::EqualNode* node) override {
    stream << std::string(indent, ' ') << "==\n";
    indent += 2;
    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    indent -= 2;
  }

  void visitAssignExpr(ast::AssignExpr* node) override {
    auto& id = node->getId()->getNameAsStr();
    stream << std::string(indent, ' ') << "assign `" << id << "`\n";
    indent += 2;
    node->getInitExpr()->accept(this);
    indent -= 2;
  }

  void visitIfThenExpr(ast::IfThenExpr* condExpr) override {
    stream << std::string(indent, ' ') << "if:\n";
    indent += 2;

    stream << std::string(indent, ' ') << "cond:\n";
    indent += 2;
    condExpr->getCondition()->accept(this);
    indent -= 2;

    stream << std::string(indent, ' ') << "then:\n";
    indent += 2;
    condExpr->getThenBody()->accept(this);
    indent -= 2;

    indent -= 2;
  }

  void visitIfThenElseExpr(ast::IfThenElseExpr* condExpr) override {
    stream << std::string(indent, ' ') << "if:\n";
    indent += 2;

    stream << std::string(indent, ' ') << "cond:\n";
    indent += 2;
    condExpr->getCondition()->accept(this);
    indent -= 2;

    stream << std::string(indent, ' ') << "then:\n";
    indent += 2;
    condExpr->getThenBody()->accept(this);
    indent -= 2;

    stream << std::string(indent, ' ') << "else:\n";
    indent += 2;
    condExpr->getElseBody()->accept(this);
    indent -= 2;

    indent -= 2;
  }

  void visitNoExpr(ast::NoExpr*) override {
    stream << std::string(indent, ' ') << "no expr.\n";
  }

  void visitLetExpr(ast::LetExpr* letExpr) override {
    stream << std::string(indent, ' ') << "let:\n";
    indent += 2;

    letExpr->getId()->accept(this);
    letExpr->getIdType()->accept(this);

    stream << std::string(indent, ' ') << "init:\n";
    indent += 2;
    letExpr->getInitExpr()->accept(this);
    indent -= 2;

    stream << std::string(indent, ' ') << "body:\n";
    indent += 2;
    letExpr->getBody()->accept(this);
    indent -= 2;

    indent -= 2;
  }

  void visitTypeId(ast::TypeId* type) override {
    stream << std::string(indent, ' ') << "type: " << type->getNameAsStr() << "\n";
  }

  void visitObjectId(ast::ObjectId* id) override {
    stream << std::string(indent, ' ') << "id: " << id->getNameAsStr() << "\n";
  }

  void visitBool(ast::Bool* item) override {
    stream << std::string(indent, ' ') << std::boolalpha << item->getValue() << "\n";
  }

  void visitInt(ast::Int* item) override {
    stream << std::string(indent, ' ') << item->getValue() << "\n";
  }

  void visitString(ast::String* str) override {
    stream << std::string(indent, ' ') << "str: " << str->getValueAsStr() << "\n";
  }

private:
  std::ostream& stream;
  size_t indent{0};
};
}
