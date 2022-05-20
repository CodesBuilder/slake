#include <util.h>

UtilListHeader *utilListIterator(void *node, size_t offset)
{
	return ((UtilListHeader *)((char *)node + offset))->next;
}

UtilListHeader *utilListReversedIterator(void *node, size_t offset)
{
	return ((UtilListHeader *)((char *)node + offset))->last;
}

void *utilListInsertFront(void *node, void *newnode, size_t offset)
{
	UtilListHeader *nodeLh = ((UtilListHeader *)((char *)node + offset));
	UtilListHeader *newnodeLh = ((UtilListHeader *)((char *)newnode + offset));
	if (nodeLh->last)
		nodeLh->last->next = newnodeLh;
	newnodeLh->next = nodeLh;
	return newnode;
}

void *utilListInsertBack(void *node, void *newnode, size_t offset)
{
	UtilListHeader *nodeLh = ((UtilListHeader *)((char *)node + offset));
	UtilListHeader *newnodeLh = ((UtilListHeader *)((char *)newnode + offset));
	if (nodeLh->next)
		nodeLh->next->last = newnodeLh;
	newnodeLh->last = nodeLh;
	return newnode;
}

void *utilListRemove(void *node, size_t offset)
{
	UtilListHeader *nodeLh = ((UtilListHeader *)((char *)node + offset));
	if (nodeLh->last)
		nodeLh->last->next = nodeLh->next;
	if (nodeLh->next)
		nodeLh->next->last = nodeLh->last;
}
