#ifndef __SLAKEDEF_H__
#define __SLAKEDEF_H__

#include "util/list.h"
#include <stdio.h>

#ifdef _WIN32

#if defined(DEBUG) || defined(_DEBUG)

#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define malloc(sz) _malloc_dbg(sz, _NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#endif

#define SLAKE_SYMBOL_MAX 63

typedef enum _SlakeValueType
{
	VALUE_TYPE_STR = 0, // String
	VALUE_TYPE_INT,		// 32-bit signed integer
	VALUE_TYPE_LONG,	// 64-bit signed interger
	VALUE_TYPE_UINT,	// 32-bit unsigned integer
	VALUE_TYPE_ULONG,	// 64-bit unsigned integer
	VALUE_TYPE_NULL		// Null
} SlakeValueType;

typedef enum _SlakeExprType
{
	EXPR_CALL = 0,		// Function call
	EXPR_CALL_ASYNC,	// Asynchronous function call
	EXPR_AWAIT,			// Awaiting
	EXPR_SUPER_CALL,	// Super function call
	EXPR_EXTERNAL_CALL, // External function call

	EXPR_IF,	   // If block
	EXPR_SWITCH, // Switch block
	EXPR_BREAK,	   // Break
	EXPR_CONTINUE, // Continue
	EXPR_LOOP,	   // Loop block
	EXPR_FOR,	   // For block
	EXPR_WHILE,	   // While block

	EXPR_UNARY,	 // Unary operations
	EXPR_BINARY, // Unary operations

	EXPR_VALUE,	 // Immediate value
	EXPR_VARREF, // Variable reference

	EXPR_INVALID = 0xffff // Invalid expression type
} SlakeExprType;

typedef enum _SlakeUnaryExprType
{
	UNARY_EXPR_NOT = 0, // Logic not (aka '!')
	UNARY_EXPR_NEG		// Negate (aka '-')
} SlakeUnaryExprType;

typedef enum _SlakeBinaryExprType
{
	BINARY_EXPR_MOV = 0, // Move (aka '=')
	BINARY_EXPR_ADD,	 // Add (aka '+')
	BINARY_EXPR_SUB,	 // Subtract (aka '-')
	BINARY_EXPR_MUL,	 // Multiply (aka '*')
	BINARY_EXPR_DIV,	 // Divide (aka '/')
	BINARY_EXPR_MOD,	 // Modulo (aka '%')
	BINARY_EXPR_AND,	 // Bit and (aka '&)
	BINARY_EXPR_OR,		 // Bit or (aka '|')
	BINARY_EXPR_XOR,	 // Bit exclusive or (aka '^')
	BINARY_EXPR_LAND,	 // Logic and (aka '&&')
	BINARY_EXPR_LOR,	 // Logic or (aka '||')

	BINARY_EXPR_EQ,	  // Equal (aka '==')
	BINARY_EXPR_NEQ,  // Not equal (aka '!=')
	BINARY_EXPR_LT,	  // Lesser than (aka '<')
	BINARY_EXPR_GT,	  // Greater than (aka '>')
	BINARY_EXPR_LTEQ, // Lesser or equal (aka '<=')
	BINARY_EXPR_GTEQ, // Greater or equal (aka '>=')
} SlakeBinaryExprType;

typedef struct _SlakeValue
{
	union
	{
		int i32;				// VALUE_TYPE_INT
		unsigned int u32;		// VALUE_TYPE_UINT
		long long i64;			// VALUE_TYPE_LONG
		unsigned long long u64; // VALUE_TYPE_ULONG
		char *str;				// VALUE_TYPE_STR
	} data;
	SlakeValueType type;
} SlakeValue;

typedef char SlakeSymbol[SLAKE_SYMBOL_MAX + 1];

typedef struct _SlakeExpr SlakeExpr;
typedef struct _SlakeFunction SlakeFunction;
typedef struct _SlakeVariable SlakeVariable;
typedef struct _SlakeScope SlakeScope;

typedef UtilList* SlakeExecBody;

typedef struct _SlakeSwitchCase
{
	SlakeExpr* condition;
	SlakeExecBody body;
}SlakeSwitchCase;

typedef struct _SlakeExpr
{
	union
	{
		struct
		{
			SlakeSymbol symbol;
			SlakeValue *params;
			unsigned short paramCount;
		} call;
		SlakeExpr* await;
		struct
		{
			SlakeSymbol moduleName, funcName;
			SlakeValue *params;
			unsigned short paramCount;
		} externalCall;

		struct
		{
			SlakeExpr *condition;
			SlakeExecBody trueBlock, falseBlock;
		} ifBlock;
		struct
		{
			SlakeExpr* condition;

			SlakeSwitchCase** cases;
			size_t caseCount;

			SlakeExecBody defaultBody;
		} switchBlock;
		struct
		{
			int times;
			SlakeExecBody body;
		} loopBlock;
		struct
		{
			SlakeExpr *condition, *loopEnd;
			SlakeExecBody body;
		} forBlock;
		struct
		{
			SlakeExpr *condition;
			SlakeExecBody body;
		} whileBlock;

		struct
		{
			SlakeBinaryExprType type;
			SlakeExpr *l, *r;
		} binaryOp;

		struct
		{
			SlakeUnaryExprType type;
			SlakeExpr *r;
		} unaryOp;

		SlakeSymbol varRef;

		SlakeValue *value;
	} attribs;
	SlakeExprType type;
} SlakeExpr;

