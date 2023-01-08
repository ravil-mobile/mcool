#include "auxiliary.h"
#include "gtest/gtest.h"
#include "gmock/gmock-matchers.h"
#include <string>
#include <vector>
#include <iostream>


TEST(Comments, SingleLineComment) {
  std::stringstream stream;
  stream << " false true (* false 125 *) 654 ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expected {
    mcool::Parser::symbol_kind_type::S_BOOLEAN,
    mcool::Parser::symbol_kind_type::S_BOOLEAN,
    mcool::Parser::symbol_kind_type::S_NUMBER,
    mcool::Parser::symbol_kind_type::S_YYEOF
  };

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expected));
}