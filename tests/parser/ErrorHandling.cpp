#include "auxiliary.h"
#include "TestParserVisitor.h"
#include <iostream>


TEST(ErrorHandling, MemberSyntaxError) {
  std::stringstream stream;
  stream << "class A {\n"
         << "    X : int <- 1;        \n"
         << "};\n";

  mcool::tests::parser::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_FALSE(ast.isAstOk());

  auto& coolClasses = ast.get()->getData();
  auto* coolClass = *(coolClasses.begin());
  auto& attrs = coolClass->getAttributes()->getData();
  ASSERT_TRUE(attrs.empty());
}


TEST(ErrorHandling, ReturnMethodTypeSyntaxError1) {
  std::stringstream stream;
  stream << "class A {\n"
         << "  foo(i : Int) : object {\n"
         << "    i <- 3 + 4\n"
         << "  };\n"
         << "};\n";

  mcool::tests::parser::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_FALSE(ast.isAstOk());

  auto& coolClasses = ast.get()->getData();
  auto* coolClass = *(coolClasses.begin());
  auto& attrs = coolClass->getAttributes()->getData();
  ASSERT_EQ(attrs.size(), 1);
}

TEST(ErrorHandling, ReturnMethodTypeSyntaxError2) {
  std::stringstream stream;
  stream << "class A {\n"
         << "  foo(i : Int) Object {\n"
         << "    i <- 3 + 4\n"
         << "  };\n"
         << "};\n";

  mcool::tests::parser::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_FALSE(ast.isAstOk());

  auto& coolClasses = ast.get()->getData();
  auto* coolClass = *(coolClasses.begin());
  auto& attrs = coolClass->getAttributes()->getData();
  ASSERT_EQ(attrs.size(), 1);
}

TEST(ErrorHandling, ParameterListMethodSyntaxError) {
  std::stringstream stream;
  stream << "class A {\n"
         << "  foo(I : int) : Object {\n"
         << "    i <- 3 + 4\n"
         << "  };\n"
         << "};\n";

  const bool expectedAstStatus{false};
  mcool::tests::AttrExtractor extractor(stream, expectedAstStatus);
  mcool::ast::SingleMethod *method{nullptr};
  extractor.getAttr(method, 0, 0);

  auto& formals = method->getParameters()->getFormals();
  ASSERT_TRUE(formals.empty());
}

TEST(ErrorHandling, MethodNameSyntaxError) {
  std::stringstream stream;
  stream << "class A {\n"
         << "  Foo(i : Int) : Object {\n"
         << "    i <- 3 + 4\n"
         << "  };\n"
         << "};\n";

  mcool::tests::parser::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_FALSE(ast.isAstOk());

  auto& coolClasses = ast.get()->getData();
  auto* coolClass = *(coolClasses.begin());
  auto& attrs = coolClass->getAttributes()->getData();
  ASSERT_TRUE(attrs.empty());
}

TEST(ErrorHandling, MethodBodySyntaxError1) {
  std::stringstream stream;
  stream << "class A {\n"
         << "  foo(i : Int) : Object {\n"
         << "    I <- 3 + 4\n"
         << "  };\n"
         << "};\n";

  const bool expectedAstStatus{false};
  mcool::tests::AttrExtractor extractor(stream, expectedAstStatus);
  mcool::ast::SingleMethod *method{nullptr};
  extractor.getAttr(method, 0, 0);

  auto* body = method->getBody();
  ASSERT_TRUE(dynamic_cast<mcool::ast::NoExpr*>(body));
}

TEST(ErrorHandling, MethodBodySyntaxError2) {
  std::stringstream stream;
  stream << "class A {\n"
         << "  foo(i : Int) : Object {\n"
         << "    {\n"
         << "      true;\n"
         << "      Obj();\n"
         << "      false;\n"
         << "    }\n"
         << "  };\n"
         << "};\n";

  const bool expectedAstStatus{false};
  mcool::tests::AttrExtractor extractor(stream, expectedAstStatus);
  mcool::ast::SingleMethod *method{nullptr};
  extractor.getAttr(method, 0, 0);

  auto* blockExpr = dynamic_cast<mcool::ast::BlockExpr*>(method->getBody());
  ASSERT_TRUE(blockExpr != nullptr);

  auto& exprs = blockExpr->getExprs()->getData();
  ASSERT_EQ(exprs.size(), 2);

  auto exprIt = exprs.begin();
  {
    auto expr = dynamic_cast<mcool::ast::PrimaryExpr*>(*exprIt);
    ASSERT_TRUE(dynamic_cast<mcool::ast::Bool*>(expr->getTerm()));
  }

  std::advance(exprIt, 1);
  {
    auto expr = dynamic_cast<mcool::ast::PrimaryExpr*>(*exprIt);
    ASSERT_TRUE(dynamic_cast<mcool::ast::Bool*>(expr->getTerm()));
  }
}