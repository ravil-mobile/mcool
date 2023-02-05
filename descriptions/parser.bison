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
  #include "Loc.h"
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
  #include "Scanner.h"
  #include "Parser.h"
  #include "AstTree.h"

  static mcool::Parser::symbol_type yylex(mcool::Scanner& scanner) {
    return scanner.get_next_token();
  }

  void setLoc(mcool::ast::Node* node, mcool::location &yyLoc) {
    auto begin = mcool::Position{yyLoc.begin.line, yyLoc.begin.column};
    auto end = mcool::Position{yyLoc.end.line, yyLoc.end.column};
    auto* filename = mcool::make<mcool::ast::StringPtr>(*(yyLoc.begin.filename));

    auto mcoolLoc = mcool::Loc(begin, end, filename);
    node->setLocation(mcoolLoc);
  }
}

%code {
  #include <iostream>
  #include <string>
  #include <vector>
}

%nterm <mcool::ast::CoolClassList*>  program;
%nterm <mcool::ast::CoolClassList*>  class_list;
%nterm <mcool::ast::CoolClass*>      class;
%nterm <mcool::ast::AttributeList*>  attribute_list;
%nterm <mcool::ast::ClassAttribute*> single_method;
%nterm <mcool::ast::ClassAttribute*> single_member;
%nterm <mcool::ast::FormalList*>     formal_list;
%nterm <mcool::ast::SingleFormal*>   single_formal;

%nterm <mcool::ast::Expression*>  expr;
%nterm <mcool::ast::Expression*>  loop_expr;
%nterm <mcool::ast::Expression*>  new_expr;
%nterm <mcool::ast::Expression*>  move_expr;
%nterm <mcool::ast::Expression*>  cond_expr;
%nterm <mcool::ast::Expression*>  not_expr;
%nterm <mcool::ast::Expression*>  block_expr;
%nterm <mcool::ast::SingleCase*>  single_case;
%nterm <mcool::ast::CaseList*>    case_list;
%nterm <mcool::ast::Expression*>  case_expr;
%nterm <mcool::ast::Expression*>  let_expr;
%nterm <mcool::ast::Expression*>  let_tail_expr;
%nterm <mcool::ast::Expressions*> exprs;
%nterm <mcool::ast::Expressions*> arg_list;

%nterm <mcool::ast::Expression*>  relational_expr;
%nterm <mcool::ast::Expression*>  additive_expr;
%nterm <mcool::ast::Expression*>  multiplicative_expr;
%nterm <mcool::ast::Expression*>  unary_expr;
%nterm <mcool::ast::Expression*>  dispatch;
%nterm <mcool::ast::Node*>        primary_expr;


%token <std::string> STRING;
%token <std::string> OBJECTID;
%token <std::string> TYPEID;
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
%token LESS   "<";
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

%token ERROR      "ERROR";
%token END              0;

%%

program
  : class_list { astTree.set($1); }
  ;

class_list
  : class SEMICOLON {
    $$ = mcool::make<mcool::ast::CoolClassList>();
    $$->add($1);
    setLoc($$, @$);
  }
  | class_list class SEMICOLON {
    $1->add($2);
    $$ = $1;
    setLoc($$, @$);
  }
  | error CURLY_RIGHTPAR SEMICOLON {
    astTree.setError();
    error(@1, "class error");
    $$ = mcool::make<mcool::ast::CoolClassList>();
    setLoc($$, @$);
    yyerrok;
  }
  | class_list error CURLY_RIGHTPAR SEMICOLON {
    error(@2, "class error");
    astTree.setError();
    $$ = $1;
    yyerrok;
  }
  ;

class
  : CLASS TYPEID CURLY_LEFTPAR attribute_list CURLY_RIGHTPAR {
    auto* className = mcool::make<mcool::ast::TypeId>($2);
    auto* parentClassName = mcool::make<mcool::ast::TypeId>(std::string("Object"));
    auto* fileName = mcool::make<mcool::ast::String>(scanner.getFileName());
    $$ = mcool::make<mcool::ast::CoolClass>(className, parentClassName, $4, fileName);
    setLoc($$, @$);
  }
  | CLASS TYPEID INHERITS TYPEID CURLY_LEFTPAR attribute_list CURLY_RIGHTPAR {
    auto* className = mcool::make<mcool::ast::TypeId>($2);
    auto* parentClassName = mcool::make<mcool::ast::TypeId>($4);
    auto* fileName = mcool::make<mcool::ast::String>(scanner.getFileName());
    $$ = mcool::make<mcool::ast::CoolClass>(className, parentClassName, $6, fileName);
    setLoc($$, @$);
  }
  ;

