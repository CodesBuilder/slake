#include "slakedef.h"
#include <assert.h>
#include <string.h>

typedef struct _SlakeExpr
{
	UtilListHeader listHeader;
	union
	{
		struct
		{
			char name[SLAKE_SYMBOL_MAX + 1];
		} call;
		struct
		{
			char name[SLAKE_SYMBOL_MAX + 1];
		} await;

		struct
		{
			SlakeExpr *condition;
			SlakeExpr *trueBlock, *falseBlock;
		} ifBlock;
		struct
		{
			int times;
			SlakeExpr *body;
		} loopBlock;
		struct
		{
			SlakeExpr *condition;
			SlakeExpr *body;
			SlakeExpr *cycleEnd;
		} forBlock;
		struct
		{
			SlakeExpr *condition;
			SlakeExpr *body;
		} whileBlock;

		struct
		{
			SlakeExpr *l, *r;
		} assignment;

		struct
		{
			char name[SLAKE_SYMBOL_MAX + 1];
		} var;

		SlakeValue value;
	} attribs;
	SlakeExprType type;
} SlakeExpr;

typedef struct _SlakeFunction
{
	UtilListHeader listHeader;
	SlakeExpr *exprs;
	char name[SLAKE_SYMBOL_MAX + 1];
} SlakeFunction;

typedef struct _SlakeVariable
{
	UtilListHeader listHeader;
	SlakeValue value;
	char name[SLAKE_SYMBOL_MAX + 1];
} SlakeVariable;

typedef struct _SlakeScope
{
	SlakeScope *parent;
	SlakeVariable *variables;
	SlakeFunction *functions;
	SlakeExpr *exprs; // Unavailable for global scope
} SlakeScope;

SlakeScope *currentScope;
SlakeScope rootScope = {
	.parent = NULL,
	.exprs = NULL,
	.functions = NULL,
	.variables = NULL};

static SlakeFunction *_slakeCreateFunction();
static SlakeVariable *_slakeCreateVariable();
static void _slakeDestroyFunction(SlakeFunction *func);
static void _slakeDestroyVariable(SlakeVariable *var);
static void _slakeDestroyExpr(SlakeExpr *expr);
static void _slakeDestroyValue(SlakeValue *value);

SlakeScope *slakeCreateScope(SlakeScope *parent)
{
	SlakeScope *scope = malloc(sizeof(SlakeScope));
	if (!parent)
		return NULL;

	parent->exprs = NULL;
	parent->functions = NULL;
	parent->parent = parent;
	parent->variables = NULL;

	return scope;
}

SlakeScope *slakeGetParentScope(SlakeScope *scope)
{
	return scope->parent;
}

size_t slakeGetScopeDepth(SlakeScope *scope)
{
	size_t depth = 0;
	for (SlakeScope *i = scope; i->parent != NULL; i = i->parent, depth++)
		;
	return depth;
}

void slakeDestroyScope(SlakeScope *scope)
{
}

void slakeEnterScope(SlakeScope *scope)
{
}

void slakePushStatement(SlakeScope *scope, SlakeExpr *statements)
{
}

SlakeFunction *slakeDefFunction(const char *name)
{
	SlakeFunction *func = _slakeCreateFunction();
	if (!func)
	{
		perror("Out of memory");
		abort();
	}

	if (!currentScope->functions)
		currentScope->functions = func;
	else
		utilListInsertFront(&(currentScope->variables->listHeader), &(func->listHeader));

	return func;
}

SlakeVariable *slakeDefVariable(const char *name, SlakeValue initValue)
{
	SlakeVariable *var = _slakeCreateVariable();
	if (!var)
	{
		perror("Out of memory");
		abort();
	}

	if (!currentScope->variables)
		currentScope->variables = var;
	else
		utilListInsertFront(&(currentScope->variables->listHeader), &(var->listHeader));

	return var;
}

SlakeFunction *slakeGetFunction(SlakeScope *scope, const char *name)
{
	SlakeFunction *i = scope->functions;
	while (i != NULL)
	{
		if (!strcmp(i->name, name))
			return i;
		i = (SlakeFunction *)(((char *)i->listHeader.next) - offsetof(SlakeFunction, listHeader));
	}

	return NULL;
}

