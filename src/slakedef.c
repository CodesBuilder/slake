#include "slakedef.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

SlakeScope *rootScope = NULL;
SlakeScope *currentScope = NULL;

/**
 * @brief Initialize Slake runtime.
 */
void slakeInit()
{
	rootScope = slakeCreateScope(NULL);
	currentScope = rootScope;
	slakeDbgPrintf("Initialized Slake runtime");
}

/**
 * @brief Get root (global) scope object.
 *
 * @return Root scope object.
 */
SlakeScope *slakeGetRootScope()
{
	return rootScope;
}

/**
 * @brief Create a new scope object.
 *
 * @param parent Parent scope object. NULL for none.
 * @return Created scope object.
 */
SlakeScope *slakeCreateScope(SlakeScope *parent)
{
	SlakeScope *scope = malloc(sizeof(SlakeScope));
	if (!scope)
		return NULL;

	parent->parent = parent;
	parent->functions = utilListNew(sizeof(SlakeFunction));
	if (parent->functions)
		slakePanic("Out of memory");
	parent->variables = utilListNew(sizeof(SlakeVariable));
	if (!parent->variables)
		slakePanic("Out of memory");

	return scope;
}

/**
 * @brief Get current scope.
 *
 * @return Current scope object.
 */
SlakeScope *slakeGetCurrentScope()
{
	return currentScope;
}

/**
 * @brief Get parent of a scope.
 *
 * @param scope Target scope.
 * @return The parent scope. NULL if no parent.
 */
SlakeScope *slakeGetParentScope(SlakeScope *scope)
{
	assert(scope != NULL);
	return scope->parent;
}

/**
 * @brief Get depth of a scope.
 *
 * @param scope Target scope.
 * @return Scope depth.
 */
size_t slakeGetScopeDepth(SlakeScope *scope)
{
	assert(scope != NULL);
	size_t depth = 0;
	for (SlakeScope *i = scope; i->parent != NULL; i = i->parent, depth++)
		;
	return depth;
}

/**
 * @brief Set a scope as current.
 *
 * @param scope Target scope.
 */
void slakeEnterScope(SlakeScope *scope)
{
	assert(scope != NULL);
	currentScope = scope;
}

/**
 * @brief Define or replace a function.
 *
 * @param scope Target scope.
 * @param name Function name.
 * @param func Function object.
 * @return Created function object.
 */
SlakeFunction *slakeSetFunction(SlakeScope *scope, const char *name, SlakeFunction *func)
{
	assert(name != NULL);

	slakeUndefFunction(scope, name);

	UtilListNode *node = utilListNodeNew(scope->functions, func);
	if (!node)
		slakePanic("Out of memory");
	utilListAppend(scope->functions, node);

	return func;
}

/**
 * @brief Define a variable or set its value. The value will be copied.
 *
 * @param scope Target scope.
 * @param name Variable name.
 * @param value Variable value.
 * @return Created variable object.
 */
SlakeVariable *slakeSetVariable(SlakeScope *scope, const char *name, SlakeValue *value)
{
	assert(name != NULL);

	slakeUndefVariable(scope, name);
	SlakeVariable *var = slakeCreateVariable();
	if (!var)
		slakePanic("Out of memory");

	var->value = slakeCopyValue(value);

	UtilListNode *node = utilListNodeNew(scope->variables, var);
	if (!node)
		slakePanic("Out of memory");

	utilListAppend(scope->variables, node);

	return var;
}

static UtilListNode *_slakeGetFunction(SlakeScope *scope, const char *name)
{
	for (UtilListNode *i = scope->functions->begin; i != scope->functions->end; i = i->next)
	{
		SlakeFunction *fn = i->data;
		if (!strcmp(fn->name, name))
			return i;
	}

	return NULL;
}

/**
 * @brief Get a named function from a scope.
 *
 * @param scope Target scope.
 * @param name Function name.
 * @return Corresponding function name. NULL if not found.
 */
SlakeFunction *slakeGetFunction(SlakeScope *scope, const char *name)
{
	UtilListNode *node = _slakeGetFunction(scope, name);

	if (node)
		return node->data;

	return NULL;
}

