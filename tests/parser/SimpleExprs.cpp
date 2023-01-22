#include "auxiliary.h"
#include "TestParserVisitor.h"
#include <iostream>


TEST(SimpleExpressions, SingleSimpleExpr) {
  std::stringstream stream;
  stream << "class A {\n main() : Object { true }; \n };";

  mcool::ast::SingleMethod* method{nullptr};
  mcool::tests::AttrExtractor extractor(stream);
  extractor.getAttr(method, 0, 0);

  auto* primaryExpr = dynamic_cast<mcool::ast::PrimaryExpr*>(method->getBody());
  ASSERT_TRUE(primaryExpr != nullptr);

  auto* boolExpr = dynamic_cast<mcool::ast::Bool*>(primaryExpr->getTerm());
  ASSERT_TRUE(boolExpr != nullptr);
  ASSERT_EQ(boolExpr->getValue(), true);
}


TEST(SimpleExpressions, Block) {
  std::stringstream stream;
  stream << "class A {\n"
         << "  main() : Object {\n"
         << "    {\n"
         << "      false; \n"
         << "      true;  \n"
         << "      false; \n"
         << "    }\n"
         << "  };\n"
         << "};\n";

  mcool::ast::SingleMethod* method{nullptr};
  mcool::tests::AttrExtractor extractor(stream);
  extractor.getAttr(method, 0, 0);

  auto* blockExpr = dynamic_cast<mcool::ast::BlockExpr*>(method->getBody());
  ASSERT_TRUE(blockExpr != nullptr);

  auto& exprs = blockExpr->getExprs()->getData();
  ASSERT_EQ(exprs.size(), 3);

  std::array<bool, 3> expectedValues{false, true, false};

  size_t counter{0};
  for (auto it = exprs.begin(); it != exprs.end(); ++it, ++counter) {
    auto *primaryExpr = dynamic_cast<mcool::ast::PrimaryExpr *>(*it);

    auto *boolExpr = dynamic_cast<mcool::ast::Bool *>(primaryExpr->getTerm());
    ASSERT_TRUE(boolExpr != nullptr);
    ASSERT_EQ(boolExpr->getValue(), expectedValues[counter]);
  }
}


TEST(SimpleExpressions, Literals) {
  std::stringstream stream;
  stream << "class A {\n"
         << "  main() : Object {\n"
         << "    {\n"
         << "      false;           \n"
         << "      true;            \n"
         << "      123;             \n"
         << "      \"hello world\"; \n"
         << "      variable;        \n"
         << "    }\n"
         << "  };\n"
         << "};\n";

  mcool::tests::parser::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  mcool::tests::parser::TestVisitor visitor;
  ast.accept(&visitor);
  auto& testAstTokens = visitor.getTestAstTokens();

  using namespace mcool::tests::parser;
  std::vector<int> expectedAstTokens {
    TestAstTokens::CoolClass,
    TestAstTokens::Method,
    TestAstTokens::Object,
    TestAstTokens::Type,
    TestAstTokens::Block,
    TestAstTokens::Bool,
    TestAstTokens::Bool,
    TestAstTokens::Int,
    TestAstTokens::String,
    TestAstTokens::Object
  };

  ASSERT_EQ(testAstTokens.size(), expectedAstTokens.size());
  ASSERT_THAT(testAstTokens, ::testing::ElementsAreArray(expectedAstTokens));
}


TEST(SimpleExpressions, SimpleArithmetics) {
  std::stringstream stream;
  stream << "class A {\n"
         << "  main() : Object {\n"
         << "    {\n"
         << "      var1 + var2;  \n"
         << "      var1 - var2;  \n"
         << "      var1 * var2;  \n"
         << "      var1 / var2;  \n"
         << "      var1 < var2;  \n"
         << "      var1 <= var2; \n"
         << "      var1 = var2;  \n"
         << "      ~var2;        \n"
         << "    }\n"
         << "  };\n"
         << "};\n";
  
  mcool::tests::parser::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  mcool::tests::parser::TestVisitor visitor;
  ast.accept(&visitor);
  auto& testAstTokens = visitor.getTestAstTokens();

  using namespace mcool::tests::parser;
  std::vector<int> expectedAstTokens {
    TestAstTokens::CoolClass,
    TestAstTokens::Method,
    TestAstTokens::Object,
    TestAstTokens::Type,
    TestAstTokens::Block,

    TestAstTokens::Plus,
    TestAstTokens::Object,
    TestAstTokens::Object,

    TestAstTokens::Minus,
    TestAstTokens::Object,
    TestAstTokens::Object,

    TestAstTokens::Mult,
    TestAstTokens::Object,
    TestAstTokens::Object,

    TestAstTokens::Div,
    TestAstTokens::Object,
    TestAstTokens::Object,

    TestAstTokens::Less,
    TestAstTokens::Object,
    TestAstTokens::Object,

    TestAstTokens::Leq,
    TestAstTokens::Object,
    TestAstTokens::Object,

    TestAstTokens::Eq,
    TestAstTokens::Object,
    TestAstTokens::Object,

    TestAstTokens::Neg,
    TestAstTokens::Object
  };


  ASSERT_EQ(testAstTokens.size(), expectedAstTokens.size());
  ASSERT_THAT(testAstTokens, ::testing::ElementsAreArray(expectedAstTokens));
}


