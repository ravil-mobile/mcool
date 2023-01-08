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
%define parse.trace
%define parse.error detailed
%define parse.lac full

%lex-param { mcool::Scanner& scanner }
%parse-param { mcool::Scanner& scanner }
%parse-param { mcool::AstTree& astTree }


%code requires {
  #include "AstTree.h"
  #include "MemoryManagement.h"
  #include <iostream>
  #include <string>
  #include <vector>
  #include <cstdlib>

  namespace mcool {
    class Scanner;
  }
}

%code top
{
  #include "scanner.h"
  #include "parser.h"
  #include "AstTree.h"

  static mcool::Parser::symbol_type yylex(mcool::Scanner& scanner) {
    return scanner.get_next_token();
  }
}

%code {
  #include <iostream>
  #include <string>
  #include <vector>
}

%nterm <mcool::ast::Expressions*> program;
%nterm <mcool::ast::Expressions*> exprs;
%nterm <mcool::ast::Expression*> expr;
%nterm <mcool::ast::Expression*> term;
%nterm <mcool::ast::Node*> factor;

%token <std::string> STRING;
%token <std::string> OBJECTID;
%token <int> NUMBER;
%token <bool> BOOLEAN;

%token CLASS     "class";
%token IF        "if";
%token THEN      "then";
%token ELSE      "else";
%token FI        "fi";
%token IN        "in";
%token INHERITS  "inherits";
%token LET       "let";
%token LOOP      "loop";
%token POOL      "pool";
%token WHILE     "while";
%token CASE      "case";
%token ESAC      "esac";
%token OF        "of";
%token NEW       "new";
%token ISVOID    "isvoid";
%token NOT       "not";

%token DARROW "=>";
%token LEQ    "<=";
%token LE     "<";
%token ASSIGN "<-";
%token EQ     "=";
%token PLUS   "+";
%token MINUS  "-";
%token STAR   "*";
%token FSLASH "/";
%token NEG    "~";

%token SEMICOLON      ";";
%token COLON          ":";
%token COMMA          ",";
%token DOT            ".";
%token LEFTPAR        "(";
%token RIGHTPAR       ")";
%token CURLY_LEFTPAR  "{";
%token CURLY_RIGHTPAR "}";
%token AT             "@";

%token END       0;

%%

program:
   exprs { astTree.set($1); }
  ;

exprs:
   expr SEMICOLON { $$ = mcool::make<mcool::ast::Expressions>(); $$->add($1); }
  | exprs expr SEMICOLON { $1->add($2); $$ = $1; }
  ;

expr:
    expr PLUS  term { $$ = mcool::make<mcool::ast::PlusNode>($1, $3); }
  | expr MINUS term { $$ = mcool::make<mcool::ast::MinusNode>($1, $3); }
  | term { $$ = $1; }
  ;

term:
    term STAR factor { $$ = mcool::make<mcool::ast::MultiplyNode>($1, $3); }
  | term FSLASH factor { $$ = mcool::make<mcool::ast::DivideNode>($1, $3); }
  | factor { $$ = mcool::make<mcool::ast::SingeltonExpression>($1); }
  ;


factor:
    NUMBER { $$ = mcool::make<mcool::ast::Int>($1); }
  | BOOLEAN { $$ = mcool::make<mcool::ast::Bool>($1); }
  | OBJECTID { $$ = mcool::make<mcool::ast::ObjectId>($1); }
  | STRING { $$ = mcool::make<mcool::ast::String>($1); }
  | LEFTPAR expr RIGHTPAR { $$ = $2; }
  ;

%%

// Report an error to the user.
void mcool::Parser::error(const location &loc , const std::string &msg) {
  std::cerr << loc << " : " << msg << '\n';
}