static UtilListNode *_slakeGetVariable(SlakeScope *scope, const char *name)
{
	for (UtilListNode *i = scope->variables->begin; i != scope->variables->end; i = i->next)
	{
		SlakeVariable *fn = i->data;
		if (!strcmp(fn->name, name))
			return i;
	}

	return NULL;
}

/**
 * @brief Get a named variable from a scope.
 *
 * @param scope Target scope.
 * @param name Variable name.
 * @return Corresponding variable object. NULL if not found.
 */
SlakeVariable *slakeGetVariable(SlakeScope *scope, const char *name)
{
	UtilListNode *node = _slakeGetVariable(scope, name);

	if (node)
		return node->data;

	return NULL;
}

/**
 * @brief Undefine a function in a scope.
 *
 * @param scope Target scope.
 * @param name Function name.
 */
void slakeUndefFunction(SlakeScope *scope, const char *name)
{
	UtilListNode *func = _slakeGetFunction(scope, name);
	if (!func)
		return;

	slakeDestroyFunction(func->data);
	utilListRemove(func);
}

/**
 * @brief Undefine a variable in a scope.
 *
 * @param scope Target scope.
 * @param name Variable name.
 */
void slakeUndefVariable(SlakeScope *scope, const char *name)
{
	SlakeVariable *var = slakeGetVariable(scope, name);
	if (!var)
		return;
}

SlakeFunction *slakeSetFunctionBody(SlakeFunction *func, SlakeExpr *exprs)
{
	assert(func != NULL);
}

/**
 * @brief Create a value object with null.
 *
 * @return Created value object.
 */
SlakeValue *slakeCreateValue()
{
	SlakeValue *v = malloc(sizeof(SlakeValue));
	if (!v)
		slakePanic("Out of memory");

	v->type = VALUE_TYPE_NULL;
	return v;
}

/**
 * @brief Copy a value object.
 *
 * @param value Source value object.
 * @return Copied value object.
 */
SlakeValue *slakeCopyValue(SlakeValue *value)
{
	SlakeValue *v = slakeCreateValue();
	v->type = value->type;
	switch (v->type)
	{
	case VALUE_TYPE_INT:
		v->data.i32 = value->data.i32;
		break;
	case VALUE_TYPE_LONG:
		v->data.i64 = value->data.i64;
		break;
	case VALUE_TYPE_UINT:
		v->data.u32 = value->data.u32;
		break;
	case VALUE_TYPE_ULONG:
		v->data.u64 = value->data.u64;
		break;
	case VALUE_TYPE_STR:
		v->data.str = strdup(value->data.str);
		if (!v->data.str)
			slakePanic("Out of memory");
		break;
	case VALUE_TYPE_NULL:
		break;
	default:
		slakePanic("Invalid value type");
	}

	return v;
}

SlakeValue *slakeMakeInt(int value)
{
	SlakeValue *v = slakeCreateValue();
	v->type = VALUE_TYPE_INT;
	v->data.i32 = value;

	return v;
}

SlakeValue *slakeMakeLong(long long value)
{
	SlakeValue *v = slakeCreateValue();
	v->type = VALUE_TYPE_LONG;
	v->data.i64 = value;

	return v;
}

SlakeValue *slakeMakeUInt(unsigned int value)
{
	SlakeValue *v = slakeCreateValue();
	v->type = VALUE_TYPE_UINT;
	v->data.u32 = value;

	return v;
}

SlakeValue *slakeMakeULong(unsigned long long value)
{
	SlakeValue *v = slakeCreateValue();
	v->type = VALUE_TYPE_ULONG;
	v->data.u64 = value;

	return v;
}

/**
 * @brief Make a string value.
 *
 * @param value Data.
 * @return A new value object with data.
 */
SlakeValue *slakeMakeString(const char *value)
{
	SlakeValue *v = slakeCreateValue();
	v->type = VALUE_TYPE_STR;
	v->data.str = strdup(value);
	if (!v->data.str)
		slakePanic("Out of memory");

	return v;
}

/**
 * @brief Generate a variable reference expression.
 *
 * @param symbol Variable name.
 * @return Generated expression.
 */
