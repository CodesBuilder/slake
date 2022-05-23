%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

%define api.prefix {slake}
%define parse.error verbose
%locations

%code requires {
#include <slakedef.h>

extern FILE* slakein;
extern int slakelineno;

int slakelex();
int slakeparse();
void slakeerror(const char* msg, ...);
}

%union
{
	char* str;
	int i32;
	unsigned int u32;
	long long i64;
	unsigned long long u64;
	SlakeValue value;
	SlakeValueType type;
}

// Tokens
%token T_EQ "=="
%token T_NEQ "!="
%token T_LEQ "<="
%token T_GEQ ">="
%token T_LOR "||"
%token T_LAND "&&"

%token T_ADD_ASSIGN "+="
%token T_SUB_ASSIGN "-="
%token T_MUL_ASSIGN "*="
%token T_DIV_ASSIGN "/="
%token T_MOD_ASSIGN "%="
%token T_OR_ASSIGN "|="
%token T_AND_ASSIGN "&="
%token T_XOR_ASSIGN "^="

// Miscellaneous
%token <str> SYMBOL

// Keywords
%token KW_VAR "var"
%token KW_FUNCTION "function"
%token KW_RETURN "return"
%token KW_ASYNC "async"
%token KW_AWAIT "await"
%token KW_IF "if"
%token KW_ELSE "else"
%token KW_ELIF "elif"
%token KW_BREAK "break"
%token KW_CONTINUE "continue"
%token KW_LOOP "loop"
%token KW_FOR "for"
%token KW_WHILE "while"
%token KW_SWITCH "switch"
%token KW_CASE "case"
%token KW_DEFAULT "default"
%token KW_PUBLIC "public"
%token KW_IMPORT "import"

// Type keywords
%token KW_TYPE_INT "int"
%token KW_TYPE_LONG "long"
%token KW_TYPE_UINT "uint"
%token KW_TYPE_ULONG "ulong"
%token KW_TYPE_STRING "string"

%token <str> STR
%token <i32> INT
%token <i64> LONG
%token <u32> UINT
%token <u64> ULONG

%type <value> immediateValue
%type <type> typeName

%left '=' "+=" "-=" "*=" "/=" "%=" "|=" "&=" "^="
%left "||"
%left "&&"
%left '|'
%left '^'
%left '&'
%left "==" "!="
%left '<' "<=" '>' ">="
%left '+' '-'
%left '*' '/' '%'

%precedence '!' T_NEG

%%

statements:
statements statement|
%empty;

statement:
import ';'|
varDeclExpr ';'|
funcDef|
pubFuncDef;

//
// Module import.
//
import:
"import" SYMBOL '=' STR
{
};

//
// Function definition.
//
funcDef:
"function" SYMBOL '(' paramDefs ')' '{' execBody '}'
{
};

//
// Public function definition.
//
pubFuncDef:
"public" "function" SYMBOL '(' paramDefs ')' '{' execBody '}'
{
};

//
// Parameter definitions.
//
paramDefs:
paramDefs ',' paramDef | paramDef | %empty;
paramDef:
SYMBOL ':' typeName
{
};

//
// Execution body.
//
execBody: exprs | %empty;

//
// Expressions.
//
exprs: exprs expr | expr;
expr: singleExpr ';' | blockExpr;

//
// Single expressions (need a semicolon to terminate).
//
singleExpr:
varDeclExpr |
return |
await |
valuedExprs;

//
// Valued expressions.
//
valuedExprs: valuedExprs ',' valuedExpr | valuedExpr;
valuedExpr:
'(' valuedExpr ')'|
leftExpr|
rightExpr;

leftExpr:
varRef;

rightExpr:
immediateValue|
funcCall|
asyncFuncCall|
superFuncCall|
externalFuncCall|
asyncExternalFuncCall|
mathExpr;

//
// Block expressions.
//
blockExpr:
switchBlock;

