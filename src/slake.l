/*
* Lexical description file for LEX.
* Copyright(C) 2022 Vizard Project
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

var {
	return VAR;
}

function {
	return FUNCTION;
}

async {
	return ASYNC;
}

await {
	return AWAIT;
}

[a-zA-Z_][a-zA-Z0-9_]* {
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

: { return ':'; }
; { return ';'; }
@ { return '@'; }
, { return ',';}
\$ { return '$'; }
\( { return '('; }
\) { return ')'; }
= { return '='; }
\? { return '?'; }
\\\n ;
\{ { return '{'; }
\} { return '}'; }

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

%%
