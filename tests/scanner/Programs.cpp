#include "auxiliary.h"
#include "gtest/gtest.h"
#include "gmock/gmock-matchers.h"


TEST(Programs, SimpleProgram) {
  std::stringstream stream;
  stream << "class Main {\n"
         << "    cells : CellularAutomaton;\n"
         << "\n"
         << "    main() : SELF_TYPE { \n"
         << "        { \n"
         << "            cells <- (new CellularAutomaton).init(\" X \");\n"
         << "            cells.print();\n"
         << "            (let countdown : Int <- 20 in\n"
         << "                while 0 < countdown loop\n"
         << "                    {\n"
         << "                        cells.evolve();\n"
         << "                        cells.print();\n"
         << "                        countdown <- countdown - 1;\n"
         << "                    }\n"
         << "                pool\n"
         << "            );\n"
         << "            self;\n"
         << "        }\n"
         << "    };\n"
         << "};\n";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expectedTokensKinds = {
      mcool::Parser::symbol_kind_type::S_CLASS,
      mcool::Parser::symbol_kind_type::S_TYPEID,
      mcool::Parser::symbol_kind_type::S_CURLY_LEFTPAR,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_COLON,
      mcool::Parser::symbol_kind_type::S_TYPEID,
      mcool::Parser::symbol_kind_type::S_SEMICOLON,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_LEFTPAR,
      mcool::Parser::symbol_kind_type::S_RIGHTPAR,
      mcool::Parser::symbol_kind_type::S_COLON,
      mcool::Parser::symbol_kind_type::S_TYPEID,
      mcool::Parser::symbol_kind_type::S_CURLY_LEFTPAR,
      mcool::Parser::symbol_kind_type::S_CURLY_LEFTPAR,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_ASSIGN,
      mcool::Parser::symbol_kind_type::S_LEFTPAR,
      mcool::Parser::symbol_kind_type::S_NEW,
      mcool::Parser::symbol_kind_type::S_TYPEID,
      mcool::Parser::symbol_kind_type::S_RIGHTPAR,
      mcool::Parser::symbol_kind_type::S_DOT,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_LEFTPAR,
      mcool::Parser::symbol_kind_type::S_STRING,
      mcool::Parser::symbol_kind_type::S_RIGHTPAR,
      mcool::Parser::symbol_kind_type::S_SEMICOLON,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_DOT,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_LEFTPAR,
      mcool::Parser::symbol_kind_type::S_RIGHTPAR,
      mcool::Parser::symbol_kind_type::S_SEMICOLON,
      mcool::Parser::symbol_kind_type::S_LEFTPAR,
      mcool::Parser::symbol_kind_type::S_LET,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_COLON,
      mcool::Parser::symbol_kind_type::S_TYPEID,
      mcool::Parser::symbol_kind_type::S_ASSIGN,
      mcool::Parser::symbol_kind_type::S_NUMBER,
      mcool::Parser::symbol_kind_type::S_IN,
      mcool::Parser::symbol_kind_type::S_WHILE,
      mcool::Parser::symbol_kind_type::S_NUMBER,
      mcool::Parser::symbol_kind_type::S_LESS,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_LOOP,
      mcool::Parser::symbol_kind_type::S_CURLY_LEFTPAR,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_DOT,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_LEFTPAR,
      mcool::Parser::symbol_kind_type::S_RIGHTPAR,
      mcool::Parser::symbol_kind_type::S_SEMICOLON,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_DOT,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_LEFTPAR,
      mcool::Parser::symbol_kind_type::S_RIGHTPAR,
      mcool::Parser::symbol_kind_type::S_SEMICOLON,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_ASSIGN,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_MINUS,
      mcool::Parser::symbol_kind_type::S_NUMBER,
      mcool::Parser::symbol_kind_type::S_SEMICOLON,
      mcool::Parser::symbol_kind_type::S_CURLY_RIGHTPAR,
      mcool::Parser::symbol_kind_type::S_POOL,
      mcool::Parser::symbol_kind_type::S_RIGHTPAR,
      mcool::Parser::symbol_kind_type::S_SEMICOLON,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_SEMICOLON,
      mcool::Parser::symbol_kind_type::S_CURLY_RIGHTPAR,
      mcool::Parser::symbol_kind_type::S_CURLY_RIGHTPAR,
      mcool::Parser::symbol_kind_type::S_SEMICOLON,
      mcool::Parser::symbol_kind_type::S_CURLY_RIGHTPAR,
      mcool::Parser::symbol_kind_type::S_SEMICOLON,
      mcool::Parser::symbol_kind_type::S_YYEOF
  };

  for (size_t index{0}; index < expectedTokensKinds.size(); ++index) {
    auto kind = tokensKinds[index];
    ASSERT_EQ(kind, expectedTokensKinds[index]);
  }
}

