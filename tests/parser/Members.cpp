#include "auxiliary.h"
#include "TestParserVisitor.h"
#include <iostream>


TEST(Members, ClassWithSingleAttribute) {
  std::stringstream stream;
  stream << "class A {\n some : Int;\n };";

  mcool::tests::AttrExtractor extractor(stream);
  mcool::ast::SingleMember* member{nullptr};
  extractor.getAttr(member, 0, 0);

  ASSERT_STREQ(member->getId()->getNameAsStr().c_str(), "some");
  ASSERT_STREQ(member->getIdType()->getNameAsStr().c_str(), "Int");
}

TEST(Members, ClassWithTwoAttributes) {
  std::stringstream stream;
  stream << "class A {\n one: Int;\n two: String; };";

  mcool::tests::AttrExtractor extractor(stream);
  mcool::ast::SingleMember* member{nullptr};
  extractor.getAttr(member, 0, 0);

  ASSERT_STREQ(member->getId()->getNameAsStr().c_str(), "one");
  ASSERT_STREQ(member->getIdType()->getNameAsStr().c_str(), "Int");

  extractor.getAttr(member, 0, 1);
  ASSERT_STREQ(member->getId()->getNameAsStr().c_str(), "two");
  ASSERT_STREQ(member->getIdType()->getNameAsStr().c_str(), "String");
}