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
  explicit Scanner(bool verbose = true) : verbose(verbose) {}
	~Scanner() override = default;
	virtual mcool::Parser::symbol_type get_next_token();

  void set(std::istream* stream, std::string* intputFilename) {
    filename = intputFilename;
    currLocation = mcool::location(intputFilename);
    yyrestart(stream);
  }

  mcool::Parser::symbol_type reportError(const std::string& msg) {
    if (verbose) {
      std::cerr << "scanner error: " << msg << " : " << currLocation << std::endl;
    }
    return mcool::Parser::make_ERROR(currLocation);
  }

std::string getFileName() {
  if (filename) {
    return *filename;
  }
  else {
    return std::string("no filename given");
  }
}


private:
  bool verbose{true};

  int commentCounter{};
  std::string currString{};

  std::string* filename{nullptr};
  mcool::location currLocation{};
  // https://panthema.net/2007/flex-bison-cpp-example/flex-bison-cpp-example-0.1/doxygen-html/classExampleFlexLexer.html
};
}
