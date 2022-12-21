// https://www.gnu.org/software/bison/manual/bison.html#A-Simple-C_002b_002b-Example

%require "3.2"
%language "c++"

//%defines
%define api.parser.class { Parser }

%define api.value.type variant
%define api.token.constructor
%define api.token.prefix {TOK_}
%define api.namespace { mcool }

%locations

%lex-param { mcool::Scanner& scanner }
%parse-param { mcool::Scanner& scanner }


%code requires {
  #include <iostream>
  #include <string>
  #include <vector>

  namespace mcool {
    class Scanner;
  }
}

%code top
{
  #include "scanner.h"
  #include "parser.h"

  static mcool::Parser::symbol_type yylex(mcool::Scanner& scanner) {
    return scanner.get_next_token();
  }
}

%code {
  #include <iostream>
  #include <string>
  #include <vector>

  // print a list of strings.
  void print_v(const std::vector<std::string>& str) {
    std::cout << '{';
    std::string sep("");
    for (const auto& item: str) {
      std::cout << sep << item;
      sep = std::string("| ");
    }
    std::cout << '}';
  }
}

%nterm <std::vector<std::string>> list;
%nterm <std::string> item;
%token <std::string> TEXT;
%token <int> NUMBER;


%%
result: 
  list { print_v($1); }
  ;


list:
  %empty { /* Generates an empty string list */ }
  | list item { $$ = $1; $$.push_back ($2); }
  ;


item:
  TEXT
| NUMBER  { $$ = std::to_string($1); }
;


%%

// Report an error to the user.
void mcool::Parser::error(const location &loc , const std::string &msg) {
  std::cerr << msg << '\n';
}
