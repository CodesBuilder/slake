#ifndef __UTIL_LIST_H__
#define __UTIL_LIST_H__

#include <stddef.h>

typedef struct _UtilListNode UtilListNode;
typedef struct _UtilList UtilList;

typedef struct _UtilListNode
{
	struct _UtilListNode *last, *next;// Previous and next node.
	void *data;// Stored data.
	UtilList *ls;// Owner node.
} UtilListNode;

typedef struct _UtilList
{
	size_t dataSize; // Size of each node's data part.
	UtilListNode *begin; // The first node.
	UtilListNode *end; // The end node (does not store any data).
} UtilList;

UtilListNode *utilListNodeNew(UtilList *ls, const void* data);
UtilListNode* utilListSetData(UtilListNode* node, const void* data);

UtilList* utilListNew(size_t dataSize);
void utilListDelete(UtilList* ls);

void utilListRemove(UtilListNode *node);
UtilListNode* utilListInsertFront(UtilListNode* dest, UtilListNode* src);
UtilListNode* utilListInsertBack(UtilListNode* dest, UtilListNode* src);
UtilListNode* utilListAppend(UtilList* ls, UtilListNode* src);
UtilListNode* utilListPrepend(UtilList* ls, UtilListNode* src);

#endif
