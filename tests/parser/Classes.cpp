#include "auxiliary.h"
#include "TestParserVisitor.h"
#include <iostream>

TEST(Classes, EmptyClasses) {
  std::stringstream stream;
  stream << "class A {\n}; \n class B inherits A {\n};";

  mcool::tests::parser::TestDriver driver(stream);
  auto [status, ast] = driver.run();

  ASSERT_THAT(status, true);
  ASSERT_THAT(ast.isAstOk(), true);

  auto* CoolClasses = ast.get();
  ASSERT_TRUE(CoolClasses != nullptr);

  auto& classes = CoolClasses->getData();
  ASSERT_EQ(classes.size(), 2);

  auto classIt = classes.begin();
  ASSERT_STREQ((*classIt)->getCoolType()->getNameAsStr().c_str(), "A");
  ASSERT_STREQ((*classIt)->getParentType()->getNameAsStr().c_str(), "Object");
  ASSERT_STREQ((*classIt)->getFileName()->getValueAsStr().c_str(), "test-stream");
  ASSERT_TRUE((*classIt)->getAttributes()->getData().empty());

  std::advance(classIt, 1);
  ASSERT_STREQ((*classIt)->getCoolType()->getNameAsStr().c_str(), "B");
  ASSERT_STREQ((*classIt)->getParentType()->getNameAsStr().c_str(), "A");
  ASSERT_STREQ((*classIt)->getFileName()->getValueAsStr().c_str(), "test-stream");
  ASSERT_TRUE((*classIt)->getAttributes()->getData().empty());
}