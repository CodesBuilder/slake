#ifndef __UTIL_LIST_H__
#define __UTIL_LIST_H__

#include <stddef.h>

typedef struct _UtilListHeader
{
	struct _UtilListHeader* next, *last;
}UtilListHeader;

UtilListHeader* utilListIterator(UtilListHeader* node);
UtilListHeader* utilListReversedIterator(UtilListHeader* node);
UtilListHeader* utilListInsertFront(UtilListHeader* node, UtilListHeader* newnode);
UtilListHeader* utilListInsertBack(UtilListHeader* node, UtilListHeader* newnode);
UtilListHeader* utilListRemove(UtilListHeader* node);
UtilListHeader* utilListGetFirst(UtilListHeader* node);
UtilListHeader* utilListGetEnd(UtilListHeader* node);

#endif
