#include "auxiliary.h"
#include "TestParserVisitor.h"
#include <iostream>


TEST(ConditionalExpr, SimpleIfExpr) {
  std::stringstream stream;
  stream <<  "class A {\n"
         << "  main() : Object {\n"
         << "    {\n"
         << "      if not i <= 0 then { foo(); i <- i - 1; } fi;"
         << "      if a + b < 0 then foo() else bar() fi;"
         << "    }\n"
         << "  };\n"
         << "};\n";

  mcool::tests::AttrExtractor extractor(stream);
  mcool::ast::SingleMethod *method{nullptr};
  extractor.getAttr(method, 0, 0);

  mcool::ast::Expression *bodyExpr = method->getBody();
  ASSERT_TRUE(dynamic_cast<mcool::ast::BlockExpr*>(bodyExpr) != nullptr);

  auto& exprs = dynamic_cast<mcool::ast::BlockExpr*>(bodyExpr)->getExprs()->getData();
  auto exprIt = exprs.begin();

  {
    auto *ifExpr = dynamic_cast<mcool::ast::IfThenExpr*>(*exprIt);
    ASSERT_TRUE(ifExpr != nullptr);

    auto* notExpr = dynamic_cast<mcool::ast::NotExpr*>(ifExpr->getCondition());
    ASSERT_TRUE(notExpr != nullptr);

    auto* thenExpr = dynamic_cast<mcool::ast::BlockExpr*>(ifExpr->getThenBody());
    ASSERT_TRUE(thenExpr != nullptr);
  }

  std::advance(exprIt, 1);
  {
    auto *ifExpr = dynamic_cast<mcool::ast::IfThenElseExpr*>(*exprIt);
    ASSERT_TRUE(ifExpr != nullptr);

    auto* lessExpr = dynamic_cast<mcool::ast::LessNode*>(ifExpr->getCondition());
    ASSERT_TRUE(lessExpr != nullptr);

    auto* thenExpr = dynamic_cast<mcool::ast::PrimaryExpr*>(ifExpr->getThenBody());
    ASSERT_TRUE(thenExpr != nullptr);

    auto* elseExpr = dynamic_cast<mcool::ast::PrimaryExpr*>(ifExpr->getElseBody());
    ASSERT_TRUE(elseExpr != nullptr);
  }
}

TEST(ConditionalExpr, NestedIfExpr) {
  std::stringstream stream;
  stream << "class A {\n"
         << "  main() : Object {\n"
         << "    {\n"
         << "      if i <= 0 then if a = 1 then true else false fi fi;"
         << "      if i < 0 then true else if i = 4 then false fi fi;"
         << "    }\n"
         << "  };\n"
         << "};\n";

  mcool::tests::AttrExtractor extractor(stream);
  mcool::ast::SingleMethod *method{nullptr};
  extractor.getAttr(method, 0, 0);

  mcool::ast::Expression *bodyExpr = method->getBody();
  ASSERT_TRUE(dynamic_cast<mcool::ast::BlockExpr*>(bodyExpr) != nullptr);

  auto& exprs = dynamic_cast<mcool::ast::BlockExpr*>(bodyExpr)->getExprs()->getData();
  auto exprIt = exprs.begin();

  {
    auto *ifExpr = dynamic_cast<mcool::ast::IfThenExpr*>(*exprIt);
    ASSERT_TRUE(ifExpr != nullptr);

    auto* lessEqExpr = dynamic_cast<mcool::ast::LessEqualNode*>(ifExpr->getCondition());
    ASSERT_TRUE(lessEqExpr != nullptr);

    auto* nestedIfExpr = dynamic_cast<mcool::ast::IfThenElseExpr*>(ifExpr->getThenBody());
    ASSERT_TRUE(nestedIfExpr != nullptr);

    {
      auto* eqExpr = dynamic_cast<mcool::ast::EqualNode*>(nestedIfExpr->getCondition());
      ASSERT_TRUE(eqExpr != nullptr);

      auto* thenExpr = dynamic_cast<mcool::ast::PrimaryExpr*>(nestedIfExpr->getThenBody());
      ASSERT_TRUE(thenExpr != nullptr);
      ASSERT_TRUE(dynamic_cast<mcool::ast::Bool*>(thenExpr->getTerm()) != nullptr);

      auto* elseExpr = dynamic_cast<mcool::ast::PrimaryExpr*>(nestedIfExpr->getThenBody());
      ASSERT_TRUE(elseExpr != nullptr);
      ASSERT_TRUE(dynamic_cast<mcool::ast::Bool*>(elseExpr->getTerm()) != nullptr);
    }
  }

  std::advance(exprIt, 1);
  {
    auto *ifExprThen = dynamic_cast<mcool::ast::IfThenElseExpr*>(*exprIt);
    ASSERT_TRUE(ifExprThen != nullptr);

    auto* lessEqExpr = dynamic_cast<mcool::ast::LessNode*>(ifExprThen->getCondition());
    ASSERT_TRUE(lessEqExpr != nullptr);

    auto* thenExpr = dynamic_cast<mcool::ast::PrimaryExpr*>(ifExprThen->getThenBody());
    ASSERT_TRUE(thenExpr != nullptr);
    ASSERT_TRUE(dynamic_cast<mcool::ast::Bool*>(thenExpr->getTerm()) != nullptr);

    auto* nestedElseExpr = dynamic_cast<mcool::ast::IfThenExpr*>(ifExprThen->getElseBody());
    ASSERT_TRUE(nestedElseExpr != nullptr);
    {
      auto* eqExpr = dynamic_cast<mcool::ast::EqualNode*>(nestedElseExpr->getCondition());
      ASSERT_TRUE(eqExpr != nullptr);

      auto* nestedThenExpr = dynamic_cast<mcool::ast::PrimaryExpr*>(nestedElseExpr->getThenBody());
      ASSERT_TRUE(nestedThenExpr != nullptr);
      ASSERT_TRUE(dynamic_cast<mcool::ast::Bool*>(nestedThenExpr->getTerm()) != nullptr);
    }
  }
}