attribute_list
  : /* empty */ {
    $$ = mcool::make<mcool::ast::AttributeList>();
    setLoc($$, @$);
  }
  | attribute_list single_method SEMICOLON {
    $1->add($2);
    $$ = $1;
    setLoc($$, @$);
  }
  | attribute_list single_member SEMICOLON {
    $1->add($2);
    $$ = $1;
    setLoc($$, @$);
  }
  | attribute_list error SEMICOLON {
    error(@2, "in attribute of a class");
    astTree.setError();
    $$ = $1;
    yyerrok;
  }
  ;

single_method
  : OBJECTID LEFTPAR formal_list RIGHTPAR COLON TYPEID CURLY_LEFTPAR expr CURLY_RIGHTPAR {
    auto* methodName = mcool::make<mcool::ast::ObjectId>($1);
    auto* returnType = mcool::make<mcool::ast::TypeId>($6);
    $$ = mcool::make<mcool::ast::SingleMethod>(methodName, $3, returnType, $8);
    setLoc($$, @$);
  }
  | OBJECTID LEFTPAR formal_list RIGHTPAR COLON TYPEID CURLY_LEFTPAR error CURLY_RIGHTPAR {
    std::string msg = "near or in the body of method `" + $1 + "`";
    error(@5, msg);
    astTree.setError();

    auto* methodName = mcool::make<mcool::ast::ObjectId>($1);
    auto* returnType = mcool::make<mcool::ast::TypeId>($6);
    auto* noExpr = mcool::make<mcool::ast::NoExpr>();
    $$ = mcool::make<mcool::ast::SingleMethod>(methodName, $3, returnType, noExpr);
    setLoc($$, @$);
    yyerrok;
  }
  | OBJECTID LEFTPAR formal_list RIGHTPAR error CURLY_LEFTPAR expr CURLY_RIGHTPAR {
    std::string msg = "in return type of method `" + $1 + "`";
    error(@6, msg);
    astTree.setError();

    auto* methodName = mcool::make<mcool::ast::ObjectId>($1);
    auto* returnType = mcool::make<mcool::ast::TypeId>(std::string("Object"));
    $$ = mcool::make<mcool::ast::SingleMethod>(methodName, $3, returnType, $7);
    setLoc($$, @$);
    yyerrok;
  }
  | OBJECTID LEFTPAR error RIGHTPAR COLON TYPEID CURLY_LEFTPAR expr CURLY_RIGHTPAR {
    std::string msg = "in parameter list of method `" + $1 + "`";
    error(@3, msg);
    astTree.setError();

    auto* methodName = mcool::make<mcool::ast::ObjectId>($1);
    auto* returnType = mcool::make<mcool::ast::TypeId>($6);
    auto* noParam = mcool::make<mcool::ast::FormalList>();
    $$ = mcool::make<mcool::ast::SingleMethod>(methodName, noParam, returnType, $8);
    setLoc($$, @$);
    yyerrok;
  }
  ;

single_member
  : OBJECTID COLON TYPEID {
    auto* variable = mcool::make<mcool::ast::ObjectId>($1);
    auto* variableType = mcool::make<mcool::ast::TypeId>($3);
    auto* noExpr = mcool::make<mcool::ast::NoExpr>();
    $$ = mcool::make<mcool::ast::SingleMember>(variable, variableType, noExpr);
    setLoc($$, @$);
  }
  | OBJECTID COLON TYPEID move_expr {
    auto* variable = mcool::make<mcool::ast::ObjectId>($1);
    auto* variableType = mcool::make<mcool::ast::TypeId>($3);
    $$ = mcool::make<mcool::ast::SingleMember>(variable, variableType, $4);
    setLoc($$, @$);
  }
  ;

formal_list
  : /* empty */ {
    $$ = mcool::make<mcool::ast::FormalList>();
    setLoc($$, @$);
  }
  | single_formal {
    $$ = mcool::make<mcool::ast::FormalList>();
    $$->add($1);
    setLoc($$, @$);
  }
  | formal_list COMMA single_formal {
    $1->add($3);
    $$ = $1;
  }
  ;

single_formal
  : OBJECTID COLON TYPEID  {
    auto* variable = mcool::make<mcool::ast::ObjectId>($1);
    auto* variableType = mcool::make<mcool::ast::TypeId>($3);
    $$ = mcool::make<mcool::ast::SingleFormal>(variable, variableType);
    setLoc($$, @$);
  }
  ;

