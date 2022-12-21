#pragma once

#if ! defined(yyFlexLexerOnce)
#undef yyFlexLexer
#define yyFlexLexer mcool_FlexLexer
#include <FlexLexer.h>
#endif


#undef YY_DECL
#define YY_DECL mcool::Parser::symbol_type mcool::Scanner::get_next_token()

#include "parser.h"
#include <istream>

namespace mcool {
class Scanner : public yyFlexLexer {
public:
  explicit Scanner(std::istream* stream) : yyFlexLexer(stream)  {}
	virtual ~Scanner() {}
	virtual mcool::Parser::symbol_type get_next_token();

  // https://panthema.net/2007/flex-bison-cpp-example/flex-bison-cpp-example-0.1/doxygen-html/classExampleFlexLexer.html
};
}