TEST(ConditionalExpr, WhileLoop) {
  std::stringstream stream;
  stream << "class A {\n"
         << "  main() : Object {\n"
         << "    {\n"
         << "      while i <= 10 loop \n"
         << "        i <- i + 1       \n"
         << "      pool;              \n"
         << "    }\n"
         << "  };\n"
         << "};\n";

  mcool::tests::AttrExtractor extractor(stream);
  mcool::ast::SingleMethod *method{nullptr};
  extractor.getAttr(method, 0, 0);

  mcool::ast::Expression *bodyExpr = method->getBody();
  auto* blockExpr = dynamic_cast<mcool::ast::BlockExpr*>(bodyExpr);
  ASSERT_TRUE(blockExpr != nullptr);

  auto& exprs = blockExpr->getExprs()->getData();
  ASSERT_EQ(exprs.size(), 1);

  auto* whileExpr = dynamic_cast<mcool::ast::WhileLoop*>(*(exprs.begin()));
  ASSERT_TRUE(whileExpr != nullptr);

  auto* condExpr = dynamic_cast<mcool::ast::LessEqualNode*>(whileExpr->getPredicate());
  ASSERT_TRUE(condExpr != nullptr);

  auto* loopBodyExpr = dynamic_cast<mcool::ast::AssignExpr*>(whileExpr->getBody());
  ASSERT_TRUE(loopBodyExpr != nullptr);
}

TEST(ConditionalExpr, CaseExpr) {
  std::stringstream stream;
  stream << "class A {\n"
         << "  main() : Object {\n"
         << "    {\n"
         << "      case someVar of           \n"
         << "        a: Int => 1;            \n"
         << "        b: String => \"hello\"; \n"
         << "      esac;                     \n"
         << "    }\n"
         << "  };\n"
         << "};\n";

  mcool::tests::AttrExtractor extractor(stream);
  mcool::ast::SingleMethod *method{nullptr};
  extractor.getAttr(method, 0, 0);

  mcool::ast::Expression *bodyExpr = method->getBody();
  auto* blockExpr = dynamic_cast<mcool::ast::BlockExpr*>(bodyExpr);
  ASSERT_TRUE(blockExpr != nullptr);

  auto& exprs = blockExpr->getExprs()->getData();
  ASSERT_EQ(exprs.size(), 1);

  auto* caseExpr = dynamic_cast<mcool::ast::CaseExpr*>(*(exprs.begin()));
  ASSERT_TRUE(caseExpr != nullptr);

  auto& casses = caseExpr->getCasses()->getData();
  ASSERT_EQ(casses.size(), 2);

  auto caseIt = casses.begin();
  {
    ASSERT_STREQ((*caseIt)->getId()->getNameAsStr().c_str(), "a");
    ASSERT_STREQ((*caseIt)->getIdType()->getNameAsStr().c_str(), "Int");
    auto* bodyExpr = dynamic_cast<mcool::ast::PrimaryExpr*>((*caseIt)->getBody());
    ASSERT_TRUE(dynamic_cast<mcool::ast::Int*>(bodyExpr->getTerm()) != nullptr);
  }

  std::advance(caseIt, 1);
  {
    ASSERT_STREQ((*caseIt)->getId()->getNameAsStr().c_str(), "b");
    ASSERT_STREQ((*caseIt)->getIdType()->getNameAsStr().c_str(), "String");
    auto* bodyExpr = dynamic_cast<mcool::ast::PrimaryExpr*>((*caseIt)->getBody());
    ASSERT_TRUE(dynamic_cast<mcool::ast::String*>(bodyExpr->getTerm()) != nullptr);
  }
}