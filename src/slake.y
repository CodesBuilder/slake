%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <slakedef.h>
#include <assert.h>

static SlakeExecBody currentExecBody = NULL;
static SlakeSwitchCase** currentSwitchCases = NULL;
static size_t currentSwitchCaseCount = 0;

//
// Push an expression into current execution body.
//
static void pushExpr(SlakeExpr* expr)
{
	assert(expr!=NULL);

	if(!currentExecBody)
		currentExecBody = slakeCreateExecBody();

	slakeExprAttach(currentExecBody, expr);
}

//
// Get current execution body and reset for new execution body.
//
static SlakeExecBody postCurrentExecBody()
{
	SlakeExecBody execBody = currentExecBody;
	currentExecBody = NULL;
	return execBody;
}

//
// Push a switch case.
//
static void pushSwitchCase(SlakeSwitchCase* swCase)
{
	if(!currentSwitchCases)
		currentSwitchCases = malloc(sizeof(SlakeSwitchCase));
	else
		currentSwitchCases = realloc(currentSwitchCases, sizeof(SlakeSwitchCase*) * (currentSwitchCaseCount + 1));

	if(!currentSwitchCases)
		slakePanic("Out of memory");

	currentSwitchCases[currentSwitchCaseCount] = swCase;
	currentSwitchCaseCount++;
}

//
// Get current switch cases and reset for new switch cases.
//
static SlakeSwitchCase** postCurrentSwitchCases()
{
	SlakeSwitchCase** swCases = currentSwitchCases;
	currentSwitchCases = NULL;
	return swCases;
}
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
	SlakeValue* value;
	SlakeValueType type;
	SlakeExpr* expr;
	SlakeExecBody execBody;
	SlakeSwitchCase* swCase;
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

%type <execBody> execBody
%type <execBody> exprs
%type <expr> leftExpr
%type <expr> rightExpr
%type <expr> valuedExprs
%type <expr> valuedExpr
%type <expr> expr
%type <expr> singleExpr
%type <expr> blockExpr

%type <expr> funcCall
%type <expr> asyncFuncCall
%type <expr> superFuncCall
%type <expr> externalFuncCall
%type <expr> asyncExternalFuncCall

%type <expr> return
%type <expr> await

%type <expr> varRef

%type <expr> basicOp

%type <expr> switchBlock
%type <swCase> switchCase
%type <execBody> switchDefault

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
	SlakeValue* value = slakeMakeString($2);
	slakeSetVariable(slakeGetRootScope(), $2, value);
	slakeDestroyValue(value);
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
execBody: exprs { $$ = postCurrentExecBody(); } | %empty;

//
// Expressions.
//
exprs:
exprs expr { pushExpr($2); } |
expr { pushExpr($1); };

expr:
singleExpr ';' { $$ = $1; } |
blockExpr { $$ = $1; };

//
// Single expressions (need a semicolon to terminate).
//
singleExpr:
varDeclExpr { $$ = NULL; } |
return { $$ = $1; } |
await { $$ = $1; } |
valuedExprs { $$ = $1; };

//
// Valued expressions.
//
valuedExprs:
valuedExprs ',' valuedExpr
{
	pushExpr($3);
}|
valuedExpr
{
	pushExpr($1);
};

valuedExpr:
'(' valuedExpr ')' { $$ = $2; }|
leftExpr { $$ = $1; }|
rightExpr { $$ = $1; };

leftExpr:
varRef { $$ = $1; };

rightExpr:
immediateValue { $$ = slakeExprImmediateValue($1); slakeDestroyValue($1); }|
funcCall { $$ = $1; }|
asyncFuncCall { $$ = $1; }|
superFuncCall { $$ = $1; }|
externalFuncCall { $$ = $1; }|
asyncExternalFuncCall { $$ = $1; }|
basicOp { $$ = $1; };

//
// Block expressions.
//
blockExpr:
switchBlock { $$ = $1; };

//
// Variable reference.
//
varRef:
SYMBOL
{
	SlakeExpr* expr = slakeExprVarRef($1);
	$$ = expr;
}

