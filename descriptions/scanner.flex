%{
	#include <iostream>
	#include <cstdlib>
  #include "scanner.h"
	#include "parser.h"
	#include "location.hh"

	// Original yyterminate() macro returns int. Since we're using Bison 3 variants
	// as tokens, we must redefine it to change type from `int` to `Parser::semantic_type`

	#define yyterminate() mcool::Parser::make_YYEOF(mcool::location());
%}

%option nodefault
%option noyywrap
%option c++
%option yyclass="Scanner"
%option prefix="mcool_"


WHITE_SPACE        [ \t\r\n\v\f]


%%


[a-z]+ { 
         std::cout << "Scanner: word: " << yytext << std::endl;
	       return mcool::Parser::make_TEXT(yytext, mcool::location());
	     }

[1-9][0-9]* {
              std::cout << "Scanner: decimal number: " << yytext << std::endl;
              uint64_t number = std::strtoull(yytext, 0, 10);
              return mcool::Parser::make_NUMBER(number, mcool::location());
            }

{WHITE_SPACE}+ {/*no action*/}

<<EOF>>     { return yyterminate(); }

. { std::cout << "Scanner: unknown character [" << yytext << "]" << std::endl; }

%%