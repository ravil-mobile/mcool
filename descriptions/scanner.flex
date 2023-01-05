%{
    #include <iostream>
    #include <cstdlib>
    #include "scanner.h"
    #include "parser.h"
    #include "location.hh"

    // Original yyterminate() macro returns int. Since we're using Bison 3 variants
    // as tokens, we must redefine it to change type from `int` to `Parser::semantic_type`

    #define yyterminate() mcool::Parser::make_END(mcool::location());
%}

%option nodefault
%option noyywrap
%option c++
%option yyclass="Scanner"
%option prefix="mcool_"


WHITE_SPACE        [ \t\r\n\v\f]
DIGIT              [0-9]
LOWER_CASE_LETTER  [a-z]
UPPER_CASE_LETTER  [A-Z]
LETTER             [a-zA-Z]+

%%

"+" { return mcool::Parser::make_PLUS(mcool::location()); }
"-" { return mcool::Parser::make_MINUS(mcool::location()); }
"*" { return mcool::Parser::make_STAR(mcool::location()); }
"/" { return mcool::Parser::make_FSLASH(mcool::location()); }
"(" { return mcool::Parser::make_LEFTPAR(mcool::location()); }
")" { return mcool::Parser::make_RIGHTPAR(mcool::location()); }
";" { return mcool::Parser::make_SEMICOLON(mcool::location()); }


t[Rr][Uu][Ee] {
  return mcool::Parser::make_BOOLEAN(true, mcool::location());
}

f[Aa][Ll][Ss][Ee] {
  return mcool::Parser::make_BOOLEAN(false, mcool::location());

}


UPPER_CASE_LETTER+ {
  return mcool::Parser::make_STRING(yytext, mcool::location());
}

{LOWER_CASE_LETTER}({LETTER}|{DIGIT}|"_")*|"self" {
  return mcool::Parser::make_OBJECTID(yytext, mcool::location());
}

{DIGIT}+ {
  uint64_t number = std::strtoull(yytext, 0, 10);
  return mcool::Parser::make_NUMBER(number, mcool::location());
}

{WHITE_SPACE}+ {/*no action*/}

<<EOF>>     { return yyterminate(); }

. { std::cout << "Scanner: unknown character [" << yytext << "]" << std::endl; }

%%