//
// Basic operators.
//
basicOp:
leftExpr '=' valuedExpr { $$ = slakeExprBinary(BINARY_EXPR_MOV, $1, $3); }|
leftExpr "+=" valuedExpr { $$ = slakeExprBinary(BINARY_EXPR_MOV, $1, slakeExprBinary(BINARY_EXPR_ADD, $1, $3)); }|
leftExpr "-=" valuedExpr { $$ = slakeExprBinary(BINARY_EXPR_MOV, $1, slakeExprBinary(BINARY_EXPR_SUB, $1, $3)); }|
leftExpr "*=" valuedExpr { $$ = slakeExprBinary(BINARY_EXPR_MOV, $1, slakeExprBinary(BINARY_EXPR_MUL, $1, $3)); }|
leftExpr "/=" valuedExpr { $$ = slakeExprBinary(BINARY_EXPR_MOV, $1, slakeExprBinary(BINARY_EXPR_DIV, $1, $3)); }|
leftExpr "%=" valuedExpr { $$ = slakeExprBinary(BINARY_EXPR_MOV, $1, slakeExprBinary(BINARY_EXPR_MOD, $1, $3)); }|
leftExpr "|=" valuedExpr { $$ = slakeExprBinary(BINARY_EXPR_MOV, $1, slakeExprBinary(BINARY_EXPR_OR, $1, $3)); }|
leftExpr "&=" valuedExpr { $$ = slakeExprBinary(BINARY_EXPR_MOV, $1, slakeExprBinary(BINARY_EXPR_AND, $1, $3)); }|
leftExpr "^=" valuedExpr { $$ = slakeExprBinary(BINARY_EXPR_MOV, $1, slakeExprBinary(BINARY_EXPR_XOR, $1, $3)); }|
valuedExpr '+' valuedExpr { $$ = slakeExprBinary(BINARY_EXPR_ADD, $1, $3); }|
valuedExpr '-' valuedExpr { $$ = slakeExprBinary(BINARY_EXPR_SUB, $1, $3); }|
valuedExpr '*' valuedExpr { $$ = slakeExprBinary(BINARY_EXPR_MUL, $1, $3); }|
valuedExpr '/' valuedExpr { $$ = slakeExprBinary(BINARY_EXPR_DIV, $1, $3); }|
valuedExpr '%' valuedExpr { $$ = slakeExprBinary(BINARY_EXPR_MOD, $1, $3); }|
valuedExpr '|' valuedExpr { $$ = slakeExprBinary(BINARY_EXPR_OR, $1, $3); }|
valuedExpr '&' valuedExpr { $$ = slakeExprBinary(BINARY_EXPR_AND, $1, $3); }|
valuedExpr '^' valuedExpr { $$ = slakeExprBinary(BINARY_EXPR_XOR, $1, $3); }|
valuedExpr "||" valuedExpr { $$ = slakeExprBinary(BINARY_EXPR_LOR, $1, $3); }|
valuedExpr "&&" valuedExpr { $$ = slakeExprBinary(BINARY_EXPR_LAND, $1, $3); }|
valuedExpr '<' valuedExpr { $$ = slakeExprBinary(BINARY_EXPR_LT, $1, $3); }|
valuedExpr "<=" valuedExpr { $$ = slakeExprBinary(BINARY_EXPR_LTEQ, $1, $3); }|
valuedExpr '>' valuedExpr { $$ = slakeExprBinary(BINARY_EXPR_GT, $1, $3); }|
valuedExpr ">=" valuedExpr { $$ = slakeExprBinary(BINARY_EXPR_GTEQ, $1, $3); }|
valuedExpr "==" valuedExpr { $$ = slakeExprBinary(BINARY_EXPR_EQ, $1, $3); }|
valuedExpr "!=" valuedExpr { $$ = slakeExprBinary(BINARY_EXPR_NEQ, $1, $3); }|
'!' valuedExpr { $$ = slakeExprUnary(UNARY_EXPR_NOT, $2); }|
'-' valuedExpr %prec T_NEG { $$ = slakeExprUnary(UNARY_EXPR_NEG, $2); };

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
SYMBOL SYMBOL '(' params ')'
{
};

//
// Asynchronous external function call.
//
asyncExternalFuncCall:
SYMBOL SYMBOL '(' params ')' "async"
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
	$$ = slakeExprAwait($2);
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
"switch" '(' valuedExpr ')' '{' switchCases switchDefault '}'
{
	$$ = slakeExprSwitch($3, postCurrentSwitchCases(), currentSwitchCaseCount, $7);
	currentSwitchCaseCount = 0;
}|
"switch" '(' valuedExpr ')' '{' switchCases '}'
{
	$$ = slakeExprSwitch($3, postCurrentSwitchCases(), currentSwitchCaseCount, NULL);
};

switchCases: switchCases switchCase
{
	pushSwitchCase($2);
}|
switchCase
{
	pushSwitchCase($1);
};

switchCase:
"case" valuedExpr '{' execBody '}'
{
	$$ = slakeCreateSwitchCase($2, $4);
};

switchDefault:
"default" '{' execBody '}'
{
	$$ = $3;
};

//
// If blocks.
//
ifBlock:
if |
ifBlock elif |
ifBlock else;

if:
"if" '(' valuedExpr ')' '{' execBody '}'
{
};
elif:
"elif" '(' valuedExpr ')' '{' execBody '}'
{
};
else:
"else" '{' execBody '}'
{

};

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
STR { $$ = slakeMakeString($1); }|
INT { $$ = slakeMakeInt($1); }|
UINT { $$ = slakeMakeUInt($1); }|
LONG { $$ = slakeMakeLong($1); }|
ULONG { $$ = slakeMakeULong($1); };

%%
