#include "auxiliary.h"
#include "TestParserVisitor.h"
#include <iostream>


TEST(MethodsDecl, SingleEmptyMethod) {
  std::stringstream stream;
  stream << "class A {\n main() : Object { true }; \n };";

  mcool::tests::AttrExtractor extractor(stream);
  mcool::ast::SingleMethod* method{nullptr};
  extractor.getAttr(method, 0, 0);

  ASSERT_STREQ(method->getId()->getNameAsStr().c_str(), "main");
  ASSERT_STREQ(method->getReturnType()->getNameAsStr().c_str(), "Object");

  auto* bodyExpr = method->getBody();
  auto* primaryExpr = dynamic_cast<mcool::ast::PrimaryExpr*>(bodyExpr);
  ASSERT_TRUE(primaryExpr != nullptr);

  auto* boolLiteral = primaryExpr->getTerm();
  ASSERT_TRUE(dynamic_cast<mcool::ast::Bool*>(boolLiteral) != nullptr);
}

TEST(MethodsDecl, TwoEmptyMethods) {
  std::stringstream stream;
  stream << "class A {          \n"
         << "  bar() : String { \n"
         << "    false          \n"
         << "  };               \n"
         << "  foo(a: String, b: Int) : Int {\n"
         << "    true \n"
         << "  };     \n"
         << "};       \n";

  mcool::ast::SingleMethod* method{nullptr};
  mcool::tests::AttrExtractor extractor(stream);

  extractor.getAttr(method, 0, 0);
  ASSERT_STREQ(method->getId()->getNameAsStr().c_str(), "bar");
  ASSERT_STREQ(method->getReturnType()->getNameAsStr().c_str(), "String");

  extractor.getAttr(method, 0, 1);
  ASSERT_STREQ(method->getId()->getNameAsStr().c_str(), "foo");
  ASSERT_STREQ(method->getReturnType()->getNameAsStr().c_str(), "Int");

  auto& formals = method->getParameters()->getFormals();
  ASSERT_EQ(formals.size(), 2);

  auto formalIt = formals.begin();

  auto* currFormal = (*formalIt);
  ASSERT_STREQ(currFormal->getId()->getNameAsStr().c_str(), "a");
  ASSERT_STREQ(currFormal->getIdType()->getNameAsStr().c_str(), "String");

  std::advance(formalIt, 1);

  currFormal = (*formalIt);
  ASSERT_STREQ(currFormal->getId()->getNameAsStr().c_str(), "b");
  ASSERT_STREQ(currFormal->getIdType()->getNameAsStr().c_str(), "Int");
}