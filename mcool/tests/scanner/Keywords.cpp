#include "auxiliary.h"
#include "gtest/gtest.h"
#include "gmock/gmock-matchers.h"

TEST(Keywords, Class) {
  std::stringstream stream;
  stream << " class ClAsS ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds{
      mcool::Parser::symbol_kind_type::S_CLASS,
      mcool::Parser::symbol_kind_type::S_CLASS,
      mcool::Parser::symbol_kind_type::S_YYEOF};

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expectedKinds));
}

TEST(Keywords, If) {
  std::stringstream stream;
  stream << " if IF ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds{
      mcool::Parser::symbol_kind_type::S_IF,
      mcool::Parser::symbol_kind_type::S_IF,
      mcool::Parser::symbol_kind_type::S_YYEOF};

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expectedKinds));
}

TEST(Keywords, Then) {
  std::stringstream stream;
  stream << " ThEn then ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds{
      mcool::Parser::symbol_kind_type::S_THEN,
      mcool::Parser::symbol_kind_type::S_THEN,
      mcool::Parser::symbol_kind_type::S_YYEOF};

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expectedKinds));
}

TEST(Keywords, Else) {
  std::stringstream stream;
  stream << " eLsE else";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds{
      mcool::Parser::symbol_kind_type::S_ELSE,
      mcool::Parser::symbol_kind_type::S_ELSE,
      mcool::Parser::symbol_kind_type::S_YYEOF};

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expectedKinds));
}

TEST(Keywords, Fi) {
  std::stringstream stream;
  stream << "fi fI ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds{
      mcool::Parser::symbol_kind_type::S_FI,
      mcool::Parser::symbol_kind_type::S_FI,
      mcool::Parser::symbol_kind_type::S_YYEOF};

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expectedKinds));
}

TEST(Keywords, In) {
  std::stringstream stream;
  stream << "in IN";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds{
      mcool::Parser::symbol_kind_type::S_IN,
      mcool::Parser::symbol_kind_type::S_IN,
      mcool::Parser::symbol_kind_type::S_YYEOF};

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expectedKinds));
}

TEST(Keywords, Inherits) {
  std::stringstream stream;
  stream << "inherits   iNhEriTs";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds{
      mcool::Parser::symbol_kind_type::S_INHERITS,
      mcool::Parser::symbol_kind_type::S_INHERITS,
      mcool::Parser::symbol_kind_type::S_YYEOF};

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expectedKinds));
}

TEST(Keywords, Let) {
  std::stringstream stream;
  stream << "lEt   LET ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds{
      mcool::Parser::symbol_kind_type::S_LET,
      mcool::Parser::symbol_kind_type::S_LET,
      mcool::Parser::symbol_kind_type::S_YYEOF};

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expectedKinds));
}

TEST(Keywords, Loop) {
  std::stringstream stream;
  stream << "loop   lOOp ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds{
      mcool::Parser::symbol_kind_type::S_LOOP,
      mcool::Parser::symbol_kind_type::S_LOOP,
      mcool::Parser::symbol_kind_type::S_YYEOF};

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expectedKinds));
}

TEST(Keywords, Pool) {
  std::stringstream stream;
  stream << "PooL   pool ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds{
      mcool::Parser::symbol_kind_type::S_POOL,
      mcool::Parser::symbol_kind_type::S_POOL,
      mcool::Parser::symbol_kind_type::S_YYEOF};

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expectedKinds));
}

TEST(Keywords, While) {
  std::stringstream stream;
  stream << "while   WHIlE ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds{
      mcool::Parser::symbol_kind_type::S_WHILE,
      mcool::Parser::symbol_kind_type::S_WHILE,
      mcool::Parser::symbol_kind_type::S_YYEOF};

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expectedKinds));
}

TEST(Keywords, Case) {
  std::stringstream stream;
  stream << "case   cAsE ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds{
      mcool::Parser::symbol_kind_type::S_CASE,
      mcool::Parser::symbol_kind_type::S_CASE,
      mcool::Parser::symbol_kind_type::S_YYEOF};

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expectedKinds));
}

TEST(Keywords, Esac) {
  std::stringstream stream;
  stream << "esac   EsAc ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds{
      mcool::Parser::symbol_kind_type::S_ESAC,
      mcool::Parser::symbol_kind_type::S_ESAC,
      mcool::Parser::symbol_kind_type::S_YYEOF};

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expectedKinds));
}

TEST(Keywords, Of) {
  std::stringstream stream;
  stream << " of   oF ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds{
      mcool::Parser::symbol_kind_type::S_OF,
      mcool::Parser::symbol_kind_type::S_OF,
      mcool::Parser::symbol_kind_type::S_YYEOF};

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expectedKinds));
}

TEST(Keywords, New) {
  std::stringstream stream;
  stream << "new   nEw ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds{
      mcool::Parser::symbol_kind_type::S_NEW,
      mcool::Parser::symbol_kind_type::S_NEW,
      mcool::Parser::symbol_kind_type::S_YYEOF};

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expectedKinds));
}

TEST(Keywords, Isvoid) {
  std::stringstream stream;
  stream << "isvoid   Isvoid ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds{
      mcool::Parser::symbol_kind_type::S_ISVOID,
      mcool::Parser::symbol_kind_type::S_ISVOID,
      mcool::Parser::symbol_kind_type::S_YYEOF};

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expectedKinds));
}

TEST(Keywords, Not) {
  std::stringstream stream;
  stream << " not   noT";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds{
      mcool::Parser::symbol_kind_type::S_NOT,
      mcool::Parser::symbol_kind_type::S_NOT,
      mcool::Parser::symbol_kind_type::S_YYEOF};

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expectedKinds));
}

TEST(Keywords, Darrow) {
  std::stringstream stream;
  stream << " => ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds{
      mcool::Parser::symbol_kind_type::S_DARROW, mcool::Parser::symbol_kind_type::S_YYEOF};

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expectedKinds));
}

TEST(Keywords, Assign) {
  std::stringstream stream;
  stream << "<- ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds{
      mcool::Parser::symbol_kind_type::S_ASSIGN, mcool::Parser::symbol_kind_type::S_YYEOF};

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expectedKinds));
}

TEST(Keywords, Less) {
  std::stringstream stream;
  stream << " < ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds{
      mcool::Parser::symbol_kind_type::S_LESS, mcool::Parser::symbol_kind_type::S_YYEOF};

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expectedKinds));
}

TEST(Keywords, LessOrEqual) {
  std::stringstream stream;
  stream << " <=";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds{
      mcool::Parser::symbol_kind_type::S_LEQ, mcool::Parser::symbol_kind_type::S_YYEOF};

  ASSERT_THAT(tokensKinds, ::testing::ElementsAreArray(expectedKinds));
}
