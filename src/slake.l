/*
* Lexical description file for LEX.
* Copyright(C) 2022 Slake Project
*/
%option prefix="slake" noyywrap yylineno nounistd never-interactive

%{
#include "slake.tab.h"
#include <slakedef.h>
#include <string.h>
%}

%x COMMENT
%x STRING
%x STRING_ESCAPE

%%

"var" { return KW_VAR; }
"function" { return KW_FUNCTION; }
"return" { return KW_RETURN; }
"async" { return KW_ASYNC; }
"await" { return KW_AWAIT; }
"if" { return KW_IF; }
"else" { return KW_ELSE; }
"elif" { return KW_ELIF; }
"loop" { return KW_LOOP; }
"for" { return KW_FOR; }
"while" { return KW_WHILE; }
"break" { return KW_BREAK; }
"continue" { return KW_CONTINUE; }
"switch" { return KW_SWITCH; }
"case" { return KW_CASE; }
"default" { return KW_DEFAULT; }
"public" { return KW_PUBLIC; }
"import" { return KW_IMPORT; }

[a-zA-Z_][a-zA-Z0-9_]* {
	if(strlen(slaketext) > SLAKE_SYMBOL_MAX + 1)
	{
		slakeerror("Symbol is too long");
		return '\0';
	}
	slakelval.str = strdup(slaketext);
	return SYMBOL;
}

[-]?[0-9]+ {
	slakelval.i32 = atoi(slaketext);
	return INT;
}
[-]?[0-9]+[lL] {
	slakelval.i64 = atoll(slaketext);
	return LONG;
}
[0-9]+[uU] {
	char* ptr;
	slakelval.u32 = strtoul(slaketext, &ptr, 0);
	return UINT;
}
[0-9]+[lL][uU] {
	char* ptr;
	slakelval.u64 = strtoull(slaketext, &ptr, 0);
	return ULONG;
}

":" { return ':'; }
";" { return ';'; }
"@" { return '@'; }
"," { return ',';}
"$" { return '$'; }
"(" { return '('; }
")" { return ')'; }
"?" { return '?'; }
"{" { return '{'; }
"}" { return '}'; }
"+" { return '+'; }
"-" { return '-'; }
"*" { return '*'; }
"/" { return '/'; }
"%" { return '%'; }
"|" { return '|'; }
"&" { return '&'; }
"^" { return '^'; }
"[" { return '['; }
"]" { return ']'; }
"!" { return '!'; }
"=" { return '='; }
"<" { return '<'; }
">" { return '>'; }
"==" { return T_EQ; }
"!=" { return T_NEQ; }
"<=" { return T_LEQ; }
">=" { return T_GEQ; }
"+=" { return T_ADD_ASSIGN; }
"-=" { return T_SUB_ASSIGN; }
"*=" { return T_MUL_ASSIGN; }
"/=" { return T_DIV_ASSIGN; }
"%=" { return T_MOD_ASSIGN; }

\" {
	BEGIN(STRING);
}

<STRING>[^\"\n\\]+ {
	slakelval.str = realloc(
		slakelval.str,
		(strlen(slakelval.str) + slakeleng + 1) * sizeof(char));
	strcat(slakelval.str,slaketext);
}

<STRING>\" {
	BEGIN(INITIAL);
	return STR;
}

<STRING>\n {
	slakeerror("Unterminated string");
	BEGIN(INITIAL);
}

<STRING>\\ {
	BEGIN(STRING_ESCAPE);
}

<STRING_ESCAPE>[ \\'"abfnrtv] {
	slakelval.str = realloc(
		slakelval.str,
		(strlen(slakelval.str) + slakeleng + 1) * sizeof(char));
	strcat(slakelval.str,"\b");

	BEGIN(STRING);
}

# {
	BEGIN(COMMENT);
}

<COMMENT>\n {
	BEGIN(INITIAL);
}

[ \t\n\r]+ ;
\\\n ;

%%
