#include "auxiliary.h"
#include "TestParserVisitor.h"
#include <iostream>

TEST(LetExpression, SimpleWithoutInit) {
  std::stringstream stream;
  stream << "class A {\n"
         << "  main() : Object {     \n"
         << "    let x: Int in x * 1 \n"
         << "  };                    \n"
         << "};\n";

  mcool::tests::AttrExtractor extractor(stream);
  mcool::ast::SingleMethod* method{nullptr};
  extractor.getAttr(method, 0, 0);

  mcool::ast::Expression* bodyExpr = method->getBody();
  auto* letExpr = dynamic_cast<mcool::ast::LetExpr*>(bodyExpr);
  ASSERT_TRUE(letExpr != nullptr);

  ASSERT_STREQ(letExpr->getId()->getNameAsStr().c_str(), "x");
  ASSERT_STREQ(letExpr->getIdType()->getNameAsStr().c_str(), "Int");

  bodyExpr = letExpr->getBody();
  auto* multExpr = dynamic_cast<mcool::ast::MultiplyNode*>(bodyExpr);
  ASSERT_TRUE(multExpr != nullptr);

  auto* leftExpr = dynamic_cast<mcool::ast::PrimaryExpr*>(multExpr->getLeft());
  ASSERT_TRUE(leftExpr != nullptr);
  ASSERT_TRUE(dynamic_cast<mcool::ast::ObjectId*>(leftExpr->getTerm()) != nullptr);

  auto* rightExpr = dynamic_cast<mcool::ast::PrimaryExpr*>(multExpr->getRight());
  ASSERT_TRUE(rightExpr != nullptr);
  ASSERT_TRUE(dynamic_cast<mcool::ast::Int*>(rightExpr->getTerm()) != nullptr);
}

TEST(LetExpression, SimpleWithInit) {
  std::stringstream stream;
  stream << "class A {\n"
         << "  main() : Object {          \n"
         << "    let y: Int <- 3 in y + 1 \n"
         << "  };                         \n"
         << "};\n";

  mcool::tests::AttrExtractor extractor(stream);
  mcool::ast::SingleMethod* method{nullptr};
  extractor.getAttr(method, 0, 0);

  mcool::ast::Expression* bodyExpr = method->getBody();
  auto* letExpr = dynamic_cast<mcool::ast::LetExpr*>(bodyExpr);
  ASSERT_TRUE(letExpr != nullptr);

  ASSERT_STREQ(letExpr->getId()->getNameAsStr().c_str(), "y");
  ASSERT_STREQ(letExpr->getIdType()->getNameAsStr().c_str(), "Int");

  auto* initExpr = letExpr->getInitExpr();
  ASSERT_TRUE(dynamic_cast<mcool::ast::PrimaryExpr*>(initExpr) != nullptr);

  auto* letBodyExpr = letExpr->getBody();
  ASSERT_TRUE(dynamic_cast<mcool::ast::PlusNode*>(letBodyExpr) != nullptr);
}

TEST(LetExpression, MultipleLocalVariables) {
  std::stringstream stream;
  stream << "class A {\n"
         << "  main() : Object {                                                   \n"
         << "    let x: Int <- 3, y: MyType, z: String <- \"my string \"  in y + 1 \n"
         << "  };                                                                  \n"
         << "};\n";

  mcool::tests::AttrExtractor extractor(stream);
  mcool::ast::SingleMethod* method{nullptr};
  extractor.getAttr(method, 0, 0);

  mcool::ast::Expression* methodBodyExpr = method->getBody();
  auto* currLetExpr = dynamic_cast<mcool::ast::LetExpr*>(methodBodyExpr);
  ASSERT_TRUE(currLetExpr != nullptr);

  ASSERT_STREQ(currLetExpr->getId()->getNameAsStr().c_str(), "x");
  ASSERT_STREQ(currLetExpr->getIdType()->getNameAsStr().c_str(), "Int");
  {
    auto* initExpr = currLetExpr->getInitExpr();
    auto* primaryExpr = dynamic_cast<mcool::ast::PrimaryExpr*>(initExpr);
    ASSERT_TRUE(primaryExpr != nullptr);
    ASSERT_TRUE(dynamic_cast<mcool::ast::Int*>(primaryExpr->getTerm()) != nullptr);

    auto* letBodyExpr = currLetExpr->getBody();
    currLetExpr = dynamic_cast<mcool::ast::LetExpr*>(letBodyExpr);
    ASSERT_TRUE(currLetExpr != nullptr);
  }

  ASSERT_STREQ(currLetExpr->getId()->getNameAsStr().c_str(), "y");
  ASSERT_STREQ(currLetExpr->getIdType()->getNameAsStr().c_str(), "MyType");
  {
    auto* initExpr = currLetExpr->getInitExpr();
    ASSERT_TRUE(dynamic_cast<mcool::ast::NoExpr*>(initExpr) != nullptr);

    auto* letBodyExpr = currLetExpr->getBody();
    currLetExpr = dynamic_cast<mcool::ast::LetExpr*>(letBodyExpr);
    ASSERT_TRUE(currLetExpr != nullptr);
  }

  ASSERT_STREQ(currLetExpr->getId()->getNameAsStr().c_str(), "z");
  ASSERT_STREQ(currLetExpr->getIdType()->getNameAsStr().c_str(), "String");

  {
    auto* initExpr = currLetExpr->getInitExpr();
    auto* primaryExpr = dynamic_cast<mcool::ast::PrimaryExpr*>(initExpr);
    ASSERT_TRUE(primaryExpr != nullptr);
    ASSERT_TRUE(dynamic_cast<mcool::ast::String*>(primaryExpr->getTerm()) != nullptr);

    auto* letBodyExpr = currLetExpr->getBody();
    ASSERT_TRUE(dynamic_cast<mcool::ast::PlusNode*>(letBodyExpr) != nullptr);
  }
}