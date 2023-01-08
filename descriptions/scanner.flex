%{
    #include <iostream>
    #include <cstdlib>
    #include "scanner.h"
    #include "parser.h"
    #include "location.hh"

    // Original yyterminate() macro returns int. Since we're using Bison 3 variants
    // as tokens, we must redefine it to change type from `int` to `Parser::semantic_type`

    #define yyterminate() mcool::Parser::make_END(currLocation);

    # define YY_USER_ACTION  currLocation.columns(yyleng);
%}

%option nodefault
%option noyywrap
%option c++
%option yylineno
%option yyclass="Scanner"
%option prefix="mcool_"


WHITE_SPACE        [ \t\r\v\f]
NEW_LINE           \n
DIGIT              [0-9]
LOWER_CASE_LETTER  [a-z]
UPPER_CASE_LETTER  [A-Z]
LETTER             [a-zA-Z]+


%x COMMENT STRING
%%

%{
  currLocation.step ();
%}

"+" { return mcool::Parser::make_PLUS(currLocation); }
"-" { return mcool::Parser::make_MINUS(currLocation); }
"*" { return mcool::Parser::make_STAR(currLocation); }
"/" { return mcool::Parser::make_FSLASH(currLocation); }
"(" { return mcool::Parser::make_LEFTPAR(currLocation); }
")" { return mcool::Parser::make_RIGHTPAR(currLocation); }
";" { return mcool::Parser::make_SEMICOLON(currLocation); }


t[Rr][Uu][Ee] {
  yylineno;
  return mcool::Parser::make_BOOLEAN(true, currLocation);
}

f[Aa][Ll][Ss][Ee] {
  return mcool::Parser::make_BOOLEAN(false, currLocation);

}

{LOWER_CASE_LETTER}({LETTER}|{DIGIT}|"_")*|"self" {
  return mcool::Parser::make_OBJECTID(yytext, currLocation);
}

{DIGIT}+ {
  uint64_t number = std::strtoull(yytext, 0, 10);
  return mcool::Parser::make_NUMBER(number, currLocation);
}

 /* Comments */
"--".*                            {/*no action*/}

"(*"                              {
  commentCounter = 1;
  BEGIN COMMENT;
}

<COMMENT>"(*"                     {
  commentCounter += 1;
}

<COMMENT>"*)"                     {
  commentCounter -= 1;
  if (commentCounter == 0) {
    BEGIN INITIAL;
  }
}


<COMMENT>{NEW_LINE}+          { currLocation.lines(yyleng); currLocation.step(); }
<COMMENT>{WHITE_SPACE}+       { currLocation.step(); }
<COMMENT><<EOF>>              {
  BEGIN INITIAL;
  return reportError("EOF in comment");
}

<COMMENT>.                    { /*no action*/ }


"*)"                              {
  return reportError("Unmatched *)");
}


 /* String Literals */

"\""                              {
  currString.clear();
  BEGIN STRING;
}

<STRING>\0.*                       {
  BEGIN INITIAL;
  return reportError("string contains null character");
}

<STRING>"\\"[tnfrvb]     {
  // escaped characters inside a string literal
  switch(yytext[1]) {
    case 't': {currString.push_back('\t'); break;}
    case 'n': {currString.push_back('\n'); break;}
    case 'f': {currString.push_back('\f'); break;}
    case 'r': {currString.push_back('\r'); break;}
    case 'v': {currString.push_back('\v'); break;}
    case 'b': {currString.push_back('\b'); break;}
    default: {break;}
  }
}

<STRING>\\{WHITE_SPACE}+{NEW_LINE} { currLocation.lines(1); currLocation.step(); }
<STRING>{NEW_LINE}+                { currLocation.lines(yyleng); currLocation.step(); }


<STRING>"\\"[^tnfrvb]    {
  // if a user types `\c` then it should just `c`
  currString.push_back(yytext[1]);
}

<STRING>[^\"]            {
  // record next char in a string
  currString.push_back(yytext[0]);
}

<STRING><<EOF>>          {
  BEGIN INITIAL;
  return reportError("EOF in string constant");
}

<STRING>"\""            {
  BEGIN INITIAL;
  return mcool::Parser::make_STRING(currString, currLocation);
}

{WHITE_SPACE}+ { currLocation.step(); }
{NEW_LINE}+    { currLocation.lines(yyleng); currLocation.step(); }

<<EOF>>     { return yyterminate(); }

. { std::string msg = "unknown character - ";
    msg += yytext[0];
    reportError(msg);
    yyterminate();
  }

%%