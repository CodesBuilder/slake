%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

%define api.prefix slake

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
	HkValue value;
}

//
// Signs.
//
%token SIGN_AT '@'
%token SIGN_COLON ':'
%token SIGN_SEMICOLON ';'
%token SIGN_COMMA ','
%token SIGN_DOLLAR '$'
%token SIGN_LPARENTHESE '('
%token SIGN_RPARENTHESE ')'
%token SIGN_NEWLINE '\n'
%token SIGN_LBRACE '{'
%token SIGN_RBRACE '}'

// Tokens.
%token <str> SYMBOL

%token VAR "var"
%token FUNCTION "function"

%token ASYNC "async"
%token AWAIT "await"

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
;

statement:
superCommand ';'|
funcDecl|
varDecls ';'
;

// Super command.
superCommand:
'@' SYMBOL
{
}|
'@' SYMBOL superCommandParam
{
}
;

superCommandParam:
superCommandParam value
{

}|
value
{
};

// Function declaration.
funcDecl:
"function" SYMBOL '(' paramDef ')' '{' funcBody '}'
{
}

funcBody: funcBody expr | expr;

// Wrap for parameter definitions.
paramDef:
paramDef ',' SYMBOL
{
}|
SYMBOL
{
}|
;

// General expression.
expr:
funcCall|
assignments ';';

// Assignment
assignments:
assignments ',' assignment|
assignment
;

assignment:
SYMBOL '=' SYMBOL
{
}|
SYMBOL '=' value
{
}|
SYMBOL '=' '?'
{
}|

// Function call.
funcCall:
SYMBOL '(' ')'
{
}|
SYMBOL '(' param ')'
{
};

// Wrap for parameters in function calls.
param:
param ',' value
{

}|
value
{
};

// Variable declaration.
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

}

// Wrap for values.
value:
STR { $$.data.str = strdup($1), $$.type = VALUE_TYPE_STR; }|
INT { $$.data.i32 = $1, $$.type = VALUE_TYPE_INT; }|
UINT { $$.data.u32 = $1, $$.type = VALUE_TYPE_UINT; }|
LONG { $$.data.i64 = $1, $$.type = VALUE_TYPE_LONG; }|
ULONG { $$.data.u64 = $1, $$.type = VALUE_TYPE_ULONG; };

%%
