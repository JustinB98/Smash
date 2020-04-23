#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct node {
	struct node *next; /* Next link, NULL if end node */
	struct node *prev; /* Previous link, NULL if root */
	void *data;		   /* Data saved */
	int key;		   /* Key used to insert, remove and find */
} NODE;

typedef struct linked_list {
	NODE *root; /* First element, NULL if empty */
} LINKED_LIST;

/**
 * Creates a linked list using malloc(3)
 *
 * @return	Linked list to be used for other functions in this module
 *			NULL if malloc(3) failed
 */
LINKED_LIST *linked_list_init();

/**
 * Inserts an item into the specified link list
 *
 * @param list	Linked list to be used
 * @param key	Key to be used to insert
 * @param data	Data to insert
 */
void linked_list_insert(LINKED_LIST *list, int key, void *data);

/**
 * Removes data from linked list
 *
 * @param list		Linked list to be used
 * @param key		Key to be used to remove
 * @param onRemove	Function that will be called just before removing the node
 *					passing in the data
 */
void linked_list_remove(LINKED_LIST *list, int key, void (*onRemove)(void *));

/**
 * Finds the data associated with the specified key
 *
 * @param list	Linked List to be used
 * @param key	Key used to find the element
 *
 * @return		Data found
 *				NULL if data was not found
 */
void *linked_list_find(LINKED_LIST *list, int key);

/**
 * All data in list
 *
 * @param list		Linked list to be freed
 * @param onRemove	Optional function that will be called just before removing
 * 					the node, passing in the data from linked_list_insert 
 */
void linked_list_fini(LINKED_LIST *list, void (*onRemove)(void *));

#endif
