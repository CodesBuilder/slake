/*
* Lexical description file for LEX.
* Copyright(C) 2022 Vizard Project
*/
%option prefix="hk" noyywrap yylineno nounistd never-interactive

%{
#include "hake.tab.h"
#include <hakedef.h>
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
	hklval.str = strdup(hktext);
	return SYMBOL;
}

[-]?[0-9]+ {
	hklval.i32 = atoi(hktext);
	return INT;
}
[-]?[0-9]+[lL] {
	hklval.i64 = atoll(hktext);
	return LONG;
}
[0-9]+[uU] {
	char* ptr;
	hklval.u32 = strtoul(hktext, &ptr, 0);
	return UINT;
}
[0-9]+[lL][uU] {
	char* ptr;
	hklval.u64 = strtoull(hktext, &ptr, 0);
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

\" {
	BEGIN(STRING);
}

<STRING>[^\"\n\\]+ {
	hklval.str = realloc(
		hklval.str,
		(strlen(hklval.str) + hkleng + 1) * sizeof(char));
	strcat(hklval.str,hktext);
}

<STRING>\" {
	BEGIN(INITIAL);
	return STR;
}

<STRING>\n {
	hkerror("Unterminated string");
	BEGIN(INITIAL);
}

<STRING>\\ {
	BEGIN(STRING_ESCAPE);
}

<STRING_ESCAPE>[ \\'"abfnrtv] {
	hklval.str = realloc(
		hklval.str,
		(strlen(hklval.str) + hkleng + 1) * sizeof(char));
	strcat(hklval.str,"\b");

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