TEST(Programs, Conditions) {
  std::stringstream stream;
  stream << "apply(istack: Stack) : Stack {\n"
         << "    {\n"
         << "      stack <- istack;\n"
         << "      stack.pop();\n"
         << "      let size: Int <- stack.get_length() in {\n"
         << "        if size < 2 then\n"
         << "          {\n"
         << "            stack;\n"
         << "          }\n"
         << "        else \n"
         << "          let one: String <- stack.pop(), two: String <- stack.pop() in {\n"
         << "            exec(one, two);\n"
         << "          }\n"
         << "        fi;\n"
         << "      };\n"
         << "    }\n"
         << "  };\n";

  mcool::tests::scanner::TestDriver driver(&stream);
  auto tokensKinds = driver.getTokensKinds();

  mcool::tests::scanner::TestDriver::TokenKindStream expectedTokensKinds = {
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_LEFTPAR,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_COLON,
      mcool::Parser::symbol_kind_type::S_TYPEID,
      mcool::Parser::symbol_kind_type::S_RIGHTPAR,
      mcool::Parser::symbol_kind_type::S_COLON,
      mcool::Parser::symbol_kind_type::S_TYPEID,
      mcool::Parser::symbol_kind_type::S_CURLY_LEFTPAR,
      mcool::Parser::symbol_kind_type::S_CURLY_LEFTPAR,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_ASSIGN,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_SEMICOLON,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_DOT,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_LEFTPAR,
      mcool::Parser::symbol_kind_type::S_RIGHTPAR,
      mcool::Parser::symbol_kind_type::S_SEMICOLON,
      mcool::Parser::symbol_kind_type::S_LET,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_COLON,
      mcool::Parser::symbol_kind_type::S_TYPEID,
      mcool::Parser::symbol_kind_type::S_ASSIGN,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_DOT,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_LEFTPAR,
      mcool::Parser::symbol_kind_type::S_RIGHTPAR,
      mcool::Parser::symbol_kind_type::S_IN,
      mcool::Parser::symbol_kind_type::S_CURLY_LEFTPAR,
      mcool::Parser::symbol_kind_type::S_IF,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_LESS,
      mcool::Parser::symbol_kind_type::S_NUMBER,
      mcool::Parser::symbol_kind_type::S_THEN,
      mcool::Parser::symbol_kind_type::S_CURLY_LEFTPAR,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_SEMICOLON,
      mcool::Parser::symbol_kind_type::S_CURLY_RIGHTPAR,
      mcool::Parser::symbol_kind_type::S_ELSE,
      mcool::Parser::symbol_kind_type::S_LET,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_COLON,
      mcool::Parser::symbol_kind_type::S_TYPEID,
      mcool::Parser::symbol_kind_type::S_ASSIGN,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_DOT,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_LEFTPAR,
      mcool::Parser::symbol_kind_type::S_RIGHTPAR,
      mcool::Parser::symbol_kind_type::S_COMMA,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_COLON,
      mcool::Parser::symbol_kind_type::S_TYPEID,
      mcool::Parser::symbol_kind_type::S_ASSIGN,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_DOT,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_LEFTPAR,
      mcool::Parser::symbol_kind_type::S_RIGHTPAR,
      mcool::Parser::symbol_kind_type::S_IN,
      mcool::Parser::symbol_kind_type::S_CURLY_LEFTPAR,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_LEFTPAR,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_COMMA,
      mcool::Parser::symbol_kind_type::S_OBJECTID,
      mcool::Parser::symbol_kind_type::S_RIGHTPAR,
      mcool::Parser::symbol_kind_type::S_SEMICOLON,
      mcool::Parser::symbol_kind_type::S_CURLY_RIGHTPAR,
      mcool::Parser::symbol_kind_type::S_FI,
      mcool::Parser::symbol_kind_type::S_SEMICOLON,
      mcool::Parser::symbol_kind_type::S_CURLY_RIGHTPAR,
      mcool::Parser::symbol_kind_type::S_SEMICOLON,
      mcool::Parser::symbol_kind_type::S_CURLY_RIGHTPAR,
      mcool::Parser::symbol_kind_type::S_CURLY_RIGHTPAR,
      mcool::Parser::symbol_kind_type::S_SEMICOLON,
      mcool::Parser::symbol_kind_type::S_YYEOF
  };

  for (size_t index{0}; index < expectedTokensKinds.size(); ++index) {
    auto kind = tokensKinds[index];
    ASSERT_EQ(kind, expectedTokensKinds[index]);
  }
}