SlakeExpr *slakeExprVarRef(const char *symbol)
{
	assert(strlen(symbol) <= SLAKE_SYMBOL_MAX);

	SlakeExpr *expr = slakeCreateExpr();
	expr->type = EXPR_VARREF;
	strcpy(expr->attribs.varRef, symbol);

	return expr;
}

/**
 * @brief Generate a call expression.
 *
 * @param symbol Function name.
 * @return Generated expression.
 */
SlakeExpr *slakeExprCall(const char *symbol, SlakeValue *params, unsigned short paramCount)
{
	assert(strlen(symbol) <= SLAKE_SYMBOL_MAX);

	SlakeExpr *expr = slakeCreateExpr();
	expr->type = EXPR_CALL;
	strcpy(expr->attribs.call.symbol, symbol);

	expr->attribs.call.paramCount = paramCount;
	expr->attribs.call.params = calloc(paramCount, sizeof(SlakeValue));
	for (unsigned short i = 0; i < paramCount; i++)
		expr->attribs.call.params = slakeCopyValue(&(params[i]));

	return expr;
}

/**
 * @brief Generate an asynchronous call expression.
 *
 * @param symbol Function name.
 * @return Generated expression.
 */
SlakeExpr *slakeExprCallAsync(const char *symbol, SlakeValue *params, unsigned short paramCount)
{
	assert(strlen(symbol) <= SLAKE_SYMBOL_MAX);

	SlakeExpr *expr = slakeCreateExpr();
	expr->type = EXPR_CALL_ASYNC;
	strcpy(expr->attribs.call.symbol, symbol);

	expr->attribs.call.paramCount = paramCount;
	expr->attribs.call.params = calloc(paramCount, sizeof(SlakeValue));
	for (unsigned short i = 0; i < paramCount; i++)
		expr->attribs.call.params = slakeCopyValue(&(params[i]));

	return expr;
}

/**
 * @brief Generate an await expression.
 *
 * @param symbol Function name.
 * @return Generated expression.
 */
SlakeExpr *slakeExprAwait(SlakeExpr *e)
{
	SlakeExpr *expr = slakeCreateExpr();
	expr->type = EXPR_AWAIT;
	expr->attribs.await = e;

	return expr;
}

/**
 * @brief Generate a super call expression.
 *
 * @param symbol Function name.
 * @return Generated expression.
 */
SlakeExpr *slakeExprSuperCall(const char *symbol, SlakeValue *params, unsigned short paramCount)
{
	assert(strlen(symbol) <= SLAKE_SYMBOL_MAX);

	SlakeExpr *expr = slakeCreateExpr();
	expr->type = EXPR_SUPER_CALL;
	strcpy(expr->attribs.call.symbol, symbol);

	expr->attribs.call.paramCount = paramCount;
	expr->attribs.call.params = calloc(paramCount, sizeof(SlakeValue));
	for (unsigned short i = 0; i < paramCount; i++)
		expr->attribs.call.params = slakeCopyValue(&(params[i]));

	return expr;
}

/**
 * @brief Generate an external call expression.
 *
 * @param moduleName Imported module symbol name.
 * @param funcName Function name.
 * @return Generated expression.
 */
SlakeExpr *slakeExprExternalCall(const char *moduleName, const char *funcName, SlakeValue *params, unsigned short paramCount)
{
	assert(strlen(moduleName) <= SLAKE_SYMBOL_MAX);
	assert(strlen(funcName) <= SLAKE_SYMBOL_MAX);

	SlakeExpr *expr = slakeCreateExpr();
	expr->type = EXPR_EXTERNAL_CALL;
	strcpy(expr->attribs.externalCall.moduleName, moduleName);
	strcpy(expr->attribs.externalCall.funcName, funcName);

	expr->attribs.externalCall.paramCount = paramCount;
	expr->attribs.externalCall.params = calloc(paramCount, sizeof(SlakeValue));
	for (unsigned short i = 0; i < paramCount; i++)
		expr->attribs.externalCall.params = slakeCopyValue(&(params[i]));

	return expr;
}

