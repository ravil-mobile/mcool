#include "auxiliary.h"
#include "gtest/gtest.h"
#include "gmock/gmock-matchers.h"


TEST(Types, ObjectType) {
  std::stringstream stream;
  stream << " Object ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokens = driver.getTokens();
  auto kinds = driver.getTokensKinds(tokens);

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds {
      mcool::Parser::symbol_kind_type::S_TYPEID,
      mcool::Parser::symbol_kind_type::S_YYEOF
  };

  ASSERT_THAT(kinds, ::testing::ElementsAreArray(expectedKinds));

  auto token = tokens.at(0);
  auto value =  token.value.as<std::string>();
  ASSERT_STREQ(value.c_str(), "Object");
}


TEST(Types, IntType) {
  std::stringstream stream;
  stream << "Int";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokens = driver.getTokens();
  auto kinds = driver.getTokensKinds(tokens);

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds {
      mcool::Parser::symbol_kind_type::S_TYPEID,
      mcool::Parser::symbol_kind_type::S_YYEOF
  };

  ASSERT_THAT(kinds, ::testing::ElementsAreArray(expectedKinds));

  auto token = tokens.at(0);
  auto value =  token.value.as<std::string>();
  ASSERT_STREQ(value.c_str(), "Int");
}


TEST(Types, BoolType) {
  std::stringstream stream;
  stream << " Bool";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokens = driver.getTokens();
  auto kinds = driver.getTokensKinds(tokens);

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds {
      mcool::Parser::symbol_kind_type::S_TYPEID,
      mcool::Parser::symbol_kind_type::S_YYEOF
  };

  ASSERT_THAT(kinds, ::testing::ElementsAreArray(expectedKinds));

  auto token = tokens.at(0);
  auto value =  token.value.as<std::string>();
  ASSERT_STREQ(value.c_str(), "Bool");
}


TEST(Types, StringType) {
  std::stringstream stream;
  stream << "String  ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokens = driver.getTokens();
  auto kinds = driver.getTokensKinds(tokens);

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds {
      mcool::Parser::symbol_kind_type::S_TYPEID,
      mcool::Parser::symbol_kind_type::S_YYEOF
  };

  ASSERT_THAT(kinds, ::testing::ElementsAreArray(expectedKinds));

  auto token = tokens.at(0);
  auto value =  token.value.as<std::string>();
  ASSERT_STREQ(value.c_str(), "String");
}


TEST(Types, SelfType) {
  std::stringstream stream;
  stream << "SELF_TYPE";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokens = driver.getTokens();
  auto kinds = driver.getTokensKinds(tokens);

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds {
      mcool::Parser::symbol_kind_type::S_TYPEID,
      mcool::Parser::symbol_kind_type::S_YYEOF
  };

  ASSERT_THAT(kinds, ::testing::ElementsAreArray(expectedKinds));

  auto token = tokens.at(0);
  auto value =  token.value.as<std::string>();
  ASSERT_STREQ(value.c_str(), "SELF_TYPE");
}


TEST(Types, DerivedType) {
  std::stringstream stream;
  stream << "DerivedType";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokens = driver.getTokens();
  auto kinds = driver.getTokensKinds(tokens);

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds {
      mcool::Parser::symbol_kind_type::S_TYPEID,
      mcool::Parser::symbol_kind_type::S_YYEOF
  };

  ASSERT_THAT(kinds, ::testing::ElementsAreArray(expectedKinds));

  auto token = tokens.at(0);
  auto value =  token.value.as<std::string>();
  ASSERT_STREQ(value.c_str(), "DerivedType");
}