TEST(SimpleExpressions, ArithmeticsPrecedence) {
  std::stringstream stream;
  stream << "class A {\n"
         << "  main() : Object {\n"
         << "    {\n"
         << "      var1 + var2 + var3;    \n"
         << "      var1 - var2 * var3;    \n"
         << "      var1 * var2 - var3;    \n"
         << "      (var1 + var2) / var3;  \n"
         << "      var1 < var2 + var3;  \n"
         << "      var1 / var2 < var3; \n"
         << "      var1 / ~var2;  \n"
         << "    }\n"
         << "  };\n"
         << "};\n";
    
  mcool::tests::parser::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  mcool::tests::parser::TestVisitor visitor;
  ast.accept(&visitor);
  auto& testAstTokens = visitor.getTestAstTokens();

  using namespace mcool::tests::parser;
  std::vector<int> expectedAstTokens {
    TestAstTokens::CoolClass,
    TestAstTokens::Method,
    TestAstTokens::Object,
    TestAstTokens::Type,
    TestAstTokens::Block,

    TestAstTokens::Plus,
    TestAstTokens::Plus,
    TestAstTokens::Object,
    TestAstTokens::Object,
    TestAstTokens::Object,

    TestAstTokens::Minus,
    TestAstTokens::Object,
    TestAstTokens::Mult,
    TestAstTokens::Object,
    TestAstTokens::Object,

    TestAstTokens::Minus,
    TestAstTokens::Mult,
    TestAstTokens::Object,
    TestAstTokens::Object,
    TestAstTokens::Object,

    TestAstTokens::Div,
    TestAstTokens::Plus,
    TestAstTokens::Object,
    TestAstTokens::Object,
    TestAstTokens::Object,

    TestAstTokens::Less,
    TestAstTokens::Object,
    TestAstTokens::Plus,
    TestAstTokens::Object,
    TestAstTokens::Object,

    TestAstTokens::Less,
    TestAstTokens::Div,
    TestAstTokens::Object,
    TestAstTokens::Object,
    TestAstTokens::Object,

    TestAstTokens::Div,
    TestAstTokens::Object,
    TestAstTokens::Neg,
    TestAstTokens::Object
  };


  ASSERT_EQ(testAstTokens.size(), expectedAstTokens.size());
  ASSERT_THAT(testAstTokens, ::testing::ElementsAreArray(expectedAstTokens));
}


TEST(SimpleExpressions, IsVoidPrecendece) {
  std::stringstream stream;
  stream << "class A {\n"
         << "  main() : Object {\n"
         << "    {\n"
         << "      isvoid var2 * var3;    \n"
         << "    }\n"
         << "  };\n"
         << "};\n";
      
  mcool::tests::parser::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  mcool::tests::parser::TestVisitor visitor;
  ast.accept(&visitor);
  auto& testAstTokens = visitor.getTestAstTokens();

  using namespace mcool::tests::parser;
  std::vector<int> expectedAstTokens {
    TestAstTokens::CoolClass,
    TestAstTokens::Method,
    TestAstTokens::Object,
    TestAstTokens::Type,
    TestAstTokens::Block,

    TestAstTokens::Mult,
    TestAstTokens::Isvoid,
    TestAstTokens::Object,
    TestAstTokens::Object
  };

  ASSERT_EQ(testAstTokens.size(), expectedAstTokens.size());
  ASSERT_THAT(testAstTokens, ::testing::ElementsAreArray(expectedAstTokens));
}


