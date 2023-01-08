#include "auxiliary.h"
#include "gtest/gtest.h"
#include "gmock/gmock-matchers.h"


TEST(Comments, LeadingSingleLineComment) {
  std::stringstream stream;
  stream << " -- it is a comment \n Int i <- 3; ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expected {
      mcool::Parser::symbol_kind_type::S_TYPEID,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_ASSIGN,
      mcool::Parser::symbol_kind_type::S_NUMBER,
      mcool::Parser::symbol_kind_type::S_SEMICOLON,
      mcool::Parser::symbol_kind_type::S_YYEOF
  };

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expected));
}


TEST(Comments, TrailingSingleLineComment) {
  std::stringstream stream;
  stream << "Int i <- 3; \n -- it is a comment";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expected {
      mcool::Parser::symbol_kind_type::S_TYPEID,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_ASSIGN,
      mcool::Parser::symbol_kind_type::S_NUMBER,
      mcool::Parser::symbol_kind_type::S_SEMICOLON,
      mcool::Parser::symbol_kind_type::S_YYEOF
  };

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expected));
}


TEST(Comments, SingleLineCommentInBetween) {
  std::stringstream stream;
  stream << "Int i <- 3; \n -- it is a comment \n false";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expected {
      mcool::Parser::symbol_kind_type::S_TYPEID,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_ASSIGN,
      mcool::Parser::symbol_kind_type::S_NUMBER,
      mcool::Parser::symbol_kind_type::S_SEMICOLON,
      mcool::Parser::symbol_kind_type::S_BOOLEAN,
      mcool::Parser::symbol_kind_type::S_YYEOF
  };

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expected));
}


TEST(Comments, CommentInBetweenLine) {
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


TEST(Comments, MissingBeginingOfComment) {
  std::stringstream stream;
  stream << " *) ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expected {
      mcool::Parser::symbol_kind_type::S_ERROR,
      mcool::Parser::symbol_kind_type::S_YYEOF
  };

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expected));
}


TEST(Comments, ForgottenClosingComment) {
  std::stringstream stream;
  stream << " (* (* ) *) ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expected {
      mcool::Parser::symbol_kind_type::S_ERROR,
      mcool::Parser::symbol_kind_type::S_YYEOF
  };

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expected));
}


TEST(Comments, NestedComments) {
  std::stringstream stream;
  stream << " (* bla (* BLA bLa  *)  123*) Int ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expected {
      mcool::Parser::symbol_kind_type::S_TYPEID,
      mcool::Parser::symbol_kind_type::S_YYEOF
  };

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expected));
}


TEST(Comments, CommentWithoutWhiteSpaces) {
  std::stringstream stream;
  stream << " (*(*(**)(**)*)*) Class ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expected {
      mcool::Parser::symbol_kind_type::S_CLASS,
      mcool::Parser::symbol_kind_type::S_YYEOF
  };

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expected));
}


TEST(Comments, CommentWithStartsInBetween) {
  std::stringstream stream;
  stream << " (* * ** * *) Class ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expected {
      mcool::Parser::symbol_kind_type::S_CLASS,
      mcool::Parser::symbol_kind_type::S_YYEOF
  };

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expected));
}
