#pragma once

#include "Parser/Scanner.h"
#include "Parser.h"
#include <vector>
#include <string>
#include <iostream>
#include <tuple>

namespace mcool::tests::scanner {
class TestDriver {
  public:
  explicit TestDriver(std::istream* stream) { scanner.set(stream, nullptr); }

  using TokenStream = std::vector<mcool::Parser::symbol_type>;
  using TokenKindStream = std::vector<mcool::Parser::symbol_kind_type>;

  TokenStream getTokens() {
    TokenStream tokens{};
    auto currTokenKind = mcool::Parser::symbol_kind_type::S_YYEMPTY;

    while (currTokenKind != mcool::Parser::symbol_kind_type::S_YYEOF) {
      auto token = scanner.get_next_token();

      token.kind();

      currTokenKind = token.kind();
      tokens.push_back(token);
    }

    return tokens;
  }

  TokenKindStream getTokensKinds() {
    auto tokens = this->getTokens();

    TokenKindStream tokensKindsStream{};
    tokensKindsStream.reserve(tokens.size());

    for (const auto& token : tokens) {
      tokensKindsStream.push_back(token.kind());
    }
    return tokensKindsStream;
  }

  TokenKindStream getTokensKinds(const TokenStream& tokens) {
    TokenKindStream tokensKindsStream{};
    tokensKindsStream.reserve(tokens.size());

    for (const auto& token : tokens) {
      tokensKindsStream.push_back(token.kind());
    }
    return tokensKindsStream;
  }

  static void printTokens(const TokenStream& tokenStream, std::ostream& os = std::cout) {
    for (const auto& token : tokenStream) {
      os << mcool::Parser::symbol_name(token.kind()) << '\n';
    }
  }

  static void printTokens(const TokenKindStream& tokensKindsStream, std::ostream& os = std::cout) {
    for (const auto& tokenKind : tokensKindsStream) {
      os << mcool::Parser::symbol_name(tokenKind) << '\n';
    }
  }

  private:
  mcool::Scanner scanner{false};
};
} // namespace mcool::tests::scanner