TEST(SimpleExpressions, NotPrecendece) {
  std::stringstream stream;
  stream << "class A {\n"
         << "  main() : Object {\n"
         << "    {\n"
         << "      not isvoid var2 * var3;    \n"
         << "    }\n"
         << "  };\n"
         << "};\n";
      
  mcool::tests::parser::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  mcool::tests::parser::TestVisitor visitor;
  ast.accept(&visitor);
  auto& testAstTokens = visitor.getTestAstTokens();

  using namespace mcool::tests::parser;
  std::vector<int> expectedAstTokens {
    TestAstTokens::CoolClass,
    TestAstTokens::Method,
    TestAstTokens::Object,
    TestAstTokens::Type,
    TestAstTokens::Block,

    TestAstTokens::Not,
    TestAstTokens::Mult,
    TestAstTokens::Isvoid,
    TestAstTokens::Object,
    TestAstTokens::Object
  };

  ASSERT_EQ(testAstTokens.size(), expectedAstTokens.size());
  ASSERT_THAT(testAstTokens, ::testing::ElementsAreArray(expectedAstTokens));
}


TEST(SimpleExpressions, SelfMethodCall) {
  std::stringstream stream;
  stream << "class A {\n"
         << "  main() : Object {\n"
         << "    {\n"
         << "      foo(a * b, c + d);    \n"
         << "    }\n"
         << "  };\n"
         << "};\n";
      
  mcool::tests::parser::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  mcool::tests::parser::TestVisitor visitor;
  ast.accept(&visitor);
  auto& testAstTokens = visitor.getTestAstTokens();

  using namespace mcool::tests::parser;
    std::vector<int> expectedAstTokens {
    TestAstTokens::CoolClass,
    TestAstTokens::Method,
    TestAstTokens::Object,
    TestAstTokens::Type,
    TestAstTokens::Block,

    TestAstTokens::Dispatch,
    TestAstTokens::Object,
    TestAstTokens::Object,
    TestAstTokens::Mult,
    TestAstTokens::Object,
    TestAstTokens::Object,
    TestAstTokens::Plus,
    TestAstTokens::Object,
    TestAstTokens::Object
  };

  ASSERT_EQ(testAstTokens.size(), expectedAstTokens.size());
  ASSERT_THAT(testAstTokens, ::testing::ElementsAreArray(expectedAstTokens));
}


TEST(SimpleExpressions, ObjectMethodCall) {
  std::stringstream stream;
  stream << "class A {\n"
         << "  main() : Object {\n"
         << "    {\n"
         << "      obj.bar();    \n"
         << "    }\n"
         << "  };\n"
         << "};\n";
      
  mcool::tests::parser::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  mcool::tests::parser::TestVisitor visitor;
  ast.accept(&visitor);
  auto& testAstTokens = visitor.getTestAstTokens();
  using namespace mcool::tests::parser;
    std::vector<int> expectedAstTokens {
    TestAstTokens::CoolClass,
    TestAstTokens::Method,
    TestAstTokens::Object,
    TestAstTokens::Type,
    TestAstTokens::Block,

    TestAstTokens::Dispatch,
    TestAstTokens::Object,
    TestAstTokens::Object
  };

  ASSERT_EQ(testAstTokens.size(), expectedAstTokens.size());
  ASSERT_THAT(testAstTokens, ::testing::ElementsAreArray(expectedAstTokens));
}


TEST(SimpleExpressions, StaticDispatch) {
  std::stringstream stream;
  stream << "class A {\n"
         << "  main() : Object {\n"
         << "    {\n"
         << "      (obj.get())@A.bar();    \n"
         << "    }\n"
         << "  };\n"
         << "};\n";

  mcool::tests::parser::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  mcool::tests::parser::TestVisitor visitor;
  ast.accept(&visitor);
  auto& testAstTokens = visitor.getTestAstTokens();
  using namespace mcool::tests::parser;
    std::vector<int> expectedAstTokens {
    TestAstTokens::CoolClass,
    TestAstTokens::Method,
    TestAstTokens::Object,
    TestAstTokens::Type,
    TestAstTokens::Block,

    TestAstTokens::StaticDispatch,
    TestAstTokens::Dispatch,
    TestAstTokens::Object,
    TestAstTokens::Object,
    TestAstTokens::Type,
    TestAstTokens::Object
  };

  ASSERT_EQ(testAstTokens.size(), expectedAstTokens.size());
  ASSERT_THAT(testAstTokens, ::testing::ElementsAreArray(expectedAstTokens));
}


