#include "auxiliary.h"
#include "gtest/gtest.h"
#include "gmock/gmock-matchers.h"


TEST(StringLiterals, SimpleString) {
  std::stringstream stream;
  stream << " \" String\" \n \" String \" \v \"String \" ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokens = driver.getTokens();
  auto tokensKinds = driver.getTokensKinds(tokens);

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds {
      mcool::Parser::symbol_kind_type::S_STRING,
      mcool::Parser::symbol_kind_type::S_STRING,
      mcool::Parser::symbol_kind_type::S_STRING,
      mcool::Parser::symbol_kind_type::S_YYEOF
  };

  std::vector<std::string> expectedStrings{" String", " String ", "String "};
  for (size_t index{0}; index < expectedStrings.size(); ++index) {
    auto token = tokens[index];
    auto value = token.value.as<std::string>();
    ASSERT_STREQ(value.c_str(), expectedStrings[index].c_str());
  }
}


TEST(StringLiterals, EofInStringLiteral) {
  std::stringstream stream;
  stream << " \" not closed string   ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokens = driver.getTokens();
  auto tokensKinds = driver.getTokensKinds(tokens);

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds {
      mcool::Parser::symbol_kind_type::S_ERROR,
      mcool::Parser::symbol_kind_type::S_YYEOF
  };
}


TEST(StringLiterals, StringWithNullCharacter) {
  std::stringstream stream;
  stream << " \" hello \0 world \" ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokens = driver.getTokens();
  auto tokensKinds = driver.getTokensKinds(tokens);

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds {
      mcool::Parser::symbol_kind_type::S_ERROR,
      mcool::Parser::symbol_kind_type::S_YYEOF
  };
}


TEST(StringLiterals, MultiLineString) {
  std::stringstream stream;
  stream << "\"This \\ \t \n is OK\"";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokens = driver.getTokens();
  auto tokensKinds = driver.getTokensKinds(tokens);

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds {
      mcool::Parser::symbol_kind_type::S_STRING,
      mcool::Parser::symbol_kind_type::S_YYEOF
  };

  auto token = tokens[0];
  auto value = token.value.as<std::string>();
  ASSERT_STREQ(value.c_str(), "This  is OK");
}


TEST(StringLiterals, ComplexString1) {
  std::stringstream stream;
  stream << " \" \\h\\e\\l\\l\\o \\w\\or\\l\\d \" ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokens = driver.getTokens();
  auto tokensKinds = driver.getTokensKinds(tokens);

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds {
      mcool::Parser::symbol_kind_type::S_STRING,
      mcool::Parser::symbol_kind_type::S_YYEOF
  };

  auto token = tokens[0];
  auto value = token.value.as<std::string>();
  ASSERT_STREQ(value.c_str(), " hello world ");
}

TEST(StringLiterals, ComplexString2) {
  std::stringstream stream;
  stream << " \" \\c \\\n \\s \" ";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokens = driver.getTokens();
  auto tokensKinds = driver.getTokensKinds(tokens);

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds {
      mcool::Parser::symbol_kind_type::S_STRING,
      mcool::Parser::symbol_kind_type::S_YYEOF
  };

  auto token = tokens[0];
  auto value = token.value.as<std::string>();
  ASSERT_STREQ(value.c_str(), " c \n s ");
}


TEST(StringLiterals, SmallString) {
  std::stringstream stream;
  stream << "\"x\"";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokens = driver.getTokens();
  auto tokensKinds = driver.getTokensKinds(tokens);

  mcool::tests::scanner::TestDriver::TokenKindStream expectedKinds {
      mcool::Parser::symbol_kind_type::S_STRING,
      mcool::Parser::symbol_kind_type::S_YYEOF
  };

  auto token = tokens[0];
  auto value = token.value.as<std::string>();
  ASSERT_STREQ(value.c_str(), "x");
}


TEST(StringLiterals, StringWithSpcialChars) {
  auto get = [](std::istream& stream) {
    mcool::tests::scanner::TestDriver driver(&stream);
    return driver.getTokens();

  };

  {
    std::stringstream stream(" \" hello\\bworld \" ");
    auto tokens = get(stream);
    auto value = tokens[0].value.as<std::string>();
    ASSERT_STREQ(value.c_str(), " hello\bworld ");
  }
  {
    std::stringstream stream(" \" hello \\t world \" ");
    auto tokens = get(stream);
    auto value = tokens[0].value.as<std::string>();
    ASSERT_STREQ(value.c_str(), " hello \t world ");
  }
  {
    std::stringstream stream(" \"hello \\n \"");
    auto tokens = get(stream);
    auto value = tokens[0].value.as<std::string>();
    ASSERT_STREQ(value.c_str(), "hello \n ");
  }
  {
    std::stringstream stream(" \"\\fhello\"");
    auto tokens = get(stream);
    auto value = tokens[0].value.as<std::string>();
    ASSERT_STREQ(value.c_str(), "\fhello");
  }
}