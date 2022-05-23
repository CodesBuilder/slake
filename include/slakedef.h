#ifndef __SLAKEDEF_H__
#define __SLAKEDEF_H__

#include "util/list.h"

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
	EXPR_CALL = 0,	 // Function call
	EXPR_CALL_ASYNC, // Asynchronous function call
	EXPR_AWAIT,		 // Awaiting

	EXPR_IF,	   // If block
	EXPR_BREAK,	   // Break
	EXPR_CONTINUE, // Continue
	EXPR_LOOP,	   // Loop block
	EXPR_FOR,	   // For block
	EXPR_WHILE,	   // While block

	EXPR_ASSIGNMENT, // Assignment
	EXPR_ADD,		 // Add
	EXPR_SUB,		 // Subtract
	EXPR_MUL,		 // Multiply
	EXPR_DIV,		 // Divide
	EXPR_MOD,		 // Modulo

	EXPR_VALUE, // Immediate value
} SlakeExprType;

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

typedef struct _SlakeExpr SlakeExpr;
typedef struct _SlakeFunction SlakeFunction;
typedef struct _SlakeVariable SlakeVariable;
typedef struct _SlakeScope SlakeScope;

SlakeScope *slakeCreateScope(SlakeScope *parent);
SlakeScope *slakeGetParentScope(SlakeScope *scope);
size_t slakeGetScopeDepth(SlakeScope *scope);
void slakeDestroyScope(SlakeScope *scope);
void slakeEnterScope(SlakeScope *scope);

void slakePushStatement(SlakeScope *scope, SlakeExpr *statements);

SlakeFunction *slakeDefFunction(const char *name);
SlakeVariable *slakeDefVariable(const char *name, SlakeValue initValue);

SlakeFunction *slakeGetFunction(SlakeScope *scope, const char *name);
SlakeVariable *slakeGetVariable(SlakeScope *scope, const char *name);

void slakeUndefFunction(SlakeScope *scope, const char *name);
void slakeUndefVariable(SlakeScope *scope, const char *name);

SlakeExpr* slakeAddExpr(SlakeExpr* x, SlakeExpr* y);
SlakeExpr* slakeSubExpr(SlakeExpr* x, SlakeExpr* y);
SlakeExpr* slakeMulExpr(SlakeExpr* x, SlakeExpr* y);
SlakeExpr* slakeDivExpr(SlakeExpr* x, SlakeExpr* y);
SlakeExpr* slakeModExpr(SlakeExpr* x, SlakeExpr* y);
SlakeExpr* slakeOrExpr(SlakeExpr* x, SlakeExpr* y);
SlakeExpr* slakeAndExpr(SlakeExpr* x, SlakeExpr* y);
SlakeExpr* slakeXorExpr(SlakeExpr* x, SlakeExpr* y);
SlakeExpr* slakeNotExpr(SlakeExpr* x);
SlakeExpr* slakeLandExpr(SlakeExpr* x, SlakeExpr* y);
SlakeExpr* slakeLorExpr(SlakeExpr* x, SlakeExpr* y);

#endif
