#include <util/list.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>

/**
 * @brief Create a new list node.
 *
 * @param ls Owner list.
 * @param data Initial data. NULL if don't need.
 * @return Created node. NULL if failed.
 */
UtilListNode *utilListNodeNew(UtilList *ls, const void *data)
{
	UtilListNode *node = malloc(sizeof(UtilListNode));
	if (!node)
		return NULL;

	node->data = malloc(ls->dataSize);
	if (!node->data)
	{
		free(node);
		return NULL;
	}

	if (data)
		memcpy(node->data, data, ls->dataSize);

	node->ls = ls;

	node->last = NULL;
	node->next = NULL;

	return node;
}

/**
 * @brief Set data of a node. The data in the buffer will be copied.
 *
 * @param node Target node.
 * @param data New data.
 * @return The target node.
 */
UtilListNode* utilListSetData(UtilListNode* node, const void* data)
{
	assert(node!=NULL);
	assert(node->ls!=NULL);
	memcpy(node->data,data,node->ls->dataSize);

	return node;
}

/**
 * @brief Create a new list.
 *
 * @param dataSize Size in byte of each node.
 * @return Created list. NULL if failed.
 */
UtilList *utilListNew(size_t dataSize)
{
	UtilList *ls = malloc(sizeof(UtilList));
	if(!ls)
		return NULL;

	ls->dataSize = dataSize; // Set the data size for end node creating.
	ls->end = utilListNodeNew(ls, NULL); // Create the end node.
	if (!ls->end)
	{
		free(ls);
		return NULL;
	}
	ls->begin = ls->end;
	ls->end->last = ls->begin;
	ls->end->next = ls->end;
}

/**
 * @brief Free a list and its nodes.
 *
 * @param ls Target list.
 */
void utilListDelete(UtilList *ls)
{
	assert(ls!=NULL);

	UtilListNode *i = ls->begin;
	while (i != ls->end)
	{
		UtilListNode *j = i;
		i = i->next;

		if(j->data)
			free(j->data);
		free(j);
	}
}

/**
 * @brief Remove a node from its list.
 *
 * @param node Target node.
 */
void utilListRemove(UtilListNode *node)
{
	assert(node != node->ls->begin);
	assert(node != node->ls->end);

	node->next->last = node->last;
	node->last->next = node->next;

	if(node->data)
		free(node->data);
	free(node);
}

/**
 * @brief Insert a new node ahead of a node.
 *
 * @param dest The original node.
 * @param src The new node.
 * @return The new node.
 */
UtilListNode* utilListInsertFront(UtilListNode* dest, UtilListNode* src)
{
	assert(dest != NULL);
	assert(dest->ls != NULL);

	src->last = dest->last;
	src->next = dest;
	dest->last = src;

	return src;
}

/**
 * @brief Insert a new node after a node.
 *
 * @param dest The original node.
 * @param src The new node.
 * @return The new node.
 */
UtilListNode* utilListInsertBack(UtilListNode* dest, UtilListNode* src)
{
	assert(dest != NULL);
	assert(dest->ls != NULL);
	assert(dest != dest->ls->end);

	src->next = dest->next;
	src->last = dest;
	dest->next = src;

	return src;
}

/**
 * @brief Append a new node in the end of a list.
 *
 * @param ls List to append.
 * @param src Node to be appended.
 * @return The new node.
 */
UtilListNode* utilListAppend(UtilList* ls, UtilListNode* src)
{
	return utilListInsertFront(ls,ls->end,src);
}

/**
 * @brief Prepend a new node in front of a list.
 *
 * @param ls List to prepend.
 * @param src Node to be prepended.
 * @return The new node.
 */
UtilListNode* utilListPrepend(UtilList* ls, UtilListNode* src)
{
	return utilListInsertFront(ls,ls->begin,src);
}
