#include "auxiliary.h"
#include "gtest/gtest.h"
#include "gmock/gmock-matchers.h"


TEST(ObjectIdentifiers, GoodStringIdentifiers) {
  std::stringstream stream;
  stream << " var \t  hElO_wOrLd \v a1234 \n aA1_23as_F4  self ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokens = driver.getTokens();
  auto tokensKinds = driver.getTokensKinds(tokens);

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds {
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_YYEOF
  };

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expectedKinds));

  std::vector<std::string> expectedObjectId {
    "var",
    "hElO_wOrLd",
    "a1234",
    "aA1_23as_F4",
    "self"
  };

  for (size_t index{0}; index < expectedObjectId.size(); ++index) {
    auto token = tokens[index];
    auto value = token.value.as<std::string>();
    ASSERT_STREQ(value.c_str(), expectedObjectId[index].c_str());
  }
}


TEST(ObjectIdentifiers, BadStringIdentifiers) {
  std::stringstream stream;
  stream << " _var \v  Var";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokens = driver.getTokens();
  auto tokensKinds = driver.getTokensKinds(tokens);

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds {
      mcool::Parser::symbol_kind_type::S_ERROR,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_TYPEID,
      mcool::Parser::symbol_kind_type::S_YYEOF
  };

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expectedKinds));

  std::vector<std::pair<size_t, std::string>> expectedPairs {
      {1, "var"},
      {2, "Var"}
  };

  for (const auto& expectedPair : expectedPairs) {
    size_t index = expectedPair.first;
    auto& expectedValue = expectedPair.second;

    auto token = tokens[index];
    auto value = token.value.as<std::string>();

    ASSERT_STREQ(value.c_str(), expectedValue.c_str());
  }
}