typedef struct _SlakeFunction
{
	UtilList* exprs;
	char name[SLAKE_SYMBOL_MAX + 1];
} SlakeFunction;

typedef struct _SlakeVariable
{
	SlakeValue *value;
	char name[SLAKE_SYMBOL_MAX + 1];
} SlakeVariable;

typedef struct _SlakeScope
{
	SlakeScope *parent;
	UtilList* variables;
	UtilList* functions;
} SlakeScope;

void slakeInit();

//
// Create functions.
//
SlakeScope *slakeCreateScope(SlakeScope *parent);
SlakeFunction *slakeCreateFunction();
SlakeVariable *slakeCreateVariable();
SlakeExpr *slakeCreateExpr();

//
// Destroy functions.
//
void slakeDestroyScope(SlakeScope *scope);
void slakeDestroyFunction(SlakeFunction *func);
void slakeDestroyVariable(SlakeVariable *var);
void slakeDestroyExpr(SlakeExpr *expr);
void slakeDestroyValue(SlakeValue *value);

//
// Scope functions.
//
SlakeScope *slakeGetRootScope();
SlakeScope *slakeGetCurrentScope();
SlakeScope *slakeGetParentScope(SlakeScope *scope);
size_t slakeGetScopeDepth(SlakeScope *scope);
void slakeEnterScope(SlakeScope *scope);

SlakeFunction *slakeSetFunction(SlakeScope *scope, const char *name, SlakeFunction *func);
SlakeVariable *slakeSetVariable(SlakeScope *scope, const char *name, SlakeValue *value);

SlakeFunction *slakeGetFunction(SlakeScope *scope, const char *name);
SlakeVariable *slakeGetVariable(SlakeScope *scope, const char *name);

void slakeUndefFunction(SlakeScope *scope, const char *name);
void slakeUndefVariable(SlakeScope *scope, const char *name);

//
// Functional functions.
//
SlakeFunction *slakeSetFunctionBody(SlakeFunction *func, SlakeExpr *exprs);

//
// Value functions.
//
SlakeValue *slakeCreateValue();
SlakeValue *slakeCopyValue(SlakeValue *value);
SlakeValue *slakeMakeInt(int value);
SlakeValue *slakeMakeLong(long long value);
SlakeValue *slakeMakeUInt(unsigned int value);
SlakeValue *slakeMakeULong(unsigned long long value);
SlakeValue *slakeMakeString(const char *value);
SlakeValue *slakeSetInt(SlakeValue *dest, int value);
SlakeValue *slakeSetLong(SlakeValue *dest, long long value);
SlakeValue *slakeSetUInt(SlakeValue *dest, unsigned int value);
SlakeValue *slakeSetULong(SlakeValue *dest, unsigned long long value);
SlakeValue *slakeSetString(SlakeValue *dest, const char *value);

//
// Expression functions.
//
SlakeExpr *slakeExprVarRef(const char *symbol);
SlakeExpr *slakeExprCall(const char *symbol, SlakeValue *params, unsigned short paramCount);
SlakeExpr *slakeExprCallAsync(const char *symbol, SlakeValue *params, unsigned short paramCount);
SlakeExpr *slakeExprAwait(SlakeExpr *e);

SlakeExpr *slakeExprSuperCall(const char *symbol, SlakeValue *params, unsigned short paramCount);

SlakeExpr *slakeExprExternalCall(const char *moduleName, const char *funcName, SlakeValue *params, unsigned short paramCount);

SlakeExpr *slakeExprIfBlock(SlakeExpr *condition, SlakeExecBody trueBlock, SlakeExecBody falseBlock);
SlakeExpr* slakeExprSwitch(SlakeExpr* condition, SlakeSwitchCase** cases, size_t caseCount, SlakeExecBody defaultBody);

SlakeExpr *slakeExprLoopBlock(int times, SlakeExecBody body);
SlakeExpr *slakeExprWhileBlock(SlakeExpr *condition, SlakeExecBody body);
SlakeExpr *slakeExprForBlock(SlakeExpr *condition, SlakeExpr *loopEnd, SlakeExecBody body);
SlakeExpr *slakeExprBreak();
SlakeExpr *slakeExprContinue();

SlakeExpr *slakeExprUnary(SlakeUnaryExprType t, SlakeExpr *x);
SlakeExpr *slakeExprBinary(SlakeBinaryExprType t, SlakeExpr *x, SlakeExpr *y);

SlakeExpr *slakeExprImmediateValue(SlakeValue *value);

SlakeExecBody slakeCreateExecBody();
void slakeDestroyExecBody(SlakeExecBody execBody);

SlakeSwitchCase* slakeCreateSwitchCase(SlakeExpr* condition, SlakeExecBody execBody);
void slakeDestroySwitchCase(SlakeSwitchCase* swCase);

SlakeExecBody slakeExprAttach(SlakeExecBody execBody, SlakeExpr* expr);
SlakeValue *slakeExprExec(SlakeExpr *expr);

//
// Miscellaneous functions.
//
void slakePanic(const char *msg);

#if defined(DEBUG)||defined(_DEBUG)
#define slakeDbgPrintf(s, ...) printf("[SLAKE DEBUG]"s,##__VA_ARGS__)
#else
#define slakeDbgPrintf(s, ...)
#endif

#endif
