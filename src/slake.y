%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

%define api.prefix {slake}

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
}

// Tokens
%token T_AT '@'
%token T_COLON ':'
%token T_SEMICOLON ';'
%token T_COMMA ','
%token T_DOLLAR '$'
%token T_LPARENTHESE '('
%token T_RPARENTHESE ')'
%token T_LBRACE '{'
%token T_RBRACE '}'
%token T_PERCENT '%'
%token T_PLUS '+'
%token T_MINUS '-'
%token T_ASTERISK '*'
%token T_SLASH '/'
%token T_OR '|'
%token T_AND '&'
%token T_XOR '^'
%token T_LBRACKET '['
%token T_RBRACKET ']'
%token T_NOT '!'
%token T_ASSIGN '='
%token T_LESSER '<'
%token T_GREATER '>'
%token T_EQ "=="
%token T_NEQ "!="
%token T_LEQ "<="
%token T_GEQ ">="
%token T_ADD_ASSIGN "+="
%token T_SUB_ASSIGN "-="
%token T_MUL_ASSIGN "*="
%token T_DIV_ASSIGN "/="
%token T_MOD_ASSIGN "%="

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

%token <str> STR
%token <i32> INT
%token <i64> LONG
%token <u32> UINT
%token <u64> ULONG

%type <value> value

%%

statements:
statements statement|
statement |
%empty;

statement:
funcDef|
pubFuncDef|
varDecls ';'|
import ';'
;

//
// Import
//
import:
"import" SYMBOL '=' STR
{
};

//
// Function definition.
//
funcDef:
"function" SYMBOL '(' paramDef ')' '{' exprs '}'
{
};

//
// Public function definition.
//
pubFuncDef:
"public" "function" SYMBOL '(' paramDef ')' '{' exprs '}'
{
};

//
// Parameter definitions.
//
paramDef:
paramDef ',' SYMBOL
{
}|
SYMBOL
{
}|
%empty;

//
// General expression.
//
exprs: exprs expr | expr | %empty;

expr: singleExpr ';' | blockExpr;

singleExpr:
varDecls |
valuedExprs;

valuedExprs:
valuedExprs valuedExpr|
valuedExpr;

valuedExpr:
'(' valuedExpr ')'|
varRef|
value|
funcCall|
superFuncCall|
externalFuncCall|
assign|
add|
sub|
mul|
div|
mod;

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
assign:
SYMBOL '=' valuedExpr
{
}|
SYMBOL '=' '?'
{
};

add:
valuedExpr '+' valuedExpr
{
};

sub:
valuedExpr '-' valuedExpr
{
};

mul:
valuedExpr '*' valuedExpr
{
};

div:
valuedExpr '/' valuedExpr
{
};

mod:
valuedExpr '%' valuedExpr
{
};

// Function call.
funcCall:
SYMBOL '(' params ')'
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
varDecls:
"var" varDecls ',' varDecl |
"var" varDecl

varDecl:
SYMBOL '=' value
{

}|
SYMBOL '=' '?'
{

}|
SYMBOL '=' SYMBOL
{

};

//
// Switch block.
//
switchBlock:
"switch" '(' valuedExpr ')' '{' switchBody '}';

switchBody:
switchCases|
switchCases switchDefault;

switchCases:
switchCases switchCase|
switchCase;

switchCase:
"case" valuedExpr '{' exprs '}'

switchDefault:
"default" '{' exprs '}'

//
// Values.
//
value:
STR { $$.data.str = strdup($1), $$.type = VALUE_TYPE_STR; }|
INT { $$.data.i32 = $1, $$.type = VALUE_TYPE_INT; }|
UINT { $$.data.u32 = $1, $$.type = VALUE_TYPE_UINT; }|
LONG { $$.data.i64 = $1, $$.type = VALUE_TYPE_LONG; }|
ULONG { $$.data.u64 = $1, $$.type = VALUE_TYPE_ULONG; };

%%