/**
 * @brief Generate an if block expression.
 *
 * @param condition Condition expression.
 * @param trueBlock Expressions to execute if the condition is true.
 * @param falseBlock Expressions to execute if the condition is false.
 * @return Generated expression.
 */
SlakeExpr *slakeExprIfBlock(SlakeExpr *condition, SlakeExecBody trueBlock, SlakeExecBody falseBlock)
{
	SlakeExpr *expr = slakeCreateExpr();
	expr->type = EXPR_IF;
	expr->attribs.ifBlock.condition = condition;
	expr->attribs.ifBlock.trueBlock = trueBlock;
	expr->attribs.ifBlock.falseBlock = falseBlock;

	return expr;
}

/**
 * @brief Create a switch expression.
 *
 * @param condition Switch condition.
 * @param cases Switch cases. This function will make a copy.
 * @param caseCount Count of cases.
 * @param defaultBody Default execution body. NULL for none.
 * @return Created expression object.
 */
SlakeExpr* slakeExprSwitch(SlakeExpr* condition, SlakeSwitchCase** cases, size_t caseCount, SlakeExecBody defaultBody)
{
	SlakeExpr* expr=slakeCreateExpr();
	expr->type=EXPR_SWITCH;

	expr->attribs.switchBlock.cases=malloc(caseCount*sizeof(SlakeSwitchCase*));
	if(!expr->attribs.switchBlock.cases)
		slakePanic("Out of memory");
	memcpy(expr->attribs.switchBlock.cases,cases,caseCount*sizeof(SlakeSwitchCase*));
	expr->attribs.switchBlock.caseCount=caseCount;

	expr->attribs.switchBlock.condition=condition;
	expr->attribs.switchBlock.defaultBody=defaultBody;

	return expr;
}

/**
 * @brief Generate a loop block expression.
 *
 * @param times Loop times.
 * @param body Loop body expressions.
 * @return Generated expression.
 */
SlakeExpr *slakeExprLoopBlock(int times, SlakeExecBody body)
{
	SlakeExpr *expr = slakeCreateExpr();
	expr->type = EXPR_WHILE;
	expr->attribs.loopBlock.times = times;
	expr->attribs.loopBlock.body = body;
	return expr;
}

/**
 * @brief Generate a while block expression.
 *
 * @param condition Loop condition expression.
 * @param body Loop body expressions.
 * @return Generated expression.
 */
SlakeExpr *slakeExprWhileBlock(SlakeExpr *condition, SlakeExecBody body)
{
	SlakeExpr *expr = slakeCreateExpr();
	expr->type = EXPR_WHILE;
	expr->attribs.whileBlock.condition = condition;
	expr->attribs.whileBlock.body = body;
	return expr;
}

/**
 * @brief Generate a for block expression.
 *
 * @param condition Loop condition expression.
 * @param loopEnd Expression to execute at the end of each cycle.
 * @param body Loop body expression.
 * @return Generated expression.
 */
SlakeExpr *slakeExprForBlock(SlakeExpr *condition, SlakeExpr *loopEnd, SlakeExecBody body)
{
	SlakeExpr *expr = slakeCreateExpr();
	expr->type = EXPR_FOR;
	expr->attribs.forBlock.condition = condition;
	expr->attribs.forBlock.loopEnd = loopEnd;
	expr->attribs.forBlock.body = body;
	return expr;
}

/**
 * @brief Generate a break expression.
 *
 * @return Generated expression.
 */
SlakeExpr *slakeExprBreak()
{
	SlakeExpr *expr = slakeCreateExpr();
	expr->type = EXPR_BREAK;
	return expr;
}

/**
 * @brief Generate a continue expression.
 *
 * @return Generated expression.
 */
SlakeExpr *slakeExprContinue()
{
	SlakeExpr *expr = slakeCreateExpr();
	expr->type = EXPR_CONTINUE;
	return expr;
}

/**
 * @brief Create an unary operation expression.
 *
 * @param t Operation type.
 * @param x Expression target.
 * @return Created expression.
 */