TEST(SimpleExpressions, DispatchInArrithmetics) {
  std::stringstream stream;
  stream << "class A {\n"
            "  main() : Object {\n"
            "    {\n"
            "      var1 * bar();     \n"
            "      obj.foo() < var1; \n"
            "      bar().foo();      \n"
            "    }\n"
            "  };\n"
            "};\n";

  mcool::tests::parser::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  mcool::tests::parser::TestVisitor visitor;
  ast.accept(&visitor);
  auto& testAstTokens = visitor.getTestAstTokens();
  using namespace mcool::tests::parser;
    std::vector<int> expectedAstTokens {
    TestAstTokens::CoolClass,
    TestAstTokens::Method,
    TestAstTokens::Object,
    TestAstTokens::Type,
    TestAstTokens::Block,

    TestAstTokens::Mult,
    TestAstTokens::Object,
    TestAstTokens::Dispatch,
    TestAstTokens::Object,
    TestAstTokens::Object,

    TestAstTokens::Less,
    TestAstTokens::Dispatch,
    TestAstTokens::Object,
    TestAstTokens::Object,
    TestAstTokens::Object,

    TestAstTokens::Dispatch,
    TestAstTokens::Dispatch,
    TestAstTokens::Object,
    TestAstTokens::Object,
    TestAstTokens::Object
  };

  ASSERT_EQ(testAstTokens.size(), expectedAstTokens.size());
  ASSERT_THAT(testAstTokens, ::testing::ElementsAreArray(expectedAstTokens));
}


TEST(SimpleExpressions, NewOperator) {
  std::stringstream stream;
  stream << "class A {\n"
            "  main() : Object {\n"
            "    {\n"
            "      new Int;            \n"
            "      new String;         \n"
            "      new SomeObjectType; \n"
            "    }\n"
            "  };\n"
            "};\n";

  mcool::tests::parser::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  mcool::tests::parser::TestVisitor visitor;
  ast.accept(&visitor);
  auto& testAstTokens = visitor.getTestAstTokens();
  using namespace mcool::tests::parser;
    std::vector<int> expectedAstTokens {
    TestAstTokens::CoolClass,
    TestAstTokens::Method,
    TestAstTokens::Object,
    TestAstTokens::Type,
    TestAstTokens::Block,

    TestAstTokens::New,
    TestAstTokens::Type,

    TestAstTokens::New,
    TestAstTokens::Type,

    TestAstTokens::New,
    TestAstTokens::Type
  };

  ASSERT_EQ(testAstTokens.size(), expectedAstTokens.size());
  ASSERT_THAT(testAstTokens, ::testing::ElementsAreArray(expectedAstTokens));
}


TEST(SimpleExpressions, Assignments) {
  std::stringstream stream;
  stream << "class A {\n"
         << "  main() : Object {\n"
         << "    {\n"
         << "      a <- 123;                \n"
         << "      b <- foo();              \n"
         << "      c <- job@SomeType.foo(); \n"
         << "      d <- a / b - c;          \n"
         //<< "      e <- new SomeType;       \n"
         << "    }\n"
         << "  };\n"
         << "};\n";

  mcool::tests::parser::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  mcool::tests::parser::TestVisitor visitor;
  ast.accept(&visitor);
  auto& testAstTokens = visitor.getTestAstTokens();
  
  using namespace mcool::tests::parser;
  std::vector<int> expectedAstTokens {
    TestAstTokens::CoolClass,
    TestAstTokens::Method,
    TestAstTokens::Object,
    TestAstTokens::Type,
    TestAstTokens::Block,

    TestAstTokens::Assign,
    TestAstTokens::Object,
    TestAstTokens::Int,

    TestAstTokens::Assign,
    TestAstTokens::Object,
    TestAstTokens::Dispatch,
    TestAstTokens::Object,
    TestAstTokens::Object,

    TestAstTokens::Assign,
    TestAstTokens::Object,
    TestAstTokens::StaticDispatch,
    TestAstTokens::Object,
    TestAstTokens::Type,
    TestAstTokens::Object,

    TestAstTokens::Assign,
    TestAstTokens::Object,
    TestAstTokens::Minus,
    TestAstTokens::Div,
    TestAstTokens::Object,
    TestAstTokens::Object,
    TestAstTokens::Object
  };

  ASSERT_EQ(testAstTokens.size(), expectedAstTokens.size());
  ASSERT_THAT(testAstTokens, ::testing::ElementsAreArray(expectedAstTokens));
}