expr
  : relational_expr { $$ = $1; }
  | loop_expr       { $$ = $1; }
  | new_expr        { $$ = $1; }
  | cond_expr       { $$ = $1; }
  | not_expr        { $$ = $1; }
  | block_expr      { $$ = $1; }
  | case_expr       { $$ = $1; }
  | OBJECTID move_expr {
    auto* variableName = mcool::make<mcool::ast::ObjectId>($1);
    $$ = mcool::make<mcool::ast::AssignExpr>(variableName, $2);
    setLoc($$, @$);
  }
  | LET let_expr    { $$ = $2; }
  ;

loop_expr
  : WHILE expr LOOP expr POOL {
    $$ = mcool::make<mcool::ast::WhileLoop>($2, $4);
    setLoc($$, @$);
  }
  ;

move_expr
  : ASSIGN expr { $$ = $2; setLoc($$, @$); }
  ;

new_expr
  : NEW TYPEID {
    auto* typeName = mcool::make<mcool::ast::TypeId>($2);
    $$ = mcool::make<mcool::ast::NewExpr>(typeName);
    setLoc($$, @$);
  }
  ;

cond_expr
  : IF expr THEN expr FI {
    $$ = mcool::make<mcool::ast::IfThenExpr>($2, $4);
    setLoc($$, @$);
  }
  | IF expr THEN expr ELSE expr FI {
    $$ = mcool::make<mcool::ast::IfThenElseExpr>($2, $4, $6);
    setLoc($$, @$);
  }
  ;

not_expr
  : NOT relational_expr {
    $$ = mcool::make<mcool::ast::NotExpr>($2);
    setLoc($$, @$);
  }
  ;

block_expr
  : CURLY_LEFTPAR exprs CURLY_RIGHTPAR {
    $$ = mcool::make<mcool::ast::BlockExpr>($2);
    setLoc($$, @$);
  }
  ;

exprs
  : expr SEMICOLON {
    $$ = mcool::make<mcool::ast::Expressions>();
    $$->add($1);
    setLoc($$, @$);
  }
  | exprs expr SEMICOLON {
    $1->add($2);
    $$ = $1;
    setLoc($$, @$);
  }
  | error SEMICOLON {
    error(@1, "invalid expression inside a block");
    astTree.setError();
    $$ = mcool::make<mcool::ast::Expressions>();
    setLoc($$, @$);
    yyerrok;
  }
  | exprs error SEMICOLON {
    error(@2 + @3, "invalid expression inside a block");
    astTree.setError();
    $$ = $1;
    yyerrok;
  }
  ;

case_expr
  : CASE expr OF case_list ESAC {
    $$ = mcool::make<mcool::ast::CaseExpr>($2, $4);
    setLoc($$, @$);
  }
  ;

case_list
  : single_case SEMICOLON {
    $$ = mcool::make<mcool::ast::CaseList>();
    $$->add($1);
    setLoc($$, @$);
  }
  | case_list single_case SEMICOLON {
    $1->add($2);
    $$ = $1;
    setLoc($$, @$);
  }
  | error SEMICOLON {
    error(@1, "invalid case");
    astTree.setError();

    $$ = mcool::make<mcool::ast::CaseList>();
    setLoc($$, @$);
    yyerrok;
  }
  | case_list error SEMICOLON {
    error(@1, "invalid case");
    astTree.setError();

    $$ = $1;
    yyerrok;
  }
  ;

single_case
   : OBJECTID COLON TYPEID DARROW expr {
     auto* variable = mcool::make<mcool::ast::ObjectId>($1);
     auto* typeName = mcool::make<mcool::ast::TypeId>($3);
     $$ = mcool::make<mcool::ast::SingleCase>(variable, typeName, $5);
     setLoc($$, @$);
   }
   ;

let_expr
  : let_tail_expr { $$ = $1; }
  | OBJECTID COLON TYPEID COMMA let_expr {
    auto* variable = mcool::make<mcool::ast::ObjectId>($1);
    auto* typeName = mcool::make<mcool::ast::TypeId>($3);
    auto* noExpr = mcool::make<mcool::ast::NoExpr>();
    $$ = mcool::make<mcool::ast::LetExpr>(variable, typeName, noExpr, $5);
    setLoc($$, @$);
  }
  | OBJECTID COLON TYPEID move_expr COMMA let_expr {
    auto* variable = mcool::make<mcool::ast::ObjectId>($1);
    auto* typeName = mcool::make<mcool::ast::TypeId>($3);
    $$ = mcool::make<mcool::ast::LetExpr>(variable, typeName, $4, $6);
    setLoc($$, @$);
  }
  ;