SlakeExpr *slakeExprUnary(SlakeUnaryExprType t, SlakeExpr *x)
{
	SlakeExpr* expr=slakeCreateExpr();
	expr->type=EXPR_UNARY;
	expr->attribs.unaryOp.type=t;
	expr->attribs.unaryOp.r=x;

	return expr;
}

/**
 * @brief Create a binary operation expression.
 *
 * @param t Operation type.
 * @param x The left expression.
 * @param y The right expression.
 * @return Created expression object.
 */
SlakeExpr *slakeExprBinary(SlakeBinaryExprType t, SlakeExpr *x, SlakeExpr *y)
{
	SlakeExpr* expr=slakeCreateExpr();
	expr->type=EXPR_BINARY;
	expr->attribs.binaryOp.type=t;
	expr->attribs.binaryOp.l=x;
	expr->attribs.binaryOp.r=y;

	return expr;
}

/**
 * @brief Generate a expression from a value. It will make a copy from the
 * source.
 *
 * @param value Value to generate the expression.
 * @return Generated expression.
 */
SlakeExpr *slakeExprImmediateValue(SlakeValue *value)
{
	SlakeExpr *expr = slakeCreateExpr();
	expr->type = EXPR_VALUE;
	expr->attribs.value = slakeCopyValue(value);

	return expr;
}

/**
 * @brief Create an empty execution body.
 *
 * @return Created execution body object.
 */
SlakeExecBody slakeCreateExecBody()
{
	SlakeExecBody execBody=utilListNew(sizeof(SlakeExpr*));
	if(!execBody)
		slakePanic("Out of memory");

	return execBody;
}

/**
 * @brief Destroy an execution body and associated expressions.
 *
 * @param execBody Execution body to destroy.
 */
void slakeDestroyExecBody(SlakeExecBody execBody)
{
	for(UtilListNode* i=execBody->begin;i!=execBody->end;i=i->next)
	{
		slakeDestroyExpr(i->data);
		i->data=NULL;
	}

	utilListDelete(execBody);
}

/**
 * @brief Create a switch case.
 *
 * @attention The condition expression and the execution body will be associated
 * with the switch case.
 *
 * @param condition Case condition.
 * @param execBody Execution body.
 * @return Created case object.
 */
SlakeSwitchCase* slakeCreateSwitchCase(SlakeExpr* condition, SlakeExecBody execBody)
{
	SlakeSwitchCase* swCase=malloc(sizeof(SlakeSwitchCase));
	if(!swCase)
		slakePanic("Out of memory");

	swCase->condition=condition;
	swCase->body=execBody;

	return swCase;
}

/**
 * @brief Destroy a case object and associated expressions.
 *
 * @param swCase Case object.
 */
void slakeDestroySwitchCase(SlakeSwitchCase* swCase)
{
	slakeDestroyExpr(swCase->condition);
	slakeDestroyExecBody(swCase->body);
}

/**
 * @brief Append an expression into an execution body.
 *
 * @param execBody Execution body.
 * @param expr Expression to append.
 * @return Appended execution body.
 */
SlakeExecBody slakeExprAttach(SlakeExecBody execBody, SlakeExpr *expr)
{
	assert(execBody != NULL);
	assert(expr != NULL);

	utilListAppend(execBody, utilListNodeNew(execBody, expr));

	return execBody;
}

/**
 * @brief Execute an expression.
 *
 * @param expr Expression to execute.
 * @return Return value of the expression.
 */
SlakeValue *slakeExprExec(SlakeExpr *expr)
{
	assert(expr!=NULL);
}

/**
 * @brief Output a panic message and abort.
 *
 * @param msg Panic message.
 */
void slakePanic(const char *msg)
{
	printf("[PANIC]%s\n", msg);
	abort();
}

/**
 * @brief Create an empty function object.
 *
 * @return Created function object.
 */
SlakeFunction *slakeCreateFunction()
{
	SlakeFunction *func = malloc(sizeof(SlakeFunction));
	if (!func)
		return NULL;

	func->exprs = utilListNew(sizeof(SlakeExpr));
	memset(func->name, 0, sizeof(func->name));
	return func;
}

/**
 * @brief Create an empty variable.
 *
 * @return Created variable object.
 */
