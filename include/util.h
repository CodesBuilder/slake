#ifndef __UTIL_H__
#define __UTIL_H__

#include <stddef.h>

typedef struct _UtilListHeader
{
	struct _UtilListHeader* next, *last;
}UtilListHeader;

/**
 * @brief Iterator for lists.
 *
 * @param node Current node.
 * @param offset Offset of list header structure in each node.
 * @return Next node.
 */
UtilListHeader* utilListIterator(void* node, size_t offset);
/**
 * @brief Reversed iterator for lists.
 *
 * @param node Current node.
 * @param offset Offset of list header structure in each node.
 * @return Last node.
 */
UtilListHeader* utilListReversedIterator(void* node, size_t offset);
/**
 * @brief Insert a new node into a list in front.
 *
 * @param node Node in the list to be inserted.
 * @param newnode Node to insert.
 * @param offset Offset of list header structure in each node.
 * @return New node.
 */
void* utilListInsertFront(void* node, void* newnode, size_t offset);
/**
 * @brief Insert a new node into a list at the back.
 *
 * @param node Node in the list to be inserted.
 * @param newnode Node to insert.
 * @param offset Offset of list header structure in each node.
 * @return New node.
 */
void* utilListInsertBack(void* node, void* newnode, size_t offset);
/**
 * @brief Remove a node from a list.
 *
 * @param node Node to remove.
 * @param offset
 * @return Next or last node if present, commonly always return the next if present.
 */
void* utilListRemove(void* node, size_t offset);

#endif