let_tail_expr
  : OBJECTID COLON TYPEID IN expr {
    auto* variable = mcool::make<mcool::ast::ObjectId>($1);
    auto* typeName = mcool::make<mcool::ast::TypeId>($3);
    auto* noExpr = mcool::make<mcool::ast::NoExpr>();
    $$ = mcool::make<mcool::ast::LetExpr>(variable, typeName, noExpr, $5);
    setLoc($$, @$);
  }
  | OBJECTID COLON TYPEID move_expr IN expr {
    auto* variable = mcool::make<mcool::ast::ObjectId>($1);
    auto* typeName = mcool::make<mcool::ast::TypeId>($3);
    $$ = mcool::make<mcool::ast::LetExpr>(variable, typeName, $4, $6);
    setLoc($$, @$);
  }
  ;

relational_expr
  : relational_expr LEQ additive_expr {
    $$ = mcool::make<mcool::ast::LessEqualNode>($1, $3);
    setLoc($$, @$);
  }
  | relational_expr LESS additive_expr {
    $$ = mcool::make<mcool::ast::LessNode>($1, $3);
    setLoc($$, @$);
  }
  | relational_expr EQ additive_expr {
    $$ = mcool::make<mcool::ast::EqualNode>($1, $3);
    setLoc($$, @$);
  }
  | additive_expr { $$ = $1; }
  ;

additive_expr
  : additive_expr PLUS  multiplicative_expr {
    $$ = mcool::make<mcool::ast::PlusNode>($1, $3);
    setLoc($$, @$);
  }
  | additive_expr MINUS multiplicative_expr {
    $$ = mcool::make<mcool::ast::MinusNode>($1, $3);
    setLoc($$, @$);
  }
  | multiplicative_expr { $$ = $1; }
  ;

multiplicative_expr
  : multiplicative_expr STAR unary_expr {
    $$ = mcool::make<mcool::ast::MultiplyNode>($1, $3);
    setLoc($$, @$);
  }
  | multiplicative_expr FSLASH unary_expr {
    $$ = mcool::make<mcool::ast::DivideNode>($1, $3);
    setLoc($$, @$);
  }
  | unary_expr { $$ = $1; }
  ;

unary_expr
  : primary_expr { $$ = mcool::make<mcool::ast::PrimaryExpr>($1); setLoc($$, @$); }
  | NEG unary_expr { $$ = mcool::make<mcool::ast::NegationNode>($2); setLoc($$, @$); }
  | ISVOID unary_expr { $$ = mcool::make<mcool::ast::IsVoidNode>($2); setLoc($$, @$); }
  ;


primary_expr
  : NUMBER { $$ = mcool::make<mcool::ast::Int>($1); setLoc($$, @$); }
  | STRING { $$ = mcool::make<mcool::ast::String>($1); setLoc($$, @$); }
  | BOOLEAN { $$ = mcool::make<mcool::ast::Bool>($1); setLoc($$, @$); }
  | OBJECTID { $$ = mcool::make<mcool::ast::ObjectId>($1); setLoc($$, @$); }
  | LEFTPAR relational_expr RIGHTPAR { $$ = $2; setLoc($$, @$); }
  | dispatch { $$ = $1; }
  ;

dispatch
  : OBJECTID LEFTPAR arg_list RIGHTPAR {
      auto* selfObject = mcool::make<mcool::ast::ObjectId>(std::string("self"));
      auto* method = mcool::make<mcool::ast::ObjectId>($1);
      $$ = mcool::make<mcool::ast::Dispatch>(selfObject, method, $3);
      setLoc($$, @$);
  }
  | primary_expr DOT OBJECTID LEFTPAR arg_list RIGHTPAR {
      auto* methodName = mcool::make<mcool::ast::ObjectId>($3);
      $$ = mcool::make<mcool::ast::Dispatch>($1, methodName, $5);
      setLoc($$, @$);
  }
  | primary_expr AT TYPEID DOT OBJECTID LEFTPAR arg_list RIGHTPAR {
      auto* typeName = mcool::make<mcool::ast::TypeId>($3);
      auto* methodName = mcool::make<mcool::ast::ObjectId>($5);
      $$ = mcool::make<mcool::ast::StaticDispatch>($1, typeName, methodName, $7);
      setLoc($$, @$);
  }
  ;

arg_list
    : /*empty*/            { $$ = mcool::make<mcool::ast::Expressions>(); setLoc($$, @$); }
    | expr                 { $$ = mcool::make<mcool::ast::Expressions>(); $$->add($1); setLoc($$, @$); }
    | arg_list COMMA expr  { $1->add($3); $$ = $1; setLoc($$, @$); }
    ;

%%

// Report an error to the user.
void mcool::Parser::error(const location &loc , const std::string &msg) {
  std::cerr << loc <<  " : " << msg << '\n';
}