SlakeVariable *slakeCreateVariable()
{
	SlakeVariable *var = malloc(sizeof(SlakeVariable));
	memset(var->name, 0, sizeof(var->name));
	var->value = slakeCreateValue();

	return var;
}

/**
 * @brief Create an empty expression object.
 *
 * @return Created expression object.
 */
SlakeExpr *slakeCreateExpr()
{
	SlakeExpr *expr = malloc(sizeof(SlakeExpr));
	if (!expr)
		return NULL;

	expr->type = EXPR_INVALID;

	return expr;
}

/**
 * @brief Destroy and release a scope and its child resources.
 *
 * @param scope Scope to destroy.
 */
void slakeDestroyScope(SlakeScope *scope)
{
	assert(scope != NULL);
	if (currentScope == scope)
		currentScope = NULL;

	for (UtilListNode *i = scope->functions->begin;
		 i != scope->functions->end;
		 i = i->next)
		slakeDestroyFunction(i->data);

	for (UtilListNode *i = scope->variables->begin;
		 i != scope->variables->end;
		 i = i->next)
		slakeDestroyFunction(i->data);

	free(scope);
}

/**
 * @brief Destroy a function and its subexpressions.
 *
 * @param func Function to destroy.
 */
void slakeDestroyFunction(SlakeFunction *func)
{
	//
	// Destroy the body.
	//
	for (UtilListNode *i = func->exprs->begin;
		 i != func->exprs->end;
		 i = i->next)
	{
		slakeDestroyExpr(i->data);
		i->data = NULL;
	}
}

/**
 * @brief Destroy a variable object.
 *
 * @param var Variable object to release.
 */
void slakeDestroyVariable(SlakeVariable *var)
{
	slakeDestroyValue(var->value);
}

/**
 * @brief Destroy and release an expression object and its subexpressions.
 *
 * @param expr Expression object to release.
 */
void slakeDestroyExpr(SlakeExpr *expr)
{
	switch (expr->type)
	{
	case EXPR_CALL:
	case EXPR_CALL_ASYNC:
	case EXPR_AWAIT:
		break;
	case EXPR_IF:
		slakeDestroyExpr(expr->attribs.ifBlock.condition);
		slakeDestroyExecBody(expr->attribs.ifBlock.trueBlock);
		slakeDestroyExecBody(expr->attribs.ifBlock.falseBlock);
		break;
	case EXPR_SWITCH:
		for(size_t i=0;i<expr->attribs.switchBlock.caseCount;i++)
			slakeDestroySwitchCase(expr->attribs.switchBlock.cases[i]);
		slakeDestroyExpr(expr->attribs.switchBlock.condition);
		slakeDestroyExecBody(expr->attribs.switchBlock.defaultBody);
		break;
	case EXPR_BREAK:
	case EXPR_CONTINUE:
		break;
	case EXPR_LOOP:
		slakeDestroyExecBody(expr->attribs.loopBlock.body);
		break;
	case EXPR_FOR:
		slakeDestroyExecBody(expr->attribs.forBlock.body);
		slakeDestroyExpr(expr->attribs.forBlock.condition);
		slakeDestroyExpr(expr->attribs.forBlock.loopEnd);
		break;
	case EXPR_WHILE:
		slakeDestroyExpr(expr->attribs.whileBlock.condition);
		slakeDestroyExecBody(expr->attribs.whileBlock.body);
		break;
	case EXPR_UNARY:
		slakeDestroyExpr(expr->attribs.unaryOp.r);
		break;
	case EXPR_BINARY:
		slakeDestroyExpr(expr->attribs.binaryOp.l);
		slakeDestroyExpr(expr->attribs.binaryOp.r);
		break;
	case EXPR_VALUE:
		slakeDestroyValue(expr->attribs.value);
		break;
	default:
		slakePanic("Invalid value type");
	}
	free(expr);
}

/**
 * @brief Destroy and free a value object.
 *
 * @param value Value object to release.
 */
void slakeDestroyValue(SlakeValue *value)
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
		slakePanic("Invalid value type");
	}

	free(value);
}