//
// Variable reference.
//
varRef:
SYMBOL
{
}

//
// Basic mathematic operations.
//
mathExpr:
leftExpr '=' valuedExpr
{
}|
leftExpr "+=" valuedExpr
{
}|
leftExpr "-=" valuedExpr
{
}|
leftExpr "*=" valuedExpr
{
}|
leftExpr "/=" valuedExpr
{
}|
leftExpr "%=" valuedExpr
{
}|
leftExpr "|=" valuedExpr
{
}|
leftExpr "&=" valuedExpr
{
}|
leftExpr "^=" valuedExpr
{
}|
valuedExpr '+' valuedExpr
{
}|
valuedExpr '-' valuedExpr
{
}|
valuedExpr '*' valuedExpr
{
}|
valuedExpr '/' valuedExpr
{
}|
valuedExpr '%' valuedExpr
{
}|
valuedExpr '|' valuedExpr
{
}|
valuedExpr '&' valuedExpr
{
}|
valuedExpr '^' valuedExpr
{
}|
valuedExpr "||" valuedExpr
{
}|
valuedExpr "&&" valuedExpr
{
}|
valuedExpr '<' valuedExpr
{
}|
valuedExpr "<=" valuedExpr
{
}|
valuedExpr '>' valuedExpr
{
}|
valuedExpr ">=" valuedExpr
{
}|
valuedExpr "==" valuedExpr
{
}|
valuedExpr "!=" valuedExpr
{
}|
'!' valuedExpr
{
}|
'-' valuedExpr %prec T_NEG
{
};

//
// Function call.
//
funcCall:
SYMBOL '(' params ')'
{
};

//
// Asynchronous function call.
//
asyncFuncCall:
SYMBOL '(' params ')' "async"
{
};

//
// Super function call.
//
superFuncCall:
'@' SYMBOL '(' params ')'
{
};

//
// External function call.
//
externalFuncCall:
'@' SYMBOL SYMBOL '(' params ')'
{
};

//
// Asynchronous external function call.
//
asyncExternalFuncCall:
'@' SYMBOL SYMBOL '(' params ')' "async"
{
};

//
// Return.
//
return:
"return" valuedExpr
{
}|
"return"
{
};

//
// Await
//
await:
"await" valuedExpr
{
};

//
// Parameters.
//
params:
params ',' valuedExpr
{

}|
valuedExpr
{
}|
%empty;

//
// Variable declaration.
//
varDeclExpr: "var" varDecls;
varDecls: varDecls ',' varDecl | varDecl;
varDecl:
SYMBOL ':' typeName '=' valuedExpr
{

}|
SYMBOL ':' typeName
{

};

//
// Switch block.
//
switchBlock:
"switch" '(' valuedExpr ')' '{' switchBody '}';
switchBody: switchCases switchDefault;

switchCases: switchCases switchCase | switchCase;
switchCase:
"case" valuedExpr '{' execBody '}'

switchDefault:
"default" '{' execBody '}'|
%empty;

//
// Value types.
//
typeName:
"int" { $$ = VALUE_TYPE_INT; }|
"uint" { $$ = VALUE_TYPE_UINT; }|
"long" { $$ = VALUE_TYPE_LONG; }|
"ulong" { $$ = VALUE_TYPE_ULONG; }|
"string" { $$ = VALUE_TYPE_STR; };

//
// Values.
//
immediateValue:
STR { $$.data.str = strdup($1), $$.type = VALUE_TYPE_STR; }|
INT { $$.data.i32 = $1, $$.type = VALUE_TYPE_INT; }|
UINT { $$.data.u32 = $1, $$.type = VALUE_TYPE_UINT; }|
LONG { $$.data.i64 = $1, $$.type = VALUE_TYPE_LONG; }|
ULONG { $$.data.u64 = $1, $$.type = VALUE_TYPE_ULONG; };

%%
