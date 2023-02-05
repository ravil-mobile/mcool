#include "auxiliary.h"
#include "gtest/gtest.h"
#include "gmock/gmock-matchers.h"

TEST(NumericalLiterals, PositiveIntLiterals) {
  std::stringstream stream;
  stream << " 2 ; +1245 ; 0000; 0001 ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokens = driver.getTokens();
  auto kinds = driver.getTokensKinds(tokens);

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds{
      mcool::Parser::symbol_kind_type::S_NUMBER,
      mcool::Parser::symbol_kind_type::S_SEMICOLON,
      mcool::Parser::symbol_kind_type::S_NUMBER,
      mcool::Parser::symbol_kind_type::S_SEMICOLON,
      mcool::Parser::symbol_kind_type::S_NUMBER,
      mcool::Parser::symbol_kind_type::S_SEMICOLON,
      mcool::Parser::symbol_kind_type::S_NUMBER,
      mcool::Parser::symbol_kind_type::S_YYEOF};

  ASSERT_THAT(kinds, ::testing::ElementsAreArray(expectedKinds));

  std::vector<std::pair<size_t, int>> expectedValues{{0, 2}, {2, 1245}, {4, 0}, {6, 1}};

  for (const auto& expectedPair : expectedValues) {
    auto index = expectedPair.first;
    auto expectedValue = expectedPair.second;

    auto token = tokens.at(index);
    ASSERT_EQ(token.value.as<int>(), expectedValue);
  }
}

TEST(NumericalLiterals, NegativeIntLiterals) {
  std::stringstream stream;
  stream << " -2 ; -1245 ; -0000; -0001 ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokens = driver.getTokens();
  auto kinds = driver.getTokensKinds(tokens);

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds{
      mcool::Parser::symbol_kind_type::S_NUMBER,
      mcool::Parser::symbol_kind_type::S_SEMICOLON,
      mcool::Parser::symbol_kind_type::S_NUMBER,
      mcool::Parser::symbol_kind_type::S_SEMICOLON,
      mcool::Parser::symbol_kind_type::S_NUMBER,
      mcool::Parser::symbol_kind_type::S_SEMICOLON,
      mcool::Parser::symbol_kind_type::S_NUMBER,
      mcool::Parser::symbol_kind_type::S_YYEOF};

  ASSERT_THAT(kinds, ::testing::ElementsAreArray(expectedKinds));

  std::vector<std::pair<size_t, int>> expectedValues{{0, -2}, {2, -1245}, {4, 0}, {6, -1}};

  for (const auto& expectedPair : expectedValues) {
    auto index = expectedPair.first;
    auto expectedValue = expectedPair.second;

    auto token = tokens.at(index);
    ASSERT_EQ(token.value.as<int>(), expectedValue);
  }
}

TEST(NumericalLiterals, BooleanLiterals) {
  std::stringstream stream;
  stream << " false fALSe true tRuE ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokens = driver.getTokens();
  auto kinds = driver.getTokensKinds(tokens);

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds{
      mcool::Parser::symbol_kind_type::S_BOOLEAN,
      mcool::Parser::symbol_kind_type::S_BOOLEAN,
      mcool::Parser::symbol_kind_type::S_BOOLEAN,
      mcool::Parser::symbol_kind_type::S_BOOLEAN,
      mcool::Parser::symbol_kind_type::S_YYEOF};

  std::vector<bool> expectedValues{false, false, true, true};
  ASSERT_THAT(kinds, ::testing::ElementsAreArray(expectedKinds));

  for (size_t index{0}; index < expectedValues.size(); ++index) {
    auto token = tokens.at(index);
    ASSERT_EQ(token.value.as<bool>(), expectedValues[index]);
  }
}