SlakeVariable *slakeGetVariable(SlakeScope *scope, const char *name)
{
	SlakeVariable *i = scope->variables;
	while (i != NULL)
	{
		if (!strcmp(i->name, name))
			return i;
		i = (SlakeVariable *)(((char *)i->listHeader.next) - offsetof(SlakeFunction, listHeader));
	}

	return NULL;
}

void slakeUndefFunction(SlakeScope *scope, const char *name)
{
	SlakeFunction *func = slakeGetFunction(scope, name);
	if (!func)
		return;

	_slakeDestroyFunction(func);
}

void slakeUndefVariable(SlakeScope *scope, const char *name)
{
	SlakeVariable *var = slakeGetVariable(scope, name);
	if (!var)
		return;

	_slakeDestroyVariable(var);
	utilListRemove(&(var->listHeader));
	free(var);
}

static SlakeFunction *_slakeCreateFunction()
{
	SlakeFunction *func = malloc(sizeof(SlakeFunction));
	if (!func)
		return NULL;

	func->exprs = NULL;
	func->listHeader.last = NULL;
	func->listHeader.next = NULL;
	memset(func->name, 0, sizeof(func->name));
	return func;
}

static SlakeVariable *_slakeCreateVariable()
{
	SlakeVariable *var = malloc(sizeof(SlakeVariable));
	var->listHeader.last = NULL;
	var->listHeader.next = NULL;
	memset(var->name, 0, sizeof(var->name));
	var->value.type = VALUE_TYPE_NULL;

	return var;
}

static void _slakeDestroyFunction(SlakeFunction *func)
{
	//
	// Destroy the body.
	//
	for (SlakeExpr *i = func->exprs;
		 i->listHeader.next != NULL;)
	{
		SlakeExpr *op = i; // Original pointer.

		i = (SlakeExpr *)(((char *)(i->listHeader.next)) - offsetof(SlakeExpr, listHeader));
		_slakeDestroyExpr(op);
	}
}

/**
 * @brief Destroy and release a variable object.
 *
 * @param var Variable object to release.
 */
static void _slakeDestroyVariable(SlakeVariable *var)
{
	_slakeDestroyValue(&(var->value));
}

/**
 * @brief Destroy and release a expression object.
 *
 * @param expr Expression object to release.
 */
static void _slakeDestroyExpr(SlakeExpr *expr)
{
	switch (expr->type)
	{
	case EXPR_CALL:
	case EXPR_CALL_ASYNC:
	case EXPR_AWAIT:
		break;
	case EXPR_IF:
		_slakeDestroyExpr(expr->attribs.ifBlock.condition);
		_slakeDestroyExpr(expr->attribs.ifBlock.trueBlock);
		_slakeDestroyExpr(expr->attribs.ifBlock.falseBlock);
		break;
	case EXPR_BREAK:
	case EXPR_CONTINUE:
		break;
	case EXPR_LOOP:
		_slakeDestroyExpr(expr->attribs.loopBlock.body);
		break;
	case EXPR_FOR:
		_slakeDestroyExpr(expr->attribs.forBlock.body);
		_slakeDestroyExpr(expr->attribs.forBlock.condition);
		_slakeDestroyExpr(expr->attribs.forBlock.cycleEnd);
		break;
	case EXPR_WHILE:
		_slakeDestroyExpr(expr->attribs.whileBlock.condition);
		_slakeDestroyExpr(expr->attribs.whileBlock.body);
		break;
	case EXPR_ASSIGNMENT:
		_slakeDestroyExpr(expr->attribs.assignment.l);
		_slakeDestroyExpr(expr->attribs.assignment.r);
		break;
	case EXPR_VALUE:
		_slakeDestroyValue(&(expr->attribs.value));
		break;
	}

	utilListRemove(&(expr->listHeader));
	free(expr);
}

/**
 * @brief Destroy a value object but do not release.
 *
 * @param value Value object to release.
 */
static void _slakeDestroyValue(SlakeValue *value)
{
	switch (value->type)
	{
	case VALUE_TYPE_INT:
	case VALUE_TYPE_LONG:
	case VALUE_TYPE_UINT:
	case VALUE_TYPE_ULONG:
	case VALUE_TYPE_NULL:
		break;
	case VALUE_TYPE_STR:
		free(value->data.str);
		break;
	default:
		abort();
	}
}
