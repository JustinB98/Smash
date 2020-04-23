#ifndef HASHTABLE_H
#define HASHTABLE_H

typedef struct hashtable {
	int size; /* Constant size of the hashtable (size of array) */
	LINKED_LIST **slots; /* Array of linked list pointers that is of size */
} HASHTABLE;

/**
 * Creates a new hashtable using malloc
 *
 * @param size	Hashtable size. Should be a prime number for best results
 *
 * @return	hashtable that be passed into other functions in this module.
 *			NULL if malloc failed
 */
HASHTABLE *hashtable_init(int size);

/**
 * Inserts key into hashtable
 *
 * @param hashtable	hashtable to be used
 * @param key		key that will be used to hash
 * @param data		data to be stored with key
 */
void hashtable_insert(HASHTABLE *hashtable, int key, void *data);

/**
 * Removes mapping from given key value pair
 *
 * @param hashtable	hashtable to use
 * @param key		key of value
 * @param onRemove	Optional function that will be called with the data removed before removing
 * 					from hashtable
 */
void hashtable_remove(HASHTABLE *hashtable, int key, void (*onRemove)(void *));

/**
 * Finds data associated with key
 *
 * @param hashtable	hashtable to use
 * @param key		key that was used when inserting into the hashtable
 */
void *hashtable_find(HASHTABLE *hashtable, int key);

/**
 * Frees hashtable and all data associated with it
 *
 * @param hashtable	hashtable to be used
 * @param onRemove	Optional function that will be called, passing in just about 
 *					to be removed data in case the caller wants to free the data
 */
void hashtable_fini(HASHTABLE *hashtable, void (*onRemove)(void *));

#endif /* HASHTABLE_H */
