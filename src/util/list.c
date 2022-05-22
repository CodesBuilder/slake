#include <util/list.h>

/**
 * @brief Iterator for lists.
 *
 * @param node Current node.
 * @param offset Offset of list header structure in each node.
 * @return Next node.
 */
UtilListHeader *utilListIterator(UtilListHeader *node)
{
	return node->next;
}

/**
 * @brief Reversed iterator for lists.
 *
 * @param node Current node.
 * @param offset Offset of list header structure in each node.
 * @return Last node.
 */
UtilListHeader *utilListReversedIterator(UtilListHeader *node)
{
	return node->last;
}

/**
 * @brief Insert a new node into a list in front.
 *
 * @param node Node in the list to be inserted.
 * @param newnode Node to insert.
 * @param offset Offset of list header structure in each node.
 * @return New node.
 */
UtilListHeader *utilListInsertFront(UtilListHeader *node, UtilListHeader *newnode)
{
	if (node->last)
		node->last->next = newnode;
	newnode->next = node;
	return newnode;
}

/**
 * @brief Insert a new node into a list at the back.
 *
 * @param node Node in the list to be inserted.
 * @param newnode Node to insert.
 * @param offset Offset of list header structure in each node.
 * @return New node.
 */
UtilListHeader *utilListInsertBack(UtilListHeader *node, UtilListHeader *newnode)
{
	if (node->next)
		node->next->last = newnode;
	newnode->last = node;
	return newnode;
}

/**
 * @brief Remove a node from a list.
 *
 * @param node Node to remove.
 * @param offset
 * @return Next or last node if present, commonly always return the next if present.
 */
UtilListHeader *utilListRemove(UtilListHeader *node)
{
	if (node->last)
		node->last->next = node->next;
	if (node->next)
		node->next->last = node->last;
}

UtilListHeader* utilListGetFirst(UtilListHeader* node)
{
	UtilListHeader* i=node;
	while(i->next!=NULL)
		i=i->next;
	return i;
}

UtilListHeader* utilListGetEnd(UtilListHeader* node)
{
	UtilListHeader* i=node;
	while(i->last!=NULL)
		i=i->last;
	return i;
}
